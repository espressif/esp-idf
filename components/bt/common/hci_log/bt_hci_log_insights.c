/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"

#include "esp_timer.h"
#include "bt_ble_insights.h"
#include "bt_common.h"
#include "hci_log/bt_hci_log.h"

#if (BT_HCI_LOG_INCLUDED == TRUE) && BT_HCI_INSIGHTS_INCLUDED
static uint8_t s_hci_log_seq_num = 0;
static portMUX_TYPE s_hci_log_mux = portMUX_INITIALIZER_UNLOCKED;

#define HCI_LOG_INSIGHTS_LINE_SIZE 128U
#define HCI_LOG_INSIGHTS_TS_LEN 8U

static const char *bt_hci_log_insights_label(uint8_t data_type)
{
    switch (data_type) {
    case HCI_LOG_DATA_TYPE_COMMAND:
        return "C";
    case HCI_LOG_DATA_TYPE_H2C_ACL:
        return "H";
    case HCI_LOG_DATA_TYPE_SCO:
        return "S";
    case HCI_LOG_DATA_TYPE_EVENT:
        return "E";
    case HCI_LOG_DATA_TYPE_ADV:
        return "ADV";
    case HCI_LOG_DATA_TYPE_C2H_ACL:
        return "D";
    case HCI_LOG_DATA_TYPE_ISO_DATA:
        return "I";
    default:
        return NULL;
    }
}

void bt_hci_log_record_insights(uint8_t data_type, const uint8_t *data, uint16_t data_len)
{
    const char *label = bt_hci_log_insights_label(data_type);
    char line[HCI_LOG_INSIGHTS_LINE_SIZE];
    uint8_t ts_bytes[HCI_LOG_INSIGHTS_TS_LEN];
    uint64_t timestamp;
    uint8_t seq_num;
    int offset;

    if (!BT_BLE_INSIGHTS_AVAILABLE || label == NULL || data == NULL || data_len == 0) {
        return;
    }

    portENTER_CRITICAL(&s_hci_log_mux);
    seq_num = ++s_hci_log_seq_num;
    portEXIT_CRITICAL(&s_hci_log_mux);

    timestamp = esp_timer_get_time();
    memcpy(ts_bytes, &timestamp, sizeof(ts_bytes));

    offset = snprintf(line, sizeof(line), "%02x %s:", (unsigned int)seq_num, label);
    if (offset < 0 || (size_t)offset >= sizeof(line) - 1) {
        return;
    }

    for (size_t i = 0; i < sizeof(ts_bytes) && offset <= (int)sizeof(line) - 3; i++) {
        offset += snprintf(&line[offset], sizeof(line) - offset, "%02x", ts_bytes[i]);
    }

    if (offset < 0 || (size_t)offset >= sizeof(line) - 1) {
        return;
    }

    line[offset++] = ' ';
    line[offset] = '\0';

    for (uint16_t i = 0; i < data_len && offset <= (int)sizeof(line) - 3; i++) {
        offset += snprintf(&line[offset], sizeof(line) - offset, "%02X", data[i]);
    }

    ble_insights_log(line);
}
#endif
