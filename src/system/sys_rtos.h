/**
 * @file    sys_rtos.h
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   CMSIS-RTOS V2 typed facade — OS-agnostic interface for the system.
 * @date    5 May 2026
 * @version 1.0.0
 */

#ifndef SYSTEM_INC_SYS_RTOS_H_
#define SYSTEM_INC_SYS_RTOS_H_

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

/** @brief Wait forever constant for blocking operations. */
#define SYS_RTOS_WAIT_FOREVER 0xFFFFFFFFU

/**
 * @brief System RTOS Status Codes.
 */
typedef enum
{
    SYS_RTOS_OK = 0,    /**< Operation completed successfully. */
    SYS_RTOS_ERROR,     /**< Unspecified RTOS error. */
    SYS_RTOS_TIMEOUT,   /**< Operation timed out. */
    SYS_RTOS_RESOURCE   /**< Resource not available (e.g. out of memory). */
} sys_rtos_status_t;

/**
 * @brief System RTOS Task Priorities.
 */
typedef enum
{
    SYS_RTOS_PRIO_IDLE = 1,         /**< Idle task priority. */
    SYS_RTOS_PRIO_LOW = 8,          /**< Low priority. */
    SYS_RTOS_PRIO_NORMAL = 24,      /**< Normal priority. */
    SYS_RTOS_PRIO_HIGH = 40,        /**< High priority. */
    SYS_RTOS_PRIO_REALTIME = 48     /**< Real-time/Critical priority. */
} sys_rtos_priority_t;

/**
 * @brief System RTOS Timer Types.
 */
typedef enum
{
    SYS_RTOS_TIMER_ONCE = 0,        /**< One-shot timer. */
    SYS_RTOS_TIMER_PERIODIC = 1     /**< Repeating periodic timer. */
} sys_rtos_timer_type_t;

/**
 * @brief Opaque handles for RTOS objects.
 */
typedef void *sys_rtos_task_t;      /**< Task handle. */
typedef void *sys_rtos_mutex_t;     /**< Mutex handle. */
typedef void *sys_rtos_sem_t;       /**< Semaphore handle. */
typedef void *sys_rtos_event_t;     /**< Event flags handle. */
typedef void *sys_rtos_queue_t;     /**< Message queue handle. */
typedef void *sys_rtos_timer_t;     /**< Software timer handle. */

/**
 * @brief Task function signature.
 * 
 * @param[in] p_arg Pointer to task arguments.
 */
typedef void (*sys_rtos_task_fn_t)(void *p_arg);

/**
 * @brief Timer callback signature.
 * 
 * @param[in] p_arg Pointer to timer arguments.
 */
typedef void (*sys_rtos_timer_fn_t)(void *p_arg);

/*******************************************************************************
 * Public Functions — Kernel
 ******************************************************************************/

/**
 * @brief Initializes the RTOS kernel.
 */
void sys_rtos_kernel_init(void);

/**
 * @brief Starts the RTOS kernel scheduler.
 */
void sys_rtos_kernel_start(void);

/**
 * @brief Wakes Up the tasks.
 */
void sys_rtos_kernel_wake_tasks(void);

/**
 * @brief Locks the RTOS kernel scheduler (prevents task switching).
 */
void sys_rtos_kernel_lock(void);

/**
 * @brief Unlocks the RTOS kernel scheduler (resumes task switching).
 */
void sys_rtos_kernel_unlock(void);

/**
 * @brief Enters a critical section (masks interrupts).
 */
void sys_rtos_enter_critical(void);

/**
 * @brief Exits a critical section (restores interrupts).
 */
void sys_rtos_exit_critical(void);

/*******************************************************************************
 * Public Functions — Delay
 ******************************************************************************/

/**
 * @brief Relative delay for the calling task.
 * 
 * @param[in] ms Delay duration in milliseconds.
 * @return SYS_RTOS_OK on success.
 */
sys_rtos_status_t sys_rtos_delay_ms(uint32_t ms);

/*******************************************************************************
 * Public Functions — Task
 ******************************************************************************/

/**
 * @brief Creates a new RTOS thread/task.
 * 
 * @param[in]  fn        Task function pointer.
 * @param[in]  p_name    Human-readable name for the task.
 * @param[in]  priority  Task priority level.
 * @param[in]  stack_sz  Stack size in bytes.
 * @param[in]  p_arg     Pointer to arguments passed to the task.
 * @param[out] p_handle  Pointer to store the created task handle.
 * @return SYS_RTOS_OK on success, error code otherwise.
 */
sys_rtos_status_t sys_rtos_task_create(sys_rtos_task_fn_t fn,
                                       const char *p_name,
                                       sys_rtos_priority_t priority, 
                                       uint32_t stack_sz, 
                                       void *p_arg,
                                       sys_rtos_task_t *p_handle);

/**
 * @brief Terminates and deletes a specific RTOS thread.
 * 
 * @param[in] handle Handle of the task to terminate.
 * @return SYS_RTOS_OK on success.
 */
sys_rtos_status_t sys_rtos_task_terminate(sys_rtos_task_t handle);

/**
 * @brief Suspends a specific RTOS thread.
 * 
 * @param[in] handle Handle of the task to suspend.
 * @return SYS_RTOS_OK on success.
 */
sys_rtos_status_t sys_rtos_task_suspend(sys_rtos_task_t handle);

/**
 * @brief Resumes a suspended task.
 * 
 * @param[in] handle Task handle to resume.
 * @return SYS_RTOS_OK on success.
 */
sys_rtos_status_t sys_rtos_task_resume(sys_rtos_task_t handle);

/**
 * @brief Checks if the current task should continue its main loop.
 *        Returns true in RTOS, false in Bare-metal to allow cooperative scheduling.
 * 
 * @return true if loop should continue, false otherwise.
 */
bool sys_rtos_task_should_loop(void);

/*******************************************************************************
 * Public Functions — Delay, Mutex, Sem, Queue, Timer
 ******************************************************************************/

/**
 * @brief Creates a recursive mutex.
 * 
 * @param[out] p_handle Pointer to store the created mutex handle.
 * @return SYS_RTOS_OK on success.
 */
sys_rtos_status_t sys_rtos_mutex_create(sys_rtos_mutex_t *p_handle);

/**
 * @brief Acquires a mutex with a specified timeout.
 * 
 * @param[in] handle   Handle of the mutex to acquire.
 * @param[in] timeout  Wait timeout in ms (or SYS_RTOS_WAIT_FOREVER).
 * @return SYS_RTOS_OK on success, SYS_RTOS_TIMEOUT otherwise.
 */
sys_rtos_status_t sys_rtos_mutex_acquire(sys_rtos_mutex_t handle, uint32_t timeout);

/**
 * @brief Releases a previously acquired mutex.
 * 
 * @param[in] handle Handle of the mutex to release.
 * @return SYS_RTOS_OK on success.
 */
sys_rtos_status_t sys_rtos_mutex_release(sys_rtos_mutex_t handle);

/**
 * @brief Deletes a mutex object.
 * 
 * @param[in] handle Handle of the mutex to delete.
 * @return SYS_RTOS_OK on success.
 */
sys_rtos_status_t sys_rtos_mutex_delete(sys_rtos_mutex_t handle);

/*******************************************************************************
 * Public Functions — Semaphore
 ******************************************************************************/

/**
 * @brief Creates a counting semaphore.
 * 
 * @param[in]  max_count   Maximum possible semaphore count.
 * @param[in]  init_count  Initial semaphore count.
 * @param[out] p_handle    Pointer to store the created semaphore handle.
 * @return SYS_RTOS_OK on success.
 */
sys_rtos_status_t sys_rtos_sem_create(uint32_t max_count, uint32_t init_count,
                                      sys_rtos_sem_t *p_handle);

/**
 * @brief Acquires a semaphore token with timeout.
 * 
 * @param[in] handle   Handle of the semaphore.
 * @param[in] timeout  Wait timeout in ms (or SYS_RTOS_WAIT_FOREVER).
 * @return SYS_RTOS_OK on success, SYS_RTOS_TIMEOUT otherwise.
 */
sys_rtos_status_t sys_rtos_sem_acquire(sys_rtos_sem_t handle, uint32_t timeout);

/**
 * @brief Releases a semaphore token (increments count).
 * 
 * @param[in] handle Handle of the semaphore.
 * @return SYS_RTOS_OK on success.
 */
sys_rtos_status_t sys_rtos_sem_release(sys_rtos_sem_t handle);

/**
 * @brief Deletes a semaphore object.
 * 
 * @param[in] handle Handle of the semaphore to delete.
 * @return SYS_RTOS_OK on success.
 */
sys_rtos_status_t sys_rtos_sem_delete(sys_rtos_sem_t handle);

/*******************************************************************************
 * Public Functions — Event Flags
 ******************************************************************************/

/**
 * @brief Creates an event flags group.
 * 
 * @param[out] p_handle Pointer to store the created event handle.
 * @return SYS_RTOS_OK on success.
 */
sys_rtos_status_t sys_rtos_event_create(sys_rtos_event_t *p_handle);

/**
 * @brief Sets specific bit flags in an event group.
 * 
 * @param[in] handle Handle of the event group.
 * @param[in] flags  Bit mask of flags to set.
 * @return SYS_RTOS_OK on success.
 */
sys_rtos_status_t sys_rtos_event_set(sys_rtos_event_t handle, uint32_t flags);

/**
 * @brief Waits for specific flags to be set with timeout.
 * 
 * @param[in]  handle   Handle of the event group.
 * @param[in]  flags    Bit mask of flags to wait for.
 * @param[in]  timeout  Wait timeout in ms (or SYS_RTOS_WAIT_FOREVER).
 * @param[out] p_result Pointer to store the flags that triggered the return.
 * @return SYS_RTOS_OK on success, SYS_RTOS_TIMEOUT otherwise.
 */
sys_rtos_status_t sys_rtos_event_wait(sys_rtos_event_t handle, 
                                      uint32_t flags, 
                                      uint32_t timeout,
                                      uint32_t *p_result);

/**
 * @brief Deletes an event flags group.
 * 
 * @param[in] handle Handle of the event group to delete.
 * @return SYS_RTOS_OK on success.
 */
sys_rtos_status_t sys_rtos_event_delete(sys_rtos_event_t handle);

/*******************************************************************************
 * Public Functions — Queue
 ******************************************************************************/

/**
 * @brief Creates a message queue.
 * 
 * @param[in]  capacity  Maximum number of messages in the queue.
 * @param[in]  msg_sz    Size of each message in bytes.
 * @param[out] p_handle  Pointer to store the created queue handle.
 * @return SYS_RTOS_OK on success.
 */
sys_rtos_status_t sys_rtos_queue_create(uint32_t capacity, 
                                        uint32_t msg_sz,
                                        sys_rtos_queue_t *p_handle);

/**
 * @brief Puts a message in the queue with timeout.
 * 
 * @param[in] handle   Handle of the queue.
 * @param[in] p_msg    Pointer to the message data to copy into the queue.
 * @param[in] timeout  Wait timeout in ms (or SYS_RTOS_WAIT_FOREVER).
 * @return SYS_RTOS_OK on success, error/timeout otherwise.
 */
sys_rtos_status_t sys_rtos_queue_put(sys_rtos_queue_t handle, 
                                     const void *p_msg, 
                                     uint32_t timeout);

/**
 * @brief Retrieves a message from the queue with timeout.
 * 
 * @param[in]  handle   Handle of the queue.
 * @param[out] p_msg    Pointer to buffer where the message will be copied.
 * @param[in]  timeout  Wait timeout in ms (or SYS_RTOS_WAIT_FOREVER).
 * @return SYS_RTOS_OK on success, error/timeout otherwise.
 */
sys_rtos_status_t sys_rtos_queue_get(sys_rtos_queue_t handle, 
                                     void *p_msg, 
                                     uint32_t timeout);

/**
 * @brief Deletes a message queue.
 * 
 * @param[in] handle Handle of the queue to delete.
 * @return SYS_RTOS_OK on success.
 */
sys_rtos_status_t sys_rtos_queue_delete(sys_rtos_queue_t handle);

/*******************************************************************************
 * Public Functions — Timer
 ******************************************************************************/

/**
 * @brief Creates a software timer.
 * 
 * @param[in]  fn        Callback function pointer.
 * @param[in]  type      Timer type (One-shot or Periodic).
 * @param[in]  p_arg     Pointer to argument passed to the callback.
 * @param[out] p_handle  Pointer to store the created timer handle.
 * @return SYS_RTOS_OK on success.
 */
sys_rtos_status_t sys_rtos_timer_create(sys_rtos_timer_fn_t fn, 
                                        sys_rtos_timer_type_t type,
                                        void *p_arg, 
                                        sys_rtos_timer_t *p_handle);

/**
 * @brief Starts or restarts a software timer.
 * 
 * @param[in] handle  Handle of the timer.
 * @param[in] ms      Timer duration/period in milliseconds.
 * @return SYS_RTOS_OK on success.
 */
sys_rtos_status_t sys_rtos_timer_start(sys_rtos_timer_t handle, uint32_t ms);

/**
 * @brief Stops a running software timer.
 * 
 * @param[in] handle Handle of the timer to stop.
 * @return SYS_RTOS_OK on success.
 */
sys_rtos_status_t sys_rtos_timer_stop(sys_rtos_timer_t handle);

/**
 * @brief Retrieves the current system tick count in milliseconds.
 * 
 * @return Current system tick value in ms.
 */
uint32_t sys_rtos_get_tick(void);

/**
 * @brief Deletes a software timer object.
 * 
 * @param[in] handle Handle of the timer to delete.
 * @return SYS_RTOS_OK on success.
 */
sys_rtos_status_t sys_rtos_timer_delete(sys_rtos_timer_t handle);


#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_INC_SYS_RTOS_H_ */
