/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_cpu.h"
#include "esp_log.h"
#include "esp_app_trace.h"
#include "esp_app_trace_port.h"
#include "esp_private/startup_internal.h"

#ifdef CONFIG_APPTRACE_DEST_UART0
#define ESP_APPTRACE_DEST_UART_NUM 0
#elif CONFIG_APPTRACE_DEST_UART1
#define ESP_APPTRACE_DEST_UART_NUM 1
#elif CONFIG_APPTRACE_DEST_UART2
#define ESP_APPTRACE_DEST_UART_NUM 2
#elif CONFIG_APPTRACE_DEST_USB_CDC
#define ESP_APPTRACE_DEST_UART_NUM 10
#else
#define ESP_APPTRACE_DEST_UART_NUM 0
#endif

#define ESP_APPTRACE_MAX_VPRINTF_ARGS 256
#define ESP_APPTRACE_HOST_BUF_SIZE 256

#define ESP_APPTRACE_PRINT_LOCK 0

const static char *TAG = "esp_apptrace";

/** tracing module internal data */
typedef struct {
    esp_apptrace_hw_t *hw;
    void               *hw_data;
} esp_apptrace_channel_t;

static esp_apptrace_channel_t   s_trace_channels[ESP_APPTRACE_DEST_MAX];
static bool s_inited;

esp_err_t esp_apptrace_init(void)
{
    int res;
    esp_apptrace_hw_t *hw = NULL;
    void *hw_data = NULL;

    // 'esp_apptrace_init()' is called on every core, so ensure to do main initialization only once
    if (esp_cpu_get_core_id() == 0) {
        memset(&s_trace_channels, 0, sizeof(s_trace_channels));
        hw = esp_apptrace_jtag_hw_get(&hw_data);
        ESP_APPTRACE_LOGD("HW interface %p", hw);
        if (hw != NULL) {
            s_trace_channels[ESP_APPTRACE_DEST_JTAG].hw = hw;
            s_trace_channels[ESP_APPTRACE_DEST_JTAG].hw_data = hw_data;
        }
        hw = esp_apptrace_uart_hw_get(ESP_APPTRACE_DEST_UART_NUM, &hw_data);
        if (hw != NULL) {
            s_trace_channels[ESP_APPTRACE_DEST_UART].hw = hw;
            s_trace_channels[ESP_APPTRACE_DEST_UART].hw_data = hw_data;
        }
        s_inited = true;
    }

    // esp_apptrace_init() is called on every core, so initialize trace channel on every core
    for (int i = 0; i < sizeof(s_trace_channels) / sizeof(s_trace_channels[0]); i++) {
        esp_apptrace_channel_t *ch = &s_trace_channels[i];
        if (ch->hw) {
            res = ch->hw->init(ch->hw_data);
            if (res != ESP_OK) {
                ESP_APPTRACE_LOGE("Failed to init trace channel HW interface (%d)!", res);
                return res;
            }
        }
    }

    return ESP_OK;
}

ESP_SYSTEM_INIT_FN(esp_apptrace_init, ESP_SYSTEM_INIT_ALL_CORES, 115)
{
    return esp_apptrace_init();
}

void esp_apptrace_down_buffer_config(uint8_t *buf, uint32_t size)
{
    esp_apptrace_channel_t *ch;

    if (!s_inited) {
        return;
    }
    // currently down buffer is supported for JTAG interface only
    // TODO: one more argument should be added to this function to specify HW inteface: JTAG, UART0 etc
    ch = &s_trace_channels[ESP_APPTRACE_DEST_JTAG];
    if (ch->hw != NULL) {
        if (ch->hw->down_buffer_config != NULL) {
            ch->hw->down_buffer_config(ch->hw_data, buf, size);
        }
    } else {
        ESP_APPTRACE_LOGD("Trace destination for JTAG not supported!");
    }
    ch = &s_trace_channels[ESP_APPTRACE_DEST_UART];
    if (ch->hw != NULL) {
        if (ch->hw->down_buffer_config != NULL) {
            ch->hw->down_buffer_config(ch->hw_data, buf, size);
        }
    } else {
        ESP_APPTRACE_LOGD("Trace destination for UART not supported!");
    }
}

uint8_t *esp_apptrace_down_buffer_get(esp_apptrace_dest_t dest, uint32_t *size, uint32_t user_tmo)
{
    esp_apptrace_tmo_t tmo;
    esp_apptrace_channel_t *ch;

    ESP_APPTRACE_LOGV("%s(): enter", __func__);
    if (dest >= ESP_APPTRACE_DEST_MAX) {
        return NULL;
    }
    if (size == NULL || *size == 0) {
        return NULL;
    }
    if (!s_inited) {
        return NULL;
    }
    ch = &s_trace_channels[dest];
    if (ch->hw == NULL) {
        ESP_APPTRACE_LOGE("Trace destination %d not supported!", dest);
        return NULL;
    }
    if (ch->hw->get_down_buffer == NULL) {
        return NULL;
    }

    esp_apptrace_tmo_init(&tmo, user_tmo);
    return ch->hw->get_down_buffer(ch->hw_data, size, &tmo);
}

esp_err_t esp_apptrace_down_buffer_put(esp_apptrace_dest_t dest, uint8_t *ptr, uint32_t user_tmo)
{
    esp_apptrace_tmo_t tmo;
    esp_apptrace_channel_t *ch;

    ESP_APPTRACE_LOGV("%s(): enter", __func__);
    if (dest >= ESP_APPTRACE_DEST_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    if (ptr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!s_inited) {
        return ESP_ERR_INVALID_STATE;
    }
    ch = &s_trace_channels[dest];
    if (ch->hw == NULL) {
        ESP_APPTRACE_LOGE("Trace destination %d not supported!", dest);
        return ESP_ERR_NOT_SUPPORTED;
    }
    if (ch->hw->get_down_buffer == NULL) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    esp_apptrace_tmo_init(&tmo, user_tmo);
    return ch->hw->put_down_buffer(ch->hw_data, ptr, &tmo);
}

esp_err_t esp_apptrace_read(esp_apptrace_dest_t dest, void *buf, uint32_t *size, uint32_t user_tmo)
{
    int res = ESP_OK;
    esp_apptrace_tmo_t tmo;
    esp_apptrace_channel_t *ch;

    ESP_APPTRACE_LOGV("%s(): enter", __func__);
    if (dest >= ESP_APPTRACE_DEST_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    if (buf == NULL || size == NULL || *size == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!s_inited) {
        return ESP_ERR_INVALID_STATE;
    }
    ch = &s_trace_channels[dest];
    if (ch->hw == NULL) {
        ESP_APPTRACE_LOGE("Trace destination %d not supported!", dest);
        return ESP_ERR_NOT_SUPPORTED;
    }
    if (ch->hw->get_down_buffer == NULL || ch->hw->put_down_buffer == NULL) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    //TODO: callback system
    esp_apptrace_tmo_init(&tmo, user_tmo);
    uint32_t act_sz = *size;
    *size = 0;
    uint8_t *ptr = ch->hw->get_down_buffer(ch->hw_data, &act_sz, &tmo);
    if (ptr && act_sz > 0) {
        ESP_APPTRACE_LOGD("Read %d bytes from host", act_sz);
        memcpy(buf, ptr, act_sz);
        res = ch->hw->put_down_buffer(ch->hw_data, ptr, &tmo);
        *size = act_sz;
    } else {
        res = ESP_ERR_TIMEOUT;
    }

    return res;
}

uint8_t *esp_apptrace_buffer_get(esp_apptrace_dest_t dest, uint32_t size, uint32_t user_tmo)
{
    esp_apptrace_tmo_t tmo;
    esp_apptrace_channel_t *ch;

    ESP_APPTRACE_LOGV("%s(): enter", __func__);
    if (dest >= ESP_APPTRACE_DEST_MAX) {
        return NULL;
    }
    if (size == 0) {
        return NULL;
    }
    if (!s_inited) {
        return NULL;
    }
    ch = &s_trace_channels[dest];
    if (ch->hw == NULL) {
        ESP_APPTRACE_LOGE("Trace destination %d not supported!", dest);
        return NULL;
    }
    if (ch->hw->get_up_buffer == NULL) {
        return NULL;
    }

    esp_apptrace_tmo_init(&tmo, user_tmo);
    return ch->hw->get_up_buffer(ch->hw_data, size, &tmo);
}

esp_err_t esp_apptrace_buffer_put(esp_apptrace_dest_t dest, uint8_t *ptr, uint32_t user_tmo)
{
    esp_apptrace_tmo_t tmo;
    esp_apptrace_channel_t *ch;

    ESP_APPTRACE_LOGV("%s(): enter", __func__);
    if (dest >= ESP_APPTRACE_DEST_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    if (ptr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!s_inited) {
        return ESP_ERR_INVALID_STATE;
    }
    ch = &s_trace_channels[dest];
    if (ch->hw == NULL) {
        ESP_APPTRACE_LOGE("Trace destination %d not supported!", dest);
        return ESP_ERR_NOT_SUPPORTED;
    }
    if (ch->hw->put_up_buffer == NULL) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    esp_apptrace_tmo_init(&tmo, user_tmo);
    return ch->hw->put_up_buffer(ch->hw_data, ptr, &tmo);
}

esp_err_t esp_apptrace_write(esp_apptrace_dest_t dest, const void *data, uint32_t size, uint32_t user_tmo)
{
    uint8_t *ptr = NULL;
    esp_apptrace_tmo_t tmo;
    esp_apptrace_channel_t *ch;

    ESP_APPTRACE_LOGV("%s(): enter", __func__);
    if (dest >= ESP_APPTRACE_DEST_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    if (data == NULL || size == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!s_inited) {
        return ESP_ERR_INVALID_STATE;
    }
    ch = &s_trace_channels[dest];
    if (ch->hw == NULL) {
        ESP_APPTRACE_LOGE("Trace destination %d not supported!", dest);
        return ESP_ERR_NOT_SUPPORTED;
    }
    if (ch->hw->get_up_buffer == NULL || ch->hw->put_up_buffer == NULL) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    esp_apptrace_tmo_init(&tmo, user_tmo);
    ptr = ch->hw->get_up_buffer(ch->hw_data, size, &tmo);
    if (ptr == NULL) {
        return ESP_ERR_NO_MEM;
    }

    // actually can be suspended here by higher prio tasks/ISRs
    //TODO: use own memcpy with dead trace calls kick-off algo and tmo expiration check
    memcpy(ptr, data, size);

    // now indicate that this buffer is ready to be sent off to host
    return ch->hw->put_up_buffer(ch->hw_data, ptr, &tmo);
}

int esp_apptrace_vprintf_to(esp_apptrace_dest_t dest, uint32_t user_tmo, const char *fmt, va_list ap)
{
    uint16_t nargs = 0;
    uint8_t *pout, *p = (uint8_t *)fmt;
    esp_apptrace_tmo_t tmo;
    esp_apptrace_channel_t *ch;

    ESP_APPTRACE_LOGV("%s(): enter", __func__);
    if (dest >= ESP_APPTRACE_DEST_MAX) {
        return -1;
    }
    if (fmt == NULL) {
        return -1;
    }
    if (!s_inited) {
        return -1;
    }
    ch = &s_trace_channels[dest];
    if (ch->hw == NULL) {
        ESP_APPTRACE_LOGE("Trace destination %d not supported!", dest);
        return -1;
    }
    if (ch->hw->get_up_buffer == NULL || ch->hw->put_up_buffer == NULL) {
        return -1;
    }

    esp_apptrace_tmo_init(&tmo, user_tmo);
    ESP_APPTRACE_LOGD("fmt %x", fmt);
    while ((p = (uint8_t *)strchr((char *)p, '%')) && nargs < ESP_APPTRACE_MAX_VPRINTF_ARGS) {
        p++;
        if (*p != '%' && *p != 0) {
            nargs++;
        }
    }
    ESP_APPTRACE_LOGD("nargs = %d", nargs);
    if (p) {
        ESP_APPTRACE_LOGE("Failed to store all printf args!");
    }

    pout = ch->hw->get_up_buffer(ch->hw_data, 1 + sizeof(char *) + nargs * sizeof(uint32_t), &tmo);
    if (pout == NULL) {
        ESP_APPTRACE_LOGE("Failed to get buffer!");
        return -1;
    }
    p = pout;
    *pout = nargs;
    pout++;
    *(const char **)pout = fmt;
    pout += sizeof(char *);
    while (nargs-- > 0) {
        uint32_t arg = va_arg(ap, uint32_t);
        *(uint32_t *)pout = arg;
        pout += sizeof(uint32_t);
        ESP_APPTRACE_LOGD("arg %x", arg);
    }

    int ret = ch->hw->put_up_buffer(ch->hw_data, p, &tmo);
    if (ret != ESP_OK) {
        ESP_APPTRACE_LOGE("Failed to put printf buf (%d)!", ret);
        return -1;
    }

    return (pout - p);
}

int esp_apptrace_vprintf(const char *fmt, va_list ap)
{
    return esp_apptrace_vprintf_to(ESP_APPTRACE_DEST_JTAG, 0, fmt, ap);
}

esp_err_t esp_apptrace_flush_nolock(esp_apptrace_dest_t dest, uint32_t min_sz, uint32_t usr_tmo)
{
    esp_apptrace_tmo_t tmo;
    esp_apptrace_channel_t *ch;

    ESP_APPTRACE_LOGV("%s(): enter", __func__);
    if (dest >= ESP_APPTRACE_DEST_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!s_inited) {
        return ESP_ERR_INVALID_STATE;
    }
    ch = &s_trace_channels[dest];
    if (ch->hw == NULL) {
        ESP_APPTRACE_LOGE("Trace destination %d not supported!", dest);
        return ESP_ERR_NOT_SUPPORTED;
    }
    if (ch->hw->flush_up_buffer_nolock == NULL) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    esp_apptrace_tmo_init(&tmo, usr_tmo);
    return ch->hw->flush_up_buffer_nolock(ch->hw_data, min_sz, &tmo);
}

esp_err_t esp_apptrace_flush(esp_apptrace_dest_t dest, uint32_t usr_tmo)
{
    esp_apptrace_tmo_t tmo;
    esp_apptrace_channel_t *ch;

    ESP_APPTRACE_LOGV("%s(): enter", __func__);
    if (dest >= ESP_APPTRACE_DEST_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!s_inited) {
        return ESP_ERR_INVALID_STATE;
    }
    ch = &s_trace_channels[dest];
    if (ch->hw == NULL) {
        ESP_APPTRACE_LOGE("Trace destination %d not supported!", dest);
        return ESP_ERR_NOT_SUPPORTED;
    }
    if (ch->hw->flush_up_buffer == NULL) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    esp_apptrace_tmo_init(&tmo, usr_tmo);
    return ch->hw->flush_up_buffer(ch->hw_data, &tmo);
}

bool esp_apptrace_host_is_connected(esp_apptrace_dest_t dest)
{
    esp_apptrace_channel_t *ch;

    ESP_APPTRACE_LOGV("%s(): enter", __func__);
    if (dest >= ESP_APPTRACE_DEST_MAX) {
        return false;
    }
    if (!s_inited) {
        return false;
    }
    ch = &s_trace_channels[dest];
    if (ch->hw == NULL) {
        ESP_APPTRACE_LOGE("Trace destination %d not supported!", dest);
        return false;
    }
    if (ch->hw->host_is_connected == NULL) {
        return false;
    }

    return ch->hw->host_is_connected(ch->hw_data);
}

#if !CONFIG_APPTRACE_DEST_JTAG
esp_apptrace_hw_t *esp_apptrace_jtag_hw_get(void **data)
{
    return NULL;
}
#endif
