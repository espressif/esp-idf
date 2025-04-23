/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/param.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "esp_log_config.h"
#include "esp_log_args.h"
#include "esp_private/log_lock.h"
#include "esp_private/log_message.h"
#include "esp_private/log_print.h"
#include "esp_private/log_util.h"
#include "soc/soc.h"
#include "esp_rom_uart.h"

#if BOOTLOADER_BUILD
#define APP_TYPE 0x01
#else
#define APP_TYPE 0x02
#endif

#define IS_LOCATED_IN_NOLOAD_SECTION(addr) (((addr) & 0xFF000000) == 0x00000000)

#if CONFIG_IDF_TARGET_LINUX
#define PRESENT_IN_ELF(addr) (false)
#else // !CONFIG_IDF_TARGET_LINUX
#if BOOTLOADER_BUILD
#define PRESENT_IN_ELF(addr) ( \
    IS_LOCATED_IN_NOLOAD_SECTION(addr) \
)
#else // APP
#define PRESENT_IN_ELF(addr) ( \
    IS_LOCATED_IN_NOLOAD_SECTION(addr) || \
    (((addr) >= SOC_DROM_LOW) && ((addr) < SOC_DROM_HIGH)) || \
    (((addr) >= SOC_IROM_LOW) && ((addr) < SOC_IROM_HIGH)) \
)
#endif // APP
#endif // !CONFIG_IDF_TARGET_LINUX

/**
 * @brief Packed control structure for binary log formatting.
 */
typedef struct {
    union {
        struct {
            uint16_t pkg_len: 10;                   /**< Package length (10 bits). */
            uint16_t log_level: ESP_LOG_LEVEL_LEN;  /**< Log level. */
            uint16_t time_64bits: 1;                /**< Timestamp is 64 bits if 1. */
            uint16_t version: 2;                    /**< Control structure version. */
        } opts;
        uint16_t data;                              /**< Raw 16-bit representation. */
    };
    uint8_t app_identifier;                         /**< Identifier byte for application type (see APP_TYPE). Used by the host to select the appropriate ELF file for decoding. */
} __attribute__((packed)) control_t;

_Static_assert(sizeof(control_t) == 3, "control_t must be exactly 24 bits (3 bytes)");

typedef struct {
    uint8_t crc;
    bool buffer_hex_log;
    bool buffer_char_log;
    bool buffer_hexdump_log;
    int buffer_len;
    bool len_calculation_stage;
} pkg_info_t;

extern const char __ESP_BUFFER_HEX_FORMAT__[];
extern const char __ESP_BUFFER_CHAR_FORMAT__[];
extern const char __ESP_BUFFER_HEXDUMP_FORMAT__[];

void update_crc8(uint8_t data, pkg_info_t *pkg_info)
{
    uint8_t crc = pkg_info->crc;
    crc ^= data;
    for (uint8_t i = 0; i < 8; i++) {
        if (crc & 0x80) {
            crc = (crc << 1) ^ 0x07;
        } else {
            crc <<= 1;
        }
    }
    pkg_info->crc = crc;
}

/**
 * @brief Outputs data in big-endian order and updates CRC8 checksum.
 *
 * This function processes the given data in big-endian order, ensuring that
 * the data is read and transmitted in a forward order. It is particularly
 * useful for distinguishing between embedded data and address information
 * while parsing the packet. The first byte is used to differentiate between
 * data and address:
 *   - 0xFF20ssssssssss: Indicates a negative length (0x20) followed by a string.
 *   - 0x3Fxxxxxx: Represents an address.
 *
 * Using big-endian order avoids the need to read strings (data) in a reversed
 * or "wired" order (0xss20FFssssssss), which would occur if little-endian order were used.
 *
 * @param src Pointer to the source data to be output.
 * @param length Length of the data to be processed.
 * @param pkg_info Pointer to the package information structure, which includes
 *                 the current stage of length calculation and CRC8 checksum data.
 * @return The length of the processed data.
 */
static unsigned output(const void *src, unsigned length, pkg_info_t *pkg_info)
{
    for (unsigned i = 0; i < length; i++) {
        uint8_t data = ((uint8_t *)src)[length - 1 - i];
        if (pkg_info->len_calculation_stage == false) {
            esp_rom_output_tx_one_char(data);
            update_crc8(data, pkg_info);
        }
    }
    return length;
}

static unsigned output_pointer(const char *ptr, pkg_info_t *pkg_info)
{
    unsigned pkg_len = 0;
    uintptr_t addr = (uintptr_t)ptr;
    if (PRESENT_IN_ELF(addr) || addr == 0) {
        pkg_len = output(&addr, sizeof(addr), pkg_info);
    } else {
        int len = (pkg_info->buffer_len) ? pkg_info->buffer_len : strlen(ptr);
        int16_t pkg_str_len = 1 - len;
        pkg_len = output(&pkg_str_len, sizeof(pkg_str_len), pkg_info);
        for (unsigned i = 0; i < MAX(len, 2); i++) {
            pkg_len += output(&ptr[i], sizeof(uint8_t), pkg_info);
        }
    }
    return pkg_len;
}

static esp_log_args_type_t get_arg_type_from_format_string(const char **format_ptr)
{
    if (!format_ptr || !(*format_ptr)) {
        return ESP_LOG_ARGS_TYPE_NONE;
    }

    const char *format = *format_ptr;
    while (*format) {
        if (*format++ == '%') {
            if (*format == '%') { // Skip "%%"
                format++;
                continue;
            }

            // Handle optional flags, width, and precision
            while (*format == '-' || *format == '.' || ((*format) >= '0' && (*format) <= '9')) {
                format++;
            }

            // Handle length modifiers
            int is_long_long = 0;
            while (*format == 'l') {
                is_long_long++;
                format++;
            }
            while (*format == 'h' || *format == 'z') {
                format++;
            }

            switch (*format++) {
            case 'f': case 'F': case 'e': case 'E': case 'g': case 'G':
                *format_ptr = format;
                return ESP_LOG_ARGS_TYPE_64BITS;
            case 'c': case 'p': case 'd': case 'i': case 'u': case 'x': case 'X': case 'o':
                *format_ptr = format;
                return is_long_long >= 2 ? ESP_LOG_ARGS_TYPE_64BITS : ESP_LOG_ARGS_TYPE_32BITS;
            case 's': case 'S':
                *format_ptr = format;
                return ESP_LOG_ARGS_TYPE_POINTER;
            default:
                *format_ptr = format;
                return ESP_LOG_ARGS_TYPE_32BITS;
            }
        }
    }
    *format_ptr = format;
    return ESP_LOG_ARGS_TYPE_NONE;
}

static unsigned output_arguments(esp_log_msg_t *message, va_list args, pkg_info_t *pkg_info)
{
    unsigned pkg_len = 0;
    unsigned idx_arg = 0;
    const char *format = message->format;
    while (1) {
        esp_log_args_type_t arg_type;
        if (!message->config.opts.binary_mode) {
            assert(!IS_LOCATED_IN_NOLOAD_SECTION((uintptr_t)format) && "Misconfiguration: format must be on flash");
            arg_type = get_arg_type_from_format_string(&format);
        } else {
            arg_type = (message->arg_types[idx_arg / 4] >> ((idx_arg % 4) * ESP_LOG_ARGS_TYPE_LEN)) & 0x03;
            idx_arg++;
        }
        switch (arg_type) {
        case ESP_LOG_ARGS_TYPE_32BITS: {
            uint32_t val = va_arg(args, uint32_t);
            pkg_len += output(&val, sizeof(val), pkg_info);
            if (pkg_info->buffer_hex_log || pkg_info->buffer_char_log || pkg_info->buffer_hexdump_log) {
                pkg_info->buffer_len = val;
            }
            break;
        }
        case ESP_LOG_ARGS_TYPE_64BITS: {
            uint64_t val = va_arg(args, uint64_t);
            pkg_len += output(&val, sizeof(val), pkg_info);
            break;
        }
        case ESP_LOG_ARGS_TYPE_POINTER: {
            const char *addr = va_arg(args, const char *);
            pkg_len += output_pointer(addr, pkg_info);
            break;
        }
        case ESP_LOG_ARGS_TYPE_NONE:
        default:
            return pkg_len;
        }
    }

    return pkg_len;
}

static unsigned calc_pkg_len(esp_log_msg_t *message, pkg_info_t *pkg_info)
{
    unsigned pkg_len = 0;
    pkg_info->len_calculation_stage = true;
    pkg_len += sizeof(control_t);
    pkg_len += output_pointer(message->format, pkg_info);
    pkg_len += output_pointer(message->tag, pkg_info);
    pkg_len += (message->timestamp >> 32) != 0 ? sizeof(uint64_t) : sizeof(uint32_t);
    va_list args;
    va_copy(args, message->args);
    pkg_len += output_arguments(message, args, pkg_info);
    va_end(args);
    pkg_len += sizeof(uint8_t); // crc8
    pkg_info->len_calculation_stage = false;
    return pkg_len;
}

/*
[0] - Type of message: 1 - bootloader, 2 - application, ...
[1] - Control byte: log level, version.
[2] - Length 10 bits, so max is 1023 bytes.
[3 - 6] - Format addr if present in ELF or embedded string
[7 - 10] - Tag addr if present in ELF or embedded string
[11 - 14] - Timestamp (if 32 bits)
[15] - crc8

Embedded string:
[0, 1] bytes - (10 bits) negative length of the string = 1 - len(str). 0xFFFC = len is 5, 0xFDD6 = len is 555. Max is 1023.
next bytes - string
*/
void esp_log_format_binary(esp_log_msg_t *message)
{
    assert(message != NULL);

    if (!message->config.opts.constrained_env) {
        esp_log_impl_lock();
    }

    pkg_info_t pkg_info = {
        .crc = 0,
        .buffer_hex_log = message->format == __ESP_BUFFER_HEX_FORMAT__,
        .buffer_char_log = message->format == __ESP_BUFFER_CHAR_FORMAT__,
        .buffer_hexdump_log = message->format == __ESP_BUFFER_HEXDUMP_FORMAT__,
        .buffer_len = 0,
        .len_calculation_stage = false,
    };

    // Output control byte
    control_t control = {
        .opts = {
            .pkg_len = calc_pkg_len(message, &pkg_info),
            .log_level = message->config.opts.log_level,
            .time_64bits = (message->timestamp >> 32) != 0,
            .version = 0,
        },
        .app_identifier = APP_TYPE,
    };
    output(&control, sizeof(control), &pkg_info);
    output_pointer(message->format, &pkg_info);
    output_pointer(message->tag, &pkg_info);
    output(&message->timestamp, (control.opts.time_64bits) ? sizeof(uint64_t) : sizeof(uint32_t), &pkg_info);
    output_arguments(message, message->args, &pkg_info);
    output(&pkg_info.crc, sizeof(pkg_info.crc), &pkg_info);

    if (!message->config.opts.constrained_env) {
        esp_log_impl_unlock();
    }
}
