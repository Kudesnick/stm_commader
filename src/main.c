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
RD          A0          PA0
WR          A1          PA1
RS          A2          PA4
CS          A3          PB0
*/

/***************************************************************************************************
 *                                      INCLUDED FILES
 **************************************************************************************************/

#include "RTE_Components.h"
#include CMSIS_device_header

#include "GPIO_STM32F10x.h"

/***************************************************************************************************
 *                                       DEFINITIONS
 **************************************************************************************************/

#define LCD_WIDTH       240
#define LCD_HEIGHT      320
#define LCD_PIXEL_COUNT	LCD_WIDTH * LCD_HEIGHT

#define LCD_PIN_D0 GPIOA, 0x09
#define LCD_PIN_D1 GPIOC, 0x07
#define LCD_PIN_D2 GPIOB, 0x06
#define LCD_PIN_D3 GPIOA, 0x07
#define LCD_PIN_D4 GPIOB, 0x05
#define LCD_PIN_D5 GPIOA, 0x05
#define LCD_PIN_D6 GPIOB, 0x0A
#define LCD_PIN_D7 GPIOA, 0x08
#define LCD_PIN_RD GPIOA, 0x00
#define LCD_PIN_WR GPIOA, 0x01
#define LCD_PIN_RS GPIOA, 0x04
#define LCD_PIN_CS GPIOB, 0x00

#define countof(a) (sizeof(a)/sizeof((a)[0]))

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
 *                                      PRIVATE TYPES
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
    {LCD_PIN_RD},
    {LCD_PIN_WR},
    {LCD_PIN_RS},
    {LCD_PIN_CS},
};

/***************************************************************************************************
 *                                       PUBLIC DATA
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

static void delay(void)
{
    for (volatile uint32_t ct = 0xFFFF; ct > 0; ct--)
        __asm("nop");
}

static void lcd_data_set(uint8_t _data)
{
    for (uint8_t i = 0; i < 8; i++, _data >>= 1)
    {
        GPIO_PinConfigure(lcd_pins[i].port, lcd_pins[i].pin, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT2MHZ);
        GPIO_PinWrite(lcd_pins[i].port, lcd_pins[i].pin, _data & 1);
    }
}

static uint8_t lcd_data_get(void)
{
    uint8_t result;
    
    for (uint8_t i = 0; i < 8; i++)
    {
        result <<= 1;
        GPIO_PinConfigure(lcd_pins[i].port, lcd_pins[i].pin, GPIO_IN_PULL_UP, GPIO_MODE_INPUT);
        result |= GPIO_PinRead(lcd_pins[i].port, lcd_pins[i].pin);
    }
    
    return result;
}

/***************************************************************************************************
 *                                    PUBLIC FUNCTIONS
 **************************************************************************************************/

void lcd_hw_init(void)
{
    for (uint8_t i = 0; i < countof(lcd_pins); i++)
        GPIO_PinConfigure(lcd_pins[i].port, lcd_pins[i].pin, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT2MHZ);
    
    GPIO_PinWrite(LCD_PIN_RD, 1);
    GPIO_PinWrite(LCD_PIN_WR, 1);
    GPIO_PinWrite(LCD_PIN_RS, 0);
    GPIO_PinWrite(LCD_PIN_CS, 0);
}

void lcd_send_cmd(uint8_t _data)
{
    lcd_data_set(_data);
    GPIO_PinWrite(LCD_PIN_RS, 0);
    GPIO_PinWrite(LCD_PIN_WR, 0);
    GPIO_PinWrite(LCD_PIN_WR, 1);
}

void lcd_send_data(uint8_t _data)
{
    lcd_data_set(_data);
    GPIO_PinWrite(LCD_PIN_RS, 1);
    GPIO_PinWrite(LCD_PIN_WR, 0);
    GPIO_PinWrite(LCD_PIN_WR, 1);
}

uint8_t lcd_read(void)
{
    lcd_data_get();
    GPIO_PinWrite(LCD_PIN_RS, 1);
    GPIO_PinWrite(LCD_PIN_RD, 0);
    GPIO_PinWrite(LCD_PIN_RD, 1);
    return lcd_data_get();
}

void LCD_SetCursorPosition(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
	lcd_send_cmd(ILI9341_COLUMN_ADDR);
	lcd_send_data(x1 >> 8);
	lcd_send_data(x1 & 0xFF);
	lcd_send_data(x2 >> 8);
	lcd_send_data(x2 & 0xFF);

	lcd_send_cmd(ILI9341_PAGE_ADDR);
	lcd_send_data(y1 >> 8);
	lcd_send_data(y1 & 0xFF);
	lcd_send_data(y2 >> 8);
	lcd_send_data(y2 & 0xFF);
}

void LCD_Fill(uint16_t color) {
	unsigned int n, i, j;
	i = color >> 8;
	j = color & 0xFF;
	LCD_SetCursorPosition(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);

	lcd_send_cmd(ILI9341_GRAM);

	for (n = 0; n < LCD_PIXEL_COUNT; n++) {
		lcd_send_data(i);
		lcd_send_data(j);
	}
}

int main(void)
{
    lcd_hw_init();
    
    lcd_send_cmd(ILI9341_RESET);
    delay();
   
    /// настраиваем дисплей
    lcd_send_cmd(ILI9341_POWERA);
    lcd_send_data(0x39);
    lcd_send_data(0x2C);
    lcd_send_data(0x00);
    lcd_send_data(0x34);
    lcd_send_data(0x02);
    lcd_send_cmd(ILI9341_POWERB);
    lcd_send_data(0x00);
    lcd_send_data(0xC1);
    lcd_send_data(0x30);
    lcd_send_cmd(ILI9341_DTCA);
    lcd_send_data(0x85);
    lcd_send_data(0x00);
    lcd_send_data(0x78);
    lcd_send_cmd(ILI9341_DTCB);
    lcd_send_data(0x00);
    lcd_send_data(0x00);
    lcd_send_cmd(ILI9341_POWER_SEQ);
    lcd_send_data(0x64);
    lcd_send_data(0x03);
    lcd_send_data(0x12);
    lcd_send_data(0x81);
    lcd_send_cmd(ILI9341_PRC);
    lcd_send_data(0x20);
    lcd_send_cmd(ILI9341_POWER1);
    lcd_send_data(0x23);
    lcd_send_cmd(ILI9341_POWER2);
    lcd_send_data(0x10);
    lcd_send_cmd(ILI9341_VCOM1);
    lcd_send_data(0x3E);
    lcd_send_data(0x28);
    lcd_send_cmd(ILI9341_VCOM2);
    lcd_send_data(0x86);
    lcd_send_cmd(ILI9341_MAC);
    lcd_send_data(0x48);
    lcd_send_cmd(ILI9341_PIXEL_FORMAT);
    lcd_send_data(0x55);
    lcd_send_cmd(ILI9341_FRC);
    lcd_send_data(0x00);
    lcd_send_data(0x18);
    lcd_send_cmd(ILI9341_DFC);
    lcd_send_data(0x08);
    lcd_send_data(0x82);
    lcd_send_data(0x27);
    lcd_send_cmd(ILI9341_3GAMMA_EN);
    lcd_send_data(0x00);
    lcd_send_cmd(ILI9341_COLUMN_ADDR);
    lcd_send_data(0x00);
    lcd_send_data(0x00);
    lcd_send_data(0x00);
    lcd_send_data(0xEF);
    lcd_send_cmd(ILI9341_PAGE_ADDR);
    lcd_send_data(0x00);
    lcd_send_data(0x00);
    lcd_send_data(0x01);
    lcd_send_data(0x3F);
    lcd_send_cmd(ILI9341_GAMMA);
    lcd_send_data(0x01);
    lcd_send_cmd(ILI9341_PGAMMA);
    lcd_send_data(0x0F);
    lcd_send_data(0x31);
    lcd_send_data(0x2B);
    lcd_send_data(0x0C);
    lcd_send_data(0x0E);
    lcd_send_data(0x08);
    lcd_send_data(0x4E);
    lcd_send_data(0xF1);
    lcd_send_data(0x37);
    lcd_send_data(0x07);
    lcd_send_data(0x10);
    lcd_send_data(0x03);
    lcd_send_data(0x0E);
    lcd_send_data(0x09);
    lcd_send_data(0x00);
    lcd_send_cmd(ILI9341_NGAMMA);
    lcd_send_data(0x00);
    lcd_send_data(0x0E);
    lcd_send_data(0x14);
    lcd_send_data(0x03);
    lcd_send_data(0x11);
    lcd_send_data(0x07);
    lcd_send_data(0x31);
    lcd_send_data(0xC1);
    lcd_send_data(0x48);
    lcd_send_data(0x08);
    lcd_send_data(0x0F);
    lcd_send_data(0x0C);
    lcd_send_data(0x31);
    lcd_send_data(0x36);
    lcd_send_data(0x0F);
    lcd_send_cmd(ILI9341_SLEEP_OUT);
    delay();
    
    lcd_send_cmd(ILI9341_DISPLAY_ON);
    lcd_send_cmd(ILI9341_GRAM);
    delay();
    
    LCD_Fill(0xFF<<8);
    
    for(;;)
    {
        
    }
}

/***************************************************************************************************
 *                                       END OF FILE
 **************************************************************************************************/
