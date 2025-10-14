/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "twai_utils_parser.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>

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
                      char *timestamp_str, size_t max_len)
{
    if (timestamp_mode == TIMESTAMP_MODE_NONE) {
        timestamp_str[0] = '\0';
        return;
    }

    int64_t timestamp_us;

    switch (timestamp_mode) {
    case TIMESTAMP_MODE_ABSOLUTE:
        timestamp_us = frame_timestamp;
        break;
    case TIMESTAMP_MODE_DELTA:
        timestamp_us = frame_timestamp - *last_frame_time_us;
        *last_frame_time_us = frame_timestamp;
        break;
    case TIMESTAMP_MODE_ZERO:
        timestamp_us = frame_timestamp - start_time_us;
        break;
    default:
        timestamp_str[0] = '\0';
        return;
    }

    /* Format output: (seconds.microseconds) */
    snprintf(timestamp_str, max_len, "(%lld.%06lld) ", timestamp_us / 1000000, timestamp_us % 1000000);
}

int parse_hex_segment(const char *str, size_t len, uint32_t *out)
{
    if (!str || len == 0 || len > TWAI_EXT_ID_CHAR_LEN || !out) {
        return PARSE_INVALID_ARG;
    }

    uint32_t result = 0;
    for (size_t i = 0; i < len; i++) {
        uint8_t nibble;
        if (parse_nibble(str[i], &nibble) != PARSE_OK) {
            return PARSE_ERROR;
        }
        result = (result << 4) | nibble;
    }

    *out = result;
    return PARSE_OK;
}

/**
 * @brief Parse payload bytes (hex pairs) up to max length, skipping '.' separators
 *
 * This function reads up to max bytes from the ASCII hex string s,
 * ignoring any '.' separators. Each pair of hex digits is converted
 * into one byte and stored into buf.
 *
 * @param[in]  s    Null-terminated input string containing hex digits and optional '.' separators
 * @param[out] buf  Buffer to store parsed byte values
 * @param[in]  max  Maximum number of bytes to parse (buffer capacity)
 *
 * @return On success, returns the number of bytes parsed (0..max).
 *         Returns PARSE_INVALID_ARG if input pointers are NULL or max <= 0.
 *         Returns PARSE_ERROR if a non-hex digit is encountered before parsing max bytes
 */
static inline int parse_payload(const char *s, uint8_t *buf, int max)
{
    if (!s || !buf || max <= 0) {
        return PARSE_INVALID_ARG;
    }
    int cnt = 0;
    while (*s && cnt < max) {
        if (*s == '.') {
            s++;
            continue;
        }
        /* Check if we have valid hex pair */
        if (!isxdigit((unsigned char)s[0])) {
            if (cnt == 0 && *s != '\0') {
                return PARSE_ERROR;
            }
            break;
        }
        if (!isxdigit((unsigned char)s[1])) {
            return PARSE_ERROR;
        }
        uint8_t high, low;
        if (parse_nibble(s[0], &high) != PARSE_OK || parse_nibble(s[1], &low) != PARSE_OK) {
            return PARSE_ERROR;
        }
        buf[cnt++] = (high << 4) | low;
        s += 2;
    }
    return cnt;
}

/**
 * @brief Parse hex ID substring of given length
 *
 * @param[in] str   Pointer to the start of the hex substring
 * @param[in] len   Number of characters in the hex substring (3 or 8)
 * @param[out] out  Pointer to the variable to receive the parsed ID value
 * @param[out] is_ext Pointer to store whether the ID is extended format
 *
 * @return PARSE_OK on success;
 *         PARSE_INVALID_ARG if pointers are NULL or len is out of range;
 *         PARSE_ERROR if any character is not a valid hex digit or length mismatch
 */
static inline int parse_hex_id(const char *str, size_t len, uint32_t *out, bool *is_ext)
{
    if (!str || !out || !is_ext || len == 0 || len > TWAI_EXT_ID_CHAR_LEN) {
        return PARSE_INVALID_ARG;
    }
    int ret = parse_hex_segment(str, len, out);
    if (ret != PARSE_OK) {
        return ret;
    }
    *is_ext = (len > TWAI_STD_ID_CHAR_LEN) || (*out > TWAI_STD_ID_MASK);
    if ((*is_ext && *out > TWAI_EXT_ID_MASK) || (!*is_ext && *out > TWAI_STD_ID_MASK)) {
        return PARSE_OUT_OF_RANGE;
    }
    return PARSE_OK;
}

int parse_twai_id(const char *str, size_t len, twai_frame_t *f)
{
    if (!str || !f) {
        return PARSE_INVALID_ARG;
    }
    bool is_ext = false;
    uint32_t id = 0;
    int res = parse_hex_id(str, len, &id, &is_ext);
    if (res != PARSE_OK) {
        return res;
    }
    f->header.id = id;
    f->header.ide = is_ext ? 1 : 0;
    return PARSE_OK;
}

int parse_classic_frame(const char *body, twai_frame_t *f)
{
    if (!body || !f) {
        return PARSE_INVALID_ARG;
    }

    /* Handle RTR frame */
    if (*body == 'R' || *body == 'r') {
        f->header.rtr = true;
        f->buffer_len = 0; // RTR frames have no data payload.
        const char *dlc_str = body + 1;
        uint8_t dlc = TWAI_RTR_DEFAULT_DLC; // Default DLC for RTR frame if not specified.

        if (*dlc_str != '\0') {
            // An explicit DLC is provided, e.g., "R8".
            char *endptr;
            dlc = (uint8_t)strtoul(dlc_str, &endptr, 16);
            if (*endptr != '\0' || dlc > TWAI_FRAME_MAX_LEN) {
                return PARSE_ERROR;
            }
        }

        f->header.dlc = dlc;
        return PARSE_OK;
    }

    /* Handle data frame */
    f->header.rtr = false; // Ensure RTR flag is cleared.

    int dl = parse_payload(body, f->buffer, TWAI_FRAME_MAX_LEN);
    if (dl < 0) {
        return dl;
    }

    /* Check for optional _dlc suffix */
    const char *underscore = strchr(body, '_');
    if (underscore && underscore[1] != '\0') {
        uint8_t dlc = (uint8_t)strtoul(underscore + 1, NULL, 16);
        if (dlc <= TWAI_FRAME_MAX_LEN) {
            f->header.dlc = dlc;
        } else {
            f->header.dlc = TWAI_FRAME_MAX_LEN;
        }
    } else {
        f->header.dlc = (uint8_t)dl;
    }
    f->buffer_len = dl;
    return PARSE_OK;
}

int parse_twaifd_frame(const char *body, twai_frame_t *f)
{
    if (!body || !f) {
        return PARSE_INVALID_ARG;
    }
    uint8_t flags;
    if (parse_nibble(*body++, &flags) != PARSE_OK || flags > TWAI_FD_FLAGS_MAX_VALUE) {
        return PARSE_OUT_OF_RANGE;
    }
    f->header.fdf = true;
    f->header.brs = !!(flags & TWAI_FD_BRS_FLAG_MASK);
    f->header.esi = !!(flags & TWAI_FD_ESI_FLAG_MASK);
    int dl = parse_payload(body, f->buffer, TWAIFD_FRAME_MAX_LEN);
    if (dl < 0) {
        return dl;
    }
    f->buffer_len = dl;
    f->header.dlc = (uint8_t)twaifd_len2dlc((uint16_t)dl);
    return PARSE_OK;
}

int parse_pair_token(const char *tok, size_t tok_len, char sep,
                     uint32_t *lhs, size_t *lhs_chars,
                     uint32_t *rhs, size_t *rhs_chars)
{
    if (!tok || tok_len == 0 || !lhs || !rhs || !lhs_chars || !rhs_chars) {
        return PARSE_INVALID_ARG;
    }

    const char *mid = (const char *)memchr(tok, sep, tok_len);
    if (!mid) {
        return PARSE_NOT_FOUND; /* not this token kind */
    }

    size_t l_len = (size_t)(mid - tok);
    size_t r_len = tok_len - l_len - 1;
    if (l_len == 0 || r_len == 0) {
        return PARSE_ERROR;
    }

    int rl = parse_hex_segment(tok,       l_len, lhs);
    int rr = parse_hex_segment(mid + 1,   r_len, rhs);
    if (rl != PARSE_OK || rr != PARSE_OK) {
        return PARSE_ERROR;
    }

    *lhs_chars = l_len;
    *rhs_chars = r_len;
    return PARSE_OK;
}

const char *twai_state_to_string(twai_error_state_t state)
{
    switch (state) {
    case TWAI_ERROR_ACTIVE:  return "Error Active";
    case TWAI_ERROR_WARNING: return "Error Warning";
    case TWAI_ERROR_PASSIVE: return "Error Passive";
    case TWAI_ERROR_BUS_OFF:  return "Bus Off";
    default: return "Unknown";
    }
}

int format_gpio_pin(int gpio_pin, char *buffer, size_t buffer_size)
{
    if (gpio_pin == GPIO_NUM_NC || gpio_pin < 0) {
        return snprintf(buffer, buffer_size, "Disabled");
    } else {
        return snprintf(buffer, buffer_size, "GPIO%d", gpio_pin);
    }
}

int parse_controller_string(const char *controller_str)
{
    int controller_id;
    const char *end = parse_controller_id(controller_str, &controller_id);
    return end ? controller_id : PARSE_ERROR;
}

void format_twaidump_frame(timestamp_mode_t timestamp_mode, const twai_frame_t *frame,
                           int64_t frame_timestamp, int64_t start_time_us, int64_t *last_frame_time_us,
                           int controller_id, char *output_line, size_t max_len)
{
    char timestamp_str[64] = {0};
    int pos = 0;

    /* Format timestamp */
    format_timestamp(timestamp_mode, frame_timestamp, start_time_us, last_frame_time_us,
                     timestamp_str, sizeof(timestamp_str));

    /* Add timestamp if enabled */
    if (strlen(timestamp_str) > 0) {
        pos += snprintf(output_line + pos, max_len - pos, "%s", timestamp_str);
    }

    /* Add interface name (e.g. use twai0, twai1) */
    pos += snprintf(output_line + pos, max_len - pos, "twai%d  ", controller_id);

    /* Format TWAI ID (formatted as: 3 digits for SFF, 8 digits for EFF) */
    if (frame->header.ide) {
        /* Extended frame: 8 hex digits */
        pos += snprintf(output_line + pos, max_len - pos, "%08" PRIX32 "  ", frame->header.id);
    } else {
        /* Standard frame: 3 hex digits (or less if ID is smaller) */
        pos += snprintf(output_line + pos, max_len - pos, "%03" PRIX32 "  ", frame->header.id);
    }

    if (frame->header.rtr) {
        /* RTR frame: add [R] and DLC */
        pos += snprintf(output_line + pos, max_len - pos, "[R%d]", frame->header.dlc);
    } else {
        /* Data frame: add DLC and data bytes with spaces */
        printf("frame->header.dlc: %d\n", frame->header.dlc);
        int actual_len = twaifd_dlc2len(frame->header.dlc);
        pos += snprintf(output_line + pos, max_len - pos, "[%d]", actual_len);
        for (int i = 0; i < actual_len && i < frame->buffer_len && pos < max_len - 4; i++) {
            pos += snprintf(output_line + pos, max_len - pos, "  %02X", frame->buffer[i]);
        }
    }

    /* Add newline */
    if (pos < max_len - 1) {
        pos += snprintf(output_line + pos, max_len - pos, "\n");
    }
}
