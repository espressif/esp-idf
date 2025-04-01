/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_BT_HCI_LOG_H__
#define __ESP_BT_HCI_LOG_H__

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif


#define    HCI_LOG_DATA_TYPE_COMMAND         (1)
#define    HCI_LOG_DATA_TYPE_H2C_ACL         (2)
#define    HCI_LOG_DATA_TYPE_SCO             (3)
#define    HCI_LOG_DATA_TYPE_EVENT           (4)
#define    HCI_LOG_DATA_TYPE_ADV             (5)
#define    HCI_LOG_DATA_TYPE_SELF_DEFINE     (6)
#define    HCI_LOG_DATA_TYPE_C2H_ACL         (7)
#define    HCI_LOG_DATA_TYPE_ISO_DATA        (8)

/**
 *
 * @brief            This function is called to record self-defining data
 * @param string :   data identification
 * @param data :     data
 * @param data_len : the length of data
 *
 * @return          ESP_OK - success, other - failed
 *
 */
esp_err_t bt_hci_log_record_custom_data(char *string, uint8_t *data, uint8_t data_len);
/**
 *
 * @brief           This function is called to print all hci data record
 *
 *
 * @return          None
 *
 */
void bt_hci_log_hci_data_show(void);

/**
 *
 * @brief           This function is called to print all adv report
 *
 *
 * @return          None
 *
 */
void bt_hci_log_hci_adv_show(void);

/**
 *
 * @brief           This function is called to init hci log env
 *
 *
 * @return          ESP_OK - success, other - failed
 *
 */
esp_err_t bt_hci_log_init(void);

/**
 *
 * @brief           This function is called to deinit hci debug mode,
 *                  and can only be called internally by Bluetooth
 *
 *
 * @return          ESP_OK - success, other - failed
 *
 */
esp_err_t bt_hci_log_deinit(void);

/**
 *
 * @brief           This function is called to record hci data without adv report event,
 *                  and can only be called internally by Bluetooth
 *
 * @param str :     data type, define in bt_data_type_to_str()
 * @param data :    data
 * @param data_len : the length of data
 *
 * @return          ESP_OK - success, other - failed
 *
 */
esp_err_t bt_hci_log_record_hci_data(uint8_t data_type, uint8_t *data, uint8_t data_len);

/**
 *
 * @brief           This function is called to record hci adv report event only
 *                  and can only be called internally by Bluetooth
 *
 * @param str :     data type, define in bt_data_type_to_str()
 * @param data :    data
 * @param data_len : the length of data
 * @return          ESP_OK - success, other - failed
 *
 */
esp_err_t bt_hci_log_record_hci_adv(uint8_t data_type, uint8_t *data, uint8_t data_len);

#ifdef __cplusplus
}
#endif

#endif /* _ESP_BT_HCI_LOG_H__ */
