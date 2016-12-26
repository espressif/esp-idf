#ifndef __I2C_MASTER_H__
#define __I2C_MASTER_H__

#include <stdio.h>
#include "driver/gpio.h"
#include "esp_types.h"
#include "rom/ets_sys.h"

#if 0
#define I2C_MASTER_SDA_MUX PERIPHS_IO_MUX_MTMS_U
#define I2C_MASTER_SCL_MUX PERIPHS_IO_MUX_MTDO_U
#define I2C_MASTER_SDA_GPIO 14
#define I2C_MASTER_SCL_GPIO 15
#define I2C_MASTER_SDA_FUNC FUNC_MTMS_GPIO14
#define I2C_MASTER_SCL_FUNC FUNC_MTDO_GPIO15
#endif

#if 0
#define I2C_MASTER_SDA_MUX PERIPHS_IO_MUX_GPIO26_U
#define I2C_MASTER_SCL_MUX PERIPHS_IO_MUX_GPIO27_U
#define I2C_MASTER_SDA_GPIO 26
#define I2C_MASTER_SDA_PIN GPIO_Pin_26

#define I2C_MASTER_SCL_PIN GPIO_Pin_27
#define I2C_MASTER_SCL_GPIO 27
#define I2C_MASTER_SDA_FUNC FUNC_GPIO26_GPIO26
#define I2C_MASTER_SCL_FUNC FUNC_GPIO27_GPIO27
#endif

#if 1

#define I2C_MASTER_SDA_GPIO 21
#define I2C_MASTER_SDA_PIN GPIO_SEL_21

#define I2C_MASTER_SCL_PIN GPIO_SEL_19
#define I2C_MASTER_SCL_GPIO 19
#endif

#if 0
#define I2C_MASTER_SDA_MUX PERIPHS_IO_MUX_GPIO23_U
#define I2C_MASTER_SCL_MUX PERIPHS_IO_MUX_GPIO5_U
#define I2C_MASTER_SDA_GPIO 23
#define I2C_MASTER_SDA_PIN GPIO_Pin_23

#define I2C_MASTER_SCL_PIN GPIO_Pin_5
#define I2C_MASTER_SCL_GPIO 5
#define I2C_MASTER_SDA_FUNC FUNC_GPIO23_GPIO23
#define I2C_MASTER_SCL_FUNC FUNC_GPIO5_GPIO5
#endif

#define I2C_MASTER_SDA_HIGH_SCL_HIGH()  \
    gpio_set_level(I2C_MASTER_SDA_GPIO,1);\
    gpio_set_level(I2C_MASTER_SCL_GPIO,1);

#define I2C_MASTER_SDA_HIGH_SCL_LOW()  \
    gpio_set_level(I2C_MASTER_SDA_GPIO,1);\
    gpio_set_level(I2C_MASTER_SCL_GPIO,0);

#define I2C_MASTER_SDA_LOW_SCL_HIGH()  \
    gpio_set_level(I2C_MASTER_SDA_GPIO,0);\
    gpio_set_level(I2C_MASTER_SCL_GPIO,1);

#define I2C_MASTER_SDA_LOW_SCL_LOW()  \
    gpio_set_level(I2C_MASTER_SDA_GPIO,0);\
    gpio_set_level(I2C_MASTER_SCL_GPIO,0);

/** \defgroup Driver_APIs Driver APIs
  * @brief Driver APIs
  */

/** @addtogroup Driver_APIs
  * @{
  */

/** \defgroup I2C_Driver_APIs I2C_MASTER Driver APIs
  * @brief UART driver APIs
  */

/** @addtogroup I2C_MASTER_Driver_APIs
  * @{
  */

/**
  * @brief   i2c_master_gpio_init,config SDA and SCL gpio to open-drain output mode.
  *
  * @param   null
  *
  * @return  null
  */
void i2c_master_gpio_init(void);

/**
  * @brief   i2c_master_gpio_init,config SDA and SCL gpio to open-drain output mode.
  *
  * @param  initilize I2C bus to enable i2c operations.
  *
  * @return  null
  */
void i2c_master_init(void);

#define i2c_master_wait    ets_delay_us


/**
  * @brief   i2c_master_gpio_init,config SDA and SCL gpio to open-drain output mode.
  *
  * @param  set i2c to stop sending state.
  *
  * @return  null
  */
void i2c_master_stop(void);

/**
  * @brief   i2c_master_gpio_init,config SDA and SCL gpio to open-drain output mode.
  *
  * @param  set i2c to start sending state.
  *
  * @return  null
  */
void i2c_master_start(void);

/**
  * @brief   i2c_master_gpio_init,config SDA and SCL gpio to open-drain output mode.
  *
  * @param  set ack to i2c bus as level value.
  *
  * @return  null
  */
void i2c_master_setAck(uint8_t level);

/**
  * @brief   confirm if peer send ack.
  *
  * @param null
  *
  * @return  null
  */
uint8_t i2c_master_getAck(void);

/**
  * @brief   read Byte from i2c bus.
  *
  * @param null
  *
  * @return  the byte which read from i2c bus.
  */
uint8_t i2c_master_readByte(void);

/**
  * @brief   write wrdata value(one byte) into i2c.
  *
  * @param uint8_t wrdata:write value
  *
  * @return  null
  */
void i2c_master_writeByte(uint8_t wrdata);

/**
  * @brief   i2c_master_checkAck.
  *
  * @param null
  *
  * @return  the result of check ack
  */
bool i2c_master_checkAck(void);

/**
  * @brief   i2c master send Ack.
  *
  * @param null
  *
  * @return  null
  */
void i2c_master_send_ack(void);

/**
  * @brief   i2c master send Nack.
  *
  * @param null
  *
  * @return  null
  */
void i2c_master_send_nack(void);

/**
  * @}
  */

/**
  * @}
  */

#endif
