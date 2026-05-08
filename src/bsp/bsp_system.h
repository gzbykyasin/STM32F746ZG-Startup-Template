/**
 * @file    bsp_system.h
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Board Support Package for System Orchestration and Initialization.
 * @date    7 May 2026
 * @version 1.0.0
 */

#ifndef BSP_INC_BSP_SYSTEM_H_
#define BSP_INC_BSP_SYSTEM_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "bsp_isr.h"

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Performs core hardware boot sequence (MPU, Cache, HAL, Clock, ISR Base).
 */
void bsp_system_boot_core(void);

/**
 * @brief Initializes all standard peripherals (GPIO, ADC, UART).
 */
void bsp_system_init_peripherals(void);

/**
 * @brief Registers handlers for all system-level hardware faults.
 * 
 * @param[in] fault_cb Callback function to handle the fault event.
 */
void bsp_system_register_fault_handlers(bsp_isr_callback_t fault_cb);

/**
 * @brief Sets up a hardware source to wake the system from sleep.
 * @param[in] wakeup_cb Callback to execute on wakeup event.
 */
void bsp_system_setup_wakeup_source(bsp_isr_callback_t wakeup_cb);

/**
 * @brief Disables and cleans up the wakeup source configuration.
 */
void bsp_system_disable_wakeup_source(void);

/**
 * @brief Prepares the hardware and enters STOP low-power mode.
 */
void bsp_system_power_enter_stop(void);

/**
 * @brief Resumes hardware operations after exiting STOP mode.
 */
void bsp_system_power_exit_stop(void);

/**
 * @brief Disables all interrupts and triggers a system reset.
 *        Used in critical fault conditions.
 */
void bsp_system_critical_error_reset(void);

#ifdef __cplusplus
}
#endif

#endif /* BSP_INC_BSP_SYSTEM_H_ */
