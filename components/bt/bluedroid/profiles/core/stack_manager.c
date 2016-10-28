#include <stdbool.h>
#include <stdint.h>
#include "bt_stack_manager.h"
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
static bt_callbacks_t *bt_hal_cbacks = NULL;
static future_t *hack_future = NULL;

static bt_status_t event_init_stack(bt_callbacks_t *cb);
static bt_status_t event_start_up_stack(void);
static bt_status_t event_shut_down_stack(void);
static bt_status_t event_clean_up_stack(void);
static void event_signal_stack_up(UNUSED_ATTR uint16_t event, UNUSED_ATTR char *p_param);
static void event_signal_stack_down(UNUSED_ATTR uint16_t event, UNUSED_ATTR char *p_param);

static bt_status_t event_init_stack(bt_callbacks_t *cb)
{
    bt_status_t ret;
    if (!stack_is_initialized) {
        hack_future = future_new();
        ret = btif_init_bluetooth();
        if (future_await(hack_future) != FUTURE_SUCCESS) {
            return BT_STATUS_FAIL;
        }
        if (ret == BT_STATUS_SUCCESS) {
            bt_hal_cbacks = cb;            
            stack_is_initialized = true;
        }
        return ret;
    }
    else {
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
    btif_transfer_context(event_signal_stack_up, 0, NULL, 0, NULL);
    return BT_STATUS_SUCCESS;
}

static bt_status_t event_shut_down_stack(void)
{
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
    btif_transfer_context(event_signal_stack_down, 0, NULL, 0, NULL);
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

static void event_signal_stack_up(UNUSED_ATTR uint16_t event, UNUSED_ATTR char *p_param)
{
    HAL_CBACK(bt_hal_cbacks, adapter_state_changed_cb, BT_STATE_ON);
}

static void event_signal_stack_down(UNUSED_ATTR uint16_t event, UNUSED_ATTR char *p_param)
{
    HAL_CBACK(bt_hal_cbacks, adapter_state_changed_cb, BT_STATE_OFF);
}

bt_status_t API_BTDM_InitStack(bt_callbacks_t *cb)
{
    return event_init_stack(cb);
}

bt_status_t API_BTDM_EnableStack(void)
{
    return event_start_up_stack();
}

bt_status_t API_BTDM_DisableStack(void)
{
    return event_shut_down_stack();
}

bt_status_t API_BTDM_CleanUpStack(void)
{
    return event_clean_up_stack();
}

bool stack_manager_is_stack_running(void)
{
    return stack_is_running;
}

future_t *stack_manager_get_hack_future(void)
{
    return hack_future;
}
