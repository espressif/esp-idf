/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/* DESCRIPTION:
 * This example contains code to make ESP32 based device recognizable by USB-hosts as a USB Mass Storage Device.
 * It either allows the embedded application i.e. example to access the partition or Host PC accesses the partition over USB MSC.
 * They can't be allowed to access the partition at the same time.
 * SPI Flash and SDMMC can be enabled independently. Enabling both exposes two LUNs to the USB host.
 * For different scenarios and behaviour, Refer to README of this example.
 */

#include <errno.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include "sdkconfig.h"
#include "esp_console.h"
#include "esp_check.h"
#ifdef CONFIG_EXAMPLE_STORAGE_MEDIA_SPIFLASH
#include "esp_partition.h"
#endif
#include "driver/gpio.h"
#include "tinyusb.h"
#include "tinyusb_default_config.h"
#include "tinyusb_msc.h"
#ifdef CONFIG_EXAMPLE_STORAGE_MEDIA_SDMMC
#include "sdmmc_cmd.h"
#include "diskio_impl.h"
#include "diskio_sdmmc.h"
#if CONFIG_EXAMPLE_SD_PWR_CTRL_LDO_INTERNAL_IO
#include "sd_pwr_ctrl_by_on_chip_ldo.h"
#endif // CONFIG_EXAMPLE_SD_PWR_CTRL_LDO_INTERNAL_IO
#endif

#if !CONFIG_EXAMPLE_STORAGE_MEDIA_SPIFLASH && !CONFIG_EXAMPLE_STORAGE_MEDIA_SDMMC
#error "Enable at least one storage media in menuconfig (SPI Flash LUN and/or SDMMC Card LUN)"
#endif

/*
 * We warn if a secondary serial console is enabled. A secondary serial console is always output-only and
 * hence not very useful for interactive console applications. If you encounter this warning, consider disabling
 * the secondary serial console in menuconfig unless you know what you are doing.
 */
#if SOC_USB_SERIAL_JTAG_SUPPORTED
#if !CONFIG_ESP_CONSOLE_SECONDARY_NONE
#warning "A secondary serial console is not useful when using the console component. Please disable it in menuconfig."
#endif
#endif

static const char *TAG = "example_main";
static esp_console_repl_t *repl = NULL;

#define EXAMPLE_MAX_LUNS            2
#define BASE_PATH_SPIFLASH          "/data"
#if CONFIG_EXAMPLE_STORAGE_MEDIA_SPIFLASH
#define BASE_PATH_SDMMC             "/sdcard"
#else
#define BASE_PATH_SDMMC             "/data"
#endif

typedef struct {
    tinyusb_msc_storage_handle_t handle;
    const char *name;
    const char *base_path;
} example_lun_t;

static example_lun_t s_luns[EXAMPLE_MAX_LUNS];
static size_t s_lun_count;

static SemaphoreHandle_t _wait_console_smp = NULL;

/* TinyUSB descriptors
   ********************************************************************* */
#define EPNUM_MSC       1
#define TUSB_DESC_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_MSC_DESC_LEN)

enum {
    ITF_NUM_MSC = 0,
    ITF_NUM_TOTAL
};

enum {
    EDPT_CTRL_OUT = 0x00,
    EDPT_CTRL_IN  = 0x80,

    EDPT_MSC_OUT  = 0x01,
    EDPT_MSC_IN   = 0x81,
};

static tusb_desc_device_t descriptor_config = {
    .bLength = sizeof(descriptor_config),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = TUSB_CLASS_MISC,
    .bDeviceSubClass = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor = 0x303A, // This is Espressif VID. This needs to be changed according to Users / Customers
    .idProduct = 0x4002,
    .bcdDevice = 0x100,
    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x03,
    .bNumConfigurations = 0x01
};

static uint8_t const msc_fs_configuration_desc[] = {
    // Config number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, TUSB_DESC_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

    // Interface number, string index, EP Out & EP In address, EP size
    TUD_MSC_DESCRIPTOR(ITF_NUM_MSC, 0, EDPT_MSC_OUT, EDPT_MSC_IN, 64),
};

#if (TUD_OPT_HIGH_SPEED)
static const tusb_desc_device_qualifier_t device_qualifier = {
    .bLength = sizeof(tusb_desc_device_qualifier_t),
    .bDescriptorType = TUSB_DESC_DEVICE_QUALIFIER,
    .bcdUSB = 0x0200,
    .bDeviceClass = TUSB_CLASS_MISC,
    .bDeviceSubClass = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .bNumConfigurations = 0x01,
    .bReserved = 0
};

static uint8_t const msc_hs_configuration_desc[] = {
    // Config number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, TUSB_DESC_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

    // Interface number, string index, EP Out & EP In address, EP size
    TUD_MSC_DESCRIPTOR(ITF_NUM_MSC, 0, EDPT_MSC_OUT, EDPT_MSC_IN, 512),
};
#endif // TUD_OPT_HIGH_SPEED

static char const *string_desc_arr[] = {
    (const char[]) { 0x09, 0x04 },  // 0: is supported language is English (0x0409)
    "TinyUSB",                      // 1: Manufacturer
    "TinyUSB Device",               // 2: Product
    "123456",                       // 3: Serials
    "Example MSC",                  // 4. MSC
};
/*********************************************************************** TinyUSB descriptors*/

#define PROMPT_STR CONFIG_IDF_TARGET
static int console_unmount(int argc, char **argv);
static int console_read(int argc, char **argv);
static int console_write(int argc, char **argv);
static int console_size(int argc, char **argv);
static int console_status(int argc, char **argv);
static int console_exit(int argc, char **argv);
const esp_console_cmd_t cmds[] = {
    {
        .command = "read",
        .help = "read README.MD from each application-mounted LUN and print its contents",
        .hint = NULL,
        .func = &console_read,
    },
    {
        .command = "write",
        .help = "create file README.MD on each application-mounted LUN if it does not exist",
        .hint = NULL,
        .func = &console_write,
    },
    {
        .command = "size",
        .help = "show storage size and sector size of each application-mounted LUN",
        .hint = NULL,
        .func = &console_size,
    },
    {
        .command = "expose",
        .help = "Expose all LUNs to Host",
        .hint = NULL,
        .func = &console_unmount,
    },
    {
        .command = "status",
        .help = "Status of storage exposure over USB",
        .hint = NULL,
        .func = &console_status,
    },
    {
        .command = "exit",
        .help = "exit from application",
        .hint = NULL,
        .func = &console_exit,
    }
};

static void example_lun_add(tinyusb_msc_storage_handle_t handle, const char *name, const char *base_path)
{
    s_luns[s_lun_count].handle = handle;
    s_luns[s_lun_count].name = name;
    s_luns[s_lun_count].base_path = base_path;
    s_lun_count++;
}

static const char *example_lun_name(tinyusb_msc_storage_handle_t handle)
{
    for (size_t i = 0; i < s_lun_count; i++) {
        if (s_luns[i].handle == handle) {
            return s_luns[i].name;
        }
    }
    return "unknown";
}

static void example_list_files(const char *base_path)
{
    ESP_LOGI(TAG, "ls %s:", base_path);
    struct dirent *d;
    DIR *dh = opendir(base_path);
    if (!dh) {
        if (errno == ENOENT) {
            ESP_LOGE(TAG, "Directory doesn't exist %s", base_path);
        } else {
            ESP_LOGE(TAG, "Unable to read directory %s", base_path);
        }
        return;
    }
    while ((d = readdir(dh)) != NULL) {
        printf("%s\n", d->d_name);
    }
    closedir(dh);
}

// Set mount point to the application and list files on each LUN
static void _mount(void)
{
    ESP_LOGI(TAG, "Mount storage...");
    for (size_t i = 0; i < s_lun_count; i++) {
        ESP_ERROR_CHECK(tinyusb_msc_set_storage_mount_point(s_luns[i].handle, TINYUSB_MSC_STORAGE_MOUNT_APP));
        example_list_files(s_luns[i].base_path);
    }
}

// unmount storage
static int console_unmount(int argc, char **argv)
{
    int already_exposed = 0;
    for (size_t i = 0; i < s_lun_count; i++) {
        tinyusb_msc_mount_point_t mp;
        ESP_ERROR_CHECK(tinyusb_msc_get_storage_mount_point(s_luns[i].handle, &mp));
        if (mp == TINYUSB_MSC_STORAGE_MOUNT_USB) {
            ESP_LOGE(TAG, "%s storage is already exposed", s_luns[i].name);
            already_exposed++;
            continue;
        }
        ESP_LOGI(TAG, "Unmount %s storage...", s_luns[i].name);
        ESP_ERROR_CHECK(tinyusb_msc_set_storage_mount_point(s_luns[i].handle, TINYUSB_MSC_STORAGE_MOUNT_USB));
    }
    return (already_exposed == (int)s_lun_count) ? -1 : 0;
}

// read README.MD from each application-mounted LUN and print its contents
static int console_read(int argc, char **argv)
{
    int ret = 0;
    for (size_t i = 0; i < s_lun_count; i++) {
        tinyusb_msc_mount_point_t mp;
        ESP_ERROR_CHECK(tinyusb_msc_get_storage_mount_point(s_luns[i].handle, &mp));
        if (mp == TINYUSB_MSC_STORAGE_MOUNT_USB) {
            ESP_LOGE(TAG, "%s storage exposed over USB. Application can't read from storage.", s_luns[i].name);
            ret = -1;
            continue;
        }
        char filename[64];
        snprintf(filename, sizeof(filename), "%s/README.MD", s_luns[i].base_path);
        ESP_LOGD(TAG, "read from %s:", filename);
        FILE *ptr = fopen(filename, "r");
        if (ptr == NULL) {
            ESP_LOGE(TAG, "Filename not present - %s", filename);
            ret = -1;
            continue;
        }
        printf("[%s]\n", s_luns[i].name);
        char buf[1024];
        while (fgets(buf, 1000, ptr) != NULL) {
            printf("%s", buf);
        }
        fclose(ptr);
    }
    return ret;
}

// create file README.MD on each application-mounted LUN if it does not exist
static int console_write(int argc, char **argv)
{
    int ret = 0;
    for (size_t i = 0; i < s_lun_count; i++) {
        tinyusb_msc_mount_point_t mp;
        ESP_ERROR_CHECK(tinyusb_msc_get_storage_mount_point(s_luns[i].handle, &mp));
        if (mp == TINYUSB_MSC_STORAGE_MOUNT_USB) {
            ESP_LOGE(TAG, "%s storage exposed over USB. Application can't write to storage.", s_luns[i].name);
            ret = -1;
            continue;
        }
        char filename[64];
        snprintf(filename, sizeof(filename), "%s/README.MD", s_luns[i].base_path);
        ESP_LOGD(TAG, "write to %s:", filename);
        FILE *fd = fopen(filename, "r");
        if (!fd) {
            ESP_LOGW(TAG, "%s doesn't exist yet, creating", filename);
            fd = fopen(filename, "w");
            fprintf(fd, "Mass Storage Devices are one of the most common USB devices. It use Mass Storage Class (MSC) that allow access to their internal data storage.\n");
            fprintf(fd, "In this example, ESP chip will be recognised by host (PC) as Mass Storage Device.\n");
            fprintf(fd, "Upon connection to USB host (PC), the example application will initialize the storage module and then the storage will be seen as removable device on PC.\n");
            fclose(fd);
        } else {
            fclose(fd);
        }
    }
    return ret;
}

// Show storage size and sector size
static int console_size(int argc, char **argv)
{
    int ret = 0;
    for (size_t i = 0; i < s_lun_count; i++) {
        tinyusb_msc_mount_point_t mp;
        ESP_ERROR_CHECK(tinyusb_msc_get_storage_mount_point(s_luns[i].handle, &mp));
        if (mp == TINYUSB_MSC_STORAGE_MOUNT_USB) {
            ESP_LOGE(TAG, "%s storage exposed over USB. Application can't access storage", s_luns[i].name);
            ret = -1;
            continue;
        }

        uint32_t sec_count;
        uint32_t sec_size;

        ESP_ERROR_CHECK(tinyusb_msc_get_storage_sector_size(s_luns[i].handle, &sec_size));
        ESP_ERROR_CHECK(tinyusb_msc_get_storage_capacity(s_luns[i].handle, &sec_count));

        uint64_t total_bytes = (uint64_t)sec_size * sec_count;
        if (total_bytes >= (1024 * 1024)) {
            uint64_t total_mb = total_bytes / (1024 * 1024);
            printf("%s Storage Capacity %lluMB\n", s_luns[i].name, total_mb);
        } else {
            uint64_t total_kb = total_bytes / 1024;
            printf("%s Storage Capacity %lluKB\n", s_luns[i].name, total_kb);
        }
    }
    return ret;
}

// Show storage status
static int console_status(int argc, char **argv)
{
    for (size_t i = 0; i < s_lun_count; i++) {
        tinyusb_msc_mount_point_t mp;
        ESP_ERROR_CHECK(tinyusb_msc_get_storage_mount_point(s_luns[i].handle, &mp));
        printf("%s storage exposed over USB: %s\n", s_luns[i].name, (mp == TINYUSB_MSC_STORAGE_MOUNT_USB) ? "Yes" : "No");
    }
    return 0;
}

// Exit from application
static int console_exit(int argc, char **argv)
{
    for (size_t i = 0; i < s_lun_count; i++) {
        ESP_ERROR_CHECK(tinyusb_msc_delete_storage(s_luns[i].handle));
        s_luns[i].handle = NULL;
    }
    ESP_ERROR_CHECK(tinyusb_driver_uninstall());

    xSemaphoreGive(_wait_console_smp);

    printf("Application Exit\n");

    return 0;
}

/**
 * @brief Storage mount changed callback
 *
 * @param handle Storage handle
 * @param event Event information
 * @param arg User argument, provided during callback registration
 */
static void storage_mount_changed_cb(tinyusb_msc_storage_handle_t handle, tinyusb_msc_event_t *event, void *arg)
{
    switch (event->id) {
    case TINYUSB_MSC_EVENT_MOUNT_START:
        // Verify that all the files are closed before unmounting
        break;
    case TINYUSB_MSC_EVENT_MOUNT_COMPLETE:
        ESP_LOGI(TAG, "%s storage mounted to application: %s", example_lun_name(handle),
                 (event->mount_point == TINYUSB_MSC_STORAGE_MOUNT_APP) ? "Yes" : "No");
        break;
    case TINYUSB_MSC_EVENT_MOUNT_FAILED:
    case TINYUSB_MSC_EVENT_FORMAT_REQUIRED:
        ESP_LOGE(TAG, "Storage mount failed or format required");
        break;
    default:
        break;
    }
}

#ifdef CONFIG_EXAMPLE_STORAGE_MEDIA_SPIFLASH
static esp_err_t storage_init_spiflash(wl_handle_t *wl_handle)
{
    ESP_LOGI(TAG, "Initializing wear levelling");

    const esp_partition_t *data_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT, NULL);
    if (data_partition == NULL) {
        ESP_LOGE(TAG, "Failed to find FATFS partition. Check the partition table.");
        return ESP_ERR_NOT_FOUND;
    }

    return wl_mount(data_partition, wl_handle);
}
#endif  // CONFIG_EXAMPLE_STORAGE_MEDIA_SPIFLASH

#ifdef CONFIG_EXAMPLE_STORAGE_MEDIA_SDMMC
static esp_err_t storage_init_sdmmc(sdmmc_card_t **card)
{
    esp_err_t ret = ESP_OK;
    bool host_init = false;
    sdmmc_card_t *sd_card;

    ESP_LOGI(TAG, "Initializing SDCard");

    // By default, SD card frequency is initialized to SDMMC_FREQ_DEFAULT (20MHz)
    // For setting a specific frequency, use host.max_freq_khz (range 400kHz - 40MHz for SDMMC)
    // Example: for fixed frequency of 10MHz, use host.max_freq_khz = 10000;
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();

    // For SoCs where the SD power can be supplied both via an internal or external (e.g. on-board LDO) power supply.
    // When using specific IO pins (which can be used for ultra high-speed SDMMC) to connect to the SD card
    // and the internal LDO power supply, we need to initialize the power supply first.
#if CONFIG_EXAMPLE_SD_PWR_CTRL_LDO_INTERNAL_IO
    sd_pwr_ctrl_ldo_config_t ldo_config = {
        .ldo_chan_id = CONFIG_EXAMPLE_SD_PWR_CTRL_LDO_IO_ID,
    };
    sd_pwr_ctrl_handle_t pwr_ctrl_handle = NULL;

    ret = sd_pwr_ctrl_new_on_chip_ldo(&ldo_config, &pwr_ctrl_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create a new on-chip LDO power control driver");
        return ret;
    }
    host.pwr_ctrl_handle = pwr_ctrl_handle;
#endif

#if SOC_SDMMC_IO_UHS_POWER_EXTERNAL
    //for uhs-i power
    host.io_voltage = 1.8f;
#endif

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    // For SD Card, set bus width to use
#ifdef CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
    slot_config.width = 4;
#else
    slot_config.width = 1;
#endif  // CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4

    // On chips where the GPIOs used for SD card can be configured, set the user defined values
#ifdef CONFIG_SOC_SDMMC_USE_GPIO_MATRIX
    slot_config.clk = CONFIG_EXAMPLE_PIN_CLK;
    slot_config.cmd = CONFIG_EXAMPLE_PIN_CMD;
    slot_config.d0 = CONFIG_EXAMPLE_PIN_D0;
#ifdef CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
    slot_config.d1 = CONFIG_EXAMPLE_PIN_D1;
    slot_config.d2 = CONFIG_EXAMPLE_PIN_D2;
    slot_config.d3 = CONFIG_EXAMPLE_PIN_D3;
#endif  // CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4

#endif  // CONFIG_SOC_SDMMC_USE_GPIO_MATRIX

    // Enable internal pullups on enabled pins. The internal pullups
    // are insufficient however, please make sure 10k external pullups are
    // connected on the bus. This is for debug / example purpose only.
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

    // not using ff_memalloc here, as allocation in internal RAM is preferred
    sd_card = (sdmmc_card_t *)malloc(sizeof(sdmmc_card_t));
    ESP_GOTO_ON_FALSE(sd_card, ESP_ERR_NO_MEM, clean, TAG, "could not allocate new sdmmc_card_t");

    ESP_GOTO_ON_ERROR((*host.init)(), clean, TAG, "Host Config Init fail");
    host_init = true;

    ESP_GOTO_ON_ERROR(sdmmc_host_init_slot(host.slot, (const sdmmc_slot_config_t *) &slot_config),
                      clean, TAG, "Host init slot fail");

    while (sdmmc_card_init(&host, sd_card)) {
        ESP_LOGE(TAG, "The detection pin of the slot is disconnected(Insert uSD card). Retrying...");
        vTaskDelay(pdMS_TO_TICKS(3000));
    }

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, sd_card);
    *card = sd_card;

    return ESP_OK;

clean:
    if (host_init) {
        if (host.flags & SDMMC_HOST_FLAG_DEINIT_ARG) {
            host.deinit_p(host.slot);
        } else {
            (*host.deinit)();
        }
    }
    if (sd_card) {
        free(sd_card);
        sd_card = NULL;
    }
#if CONFIG_EXAMPLE_SD_PWR_CTRL_LDO_INTERNAL_IO
    // We don't need to duplicate error here as all error messages are handled via sd_pwr_* call
    sd_pwr_ctrl_del_on_chip_ldo(pwr_ctrl_handle);
#endif // CONFIG_EXAMPLE_SD_PWR_CTRL_LDO_INTERNAL_IO
    return ret;
}
#endif  // CONFIG_EXAMPLE_STORAGE_MEDIA_SDMMC

void app_main(void)
{
    ESP_LOGI(TAG, "Initializing storage...");

    _wait_console_smp = xSemaphoreCreateBinary();
    if (_wait_console_smp == NULL) {
        ESP_LOGE(TAG, "Failed to create semaphore");
        return;
    }

    tinyusb_msc_storage_config_t storage_cfg = {
        .mount_point = TINYUSB_MSC_STORAGE_MOUNT_USB,       // Initial mount point to USB
        .fat_fs = {
            .base_path = NULL,                              // Use default base path
            .config.max_files = 5,                          // Maximum number of files that can be opened simultaneously
            .format_flags = 0,                              // No special format flags
        },
    };

#ifdef CONFIG_EXAMPLE_STORAGE_MEDIA_SPIFLASH
    static wl_handle_t wl_handle = WL_INVALID_HANDLE;
    tinyusb_msc_storage_handle_t spiflash_hdl = NULL;
    ESP_ERROR_CHECK(storage_init_spiflash(&wl_handle));
    storage_cfg.medium.wl_handle = wl_handle;
    storage_cfg.fat_fs.base_path = (char *)BASE_PATH_SPIFLASH;
    ESP_ERROR_CHECK(tinyusb_msc_new_storage_spiflash(&storage_cfg, &spiflash_hdl));
    example_lun_add(spiflash_hdl, "spiflash", BASE_PATH_SPIFLASH);
#endif  // CONFIG_EXAMPLE_STORAGE_MEDIA_SPIFLASH

#ifdef CONFIG_EXAMPLE_STORAGE_MEDIA_SDMMC
    static sdmmc_card_t *card = NULL;
    tinyusb_msc_storage_handle_t sdmmc_hdl = NULL;
    ESP_ERROR_CHECK(storage_init_sdmmc(&card));
    storage_cfg.medium.card = card;
    storage_cfg.fat_fs.base_path = (char *)BASE_PATH_SDMMC;
    ESP_ERROR_CHECK(tinyusb_msc_new_storage_sdmmc(&storage_cfg, &sdmmc_hdl));
    example_lun_add(sdmmc_hdl, "sdmmc", BASE_PATH_SDMMC);
#endif  // CONFIG_EXAMPLE_STORAGE_MEDIA_SDMMC

    // Configure the callback for mount changed events
    ESP_ERROR_CHECK(tinyusb_msc_set_storage_callback(storage_mount_changed_cb, NULL));
    // Re-mount to the APP
    _mount();

    ESP_LOGI(TAG, "USB MSC initialization");

    tinyusb_config_t tusb_cfg = TINYUSB_DEFAULT_CONFIG();

    tusb_cfg.descriptor.device = &descriptor_config;
    tusb_cfg.descriptor.full_speed_config = msc_fs_configuration_desc;
    tusb_cfg.descriptor.string = string_desc_arr;
    tusb_cfg.descriptor.string_count = sizeof(string_desc_arr) / sizeof(string_desc_arr[0]);
#if (TUD_OPT_HIGH_SPEED)
    tusb_cfg.descriptor.high_speed_config = msc_hs_configuration_desc;
    tusb_cfg.descriptor.qualifier = &device_qualifier;
#endif // TUD_OPT_HIGH_SPEED

    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    ESP_LOGI(TAG, "USB MSC initialization DONE");

    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    /* Prompt to be printed before each line.
     * This can be customized, made dynamic, etc.
     */
    repl_config.prompt = PROMPT_STR ">";
    repl_config.max_cmdline_length = 64;

    // Init console based on menuconfig settings
#if defined(CONFIG_ESP_CONSOLE_UART_DEFAULT) || defined(CONFIG_ESP_CONSOLE_UART_CUSTOM)
    esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &repl));

    // USJ console can be set only on esp32p4, having separate USB PHYs for USB_OTG and USJ
#elif defined(CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG) && defined(CONFIG_IDF_TARGET_ESP32P4)
    esp_console_dev_usb_serial_jtag_config_t hw_config = ESP_CONSOLE_DEV_USB_SERIAL_JTAG_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_serial_jtag(&hw_config, &repl_config, &repl));

#else
#error Unsupported console type
#endif

    for (int count = 0; count < sizeof(cmds) / sizeof(esp_console_cmd_t); count++) {
        ESP_ERROR_CHECK(esp_console_cmd_register(&cmds[count]));
    }

    ESP_ERROR_CHECK(esp_console_start_repl(repl));

    xSemaphoreTake(_wait_console_smp, portMAX_DELAY);
    ESP_ERROR_CHECK(esp_console_stop_repl(repl));
    vSemaphoreDelete(_wait_console_smp);
}
