/***************************************************************************************************
 *   Project:       stm commander
 *   Author:        Stulov Tikhon (kudesnick@inbox.ru)
 ***************************************************************************************************
 *   Distribution:  
 *
 ***************************************************************************************************
 *   MCU Family:    STM32F
 *   Compiler:      ARMCC
 ***************************************************************************************************
 *   File:          bsp_keyboard.c
 *   Description:   
 *
 ***************************************************************************************************
 *   History:       02.05.2019 - file created
 *
 **************************************************************************************************/

/***************************************************************************************************
 *                                      INCLUDED FILES
 **************************************************************************************************/

#include <stdint.h>
#include <string.h>

#include "bsp_keyboard.h"

#include "RTE_Components.h"
#include CMSIS_device_header
#include "GPIO_STM32F10x.h"

/***************************************************************************************************
 *                                       DEFINITIONS
 **************************************************************************************************/

#define KEY_UP_PIN     GPIOA, 0x06
#define KEY_DOWN_PIN   GPIOB, 0x01
#define KEY_LEFT_PIN   GPIOA, 0x07
#define KEY_RIGHT_PIN  GPIOB, 0x02
#define KEY_CENTER_PIN GPIOA, 0x05

#define TIME_SCAN 25
#define TIME_DBL  500
#define TIME_LONG 500

/***************************************************************************************************
 *                                      PRIVATE TYPES
 **************************************************************************************************/


/***************************************************************************************************
 *                               PRIVATE FUNCTION PROTOTYPES
 **************************************************************************************************/

/***************************************************************************************************
 *                                       PRIVATE DATA
 **************************************************************************************************/

const struct
{
    GPIO_TypeDef * port;
    uint32_t        pin;
} pins[] =
{
    {KEY_UP_PIN    },
    {KEY_DOWN_PIN  },   
    {KEY_LEFT_PIN  },   
    {KEY_RIGHT_PIN },   
    {KEY_CENTER_PIN},   
};

struct
{
    uint64_t timer; // Таймер для отсчета времени между любыми событиями
    uint64_t push_timer; // Таймер для отсчета времени между событиями KEY_PUSH
    key_event_t prev_status;
    bool dbl_flag;
    bool long_flag;
    void(* events[KEY_EVENT_CNT])(const key_event_t, const key_t);
} keys[KEY_CNT];

/***************************************************************************************************
 *                                       PUBLIC DATA
 **************************************************************************************************/

/***************************************************************************************************
 *                                      EXTERNAL DATA
 **************************************************************************************************/

/***************************************************************************************************
 *                              EXTERNAL FUNCTION PROTOTYPES
 **************************************************************************************************/

/***************************************************************************************************
 *                                    PRIVATE FUNCTIONS
 **************************************************************************************************/

static void _keyscan(void)
{
    static uint64_t timer = 0;
    
    timer += TIME_SCAN;
    
    for (uint8_t i = 0; i < KEY_CNT; i++)
    {
        key_event_t curr = GPIO_ReadInputDataBit(pins[i].port, pins[i].pin) ? KEY_POP : KEY_PUSH;
        
        if (keys[i].prev_status != curr)
        {
            if (keys[i].events[curr] != NULL)
            {
                keys[i].events[curr](curr, i); // KEY_POP, KEY_PUSH
            }
            
            if (curr == KEY_PUSH)
            {
                keys[i].dbl_flag = ((timer - keys[i].push_timer) < TIME_DBL);
                
                keys[i].push_timer = timer;
            }
            else /* if (curr == KEY_POP) */
            {
                if ((timer - keys[i].timer) < TIME_LONG)
                {
                    key_event_t event = keys[i].dbl_flag ? KEY_DOUBLE_CLICK : KEY_CLICK;
                    keys[i].dbl_flag = false;
                    
                    if (keys[i].events[event] != NULL)
                    {
                        keys[i].events[event](event, i); // KEY_DOUBLE_CLICK, KEY_CLICK
                    }
                }
            }
            
            keys[i].timer = timer;
            keys[i].prev_status = curr;
            keys[i].long_flag = false;
        }
        else
        {
            if (   curr == KEY_PUSH
                && (timer - keys[i].timer) >= TIME_LONG
                && keys[i].long_flag == false
                )
            {
                key_event_t event = keys[i].dbl_flag ? KEY_DBL_LONG_PRESS : KEY_LONG_PRESS;
                keys[i].dbl_flag = false;
                
                if (keys[i].events[event] != NULL)
                {
                    keys[i].events[event](event, i); // KEY_DBL_LONG_PRESS, KEY_LONG_PRESS
                }

                keys[i].long_flag = true;
            }
        }
    }
};

void SysTick_Handler(void)
{
    _keyscan();
};

/***************************************************************************************************
 *                                    PUBLIC FUNCTIONS
 **************************************************************************************************/

void bsp_keyboard_init(void)
{
    RCC_ClocksTypeDef RCC_Clocks;

    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000 * TIME_SCAN);

    for (uint8_t i = 0; i < KEY_CNT; i++)
    {
        GPIO_PinConfigure(pins[i].port, pins[i].pin, GPIO_IN_PULL_UP, GPIO_MODE_INPUT);
    }
};

void bsp_callback_init(key_t _key_num, key_event_t _event, void(* _func)(const key_event_t, const key_t))
{
    if (_key_num < KEY_CNT && _event < KEY_EVENT_CNT)
    {
        keys[_key_num].events[_event] = _func;
    }
}

/***************************************************************************************************
 *                                       END OF FILE
 **************************************************************************************************/
