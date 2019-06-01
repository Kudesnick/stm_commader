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
#include "ff.h"
#include "bsp_gpio.h"
#include "cpp_terminal.h"
#include "cpp_font.h"

extern "C"
{
#include "GPIO_STM32F10x.h"
}

/***************************************************************************************************
 *                                       DEFINITIONS
 **************************************************************************************************/

#define LED2 GPIOA, 0x5

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
#if (0)
    // Проверка того, что проект запустился (мигаем светодиодом на борде)
    GPIO_PinConfigure(LED2, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT2MHZ);
    for (uint8_t i = 1; i <= 6; i++)
    {
        GPIO_PinWrite(LED2, i & 1);
        for (volatile uint32_t i = 0; i < 0xFFFFF; i++){};
    }
#endif

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

    ili9341::init();

    term.set_brush(brush);
    term.clear();
    term.print(demo_str);

for(;;){};
    
    // Тест флешки // see http://we.easyelectronics.ru/aliaksei/stm32f103-i-fatfs-nachinayuschim.html
    static char buff[1024];             // буфер для чтения/записи

    FRESULT result;

    // смонтировать диск
    FATFS FATFS_Obj;

    result = f_mount(&FATFS_Obj, "0", 1);
    if (result != FR_OK)
    {
            //printf("Ошибка монтирования диска %d\r\n", result);
    }
    else
    {
        // считаем файлы в корневом каталоге
        DIR dir;
        FILINFO fileInfo;
        int nFiles = 0;
    
        result = f_opendir(&dir, "/");
        if (result == FR_OK)
        {
                while (((result = f_readdir(&dir, &fileInfo)) == FR_OK) && fileInfo.fname[0])
                {
                        nFiles++;
                }
        }
        f_closedir(&dir);
    
    
        // открываем файл readme.txt для чтения
        FIL file;
        UINT nRead, nWritten;
    
        result = f_open(&file, "readme.txt", FA_OPEN_EXISTING | FA_READ);
        if (result == FR_OK)
        {
                f_read(&file, &buff, 1024, &nRead);
                f_close(&file);
        }
    
        // создаем файл write.txt
        result = f_open(&file, "write.txt", FA_CREATE_ALWAYS | FA_WRITE);
        if (result == FR_OK)
        {
                f_write(&file, &buff, nRead, &nWritten);
                f_close(&file);

//                font::courier_new.draw(16, 8, '+');
        }
        else
        {
//            font::courier_new.draw(16, 8, '-');
        }
    }
    
    for(;;);

}

/***************************************************************************************************
 *                                       END OF FILE
 **************************************************************************************************/
