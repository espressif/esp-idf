
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "esp_log.h"
#include "esp_system.h"
#include "time.h" // For working with system time

// Pin configuration for SD card communication
#define PIN_NUM_MISO 10
#define PIN_NUM_MOSI 9
#define PIN_NUM_CLK 8
#define PIN_NUM_CS 2
#define EXAMPLE_MAX_CHAR_SIZE 64 // Max size of data buffer
#define MOUNT_POINT "/sdcard"    // SD card mount point

static const char *TAG = "example"; // Logging tag
static char current_file_name[32];  // Buffer for storing the current file name based on the date

// Function to get the current date and format the file name based on the date
void get_date_based_filename(char *filename, size_t size)
{
    time_t now;                   // Variable to hold the current time
    struct tm timeinfo;           // Struct to hold broken-down time components
    time(&now);                   // Get the current system time
    localtime_r(&now, &timeinfo); // Convert the time to local time components (year, month, day, etc.)
    // Format the filename using the date, example format: "/sdcard/20241003.txt" (YYYYMMDD.txt)
    strftime(filename, size, MOUNT_POINT "/%Y%m%d.txt", &timeinfo);
}

// Function to write data to the current file
// Opens the file in append mode and writes the provided data
static esp_err_t s_example_write_file(const char *path, char *data)
{
    ESP_LOGI(TAG, "Opening file %s", path); // Log which file is being opened
    FILE *f = fopen(path, "a");             // Open file in append mode ("a")
    if (f == NULL)
    {                                                     // Check if file opening failed
        ESP_LOGE(TAG, "Failed to open file for writing"); // Log the error
        return ESP_FAIL;
    }
    fprintf(f, data);                      // Write the provided data to the file
    fclose(f);                             // Close the file after writing
    ESP_LOGI(TAG, "Data written to file"); // Log that data has been written
    return ESP_OK;                         // Return success
}

// Function to read data from the current file
// static esp_err_t s_example_read_file(const char *path)
// {
//     ESP_LOGI(TAG, "Reading file %s", path); // Log which file is being read
//     FILE *f = fopen(path, "r");
//     if (f == NULL)
//     {
//         ESP_LOGE(TAG, "FAILED TO OPEN FILE FOR READING");
//         return ESP_FAIL;
//     }
//     char line[EXAMPLE_MAX_CHAR_SIZE] = {0};
//     fgets(line, sizeof(line), f);
//     fclose(f);
//     ESP_LOGI(TAG, "READ FROM FILE: %s", line);
//     return ESP_OK;
// }

// Function to read the last line from the current file
static esp_err_t s_example_read_file(const char *path)
{
    ESP_LOGI(TAG, "Reading last data from file %s", path); // Log which file is being read
    FILE *f = fopen(path, "r");
    if (f == NULL)
    {
        ESP_LOGI(TAG, "Failed to open file for reading");
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


// Function to check if the date has changed and update the file name if necessary
void check_and_update_filename()
{
    char new_file_name[32];                                        // Buffer for the new file name
    get_date_based_filename(new_file_name, sizeof(new_file_name)); // Get the current date-based file name

    // Compare the new file name with the previously stored file name
    if (strcmp(new_file_name, current_file_name) != 0)
    {
        // If the date has changed (file names differ), update the current file name
        strcpy(current_file_name, new_file_name);
        ESP_LOGI(TAG, "New day detected, switching to new file: %s", current_file_name); // Log the new file name
    }
}

void app_main(void)
{
    esp_err_t ret; // Variable for error handling
    // Configure FAT filesystem options when mounting the SD card
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,  // Do not format the SD card if mounting fails
        .max_files = 5,                   // Maximum number of open files at the same time
        .allocation_unit_size = 16 * 1024 // Allocation unit size
    };
    sdmmc_card_t *card;                    // Structure to hold SD card information
    ESP_LOGI(TAG, "Initializing SD card"); // Log SD card initialization

    // SD card SPI host configuration
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI, // MOSI pin
        .miso_io_num = PIN_NUM_MISO, // MISO pin
        .sclk_io_num = PIN_NUM_CLK,  // Clock pin
        .quadwp_io_num = -1,         // Not used in this example
        .quadhd_io_num = -1,         // Not used in this example
        .max_transfer_sz = 4000,     // Maximum transfer size in bytes
    };

    // Initialize the SPI bus for SD card communication
    ret = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
    if (ret != ESP_OK)
    {                                               // Check if SPI bus initialization failed
        ESP_LOGE(TAG, "Failed to initialize bus."); // Log the error
        return;
    }

    // Configure the SD card slot (chip select pin, etc.)
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS; // Chip select pin for the SD card
    slot_config.host_id = host.slot;  // SPI host ID

    // Mount the FAT filesystem from the SD card
    ESP_LOGI(TAG, "Mounting filesystem");
    ret = esp_vfs_fat_sdspi_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &card);
    if (ret != ESP_OK)
    {                                                 // Check if mounting the filesystem failed
        ESP_LOGE(TAG, "Failed to mount filesystem."); // Log the error
        return;
    }
    ESP_LOGI(TAG, "Filesystem mounted"); // Log that the filesystem has been mounted
    sdmmc_card_print_info(stdout, card); // Print SD card information

    // Initialize the current file name based on the current date
    get_date_based_filename(current_file_name, sizeof(current_file_name));

    // Main loop: check date, write data, and handle file changes
    while (1)
    {
        // Periodically check if the date has changed and update the file name accordingly
        check_and_update_filename();

        // Simulate writing data to the current file with a timestamp
        time_t now;                       // Variable to store current time
        char data[EXAMPLE_MAX_CHAR_SIZE]; // Buffer to store data
        time(&now);                       // Get the current time
        // Format the data as "Timestamp: <current_time>"
        snprintf(data, EXAMPLE_MAX_CHAR_SIZE, "Timestamp: %lld\n", now);

        // Write the timestamp data to the current file
        ret = s_example_write_file(current_file_name, data);
        if (ret != ESP_OK)
        {                                                  // Check if writing failed
            ESP_LOGE(TAG, "Failed to write data to file"); // Log the error
        }

        // READ DATA FROM CURRENT FILE
        // Read the data from the current file
        ret = s_example_read_file(current_file_name);
        if (ret != ESP_OK)
        {                                                   // Check if reading failed
            ESP_LOGE(TAG, "Failed to read data from file"); // Log the error
        }
        // Delay for a specified time before writing the next data (e.g., 1 minute)
        vTaskDelay(60000 / portTICK_PERIOD_MS); // Delay for 60 seconds
    }

    // Clean up when done: unmount the SD card and free SPI bus resources
    esp_vfs_fat_sdcard_unmount(MOUNT_POINT, card);
    ESP_LOGI(TAG, "Card unmounted"); // Log that the SD card has been unmounted
    spi_bus_free(host.slot);         // Free the SPI bus
}
