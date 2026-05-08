/**
 * @file    sys_rtos.c
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Universal Execution Interface — Supports RTOS and Bare-metal (Cooperative).
 * @date    7 May 2026
 * @version 1.0.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "sys_rtos.h"
#include "bsp_mcu.h"
#include <string.h>
#include <stdbool.h>

#ifdef SYS_USE_RTOS
#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"
#endif

/*******************************************************************************
 * Defines
 ******************************************************************************/

/** @brief Maximum number of tasks allowed in the system. */
#define SYS_RTOS_MAX_TASKS 16U

#ifdef SYS_USE_RTOS
/** @brief Maximum number of mutexes (RTOS mode). */
#define SYS_RTOS_MAX_MUTEXES 16U
/** @brief Maximum number of semaphores (RTOS mode). */
#define SYS_RTOS_MAX_SEMS 16U
/** @brief Maximum number of event groups (RTOS mode). */
#define SYS_RTOS_MAX_EVENTS 16U
/** @brief Maximum number of message queues (RTOS mode). */
#define SYS_RTOS_MAX_QUEUES 4U
/** @brief Maximum number of software timers (RTOS mode). */
#define SYS_RTOS_MAX_TIMERS 16U
/** @brief Default task stack size in words. */
#define SYS_RTOS_TASK_STACK_SZ 512U
/** @brief Total storage size for all queues. */
#define SYS_RTOS_QUEUE_STORAGE_SZ 16384U
#else
/** @brief Maximum number of mutexes (Bare-metal mode). */
#define SYS_BM_MAX_MUTEXES 8U
/** @brief Maximum number of queues (Bare-metal mode). */
#define SYS_BM_MAX_QUEUES 8U
/** @brief Buffer size for each bare-metal queue. */
#define SYS_BM_BUF_SIZE 4096U
#endif

/*******************************************************************************
 * Static Allocation Pools
 ******************************************************************************/

#ifdef SYS_USE_RTOS
/** @brief Static TCB storage for RTOS tasks. */
static StaticTask_t s_task_tcb_pool[SYS_RTOS_MAX_TASKS] __attribute__((aligned(8)));
/** @brief Static stack storage for RTOS tasks. */
static StackType_t s_task_stack_pool[SYS_RTOS_MAX_TASKS][SYS_RTOS_TASK_STACK_SZ] __attribute__((aligned(8)));

/** @brief Static storage for RTOS mutexes. */
static StaticSemaphore_t s_mutex_pool[SYS_RTOS_MAX_MUTEXES] __attribute__((aligned(8)));
/** @brief Static storage for RTOS semaphores. */
static StaticSemaphore_t s_sem_pool[SYS_RTOS_MAX_SEMS] __attribute__((aligned(8)));
/** @brief Static storage for RTOS event groups. */
static StaticEventGroup_t s_event_pool[SYS_RTOS_MAX_EVENTS] __attribute__((aligned(8)));
/** @brief Static TCB storage for RTOS queues. */
static StaticQueue_t s_queue_tcb_pool[SYS_RTOS_MAX_QUEUES] __attribute__((aligned(8)));
/** @brief Static storage for RTOS queue data. */
static uint8_t s_queue_storage_pool[SYS_RTOS_MAX_QUEUES][SYS_RTOS_QUEUE_STORAGE_SZ] __attribute__((aligned(8)));
/** @brief Static TCB storage for RTOS timers. */
static StaticTimer_t s_timer_tcb_pool[SYS_RTOS_MAX_TIMERS] __attribute__((aligned(8)));

/** @brief Current index for mutex allocation. */
static uint32_t s_mutex_idx = 0U;
/** @brief Current index for semaphore allocation. */
static uint32_t s_sem_idx = 0U;
/** @brief Current index for event group allocation. */
static uint32_t s_event_idx = 0U;
/** @brief Current index for queue allocation. */
static uint32_t s_queue_idx = 0U;
/** @brief Current index for timer allocation. */
static uint32_t s_timer_idx = 0U;
#else
/** @brief Bare-metal task control structure. */
typedef struct
{
    sys_rtos_task_fn_t fn;      /**< Task entry function. */
    void *p_arg;                /**< Task entry argument. */
    uint32_t next_run_ms;       /**< Next scheduled run time. */
    bool is_suspended;          /**< Suspended state flag. */
    bool is_active;             /**< Active state flag. */
} sys_bm_task_t;

/** @brief Bare-metal queue control structure. */
typedef struct
{
    uint8_t buffer[SYS_BM_BUF_SIZE]; /**< Data storage. */
    uint32_t item_sz;               /**< Size of each item. */
    uint32_t capacity;              /**< Max item count. */
    uint32_t head;                  /**< Head pointer. */
    uint32_t tail;                  /**< Tail pointer. */
    uint32_t count;                 /**< Current item count. */
    bool in_use;                    /**< Allocation flag. */
} sys_bm_queue_t;

/** @brief Bare-metal mutex control structure. */
typedef struct
{
    bool locked; /**< Lock state. */
    bool in_use; /**< Allocation flag. */
} sys_bm_mutex_t;

/** @brief Static pool for bare-metal tasks. */
static sys_bm_task_t s_bm_task_pool[SYS_RTOS_MAX_TASKS] __attribute__((aligned(32)));
/** @brief Static pool for bare-metal queues. */
static sys_bm_queue_t s_bm_queue_pool[SYS_BM_MAX_QUEUES] __attribute__((aligned(32)));
/** @brief Static pool for bare-metal mutexes. */
static sys_bm_mutex_t s_bm_mutex_pool[SYS_BM_MAX_MUTEXES] __attribute__((aligned(32)));

/** @brief Current running task index for bare-metal scheduler. */
static uint32_t s_current_bm_task_idx = 0U;
#endif

/** @brief Global task allocation index. */
static uint32_t s_task_idx = 0U;

/*******************************************************************************
 * RTOS Specific Hooks
 ******************************************************************************/
#ifdef SYS_USE_RTOS
/** @brief Provides memory for the FreeRTOS Idle Task. */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize)
{
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[512];
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = 512;
}

/** @brief Provides memory for the FreeRTOS Timer Task. */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize)
{
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[512];
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *pulTimerTaskStackSize = 512;
}

/** @brief Hook called before entering sleep mode (Tickless). */
void PreSleepProcessing(uint32_t ulExpectedIdleTime)
{
    (void)ulExpectedIdleTime;
    bsp_mcu_suspend_tick();
}

/** @brief Hook called after exiting sleep mode (Tickless). */
void PostSleepProcessing(uint32_t ulExpectedIdleTime)
{
    (void)ulExpectedIdleTime;
    bsp_mcu_resume_tick();
}

/** @brief Helper to convert CMSIS-RTOS status to system status. */
static sys_rtos_status_t os_to_status(osStatus_t s)
{
    switch (s)
    {
    case osOK:
        return SYS_RTOS_OK;
    case osErrorTimeout:
        return SYS_RTOS_TIMEOUT;
    case osErrorResource:
        return SYS_RTOS_RESOURCE;
    default:
        return SYS_RTOS_ERROR;
    }
}
#endif

/*******************************************************************************
 * Public Functions — Kernel
 ******************************************************************************/

/** @brief Initializes the system kernel. */
void sys_rtos_kernel_init(void)
{
    s_task_idx = 0U;

#ifdef SYS_USE_RTOS
    s_mutex_idx = 0U;
    s_sem_idx = 0U;
    s_event_idx = 0U;
    s_queue_idx = 0U;
    s_timer_idx = 0U;
    osKernelInitialize();
#else
    memset(s_bm_task_pool, 0, sizeof(s_bm_task_pool));
    memset(s_bm_queue_pool, 0, sizeof(s_bm_queue_pool));
    memset(s_bm_mutex_pool, 0, sizeof(s_bm_mutex_pool));
    s_current_bm_task_idx = 0U;
#endif
}

/** @brief Starts the system kernel scheduler. */
void sys_rtos_kernel_start(void)
{
#ifdef SYS_USE_RTOS
    osKernelStart();
#else
    while (1)
    {
        uint32_t now = bsp_mcu_get_tick();

        for (uint32_t i = 0U; i < s_task_idx; i++)
        {
            if (s_bm_task_pool[i].is_active && !s_bm_task_pool[i].is_suspended)
            {
                if (now >= s_bm_task_pool[i].next_run_ms)
                {
                    if (NULL != s_bm_task_pool[i].fn)
                    {
                        s_current_bm_task_idx = i;
                        s_bm_task_pool[i].fn(s_bm_task_pool[i].p_arg);
                    }
                }
            }
        }
    }
#endif
}

/** @brief Wakes up all suspended tasks (BM only). */
void sys_rtos_kernel_wake_tasks(void)
{
#ifndef SYS_USE_RTOS
    for (uint32_t i = 0U; i < s_task_idx; i++)
    {
        s_bm_task_pool[i].next_run_ms = 0U;
    }
#endif
}

/** @brief Locks the kernel scheduler. */
void sys_rtos_kernel_lock(void)
{
#ifdef SYS_USE_RTOS
    osKernelLock();
#endif
}

/** @brief Unlocks the kernel scheduler. */
void sys_rtos_kernel_unlock(void)
{
#ifdef SYS_USE_RTOS
    osKernelUnlock();
#endif
}

/** @brief Enters a critical section by disabling interrupts. */
void sys_rtos_enter_critical(void)
{
    bsp_mcu_disable_global_interrupts();
}

/** @brief Exits a critical section by enabling interrupts. */
void sys_rtos_exit_critical(void)
{
    bsp_mcu_enable_global_interrupts();
}

/*******************************************************************************
 * Public Functions — Task
 ******************************************************************************/

/** @brief Creates a new task. */
sys_rtos_status_t sys_rtos_task_create(sys_rtos_task_fn_t fn,
                                       const char *p_name,
                                       sys_rtos_priority_t priority,
                                       uint32_t stack_sz,
                                       void *p_arg,
                                       sys_rtos_task_t *p_handle)
{
    if (s_task_idx >= SYS_RTOS_MAX_TASKS)
    {
        return SYS_RTOS_ERROR;
    }

#ifdef SYS_USE_RTOS
    if (stack_sz > SYS_RTOS_TASK_STACK_SZ)
    {
        return SYS_RTOS_ERROR;
    }

    osThreadAttr_t attr = {
        .name = p_name,
        .priority = (osPriority_t)priority,
        .cb_mem = &s_task_tcb_pool[s_task_idx],
        .cb_size = sizeof(StaticTask_t),
        .stack_mem = s_task_stack_pool[s_task_idx],
        .stack_size = stack_sz * sizeof(StackType_t)};

    osThreadId_t id = osThreadNew(fn, p_arg, &attr);
    if (NULL == id)
    {
        return SYS_RTOS_ERROR;
    }
    if (NULL != p_handle)
    {
        *p_handle = id;
    }
#else
    (void)p_name;
    (void)priority;
    (void)stack_sz;
    s_bm_task_pool[s_task_idx].fn = fn;
    s_bm_task_pool[s_task_idx].p_arg = p_arg;
    s_bm_task_pool[s_task_idx].is_active = true;
    s_bm_task_pool[s_task_idx].is_suspended = false;
    s_bm_task_pool[s_task_idx].next_run_ms = 0U;
    if (NULL != p_handle)
    {
        *p_handle = (sys_rtos_task_t)(uintptr_t)(s_task_idx + 1U);
    }
#endif

    s_task_idx++;
    return SYS_RTOS_OK;
}

/** @brief Terminates an existing task. */
sys_rtos_status_t sys_rtos_task_terminate(sys_rtos_task_t handle)
{
#ifdef SYS_USE_RTOS
    return os_to_status(osThreadTerminate(handle));
#else
    (void)handle;
    return SYS_RTOS_OK;
#endif
}

/** @brief Suspends a task. */
sys_rtos_status_t sys_rtos_task_suspend(sys_rtos_task_t handle)
{
#ifdef SYS_USE_RTOS
    return os_to_status(osThreadSuspend(handle));
#else
    uint32_t idx = (uint32_t)(uintptr_t)handle - 1U;
    if (idx < s_task_idx)
    {
        s_bm_task_pool[idx].is_suspended = true;
        return SYS_RTOS_OK;
    }
    return SYS_RTOS_ERROR;
#endif
}

/** @brief Resumes a suspended task. */
sys_rtos_status_t sys_rtos_task_resume(sys_rtos_task_t handle)
{
#ifdef SYS_USE_RTOS
    return os_to_status(osThreadResume(handle));
#else
    uint32_t idx = (uint32_t)(uintptr_t)handle - 1U;
    if (idx < s_task_idx)
    {
        s_bm_task_pool[idx].is_suspended = false;
        return SYS_RTOS_OK;
    }
    return SYS_RTOS_ERROR;
#endif
}

/** @brief Checks if task execution requires a loop pattern. */
bool sys_rtos_task_should_loop(void)
{
#ifdef SYS_USE_RTOS
    return true;
#else
    return false;
#endif
}

/** @brief Gets the current system tick count. */
uint32_t sys_rtos_get_tick(void)
{
#ifdef SYS_USE_RTOS
    return osKernelGetTickCount();
#else
    return bsp_mcu_get_tick();
#endif
}

/** @brief Delays execution for a specified number of milliseconds. */
sys_rtos_status_t sys_rtos_delay_ms(uint32_t ms)
{
#ifdef SYS_USE_RTOS
    return os_to_status(osDelay(ms));
#else
    s_bm_task_pool[s_current_bm_task_idx].next_run_ms = bsp_mcu_get_tick() + ms;
    return SYS_RTOS_OK;
#endif
}

/*******************************************************************************
 * Public Functions — Mutex, Sem, Queue, Timer
 ******************************************************************************/

/** @brief Creates a mutex. */
sys_rtos_status_t sys_rtos_mutex_create(sys_rtos_mutex_t *p_handle)
{
#ifdef SYS_USE_RTOS
    if (s_mutex_idx >= SYS_RTOS_MAX_MUTEXES)
    {
        return SYS_RTOS_ERROR;
    }
    osMutexAttr_t attr = {
        .attr_bits = osMutexRecursive,
        .cb_mem = &s_mutex_pool[s_mutex_idx],
        .cb_size = sizeof(StaticSemaphore_t)};
    osMutexId_t id = osMutexNew(&attr);
    if (NULL == id)
    {
        return SYS_RTOS_ERROR;
    }
    s_mutex_idx++;
    *p_handle = id;
    return SYS_RTOS_OK;
#else
    for (uint32_t i = 0; i < SYS_BM_MAX_MUTEXES; i++)
    {
        if (!s_bm_mutex_pool[i].in_use)
        {
            s_bm_mutex_pool[i].locked = false;
            s_bm_mutex_pool[i].in_use = true;
            *p_handle = (sys_rtos_mutex_t)&s_bm_mutex_pool[i];
            return SYS_RTOS_OK;
        }
    }
    return SYS_RTOS_RESOURCE;
#endif
}

/** @brief Acquires a mutex. */
sys_rtos_status_t sys_rtos_mutex_acquire(sys_rtos_mutex_t handle, uint32_t timeout)
{
#ifdef SYS_USE_RTOS
    return os_to_status(osMutexAcquire(handle, timeout));
#else
    (void)timeout;
    sys_bm_mutex_t *m = (sys_bm_mutex_t *)handle;
    if (NULL == m || m->locked)
    {
        return SYS_RTOS_ERROR;
    }
    m->locked = true;
    return SYS_RTOS_OK;
#endif
}

/** @brief Releases a mutex. */
sys_rtos_status_t sys_rtos_mutex_release(sys_rtos_mutex_t handle)
{
#ifdef SYS_USE_RTOS
    return os_to_status(osMutexRelease(handle));
#else
    sys_bm_mutex_t *m = (sys_bm_mutex_t *)handle;
    if (NULL != m)
    {
        m->locked = false;
    }
    return SYS_RTOS_OK;
#endif
}

/** @brief Deletes a mutex. */
sys_rtos_status_t sys_rtos_mutex_delete(sys_rtos_mutex_t handle)
{
#ifdef SYS_USE_RTOS
    return os_to_status(osMutexDelete(handle));
#else
    sys_bm_mutex_t *m = (sys_bm_mutex_t *)handle;
    if (NULL != m)
    {
        m->in_use = false;
    }
    return SYS_RTOS_OK;
#endif
}

/** @brief Creates a semaphore. */
sys_rtos_status_t sys_rtos_sem_create(uint32_t max_count, uint32_t init_count, sys_rtos_sem_t *p_handle)
{
#ifdef SYS_USE_RTOS
    if (s_sem_idx >= SYS_RTOS_MAX_SEMS)
    {
        return SYS_RTOS_ERROR;
    }
    osSemaphoreAttr_t attr = {
        .cb_mem = &s_sem_pool[s_sem_idx],
        .cb_size = sizeof(StaticSemaphore_t)};
    osSemaphoreId_t id = osSemaphoreNew(max_count, init_count, &attr);
    if (NULL == id)
    {
        return SYS_RTOS_ERROR;
    }
    s_sem_idx++;
    *p_handle = id;
    return SYS_RTOS_OK;
#else
    (void)max_count;
    (void)init_count;
    (void)p_handle;
    return SYS_RTOS_OK;
#endif
}

/** @brief Acquires a semaphore. */
sys_rtos_status_t sys_rtos_sem_acquire(sys_rtos_sem_t handle, uint32_t timeout)
{
#ifdef SYS_USE_RTOS
    return os_to_status(osSemaphoreAcquire(handle, timeout));
#else
    (void)handle;
    (void)timeout;
    return SYS_RTOS_OK;
#endif
}

/** @brief Releases a semaphore. */
sys_rtos_status_t sys_rtos_sem_release(sys_rtos_sem_t handle)
{
#ifdef SYS_USE_RTOS
    return os_to_status(osSemaphoreRelease(handle));
#else
    (void)handle;
    return SYS_RTOS_OK;
#endif
}

/** @brief Deletes a semaphore. */
sys_rtos_status_t sys_rtos_sem_delete(sys_rtos_sem_t handle)
{
#ifdef SYS_USE_RTOS
    return os_to_status(osSemaphoreDelete(handle));
#else
    (void)handle;
    return SYS_RTOS_OK;
#endif
}

/** @brief Creates an event group. */
sys_rtos_status_t sys_rtos_event_create(sys_rtos_event_t *p_handle)
{
#ifdef SYS_USE_RTOS
    if (s_event_idx >= SYS_RTOS_MAX_EVENTS)
    {
        return SYS_RTOS_ERROR;
    }
    osEventFlagsAttr_t attr = {
        .cb_mem = &s_event_pool[s_event_idx],
        .cb_size = sizeof(StaticEventGroup_t)};
    osEventFlagsId_t id = osEventFlagsNew(&attr);
    if (NULL == id)
    {
        return SYS_RTOS_ERROR;
    }
    s_event_idx++;
    *p_handle = id;
    return SYS_RTOS_OK;
#else
    (void)p_handle;
    return SYS_RTOS_OK;
#endif
}

/** @brief Sets event flags. */
sys_rtos_status_t sys_rtos_event_set(sys_rtos_event_t handle, uint32_t flags)
{
#ifdef SYS_USE_RTOS
    uint32_t result = osEventFlagsSet(handle, flags);
    return ((result & osFlagsError) != 0U) ? SYS_RTOS_ERROR : SYS_RTOS_OK;
#else
    (void)handle;
    (void)flags;
    return SYS_RTOS_OK;
#endif
}

/** @brief Waits for event flags. */
sys_rtos_status_t sys_rtos_event_wait(sys_rtos_event_t handle, uint32_t flags, uint32_t timeout, uint32_t *p_result)
{
#ifdef SYS_USE_RTOS
    uint32_t result = osEventFlagsWait(handle, flags, osFlagsWaitAny, timeout);
    if ((result & osFlagsError) != 0U)
    {
        return (result == osFlagsErrorTimeout) ? SYS_RTOS_TIMEOUT : SYS_RTOS_ERROR;
    }
    if (NULL != p_result)
    {
        *p_result = result;
    }
    return SYS_RTOS_OK;
#else
    (void)handle;
    (void)flags;
    (void)timeout;
    if (p_result)
    {
        *p_result = flags;
    }
    return SYS_RTOS_OK;
#endif
}

/** @brief Deletes an event group. */
sys_rtos_status_t sys_rtos_event_delete(sys_rtos_event_t handle)
{
#ifdef SYS_USE_RTOS
    return os_to_status(osEventFlagsDelete(handle));
#else
    (void)handle;
    return SYS_RTOS_OK;
#endif
}

/** @brief Creates a message queue. */
sys_rtos_status_t sys_rtos_queue_create(uint32_t capacity, uint32_t msg_sz, sys_rtos_queue_t *p_handle)
{
#ifdef SYS_USE_RTOS
    if (s_queue_idx >= SYS_RTOS_MAX_QUEUES)
    {
        return SYS_RTOS_ERROR;
    }
    osMessageQueueAttr_t attr = {0};
    attr.cb_mem = &s_queue_tcb_pool[s_queue_idx];
    attr.cb_size = sizeof(StaticQueue_t);
    attr.mq_mem = s_queue_storage_pool[s_queue_idx];
    attr.mq_size = (capacity * msg_sz);
    osMessageQueueId_t id = osMessageQueueNew(capacity, msg_sz, &attr);
    if (NULL == id)
    {
        return SYS_RTOS_ERROR;
    }
    s_queue_idx++;
    *p_handle = id;
    return SYS_RTOS_OK;
#else
    for (uint32_t i = 0; i < SYS_BM_MAX_QUEUES; i++)
    {
        if (!s_bm_queue_pool[i].in_use)
        {
            if ((capacity * msg_sz) > SYS_BM_BUF_SIZE)
            {
                return SYS_RTOS_RESOURCE;
            }

            s_bm_queue_pool[i].capacity = capacity;
            s_bm_queue_pool[i].item_sz = msg_sz;
            s_bm_queue_pool[i].head = 0;
            s_bm_queue_pool[i].tail = 0;
            s_bm_queue_pool[i].count = 0;
            s_bm_queue_pool[i].in_use = true;
            *p_handle = (sys_rtos_queue_t)&s_bm_queue_pool[i];
            return SYS_RTOS_OK;
        }
    }
    return SYS_RTOS_RESOURCE;
#endif
}

/** @brief Puts a message into the queue. */
sys_rtos_status_t sys_rtos_queue_put(sys_rtos_queue_t handle, const void *p_msg, uint32_t timeout)
{
#ifdef SYS_USE_RTOS
    return os_to_status(osMessageQueuePut(handle, p_msg, 0U, timeout));
#else
    (void)timeout;
    sys_bm_queue_t *q = (sys_bm_queue_t *)handle;
    if (NULL == q || q->count >= q->capacity)
    {
        return SYS_RTOS_ERROR;
    }

    memcpy(&q->buffer[q->head * q->item_sz], p_msg, q->item_sz);
    
    bsp_mcu_disable_global_interrupts();
    q->head = (q->head + 1) % q->capacity;
    q->count++;
    bsp_mcu_enable_global_interrupts();
    
    return SYS_RTOS_OK;
#endif
}

/** @brief Gets a message from the queue. */
sys_rtos_status_t sys_rtos_queue_get(sys_rtos_queue_t handle, void *p_msg, uint32_t timeout)
{
#ifdef SYS_USE_RTOS
    return os_to_status(osMessageQueueGet(handle, p_msg, NULL, timeout));
#else
    sys_bm_queue_t *q = (sys_bm_queue_t *)handle;
    if (NULL == q || q->count == 0)
    {
        if (timeout > 0U)
        {
            (void)sys_rtos_delay_ms(timeout);
        }
        return SYS_RTOS_ERROR;
    }

    memcpy(p_msg, &q->buffer[q->tail * q->item_sz], q->item_sz);
    
    bsp_mcu_disable_global_interrupts();
    q->tail = (q->tail + 1) % q->capacity;
    q->count--;
    bsp_mcu_enable_global_interrupts();
    
    return SYS_RTOS_OK;
#endif
}

/** @brief Deletes a queue. */
sys_rtos_status_t sys_rtos_queue_delete(sys_rtos_queue_t handle)
{
#ifdef SYS_USE_RTOS
    return os_to_status(osMessageQueueDelete(handle));
#else
    sys_bm_queue_t *q = (sys_bm_queue_t *)handle;
    if (NULL != q)
    {
        q->in_use = false;
    }
    return SYS_RTOS_OK;
#endif
}

/** @brief Creates a timer. */
sys_rtos_status_t sys_rtos_timer_create(sys_rtos_timer_fn_t fn, sys_rtos_timer_type_t type, void *p_arg, sys_rtos_timer_t *p_handle)
{
#ifdef SYS_USE_RTOS
    if (s_timer_idx >= SYS_RTOS_MAX_TIMERS)
    {
        return SYS_RTOS_ERROR;
    }
    osTimerAttr_t attr = {
        .cb_mem = &s_timer_tcb_pool[s_timer_idx],
        .cb_size = sizeof(StaticTimer_t)};
    osTimerId_t id = osTimerNew(fn, (osTimerType_t)type, p_arg, &attr);
    if (NULL == id)
    {
        return SYS_RTOS_ERROR;
    }
    s_timer_idx++;
    *p_handle = id;
    return SYS_RTOS_OK;
#else
    (void)fn;
    (void)type;
    (void)p_arg;
    (void)p_handle;
    return SYS_RTOS_OK;
#endif
}

/** @brief Starts a timer. */
sys_rtos_status_t sys_rtos_timer_start(sys_rtos_timer_t handle, uint32_t ms)
{
#ifdef SYS_USE_RTOS
    return os_to_status(osTimerStart(handle, ms));
#else
    (void)handle;
    (void)ms;
    return SYS_RTOS_OK;
#endif
}

/** @brief Stops a timer. */
sys_rtos_status_t sys_rtos_timer_stop(sys_rtos_timer_t handle)
{
#ifdef SYS_USE_RTOS
    return os_to_status(osTimerStop(handle));
#else
    (void)handle;
    return SYS_RTOS_OK;
#endif
}

/** @brief Deletes a timer. */
sys_rtos_status_t sys_rtos_timer_delete(sys_rtos_timer_t handle)
{
#ifdef SYS_USE_RTOS
    return os_to_status(osTimerDelete(handle));
#else
    (void)handle;
    return SYS_RTOS_OK;
#endif
}

/* End of File: sys_rtos.c */
