/**
 * @file    api_log.c
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Logging API Implementation.
 * @date    11 May 2026
 * @version 1.0.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "api_log.h"
#include "sys_logger.h"

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/** @brief */
void api_log_task(void *p_arg)
{
    sys_logger_task(p_arg);
}

/* End of File: api_log.c */
