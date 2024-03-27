/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

/* Test that mock headers can be included */
#include "driver/i2c.h"
#include "driver/i2c.h"
#include "driver/spi_master.h"
#include "driver/spi_common.h"
#include "driver/usb_serial_jtag.h"
#include "driver/gpio.h"

#include "Mockrmt_encoder.h"
#include "Mockrmt_common.h"
#include "Mockrmt_tx.h"
#include "Mockrmt_rx.h"

/* Test that mock functions exist and that required definitions are available */
void app_main(void)
{
    i2c_driver_delete(0);
    int nc_gpio_num = GPIO_NUM_NC;
    (void)nc_gpio_num;
    int gpio_num = GPIO_NUM_39;
    (void)gpio_num;

    spi_host_device_t spi_dev = SPI1_HOST;
    spi_dev = SPI2_HOST;
    (void)spi_dev;
    spi_clock_source_t spi_default_clock = SPI_CLK_SRC_DEFAULT;
    (void)spi_default_clock;
    soc_periph_spi_clk_src_t soc_periph_spi_clk_src = 0;
    (void)soc_periph_spi_clk_src;

    rmt_clock_source_t rmt_clk_source = 0;
    (void)rmt_clk_source;
    rmt_symbol_word_t rm_symbol_word = { .duration0 = 47, .level0 = 0, .duration1 = 47, .level1 = 1 };
    (void)rm_symbol_word;

    i2c_port_t i2c_port = I2C_NUM_0;
    (void)i2c_port;
    i2c_mode_t i2c_mode = I2C_MODE_MASTER;
    i2c_mode = I2C_MODE_SLAVE;
    (void)i2c_mode;
    i2c_rw_t i2c_rw = I2C_MASTER_WRITE;
    i2c_rw = I2C_MASTER_READ;
    (void)i2c_rw;
    i2c_trans_mode_t i2c = I2C_DATA_MODE_MSB_FIRST;
    i2c = I2C_DATA_MODE_LSB_FIRST;
    (void)i2c;
    i2c_addr_bit_len_t i2c_addr_bit_len = I2C_ADDR_BIT_LEN_7;

    #if SOC_I2C_SUPPORT_10BIT_ADDR
    i2c_addr_bit_len = I2C_ADDR_BIT_LEN_10;
    #endif

    (void)i2c_addr_bit_len;
    i2c_ack_type_t i2c_ack = I2C_MASTER_ACK;
    i2c_ack = I2C_MASTER_NACK;
    i2c_ack = I2C_MASTER_LAST_NACK;
    (void)i2c_ack;

    rmt_channel_handle_t channel = 0;
    rmt_new_bytes_encoder(NULL, NULL);
    rmt_new_rx_channel(NULL, NULL);
    rmt_del_channel(channel);
    rmt_new_tx_channel(NULL, NULL);

    usb_serial_jtag_driver_install(NULL);
}
