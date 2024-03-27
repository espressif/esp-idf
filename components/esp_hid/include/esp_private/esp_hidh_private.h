/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_hidh.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "sys/queue.h"
#include "esp_timer.h"
#if CONFIG_BT_NIMBLE_ENABLED
#include "nimble/ble.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief HIDH device report data
 */
typedef struct esp_hidh_dev_report_s {
    struct esp_hidh_dev_report_s *next;
    uint8_t map_index;      //the index of the report map
    uint8_t report_id;      //the id of the report
    uint8_t report_type;    //input, output or feature
    uint8_t protocol_mode;  //boot or report
    size_t value_len;       //maximum len of value by report map
    esp_hid_usage_t usage;  //generic, keyboard or mouse
    //BLE properties
    uint16_t handle;        //handle to the value
    uint16_t ccc_handle;    //handle to client config
    uint8_t permissions;    //report permissions
} esp_hidh_dev_report_t;

/**
 * @brief HIDH device data
 */
struct esp_hidh_dev_s {
    esp_hid_address_t       addr;
    esp_hid_device_config_t config;
    esp_hid_usage_t         usage;
    esp_hid_transport_t     transport;      //BT, BLE or USB
    esp_hid_trans_type_t    trans_type;     //indicate what transaction is going on, new transaction only be allowed after the previous done
    esp_timer_handle_t      trans_timer;    //transaction timer
    uint8_t                 report_type;    //Get_Report transaction report_type
    uint8_t                 report_id;      //Get_Report transaction report_id
#if CONFIG_BT_NIMBLE_ENABLED
    uint8_t                 *protocol_mode;  // protocol mode is unique for each hid service instance
#else
    uint8_t                 protocol_mode;  //device protocol mode
#endif
    bool                    connected;      //we have all required data to communicate
    bool                    opened;         //we opened the device manually, else the device connected to us
    bool                    added;          //If lower layer has added the device
    bool                    is_orig;        //If host initiate the connection
    bool                    in_use;         //If false, it will be deleted from the devices list.
    int                     status;         //status of the last command

    size_t                  reports_len;
    esp_hidh_dev_report_t   *reports;

    void                    *tmp;
    size_t                  tmp_len;

    SemaphoreHandle_t       semaphore;
    SemaphoreHandle_t       mutex;

    esp_err_t (*close)(esp_hidh_dev_t *dev);
    esp_err_t (*report_write)(esp_hidh_dev_t *dev, size_t map_index, size_t report_id, int report_type, uint8_t *data, size_t len);
    esp_err_t (*report_read)(esp_hidh_dev_t *dev, size_t map_index, size_t report_id, int report_type, size_t max_length, uint8_t *value, size_t *value_len);
    esp_err_t (*set_report)(esp_hidh_dev_t *dev, size_t map_index, size_t report_id, int report_type, uint8_t *data, size_t len);
    esp_err_t (*get_idle)(esp_hidh_dev_t *dev);
    esp_err_t (*set_idle)(esp_hidh_dev_t *dev, uint8_t idle_time);
    esp_err_t (*get_protocol)(esp_hidh_dev_t *dev);
    esp_err_t (*set_protocol)(esp_hidh_dev_t *dev, uint8_t protocol_mode);
    void (*dump)(esp_hidh_dev_t *dev, FILE *fp);

    union {
#if CONFIG_BT_HID_HOST_ENABLED
        struct {
            uint8_t handle;
        } bt;
#endif /* CONFIG_BT_HID_HOST_ENABLED */
#if CONFIG_GATTC_ENABLE
        struct {
            esp_ble_addr_type_t address_type;
            int conn_id;
            uint16_t appearance;
            uint16_t battery_handle;
            uint16_t battery_ccc_handle;
        } ble;
#endif /* CONFIG_GATTC_ENABLE */
#if CONFIG_BT_NIMBLE_ENABLED
        struct {
            uint8_t address_type;
            int conn_id;
            uint16_t appearance;
            uint16_t battery_handle;
            uint16_t battery_ccc_handle;
        } ble;
#endif
    };
    TAILQ_ENTRY(esp_hidh_dev_s) devices;
};

// ------------------------------------------------- Transport layer functions --------------------------------------------------

/**
 * @brief Get HIDH event loop
 *
 * Transport layers will post events into the loop
 *
 * @return esp_event_loop_handle_t Handle to HIDH event loop
 */
esp_event_loop_handle_t esp_hidh_get_event_loop(void);

/**
 * @brief Allocate HIDH device
 *
 * The resources can be freed by esp_hidh_dev_free_inner()
 *
 * @return esp_hidh_dev_t* Pointer to newly allocated HIDH device
 */
esp_hidh_dev_t *esp_hidh_dev_malloc(void);

/**
 * @brief Free HIDH device
 *
 * @param[in] dev Device handle obtained from esp_hidh_dev_malloc()
 * @return
 *     - ESP_OK:   Success
 *     - ESP_FAIL: Parameter is NULL or it is not a valid HIDH device
 */
esp_err_t esp_hidh_dev_free_inner(esp_hidh_dev_t *dev);

#if CONFIG_BLUEDROID_ENABLED
esp_hidh_dev_t *esp_hidh_dev_get_by_bda(esp_bd_addr_t bda); //BT/BLE
esp_hidh_dev_t *esp_hidh_dev_get_by_handle(uint8_t handle); //Classic Bluetooth Only
esp_hidh_dev_t *esp_hidh_dev_get_by_conn_id(uint16_t conn_id); //BLE Only
#endif /* CONFIG_BLUEDROID_ENABLED */
#if CONFIG_BT_NIMBLE_ENABLED
esp_hidh_dev_t *esp_hidh_dev_get_by_bda(uint8_t* bda); // BLE Only
esp_hidh_dev_t *esp_hidh_dev_get_by_conn_id(uint16_t conn_id); //BLE Only
#endif

esp_hidh_dev_report_t *esp_hidh_dev_get_report_by_id_type_proto(esp_hidh_dev_t *dev, size_t map_index, size_t report_id, int report_type, uint8_t protocol_mode);
esp_hidh_dev_report_t *esp_hidh_dev_get_report_by_id_and_type(esp_hidh_dev_t *dev, size_t map_index, size_t report_id, int report_type);
esp_hidh_dev_report_t *esp_hidh_dev_get_input_report_by_len_and_proto(esp_hidh_dev_t *dev, size_t len, int protocol_mode);
esp_hidh_dev_report_t *esp_hidh_dev_get_input_report_by_id_and_proto(esp_hidh_dev_t *dev, size_t report_id, int protocol_mode);
esp_hidh_dev_report_t *esp_hidh_dev_get_input_report_by_proto_and_data(esp_hidh_dev_t *dev, int protocol_mode,
                                                                       size_t len, const uint8_t *data, bool *has_report_id);
esp_hidh_dev_report_t *esp_hidh_dev_get_report_by_handle(esp_hidh_dev_t *dev, uint16_t handle);  //BLE Only
void esp_hidh_preprocess_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id,
                                       void *event_data);
void esp_hidh_postprocess_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id,
                                        void *event_data);
void esp_hidh_dev_lock(esp_hidh_dev_t *dev);
void esp_hidh_dev_unlock(esp_hidh_dev_t *dev);
void esp_hidh_dev_wait(esp_hidh_dev_t *dev);
void esp_hidh_dev_send(esp_hidh_dev_t *dev);
#ifdef __cplusplus
}
#endif
