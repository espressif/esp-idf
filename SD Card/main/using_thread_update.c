#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "esp_log.h"
#include "esp_system.h"
#include "time.h" // For working with system time
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// Pin configuration for SD card communication
#define PIN_NUM_MISO 10
#define PIN_NUM_MOSI 9
#define PIN_NUM_CLK 8
#define PIN_NUM_CS 2
#define EXAMPLE_MAX_CHAR_SIZE 64 // Max size of data buffer
#define MOUNT_POINT "/sdcard"    // SD card mount point

static const char *TAG = "Task"; // Logging tag
static char current_file_name[32];  // Buffer for storing the current file name based on the date
static QueueHandle_t queue;          // Queue for communication between tasks

// Function to get the current date and format the file name based on the date
void get_date_based_filename(char *filename, size_t size)
{
    time_t now;                   // Variable to hold the current time
    struct tm timeinfo;           // Struct to hold broken-down time components
    time(&now);                   // Get the current system time
    localtime_r(&now, &timeinfo); // Convert the time to local time components
    strftime(filename, size, MOUNT_POINT "/%Y%m%d.txt", &timeinfo);
}

// Function to write data to the current file
static esp_err_t s_example_write_file(const char *path, char *data)
{
    ESP_LOGI(TAG, "Opening file %s", path);
    FILE *f = fopen(path, "a"); // Open file in append mode
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return ESP_FAIL;
    }
    fprintf(f, data); // Write the provided data to the file
    fclose(f);        // Close the file after writing
    ESP_LOGI(TAG, "Data written to file");
    return ESP_OK;    // Return success
}

// Function to read the last line from the current file
static esp_err_t s_example_read_latest_data(const char *path)
{
    ESP_LOGI(TAG, "Reading last data from file %s", path); // Log which file is being read
    FILE *f = fopen(path, "r");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return ESP_FAIL;
    }

    // Seek to the end of the file
    fseek(f, 0, SEEK_END);

    // Get file size
    long file_size = ftell(f);
    if (file_size == 0)
    {
        ESP_LOGI(TAG, "File is empty");
        fclose(f);
        return ESP_OK;
    }

    // Start reading backward to find the last line
    char buffer[EXAMPLE_MAX_CHAR_SIZE] = {0};
    long offset = 1;
    while (offset <= file_size)
    {
        fseek(f, -offset, SEEK_END); // Move backward from the end of the file
        if (fgetc(f) == '\n' && offset > 1)
        {
            // Stop at the first newline found (indicating the last full line)
            break;
        }
        offset++;
    }

    // Now read the last line
    fgets(buffer, sizeof(buffer), f);
    fclose(f);
    ESP_LOGI(TAG, "Latest data: %s", buffer);

    return ESP_OK;
}

// Task to handle writing timestamps to the file
void write_task(void *pvParameters)
{
    while (1)
    {
        // Simulate writing data to the current file with a timestamp
        time_t now;
        char data[EXAMPLE_MAX_CHAR_SIZE];
        time(&now);
        snprintf(data, EXAMPLE_MAX_CHAR_SIZE, "Timestamp: %lld\n", now);

        // Write the timestamp data to the current file
        esp_err_t ret = s_example_write_file(current_file_name, data);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to write data to file");
        }

        // Delay for a specified time before writing the next data (e.g., 1 minute)
        vTaskDelay(10000 / portTICK_PERIOD_MS); // Delay for 10 seconds
    }
}

// Task to handle reading the latest data from the file
void read_task(void *pvParameters)
{
    while (1)
    {
        // Read the latest data from the current file
        esp_err_t ret = s_example_read_latest_data(current_file_name);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to read data from file");
        }

        // Delay for a specified time before reading again (e.g., 1 minute)
        vTaskDelay(10000 / portTICK_PERIOD_MS); // Delay for 10 seconds
    }
}

void check_and_update_filename(void)
{
    char new_file_name[32];
    get_date_based_filename(new_file_name, sizeof(new_file_name));

    if (strcmp(current_file_name, new_file_name) != 0)
    {
        strcpy(current_file_name, new_file_name);
        ESP_LOGI(TAG, "File name updated: %s", current_file_name);
    }
}

void app_main(void)
{
    esp_err_t ret;
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    sdmmc_card_t *card;
    ESP_LOGI(TAG, "Initializing SD card");

    // SD card SPI host configuration
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };

    // Initialize the SPI bus for SD card communication
    ret = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize bus.");
        return;
    }

    // Configure the SD card slot
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = host.slot;

    // Mount the FAT filesystem from the SD card
    ESP_LOGI(TAG, "Mounting filesystem");
    ret = esp_vfs_fat_sdspi_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &card);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to mount filesystem.");
        return;
    }
    ESP_LOGI(TAG, "Filesystem mounted");
    sdmmc_card_print_info(stdout, card);

    // Initialize the current file name based on the current date
    get_date_based_filename(current_file_name, sizeof(current_file_name));

    // Create FreeRTOS tasks
    xTaskCreate(write_task, "write_task", 2048, NULL, 1, NULL);
    xTaskCreate(read_task, "read_task", 2048, NULL, 1, NULL);

    // Main loop: check date and update file name if necessary
    while (1)
    {
        check_and_update_filename(); // Check if the date has changed
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay for a while before checking again
    }

    // Clean up when done: unmount the SD card and free SPI bus resources
    esp_vfs_fat_sdcard_unmount(MOUNT_POINT, card);
    ESP_LOGI(TAG, "Card unmounted");
    spi_bus_free(host.slot);
}
