/**
 * @file    app_system.c
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Application System Orchestrator implementation.
 * @date    11 May 2026
 * @version 1.0.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "app_system.h"
#include "api_task.h"
#include "api_system.h"
#include "api_log.h"
#include "api_ambient.h"
#include "app_temp.h"
#include "app_led.h"

#include <stddef.h>

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/** @brief Handle for the temperature measurement task. */
static api_task_handle_t s_task_temp     = NULL;
/** @brief Handle for the LED status task. */
static api_task_handle_t s_task_led      = NULL;
/** @brief Handle for the main application orchestrator task. */
static api_task_handle_t s_task_manager  = NULL;
/** @brief Handle for the system logging task. */
static api_task_handle_t s_task_logger   = NULL;

/** @brief Stores the previous lifecycle state to detect transitions. */
static api_sys_state_t s_previous_state = API_SYS_STATE_INIT;

/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

/** @brief Main orchestrator task that manages task lifecycles. */
static void app_system_task(void *p_arg);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/** @brief */
void app_system_init(void)
{
    api_sys_init();

    api_ambient_init();
    app_temp_init();

    s_task_temp    = api_task_create(app_temp_task, "TempTask", API_TASK_PRIO_NORMAL, 512U, NULL);
    s_task_led     = api_task_create(app_led_task, "LedTask", API_TASK_PRIO_NORMAL, 512U, NULL);
    s_task_manager = api_task_create(app_system_task, "SysTask", API_TASK_PRIO_HIGH, 512U, NULL);
    s_task_logger  = api_task_create(api_log_task, "LoggerTask", API_TASK_PRIO_LOW, 512U, NULL);

    s_previous_state = API_SYS_STATE_INIT;

    api_sys_dispatch(API_SYS_EVT_START);
    
    API_LOG_INFO("APP", "NUCLEO-F746ZG Startup Project Initialized");
}

/** @brief */
void app_system_start(void)
{
    api_sys_start();
}

/*******************************************************************************
 * Private Task — The Orchestrator
 ******************************************************************************/

/** @brief */
static void app_system_task(void *p_arg)
{
    (void)p_arg;
    api_sys_state_t current_state;

    do {
        api_sys_evt_t pending = api_sys_get_pending_event();

        if (API_SYS_EVT_SLEEP_REQ == pending)
        {
            api_task_suspend(s_task_temp);
            api_task_suspend(s_task_led);
            
            API_LOG_INFO("APP_SYS", "Apps suspended. Entering sleep...");
        }

        api_sys_process_requests();

        current_state = api_sys_get_state();

        if (current_state != s_previous_state)
        {
            if (API_SYS_STATE_RUNNING == current_state && API_SYS_STATE_SLEEP == s_previous_state)
            {
                api_task_resume(s_task_temp);
                api_task_resume(s_task_led);
                API_LOG_INFO("APP_SYS", "System Woke Up: Apps Resumed");
            }

            s_previous_state = current_state;
        }

        if (api_sys_get_pending_event() != API_SYS_EVT_NONE)
        {
            continue;
        }

        api_task_delay_ms(20U);
    } while (api_task_should_loop());
}

/* End of File: app_system.c */
