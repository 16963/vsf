
#ifndef __VSFIP_H_INCLUDED__
#define __VSFIP_H_INCLUDED__

#include "vsfip_cfg.h"

#include "framework/vsftimer/vsftimer.h"
#include "stack/tcpip/vsfip.h"

#define VSFIP_IPADDR_ANY				0

#define VSFIP_UDP_HEADLEN				8
#define VSFIP_TCP_HEADLEN				20

struct vsfip_addr_t
{
	uint32_t size;
	union
	{
		uint32_t s_addr;
		uint64_t s_addr64;
		uint8_t s_addr_buf[16];
	} addr;
};

#define vsfip_ipaddr_t		vsfip_addr_t
#define vsfip_macaddr_t		vsfip_addr_t

struct vsfip_bufferlist_t
{
	struct vsfip_buffer_t *head;
	struct vsfip_buffer_t *tail;
};

#include "netif/vsfip_netif.h"

enum vsfip_sockfamilt_t
{
	AF_NONE		= 0,
	AF_INET		= 2,
	AF_INET6	= 10,
};

enum vsfip_sockproto_t
{
//	IPPROTO_IP	= 0,
	IPPROTO_ICMP= 1,
	IPPROTO_IGMP= 2,
	IPPROTO_TCP	= 6,
	IPPROTO_UDP	= 17,
//	IPPROTO_SCTP= 132,
//	IPPROTO_RAW	= 255,
};

struct vsfip_sockaddr_t
{
	uint16_t sin_port;
	struct vsfip_ipaddr_t sin_addr;
};

struct vsfip_ippcb_t
{
	struct vsfip_ipaddr_t src;
	struct vsfip_ipaddr_t dest;
	struct vsfip_buffer_t *buf;
	
	struct vsfsm_pt_t output_pt;
	
	uint32_t len;
	uint32_t xfed_len;
	uint16_t id;
};

// headers
// IPv4
PACKED_HEAD struct PACKED_MID vsfip_ip4head_t
{
	uint8_t ver_hl;
	uint8_t tos;
	uint16_t len;
	uint16_t id;
#define VSFIP_IPH_RF			0x8000
#define VSFIP_IPH_DF			0x4000
#define VSFIP_IPH_MF			0x2000
#define VSFIP_IPH_OFFSET_MASK	0x1FFF
	uint16_t offset;
	uint8_t ttl;
	uint8_t proto;
	uint16_t checksum;
	uint32_t ipsrc;
	uint32_t ipdest;
}; PACKED_TAIL
#define VSFIP_IP4H_V(h)			(h->ver_hl >> 4)
#define VSFIP_IP4H_HLEN(h)		(h->ver_hl & 0x0F)
// PROTO PORT
PACKED_HEAD struct PACKED_MID vsfip_protoport_t
{
	uint16_t src;
	uint16_t dst;
}; PACKED_TAIL
// UDP
PACKED_HEAD struct PACKED_MID vsfip_udphead_t
{
	struct vsfip_protoport_t port;
	uint16_t len;
	uint16_t checksum;
}; PACKED_TAIL
// TCP
PACKED_HEAD struct PACKED_MID vsfip_tcphead_t
{
	struct vsfip_protoport_t port;
	uint32_t seq;
	uint32_t ackseq;
	uint8_t headlen;
	uint8_t flags;
	uint16_t window_size;
	uint16_t checksum;
	uint16_t urgent_ptr;
}; PACKED_TAIL
// ICMP
PACKED_HEAD struct PACKED_MID vsfip_icmphead_t
{
	uint8_t type;
	uint8_t code;
	uint16_t checksum;
	union
	{
		struct
		{
			uint16_t flags;
			uint16_t seqnum;
		} echo;
	} body;
}; PACKED_TAIL

struct vsfip_socket_t;
struct vsfip_tcppcb_t
{
	enum 
	{
		VSFIP_TCPSTAT_INVALID,
		VSFIP_TCPSTAT_CLOSED,
		VSFIP_TCPSTAT_LISTEN,
		VSFIP_TCPSTAT_SYN_SENT,
		VSFIP_TCPSTAT_SYN_GET,
		VSFIP_TCPSTAT_ESTABLISHED,
		VSFIP_TCPSTAT_FINWAIT1,
		VSFIP_TCPSTAT_FINWAIT2,
		VSFIP_TCPSTAT_CLOSEWAIT,
		VSFIP_TCPSTAT_LASTACK,
	} state;
	uint32_t lseq;
	uint32_t acked_lseq;
	uint32_t rseq;
	uint32_t acked_rseq;
	uint32_t rwnd;
	
	// tx
	struct vsfsm_t *tx_sm;
	uint32_t tx_timeout_ms;
	uint32_t tx_retry;
	
	// rx
	struct vsfsm_t *rx_sm;
	uint32_t rx_timeout_ms;
	
	struct vsfip_socket_t *new_socket;
	
	uint32_t ack_tick;
	bool rclose;
	bool lclose;
	bool reset;
	bool ack_timeout;
	
	struct vsfip_bufferlist_t input_bufferlist;
	vsf_err_t err;
	
	struct vsfip_buffer_t *buf;
};

struct vsfip_pcb_t
{
	struct vsfip_ippcb_t ippcb;
	void *protopcb;
};

struct vsfip_socket_t
{
	enum vsfip_sockfamilt_t family;
	enum vsfip_sockproto_t protocol;
	
	struct vsfip_sockaddr_t local_sockaddr;
	struct vsfip_sockaddr_t remote_sockaddr;
	struct vsfip_netif_t *netif;
	
	struct vsfip_pcb_t pcb;
	struct vsfsm_sem_t input_sem;
	struct vsfip_bufferlist_t input_bufferlist;
	
	bool can_rx;
	
	uint32_t timeout_ms;
	struct vsftimer_timer_t tx_timer;
	struct vsftimer_timer_t rx_timer;
	
	struct
	{
		void (*input)(void *param, struct vsfip_buffer_t *buf);
		void *param;
	} callback;
	
	struct vsfip_socket_t *next;
};

struct vsfip_socket_t * vsfip_socket_get(void);
void vsfip_socket_release(struct vsfip_socket_t *socket);

struct vsfip_tcppcb_t * vsfip_tcppcb_get(void);
void vsfip_tcppcb_release(struct vsfip_tcppcb_t *pcb);

void vsfip_bufferlist_init(struct vsfip_bufferlist_t *list);
void vsfip_bufferlist_queue(struct vsfip_bufferlist_t *list,
								struct vsfip_buffer_t *buf);
struct vsfip_buffer_t *
vsfip_bufferlist_dequeue(struct vsfip_bufferlist_t *list);
void vsfip_bufferlist_remove(struct vsfip_bufferlist_t *list,
								struct vsfip_buffer_t *buf);

vsf_err_t vsfip_netif_add(struct vsfsm_pt_t *pt, vsfsm_evt_t evt,
							struct vsfip_netif_t *netif);

vsf_err_t vsfip_init(void);
vsf_err_t vsfip_fini(void);

// different from stant socket call,
// vsfip_socket will return a pointer to vsfip_socket_t structure
struct vsfip_socket_t* vsfip_socket(enum vsfip_sockfamilt_t family,
									enum vsfip_sockproto_t protocol);
vsf_err_t vsfip_close(struct vsfip_socket_t *socket);

// for UPD and TCP
vsf_err_t vsfip_listen(struct vsfip_socket_t *socket);
vsf_err_t vsfip_bind(struct vsfip_socket_t *socket, uint16_t port);

// for TCP
vsf_err_t vsfip_tcp_connect(struct vsfsm_pt_t *pt, vsfsm_evt_t evt,
			struct vsfip_socket_t *socket, struct vsfip_sockaddr_t *sockaddr);
vsf_err_t vsfip_tcp_accept(struct vsfsm_pt_t *pt, vsfsm_evt_t evt,
				struct vsfip_socket_t *socket, struct vsfip_socket_t **remote);
vsf_err_t vsfip_tcp_send(struct vsfsm_pt_t *pt, vsfsm_evt_t evt,
			struct vsfip_socket_t *socket, struct vsfip_sockaddr_t *sockaddr,
			struct vsfip_buffer_t *buf);
vsf_err_t vsfip_tcp_recv(struct vsfsm_pt_t *pt, vsfsm_evt_t evt,
			struct vsfip_socket_t *socket, struct vsfip_sockaddr_t *sockaddr,
			struct vsfip_buffer_t **buf);
vsf_err_t vsfip_tcp_close(struct vsfsm_pt_t *pt, vsfsm_evt_t evt,
							struct vsfip_socket_t *socket);

// for UDP
vsf_err_t vsfip_udp_send(struct vsfsm_pt_t *pt, vsfsm_evt_t evt,
			struct vsfip_socket_t *socket, struct vsfip_sockaddr_t *sockaddr,
			struct vsfip_buffer_t *buf);
vsf_err_t vsfip_udp_recv(struct vsfsm_pt_t *pt, vsfsm_evt_t evt,
			struct vsfip_socket_t *socket, struct vsfip_sockaddr_t *sockaddr,
			struct vsfip_buffer_t **buf);

#endif		// __VSFIP_H_INCLUDED__
