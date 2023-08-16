/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>

#include "sdkconfig.h"
#include "esp_wifi.h"
#include "Mockesp_wifi.h"
#include "Mockesp_wifi_default.h"
#include "Mockesp_wifi_netif.h"
#include "Mockesp_wifi_he.h"
#include "Mockesp_wifi_ap_get_sta_list.h"
#include "Mockesp_netif.h"
#include "Mockesp_mesh.h"
#include "Mockesp_now.h"
#include "Mockesp_smartconfig.h"
#include "Mocksmartconfig_ack.h"

void app_main(void)
{
    esp_netif_init_IgnoreAndReturn(ESP_OK);
    ESP_ERROR_CHECK(esp_netif_init());

    esp_netif_create_default_wifi_sta_IgnoreAndReturn(ESP_OK);
    esp_netif_create_default_wifi_sta();

    esp_netif_create_default_wifi_ap_IgnoreAndReturn(ESP_OK);
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init_IgnoreAndReturn(ESP_OK);
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_wifi_set_mode_IgnoreAndReturn(ESP_OK);
    esp_wifi_set_config_IgnoreAndReturn(ESP_OK);
    esp_wifi_start_IgnoreAndReturn(ESP_OK);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, NULL) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    esp_wifi_connect_IgnoreAndReturn(ESP_OK);
    esp_wifi_connect();

    esp_wifi_create_if_driver_IgnoreAndReturn(NULL);
    esp_wifi_create_if_driver((wifi_interface_t) NULL);

    esp_wifi_ap_get_sta_list_with_ip_IgnoreAndReturn(ESP_OK);
    esp_wifi_ap_get_sta_list_with_ip(NULL, NULL);

    esp_wifi_sta_itwt_setup_IgnoreAndReturn(ESP_OK);
    esp_wifi_sta_itwt_setup((wifi_twt_setup_config_t*) NULL);

    sc_send_ack_stop_Ignore();
    sc_send_ack_stop();

    esp_smartconfig_get_version_IgnoreAndReturn(NULL);
    esp_smartconfig_get_version();

    esp_mesh_init_IgnoreAndReturn(ESP_OK);
    ESP_ERROR_CHECK(esp_mesh_init());

    esp_now_init_IgnoreAndReturn(ESP_OK);
    ESP_ERROR_CHECK(esp_now_init());

    printf("WiFi build test done\n");
}
