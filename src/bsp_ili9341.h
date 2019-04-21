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
 *   File:          bsp_ili9341.h
 *   Description:   
 *
 ***************************************************************************************************
 *   History:       21.04.2019 - file created
 *
 **************************************************************************************************/

/*
Nucleo-64 pinout

LCD Shield | Arduino | Nucleo
D0          D8          PA9
D1          D9          PC7
D2          D10         PA6
D3          D11         PA7
D4          D4          PB5
D5          D13         PA5
D6          D6          PB10
D7          D7          PA8
RS          A2          PA4
RD          A0          PA0
WR          A1          PA1
CS          A3          PB0
*/

/***************************************************************************************************
 *                                      INCLUDED FILES
 **************************************************************************************************/

#include "RTE_Components.h"
#include CMSIS_device_header

#include <stdint.h>
#include <stdio.h>
#include "GPIO_STM32F10x.h"

/***************************************************************************************************
 *                                       DEFINITIONS
 **************************************************************************************************/

//-- lcd depend defines

#define LCD_WIDTH       240
#define LCD_HEIGHT      320

#define ILI9341_RESET                0x01
#define ILI9341_SLEEP_OUT            0x11
#define ILI9341_GAMMA                0x26
#define ILI9341_DISPLAY_OFF          0x28
#define ILI9341_DISPLAY_ON           0x29
#define ILI9341_COLUMN_ADDR          0x2A
#define ILI9341_PAGE_ADDR            0x2B
#define ILI9341_GRAM                 0x2C
#define ILI9341_MAC                  0x36
#define ILI9341_PIXEL_FORMAT         0x3A
#define ILI9341_WDB                  0x51
#define ILI9341_WCD                  0x53
#define ILI9341_RGB_INTERFACE        0xB0
#define ILI9341_FRC                  0xB1
#define ILI9341_BPC                  0xB5
#define ILI9341_DFC                  0xB6
#define ILI9341_POWER1               0xC0
#define ILI9341_POWER2               0xC1
#define ILI9341_VCOM1                0xC5
#define ILI9341_VCOM2                0xC7
#define ILI9341_POWERA               0xCB
#define ILI9341_POWERB               0xCF
#define ILI9341_PGAMMA               0xE0
#define ILI9341_NGAMMA               0xE1
#define ILI9341_DTCA                 0xE8
#define ILI9341_DTCB                 0xEA
#define ILI9341_POWER_SEQ            0xED
#define ILI9341_3GAMMA_EN            0xF2
#define ILI9341_INTERFACE            0xF6
#define ILI9341_PRC                  0xF7

/***************************************************************************************************
 *                                      PUBLIC TYPES
 **************************************************************************************************/

typedef struct
{
    uint16_t x1, y1, x2, y2;
} rect_t;

typedef uint32_t lcd_bmp_size_t;
typedef uint16_t lcd_color_t;

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
 *                              PUBLIC FUNCTION PROTOTYPES
 **************************************************************************************************/

void lcd_send_data(const uint8_t * _data, const lcd_bmp_size_t _size);
void lcd_send_cmd(const uint8_t * _data);
void lcd_fill_rect(const rect_t * _rect, const lcd_color_t _color);
void lcd_draw_bmp(const rect_t * _rect, const lcd_color_t * _bmp);
void lcd_init(void);

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

/***************************************************************************************************
 *                                       END OF FILE
 **************************************************************************************************/
