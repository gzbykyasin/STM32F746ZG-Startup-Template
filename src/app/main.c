/**
 * @file    main.c
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Main application entry point — NUCLEO-F746ZG Startup.
 * @date    6 May 2026
 * @version 1.0.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
 #include "api_app_manager.h"
 
 #include <stddef.h>

/*******************************************************************************
 * Public Function Definitions
 ******************************************************************************/

/** @brief */
int main(void)
{
    api_app_manager_init();
    
    api_app_manager_start();

    return 0;
}

/* End of File: main.c */
