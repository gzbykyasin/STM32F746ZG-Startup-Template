/**
 * @file    app_led.c
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Implementation of the LED Control Application.
 * @date    7 May 2026
 * @version 1.0.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "app_led.h"
#include "app_temp.h"
#include "api_ambient.h"
#include "api_sync.h"

#include "api_log.h"
#include "api_task.h"

#include <stddef.h>

/*******************************************************************************
 * Defines
 ******************************************************************************/

/** @brief Maximum wait time for receiving temperature data from queue. */
#define MAX_QUEUE_TIMEOUT 100U
/** @brief Temperature threshold to trigger the red warning LED. */
#define TEMP_THRESHOLD 25.0f

/*******************************************************************************
 * Public Function Definitions
 ******************************************************************************/

/** @brief */
void app_led_task(void *p_arg)
{
    (void)p_arg;
    app_temp_data_t temp_msg;
    api_queue_t temp_q = app_temp_get_queue();
    
    do {
        if (api_queue_get(temp_q, &temp_msg, MAX_QUEUE_TIMEOUT))
        {
            if (temp_msg.temperature_c > TEMP_THRESHOLD)
            {
                api_ambient_set_led(API_AMBIENT_LED_RED, true);
                api_ambient_set_led(API_AMBIENT_LED_BLUE, false);
            }
            else
            {
                api_ambient_set_led(API_AMBIENT_LED_RED, false);
                api_ambient_set_led(API_AMBIENT_LED_BLUE, true);
            }
        }
        else
        {
            static bool toggle = false;
            toggle = !toggle;
            api_ambient_set_led(API_AMBIENT_LED_BLUE, toggle);
        }
    } while (api_task_should_loop());
}

/* End of File: app_led.c */
