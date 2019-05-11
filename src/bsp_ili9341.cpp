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
 *   File:          bsp_ili9341.cpp
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
#include "bsp_gpio.h"

#include "RTE_Components.h"
#include CMSIS_device_header

extern "C"
{
#include "GPIO_STM32F10x.h"
};

/***************************************************************************************************
 *                                       DEFINITIONS
 **************************************************************************************************/

#define countof(a) (sizeof(a)/sizeof((a)[0]))

//-- board depend defines

#define LCD_PIN_D0 PORTA_09
#define LCD_PIN_D1 PORTC_07
#define LCD_PIN_D2 PORTB_06
#define LCD_PIN_D3 PORTA_07
#define LCD_PIN_D4 PORTB_05
#define LCD_PIN_D5 PORTA_05
#define LCD_PIN_D6 PORTB_10
#define LCD_PIN_D7 PORTA_08
#define LCD_PIN_RS PORTA_04
#define LCD_PIN_RD PORTA_00
#define LCD_PIN_WR PORTA_01
#define LCD_PIN_CS PORTB_00

#define LCD_CMD_WR           (1 <<  9)
#define LCD_DATA_WR (1 << 8 | 1 <<  9)
#define LCD_DATA_RD (1 << 8 | 1 << 10)

#define LCD_WIDTH  240
#define LCD_HEIGHT 320

#define ILI9341_RESET         0x01
#define ILI9341_SLEEP_OUT     0x11
#define ILI9341_PTLON         0x12
#define ILI9341_NORON         0x13
#define ILI9341_GAMMA         0x26
#define ILI9341_DISPLAY_OFF   0x28
#define ILI9341_DISPLAY_ON    0x29
#define ILI9341_COLUMN_ADDR   0x2A
#define ILI9341_PAGE_ADDR     0x2B
#define ILI9341_RAMWR         0x2C
#define ILI9341_RAMRD         0x2E
#define ILI9341_VSCRDEF       0x33
#define ILI9341_MAC           0x36
#define ILI9341_VSCRSADD      0x37
#define ILI9341_IDMOFF        0x38
#define ILI9341_IDMON         0x39
#define ILI9341_PIXEL_FORMAT  0x3A
#define ILI9341_WDB           0x51
#define ILI9341_WCD           0x53
#define ILI9341_RGB_INTERFACE 0xB0
#define ILI9341_FRC           0xB1
#define ILI9341_BPC           0xB5
#define ILI9341_DFC           0xB6
#define ILI9341_POWER1        0xC0
#define ILI9341_POWER2        0xC1
#define ILI9341_VCOM1         0xC5
#define ILI9341_VCOM2         0xC7
#define ILI9341_POWERA        0xCB
#define ILI9341_POWERB        0xCF
#define ILI9341_PGAMMA        0xE0
#define ILI9341_NGAMMA        0xE1
#define ILI9341_DTCA          0xE8
#define ILI9341_DTCB          0xEA
#define ILI9341_POWER_SEQ     0xED
#define ILI9341_3GAMMA_EN     0xF2
#define ILI9341_INTERFACE     0xF6
#define ILI9341_PRC           0xF7

#define MAC_MY  (1 << 7)
#define MAC_MX  (1 << 6)
#define MAC_MV  (1 << 5)
#define MAC_ML  (1 << 4)
#define MAC_MH  (1 << 2)
#define MAC_BGR (1 << 3)

#define ILI9341_ROTATE_0 (MAC_MX                  )
#define ILI9341_ROTATE_1 (                  MAC_MV)
#define ILI9341_ROTATE_2 (         MAC_MY         )
#define ILI9341_ROTATE_3 (MAC_MX | MAC_MY | MAC_MV)
#define ILI9341_ROTATE_4 (MAC_MX | MAC_MY         )
#define ILI9341_ROTATE_5 (MAC_MX          | MAC_MV)
#define ILI9341_ROTATE_6 (           0            )
#define ILI9341_ROTATE_7 (         MAC_MY | MAC_MV)

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
} pins[] =
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

uint8_t rot_mode;

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

static void _delay(volatile uint16_t _tm)
{
    for (; _tm > 0; _tm--)
        for (volatile uint32_t ct = 0xFFF; ct > 0; ct--);
}

void _hw_init(void)
{
    for (uint8_t i = 0; i < countof(pins); i++)
        GPIO_PinConfigure(pins[i].port, pins[i].pin, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT2MHZ);
    
    GPIO_PinWrite(LCD_PIN_CS, 1);
    GPIO_PinWrite(LCD_PIN_RD, 1);
    GPIO_PinWrite(LCD_PIN_WR, 1);
    GPIO_PinWrite(LCD_PIN_RS, 0);
    GPIO_PinWrite(LCD_PIN_CS, 0);
}

static void _set(uint16_t _data)
{
    if (data_mode != GPIO_MODE_OUT2MHZ)
    {
        for (uint8_t i = 0; i < 8; i++)
            GPIO_PinConfigure(pins[i].port, pins[i].pin, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT2MHZ);
        
        data_mode = GPIO_MODE_OUT2MHZ;
    }
    
    for (uint8_t i = 0; i < countof(pins); i++, _data >>= 1)
    {
        GPIO_PinWrite(pins[i].port, pins[i].pin, _data & 1);
    }
        
    GPIO_PinWrite(LCD_PIN_WR, 1);
}

static uint8_t _get(void)
{
    uint8_t result = 0;
    
    GPIO_PinWrite(LCD_PIN_RS, 1);
    GPIO_PinWrite(LCD_PIN_RD, 0);
    
    if (data_mode != GPIO_MODE_INPUT)
    {
        for (uint8_t i = 0; i < 8; i++)
            GPIO_PinConfigure(pins[i].port, pins[i].pin, GPIO_IN_FLOATING, GPIO_MODE_INPUT);
        
        data_mode = GPIO_MODE_INPUT;
    }

    GPIO_PinWrite(LCD_PIN_RD, 1);
        
    for (uint8_t i = 0; i < 8; i++)
    {
        result <<= 1;
        result |= GPIO_PinRead(pins[i].port, pins[i].pin);
    }
    
    return result;
}

void _rotate(uint8_t _rot)
{
    uint8_t rot_cmd[3] = {2, ILI9341_MAC, MAC_ML | MAC_MH};
    
    rot_mode = _rot;
    
    rot_cmd[2] |= rot_mode;
    
    ili9341::send_cmd(rot_cmd);
}

/***************************************************************************************************
 *                                    PUBLIC FUNCTIONS
 **************************************************************************************************/

namespace ili9341
{

bmp_size_t get_pixel_cnt(const rect_t * _rect)
{
    if (_rect != NULL)
    {
        return (bmp_size_t)(_rect->x2 - _rect->x1 + 1) * (_rect->y2 - _rect->y1 + 1);
    }
    else
    {
        return LCD_WIDTH * LCD_HEIGHT;
    }
}

void send_data(const uint8_t * _data, const bmp_size_t _size)
{
    for (bmp_size_t i = 0; i < _size; i++)
    {
        _set(LCD_DATA_WR | _data[i]);
    }
}

void get_data(uint8_t * _data, const bmp_size_t _size)
{
    for (bmp_size_t i = 0; i < _size >> 1; i++)
    {
        uint8_t r = _get();
        uint8_t g = _get();
        uint8_t b = _get();

        // origin. (see https://www.avrfreaks.net/forum/reading-pixels-gram-memory-ili9341-and-ili9325)
        // uint16_t color =  ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        uint16_t color =  ((r & 0xFC) << 9) | ((g & 0xFC) << 4) | (b >> 2);
        ((uint16_t *)_data)[i] = color ;
    }
}

void send_cmd(const uint8_t * _data)
{
    if (_data != NULL && _data[0] > 0)
    {
        _set(LCD_CMD_WR | _data[1]);
        
        if (_data[0] > 1)
        {
            send_data(&_data[2], _data[0] - 1);
        }
    }
}

void set_rect(const rect_t * _rect, const bool _to_read)
{
    rect_t rect;
    
    if (_rect != NULL)
    {
        rect = * _rect;
    }
    else
    {
        rect =
        {
            .x1 = 0,
            .y1 = 0,
            .x2 = LCD_WIDTH - 1,
            .y2 = LCD_HEIGHT - 1,
        };
    }

    
    static uint8_t column[6] = {5, ILI9341_COLUMN_ADDR};
    static uint8_t row   [6] = {5, ILI9341_PAGE_ADDR};
    
    if (_rect == NULL) return;
    
    column[2] = rect.x1 >> 8;
    column[3] = rect.x1 & 0xFF;
    column[4] = rect.x2 >> 8;
    column[5] = rect.x2 & 0xFF;
    
    row[2] = rect.y1 >> 8;
    row[3] = rect.y1 & 0xFF;
    row[4] = rect.y2 >> 8;
    row[5] = rect.y2 & 0xFF;
    
    send_cmd(column);
    send_cmd(row);

    if (_to_read)
    {
        send_cmd((const uint8_t []){1, ILI9341_RAMRD});
    }
    else
    {
        send_cmd((const uint8_t []){1, ILI9341_RAMWR});
    }
}

void fill_rect(const rect_t * const _rect, const color_t _color)
{
    set_rect(_rect); 
    
    for(bmp_size_t i = get_pixel_cnt(_rect);
        i > 0; i--)
    {
        send_data((uint8_t *)&_color, sizeof(_color));
    }
}

void draw_bmp(const rect_t * _rect, const color_t * _bmp)
{
    set_rect(_rect);
    
    send_data((uint8_t *)_bmp, get_pixel_cnt(_rect) * sizeof(color_t));
}

void init(void)
{
    _hw_init();
    _delay(1000);
    
    send_cmd((const uint8_t []){1, ILI9341_RESET});
    _delay(100);
    
    send_cmd((const uint8_t []){6 , ILI9341_POWERA      , 0x39, 0x2C, 0x00, 0x34, 0x02});
    send_cmd((const uint8_t []){4 , ILI9341_POWERB      , 0x00, 0xC1, 0x30});
    send_cmd((const uint8_t []){4 , ILI9341_DTCA        , 0x85, 0x00, 0x78});
    send_cmd((const uint8_t []){3 , ILI9341_DTCB        , 0x00, 0x00});
    send_cmd((const uint8_t []){5 , ILI9341_POWER_SEQ   , 0x64, 0x03, 0x12, 0x81});
    send_cmd((const uint8_t []){2 , ILI9341_PRC         , 0x20});
    send_cmd((const uint8_t []){2 , ILI9341_POWER1      , 0x23});
    send_cmd((const uint8_t []){2 , ILI9341_POWER2      , 0x10});
    send_cmd((const uint8_t []){3 , ILI9341_VCOM1       , 0x3E, 0x28});
    send_cmd((const uint8_t []){2 , ILI9341_VCOM2       , 0x86});

    //-- pixel orientation and draw direction
    _rotate(ILI9341_ROTATE_4);

    send_cmd((const uint8_t []){2 , ILI9341_PIXEL_FORMAT, 0x55});
    send_cmd((const uint8_t []){3 , ILI9341_FRC         , 0x00, 0x18});
    send_cmd((const uint8_t []){4 , ILI9341_DFC         , 0x08, 0x82, 0x27});
    send_cmd((const uint8_t []){2 , ILI9341_3GAMMA_EN   , 0x00});
    send_cmd((const uint8_t []){2 , ILI9341_GAMMA       , 0x01});
    send_cmd((const uint8_t []){16, ILI9341_PGAMMA      , 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00});
    send_cmd((const uint8_t []){16, ILI9341_NGAMMA      , 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F});
    send_cmd((const uint8_t []){1 , ILI9341_SLEEP_OUT   });
    _delay(100);

    send_cmd((const uint8_t []){1 , ILI9341_DISPLAY_ON});
    _delay(100);
}

void scroll(const uint16_t _scr)
{
    static uint8_t scroll_set[8] = {7, ILI9341_VSCRDEF, 0, 0, 0, 0, 0, 0};
    static uint8_t scroll[4] = {3, ILI9341_VSCRSADD, 0, 0};
    uint16_t scroll_lines = LCD_HEIGHT;
    
    scroll_set[4] = scroll_lines >> 8;
    scroll_set[5] = scroll_lines & 0xFF;
    scroll[2] = _scr >> 8;    
    scroll[3] = _scr & 0xFF;  

    send_cmd(scroll_set);
    send_cmd(scroll);  
}

}; // namespace ili9341

/***************************************************************************************************
 *                                       END OF FILE
 **************************************************************************************************/
