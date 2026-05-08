/**
 * @file    bsp_mcu.c
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Implementation of core MCU management features.
 * @date    5 May 2026
 * @version 1.0.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
 #include "stm32f7xx_hal.h"

#include "bsp_mcu.h"
#include "bsp_isr.h"
#include <stm32f7xx_hal_pwr.h>
#include <stm32f7xx_hal_tim.h>

/*******************************************************************************
 * Defines
 ******************************************************************************/

/** @brief PLL M divider for system clock configuration. */
#define BSP_MCU_PLL_M      4U
/** @brief PLL N multiplier for system clock configuration. */
#define BSP_MCU_PLL_N      216U
/** @brief PLL Q divider for system clock configuration. */
#define BSP_MCU_PLL_Q      9U
/** @brief Estimated CPU cycles per iteration for busy-wait loops. */
#define BSP_MCU_BUSY_WAIT_CYCLES_PER_ITER  5U

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/** @brief HAL Timer handle for the system tick source (TIM6). */
static TIM_HandleTypeDef h_tick_timer;

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/** @brief Configures the Memory Protection Unit (MPU) for standard memory layout. */
static void mpu_config(void)
{
    MPU_Region_InitTypeDef mpu_init = {0};

    HAL_MPU_Disable();

    mpu_init.Enable = MPU_REGION_ENABLE;
    mpu_init.BaseAddress = 0x20000000;
    mpu_init.Size = MPU_REGION_SIZE_512KB;
    mpu_init.AccessPermission = MPU_REGION_FULL_ACCESS;
    mpu_init.IsBufferable = MPU_ACCESS_BUFFERABLE;
    mpu_init.IsCacheable = MPU_ACCESS_CACHEABLE;
    mpu_init.IsShareable = MPU_ACCESS_SHAREABLE;
    mpu_init.Number = MPU_REGION_NUMBER0;
    mpu_init.TypeExtField = MPU_TEX_LEVEL0;
    mpu_init.SubRegionDisable = 0x00;
    mpu_init.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

    HAL_MPU_ConfigRegion(&mpu_init);

    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/*******************************************************************************
 * MCU Core Functions
 ******************************************************************************/

/** @brief */
void bsp_mcu_init(void)
{
    mpu_config();
    SCB_EnableICache();
    SCB_EnableDCache();
    HAL_Init();

    HAL_DBGMCU_EnableDBGSleepMode();
    HAL_DBGMCU_EnableDBGStopMode();
}

/** @brief */
void bsp_mcu_clock_config(void)
{
    RCC_OscInitTypeDef rcc_osc_init = {0};
    RCC_ClkInitTypeDef rcc_clk_init = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    
    __HAL_DBGMCU_FREEZE_TIM6();

    rcc_osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    rcc_osc_init.HSEState = RCC_HSE_BYPASS;
    rcc_osc_init.PLL.PLLState = RCC_PLL_ON;
    rcc_osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    rcc_osc_init.PLL.PLLM = BSP_MCU_PLL_M;
    rcc_osc_init.PLL.PLLN = BSP_MCU_PLL_N;
    rcc_osc_init.PLL.PLLP = RCC_PLLP_DIV2;
    rcc_osc_init.PLL.PLLQ = BSP_MCU_PLL_Q;
    HAL_RCC_OscConfig(&rcc_osc_init);

    HAL_PWREx_EnableOverDrive();

    rcc_clk_init.ClockType =
        RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    rcc_clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    rcc_clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
    rcc_clk_init.APB1CLKDivider = RCC_HCLK_DIV4;
    rcc_clk_init.APB2CLKDivider = RCC_HCLK_DIV2;

    HAL_RCC_ClockConfig(&rcc_clk_init, FLASH_LATENCY_7);
}

/** @brief */
void bsp_mcu_get_uid(uint32_t uid[3])
{
    uid[0] = HAL_GetUIDw0();
    uid[1] = HAL_GetUIDw1();
    uid[2] = HAL_GetUIDw2();
}

/** @brief */
void bsp_mcu_get_clock(bsp_mcu_clock_t *p_clock)
{
    if (NULL != p_clock)
    {
        p_clock->sys_clk = HAL_RCC_GetSysClockFreq();
        p_clock->h_clk = HAL_RCC_GetHCLKFreq();
        p_clock->p_clk1 = HAL_RCC_GetPCLK1Freq();
        p_clock->p_clk2 = HAL_RCC_GetPCLK2Freq();
    }
}

/** @brief */
void bsp_mcu_reset(void)
{
    HAL_NVIC_SystemReset();
}

/** @brief */
void bsp_mcu_enter_stop_mode(void)
{
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;

    SCB->ICSR = SCB_ICSR_PENDSVCLR_Msk | SCB_ICSR_PENDSTCLR_Msk;
    
    for (uint32_t i = 0; i < 8; i++) 
    {
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }
    
    EXTI->PR = 0x007FFFFF; 

    __DSB();
    __ISB();

    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

/** @brief */
void bsp_mcu_delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}

/** @brief */
void bsp_mcu_delay_busy_ms(uint32_t ms)
{
    bsp_mcu_delay_busy_us(ms * 1000U);
}

/** @brief */
void bsp_mcu_delay_busy_us(uint32_t us)
{
    uint32_t cycles_per_us = SystemCoreClock / 1000000U;
    uint32_t iterations = (us * cycles_per_us) / BSP_MCU_BUSY_WAIT_CYCLES_PER_ITER;

    for (volatile uint32_t i = 0U; i < iterations; i++)
    {
        __NOP();
    }
}

/** @brief */
void bsp_mcu_wait_for_interrupt(void)
{
    __WFI();
}

/** @brief */
void bsp_mcu_clean_dcache(void *addr, int32_t size)
{
    SCB_CleanDCache_by_Addr((uint32_t *)addr, size);
}

/** @brief */
void bsp_mcu_invalidate_dcache(void *addr, int32_t size)
{
    SCB_InvalidateDCache_by_Addr((uint32_t *)addr, size);
}

/** @brief */
void bsp_mcu_clean_invalidate_dcache(void *addr, int32_t size)
{
    SCB_CleanInvalidateDCache_by_Addr((uint32_t *)addr, size);
}

/** @brief */
void bsp_mcu_disable_global_interrupts(void)
{
    __disable_irq();
}

/** @brief */
void bsp_mcu_enable_global_interrupts(void)
{
    __enable_irq();
}

/*******************************************************************************
 * System Tick Functions
 ******************************************************************************/

/** @brief */
uint32_t bsp_mcu_get_tick(void)
{
    return HAL_GetTick();
}

/** @brief */
void bsp_mcu_suspend_tick(void)
{
    HAL_SuspendTick();
    (void)HAL_TIM_Base_Stop_IT(&h_tick_timer);
}

/** @brief */
void bsp_mcu_resume_tick(void)
{
    (void)HAL_TIM_Base_Start_IT(&h_tick_timer);
    HAL_ResumeTick();
}

/** @brief */
void bsp_mcu_tick_handler(void)
{
    if (RESET != __HAL_TIM_GET_FLAG(&h_tick_timer, TIM_FLAG_UPDATE))
    {
        if (RESET != __HAL_TIM_GET_IT_SOURCE(&h_tick_timer, TIM_IT_UPDATE))
        {
            __HAL_TIM_CLEAR_IT(&h_tick_timer, TIM_IT_UPDATE);
            HAL_IncTick();
        }
    }
}

/*******************************************************************************
 * HAL Callback Overrides
 ******************************************************************************/

/** @brief Overrides the HAL Tick initialization to use TIM6 instead of SysTick. */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
    RCC_ClkInitTypeDef clk_init;
    uint32_t uwTimclock, uwAPB1Prescaler, uwPrescalerValue;
    uint32_t pFLatency;

    __HAL_RCC_TIM6_CLK_ENABLE();

    HAL_RCC_GetClockConfig(&clk_init, &pFLatency);
    uwAPB1Prescaler = clk_init.APB1CLKDivider;

    if (RCC_HCLK_DIV1 == uwAPB1Prescaler)
    {
        uwTimclock = HAL_RCC_GetPCLK1Freq();
    }
    else
    {
        uwTimclock = 2 * HAL_RCC_GetPCLK1Freq();
    }

    uwPrescalerValue = (uint32_t)((uwTimclock / 1000000U) - 1U);

    h_tick_timer.Instance = TIM6;
    h_tick_timer.Init.Period = (1000000U / 1000U) - 1U;
    h_tick_timer.Init.Prescaler = uwPrescalerValue;
    h_tick_timer.Init.ClockDivision = 0;
    h_tick_timer.Init.CounterMode = TIM_COUNTERMODE_UP;

    if (HAL_OK != HAL_TIM_Base_Init(&h_tick_timer))
    {
        return HAL_ERROR;
    }

    if (HAL_OK != HAL_TIM_Base_Start_IT(&h_tick_timer))
    {
        return HAL_ERROR;
    }

    HAL_NVIC_SetPriority(TIM6_DAC_IRQn, TickPriority, 0U);
    HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);

    return HAL_OK;
}

/** @brief Suspends the HAL tick source (TIM6). */
void HAL_SuspendTick(void)
{
    __HAL_TIM_DISABLE_IT(&h_tick_timer, TIM_IT_UPDATE);
}

/** @brief Resumes the HAL tick source (TIM6). */
void HAL_ResumeTick(void)
{
    __HAL_TIM_ENABLE_IT(&h_tick_timer, TIM_IT_UPDATE);
}

/* End of File: bsp_mcu.c */
