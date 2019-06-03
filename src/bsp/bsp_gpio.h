/***************************************************************************************************
 *   Project:     
 *   Author:        Stulov Tikhon (kudesnick@inbox.ru)
 ***************************************************************************************************
 *   Distribution:  
 *
 ***************************************************************************************************
 *   MCU Family:    STM32F
 *   Compiler:      ARMCC
 ***************************************************************************************************
 *   File:          bsp_gpio.h
 *   Description:   
 *
 ***************************************************************************************************
 *   History:       03.05.2019 - file created
 *
 **************************************************************************************************/

#pragma once

#include <stdint.h>

#include "RTE_Components.h"
#include CMSIS_device_header

#if defined (STM32F10X_LD) || defined (STM32F10X_LD_VL) || defined (STM32F10X_MD) || defined (STM32F10X_MD_VL) || defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_XL) || defined (STM32F10X_CL)
    #ifdef  __cplusplus
    extern "C"
    {
    #endif
        #include "GPIO_STM32F10x.h"
    #ifdef  __cplusplus
    };
    #endif
#else 
    #include "stm32l4xx_hal.h"
#endif

/***************************************************************************************************
 *                                       DEFINITIONS
 **************************************************************************************************/

#ifdef HAL_MODULE_ENABLED

#define GPIO_OUT_CONFIG(port, pin)                  \
    {                                               \
        GPIO_InitTypeDef GPIO_Init;                 \
                                                    \
        GPIO_Init.Pin  = 1 << pin;                  \
        GPIO_Init.Mode  = GPIO_MODE_OUTPUT_PP;      \
        GPIO_Init.Speed = GPIO_SPEED_FREQ_MEDIUM;   \
                                                    \
        HAL_GPIO_Init(port, &GPIO_Init);            \
    }

#define GPIO_IN_FLT_CONFIG(port, pin)       \
    {                                       \
        GPIO_InitTypeDef GPIO_Init;         \
                                            \
        GPIO_Init.Pin   = 1 << pin;         \
        GPIO_Init.Mode  = GPIO_MODE_INPUT;  \
        GPIO_Init.Pull = GPIO_NOPULL;       \
                                            \
        HAL_GPIO_Init(port, &GPIO_Init);    \
    }
    
#define GPIO_IN_PUP_CONFIG(port, pin)       \
    {                                       \
        GPIO_InitTypeDef GPIO_Init;         \
                                            \
        GPIO_Init.Pin   = 1 << pin;         \
        GPIO_Init.Mode  = GPIO_MODE_INPUT;  \
        GPIO_Init.Pull  = GPIO_PULLUP;      \
                                            \
        HAL_GPIO_Init(port, &GPIO_Init);    \
    }

#define GPIO_WRITE(port, pin, val)    HAL_GPIO_WritePin(port, pin, static_cast<GPIO_PinState>(val))
#define GPIO_WRITE_PIN(port_pin, val) HAL_GPIO_WritePin(port_pin, static_cast<GPIO_PinState>(val))
#define GPIO_READ(port, pin)          HAL_GPIO_ReadPin(port, pin)
#define GPIO_READ_PIN(port_pin)       HAL_GPIO_ReadPin(port_pin)


#define GPIO_PinWrite HAL_GPIO_WritePin
#define GPIO_PinRead  HAL_GPIO_ReadPin

#else

#define GPIO_OUT_CONFIG(port, pin)    GPIO_PinConfigure(port, pin, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT10MHZ)
#define GPIO_IN_FLT_CONFIG(port, pin) GPIO_PinConfigure(port, pin, GPIO_IN_PULL_UP, GPIO_MODE_INPUT)
#define GPIO_IN_PUP_CONFIG(port, pin) GPIO_PinConfigure(port, pin, GPIO_IN_FLOATING, GPIO_MODE_INPUT)
#define GPIO_WRITE(port, pin, val)    GPIO_PinWrite(port, pin, val)
#define GPIO_WRITE_PIN(port_pin, val) GPIO_PinWrite(port_pin, val)
#define GPIO_READ(port, pin)          GPIO_PinRead(port, pin)
#define GPIO_READ_PIN(port_pin)       GPIO_PinRead(port_pin)

#endif

#define PORTA_00 GPIOA, 0x0
#define PORTA_01 GPIOA, 0x1
#define PORTA_02 GPIOA, 0x2
#define PORTA_03 GPIOA, 0x3
#define PORTA_04 GPIOA, 0x4
#define PORTA_05 GPIOA, 0x5
#define PORTA_06 GPIOA, 0x6
#define PORTA_07 GPIOA, 0x7
#define PORTA_08 GPIOA, 0x8
#define PORTA_09 GPIOA, 0x9
#define PORTA_10 GPIOA, 0xA
#define PORTA_11 GPIOA, 0xB
#define PORTA_12 GPIOA, 0xC
#define PORTA_13 GPIOA, 0xD
#define PORTA_14 GPIOA, 0xE
#define PORTA_15 GPIOA, 0xF

#define PORTB_00 GPIOB, 0x0
#define PORTB_01 GPIOB, 0x1
#define PORTB_02 GPIOB, 0x2
#define PORTB_03 GPIOB, 0x3
#define PORTB_04 GPIOB, 0x4
#define PORTB_05 GPIOB, 0x5
#define PORTB_06 GPIOB, 0x6
#define PORTB_07 GPIOB, 0x7
#define PORTB_08 GPIOB, 0x8
#define PORTB_09 GPIOB, 0x9
#define PORTB_10 GPIOB, 0xA
#define PORTB_11 GPIOB, 0xB
#define PORTB_12 GPIOB, 0xC
#define PORTB_13 GPIOB, 0xD
#define PORTB_14 GPIOB, 0xE
#define PORTB_15 GPIOB, 0xF

#define PORTC_00 GPIOC, 0x0
#define PORTC_01 GPIOC, 0x1
#define PORTC_02 GPIOC, 0x2
#define PORTC_03 GPIOC, 0x3
#define PORTC_04 GPIOC, 0x4
#define PORTC_05 GPIOC, 0x5
#define PORTC_06 GPIOC, 0x6
#define PORTC_07 GPIOC, 0x7
#define PORTC_08 GPIOC, 0x8
#define PORTC_09 GPIOC, 0x9
#define PORTC_10 GPIOC, 0xA
#define PORTC_11 GPIOC, 0xB
#define PORTC_12 GPIOC, 0xC
#define PORTC_13 GPIOC, 0xD
#define PORTC_14 GPIOC, 0xE
#define PORTC_15 GPIOC, 0xF

#define PORTD_00 GPIOD, 0x0
#define PORTD_01 GPIOD, 0x1
#define PORTD_02 GPIOD, 0x2
#define PORTD_03 GPIOD, 0x3
#define PORTD_04 GPIOD, 0x4
#define PORTD_05 GPIOD, 0x5
#define PORTD_06 GPIOD, 0x6
#define PORTD_07 GPIOD, 0x7
#define PORTD_08 GPIOD, 0x8
#define PORTD_09 GPIOD, 0x9
#define PORTD_10 GPIOD, 0xA
#define PORTD_11 GPIOD, 0xB
#define PORTD_12 GPIOD, 0xC
#define PORTD_13 GPIOD, 0xD
#define PORTD_14 GPIOD, 0xE
#define PORTD_15 GPIOD, 0xF

/***************************************************************************************************
 *                                       END OF FILE
 **************************************************************************************************/
