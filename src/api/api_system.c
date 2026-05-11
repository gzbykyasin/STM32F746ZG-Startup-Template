/**
 * @file    api_system.c
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   System Control API Implementation.
 * @date    11 May 2026
 * @version 1.0.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "api_system.h"
#include "sys_lifecycle.h"
#include "sys_rtos.h"

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/** @brief */
void api_sys_init(void)
{
    sys_lifecycle_init();
}

/** @brief */
void api_sys_start(void)
{
    sys_rtos_kernel_start();
}

/** @brief */
void api_sys_dispatch(api_sys_evt_t event)
{
    sys_lifecycle_dispatch((sys_event_t)event);
}

/** @brief */
void api_sys_process_requests(void)
{
    sys_lifecycle_process_requests();
}

/** @brief */
api_sys_state_t api_sys_get_state(void)
{
    return (api_sys_state_t)sys_lifecycle_get_state();
}

/** @brief */
api_sys_evt_t api_sys_get_pending_event(void)
{
    return (api_sys_evt_t)sys_lifecycle_get_pending_event();
}

/* End of File: api_system.c */
