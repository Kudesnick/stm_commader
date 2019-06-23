/***************************************************************************************************
 *   Project:       
 *   Author:        
 ***************************************************************************************************
 *   Distribution:  
 *
 ***************************************************************************************************
 *   MCU Family:    STM32F
 *   Compiler:      ARMCC
 ***************************************************************************************************
 *   File:          test_terminal.cpp
 *   Description:   
 *
 ***************************************************************************************************
 *   History:       23.06.2019 - file created
 *
 **************************************************************************************************/

/***************************************************************************************************
 *                                      INCLUDED FILES
 **************************************************************************************************/

#include <stdlib.h>

#include "test_terminal.h"
#include "cpp_os.h"
#include "cpp_terminal.h"
#include "cpp_font.h"
#include "cpp_storage.h"

#ifdef __cplusplus
    using namespace std;
#endif

/***************************************************************************************************
 *                                       DEFINITIONS
 **************************************************************************************************/

/***************************************************************************************************
 *                                      PRIVATE TYPES
 **************************************************************************************************/

/***************************************************************************************************
 *                               PRIVATE FUNCTION PROTOTYPES
 **************************************************************************************************/

/***************************************************************************************************
 *                                       PRIVATE DATA
 **************************************************************************************************/

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

class : public cpp_os_thread<>
{
private:
    cpp_terminal term_ = {0, 0, 30, 40, font::zx};

    const char * const demo_str_ = 
    "Соцсети, чаты.. Кто здесь? Я?\r\n"
    "Да, я! Бегу двоичным кодом\r\n"
    "По свитым в пары проводам,\r\n"
    "По магистралям световодов\r\n"
    "\r\n"
    "Я - ваш 3G и GPS,\r\n"
    "Сигнал на старт и остановку.\r\n"
    "Я весь ваш творческий процесс\r\n"
    "Cведу к нажатию на кнопку.\r\n"
    "\r\n"
    "\t#kudesnick\r\nwith love to Speccy and c++";
        
    font::brush_t brush_ = {font::GREEN, font::BLACK};    

    void _color_select(queue_message_t<key_event_t> &key)
    {
        if (key.id != &mediator::btns[KEY_CENTER]) return;
        
        if (key.val != KEY_DOUBLE_CLICK && key.val != KEY_DBL_LONG_PRESS) return;
        
        font::color_t curr_color = (key.val != KEY_DOUBLE_CLICK) ? brush_.bg : brush_.txt;
        
        const font::color_t colors[] = {
            font::BLACK     ,
            font::BLUE_D    ,
            font::BLUE      ,
            font::RED_D     ,
            font::RED       ,
            font::MAGENTA_D ,
            font::MAGENTA   ,
            font::GREEN_D   ,
            font::GREEN     ,
            font::CYAN_D    ,
            font::CYAN      ,
            font::YELLOW_D  ,
            font::YELLOW    ,
            font::WHITE_D   ,
            font::WHITE     ,
        };
        
        for (auto i = 0; i < sizeof(colors)/sizeof(colors[0]); i++)
        {
            if (colors[i] == curr_color)
            {
                if (++i >= sizeof(colors)/sizeof(colors[0])) i = 0;
                
                if (key.val != KEY_DOUBLE_CLICK)
                {
                    brush_.bg = colors[i];
                }
                else
                {
                    brush_.txt = colors[i];
                }
                
                term_.set_brush(brush_);
                term_.clear();
                term_.print(demo_str_);
                
                break;
            }
        }
    };
    
    void _move_text(queue_message_t<key_event_t> &key)
    {
        bool cycle;
        
        auto x = (key.id == &mediator::btns[KEY_UP   ]) ? -1 :
                 (key.id == &mediator::btns[KEY_DOWN ]) ?  1 : 0;
        auto y = (key.id == &mediator::btns[KEY_LEFT ]) ? -1 :
                 (key.id == &mediator::btns[KEY_RIGHT]) ?  1 : 0;

        if (x == 0 && y == 0) return;
        
        switch(key.val)
        {
            case KEY_CLICK     : cycle = true ; break;
            case KEY_LONG_PRESS: cycle = false; break;
            default: return;
        }
        
        term_.scroll(x, y, cycle);
    };
    
    subscriber_queue<key_event_t, 4> queue_;
    
    void thread_func(void)
    {
        //-- display demo
        ili9341::init();
        
        term_.set_brush(brush_);
        term_.clear();
        term_.print(demo_str_);
        
        for(queue_message_t<key_event_t> key; ; queue_.queue.get(&key, NULL, osWaitForever))
        {
            _move_text(key);
            _color_select(key);
        }
    };

} test_terminal = {};

/***************************************************************************************************
 *                                    PUBLIC FUNCTIONS
 **************************************************************************************************/

/***************************************************************************************************
 *                                       END OF FILE
 **************************************************************************************************/
