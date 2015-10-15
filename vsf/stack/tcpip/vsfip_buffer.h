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
#ifndef __VSFIP_BUFFER_H_INCLUDED__
#define __VSFIP_BUFFER_H_INCLUDED__

#include "vsfip_cfg.h"

#include "tool/buffer/buffer.h"

struct vsfip_netif_t;
struct vsfip_buffer_t
{
	// inherent from vsfq_node_t
	struct vsfq_node_t proto_node;
	struct vsfq_node_t netif_node;

	struct vsf_buffer_t buf;
	struct vsf_buffer_t app;

	union
	{
		uint8_t *ipver;
		struct vsfip_ip4head_t *ip4head;
//		struct vsfip_ip6head_t *ip6head;
	} iphead;

	uint16_t ref;
	uint16_t ttl;
	uint16_t retry;

	uint8_t *buffer;
//	uint32_t size;

	struct vsfip_netif_t *netif;
};

void vsfip_buffer_init(void);
struct vsfip_buffer_t* vsfip_buffer_get(uint32_t size);
struct vsfip_buffer_t* vsfip_appbuffer_get(uint32_t header, uint32_t app);
void vsfip_buffer_reference(struct vsfip_buffer_t *buf);
void vsfip_buffer_release(struct vsfip_buffer_t *buf);

#define VSFIP_BUF_GET(s)		vsfip_buffer_get(s)
#define VSFIP_NETIFBUF_GET(s)	VSFIP_BUF_GET((s) + VSFIP_CFG_NETIF_HEADLEN)
#define VSFIP_IPBUF_GET(s)		VSFIP_NETIFBUF_GET((s) + VSFIP_IP_HEADLEN)

#define VSFIP_PROTO_HEADLEN		(VSFIP_CFG_NETIF_HEADLEN + VSFIP_IP_HEADLEN)
#define VSFIP_UDPBUF_GET(s)		vsfip_appbuffer_get(VSFIP_PROTO_HEADLEN + VSFIP_UDP_HEADLEN, (s))
#define VSFIP_TCPBUF_GET(s)		vsfip_appbuffer_get(VSFIP_PROTO_HEADLEN + VSFIP_TCP_HEADLEN, (s))

#endif		// __VSFIP_BUFFER_H_INCLUDED__
