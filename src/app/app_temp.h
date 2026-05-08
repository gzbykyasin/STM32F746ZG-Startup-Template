/**
 * @file    app_temp.h
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Temperature Monitoring Application.
 * @date    7 May 2026
 * @version 1.0.0
 */

#ifndef APP_INC_APP_TEMP_H_
#define APP_INC_APP_TEMP_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include "api_sync.h"

/*******************************************************************************
 * Typedef, Enum & Structs
 ******************************************************************************/

/**
 * @brief Structure for temperature data shared between app modules.
 */
typedef struct
{
    float temperature_c; /**< Temperature in Celsius */
    uint32_t timestamp;  /**< Sample timestamp */
} app_temp_data_t;

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Initializes the temperature application.
 * 
 * @return true if successful.
 */
bool app_temp_init(void);

/**
 * @brief Temperature monitoring task function.
 * 
 * @param[in] p_arg Task arguments.
 */
void app_temp_task(void *p_arg);

/**
 * @brief Gets the queue handle used for publishing temperature data.
 * 
 * @return Handle to the IPC queue.
 */
api_queue_t app_temp_get_queue(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_INC_APP_TEMP_H_ */
