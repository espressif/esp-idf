/*
 * SPDX-FileCopyrightText: 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <zephyr/sys/__assert.h>

#define ASCII_CHAR 0x7F
#define MSB_SET 0x80
#define SEQUENCE_FIRST_MASK 0xC0
#define SEQUENCE_LEN_2_BYTE 0xC0
#define SEQUENCE_LEN_3_BYTE 0xE0
#define SEQUENCE_LEN_4_BYTE 0xF0

char *utf8_trunc(char *utf8_str)
{
    const size_t len = strlen(utf8_str);

    if (len == 0U) {
        /* no-op */
        return utf8_str;
    }

    char *last_byte_p = utf8_str + len - 1U;
    uint8_t bytes_truncated;
    char seq_start_byte;

    if ((*last_byte_p & ASCII_CHAR) == *last_byte_p) {
        /* Not part of an UTF8 sequence, return */
        return utf8_str;
    }

    /* Find the starting byte and NULL-terminate other bytes */
    bytes_truncated = 0;
    while ((*last_byte_p & SEQUENCE_FIRST_MASK) != SEQUENCE_FIRST_MASK &&
            last_byte_p > utf8_str) {
        last_byte_p--;
        bytes_truncated++;
    }
    bytes_truncated++; /* include the starting byte */

    /* Verify if the last character actually need to be truncated
     * Handles the case where the number of bytes in the last UTF8-char
     * matches the number of bytes we searched for the starting byte
     */
    seq_start_byte = *last_byte_p;
    if ((seq_start_byte & SEQUENCE_LEN_4_BYTE) == SEQUENCE_LEN_4_BYTE) {
        if (bytes_truncated == 4) {
            return utf8_str;
        }
    } else if ((seq_start_byte & SEQUENCE_LEN_3_BYTE) == SEQUENCE_LEN_3_BYTE) {
        if (bytes_truncated == 3) {
            return utf8_str;
        }
    } else if ((seq_start_byte & SEQUENCE_LEN_2_BYTE) == SEQUENCE_LEN_2_BYTE) {
        if (bytes_truncated == 2) {
            return utf8_str;
        }
    }

    /* NULL-terminate the unterminated starting byte */
    *last_byte_p = '\0';

    return utf8_str;
}

char *utf8_lcpy(char *dst, const char *src, size_t n)
{
    if (n > 0) {
        strncpy(dst, src, n - 1);
        dst[n - 1] = '\0';

        if (n != 1) {
            utf8_trunc(dst);
        }
    }

    return dst;
}

int utf8_count_chars(const char *s)
{
    int count = 0;
    const char *p = s; /* getting a pointer to increment */

    while (*p != '\0') {
        if ((*p & MSB_SET) == 0) { /* 1-byte character: 0xxxxxxx */
            p += 1;
        } else if ((*p & SEQUENCE_LEN_3_BYTE) == SEQUENCE_FIRST_MASK) {
            /* 2-byte character: 110xxxxx */
            if ((p[1] & SEQUENCE_FIRST_MASK) != MSB_SET) {
                /* invalid continuation */
                return -EINVAL;
            }
            p += 2;
        } else if ((*p & SEQUENCE_LEN_4_BYTE) == SEQUENCE_LEN_3_BYTE) {
            /* 3-byte character: 1110xxxx */
            if ((p[1] & SEQUENCE_FIRST_MASK) != MSB_SET
                || (p[2] & SEQUENCE_FIRST_MASK) != MSB_SET) {
                /* invalid continuation */
                return -EINVAL;
            }
            p += 3;
        } else if ((*p & 0xF8) == SEQUENCE_LEN_4_BYTE) {
            /* 4-byte character: 11110xxx */
            if ((p[1] & SEQUENCE_FIRST_MASK) != MSB_SET
                || (p[2] & SEQUENCE_FIRST_MASK) != MSB_SET
                || (p[3] & SEQUENCE_FIRST_MASK) != MSB_SET) {
                /* invalid continuation */
                return -EINVAL;
            }
            p += 4;
        } else {
            /* Invalid UTF-8 byte (return) */
            return -EINVAL;
        }
        count++;
    }

    return count;
}
