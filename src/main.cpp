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

#include "RTE_Components.h"
#include CMSIS_device_header

#include "bsp_gpio.h"
#include "cpp_os.h"
#include "misc_macro.h"

extern "C"
{
#include "Board_LED.h"
}

/***************************************************************************************************
 *                                       DEFINITIONS
 **************************************************************************************************/

/***************************************************************************************************
 *                                    PUBLIC FUNCTIONS
 **************************************************************************************************/

#if defined(__ARMCC_VERSION) && !defined(__OPTIMIZE__)
    /*
    Without this directive, it does not start if -o0 optimization is used and the "main"
    function without parameters.
    see http://www.keil.com/support/man/docs/armclang_mig/armclang_mig_udb1499267612612.htm
    */
    __asm (".global __ARM_use_no_argv\n\t" "__ARM_use_no_argv:\n\t");
#endif

#ifdef HAL_MODULE_ENABLED
extern "C"
{
int cpp_main(void)
#else
int main(void)
#endif
{
    SystemCoreClockUpdate();
    
    cpp_os::create_os();
    
    BRK_PTR();

    return 0;
}

#ifdef HAL_MODULE_ENABLED
} // extern "C"
#endif

/***************************************************************************************************
 *                                       END OF FILE
 **************************************************************************************************/
