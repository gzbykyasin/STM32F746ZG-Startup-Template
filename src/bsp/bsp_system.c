/**
 * @file    bsp_system.c
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Implementation of the System Orchestration BSP.
 * @date    7 May 2026
 * @version 1.0.0
 */

#include "bsp_system.h"
#include "bsp_mcu.h"
#include "bsp_isr.h"
#include "bsp_gpio.h"
#include "bsp_adc.h"
#include "bsp_uart.h"

#include <stddef.h>

/*******************************************************************************
 * Public Function Definitions
 ******************************************************************************/

/** @brief */
void bsp_system_boot_core(void)
{
    bsp_mcu_init();
    bsp_isr_init();
    bsp_mcu_clock_config();
    (void)bsp_gpio_init();
}

/** @brief */
void bsp_system_init_peripherals(void)
{
    (void)bsp_adc_init();
    (void)bsp_uart_init(BSP_UART_DEBUG, 115200U);
}

/** @brief */
void bsp_system_register_fault_handlers(bsp_isr_callback_t fault_cb)
{
    if (NULL != fault_cb)
    {
        bsp_isr_register_callback(BSP_ISR_SYS_HARD_FAULT,  fault_cb, NULL);
        bsp_isr_register_callback(BSP_ISR_SYS_MEM_MANAGE,  fault_cb, NULL);
        bsp_isr_register_callback(BSP_ISR_SYS_BUS_FAULT,   fault_cb, NULL);
        bsp_isr_register_callback(BSP_ISR_SYS_USAGE_FAULT, fault_cb, NULL);
    }
}

/** @brief */
void bsp_system_setup_wakeup_source(bsp_isr_callback_t wakeup_cb)
{
    if (NULL != wakeup_cb)
    {
        bsp_isr_register_callback(BSP_ISR_EXTI_USER_BTN, wakeup_cb, NULL);
        bsp_gpio_ext_interrupt_set(BSP_GPIO_USER_BTN, BSP_GPIO_IT_RISING, BSP_GPIO_PULL_NONE);
        bsp_isr_enable(BSP_ISR_EXTI_USER_BTN, 6U, 0U);
    }
}

/** @brief */
void bsp_system_disable_wakeup_source(void)
{
    bsp_isr_disable(BSP_ISR_EXTI_USER_BTN);
    bsp_isr_clear_pending(BSP_ISR_EXTI_USER_BTN);
    (void)bsp_gpio_ext_interrupt_reset(BSP_GPIO_USER_BTN);
    (void)bsp_gpio_clear_pending(BSP_GPIO_USER_BTN);
    bsp_isr_register_callback(BSP_ISR_EXTI_USER_BTN, NULL, NULL);
}

/** @brief */
void bsp_system_power_enter_stop(void)
{
    (void)bsp_gpio_write(BSP_GPIO_LD1, BSP_GPIO_LOW);
    (void)bsp_gpio_write(BSP_GPIO_LD2, BSP_GPIO_LOW);
    (void)bsp_gpio_write(BSP_GPIO_LD3, BSP_GPIO_LOW);

    bsp_adc_deinit();
    bsp_uart_deinit(BSP_UART_DEBUG);
    bsp_mcu_suspend_tick();
    bsp_mcu_enter_stop_mode();
}

/** @brief */
void bsp_system_power_exit_stop(void)
{
    bsp_mcu_clock_config();
    bsp_mcu_resume_tick();

    (void)bsp_gpio_write(BSP_GPIO_LD3, BSP_GPIO_HIGH);
}

/** @brief */
void bsp_system_critical_error_reset(void)
{
    bsp_mcu_disable_global_interrupts();
    bsp_mcu_reset();
}

/* End of File: bsp_system.c */
