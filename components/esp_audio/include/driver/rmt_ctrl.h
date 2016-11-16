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

#ifndef _DRIVER_RMT_CTRL_H_
#define _DRIVER_RMT_CTRL_H_

#include <stdbool.h>
#include "soc/rmt_reg.h"
#include "soc/dport_reg.h"
#include "driver/gpio.h"
#ifdef __cplusplus
extern "C" {
#endif

#define RMT_CHANNEL_MEMORY_CHAR_CNT (256)
#define RMT_CHANNEL_MEMORY_WORD_CNT (RMT_CHANNEL_MEMORY_CHAR_CNT/4)
#define RMT_DIV_CLK_KHZ        (320)//Do not change this

#define RMT_CHXCONF1(CHANNEL) ((RMT_CH0CONF1_REG)+((CHANNEL)*0X08))
#define RMT_CHXCONF0(CHANNEL) ((RMT_CH0CONF0_REG)+((CHANNEL)*0X08))
#define RMT_CHXSTATUS(CHANNEL) ((RMT_CH0STATUS_REG)+((CHANNEL)*0X04))
#define RMT_CHXCARRIER_DUTY(CHANNEL) ((RMT_CH0CARRIER_DUTY_REG)+(CHANNEL)*0X04)
#define RMT_CHXADDR_REG(CHANNEL)  (RMT_CH0ADDR_REG+((CHANNEL)*0X04))
#define RMT_CHXSTATUS_REG(CHANNEL) (RMT_CH0STATUS_REG+((CHANNEL)*0X04))
#define RMT_CHX_MEMEOY(channel) (DR_REG_RMT_BASE+0X800+0X4*(channel*0x40))

//Config0
#define RMT_GET_REG_CARRIER_OUT_LV_CHX(CHANNEL)  GET_PERI_REG_BITS2(RMT_CHXCONF0(CHANNEL),RMT_CARRIER_OUT_LV_CH0_V, RMT_CARRIER_OUT_LV_CH0_S)
#define RMT_SET_REG_CARRIER_OUT_LV_CHX(CHANNEL,val)  SET_PERI_REG_BITS(RMT_CHXCONF0(CHANNEL),RMT_CARRIER_OUT_LV_CH0_V,(val),RMT_CARRIER_OUT_LV_CH0_S)
#define RMT_GET_REG_CARRIER_EN_CHX(CHANNEL)  GET_PERI_REG_BITS2(RMT_CHXCONF0(CHANNEL),RMT_CARRIER_EN_CH0_V, RMT_CARRIER_EN_CH0_S)
#define RMT_SET_REG_CARRIER_EN_CHX(CHANNEL,val)  SET_PERI_REG_BITS(RMT_CHXCONF0(CHANNEL),RMT_CARRIER_EN_CH0_V,(val),RMT_CARRIER_EN_CH0_S)
#define RMT_GET_REG_MEM_SIZE_CHX(CHANNEL)	GET_PERI_REG_BITS2(RMT_CHXCONF0(CHANNEL),RMT_MEM_SIZE_CH0_V, RMT_MEM_SIZE_CH0_S)
#define RMT_SET_REG_MEM_SIZE_CHX(CHANNEL,val)  SET_PERI_REG_BITS(RMT_CHXCONF0(CHANNEL),RMT_MEM_SIZE_CH0_V,(val),RMT_MEM_SIZE_CH0_S)
#define RMT_GET_REG_IDLE_THRES_CHX(CHANNEL)  GET_PERI_REG_BITS2(RMT_CHXCONF0(CHANNEL),RMT_IDLE_THRES_CH0_V, RMT_IDLE_THRES_CH0_S)
#define RMT_SET_REG_IDLE_THRES_CHX(CHANNEL,val)  SET_PERI_REG_BITS(RMT_CHXCONF0(CHANNEL),RMT_IDLE_THRES_CH0_V,(val),RMT_IDLE_THRES_CH0_S)
#define RMT_GET_REG_DIV_CNT_CHX(CHANNEL)  GET_PERI_REG_BITS2(RMT_CHXCONF0(CHANNEL),RMT_DIV_CNT_CH0_V, RMT_DIV_CNT_CH0_S)
#define RMT_SET_REG_DIV_CNT_CHX(CHANNEL,val)  SET_PERI_REG_BITS(RMT_CHXCONF0(CHANNEL),RMT_DIV_CNT_CH0_V,(val),RMT_DIV_CNT_CH0_S)
//Config1
#define RMT_GET_REG_IDLE_OUT_EN_CHX(CHANNEL)  GET_PERI_REG_BITS2(RMT_CHXCONF1(CHANNEL), RMT_IDLE_OUT_EN_CH0_V, RMT_IDLE_OUT_EN_CH0_S)
#define RMT_SET_REG_IDLE_OUT_EN_CHX(CHANNEL,val)  SET_PERI_REG_BITS(RMT_CHXCONF1(CHANNEL),RMT_IDLE_OUT_EN_CH0_V,(val),RMT_IDLE_OUT_EN_CH0_S)
#define RMT_GET_REG_IDLE_OUT_LV_CHX(CHANNEL)  GET_PERI_REG_BITS2(RMT_CHXCONF1(CHANNEL), RMT_IDLE_OUT_LV_CH0_V, RMT_IDLE_OUT_LV_CH0_S)
#define RMT_SET_REG_IDLE_OUT_LV_CHX(CHANNEL,val)  SET_PERI_REG_BITS(RMT_CHXCONF1(CHANNEL) ,RMT_IDLE_OUT_LV_CH0_V,(val),RMT_IDLE_OUT_LV_CH0_S)
#define RMT_GET_REG_REF_ALWAYS_ON_CHX(CHANNEL)  GET_PERI_REG_BITS2(RMT_CHXCONF1(CHANNEL), RMT_REF_ALWAYS_ON_CH0_V, RMT_REF_ALWAYS_ON_CH0_S)
#define RMT_SET_REG_REF_ALWAYS_ON_CHX(CHANNEL,val)	SET_PERI_REG_BITS(RMT_CHXCONF1(CHANNEL),RMT_REF_ALWAYS_ON_CH0_V,(val),RMT_REF_ALWAYS_ON_CH0_S)
#define RMT_GET_REG_REF_CNT_RST_CHX(CHANNEL)  GET_PERI_REG_BITS2(RMT_CHXCONF1(CHANNEL), RMT_REF_CNT_RST_CH0_V, RMT_REF_CNT_RST_CH0_S)
#define RMT_SET_REG_REF_CNT_RST_CHX(CHANNEL,val)  SET_PERI_REG_BITS(RMT_CHXCONF1(CHANNEL),RMT_REF_CNT_RST_CH0_V,(val),RMT_REF_CNT_RST_CH0_S)
#define RMT_GET_REG_RX_FILTER_THRES_CHX(CHANNEL)  GET_PERI_REG_BITS2(RMT_CHXCONF1(CHANNEL), RMT_RX_FILTER_THRES_CH0_V, RMT_RX_FILTER_THRES_CH0_S)
#define RMT_SET_REG_RX_FILTER_THRES_CHX(CHANNEL,val)  SET_PERI_REG_BITS(RMT_CHXCONF1(CHANNEL),RMT_RX_FILTER_THRES_CH0_V,(val),RMT_RX_FILTER_THRES_CH0_S)
#define RMT_GET_REG_RX_FILTER_EN_CHX(CHANNEL)	GET_PERI_REG_BITS2(RMT_CHXCONF1(CHANNEL), RMT_RX_FILTER_EN_CH0_V, RMT_RX_FILTER_EN_CH0_S)
#define RMT_SET_REG_RX_FILTER_EN_CHX(CHANNEL,val)  SET_PERI_REG_BITS(RMT_CHXCONF1(CHANNEL),RMT_RX_FILTER_EN_CH0_V,(val),RMT_RX_FILTER_EN_CH0_S)
#define RMT_GET_REG_TX_CONTI_MODE_CHX(CHANNEL)  GET_PERI_REG_BITS2(RMT_CHXCONF1(CHANNEL), RMT_TX_CONTI_MODE_CH0_V, RMT_TX_CONTI_MODE_CH0_S)
#define RMT_SET_REG_TX_CONTI_MODE_CHX(CHANNEL,val)	SET_PERI_REG_BITS(RMT_CHXCONF1(CHANNEL),RMT_TX_CONTI_MODE_CH0_V,(val),RMT_TX_CONTI_MODE_CH0_S)
#define RMT_GET_REG_MEM_OWNER_CHX(CHANNEL)  GET_PERI_REG_BITS2(RMT_CHXCONF1(CHANNEL), RMT_MEM_OWNER_CH0_V, RMT_MEM_OWNER_CH0_S)
#define RMT_SET_REG_MEM_OWNER_CHX(CHANNEL,val)	SET_PERI_REG_BITS(RMT_CHXCONF1(CHANNEL),RMT_MEM_OWNER_CH0_V,(val),RMT_MEM_OWNER_CH0_S)
#define RMT_GET_REG_APB_MEM_RST_CHX(CHANNEL)  GET_PERI_REG_BITS2(RMT_CHXCONF1(CHANNEL), RMT_APB_MEM_RST_CH0_V, RMT_APB_MEM_RST_CH0_S)
#define RMT_SET_REG_APB_MEM_RST_CHX(CHANNEL,val)  SET_PERI_REG_BITS(RMT_CHXCONF1(CHANNEL) ,RMT_APB_MEM_RST_CH0_V,(val),RMT_APB_MEM_RST_CH0_S)
#define RMT_GET_REG_MEM_RD_RST_CHX(CHANNEL)  GET_PERI_REG_BITS2(RMT_CHXCONF1(CHANNEL) , RMT_MEM_RD_RST_CH0_V, RMT_MEM_RD_RST_CH0_S)
#define RMT_SET_REG_MEM_RD_RST_CHX(CHANNEL,val)  SET_PERI_REG_BITS(RMT_CHXCONF1(CHANNEL) ,RMT_MEM_RD_RST_CH0_V,(val),RMT_MEM_RD_RST_CH0_S)
#define RMT_GET_REG_MEM_WR_RST_CHX(CHANNEL)  GET_PERI_REG_BITS2(RMT_CHXCONF1(CHANNEL) , RMT_MEM_WR_RST_CH0_V, RMT_MEM_WR_RST_CH0_S)
#define RMT_SET_REG_MEM_WR_RST_CHX(CHANNEL,val)  SET_PERI_REG_BITS(RMT_CHXCONF1(CHANNEL) ,RMT_MEM_WR_RST_CH0_V,(val),RMT_MEM_WR_RST_CH0_S)
#define RMT_GET_REG_RX_EN_CHX(CHANNEL)  GET_PERI_REG_BITS2(RMT_CHXCONF1(CHANNEL) , RMT_RX_EN_CH0_V, RMT_RX_EN_CH0_S)
#define RMT_SET_REG_RX_EN_CHX(CHANNEL,val)	SET_PERI_REG_BITS(RMT_CHXCONF1(CHANNEL) ,RMT_RX_EN_CH0_V,(val),RMT_RX_EN_CH0_S)
#define RMT_GET_REG_TX_START_CHX(CHANNEL)	GET_PERI_REG_BITS2(RMT_CHXCONF1(CHANNEL) , RMT_TX_START_CH0_V, RMT_TX_START_CH0_S)
#define RMT_SET_REG_TX_START_CHX(CHANNEL,val)  SET_PERI_REG_BITS(RMT_CHXCONF1(CHANNEL) ,RMT_TX_START_CH0_V,(val),RMT_TX_START_CH0_S)
//Carrier
#define RMT_GET_REG_CARRIER_HIGH_CHX(CHANNEL)	GET_PERI_REG_BITS2(RMT_CHXCARRIER_DUTY(CHANNEL) , RMT_CARRIER_HIGH_CH0_V, RMT_CARRIER_HIGH_CH0_S)
#define RMT_SET_REG_CARRIER_HIGH_CHX(CHANNEL,val)  SET_PERI_REG_BITS(RMT_CHXCARRIER_DUTY(CHANNEL) ,RMT_CARRIER_HIGH_CH0_V,(val),RMT_CARRIER_HIGH_CH0_S)
#define RMT_GET_REG_CARRIER_LOW_CHX(CHANNEL)  GET_PERI_REG_BITS2(RMT_CHXCARRIER_DUTY(CHANNEL) , RMT_CARRIER_LOW_CH0_V, RMT_CARRIER_LOW_CH0_S)
#define RMT_SET_REG_CARRIER_LOW_CHX(CHANNEL,val)  SET_PERI_REG_BITS(RMT_CHXCARRIER_DUTY(CHANNEL) ,RMT_CARRIER_LOW_CH0_V,(val),RMT_CARRIER_LOW_CH0_S)

#define RMT_GET_MEM_TX_WRAP_EN()  GET_PERI_REG_BITS2(RMT_APB_CONF_REG , RMT_MEM_TX_WRAP_EN_V, RMT_MEM_TX_WRAP_EN_S)
#define RMT_SET_MEM_TX_WRAP_EN(val)  SET_PERI_REG_BITS(RMT_APB_CONF_REG ,RMT_MEM_TX_WRAP_EN_V,(val),RMT_MEM_TX_WRAP_EN_S)
#define RMT_GET_APB_FIFO_MASK()  GET_PERI_REG_BITS2(RMT_APB_CONF_REG , RMT_APB_FIFO_MASK_V, RMT_APB_FIFO_MASK_S)
#define RMT_SET_APB_FIFO_MASK(val)  SET_PERI_REG_BITS(RMT_APB_CONF_REG ,RMT_APB_FIFO_MASK_V,(val),RMT_APB_FIFO_MASK_S)

enum rmt_channel{
    RMT_CTRL_CHANNEL0=0, /**< Rmt Channel0 */
    RMT_CTRL_CHANNEL1,   /**< Rmt Channel1 */
    RMT_CTRL_CHANNEL2,   /**< Rmt Channel2 */
    RMT_CTRL_CHANNEL3,   /**< Rmt Channel3 */
    RMT_CTRL_CHANNEL4,   /**< Rmt Channel4 */
    RMT_CTRL_CHANNEL5,   /**< Rmt Channel5 */
    RMT_CTRL_CHANNEL6,   /**< Rmt Channel6 */
    RMT_CTRL_CHANNEL7,   /**< Rmt Channel7 */
};
enum rmt_ram_owner{
    RMT_RAM_OWNER_HARDWARE=0,/**< Rmt Rx mode ,Memory owner RMT_RAM_OWNER_HARDWARE */
    RMT_RAM_OWNER_SOFTWARE=1,/**< Rmt Rx mode ,Memory owner RMT_RAM_OWNER_SOFTWARE */
};
enum rmt_source_clk{
    RMT_BASECLK_REF=0,/**< Rmt Clk is Refclk defautl is 1MHz */
    RMT_BASECLK_APB,/**< Rmt Clk is APB CLK defautl is 80MHz */
};
enum rmt_ctrl_mode{
    RMT_TX_MODE=0,/**< Rmt TX Mode */
    RMT_RX_MODE,  /**< Rmt RX Mode */
};
enum rmt_idle_level{
    RMT_IDLE_LOW_LEVEL=0,/**< Rmt TX IDLE Low Level */
    RMT_IDLE_HIGH_LEVEL, /**< Rmt TX IDLE High Level */
};
enum rmt_carrier_level{
    RMT_CARRIER_LOW_LEVEL=0,/**< Rmt Carrier Low Level */
    RMT_CARRIER_HIGH_LEVEL, /**< Rmt Carrier High Level */
};

typedef struct rmt_ctrl_tx{
    uint32_t carrier_fre_Hz;             /**< Rmt Carrier Fre */
    uint8_t carrier_duty;                /**< Rmt Carrier duty */
    enum rmt_idle_level idle_level;      /**< Rmt Idle level */
    enum rmt_carrier_level carrier_level;/**< Rmt Carrier level */
}rmt_ctrl_tx_config_t;

typedef struct rmt_ctrl_rx{
    uint8_t filter_tick_cnt;       /**< Rmt filter tick  cnt */
    uint16_t rx_threshold_tick_cnt;/**< Rmt rx  interrupt threshold */
}rmt_ctrl_rx_config_t;

typedef struct rmt_ctrl_config{
    enum rmt_ctrl_mode rmt_mode;
    enum rmt_channel channel;
    gpio_num_t gpio_num;
    uint8_t channel_mem_cnt;//NAME
    bool intr_enable;
    union{
        rmt_ctrl_tx_config_t tx_config;
        rmt_ctrl_rx_config_t rx_config;
    }mode_config;
}rmt_ctrl_channel_config_t;

typedef struct rmt_event_message{
    enum rmt_ctrl_mode mode;
    enum rmt_channel channel;
}rmt_event_msg_t;

typedef void (*rmt_event_callback)(rmt_event_msg_t rmt_message);

/**
 * @brief	    register rmt interrupt occur callback function
 *
 * Use this Function,register interrupt event callback . when rmt interrupt is triggered , the event callback
 * will give appliacation .rmt_event_callback is called by rmt interrupt , so the rmt_event_callback
 * should be completed in a very short period of time. If you want to deal with a lot of things,Please Creat
 * a task ,and post message from the rmt_event_callback to the task.

 * @param[in]  func : the func will be called,when rmt interrupt is triggered ,and the param of func is the
 *                    mode of rmt and channel of rmt
 *
 *
 * @return	  None
 *
 */
void rmt_register_event_callback(rmt_event_callback func);

/**
 * @brief	   rmt common configuration
 *
 * User this Function,config rmt the channel tx mode or rx mode
 *
 * @param[in]  rmt_config  rmt_config.rmt_mode              : select rmt tx mode or rx mode.
 *                         rmt_config.rmt_channel           : current configuration channel.
 *                         rmt_config.gpio_num              : tx or rx mode ,signagle use the gpio.
 *                         rmt_config.channel_mem_cnt       : the tx or rx used the memory block cnt,every mem cnt have 256Byte contents
 *                         rmt_config.intr_enable           : enable or disable tx or rx interrupt.when enable tx mode , send the tx memory end ,will trigger
 *                                                            tx event . when enable rx mode ,rx line idle time beyond rx_threshold_tick_cnt will trigger rx
 *                                                            event.
 *                         rmt_config.mode_config.tx_config : tx common config.
 *                         rmt_config.mode_config.rx_config : rx common config.
 *
 * @return	  None
 *
 */
void rmt_ctrl_init(rmt_ctrl_channel_config_t* rmt_config);

/**
 * @brief	   write rmt tx memory
 *
 * When rmt tx memory content is 0x00,the tx will stop,and trigger tx intr event
 *
 * @param[in]  channel  : rmt tx channel
 *             mem      : write the mem to tx memory
 *             cnt      : the cnt max is   rmt_config.channel_mem_cnt *32
 *
 * @return	  None
 *
 */
void rmt_ctrl_tx_wr_memory(enum rmt_channel channel,uint32_t* mem,uint16_t cnt);

/**
 * @brief	   rmt rx start
 *
 * @param[in]  channel  : rmt rx channel
 *
 * @return	  None
 *
 */
void rmt_ctrl_rx_start(enum rmt_channel channel);

/**
 * @brief	   rmt tx start
 *
 * @param[in]  channel  : rmt tx channel
 *
 * @return	  None
 *
 */
void rmt_ctrl_tx_start(enum rmt_channel channel);

/**
 * @brief	  rmt read rx memory
 *
 * @param[in]  channel  : rmt rx channel
 *
 * @param[in]  mem      : the rx memory data copy to the mem,the mem content cnt = rmt_config.channel_mem_cnt *32
 *
 * @return	  rx memory  number of received data
 *
 */
uint16_t rmt_ctrl_rx_rd_memory(enum rmt_channel channel,uint32_t* mem);
/**
 * @brief   register rmt interrupt handler
 *
 * @param   uint8_t ledc_intr_num     : rmt interrupt num,check the info in soc.h, and please see the core-isa.h for more details
 * @param   void (* fn)(void* )       : intr handler function
 * @param   void * arg                : parameter for handler
 *
 * @return  null
 */
void rmt_intr_handler_register(uint8_t rmt_intr_num,void (* fn)(void* ),void * arg);
/**
 * @brief   default rmt interrupt init function to initialize the rmt intrrupt parameters.
 *
 * @param   uint8_t rmt_intr_num    : ledc interrupt num,check the info in soc.h, and please see the core-isa.h for more details
 *
 * @return  null
 */

void rmt_intr_init_default(uint8_t rmt_intr_num);


#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_RMT_CTRL_H_ */
