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

