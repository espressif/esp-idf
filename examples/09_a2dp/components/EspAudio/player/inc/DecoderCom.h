/*****************************************************************************
*
* File Name : wm_http_client.h
*
* Description: Http client header file.
*
* Copyright (c) 2014 Winner Microelectronics Co., Ltd.
* All rights reserved.
*
* Author : wanghf
*
* Date : 2014-6-6
*****************************************************************************/

#ifndef __DECODER_COM_H__
#define __DECODER_COM_H__
#include "esp_types.h"



// HTTP Type Definitions

//#define pthread_t       sys_thread_t


typedef enum {
    TaskState_Unknown   = 0,
    TaskState_Idle      = 1, // running state
    TaskState_Running   = 2,
    TaskState_Stoped    = 3,
    TaskState_Finished  = 4,

    TaskState_Ready     = 10, //control state
    TaskState_Resume    = 11,
    TaskState_Pause     = 12,

} TaskState;




#endif //__DECODER_COM_H__
