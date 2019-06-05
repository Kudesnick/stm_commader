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

/***************************************************************************************************
 *                                      INCLUDED FILES
 **************************************************************************************************/

#include <stdint.h>
#include <stdio.h>

/***************************************************************************************************
 *                                       DEFINITIONS
 **************************************************************************************************/

/***************************************************************************************************
 *                                      PUBLIC TYPES
 **************************************************************************************************/

namespace ili9341
{

typedef struct
{
    uint16_t x1, y1, x2, y2;
} rect_t;

typedef uint32_t bmp_size_t;
typedef uint16_t color_t;

typedef enum : uint8_t
{
    ROT_0,
    ROT_1,
    ROT_2,
    ROT_3,
    ROT_4,
    ROT_5,
    ROT_6,
    ROT_7,
} rot_t;

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

bmp_size_t get_pixel_cnt(const rect_t * _rect);
void send_data(const uint8_t * _data, const bmp_size_t _size);
void get_data(uint8_t * _data, const bmp_size_t _size);
void send_cmd(const uint8_t * _data);
void set_rect(const rect_t * _rect, const bool _to_read = false);
void fill_rect(const rect_t * const _rect, const color_t _color);
void draw_bmp(const rect_t * _rect, const color_t * _bmp);
void init(void);
void scroll(const uint16_t);

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