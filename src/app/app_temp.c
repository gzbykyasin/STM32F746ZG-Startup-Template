/**
 * @file    app_temp.c
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Implementation of the Temperature Monitoring Application.
 * @date    7 May 2026
 * @version 1.0.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "app_temp.h"
#include "api_ambient.h"
#include "api_sync.h"

#include "sys_logger.h"
#include "api_app_manager.h"

#include <stddef.h>

/*******************************************************************************
 * Defines
 ******************************************************************************/

/** @brief Capacity of the temperature data publication queue. */
#define APP_TEMP_QUEUE_SIZE  2U

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/** @brief Queue for publishing temperature data to other application modules. */
static api_queue_t s_app_temp_queue = NULL;

/*******************************************************************************
 * Public Function Definitions
 ******************************************************************************/

/** @brief */
bool app_temp_init(void)
{
    api_queue_destroy(s_app_temp_queue);
    s_app_temp_queue = api_queue_create(APP_TEMP_QUEUE_SIZE, sizeof(app_temp_data_t));
    return (NULL != s_app_temp_queue);
}

/** @brief */
void app_temp_task(void *p_arg)
{
    (void)p_arg;
    api_ambient_data_t ambient_data;
    app_temp_data_t app_data;

    do {
        if (api_ambient_read_temp(&ambient_data))
        {
            app_data.temperature_c = ambient_data.temperature_c;
            app_data.timestamp = api_app_get_tick();

            SYS_LOG_INFO("APP_TEMP", "Temp: %ld (x1000), Vref: %ld (mV)", 
                         (int32_t)(app_data.temperature_c * 1000.0f),
                         (int32_t)(ambient_data.vref_v * 1000.0f));

            api_queue_put(s_app_temp_queue, &app_data);
        }
        else
        {
            SYS_LOG_ERROR("APP_TEMP", "Failed to read temperature from API");
        }

        api_app_delay_ms(1000U);
    } while (api_app_should_loop());
}

/** @brief */
api_queue_t app_temp_get_queue(void)
{
    return s_app_temp_queue;
}

/* End of File: app_temp.c */
