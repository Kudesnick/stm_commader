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

#include "cpp_font.h"
#include "courier_new.h"
#include "fnt8x8.h"
#include "fnt8x16.h"
#include "atari.h"
#include "zx.h"

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
    ili9341::init();

    const char str[] = "Hello word! Привет мир!";
    ili9341::fill_rect(NULL, 0xFF);
    font::courier_new.draw({16, 8, 27, 300}, "Hello word! Courier new");
    font::fnt8x8.draw     ({32, 8, 39, 300}, "Hello word! 8x8");
    font::fnt8x16.draw    ({48, 8, 63, 300}, "Hello word! 8x16");
    font::atari.draw      ({64, 8, 71, 300}, "Hello word! atari 400/400");
    font::zx.draw         ({80, 8, 87, 300}, "Hello word! spectrum ZX");

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

                font::courier_new.draw({16, 8, 27, 300}, "Hello word!");
        }
        else
        {
            font::courier_new.draw({16, 8, 27, 300}, "SD card error.");
        }
    }
    
    for(;;);

}

/***************************************************************************************************
 *                                       END OF FILE
 **************************************************************************************************/
