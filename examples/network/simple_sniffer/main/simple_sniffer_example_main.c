/* Sniffer example.
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sdkconfig.h"
#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"
#include "esp_console.h"
#include "esp_event.h"
#include "esp_vfs_fat.h"
#include "esp_wifi.h"
#include "ethernet_init.h"
#include "esp_err.h"
#include "esp_log.h"
#if CONFIG_SNIFFER_PCAP_DESTINATION_SD
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#endif
#include "nvs_flash.h"
#include "sdmmc_cmd.h"
#include "cmd_sniffer.h"
#include "cmd_pcap.h"

#if CONFIG_SNIFFER_STORE_HISTORY
#define HISTORY_MOUNT_POINT "/data"
#define HISTORY_FILE_PATH HISTORY_MOUNT_POINT "/history.txt"
#endif

#if CONFIG_SNIFFER_SD_SPI_MODE

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2
#define PIN_NUM_MISO 2
#define PIN_NUM_MOSI 15
#define PIN_NUM_CLK  14
#define PIN_NUM_CS   13

#elif CONFIG_IDF_TARGET_ESP32C3
#define PIN_NUM_MISO 18
#define PIN_NUM_MOSI 9
#define PIN_NUM_CLK  8
#define PIN_NUM_CS   19
#endif //CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2

#endif  // CONFIG_SNIFFER_SD_SPI_MODE

static const char *TAG = "example";

#if CONFIG_SNIFFER_STORE_HISTORY
/* Initialize filesystem for command history store */
static void initialize_filesystem(void)
{
    static wl_handle_t wl_handle;
    const esp_vfs_fat_mount_config_t mount_config = {
        .max_files = 4,
        .format_if_mount_failed = true
    };
    esp_err_t err = esp_vfs_fat_spiflash_mount_rw_wl(HISTORY_MOUNT_POINT, "storage", &mount_config, &wl_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(err));
        return;
    }
}
#endif

static void initialize_nvs(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}

/* Initialize wifi with tcp/ip adapter */
static void initialize_wifi(void)
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));
}

/** Event handler for Ethernet events */
static void eth_event_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data)
{
    uint8_t mac_addr[6] = {0};
    /* we can get the ethernet driver handle from event data */
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;

    switch (event_id) {
    case ETHERNET_EVENT_CONNECTED:
        esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
        printf("\n");
        ESP_LOGI(TAG, "Ethernet Link Up");
        ESP_LOGI(TAG, "Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x",
                 mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
        break;
    case ETHERNET_EVENT_DISCONNECTED:
        printf("\n");
        ESP_LOGI(TAG, "Ethernet Link Down");
        break;
    case ETHERNET_EVENT_START:
        printf("\n");
        ESP_LOGI(TAG, "Ethernet Started");
        break;
    case ETHERNET_EVENT_STOP:
        printf("\n");
        ESP_LOGI(TAG, "Ethernet Stopped");
        break;
    default:
        break;
    }
}

static void initialize_eth(void)
{
    // Initialize Ethernet driver
    uint8_t eth_port_cnt = 0;
    esp_eth_handle_t *eth_handles;
    ESP_ERROR_CHECK(example_eth_init(&eth_handles, &eth_port_cnt));

    if (eth_port_cnt > 0) {
        // Register user defined event handers
        ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL));

        for (uint32_t i = 0; i < eth_port_cnt; i++) {
            /* start Ethernet driver state machine */
            ESP_ERROR_CHECK(esp_eth_start(eth_handles[i]));
            /* Register Ethernet interface to could be used by sniffer */
            ESP_ERROR_CHECK(sniffer_reg_eth_intf(eth_handles[i]));
        }
    }
}

#if CONFIG_SNIFFER_PCAP_DESTINATION_SD
static struct {
    struct arg_str *device;
    struct arg_end *end;
} mount_args;

/** 'mount' command */
static int mount(int argc, char **argv)
{
    esp_err_t ret;

    int nerrors = arg_parse(argc, argv, (void **)&mount_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, mount_args.end, argv[0]);
        return 1;
    }
    /* mount sd card */
    if (!strncmp(mount_args.device->sval[0], "sd", 2)) {
        ESP_LOGI(TAG, "Initializing SD card");
        esp_vfs_fat_sdmmc_mount_config_t mount_config = {
            .format_if_mount_failed = true,
            .max_files = 4,
            .allocation_unit_size = 16 * 1024
        };

        // initialize SD card and mount FAT filesystem.
        sdmmc_card_t *card;

#if CONFIG_SNIFFER_SD_SPI_MODE
        ESP_LOGI(TAG, "Using SPI peripheral");
        sdmmc_host_t host = SDSPI_HOST_DEFAULT();
        spi_bus_config_t bus_cfg = {
            .mosi_io_num = PIN_NUM_MOSI,
            .miso_io_num = PIN_NUM_MISO,
            .sclk_io_num = PIN_NUM_CLK,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = 4000,
        };
        ret = spi_bus_initialize(host.slot, &bus_cfg, SPI_DMA_CH_AUTO);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to initialize bus.");
            return 1;
        }

        // This initializes the slot without card detect (CD) and write protect (WP) signals.
        // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
        sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
        slot_config.gpio_cs = PIN_NUM_CS;
        slot_config.host_id = host.slot;

        ret = esp_vfs_fat_sdspi_mount(CONFIG_SNIFFER_MOUNT_POINT, &host, &slot_config, &mount_config, &card);

#else
        ESP_LOGI(TAG, "Using SDMMC peripheral");
        sdmmc_host_t host = SDMMC_HOST_DEFAULT();
        sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

        gpio_set_pull_mode(15, GPIO_PULLUP_ONLY); // CMD, needed in 4- and 1-line modes
        gpio_set_pull_mode(2, GPIO_PULLUP_ONLY);  // D0, needed in 4- and 1-line modes
        gpio_set_pull_mode(4, GPIO_PULLUP_ONLY);  // D1, needed in 4-line mode only
        gpio_set_pull_mode(12, GPIO_PULLUP_ONLY); // D2, needed in 4-line mode only
        gpio_set_pull_mode(13, GPIO_PULLUP_ONLY); // D3, needed in 4- and 1-line modes

        ret = esp_vfs_fat_sdmmc_mount(CONFIG_SNIFFER_MOUNT_POINT, &host, &slot_config, &mount_config, &card);
#endif

        if (ret != ESP_OK) {
            if (ret == ESP_FAIL) {
                ESP_LOGE(TAG, "Failed to mount filesystem. "
                         "If you want the card to be formatted, set format_if_mount_failed = true.");
            } else {
                ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                         "Make sure SD card lines have pull-up resistors in place.",
                         esp_err_to_name(ret));
            }
            return 1;
        }
        /* print card info if mount successfully */
        sdmmc_card_print_info(stdout, card);
    }
    return 0;
}

static void register_mount(void)
{
    mount_args.device = arg_str1(NULL, NULL, "<sd>", "choose a proper device to mount/unmount");
    mount_args.end = arg_end(1);
    const esp_console_cmd_t cmd = {
        .command = "mount",
        .help = "mount the filesystem",
        .hint = NULL,
        .func = &mount,
        .argtable = &mount_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static int unmount(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&mount_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, mount_args.end, argv[0]);
        return 1;
    }
    /* unmount sd card */
    if (!strncmp(mount_args.device->sval[0], "sd", 2)) {
        if (esp_vfs_fat_sdmmc_unmount() != ESP_OK) {
            ESP_LOGE(TAG, "Card unmount failed");
            return -1;
        }
        ESP_LOGI(TAG, "Card unmounted");
    }
    return 0;
}

static void register_unmount(void)
{
    mount_args.device = arg_str1(NULL, NULL, "<sd>", "choose a proper device to mount/unmount");
    mount_args.end = arg_end(1);
    const esp_console_cmd_t cmd = {
        .command = "unmount",
        .help = "unmount the filesystem",
        .hint = NULL,
        .func = &unmount,
        .argtable = &mount_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}
#endif // CONFIG_SNIFFER_PCAP_DESTINATION_SD

void app_main(void)
{
    initialize_nvs();

    /*--- Initialize Network ---*/
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    /* Initialize WiFi */
    initialize_wifi();
    /* Initialize Ethernet */
    initialize_eth();

    /*--- Initialize Console ---*/
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
#if CONFIG_SNIFFER_STORE_HISTORY
    initialize_filesystem();
    repl_config.history_save_path = HISTORY_FILE_PATH;
#endif
    repl_config.prompt = "sniffer>";

    // install console REPL environment
#if CONFIG_ESP_CONSOLE_UART
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &repl));
#elif CONFIG_ESP_CONSOLE_USB_CDC
    esp_console_dev_usb_cdc_config_t cdc_config = ESP_CONSOLE_DEV_CDC_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_cdc(&cdc_config, &repl_config, &repl));
#elif CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
    esp_console_dev_usb_serial_jtag_config_t usbjtag_config = ESP_CONSOLE_DEV_USB_SERIAL_JTAG_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_serial_jtag(&usbjtag_config, &repl_config, &repl));
#endif

    /* Register commands */
#if CONFIG_SNIFFER_PCAP_DESTINATION_SD
    register_mount();
    register_unmount();
#endif
    register_sniffer_cmd();
    register_pcap_cmd();
#if CONFIG_SNIFFER_PCAP_DESTINATION_SD
    printf("\n =======================================================\n");
    printf(" |         Steps to sniff network packets              |\n");
    printf(" |                                                     |\n");
    printf(" |  1. Enter 'help' to check all commands usage        |\n");
    printf(" |  2. Enter 'mount <device>' to mount filesystem      |\n");
    printf(" |  3. Enter 'pcap' to create pcap file                |\n");
    printf(" |  4. Enter 'sniffer' to start capture packets        |\n");
    printf(" |  5. Enter 'unmount <device>' to unmount filesystem  |\n");
    printf(" |                                                     |\n");
    printf(" =======================================================\n\n");
#else
    printf("\n =======================================================\n");
    printf(" |         Steps to sniff network packets              |\n");
    printf(" |                                                     |\n");
    printf(" |  1. Enter 'help' to check all commands' usage       |\n");
    printf(" |  2. Enter 'pcap' to create pcap file                |\n");
    printf(" |  3. Enter 'sniffer' to start capture packets        |\n");
    printf(" |                                                     |\n");
    printf(" =======================================================\n\n");
#endif

    // start console REPL
    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}
