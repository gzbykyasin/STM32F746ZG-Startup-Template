/**
 * @file    app_system.h
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Application System Orchestrator — Manages project-specific lifecycles.
 * @date    11 May 2026
 * @version 1.0.0
 */

#ifndef APP_INC_APP_SYSTEM_H_
#define APP_INC_APP_SYSTEM_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Initializes the application system, services, and tasks.
 */
void app_system_init(void);

/**
 * @brief Starts the application system kernel.
 */
void app_system_start(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_INC_APP_SYSTEM_H_ */
