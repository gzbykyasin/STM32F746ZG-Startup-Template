/**
 * @file    sys_lifecycle.h
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   System Lifecycle Manager — Orchestrates system states and events.
 * @date    6 May 2026
 * @version 1.0.0
 */

#ifndef SYSTEM_INC_SYS_LIFECYCLE_H_
#define SYSTEM_INC_SYS_LIFECYCLE_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>

/*******************************************************************************
 * Typedef, Enum & Structs
 ******************************************************************************/

/**
 * @brief System Operational States.
 */
typedef enum
{
    SYS_STATE_INIT = 0,     /**< Initialization state. */
    SYS_STATE_RUNNING,      /**< Normal operating state. */
    SYS_STATE_SLEEP,        /**< Low-power sleep state. */
    SYS_STATE_FAULT         /**< System fault/error state. */
} sys_state_t;

/**
 * @brief System Lifecycle Events.
 */
typedef enum
{
    SYS_EVT_NONE = 0,       /**< No event. */
    SYS_EVT_START,          /**< System start event. */
    SYS_EVT_SLEEP_REQ,      /**< Sleep request event. */
    SYS_EVT_WAKEUP,         /**< Wakeup event. */
    SYS_EVT_CRITICAL        /**< Critical fault event. */
} sys_event_t;

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Initializes the system lifecycle manager and its internal FSM.
 */
void sys_lifecycle_init(void);

/**
 * @brief Dispatches an event to the lifecycle state machine.
 *
 * @param[in] event Event to process immediately.
 */
void sys_lifecycle_dispatch(sys_event_t event);

/**
 * @brief Requests an event (ISR-safe).
 *        The event will be processed during the next call to process_requests.
 *
 * @param[in] event Event to request.
 */
void sys_lifecycle_request(sys_event_t event);

/**
 * @brief Processes pending event requests from the request queue.
 */
void sys_lifecycle_process_requests(void);

/**
 * @brief Returns the current system operational state.
 *
 * @return Current system state (e.g. RUNNING, SLEEP).
 */
sys_state_t sys_lifecycle_get_state(void);

/**
 * @brief Returns the currently pending event request.
 *
 * @return Pending event identifier.
 */
sys_event_t sys_lifecycle_get_pending_event(void);


#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_INC_SYS_LIFECYCLE_H_ */
