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
 *   File:          main.c
 *   Description:   
 *
 ***************************************************************************************************
 *   History:       13.04.2019 - file created
 *
 **************************************************************************************************/

/***************************************************************************************************
 *                                      INCLUDED FILES
 **************************************************************************************************/

#include "bsp_track_point.h"
#include "bsp_ili9341.h"
#include "cpp_terminal.h"
#include "cpp_font.h"

#include "cpp_os.h"
#include "misc_macro.h"

extern "C"
{
#include "Board_LED.h"
}

/***************************************************************************************************
 *                                       DEFINITIONS
 **************************************************************************************************/

/***************************************************************************************************
 *                                    PUBLIC FUNCTIONS
 **************************************************************************************************/

const char * const demo_str = 
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

static terminal::cpp_terminal term = {0, 0, 30, 40, font::zx};

static font::brush_t brush = {font::GREEN, font::BLACK};

static void color_select(const track_point::key_t _key, const track_point::key_event_t _event)
{
    font::color_t curr_color = (_event != track_point::KEY_DOUBLE_CLICK) ? brush.bg : brush.txt;
    
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
            
            if (_event != track_point::KEY_DOUBLE_CLICK)
            {
                brush.bg = colors[i];
            }
            else
            {
                brush.txt = colors[i];
            }
            
            term.set_brush(brush);
            term.clear();
            term.print(demo_str);
            
            break;
        }
    }
};

static void move_text(const track_point::key_t _key, const track_point::key_event_t _event)
{
    auto x = 0, y = 0;
    bool cycle;
    
    switch(_key)
    {
        case track_point::KEY_UP   : x = -1; break;
        case track_point::KEY_DOWN : x =  1; break;
        case track_point::KEY_LEFT : y = -1; break;
        case track_point::KEY_RIGHT: y =  1; break;
        default: return;
    }
    
    switch(_event)
    {
        case track_point::KEY_CLICK     : cycle = true ; break;
        case track_point::KEY_LONG_PRESS: cycle = false; break;
        default: return;
    }
    
    term.scroll(x, y, cycle);
};

int main(void)
{
#if (1)
    // Проверка того, что проект запустился (мигаем светодиодом на борде)
    LED_Initialize();
    for (uint8_t i = 1; i <= 6; i++)
    {
        if (i & 1)
        {
            LED_On(0);
        }
        else
        {
            LED_Off(0);
        }
        for (volatile uint32_t i = 0; i < 0xFFFFF; i++){};
    }
#endif

    //-- display demo
    ili9341::init();

    term.set_brush(brush);
    term.clear();
    term.print(demo_str);

    //-- track point demo
    track_point::init();
    
    track_point::callback_init(track_point::KEY_UP   , track_point::KEY_CLICK, move_text);
    track_point::callback_init(track_point::KEY_DOWN , track_point::KEY_CLICK, move_text);
    track_point::callback_init(track_point::KEY_LEFT , track_point::KEY_CLICK, move_text);
    track_point::callback_init(track_point::KEY_RIGHT, track_point::KEY_CLICK, move_text);

    track_point::callback_init(track_point::KEY_UP   , track_point::KEY_LONG_PRESS, move_text);
    track_point::callback_init(track_point::KEY_DOWN , track_point::KEY_LONG_PRESS, move_text);
    track_point::callback_init(track_point::KEY_LEFT , track_point::KEY_LONG_PRESS, move_text);
    track_point::callback_init(track_point::KEY_RIGHT, track_point::KEY_LONG_PRESS, move_text);
    
    track_point::callback_init(track_point::KEY_CENTER, track_point::KEY_DOUBLE_CLICK, color_select);
    track_point::callback_init(track_point::KEY_CENTER, track_point::KEY_DBL_LONG_PRESS, color_select);

    cpp_os::create_os();
    
    BRK_PTR();

    for(;;){};

}

/***************************************************************************************************
 *                                       END OF FILE
 **************************************************************************************************/
