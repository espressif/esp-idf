/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "tinyxml2.h"

static const char *TAG = "example";


extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Initializing the filesystem");
    esp_vfs_fat_mount_config_t mount_config = {};
    mount_config.max_files = 1;

    wl_handle_t wl_handle = WL_INVALID_HANDLE;
    esp_err_t err = esp_vfs_fat_spiflash_mount_rw_wl("/spiflash", "storage", &mount_config, &wl_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(err));
        return;
    }

    // Load the XML file from the filesystem and parse it using tinyxml2
    ESP_LOGI(TAG, "Reading XML file");
    tinyxml2::XMLDocument data;
    data.LoadFile("/spiflash/sample.xml");

    tinyxml2::XMLPrinter printer;
    data.Print(&printer);

    ESP_LOGI(TAG, "Read XML data:\n%s", printer.CStr());

    const char* to_data = data.FirstChildElement("note")->FirstChildElement("to")->GetText();
    const char* from_data = data.FirstChildElement("note")->FirstChildElement("from")->GetText();
    const char* heading_data = data.FirstChildElement("note")->FirstChildElement("heading")->GetText();
    const char* body_data = data.FirstChildElement("note")->FirstChildElement("body")->GetText();

    ESP_LOGI(TAG, "Parsed XML data:\n\nTo: %s\nFrom: %s\nHeading: %s\nBody: %s",
                to_data, from_data, heading_data, body_data);

    // Clean up
    esp_vfs_fat_spiflash_unmount_rw_wl("/spiflash", wl_handle);

    ESP_LOGI(TAG, "Example end");
}
