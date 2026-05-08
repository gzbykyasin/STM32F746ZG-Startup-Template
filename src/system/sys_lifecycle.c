/**
 * @file    sys_lifecycle.c
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   System Lifecycle Management using Generic FSM Engine.
 * @date    7 May 2026
 * @version 1.0.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "sys_lifecycle.h"
#include "sys_fsm.h"
#include "sys_logger.h"
#include "sys_rtos.h"

#include "bsp_system.h"
#include "bsp_gpio.h"
#include "bsp_mcu.h"

#include <stddef.h>

/*******************************************************************************
 * Defines
 ******************************************************************************/

/** @brief Button debounce filter time in ms. */
#define SYS_BTN_FILTER_MS             500U
/** @brief Timeout for button release during sleep transition. */
#define SYS_SLEEP_RELEASE_TIMEOUT_MS  2000U
/** @brief Delay for hardware stabilization before sleep. */
#define SYS_SLEEP_SETTLE_DELAY_MS     50U
/** @brief Short yield delay during polling. */
#define SYS_SLEEP_YIELD_DELAY_MS      10U
/** @brief User button interrupt priority. */
#define SYS_BTN_IRQ_PRIORITY          6U

/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

/** @brief Handler for FSM INIT state entry. */
static void state_init_enter(void);
/** @brief Handler for FSM INIT state exit. */
static void state_init_exit(void);
/** @brief Handler for FSM RUNNING state entry. */
static void state_running_enter(void);
/** @brief Handler for FSM RUNNING state exit. */
static void state_running_exit(void);
/** @brief Handler for FSM SLEEP state entry. */
static void state_sleep_enter(void);
/** @brief Handler for FSM SLEEP state exit. */
static void state_sleep_exit(void);
/** @brief Handler for FSM FAULT state entry. */
static void state_fault_enter(void);

/** @brief Callback for user button interrupt events. */
static void on_button_event(void *p_arg);
/** @brief Callback for system hardware fault events. */
static void on_hardware_fault(void *p_arg);

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/** @brief Last tick value for button debounce. */
static uint32_t s_btn_last_tick = 0U;
/** @brief Main system lifecycle FSM instance. */
static sys_fsm_t s_lifecycle_fsm;
/** @brief Pending lifecycle event for deferred processing. */
static volatile sys_event_t s_pending_event = SYS_EVT_NONE;

/** @brief Lifecycle state handlers mapping. */
static const sys_fsm_state_handler_t s_state_handlers[] = {
    [SYS_STATE_INIT]    = { .enter = state_init_enter,    .exit = state_init_exit,    .process = NULL },
    [SYS_STATE_RUNNING] = { .enter = state_running_enter, .exit = state_running_exit, .process = NULL },
    [SYS_STATE_SLEEP]   = { .enter = state_sleep_enter,   .exit = state_sleep_exit,   .process = NULL },
    [SYS_STATE_FAULT]   = { .enter = state_fault_enter,   .exit = NULL,               .process = NULL }
};

/** @brief Lifecycle state transition rules. */
static const sys_fsm_transition_t s_transitions[] = {
    { SYS_STATE_INIT,    SYS_EVT_START,     SYS_STATE_RUNNING },
    { SYS_STATE_RUNNING, SYS_EVT_SLEEP_REQ, SYS_STATE_SLEEP   },
    { SYS_STATE_SLEEP,   SYS_EVT_WAKEUP,    SYS_STATE_RUNNING },
    { SYS_FSM_ANY_STATE, SYS_EVT_CRITICAL,  SYS_STATE_FAULT   }
};

/*******************************************************************************
 * Public Function Definitions
 ******************************************************************************/

/**
 * @brief
 */
void sys_lifecycle_init(void)
{
    sys_fsm_init(&s_lifecycle_fsm, 
                 s_transitions, (uint8_t)(sizeof(s_transitions) / sizeof(s_transitions[0])),
                 s_state_handlers, (uint8_t)(sizeof(s_state_handlers) / sizeof(s_state_handlers[0])),
                 SYS_STATE_INIT);
}

/**
 * @brief
 */
void sys_lifecycle_dispatch(sys_event_t event)
{
    sys_state_t old_state = (sys_state_t)sys_fsm_get_state(&s_lifecycle_fsm);
    
    if (sys_fsm_dispatch(&s_lifecycle_fsm, (sys_fsm_event_t)event))
    {
        sys_state_t new_state = (sys_state_t)sys_fsm_get_state(&s_lifecycle_fsm);
        SYS_LOG_INFO("SYS", "Lifecycle: %d -> %d (Evt: %d)", (int)old_state, (int)new_state, (int)event);
    }
}

/**
 * @brief
 */
void sys_lifecycle_request(sys_event_t event)
{
    s_pending_event = event;
}

/**
 * @brief
 */
void sys_lifecycle_process_requests(void)
{
    if (s_pending_event != SYS_EVT_NONE)
    {
        sys_event_t evt = s_pending_event;
        s_pending_event = SYS_EVT_NONE;
        sys_lifecycle_dispatch(evt);
    }
}

/**
 * @brief
 */
sys_event_t sys_lifecycle_get_pending_event(void)
{
    return s_pending_event;
}

/**
 * @brief
 */
sys_state_t sys_lifecycle_get_state(void)
{
    return (sys_state_t) sys_fsm_get_state(&s_lifecycle_fsm);
}

/*******************************************************************************
 * State Action Handlers
 ******************************************************************************/

/** @brief */
static void state_init_enter(void)
{
    bsp_system_boot_core();
    sys_rtos_kernel_init();
    (void)sys_logger_init();
    
    bsp_system_register_fault_handlers(on_hardware_fault);
    bsp_system_setup_wakeup_source(on_button_event);
    bsp_isr_enable(BSP_ISR_EXTI_USER_BTN, SYS_BTN_IRQ_PRIORITY, 0U);

    SYS_LOG_INFO("SYS", "Lifecycle: INIT -> Core system ready");
}

/** @brief */
static void state_init_exit(void)
{
    SYS_LOG_INFO("SYS", "Lifecycle: INIT -> Leaving initial state");
}

/** @brief */
static void state_running_enter(void)
{
    bsp_system_init_peripherals();
    bsp_mcu_delay_ms(150);
    SYS_LOG_INFO("SYS", "Lifecycle: RUNNING -> System is now operational");
}

/** @brief */
static void state_running_exit(void)
{
    SYS_LOG_INFO("SYS", "Lifecycle: RUNNING -> Exiting operational mode");
}

/** @brief */
static void state_sleep_enter(void)
{
    SYS_LOG_INFO("SYS", "Lifecycle: SLEEP -> Preparing for power down");
    sys_logger_flush();

    sys_rtos_enter_critical();
    
    while (bsp_gpio_read(BSP_GPIO_USER_BTN) == BSP_GPIO_HIGH)
    {
        for (volatile uint32_t i = 0; i < 1000; i++);
    }

    bsp_mcu_delay_busy_ms(SYS_SLEEP_SETTLE_DELAY_MS);

    (void)bsp_gpio_clear_pending(BSP_GPIO_USER_BTN);
    bsp_isr_clear_pending(BSP_ISR_EXTI_USER_BTN);
    s_pending_event = SYS_EVT_NONE;

    bsp_system_power_enter_stop();
    bsp_system_power_exit_stop();

    bsp_mcu_delay_busy_ms(10);

    sys_rtos_exit_critical();
    sys_rtos_kernel_wake_tasks();
}

/** @brief */
static void state_sleep_exit(void)
{
    SYS_LOG_INFO("SYS", "Lifecycle: SLEEP -> Wakeup sequence triggered");
}

/** @brief */
static void state_fault_enter(void)
{
    SYS_LOG_ERROR("SYS", "Lifecycle: FAULT -> System halting for safety");
    sys_logger_flush();
    bsp_system_critical_error_reset();
}

/*******************************************************************************
 * Interrupt Callbacks
 ******************************************************************************/

/** @brief */
static void on_button_event(void *p_arg)
{
    (void)p_arg;
    uint32_t now = sys_rtos_get_tick();
    sys_state_t current = sys_lifecycle_get_state();

    if (current == SYS_STATE_RUNNING)
    {
        if ((sys_lifecycle_get_pending_event() != SYS_EVT_NONE) || 
            ((now - s_btn_last_tick) < SYS_BTN_FILTER_MS))
        {
            return; 
        }
    }

    if (current == SYS_STATE_RUNNING)
    {
        s_btn_last_tick = now;
        sys_lifecycle_request(SYS_EVT_SLEEP_REQ);
    }
    else if (current == SYS_STATE_SLEEP)
    {
        s_btn_last_tick = now;
        sys_lifecycle_request(SYS_EVT_WAKEUP);
    }
}

/** @brief */
static void on_hardware_fault(void *p_arg)
{
    SYS_LOG_ERROR("SYS", "Lifecycle: HARD FAULT DETECTED!");
    
    if (p_arg)
    {
        bsp_isr_fault_t *reg = (bsp_isr_fault_t *)p_arg;
        SYS_LOG_ERROR("SYS", "PC:%x, LR:%x, PSR:%x", reg->pc, reg->lr, reg->psr);
    }

    sys_lifecycle_request(SYS_EVT_CRITICAL);
}

/* End of File: sys_lifecycle.c */
