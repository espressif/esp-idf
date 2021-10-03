/* OpenThread Command Line Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief User command "udpsockserver" process.
 *
 */
void esp_ot_process_udp_server(void *aContext, uint8_t aArgsLength, char *aArgs[]);

/**
 * @brief User command "udpsockclient" process.
 *
 */
void esp_ot_process_udp_client(void *aContext, uint8_t aArgsLength, char *aArgs[]);

#ifdef __cplusplus
}
#endif
