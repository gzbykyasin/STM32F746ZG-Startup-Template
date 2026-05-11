/**
 * @file    bsp_adc.c
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Implementation of DMA-based ADC with circular buffering and averaging.
 * @date    6 May 2026
 * @version 1.0.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
 #include "stm32f7xx_hal.h"

#include "bsp_adc.h"
#include "bsp_isr.h"
#include "bsp_mcu.h"

#include <stddef.h>

/*******************************************************************************
 * Defines
 ******************************************************************************/

/** @brief Number of oversampling samples per ADC channel. */
#define BSP_ADC_SAMPLES_PER_CH 5U 

/** @brief Total size of the circular DMA buffer. */
#define BSP_ADC_BUFFER_SIZE    (BSP_ADC_CH_MAX * BSP_ADC_SAMPLES_PER_CH)

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/** @brief HAL ADC handle for the primary ADC1 peripheral. */
static ADC_HandleTypeDef s_adc1;

/** @brief HAL DMA handle for high-speed ADC1 data transfer. */
static DMA_HandleTypeDef s_dma_adc1;

/** @brief Circular DMA buffer storing raw ADC samples for all active channels. */
static uint32_t s_adc_buffer[BSP_ADC_BUFFER_SIZE] = {0};

/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

/** @brief Initializes low-level ADC hardware (Clocks, DMA, Interrupts). */
static bool adc_msp_init(ADC_HandleTypeDef *hadc);

/** @brief DMA conversion complete interrupt service routine callback. */
static void adc_dma_callback(void *p_arg);

/*******************************************************************************
 * Public Function Definitions
 ******************************************************************************/

/** @brief */
bool bsp_adc_init(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    s_adc1.Instance = ADC1;

    if (!adc_msp_init(&s_adc1))
    {
        return false;
    }

    s_adc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    s_adc1.Init.Resolution = ADC_RESOLUTION_12B;
    s_adc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
    s_adc1.Init.ContinuousConvMode = ENABLE;
    s_adc1.Init.DiscontinuousConvMode = DISABLE;
    s_adc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    s_adc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    s_adc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    s_adc1.Init.NbrOfConversion = BSP_ADC_CH_MAX;
    s_adc1.Init.DMAContinuousRequests = ENABLE;
    s_adc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;

    if (HAL_OK != HAL_ADC_Init(&s_adc1))
    {
        return false;
    }

    sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
    if (HAL_OK != HAL_ADC_ConfigChannel(&s_adc1, &sConfig))
    {
        return false;
    }

    sConfig.Channel = ADC_CHANNEL_VREFINT;
    sConfig.Rank = ADC_REGULAR_RANK_2;
    if (HAL_OK != HAL_ADC_ConfigChannel(&s_adc1, &sConfig))
    {
        return false;
    }

    bsp_mcu_clean_dcache((void *)s_adc_buffer,
                         (int32_t)(BSP_ADC_BUFFER_SIZE * sizeof(s_adc_buffer[0])));

    if (HAL_OK != HAL_ADC_Start_DMA(&s_adc1, (uint32_t *)s_adc_buffer, BSP_ADC_BUFFER_SIZE))
    {
        return false;
    }

    return true;
}

/** @brief */
void bsp_adc_deinit(void)
{
    HAL_ADC_Stop_DMA(&s_adc1);

    __HAL_RCC_ADC1_CLK_DISABLE();

    HAL_DMA_DeInit(&s_dma_adc1);

    bsp_isr_disable(BSP_ISR_DMA_ADC);
    bsp_isr_unregister_callback(BSP_ISR_DMA_ADC);
    
    __HAL_RCC_DMA2_CLK_DISABLE();

    HAL_ADC_DeInit(&s_adc1);
}

/** @brief */
bool bsp_adc_read(bsp_adc_channel_t channel, uint32_t *p_value)
{
    if ((NULL == p_value) || (BSP_ADC_CH_MAX <= channel))
    {
        return false;
    }

    bsp_mcu_invalidate_dcache((void *)s_adc_buffer,
                              (int32_t)(BSP_ADC_BUFFER_SIZE * sizeof(s_adc_buffer[0])));

    uint64_t sum = 0;
    for (uint32_t i = 0; i < BSP_ADC_SAMPLES_PER_CH; i++)
    {
        sum += s_adc_buffer[channel + (i * BSP_ADC_CH_MAX)];
    }

    if (sum == 0)
    {
        return false;
    }

    *p_value = (uint32_t)(sum / BSP_ADC_SAMPLES_PER_CH);

    return true;
}

/** @brief */
uint32_t bsp_adc_error_get(void)
{
    return HAL_ADC_GetError(&s_adc1);
}

/*******************************************************************************
 * Private Function Definitions
 ******************************************************************************/

/** @brief */
static bool adc_msp_init(ADC_HandleTypeDef *hadc)
{
    if (ADC1 == hadc->Instance)
    {
        __HAL_RCC_ADC1_CLK_ENABLE();
        __HAL_RCC_DMA2_CLK_ENABLE();

        s_dma_adc1.Instance = DMA2_Stream0;
        s_dma_adc1.Init.Channel = DMA_CHANNEL_0;
        s_dma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
        s_dma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
        s_dma_adc1.Init.MemInc = DMA_MINC_ENABLE;
        s_dma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
        s_dma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
        s_dma_adc1.Init.Mode = DMA_CIRCULAR;
        s_dma_adc1.Init.Priority = DMA_PRIORITY_LOW;
        s_dma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

        if (HAL_OK != HAL_DMA_Init(&s_dma_adc1))
        {
            return false;
        }

        __HAL_LINKDMA(hadc, DMA_Handle, s_dma_adc1);
        
        bsp_isr_register_callback(BSP_ISR_DMA_ADC, adc_dma_callback, NULL);
        bsp_isr_enable(BSP_ISR_DMA_ADC, 7, 0);
        
        return true;
    }

    return false;
}

/** @brief */
static void adc_dma_callback(void *p_arg)
{
    (void)p_arg;
    HAL_DMA_IRQHandler(s_adc1.DMA_Handle);
}

/* End of File: bsp_adc.c */
