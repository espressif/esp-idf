/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BLE_MESH_CONSOLE_TRANSACTION_H_
#define _BLE_MESH_CONSOLE_TRANSACTION_H_

/* In esp-idf, bluetooth and wifi stack APIs are async (using callbacks).
 * transaction module provides a common method to let user transfer async APIs to sync transactions.
 */

#include "sdkconfig.h"
#include "esp_err.h"
#include "freertos/event_groups.h"

#define MAX_TRANSACTION_COUNT           5

#define TRANSACTION_TYPE_ALL            0xFF

#define TRANSACTION_SUB_TYPE_ALL        0xFFFFFFFF

// The higher 12 bits of event is reversed for event group or transaction module.
// Application can only use lower 20 bits of the event.
#define TRANSACTION_TIMEOUT_EVENT           0x00100000UL  // only wait for timeout
#define TRANSACTION_ABORT_EVENT             0x00800000UL  // this event is reserved for internal use only
#define TRANSACTION_EVENT_MASK              0xFFF00000UL


enum {
    TRANS_RET_ERROR_START = 0x10000,
    TRANS_RET_FAILED_TO_ALLOCATE,
    TRANS_RET_STATE_ERR,
    TRANS_RET_TEST_EVENT_FAILED,
    TRANS_RET_EVENTS_CONFLICT,    // events bit conflicts with TRANSACTION_ABORT_EVENT
    TRANS_RET_INVALID_TRNSACTION,
    TRANS_RET_TIMEOUT,
    TRANS_RET_ABORTED
};

typedef enum {
    TRANSACTION_INACTIVE,
    TRANSACTION_ACTIVE,
} transaction_state_t;

typedef struct {
    /* input data passed to callback */
    void                *input;
    /* output data from callback */
    void                *output;
    /* retrun value, can be set by callback or transaction module (like timeout, or other common errors)
     * transaction_run will return this member by default. */
    esp_err_t           ret;
    // private member
    transaction_state_t state;
    uint8_t             type;
    uint32_t            sub_type;
    uint32_t            timeout;
    EventBits_t         wait_events;
    EventGroupHandle_t  event_group;
    EventBits_t         current_bits;
} transaction_t;


void init_transactions(void);

esp_err_t transaction_init(transaction_t **trans, uint8_t type, uint32_t sub_type, EventBits_t wait_events, uint32_t timeout, void *input, void *output);
esp_err_t transaction_run(transaction_t *trans);

transaction_t *transaction_get(uint8_t type, uint32_t sub_type, transaction_t *start);

esp_err_t transaction_set_events(transaction_t *trans, EventBits_t events);
esp_err_t transaction_test_events(transaction_t *trans, EventBits_t events);
esp_err_t transaction_clear_events(transaction_t *trans, EventBits_t events);

esp_err_t transaction_abort(transaction_t *trans, esp_err_t reason);

#define TRANSACTION_INIT(trans, type, sub_type, wait_events, timeout, input, output) \
    ESP_ERROR_CHECK(transaction_init(trans, type, sub_type, wait_events, timeout, input, output))


#define transaction_get_first(type, sub_type) transaction_get(type, sub_type, NULL)


/* We declare all transaction type / sub type below */
#define TRANS_TYPE_BLE_GAP        0x01
#define TRANS_TYPE_BLE_GATTC      0x02
#define TRANS_TYPE_BLE_GATTS      0x03
#define TRANS_TYPE_WIFI           0x04
#define TRANS_TYPE_BT             0x05


#endif /* _BLE_MESH_CONSOLE_TRANSACTION_H_ */
