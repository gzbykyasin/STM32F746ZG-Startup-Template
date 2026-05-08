/**
 * @file    sys_logger.h
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   System-wide logging service using RTOS Message Queues.
 * @date    6 May 2026
 * @version 1.0.0
 */

#ifndef SYSTEM_INC_SYS_LOGGER_H_
#define SYSTEM_INC_SYS_LOGGER_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * Typedef, Enum & Structs
 ******************************************************************************/

/**
 * @brief System Log Levels.
 */
typedef enum
{
    SYS_LOG_LEVEL_DEBUG = 0,    /**< Verbose debug information. */
    SYS_LOG_LEVEL_INFO,         /**< General information messages. */
    SYS_LOG_LEVEL_WARN,         /**< Warning messages for non-critical issues. */
    SYS_LOG_LEVEL_ERROR,        /**< Error messages for critical failures. */
    SYS_LOG_LEVEL_NONE          /**< No logging. */
} sys_log_level_t;

/*******************************************************************************
 * Public Macros
 ******************************************************************************/

/** @brief Log a debug message. */
#define SYS_LOG_DEBUG(mod, fmt, ...) sys_logger_log(SYS_LOG_LEVEL_DEBUG, mod, fmt, ##__VA_ARGS__)
/** @brief Log an info message. */
#define SYS_LOG_INFO(mod, fmt, ...)  sys_logger_log(SYS_LOG_LEVEL_INFO, mod, fmt, ##__VA_ARGS__)
/** @brief Log a warning message. */
#define SYS_LOG_WARN(mod, fmt, ...)  sys_logger_log(SYS_LOG_LEVEL_WARN, mod, fmt, ##__VA_ARGS__)
/** @brief Log an error message. */
#define SYS_LOG_ERROR(mod, fmt, ...) sys_logger_log(SYS_LOG_LEVEL_ERROR, mod, fmt, ##__VA_ARGS__)

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Initializes the system logger service and its internal queue.
 *
 * @return true if initialization successful, false otherwise.
 */
bool sys_logger_init(void);

/**
 * @brief De-Initializes the system logger service and releases its resources.
 */
void sys_logger_deinit(void);

/**
 * @brief Background task that processes the log queue and transmits via UART.
 *
 * @param[in] p_arg Optional user argument passed by the task manager.
 */
void sys_logger_task(void *p_arg);

/**
 * @brief Submits a log message to the system queue (Non-blocking).
 *
 * @param[in] level  Severity level of the message.
 * @param[in] module Originating module name for identification.
 * @param[in] fmt    Format string (printf-style).
 * @param[in] ...    Variable arguments for the format string.
 */
void sys_logger_log(sys_log_level_t level, const char *module, const char *fmt, ...);

/**
 * @brief Blocks until all pending log messages in the queue are transmitted.
 */
void sys_logger_flush(void);


#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_INC_SYS_LOGGER_H_ */
