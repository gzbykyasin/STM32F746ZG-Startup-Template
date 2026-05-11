/**
 * @file    api_log.h
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Logging API — Facade for system logging services.
 * @date    11 May 2026
 * @version 1.0.0
 */

#ifndef API_INC_API_LOG_H_
#define API_INC_API_LOG_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "sys_logger.h"

/*******************************************************************************
 * Macros
 ******************************************************************************/

#define API_LOG_INFO(tag, fmt, ...)  SYS_LOG_INFO(tag, fmt, ##__VA_ARGS__)
#define API_LOG_WARN(tag, fmt, ...)  SYS_LOG_WARN(tag, fmt, ##__VA_ARGS__)
#define API_LOG_ERROR(tag, fmt, ...) SYS_LOG_ERROR(tag, fmt, ##__VA_ARGS__)
#define API_LOG_DEBUG(tag, fmt, ...) SYS_LOG_DEBUG(tag, fmt, ##__VA_ARGS__)

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Logger task function.
 */
void api_log_task(void *p_arg);

#ifdef __cplusplus
}
#endif

#endif /* API_INC_API_LOG_H_ */
