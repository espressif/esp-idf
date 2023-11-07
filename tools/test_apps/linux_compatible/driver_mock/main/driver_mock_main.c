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

/*Test that mock functions exist*/
void app_main(void)
{
    i2c_driver_delete(0);
    int nc_gpio_num = GPIO_NUM_NC;
    (void)nc_gpio_num;
    int gpio_num = GPIO_NUM_39;
    (void)gpio_num;

    rmt_channel_handle_t channel = 0;
    rmt_new_bytes_encoder(NULL, NULL);
    rmt_new_rx_channel(NULL, NULL);
    rmt_del_channel(channel);
    rmt_new_tx_channel(NULL, NULL);

    usb_serial_jtag_driver_install(NULL);
}
