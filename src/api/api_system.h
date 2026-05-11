/**
 * @file    api_system.h
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   System Control API — Facade for system lifecycle and kernel services.
 * @date    11 May 2026
 * @version 1.0.0
 */

#ifndef API_INC_API_SYSTEM_H_
#define API_INC_API_SYSTEM_H_

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
 * @brief System States (Mirrors sys_lifecycle states).
 */
typedef enum
{
    API_SYS_STATE_INIT = 0,     /**< Initialization state. */
    API_SYS_STATE_RUNNING,      /**< Normal operating state. */
    API_SYS_STATE_SLEEP,        /**< Low-power sleep state. */
    API_SYS_STATE_FAULT         /**< System fault/error state. */
} api_sys_state_t;

/**
 * @brief System Events (Mirrors sys_lifecycle events).
 */
typedef enum
{
    API_SYS_EVT_NONE = 0,       /**< No event. */
    API_SYS_EVT_START,          /**< System start event. */
    API_SYS_EVT_SLEEP_REQ,      /**< Sleep request event. */
    API_SYS_EVT_WAKEUP,         /**< Wakeup event. */
    API_SYS_EVT_CRITICAL        /**< Critical fault event. */
} api_sys_evt_t;

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Initializes the system core services.
 */
void api_sys_init(void);

/**
 * @brief Starts the system runtime (Kernel).
 */
void api_sys_start(void);

/**
 * @brief Dispatches an event to the system state machine.
 *
 * @param[in] event Event to process immediately.
 */
void api_sys_dispatch(api_sys_evt_t event);

/**
 * @brief Processes pending system requests.
 */
void api_sys_process_requests(void);

/**
 * @brief Retrieves the current system state.
 *
 * @return Current system state (e.g. RUNNING, SLEEP).
 */
api_sys_state_t api_sys_get_state(void);

/**
 * @brief Retrieves the currently pending system event.
 *
 * @return Pending event identifier.
 */
api_sys_evt_t api_sys_get_pending_event(void);

#ifdef __cplusplus
}
#endif

#endif /* API_INC_API_SYSTEM_H_ */
