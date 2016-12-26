#include <stdbool.h>
#include <stdint.h>
#include "esp_bt_stack_manager.h"
#include "stack_manager.h"
#include "bt_defs.h"
#include "bt_trace.h"
#include "future.h"
#include "btif_common.h"
#include "btif_api.h"
#include "btif_dm.h"

/************************************************************************************
**  Constants & Macros
************************************************************************************/
/************************************************************************************
**  Local type definitions
************************************************************************************/
/************************************************************************************
**  Static variables
************************************************************************************/
static bool stack_is_initialized = false;
static bool stack_is_running = false;
static future_t *hack_future = NULL;

static bt_status_t event_init_stack(void);
static bt_status_t event_start_up_stack(void);
static bt_status_t event_shut_down_stack(void);
static bt_status_t event_clean_up_stack(void);

static bt_status_t event_init_stack(void)
{
    bt_status_t ret;
    if (!stack_is_initialized) {
        hack_future = future_new();
        ret = btif_init_bluetooth();
        if (future_await(hack_future) != FUTURE_SUCCESS) {
            return BT_STATUS_FAIL;
        }
        if (ret == BT_STATUS_SUCCESS) {
            stack_is_initialized = true;
        }
        return ret;
    } else {
        return BT_STATUS_DONE;
    }
}

static bt_status_t event_start_up_stack(void)
{
    if (!stack_is_initialized) {
        LOG_DEBUG("%s stack not initialized yet.\n", __func__);
        return BT_STATUS_NOT_READY;
    }

    if (stack_is_running) {
        LOG_DEBUG("%s stack already brought up.\n", __func__);
        return BT_STATUS_DONE;
    }

    LOG_DEBUG("%s is bringing up the stack.\n", __func__);
    hack_future = future_new();

    btif_enable_bluetooth();

    if (future_await(hack_future) != FUTURE_SUCCESS) {
        stack_is_running = true; // So stack shutdown actually happens
        event_shut_down_stack();
        return BT_STATUS_FAIL;
    }

    stack_is_running = true;
    LOG_DEBUG("%s finished\n", __func__);
    return BT_STATUS_SUCCESS;
}

static bt_status_t event_shut_down_stack(void)
{
    if (!stack_is_initialized) {
        LOG_DEBUG("%s stack not initialized yet.\n", __func__);
        return BT_STATUS_NOT_READY;
    }
	
    if (!stack_is_running) {
        LOG_DEBUG("%s stack is already brought down.\n", __func__);
        return BT_STATUS_DONE;
    }

    LOG_DEBUG("%s is bringing down the stack.\n", __func__);
    hack_future = future_new();
    stack_is_running = false;

    btif_disable_bluetooth();

    future_await(hack_future);

    LOG_DEBUG("%s finished.\n", __func__);
    return BT_STATUS_SUCCESS;
}

static bt_status_t event_clean_up_stack(void)
{
    if (!stack_is_initialized) {
        LOG_DEBUG("%s found the stack already in a clean state.\n", __func__);
        return BT_STATUS_DONE;
    }

    if (stack_is_running) {
        event_shut_down_stack();
    }

    LOG_DEBUG("%s is cleaning up the stack.\n", __func__);

    stack_is_initialized = false;

    btif_shutdown_bluetooth();

    return BT_STATUS_SUCCESS;
}

esp_err_t esp_bt_init_stack(void)
{
    bt_status_t status;
    status = event_init_stack();
    switch (status) {
    case BT_STATUS_SUCCESS: return ESP_OK;
    case BT_STATUS_DONE: return ESP_ERR_INVALID_STATE;
    default: return ESP_FAIL;
    }
}

esp_err_t esp_bt_deinit_stack(void)
{
    bt_status_t status;
    status = event_clean_up_stack();
    switch (status) {
    case BT_STATUS_SUCCESS: return ESP_OK;
    default: return ESP_ERR_INVALID_STATE;
    }
}

esp_err_t esp_bt_enable_stack(void)
{
    bt_status_t status;
    status = event_start_up_stack();
    switch (status) {
    case BT_STATUS_SUCCESS: return ESP_OK;
    case BT_STATUS_NOT_READY:
    case BT_STATUS_DONE:
        return ESP_ERR_INVALID_STATE;
    default: return ESP_FAIL;
    }
}

esp_err_t esp_bt_disable_stack(void)
{
    bt_status_t status;
    status = event_shut_down_stack();
    switch (status) {
    case BT_STATUS_SUCCESS: return ESP_OK;
    case BT_STATUS_NOT_READY:
    case BT_STATUS_DONE:
        return ESP_ERR_INVALID_STATE;
    default: return ESP_FAIL;
    }
}

bool stack_manager_is_stack_running(void)
{
    return stack_is_running;
}

future_t *stack_manager_get_hack_future(void)
{
    return hack_future;
}
