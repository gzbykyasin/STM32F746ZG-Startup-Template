/**
 * @file    api_ambient.c
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Implementation of the synchronized Ambient API.
 * @date    7 May 2026
 * @version 1.0.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "api_ambient.h"
#include "api_sync.h"

#include "driver_ambient.h"

#include <stddef.h>

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/** @brief Mutex for synchronizing access to ambient hardware resources. */
static api_mutex_t s_ambient_mutex = NULL;

/*******************************************************************************
 * Public Function Definitions
 ******************************************************************************/

/** @brief */
bool api_ambient_init(void)
{
    if (NULL == s_ambient_mutex)
    {
        s_ambient_mutex = api_mutex_create();
    }
    return (NULL != s_ambient_mutex);
}

/** @brief */
bool api_ambient_read_temp(api_ambient_data_t *p_data)
{
    bool success = false;

    if (NULL == p_data)
    {
        return false;
    }

    if (api_mutex_lock(s_ambient_mutex, 100U))
    {
        driver_ambient_data_t drv_data;
        if (driver_ambient_read(&drv_data))
        {
            p_data->temperature_c = drv_data.temperature_c;
            p_data->vref_v = drv_data.vref_v;
            success = true;
        }
        api_mutex_unlock(s_ambient_mutex);
    }

    return success;
}

/** @brief */
bool api_ambient_set_led(api_ambient_led_t led, bool state)
{
    bool success = false;

    if (api_mutex_lock(s_ambient_mutex, 100U))
    {
        driver_ambient_led_t drv_led;

        switch (led)
        {
            case API_AMBIENT_LED_GREEN: drv_led = DRIVER_AMBIENT_LED_GREEN; break;
            case API_AMBIENT_LED_BLUE:  drv_led = DRIVER_AMBIENT_LED_BLUE;  break;
            case API_AMBIENT_LED_RED:   drv_led = DRIVER_AMBIENT_LED_RED;   break;
            default: api_mutex_unlock(s_ambient_mutex); return false;
        }

        success = driver_ambient_set_led(drv_led, state);
        api_mutex_unlock(s_ambient_mutex);
    }

    return success;
}

/* End of File: api_ambient.c */
