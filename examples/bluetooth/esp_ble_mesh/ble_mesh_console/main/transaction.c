#include <stdio.h>

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_err.h"
#include "transaction.h"
#define TAG    "TRANS"

static transaction_t transactions[MAX_TRANSACTION_COUNT];
static SemaphoreHandle_t trans_mutex;

static uint32_t utils_get_system_ts(void)
{
    return esp_log_timestamp();
}

static void transaction_reset(transaction_t *trans)
{
    EventBits_t bits;

    ESP_LOGV(TAG, "transaction reset: %x", (uint32_t) trans);
    // set to inactive state and clear all bits of the transaction
    xSemaphoreTakeRecursive(trans_mutex, portMAX_DELAY);
    trans->type = 0;
    trans->sub_type = 0;
    trans->current_bits = 0;
    bits = xEventGroupGetBits(trans->event_group);
    xEventGroupClearBits(trans->event_group, bits);
    trans->state = TRANSACTION_INACTIVE;
    trans->ret = ESP_OK;
    xSemaphoreGiveRecursive(trans_mutex);
}

void transaction_deinit(transaction_t *trans)
{
    if (trans != NULL) {
        transaction_reset(trans);
    }
}

esp_err_t transaction_set_events(transaction_t *trans, EventBits_t events)
{
    esp_err_t ret = ESP_OK;
    xSemaphoreTakeRecursive(trans_mutex, portMAX_DELAY);
    if (trans) {
        if (trans->state == TRANSACTION_INACTIVE) {
            ret = TRANS_RET_STATE_ERR;
        } else {
            // if the task (task A) setting current bits is with higher priority than the task (task B) run transaction,
            // current_bits might not be updated until task A yield (not it's only update in run_transaction function).
            // If task A set events and immediately use current_bits, current_bits is not correct.
            // update current_bits here to make sure it's updated
            trans->current_bits |= events;
            xEventGroupSetBits(trans->event_group, events);
        }
        ESP_LOGD(TAG, "transactions set events: %x, %x, %x, %x; ret: %x", (uint32_t) trans, trans->type, trans->sub_type, events, ret);
    } else {
        ret = TRANS_RET_INVALID_TRNSACTION;
    }
    xSemaphoreGiveRecursive(trans_mutex);
    return ret;
}

esp_err_t transaction_test_events(transaction_t *trans, EventBits_t events)
{
    esp_err_t ret = TRANS_RET_TEST_EVENT_FAILED;

    xSemaphoreTakeRecursive(trans_mutex, portMAX_DELAY);
    if (trans) {
        if (trans->state == TRANSACTION_INACTIVE) {
            ret = TRANS_RET_STATE_ERR;
        } else {
            if ((trans->current_bits & events) == events) {
                ret = ESP_OK;
            }
        }
        ESP_LOGV(TAG, "transactions test events: %x, %x; ret: %x", (uint32_t) trans, events, ret);
    } else {
        ret = TRANS_RET_INVALID_TRNSACTION;
    }
    xSemaphoreGiveRecursive(trans_mutex);
    return ret;
}

esp_err_t transaction_clear_events(transaction_t *trans, EventBits_t events)
{
    esp_err_t ret = ESP_OK;
    xSemaphoreTakeRecursive(trans_mutex, portMAX_DELAY);
    if (trans) {
        if (trans->state == TRANSACTION_INACTIVE) {
            ret = TRANS_RET_STATE_ERR;
        } else {
            trans->current_bits &= ~events;
            xEventGroupClearBits(trans->event_group, events);
        }
        ESP_LOGD(TAG, "transactions clear events: %x, %x, %x, %x; ret: %x", (uint32_t) trans, trans->type, trans->sub_type, events, ret);
    } else {
        ret = TRANS_RET_INVALID_TRNSACTION;
    }
    xSemaphoreGiveRecursive(trans_mutex);
    return ret;
}

esp_err_t transaction_abort(transaction_t *trans, esp_err_t reason)
{
    esp_err_t ret = ESP_OK;
    xSemaphoreTakeRecursive(trans_mutex, portMAX_DELAY);
    if (trans) {
        if (trans->state == TRANSACTION_INACTIVE) {
            ret = TRANS_RET_STATE_ERR;
        } else {
            trans->ret = reason;
            xEventGroupSetBits(trans->event_group, TRANSACTION_ABORT_EVENT);
        }
        ESP_LOGD(TAG, "transactions abort: %x, %x, %x, %x; ret: %x", (uint32_t) trans, trans->type, trans->sub_type, reason, ret);
    } else {
        ret = TRANS_RET_INVALID_TRNSACTION;
    }
    xSemaphoreGiveRecursive(trans_mutex);
    return ret;
}

esp_err_t transaction_init(transaction_t **trans, uint8_t type, uint32_t sub_type, EventBits_t wait_events, uint32_t timeout, void *input, void *output)
{
    esp_err_t ret = ESP_OK;
    uint8_t i;

    if ((wait_events & TRANSACTION_EVENT_MASK)
            && wait_events != TRANSACTION_TIMEOUT_EVENT) {
        ret = TRANS_RET_EVENTS_CONFLICT;
        return ret;
    }

    xSemaphoreTakeRecursive(trans_mutex, portMAX_DELAY);
    for (i = 0; i < MAX_TRANSACTION_COUNT; i++) {
        if ( transactions[i].state == TRANSACTION_INACTIVE ) {
            transactions[i].state = TRANSACTION_ACTIVE;
            *trans = &transactions[i];
            break;
        }
    }

    if ( i == MAX_TRANSACTION_COUNT ) {
        ret = TRANS_RET_FAILED_TO_ALLOCATE;
    } else {
        // init transaction
        transactions[i].type = type;
        transactions[i].wait_events = wait_events;
        transactions[i].sub_type = sub_type;
        transactions[i].timeout = timeout;
        transactions[i].ret = ESP_OK;
        transactions[i].input = input;
        transactions[i].output = output;
    }
    xSemaphoreGiveRecursive(trans_mutex);

    if (ret == ESP_OK) {
        ESP_LOGD(TAG, "transaction created: %x, %x, %x; ret: %x", type, sub_type, (uint32_t) *trans, ret);
    }
    return ret;
}

esp_err_t transaction_run(transaction_t *trans)
{
    esp_err_t ret = ESP_OK;
    uint32_t start_time;
    int32_t wait_time;
    EventBits_t current_bits;

    if (trans) {
        start_time = utils_get_system_ts();
        // wait for wait events
        while (1) {
            //TODO: we didn't handle ts overflow
            wait_time = start_time + trans->timeout - utils_get_system_ts();

            if ( wait_time < 0 ) {
                ESP_LOGI(TAG, "transaction timeout: %x, %x, %x, %x, %x", (uint32_t) trans, trans->type, trans->sub_type, trans->wait_events, trans->current_bits);
                ret = TRANS_RET_TIMEOUT;
                break;
            }

            // trans->event_group and trans->wait_events will not be changed once trans is created, so we don't need protect them
            current_bits = xEventGroupWaitBits(trans->event_group, trans->wait_events | TRANSACTION_ABORT_EVENT,
                    1, 0, wait_time/portTICK_RATE_MS);

            xSemaphoreTakeRecursive(trans_mutex, portMAX_DELAY);
            trans->current_bits |= current_bits;

            if (trans->current_bits == trans->wait_events) {
                // wait succeeded, we copy the trans->ret as ret of run transaction. This value could be changed by
                ret = trans->ret;
                xSemaphoreGiveRecursive(trans_mutex);
                break;
            } else if ( trans->current_bits & TRANSACTION_ABORT_EVENT ) {
                if ( trans->ret ) {
                    // copy user defined ret value if it's set
                    ret = trans->ret;
                } else {
                    ret = TRANS_RET_ABORTED;
                }
                xSemaphoreGiveRecursive(trans_mutex);
                break;
            }
            xSemaphoreGiveRecursive(trans_mutex);
        }
        ESP_LOGD(TAG, "transaction run: %x, %x, %x; ret: %x", (uint32_t) trans, trans->type, trans->sub_type, ret);
        // reset after it's finished
        transaction_reset(trans);
    } else {
        ESP_LOGD(TAG, "transaction run: %x; ret: %x", (uint32_t) trans, ret);
        ret = TRANS_RET_INVALID_TRNSACTION;
    }

    return ret;
}

transaction_t *transaction_get(uint8_t type, uint32_t sub_type, transaction_t *start)
{
    uint8_t i, start_index;
    transaction_t *trans = NULL;

    if ( start == NULL ) {
        start_index = 0;
    } else {
        start_index = (start - transactions) + 1;
    }

    xSemaphoreTakeRecursive(trans_mutex, portMAX_DELAY);
    for (i = start_index; i < MAX_TRANSACTION_COUNT; i++) {
        if ( transactions[i].state == TRANSACTION_ACTIVE ) {
            if ( (transactions[i].type == type) && (transactions[i].sub_type & sub_type) ) {
                trans = &transactions[i];
                break;
            }
        }
    }
    xSemaphoreGiveRecursive(trans_mutex);
    ESP_LOGV(TAG, "transaction get: %x, %x, %x, %x", type, sub_type, (uint32_t) start, (uint32_t) trans);
    return trans;
}

void init_transactions(void)
{
    uint8_t i;

    ESP_LOGI(TAG, "init transactions");

    trans_mutex = xSemaphoreCreateRecursiveMutex();

    for (i = 0; i < MAX_TRANSACTION_COUNT; i++) {
        transactions[i].event_group = xEventGroupCreate();
        transaction_reset(&transactions[i]);
    }
}
