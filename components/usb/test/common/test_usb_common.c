/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/usb_wrap_struct.h"
#include "test_usb_common.h"

void test_usb_force_conn_state(bool connected, TickType_t delay_ticks)
{
    if (delay_ticks > 0) {
        //Delay of 0 ticks causes a yield. So skip if delay_ticks is 0.
        vTaskDelay(delay_ticks);
    }
    usb_wrap_dev_t *wrap = &USB_WRAP;
    if (connected) {
        //Disable test mode to return to previous internal PHY configuration
        wrap->test_conf.test_enable = 0;
    } else {
        /*
        Mimic a disconnection by using the internal PHY's test mode.
        Force Output Enable to 1 (even if the controller isn't outputting). With test_tx_dp and test_tx_dm set to 0,
        this will look like a disconnection.
        */
        wrap->test_conf.val = 0;
        wrap->test_conf.test_usb_wrap_oe = 1;
        wrap->test_conf.test_enable = 1;
    }
}
