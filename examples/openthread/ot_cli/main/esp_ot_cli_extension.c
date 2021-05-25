// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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

#include "esp_openthread.h"
#include "esp_ot_cli_extension.h"
#include "esp_ot_tcp_socket.h"
#include "esp_ot_udp_socket.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"
#include "openthread/cli.h"

static const otCliCommand kCommands[] = {
#if CONFIG_OPENTHREAD_ENABLE_TCP_SOCKET_EXAMPLE
    {"tcpsockserver", esp_ot_process_tcp_server},
    {"tcpsockclient", esp_ot_process_tcp_client},
#endif // CONFIG_OPENTHREAD_ENABLE_TCP_SOCKET_EXAMPLE
#if CONFIG_OPENTHREAD_ENABLE_UDP_SOCKET_EXAMPLE
    {"udpsockserver", esp_ot_process_udp_server},
    {"udpsockclient", esp_ot_process_udp_client},
#endif // CONFIG_OPENTHREAD_ENABLE_UDP_SOCKET_EXAMPLE
};

void esp_cli_custom_command_init()
{
    otInstance *instance = esp_openthread_get_instance();
    otCliSetUserCommands(kCommands, (sizeof(kCommands) / sizeof(kCommands[0])), instance);
}
