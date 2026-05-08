/**
 * @file    driver_ambient.h
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Header for ambient sensor driver (Internal Temperature).
 * @date    7 May 2026
 * @version 1.0.0
 */

#ifndef DRIVER_INC_AMBIENT_H_
#define DRIVER_INC_AMBIENT_H_

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
 * @brief Ambient sensor data structure.
 */
typedef struct
{
    float temperature_c;    /**< Calculated temperature in Celsius. */
    float vref_v;           /**< Internal voltage reference in Volts. */
} driver_ambient_data_t;

/**
 * @brief Ambient LED identifiers.
 */
typedef enum
{
    DRIVER_AMBIENT_LED_GREEN = 0,   /**< Onboard Green LED. */
    DRIVER_AMBIENT_LED_BLUE,        /**< Onboard Blue LED. */
    DRIVER_AMBIENT_LED_RED          /**< Onboard Red LED. */
} driver_ambient_led_t;

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Reads data from the ambient sensors.
 *
 * @param[out] p_data Pointer to the structure to store conversion results.
 * @return true if reading successful, false otherwise.
 */
bool driver_ambient_read(driver_ambient_data_t *p_data);

/**
 * @brief Sets the state of an ambient LED.
 * 
 * @param[in] led    LED identifier.
 * @param[in] state  true to turn ON, false to turn OFF.
 * @return true if successful.
 */
bool driver_ambient_set_led(driver_ambient_led_t led, bool state);

#ifdef __cplusplus
}
#endif

#endif /* DRIVER_INC_AMBIENT_H_ */

