/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdbool.h>
#include <sys/reent.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_log_color.h"
#include "bootloader_init.h"
#include "bootloader_utility.h"
#include "bootloader_common.h"
#include "bootloader_flash_priv.h"
#include "esp_app_desc.h"

#define MAX_PARTITIONS  8
#define LABEL_LENGTH    32

/**
 * @brief Helper macro to test the equality of two partition positions
 */
#define PART_POS_EQUAL(p0, p1)      (((p0).offset == (p1).offset) && ((p0).size == (p1).size))

static const char* TAG = "boot";

/**
 * @brief Structure describing a partition, with its name label and its position, used as an identifier here.
 */
typedef struct {
    const esp_partition_pos_t* pos;
    int number;
    char label[LABEL_LENGTH];
} bootloader_part_info;


/**
 * @brief Enumeration of the possible input events
 */
typedef enum {
    INPUT_UP,
    INPUT_DOWN,
    INPUT_VALIDATE,
} bootloader_input_t;


extern char uart_rx_one_char_block(void);
extern esp_err_t bootloader_common_get_partition_description(const esp_partition_pos_t *partition, esp_app_desc_t *app_desc);

static int bootloader_utility_list_partitions(const bootloader_state_t* st, bootloader_part_info *bp, int length);
static bootloader_input_t bootloader_get_next_event(void);

/*
 * We arrive here after the ROM bootloader finished loading this second stage bootloader from flash.
 * The hardware is mostly uninitialized, flash cache is down and the app CPU is in reset.
 * We do have a stack, so we can do the initialization in C.
 */
void __attribute__((noreturn)) call_start_cpu0(void)
{
    bootloader_state_t bs = {0};
    bootloader_part_info partitions[MAX_PARTITIONS];

    // 1. Hardware initialization
    if (bootloader_init() != ESP_OK) {
        bootloader_reset();
    }

#ifdef CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP
    // If this boot is a wake up from the deep sleep then go to the short way,
    // try to load the application which worked before deep sleep.
    // It skips a lot of checks due to it was done before (while first boot).
    bootloader_utility_load_boot_image_from_deep_sleep();
    // If it is not successful try to load an application as usual.
#endif

    // 2. Load partition table
    if (!bootloader_utility_load_partition_table(&bs)) {
        ESP_LOGE(TAG, "load partition table error!");
        bootloader_reset();
    }

    // 3. Show the available partitions menu
    int count = bootloader_utility_list_partitions(&bs, partitions, MAX_PARTITIONS);
    if (count == 0) {
        esp_rom_printf("No boot partition available, rebooting...\n");
        esp_rom_delay_us(1000000);
        bootloader_reset();
    }

    // 4. Show all the partitions on screen and wait for the user to select one

    /* Keep track of the select entry */
    int select = 0;
    int validate = 0;
    while (validate == 0) {
        /* Clear the screen and print the partition boot selector */
        esp_rom_printf(LOG_ANSI_CLEAR_SCREEN LOG_ANSI_SET_CURSOR_HOME
                       "\nPlease select a partition to boot.\n\n"
                       "FOUND %d BOOT PARTITIONS:\n", count);

        for (int i = 0; i < count; i++) {
            if (i == select) {
                esp_rom_printf("    " LOG_ANSI_COLOR_REVERSE " %s " LOG_ANSI_COLOR_RESET "\n", partitions[i].label);
            } else {
                esp_rom_printf("     %s \n", partitions[i].label);
            }
        }

        /* Get the next input from the user */
        switch(bootloader_get_next_event()) {
            case INPUT_UP:
                /* Select the option above, roll it back to the bottom of the menu if negative */
                select = (select - 1 + count) % count;
                break;
            case INPUT_DOWN:
                select = (select + 1) % count;
                break;
            case INPUT_VALIDATE:
                /* The partition to boot has been selected! */
                validate = 1;
                break;
        }
    }
    esp_rom_printf("BOOTING PARTITION %s\n", partitions[select].label);

    // 5. Load the app image for booting
    bootloader_utility_load_boot_image(&bs, partitions[select].number);
}


static esp_err_t bootloader_utility_fill_info(const esp_partition_pos_t* partition, bootloader_part_info* info)
{
    esp_image_metadata_t image_data = {0};
    esp_app_desc_t app_desc;
    esp_err_t ret;

    ret = esp_image_verify(ESP_IMAGE_VERIFY_SILENT, partition, &image_data);

    if (ret == ESP_OK) {
        ret = bootloader_common_get_partition_description(partition, &app_desc);
    }

    if (ret == ESP_OK) {
        info->pos = partition;
        /* Make sure to always have a NULL-byte */
        strncpy(info->label, app_desc.project_name, LABEL_LENGTH - 1);
        info->label[LABEL_LENGTH - 1] = 0;
    }

    return ret;
}


/**
 * @brief Get bootable partitions information.
 * We could override the function `bootloader_utility_load_partition_table` to return these information at the same time
 * when loading and populating `bootloader_state_t` structure. But let's keep it simple and avoid copy-paste.
 */
static int bootloader_utility_list_partitions(const bootloader_state_t* st, bootloader_part_info *bp, int length)
{
    int index = 0;

    // Add factory partition if available and if `bp` is big enough
    if (st->factory.offset && index < length) {
        if (bootloader_utility_fill_info(&st->factory, bp + index) == ESP_OK) {
            bp[index++].number = FACTORY_INDEX;
        } else {
            esp_rom_printf("Error getting the description for the factory partition\n");
        }
    }

    // Add test partition if available
    if (st->test.offset && index < length) {
        if (bootloader_utility_fill_info(&st->test, bp + index) == ESP_OK) {
            bp[index++].number = TEST_APP_INDEX;
        } else {
            esp_rom_printf("Error getting the description for the test partition\n");
        }
    }

    // Add OTA partitions
    for (int i = 0; i < st->app_count && index < length; i++) {
        if (bootloader_utility_fill_info(&st->ota[i], bp + index) == ESP_OK) {
            bp[index++].number = i;
        } else {
            esp_rom_printf("Error getting the description for the ota_%d partition\n", i);
        }
    }

    return index;
}


/**
 * @brief Get an input from the user, this is currently implemented with the UART interface but we can
 * imagine implementing it with a GPIO or any other mean.
 */
static bootloader_input_t bootloader_get_next_event(void)
{
    /* Up arrow, down arrow and Enter will be used for up, down and validate events respectively */
    for (;;) {
        char input = uart_rx_one_char_block();

        /* Check if the received character is an escaped character */
        if (input == 0x1B) {
            input = uart_rx_one_char_block();

            /* Likely to be an arrow key */
            if (input == '[') {
                input = uart_rx_one_char_block();
                switch (input) {
                    case 'A':
                        return INPUT_UP;
                    case 'B':
                        return INPUT_DOWN;
                    default:
                        /* Ignore unknown sequences */
                        break;
                }
            }
        } else if (input == '\n' || input == '\r') {
            return INPUT_VALIDATE;
        }
    }
}


#if CONFIG_LIBC_NEWLIB
// Return global reent struct if any newlib functions are linked to bootloader
struct _reent *__getreent(void)
{
    return _GLOBAL_REENT;
}
#endif
