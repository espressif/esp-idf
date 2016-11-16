// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _DRIVER_TOUCH_PAD_H_
#define _DRIVER_TOUCH_PAD_H_
#ifdef __cplusplus
extern "C" {
#endif
#define ETS_RTC_INTR_INUM  19  


typedef enum{
	TOUCH_PAD_NUM0=0,  //GPIO4
	TOUCH_PAD_NUM1,    //GPIO0
	TOUCH_PAD_NUM2,   //GPIO2
	TOUCH_PAD_NUM3,   //GPIO15
	TOUCH_PAD_NUM4,   //GPIO13
	TOUCH_PAD_NUM5,   //GPIO12
	TOUCH_PAD_NUM6,   //GPIO14
	TOUCH_PAD_NUM7,	  //GPIO27
	TOUCH_PAD_NUM8,   //GPIO33
	TOUCH_PAD_NUM9,   //GPIO32
	TOUCH_PAD_MAX,
}enum_touch_pad_t;

typedef struct touch_message{
	enum_touch_pad_t touch_num;
}touch_message_t;
typedef void (*touch_event_callback)(touch_message_t touch_mes);
/**
* @brief	    touch module int
*
*This function int touch pad module ,enable touch module
*
* @return	  None             
*             
*/
void touch_module_init();

/**
* @brief	    config touch pad intr callback thresold
*
*When you touch the pad ,touch_pad_register reduce .When the touch_pad_register less than threshold
*will trigger the touch event callback
*
* @param[in]  touch_num  : config touch num
*
* @param[in]  threshold : interrupt threshold  ,When the touch_pad_register less than threshold£¬
*                         will trigger the touch event callback.User can use touch_pad_read function
*                         to determine the threshold.
*
* @return	  None             
*             
*/
void touch_pad_config(enum_touch_pad_t touch_num,uint16_t threshold);

/**
* @brief	    register touch pad interrupt occur callback function
*
* Use this Function,register interrupt event callback . when touch pad interrupt is triggered , the event callback
* will give appliacation .touch_event_callback is called by touch interrupt , so the touch_event_callback
* should be completed in a very short period of time. If you want to deal with a lot of things,Please Creat
* a task ,and post message from the gpio_event_callback to the task.
*
* @param[in]  func : the func will be called,when touch interrupt is triggered ,and the param of func is the 
*                    number of the touch pad num occurs interrupt
*
*
* @return	  None             
*             
*/
void touch_module_register_event_callback(touch_event_callback touch_event_cb);

/**
* @brief	    get touch pad touch_pad_register number
*
*User can use this function to determine the the interrupt threshold .When you do not touch  the 
*pad ,read the touch_pad_read number(NumNotTouch) by the touch_pad_register.When you touch the pad ,read the touch_pad_register
*number(NumTouch) by the touch_pad_read.Normal NumNotTouch>NumTouch,so you can select a interrupt threshold
*
* @param[in]  touch_num  : touch num
*
*
* @return	  -1   : the touch num is err
*            Other : the number of the touch_pad_read
*             
*/
int32_t touch_pad_read(enum_touch_pad_t touch_num);
#ifdef __cplusplus
}
#endif

#endif

