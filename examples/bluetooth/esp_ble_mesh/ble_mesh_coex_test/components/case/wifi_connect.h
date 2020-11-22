/* ESP BLE Mesh Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#ifndef __WIFI_CONNECT_H__
#define __WIFI_CONNECT_H__

#include "esp_err.h"
#include "esp_netif.h"
#include "esp_event.h"

esp_err_t example_connect(const char *ssid, const char *passwd);
esp_err_t example_disconnect(void);
esp_err_t example_configure_stdin_stdout(void);
esp_netif_t *get_example_netif(void);

#endif  /* __WIFI_CONNECT_H__ */
