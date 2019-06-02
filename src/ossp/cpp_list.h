/***************************************************************************************************
 *   Project:       
 *   Author:        
 ***************************************************************************************************
 *   Distribution:  
 *
 ***************************************************************************************************
 *   MCU Family:    STM32F
 *   Compiler:      ARMCC
 ***************************************************************************************************
 *   File:          cpp_list.h
 *   Description:   
 *
 ***************************************************************************************************
 *   History:       17.05.2019 - file created
 *
 **************************************************************************************************/

#pragma once

/***************************************************************************************************
 *                                      INCLUDED FILES
 **************************************************************************************************/

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
    using namespace std;
#endif

/***************************************************************************************************
 *                                       DEFINITIONS
 **************************************************************************************************/

/***************************************************************************************************
 *                                      PUBLIC TYPES
 **************************************************************************************************/

/***************************************************************************************************
 *                                PUBLIC FUNCTION PROTOTYPES
 **************************************************************************************************/

/***************************************************************************************************
 *                               PUBLIC CLASS
 **************************************************************************************************/

//-- class cpp_list

template<typename T> class cpp_list
{
private:
    static cpp_list * p_first_;
    cpp_list * p_next_;

public:
    
    cpp_list():
        p_next_(NULL)
    {
        static cpp_list * p_last_ = NULL;
    
        if (p_first_ == NULL) p_first_ = this;
    
        if (p_last_ != NULL) p_last_->p_next_ = this;
        
        p_last_ = this;
    };
    
    static cpp_list * get_first(void)
    {
        return p_first_;
    };
    
    cpp_list * get_next(void)
    {
        return p_next_;
    };
    
    void del_item(void)
    {
        if (this == p_first_)
        {
            p_first_ = this->p_next_;
            
            return;
        }
        
        cpp_list * prev_ptr = p_first_;
        
        for (cpp_list * curr_ptr = p_first_->p_next_;
            curr_ptr != NULL;
            curr_ptr = curr_ptr->p_next_
            )
        {
            if (this == curr_ptr)
            {
                prev_ptr->p_next_ = curr_ptr->p_next_;
                
                return;
            }
            else
            {
                prev_ptr = curr_ptr;
            }
        }
    };
    
    void pop_item(void)
    {
        this->del_item();
        p_next_ = p_first_;
        p_first_ = this;
    };
    
    static void enumerate(bool(* _enum_func)(T *&))
    {
        if (_enum_func == NULL) return;
        
        for (T * el_ptr = static_cast<T *>(cpp_list::get_first());
             el_ptr != NULL;
             el_ptr = static_cast<T *>(el_ptr->get_next())
             )
        {
            if (_enum_func(el_ptr) == false) break;
        }    
    };
    
    static void * enumerate(void * _params, bool(* _enum_func)(T *&, void *))
    {
        if (_enum_func == NULL) return NULL;
    
        for (T * el_ptr = static_cast<T *>(cpp_list::get_first());
             el_ptr != NULL;
             el_ptr = static_cast<T *>(el_ptr->get_next())
             )
        {
            if (_enum_func(el_ptr, _params) == false) break;
        }
        
        return _params;
    };
};

template<typename T> cpp_list<T> * cpp_list<T>::p_first_ = NULL;

/***************************************************************************************************
 *                                     GLOBAL VARIABLES
 **************************************************************************************************/
    
/***************************************************************************************************
 *                                       END OF FILE
 **************************************************************************************************/
