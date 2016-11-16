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

#ifndef _DRIVER_LEDC_PWM_H_
#define _DRIVER_LEDC_PWM_H_

#include "soc/soc.h"
#include "soc/ledc_reg.h"
#include "driver/gpio.h"
#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------
                   HSCHX_LEDC_PWM
---------------------------------------------------*/
#define LEDC_HSCHX_CONF0_REG(CHANNEL)  (LEDC_HSCH0_CONF0_REG+0X14*(CHANNEL)) 
#define LEDC_HSCHX_HPOINT_REG(CHANNEL) (LEDC_HSCH0_HPOINT_REG+0X14*(CHANNEL))
#define LEDC_HSCHX_DUTY_REG(CHANNEL)   (LEDC_HSCH0_DUTY_REG+0X14*(CHANNEL))
#define LEDC_HSCHX_CONF1_REG(CHANNEL)   (LEDC_HSCH0_CONF1_REG+0X14*(CHANNEL))
#define LEDC_HSCHX_DUTY_R_REG(CHANNEL) (LEDC_HSCH0_DUTY_R_REG+0X14*(CHANNEL))
/*-------------------------------------------------
                   LSCHX_LEDC_PWM
--------------------------------------------------*/
#define LEDC_LSCHX_CONF0_REG(CHANNEL)  (LEDC_LSCH0_CONF0_REG+0X14*(CHANNEL)) 
#define LEDC_LSCHX_HPOINT_REG(CHANNEL) (LEDC_LSCH0_HPOINT_REG+0X14*(CHANNEL))
#define LEDC_LSCHX_DUTY_REG(CHANNEL)   (LEDC_LSCH0_DUTY_REG+0X14*(CHANNEL))
#define LEDC_LSCHX_CONF1_REG(CHANNEL)   (LEDC_LSCH0_CONF1_REG+0X14*(CHANNEL))
#define LEDC_LSCHX_DUTY_R_REG(CHANNEL) (LEDC_LSCH0_DUTY_R_REG+0X14*(CHANNEL))

/*---------------------------------------------------
               HTIMERX_LEDC_PWM
---------------------------------------------------*/
#define LEDC_HSTIMERX_CONF_REG(TIME) (LEDC_HSTIMER0_CONF_REG+0X08*(TIME))
#define LEDC_HSTIMERX_VALUE_REG(TIME) (LEDC_HSTIMER0_VALUE_REG+0X08*(TIME))
/*----------------------------------------------
               LTIMERX_LEDC_PWM
-----------------------------------------------*/
#define LEDC_LSTIMERX_CONF_REG(TIME) (LEDC_LSTIMER0_CONF_REG+0X08*(TIME))
#define LEDC_LSTIMERX_VALUE_REG(TIME) (LEDC_LSTIMER0_VALUE_REG+0X08*(TIME))

enum ledc_pwm_mode{
    LEDC_HIGH_SPEED_MODE=0, /**< Ledc high speed mode */
    LEDC_LOW_SPEED_MODE,     /**< Ledc low speed mode */
};
enum ledc_pwm_intr_type{
    LEDC_INTR_DISABLE=0,/**< Disable ledc interrupt */
    LEDC_INTR_CHANGE_END,    /**< Enable ledc interrupt */
};
enum ledc_pwm_increase_direction{
    LEDC_DIR_DECREASE=0,/**<ledc change decrease direction */
    LEDC_DIR_INCREASE=1,/**<ledc change increase direction */
};
enum ledc_timer_source{
    LEDC_TIMER0=0,/**<ledc source time TIME0 */
    LEDC_TIMER1,  /**<ledc source time TIME1 */
    LEDC_TIMER2,  /**<ledc source time TIME2 */
    LEDC_TIMER3,  /**<ledc source time TIME3 */
};
enum ledc_pwm_channel{
    LEDC_PWM_CHANNEL0=0,/**<ledc channel 0 */
    LEDC_PWM_CHANNEL1,  /**<ledc channel 1 */
    LEDC_PWM_CHANNEL2,  /**<ledc channel 2 */
    LEDC_PWM_CHANNEL3,  /**<ledc channel 3 */
    LEDC_PWM_CHANNEL4,  /**<ledc channel 4 */
    LEDC_PWM_CHANNEL5,  /**<ledc channel 5 */
    LEDC_PWM_CHANNEL6,  /**<ledc channel 6 */
    LEDC_PWM_CHANNEL7,  /**<ledc channel 7 */
};
enum ledc_pwm_duty_depth{
    LEDC_PWM_DUTY_DEPTH_10_BIT=10, /**<ledc pwm depth 10Bit */
    LEDC_PWM_DUTY_DEPTH_11_BIT=11, /**<ledc pwm depth 11Bit */
    LEDC_PWM_DUTY_DEPTH_12_BIT=12, /**<ledc pwm depth 12Bit */
    LEDC_PWM_DUTY_DEPTH_13_BIT=13, /**<ledc pwm depth 13Bit */
    LEDC_PWM_DUTY_DEPTH_14_BIT=14, /**<ledc pwm depth 14Bit */
    LEDC_PWM_DUTY_DEPTH_15_BIT=15, /**<ledc pwm depth 15Bit */
};
typedef struct ledc_event_message{
    enum ledc_pwm_mode mode;
    enum ledc_pwm_channel channel;
}ledc_event_msg_t;

typedef void (*ledc_event_callback)(ledc_event_msg_t ledc_mes);

typedef struct ledc_pwm_channel_config{
    gpio_num_t gpio_num;
    enum ledc_pwm_channel channel;
    enum ledc_pwm_intr_type intr_enable;
    /*if selet same time,the fre and the ledc_depth must be same*/
    enum ledc_timer_source time;
    uint32_t fre_hz;
    uint16_t duty;
    enum ledc_pwm_duty_depth ledc_pre_depth;
}ledc_channel_config_t;

/**
 * @brief	   ledc common configuration
 *
 * User this Function,config ledc the channel output gpio_num,interrupt,source time,frequench(Hz),start duty,ledc depth
 *
 * @param[in]  mode : select the ledc mode,high mode and low mode,now we only support high mode ,next will add low mode
 *
 * @param[in]  ledc_channel_config_t  ledc_channel_config_t.gpio_num       : the ledc output gpio_num,if you want to use gpio16,
 *                                                                       ledc_channel_config_t.gpio_num=GPIO_NUM_16
 *
 *                                  ledc_channel_config_t.channel        : current configuration channel
 *                                  ledc_channel_config_t.intr_enable    : config interrupe ,Gradient interrupt enable  or Gradient interrupt disable
 *                                  ledc_channel_config_t.time           : select the time of channel, when different channel ,select same time ,their
 *                                                                       fre_hz and ledc_pre_depth must be same
 *                                  ledc_channel_config_t.fre_hz         : ledc channel frequech(Hz),when different channel ,select same time ,their
 *                                                                       fre_hz and ledc_pre_depth must be same
 *                                  ledc_channel_config_t.duty           : ledc channel duty,if the ledc_pre_depth = 10,the duty range is [0,2**10],
 *                                  ledc_channel_config_t.ledc_pre_depth : ledc channel duty depth when different channel ,select same time ,their
 *                                                                       fre_hz and ledc_pre_depth must be same
 * @return	  None
 *
 */
void ledc_pwm_channel_config(enum ledc_pwm_mode mode,ledc_channel_config_t* ledc_channel_config);

/**
 * @brief	   ledc start
 *
 * After call ledc_pwm_set_fre,ledc_pwm_set_duty,ledc_pwm_set_gradient need to call this function
 *
 * @param[in]  mode    : select the ledc mode,high mode and low mode,now we only support high mode ,next will add low mode
 *
 * @param[in]  channel : current configuration channel
 *
 * @return	  None
 *
 */
void ledc_pwm_start(enum ledc_pwm_mode mode,enum ledc_pwm_channel channel);

/**
 * @brief	   ledc stop
 *
 * Disable ledc output,and set idle level
 *
 * @param[in]  mode    : select the ledc mode,high mode and low mode,now we only support high mode ,next will add low mode
 *
 * @param[in]  channel : current configuration channel
 *
 * @return	  None
 *
 */
void ledc_pwm_stop(enum ledc_pwm_mode mode,enum ledc_pwm_channel channel,uint8_t idle_level);

/**
 * @brief	   ledc set fre
 *
 * Set ledc fre ,the unit is Hz,After the function calls the ledc_pwm_start function, the function can take effect.
 *
 * @param[in]  mode    : select the ledc mode,high mode and low mode,now we only support high mode ,next will add low mode
 *
 * @param[in]  channel : current configuration channel
 *
 * @param[in]  fre_hz  : set the ledc fre
 *
 * @return	  None
 *
 */
void ledc_pwm_set_fre(enum ledc_pwm_mode mode,enum ledc_pwm_channel channel,uint32_t fre_hz);

/**
 * @brief	   ledc set duty
 *
 * Set ledc duty ,After the function calls the ledc_pwm_start function, the function can take effect.
 *
 * @param[in]  mode    : select the ledc mode,high mode and low mode,now we only support high mode ,next will add low mode
 *
 * @param[in]  channel : current configuration channel
 *
 * @param[in]  duty  : set the ledc duty ,if the ledc_pre_depth = 10,the duty range is [0,2**10]
 *
 * @return	  None
 *
 */
void ledc_pwm_set_duty(enum ledc_pwm_mode mode,enum ledc_pwm_channel channel,uint16_t duty);

/**
 * @brief	   ledc get duty
 *
 * @param[in]  mode    : select the ledc mode,high mode and low mode,now we only support high mode ,next will add low mode
 *
 * @param[in]  channel : current configuration channel
 *
 *
 * @return	  current ledc duty
 *
 */
uint32_t ledc_pwm_get_duty(enum ledc_pwm_mode mode,enum ledc_pwm_channel channel);

/**
 * @brief	   ledc get fre
 *
 * @param[in]  mode    : select the ledc mode,high mode and low mode,now we only support high mode ,next will add low mode
 *
 * @param[in]  channel : current configuration channel
 *
 * @return	  current ledc fre
 *
 */
uint32_t ledc_pwm_get_fre_hz(enum ledc_pwm_mode mode,enum ledc_pwm_channel channel);

/**
 * @brief	    register ledc interrupt occur callback function
 *
 * User this Function,register interrupt event callback . when ledc interrupt is triggered , the event callback
 * will give appliacation .ledc_event_callback is called by ledc interrupt , so the ledc_event_callback
 * should be completed in a very short period of time. If you want to deal with a lot of things,Please Creat
 * a task ,and post message from the gpio_event_callback to the task.
 *
 * @param[in]  func : the func will be called,when ledc interrupt is triggered ,and the param of func is the
 *                    ledc interrupt channl and the mode
 *
 *
 * @return	  None
 *
 */
void ledc_register_event_callback(ledc_event_callback func);

/**
 * @brief	   ledc set gradient
 *
 * Set ledc gradient , After the function calls the ledc_pwm_start function , the function can take effect.
 *
 * @param[in]  mode                 : select the ledc mode,high mode and low mode,now we only support high mode ,next will add low mode
 *
 * @param[in]  channel              : current configuration channel
 *
 * @param[in]  duty                 : set the start of the gradient duty , if the ledc_pre_depth = 10,the duty range is [0,2**10]
 *
 * @param[in]  gradule_direction    : set the direction of the gradient
 *
 * @param[in]  gradule_num          : set the number of the gradient
 *
 * @param[in]  duty_cyle_num        : set how many ledc pwm tick each time the gradient lasts
 *
 * @param[in]  duty_scale           : set gradient change amplitude

 * @return	  None
 *
 */
void ledc_pwm_set_gradient(enum ledc_pwm_mode mode,uint8_t channel,uint32_t duty,enum ledc_pwm_increase_direction gradule_direction,
        uint16_t gradule_num,uint16_t duty_cyle_num,uint16_t duty_scale);
/**
 * @brief   register ledc interrupt handler
 *
 * @param   uint8_t ledc_intr_num     : ledc interrupt num,check the info in soc.h, and please see the core-isa.h for more details
 * @param   void (* fn)(void* )       : intr handler function
 * @param   void * arg                : parameter for handler
 *
 * @return  null
 */
void ledc_intr_handler_register(uint8_t ledc_intr_num,void (* fn)(void* ),void * arg);

/**
 * @brief   default ledc interrupt init function to initialize the ledc intrrupt parameters.
 *
 * @param   uint8_t pcnt_intr_num    : ledc interrupt num,check the info in soc.h, and please see the core-isa.h for more details
 *
 * @return  null
 */
void ledc_intr_init_default(uint8_t ledc_intr_num);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_LEDC_PWM_H_ */
