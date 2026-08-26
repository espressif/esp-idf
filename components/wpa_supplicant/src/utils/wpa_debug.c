/*
 * wpa_supplicant/hostapd / Debug prints
 * Copyright (c) 2002-2013, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */
#ifdef ESP_SUPPLICANT
#include "utils/includes.h"
#include "utils/common.h"
#include "utils/wpa_debug.h"

static inline int
_wpa_snprintf_hex(char *buf, size_t buf_size, const u8 *data, size_t len,
                  int uppercase, int whitespace)
{
    size_t i;
    char *pos = buf, *end = buf + buf_size;
    int ret;

    static const char *fmt_upper = "%02X";
    static const char *fmt_lower = "%02x";
    static const char *fmt_upper_ws = "%02X ";
    static const char *fmt_lower_ws = "%02x ";
    const char *fmt = uppercase ? (whitespace ? fmt_upper_ws : fmt_upper) :
                                  (whitespace ? fmt_lower_ws : fmt_lower);

    if (buf_size == 0)
        return 0;

    for (i = 0; i < len; i++) {
        ret = snprintf(pos, end - pos, fmt, data[i]);
        if (ret < 0 || ret >= end - pos) {
            end[-1] = '\0';
            return pos - buf;
        }
        pos += ret;
    }
    end[-1]='\0';
    return pos - buf;
}

int  wpa_snprintf_hex_uppercase(char *buf, size_t buf_size, const u8 *data, size_t len)
{
	return _wpa_snprintf_hex(buf, buf_size, data, len, 1, 0);
}

int  wpa_snprintf_hex(char *buf, size_t buf_size, const u8 *data, size_t len)
{
	return _wpa_snprintf_hex(buf, buf_size, data, len, 0, 0);
}

#ifdef DEBUG_PRINT
void  wpa_dump_mem(char* desc, uint8_t *addr, uint16_t len)
{
    char output[50];
    wpa_printf(MSG_DEBUG, "%s", desc);
    if (addr){
        uint16_t i=0;
        for (i = 0; i < len / 16; i++) {
            _wpa_snprintf_hex(output, 50, addr + i * 16, 16, 0, 1);
            wpa_printf(MSG_DEBUG, "%s", output);
        }
        if (len % 16) {
            int bytes_printed = (len / 16) * 16;
            _wpa_snprintf_hex(output, 50, addr + bytes_printed,
                              len - bytes_printed, 0, 1);
            wpa_printf(MSG_DEBUG, "%s", output);
        }
    }
}

void  wpa_debug_print_timestamp(void)
{
#ifdef DEBUG_PRINT
    struct os_time tv;
    os_get_time(&tv);
    wpa_printf(MSG_DEBUG, "%ld.%06u: ", (long) tv.sec, (unsigned int) tv.usec);
#endif
}

void  wpa_hexdump(int level, const char *title, const u8 *buf, size_t len)
{
#ifdef DEBUG_PRINT
	size_t i;
	char output[50];

	if (level < MSG_ERROR)
		return;

	wpa_printf(level, "%s - hexdump(len=%lu):", title, (unsigned long) len);
	if (buf == NULL) {
		wpa_printf(level, " [NULL]");
	} else {
		for (i = 0; i < len / 16; i++) {
			_wpa_snprintf_hex(output, 50, buf + i * 16, 16, 0, 1);
			wpa_printf(level, "%s", output);
		}
		if (len % 16) {
			int bytes_printed = (len / 16) * 16;
			_wpa_snprintf_hex(output, 50, buf + bytes_printed,
							  len - bytes_printed, 0, 1);
			wpa_printf(level, "%s", output);
		}
	}
#endif
}

void  wpa_hexdump_key(int level, const char *title, const u8 *buf, size_t len)
{
     wpa_hexdump(level, title, buf, len);
}
#endif

#endif // ESP_SUPPLICANT
