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
#ifndef _DRIVER_I2C_H_
#define _DRIVER_I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_types.h"
#include "soc/i2c_reg.h"
#include "soc/soc.h"
#include "soc/dport_reg.h"

#define I2C_DEV(i)   ((i2c_dev_t *)(REG_I2C_BASE(i)))
#define I2C_APB_CLK_FREQ  APB_CLK_FREQ
#define I2C_COMD_REG(i2c_no, cmd_no) (I2C_COMD0_REG(i2c_no)+ (cmd_no)*4 )
#define I2C_COMMAND_DONE (BIT(31))
#define I2C_COMMAND_DONE_M (BIT(31))
#define I2C_COMMAND_DONE_V 0x1
#define I2C_COMMAND_DONE_S 31
#define I2C_COMMAND_OP_CODE   0x7
#define I2C_COMMAND_OP_CODE_M  ((I2C_COMMAND_OP_CODE_V)<<(I2C_COMMAND_OP_CODE_S))
#define I2C_COMMAND_OP_CODE_V 0x7
#define I2C_COMMAND_OP_CODE_S  11
#define I2C_COMMAND_ACK_VALUE   (BIT(10))
#define I2C_COMMAND_ACK_VALUE_M (BIT(10))
#define I2C_COMMAND_ACK_VALUE_V     0x1
#define I2C_COMMAND_ACK_VALUE_S     10
#define I2C_COMMAND_ACK_EXP     (BIT(9))
#define I2C_COMMAND_ACK_EXP_M   (BIT(9))
#define I2C_COMMAND_ACK_EXP_V       0x1
#define I2C_COMMAND_ACK_EXP_S       9
#define I2C_COMMAND_ACK_EN      (BIT(8))
#define I2C_COMMAND_ACK_EN_M	(BIT(8))
#define I2C_COMMAND_ACK_EN_V		0x1
#define I2C_COMMAND_ACK_EN_S		8
#define I2C_COMMAND_BYTE_NUM     0xff
#define I2C_COMMAND_BYTE_NUM_M	 0xff
#define I2C_COMMAND_BYTE_NUM_V	 0xff
#define I2C_COMMAND_BYTE_NUM_S	    0

typedef enum{
    I2C_SLAVE_MODE = 0,
    I2C_MASTER_MODE,
}enum_i2c_mode_t;

typedef enum{
    I2C_CMD_RESTART = 0,
    I2C_CMD_WRITE,
    I2C_CMD_READ,
    I2C_CMD_STOP,
    I2C_CMD_END
}enum_cmd_opcode_t;

typedef enum{
    I2C0 = 0,  // 0x3ff53000
    I2C1 ,     // 0x3ff67000
    I2C_MAX
} enum_i2c_port_t;

typedef struct{
    enum_i2c_mode_t mode;
    uint8_t addr_10bit_en;
    uint16_t slave_addr;
    uint32_t clk_speed;
}i2c_config_t;

typedef struct{
    uint8_t byte_num;
    uint8_t ack_en;
    uint8_t ack_exp;
    uint8_t ack_val;
    enum_cmd_opcode_t op_code;
}i2c_cmd_def_t;


/**
 * @brief  Reset I2C tx fifo, flush the data in tx fifo.
 *
 * @param  enum_i2c_port_t i2c_no : I2C port num (I2C0 or I2C1)
 *
 * @return null
 */
void i2c_reset_tx_fifo(enum_i2c_port_t i2c_no );

/**
 * @brief  Fill the tx fifo with the given data bytes
 *
 * @param  enum_i2c_port_t i2c_no : I2C port num (I2C0 or I2C1)
 * @param  uint8_t *data          : pointer to the data buffer
 * @param  uint8_t len            : data length of the data
 *
 * @return null
 */
void i2c_tx_fifo_push(enum_i2c_port_t i2c_no,uint8_t *data , uint8_t len);

/**
 * @brief  Reset I2C rx fifo, flush the data in rx fifo.
 *
 * @param  enum_i2c_port_t i2c_no : I2C port num (I2C0 or I2C1)
 *
 * @return null
 */
void i2c_reset_rx_fifo(enum_i2c_port_t i2c_no);

/**
 * @brief  Fill the tx fifo with the given data bytes
 *
 * @param  enum_i2c_port_t i2c_no : I2C port num (I2C0 or I2C1)
 * @param  uint8_t cmd_idx        : i2c command index(at most 16 commands one time)
 * @param  i2c_cmd_def_t* cmd     : command definition.
 *
 * @return null
 */
void i2c_config_cmd(enum_i2c_port_t i2c_no,uint8_t cmd_idx, i2c_cmd_def_t* cmd);

/**
 * @brief  Start sending data to i2c bus.The data is defined in the command registers.Stop when there is a "STOP" command
 *
 * @param  enum_i2c_port_t i2c_no : I2C port num (I2C0 or I2C1)
 *
 * @return null
 */
void i2c_start(enum_i2c_port_t i2c_no);

/**
 * @brief  Set and initialize i2c params
 *
 * @param  enum_i2c_port_t i2c_no : I2C port num (I2C0 or I2C1)
 * @param  i2c_config_t* i2c_conf : i2c setting param struct
 *
 * @return null
 */
void i2c_init( enum_i2c_port_t i2c_no, i2c_config_t* i2c_conf);

/**
 * @brief  Set and initialize i2c interruption handler
 *
 * @param  uint8_t cpu_num        : CPU number (PRO_CPU_NUM or APP_CPU_NUM)
 * @param  enum_i2c_port_t i2c_no : I2C port num (I2C0 or I2C1)
 * @param  uint8_t i2c_intr_num   : i2c interrupt number, set this value according to interrupt cpu using table, Please see the core-isa.h
 * @param  void (* fn)(void* )    : i2c interrupt handler
 * @param  void * arg             : param pointer of i2c interrupt handler
 *
 * @return null
 */
void i2c_intr_register(uint8_t cpu_num,enum_i2c_port_t i2c_no,uint8_t i2c_intr_num,void (* fn)(void* ),void * arg);


#ifdef __cplusplus
}
#endif

#endif  /*_DRIVER_I2C_H_*/
