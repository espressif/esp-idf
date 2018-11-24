// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string.h>
#include "unity.h"
#include "test_utils.h"
#include "rom/ets_sys.h"
#include "rom/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "tcpip_adapter.h"
#include "lwip/sockets.h"

const esp_partition_t *get_test_data_partition()
{
    /* This finds "flash_test" partition defined in partition_table_unit_test_app.csv */
    const esp_partition_t *result = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
            ESP_PARTITION_SUBTYPE_ANY, "flash_test");
    TEST_ASSERT_NOT_NULL(result); /* means partition table set wrong */
    return result;
}

// wait user to send "Enter" key
static void wait_user_control()
{
    char sign[5] = {0};
    while(strlen(sign) == 0)
    {
        /* Flush anything already in the RX buffer */
        while(uart_rx_one_char((uint8_t *) sign) == OK) {
        }
        /* Read line */
        UartRxString((uint8_t*) sign, sizeof(sign) - 1);
    }
}

void test_case_uses_tcpip()
{
    // Can be called more than once, does nothing on subsequent calls
    tcpip_adapter_init();

    // Allocate all sockets then free them
    // (First time each socket is allocated some one-time allocations happen.)
    int sockets[CONFIG_LWIP_MAX_SOCKETS];
    for (int i = 0; i < CONFIG_LWIP_MAX_SOCKETS; i++) {
        int type = (i % 2 == 0) ? SOCK_DGRAM : SOCK_STREAM;
        int family = (i % 3 == 0) ? PF_INET6 : PF_INET;
        sockets[i] = socket(family, type, IPPROTO_IP);
    }
    for (int i = 0; i < CONFIG_LWIP_MAX_SOCKETS; i++) {
        close(sockets[i]);
    }

    // Allow LWIP tasks to finish initialising themselves
    vTaskDelay(25 / portTICK_RATE_MS);

    printf("Note: tcpip_adapter_init() has been called. Until next reset, TCP/IP task will periodicially allocate memory and consume CPU time.\n");

    // Reset the leak checker as LWIP allocates a lot of memory on first run
    unity_reset_leak_checks();
}

// signal functions, used for sync between unity DUTs for multiple devices cases
void unity_wait_for_signal(const char* signal_name)
{
    printf("Waiting for signal: [%s]!\n"
            "Please press \"Enter\" key to once any board send this signal.\n", signal_name);
    wait_user_control();
}

void unity_send_signal(const char* signal_name)
{
    printf("Send signal: [%s]!\n", signal_name);
}

