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

#include "compiler.h"

#include "vsftimer.h"
#include "interfaces.h"
#include "framework/vsfsm/vsfsm.h"

#include "tool/bittool/bittool.h"

static struct vsfsm_state_t *
vsftimer_init_handler(struct vsfsm_t *sm, vsfsm_evt_t evt);

struct vsftimer_info_t
{
	struct vsfsm_t sm;
	struct vsfq_t timerlist;
} static vsftimer =
{
	{
		{
			vsftimer_init_handler,
		},								// struct vsfsm_state_t init_state;
	},
};

// vsftimer_callback_int is called in interrupt,
// simply send event to vsftimer SM
void vsftimer_callback_int(void)
{
	vsfsm_post_evt_pending(&vsftimer.sm, VSFSM_EVT_TIMER);
}

static struct vsftimer_t vsftimer_pool[VSFTIMER_CFG_POOL_SIZE];
static uint32_t vsftimer_poll_mskarr[(VSFTIMER_CFG_POOL_SIZE + 31) / 32];
vsf_err_t vsftimer_init(void)
{
	memset(vsftimer_pool, 0, sizeof(vsftimer_pool));
	memset(vsftimer_poll_mskarr, 0, sizeof(vsftimer_poll_mskarr));
	
	vsfq_init(&vsftimer.timerlist);
	return vsfsm_init(&vsftimer.sm);
}

static struct vsftimer_t *vsftimer_allocate(void)
{
	int index = mskarr_ffz(vsftimer_poll_mskarr, dimof(vsftimer_poll_mskarr));
	if (index > VSFTIMER_CFG_POOL_SIZE)
	{
		return NULL;
	}
	mskarr_set(vsftimer_poll_mskarr, index);
	return &vsftimer_pool[index];
}

void vsftimer_enqueue(struct vsftimer_t *timer)
{
	timer->node.addr = timer->interval + core_interfaces.tickclk.get_count();
	vsftimer_dequeue(timer);
	vsfq_enqueue(&vsftimer.timerlist, &timer->node);
}

void vsftimer_dequeue(struct vsftimer_t *timer)
{
	vsfq_remove(&vsftimer.timerlist, &timer->node);
}

static struct vsfsm_state_t *
vsftimer_init_handler(struct vsfsm_t *sm, vsfsm_evt_t evt)
{
	uint32_t cur_tick = core_interfaces.tickclk.get_count();
	struct vsftimer_t *timer;
	
	switch (evt)
	{
	case VSFSM_EVT_TIMER:
		timer = (struct vsftimer_t *)vsftimer.timerlist.head;
		while (timer != NULL)
		{
			if (cur_tick >= timer->node.addr)
			{
				if (timer->trigger_cnt > 0)
				{
					timer->trigger_cnt--;
				}
				if (timer->trigger_cnt != 0)
				{
					vsftimer_enqueue(timer);
				}
				else
				{
					vsftimer_free(timer);
				}
				
				if ((timer->sm != NULL) && (timer->evt != VSFSM_EVT_INVALID))
				{
					vsfsm_post_evt(timer->sm, timer->evt);
				}
				timer = (struct vsftimer_t *)vsftimer.timerlist.head;
			}
			else
			{
				break;
			}
		}
		break;
	}
	return NULL;
}

struct vsftimer_t *vsftimer_create(struct vsfsm_t *sm, uint32_t interval,
									int16_t trigger_cnt, vsfsm_evt_t evt)
{
	struct vsftimer_t *timer = vsftimer_allocate();
	if (NULL == timer)
	{
		return NULL;
	}
	
	timer->sm = sm;
	timer->evt = evt;
	timer->interval = interval;
	timer->trigger_cnt = trigger_cnt;
	vsftimer_enqueue(timer);
	return timer;
}

void vsftimer_free(struct vsftimer_t *timer)
{
	vsftimer_dequeue(timer);
	if ((timer >= &vsftimer_pool[0]) &&
		(timer <= &vsftimer_pool[dimof(vsftimer_pool) - 1]))
	{
		// timer in pool
		mskarr_clr(vsftimer_poll_mskarr, timer - vsftimer_pool);
	}
}
