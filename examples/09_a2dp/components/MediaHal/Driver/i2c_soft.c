/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP32 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */
#include "rom/ets_sys.h"
#include "driver/gpio.h"
#include "i2c_soft.h"

static uint8_t m_nLastSDA;
static uint8_t m_nLastSCL;

/******************************************************************************
 * FunctionName : i2c_master_setDC
 * Description  : Internal used function -
 *                    set i2c SDA and SCL bit value for half clk cycle
 * Parameters   : uint8_t SDA
 *                uint8_t SCL
 * Returns      : NONE
*******************************************************************************/
static void i2c_master_setDC(uint8_t SDA, uint8_t SCL)
{
    SDA &= 0x01;
    SCL &= 0x01;
    m_nLastSDA = SDA;
    m_nLastSCL = SCL;
    //ETS_INTR_LOCK();
    if ((0 == SDA) && (0 == SCL)) {
        I2C_MASTER_SDA_LOW_SCL_LOW();
    } else if ((0 == SDA) && (1 == SCL)) {
        I2C_MASTER_SDA_LOW_SCL_HIGH();
    } else if ((1 == SDA) && (0 == SCL)) {
        I2C_MASTER_SDA_HIGH_SCL_LOW();
    } else {
        I2C_MASTER_SDA_HIGH_SCL_HIGH();
    }
    //ETS_INTR_UNLOCK();
}

/******************************************************************************
 * FunctionName : i2c_master_getDC
 * Description  : Internal used function -
 *                    get i2c SDA bit value
 * Parameters   : NONE
 * Returns      : uint8_t - SDA bit value
*******************************************************************************/
static uint8_t i2c_master_getDC(void)
{
    uint8_t sda_out;
    //ETS_INTR_LOCK();
    //sda_out = GPIO_INPUT_GET(GPIO_ID_PIN(I2C_MASTER_SDA_GPIO));
    sda_out = gpio_get_level(GPIO_ID_PIN(I2C_MASTER_SDA_GPIO));
    //ETS_INTR_UNLOCK();
    return sda_out;
}

/******************************************************************************
 * FunctionName : i2c_master_init
 * Description  : initilize I2C bus to enable i2c operations
 * Parameters   : NONE
 * Returns      : NONE
*******************************************************************************/
void i2c_master_init(void)
{
    uint8_t i;

    i2c_master_setDC(1, 0);
    i2c_master_wait(5);

    // when SCL = 0, toggle SDA to clear up
    i2c_master_setDC(0, 0) ;
    i2c_master_wait(5);
    i2c_master_setDC(1, 0) ;
    i2c_master_wait(5);

    // set data_cnt to max value
    for (i = 0; i < 28; i++) {
        i2c_master_setDC(1, 0);
        i2c_master_wait(5); // sda 1, scl 0
        i2c_master_setDC(1, 1);
        i2c_master_wait(5); // sda 1, scl 1
    }

    // reset all
    i2c_master_stop();
    //return;
}

/******************************************************************************
 * FunctionName : i2c_master_gpio_init
 * Description  : config SDA and SCL gpio to open-drain output mode,
 *                mux and gpio num defined in i2c_master.h
 * Parameters   : NONE
 * Returns      : NONE
*******************************************************************************/
void i2c_master_gpio_init(void)
{

	gpio_config_t io_config;
	io_config.pin_bit_mask= (I2C_MASTER_SDA_PIN) | (I2C_MASTER_SCL_PIN);
	io_config.mode= GPIO_MODE_INPUT_OUTPUT_OD;
	io_config.pull_up_en= GPIO_PULLUP_ENABLE;
	io_config.pull_down_en= GPIO_PULLDOWN_DISABLE;
	io_config.intr_type = GPIO_PIN_INTR_DISABLE;

	gpio_config(&io_config);
#if 0
	io_config.gpio_intry_type_sel = GPIO_PIN_INTR_DISABLE;
	io_config.gpio_mode_sel = GPIO_MODE_OUTPUT_OD;
	io_config.gpio_pin_sel = I2C_MASTER_SCL_PIN;
	io_config.gpio_pulldown_sel = GPIO_PULLDOWN_DISABLE;
	io_config.gpio_pullup_sel  = GPIO_PULLUP_ENABLE;	

    gpio_config(&io_config);
#endif
	#if 0
     ETS_INTR_UNLOCK();
    // Set to GPIO function
    PIN_FUNC_SELECT(I2C_MASTER_SDA_MUX, I2C_MASTER_SDA_FUNC);
    PIN_FUNC_SELECT(I2C_MASTER_SCL_MUX, I2C_MASTER_SCL_FUNC);    
    // Enable input
    SET_PERI_REG_MASK(I2C_MASTER_SDA_MUX, FUN_IE);
    
    GPIO_REG_WRITE(GPIO_PIN_ADDR(I2C_MASTER_SDA_GPIO),
        GPIO_REG_READ(GPIO_PIN_ADDR(I2C_MASTER_SDA_GPIO)) | GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_ENABLE)); //open drain;
    GPIO_REG_WRITE(GPIO_ENABLE, GPIO_REG_READ(GPIO_ENABLE) | (1 << I2C_MASTER_SDA_GPIO));

    GPIO_REG_WRITE(GPIO_PIN_ADDR(I2C_MASTER_SCL_GPIO),
        GPIO_PIN_ADDR(I2C_MASTER_SCL_GPIO) | GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_ENABLE)); //open drain;
    GPIO_REG_WRITE(GPIO_ENABLE, GPIO_REG_READ(GPIO_ENABLE) | (1 << I2C_MASTER_SCL_GPIO));
    I2C_MASTER_SDA_HIGH_SCL_HIGH();
	PIN_PULLUP_EN(I2C_MASTER_SDA_MUX);
	PIN_PULLUP_EN(I2C_MASTER_SCL_MUX);
    ETS_INTR_UNLOCK();
	#endif

    i2c_master_init();
}

/******************************************************************************
 * FunctionName : i2c_master_start
 * Description  : set i2c to send state
 * Parameters   : NONE
 * Returns      : NONE
*******************************************************************************/
void i2c_master_start(void)
{
    // ETS_INTR_UNLOCK();
    i2c_master_setDC(1, m_nLastSCL);
    i2c_master_wait(5);
    i2c_master_setDC(1, 1);
    i2c_master_wait(5); // sda 1, scl 1
    i2c_master_setDC(0, 1);
    i2c_master_wait(5); // sda 0, scl 1
    // ETS_INTR_UNLOCK();
}

/******************************************************************************
 * FunctionName : i2c_master_stop
 * Description  : set i2c to stop sending state
 * Parameters   : NONE
 * Returns      : NONE
*******************************************************************************/
void i2c_master_stop(void)
{
    // ETS_INTR_UNLOCK();
    i2c_master_wait(5);

    i2c_master_setDC(0, m_nLastSCL);
    i2c_master_wait(5); // sda 0
    i2c_master_setDC(0, 1);
    i2c_master_wait(5); // sda 0, scl 1
    i2c_master_setDC(1, 1);
    i2c_master_wait(5); // sda 1, scl 1
    // ETS_INTR_UNLOCK();
}

/******************************************************************************
 * FunctionName : i2c_master_setAck
 * Description  : set ack to i2c bus as level value
 * Parameters   : uint8_t level - 0 or 1
 * Returns      : NONE
*******************************************************************************/
void i2c_master_setAck(uint8_t level)
{
    // ETS_INTR_UNLOCK();

    i2c_master_setDC(m_nLastSDA, 0);
    i2c_master_wait(5);
    i2c_master_setDC(level, 0);
    i2c_master_wait(5); // sda level, scl 0
    i2c_master_setDC(level, 1);
    i2c_master_wait(8); // sda level, scl 1
    i2c_master_setDC(level, 0);
    i2c_master_wait(5); // sda level, scl 0
    i2c_master_setDC(1, 0);
    i2c_master_wait(5);
    // ETS_INTR_UNLOCK();
}

/******************************************************************************
 * FunctionName : i2c_master_getAck
 * Description  : confirm if peer send ack
 * Parameters   : NONE
 * Returns      : uint8_t - ack value, 0 or 1
*******************************************************************************/
uint8_t i2c_master_getAck(void)
{
    uint8_t retVal;
    // ETS_INTR_UNLOCK();
    i2c_master_setDC(m_nLastSDA, 0);
    i2c_master_wait(5);
    i2c_master_setDC(1, 0);
    i2c_master_wait(5);
    i2c_master_setDC(1, 1);
    i2c_master_wait(5);

    retVal = i2c_master_getDC();
    i2c_master_wait(5);
    i2c_master_setDC(1, 0);
    i2c_master_wait(5);
    // ETS_INTR_UNLOCK();

    return retVal;
}

/******************************************************************************
* FunctionName : i2c_master_checkAck
* Description  : get dev response
* Parameters   : NONE
* Returns      : true : get ack ; false : get nack
*******************************************************************************/
bool i2c_master_checkAck(void)
{
    if(i2c_master_getAck()){
        return false;
    }else{
        return true;
    }
}

/******************************************************************************
* FunctionName : i2c_master_send_ack
* Description  : response ack
* Parameters   : NONE
* Returns      : NONE
*******************************************************************************/
void i2c_master_send_ack(void)
{
    i2c_master_setAck(0x0);
}
/******************************************************************************
* FunctionName : i2c_master_send_nack
* Description  : response nack
* Parameters   : NONE
* Returns      : NONE
*******************************************************************************/
void i2c_master_send_nack(void)
{
    i2c_master_setAck(0x1);
}

/******************************************************************************
 * FunctionName : i2c_master_readByte
 * Description  : read Byte from i2c bus
 * Parameters   : NONE
 * Returns      : uint8_t - readed value
*******************************************************************************/
uint8_t i2c_master_readByte(void)
{
    uint8_t retVal = 0;
    uint8_t k, i;
    // ETS_INTR_UNLOCK();
    i2c_master_wait(5);
    i2c_master_setDC(m_nLastSDA, 0);
    i2c_master_wait(5); // sda 1, scl 0

    for (i = 0; i < 8; i++) {
        i2c_master_wait(5);
        i2c_master_setDC(1, 0);
        i2c_master_wait(5); // sda 1, scl 0
        i2c_master_setDC(1, 1);
        i2c_master_wait(5); // sda 1, scl 1

        k = i2c_master_getDC();
        i2c_master_wait(5);

        if (i == 7) {
            i2c_master_wait(3);   ////
        }

        k <<= (7 - i);
        retVal |= k;
    }

    i2c_master_setDC(1, 0);
    i2c_master_wait(5); // sda 1, scl 0
   // ETS_INTR_UNLOCK();
    return retVal;
}
#define DBG_TONY ets_printf
/******************************************************************************
 * FunctionName : i2c_master_writeByte
 * Description  : write wrdata value(one byte) into i2c
 * Parameters   : uint8_t wrdata - write value
 * Returns      : NONE
*******************************************************************************/
void i2c_master_writeByte(uint8_t wrdata)
{
    uint8_t dat;
    int8_t i;

    i2c_master_wait(5);
    i2c_master_setDC(m_nLastSDA, 0);
    i2c_master_wait(5);

	for (i = 7; i >= 0; i--) {
        dat = wrdata >> i;
        i2c_master_setDC(dat, 0);
        i2c_master_wait(5);
        i2c_master_setDC(dat, 1);
        i2c_master_wait(5);

        if (i == 0) {
            i2c_master_wait(3);   ////
        }

        i2c_master_setDC(dat, 0);
        i2c_master_wait(5);
    }

}
