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
#include "vsfsm.h"

struct vsfsm_evtqueue_t
{
	struct vsfsm_t *sm;
	vsfsm_evt_t evt;
};

static struct vsfsm_evtqueue_t vsfsm_evtq[VSFSM_CFG_EVTQ_SIZE];
static struct vsfsm_evtqueue_t *vsfsm_evtq_head = &vsfsm_evtq[0];
static volatile struct vsfsm_evtqueue_t *vsfsm_evtq_tail = &vsfsm_evtq[0];
static volatile uint32_t vsfsm_evt_count = 0;

// vsfsm_get_event_pending should be called with interrupt disabled
uint32_t vsfsm_get_event_pending(void)
{
	return vsfsm_evt_count;
}

static vsf_err_t vsfsm_evtq_post(struct vsfsm_t *sm, vsfsm_evt_t evt)
{
	vsf_enter_critical();
	
	if (vsfsm_evt_count >= VSFSM_CFG_EVTQ_SIZE)
	{
		vsf_leave_critical();
		return VSFERR_NOT_ENOUGH_RESOURCES;
	}
	
	vsfsm_evtq_tail->sm = sm;
	vsfsm_evtq_tail->evt = evt;
	(vsfsm_evtq_tail == &vsfsm_evtq[VSFSM_CFG_EVTQ_SIZE - 1]) ?
		vsfsm_evtq_tail = &vsfsm_evtq[0] : vsfsm_evtq_tail++;
	sm->evt_count++;
	vsfsm_evt_count++;
	
	vsf_leave_critical();
	
	return VSFERR_NONE;
}

#if VSFSM_CFG_SM_EN && VSFSM_CFG_HSM_EN
static bool vsfsm_is_in(struct vsfsm_state_t *s, struct vsfsm_state_t *t)
{
	while (t != NULL)
	{
		if (s == t)
		{
			return true;
		}
		t = t->super;
	}
	return false;
}
#endif

static vsf_err_t vsfsm_dispatch_evt(struct vsfsm_t *sm, vsfsm_evt_t evt)
{
#if VSFSM_CFG_SM_EN && VSFSM_CFG_HSM_EN
	struct vsfsm_state_t *temp_state = NULL, *lca_state;
	struct vsfsm_state_t *temp_processor_state, *temp_target_state;
	struct vsfsm_state_t *processor_state = sm->cur_state;
	struct vsfsm_state_t *target_state = processor_state->evt_handler(sm, evt);
#elif VSFSM_CFG_SM_EN
	struct vsfsm_state_t *target_state = sm->cur_state->evt_handler(sm, evt);
#else
	sm->init_state.evt_handler(sm, evt);
#endif
	
#if !VSFSM_CFG_SM_EN
	return VSFERR_NONE;
#else
	// local event can not transmit or be passed to superstate
	if (evt >= VSFSM_EVT_LOCAL)
	{
		return VSFERR_NONE;
	}
	
#if VSFSM_CFG_HSM_EN
	// superstate
	while (target_state == (struct vsfsm_state_t *)-1)
	{
		processor_state = sm->cur_state->super;
		if (processor_state != NULL)
		{
			target_state = processor_state->evt_handler(sm, evt);
		}
	}
#endif
	
	if ((NULL == target_state)
#if !VSFSM_CFG_HSM_EN
		|| ((struct vsfsm_state_t *)-1 == target_state)
#endif
		)
	{
		// handled, or even topstate can not handle this event
		return VSFERR_NONE;
	}
	
	// need to transmit
#if VSFSM_CFG_HSM_EN
	// 1. exit to processor_state
	for (temp_state = sm->cur_state; temp_state != processor_state;)
	{
		temp_state->evt_handler(sm, VSFSM_EVT_EXIT);
		temp_state = temp_state->super;
	}
	// 2. some simple transition which happens in most cases
	if ((processor_state == target_state) ||
		(processor_state->super == target_state->super))
	{
		processor_state->evt_handler(sm, VSFSM_EVT_EXIT);
		target_state->evt_handler(sm, VSFSM_EVT_ENTER);
		goto update_cur_state;
	}
	if (processor_state->super == target_state)
	{
		processor_state->evt_handler(sm, VSFSM_EVT_EXIT);
		goto update_cur_state;
	}
	if (processor_state == target_state->super)
	{
		target_state->evt_handler(sm, VSFSM_EVT_ENTER);
		goto update_cur_state;
	}
	// 3. find the LCA
	lca_state = NULL;
	temp_processor_state = processor_state;
	temp_target_state = target_state;
	do
	{
		if (temp_processor_state != NULL)
		{
			if (vsfsm_is_in(temp_processor_state, target_state))
			{
				lca_state = temp_processor_state;
				break;
			}
			temp_processor_state = temp_processor_state->super;
		}
		if (temp_target_state != NULL)
		{
			if (vsfsm_is_in(temp_target_state, processor_state))
			{
				lca_state = temp_target_state;
				break;
			}
			temp_target_state = temp_target_state->super;
		}
		if ((NULL == temp_processor_state) && (NULL == temp_target_state))
		{
			return VSFERR_BUG;
		}
	} while (NULL == lca_state);
	// 4. exit from processor_state to lca
	for (temp_state = processor_state; temp_state != lca_state;)
	{
		temp_state->evt_handler(sm, VSFSM_EVT_EXIT);
		temp_state = temp_state->super;
	}
	// 5. enter from lca to target_state
	for (temp_state = lca_state; temp_state != target_state;)
	{
		temp_state->evt_handler(sm, VSFSM_EVT_ENTER);
		temp_state = temp_state->super;
	}
	// 6. update cur_state
update_cur_state:
	sm->cur_state = target_state;
	// 7. send VSFSM_EVT_INIT to target_state
#else
	sm->cur_state->evt_handler(sm, VSFSM_EVT_EXIT);
	sm->cur_state = target_state;
	sm->cur_state->evt_handler(sm, VSFSM_EVT_ENTER);
#endif
	return vsfsm_post_evt(sm, VSFSM_EVT_INIT);
#endif
}

#if VSFSM_CFG_HSM_EN
static struct vsfsm_state_t *
vsfsm_top_handler(struct vsfsm_t *sm, vsfsm_evt_t evt)
{
	REFERENCE_PARAMETER(sm);
	REFERENCE_PARAMETER(evt);
	return NULL;
}
struct vsfsm_state_t vsfsm_top = {vsfsm_top_handler};
#endif

#if (VSFSM_CFG_SM_EN && VSFSM_CFG_SUBSM_EN) || VSFSM_CFG_HSM_EN
static bool vsfsm_subsm_exists(struct vsfsm_state_t *state, struct vsfsm_t *sm)
{
	struct vsfsm_t *sm_temp = state->subsm;
	while (sm_temp != NULL)
	{
		if (sm_temp == sm)
		{
			return true;
		}
		sm_temp = sm_temp->next;
	}
	return false;
}

vsf_err_t vsfsm_add_subsm(struct vsfsm_state_t *state, struct vsfsm_t *sm)
{
	if (!vsfsm_subsm_exists(state, sm))
	{
		if (NULL == state->subsm)
		{
			sm->next = NULL;
		}
		else
		{
			sm->next = state->subsm;
		}
		state->subsm = sm;
	}
	return VSFERR_NONE;
}

vsf_err_t vsfsm_remove_subsm(struct vsfsm_state_t *state, struct vsfsm_t *sm)
{
	struct vsfsm_t *sm_temp = state->subsm;
	if (sm_temp == sm)
	{
		state->subsm = sm->next;
	}
	else
	{
		while (sm_temp != NULL)
		{
			if (sm_temp->next == sm)
			{
				sm_temp->next = sm->next;
				break;
			}
			sm_temp = sm_temp->next;
		}
	}
	return VSFERR_NONE;
}
#endif

vsf_err_t vsfsm_init(struct vsfsm_t *sm)
{
	sm->evt_count = 0;
#if VSFSM_CFG_SYNC_EN
	sm->pending_next = NULL;
#endif
#if VSFSM_CFG_SM_EN || VSFSM_CFG_HSM_EN
	sm->cur_state = &sm->init_state;
#endif
	// ignore any state transition on VSFSM_EVT_ENTER
	sm->init_state.evt_handler(sm, VSFSM_EVT_ENTER);
#if VSFSM_CFG_ACTIVE_EN
	// set active so that sm can accept events
	vsfsm_set_active(sm, true);
#endif
	// process state transition on VSFSM_EVT_INIT
	return vsfsm_post_evt(sm, VSFSM_EVT_INIT);
}

vsf_err_t vsfsm_poll(void)
{
	struct vsfsm_t *sm;
	
	while (vsfsm_evt_count)
	{
		sm = vsfsm_evtq_head->sm;
		vsfsm_dispatch_evt(sm, vsfsm_evtq_head->evt);
		(vsfsm_evtq_head == &vsfsm_evtq[VSFSM_CFG_EVTQ_SIZE - 1]) ?
			vsfsm_evtq_head = &vsfsm_evtq[0] : vsfsm_evtq_head++;
		vsf_enter_critical();
		sm->evt_count--;
		vsfsm_evt_count--;
		vsf_leave_critical();
	}
	return VSFERR_NONE;
}

#if VSFSM_CFG_ACTIVE_EN
vsf_err_t vsfsm_set_active(struct vsfsm_t *sm, bool active)
{
	vsf_enter_critical();
	sm->active = active;
	vsf_leave_critical();
	return VSFERR_NONE;
}
#endif

vsf_err_t vsfsm_post_evt(struct vsfsm_t *sm, vsfsm_evt_t evt)
{
	return
#if VSFSM_CFG_ACTIVE_EN
			(!sm->active) ? VSFERR_FAIL :
#endif
			((evt >= VSFSM_EVT_INSTANT) &&
				(evt <= VSFSM_EVT_INSTANT_END)) ||
			((evt >= VSFSM_EVT_LOCAL_INSTANT) &&
				(evt <= VSFSM_EVT_LOCAL_INSTANT_END)) ||
			(0 == sm->evt_count) ?
				vsfsm_dispatch_evt(sm, evt) : vsfsm_evtq_post(sm, evt);
}

// pending event will be forced to be sent to event queue
vsf_err_t vsfsm_post_evt_pending(struct vsfsm_t *sm, vsfsm_evt_t evt)
{
	return
#if VSFSM_CFG_ACTIVE_EN
			(!sm->active) ||
#endif
			((evt >= VSFSM_EVT_INSTANT) &&
				(evt <= VSFSM_EVT_INSTANT_END)) ||
			((evt >= VSFSM_EVT_LOCAL_INSTANT) &&
				(evt <= VSFSM_EVT_LOCAL_INSTANT_END)) ?
				VSFERR_FAIL : vsfsm_evtq_post(sm, evt);
}

#if VSFSM_CFG_PT_EN
#include "interfaces.h"

struct vsfsm_state_t * vsfsm_pt_evt_handler(struct vsfsm_t *sm, vsfsm_evt_t evt)
{
	struct vsfsm_pt_t *pt = (struct vsfsm_pt_t *)sm->user_data;
	
	switch (evt)
	{
	case VSFSM_EVT_ENTER:
		break;
	case VSFSM_EVT_INIT:
		pt->state = 0;
		// fall through
	default:
		{
		#if VSFSM_CFG_PT_STACK_EN
			uint32_t stack;
			bool pt_stack_enable = false;
			// pt->stack maybe modified in thread, eg: thread can setup the stack
			if (pt->stack != NULL)
			{
				pt_stack_enable = true;
				stack = interfaces->core.get_stack();
				interfaces->core.set_stack((uint32_t)pt->stack);
			}
		#endif
			
			pt->thread(pt, evt);
			
		#if VSFSM_CFG_PT_STACK_EN
			if (pt_stack_enable)
			{
				interfaces->core.set_stack(stack);
			}
		#endif
		}
	}
	return NULL;
}

vsf_err_t vsfsm_pt_init(struct vsfsm_t *sm, struct vsfsm_pt_t *pt)
{
	sm->user_data = pt;
	sm->init_state.evt_handler = vsfsm_pt_evt_handler;
#if (VSFSM_CFG_SM_EN && VSFSM_CFG_SUBSM_EN) || VSFSM_CFG_HSM_EN
	sm->init_state.subsm = NULL;
#endif
	pt->sm = sm;
	return vsfsm_init(sm);
}
#endif

#if VSFSM_CFG_SYNC_EN
// vsfsm_sync_t
vsf_err_t vsfsm_sync_init(struct vsfsm_sync_t *sync, uint32_t cur_value,
				uint32_t max_value, vsfsm_evt_t evt)
{
	sync->cur_value = cur_value;
	sync->max_value = max_value;
	sync->sm_pending = NULL;
	sync->evt = evt;
	return VSFERR_NONE;
}

static void vsfsm_sync_append_sm(struct vsfsm_sync_t *sync, struct vsfsm_t *sm)
{
	struct vsfsm_t *sm_pending;
	
	sm->pending_next = NULL;
	if (NULL == sync->sm_pending)
	{
		sync->sm_pending = sm;
	}
	else
	{
		sm_pending = sync->sm_pending;
		while (sm_pending->pending_next != NULL)
		{
			sm_pending = sm_pending->pending_next;
		}
		sm_pending->pending_next = sm;
	}
}

vsf_err_t vsfsm_sync_cancel(struct vsfsm_t *sm, struct vsfsm_sync_t *sync)
{
	struct vsfsm_t *sm_pending;
	
	if (sync->sm_pending == sm)
	{
		sync->sm_pending = sm->pending_next;
	}
	else
	{
		sm_pending = sync->sm_pending;
		while (sm_pending->pending_next != NULL)
		{
			if (sm_pending->pending_next == sm)
			{
				sm_pending->pending_next = sm->pending_next;
				break;
			}
			sm_pending = sm_pending->pending_next;
		}
	}
	return VSFERR_NONE;
}

vsf_err_t vsfsm_sync_increase(struct vsfsm_sync_t *sync)
{
	if (sync->sm_pending)
	{
		if (vsfsm_post_evt(sync->sm_pending, sync->evt))
		{
			// should increase the evtq buffer size
			return VSFERR_BUG;
		}
		sync->sm_pending = sync->sm_pending->pending_next;
	}
	else if (sync->cur_value < sync->max_value)
	{
		sync->cur_value++;
	}
	else
	{
		return VSFERR_BUG;
	}
	return VSFERR_NONE;
}

vsf_err_t vsfsm_sync_decrease(struct vsfsm_sync_t *sync, struct vsfsm_t *sm)
{
	if (sync->cur_value > 0)
	{
		sync->cur_value--;
		return VSFERR_NONE;
	}
	vsfsm_sync_append_sm(sync, sm);
	return VSFERR_NOT_READY;
}
#endif	// VSFSM_CFG_SYNC_EN
