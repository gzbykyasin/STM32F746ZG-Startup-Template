/**
 * @file    api_app_manager.h
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Application Manager — Orchestrates app lifecycle and tasking.
 * @date    6 May 2026
 * @version 1.0.0
 */

#ifndef API_INC_API_APP_MANAGER_H_
#define API_INC_API_APP_MANAGER_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * Typedef, Enum & Structs
 ******************************************************************************/

/**
 * @brief Opaque handle for application tasks.
 */
typedef void *api_app_handle_t;

/**
 * @brief Application Task Priorities.
 */
typedef enum
{
    API_APP_PRIO_IDLE = 0,      /**< Idle priority. */
    API_APP_PRIO_LOW,           /**< Low priority. */
    API_APP_PRIO_NORMAL,        /**< Normal priority. */
    API_APP_PRIO_HIGH,          /**< High priority. */
    API_APP_PRIO_REALTIME       /**< Real-time priority. */
} api_app_prio_t;

/**
 * @brief Application task function signature.
 * 
 * @param[in] p_arg Pointer to task arguments.
 */
typedef void (*api_app_task_fn_t)(void *p_arg);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Initializes the application manager and system services.
 */
void api_app_manager_init(void);

/**
 * @brief Starts the application manager loop.
 */
void api_app_manager_start(void);

/**
 * @brief Creates a new application task.
 * 
 * @param[in] fn        Task function pointer.
 * @param[in] p_name    Human-readable name for the task.
 * @param[in] priority  Task priority level.
 * @param[in] stack_sz  Stack size in bytes.
 * @param[in] p_arg     Pointer to arguments passed to the task.
 * @return Handle to the created task, or NULL on failure.
 */
api_app_handle_t api_app_task_create(api_app_task_fn_t fn, 
                                     const char *p_name,
                                     api_app_prio_t priority, 
                                     uint32_t stack_sz, void *p_arg);

/**
 * @brief Retrieves the current application tick count.
 * @return Tick count in milliseconds.
 */
uint32_t api_app_get_tick(void);

/**
 * @brief Checks if the current application task should continue its execution loop.
 * @return true if loop should continue, false otherwise.
 */
bool api_app_should_loop(void);

/**
 * @brief Delays the application task for a specific duration.
 * @param[in] ms Delay in milliseconds.
 */
void api_app_delay_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* API_INC_API_APP_MANAGER_H_ */

