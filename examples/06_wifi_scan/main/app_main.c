/*
 *  Copyright (c) 2016 - 2026 MaiKe Labs
 *
 *  Written by Jack Tan <jiankemeng@gmail.com>
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
*/
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_system.h"
#include "nvs_flash.h"

void scan_ap_task(void *pvParameters)
{
	uint16_t n = 0;
	wifi_ap_list_t *al;
    while (1) {

		esp_wifi_scan_start(NULL, true);
		esp_wifi_get_ap_num(&n);

		printf("Scanned %d APs \r\n", n);
		al = malloc(n * sizeof(wifi_ap_list_t));
		if (al == NULL) {
			return;
		}
		if (esp_wifi_get_ap_list(&n, al) == ESP_OK) {
			for (uint16_t i = 0; i < n; i++) {
				uint8_t *bi = al[i].bssid;
				printf("%32s (%02x:%02x:%02x:%02x:%02x:%02x) rssi: %02d auth: %02d\r\n",
						al[i].ssid,
						MAC2STR(bi),
						al[i].rssi,
						al[i].authmode
					);
			}
		}
		free(al);

		printf("------------ delay 6s to start a new scanning ... --------------\r\n");
        vTaskDelay(6000 / portTICK_PERIOD_MS);
    }
}

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
	return 0;
}

static void wifi_init(void)
{
	tcpip_adapter_init();
	esp_event_loop_init(event_handler, NULL);
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	esp_wifi_init(&cfg);

	esp_wifi_set_mode(WIFI_MODE_STA);
	esp_wifi_start();
}

void app_main()
{
	nvs_flash_init();
	system_init();
	wifi_init();
	printf("Welcome to Noduino Quantum\r\n");
	printf("WiFi AP SSID Scanning... \r\n");
    xTaskCreatePinnedToCore(&scan_ap_task, "scan_ap_task", 1024, NULL, 5, NULL, 0);
}
