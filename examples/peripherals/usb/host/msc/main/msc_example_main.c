/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <dirent.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_timer.h"
#include "esp_err.h"
#include "esp_log.h"
#include "usb/usb_host.h"
#include "usb/msc_host_vfs.h"
#include "ffconf.h"
#include "errno.h"
#include "driver/gpio.h"

static const char *TAG = "example";

#define MNT_PATH         "/usb"     // Base mount path prefix, devices will be mounted as /usb0, /usb1, /usb2...
#define APP_QUIT_PIN     GPIO_NUM_0 // BOOT button on most boards
#define BUFFER_SIZE      4096       // The read/write performance can be improved with larger buffer for the cost of RAM, 4kB is enough for most usecases
#define MAX_MSC_DEVICES  CONFIG_FATFS_VOLUME_COUNT /*!< Maximum number of simultaneously connected MSC (Mass Storage Class) devices.
                                         Adjust this value based on available system resources and expected use cases. */

/**
 * @brief MSC Device Entry
 *
 * This structure holds information about a connected MSC device,
 * including the USB address, MSC device handle, VFS handle, and assigned mount point.
 */
typedef struct {
    uint8_t usb_addr;                     /*!< USB device address */
    msc_host_device_handle_t msc_device;  /*!< Handle of the MSC device */
    msc_host_vfs_handle_t vfs_handle;     /*!< VFS handle assigned to the MSC device */
} msc_dev_entry_t;

static msc_dev_entry_t *msc_devices[MAX_MSC_DEVICES] = {0};

/**
 * @brief Application Queue and its messages ID
 */
static QueueHandle_t app_queue;
typedef struct {
    enum {
        APP_QUIT,                // Signals request to exit the application
        APP_DEVICE_CONNECTED,    // USB device connect event
        APP_DEVICE_DISCONNECTED, // USB device disconnect event
    } id;
    union {
        uint8_t new_dev_address; // Address of new USB device for APP_DEVICE_CONNECTED event
        msc_host_device_handle_t device_handle; // Handle of removed USB device for APP_DEVICE_DISCONNECTED event
    } data;
} app_message_t;

/**
 * @brief Find a free slot in the device table.
 *
 * @return Index of the free slot, or -1 if no free slot is available.
 */
static inline int find_free_slot(void)
{
    for (int i = 0; i < MAX_MSC_DEVICES; i++) {
        if (msc_devices[i] == NULL) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Allocates a new MSC device entry and mounts it to VFS.
 *
 * This function finds a free slot for a new MSC device, allocates memory for the device entry,
 * installs the MSC device, and mounts it to the virtual file system (VFS).
 *
 * If any step fails, the function ensures proper cleanup of allocated resources before returning an error.
 *
 * @param[in] msg        Message containing the address of the new USB device.
 * @param[out] out_slot  Pointer to store the allocated slot index on success.
 *
 * @return
 *         - ESP_OK on success.
 *         - ESP_ERR_NOT_FOUND if no free slot is available.
 *         - ESP_ERR_NO_MEM if memory allocation fails.
 *         - Other esp_err_t codes if device installation or VFS registration fails.
 */
static esp_err_t allocate_new_msc_device(const app_message_t *msg, int *out_slot)
{
    int slot = find_free_slot();
    if (slot < 0) {
        ESP_LOGW(TAG, "No free slots for new MSC device (max %d)", MAX_MSC_DEVICES);
        return ESP_ERR_NOT_FOUND;
    }

    msc_devices[slot] = calloc(1, sizeof(msc_dev_entry_t));
    if (!msc_devices[slot]) {
        ESP_LOGE(TAG, "Failed to allocate memory for new MSC device entry");
        return ESP_ERR_NO_MEM;
    }
    esp_err_t err = msc_host_install_device(msg->data.new_dev_address, &msc_devices[slot]->msc_device);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "msc_host_install_device failed: %s", esp_err_to_name(err));
        free(msc_devices[slot]);
        msc_devices[slot] = NULL;
        return err;
    }

    msc_devices[slot]->usb_addr = msg->data.new_dev_address;

    const esp_vfs_fat_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 3,
        .allocation_unit_size = 8192,
    };

    char mount_path[16];
    snprintf(mount_path, sizeof(mount_path), MNT_PATH "%d", slot);

    err = msc_host_vfs_register(msc_devices[slot]->msc_device, mount_path, &mount_config, &msc_devices[slot]->vfs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "msc_host_vfs_register failed: %s", esp_err_to_name(err));
        ESP_ERROR_CHECK(msc_host_uninstall_device(msc_devices[slot]->msc_device));
        free(msc_devices[slot]);
        msc_devices[slot] = NULL;
        return err;
    }

    *out_slot = slot;
    return ESP_OK;
}

/**
 * @brief Find a slot by MSC device handle.
 *
 * This function searches for the slot corresponding to a given MSC device handle.
 *
 * @param handle MSC device handle to search for.
 * @return Index of the slot if found, otherwise -1.
 */
static int find_slot_by_handle(msc_host_device_handle_t handle)
{
    for (int i = 0; i < MAX_MSC_DEVICES; i++) {
        if (msc_devices[i] && msc_devices[i]->msc_device == handle) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Free resources associated with a specific MSC device by slot index.
 *
 * This function releases all resources associated with a device identified by its slot index.
 * It unmounts the VFS, uninstalls the MSC device, and frees the allocated memory.
 *
 * @param slot Index of the MSC device in the device array.
 */
static void free_msc_device(int slot)
{
    if (slot < 0 || slot >= MAX_MSC_DEVICES || !msc_devices[slot]) {
        ESP_LOGE(TAG, "Invalid slot index for MSC device deallocation");
        return;
    }

    if (msc_devices[slot]->vfs_handle) {
        ESP_ERROR_CHECK(msc_host_vfs_unregister(msc_devices[slot]->vfs_handle));
    }
    if (msc_devices[slot]->msc_device) {
        ESP_ERROR_CHECK(msc_host_uninstall_device(msc_devices[slot]->msc_device));
    }

    free(msc_devices[slot]);
    msc_devices[slot] = NULL;
}

/**
 * @brief Free all connected MSC devices.
 *
 * Iterates over all allocated MSC devices, unmounts them from VFS, and frees their memory.
 */
static void free_all_msc_devices(void)
{
    for (int i = 0; i < MAX_MSC_DEVICES; i++) {
        if (msc_devices[i]) {
            free_msc_device(i);
        }
    }
}

/**
 * @brief BOOT button pressed callback
 *
 * Signal application to exit the main task
 *
 * @param[in] arg Unused
 */
static void gpio_cb(void *arg)
{
    BaseType_t xTaskWoken = pdFALSE;
    app_message_t message = {
        .id = APP_QUIT,
    };

    if (app_queue) {
        xQueueSendFromISR(app_queue, &message, &xTaskWoken);
    }

    if (xTaskWoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

/**
 * @brief Find a USB addr by MSC device handle.
 *
 * @param handle MSC device handle
 * @return USB addr, or -1 if not found.
 */
static inline int8_t find_usb_addr_by_handle(msc_host_device_handle_t handle)
{
    for (int8_t i = 0; i < MAX_MSC_DEVICES; i++) {
        if (msc_devices[i] && msc_devices[i]->msc_device == handle) {
            return msc_devices[i]->usb_addr;
        }
    }
    return -1;
}

/**
 * @brief MSC driver callback
 *
 * Signal device connection/disconnection to the main task
 *
 * @param[in] event MSC event
 * @param[in] arg   MSC event data
 */
static void msc_event_cb(const msc_host_event_t *event, void *arg)
{
    if (event->event == MSC_DEVICE_CONNECTED) {
        ESP_LOGI(TAG, "MSC device connected (usb_addr=%d)", event->device.address);
        app_message_t message = {
            .id = APP_DEVICE_CONNECTED,
            .data.new_dev_address = event->device.address,
        };
        xQueueSend(app_queue, &message, portMAX_DELAY);
    } else if (event->event == MSC_DEVICE_DISCONNECTED) {
        int usb_addr = find_usb_addr_by_handle(event->device.handle);
        if (usb_addr >= 0) {
            ESP_LOGI(TAG, "MSC device disconnected (usb_addr=%d)", usb_addr);
        } else {
            ESP_LOGW(TAG, "MSC device disconnected, but failed to retrieve USB address");
        }
        app_message_t message = {
            .id = APP_DEVICE_DISCONNECTED,
            .data.device_handle = event->device.handle,
        };
        xQueueSend(app_queue, &message, portMAX_DELAY);
    }
}

/**
 * @brief Print information about the connected MSC device.
 *
 * This function prints detailed information about the connected USB MSC device,
 * such as capacity, sector size, PID, VID, and string descriptors.
 *
 * @param[in] info Pointer to MSC device information structure.
 */
static void print_device_info(msc_host_device_info_t *info)
{
    const size_t megabyte = 1024 * 1024;
    uint64_t capacity = ((uint64_t)info->sector_size * info->sector_count) / megabyte;

    printf("Device info:\n");
    printf("\t Capacity: %llu MB\n", capacity);
    printf("\t Sector size: %"PRIu32"\n", info->sector_size);
    printf("\t Sector count: %"PRIu32"\n", info->sector_count);
    printf("\t PID: 0x%04X \n", info->idProduct);
    printf("\t VID: 0x%04X \n", info->idVendor);
#ifndef CONFIG_NEWLIB_NANO_FORMAT
    wprintf(L"\t iProduct: %S \n", info->iProduct);
    wprintf(L"\t iManufacturer: %S \n", info->iManufacturer);
    wprintf(L"\t iSerialNumber: %S \n", info->iSerialNumber);
#endif
}

/**
 * @brief Perform basic file operations on the mounted USB storage device.
 *
 * This function demonstrates basic file I/O operations:
 *  - Create a directory `/usb<slot>/esp` if it does not exist.
 *  - Create a file `test.txt` in the directory with sample content if it does not exist.
 *  - Read the content of the `test.txt` file and print it to the console.
 *
 * @param[in] slot Index of the mounted USB device (0 to MAX_MSC_DEVICES-1).
 */
static void file_operations(int slot)
{
    char directory[32];
    char file_path[32];
    snprintf(directory, sizeof(directory), MNT_PATH "%d/esp", slot);
    snprintf(file_path, sizeof(file_path), MNT_PATH "%d/esp/test.txt", slot);

    // Create /usb<slot>/esp directory
    struct stat s = {0};
    bool directory_exists = stat(directory, &s) == 0;
    if (!directory_exists) {
        if (mkdir(directory, 0775) != 0) {
            ESP_LOGE(TAG, "mkdir failed with errno: %s", strerror(errno));
        }
    }

    // Create /usb<slot>/esp/test.txt file, if it doesn't exist
    if (stat(file_path, &s) != 0) {
        ESP_LOGI(TAG, "Creating file");
        FILE *f = fopen(file_path, "w");
        if (f == NULL) {
            ESP_LOGE(TAG, "Failed to open file for writing");
            return;
        }
        fprintf(f, "Hello World!\n");
        fclose(f);
    }

    // Read back the file
    FILE *f;
    ESP_LOGI(TAG, "Reading file");
    f = fopen(file_path, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }
    char line[64];
    fgets(line, sizeof(line), f);
    fclose(f);
    // strip newline
    char *pos = strchr(line, '\n');
    if (pos) {
        *pos = '\0';
    }
    ESP_LOGI(TAG, "Read from file '%s': '%s'", file_path, line);
}

/**
 * @brief Perform sequential write and read speed test on the mounted USB storage device.
 *
 * This function performs:
 *  - A write speed test by writing a series of 4KB blocks to a test file.
 *  - A read speed test by reading the same number of 4KB blocks from the file.
 *
 * The results (in MiB/s) are printed to the console.
 *
 * @param[in] slot Index of the mounted USB device (0 to MAX_MSC_DEVICES-1).
 */
static void speed_test(int slot)
{
#define ITERATIONS  256  // 256 * 4kb = 1MB
    int64_t test_start, test_end;
    char test_file[32];
    snprintf(test_file, sizeof(test_file), MNT_PATH "%d/esp/dummy", slot);

    FILE *f = fopen(test_file, "wb+");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }
    // Set larger buffer for this file. It results in larger and more effective USB transfers
    setvbuf(f, NULL, _IOFBF, BUFFER_SIZE);

    // Allocate application buffer used for read/write
    uint8_t *data = malloc(BUFFER_SIZE);
    assert(data);

    ESP_LOGI(TAG, "Writing to file %s", test_file);
    test_start = esp_timer_get_time();
    for (int i = 0; i < ITERATIONS; i++) {
        if (fwrite(data, BUFFER_SIZE, 1, f) == 0) {
            ESP_LOGE(TAG, "Write error");
            fclose(f);
            free(data);
            return;
        }
    }
    test_end = esp_timer_get_time();
    ESP_LOGI(TAG, "Write speed %1.2f MiB/s", (BUFFER_SIZE * ITERATIONS) / (float)(test_end - test_start));
    rewind(f);

    ESP_LOGI(TAG, "Reading from file %s", test_file);
    test_start = esp_timer_get_time();
    for (int i = 0; i < ITERATIONS; i++) {
        if (0 == fread(data, BUFFER_SIZE, 1, f)) {
            ESP_LOGE(TAG, "Read error");
            fclose(f);
            free(data);
            return;
        }
    }
    test_end = esp_timer_get_time();
    ESP_LOGI(TAG, "Read speed %1.2f MiB/s", (BUFFER_SIZE * ITERATIONS) / (float)(test_end - test_start));

    fclose(f);
    free(data);
}

/**
 * @brief USB task
 *
 * Install USB Host Library and MSC driver.
 * Handle USB Host Library events
 *
 * @param[in] args Unused
 */
static void usb_task(void *args)
{
    const usb_host_config_t host_config = { .intr_flags = ESP_INTR_FLAG_LEVEL1 };
    ESP_ERROR_CHECK(usb_host_install(&host_config));

    const msc_host_driver_config_t msc_config = {
        .create_backround_task = true,
        .task_priority = 5,
        .stack_size = 4096,
        .callback = msc_event_cb,
    };
    ESP_ERROR_CHECK(msc_host_install(&msc_config));

    bool has_clients = true;
    while (true) {
        uint32_t event_flags;
        usb_host_lib_handle_events(portMAX_DELAY, &event_flags);

        // Release devices once all clients has deregistered
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
            has_clients = false;
            if (usb_host_device_free_all() == ESP_OK) {
                break;
            };
        }
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE && !has_clients) {
            break;
        }
    }

    vTaskDelay(10); // Give clients some time to uninstall
    ESP_LOGI(TAG, "Deinitializing USB");
    ESP_ERROR_CHECK(usb_host_uninstall());
    vTaskDelete(NULL);
}

/**
 * @brief List contents of the root directories of all mounted USB storage devices.
 *
 * This function iterates over all mounted MSC devices and lists the contents
 * of their root directories. It is intended for debugging and demonstration purposes.
 *
 * For each connected and mounted device, the function attempts to open the root directory
 * `/usb<slot>` and print the names of all files and directories contained within.
 *
 * If opening the directory fails, an error is logged.
 */
static inline void show_list_files_all_devices(void)
{
    ESP_LOGI(TAG, "ls command output for all connected devices:");
    for (int i = 0; i < MAX_MSC_DEVICES; i++) {
        if (msc_devices[i]) {
            char mount_path[16];
            snprintf(mount_path, sizeof(mount_path), MNT_PATH "%d", i);

            ESP_LOGI(TAG, "Listing contents of %s", mount_path);
            struct dirent *d;
            DIR *dh = opendir(mount_path);
            if (!dh) {
                ESP_LOGE(TAG, "Failed to open directory: %s", mount_path);
                continue;
            }

            while ((d = readdir(dh)) != NULL) {
                printf("%s/%s\n", mount_path, d->d_name);
            }
            closedir(dh);
        }
    }
}

void app_main(void)
{
    // Create FreeRTOS primitives
    app_queue = xQueueCreate(5, sizeof(app_message_t));
    assert(app_queue);

    BaseType_t task_created = xTaskCreate(usb_task, "usb_task", 4096, NULL, 2, NULL);
    assert(task_created);

    // Init BOOT button: Pressing the button simulates app request to exit
    // It will disconnect the USB device and uninstall the MSC drivers and USB Host Lib
    const gpio_config_t input_pin = {
        .pin_bit_mask = BIT64(APP_QUIT_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };
    ESP_ERROR_CHECK(gpio_config(&input_pin));
    ESP_ERROR_CHECK(gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1));
    ESP_ERROR_CHECK(gpio_isr_handler_add(APP_QUIT_PIN, gpio_cb, NULL));

    ESP_LOGI(TAG, "Waiting for USB flash drive to be connected");

    // Perform all example operations in a loop to allow USB reconnections
    while (1) {
        app_message_t msg;
        xQueueReceive(app_queue, &msg, portMAX_DELAY);

        if (msg.id == APP_DEVICE_CONNECTED) {
            int slot;
            esp_err_t res = allocate_new_msc_device(&msg, &slot);
            if (res != ESP_OK) {
                continue;
            }
            // 2. Print information about the connected disk
            msc_host_device_info_t info;
            ESP_ERROR_CHECK(msc_host_get_device_info(msc_devices[slot]->msc_device, &info));
            msc_host_print_descriptors(msc_devices[slot]->msc_device);
            print_device_info(&info);

            // 3. List all the files in root directory from all connected msc devices
            show_list_files_all_devices();

            // 4. The disk is mounted to Virtual File System, perform some basic demo file operation
            file_operations(slot);
            // 5. Perform speed test
            speed_test(slot);

            ESP_LOGI(TAG, "Example finished, you can disconnect the USB flash drive (or connect another USB flash drive)");
        }
        if (msg.id == APP_DEVICE_DISCONNECTED) {
            int slot = find_slot_by_handle(msg.data.device_handle);
            if (slot >= 0) {
                free_msc_device(slot);
            }
        }
        if (msg.id == APP_QUIT) {
            free_all_msc_devices();
            msc_host_uninstall();
            break;
        }
    }

    ESP_LOGI(TAG, "Done");
    gpio_isr_handler_remove(APP_QUIT_PIN);
    gpio_uninstall_isr_service();
    vQueueDelete(app_queue);
}
