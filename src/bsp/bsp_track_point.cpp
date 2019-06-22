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
#include "cpp_storage.h"
#include "cpp_os.h"

/***************************************************************************************************
 *                                       DEFINITIONS
 **************************************************************************************************/

#define CW_ROT 180

#if (CW_ROT == 0)
    #define KEY_UP_PIN      PORTA_11
    #define KEY_RIGHT_PIN   PORTB_02
    #define KEY_DOWN_PIN    PORTB_01
    #define KEY_LEFT_PIN    PORTA_06
#elif (CW_ROT == 90)
    #define KEY_UP_PIN      PORTA_06
    #define KEY_RIGHT_PIN   PORTA_11
    #define KEY_DOWN_PIN    PORTB_02
    #define KEY_LEFT_PIN    PORTB_01
#elif (CW_ROT == 180)
    #define KEY_UP_PIN      PORTB_01
    #define KEY_RIGHT_PIN   PORTA_06
    #define KEY_DOWN_PIN    PORTA_11
    #define KEY_LEFT_PIN    PORTB_02
#elif (CW_ROT == 270)
    #define KEY_UP_PIN      PORTB_02
    #define KEY_RIGHT_PIN   PORTB_01
    #define KEY_DOWN_PIN    PORTA_06
    #define KEY_LEFT_PIN    PORTA_11
#else
    #error CW_ROT must be 0/90/180/270
#endif
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
    {KEY_RIGHT_PIN },   
    {KEY_DOWN_PIN  },   
    {KEY_LEFT_PIN  },   
    {KEY_CENTER_PIN},   
};

struct
{
    uint32_t timer; // Таймер для отсчета времени между любыми событиями
    uint32_t push_timer; // Таймер для отсчета времени между событиями KEY_PUSH
    key_event_t prev_status;
    bool dbl_flag;
    bool long_flag;
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

/***************************************************************************************************
 *                                    PUBLIC FUNCTIONS
 **************************************************************************************************/

class : public cpp_os_thread<>
{
private:
    void thread_func(void)
    {
        // Gpio init
        for (uint8_t i = 0; i < KEY_CNT; i++)
        {
            GPIO_IN_PUP_CONFIG(pins[i].port, pins[i].pin);
        }
        
        uint32_t timer = 0;
        
        for (uint32_t timer = 0; true; timer += TIME_SCAN)
        {
            cpp_os::delay(TIME_SCAN);
            
            for (uint8_t i = 0; i < KEY_CNT; i++)
            {
                key_event_t curr = GPIO_READ(pins[i].port, pins[i].pin) ? KEY_POP : KEY_PUSH;
                
                if (keys[i].prev_status != curr)
                {
                    mediator::btns[i].set(curr); // KEY_POP, KEY_PUSH
                    
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
                            
                            mediator::btns[i].set(event); // KEY_DOUBLE_CLICK, KEY_CLICK
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
                        
                        mediator::btns[i].set(event); // KEY_DBL_LONG_PRESS, KEY_LONG_PRESS
        
                        keys[i].long_flag = true;
                    }
                }
            }
        }
    };
} track_point = {};

/***************************************************************************************************
 *                                       END OF FILE
 **************************************************************************************************/
