/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * This file implements esp_err_to_name() using a link-time array.
 *
 * Components register their error codes by placing esp_err_msg_t arrays into
 * the ".esp_err_msg_tbl" linker section via idf_define_esp_err_codes() in
 * their CMakeLists.txt. The linker collects all such entries between the
 * _esp_err_msg_tbl_start and _esp_err_msg_tbl_end symbols.
 */

#include <string.h>
#include <stdio.h>
#include "esp_err.h"
#include "esp_err_msg.h"
#include "esp_attr.h"

#ifdef CONFIG_ESP_ERR_TO_NAME_LOOKUP
_SECTION_ATTR_SYMBOL_DECL_GENERIC(esp_err_msg_t, esp_err_msg_tbl)
#endif /* CONFIG_ESP_ERR_TO_NAME_LOOKUP */

static const char esp_unknown_msg[] =
#ifdef CONFIG_ESP_ERR_TO_NAME_LOOKUP
    "ERROR";
#else
    "UNKNOWN ERROR";
#endif //CONFIG_ESP_ERR_TO_NAME_LOOKUP

const char *esp_err_to_name(esp_err_t code)
{
#ifdef CONFIG_ESP_ERR_TO_NAME_LOOKUP
    const esp_err_msg_t *p;
    for (p = _SECTION_START(esp_err_msg_tbl); p < _SECTION_END(esp_err_msg_tbl); ++p) {
        if (p->code == code) {
            return p->msg;
        }
    }
#endif //CONFIG_ESP_ERR_TO_NAME_LOOKUP

    return esp_unknown_msg;
}

const char *esp_err_to_name_r(esp_err_t code, char *buf, size_t buflen)
{
#ifdef CONFIG_ESP_ERR_TO_NAME_LOOKUP
    const esp_err_msg_t *p;
    for (p = _SECTION_START(esp_err_msg_tbl); p < _SECTION_END(esp_err_msg_tbl); ++p) {
        if (p->code == code) {
            strlcpy(buf, p->msg, buflen);
            return buf;
        }
    }
#endif //CONFIG_ESP_ERR_TO_NAME_LOOKUP

    if (strerror_r(code, buf, buflen) == 0) {
        return buf;
    }

    snprintf(buf, buflen, "%s 0x%x(%d)", esp_unknown_msg, code, code);

    return buf;
}
