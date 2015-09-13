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

#ifndef __VSFTIMER_H_INCLUDED__
#define __VSFTIMER_H_INCLUDED__

#include "tool/list/list.h"
#include "framework/vsfsm/vsfsm.h"

struct vsftimer_t
{
	struct vsfsm_t *sm;
	vsfsm_evt_t evt;
	uint32_t interval;
	int trigger_cnt;

	// private
	struct sllist list;
	uint32_t trigger_tick;
};

vsf_err_t vsftimer_init(void);
// call vsftimer_callback_int in hw timer interrupt
void vsftimer_callback_int(void);

struct vsftimer_t *vsftimer_create(struct vsfsm_t *sm, uint32_t interval,
							int16_t trigger_cnt, vsfsm_evt_t evt);
void vsftimer_free(struct vsftimer_t *timer);

void vsftimer_enqueue(struct vsftimer_t *timer);
void vsftimer_dequeue(struct vsftimer_t *timer);

#define vsfsm_pt_delay(pt, tick)		\
	do {\
		if (NULL == vsfimer_create((pt)->sm, (tick), 1, VSFSM_EVT_DELAY_DONE))\
		{\
			return VSFERR_NOT_ENOUGH_RESOURCES;\
		}\
		vsfsm_pt_wfe(pt, VSFSM_EVT_DELAY_DONE);\
	} while (0)

#endif	// #ifndef __VSFTIMER_H_INCLUDED__
