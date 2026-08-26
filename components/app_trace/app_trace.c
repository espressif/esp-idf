/*
 * SPDX-FileCopyrightText: 2017-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#include <string.h>
#include "esp_cpu.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "esp_app_trace.h"
#include "esp_app_trace_port.h"
#include "esp_app_trace_types.h"
#include "esp_private/startup_internal.h"

#if CONFIG_ESP_CONSOLE_UART && CONFIG_APPTRACE_DEST_UART && (CONFIG_APPTRACE_DEST_UART_NUM == CONFIG_ESP_CONSOLE_UART_NUM)
#error "Application trace UART and console UART cannot use the same port number"
#endif

#define ESP_APPTRACE_MAX_VPRINTF_ARGS 256
#define ESP_APPTRACE_HOST_BUF_SIZE 256

const static char *TAG = "esp_apptrace";

/** tracing module internal data */
typedef struct {
    esp_apptrace_hw_t *hw;
    void               *hw_data;
    esp_apptrace_dest_t dest;
} esp_apptrace_channel_t;

static esp_apptrace_channel_t s_trace_ch;
static volatile int s_trace_ch_hw_initialized = 0;

esp_err_t esp_apptrace_init(const esp_apptrace_config_t *config)
{
    __attribute__((unused)) void *hw_data = NULL;

    if (esp_cpu_get_core_id() == 0) {
#if CONFIG_APPTRACE_DEST_JTAG
        s_trace_ch.hw = esp_apptrace_jtag_hw_get(&hw_data);
        s_trace_ch.hw_data = hw_data;
#elif CONFIG_APPTRACE_DEST_UART
        const esp_apptrace_uart_config_t *uart_config = &config->dest_cfg.uart;
        s_trace_ch.hw = esp_apptrace_uart_hw_get(uart_config->uart_num, &hw_data);
        s_trace_ch.hw_data = hw_data;
#else // CONFIG_APPTRACE_DEST_ALL allows runtime selection between destinations
        if (config->dest == ESP_APPTRACE_DEST_JTAG) {
            s_trace_ch.hw = esp_apptrace_jtag_hw_get(&hw_data);
            s_trace_ch.hw_data = hw_data;
        } else if (config->dest == ESP_APPTRACE_DEST_UART) {
            const esp_apptrace_uart_config_t *uart_config = &config->dest_cfg.uart;
            s_trace_ch.hw = esp_apptrace_uart_hw_get(uart_config->uart_num, &hw_data);
            s_trace_ch.hw_data = hw_data;
        } else {
            s_trace_ch.hw = NULL;
            s_trace_ch.hw_data = NULL;
            ESP_APPTRACE_LOGE("Invalid destination type (%d)!", config->dest);
            return ESP_ERR_INVALID_ARG;
        }
#endif
        s_trace_ch.dest = config->dest;
        s_trace_ch_hw_initialized = 1;
    } else {
        // There is NO guarantee that system init functions will execute on core 0 first
        // So we need to wait for core 0 to set up the hardware interface
        while (!s_trace_ch_hw_initialized) {
            esp_rom_delay_us(10);
        }
    }

    if (s_trace_ch.hw) {
        int res = s_trace_ch.hw->init(s_trace_ch.hw_data, config);
        if (res != ESP_OK) {
            ESP_APPTRACE_LOGE("Failed to init trace channel HW interface (%d)!", res);
            return res;
        }
    }

    return ESP_OK;
}

esp_err_t esp_apptrace_down_buffer_config(uint8_t *buf, uint32_t size)
{
    if (!buf || size == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!s_trace_ch.hw) {
        return ESP_ERR_INVALID_STATE;
    }
    if (s_trace_ch.hw->down_buffer_config) {
        s_trace_ch.hw->down_buffer_config(s_trace_ch.hw_data, buf, size);
    }

    return ESP_OK;
}

uint8_t *esp_apptrace_down_buffer_get(uint32_t *size, uint32_t user_tmo)
{
    ESP_APPTRACE_LOGV("%s(): enter", __func__);

    if (!size || *size == 0) {
        return NULL;
    }
    if (!s_trace_ch.hw) {
        return NULL;
    }
    if (!s_trace_ch.hw->get_down_buffer) {
        return NULL;
    }

    esp_apptrace_tmo_t tmo;
    esp_apptrace_tmo_init(&tmo, user_tmo);

    return s_trace_ch.hw->get_down_buffer(s_trace_ch.hw_data, size, &tmo);
}

esp_err_t esp_apptrace_down_buffer_put(uint8_t *ptr, uint32_t user_tmo)
{
    ESP_APPTRACE_LOGV("%s(): enter", __func__);

    if (!ptr) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!s_trace_ch.hw) {
        return ESP_ERR_INVALID_STATE;
    }
    if (!s_trace_ch.hw->get_down_buffer) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    esp_apptrace_tmo_t tmo;
    esp_apptrace_tmo_init(&tmo, user_tmo);

    return s_trace_ch.hw->put_down_buffer(s_trace_ch.hw_data, ptr, &tmo);
}

esp_err_t esp_apptrace_read(void *buf, uint32_t *size, uint32_t user_tmo)
{
    ESP_APPTRACE_LOGV("%s(): enter", __func__);

    if (!buf || !size || *size == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!s_trace_ch.hw) {
        return ESP_ERR_INVALID_STATE;
    }
    if (!s_trace_ch.hw->get_down_buffer || !s_trace_ch.hw->put_down_buffer) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    //TODO: callback system
    esp_apptrace_tmo_t tmo;
    esp_apptrace_tmo_init(&tmo, user_tmo);

    uint32_t act_sz = *size;
    *size = 0;
    uint8_t *ptr = s_trace_ch.hw->get_down_buffer(s_trace_ch.hw_data, &act_sz, &tmo);
    if (ptr && act_sz > 0) {
        ESP_APPTRACE_LOGD("Read %" PRIu32 " bytes from host", act_sz);
        memcpy(buf, ptr, act_sz);
        *size = act_sz;
        return s_trace_ch.hw->put_down_buffer(s_trace_ch.hw_data, ptr, &tmo);
    }

    return ESP_ERR_TIMEOUT;
}

uint8_t *esp_apptrace_buffer_get(uint32_t size, uint32_t user_tmo)
{
    ESP_APPTRACE_LOGV("%s(): enter", __func__);

    if (size == 0) {
        return NULL;
    }
    if (!s_trace_ch.hw) {
        return NULL;
    }
    if (!s_trace_ch.hw->get_up_buffer) {
        return NULL;
    }

    esp_apptrace_tmo_t tmo;
    esp_apptrace_tmo_init(&tmo, user_tmo);

    return s_trace_ch.hw->get_up_buffer(s_trace_ch.hw_data, size, &tmo);
}

esp_err_t esp_apptrace_buffer_put(uint8_t *ptr, uint32_t user_tmo)
{
    ESP_APPTRACE_LOGV("%s(): enter", __func__);

    if (!ptr) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!s_trace_ch.hw) {
        return ESP_ERR_INVALID_STATE;
    }
    if (!s_trace_ch.hw->put_up_buffer) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    esp_apptrace_tmo_t tmo;
    esp_apptrace_tmo_init(&tmo, user_tmo);

    return s_trace_ch.hw->put_up_buffer(s_trace_ch.hw_data, ptr, &tmo);
}

esp_err_t esp_apptrace_write(const void *data, uint32_t size, uint32_t user_tmo)
{
    ESP_APPTRACE_LOGV("%s(): enter", __func__);

    if (!data || size == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!s_trace_ch.hw) {
        return ESP_ERR_INVALID_STATE;
    }
    if (!s_trace_ch.hw->get_up_buffer || !s_trace_ch.hw->put_up_buffer) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    esp_apptrace_tmo_t tmo;
    esp_apptrace_tmo_init(&tmo, user_tmo);

    uint8_t *ptr = s_trace_ch.hw->get_up_buffer(s_trace_ch.hw_data, size, &tmo);
    if (!ptr) {
        return ESP_ERR_NO_MEM;
    }

    // actually can be suspended here by higher prio tasks/ISRs
    //TODO: use own memcpy with dead trace calls kick-off algo and tmo expiration check
    memcpy(ptr, data, size);

    // now indicate that this buffer is ready to be sent off to host
    return s_trace_ch.hw->put_up_buffer(s_trace_ch.hw_data, ptr, &tmo);
}

int esp_apptrace_vprintf_to(uint32_t user_tmo, const char *fmt, va_list ap)
{
    uint16_t nargs = 0;
    uint8_t *pout, *p = (uint8_t *)fmt;

    ESP_APPTRACE_LOGV("%s(): enter", __func__);

    if (!fmt) {
        return -1;
    }
    if (!s_trace_ch.hw) {
        return -1;
    }
    if (!s_trace_ch.hw->get_up_buffer || !s_trace_ch.hw->put_up_buffer) {
        return -1;
    }

    esp_apptrace_tmo_t tmo;
    esp_apptrace_tmo_init(&tmo, user_tmo);

    ESP_APPTRACE_LOGD("fmt %p", fmt);
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

    pout = s_trace_ch.hw->get_up_buffer(s_trace_ch.hw_data, 1 + sizeof(char *) + nargs * sizeof(uint32_t), &tmo);
    if (!pout) {
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
        ESP_APPTRACE_LOGD("arg %" PRIx32, arg);
    }

    int ret = s_trace_ch.hw->put_up_buffer(s_trace_ch.hw_data, p, &tmo);
    if (ret != ESP_OK) {
        ESP_APPTRACE_LOGE("Failed to put printf buf (%d)!", ret);
        return -1;
    }

    return (pout - p);
}

int esp_apptrace_vprintf(const char *fmt, va_list ap)
{
    return esp_apptrace_vprintf_to(0, fmt, ap);
}

esp_err_t esp_apptrace_flush_nolock(uint32_t min_sz, uint32_t usr_tmo)
{
    ESP_APPTRACE_LOGV("%s(): enter", __func__);

    if (!s_trace_ch.hw) {
        return ESP_ERR_INVALID_STATE;
    }
    if (!s_trace_ch.hw->flush_up_buffer_nolock) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    esp_apptrace_tmo_t tmo;
    esp_apptrace_tmo_init(&tmo, usr_tmo);

    return s_trace_ch.hw->flush_up_buffer_nolock(s_trace_ch.hw_data, min_sz, &tmo);
}

esp_err_t esp_apptrace_flush(uint32_t usr_tmo)
{
    ESP_APPTRACE_LOGV("%s(): enter", __func__);

    if (!s_trace_ch.hw) {
        return ESP_ERR_INVALID_STATE;
    }
    if (!s_trace_ch.hw->flush_up_buffer) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    esp_apptrace_tmo_t tmo;
    esp_apptrace_tmo_init(&tmo, usr_tmo);

    return s_trace_ch.hw->flush_up_buffer(s_trace_ch.hw_data, &tmo);
}

bool esp_apptrace_host_is_connected(void)
{
    ESP_APPTRACE_LOGV("%s(): enter", __func__);

    if (!s_trace_ch.hw) {
        return false;
    }
    if (!s_trace_ch.hw->host_is_connected) {
        return false;
    }

    return s_trace_ch.hw->host_is_connected(s_trace_ch.hw_data);
}

esp_apptrace_dest_t esp_apptrace_get_destination(void)
{
    return s_trace_ch.dest;
}

esp_err_t esp_apptrace_set_header_size(esp_apptrace_header_size_t header_size)
{
    if (!s_trace_ch.hw) {
        return ESP_ERR_INVALID_STATE;
    }
    if (s_trace_ch.hw->set_header_size) {
        s_trace_ch.hw->set_header_size(s_trace_ch.hw_data, header_size);
    }
    return ESP_OK;
}

/* If any trace library (sysview or external) is selected with the apptrace transport,
 * initialization will be handled by the esp_trace component
 */
#if CONFIG_ESP_TRACE_LIB_NONE && CONFIG_ESP_TRACE_TRANSPORT_APPTRACE

esp_apptrace_config_t __attribute__((weak)) esp_apptrace_get_user_params(void)
{
    esp_apptrace_config_t default_config = APPTRACE_CONFIG_DEFAULT();
    return default_config;
}

ESP_SYSTEM_INIT_FN(apptrace_early_init, SECONDARY, ESP_SYSTEM_INIT_ALL_CORES, 115)
{
    esp_apptrace_config_t config = esp_apptrace_get_user_params();
    return esp_apptrace_init(&config);
}
#endif
