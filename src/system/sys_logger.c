/**
 * @file    sys_logger.c
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Asynchronous logging implementation using Universal Executive Queues.
 * @date    6 May 2026
 * @version 1.0.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "sys_logger.h"
#include "sys_rtos.h"

#include "bsp_uart.h"
#include "bsp_gpio.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/*******************************************************************************
 * Defines
 ******************************************************************************/

/** @brief Maximum length of the log message text. */
#define LOG_MSG_TEXT_LEN      128U

/** @brief Maximum length of the module identifier name. */
#define LOG_MSG_MOD_LEN       12U

/** @brief Size of the final output buffer for UART transmission. */
#define LOG_OUT_BUF_LEN       256U

/** @brief Capacity of the asynchronous log message queue. */
#define LOG_QUEUE_SIZE        16U

/** @brief Timeout for queue operations in milliseconds. */
#define LOG_QUEUE_MAX_TIMEOUT 100U

/*******************************************************************************
 * Typedef, Enum & Structs
 ******************************************************************************/

/** @brief Log Message structure stored in the queue. */
typedef struct
{
    uint32_t timestamp;             /**< System tick at log creation. */
    sys_log_level_t level;          /**< Severity level of the log. */
    char module[LOG_MSG_MOD_LEN];   /**< Name of the source module. */
    char text[LOG_MSG_TEXT_LEN];    /**< Actual formatted log text. */
} sys_log_msg_t;

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/** @brief Handle for the system-wide logging queue. */
static sys_rtos_queue_t s_log_queue = NULL;

/** @brief Human-readable strings for each log level. */
static const char *s_level_strings[] = {
    "DEBUG", "INFO ", "WARN ", "ERROR", "NONE "};

/** @brief Output buffer for final UART log formatting. */
static char s_log_out_buf[LOG_OUT_BUF_LEN] __attribute__((aligned(32)));

/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

/** @brief Formats and writes a log message directly to UART. */
static void logger_write_to_uart(const sys_log_msg_t *p_msg);

/*******************************************************************************
 * Private Function Definitions
 ******************************************************************************/

/** @brief */
static void logger_write_to_uart(const sys_log_msg_t *p_msg)
{
    int len = snprintf(s_log_out_buf, sizeof(s_log_out_buf), "[%08lu] [%s] %s: %s\r\n",
                       p_msg->timestamp, s_level_strings[p_msg->level],
                       p_msg->module, p_msg->text);

    if (len > 0)
    {
        bsp_uart_transmit_dma(BSP_UART_DEBUG, (const uint8_t *)s_log_out_buf, (uint16_t)len);
    }
}

/*******************************************************************************
 * Public Function Definitions
 ******************************************************************************/

/** @brief */
bool sys_logger_init(void)
{
    if (SYS_RTOS_OK != sys_rtos_queue_create(LOG_QUEUE_SIZE, sizeof(sys_log_msg_t), &s_log_queue))
    {
        return false;
    }

    return true;
}

/** @brief */
void sys_logger_deinit(void)
{
    if (NULL != s_log_queue)
    {
        (void)sys_rtos_queue_delete(s_log_queue);
        s_log_queue = NULL;
    }
}

/** @brief */
void sys_logger_log(sys_log_level_t level, const char *module, const char *fmt, ...)
{
    if (level >= SYS_LOG_LEVEL_NONE)
    {
        return;
    }

    sys_log_msg_t msg;
    msg.timestamp = sys_rtos_get_tick();
    msg.level = level;

    strncpy(msg.module, module, LOG_MSG_MOD_LEN - 1U);
    msg.module[LOG_MSG_MOD_LEN - 1U] = '\0';

    va_list args;
    va_start(args, fmt);
    vsnprintf(msg.text, LOG_MSG_TEXT_LEN, fmt, args);
    va_end(args);

    if (NULL != s_log_queue)
    {
        (void)sys_rtos_queue_put(s_log_queue, &msg, 0U);
    }
}

/** @brief */
void sys_logger_flush(void)
{
    if (NULL == s_log_queue)
    {
        return;
    }

    sys_log_msg_t msg;

    while (SYS_RTOS_OK == sys_rtos_queue_get(s_log_queue, &msg, 0U))
    {
        while (bsp_uart_is_tx_busy(BSP_UART_DEBUG))
        {
            sys_rtos_delay_ms(1U);
        }
        logger_write_to_uart(&msg);
    }

    while (bsp_uart_is_tx_busy(BSP_UART_DEBUG))
    {
        sys_rtos_delay_ms(1U);
    }
    
    sys_rtos_delay_ms(5U);
}

/** @brief */
void sys_logger_task(void *p_arg)
{
    (void)p_arg;
    sys_log_msg_t msg;

    do {
        if (bsp_uart_is_tx_busy(BSP_UART_DEBUG))
        {
            (void)sys_rtos_delay_ms(1U);
            if (!sys_rtos_task_should_loop())
            {
                return;
            }
            continue;
        }

        if (SYS_RTOS_OK == sys_rtos_queue_get(s_log_queue, &msg, 0U))
        {
            bsp_gpio_toggle(BSP_GPIO_LD1);
            logger_write_to_uart(&msg);
        }
        else
        {
            if (!sys_rtos_task_should_loop())
            {
                return;
            }
            (void)sys_rtos_delay_ms(10U);
        }
    } while (sys_rtos_task_should_loop());
}

/* End of File: sys_logger.c */
