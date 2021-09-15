/* OpenThread Command Line Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_openthread.h"
#include "esp_ot_cli_extension.h"
#include "esp_ot_iperf.h"
#include "esp_ot_tcp_socket.h"
#include "esp_ot_udp_socket.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"
#include "openthread/cli.h"

static const otCliCommand kCommands[] = {
    {"tcpsockserver", esp_ot_process_tcp_server},
    {"tcpsockclient", esp_ot_process_tcp_client},
    {"udpsockserver", esp_ot_process_udp_server},
    {"udpsockclient", esp_ot_process_udp_client},
    {"iperf", esp_ot_process_iperf}
};

void esp_cli_custom_command_init()
{
    otInstance *instance = esp_openthread_get_instance();
    otCliSetUserCommands(kCommands, (sizeof(kCommands) / sizeof(kCommands[0])), instance);
}
