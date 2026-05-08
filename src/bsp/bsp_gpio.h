/**
 * @file    bsp_gpio.h
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Board Support Package for GPIO management.
 * @date    6 May 2026
 * @version 1.0.0
 */

#ifndef BSP_INC_BSP_GPIO_H_
#define BSP_INC_BSP_GPIO_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>

/*******************************************************************************
 * Typedef, Enum & Structs
 ******************************************************************************/

/**
 * @brief GPIO Status codes.
 */
typedef enum
{
    BSP_GPIO_OK = 0,    /**< Operation successful. */
    BSP_GPIO_ERROR      /**< Operation failed. */
} bsp_gpio_status_t;

/**
 * @brief GPIO Hardware identifiers.
 */
typedef enum
{
    BSP_GPIO_LD1 = 0,   /**< Onboard LED1 (Green). */
    BSP_GPIO_LD2,       /**< Onboard LED2 (Blue). */
    BSP_GPIO_LD3,       /**< Onboard LED3 (Red). */
    BSP_GPIO_USER_BTN,  /**< Onboard User Button (Blue). */
    BSP_GPIO_COUNT      /**< Total number of GPIO pins managed. */
} bsp_gpio_pin_t;

/**
 * @brief GPIO Pin State enumeration.
 */
typedef enum
{
    BSP_GPIO_LOW = 0,   /**< Logic low level (0V). */
    BSP_GPIO_HIGH       /**< Logic high level (3.3V). */
} bsp_gpio_state_t;

/**
 * @brief GPIO Interrupt Trigger Modes.
 */
typedef enum
{
    BSP_GPIO_IT_RISING = 0,         /**< Trigger on rising edge. */
    BSP_GPIO_IT_FALLING,            /**< Trigger on falling edge. */
    BSP_GPIO_IT_RISING_FALLING      /**< Trigger on both rising and falling edges. */
} bsp_gpio_it_mode_t;

/**
 * @brief GPIO Pull Resistor configuration.
 */
typedef enum
{
    BSP_GPIO_PULL_NONE = 0,     /**< No pull resistor. */
    BSP_GPIO_PULL_UP,           /**< Internal pull-up resistor. */
    BSP_GPIO_PULL_DOWN          /**< Internal pull-down resistor. */
} bsp_gpio_pull_t;

/**
 * @brief GPIO Callback Function Type.
 *
 * @param[in] p_arg User argument passed during registration.
 */
typedef void (*bsp_gpio_callback_t)(void *p_arg);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Initializes all onboard GPIOs (LEDs and User Button).
 */
void bsp_gpio_init(void);

/**
 * @brief De-initializes all GPIO peripherals.
 */
void bsp_gpio_deinit(void);

/**
 * @brief Sets the output state of a GPIO pin.
 *
 * @param[in] pin   Target GPIO pin identifier.
 * @param[in] state Logical state to set (HIGH/LOW).
 * @return BSP_GPIO_OK on success, BSP_GPIO_ERROR otherwise.
 */
bsp_gpio_status_t bsp_gpio_write(bsp_gpio_pin_t pin, bsp_gpio_state_t state);

/**
 * @brief Reads the current state of a GPIO pin.
 *
 * @param[in] pin Target GPIO pin identifier.
 * @return Current logical state (HIGH/LOW).
 */
bsp_gpio_state_t bsp_gpio_read(bsp_gpio_pin_t pin);

/**
 * @brief Inverts the current output state of a GPIO pin.
 *
 * @param[in] pin Target GPIO pin identifier.
 * @return BSP_GPIO_OK on success, BSP_GPIO_ERROR otherwise.
 */
bsp_gpio_status_t bsp_gpio_toggle(bsp_gpio_pin_t pin);

/**
 * @brief Registers a callback for a specific GPIO (Not used for EXTI).
 *
 * @param[in] pin        Target GPIO pin identifier.
 * @param[in] p_callback Handler function pointer.
 * @return BSP_GPIO_OK on success, BSP_GPIO_ERROR otherwise.
 */
bsp_gpio_status_t bsp_gpio_register_callback(bsp_gpio_pin_t pin, bsp_gpio_callback_t p_callback);

/**
 * @brief Configures a GPIO pin for external interrupt triggering.
 *
 * @param[in] pin   Target GPIO pin (e.g., USER_BTN).
 * @param[in] mode  Trigger edge (Rising, Falling, etc.).
 * @param[in] pull  Internal resistor configuration.
 * @return BSP_GPIO_OK on success, BSP_GPIO_ERROR otherwise.
 */
bsp_gpio_status_t bsp_gpio_ext_interrupt_set(bsp_gpio_pin_t pin, bsp_gpio_it_mode_t mode,
                                             bsp_gpio_pull_t pull);

/**
 * @brief De-configures an external interrupt on a GPIO pin.
 *
 * @param[in] pin Target GPIO pin identifier.
 * @return BSP_GPIO_OK on success, BSP_GPIO_ERROR otherwise.
 */
bsp_gpio_status_t bsp_gpio_ext_interrupt_reset(bsp_gpio_pin_t pin);

/**
 * @brief Clears the pending external interrupt flag for a specific pin.
 *
 * @param[in] pin Target GPIO pin identifier.
 * @return BSP_GPIO_OK on success, BSP_GPIO_ERROR otherwise.
 */
bsp_gpio_status_t bsp_gpio_clear_pending(bsp_gpio_pin_t pin);


#ifdef __cplusplus
}
#endif

#endif /* BSP_INC_BSP_GPIO_H_ */
