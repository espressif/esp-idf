/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "esp_twai.h"
#include "cmd_twai_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* TWAI frame constants */
#define TWAI_STD_ID_CHAR_LEN      3
#define TWAI_EXT_ID_CHAR_LEN      8

/** @brief Parser return codes */
#define PARSE_OK            0
#define PARSE_ERROR         -1
#define PARSE_INVALID_ARG   -2
#define PARSE_OUT_OF_RANGE  -3
#define PARSE_TOO_LONG      -4
#define PARSE_NOT_FOUND     -5

/* Additional constants */
#define TWAI_RTR_DEFAULT_DLC        0
#define TWAI_FD_FLAGS_MAX_VALUE     15
#define TWAI_FD_BRS_FLAG_MASK       0x01
#define TWAI_FD_ESI_FLAG_MASK       0x02

/**
 * @brief Parse TWAI ID from string
 *
 * @param[in] str  Pointer to the start of the ID string
 * @param[in] len  Length of the ID string
 * @param[out] f   Pointer to frame structure to fill
 *
 * @return PARSE_OK on success;
 *         PARSE_INVALID_ARG if pointers are NULL;
 *         PARSE_ERROR or PARSE_OUT_OF_RANGE on format or range error
 */
int parse_twai_id(const char *str, size_t len, twai_frame_t *f);

/**
 * @brief Parse TWAI-FD frames with flags and extended payload
 *
 * Body format: <flags>{data}
 *   flags: single hex nibble (0..F)
 *   data: up to 64 bytes hex pairs
 *
 * @param[in] body  Pointing to the substring after '#'
 * @param[out] f     Pointer to frame structure to fill
 *
 * @return PARSE_OK on success;
 *         PARSE_INVALID_ARG if arguments are NULL;
 *         PARSE_ERROR or PARSE_OUT_OF_RANGE on format or range error
 */
int parse_twaifd_frame(const char *str, twai_frame_t *f);

/**
 * @brief Parse Classical TWAI data and RTR frames
 *
 * Supports:
 *   <twai_id>#{data}          Data frame with up to 8 bytes
 *   <twai_id>#R{len}          RTR frame with specified length
 *   <twai_id>#{data}_{dlc}    Data frame with extended DLC (9..F)
 *
 * @param[in]  body  Pointing to the substring after '#'
 * @param[out] f     Pointer to frame structure to fill
 *
 * @return PARSE_OK on success;
 *         PARSE_INVALID_ARG if arguments are NULL;
 *         PARSE_ERROR or PARSE_OUT_OF_RANGE on format or range error
 */
int parse_classic_frame(const char *str, twai_frame_t *f);

/**
 * @brief Parse controller string and return controller ID
 *
 * @param[in] controller_str  Controller string (e.g., "twai0")
 *
 * @return Controller ID (0-9) on success, PARSE_ERROR on failure
 */
int parse_controller_string(const char *controller_str);

/**
 * @brief Convert TWAI state to string
 *
 * @param[in] state  TWAI error state
 *
 * @return Pointer to the string representation of the state
 */
const char *twai_state_to_string(twai_error_state_t state);

/**
 * @brief Format GPIO pin display
 *
 * @param[in] gpio_pin  GPIO pin number
 * @param[out] buffer   Buffer to store the formatted string
 * @param[in] buffer_size  Size of the buffer
 *
 * @return Number of characters written to buffer
 */
int format_gpio_pin(int gpio_pin, char *buffer, size_t buffer_size);

/**
 * @brief Parse hex string with specified length (no null terminator required)
 *
 * @param[in] str Input string pointer
 * @param[in] len Length of hex string to parse
 * @param[out] out Output value pointer
 *
 * @return PARSE_OK on success, PARSE_ERROR on format error
 */
int parse_hex_segment(const char *str, size_t len, uint32_t *out);

/**
 * @brief Parse a "lhs <sep> rhs" token where both sides are hex strings.
 *
 * The function splits by @p sep, parses both halves as hex (no null terminators required),
 * and returns their values and lengths.
 *
 * @param[in]  tok        Pointer to token start
 * @param[in]  tok_len    Token length in bytes
 * @param[in]  sep        Separator character (':' for mask, '-' for range)
 * @param[out] lhs        Parsed left-hand value
 * @param[out] lhs_chars  Characters consumed by left-hand substring
 * @param[out] rhs        Parsed right-hand value
 * @param[out] rhs_chars  Characters consumed by right-hand substring
 *
 * @return PARSE_OK on success;
 *         PARSE_INVALID_ARG for bad args;
 *         PARSE_ERROR if separator missing or hex parse fails.
 */
int parse_pair_token(const char *tok, size_t tok_len, char sep,
                     uint32_t *lhs, size_t *lhs_chars,
                     uint32_t *rhs, size_t *rhs_chars);

/**
 * @brief Parse a single hex nibble character
 *
 * @param[in]  c    Input character (0-9, A-F, a-f)
 * @param[out] out  Output pointer to store the parsed nibble value (0-15)
 *
 * @return PARSE_OK on success;
 *         PARSE_INVALID_ARG if out pointer is NULL;
 *         PARSE_ERROR if character is not a valid hex digit
 */
static inline int parse_nibble(char c, uint8_t *out)
{
    if (!out) {
        return PARSE_INVALID_ARG;
    }

    if (c >= '0' && c <= '9') {
        *out = (uint8_t)(c - '0');
        return PARSE_OK;
    }
    if (c >= 'A' && c <= 'F') {
        *out = (uint8_t)(c - 'A' + 10);
        return PARSE_OK;
    }
    if (c >= 'a' && c <= 'f') {
        *out = (uint8_t)(c - 'a' + 10);
        return PARSE_OK;
    }

    return PARSE_ERROR;
}

/**
 * @brief Locate first '#' and count consecutives
 *
 * @param[in] input Input string
 * @param[out] sep Pointer to the separator
 * @param[out] hash_count Pointer to the hash count
 *
 * @return PARSE_OK if successful, PARSE_INVALID_ARG if input is NULL, PARSE_ERROR if no '#' is found
 */
static inline int locate_hash(const char *input, const char **sep, int *hash_count)
{
    if (!input || !sep || !hash_count) {
        return PARSE_INVALID_ARG;
    }
    const char *s = strchr(input, '#');
    if (!s) {
        return PARSE_ERROR;
    }
    *sep = s;
    *hash_count = 1;
    while (s[*hash_count] == '#') {
        (*hash_count)++;
    }
    return PARSE_OK;
}

/**
 * @brief Format timestamp string based on the specified mode
 *
 * @param[in] timestamp_mode Timestamp mode configuration
 * @param[in] frame_timestamp Frame timestamp in microseconds
 * @param[in] start_time_us Start time for zero-based timestamps
 * @param[in,out] last_frame_time_us Pointer to last frame time for delta mode (updated if delta mode)
 * @param[out] timestamp_str Buffer to store formatted timestamp string
 * @param[in] max_len Maximum length of timestamp string buffer
 */
void format_timestamp(timestamp_mode_t timestamp_mode, int64_t frame_timestamp,
                      int64_t start_time_us, int64_t *last_frame_time_us,
                      char *timestamp_str, size_t max_len);

/**
 * @brief Format TWAI frame in twai_dump format
 *
 * @param[in] timestamp_mode Timestamp mode configuration
 * @param[in] frame TWAI frame structure
 * @param[in] frame_timestamp Frame timestamp in microseconds
 * @param[in] start_time_us Start time for zero-based timestamps
 * @param[in,out] last_frame_time_us Pointer to last frame time for delta mode (updated if delta mode)
 * @param[in] controller_id Controller ID for interface name
 * @param[out] output_line Buffer to store formatted output line
 * @param[in] max_len Maximum length of output line buffer
 */
void format_twaidump_frame(timestamp_mode_t timestamp_mode, const twai_frame_t *frame,
                           int64_t frame_timestamp, int64_t start_time_us, int64_t *last_frame_time_us,
                           int controller_id, char *output_line, size_t max_len);

/**
 * @brief Parse the controller ID string and return the end of the controller substring
 *
 * This function parses a controller string in the format "twai0", "twai1", ..., "twaix"
 * and extracts the controller ID (0-x). It also supports controller strings with filters,
 * such as "twai0,123:7FF", and returns a pointer to the end of the substring(e.g. the ',' or '\0').
 *
 * @param[in] controller_str Input controller string (e.g., "twai0" or "twai0,123:7FF")
 * @param[out] controller_id Output pointer to store the parsed controller ID
 *
 * @return Pointer to the end of the controller substring (e.g., the ',' or '\0'), or NULL on error
 */
static inline const char *parse_controller_id(const char *controller_str, int *controller_id)
{
    if (!controller_str || !controller_id) {
        return NULL;
    }

    /* Support "twai0" ~ "twaix" format (which is dependent on SOC_TWAI_CONTROLLER_NUM) */
    if (strncmp(controller_str, "twai", 4) == 0 && strlen(controller_str) >= 5) {
        char id_char = controller_str[4];
        if (id_char >= '0' && id_char <= '9' && id_char < '0' + SOC_TWAI_CONTROLLER_NUM) {
            *controller_id = id_char - '0';
            /* Return pointer to character after the ID digit */
            return controller_str + 5;
        }
    }

    return NULL;
}

#ifdef __cplusplus
}
#endif
