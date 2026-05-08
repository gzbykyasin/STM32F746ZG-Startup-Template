/**
 * @file    bsp_uart.h
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Board Support Package for UART communication using DMA.
 * @date    6 May 2026
 * @version 1.0.0
 */

#ifndef BSP_INC_BSP_UART_H_
#define BSP_INC_BSP_UART_H_

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
 * @brief UART Instance Identifiers.
 */
typedef enum
{
    BSP_UART_DEBUG = 0, /**< USART3 (PD8/PD9) connected to ST-LINK VCP. */
    BSP_UART_COUNT      /**< Total count of UART instances. */
} bsp_uart_t;

/**
 * @brief UART Callback Event Types.
 */
typedef enum
{
    BSP_UART_EVENT_TX_COMPLETE, /**< DMA Transmission finished. */
    BSP_UART_EVENT_RX_COMPLETE, /**< DMA Reception finished. */
    BSP_UART_EVENT_ERROR        /**< Hardware or DMA error occurred. */
} bsp_uart_event_t;

/**
 * @brief UART Callback Function Type.
 *
 * @param[in] event Event type triggered by the ISR.
 * @param[in] p_arg User argument passed during registration.
 */
typedef void (*bsp_uart_callback_t)(bsp_uart_event_t event, void *p_arg);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Initializes the specified UART peripheral with DMA support.
 *
 * @param[in] uart      Logical UART identifier.
 * @param[in] baudrate  Communication speed (e.g., 115200).
 * @return true if initialization successful, false otherwise.
 */
bool bsp_uart_init(bsp_uart_t uart, uint32_t baudrate);

/**
 * @brief De-initializes the specified UART peripheral.
 *
 * @param[in] uart Logical UART identifier.
 */
void bsp_uart_deinit(bsp_uart_t uart);

/**
 * @brief Registers a callback for UART events.
 *
 * @param[in] uart        Logical UART identifier.
 * @param[in] p_callback  Function pointer to the handler function.
 * @param[in] p_arg       Optional user argument passed to the callback.
 */
void bsp_uart_register_callback(bsp_uart_t uart, 
                                bsp_uart_callback_t p_callback, 
                                void *p_arg);

/**
 * @brief Transmits data via DMA (Asynchronous).
 *
 * @param[in] uart    Logical UART identifier.
 * @param[in] p_data  Pointer to the data buffer to transmit.
 * @param[in] size    Number of bytes to transmit.
 * @return true if transmission started successfully, false if busy or error.
 */
bool bsp_uart_transmit_dma(bsp_uart_t uart, const uint8_t *p_data, uint16_t size);

/**
 * @brief Transmits data via Polling (Blocking).
 *
 * @param[in] uart    Logical UART identifier.
 * @param[in] p_data  Pointer to the data buffer to transmit.
 * @param[in] size    Number of bytes to transmit.
 * @return true if transmission successful, false otherwise.
 */
bool bsp_uart_transmit(bsp_uart_t uart, const uint8_t *p_data, uint16_t size);

/**
 * @brief Receives data via DMA (Asynchronous).
 *
 * @param[in] uart    Logical UART identifier.
 * @param[out] p_data Pointer to the destination buffer to store received data.
 * @param[in] size    Number of bytes to receive.
 * @return true if reception started successfully, false if busy or error.
 */
bool bsp_uart_receive_dma(bsp_uart_t uart, uint8_t *p_data, uint16_t size);

/**
 * @brief Checks if the UART TX is busy.
 *
 * @param[in] uart Logical UART identifier.
 * @return true if busy, false if ready for next transmission.
 */
bool bsp_uart_is_tx_busy(bsp_uart_t uart);

/**
 * @brief Returns the last HAL error code for the specified UART.
 *
 * @param[in] uart Logical UART identifier.
 * @return Raw HAL error flags (0 = no error). Use HAL_UART_ERROR_* for bit masks.
 */
uint32_t bsp_uart_error_get(bsp_uart_t uart);


#ifdef __cplusplus
}
#endif

#endif /* BSP_INC_BSP_UART_H_ */
