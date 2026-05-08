/**
 * @file    api_ambient.h
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Ambient API — Synchronized access to ambient sensors and LEDs.
 * @date    7 May 2026
 * @version 1.0.0
 */

#ifndef API_INC_API_AMBIENT_H_
#define API_INC_API_AMBIENT_H_

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
 * @brief API Ambient LED identifiers (mapped to driver identifiers).
 */
typedef enum
{
    API_AMBIENT_LED_GREEN = 0,  /**< System OK / Status indicator (Green). */
    API_AMBIENT_LED_BLUE,       /**< Connectivity / User indicator (Blue). */
    API_AMBIENT_LED_RED         /**< Fault / Warning indicator (Red).      */
} api_ambient_led_t;

/**
 * @brief API Ambient Data structure.
 */
typedef struct
{
    float temperature_c;        /**< Ambient temperature in degrees Celsius. */
    float vref_v;               /**< Internal reference voltage in Volts.    */
} api_ambient_data_t;

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Initializes the ambient API and synchronization primitives.
 * 
 * @return true if successful.
 */
bool api_ambient_init(void);

/**
 * @brief Reads the ambient temperature.
 * 
 * @param[out] p_data Pointer to store the temperature data.
 * @return true if reading successful.
 */
bool api_ambient_read_temp(api_ambient_data_t *p_data);

/**
 * @brief Sets the state of an ambient LED.
 * 
 * @param[in] led    LED identifier.
 * @param[in] state  true to turn ON, false to turn OFF.
 * @return true if successful.
 */
bool api_ambient_set_led(api_ambient_led_t led, bool state);

#ifdef __cplusplus
}
#endif

#endif /* API_INC_API_AMBIENT_H_ */
