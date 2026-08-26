/* Startup time example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "esp_log.h"

static const char *TAG = "example";

/* The purpose of this app is to demonstrate fast startup time only, so feel free
   to replace this app_main() with your own code or copy the sdkconfig.defaults contents
   into a different project's sdkconfig file.
*/
void app_main(void)
{
    // Calling this function restores all Info-level logging at runtime (as "Log Maximum Verbosity" set to "Info")
    esp_log_level_set("*", ESP_LOG_INFO);
    ESP_LOGI(TAG, "App started!");
}
