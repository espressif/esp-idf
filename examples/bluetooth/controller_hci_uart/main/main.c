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

#include <stdio.h>
#include <string.h>

#include "bt.h"
#include "driver/uart.h"
#include "esp_log.h"

static const char *tag = "CONTROLLER_UART_HCI";

static void uart_gpio_reset(void)
{
    ESP_LOGI(tag, "HCI UART Pin select: TX 5, RX, 18, CTS 23, RTS 19\n");

#if CONFIG_HCI_UART_NO
    uart_set_pin(CONFIG_HCI_UART_NO, 5, 18, 19, 23);
#endif
}

void app_main()
{ 
    /* As the UART1/2 pin conflict with flash pin, so do matrix of the signal and pin */
    uart_gpio_reset();
 
    esp_bt_controller_init();

    if (esp_bt_controller_enable(ESP_BT_MODE_BTDM) != ESP_OK) {
        return;
    }
}

