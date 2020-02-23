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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_netif.h"
#include "lwip/sockets.h"

const esp_partition_t *get_test_data_partition(void)
{
    /* This finds "flash_test" partition defined in partition_table_unit_test_app.csv */
    const esp_partition_t *result = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
            ESP_PARTITION_SUBTYPE_ANY, "flash_test");
    TEST_ASSERT_NOT_NULL(result); /* means partition table set wrong */
    return result;
}

void test_case_uses_tcpip(void)
{
    // Can be called more than once, does nothing on subsequent calls
    esp_netif_init();

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

    printf("Note: esp_netif_init() has been called. Until next reset, TCP/IP task will periodicially allocate memory and consume CPU time.\n");

    // Reset the leak checker as LWIP allocates a lot of memory on first run
    unity_reset_leak_checks();
    test_utils_set_leak_level(0, TYPE_LEAK_CRITICAL, COMP_LEAK_GENERAL);
    test_utils_set_leak_level(CONFIG_UNITY_CRITICAL_LEAK_LEVEL_LWIP, TYPE_LEAK_CRITICAL, COMP_LEAK_LWIP);
}

// wait user to send "Enter" key or input parameter
static void wait_user_control(char* parameter_buf, uint8_t buf_len)
{
    char *buffer = parameter_buf;
    char sign[5];
    uint8_t buffer_len = buf_len - 1;

    if (parameter_buf == NULL) {
        buffer = sign;
        buffer_len = sizeof(sign) - 1;
    }
    // workaround that unity_gets (UartRxString) will not set '\0' correctly
    bzero(buffer, buffer_len);

    unity_gets(buffer, buffer_len);
}

// signal functions, used for sync between unity DUTs for multiple devices cases
void unity_wait_for_signal_param(const char* signal_name, char* parameter_buf, uint8_t buf_len)
{
    printf("Waiting for signal: [%s]!\n", signal_name);
    if (parameter_buf == NULL) {
        printf("Please press \"Enter\" key once any board send this signal.\n");
    } else {
        printf("Please input parameter value from any board send this signal and press \"Enter\" key.\n");
    }
    wait_user_control(parameter_buf, buf_len);
}

void unity_send_signal_param(const char* signal_name, const char *parameter)
{
    if (parameter == NULL) {
        printf("Send signal: [%s]!\n", signal_name);
    } else {
        printf("Send signal: [%s][%s]!\n", signal_name, parameter);
    }
}

bool unity_util_convert_mac_from_string(const char* mac_str, uint8_t *mac_addr)
{
    uint8_t loop = 0;
    uint8_t tmp = 0;
    const char *start;
    char *stop;

    for (loop = 0; loop < 6; loop++) {
        start = mac_str + loop * 3;
        tmp = strtol(start, &stop, 16);
        if (stop - start == 2 && (*stop == ':' || (*stop == 0 && loop == 5))) {
            mac_addr[loop] = tmp;
        } else {
            return false;
        }
    }
    return true;
}

static size_t test_unity_leak_level[TYPE_LEAK_MAX][COMP_LEAK_ALL] = { 0 };

esp_err_t test_utils_set_leak_level(size_t leak_level, esp_type_leak_t type_of_leak, esp_comp_leak_t component)
{
    if (type_of_leak >= TYPE_LEAK_MAX || component >= COMP_LEAK_ALL) {
        return ESP_ERR_INVALID_ARG;
    }
    test_unity_leak_level[type_of_leak][component] = leak_level;
    return ESP_OK;
}

size_t test_utils_get_leak_level(esp_type_leak_t type_of_leak, esp_comp_leak_t component)
{
    size_t leak_level = 0;
    if (type_of_leak >= TYPE_LEAK_MAX || component > COMP_LEAK_ALL) {
        leak_level = 0;
    } else {
        if (component == COMP_LEAK_ALL) {
            for (int comp = 0; comp < COMP_LEAK_ALL; ++comp) {
                leak_level += test_unity_leak_level[type_of_leak][comp];
            }
        } else {
            leak_level = test_unity_leak_level[type_of_leak][component];
        }
    }
    return leak_level;
}
