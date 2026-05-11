/**
 * @file    api_sync.c
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Implementation of OS-agnostic synchronization primitives.
 * @date    7 May 2026
 * @version 1.0.0
 */

#include "api_sync.h"
#include "sys_rtos.h"

#include <stddef.h>

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/* --- Queue Management --- */

/** @brief */
api_queue_t api_queue_create(uint32_t capacity, uint32_t item_sz)
{
    sys_rtos_queue_t handle = NULL;
    
    if (SYS_RTOS_OK == sys_rtos_queue_create(capacity, item_sz, &handle))
    {
        return (api_queue_t)handle;
    }
    
    return NULL;
}

/** @brief */
void api_queue_destroy(api_queue_t handle)
{
    if (NULL != handle)
    {
        (void)sys_rtos_queue_delete((sys_rtos_queue_t)handle);
    }
}

/** @brief */
bool api_queue_put(api_queue_t handle, const void *p_data)
{
    if ((NULL == handle) || (NULL == p_data))
    {
        return false;
    }

    return (SYS_RTOS_OK == sys_rtos_queue_put((sys_rtos_queue_t)handle, p_data, 0));
}

/** @brief */
bool api_queue_get(api_queue_t handle, void *p_data, uint32_t timeout)
{
    if ((NULL == handle) || (NULL == p_data))
    {
        return false;
    }

    return (SYS_RTOS_OK == sys_rtos_queue_get((sys_rtos_queue_t)handle, p_data, timeout));
}

/* --- Mutex Management --- */

/** @brief */
api_mutex_t api_mutex_create(void)
{
    sys_rtos_mutex_t handle = NULL;
    
    if (SYS_RTOS_OK == sys_rtos_mutex_create(&handle))
    {
        return (api_mutex_t)handle;
    }
    
    return NULL;
}

/** @brief */
void api_mutex_destroy(api_mutex_t handle)
{
    if (NULL != handle)
    {
        (void)sys_rtos_mutex_delete((sys_rtos_mutex_t)handle);
    }
}

/** @brief */
bool api_mutex_lock(api_mutex_t handle, uint32_t timeout)
{
    if (NULL == handle)
    {
        return false;
    }

    return (SYS_RTOS_OK == sys_rtos_mutex_acquire((sys_rtos_mutex_t)handle, timeout));
}

/** @brief */
void api_mutex_unlock(api_mutex_t handle)
{
    if (NULL != handle)
    {
        (void)sys_rtos_mutex_release((sys_rtos_mutex_t)handle);
    }
}

/* End of File: api_sync.c */
