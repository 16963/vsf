/***************************************************************************
 *   Copyright (C) 2009 - 2010 by Simon Qian <SimonQian@SimonQian.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "app_type.h"
#include "compiler.h"

#include "interfaces.h"

#include "framework/vsfsm/vsfsm.h"
#include "framework/vsftimer/vsftimer.h"

#include "vsfip.h"
#include "vsfip_buffer.h"

// TODO:
// add ICMP support
// add ip_router
// ....

enum vsfip_EVT_t
{
	VSFIP_EVT_IP_OUTPUT			= VSFSM_EVT_USER_LOCAL + 0,
	VSFIP_EVT_IP_OUTPUT_READY	= VSFSM_EVT_USER_LOCAL + 1,

	VSFIP_EVT_TICK				= VSFSM_EVT_USER_LOCAL + 4,

	VSFIP_EVT_TCP_CONNECTOK		= VSFSM_EVT_USER_LOCAL + 5,
	VSFIP_EVT_TCP_CONNECTFAIL	= VSFSM_EVT_USER_LOCAL + 6,
	VSFIP_EVT_TCP_CLOSED		= VSFSM_EVT_USER_LOCAL + 7,
	VSFIP_EVT_TCP_TXOK			= VSFSM_EVT_USER_LOCAL + 8,
	VSFIP_EVT_TCP_TXFAIL		= VSFSM_EVT_USER_LOCAL + 9,
	VSFIP_EVT_TCP_RXOK			= VSFSM_EVT_USER_LOCAL + 10,
	VSFIP_EVT_TCP_RXFAIL		= VSFSM_EVT_USER_LOCAL + 11,

	// semaphore and timer use instant event, so that they can be exclusive
	VSFIP_EVT_SOCKET_TIMEOUT	= VSFSM_EVT_USER_LOCAL_INSTANT + 0,
	VSFIP_EVT_SOCKET_RECV		= VSFSM_EVT_USER_LOCAL_INSTANT + 1,
};

struct vsfip_t
{
	struct vsfip_netif_t *netif_list;
	struct vsfip_netif_t *netif_default;

	struct vsfip_socket_t *udpconns;
	struct vsfip_socket_t *tcpconns;

	struct vsfsm_t tick_sm;
	struct vsftimer_t tick_timer;

	uint16_t udp_port;
	uint16_t tcp_port;
	uint16_t ip_id;
	uint32_t tsn;

	bool quit;
} static vsfip;
void (*vsfip_input_sniffer)(struct vsfip_buffer_t *buf) = NULL;
void (*vsfip_output_sniffer)(struct vsfip_buffer_t *buf) = NULL;

// vsfip_sockets and vsfip_tcppcb memory pool
static struct vsfip_socket_t vsfip_sockets[VSFIP_SOCKET_NUM];
static struct vsfip_tcppcb_t vsfip_tcppcb[VSFIP_TCPPCB_NUM];

// socket buffer
struct vsfip_socket_t* vsfip_socket_get(void)
{
	int i;
	for (i = 0; i < VSFIP_SOCKET_NUM; i++)
	{
		if (AF_NONE == vsfip_sockets[i].family)
		{
			vsfip_sockets[i].family = AF_INET;
			return &vsfip_sockets[i];
		}
	}
	return NULL;
}

void vsfip_socket_release(struct vsfip_socket_t *socket)
{
	if (socket != NULL)
	{
		socket->family = AF_NONE;
	}
}

// tcppcb buffer
struct vsfip_tcppcb_t* vsfip_tcppcb_get(void)
{
	int i;
	for (i = 0; i < VSFIP_TCPPCB_NUM; i++)
	{
		if (VSFIP_TCPSTAT_INVALID == vsfip_tcppcb[i].state)
		{
			vsfip_tcppcb[i].state = VSFIP_TCPSTAT_CLOSED;
			return &vsfip_tcppcb[i];
		}
	}
	return NULL;
}

void vsfip_tcppcb_release(struct vsfip_tcppcb_t *pcb)
{
	if (pcb != NULL)
	{
		pcb->state = VSFIP_TCPSTAT_INVALID;
	}
}

// bufferlist
void vsfip_bufferlist_free(struct vsfq_t *list)
{
	struct vsfip_buffer_t *tmpbuf;

	while (1)
	{
		tmpbuf = (struct vsfip_buffer_t *)vsfq_dequeue(list);
		if (tmpbuf != NULL)
		{
			vsfip_buffer_release(tmpbuf);
		}
		else
		{
			break;
		}
	}
}

static void vsfip_socketlist_add(struct vsfip_socket_t **head,
									struct vsfip_socket_t *socket)
{
	if (*head != NULL)
	{
		socket->next = *head;
	}
	*head = socket;
}

static bool vsfip_socketlist_remove(struct vsfip_socket_t **head,
									struct vsfip_socket_t *socket)
{
	if (NULL == *head)
	{
		return false;
	}
	if (*head == socket)
	{
		*head = (*head)->next;
		return true;
	}
	else
	{
		struct vsfip_socket_t *tmp;

		tmp = *head;
		while (tmp != NULL)
		{
			if (tmp->next == socket)
			{
				tmp->next = socket->next;
				return true;
			}
			tmp = tmp->next;
		}
	}
	return false;
}

void vsfip_set_default_netif(struct vsfip_netif_t *netif)
{
	vsfip.netif_default = netif;
}

vsf_err_t vsfip_netif_add(struct vsfsm_pt_t *pt, vsfsm_evt_t evt,
							struct vsfip_netif_t *netif)
{
	vsf_err_t err;

	vsfsm_pt_begin(pt);

	if (!netif->ipaddr.size)
	{
		netif->ipaddr.size = 4;		// default IPv4
	}
	memset(netif->ipaddr.addr.s_addr_buf, 0, netif->ipaddr.size);

	vsfip_netif_construct(netif);
	netif->init_pt.user_data = netif;
	netif->init_pt.sm = pt->sm;
	netif->init_pt.state = 0;
	vsfsm_pt_entry(pt);
	err = vsfip_netif_init(&netif->init_pt, evt);
	if (err != 0) return err;

	// add to netif list
	if (NULL == vsfip.netif_list)
	{
		// first netif is set to the default netif
		vsfip_set_default_netif(netif);
	}
	netif->next = vsfip.netif_list;
	vsfip.netif_list = netif;

	vsfsm_pt_end(pt);
	return VSFERR_NONE;
}

vsf_err_t vsfip_netif_remove(struct vsfsm_pt_t *pt, vsfsm_evt_t evt,
							struct vsfip_netif_t *netif)
{
	struct vsfip_netif_t *netif_tmp = vsfip.netif_list;

	vsfsm_pt_begin(pt);

	if (netif_tmp == netif)
	{
		vsfip.netif_list = netif->next;
		goto remove_netif;
	}

	while (netif_tmp != NULL)
	{
		if (netif_tmp->next == netif)
		{
			netif_tmp->next = netif->next;

		remove_netif:
			netif->init_pt.user_data = netif;
			netif->init_pt.sm = pt->sm;
			netif->init_pt.state = 0;
			vsfsm_pt_entry(pt);
			return vsfip_netif_fini(&netif->init_pt, evt);
		}
		netif_tmp = netif_tmp->next;
	}
	vsfsm_pt_end(pt);
	return VSFERR_NONE;
}

struct vsfip_netif_t* vsfip_ip_route(struct vsfip_ipaddr_t *addr)
{
	// TODO:
	return vsfip.netif_default;
}

static void vsfip_check_buff(struct vsfip_socket_t *socket)
{
	struct vsfip_buffer_t *buf = (struct vsfip_buffer_t *)socket->inq.head;

	while (buf != NULL)
	{
		if (!buf->ttl || !--buf->ttl)
		{
			vsfq_remove(&socket->inq, (struct vsfq_node_t *)buf);
		}
		buf = (struct vsfip_buffer_t *)buf->node.next;
	}
}

static void vsfip_tcp_socket_tick(struct vsfip_socket_t *socket);
struct vsfsm_state_t* vsfip_tick(struct vsfsm_t *sm, vsfsm_evt_t evt)
{
	struct vsfip_t *vsfip = (struct vsfip_t *)sm->user_data;

	switch (evt)
	{
	case VSFSM_EVT_INIT:
		vsfip->tick_timer.evt = VSFIP_EVT_TICK;
		vsfip->tick_timer.sm = sm;
		vsfip->tick_timer.interval = 1;
		vsfip->tick_timer.trigger_cnt = 1;
		vsftimer_enqueue(&vsfip->tick_timer);
		break;
	case VSFSM_EVT_FINI:
		vsftimer_dequeue(&vsfip->tick_timer);
		break;
	case VSFIP_EVT_TICK:
	{
		struct vsfip_socket_t *socket;
		struct vsfip_socket_t *child;

		// remove ip packet in reass if ttl reach 0

		// remove packet in input buf if ttl reach 0
		socket = vsfip->udpconns;
		while (socket != NULL)
		{
			vsfip_check_buff(socket);
			socket = socket->next;
		}
		socket = vsfip->tcpconns;
		while (socket != NULL)
		{
			child = socket->listener.child;
			while (child != NULL)
			{
				vsfip_check_buff(child);
				vsfip_tcp_socket_tick(child);
				child = child->next;
			}
			vsfip_check_buff(socket);
			vsfip_tcp_socket_tick(socket);
			socket = socket->next;
		}
	}
		break;
	}
	return NULL;
}

vsf_err_t vsfip_init(void)
{
	int i;

	for (i = 0; i < VSFIP_SOCKET_NUM; i++)
	{
		vsfip_sockets[i].family = AF_NONE;
	}
	for (i = 0; i < VSFIP_TCPPCB_NUM; i++)
	{
		vsfip_tcppcb[i].state = VSFIP_TCPSTAT_INVALID;
	}

	vsfip_buffer_init();
	vsfip.quit = false;
	vsfip.udp_port = VSFIP_CFG_UDP_PORT;
	vsfip.tcp_port = VSFIP_CFG_TCP_PORT;
	vsfip.ip_id = 0;
	vsfip.tsn = 0x00000000;
	vsfip.tcpconns = NULL;
	vsfip.udpconns = NULL;

	memset(&vsfip.tick_sm, 0, sizeof(vsfip.tick_sm));
	vsfip.tick_sm.init_state.evt_handler = vsfip_tick;
	vsfip.tick_sm.user_data = (void*)&vsfip;
	return vsfsm_init(&vsfip.tick_sm);
}

uint16_t vsfip_get_port(enum vsfip_sockproto_t proto)
{
	uint16_t port = 0;

	switch (proto)
	{
	case IPPROTO_TCP:
		port = vsfip.tcp_port++;
		break;
	case IPPROTO_UDP:
		port = vsfip.udp_port++;
		break;
	}
	return port;
}

vsf_err_t vsfip_fini(void)
{
	return vsfsm_post_evt(&vsfip.tick_sm, VSFSM_EVT_FINI);
}

static uint16_t vsfip_checksum(uint8_t *data, uint16_t len)
{
	uint32_t checksum = 0;

	while (len > 1)
	{
		checksum += GET_BE_U16(data);
		data += 2;
		len -= 2;
	}
	if (1 == len)
	{
		checksum += (uint16_t)(*data) << 8;
	}
	checksum = checksum + (checksum >> 16);
	return (uint16_t)checksum;
}

static uint16_t vsfip_proto_checksum(struct vsfip_socket_t *socket,
										struct vsfip_buffer_t *buf)
{
	uint32_t checksum = vsfip_checksum(buf->buf.buffer, buf->buf.size);
	struct vsfip_sockaddr_t *local_addr = &socket->local_sockaddr;
	struct vsfip_sockaddr_t *remote_addr = &socket->remote_sockaddr;

	checksum += GET_BE_U16(&local_addr->sin_addr.addr.s_addr_buf[0]);
	checksum += GET_BE_U16(&local_addr->sin_addr.addr.s_addr_buf[2]);
	checksum += GET_BE_U16(&remote_addr->sin_addr.addr.s_addr_buf[0]);
	checksum += GET_BE_U16(&remote_addr->sin_addr.addr.s_addr_buf[2]);
	checksum += socket->protocol;
	checksum += buf->buf.size;
	checksum = checksum + (checksum >> 16);
	return checksum;
}

static struct vsfip_buffer_t *vsfip_ip4_reass(struct vsfip_buffer_t *buf)
{
	// TODO: add ip_reass support
	if (buf->iphead.ip4head->offset & (VSFIP_IPH_MF | VSFIP_IPH_OFFSET_MASK))
	{
		vsfip_buffer_release(buf);
		return NULL;
	}

	return buf;
}

void vsfip_udp_input(struct vsfip_buffer_t *buf);
void vsfip_tcp_input(struct vsfip_buffer_t *buf);
void vsfip_icmp_input(struct vsfip_buffer_t *buf);
void vsfip_ip4_input(struct vsfip_buffer_t *buf)
{
	struct vsfip_ip4head_t *iphead = (struct vsfip_ip4head_t *)buf->buf.buffer;
	uint16_t iph_hlen = VSFIP_IP4H_HLEN(iphead) * 4;

	// ip header check
	if (vsfip_checksum((uint8_t *)iphead, iph_hlen) != 0xFFFF)
	{
		vsfip_buffer_release(buf);
		return;
	}
	buf->iphead.ip4head = iphead;

	// endian fix
	iphead->len = BE_TO_SYS_U16(iphead->len);
	iphead->id = BE_TO_SYS_U16(iphead->id);
	iphead->offset = BE_TO_SYS_U16(iphead->offset);
	iphead->checksum = BE_TO_SYS_U16(iphead->checksum);
	if ((VSFIP_IP4H_V(iphead) != 4) || (iph_hlen > iphead->len) ||
		(iphead->len > buf->buf.size))
	{
		vsfip_buffer_release(buf);
		return;
	}
	buf->buf.size = iphead->len - iph_hlen;
	buf->buf.buffer += iph_hlen;

	// ip reassembly
	if (!(iphead->offset & VSFIP_IPH_DF))
	{
		buf = vsfip_ip4_reass(buf);
		if (NULL == buf)
		{
			return;
		}
	}

/*	if (is multicast)
	{
	}
	else
*/	{
		switch (iphead->proto)
		{
		case IPPROTO_UDP:
			vsfip_udp_input(buf);
			break;
		case IPPROTO_TCP:
			vsfip_tcp_input(buf);
			break;
		case IPPROTO_ICMP:
			vsfip_icmp_input(buf);
			break;
		default:
			vsfip_buffer_release(buf);
			break;
		}
	}
}

void vsfip_ip6_input(struct vsfip_buffer_t *buf)
{
	vsfip_buffer_release(buf);
}

static vsf_err_t vsfip_add_ip4head(struct vsfip_socket_t *socket)
{
	struct vsfip_ippcb_t *pcb = &socket->pcb.ippcb;
	struct vsfip_ip4head_t *ip4head;
	uint16_t offset;
	uint16_t checksum;

	if (pcb->buf->app.buffer - pcb->buf->buffer <
			sizeof(struct vsfip_ip4head_t))
	{
		return VSFERR_FAIL;
	}

	pcb->buf->buf.buffer -= sizeof(struct vsfip_ip4head_t);
	pcb->buf->buf.size += sizeof(struct vsfip_ip4head_t);
	pcb->buf->iphead.ip4head = (struct vsfip_ip4head_t *)pcb->buf->buf.buffer;
	ip4head = pcb->buf->iphead.ip4head;
	ip4head->ver_hl = 0x45;		// IPv4 and 5 dwrod header len
	ip4head->tos = 0;
	ip4head->len = SYS_TO_BE_U16(pcb->buf->buf.size);
	ip4head->id = SYS_TO_BE_U16(pcb->id);
	ip4head->ttl = 32;
	ip4head->proto = (uint8_t)socket->protocol;
	offset = 0;
	offset += pcb->xfed_len;
	ip4head->offset = SYS_TO_BE_U16(offset);
	ip4head->checksum = SYS_TO_BE_U16(0);
	ip4head->ipsrc = socket->local_sockaddr.sin_addr.addr.s_addr;
	ip4head->ipdest = socket->remote_sockaddr.sin_addr.addr.s_addr;
	checksum = ~vsfip_checksum((uint8_t *)ip4head, sizeof(*ip4head));
	ip4head->checksum = SYS_TO_BE_U16(checksum);
	return VSFERR_NONE;
}

static vsf_err_t vsfip_ip_output_do(struct vsfip_buffer_t *buf)
{
	if (vsfip_output_sniffer != NULL)
	{
		vsfip_output_sniffer(buf);
	}
	else
	{
		vsfip_netif_ip_output(buf);
	}
	return VSFERR_NONE;
}

static vsf_err_t vsfip_ip6_output(struct vsfip_socket_t *socket)
{
	vsfip_buffer_release(socket->pcb.ippcb.buf);
	return VSFERR_NONE;
}

static vsf_err_t vsfip_ip4_output(struct vsfip_socket_t *socket)
{
	struct vsfip_ippcb_t *pcb = &socket->pcb.ippcb;
	vsf_err_t err = VSFERR_NONE;

	if (pcb->buf->buf.size >
			(pcb->buf->netif->mtu - sizeof(struct vsfip_ip4head_t)))
	{
		err = VSFERR_NOT_ENOUGH_RESOURCES;
		goto cleanup;
	}

	pcb->id = vsfip.ip_id++;
	err = vsfip_add_ip4head(socket);
	if (err) goto cleanup;

	return vsfip_ip_output_do(pcb->buf);
cleanup:
	vsfip_buffer_release(pcb->buf);
	return err;
}

static vsf_err_t vsfip_ip_output(struct vsfip_socket_t *socket)
{
	return (AF_INET == socket->family) ?
				vsfip_ip4_output(socket) : vsfip_ip6_output(socket);
}

// socket layer
static void vsfip_tcp_socket_input(void *param, struct vsfip_buffer_t *buf);
struct vsfip_socket_t* vsfip_socket(enum vsfip_sockfamilt_t family,
									enum vsfip_sockproto_t protocol)
{
	struct vsfip_socket_t *socket = vsfip_socket_get();

	if (socket != NULL)
	{
		memset(socket, 0, sizeof(struct vsfip_socket_t));
		socket->family = family;
		socket->protocol = protocol;
		socket->can_rx = false;
		socket->listener.child = NULL;
		socket->listener.backlog = 0;
		socket->listener.accepted_num = 0;
		socket->listener.closing = false;
		socket->accepted = false;
		socket->father = NULL;
		socket->tx_timer.evt = VSFIP_EVT_SOCKET_TIMEOUT;
		socket->rx_timer.evt = VSFIP_EVT_SOCKET_TIMEOUT;
		vsfq_init(&socket->inq);
		socket->callback.input = NULL;
		vsfsm_sem_init(&socket->input_sem, 0, VSFIP_EVT_SOCKET_RECV);

		if (IPPROTO_TCP == protocol)
		{
			socket->can_rx = true;
			socket->callback.input = vsfip_tcp_socket_input;
			socket->callback.param = socket;

			struct vsfip_tcppcb_t *pcb = vsfip_tcppcb_get();
			if (NULL == pcb)
			{
				vsfip_socket_release(socket);
				return NULL;
			}
			memset(pcb, 0, sizeof(struct vsfip_tcppcb_t));
			pcb->rclose = pcb->lclose = true;
			pcb->state = VSFIP_TCPSTAT_CLOSED;
			socket->pcb.protopcb = (struct vsfip_tcppcb_t *)pcb;
		}
	}

	return socket;
}

static vsf_err_t vsfip_free_socket(struct vsfip_socket_t *socket)
{
	struct vsfip_socket_t **list;

	if (socket->father != NULL)
	{
		list = &socket->father->listener.child;
	}
	else
	{
		switch (socket->protocol)
		{
		case IPPROTO_UDP:
			list = &vsfip.udpconns;
			break;
		case IPPROTO_TCP:
			list = &vsfip.tcpconns;
			break;
		}
	}
	vsfip_socketlist_remove(list, socket);

	// remove pending incoming buffer
	vsfip_bufferlist_free(&socket->inq);
	vsftimer_dequeue(&socket->tx_timer);
	vsftimer_dequeue(&socket->rx_timer);

	if ((IPPROTO_TCP == socket->protocol) && (socket->pcb.protopcb != NULL))
	{
		struct vsfip_tcppcb_t *tcppcb =
							(struct vsfip_tcppcb_t *)socket->pcb.protopcb;

		if ((VSFIP_TCPSTAT_LISTEN == tcppcb->state) && (tcppcb->rx_sm != NULL))
		{
			static void vsfip_tcp_postevt(struct vsfsm_t **sm, vsfsm_evt_t evt);
			vsfip_tcp_postevt(&tcppcb->rx_sm, VSFIP_EVT_TCP_CONNECTFAIL);
		}

		vsfip_tcppcb_release(tcppcb);
	}
	vsfip_socket_release(socket);
	return VSFERR_NONE;
}

vsf_err_t vsfip_close(struct vsfip_socket_t *socket)
{
	if ((socket->listener.backlog > 0) && (socket->listener.accepted_num > 0))
	{
		socket->listener.closing = true;
		return VSFERR_NONE;
	}

	if (socket->father != NULL)
	{
		struct vsfip_socket_t *father = socket->father;

		if (vsfip_socketlist_remove(&father->listener.child, socket))
		{
			father->listener.accepted_num--;
		}

		if (father->listener.closing && !father->listener.accepted_num)
		{
			vsfip_free_socket(father);
		}
	}
	return vsfip_free_socket(socket);
}

vsf_err_t vsfip_bind(struct vsfip_socket_t *socket, uint16_t port)
{
	struct vsfip_socket_t **head, *tmp;

	switch (socket->protocol)
	{
	case IPPROTO_UDP:
		head = &vsfip.udpconns;
		break;
	case IPPROTO_TCP:
		head = &vsfip.tcpconns;
		break;
	}

	// check if port already binded
	tmp = *head;
	while (tmp != NULL)
	{
		if (tmp->local_sockaddr.sin_port == port)
		{
			return VSFERR_FAIL;
		}
		tmp = tmp->next;
	}

	socket->next = *head;
	*head = socket;
	socket->local_sockaddr.sin_port = port;
	return VSFERR_NONE;
}

// for UDP port, vsfip_listen will enable rx for the socket
// for TCP port, vsfip_listen is same as listen socket API
vsf_err_t vsfip_listen(struct vsfip_socket_t *socket,uint8_t backlog)
{
	socket->can_rx = true;

	//set listen netif
	socket->netif = vsfip_ip_route(&socket->local_sockaddr.sin_addr);
	socket->local_sockaddr.sin_addr = socket->netif->ipaddr;

	if (socket->protocol == IPPROTO_TCP)
	{
		struct vsfip_tcppcb_t *tcppcb = socket->pcb.protopcb;

		if (0 == backlog)
		{
			return VSFERR_INVALID_PARAMETER;
		}
		if (socket->listener.backlog > 0)
		{
			return VSFERR_FAIL;
		}

		socket->listener.backlog = backlog;
		tcppcb->state = VSFIP_TCPSTAT_LISTEN;
	}

	return VSFERR_NONE;
}

// proto common
// TODO: fix for IPv6
static void vsfip_proto_input(struct vsfip_socket_t *list,
				struct vsfip_buffer_t *buf, struct vsfip_protoport_t *port)
{
	struct vsfip_ip4head_t *iphead = buf->iphead.ip4head;
	struct vsfip_socket_t *socket = list, *child;
	struct vsfip_sockaddr_t *remote_addr;
	uint32_t remote_ipaddr = iphead->ipsrc;

	// find the socket
	while (socket != NULL)
	{
		if (socket->local_sockaddr.sin_port == port->dst)
		{
			if (socket->listener.backlog == 0)
			{
				// normal socket
				remote_addr = &socket->remote_sockaddr;
				if (((remote_addr->sin_addr.addr.s_addr == VSFIP_IPADDR_ANY) ||
						(remote_addr->sin_addr.addr.s_addr == remote_ipaddr)) &&
					((0 == remote_addr->sin_port) ||
						(remote_addr->sin_port == port->src)))
				{
					break;
				}
			}
			else
			{
				// listener socket
				child = socket->listener.child;
				while (child != NULL)
				{
					remote_addr = &child->remote_sockaddr;
					if ((remote_addr->sin_addr.addr.s_addr == remote_ipaddr) &&
						(remote_addr->sin_port == port->src))
					{
						socket = child;
						break;
					}
					child = child->next;
				}
				break;
			}
		}
		socket = socket->next;
	}

	if ((NULL == socket) || !socket->can_rx)
	{
		goto discard_buffer;
	}

	if (socket->callback.input != NULL)
	{
		socket->callback.input(socket->callback.param, buf);
	}
	else
	{
		buf->ttl = VSFIP_CFG_TTL_INPUT;
		vsfq_append(&socket->inq, (struct vsfq_node_t *)buf);
		vsfsm_sem_post(&socket->input_sem);
	}
	return;
discard_buffer:
	vsfip_buffer_release(buf);
	return;
}

// UDP
static void vsfip_udp_input(struct vsfip_buffer_t *buf)
{
	struct vsfip_udphead_t *udphead = (struct vsfip_udphead_t *)buf->buf.buffer;

	// endian fix
	udphead->port.src = BE_TO_SYS_U16(udphead->port.src);
	udphead->port.dst = BE_TO_SYS_U16(udphead->port.dst);
	udphead->len = BE_TO_SYS_U16(udphead->len);
	udphead->checksum = BE_TO_SYS_U16(udphead->checksum);

	buf->app.buffer = buf->buf.buffer + sizeof(struct vsfip_udphead_t);
	buf->app.size = buf->buf.size - sizeof(struct vsfip_udphead_t);
	vsfip_proto_input(vsfip.udpconns, buf, &udphead->port);
}

static vsf_err_t vsfip_add_udphead(struct vsfip_socket_t *socket,
									struct vsfip_buffer_t *buf)
{
	struct vsfip_udphead_t *udphead;

	if (buf->app.buffer - buf->buffer < sizeof(struct vsfip_udphead_t))
	{
		return VSFERR_NOT_ENOUGH_RESOURCES;
	}

	buf->buf.buffer = buf->app.buffer - sizeof(struct vsfip_udphead_t);
	buf->buf.size = buf->app.size + sizeof(struct vsfip_udphead_t);
	udphead = (struct vsfip_udphead_t *)buf->buf.buffer;
	udphead->port.src = SYS_TO_BE_U16(socket->local_sockaddr.sin_port);
	udphead->port.dst = SYS_TO_BE_U16(socket->remote_sockaddr.sin_port);
	udphead->len = SYS_TO_BE_U16(buf->buf.size);
	// no checksum
	udphead->checksum = SYS_TO_BE_U16(0x0000);
	return VSFERR_NONE;
}

vsf_err_t vsfip_udp_send(struct vsfsm_pt_t *pt, vsfsm_evt_t evt,
			struct vsfip_socket_t *socket, struct vsfip_sockaddr_t *sockaddr,
			struct vsfip_buffer_t *buf)
{
	vsf_err_t err = VSFERR_NONE;

	if ((NULL == socket) || (NULL == sockaddr) || (NULL == buf))
	{
		err = VSFERR_INVALID_PARAMETER;
		goto cleanup;
	}
	socket->remote_sockaddr = *sockaddr;

	socket->netif = vsfip_ip_route(&sockaddr->sin_addr);
	if (NULL == socket->netif)
	{
		err = VSFERR_FAIL;
		goto cleanup;
	}
	socket->local_sockaddr.sin_addr = socket->netif->ipaddr;
	buf->netif = socket->netif;

	// allocate local port if not set
	if ((0 == socket->local_sockaddr.sin_port) &&
		vsfip_bind(socket, vsfip_get_port(IPPROTO_UDP)))
	{
		err = VSFERR_FAIL;
		goto cleanup;
	}

	// add udp header
	err = vsfip_add_udphead(socket, buf);
	if (err) goto cleanup;

	socket->pcb.ippcb.buf = buf;
	return vsfip_ip_output(socket);
cleanup:
	vsfip_buffer_release(buf);
	return err;
}

// TODO: fix for IPv6
static struct vsfip_buffer_t *
vsfip_udp_match(struct vsfip_socket_t *socket, struct vsfip_sockaddr_t *addr)
{
	struct vsfip_buffer_t *buf = (struct vsfip_buffer_t *)socket->inq.head;
	struct vsfip_ip4head_t *iphead;

	while (buf != NULL)
	{
		iphead = (struct vsfip_ip4head_t *)buf->iphead.ip4head;
		if (VSFIP_IPADDR_ANY == addr->sin_addr.addr.s_addr)
		{
			addr->sin_addr.addr.s_addr = iphead->ipsrc;
		}
		if (addr->sin_addr.addr.s_addr == iphead->ipsrc)
		{
			vsfq_remove(&socket->inq, (struct vsfq_node_t *)buf);
			buf->app.buffer = buf->buf.buffer + sizeof(struct vsfip_udphead_t);
			buf->app.size = buf->buf.size - sizeof(struct vsfip_udphead_t);
			break;
		}
		buf = (struct vsfip_buffer_t *)buf->node.next;
	}

	return buf;
}

vsf_err_t vsfip_udp_recv(struct vsfsm_pt_t *pt, vsfsm_evt_t evt,
			struct vsfip_socket_t *socket, struct vsfip_sockaddr_t *sockaddr,
			struct vsfip_buffer_t **buf)
{
	vsfsm_pt_begin(pt);

	if ((NULL == socket) || (NULL == sockaddr) || (NULL == buf) ||
		!socket->local_sockaddr.sin_port)
	{
		return VSFERR_INVALID_PARAMETER;
	}

	socket->remote_sockaddr = *sockaddr;

	*buf = vsfip_udp_match(socket, sockaddr);
	if (*buf != NULL)
	{
		return VSFERR_NONE;
	}

	// set pending with timeout
	// TODO: implement pcb for UDP and put these resources in
	if (socket->timeout_ms)
	{
		socket->rx_timer.interval = socket->timeout_ms;
		socket->rx_timer.sm = pt->sm;
		socket->rx_timer.evt = VSFIP_EVT_SOCKET_TIMEOUT;
		socket->rx_timer.trigger_cnt = 1;
		vsftimer_enqueue(&socket->rx_timer);
	}

	while (1)
	{
		if (vsfsm_sem_pend(&socket->input_sem, pt->sm))
		{
			evt = VSFSM_EVT_NONE;
			vsfsm_pt_entry(pt);
			if ((evt != VSFIP_EVT_SOCKET_RECV) &&
				(evt != VSFIP_EVT_SOCKET_TIMEOUT))
			{
				return VSFERR_NOT_READY;
			}

			if (VSFIP_EVT_SOCKET_TIMEOUT == evt)
			{
				vsftimer_dequeue(&socket->rx_timer);
				vsfsm_sync_cancel(&socket->input_sem, pt->sm);
				return VSFERR_FAIL;
			}
		}

		*buf = vsfip_udp_match(socket, sockaddr);
		if (*buf != NULL)
		{
			vsftimer_dequeue(&socket->rx_timer);
			return VSFERR_NONE;
		}
	}

	vsfsm_pt_end(pt);
	return VSFERR_NONE;
}

// TCP
#define VSFIP_TCPFLAG_FIN		(uint16_t)(1 << 0)
#define VSFIP_TCPFLAG_SYN		(uint16_t)(1 << 1)
#define VSFIP_TCPFLAG_RST		(uint16_t)(1 << 2)
#define VSFIP_TCPFLAG_PSH		(uint16_t)(1 << 3)
#define VSFIP_TCPFLAG_ACK		(uint16_t)(1 << 4)
#define VSFIP_TCPFLAG_URG		(uint16_t)(1 << 5)

#define VSFIP_TCPOPT_MSS		0x02
#define VSFIP_TCPOPT_MSS_LEN	0x04

static uint32_t vsfip_get_tsn(void)
{
	uint32_t tsn = vsfip.tsn;
	vsfip.tsn += 0x10000;
	return tsn;
}

static struct vsfip_socket_t* vsfip_new_childsocket(
			struct vsfip_socket_t *father, struct vsfip_sockaddr_t *sockaddr)
{
	struct vsfip_socket_t *new_socket =
								vsfip_socket(father->family, father->protocol);

	if (new_socket != NULL)
	{
		new_socket->can_rx = father->can_rx;
		new_socket->netif = father->netif;
		memcpy(&new_socket->remote_sockaddr, sockaddr,
					sizeof(struct vsfip_sockaddr_t));
		memcpy(&new_socket->local_sockaddr, &father->local_sockaddr,
					sizeof(struct vsfip_sockaddr_t));
		new_socket->father = father;
		vsfip_socketlist_add(&father->listener.child, new_socket);
	}
	return new_socket;
}

static void vsfip_tcp_input(struct vsfip_buffer_t *buf)
{
	struct vsfip_tcphead_t *tcphead = (struct vsfip_tcphead_t *)buf->buf.buffer;

	// endian fix
	tcphead->port.src = BE_TO_SYS_U16(tcphead->port.src);
	tcphead->port.dst = BE_TO_SYS_U16(tcphead->port.dst);
	tcphead->seq = BE_TO_SYS_U32(tcphead->seq);
	tcphead->ackseq = BE_TO_SYS_U32(tcphead->ackseq);
	tcphead->headlen = (tcphead->headlen >> 4) << 2;
	tcphead->window_size = BE_TO_SYS_U16(tcphead->window_size);
	tcphead->checksum = BE_TO_SYS_U16(tcphead->checksum);
	tcphead->urgent_ptr = BE_TO_SYS_U16(tcphead->urgent_ptr);

	buf->node.addr = tcphead->seq;
	buf->app.buffer = buf->buf.buffer + tcphead->headlen;
	buf->app.size = buf->buf.size - tcphead->headlen;
	vsfip_proto_input(vsfip.tcpconns, buf,&tcphead->port);
}

static vsf_err_t vsfip_add_tcphead(struct vsfip_socket_t *socket,
									struct vsfip_buffer_t *buf, uint8_t flags)
{
	struct vsfip_buffer_t *bufhead = (struct vsfip_buffer_t *)socket->inq.head;
	uint16_t window_size;
	struct vsfip_tcppcb_t *pcb = (struct vsfip_tcppcb_t *)socket->pcb.protopcb;
	struct vsfip_tcphead_t *head;
	uint8_t *option;
	uint8_t headlen = sizeof(struct vsfip_tcphead_t);

	if (flags & VSFIP_TCPFLAG_SYN)
	{
		headlen += 4;
	}
	if (buf->app.buffer - buf->buffer < headlen)
	{
		return VSFERR_NOT_ENOUGH_RESOURCES;
	}

	buf->buf.buffer = buf->app.buffer - headlen;
	buf->buf.size = buf->app.size + headlen;

	head = (struct vsfip_tcphead_t *)buf->buf.buffer;
	option = (uint8_t *)head + sizeof(struct vsfip_tcphead_t);
	memset(head, 0, sizeof(struct vsfip_tcphead_t));
	head->port.src = SYS_TO_BE_U16(socket->local_sockaddr.sin_port);
	head->port.dst = SYS_TO_BE_U16(socket->remote_sockaddr.sin_port);
	head->seq = SYS_TO_BE_U32(pcb->lseq);
	head->ackseq = SYS_TO_BE_U32(pcb->rseq);
	head->headlen = (headlen >> 2) << 4;
	flags |= (pcb->rseq != 0) ? VSFIP_TCPFLAG_ACK : 0;
	head->flags = flags;

	if (NULL == bufhead)
	{
		window_size = VSFIP_CFG_TCP_WINDOW;
	}
	else
	{
		window_size = VSFIP_CFG_TCP_WINDOW + bufhead->node.addr - pcb->rseq;
	}
	head->window_size = SYS_TO_BE_U16(window_size);

	head->checksum = SYS_TO_BE_U16(0);
	head->urgent_ptr = SYS_TO_BE_U16(0);

	if (flags & VSFIP_TCPFLAG_SYN)
	{
		// add MSS
		*option++ = VSFIP_TCPOPT_MSS;
		*option++ = VSFIP_TCPOPT_MSS_LEN;
		SET_BE_U16(option, VSFIP_CFG_TCP_MSS);
	}

	pcb->acked_rseq = pcb->rseq;
	return VSFERR_NONE;
}

static uint8_t *vsfip_tcp_buffer(struct vsfip_socket_t *socket, uint8_t flags)
{
	struct vsfip_tcppcb_t *pcb = (struct vsfip_tcppcb_t *)socket->pcb.protopcb;

	pcb->buf = vsfip_buffer_get(VSFIP_CFG_HEADLEN + VSFIP_TCP_HEADLEN + 32);
	if (NULL == pcb->buf)
	{
		return NULL;
	}

	pcb->buf->app.buffer += pcb->buf->app.size;
	pcb->buf->app.size = 0;
	if (vsfip_add_tcphead(socket, pcb->buf, flags) < 0)
	{
		vsfip_buffer_release(pcb->buf);
		return NULL;
	}

	return pcb->buf->app.buffer;
}

static void vsfip_add_tcpchecksum(struct vsfip_socket_t *socket,
									struct vsfip_buffer_t *buf)
{
	uint16_t checksum = ~vsfip_proto_checksum(socket, buf);
	struct vsfip_tcphead_t *head = (struct vsfip_tcphead_t *)buf->buf.buffer;

	head->checksum = SYS_TO_BE_U16(checksum);
}

static vsf_err_t vsfip_tcp_sendflags(struct vsfip_socket_t *socket,
										uint8_t flags)
{
	struct vsfip_tcppcb_t *pcb = (struct vsfip_tcppcb_t *)socket->pcb.protopcb;

	if (NULL == vsfip_tcp_buffer(socket, flags))
	{
		return VSFERR_NOT_ENOUGH_RESOURCES;
	}
	pcb->buf->netif = socket->netif;

	vsfip_add_tcpchecksum(socket, pcb->buf);
	socket->pcb.ippcb.buf = pcb->buf;
	return vsfip_ip_output(socket);
}

static void vsfip_tcp_postevt(struct vsfsm_t **sm, vsfsm_evt_t evt)
{
	struct vsfsm_t *sm_tmp;

	if ((sm != NULL) && (*sm != NULL))
	{
		sm_tmp = *sm;
		*sm = NULL;
		vsfsm_post_evt(sm_tmp, evt);
	}
}

static void vsfip_tcp_socket_tick(struct vsfip_socket_t *socket)
{
	struct vsfip_tcppcb_t *pcb = (struct vsfip_tcppcb_t *)socket->pcb.protopcb;

	if ((0 == pcb->tx_timeout_ms) && (0 == pcb->rx_timeout_ms))
	{
		return;
	}

	if (pcb->tx_timeout_ms > 0)
	{
		pcb->tx_timeout_ms--;
		if (0 == pcb->tx_timeout_ms)
		{
			switch (pcb->state)
			{
			case VSFIP_TCPSTAT_SYN_SENT:
			case VSFIP_TCPSTAT_SYN_GET:
			case VSFIP_TCPSTAT_LASTACK:
				pcb->state = VSFIP_TCPSTAT_CLOSED;
			default:
				vsfip_tcp_postevt(&pcb->tx_sm, VSFIP_EVT_SOCKET_TIMEOUT);
				break;
			}
		}
	}
	if (pcb->rx_timeout_ms > 0)
	{
		pcb->rx_timeout_ms--;
		if (0 == pcb->rx_timeout_ms)
		{
			vsfip_tcp_postevt(&pcb->rx_sm, VSFIP_EVT_SOCKET_TIMEOUT);
		}
	}
	if ((pcb->acked_rseq != pcb->rseq) && !--pcb->ack_tick)
	{
		vsfip_tcp_sendflags(socket, VSFIP_TCPFLAG_ACK);
	}
}

static void vsfip_tcp_socket_input(void *param, struct vsfip_buffer_t *buf)
{
	struct vsfip_socket_t *socket = (struct vsfip_socket_t *)param;
	struct vsfip_tcppcb_t *pcb = (struct vsfip_tcppcb_t *)socket->pcb.protopcb;
	struct vsfip_tcphead_t head = *(struct vsfip_tcphead_t *)buf->buf.buffer;
	bool release_buffer = true;

	pcb->rwnd = head.window_size;
	switch (pcb->state)
	{
	case VSFIP_TCPSTAT_INVALID:
		goto release_buf;
	case VSFIP_TCPSTAT_CLOSED:
		vsfip_tcp_sendflags(socket, VSFIP_TCPFLAG_RST);
		goto release_buf;
	case VSFIP_TCPSTAT_LISTEN:
		if ((VSFIP_TCPFLAG_SYN == head.flags) &&
			(socket->listener.accepted_num < socket->listener.backlog) &&
			!socket->listener.closing)
		{
			struct vsfip_sockaddr_t sockaddr;
			struct vsfip_ip4head_t *iphead = buf->iphead.ip4head;
			struct vsfip_socket_t *new_socket;

			sockaddr.sin_port = head.port.src;
			sockaddr.sin_addr.addr.s_addr = iphead->ipsrc;
			sockaddr.sin_addr.size = 4;

			//new a socket to process the data
			new_socket = vsfip_new_childsocket(socket, &sockaddr);
			if (NULL == new_socket)
			{
				goto release_buf;
			}
			socket->listener.accepted_num++;

			pcb = (struct vsfip_tcppcb_t *)new_socket->pcb.protopcb;
			pcb->lseq = pcb->acked_lseq = vsfip_get_tsn();
			pcb->rseq = 0;
			pcb->tx_retry = 3;
			pcb->state = VSFIP_TCPSTAT_SYN_GET;
			pcb->rseq = head.seq + 1;
			pcb->acked_lseq = pcb->lseq;
			pcb->tx_timeout_ms = socket->timeout_ms;

			vsfip_tcp_sendflags(new_socket,
								VSFIP_TCPFLAG_SYN | VSFIP_TCPFLAG_ACK);
			pcb->lseq++;
			pcb->rclose = 0;

			pcb = (struct vsfip_tcppcb_t *)socket->pcb.protopcb;
			vsfip_tcp_postevt(&pcb->rx_sm, VSFIP_EVT_TCP_CONNECTOK);
		}
		goto release_buf;
		break;
	case VSFIP_TCPSTAT_SYN_SENT:
		if (VSFIP_TCPFLAG_SYN == head.flags)
		{
			// simultaneous open
			pcb->rseq = head.seq + 1;

			vsfip_tcp_sendflags(socket, VSFIP_TCPFLAG_SYN | VSFIP_TCPFLAG_ACK);
			pcb->tx_timeout_ms = socket->timeout_ms;
			pcb->state = VSFIP_TCPSTAT_SYN_GET;
		}
		else if ((head.flags & (VSFIP_TCPFLAG_SYN | VSFIP_TCPFLAG_ACK)) &&
					(head.ackseq == pcb->lseq))
		{
			// send ACK
			pcb->rseq = head.seq + 1;
			pcb->acked_lseq = pcb->lseq;

			vsfip_tcp_sendflags(socket, VSFIP_TCPFLAG_ACK);
		connected:
			pcb->rclose = pcb->lclose = false;
			pcb->state = VSFIP_TCPSTAT_ESTABLISHED;
			if (release_buffer)
			{
				vsfip_buffer_release(buf);
			}
			vsfip_tcp_postevt(&pcb->tx_sm, VSFIP_EVT_TCP_CONNECTOK);
			return;
		}
		goto release_buf;
		break;
	case VSFIP_TCPSTAT_SYN_GET:
		if (VSFIP_TCPFLAG_SYN == head.flags)
		{
			// get resend SYN(previous SYN + ACK lost), reply again
			vsfip_tcp_sendflags(socket, VSFIP_TCPFLAG_SYN | VSFIP_TCPFLAG_ACK);
		}
		else if ((head.flags & VSFIP_TCPFLAG_ACK) &&
					(head.ackseq == (pcb->acked_lseq + 1)))
		{
			pcb->acked_lseq = pcb->lseq = head.ackseq;
			if (buf->app.size > 0)
			{
				if (pcb->rseq == head.seq)
				{
					release_buffer = false;
					pcb->rseq += buf->app.size;
					pcb->ack_tick = 200;
					vsfq_append(&socket->inq, (struct vsfq_node_t *)buf);
				}
				else
				{
					// send the ACK with the latest valid seq to remote
					vsfip_tcp_sendflags(socket, VSFIP_TCPFLAG_ACK);
				}
			}
			goto connected;
		}
		else if (VSFIP_TCPFLAG_FIN & head.flags)
		{
			vsfip_tcp_sendflags(socket, VSFIP_TCPFLAG_FIN);
			pcb->state = VSFIP_TCPSTAT_FINWAIT1;
		}
		goto release_buf;
	case VSFIP_TCPSTAT_ESTABLISHED:
		if (buf->app.size > 0)
		{
			if (pcb->rseq == head.seq)
			{
				release_buffer = false;
				pcb->rseq += buf->app.size;
				pcb->ack_tick = 200;
				vsfq_append(&socket->inq, (struct vsfq_node_t *)buf);
				vsfip_tcp_postevt(&pcb->rx_sm, VSFIP_EVT_TCP_RXOK);
			}
			else
			{
				vsfip_tcp_sendflags(socket, VSFIP_TCPFLAG_ACK);
			}
		}
	case VSFIP_TCPSTAT_CLOSEWAIT:
		if ((head.flags & VSFIP_TCPFLAG_ACK) &&
			(head.ackseq != pcb->acked_lseq))
		{
			// ACK received
			pcb->acked_lseq = head.ackseq;
			if (pcb->acked_lseq == pcb->lseq)
			{
				vsfip_tcp_postevt(&pcb->tx_sm, VSFIP_EVT_TCP_TXOK);
			}
		}

		if (head.flags & VSFIP_TCPFLAG_FIN)
		{
			// passive close
		remote_close:
			pcb->rclose = true;
			pcb->rseq++;
			vsfip_tcp_sendflags(socket, VSFIP_TCPFLAG_ACK);

			// remote close, when local socket is closed
			// set the pcb->state = VSFIP_TCPSTAT_LASTACK
			pcb->state = pcb->lclose ?
					VSFIP_TCPSTAT_CLOSED : VSFIP_TCPSTAT_CLOSEWAIT;
			vsfip_tcp_postevt(&pcb->rx_sm, VSFIP_EVT_TCP_RXFAIL);
			if (VSFIP_TCPSTAT_CLOSED == pcb->state)
			{
				vsfip_tcp_postevt(&pcb->tx_sm, VSFIP_EVT_TCP_CLOSED);
			}
		}

		if (release_buffer)
		{
			goto release_buf;
		}
		break;
	case VSFIP_TCPSTAT_FINWAIT1:
		if ((head.flags & VSFIP_TCPFLAG_ACK) && (head.ackseq == pcb->lseq))
		{
			pcb->acked_lseq = pcb->lseq;
			pcb->lclose = true;
			if (head.flags & VSFIP_TCPFLAG_FIN)
			{
				goto remote_close;
			}
			pcb->state = VSFIP_TCPSTAT_FINWAIT2;
		}
		goto release_buf;
	case VSFIP_TCPSTAT_FINWAIT2:
		if (head.flags & VSFIP_TCPFLAG_FIN)
		{
			goto remote_close;
		}
		else
		{
			goto release_buf;
		}
	case VSFIP_TCPSTAT_LASTACK:
		if ((VSFIP_TCPFLAG_ACK == head.flags) && (head.ackseq == pcb->lseq))
		{
			pcb->acked_lseq = pcb->lseq;
			pcb->state = VSFIP_TCPSTAT_CLOSED;
			vsfip_buffer_release(buf);
			vsfip_tcp_postevt(&pcb->tx_sm, VSFIP_EVT_TCP_CLOSED);
		}
		else
		{
			goto release_buf;
		}
		break;
	default:
	release_buf:
		vsfip_buffer_release(buf);
		break;
	}
}


vsf_err_t vsfip_tcp_connect(struct vsfsm_pt_t *pt, vsfsm_evt_t evt,
			struct vsfip_socket_t *socket, struct vsfip_sockaddr_t *sockaddr)
{
	struct vsfip_tcppcb_t *pcb = (struct vsfip_tcppcb_t *)socket->pcb.protopcb;

	vsfsm_pt_begin(pt);

	if ((NULL == socket) || (NULL == sockaddr) || (NULL == pcb) ||
		(pcb->state != VSFIP_TCPSTAT_CLOSED))
	{
		return VSFERR_INVALID_PARAMETER;
	}
	socket->remote_sockaddr = *sockaddr;

	socket->netif = vsfip_ip_route(&sockaddr->sin_addr);
	if (NULL == socket->netif)
	{
		return VSFERR_FAIL;
	}
	socket->local_sockaddr.sin_addr = socket->netif->ipaddr;

	// allocate local port if not set
	if ((0 == socket->local_sockaddr.sin_port) &&
		vsfip_bind(socket, vsfip_get_port(IPPROTO_TCP)))
	{
		return VSFERR_NONE;
	}

	pcb->lseq = pcb->acked_lseq = vsfip_get_tsn();
	pcb->rseq = 0;
	pcb->tx_retry = 3;

retry:
	vsfip_tcp_sendflags(socket, VSFIP_TCPFLAG_SYN);

	pcb->lseq++;
	pcb->tx_timeout_ms = socket->timeout_ms;
	pcb->tx_sm = pt->sm;
	pcb->state = VSFIP_TCPSTAT_SYN_SENT;

	// wait VSFIP_EVT_TCP_CONNECTOK, VSFIP_EVT_TCP_CONNECTFAIL,
	// 		VSFIP_EVT_SOCKET_TIMEOUT
	evt = VSFSM_EVT_NONE;
	vsfsm_pt_entry(pt);
	if ((evt != VSFIP_EVT_TCP_CONNECTOK) &&
		(evt != VSFIP_EVT_TCP_CONNECTFAIL) && (evt != VSFIP_EVT_SOCKET_TIMEOUT))
	{
		return VSFERR_NOT_READY;
	}

	pcb->lseq = pcb->acked_lseq;
	if (VSFIP_EVT_SOCKET_TIMEOUT == evt)
	{
		if (--pcb->tx_retry)
		{
			goto retry;
		}
		else
		{
			return VSFERR_FAIL;
		}
	}
	else
	{
		return (VSFIP_EVT_TCP_CONNECTOK == evt) ? VSFERR_NONE : VSFERR_FAIL;
	}

	vsfsm_pt_end(pt);
	return VSFERR_NONE;
}

vsf_err_t vsfip_tcp_accept(struct vsfsm_pt_t *pt, vsfsm_evt_t evt,
		struct vsfip_socket_t *socket, struct vsfip_socket_t ** acceptsocket)
{
	struct vsfip_tcppcb_t *pcb = (struct vsfip_tcppcb_t *)socket->pcb.protopcb;

	vsfsm_pt_begin(pt);

	if ((pcb->state != VSFIP_TCPSTAT_LISTEN) || socket->listener.closing)
	{
		return VSFERR_FAIL;
	}

	pcb->rx_sm = pt->sm;

	// wait for VSFIP_EVT_TCP_CONNECTOK or VSFIP_EVT_TCP_CONNECTFAIL
	evt = VSFSM_EVT_NONE;
	vsfsm_pt_entry(pt);
	if ((evt != VSFIP_EVT_TCP_CONNECTOK) && (evt != VSFIP_EVT_TCP_CONNECTFAIL))
	{
		return VSFERR_NOT_READY;
	}
	if (VSFIP_EVT_TCP_CONNECTFAIL == evt)
	{
		return VSFERR_FAIL;
	}

	vsfsm_pt_end(pt);

	socket = socket->listener.child;
	while (socket != NULL)
	{
		if (!socket->accepted)
		{
			socket->accepted = true;
			*acceptsocket = socket;
			return VSFERR_NONE;
		}
	}
	return VSFERR_NOT_AVAILABLE;
}

vsf_err_t vsfip_tcp_send(struct vsfsm_pt_t *pt, vsfsm_evt_t evt,
			struct vsfip_socket_t *socket, struct vsfip_sockaddr_t *sockaddr,
			struct vsfip_buffer_t *buf)
{
	struct vsfip_tcppcb_t *pcb = (struct vsfip_tcppcb_t *)socket->pcb.protopcb;
	vsf_err_t err = VSFERR_NONE;

	vsfsm_pt_begin(pt);
	if (pcb->lclose)
	{
		err = VSFERR_FAIL;
		goto cleanup;
	}
	buf->netif = socket->netif;

	pcb->tx_retry = 3;

retry:
	if (vsfip_add_tcphead(socket, buf, VSFIP_TCPFLAG_PSH) < 0)
	{
		err = VSFERR_FAIL;
		goto cleanup;
	}
	vsfip_add_tcpchecksum(socket, buf);

	vsfip_buffer_reference(buf);
	socket->pcb.ippcb.buf = buf;
	vsfip_ip_output(socket);

	pcb->lseq += buf->app.size;
	pcb->tx_timeout_ms = socket->timeout_ms;
	pcb->tx_sm = pt->sm;

	// wait VSFIP_EVT_TCP_TXOK, VSFIP_EVT_TCP_TXFAIL, VSFIP_EVT_SOCKET_TIMEOUT
	evt = VSFSM_EVT_NONE;
	vsfsm_pt_entry(pt);
	if ((evt != VSFIP_EVT_TCP_TXOK) && (evt != VSFIP_EVT_SOCKET_TIMEOUT) &&
		(evt != VSFIP_EVT_TCP_TXFAIL))
	{
		return VSFERR_NOT_READY;
	}

	// NO need to release the buffer because low level driver will release it
	pcb->lseq = pcb->acked_lseq;
	if (VSFIP_EVT_SOCKET_TIMEOUT == evt)
	{
		if (--pcb->tx_retry)
		{
			goto retry;
		}
		else
		{
			err = VSFERR_FAIL;
		}
	}
	else
	{
		err = (VSFIP_EVT_TCP_TXOK == evt) ? VSFERR_NONE : VSFERR_FAIL;
	}
	buf->ref--;

	vsfsm_pt_end(pt);
	return err;
cleanup:
	vsfip_buffer_release(buf);
	return err;
}

vsf_err_t vsfip_tcp_recv(struct vsfsm_pt_t *pt, vsfsm_evt_t evt,
			struct vsfip_socket_t *socket, struct vsfip_sockaddr_t *sockaddr,
			struct vsfip_buffer_t **buf)
{
	struct vsfip_tcppcb_t *pcb = (struct vsfip_tcppcb_t *)socket->pcb.protopcb;

	vsfsm_pt_begin(pt);
	if (pcb->rclose)
	{
		return VSFERR_FAIL;
	}

	*buf = (struct vsfip_buffer_t *)vsfq_dequeue(&socket->inq);
	if (NULL == *buf)
	{
		pcb->rx_timeout_ms = socket->timeout_ms;
		pcb->rx_sm = pt->sm;

		// wait VSFIP_EVT_TCP_RXOK, VSFIP_EVT_TCP_RXFAIL,
		// 			VSFIP_EVT_SOCKET_TIMEOUT
		evt = VSFSM_EVT_NONE;
		vsfsm_pt_entry(pt);
		if ((evt != VSFIP_EVT_TCP_RXOK) && (evt != VSFIP_EVT_SOCKET_TIMEOUT) &&
			(evt != VSFIP_EVT_TCP_RXFAIL))
		{
			return VSFERR_NOT_READY;
		}

		*buf = (struct vsfip_buffer_t *)vsfq_dequeue(&socket->inq);
		return ((VSFIP_EVT_SOCKET_TIMEOUT == evt) ||
			(VSFIP_EVT_TCP_RXFAIL == evt)) ? VSFERR_FAIL : VSFERR_NONE;
	}

	vsfsm_pt_end(pt);
	return VSFERR_NONE;
}

vsf_err_t vsfip_tcp_close(struct vsfsm_pt_t *pt, vsfsm_evt_t evt,
							struct vsfip_socket_t *socket)
{
	struct vsfip_tcppcb_t *pcb = (struct vsfip_tcppcb_t *)socket->pcb.protopcb;

	vsfsm_pt_begin(pt);
	if (VSFIP_TCPSTAT_CLOSED == pcb->state)
	{
		return VSFERR_NONE;
	}

	pcb->tx_retry = 3;

retry:
	vsfip_tcp_sendflags(socket, VSFIP_TCPFLAG_FIN);

	pcb->lseq++;
	pcb->tx_timeout_ms = socket->timeout_ms;
	pcb->tx_sm = pt->sm;
	pcb->state = (VSFIP_TCPSTAT_CLOSEWAIT == pcb->state) ?
					VSFIP_TCPSTAT_LASTACK : VSFIP_TCPSTAT_FINWAIT1;

	// wait VSFIP_EVT_TCP_CLOSED, VSFIP_EVT_SOCKET_TIMEOUT
	evt = VSFSM_EVT_NONE;
	vsfsm_pt_entry(pt);
	if ((evt != VSFIP_EVT_TCP_CLOSED) && (evt != VSFIP_EVT_SOCKET_TIMEOUT))
	{
		return VSFERR_NOT_READY;
	}

	pcb->lseq = pcb->acked_lseq;
	if (VSFIP_EVT_SOCKET_TIMEOUT == evt)
	{
		if (--pcb->tx_retry)
		{
			goto retry;
		}
		else
		{
			return VSFERR_FAIL;
		}
	}
	else
	{
		return (VSFIP_EVT_TCP_CLOSED == evt) ? VSFERR_NONE : VSFERR_FAIL;
	}

	vsfsm_pt_end(pt);
	return VSFERR_NONE;
}

// ICMP
#define VSFIP_ICMP_ECHO_REPLY	0
#define VSFIP_ICMP_ECHO			8
static void vsfip_icmp_input(struct vsfip_buffer_t *buf)
{
	struct vsfip_ip4head_t *iphead = buf->iphead.ip4head;
	uint16_t iph_hlen = VSFIP_IP4H_HLEN(iphead) * 4;
	struct vsfip_icmphead_t *icmphead =
						(struct vsfip_icmphead_t *)buf->buf.buffer;

	if (icmphead->type == VSFIP_ICMP_ECHO)
	{
		uint32_t swapipcache;

		//swap ipaddr and convert to bigger endian
		swapipcache = iphead->ipsrc;
		iphead->ipsrc = iphead->ipdest;
		iphead->ipdest = swapipcache;
		iphead->len = SYS_TO_BE_U16(iphead->len);
		iphead->id = SYS_TO_BE_U16(iphead->id);
		iphead->checksum = SYS_TO_BE_U16(iphead->checksum);

		icmphead->type = VSFIP_ICMP_ECHO_REPLY;
		icmphead->checksum += SYS_TO_BE_U16(VSFIP_ICMP_ECHO << 8);
		if (icmphead->checksum >=
				SYS_TO_BE_U16(0xffff - (VSFIP_ICMP_ECHO << 8)))
		{
			icmphead->checksum++;
		}

		buf->buf.buffer -= iph_hlen;
		buf->buf.size += iph_hlen;
		vsfip_ip_output_do(buf);
		return;
	}
	// TODO:
	vsfip_buffer_release(buf);
}
