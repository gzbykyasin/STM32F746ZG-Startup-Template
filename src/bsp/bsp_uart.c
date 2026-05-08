/**
 * @file    bsp_uart.c
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Implementation of UART communication using DMA for Nucleo-F746ZG.
 * @date    6 May 2026
 * @version 1.0.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
 #include "stm32f7xx_hal.h"

#include "bsp_uart.h"
#include "bsp_isr.h"
#include "bsp_mcu.h"

#include <stddef.h>

/*******************************************************************************
 * Defines
 ******************************************************************************/

/** @brief Maximum timeout for blocking UART operations (ms). */
#define UART_MAX_TIMEOUT 1000U

/*******************************************************************************
 * Typedef, Enum & Structs
 ******************************************************************************/

/**
 * @brief UART Context structure for managing hardware handles and callbacks.
 */
typedef struct
{
    UART_HandleTypeDef h_uart;      /**< HAL UART handle. */
    DMA_HandleTypeDef h_dma_tx;     /**< HAL DMA TX handle. */
    DMA_HandleTypeDef h_dma_rx;     /**< HAL DMA RX handle. */
    bsp_uart_callback_t p_callback; /**< User callback function. */
    void *p_arg;                    /**< User argument for callback. */
} bsp_uart_ctx_t;

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/** @brief Internal contexts for all supported UART instances. */
static bsp_uart_ctx_t s_uart_ctx[BSP_UART_COUNT];

/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

/** @brief Initializes low-level hardware (GPIO, DMA, Clocks) for a UART instance. */
static bool uart_msp_init(UART_HandleTypeDef *huart);
/** @brief DMA TX complete interrupt service routine callback. */
static void uart_tx_dma_callback(void *p_arg);
/** @brief DMA RX complete interrupt service routine callback. */
static void uart_rx_dma_callback(void *p_arg);
/** @brief UART peripheral interrupt service routine callback. */
static void uart_irq_callback(void *p_arg);

/*******************************************************************************
 * Public Function Definitions
 ******************************************************************************/

/** @brief */
bool bsp_uart_init(bsp_uart_t uart, uint32_t baudrate)
{
    if (BSP_UART_COUNT <= uart)
    {
        return false;
    }

    if (BSP_UART_DEBUG == uart)
    {
        s_uart_ctx[uart].h_uart.Instance = USART3;
    }

    s_uart_ctx[uart].h_uart.Init.BaudRate = baudrate;
    s_uart_ctx[uart].h_uart.Init.WordLength = UART_WORDLENGTH_8B;
    s_uart_ctx[uart].h_uart.Init.StopBits = UART_STOPBITS_1;
    s_uart_ctx[uart].h_uart.Init.Parity = UART_PARITY_NONE;
    s_uart_ctx[uart].h_uart.Init.Mode = UART_MODE_TX_RX;
    s_uart_ctx[uart].h_uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    s_uart_ctx[uart].h_uart.Init.OverSampling = UART_OVERSAMPLING_16;
    s_uart_ctx[uart].h_uart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    s_uart_ctx[uart].h_uart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    if (!uart_msp_init(&s_uart_ctx[uart].h_uart))
    {
        return false;
    }

    if (HAL_UART_Init(&s_uart_ctx[uart].h_uart) != HAL_OK)
    {
        return false;
    }

    if (BSP_UART_DEBUG == uart)
    {
        bsp_isr_register_callback(BSP_ISR_DMA_UART_TX, uart_tx_dma_callback, &s_uart_ctx[uart]);
        bsp_isr_register_callback(BSP_ISR_DMA_UART_RX, uart_rx_dma_callback, &s_uart_ctx[uart]);
        bsp_isr_register_callback(BSP_ISR_UART_DEBUG, uart_irq_callback, &s_uart_ctx[uart]);

        bsp_isr_enable(BSP_ISR_DMA_UART_TX, 7, 0);
        bsp_isr_enable(BSP_ISR_DMA_UART_RX, 7, 0);
        bsp_isr_enable(BSP_ISR_UART_DEBUG, 7, 0);
    }

    return true;
}

/** @brief */
void bsp_uart_deinit(bsp_uart_t uart)
{
    if (BSP_UART_COUNT <= uart)
    {
        return;
    }

    HAL_UART_DeInit(&s_uart_ctx[uart].h_uart);
    HAL_DMA_DeInit(&s_uart_ctx[uart].h_dma_tx);
    HAL_DMA_DeInit(&s_uart_ctx[uart].h_dma_rx);

    if (BSP_UART_DEBUG == uart)
    {
        bsp_isr_disable(BSP_ISR_DMA_UART_TX);
        bsp_isr_disable(BSP_ISR_DMA_UART_RX);
        bsp_isr_disable(BSP_ISR_UART_DEBUG);
    }
}

/** @brief */
void bsp_uart_register_callback(bsp_uart_t uart, bsp_uart_callback_t p_callback, void *p_arg)
{
    if (BSP_UART_COUNT > uart)
    {
        s_uart_ctx[uart].p_callback = p_callback;
        s_uart_ctx[uart].p_arg = p_arg;
    }
}

/** @brief */
bool bsp_uart_transmit_dma(bsp_uart_t uart, const uint8_t *p_data, uint16_t size)
{
    if (BSP_UART_COUNT <= uart)
    {
        return false;
    }

    bsp_mcu_clean_dcache((void *)p_data, (int32_t)size);

    if (HAL_UART_Transmit_DMA(&s_uart_ctx[uart].h_uart, (uint8_t *)p_data, size) != HAL_OK)
    {
        return false;
    }

    return true;
}

/** @brief */
bool bsp_uart_transmit(bsp_uart_t uart, const uint8_t *p_data, uint16_t size)
{
    if ((BSP_UART_COUNT > uart) && (NULL != p_data) && (0 < size))
    {
        if (HAL_OK == HAL_UART_Transmit(&s_uart_ctx[uart].h_uart, (uint8_t *)p_data, size, UART_MAX_TIMEOUT))
        {
            return true;
        }
    }
    return false;
}

/** @brief */
bool bsp_uart_receive_dma(bsp_uart_t uart, uint8_t *p_data, uint16_t size)
{
    if (BSP_UART_COUNT <= uart)
    {
        return false;
    }

    if (HAL_OK != HAL_UART_Receive_DMA(&s_uart_ctx[uart].h_uart, p_data, size))
    {
        return false;
    }

    return true;
}

/** @brief */
bool bsp_uart_is_tx_busy(bsp_uart_t uart)
{
    if (BSP_UART_COUNT <= uart)
    {
        return false;
    }
    return (s_uart_ctx[uart].h_uart.gState != HAL_UART_STATE_READY);
}

/** @brief */
uint32_t bsp_uart_error_get(bsp_uart_t uart)
{
    if (BSP_UART_COUNT <= uart)
    {
        return 0xFFFFFFFFU;
    }
    return HAL_UART_GetError(&s_uart_ctx[uart].h_uart);
}

/*******************************************************************************
 * Private Function Definitions
 ******************************************************************************/

/** @brief */
static bool uart_msp_init(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (USART3 == huart->Instance)
    {
        __HAL_RCC_USART3_CLK_ENABLE();
        __HAL_RCC_GPIOD_CLK_ENABLE();
        __HAL_RCC_DMA1_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

        s_uart_ctx[BSP_UART_DEBUG].h_dma_tx.Instance = DMA1_Stream3;
        s_uart_ctx[BSP_UART_DEBUG].h_dma_tx.Init.Channel = DMA_CHANNEL_4;
        s_uart_ctx[BSP_UART_DEBUG].h_dma_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        s_uart_ctx[BSP_UART_DEBUG].h_dma_tx.Init.PeriphInc = DMA_PINC_DISABLE;
        s_uart_ctx[BSP_UART_DEBUG].h_dma_tx.Init.MemInc = DMA_MINC_ENABLE;
        s_uart_ctx[BSP_UART_DEBUG].h_dma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        s_uart_ctx[BSP_UART_DEBUG].h_dma_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        s_uart_ctx[BSP_UART_DEBUG].h_dma_tx.Init.Mode = DMA_NORMAL;
        s_uart_ctx[BSP_UART_DEBUG].h_dma_tx.Init.Priority = DMA_PRIORITY_LOW;
        s_uart_ctx[BSP_UART_DEBUG].h_dma_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

        if (HAL_OK != HAL_DMA_Init(&s_uart_ctx[BSP_UART_DEBUG].h_dma_tx))
        {
            return false;
        }

        __HAL_LINKDMA(huart, hdmatx, s_uart_ctx[BSP_UART_DEBUG].h_dma_tx);

        s_uart_ctx[BSP_UART_DEBUG].h_dma_rx.Instance = DMA1_Stream1;
        s_uart_ctx[BSP_UART_DEBUG].h_dma_rx.Init.Channel = DMA_CHANNEL_4;
        s_uart_ctx[BSP_UART_DEBUG].h_dma_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        s_uart_ctx[BSP_UART_DEBUG].h_dma_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        s_uart_ctx[BSP_UART_DEBUG].h_dma_rx.Init.MemInc = DMA_MINC_ENABLE;
        s_uart_ctx[BSP_UART_DEBUG].h_dma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        s_uart_ctx[BSP_UART_DEBUG].h_dma_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        s_uart_ctx[BSP_UART_DEBUG].h_dma_rx.Init.Mode = DMA_NORMAL;
        s_uart_ctx[BSP_UART_DEBUG].h_dma_rx.Init.Priority = DMA_PRIORITY_LOW;
        s_uart_ctx[BSP_UART_DEBUG].h_dma_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

        if (HAL_OK != HAL_DMA_Init(&s_uart_ctx[BSP_UART_DEBUG].h_dma_rx))
        {
            HAL_DMA_DeInit(&s_uart_ctx[BSP_UART_DEBUG].h_dma_tx);
            return false;
        }

        __HAL_LINKDMA(huart, hdmarx, s_uart_ctx[BSP_UART_DEBUG].h_dma_rx);
        return true;
    }

    return false;
}

/** @brief */
static void uart_tx_dma_callback(void *p_arg)
{
    bsp_uart_ctx_t *p_ctx = (bsp_uart_ctx_t *)p_arg;
    HAL_DMA_IRQHandler(&p_ctx->h_dma_tx);
}

/** @brief */
static void uart_rx_dma_callback(void *p_arg)
{
    bsp_uart_ctx_t *p_ctx = (bsp_uart_ctx_t *)p_arg;
    HAL_DMA_IRQHandler(&p_ctx->h_dma_rx);
}

/** @brief */
static void uart_irq_callback(void *p_arg)
{
    bsp_uart_ctx_t *p_ctx = (bsp_uart_ctx_t *)p_arg;
    HAL_UART_IRQHandler(&p_ctx->h_uart);
}

/*******************************************************************************
 * HAL Callback Overrides
 ******************************************************************************/

/** @brief Overrides the HAL UART TX complete callback to trigger registered user events. */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    for (uint32_t i = 0; i < BSP_UART_COUNT; i++)
    {
        if (huart == &s_uart_ctx[i].h_uart)
        {
            if (s_uart_ctx[i].p_callback)
            {
                s_uart_ctx[i].p_callback(BSP_UART_EVENT_TX_COMPLETE, s_uart_ctx[i].p_arg);
            }
            break;
        }
    }
}

/** @brief Overrides the HAL UART RX complete callback to trigger registered user events. */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    for (uint32_t i = 0; i < BSP_UART_COUNT; i++)
    {
        if (huart == &s_uart_ctx[i].h_uart)
        {
            if (s_uart_ctx[i].p_callback)
            {
                s_uart_ctx[i].p_callback(BSP_UART_EVENT_RX_COMPLETE, s_uart_ctx[i].p_arg);
            }
            break;
        }
    }
}

/** @brief Overrides the HAL UART error callback to trigger registered user events. */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    for (uint32_t i = 0; i < BSP_UART_COUNT; i++)
    {
        if (huart == &s_uart_ctx[i].h_uart)
        {
            if (s_uart_ctx[i].p_callback)
            {
                s_uart_ctx[i].p_callback(BSP_UART_EVENT_ERROR, s_uart_ctx[i].p_arg);
            }
            break;
        }
    }
}

/* End of File: bsp_uart.c */
