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
 *   File:          bsp_ili9341.c
 *   Description:   see items: https://blablacode.ru/mikrokontrollery/490
 *                             http://we.easyelectronics.ru/lcd_gfx/shrifty-s-glcd-font-creator-na-kolenke.html
 *
 ***************************************************************************************************
 *   History:       13.04.2019 - file created
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

#include "bsp_ili9341.h"

/***************************************************************************************************
 *                                       DEFINITIONS
 **************************************************************************************************/

#define countof(a) (sizeof(a)/sizeof((a)[0]))

//-- board depend defines

#define LCD_PIN_D0 GPIOA, 0x09
#define LCD_PIN_D1 GPIOC, 0x07
#define LCD_PIN_D2 GPIOB, 0x06
#define LCD_PIN_D3 GPIOA, 0x07
#define LCD_PIN_D4 GPIOB, 0x05
#define LCD_PIN_D5 GPIOA, 0x05
#define LCD_PIN_D6 GPIOB, 0x0A
#define LCD_PIN_D7 GPIOA, 0x08
#define LCD_PIN_RS GPIOA, 0x04
#define LCD_PIN_RD GPIOA, 0x00
#define LCD_PIN_WR GPIOA, 0x01
#define LCD_PIN_CS GPIOB, 0x00

#define LCD_CMD_WR  (1 << 9 | 1 << 11)
#define LCD_DATA_WR (1 << 8 | 1 <<  9 | 1 << 11)
#define LCD_DATA_RD (1 << 8 | 1 << 10 | 1 << 11)

/***************************************************************************************************
 *                                      PUBLIC TYPES
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
    uint8_t        pin;
} lcd_pins[] =
{
    {LCD_PIN_D0},
    {LCD_PIN_D1},
    {LCD_PIN_D2},
    {LCD_PIN_D3},
    {LCD_PIN_D4},
    {LCD_PIN_D5},
    {LCD_PIN_D6},
    {LCD_PIN_D7},
    {LCD_PIN_RS},
    {LCD_PIN_RD},
    {LCD_PIN_WR},
    {LCD_PIN_CS},
};

GPIO_MODE data_mode;

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

static void _delay(uint8_t _tm)
{
    for (; _tm > 0; _tm--)
        for (volatile uint32_t ct = 0xFFF; ct > 0; ct--)
            __asm("nop");
}

void _lcd_hw_init(void)
{
    for (uint8_t i = 0; i < countof(lcd_pins); i++)
        GPIO_PinConfigure(lcd_pins[i].port, lcd_pins[i].pin, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT2MHZ);
    
    GPIO_PinWrite(LCD_PIN_RD, 1);
    GPIO_PinWrite(LCD_PIN_WR, 1);
    GPIO_PinWrite(LCD_PIN_RS, 0);
    GPIO_PinWrite(LCD_PIN_CS, 0);
}

static void _lcd_set(uint16_t _data)
{
    if (data_mode != GPIO_MODE_OUT2MHZ)
    {
        for (uint8_t i = 0; i < 8; i++)
            GPIO_PinConfigure(lcd_pins[i].port, lcd_pins[i].pin, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT2MHZ);
        
        data_mode = GPIO_MODE_OUT2MHZ;
    }
    
    for (uint8_t i = 0; i < countof(lcd_pins); i++, _data >>= 1)
    {
        GPIO_PinWrite(lcd_pins[i].port, lcd_pins[i].pin, _data & 1);
    }
    
    GPIO_PinWrite(LCD_PIN_WR, 1);
}

static uint8_t _lcd_get(void)
{
    uint8_t result;
    
    GPIO_PinWrite(LCD_PIN_RS, 1);
    GPIO_PinWrite(LCD_PIN_RD, 0);
    
    if (data_mode != GPIO_MODE_INPUT)
    {
        for (uint8_t i = 0; i < 8; i++)
            GPIO_PinConfigure(lcd_pins[i].port, lcd_pins[i].pin, GPIO_IN_PULL_UP, GPIO_MODE_INPUT);
        
        data_mode = GPIO_MODE_INPUT;
    }

    GPIO_PinWrite(LCD_PIN_RD, 1);
    
    for (uint8_t i = 0; i < 8; i++)
    {
        result <<= 1;
        result |= GPIO_PinRead(lcd_pins[i].port, lcd_pins[i].pin);
    }
    
    return result;
}

/***************************************************************************************************
 *                                    PUBLIC FUNCTIONS
 **************************************************************************************************/

void lcd_send_data(const uint8_t * _data, const lcd_bmp_size_t _size)
{
    for (lcd_bmp_size_t i = 0; i < _size; i++)
    {
        _lcd_set(LCD_DATA_WR | _data[i]);
    }
}

void lcd_send_cmd(const uint8_t * _data)
{
    if (_data != NULL && _data[0] > 0)
    {
        _lcd_set(LCD_CMD_WR | _data[1]);
        
        if (_data[0] > 1)
        {
            lcd_send_data(&_data[2], _data[0] - 1);
        }
    }
}

void lcd_set_rect(const rect_t * _rect)
{
    static uint8_t column[6] = {5, ILI9341_COLUMN_ADDR};
    static uint8_t row   [6] = {5, ILI9341_PAGE_ADDR};
    
    if (_rect == NULL) return;
    
    column[2] = _rect->x1 >> 8;
    column[3] = _rect->x1 & 0xFF;
    column[4] = _rect->x2 >> 8;
    column[5] = _rect->x2 & 0xFF;
    
    row[2] = _rect->y1 >> 8;
    row[3] = _rect->y1 & 0xFF;
    row[4] = _rect->y2 >> 8;
    row[5] = _rect->y2 & 0xFF;
    
    lcd_send_cmd(column);
    lcd_send_cmd(row);

    lcd_send_cmd((const uint8_t []){1, ILI9341_GRAM});
}

void lcd_fill_rect(const rect_t * _rect, const lcd_color_t _color)
{
    lcd_set_rect(_rect);
    
    for(lcd_bmp_size_t i = (lcd_bmp_size_t)(_rect->x2 - _rect->x1) * (_rect->y2 - _rect->y1);
        i > 0; i--)
    {
        lcd_send_data((uint8_t *)&_color, sizeof(_color));
    }
}

void lcd_draw_bmp(const rect_t * _rect, const lcd_color_t * _bmp)
{
    lcd_set_rect(_rect);
    
    lcd_send_data((uint8_t *)_bmp,
                  (lcd_bmp_size_t)(_rect->x2 - _rect->x1) *
                                  (_rect->y2 - _rect->y1) *
                                  sizeof(lcd_color_t)
                  );
}

void lcd_init(void)
{
    _lcd_hw_init();
    _delay(100);
    
    lcd_send_cmd((const uint8_t []){1, ILI9341_RESET});
    _delay(1);
    
    lcd_send_cmd((const uint8_t []){6 , ILI9341_POWERA      , 0x39, 0x2C, 0x00, 0x34, 0x02});
    lcd_send_cmd((const uint8_t []){4 , ILI9341_POWERB      , 0x00, 0xC1, 0x30});
    lcd_send_cmd((const uint8_t []){4 , ILI9341_DTCA        , 0x85, 0x00, 0x78});
    lcd_send_cmd((const uint8_t []){3 , ILI9341_DTCB        , 0x00, 0x00});
    lcd_send_cmd((const uint8_t []){5 , ILI9341_POWER_SEQ   , 0x64, 0x03, 0x12, 0x81});
    lcd_send_cmd((const uint8_t []){2 , ILI9341_PRC         , 0x20});
    lcd_send_cmd((const uint8_t []){2 , ILI9341_POWER1      , 0x23});
    lcd_send_cmd((const uint8_t []){2 , ILI9341_POWER2      , 0x10});
    lcd_send_cmd((const uint8_t []){3 , ILI9341_VCOM1       , 0x3E, 0x28});
    lcd_send_cmd((const uint8_t []){2 , ILI9341_VCOM2       , 0x86});

    //-- pixel orientation and draw direction
    lcd_send_cmd((const uint8_t []){2 , ILI9341_MAC         , 0x48});
    /* MY MX MV ML BGR MH 0 0
     *  0  1  0  0   1  0 0 0 - 0x48 - vertical orientation, top-left start point
     */

    lcd_send_cmd((const uint8_t []){2 , ILI9341_PIXEL_FORMAT, 0x55});
    lcd_send_cmd((const uint8_t []){3 , ILI9341_FRC         , 0x00, 0x18});
    lcd_send_cmd((const uint8_t []){4 , ILI9341_DFC         , 0x08, 0x82, 0x27});
    lcd_send_cmd((const uint8_t []){2 , ILI9341_3GAMMA_EN   , 0x00});
    lcd_send_cmd((const uint8_t []){5 , ILI9341_COLUMN_ADDR , 0x00, 0x00, 0x00, 0xEF});
    lcd_send_cmd((const uint8_t []){5 , ILI9341_PAGE_ADDR   , 0x00, 0x00, 0x01, 0x3F});
    lcd_send_cmd((const uint8_t []){2 , ILI9341_GAMMA       , 0x01});
    lcd_send_cmd((const uint8_t []){16, ILI9341_PGAMMA      , 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00});
    lcd_send_cmd((const uint8_t []){16, ILI9341_NGAMMA      , 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F});
    lcd_send_cmd((const uint8_t []){1 , ILI9341_SLEEP_OUT   });
    _delay(1);

    lcd_send_cmd((const uint8_t []){1 , ILI9341_DISPLAY_ON});
    _delay(1);
}

/***************************************************************************************************
 *                                       END OF FILE
 **************************************************************************************************/
