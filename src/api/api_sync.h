/**
 * @file    api_sync.h
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Synchronization Facade — Abstracts IPC for RTOS and Bare-metal.
 * @date    7 May 2026
 * @version 1.0.0
 */

#ifndef API_INC_API_SYNC_H_
#define API_INC_API_SYNC_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
 * Typedef, Enum & Structs
 ******************************************************************************/

/**
 * @brief Opaque handle for a synchronization queue.
 */
typedef void *api_queue_t;

/**
 * @brief Opaque handle for a recursive mutex.
 */
typedef void *api_mutex_t;

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/* --- Queue Management --- */

/**
 * @brief Creates a queue for inter-task communication.
 *
 * @param[in] capacity  Maximum number of items in the queue.
 * @param[in] item_sz   Size of each item in bytes.
 * @return Handle to the created queue, or NULL on error.
 */
api_queue_t api_queue_create(uint32_t capacity, uint32_t item_sz);

/**
 * @brief Destroys a queue for inter-task communication.
 *
 * @param[in] handle Handle to the queue to be destroyed.
 */
void api_queue_destroy(api_queue_t handle);

/**
 * @brief Pushes an item into the queue (Non-blocking).
 *
 * @param[in] handle Handle to the queue.
 * @param[in] p_data Pointer to the data to copy into the queue.
 * @return true if successful, false if queue is full or handle is invalid.
 */
bool api_queue_put(api_queue_t handle, const void *p_data);

/**
 * @brief Retrieves an item from the queue.
 *        In RTOS mode, this may block the calling thread.
 *        In BM mode, it returns immediately if no data is available.
 *
 * @param[in]  handle   Handle to the queue.
 * @param[out] p_data   Pointer to buffer where data will be copied.
 * @param[in]  timeout  Timeout in ms (Only used in RTOS).
 * @return true if data was retrieved, false otherwise (e.g. timeout or empty).
 */
bool api_queue_get(api_queue_t handle, void *p_data, uint32_t timeout);

/* --- Mutex Management --- */

/**
 * @brief Creates a recursive mutex.
 * 
 * @return Handle to the created mutex, or NULL on error.
 */
api_mutex_t api_mutex_create(void);

/**
 * @brief Destroys a mutex.
 * 
 * @param[in] handle Handle to the mutex.
 */
void api_mutex_destroy(api_mutex_t handle);

/**
 * @brief Acquires the mutex (Blocks in RTOS).
 * 
 * @param[in] handle   Handle to the mutex.
 * @param[in] timeout  Wait timeout in ms (or 0xFFFFFFFF for infinite).
 * @return true if acquired.
 */
bool api_mutex_lock(api_mutex_t handle, uint32_t timeout);

/**
 * @brief Releases the mutex.
 * 
 * @param[in] handle Handle to the mutex.
 */
void api_mutex_unlock(api_mutex_t handle);

#ifdef __cplusplus
}
#endif

#endif /* API_INC_API_SYNC_H_ */

