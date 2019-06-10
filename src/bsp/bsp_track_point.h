/***************************************************************************************************
 *   Project:       stm commander
 *   Author:        Stulov Tikhon (kudesnick@inbox.ru)
 ***************************************************************************************************
 *   Distribution:  
 *
 ***************************************************************************************************
 *   MCU Family:    STM32F
 *   Compiler:      ARMCC
 ***************************************************************************************************
 *   File:          bsp_track_point.h
 *   Description:   
 *
 ***************************************************************************************************
 *   History:       02.05.2019 - file created
 *
 **************************************************************************************************/

#pragma once

/***************************************************************************************************
 *                                      INCLUDED FILES
 **************************************************************************************************/

#include "cpp_storage.h"

/***************************************************************************************************
 *                                       DEFINITIONS
 **************************************************************************************************/

/***************************************************************************************************
 *                                      PUBLIC TYPES
 **************************************************************************************************/

namespace track_point
{

typedef void(* fn_event_t)(const key_t, const key_event_t);

/***************************************************************************************************
 *                                     GLOBAL VARIABLES
 **************************************************************************************************/

/***************************************************************************************************
 *                                PUBLIC FUNCTION PROTOTYPES
 **************************************************************************************************/

void init(void);
void callback_init(key_t _key_num, key_event_t _event, fn_event_t _func);

}; // namespace track_point

/***************************************************************************************************
 *                                       END OF FILE
 **************************************************************************************************/
