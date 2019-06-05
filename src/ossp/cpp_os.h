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
 *   File:          cpp_os.h
 *   Description:   
 *
 ***************************************************************************************************
 *   History:       27.05.2019 - file created
 *
 **************************************************************************************************/

#pragma once

/***************************************************************************************************
 *                                      INCLUDED FILES
 **************************************************************************************************/

#include <stdlib.h>
#include <stdint.h>

#include "cpp_list.h"
#include "rtx_os.h"
#include "RTX_Config.h"

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

//-- os_elements

class cpp_os : public cpp_list<cpp_os>
{
private:
    static void all_elements_create(void);

protected:
    static osStatus_t os_chck(osStatus_t);
    static void * os_chck(void *);
    
public:
    using cpp_list::cpp_list;

    virtual void * create(void) = 0;

    static void create_os(void);

    static osStatus_t delay(const uint32_t _ticks)
    {
        return osDelay(_ticks);
    };
    
    static osStatus_t delay_until(const uint32_t _ticks)
    {
        return osDelayUntil(_ticks);
    };

    static uint32_t get_tick_count(void)
    {
        return osKernelGetTickCount();
    };
    
    static uint32_t get_tick_freq(void)
    {
        return osKernelGetTickFreq();
    }
};

//-- thread

template<uint32_t T_stack_size = OS_STACK_SIZE, bool T_create_now = true> class cpp_os_thread : public cpp_os
{
private:
    uint64_t stack[(T_stack_size + sizeof(uint64_t) - 1) / sizeof(uint64_t)]; // Выравнивание по uint64_t обязательно
    uint32_t tcb[osRtxThreadCbSize / sizeof(uint32_t)];
    osThreadId_t id_;
    bool must_create = T_create_now;

    static void thread_run(void * argument)
    {
        static_cast<cpp_os_thread<T_stack_size, T_create_now> *>(argument)->thread_func();
    };
    
    virtual void thread_func() = 0;

protected:
    
    osStatus_t yeld(void)
    {        
        return osThreadYield();
    };
    
    void exit(void)
    {
        osThreadExit();
    };

public:
    using cpp_os::cpp_os;

    osThreadId_t create(void)
    {
        const osThreadAttr_t attr =
        {
            .cb_mem     = &tcb,
            .cb_size    = sizeof(tcb),
            .stack_mem  = stack,
            .stack_size = sizeof(stack),
        };
        
        if (must_create)
        {
            id_ = os_chck(osThreadNew(thread_run, this, &attr));
        }
        
        must_create = true;
        
        return id_;
    };
    
    osThreadState_t get_state(void)
    {
        return osThreadGetState(id_);
    };
    
    osStatus_t set_priority(osPriority_t _priority)
    {
        return osThreadSetPriority(id_, _priority);
    };
    
    osPriority_t get_priority(void)
    {        
        return osThreadGetPriority(id_);
    };

    osStatus_t suspend(void)
    {        
        return osThreadSuspend(id_);
    };
    
    osStatus_t resume(void)
    {        
        return osThreadResume(id_);
    };
    
    osStatus_t detach(void)
    {        
        return osThreadDetach(id_);
    };
    
    osStatus_t join(void)
    {        
        return osThreadJoin(id_);
    };
    
    osStatus_t terminate(void)
    {        
        return osThreadTerminate(id_);
    };
};

//-- queue

template<typename T_queue_elment_t, uint32_t T_queue_count> class cpp_os_queue : public cpp_os
{
private:
    uint32_t qdata[osRtxMessageQueueMemSize(T_queue_count, sizeof(T_queue_elment_t)) / sizeof(uint32_t)];
    uint32_t qcb[osRtxMessageQueueCbSize / sizeof(uint32_t)];
    osMessageQueueId_t id_;

public:
    using cpp_os::cpp_os;

    osMessageQueueId_t create(void)
    {
        const osMessageQueueAttr_t attr =
        {
            .cb_mem  = &qcb,
            .cb_size = sizeof(qcb),
            .mq_mem  = &qdata,
            .mq_size = sizeof(qdata),
        };

        id_ = os_chck(osMessageQueueNew(T_queue_count, sizeof(T_queue_elment_t), &attr));
        return id_;
    };

    osStatus_t put(const void * _msg_ptr, uint8_t _msg_prio, uint32_t _timeout)
    {
        return osMessageQueuePut(id_, _msg_ptr, _msg_prio, _timeout);
    };

    osStatus_t get(void * _msg_ptr, uint8_t * _msg_prio, uint32_t _timeout)
    {
        return osMessageQueueGet(id_, _msg_ptr, _msg_prio, _timeout);
    };

    uint32_t get_capacity(void)
    {
        return osMessageQueueGetCapacity(id_);
    };

    uint32_t get_msg_size(void)
    {
        return osMessageQueueGetMsgSize(id_);
    };

    uint32_t get_count(void)
    {
        return osMessageQueueGetCount(id_);
    };  

    uint32_t get_space(void)
    {
        return osMessageQueueGetSpace(id_);
    };

    osStatus_t reset(void)   
    {
        return osMessageQueueReset(id_);
    };
};

//-- event_flags

class cpp_os_event : public cpp_os
{
private:
    uint32_t ecb[osRtxEventFlagsCbSize / sizeof(uint32_t)];
    osEventFlagsId_t id_;

public:
    using cpp_os::cpp_os;

    osEventFlagsId_t create(void)
    {
        const osEventFlagsAttr_t attr =
        {
            .cb_mem  = &ecb,
            .cb_size = sizeof(ecb),
        };

        id_ = os_chck(osEventFlagsNew(&attr));
        return id_;
    };

    uint32_t set(uint32_t _flags)
    {
        return osEventFlagsSet(id_, _flags);
    };

    uint32_t clear(uint32_t _flags)
    {
        return osEventFlagsClear(id_, _flags);
    };

    uint32_t get(void)
    {
        return osEventFlagsGet(id_);
    };

    uint32_t wait(uint32_t _flags, uint32_t _options, uint32_t _timeout)
    {
        return osEventFlagsWait(id_, _flags, _options, _timeout);
    };
};

//-- mutex

template<uint32_t T_flags = 0> class cpp_os_mutex : public cpp_os
{
private:
    uint32_t mcb[osRtxMutexCbSize / sizeof(uint32_t)];
    osMutexId_t id_;

public:
    using cpp_os::cpp_os;

    osMutexId_t create(void)
    {
        const osMutexAttr_t attr =
        {
            .attr_bits = T_flags,
            .cb_mem  = &mcb,
            .cb_size = sizeof(mcb),
        };

        id_ = os_chck(osMutexNew(&attr));
        return id_;
    };

    osStatus_t acquire(uint32_t _timeout)
    {
        return osMutexAcquire(id_, _timeout);
    };

    osStatus_t release(void)
    {
        return osMutexRelease(id_);
    };

    osThreadId_t get_owner(void)
    {
        return osMutexGetOwner(id_);
    };
};

/***************************************************************************************************
 *                                     GLOBAL VARIABLES
 **************************************************************************************************/
    
/***************************************************************************************************
 *                                       END OF FILE
 **************************************************************************************************/