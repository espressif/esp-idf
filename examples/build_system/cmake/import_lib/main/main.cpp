#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "lib/tinyxml2/tinyxml2.h"

using namespace tinyxml2;

static const char *TAG = "example";

// Handle of the wear levelling library instance
static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;

// Mount path for the partition
const char *base_path = "/spiflash";

extern "C" void app_main(void)
{
    // Do example setup
    ESP_LOGI(TAG, "Setting up...");
    esp_vfs_fat_mount_config_t mount_config;
    mount_config.max_files = 4;
    mount_config.format_if_mount_failed = true;
    mount_config.allocation_unit_size = CONFIG_WL_SECTOR_SIZE;

    esp_err_t err = esp_vfs_fat_spiflash_mount(base_path, "storage", &mount_config, &s_wl_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(err));
        return;
    }

    // The sample XML is embedded binary data. Create a file first containing the embedded
    // so it can be accessed.
    ESP_LOGI(TAG, "Copying sample XML to filesystem...");

    extern const char data_start[] asm("_binary_sample_xml_start");
    extern const char data_end[]   asm("_binary_sample_xml_end");
    FILE *f = fopen("/spiflash/sample.xml", "wb");

    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }
    fwrite(data_start, sizeof(char), data_end - data_start + 1, f);
    fclose(f);

    // Now that the file is created, load it using tinyxml2 and parse
    ESP_LOGI(TAG, "Reading XML file");

    XMLDocument data;
    data.LoadFile("/spiflash/sample.xml");

    XMLPrinter printer;
    data.Print(&printer);

    ESP_LOGI(TAG, "Read XML data:\n%s", printer.CStr());

    const char* to_data = data.FirstChildElement("note")->FirstChildElement("to")->GetText();
    const char* from_data = data.FirstChildElement("note")->FirstChildElement("from")->GetText();
    const char* heading_data = data.FirstChildElement("note")->FirstChildElement("heading")->GetText();
    const char* body_data = data.FirstChildElement("note")->FirstChildElement("body")->GetText();

    ESP_LOGI(TAG, "Parsed XML data:\n\nTo: %s\nFrom: %s\nHeading: %s\nBody: %s",
                to_data, from_data, heading_data, body_data);

    ESP_LOGI(TAG, "Example end");
}