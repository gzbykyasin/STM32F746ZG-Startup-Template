/**
 * @file    app_led.h
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   LED Control Application.
 * @date    7 May 2026
 * @version 1.0.0
 */

#ifndef APP_INC_APP_LED_H_
#define APP_INC_APP_LED_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief LED control task function.
 * 
 * @param[in] p_arg Task arguments.
 */
void app_led_task(void *p_arg);

#ifdef __cplusplus
}
#endif

#endif /* APP_INC_APP_LED_H_ */
