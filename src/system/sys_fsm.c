/**
 * @file    sys_fsm.c
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Generic Finite State Machine engine implementation.
 * @date    5 May 2026
 * @version 1.0.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "sys_fsm.h"

#include <stddef.h>

/*******************************************************************************
 * Public Function Definitions
 ******************************************************************************/

/** @brief */
void sys_fsm_init(sys_fsm_t *p_fsm, 
                  const sys_fsm_transition_t *p_transitions, uint8_t transitions_len,
                  const sys_fsm_state_handler_t *p_state_handlers, uint8_t states_count,
                  sys_fsm_state_t init_state)
{
    if ((NULL == p_fsm) || (NULL == p_transitions) || (0U == transitions_len) || (NULL == p_state_handlers))
    {
        return;
    }

    p_fsm->p_transitions = p_transitions;
    p_fsm->transitions_len = transitions_len;
    p_fsm->p_state_handlers = p_state_handlers;
    p_fsm->states_count = states_count;
    p_fsm->current_state = init_state;

    if (init_state < states_count)
    {
        if (NULL != p_fsm->p_state_handlers[init_state].enter)
        {
            p_fsm->p_state_handlers[init_state].enter();
        }
    }
}

/** @brief */
bool sys_fsm_dispatch(sys_fsm_t *p_fsm, sys_fsm_event_t event)
{
    if (NULL == p_fsm)
    {
        return false;
    }

    const sys_fsm_transition_t *p_entry = p_fsm->p_transitions;

    for (uint8_t i = 0U; i < p_fsm->transitions_len; i++, p_entry++)
    {
        const bool state_match = (p_entry->from_state == p_fsm->current_state) ||
                                 (p_entry->from_state == SYS_FSM_ANY_STATE);
        const bool event_match = (p_entry->event == event);

        if (state_match && event_match)
        {
            sys_fsm_state_t old_state = p_fsm->current_state;
            sys_fsm_state_t next_state = p_entry->to_state;

            if (old_state < p_fsm->states_count)
            {
                if (NULL != p_fsm->p_state_handlers[old_state].exit)
                {
                    p_fsm->p_state_handlers[old_state].exit();
                }
            }

            p_fsm->current_state = next_state;

            if (next_state < p_fsm->states_count)
            {
                if (NULL != p_fsm->p_state_handlers[next_state].enter)
                {
                    p_fsm->p_state_handlers[next_state].enter();
                }
            }

            return true;
        }
    }

    return false;
}

/** @brief */
sys_fsm_state_t sys_fsm_get_state(const sys_fsm_t *p_fsm)
{
    if (NULL == p_fsm)
    {
        return 0U;
    }

    return p_fsm->current_state;
}

/** @brief */
void sys_fsm_process(sys_fsm_t *p_fsm)
{
    if (NULL == p_fsm)
    {
        return;
    }

    sys_fsm_state_t current = p_fsm->current_state;

    if (current < p_fsm->states_count)
    {
        if (NULL != p_fsm->p_state_handlers[current].process)
        {
            p_fsm->p_state_handlers[current].process();
        }
    }
}

/* End of File: sys_fsm.c */
