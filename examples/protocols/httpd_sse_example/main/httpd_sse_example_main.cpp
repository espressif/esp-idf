/*
 * Main.cpp
 *
 * Simple HTTP Server Example demonstrating Server Sent Event for
 *  server->client communication and HTTP PUT on the other direction.
 *
 * This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   V 1.0 2020.04.29 Nicolas Korber (nicolas.korber_at_bluewin.ch)
 *
 *   */
#include <esp_event.h>
#include <esp_log.h>
#include <esp_ota_ops.h>
#include <nvs_flash.h>
#include "button_scanner.hpp"
#include "httpd_sse_server.hpp"
#include "output_manager.hpp"
#include "softap.hpp"

static const char *TAG = "main";

// Consumer has higher priority
const UBaseType_t ScanPriority = 3;
const UBaseType_t OutputPriority = 2;

// Create tasks
C_OutputManager TheHWManager;
C_ButtonScanner TheBtnScanner;
static httpd_handle_t TheHttpServer = NULL;


void disconnect_handler(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data)
{
    httpd_handle_t *server = (httpd_handle_t *) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        stop_webserver(*server);
        *server = NULL;
    }
}

void connect_handler(void *arg, esp_event_base_t event_base,
                     int32_t event_id, void *event_data)
{
    httpd_handle_t *server = (httpd_handle_t *) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver(&TheHWManager);
    }
}

void setup (void)
{

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    // Start Wi-Fi as access point
    wifi_init_softap();

    // Register event handlers to stop the TheHttpServer when Wi-Fi  is disconnected,
    // and re-start it upon connection.
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &TheHttpServer));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &TheHttpServer));

    /* Start the TheHttpServer for the first time */
    TheHttpServer = start_webserver(TheHWManager.get_outEvtQueue());
    esp_log_level_set("*", ESP_LOG_INFO);

    // Start them and bind the queues
    TheBtnScanner.Start(ScanPriority,
                        TheHWManager.get_outEvtQueue());
    TheHWManager.Start(OutputPriority);
}

extern "C" void app_main(void)
{
    const esp_app_desc_t *app_desc = esp_ota_get_app_description();
    ESP_LOGI(TAG, "Project %s, version %s compiled: %s %s\n",
             app_desc->project_name, app_desc->version, app_desc->date, app_desc->time);
    setup ();

    while (1) {
        // nothing to do here
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}



