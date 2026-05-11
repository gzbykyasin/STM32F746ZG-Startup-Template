/**
 * @file    api_task.h
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Generic Tasking API — Wraps RTOS tasking and timing services.
 * @date    11 May 2026
 * @version 1.0.0
 */

#ifndef API_INC_API_TASK_H_
#define API_INC_API_TASK_H_

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
 * @brief Opaque handle for tasks.
 */
typedef void *api_task_handle_t;

/**
 * @brief Task Priority Levels.
 */
typedef enum
{
    API_TASK_PRIO_IDLE = 0,      /**< Idle priority. */
    API_TASK_PRIO_LOW,           /**< Low priority. */
    API_TASK_PRIO_NORMAL,        /**< Normal priority. */
    API_TASK_PRIO_HIGH,          /**< High priority. */
    API_TASK_PRIO_REALTIME       /**< Real-time priority. */
} api_task_prio_t;

/**
 * @brief Task function signature.
 * 
 * @param[in] p_arg Pointer to task arguments.
 */
typedef void (*api_task_fn_t)(void *p_arg);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Creates a new task.
 * 
 * @param[in] fn        Task function pointer.
 * @param[in] p_name    Human-readable name for the task.
 * @param[in] priority  Task priority level.
 * @param[in] stack_sz  Stack size in bytes.
 * @param[in] p_arg     Pointer to arguments passed to the task.
 * @return Handle to the created task, or NULL on failure.
 */
api_task_handle_t api_task_create(api_task_fn_t fn, 
                                  const char *p_name,
                                  api_task_prio_t priority, 
                                  uint32_t stack_sz, void *p_arg);

/**
 * @brief Retrieves the current system tick count.
 * @return Tick count in milliseconds.
 */
uint32_t api_task_get_tick(void);

/**
 * @brief Checks if the current task should continue its execution loop.
 * @return true if loop should continue, false otherwise.
 */
bool api_task_should_loop(void);

/**
 * @brief Delays the calling task for a specific duration.
 * @param[in] ms Delay in milliseconds.
 */
void api_task_delay_ms(uint32_t ms);

/**
 * @brief Suspends a task.
 * @param[in] handle Handle to the task to suspend.
 */
void api_task_suspend(api_task_handle_t handle);

/**
 * @brief Resumes a suspended task.
 * @param[in] handle Handle to the task to resume.
 */
void api_task_resume(api_task_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif /* API_INC_API_TASK_H_ */
