/**
 * @file    bsp_adc.h
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Board Support Package for ADC management.
 * @date    6 May 2026
 * @version 1.0.0
 */

#ifndef BSP_INC_BSP_ADC_H_
#define BSP_INC_BSP_ADC_H_

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
 * @brief ADC Channel identifiers.
 */
typedef enum
{
    BSP_ADC_CH_INTERNAL_TEMP, /**< Internal Temperature Sensor. */
    BSP_ADC_CH_VREFINT,       /**< Internal Voltage Reference. */
    BSP_ADC_CH_MAX            /**< Total number of channels. */
} bsp_adc_channel_t;

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Initializes the ADC peripheral.
 * 
 * @return true if initialization successful, false otherwise.
 */
bool bsp_adc_init(void);

/**
 * @brief De-Initializes the ADC peripheral.
 */
void bsp_adc_deinit(void);

/**
 * @brief Reads a value from a specific ADC channel.
 *
 * @param[in]  channel The ADC channel to read from.
 * @param[out] p_value Pointer to store the 12-bit ADC value.
 * @return true if reading successful, false otherwise.
 */
bool bsp_adc_read(bsp_adc_channel_t channel, uint32_t *p_value);

/**
 * @brief Returns the last HAL error code from the ADC peripheral.
 *
 * @return Raw HAL error flags (0 = no error). Use HAL_ADC_ERROR_* for bit masks.
 */
uint32_t bsp_adc_error_get(void);


#ifdef __cplusplus
}
#endif

#endif /* BSP_INC_BSP_ADC_H_ */
