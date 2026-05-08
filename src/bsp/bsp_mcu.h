/**
 * @file    bsp_mcu.h
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Board Support Package for core MCU features.
 * @date    5 May 2026
 * @version 1.0.0
 */

#ifndef BSP_INC_BSP_MCU_H_
#define BSP_INC_BSP_MCU_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>

/*******************************************************************************
 * Typedef, Enum & Structs
 ******************************************************************************/

/**
 * @brief Structure to hold MCU clock frequencies.
 */
typedef struct
{
    uint32_t sys_clk; /**< System clock frequency (Hz). */
    uint32_t h_clk;   /**< HCLK frequency (Hz). */
    uint32_t p_clk1;  /**< PCLK1 frequency (Hz). */
    uint32_t p_clk2;  /**< PCLK2 frequency (Hz). */
} bsp_mcu_clock_t;

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Initializes core MCU features.
 *        Configures MPU, enables I/D Caches, and initializes the HAL library.
 */
void bsp_mcu_init(void);

/**
 * @brief Configures the system clock to maximum performance (216MHz).
 *        Sets up HSE, PLL, OverDrive, and bus dividers (HCLK, PCLK1, PCLK2).
 */
void bsp_mcu_clock_config(void);

/**
 * @brief Retrieves the 96-bit unique device identifier.
 *
 * @param[out] uid Pointer to an array of 3 uint32_t values to store the UID.
 */
void bsp_mcu_get_uid(uint32_t uid[3]);

/**
 * @brief Retrieves the current MCU clock frequencies.
 *
 * @param[out] p_clock Pointer to the clock structure to be populated.
 */
void bsp_mcu_get_clock(bsp_mcu_clock_t *p_clock);

/**
 * @brief Triggers a software system reset.
 */
void bsp_mcu_reset(void);

/**
 * @brief Enters the STOP low-power mode.
 */
void bsp_mcu_enter_stop_mode(void);

/**
 * @brief Relative delay for the calling task (blocking).
 * 
 * @param[in] ms Delay duration in milliseconds.
 */
void bsp_mcu_delay_ms(uint32_t ms);

/**
 * @brief Busy-wait delay for critical sections (blocking, no interrupts).
 * 
 * @param[in] ms Delay duration in milliseconds.
 */
void bsp_mcu_delay_busy_ms(uint32_t ms);

/**
 * @brief Busy-wait delay for critical sections (blocking, no interrupts).
 * 
 * @param[in] us Delay duration in microseconds.
 */
void bsp_mcu_delay_busy_us(uint32_t us);

/**
 * @brief Places the CPU into a low-power wait-for-interrupt (WFI) state.
 */
void bsp_mcu_wait_for_interrupt(void);

/**
 * @brief Cleans the D-cache for a memory region before DMA reads from it.
 * 
 * @param[in] addr Starting address of the memory region.
 * @param[in] size Size of the memory region in bytes.
 */
void bsp_mcu_clean_dcache(void *addr, int32_t size);

/**
 * @brief Invalidates the D-cache for a memory region after DMA writes to it.
 * 
 * @param[in] addr Starting address of the memory region.
 * @param[in] size Size of the memory region in bytes.
 */
void bsp_mcu_invalidate_dcache(void *addr, int32_t size);

/**
 * @brief Cleans and invalidates the D-cache for a memory region.
 * 
 * @param[in] addr Starting address of the memory region.
 * @param[in] size Size of the memory region in bytes.
 */
void bsp_mcu_clean_invalidate_dcache(void *addr, int32_t size);

/**
 * @brief Disables all maskable interrupts globally.
 */
void bsp_mcu_disable_global_interrupts(void);

/**
 * @brief Enables all maskable interrupts globally.
 */
void bsp_mcu_enable_global_interrupts(void);

/**
 * @brief  Handle the TIM6 Tick interrupt.
 * @note   Called by the ISR layer.
 */
void bsp_mcu_tick_handler(void);

/**
 * @brief Retrieves the current system tick count in milliseconds.
 * 
 * @return Current tick value in ms.
 */
uint32_t bsp_mcu_get_tick(void);

/**
 * @brief Suspends the system tick increment.
 */
void bsp_mcu_suspend_tick(void);

/**
 * @brief Resumes the system tick increment.
 */
void bsp_mcu_resume_tick(void);


#ifdef __cplusplus
}
#endif

#endif /* BSP_INC_BSP_MCU_H_ */
