/**
 * @file    api_app_manager.c
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Application Manager implementation — Agnostic Orchestration.
 * @date    7 May 2026
 * @version 1.0.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "api_app_manager.h"
#include "api_ambient.h"
#include "app_temp.h"
#include "app_led.h"

#include "sys_lifecycle.h"
#include "sys_logger.h"
#include "sys_rtos.h"

#include <stddef.h>

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/** @brief Handle for the temperature measurement task. */
static api_app_handle_t s_task_temp     = NULL;
/** @brief Handle for the LED status task. */
static api_app_handle_t s_task_led      = NULL;
/** @brief Handle for the main application orchestrator task. */
static api_app_handle_t s_task_manager  = NULL;
/** @brief Handle for the system logging task. */
static api_app_handle_t s_task_logger   = NULL;

/** @brief Stores the previous lifecycle state to detect transitions. */
static sys_state_t s_previous_state = SYS_STATE_INIT;

/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

/** @brief Main orchestrator thread that manages task lifecycles. */
static void task_manager_thread(void *p_arg);

/*******************************************************************************
 * Public Functions — Lifecycle
 ******************************************************************************/

/** @brief */
void api_app_manager_init(void)
{
    sys_lifecycle_init();

    api_ambient_init();
    app_temp_init();

    s_task_temp    = api_app_task_create(app_temp_task, "TempTask", API_APP_PRIO_NORMAL, 512U, NULL);
    s_task_led     = api_app_task_create(app_led_task, "LedTask", API_APP_PRIO_NORMAL, 512U, NULL);
    s_task_manager = api_app_task_create(task_manager_thread, "MgrTask", API_APP_PRIO_HIGH, 512U, NULL);
    s_task_logger  = api_app_task_create(sys_logger_task, "LoggerTask", API_APP_PRIO_LOW, 512U, NULL);

    s_previous_state = SYS_STATE_INIT;

    sys_lifecycle_dispatch(SYS_EVT_START);
    
    SYS_LOG_INFO("APP", "NUCLEO-F746ZG Startup Project Initialized");
}

/** @brief */
void api_app_manager_start(void)
{
    sys_rtos_kernel_start();
}

/*******************************************************************************
 * Private Task — The Orchestrator
 ******************************************************************************/

/** @brief */
static void task_manager_thread(void *p_arg)
{
    (void)p_arg;
    sys_state_t current_state;

    do {
        sys_event_t pending = sys_lifecycle_get_pending_event();

        if (SYS_EVT_SLEEP_REQ == pending)
        {
            (void)sys_rtos_task_suspend(s_task_temp);
            (void)sys_rtos_task_suspend(s_task_led);
            
            SYS_LOG_INFO("MGR", "Apps suspended. Entering sleep...");
        }

        sys_lifecycle_process_requests();

        current_state = sys_lifecycle_get_state();

        if (current_state != s_previous_state)
        {
            if (SYS_STATE_RUNNING == current_state && SYS_STATE_SLEEP == s_previous_state)
            {
                (void)sys_rtos_task_resume(s_task_temp);
                (void)sys_rtos_task_resume(s_task_led);
                SYS_LOG_INFO("MGR", "System Woke Up: Apps Resumed");
            }

            s_previous_state = current_state;
        }

        if (sys_lifecycle_get_pending_event() != SYS_EVT_NONE)
        {
            continue;
        }

        (void)sys_rtos_delay_ms(20U);
    } while (sys_rtos_task_should_loop());
}

/*******************************************************************************
 * Public Functions — Task Management
 ******************************************************************************/

/** @brief */
api_app_handle_t api_app_task_create(api_app_task_fn_t fn, 
                                     const char *p_name,
                                     api_app_prio_t priority, 
                                     uint32_t stack_sz, void *p_arg)
{
    sys_rtos_task_t handle = NULL;
    sys_rtos_priority_t rtos_prio = SYS_RTOS_PRIO_NORMAL;
    
    switch (priority)
    {
        case API_APP_PRIO_IDLE:     rtos_prio = SYS_RTOS_PRIO_IDLE; break;
        case API_APP_PRIO_LOW:      rtos_prio = SYS_RTOS_PRIO_LOW; break;
        case API_APP_PRIO_NORMAL:   rtos_prio = SYS_RTOS_PRIO_NORMAL; break;
        case API_APP_PRIO_HIGH:     rtos_prio = SYS_RTOS_PRIO_HIGH; break;
        case API_APP_PRIO_REALTIME: rtos_prio = SYS_RTOS_PRIO_REALTIME; break;
    }

    (void)sys_rtos_task_create(fn, p_name, rtos_prio, stack_sz, p_arg, &handle);
    
    return (api_app_handle_t)handle;
}

/** @brief */
uint32_t api_app_get_tick(void)
{
    return sys_rtos_get_tick();
}

/** @brief */
bool api_app_should_loop(void)
{
    return sys_rtos_task_should_loop();
}

/** @brief */
void api_app_delay_ms(uint32_t ms)
{
    (void)sys_rtos_delay_ms(ms);
}

/* End of File: api_app_manager.c */
