/**
 * @file    driver_ambient.c
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Implementation of the ambient driver using internal ADC.
 * @date    7 May 2026
 * @version 1.0.0
 */

#include "driver_ambient.h"
#include "bsp_adc.h"
#include "bsp_gpio.h"

#include <stddef.h>

/*******************************************************************************
 * Defines
 ******************************************************************************/

/** @brief Factory temperature calibration values for STM32F746. */
#define TS_CAL1_ADDR      ((uint16_t *)((uint32_t)0x1FF0F44C))
#define TS_CAL2_ADDR      ((uint16_t *)((uint32_t)0x1FF0F44E))
#define TS_CAL1_TEMP      30.0f
#define TS_CAL2_TEMP      110.0f

/** @brief Factory VREFINT calibration values. */
#define VREFINT_CAL_ADDR  ((uint16_t *)((uint32_t)0x1FF0F44A))
#define VREFINT_CAL_VREF  3.3f

/*******************************************************************************
 * Public Function Definitions
 ******************************************************************************/

/** @brief */
bool driver_ambient_read(driver_ambient_data_t *p_data)
{
    if (NULL == p_data)
    {
        return false;
    }

    uint32_t ts_val;
    uint32_t vref_val;

    if (bsp_adc_read(BSP_ADC_CH_INTERNAL_TEMP, &ts_val) && (ts_val > 0U))
    {
        float ts_cal1 = (float)(*TS_CAL1_ADDR);
        float ts_cal2 = (float)(*TS_CAL2_ADDR);

        if (ts_cal2 != ts_cal1)
        {
            p_data->temperature_c =
                ((TS_CAL2_TEMP - TS_CAL1_TEMP) / (ts_cal2 - ts_cal1)) * ((float)ts_val - ts_cal1) +
                TS_CAL1_TEMP;
        }
    }

    if (bsp_adc_read(BSP_ADC_CH_VREFINT, &vref_val) && (vref_val > 0U))
    {
        p_data->vref_v = (VREFINT_CAL_VREF * (float)(*VREFINT_CAL_ADDR)) / (float)vref_val;
    }

    return true;
}

/** @brief */
bool driver_ambient_set_led(driver_ambient_led_t led, bool state)
{
    bsp_gpio_pin_t pin;

    switch (led)
    {
        case DRIVER_AMBIENT_LED_GREEN: pin = BSP_GPIO_LD1; break;
        case DRIVER_AMBIENT_LED_BLUE:  pin = BSP_GPIO_LD2; break;
        case DRIVER_AMBIENT_LED_RED:   pin = BSP_GPIO_LD3; break;
        default: return false;
    }

    if (state)
    {
        bsp_gpio_write(pin, true);
    }
    else
    {
        bsp_gpio_write(pin, false);
    }

    return true;
}

/* End of File: driver_ambient.c */
