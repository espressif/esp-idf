/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "esp_check.h"
#include "esp_log.h"
#include "onewire_bus.h"

static const char *TAG = "onewire";

struct onewire_rom_search_context_t {
    onewire_bus_handle_t bus_handle;

    uint8_t last_device_flag;
    uint16_t last_discrepancy;
    uint8_t rom_number[8];
};

// Algorithm inspired by https://www.maximintegrated.com/en/design/technical-documents/app-notes/1/187.html

static const uint8_t dscrc_table[] = {
    0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
  157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
   35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98,
  190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
   70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7,
  219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
  101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
  248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
  140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
   17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
  175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
   50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
  202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
   87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
  233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
  116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53
};

uint8_t onewire_check_crc8(uint8_t *input, size_t input_size)
{
    uint8_t crc8 = 0;

    for (size_t i = 0; i < input_size; i ++) {
        crc8 = dscrc_table[crc8 ^ input[i]];
    }

    return crc8;
}

esp_err_t onewire_rom_search_context_create(onewire_bus_handle_t handle, onewire_rom_search_context_handler_t *context_out)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "invalid 1-wire handle");
    ESP_RETURN_ON_FALSE(context_out, ESP_ERR_INVALID_ARG, TAG, "invalid context handler pointer");

    struct onewire_rom_search_context_t *context = calloc(1, sizeof(struct onewire_rom_search_context_t));
    if (!context) {
        return ESP_ERR_NO_MEM;
    }

    context->bus_handle = handle;
    *context_out = context;

    return ESP_OK;
}

esp_err_t onewire_rom_search_context_delete(onewire_rom_search_context_handler_t context)
{
    ESP_RETURN_ON_FALSE(context, ESP_ERR_INVALID_ARG, TAG, "invalid context handler pointer");

    free(context);

    return ESP_OK;
}

esp_err_t onewire_rom_search(onewire_rom_search_context_handler_t context)
{
    ESP_RETURN_ON_FALSE(context, ESP_ERR_INVALID_ARG, TAG, "invalid context handler pointer");

    uint8_t last_zero = 0;

    if (!context->last_device_flag) {
        if (onewire_bus_reset(context->bus_handle) != ESP_OK) { // no device present
            return ESP_ERR_NOT_FOUND;
        }

        // send rom search command and start search algorithm
        ESP_RETURN_ON_ERROR(onewire_bus_write_bytes(context->bus_handle, (uint8_t[]){ONEWIRE_CMD_SEARCH_ROM}, 1),
                            TAG, "error while sending search rom command");

        for (uint16_t rom_bit_index = 0; rom_bit_index < 64; rom_bit_index ++) {
            uint8_t rom_byte_index = rom_bit_index / 8;
            uint8_t rom_bit_mask = 1 << (rom_bit_index % 8); // calculate byte index and bit mask in advance for convenience

            uint8_t rom_bit, rom_bit_complement;
            ESP_RETURN_ON_ERROR(onewire_bus_read_bit(context->bus_handle, &rom_bit), TAG, "error while reading rom bit"); // write 1 bit to read from the bus
            ESP_RETURN_ON_ERROR(onewire_bus_read_bit(context->bus_handle, &rom_bit_complement),
                                TAG, "error while reading rom bit"); // read a bit and its complement

            uint8_t search_direction;
            if (rom_bit && rom_bit_complement) { // No devices participating in search.
                ESP_LOGE(TAG, "no devices participating in search");
                return ESP_ERR_NOT_FOUND;
            } else {
                if (rom_bit != rom_bit_complement) { // There are only 0s or 1s in the bit of the participating ROM numbers.
                    search_direction = rom_bit;  // just go ahead
                } else { // There are both 0s and 1s in the current bit position of the participating ROM numbers. This is a discrepancy.
                    if (rom_bit_index < context->last_discrepancy) { // current id bit is before the last discrepancy bit
                        search_direction = (context->rom_number[rom_byte_index] & rom_bit_mask) ? 0x01 : 0x00; // follow previous way
                    } else {
                        search_direction = (rom_bit_index == context->last_discrepancy) ? 0x01 : 0x00; // search for 0 bit first
                    }

                    if (search_direction == 0) { // record zero's position in last zero
                        last_zero = rom_bit_index;
                    }
                }

                if (search_direction == 1) { // set corrsponding rom bit by serach direction
                    context->rom_number[rom_byte_index] |= rom_bit_mask;
                } else {
                    context->rom_number[rom_byte_index] &= ~rom_bit_mask;
                }

                ESP_RETURN_ON_ERROR(onewire_bus_write_bit(context->bus_handle, search_direction),
                                    TAG, "error while writing direction bit"); // set search direction
            }
        }
    } else {
        ESP_LOGD(TAG, "1-wire rom search finished");
        return ESP_FAIL;
    }

    // if the search was successful
    context->last_discrepancy = last_zero;
    if (context->last_discrepancy == 0) { // last zero loops back to the first bit
        context->last_device_flag = true;
    }

    if (onewire_check_crc8(context->rom_number, 7) != context->rom_number[7]) { // check crc
        ESP_LOGE(TAG, "bad crc checksum of device with id " ONEWIRE_ROM_ID_STR, ONEWIRE_ROM_ID(context->rom_number));
        return ESP_ERR_INVALID_CRC;
    }

    return ESP_OK;
}

esp_err_t onewire_rom_get_number(onewire_rom_search_context_handler_t context, uint8_t *rom_number_out)
{
    ESP_RETURN_ON_FALSE(context, ESP_ERR_INVALID_ARG, TAG, "invalid context pointer");
    ESP_RETURN_ON_FALSE(rom_number_out, ESP_ERR_INVALID_ARG, TAG, "invalid rom_number pointer");

    memcpy(rom_number_out, context->rom_number, sizeof(context->rom_number));

    return ESP_OK;
}
