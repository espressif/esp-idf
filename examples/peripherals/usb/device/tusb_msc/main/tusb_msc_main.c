/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/* DESCRIPTION:
 * This example contains code to make ESP32-S3 based device recognizable by USB-hosts as a USB Mass Storage Device.
 * It either allows the embedded application ie example to access the partition or Host PC accesses the partition over USB MSC.
 * They can't be allowed to access the partition at the same time.
 * The access to the underlying block device is provided by functions in tusb_msc_storage.c
 * For different scenarios and behaviour, Refer to README of this example.
 */

#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include "esp_log.h"
#include "esp_console.h"
#include "tinyusb.h"
#include "class/msc/msc_device.h"
#include "tusb_msc_storage.h"
#include "driver/gpio.h"

static const char *TAG = "example_msc_main";
#define PROMPT_STR CONFIG_IDF_TARGET

/********* TinyUSB MSC callbacks ***************/

/** SCSI ASC/ASCQ codes. **/
/** User can add and use more codes as per the need of the application **/
#define SCSI_CODE_ASC_MEDIUM_NOT_PRESENT 0x3A /** SCSI ASC code for 'MEDIUM NOT PRESENT' **/
#define SCSI_CODE_ASC_INVALID_COMMAND_OPERATION_CODE 0x20 /** SCSI ASC code for 'INVALID COMMAND OPERATION CODE' **/
#define SCSI_CODE_ASCQ 0x00

static void _mount(void);
static void _unmount(void);
static bool is_eject = false;

// Invoked when received SCSI_CMD_INQUIRY
// Application fill vendor id, product id and revision with string up to 8, 16, 4 characters respectively
void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4])
{
    (void) lun;
    ESP_LOGD(TAG, "tud_msc_inquiry_cb() invoked");

    const char vid[] = "TinyUSB";
    const char pid[] = "Flash Storage";
    const char rev[] = "0.1";

    memcpy(vendor_id, vid, strlen(vid));
    memcpy(product_id, pid, strlen(pid));
    memcpy(product_rev, rev, strlen(rev));
}

// Invoked when received Test Unit Ready command.
// return true allowing host to read/write this LUN e.g SD card inserted
bool tud_msc_test_unit_ready_cb(uint8_t lun)
{
    (void) lun;
    ESP_LOGD(TAG, "tud_msc_test_unit_ready_cb() invoked");

    if (is_eject) {
        tud_msc_set_sense(lun, SCSI_SENSE_NOT_READY, SCSI_CODE_ASC_MEDIUM_NOT_PRESENT, SCSI_CODE_ASCQ);
        return false;
    } else {
        ESP_LOGD(TAG, "tud_msc_test_unit_ready_cb: MSC START: Expose Over USB");
        _unmount();
        return true;
    }
}

// Invoked when received SCSI_CMD_READ_CAPACITY_10 and SCSI_CMD_READ_FORMAT_CAPACITY to determine the disk size
// Application update block count and block size
void tud_msc_capacity_cb(uint8_t lun, uint32_t *block_count, uint16_t *block_size)
{
    (void) lun;

    size_t size = storage_get_size();
    size_t sec_size = storage_get_sector_size();
    ESP_LOGI(TAG, "tud_msc_capacity_cb() size(%d), sec_size(%d)", size, sec_size);
    *block_count = size / sec_size;
    *block_size  = sec_size;
}

// Invoked when received Start Stop Unit command
// - Start = 0 : stopped power mode, if load_eject = 1 : unload disk storage
// - Start = 1 : active mode, if load_eject = 1 : load disk storage
bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition, bool start, bool load_eject)
{
    (void) lun;
    (void) power_condition;
    ESP_LOGI(TAG, "tud_msc_start_stop_cb() invoked, power_condition=%d, start=%d, load_eject=%d", power_condition, start, load_eject);

    if (load_eject && !start) {
        is_eject = true;
        ESP_LOGI(TAG, "tud_msc_start_stop_cb: MSC EJECT: Mount on Example");
        _mount();
    }
    return true;
}

// Invoked when received SCSI READ10 command
// - Address = lba * BLOCK_SIZE + offset
// - Application fill the buffer (up to bufsize) with address contents and return number of read byte.
int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void *buffer, uint32_t bufsize)
{
    ESP_LOGD(TAG, "tud_msc_read10_cb() invoked, lun=%d, lba=%lu, offset=%lu, bufsize=%lu", lun, lba, offset, bufsize);

    size_t addr = lba * storage_get_sector_size() + offset;
    esp_err_t err = storage_read_sector(addr, bufsize, buffer);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "storage_read_sector failed: 0x%x", err);
        return 0;
    }
    return bufsize;
}

// Invoked when received SCSI WRITE10 command
// - Address = lba * BLOCK_SIZE + offset
// - Application write data from buffer to address contents (up to bufsize) and return number of written byte.
int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t *buffer, uint32_t bufsize)
{
    ESP_LOGD(TAG, "tud_msc_write10_cb() invoked, lun=%d, lba=%lu, offset=%lu", lun, lba, offset);

    size_t addr = lba * storage_get_sector_size() + offset;
    esp_err_t err = storage_write_sector(addr, bufsize, buffer);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "storage_write_sector failed: 0x%x", err);
        return 0;
    }
    return bufsize;
}

/**
 * Invoked when received an SCSI command not in built-in list below.
 * - READ_CAPACITY10, READ_FORMAT_CAPACITY, INQUIRY, TEST_UNIT_READY, START_STOP_UNIT, MODE_SENSE6, REQUEST_SENSE
 * - READ10 and WRITE10 has their own callbacks
 *
 * \param[in]   lun         Logical unit number
 * \param[in]   scsi_cmd    SCSI command contents which application must examine to response accordingly
 * \param[out]  buffer      Buffer for SCSI Data Stage.
 *                            - For INPUT: application must fill this with response.
 *                            - For OUTPUT it holds the Data from host
 * \param[in]   bufsize     Buffer's length.
 *
 * \return      Actual bytes processed, can be zero for no-data command.
 * \retval      negative    Indicate error e.g unsupported command, tinyusb will \b STALL the corresponding
 *                          endpoint and return failed status in command status wrapper phase.
 */
int32_t tud_msc_scsi_cb(uint8_t lun, uint8_t const scsi_cmd[16], void *buffer, uint16_t bufsize)
{
    int32_t ret;

    ESP_LOGD(TAG, "tud_msc_scsi_cb() invoked. bufsize=%d", bufsize);

    switch (scsi_cmd[0]) {
    case SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL:
        /* SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL is the Prevent/Allow Medium Removal
        command (1Eh) that requests the library to enable or disable user access to
        the storage media/partition. */
        ESP_LOGI(TAG, "tud_msc_scsi_cb() invoked: SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL");
        ret = 0;
        break;
    default:
        ESP_LOGW(TAG, "tud_msc_scsi_cb() invoked: %d", scsi_cmd[0]);
        tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, SCSI_CODE_ASC_INVALID_COMMAND_OPERATION_CODE, SCSI_CODE_ASCQ);
        ret = -1;
        break;
    }
    return ret;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
    is_eject = true;
    ESP_LOGI(TAG, "tud_umount_cb: Mount on Example");
    _mount();
}

// Invoked when device is mounted (configured)
void tud_mount_cb(void)
{
    ESP_LOGI(TAG, "tud_mount_cb MSC START: Expose Over USB");
    _unmount();
}

/************* Application Code *******************/

/************* TinyUSB descriptors ****************/
#define EPNUM_MSC       1
#define TUSB_DESC_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_MSC_DESC_LEN)
#define VBUS_MONITORING_GPIO_NUM GPIO_NUM_4

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

static uint8_t const desc_configuration[] = {
    // Config number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, TUSB_DESC_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

    // Interface number, string index, EP Out & EP In address, EP size
    TUD_MSC_DESCRIPTOR(ITF_NUM_MSC, 0, EDPT_MSC_OUT, EDPT_MSC_IN, TUD_OPT_HIGH_SPEED ? 512 : 64),
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

static char const *string_desc_arr[] = {
    (const char[]) { 0x09, 0x04 },  // 0: is supported language is English (0x0409)
    "TinyUSB",                      // 1: Manufacturer
    "TinyUSB Device",               // 2: Product
    "123456",                       // 3: Serials
    "Example MSC",                  // 4. MSC
};

#define BASE_PATH "/data" // base path to mount the partition
static bool is_mount = false;

// mount the partition and show all the files in BASE_PATH
static void _mount(void)
{
    ESP_LOGI(TAG, "Mount storage...");
    if (!is_mount) {
        ESP_ERROR_CHECK(storage_mount(BASE_PATH));
        is_mount = true;
    }

    // List all the files in this directory
    ESP_LOGI(TAG, "\nls command output:");
    struct dirent *d;
    DIR *dh = opendir(BASE_PATH);
    if (!dh) {
        if (errno = ENOENT) {
            //If the directory is not found
            ESP_LOGE(TAG, "Directory doesn't exist %s", BASE_PATH);
        } else {
            //If the directory is not readable then throw error and exit
            ESP_LOGE(TAG, "Unable to read directory %s", BASE_PATH);
        }
        return;
    }
    //While the next entry is not readable we will print directory files
    while ((d = readdir(dh)) != NULL) {
        printf("%s\n", d->d_name);
    }
    return;
}

// unmount the partition
static void _unmount(void)
{
    if (!is_mount) {
        ESP_LOGD(TAG, "storage exposed over USB...");
        return;
    }
    ESP_LOGI(TAG, "Unmount storage...");
    ESP_ERROR_CHECK(storage_unmount());
    is_mount = false;
    is_eject = false;
}

static int f_unmount(int argc, char **argv)
{
    _unmount();
    return 0;
}

// read BASE_PATH/README.MD and print its contents
static int f_read(int argc, char **argv)
{
    if (!is_mount) {
        ESP_LOGE(TAG, "storage exposed over USB. Application can't read from storage.");
        return -1;
    }
    ESP_LOGD(TAG, "read from storage:");
    const char *filename = BASE_PATH "/README.MD";
    FILE *ptr = fopen(filename, "r");
    if (ptr == NULL) {
        ESP_LOGE(TAG, "Filename not present - %s", filename);
        return -1;
    }
    char buf[1024];
    while (fgets(buf, 1000, ptr) != NULL) {
        printf("%s", buf);
    }
    fclose(ptr);
    return 0;
}

// create file BASE_PATH/README.MD if it does not exist
static int f_write(int argc, char **argv)
{
    if (!is_mount) {
        ESP_LOGE(TAG, "storage exposed over USB. Application can't write to storage.");
        return -1;
    }
    ESP_LOGD(TAG, "write to storage:");
    const char *filename = BASE_PATH "/README.MD";
    FILE *fd = fopen(filename, "r");
    if (!fd) {
        ESP_LOGW(TAG, "README.MD doesn't exist yet, creating");
        fd = fopen(filename, "w");
        fprintf(fd, "Mass Storage Devices are one of the most common USB devices. It use Mass Storage Class (MSC) that allow access to their internal data storage.\n");
        fprintf(fd, "In this example, ESP chip will be recognised by host (PC) as Mass Storage Device.\n");
        fprintf(fd, "Upon connection to USB host (PC), the example application will initialize the storage module and then the storage will be seen as removable device on PC.\n");
        fclose(fd);
    }
    return 0;
}

// Show storage size and sector size
static int f_size(int argc, char **argv)
{
    if (!is_mount) {
        ESP_LOGE(TAG, "storage exposed over USB. Application can't access storage");
        return -1;
    }
    size_t size = storage_get_size();
    size_t sec_size = storage_get_sector_size();
    printf("storage size(%d), sec_size(%d)\n", size, sec_size);
    return 0;
}

// exit from application
static int f_status(int argc, char **argv)
{
    printf("storage exposed over USB: %s\n", is_mount ? "No" : "Yes");
    return 0;
}

// exit from application
static int f_exit(int argc, char **argv)
{
    printf("Application Exiting\n");
    exit(0);
    return 0;
}

void app_main(void)
{
    // Configure GPIO Pin for vbus monitorung
    const gpio_config_t vbus_gpio_config = {
        .pin_bit_mask = BIT64(VBUS_MONITORING_GPIO_NUM),
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pull_up_en = true,
        .pull_down_en = false,
    };
    ESP_ERROR_CHECK(gpio_config(&vbus_gpio_config));

    ESP_LOGI(TAG, "Initializing storage...");
    ESP_ERROR_CHECK(storage_init());

    ESP_LOGI(TAG, "USB MSC initialization");
    const tinyusb_config_t tusb_cfg = {
        .device_descriptor = &descriptor_config,
        .string_descriptor = string_desc_arr,
        .external_phy = false,
        .configuration_descriptor = desc_configuration,
        .self_powered = true,
        .vbus_monitor_io = VBUS_MONITORING_GPIO_NUM,
    };
    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));
    ESP_LOGI(TAG, "USB MSC initialization DONE");

    //mounted in the app by default
    _mount();

    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    /* Prompt to be printed before each line.
     * This can be customized, made dynamic, etc.
     */
    repl_config.prompt = PROMPT_STR ">";
    repl_config.max_cmdline_length = 64;
    esp_console_register_help_command();
    esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &repl));

    const esp_console_cmd_t cmd_read = {
        .command = "read",
        .help = "read BASE_PATH/README.MD and print its contents",
        .hint = NULL,
        .func = &f_read,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd_read) );

    const esp_console_cmd_t cmd_write = {
        .command = "write",
        .help = "create file BASE_PATH/README.MD if it does not exist",
        .hint = NULL,
        .func = &f_write,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd_write) );

    const esp_console_cmd_t cmd_size = {
        .command = "size",
        .help = "show storage size and sector size",
        .hint = NULL,
        .func = &f_size,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd_size) );

    const esp_console_cmd_t cmd_umount = {
        .command = "expose",
        .help = "Expose Storage to Host",
        .hint = NULL,
        .func = &f_unmount,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd_umount) );

    const esp_console_cmd_t cmd_status = {
        .command = "status",
        .help = "Status of storage exposure over USB",
        .hint = NULL,
        .func = &f_status,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd_status) );

    const esp_console_cmd_t cmd_exit = {
        .command = "exit",
        .help = "exit from application",
        .hint = NULL,
        .func = &f_exit,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd_exit) );

    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}
