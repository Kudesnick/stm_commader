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
 *   File:          font.c
 *   Description:   
 *
 ***************************************************************************************************
 *   History:       21.04.2019 - file created
 *
 **************************************************************************************************/

/***************************************************************************************************
 *                                      INCLUDED FILES
 **************************************************************************************************/

#include "font.h"
#include "string.h"
#include "bsp_ili9341.h"

/***************************************************************************************************
 *                                       DEFINITIONS
 **************************************************************************************************/

/***************************************************************************************************
 *                                      PUBLIC TYPES
 **************************************************************************************************/

/***************************************************************************************************
 *                               PRIVATE FUNCTION PROTOTYPES
 **************************************************************************************************/

/***************************************************************************************************
 *                                       PRIVATE DATA
 **************************************************************************************************/

const font_t * font;
lcd_color_t background_cl;
lcd_color_t text_cl;

/***************************************************************************************************
 *                                       PUBLIC DATA
 **************************************************************************************************/

/***************************************************************************************************
 *                              PUBLIC FUNCTION PROTOTYPES
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

void font_set_attr(const font_t * _font, const lcd_color_t _background_cl, const lcd_color_t _text_cl)
{
    font = _font;
    background_cl = _background_cl;
    text_cl = _text_cl;
};

void font_draw_text(uint8_t _x, uint8_t _y, char * _str)
{
    if (font != NULL)
    {
        rect_t rect = {_x, _y, LCD_WIDTH, LCD_HEIGHT};
        uint8_t len = strlen(_str);
        lcd_bmp_size_t max_pixel_cnt = (lcd_bmp_size_t)(LCD_WIDTH - _x) * (LCD_HEIGHT - _y);

        lcd_bmp_size_t curr_pixel = 0;

        lcd_set_rect(&rect);

        for(uint8_t i = 0; i < len; i++)
        {
            if ((_str[i] >= font->start_code) && (_str[i] <= font->end_code))
            {
                const uint8_t * pixel_ptr = &(font->p_font[_str[i] - font->start_code]);
                uint8_t pixel_cnt = font->height_glyph * font->width_glyph;
                uint8_t curr_byte = *pixel_ptr;

                for (uint8_t j = 0; j < pixel_cnt; j++)
                {
                    if ((curr_byte & 1) != 0)
                    {
                        lcd_send_data((const uint8_t *)((curr_byte & 1) ? &text_cl : &background_cl),
                                      sizeof(lcd_color_t));
                    }

                    if (++curr_pixel >= max_pixel_cnt) return;

                    if ((j & 7) == 7)
                    {
                        curr_byte = *(++pixel_ptr);
                    }
                    else
                    {
                        curr_byte >>= 1;
                    }
                }
            }
        }
    }
};

/***************************************************************************************************
 *                                       END OF FILE
 **************************************************************************************************/
