/******************************************************************************
 * File Name   : MX_Device.h
 * Date        : 07/06/2019 09:51:55
 * Description : STM32Cube MX parameter definitions
 * Note        : This file is generated by STM32CubeMX (DO NOT EDIT!)
 ******************************************************************************/

#ifndef __MX_DEVICE_H
#define __MX_DEVICE_H

/*---------------------------- Clock Configuration ---------------------------*/

#define MX_LSI_VALUE                            32000
#define MX_LSE_VALUE                            32768
#define MX_HSI_VALUE                            16000000
#define MX_HSE_VALUE                            8000000
#define MX_SYSCLKFreq_VALUE                     8000000
#define MX_HCLKFreq_Value                       8000000
#define MX_FCLKCortexFreq_Value                 8000000
#define MX_CortexFreq_Value                     8000000
#define MX_AHBFreq_Value                        8000000
#define MX_APB1Freq_Value                       8000000
#define MX_APB2Freq_Value                       8000000
#define MX_APB1TimFreq_Value                    8000000
#define MX_APB2TimFreq_Value                    8000000
#define MX_PWRFreq_Value                        8000000
#define MX_RTCFreq_Value                        32000
#define MX_USBFreq_Value                        16000000
#define MX_WatchDogFreq_Value                   32000
#define MX_MCO1PinFreq_Value                    8000000

/*-------------------------------- SPI2       --------------------------------*/

#define MX_SPI2                                 1

/* GPIO Configuration */

/* Pin PB10 */
#define MX_SPI2_SCK_GPIO_Speed                  GPIO_SPEED_FREQ_VERY_HIGH
#define MX_SPI2_SCK_Pin                         PB10
#define MX_SPI2_SCK_GPIOx                       GPIOB
#define MX_SPI2_SCK_GPIO_PuPd                   GPIO_NOPULL
#define MX_SPI2_SCK_GPIO_Pin                    GPIO_PIN_10
#define MX_SPI2_SCK_GPIO_AF                     GPIO_AF5_SPI2
#define MX_SPI2_SCK_GPIO_Mode                   GPIO_MODE_AF_PP

/* Pin PC2 */
#define MX_SPI2_MISO_GPIO_Speed                 GPIO_SPEED_FREQ_VERY_HIGH
#define MX_SPI2_MISO_Pin                        PC2
#define MX_SPI2_MISO_GPIOx                      GPIOC
#define MX_SPI2_MISO_GPIO_PuPd                  GPIO_NOPULL
#define MX_SPI2_MISO_GPIO_Pin                   GPIO_PIN_2
#define MX_SPI2_MISO_GPIO_AF                    GPIO_AF5_SPI2
#define MX_SPI2_MISO_GPIO_Mode                  GPIO_MODE_AF_PP

/* Pin PC3 */
#define MX_SPI2_MOSI_GPIO_Speed                 GPIO_SPEED_FREQ_VERY_HIGH
#define MX_SPI2_MOSI_Pin                        PC3
#define MX_SPI2_MOSI_GPIOx                      GPIOC
#define MX_SPI2_MOSI_GPIO_PuPd                  GPIO_NOPULL
#define MX_SPI2_MOSI_GPIO_Pin                   GPIO_PIN_3
#define MX_SPI2_MOSI_GPIO_AF                    GPIO_AF5_SPI2
#define MX_SPI2_MOSI_GPIO_Mode                  GPIO_MODE_AF_PP

/*-------------------------------- SYS        --------------------------------*/

#define MX_SYS                                  1

/* GPIO Configuration */

/*-------------------------------- NVIC       --------------------------------*/

#define MX_NVIC                                 1

/*-------------------------------- GPIO       --------------------------------*/

#define MX_GPIO                                 1

/* GPIO Configuration */

#endif  /* __MX_DEVICE_H */

