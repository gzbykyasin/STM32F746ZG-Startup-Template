/**
 * @file    api_task.c
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Generic Tasking API Implementation.
 * @date    11 May 2026
 * @version 1.0.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "api_task.h"
#include "sys_rtos.h"

#include <stddef.h>

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/** @brief */
api_task_handle_t api_task_create(api_task_fn_t fn, 
                                  const char *p_name,
                                  api_task_prio_t priority, 
                                  uint32_t stack_sz, void *p_arg)
{
    sys_rtos_task_t handle = NULL;
    sys_rtos_priority_t rtos_prio = SYS_RTOS_PRIO_NORMAL;
    
    switch (priority)
    {
        case API_TASK_PRIO_IDLE:     rtos_prio = SYS_RTOS_PRIO_IDLE; break;
        case API_TASK_PRIO_LOW:      rtos_prio = SYS_RTOS_PRIO_LOW; break;
        case API_TASK_PRIO_NORMAL:   rtos_prio = SYS_RTOS_PRIO_NORMAL; break;
        case API_TASK_PRIO_HIGH:     rtos_prio = SYS_RTOS_PRIO_HIGH; break;
        case API_TASK_PRIO_REALTIME: rtos_prio = SYS_RTOS_PRIO_REALTIME; break;
    }

    (void)sys_rtos_task_create(fn, p_name, rtos_prio, stack_sz, p_arg, &handle);
    
    return (api_task_handle_t)handle;
}

/** @brief */
uint32_t api_task_get_tick(void)
{
    return sys_rtos_get_tick();
}

/** @brief */
bool api_task_should_loop(void)
{
    return sys_rtos_task_should_loop();
}

/** @brief */
void api_task_delay_ms(uint32_t ms)
{
    (void)sys_rtos_delay_ms(ms);
}

/** @brief */
void api_task_suspend(api_task_handle_t handle)
{
    (void)sys_rtos_task_suspend((sys_rtos_task_t)handle);
}

/** @brief */
void api_task_resume(api_task_handle_t handle)
{
    (void)sys_rtos_task_resume((sys_rtos_task_t)handle);
}

/* End of File: api_task.c */
