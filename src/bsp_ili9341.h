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

#pragma once

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

#include <stdint.h>
#include <stdio.h>

/***************************************************************************************************
 *                                       DEFINITIONS
 **************************************************************************************************/

namespace ili9341
{

//-- lcd depend defines

extern const uint16_t LCD_WIDTH ;
extern const uint16_t LCD_HEIGHT;

extern const uint8_t ILI9341_RESET        ;
extern const uint8_t ILI9341_SLEEP_OUT    ;
extern const uint8_t ILI9341_GAMMA        ;
extern const uint8_t ILI9341_DISPLAY_OFF  ;
extern const uint8_t ILI9341_DISPLAY_ON   ;
extern const uint8_t ILI9341_COLUMN_ADDR  ;
extern const uint8_t ILI9341_PAGE_ADDR    ;
extern const uint8_t ILI9341_GRAM         ;
extern const uint8_t ILI9341_MAC          ;
extern const uint8_t ILI9341_PIXEL_FORMAT ;
extern const uint8_t ILI9341_WDB          ;
extern const uint8_t ILI9341_WCD          ;
extern const uint8_t ILI9341_RGB_INTERFACE;
extern const uint8_t ILI9341_FRC          ;
extern const uint8_t ILI9341_BPC          ;
extern const uint8_t ILI9341_DFC          ;
extern const uint8_t ILI9341_POWER1       ;
extern const uint8_t ILI9341_POWER2       ;
extern const uint8_t ILI9341_VCOM1        ;
extern const uint8_t ILI9341_VCOM2        ;
extern const uint8_t ILI9341_POWERA       ;
extern const uint8_t ILI9341_POWERB       ;
extern const uint8_t ILI9341_PGAMMA       ;
extern const uint8_t ILI9341_NGAMMA       ;
extern const uint8_t ILI9341_DTCA         ;
extern const uint8_t ILI9341_DTCB         ;
extern const uint8_t ILI9341_POWER_SEQ    ;
extern const uint8_t ILI9341_3GAMMA_EN    ;
extern const uint8_t ILI9341_INTERFACE    ;
extern const uint8_t ILI9341_PRC          ;

/***************************************************************************************************
 *                                      PUBLIC TYPES
 **************************************************************************************************/

typedef struct
{
    uint16_t x1, y1, x2, y2;
} rect_t;

typedef uint32_t bmp_size_t;
typedef uint16_t color_t;

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

bmp_size_t get_data_size(const rect_t * _rect);
void send_data(const uint8_t * _data, const bmp_size_t _size);
void send_cmd(const uint8_t * _data);
void set_rect(const rect_t * _rect);
void fill_rect(const rect_t * const _rect, const color_t _color);
void draw_bmp(const rect_t * _rect, const color_t * _bmp);
void init(void);

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

}; // namespace ili9341

/***************************************************************************************************
 *                                       END OF FILE
 **************************************************************************************************/
