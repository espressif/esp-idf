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

#ifndef _DRIVER_PCNT_H_
#define _DRIVER_PCNT_H_

#include <stdbool.h>
#include "soc/dport_reg.h"
#include "soc/pcnt_reg.h"

#ifdef __cplusplus
extern "C" {
#endif
//register address redefine,just only use simple
#define PCNT_UX_CONF0(Channel) (PCNT_U0_CONF0_REG+(Channel)*12)
#define PCNT_UX_CONF1(Channel) (PCNT_U0_CONF1_REG+(Channel)*12)
#define PCNT_UX_CONF2(Channel) (PCNT_U0_CONF2_REG+(Channel)*12)
#define PCNT_CNT_CURRENT_VALUE(Channel) ( PCNT_U0_CNT_REG+((Channel)*4))
#define PCNT_UX_STATUS(Channel) (PCNT_U0_STATUS_REG+(Channel)*4)
//if user not use pin param ,please give this define
#define PCNT_INVALID_PIN_PARAM (40)

typedef enum {
    PCNT_CHANNEL0=0,
    PCNT_CHANNEL1=1,
    PCNT_CHANNEL2=2,
    PCNT_CHANNEL3=3,
    PCNT_CHANNEL4=4,
    PCNT_CHANNEL5=5,
    PCNT_CHANNEL6=6,
    PCNT_CHANNEL7=7,
}enum_pcnt_channel_t;
//Control Mode
enum pcnt_ctrl_high_level{
    PCNT_H_LEVEL_INCREASE=0,
    PCNT_H_LEVEL_DECREASE=1,
    PCNT_H_LEVEL_FORBIDDEN=2,
};
enum pcnt_ctrl_low_level{
    PCNT_L_LEVEL_INCREASE=0,
    PCNT_L_LEVEL_DECREASE=1,
    PCNT_L_LEVEL_FORBIDDEN=2,
};
//Sig Mode
enum pcnt_sig_positive_edge{
    PCNT_POSEDGE_INCREASE=1,
    PCNT_POSEDGE_DECREASE=2,
    PCNT_POSEDGE_FORBIDDEN=0,
};
enum pcnt_sig_negative_edge{
    PCNT_NEGEDGE_INCREASE=1,
    PCNT_NEGEDGE_DECREASE=2,
    PCNT_NEGEDGE_FORBIDDEN=0,
};

//Gate Config
typedef struct {
    uint8_t ctrl_pin_num;
    uint8_t sig_pin_num;
    enum pcnt_ctrl_high_level ctrl_high_level;
    enum pcnt_ctrl_low_level ctrl_low_level;
    enum pcnt_sig_positive_edge sig_positive_edge;
    enum pcnt_sig_negative_edge sig_negative_edge;
}pcnt_gate_t;
//Channel Config
typedef struct {
    enum_pcnt_channel_t channel;
    int16_t L_limit;
    int16_t H_limit;
    uint16_t Filter_tick;
    pcnt_gate_t * Gate0;
    pcnt_gate_t * Gate1;
}Pcnt_channel_config;

typedef enum pcnt_intr_type{
    PCNT_CNT_EQU_THRESH1=BIT(2),
    PCNT_CNT_EQU_THRESH0=BIT(3),
    PCNT_CNT_LESS_THAN_LIMIT_L=BIT(4),
    PCNT_CNT_EXCEED_LIMIT_H=BIT(5),
    PCNT_CNT_ZERO_RELATED=BIT(6),
}enum_intr_type_t;

enum pcnt_count_zero_intr_type{
    PCNT_CNT_INCREASE_TO_ZERO=0,
    PCNT_CNT_DECREASE_TO_ZERO=1,
    PCNT_CNT_NEGATIVE=2,
    PCNT_CNT_POSITIVE=3,
};

enum pcnt_intr_mask{
    PCNT_THRESH1_INTR_ENABLE=PCNT_THR_THRES1_EN_U0,
    PCNT_THRESH0_INTR_ENABLE=PCNT_THR_THRES0_EN_U0,
    PCNT_L_LIMIT_INTR_ENABLE=PCNT_THR_L_LIM_EN_U0,
    PCNT_H_LIMIT_INTR_ENABLE=PCNT_THR_H_LIM_EN_U0,
    PCNT_ZERO_INTR_ENABLE=PCNT_THR_ZERO_EN_U0,
    PCNT_ALL_INTR_ENABLE = (PCNT_THR_THRES1_EN_U0|PCNT_THR_THRES0_EN_U0|PCNT_THR_L_LIM_EN_U0|PCNT_THR_H_LIM_EN_U0|PCNT_THR_ZERO_EN_U0)
};

typedef struct{
    enum_pcnt_channel_t channel;
    int16_t thres1;
    int16_t thres0;
    uint32_t PcntIntrEnMask;
}pcnt_intr_config;

typedef struct pcnt_event_message{
    enum_pcnt_channel_t channel;
    enum_intr_type_t intr_type;
}pcnt_event_mes;

typedef void (*pcnt_event_callback)(pcnt_event_mes pcnt_message);

/**
 * @brief	  resert pcnt counter,the pcnt cnt is cleared zero
 *
 * @param[in]  channel : the channel of the pcnt
 *
 * @return	  true    : reset succed
 *             fail    : reset fail
 *
 */
bool pcnt_reset_counter(enum_pcnt_channel_t channel);

/**
 * @brief	  start pnct counter
 *
 * @param[in]  channel : the channel of the pcnt
 *
 * @return	  None
 *
 */
void pcnt_start_counter(enum_pcnt_channel_t channel);

/**
 * @brief	   get the pcnt counter value
 *
 * @param[in]  channel : the channel of the pcnt
 *
 * @return	  the value of the pcnt
 *
 */
int16_t pcnt_get_counter_value(enum_pcnt_channel_t channel);

/**
 * @brief	   enable the pcnt pcnt interrupt
 *
 * @param[in]  channel : the channel of the pcnt
 *
 * @return	  None
 *
 */
void pcnt_enable_channel_intr(enum_pcnt_channel_t channel);

/**
 * @brief	   disable the pcnt pcnt interrupt
 *
 * @param[in]   channel : the channel of the pcnt
 *
 * @return	  None
 *
 */
void pcnt_disable_channel_intr(enum_pcnt_channel_t channel);
/**
 * @brief	    register pcnt interrupt occur callback function
 *
 * Use this Function,register interrupt event callback . when pcnt interrupt is triggered , the event callback
 * will give appliacation .pcnt_event_callback is called by pcnt interrupt , so the gpio_event_callback
 * should be completed in a very short period of time. If you want to deal with a lot of things,Please Creat
 * a task ,and post message from the pcnt_event_callback to the task.
 *
 * @param[in]  func : the func will be called,when pcnt interrupt is triggered ,and the param of func is the
 *                    number of the pnct channel occurs interrupt and intr type
 *
 *
 * @return	  None
 *
 */
void pcnt_register_event_callback(pcnt_event_callback func);

/**
 * @brief	   config the pcnt pcnt interrupt
 *
 * @param[in]   intr_config   intr_config.channel        : the channel of pcnt
 *                            intr_config.thres1         : the intr threshold1
 *                            intr_config.thres0         : the intr threshold0
 *                            intr_config.PcntIntrEnMask : reference the enum pcnt_intr_mask.if user want to enable thres1 intr
 *                                                          and thres0 intr,PcntIntrEnMask=PCNT_THRESH1_INTR_ENABLE|PCNT_THRESH0_INTR_ENABLE.
 *
 * @return	  None
 *
 */
void pcnt_set_intr(pcnt_intr_config* intr_config);

/**
 * @brief	   pcnt common configuration
 *
 * @param[in]   channel_config   channel_config.channel     : the channel of pcnt
 *                               channel_config.L_limit     : the counter range [L_limit,H_limit]
 *                               channel_config.H_limit     : the counter range [L_limit,H_limit]
 *                               channel_config.Filter_tick : the counter filte tick cnt ,tick=80M
 *                               channel_config.Gate0       : config the Gate0
 *                               channel_config.Gate1       : config the Gate1
 *
 * @return	  None
 *
 */
void pcnt_init_channel(Pcnt_channel_config *channel_config);

/**
 * @brief   register pcnt interrupt handler
 *
 * @param   uint8_t pcnt_intr_num     : pcnt interrupt num,check the info in soc.h, and please see the core-isa.h for more details
 * @param   void (* fn)(void* )       : intr handler function
 * @param   void * arg                : parameter for handler
 *
 * @return  null
 */
void pcnt_intr_handler_register(uint8_t pcnt_intr_num,void (* fn)(void* ),void * arg);

/**
 * @brief   default pcnt interrupt init function to initialize the pcnt intrrupt parameters.
 *
 * @param   uint8_t pcnt_intr_num    : pcnt interrupt num,check the info in soc.h, and please see the core-isa.h for more details
 *
 * @return  null
 */
void pcnt_intr_init_default(uint8_t pcnt_intr_num);
#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_PCNT_H_ */
