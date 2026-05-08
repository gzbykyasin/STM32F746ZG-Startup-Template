/**
 * @file    bsp_isr.c
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Implementation of the Centralized ISR Event Dispatcher.
 * @date    7 May 2026
 * @version 1.0.0
 */

#include "stm32f7xx_hal.h"

#include "bsp_isr.h"
#include "bsp_mcu.h"

#include <stdint.h>
#include <stddef.h>

/*******************************************************************************
 * Typedef, Enum & Structs
 ******************************************************************************/

/**
 * @brief ISR handler registration structure.
 */
typedef struct
{
    bsp_isr_callback_t p_callback;
    void *p_arg;
} bsp_isr_handler_t;

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/** @brief Array of registered ISR handlers and their user arguments. */
static bsp_isr_handler_t s_isr_handlers[BSP_ISR_COUNT];

/** @brief Global storage for fault diagnostic information captured during exceptions. */
static volatile bsp_isr_fault_t s_fault_info;

/** @brief Mapping of logical ISR identifiers to hardware NVIC IRQ numbers. */
static const IRQn_Type s_isr_hw_map[BSP_ISR_COUNT] = {
    [BSP_ISR_SYS_TICK] = SysTick_IRQn,
    [BSP_ISR_SYS_PEND_SV] = PendSV_IRQn,
    [BSP_ISR_SYS_SVC] = SVCall_IRQn,
    [BSP_ISR_SYS_MEM_MANAGE] = MemoryManagement_IRQn,
    [BSP_ISR_SYS_BUS_FAULT] = BusFault_IRQn,
    [BSP_ISR_SYS_USAGE_FAULT] = UsageFault_IRQn,
    [BSP_ISR_EXTI_USER_BTN] = EXTI15_10_IRQn,
    [BSP_ISR_DMA_UART_TX] = DMA1_Stream3_IRQn,
    [BSP_ISR_DMA_UART_RX] = DMA1_Stream1_IRQn,
    [BSP_ISR_DMA_ADC] = DMA2_Stream0_IRQn,
    [BSP_ISR_PER_TIM6] = TIM6_DAC_IRQn,
    [BSP_ISR_UART_DEBUG] = USART3_IRQn
};

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/** @brief Dispatches an ISR event to its registered user callback. */
static void bsp_isr_notify(bsp_isr_t isr, void *p_data)
{
    if ((BSP_ISR_COUNT > isr) && (NULL != s_isr_handlers[isr].p_callback))
    {
        void *p_arg = (NULL != p_data) ? p_data : s_isr_handlers[isr].p_arg;
        s_isr_handlers[isr].p_callback(p_arg);
    }
}

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/** @brief */
void bsp_isr_init(void)
{
    for (uint32_t i = 0; i < (uint32_t)BSP_ISR_COUNT; i++)
    {
        s_isr_handlers[i].p_callback = NULL;
        s_isr_handlers[i].p_arg = NULL;
    }
}

/** @brief */
void bsp_isr_register_callback(bsp_isr_t isr, bsp_isr_callback_t p_callback, void *p_arg)
{
    if (BSP_ISR_COUNT > isr)
    {
        s_isr_handlers[isr].p_callback = p_callback;
        s_isr_handlers[isr].p_arg = p_arg;
    }
}

/** @brief */
void bsp_isr_unregister_callback(bsp_isr_t isr)
{
    if (BSP_ISR_COUNT > isr)
    {
        s_isr_handlers[isr].p_callback = NULL;
        s_isr_handlers[isr].p_arg = NULL;
    }
}

/** @brief */
void bsp_isr_enable(bsp_isr_t isr, uint32_t preempt_prio, uint32_t sub_prio)
{
    if ((BSP_ISR_COUNT > isr) && (BSP_ISR_SYS_HARD_FAULT != isr))
    {
        HAL_NVIC_SetPriority(s_isr_hw_map[isr], preempt_prio, sub_prio);
        HAL_NVIC_EnableIRQ(s_isr_hw_map[isr]);
    }
}

/** @brief */
void bsp_isr_disable(bsp_isr_t isr)
{
    if (BSP_ISR_COUNT > isr)
    {
        HAL_NVIC_DisableIRQ(s_isr_hw_map[isr]);
    }
}

/** @brief */
void bsp_isr_clear_pending(bsp_isr_t isr)
{
    if (BSP_ISR_COUNT > isr)
    {
        HAL_NVIC_ClearPendingIRQ(s_isr_hw_map[isr]);
    }
}

/** @brief */
void bsp_isr_clear_all_pending(void)
{
    for (uint32_t i = 0; i < (uint32_t)BSP_ISR_COUNT; i++)
    {
        HAL_NVIC_ClearPendingIRQ(s_isr_hw_map[i]);
    }
}

/*******************************************************************************
 * Fault Diagnosis Functions
 ******************************************************************************/

/** @brief Extracts CPU register state from the stack during a fault exception. */
static void capture_fault_context(uint32_t *p_stack)
{
    s_fault_info.r0 = p_stack[0];
    s_fault_info.r1 = p_stack[1];
    s_fault_info.r2 = p_stack[2];
    s_fault_info.r3 = p_stack[3];
    s_fault_info.r12 = p_stack[4];
    s_fault_info.lr = p_stack[5];
    s_fault_info.pc = p_stack[6];
    s_fault_info.psr = p_stack[7];

    s_fault_info.scb_regs.cfsr = SCB->CFSR;
    s_fault_info.scb_regs.hfsr = SCB->HFSR;
    s_fault_info.scb_regs.dfsr = SCB->DFSR;
    s_fault_info.scb_regs.mmfar = SCB->MMFAR;
    s_fault_info.scb_regs.bfar = SCB->BFAR;
    s_fault_info.scb_regs.afsr = SCB->AFSR;
}

/** @brief */
void bsp_isr_hard_fault_handler_c(uint32_t *p_stack)
{
    capture_fault_context(p_stack);
    bsp_isr_notify(BSP_ISR_SYS_HARD_FAULT, (void *)&s_fault_info);
    __disable_irq();
    while (1);
}

/** @brief */
void bsp_isr_mem_manage_handler_c(uint32_t *p_stack)
{
    capture_fault_context(p_stack);
    bsp_isr_notify(BSP_ISR_SYS_MEM_MANAGE, (void *)&s_fault_info);
    __disable_irq();
    while (1);
}

/** @brief */
void bsp_isr_bus_fault_handler_c(uint32_t *p_stack)
{
    capture_fault_context(p_stack);
    bsp_isr_notify(BSP_ISR_SYS_BUS_FAULT, (void *)&s_fault_info);
    __disable_irq();
    while (1);
}

/** @brief */
void bsp_isr_usage_fault_handler_c(uint32_t *p_stack)
{
    capture_fault_context(p_stack);
    bsp_isr_notify(BSP_ISR_SYS_USAGE_FAULT, (void *)&s_fault_info);
    __disable_irq();
    while (1);
}

/*******************************************************************************
 * Low-Level Exception Handlers
 ******************************************************************************/

/** @brief Low-level HardFault exception entry point. */
__attribute__((naked)) void HardFault_Handler(void)
{
    __asm volatile(" tst lr, #4\n"
                   " ite eq\n"
                   " mrseq r0, msp\n"
                   " mrsne r0, psp\n"
                   " b bsp_isr_hard_fault_handler_c\n");
}

/** @brief Low-level Memory Management fault entry point. */
__attribute__((naked)) void MemManage_Handler(void)
{
    __asm volatile(" tst lr, #4\n"
                   " ite eq\n"
                   " mrseq r0, msp\n"
                   " mrsne r0, psp\n"
                   " b bsp_isr_mem_manage_handler_c\n");
}

/** @brief Low-level Bus Fault entry point. */
__attribute__((naked)) void BusFault_Handler(void)
{
    __asm volatile(" tst lr, #4\n"
                   " ite eq\n"
                   " mrseq r0, msp\n"
                   " mrsne r0, psp\n"
                   " b bsp_isr_bus_fault_handler_c\n");
}

/** @brief Low-level Usage Fault entry point. */
__attribute__((naked)) void UsageFault_Handler(void)
{
    __asm volatile(" tst lr, #4\n"
                   " ite eq\n"
                   " mrseq r0, msp\n"
                   " mrsne r0, psp\n"
                   " b bsp_isr_usage_fault_handler_c\n");
}

#ifndef SYS_USE_RTOS
/** @brief Supervisor Call (SVC) handler for non-RTOS builds. */
void SVC_Handler(void)
{
    bsp_isr_notify(BSP_ISR_SYS_SVC, NULL);
}

/** @brief Pendable Service (PendSV) handler for non-RTOS builds. */
void PendSV_Handler(void)
{
    bsp_isr_notify(BSP_ISR_SYS_PEND_SV, NULL);
}

/** @brief System Tick (SysTick) handler for non-RTOS builds. */
void SysTick_Handler(void)
{
    HAL_IncTick();
    bsp_isr_notify(BSP_ISR_SYS_TICK, NULL);
}
#endif

/** @brief Shared IRQ handler for EXTI lines 10 to 15. */
void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

/** @brief IRQ handler for DMA1 Stream 3 (UART TX). */
void DMA1_Stream3_IRQHandler(void)
{
    bsp_isr_notify(BSP_ISR_DMA_UART_TX, NULL);
}

/** @brief IRQ handler for DMA1 Stream 1 (UART RX). */
void DMA1_Stream1_IRQHandler(void)
{
    bsp_isr_notify(BSP_ISR_DMA_UART_RX, NULL);
}

/** @brief IRQ handler for DMA2 Stream 0 (ADC). */
void DMA2_Stream0_IRQHandler(void)
{
    bsp_isr_notify(BSP_ISR_DMA_ADC, NULL);
}

/** @brief IRQ handler for TIM6 (System Tick source). */
void TIM6_DAC_IRQHandler(void)
{
    bsp_mcu_tick_handler();
    bsp_isr_notify(BSP_ISR_PER_TIM6, NULL);
}

/** @brief IRQ handler for USART3 (Debug UART). */
void USART3_IRQHandler(void)
{
    bsp_isr_notify(BSP_ISR_UART_DEBUG, NULL);
}

/** @brief Overrides the HAL EXTI callback to dispatch events via the central ISR manager. */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_PIN_13 == GPIO_Pin)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_13); 

        bsp_isr_notify(BSP_ISR_EXTI_USER_BTN, NULL);
    }
}

/* End of File: bsp_isr.c */
