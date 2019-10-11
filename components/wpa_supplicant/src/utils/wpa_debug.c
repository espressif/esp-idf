/*
 * wpa_supplicant/hostapd / Debug prints
 * Copyright (c) 2002-2007, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */
#ifdef ESP_SUPPLICANT
#include "utils/includes.h"
#include "utils/common.h"
#include "utils/wpa_debug.h"

static inline int  _wpa_snprintf_hex(char *buf, size_t buf_size, const u8 *data, size_t len, int uppercase)
{
    size_t i;
    char *pos = buf, *end = buf + buf_size;
    int ret;

    if (buf_size == 0)
        return 0;

    for (i = 0; i < len; i++) {
        ret = snprintf(pos, end - pos, uppercase? "%02X":"%02x", data[i]);
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
	return _wpa_snprintf_hex(buf, buf_size, data, len, 1);
}

int  wpa_snprintf_hex(char *buf, size_t buf_size, const u8 *data, size_t len)
{
	return _wpa_snprintf_hex(buf, buf_size, data, len, 0);
}

#ifdef DEBUG_PRINT
void  wpa_dump_mem(char* desc, uint8_t *addr, uint16_t len)
{
    wpa_printf(MSG_DEBUG, "%s\n", desc);
    if (addr){
        uint16_t i=0;
        for (i=0; i<len; i++){
            if (i%16==0) wpa_printf(MSG_DEBUG, "\n");
            wpa_printf(MSG_DEBUG, "%02x ", addr[i]);
        }
        wpa_printf(MSG_DEBUG, "\n");
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

	if (level < MSG_MSGDUMP)
		return;

	wpa_printf(MSG_DEBUG, "%s - hexdump(len=%lu):\n", title, (unsigned long) len);
	if (buf == NULL) {
		wpa_printf(MSG_DEBUG, " [NULL]\n");
	} else {
		for (i = 0; i < len; i++) {
			wpa_printf(MSG_DEBUG, " %02x", buf[i]);
            if((i+1) % 16 == 0)
                wpa_printf(MSG_DEBUG, "\n");
        }
	} 
	wpa_printf(MSG_DEBUG, "\n");
#endif	
}

void  wpa_hexdump_key(int level, const char *title, const u8 *buf, size_t len)
{
     wpa_hexdump(level, title, buf, len);
}
#endif

int  eloop_cancel_timeout(eloop_timeout_handler handler,
			 void *eloop_data, void *user_data)
{
    return 0;	
}

int  eloop_register_timeout(unsigned int secs, unsigned int usecs,
			   eloop_timeout_handler handler,
			   void *eloop_data, void *user_data)
{
    return 0;
}
#endif // ESP_SUPPLICANT

