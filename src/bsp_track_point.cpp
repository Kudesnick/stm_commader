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
 *   File:          bsp_track_point.cpp
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

#include "bsp_track_point.h"
#include "bsp_gpio.h"

#include "RTE_Components.h"
#include CMSIS_device_header

extern "C"
{
#include "GPIO_STM32F10x.h"
};

using namespace track_point;

/***************************************************************************************************
 *                                       DEFINITIONS
 **************************************************************************************************/

#define KEY_UP_PIN      PORTA_11
#define KEY_DOWN_PIN    PORTB_01
#define KEY_LEFT_PIN    PORTB_11
#define KEY_RIGHT_PIN   PORTB_02
#define KEY_CENTER_PIN  PORTA_12

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
    fn_event_t events[KEY_EVENT_CNT];
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
        key_event_t curr = GPIO_PinRead(pins[i].port, pins[i].pin) ? KEY_POP : KEY_PUSH;
        
        if (keys[i].prev_status != curr)
        {
            if (keys[i].events[curr] != NULL)
            {
                keys[i].events[curr](static_cast<key_t>(i), curr); // KEY_POP, KEY_PUSH
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
                        keys[i].events[event](static_cast<key_t>(i), event); // KEY_DOUBLE_CLICK, KEY_CLICK
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
                    keys[i].events[event](static_cast<key_t>(i), event); // KEY_DBL_LONG_PRESS, KEY_LONG_PRESS
                }

                keys[i].long_flag = true;
            }
        }
    }
};

extern "C" void SysTick_Handler(void)
{
    _keyscan();
};

/***************************************************************************************************
 *                                    PUBLIC FUNCTIONS
 **************************************************************************************************/

namespace track_point
{

void init(void)
{
    for (uint8_t i = 0; i < KEY_CNT; i++)
    {
        GPIO_PinConfigure(pins[i].port, pins[i].pin, GPIO_IN_PULL_UP, GPIO_MODE_INPUT);
    }

    RCC_ClocksTypeDef RCC_Clocks;

    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.HCLK_Frequency / (1000 * TIME_SCAN));
};

void callback_init(key_t _key_num, key_event_t _event, fn_event_t _func)
{
    if (_key_num < KEY_CNT && _event < KEY_EVENT_CNT)
    {
        keys[_key_num].events[_event] = _func;
    }
};

}; // namespace track_point

/***************************************************************************************************
 *                                       END OF FILE
 **************************************************************************************************/
