/**
 * @file    bsp_gpio.c
 * @author  Yasin GOZUBUYUK (gzbyk.yasinn@gmail.com)
 * @brief   Board Support Package for GPIO management.
 * @date    5 May 2026
 * @version 1.0.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
 #include "stm32f7xx_hal.h"

 #include "bsp_gpio.h"

 #include <stddef.h>

/*******************************************************************************
 * Defines
 ******************************************************************************/

/** @brief Onboard User LEDs. */
#define LD1_PIN GPIO_PIN_0
#define LD1_GPIO_PORT GPIOB
#define LD2_PIN GPIO_PIN_7
#define LD2_GPIO_PORT GPIOB
#define LD3_PIN GPIO_PIN_14
#define LD3_GPIO_PORT GPIOB

/** @brief Onboard User Button. */
#define USER_BTN_PIN GPIO_PIN_13
#define USER_BTN_GPIO_PORT GPIOC

/*******************************************************************************
 * Typedef, Enum & Structs
 ******************************************************************************/

/** @brief GPIO Pin hardware mapping structure. */
typedef struct
{
    GPIO_TypeDef *p_port;   /**< GPIO Port register base. */
    uint16_t pin;           /**< GPIO Pin identifier. */
} bsp_gpio_map_t;

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/** @brief Mapping of logical pins to hardware registers. */
static const bsp_gpio_map_t s_gpio_map[BSP_GPIO_COUNT] = {
    [BSP_GPIO_LD1] = {LD1_GPIO_PORT, LD1_PIN},
    [BSP_GPIO_LD2] = {LD2_GPIO_PORT, LD2_PIN},
    [BSP_GPIO_LD3] = {LD3_GPIO_PORT, LD3_PIN},
    [BSP_GPIO_USER_BTN] = {USER_BTN_GPIO_PORT, USER_BTN_PIN},
};

/** @brief Array of registered GPIO callbacks for external events. */
static bsp_gpio_callback_t s_gpio_callbacks[BSP_GPIO_COUNT] = {NULL};

/*******************************************************************************
 * Public Function Definitions
 ******************************************************************************/

/** @brief */
void bsp_gpio_init(void)
{
    GPIO_InitTypeDef gpio_init = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    gpio_init.Pin = LD1_PIN | LD2_PIN | LD3_PIN;
    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOB, &gpio_init);

    gpio_init.Pin = USER_BTN_PIN;
    gpio_init.Mode = GPIO_MODE_INPUT;
    gpio_init.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(USER_BTN_GPIO_PORT, &gpio_init);
}

/** @brief */
void bsp_gpio_deinit(void)
{
    HAL_GPIO_DeInit(GPIOB, LD1_PIN | LD2_PIN | LD3_PIN);
    HAL_GPIO_DeInit(GPIOC, USER_BTN_PIN);
}

/** @brief */
bsp_gpio_status_t bsp_gpio_write(bsp_gpio_pin_t pin, bsp_gpio_state_t state)
{
    if (BSP_GPIO_COUNT > pin)
    {
        HAL_GPIO_WritePin(s_gpio_map[pin].p_port, s_gpio_map[pin].pin,
                          (BSP_GPIO_HIGH == state) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        return BSP_GPIO_OK;
    }
    return BSP_GPIO_ERROR;
}

/** @brief */
bsp_gpio_state_t bsp_gpio_read(bsp_gpio_pin_t pin)
{
    if (BSP_GPIO_COUNT > pin)
    {
        GPIO_PinState state = HAL_GPIO_ReadPin(s_gpio_map[pin].p_port, s_gpio_map[pin].pin);
        return (state == GPIO_PIN_SET) ? BSP_GPIO_HIGH : BSP_GPIO_LOW;
    }
    return BSP_GPIO_LOW;
}

/** @brief */
bsp_gpio_status_t bsp_gpio_toggle(bsp_gpio_pin_t pin)
{
    if (BSP_GPIO_COUNT > pin)
    {
        HAL_GPIO_TogglePin(s_gpio_map[pin].p_port, s_gpio_map[pin].pin);
        return BSP_GPIO_OK;
    }
    return BSP_GPIO_ERROR;
}

/** @brief */
bsp_gpio_status_t bsp_gpio_register_callback(bsp_gpio_pin_t pin, bsp_gpio_callback_t p_callback)
{
    if (BSP_GPIO_COUNT > pin)
    {
        s_gpio_callbacks[pin] = p_callback;
        return BSP_GPIO_OK;
    }
    return BSP_GPIO_ERROR;
}

/** @brief */
bsp_gpio_status_t bsp_gpio_ext_interrupt_set(bsp_gpio_pin_t pin, bsp_gpio_it_mode_t mode,
                                             bsp_gpio_pull_t pull)
{
    if (BSP_GPIO_COUNT > pin)
    {
        GPIO_InitTypeDef gpio_init = {0};
        gpio_init.Pin = s_gpio_map[pin].pin;
        gpio_init.Mode = (BSP_GPIO_IT_FALLING == mode) ? GPIO_MODE_IT_FALLING
                         : (BSP_GPIO_IT_RISING_FALLING == mode) ? GPIO_MODE_IT_RISING_FALLING
                                                                : GPIO_MODE_IT_RISING;
        gpio_init.Pull = (BSP_GPIO_PULL_UP == pull) ? GPIO_PULLUP
                         : (BSP_GPIO_PULL_DOWN == pull) ? GPIO_PULLDOWN 
                                                        : GPIO_NOPULL;
                                                        
        HAL_GPIO_Init(s_gpio_map[pin].p_port, &gpio_init);
        return BSP_GPIO_OK;
    }
    return BSP_GPIO_ERROR;
}

/** @brief */
bsp_gpio_status_t bsp_gpio_ext_interrupt_reset(bsp_gpio_pin_t pin)
{
    if (BSP_GPIO_COUNT > pin)
    {
        HAL_GPIO_DeInit(s_gpio_map[pin].p_port, s_gpio_map[pin].pin);
        return BSP_GPIO_OK;
    }
    return BSP_GPIO_ERROR;
}

/** @brief */
bsp_gpio_status_t bsp_gpio_clear_pending(bsp_gpio_pin_t pin)
{
    if (BSP_GPIO_COUNT > pin)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(s_gpio_map[pin].pin);
        return BSP_GPIO_OK;
    }
    return BSP_GPIO_ERROR;
}

/* End of File: bsp_gpio.c */
