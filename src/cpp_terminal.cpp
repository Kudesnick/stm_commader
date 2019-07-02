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

#include <stdlib.h>

#include "cpp_terminal.h"
#include "bsp_ili9341.h"

/***************************************************************************************************
 *                                       DEFINITIONS
 **************************************************************************************************/

#define TAB_SIZE 4

/***************************************************************************************************
 *                                      PUBLIC TYPES
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


cpp_terminal::cpp_terminal(
    const uint16_t _x,
    const uint16_t _y,
    const uint8_t _height,
    const uint8_t _width,
    const font::font_t& _font,
    char * const _buffer,
    uint16_t _buf_size):
    
    win_crd_({_x, _y}),
    win_size_({_height, _width}),
    cpp_font(_font),
    p_buf_(_buffer),
    buf_size_(_buf_size)
{
    // clear();
};


void cpp_terminal::rect_calc_of_str_(const uint8_t _n, ili9341::rect_t &_rect)
{
    _rect.x1 = _n * font_->attr.height_glyph;
    _rect.y1 = 0;
    _rect.x2 = _rect.x1 + font_->attr.height_glyph - 1;
    _rect.y2 = win_size_.y * font_->attr.width_glyph - 1;
};

void cpp_terminal::rect_calc_of_col_(const uint8_t _n, ili9341::rect_t &_rect)
{
    _rect.x1 = 0;
    _rect.y1 = _n * font_->attr.width_glyph;
    _rect.x2 = win_size_.x * font_->attr.height_glyph - 1;
    _rect.y2 = _rect.y1 + font_->attr.width_glyph - 1;
};


void cpp_terminal::clear(void)
{
    buf_ptr_ = scroll_ptr_ = 0;
    cursor_ = {0, 0};
    
    ili9341::rect_t rect =
    {
        .x1 = win_crd_.x,
        .y1 = win_crd_.y,
        .x2 = win_crd_.x,
        .y2 = win_crd_.y,
    };
    
    rect.x2 += win_size_.x * font_->attr.height_glyph - 1;
    rect.y2 += win_size_.y * font_->attr.width_glyph - 1;
    
    ili9341::fill_rect(&rect, color_converter(brush_.bg));
};

void cpp_terminal::print(const char * const _str)
{
    if (_str == NULL) return;
    
    const char * str = _str;
    uint16_t len = strlen(_str);
    
    while(len > 0)
    {
        uint8_t print_len = 1;
        
        switch (str[0])
        {
            case '\a': // BELL
            {
                // TO_DO beep
            }
            break;
            
            case '\b': // BACKSPACE
            {
                if (cursor_.y > 0)
                {
                    cursor_.y--;
                }
                else if (cursor_.x > 0)
                {
                    cursor_.x--;
                    cursor_.y = win_size_.y - 1;
                }
                
                draw(win_crd_.x + cursor_.x * font_->attr.height_glyph,
                    win_crd_.y + cursor_.y * font_->attr.width_glyph,
                    " ", 1);
            }
            break;
                
            case '\t': // CHARACTER TABULATION 
            {
                cursor_.y = cursor_.y / TAB_SIZE + 1;
                cursor_.y *= TAB_SIZE;
            }
            break;

            case '\n': // LINE FEED
            case '\v': // LINE TABULATION 
            {
                cursor_.x++;
            }
            break;
            
            case '\f': // FORM FEED
            {
                clear();
            }
            break;

            case '\r': // CARRIAGE RETURN
            {
                cursor_.y = 0;
            }
            break;
                        
            default:
            {
                print_len = win_size_.y - cursor_.y;
                
                print_len = draw(win_crd_.x + cursor_.x * font_->attr.height_glyph,
                    win_crd_.y + cursor_.y * font_->attr.width_glyph,
                    str, print_len);
                
                // Перевод курсора
                cursor_.y += print_len;
                
                // Если ничего не напечатали - значит непечатный символ, который не был обработан выше
                if (print_len == 0) print_len = 1;
            }
            break;
        }
        
        // Переводим указатель
        str += print_len;
        len -= print_len;

        if (cursor_.y >= win_size_.y)
        {
            cursor_.y = 0;
            cursor_.x++;
        }

        if (cursor_.x >= win_size_.x)
        {
            // TO_DO реализовать прокручивание экрана
        }
        
        // TO_DO реализовать работу с буфером
    };
};

void cpp_terminal::print_int(uint32_t _d)
{
    char str[11] = "4294967295";
    uint8_t ptr = sizeof(str) - 1;
    
    if (_d == 0)
    {
        str[--ptr] = '0';
    }
    else
    {
        for (; _d != 0; _d /= 10)
        {
            str[--ptr] = _d % 10 + '0';
        }
    }
    
    print(&str[ptr]);
}

void cpp_terminal::set_brush(const font::brush_t &_brush)
{
    brush_ = _brush;
}

void cpp_terminal::scroll(int16_t _x, int16_t _y, const bool _cycle)
{
    ili9341::rect_t rect;
    uint8_t * curr_buf = NULL;
    uint8_t * first_buf = NULL;
    
    if (_x != 0)
    {
        rect_calc_of_str_(0, rect);
        uint16_t buf_size = ili9341::get_pixel_cnt(&rect) * sizeof(ili9341::color_t);
        curr_buf = static_cast<uint8_t *>(malloc(buf_size));
        
        if (_cycle)
        {
            first_buf = static_cast<uint8_t *>(malloc(buf_size));
        }
    
        while (_x != 0)
        {
            // Копируем первую строку
            if (_cycle)
            {
                rect_calc_of_str_((_x > 0) ? 0 : win_size_.x - 1, rect);
                
                ili9341::set_rect(&rect, true);
                ili9341::get_data(first_buf, buf_size);
            }
    
            if (_x > 0)
            {
                for (uint8_t i = 1; i < win_size_.x; i++)
                {
                    rect_calc_of_str_(i, rect);
                    ili9341::set_rect(&rect, true);
                    ili9341::get_data(curr_buf, buf_size);
                    rect_calc_of_str_(i - 1, rect);
                    ili9341::set_rect(&rect, false);
                    ili9341::send_data(curr_buf, buf_size);
                }
            }
            
            if (_x < 0)
            {
                for (auto i = win_size_.x - 2; i >= 0; i--)
                {
                    rect_calc_of_str_(i, rect);
                    ili9341::set_rect(&rect, true);
                    ili9341::get_data(curr_buf, buf_size);
                    rect_calc_of_str_(i + 1, rect);
                    ili9341::set_rect(&rect, false);
                    ili9341::send_data(curr_buf, buf_size);
                }
            }
            
            rect_calc_of_str_((_x < 0) ? 0 : win_size_.x - 1, rect);
            
            if (first_buf != NULL)
            {
                ili9341::set_rect(&rect, false);
                ili9341::send_data(first_buf, buf_size);
            }
            else
            {
                ili9341::fill_rect(&rect, font::color_converter(brush_.bg));
            }
            
            if (_x > 0) _x--;
            if (_x < 0) _x++;
        }
            
        if (curr_buf  != NULL) free(curr_buf);
        if (first_buf != NULL) free(first_buf);
    }
    
    if (_y != 0)
    {
        rect_calc_of_col_(0, rect);
        uint16_t buf_size = ili9341::get_pixel_cnt(&rect) * sizeof(ili9341::color_t);
        curr_buf = static_cast<uint8_t *>(malloc(buf_size));
        
        if (_cycle)
        {
            first_buf = static_cast<uint8_t *>(malloc(buf_size));
        }
    
        while (_y != 0)
        {
            // Копируем первую строку
            if (_cycle)
            {
                rect_calc_of_col_((_y > 0) ? 0 : win_size_.y - 1, rect);
                
                ili9341::set_rect(&rect, true);
                ili9341::get_data(first_buf, buf_size);
            }
    
            if (_y > 0)
            {
                for (uint8_t i = 1; i < win_size_.y; i++)
                {
                    rect_calc_of_col_(i, rect);
                    ili9341::set_rect(&rect, true);
                    ili9341::get_data(curr_buf, buf_size);
                    rect_calc_of_col_(i - 1, rect);
                    ili9341::set_rect(&rect, false);
                    ili9341::send_data(curr_buf, buf_size);
                }
            }
            
            if (_y < 0)
            {
                for (auto i = win_size_.y - 2; i >= 0; i--)
                {
                    rect_calc_of_col_(i, rect);
                    ili9341::set_rect(&rect, true);
                    ili9341::get_data(curr_buf, buf_size);
                    rect_calc_of_col_(i + 1, rect);
                    ili9341::set_rect(&rect, false);
                    ili9341::send_data(curr_buf, buf_size);
                }
            }
            
            rect_calc_of_col_((_y < 0) ? 0 : win_size_.y - 1, rect);
            
            if (first_buf != NULL)
            {
                ili9341::set_rect(&rect, false);
                ili9341::send_data(first_buf, buf_size);
            }
            else
            {
                ili9341::fill_rect(&rect, font::color_converter(brush_.bg));
            }
            
            if (_y > 0) _y--;
            if (_y < 0) _y++;
        }
            
        if (curr_buf  != NULL) free(curr_buf);
        if (first_buf != NULL) free(first_buf);
    }
}

/***************************************************************************************************
 *                                       END OF FILE
 **************************************************************************************************/
