/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "esp_wifi.h"

#include "driver/i2c.h"
#include "driver/i2c.h"
#include "driver/spi_master.h"
#include "driver/spi_common.h"
#include "driver/usb_serial_jtag.h"
#include "driver/gpio.h"

/* Test that mock headers can be included */
#include "Mockesp_wifi.h"
#include "Mockesp_wifi_default.h"
#include "Mockesp_wifi_netif.h"
#include "Mockesp_wifi_he.h"
#include "Mockesp_wifi_ap_get_sta_list.h"
#include "Mockesp_netif.h"
#include "Mockesp_mesh.h"
#include "Mockesp_now.h"
#include "Mockesp_smartconfig.h"
#include "Mocksmartconfig_ack.h"
#include "Mockrmt_encoder.h"
#include "Mockrmt_common.h"
#include "Mockrmt_tx.h"
#include "Mockrmt_rx.h"

/* Test that mock functions exist and that required definitions are available */
void app_main(void)
{
    // Drivers
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

    // WiFi
    esp_netif_init_IgnoreAndReturn(ESP_OK);
    ESP_ERROR_CHECK(esp_netif_init());

    esp_netif_create_default_wifi_sta_IgnoreAndReturn(ESP_OK);
    esp_netif_create_default_wifi_sta();

    esp_netif_create_default_wifi_ap_IgnoreAndReturn(ESP_OK);
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init_IgnoreAndReturn(ESP_OK);
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_wifi_set_mode_IgnoreAndReturn(ESP_OK);
    esp_wifi_set_config_IgnoreAndReturn(ESP_OK);
    esp_wifi_start_IgnoreAndReturn(ESP_OK);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, NULL) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    esp_wifi_connect_IgnoreAndReturn(ESP_OK);
    esp_wifi_connect();

    esp_wifi_create_if_driver_IgnoreAndReturn(NULL);
    esp_wifi_create_if_driver((wifi_interface_t) NULL);

    esp_wifi_ap_get_sta_list_with_ip_IgnoreAndReturn(ESP_OK);
    esp_wifi_ap_get_sta_list_with_ip(NULL, NULL);

    esp_wifi_sta_itwt_setup_IgnoreAndReturn(ESP_OK);
    esp_wifi_sta_itwt_setup((wifi_twt_setup_config_t*) NULL);

    sc_send_ack_stop_Ignore();
    sc_send_ack_stop();

    esp_smartconfig_get_version_IgnoreAndReturn(NULL);
    esp_smartconfig_get_version();

    esp_mesh_init_IgnoreAndReturn(ESP_OK);
    ESP_ERROR_CHECK(esp_mesh_init());

    esp_now_init_IgnoreAndReturn(ESP_OK);
    ESP_ERROR_CHECK(esp_now_init());

    printf("Mock build test done\n");
}
