/**
 * @file    bsp_isr.h
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Board Support Package for Interrupt Service Routine management.
 * @date    6 May 2026
 * @version 1.0.0
 */

#ifndef BSP_INC_BSP_ISR_H_
#define BSP_INC_BSP_ISR_H_

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
 * @brief Logical Interrupt Source Identifiers.
 */
typedef enum
{
    /* System Exception Handlers */
    BSP_ISR_SYS_TICK = 0,       /**< System Tick Interrupt. */
    BSP_ISR_SYS_PEND_SV,        /**< PendSV Interrupt. */
    BSP_ISR_SYS_SVC,            /**< Supervisor Call Interrupt. */
    BSP_ISR_SYS_HARD_FAULT,     /**< Hard Fault Interrupt. */
    BSP_ISR_SYS_MEM_MANAGE,     /**< Memory Management Fault. */
    BSP_ISR_SYS_BUS_FAULT,      /**< Bus Fault Interrupt. */
    BSP_ISR_SYS_USAGE_FAULT,    /**< Usage Fault Interrupt. */

    /* Peripheral Interrupts */
    BSP_ISR_DMA_UART_TX,        /**< UART DMA Transmit Interrupt. */
    BSP_ISR_DMA_UART_RX,        /**< UART DMA Receive Interrupt. */
    BSP_ISR_DMA_ADC,            /**< ADC DMA Interrupt. */
    BSP_ISR_EXTI_USER_BTN,      /**< User Button EXTI Interrupt. */
    BSP_ISR_PER_TIM6,           /**< Timer 6 (Tick) Interrupt. */
    BSP_ISR_UART_DEBUG,         /**< UART Debug Peripheral Interrupt. */

    BSP_ISR_COUNT               /**< Total number of ISR sources. */
} bsp_isr_t;

/**
 * @brief CPU Core Fault diagnostic information.
 */
typedef struct
{
    uint32_t r0;                /**< Register R0. */
    uint32_t r1;                /**< Register R1. */
    uint32_t r2;                /**< Register R2. */
    uint32_t r3;                /**< Register R3. */
    uint32_t r12;               /**< Register R12. */
    uint32_t lr;                /**< Link Register. */
    uint32_t pc;                /**< Program Counter. */
    uint32_t psr;               /**< Program Status Register. */
    struct
    {
        uint32_t cfsr;          /**< Configurable Fault Status Register. */
        uint32_t hfsr;          /**< HardFault Status Register. */
        uint32_t dfsr;          /**< Debug Fault Status Register. */
        uint32_t mmfar;         /**< MemManage Fault Address Register. */
        uint32_t bfar;          /**< BusFault Address Register. */
        uint32_t afsr;          /**< Auxiliary Fault Status Register. */
    } scb_regs;                 /**< SCB Fault Status Registers. */
} bsp_isr_fault_t;

/**
 * @brief ISR Callback Function Type.
 *
 * @param[in] p_arg Optional user argument passed during registration.
 */
typedef void (*bsp_isr_callback_t)(void *p_arg);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Initializes the ISR management layer.
 */
void bsp_isr_init(void);

/**
 * @brief Registers a callback for a specific interrupt source.
 *
 * @param[in] source      Target interrupt source identifier.
 * @param[in] p_callback  Handler function pointer.
 * @param[in] p_arg       User argument passed to the handler.
 */
void bsp_isr_register_callback(bsp_isr_t source, bsp_isr_callback_t p_callback, void *p_arg);

/**
 * @brief Unregisters a callback for a specific interrupt source.
 *
 * @param[in] source  Target interrupt source identifier.
 */
void bsp_isr_unregister_callback(bsp_isr_t source);

/**
 * @brief Enables a specific interrupt in the NVIC.
 *
 * @param[in] source    Target interrupt source identifier.
 * @param[in] priority  Preemption priority (0 is highest).
 * @param[in] subprio   Sub-priority level.
 */
void bsp_isr_enable(bsp_isr_t source, uint32_t priority, uint32_t subprio);

/**
 * @brief Disables a specific interrupt in the NVIC.
 *
 * @param[in] source Target interrupt source identifier.
 */
void bsp_isr_disable(bsp_isr_t source);

/**
 * @brief Clears the pending flag of a specific interrupt in the NVIC.
 *
 * @param[in] source Target interrupt source identifier.
 */
void bsp_isr_clear_pending(bsp_isr_t source);

/**
 * @brief Clears all peripheral pending flags except for critical wakeup sources.
 */
void bsp_isr_clear_all_pending(void);


#ifdef __cplusplus
}
#endif

#endif /* BSP_INC_BSP_ISR_H_ */
