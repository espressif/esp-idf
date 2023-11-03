/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_BLUEDROID_HCI_H__
#define __ESP_BLUEDROID_HCI_H__

#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/* HCI driver callbacks */
typedef struct esp_bluedroid_hci_driver_callbacks {
    /**
     * @brief callback used to notify that the host can send packet to controller
     */
    void (*notify_host_send_available)(void);

    /**
     * @brief callback used to notify that the controller has a packet to send to the host
     *
     * @param[in] data  pointer to data buffer
     * @param[in] len   length of data
     *
     * @return 0 received successfully, failed otherwise
     */
    int (*notify_host_recv)(uint8_t *data, uint16_t len);
} esp_bluedroid_hci_driver_callbacks_t;

/* HCI driver operations */
typedef struct esp_bluedroid_hci_driver_operations {
    /**
     * @brief send data from host to controller
     *
     * @param[in] data  pointer to data buffer
     * @param[in] len   length of data
     */
    void (*send)(uint8_t *data, uint16_t len);

    /**
     * @brief host checks whether it can send data to controller
     *
     * @return true if host can send data, false otherwise
     */
    bool (*check_send_available)(void);

    /**
     * @brief register host callback
     *
     * @param[in] callback  HCI driver callbacks
     */
    esp_err_t (* register_host_callback)(const esp_bluedroid_hci_driver_callbacks_t *callback);
} esp_bluedroid_hci_driver_operations_t;

/**
 * @brief get the operations of HCI transport layer. This API should only be used in
 *        Bluedroid Host-only mode before Bluedroid initialization.
 *
 * @param[in] ops struct containing operations of HCI transport layer
 *
 * @return ESP_OK if get successfully, ESP_FAIL otherwise
 */
esp_err_t esp_bluedroid_attach_hci_driver(const esp_bluedroid_hci_driver_operations_t *ops);

/**
 * @brief remove the operations of HCI transport layer. This API should only be used in
 *        Bluedroid Host-only mode before Bluedroid initialization.
 *
 * @param[in] ops struct containing operations of HCI transport layer
 *
 * @return ESP_OK if get successfully, ESP_FAIL otherwise
 */
esp_err_t esp_bluedroid_detach_hci_driver(void);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_BLUEDROID_HCI_H__ */
