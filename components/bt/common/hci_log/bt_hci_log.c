/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "hci_log/bt_hci_log.h"
#include "bt_common.h"
#include "osi/mutex.h"
#include "esp_attr.h"

#if (BT_HCI_LOG_INCLUDED == TRUE)
#define BT_HCI_LOG_PRINT_TAG                     (1)
#define BT_HCI_LOG_DATA_BUF_SIZE                 (1024 * HCI_LOG_DATA_BUFFER_SIZE)
#define BT_HCI_LOG_ADV_BUF_SIZE                  (1024 * HCI_LOG_ADV_BUFFER_SIZE)

typedef struct {
    osi_mutex_t mutex_lock;
    uint64_t log_record_in;
    uint64_t log_record_out;
    uint64_t buf_size;
    uint8_t *p_hci_log_buffer;
    uint8_t index;
    bool overflow;
} bt_hci_log_t;

static const char s_hex_to_char_mapping[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

bt_hci_log_t g_bt_hci_log_data_ctl  = {0};
bt_hci_log_t g_bt_hci_log_adv_ctl  = {0};

esp_err_t bt_hci_log_init(void)
{
    uint8_t *g_bt_hci_log_data_buffer = NULL;
    uint8_t *g_bt_hci_log_adv_buffer  = NULL;

    g_bt_hci_log_data_buffer = malloc(BT_HCI_LOG_DATA_BUF_SIZE);
    if (!g_bt_hci_log_data_buffer) {
        return ESP_ERR_NO_MEM;
    }
    g_bt_hci_log_adv_buffer = malloc(BT_HCI_LOG_ADV_BUF_SIZE);
    if (!g_bt_hci_log_adv_buffer) {
        if (g_bt_hci_log_data_buffer) {
            free(g_bt_hci_log_data_buffer);
            g_bt_hci_log_data_buffer = NULL;
        }
        return ESP_ERR_NO_MEM;
    }

    memset(g_bt_hci_log_data_buffer, 0, BT_HCI_LOG_DATA_BUF_SIZE);
    memset(g_bt_hci_log_adv_buffer, 0, BT_HCI_LOG_ADV_BUF_SIZE);

    memset(&g_bt_hci_log_data_ctl, 0, sizeof(bt_hci_log_t));
    g_bt_hci_log_data_ctl.buf_size = BT_HCI_LOG_DATA_BUF_SIZE;
    g_bt_hci_log_data_ctl.p_hci_log_buffer = g_bt_hci_log_data_buffer;

    memset(&g_bt_hci_log_adv_ctl, 0, sizeof(bt_hci_log_t));
    g_bt_hci_log_adv_ctl.buf_size = BT_HCI_LOG_ADV_BUF_SIZE;
    g_bt_hci_log_adv_ctl.p_hci_log_buffer = g_bt_hci_log_adv_buffer;

    osi_mutex_new((osi_mutex_t *)&g_bt_hci_log_data_ctl.mutex_lock);
    osi_mutex_new((osi_mutex_t *)&g_bt_hci_log_adv_ctl.mutex_lock);

    return ESP_OK;
}

esp_err_t bt_hci_log_deinit(void)
{
    if (g_bt_hci_log_data_ctl.p_hci_log_buffer) {
        free(g_bt_hci_log_data_ctl.p_hci_log_buffer);
        g_bt_hci_log_data_ctl.p_hci_log_buffer = NULL;
    }

    if (g_bt_hci_log_adv_ctl.p_hci_log_buffer) {
        free(g_bt_hci_log_adv_ctl.p_hci_log_buffer);
        g_bt_hci_log_adv_ctl.p_hci_log_buffer = NULL;
    }

    osi_mutex_free((osi_mutex_t *)&g_bt_hci_log_data_ctl.mutex_lock);
    osi_mutex_free((osi_mutex_t *)&g_bt_hci_log_adv_ctl.mutex_lock);

    memset(&g_bt_hci_log_data_ctl, 0, sizeof(bt_hci_log_t));
    memset(&g_bt_hci_log_adv_ctl, 0, sizeof(bt_hci_log_t));

    return ESP_OK;
}

#if (BT_HCI_LOG_PRINT_TAG)
static char IRAM_ATTR *bt_data_type_to_str(uint8_t data_type)
{
    char *tag = NULL;
    switch (data_type)
    {
    case HCI_LOG_DATA_TYPE_COMMAND:
        // hci cmd data
        tag = "CMD";
        break;
    case HCI_LOG_DATA_TYPE_H2C_ACL:
        // host to controller hci acl data
        tag = "HAL";
        break;
    case HCI_LOG_DATA_TYPE_SCO:
        // hci sco data
        tag = "SCO";
        break;
    case HCI_LOG_DATA_TYPE_EVENT:
        // hci event
        tag = "EVT";
        break;
    case HCI_LOG_DATA_TYPE_ADV:
        // controller adv report data
        tag = "ADV";
        break;
    case HCI_LOG_DATA_TYPE_C2H_ACL:
        // controller to host hci acl data
        tag = "CAL";
        break;
    case HCI_LOG_DATA_TYPE_SELF_DEFINE:
        // self-defining data
        tag = "ST";
        break;
    default:
        // unknown data type
        tag = "UK";
        break;
    }

    return tag;
}
#endif

void bt_hci_log_record_hex(bt_hci_log_t *p_hci_log_ctl, uint8_t *hex, uint8_t hex_len)
{
    uint8_t hci_log_char;
    uint8_t *g_hci_log_buffer;

    g_hci_log_buffer = p_hci_log_ctl->p_hci_log_buffer;

    while (hex_len--)
    {
        hci_log_char = ((*hex) >> 4);

        g_hci_log_buffer[p_hci_log_ctl->log_record_in] = s_hex_to_char_mapping [hci_log_char];

        if (++ p_hci_log_ctl->log_record_in >= p_hci_log_ctl->buf_size) {
            p_hci_log_ctl->log_record_in = 0;
        }
        if (p_hci_log_ctl->log_record_in == p_hci_log_ctl->log_record_out) {
            p_hci_log_ctl->overflow = true;
        }

        hci_log_char = ((*hex) & 0x0f);

        g_hci_log_buffer[p_hci_log_ctl->log_record_in] = s_hex_to_char_mapping [hci_log_char];

        if (++p_hci_log_ctl->log_record_in >= p_hci_log_ctl->buf_size) {
            p_hci_log_ctl->log_record_in = 0;
        }

        if (p_hci_log_ctl->log_record_in == p_hci_log_ctl->log_record_out) {
            p_hci_log_ctl->overflow = true;
        }

        g_hci_log_buffer[p_hci_log_ctl->log_record_in] = ' ';

        if (++ p_hci_log_ctl->log_record_in >= p_hci_log_ctl->buf_size) {
            p_hci_log_ctl->log_record_in = 0;
        }
        if (p_hci_log_ctl->log_record_in == p_hci_log_ctl->log_record_out) {
            p_hci_log_ctl->overflow = true;
        }

        ++ hex;
    }
}

void bt_hci_log_record_string(bt_hci_log_t *p_hci_log_ctl, char *string)
{
    uint8_t *g_hci_log_buffer;

    g_hci_log_buffer = p_hci_log_ctl->p_hci_log_buffer;

    while (*string != '\0') {
        g_hci_log_buffer[p_hci_log_ctl->log_record_in] = *string;
        ++string;

        if (++p_hci_log_ctl->log_record_in >= p_hci_log_ctl->buf_size) {
            p_hci_log_ctl->log_record_in = 0;
        }

        if (p_hci_log_ctl->log_record_in == p_hci_log_ctl->log_record_out) {
            p_hci_log_ctl->overflow = true;
        }
    }
}

esp_err_t IRAM_ATTR bt_hci_log_record_data(bt_hci_log_t *p_hci_log_ctl, char *str, uint8_t data_type, uint8_t *data, uint8_t data_len)
{
    osi_mutex_t mutex_lock;
    uint8_t *g_hci_log_buffer;

    if (!p_hci_log_ctl->p_hci_log_buffer) {
        return ESP_FAIL;
    }

    g_hci_log_buffer = p_hci_log_ctl->p_hci_log_buffer;

    if (!g_hci_log_buffer) {
        return ESP_FAIL;
    }

    mutex_lock = p_hci_log_ctl->mutex_lock;
    osi_mutex_lock(&mutex_lock, OSI_MUTEX_MAX_TIMEOUT);

#if (1)
    // Add hci data index
    bt_hci_log_record_hex(p_hci_log_ctl, &p_hci_log_ctl->index, 1);
#endif

#if (BT_HCI_LOG_PRINT_TAG)
    char *tag = NULL;
    tag = bt_data_type_to_str(data_type);

    if (tag) {
        bt_hci_log_record_string(p_hci_log_ctl, tag);

        g_hci_log_buffer[p_hci_log_ctl->log_record_in] = ':';

        if (++p_hci_log_ctl->log_record_in >= p_hci_log_ctl->buf_size) {
            p_hci_log_ctl->log_record_in = 0;
        }

        if (p_hci_log_ctl->log_record_in == p_hci_log_ctl->log_record_out) {
            p_hci_log_ctl->overflow = true;
        }
    }
#endif

    if (str) {
        bt_hci_log_record_string(p_hci_log_ctl, str);
    }

    bt_hci_log_record_hex(p_hci_log_ctl, data, data_len);

    g_hci_log_buffer[p_hci_log_ctl->log_record_in] = '\n';

    if (++p_hci_log_ctl->log_record_in >= p_hci_log_ctl->buf_size) {
        p_hci_log_ctl->log_record_in = 0;
    }

    if (p_hci_log_ctl->log_record_in == p_hci_log_ctl->log_record_out) {
        p_hci_log_ctl->overflow = true;
    }

    p_hci_log_ctl->index ++;

    osi_mutex_unlock(&mutex_lock);

    return ESP_OK;
}

void bt_hci_log_data_show(bt_hci_log_t *p_hci_log_ctl)
{
    volatile uint64_t log_record_in,log_record_out;
    uint8_t *g_hci_log_buffer;

    if (!p_hci_log_ctl->p_hci_log_buffer) {
        return;
    }

    osi_mutex_t mutex_lock = p_hci_log_ctl->mutex_lock;

    osi_mutex_lock(&mutex_lock, OSI_MUTEX_MAX_TIMEOUT);

    log_record_in  = p_hci_log_ctl->log_record_in;
    log_record_out = p_hci_log_ctl->log_record_out;

    g_hci_log_buffer = p_hci_log_ctl->p_hci_log_buffer;

    if (p_hci_log_ctl->overflow) {
        log_record_out = log_record_in;
        printf("%c",g_hci_log_buffer[log_record_out]);

        if (++log_record_out >= p_hci_log_ctl->buf_size) {
            log_record_out = 0;
        }
    }

    while (log_record_in != log_record_out)
    {
        printf("%c",g_hci_log_buffer[log_record_out]);

        if (++log_record_out >= p_hci_log_ctl->buf_size) {
            log_record_out = 0;
        }
    }

    p_hci_log_ctl->log_record_out = log_record_out;
    p_hci_log_ctl->overflow = false;

    osi_mutex_unlock(&mutex_lock);
}

esp_err_t IRAM_ATTR bt_hci_log_record_hci_data(uint8_t data_type, uint8_t *data, uint8_t data_len)
{
    return bt_hci_log_record_data(&g_bt_hci_log_data_ctl, NULL, data_type, data, data_len);
}

esp_err_t IRAM_ATTR bt_hci_log_record_custom_data(char *string, uint8_t *data, uint8_t data_len)
{
    return bt_hci_log_record_data(&g_bt_hci_log_data_ctl, string, HCI_LOG_DATA_TYPE_SELF_DEFINE, data, data_len);
}

esp_err_t IRAM_ATTR bt_hci_log_record_hci_adv(uint8_t data_type, uint8_t *data, uint8_t data_len)
{
    return bt_hci_log_record_data(&g_bt_hci_log_adv_ctl, NULL, data_type, data, data_len);
}

void bt_hci_log_hci_data_show(void)
{
    bt_hci_log_data_show(&g_bt_hci_log_data_ctl);
}

void bt_hci_log_hci_adv_show(void)
{
    bt_hci_log_data_show(&g_bt_hci_log_adv_ctl);
}

#endif // (BT_HCI_LOG_INCLUDED == TRUE)
