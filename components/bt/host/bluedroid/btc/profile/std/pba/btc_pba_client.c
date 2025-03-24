/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "osi/allocator.h"

#include "bta/bta_api.h"
#include "bta/bta_pba_defs.h"
#include "bta/bta_pba_client_api.h"
#include "btc/btc_profile_queue.h"
#include "btc/btc_manage.h"
#include "btc/btc_task.h"
#include "btc_pba_client.h"
#include "esp_pbac_api.h"

#if BTC_PBA_CLIENT_INCLUDED

#define BTC_PBA_CLIENT_SECURITY     (BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT)

static bool s_btc_pba_client_init = 0;

btc_pba_client_cb_t btc_pba_client_cb;

static void bte_pba_client_evt(tBTA_PBA_CLIENT_EVT event, tBTA_PBA_CLIENT *p_data)
{
    bt_status_t status;
    int param_len = 0;
    bool ignore = false;

    switch (event) {
    case BTA_PBA_CLIENT_CONN_OPEN_EVT:
    case BTA_PBA_CLIENT_CONN_CLOSE_EVT:
        param_len = sizeof(tBTA_PBA_CLIENT_CONN);
        break;
    case BTA_PBA_CLIENT_PULL_PHONE_BOOK_RSP_EVT:
    case BTA_PBA_CLIENT_SET_PHONE_BOOK_RSP_EVT:
    case BTA_PBA_CLIENT_PULL_VCARD_LISTING_RSP_EVT:
    case BTA_PBA_CLIENT_PULL_VCARD_ENTRY_RSP_EVT:
        param_len = sizeof(tBTA_PBA_CLIENT_RESPONSE);
        break;
    case BTA_PBA_CLIENT_REGISTER_EVT:
        param_len = 0;
        break;
    case BTA_PBA_CLIENT_DISABLE_EVT:
        param_len = 0;
        break;
    case BTA_PBA_CLIENT_ENABLE_EVT:
    case BTA_PBA_CLIENT_DEREGISTER_EVT:
    default:
        ignore = true;
        break;
    }

    if (ignore) {
        return;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_PBA_CLIENT;
    msg.act = event;

    status = btc_transfer_context(&msg, p_data, param_len, NULL, NULL);
    if (status != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("context transfer failed");
    }
}

static void btc_pba_client_callback_to_app(esp_pbac_event_t event, esp_pbac_param_t *param)
{
    esp_pbac_callback_t callback = (esp_pbac_callback_t)btc_profile_cb_get(BTC_PID_PBA_CLIENT);
    if (callback) {
        callback(event, param);
    }
}

static void btc_pba_client_init(void)
{
    if (!s_btc_pba_client_init) {
        s_btc_pba_client_init = true;
        memset(&btc_pba_client_cb, 0, sizeof(btc_pba_client_cb_t));
        /* enable pba client */
        BTA_PbaClientEnable(bte_pba_client_evt);
        /* register sdp record */
        BTA_PbaClientRegister("Phonebook Access PCE");
    }
}

static void btc_pba_client_deinit(void)
{
    if (s_btc_pba_client_init) {
        s_btc_pba_client_init = false;
        /* deregister sdp record */
        BTA_PbaClientDeregister();
        /* disable pba client */
        BTA_PbaClientDisable();
    }
}

static BOOLEAN is_connected(bt_bdaddr_t *bd_addr)
{
    for (int i = 0; i < BTC_PBA_CLIENT_MAX_CONN_NUM; ++i) {
        if (btc_pba_client_cb.ccb[i].handle != 0 && bdcmp(bd_addr->address, btc_pba_client_cb.ccb[i].bd_addr.address) == 0) {
            return TRUE;
        }
    }

    return FALSE;
}

static bt_status_t connect_int(bt_bdaddr_t *bd_addr, uint16_t uuid)
{
    if (is_connected(bd_addr)) {
        return BT_STATUS_BUSY;
    }

    BTA_PbaClientOpen(bd_addr->address, BTC_PBA_CLIENT_SECURITY, (uint32_t)BTC_PBA_SUPPORTED_FEAT, (uint16_t)BTC_PBA_PREFERRED_MTU);

    return BT_STATUS_SUCCESS;
}

static void btc_pba_client_connect(bt_bdaddr_t *bd_addr)
{
    if (!s_btc_pba_client_init) {
        return;
    }

    btc_queue_connect(UUID_SERVCLASS_PBAP_PCE, bd_addr, connect_int);
}

static void btc_pba_client_disconnect(uint16_t handle)
{
    do {
        if (!s_btc_pba_client_init) {
            break;
        }

        if (handle == 0 || handle > BTC_PBA_CLIENT_MAX_CONN_NUM) {
            /* invalid handle value */
            break;
        }

        btc_pba_client_ccb_t *p_ccb = &btc_pba_client_cb.ccb[handle - 1];
        if (p_ccb->handle != handle) {
            /* not connect */
            break;
        }

        BTA_PbaClientClose(handle);
    } while (0);
}

static bool btc_pba_client_pull_phone_book(uint16_t handle, char *name, bool include_app_param,  esp_pbac_pull_phone_book_app_param_t *app_param)
{
    bt_status_t err = BT_STATUS_FAIL;
    uint8_t *app_param_buff = NULL;
    uint16_t app_param_len = 0;

    do {
        if (!s_btc_pba_client_init) {
            /* pba client not init */
            err = BT_STATUS_NOT_READY;
            break;
        }

        if (handle == 0 || handle > BTC_PBA_CLIENT_MAX_CONN_NUM) {
            /* invalid handle value */
            err = BT_STATUS_PARM_INVALID;
            break;
        }

        btc_pba_client_ccb_t *p_ccb = &btc_pba_client_cb.ccb[handle - 1];
        if (p_ccb->handle != handle) {
            /* not connect */
            err = BT_STATUS_PARM_INVALID;
            break;
        }

        if (p_ccb->busy) {
            /* busy */
            err = BT_STATUS_BUSY;
            break;
        }

        if (include_app_param) {
            app_param_buff = osi_malloc(BTA_PBAP_PULL_PHONE_BOOK_APP_PARAM_BUFF_SIZE_MIN);
            if (app_param_buff == NULL) {
                err = BT_STATUS_NOMEM;
                break;
            }
            uint8_t *p = app_param_buff;
            if (app_param->include_property_selector) {
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_PROPERTY_SELECTOR);
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_LENGTH_PROPERTY_SELECTOR);
                UINT64_TO_BE_STREAM(p, app_param->property_selector);
            }
            if (app_param->include_format) {
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_FORMAT);
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_LENGTH_FORMAT);
                UINT8_TO_BE_STREAM(p, app_param->format);
            }
            if (app_param->include_max_list_count) {
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_MAX_LIST_COUNT);
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_LENGTH_MAX_LIST_COUNT);
                UINT16_TO_BE_STREAM(p, app_param->max_list_count);
            }
            if (app_param->include_list_start_offset) {
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_LIST_START_OFFSET);
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_LENGTH_LIST_START_OFFSET);
                UINT16_TO_BE_STREAM(p, app_param->list_start_offset);
            }
            if (app_param->include_reset_new_missed_calls) {
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_RESET_NEW_MISSED_CALLS);
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_LENGTH_RESET_NEW_MISSED_CALLS);
                UINT8_TO_BE_STREAM(p, app_param->reset_new_missed_calls);
            }
            if (app_param->include_vcard_selector) {
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_VCARD_SELECTOR);
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_LENGTH_VCARD_SELECTOR);
                UINT64_TO_BE_STREAM(p, app_param->vcard_selector);
            }
            if (app_param->include_vcard_selector_operator) {
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_VCARD_SELECTOR_OPERATOR);
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_LENGTH_VCARD_SELECTOR_OPERATOR);
                UINT8_TO_BE_STREAM(p, app_param->vcard_selector_operator);
            }
            app_param_len = p - app_param_buff;
            assert(app_param_len <= BTA_PBAP_PULL_PHONE_BOOK_APP_PARAM_BUFF_SIZE_MIN);
            if (app_param_len == 0) {
                /* user give us an empty app param, allow but not recommend */
                osi_free(app_param_buff);
                app_param_buff = NULL;
            }
        }

        p_ccb->busy = true;
        BTA_PbaClientPullPhoneBook(handle, name, app_param_buff, app_param_len);
        err = BT_STATUS_SUCCESS;
    } while (0);

    if (err != BT_STATUS_SUCCESS) {
        BTC_TRACE_WARNING("%s failed, handle: %d, reason: %d", __FUNCTION__, handle, err);
        return false;
    }

    return true;
}

static bool btc_pba_client_set_phone_book(uint16_t handle, uint8_t flags, char *name)
{
    bt_status_t err = BT_STATUS_FAIL;

    do {
        if (!s_btc_pba_client_init) {
            /* pba client not init */
            err = BT_STATUS_NOT_READY;
            break;
        }

        if (handle == 0 || handle > BTC_PBA_CLIENT_MAX_CONN_NUM) {
            /* invalid handle value */
            err = BT_STATUS_PARM_INVALID;
            break;
        }

        btc_pba_client_ccb_t *p_ccb = &btc_pba_client_cb.ccb[handle - 1];
        if (p_ccb->handle != handle) {
            /* not connect */
            err = BT_STATUS_PARM_INVALID;
            break;
        }

        if (p_ccb->busy) {
            /* busy */
            err = BT_STATUS_BUSY;
            break;
        }

        p_ccb->busy = true;
        BTA_PbaClientSetPhoneBook(handle, flags, (char *)name);
        err = BT_STATUS_SUCCESS;
    } while (0);

    if (err != BT_STATUS_SUCCESS) {
        BTC_TRACE_WARNING("%s failed, handle: %d, reason: %d", __FUNCTION__, handle, err);
        return false;
    }

    return true;
}

static bool btc_pba_client_set_phone_book2(uint16_t handle, char *path)
{
    bt_status_t err = BT_STATUS_FAIL;

    do {
        if (!s_btc_pba_client_init) {
            /* pba client not init */
            err = BT_STATUS_NOT_READY;
            break;
        }

        if (handle == 0 || handle > BTC_PBA_CLIENT_MAX_CONN_NUM) {
            /* invalid handle value */
            err = BT_STATUS_PARM_INVALID;
            break;
        }

        btc_pba_client_ccb_t *p_ccb = &btc_pba_client_cb.ccb[handle - 1];
        if (p_ccb->handle != handle) {
            /* not connect */
            err = BT_STATUS_PARM_INVALID;
            break;
        }

        if (p_ccb->busy) {
            /* busy */
            err = BT_STATUS_BUSY;
            break;
        }

        p_ccb->busy = true;
        if (path != NULL) {
            p_ccb->path_len = strlen(path) + 1;
            /* ignore the first slash */
            if (path[0] == '/') {
                p_ccb->path_pos = 1;
            }
            else {
                p_ccb->path_pos = 0;
            }
            /* since we use absolute path, treat empty path as go to ROOT */
            if (p_ccb->path_len == p_ccb->path_pos + 1) {
                p_ccb->path_len = 0;
                p_ccb->path_pos = 0;
                osi_free(path);
                path = NULL;
            }
            else {
                p_ccb->path = path;
            }
        }
        /* anyway, go to ROOT first */
        char *empty_name = osi_malloc(1);
        assert(empty_name != NULL);
        *empty_name = '\0';
        BTA_PbaClientSetPhoneBook(handle, ESP_PBAC_SET_PHONE_BOOK_FLAGS_ROOT, empty_name);
        err = BT_STATUS_SUCCESS;
    } while (0);

    if (err != BT_STATUS_SUCCESS) {
        BTC_TRACE_WARNING("%s failed, handle: %d, reason: %d", __FUNCTION__, handle, err);
        return false;
    }

    return true;
}

static bool btc_pba_client_pull_vcard_listing(uint16_t handle, char *name, bool include_app_param, esp_pbac_pull_vcard_listing_app_param_t *app_param)
{
    bt_status_t err = BT_STATUS_FAIL;
    uint8_t *app_param_buff = NULL;
    uint16_t app_param_len = 0;

    do {
        if (!s_btc_pba_client_init) {
            /* pba client not init */
            err = BT_STATUS_NOT_READY;
            break;
        }

        if (handle == 0 || handle > BTC_PBA_CLIENT_MAX_CONN_NUM) {
            /* invalid handle value */
            err = BT_STATUS_PARM_INVALID;
            break;
        }

        btc_pba_client_ccb_t *p_ccb = &btc_pba_client_cb.ccb[handle - 1];
        if (p_ccb->handle != handle) {
            /* not connect */
            err = BT_STATUS_PARM_INVALID;
            break;
        }

        if (p_ccb->busy) {
            /* busy */
            err = BT_STATUS_BUSY;
            break;
        }

        if (include_app_param) {
            uint8_t search_value_len = 0;
            if (app_param->include_search_value) {
                search_value_len = strlen(app_param->search_value) + 1;
            }
            app_param_buff = osi_malloc(BTA_PBAP_PULL_VCARD_LISTING_APP_PARAM_BUFF_SIZE_MIN + search_value_len);
            if (app_param_buff == NULL) {
                err = BT_STATUS_NOMEM;
                break;
            }

            uint8_t *p = app_param_buff;
            if (app_param->include_order) {
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_ORDER);
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_LENGTH_ORDER);
                UINT8_TO_BE_STREAM(p, app_param->order);
            }
            if (app_param->include_search_value) {
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_SEARCH_VALUE);
                UINT8_TO_BE_STREAM(p, search_value_len);
                memcpy(p, app_param->search_value, search_value_len);
                p += search_value_len;
            }
            if (app_param->include_search_property) {
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_SEARCH_PROPERTY);
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_LENGTH_SEARCH_PROPERTY);
                UINT8_TO_BE_STREAM(p, app_param->search_property);
            }
            if (app_param->include_max_list_count) {
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_MAX_LIST_COUNT);
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_LENGTH_MAX_LIST_COUNT);
                UINT16_TO_BE_STREAM(p, app_param->max_list_count);
            }
            if (app_param->include_list_start_offset) {
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_LIST_START_OFFSET);
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_LENGTH_LIST_START_OFFSET);
                UINT16_TO_BE_STREAM(p, app_param->list_start_offset);
            }
            if (app_param->include_reset_new_missed_calls) {
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_RESET_NEW_MISSED_CALLS);
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_LENGTH_RESET_NEW_MISSED_CALLS);
                UINT8_TO_BE_STREAM(p, app_param->reset_new_missed_calls);
            }
            if (app_param->include_vcard_selector) {
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_VCARD_SELECTOR);
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_LENGTH_VCARD_SELECTOR);
                UINT64_TO_BE_STREAM(p, app_param->vcard_selector);
            }
            if (app_param->include_vcard_selector_operator) {
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_VCARD_SELECTOR_OPERATOR);
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_LENGTH_VCARD_SELECTOR_OPERATOR);
                UINT8_TO_BE_STREAM(p, app_param->vcard_selector_operator);
            }
            app_param_len = p - app_param_buff;
            assert(app_param_len <= BTA_PBAP_PULL_VCARD_LISTING_APP_PARAM_BUFF_SIZE_MIN + search_value_len);
            if (app_param_len == 0) {
                /* user give us an empty app param, allow but not recommend */
                osi_free(app_param_buff);
                app_param_buff = NULL;
            }
            /* free search_value (allocated by deep_copy) */
            if (app_param->include_search_value && app_param->search_value) {
                osi_free(app_param->search_value);
                app_param->search_value = NULL;
            }
        }

        p_ccb->busy = true;
        BTA_PbaClientPullvCardListing(handle, (char *)name, app_param_buff, app_param_len);
        err = BT_STATUS_SUCCESS;
    } while (0);

    if (err != BT_STATUS_SUCCESS) {
        BTC_TRACE_WARNING("%s failed, handle: %d, reason: %d", __FUNCTION__, handle, err);
        return false;
    }

    return true;
}

static bool btc_pba_client_pull_vcard_entry(uint16_t handle, char *name, bool include_app_param, esp_pbac_pull_vcard_entry_app_param_t *app_param)
{
    bt_status_t err = BT_STATUS_FAIL;
    uint8_t *app_param_buff = NULL;
    uint16_t app_param_len = 0;

    do {
        if (!s_btc_pba_client_init) {
            /* pba client not init */
            err = BT_STATUS_NOT_READY;
            break;
        }

        if (handle == 0 || handle > BTC_PBA_CLIENT_MAX_CONN_NUM) {
            /* invalid handle value */
            err = BT_STATUS_PARM_INVALID;
            break;
        }

        btc_pba_client_ccb_t *p_ccb = &btc_pba_client_cb.ccb[handle - 1];
        if (p_ccb->handle != handle) {
            /* not connect */
            err = BT_STATUS_PARM_INVALID;
            break;
        }

        if (p_ccb->busy) {
            /* busy */
            err = BT_STATUS_BUSY;
            break;
        }

        if (include_app_param) {
            app_param_buff = osi_malloc(BTA_PBAP_PULL_VCARD_ENTRY_APP_PARAM_BUFF_SIZE_MIN);
            if (app_param_buff == NULL) {
                err = BT_STATUS_NOMEM;
                break;
            }
            uint8_t *p = app_param_buff;
            if (app_param->include_property_selector) {
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_PROPERTY_SELECTOR);
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_LENGTH_PROPERTY_SELECTOR);
                UINT64_TO_BE_STREAM(p, app_param->property_selector);
            }
            if (app_param->include_format) {
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_FORMAT);
                UINT8_TO_BE_STREAM(p, BTA_PBAP_APP_PARAM_LENGTH_FORMAT);
                UINT8_TO_BE_STREAM(p, app_param->format);
            }
            app_param_len = p - app_param_buff;
            assert(app_param_len <= BTA_PBAP_PULL_VCARD_ENTRY_APP_PARAM_BUFF_SIZE_MIN);
            if (app_param_len == 0) {
                /* user give us an empty app param, allow but not recommend */
                osi_free(app_param_buff);
                app_param_buff = NULL;
            }
        }

        p_ccb->busy = true;
        BTA_PbaClientPullvCardEntry(handle, (char *)name, app_param_buff, app_param_len);
        err = BT_STATUS_SUCCESS;
    } while (0);

    if (err != BT_STATUS_SUCCESS) {
        BTC_TRACE_WARNING("%s failed, handle: %d, reason: %d", __FUNCTION__, handle, err);
        return false;
    }

    return true;
}

void btc_pba_client_args_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_pba_client_args_t *dst = (btc_pba_client_args_t *)p_dest;
    btc_pba_client_args_t *src = (btc_pba_client_args_t *)p_src;
    size_t len;

    switch (msg->act) {
    case BTC_PBA_CLIENT_PULL_PHONE_BOOK_EVT:
        len = strlen(src->pull_phone_book.name) + 1;
        dst->pull_phone_book.name = (char *)osi_malloc(len);
        if (dst->pull_phone_book.name) {
            memcpy(dst->pull_phone_book.name, src->pull_phone_book.name, len);
        } else {
            BTC_TRACE_ERROR("%s %d no mem\n", __FUNCTION__, msg->act);
        }
        break;
    case BTC_PBA_CLIENT_PULL_VCARD_LISTING_EVT:
        len = strlen(src->pull_vcard_listing.name) + 1;
        dst->pull_vcard_listing.name = (char *)osi_malloc(len);
        if (dst->pull_vcard_listing.name) {
            memcpy(dst->pull_vcard_listing.name, src->pull_vcard_listing.name, len);
        } else {
            BTC_TRACE_ERROR("%s %d no mem\n", __FUNCTION__, msg->act);
        }
        if (src->pull_vcard_listing.include_app_param && src->pull_vcard_listing.app_param.include_search_value) {
            len = strlen(src->pull_vcard_listing.app_param.search_value) + 1;
            dst->pull_vcard_listing.app_param.search_value = (char *)osi_malloc(len);
            if (dst->pull_vcard_listing.app_param.search_value) {
                memcpy(dst->pull_vcard_listing.app_param.search_value, src->pull_vcard_listing.app_param.search_value, len);
            } else {
                BTC_TRACE_ERROR("%s %d no mem\n", __FUNCTION__, msg->act);
            }
        }
        break;
    case BTC_PBA_CLIENT_PULL_VCARD_ENTRY_EVT:
        len = strlen(src->pull_vcard_entry.name) + 1;
        dst->pull_vcard_entry.name = (char *)osi_malloc(len);
        if (dst->pull_vcard_entry.name) {
            memcpy(dst->pull_vcard_entry.name, src->pull_vcard_entry.name, len);
        } else {
            BTC_TRACE_ERROR("%s %d no mem\n", __FUNCTION__, msg->act);
        }
        break;
    case BTC_PBA_CLIENT_SET_PHONE_BOOK_EVT:
    case BTC_PBA_CLIENT_SET_PHONE_BOOK2_EVT:
        /* set phone book name may be NULL */
        if (src->set_phone_book.name) {
            len = strlen(src->set_phone_book.name) + 1;
            dst->set_phone_book.name = (char *)osi_malloc(len);
            if (dst->set_phone_book.name) {
                memcpy(dst->set_phone_book.name, src->set_phone_book.name, len);
            } else {
                BTC_TRACE_ERROR("%s %d no mem\n", __FUNCTION__, msg->act);
            }
        }
        break;
    default:
        BTC_TRACE_DEBUG("%s Unhandled deep copy %d\n", __FUNCTION__, msg->act);
        UNUSED(dst);
        UNUSED(src);
        UNUSED(len);
        break;
    }
}

void btc_pba_client_args_deep_free(btc_msg_t *msg)
{
    btc_pba_client_args_t *arg = (btc_pba_client_args_t *)msg->arg;

    switch (msg->act) {
    case BTC_PBA_CLIENT_PULL_PHONE_BOOK_EVT:
        if (arg->pull_phone_book.name) {
            osi_free(arg->pull_phone_book.name);
        }
        break;
    case BTC_PBA_CLIENT_PULL_VCARD_LISTING_EVT:
        if (arg->pull_vcard_listing.name) {
            osi_free(arg->pull_vcard_listing.name);
        }
        if (arg->pull_vcard_listing.include_app_param
            && arg->pull_vcard_listing.app_param.include_search_value
            && arg->pull_vcard_listing.app_param.search_value) {
            osi_free(arg->pull_vcard_listing.app_param.search_value);
        }
        break;
    case BTC_PBA_CLIENT_PULL_VCARD_ENTRY_EVT:
        if (arg->pull_vcard_entry.name) {
            osi_free(arg->pull_vcard_entry.name);
        }
        break;
    case BTC_PBA_CLIENT_SET_PHONE_BOOK_EVT:
    case BTC_PBA_CLIENT_SET_PHONE_BOOK2_EVT:
        if (arg->set_phone_book.name) {
            osi_free(arg->set_phone_book.name);
        }
        break;
    default:
        BTC_TRACE_DEBUG("%s Unhandled deep free %d\n", __FUNCTION__, msg->act);
        UNUSED(arg);
        break;
    }
}

void btc_pba_client_call_handler(btc_msg_t *msg)
{
    bool ret = true;
    btc_pba_client_args_t *arg = (btc_pba_client_args_t *)(msg->arg);
    switch (msg->act) {
    case BTC_PBA_CLIENT_INIT_EVT:
        btc_pba_client_init();
        break;
    case BTC_PBA_CLIENT_DEINIT_EVT:
        btc_pba_client_deinit();
        break;
    case BTC_PBA_CLIENT_CONNECT_EVT:
        btc_pba_client_connect(&arg->connect.bd_addr);
        break;
    case BTC_PBA_CLIENT_DISCONNECT_EVT:
        btc_pba_client_disconnect(arg->disconnect.handle);
        break;
    case BTC_PBA_CLIENT_PULL_PHONE_BOOK_EVT:
        ret = btc_pba_client_pull_phone_book(arg->pull_phone_book.handle, arg->pull_phone_book.name, arg->pull_phone_book.include_app_param, &arg->pull_phone_book.app_param);
        break;
    case BTC_PBA_CLIENT_SET_PHONE_BOOK_EVT:
        ret = btc_pba_client_set_phone_book(arg->set_phone_book.handle, arg->set_phone_book.flags, arg->set_phone_book.name);
        break;
    case BTC_PBA_CLIENT_SET_PHONE_BOOK2_EVT:
        ret = btc_pba_client_set_phone_book2(arg->set_phone_book.handle, arg->set_phone_book.name);
        break;
    case BTC_PBA_CLIENT_PULL_VCARD_LISTING_EVT:
        ret = btc_pba_client_pull_vcard_listing(arg->pull_vcard_listing.handle, arg->pull_vcard_listing.name, arg->pull_vcard_listing.include_app_param, &arg->pull_vcard_listing.app_param);
        break;
    case BTC_PBA_CLIENT_PULL_VCARD_ENTRY_EVT:
        ret = btc_pba_client_pull_vcard_entry(arg->pull_vcard_entry.handle, arg->pull_vcard_entry.name, arg->pull_vcard_entry.include_app_param, &arg->pull_vcard_entry.app_param);
        break;
    default:
        BTC_TRACE_WARNING("unknown pba client action %i", msg->act);
        break;
    }

    if (!ret) {
        /* operation failed, do deep free */
        btc_pba_client_args_deep_free(msg);
    }
}

static void parse_pull_phone_book_app_param(esp_pbac_param_t *param, uint8_t *app_param, uint16_t app_param_len)
{
    if (app_param == NULL || app_param_len == 0) {
        return;
    }
    uint8_t *ptr = app_param;
    while(ptr < app_param + app_param_len) {
        switch (*ptr)
        {
        case BTA_PBAP_APP_PARAM_PHONE_BOOK_SIZE:
            param->pull_phone_book_rsp.include_phone_book_size = 1;
            ptr += BTA_PBAP_APP_PARAM_HEADER_LENGTH;
            BE_STREAM_TO_UINT16(param->pull_phone_book_rsp.phone_book_size, ptr);
            break;
        case BTA_PBAP_APP_PARAM_NEW_MISSED_CALLS:
            param->pull_phone_book_rsp.include_new_missed_calls = 1;
            ptr += BTA_PBAP_APP_PARAM_HEADER_LENGTH;
            BE_STREAM_TO_UINT8(param->pull_phone_book_rsp.new_missed_calls, ptr);
            break;
        case BTA_PBAP_APP_PARAM_PRIMARY_FOLDER_VERSION:
            param->pull_phone_book_rsp.include_primary_folder_version = 1;
            ptr += BTA_PBAP_APP_PARAM_HEADER_LENGTH;
            /* don't copy */
            param->pull_phone_book_rsp.primary_folder_version = ptr;
            ptr += BTA_PBAP_APP_PARAM_LENGTH_PRIMARY_FOLDER_VERSION;
            break;
        case BTA_PBAP_APP_PARAM_SECONDARY_FOLDER_VERSION:
            param->pull_phone_book_rsp.include_secondary_folder_version = 1;
            ptr += BTA_PBAP_APP_PARAM_HEADER_LENGTH;
            /* don't copy */
            param->pull_phone_book_rsp.secondary_folder_version = ptr;
            ptr += BTA_PBAP_APP_PARAM_LENGTH_SECONDARY_FOLDER_VERSION;
            break;
        case BTA_PBAP_APP_PARAM_DATABASE_IDENTIFIER:
            param->pull_phone_book_rsp.include_database_identifier = 1;
            ptr += BTA_PBAP_APP_PARAM_HEADER_LENGTH;
            /* don't copy */
            param->pull_phone_book_rsp.database_identifier = ptr;
            ptr += BTA_PBAP_APP_PARAM_LENGTH_DATABASE_IDENTIFIER;
            break;
        default:
            goto error;
            break;
        }
    }
error:
    return;
}

static void parse_pull_vcard_listing_app_param(esp_pbac_param_t *param, uint8_t *app_param, uint16_t app_param_len)
{
    if (app_param == NULL || app_param_len == 0) {
        return;
    }
    uint8_t *ptr = app_param;
    while(ptr < app_param + app_param_len) {
        switch (*ptr)
        {
        case BTA_PBAP_APP_PARAM_PHONE_BOOK_SIZE:
            param->pull_vcard_listing_rsp.include_phone_book_size = 1;
            ptr += BTA_PBAP_APP_PARAM_HEADER_LENGTH;
            BE_STREAM_TO_UINT16(param->pull_vcard_listing_rsp.phone_book_size, ptr);
            break;
        case BTA_PBAP_APP_PARAM_NEW_MISSED_CALLS:
            param->pull_vcard_listing_rsp.include_new_missed_calls = 1;
            ptr += BTA_PBAP_APP_PARAM_HEADER_LENGTH;
            BE_STREAM_TO_UINT8(param->pull_vcard_listing_rsp.new_missed_calls, ptr);
            break;
        case BTA_PBAP_APP_PARAM_PRIMARY_FOLDER_VERSION:
            param->pull_vcard_listing_rsp.include_primary_folder_version = 1;
            ptr += BTA_PBAP_APP_PARAM_HEADER_LENGTH;
            /* don't copy */
            param->pull_vcard_listing_rsp.primary_folder_version = ptr;
            ptr += BTA_PBAP_APP_PARAM_LENGTH_PRIMARY_FOLDER_VERSION;
            break;
        case BTA_PBAP_APP_PARAM_SECONDARY_FOLDER_VERSION:
            param->pull_vcard_listing_rsp.include_secondary_folder_version = 1;
            ptr += BTA_PBAP_APP_PARAM_HEADER_LENGTH;
            /* don't copy */
            param->pull_vcard_listing_rsp.secondary_folder_version = ptr;
            ptr += BTA_PBAP_APP_PARAM_LENGTH_SECONDARY_FOLDER_VERSION;
            break;
        case BTA_PBAP_APP_PARAM_DATABASE_IDENTIFIER:
            param->pull_vcard_listing_rsp.include_database_identifier = 1;
            ptr += BTA_PBAP_APP_PARAM_HEADER_LENGTH;
            /* don't copy */
            param->pull_vcard_listing_rsp.database_identifier = ptr;
            ptr += BTA_PBAP_APP_PARAM_LENGTH_DATABASE_IDENTIFIER;
            break;
        default:
            goto error;
            break;
        }
    }
error:
    return;
}

static void parse_pull_vcard_entry_app_param(esp_pbac_param_t *param, uint8_t *app_param, uint16_t app_param_len)
{
    if (app_param == NULL || app_param_len == 0) {
        return;
    }
    uint8_t *ptr = app_param;
    while(ptr < app_param + app_param_len) {
        switch (*ptr)
        {
        case BTA_PBAP_APP_PARAM_DATABASE_IDENTIFIER:
            param->pull_vcard_entry_rsp.include_database_identifier = 1;
            ptr += BTA_PBAP_APP_PARAM_HEADER_LENGTH;
            /* don't copy */
            param->pull_vcard_entry_rsp.database_identifier = ptr;
            ptr += BTA_PBAP_APP_PARAM_LENGTH_DATABASE_IDENTIFIER;
            break;
        default:
            goto error;
            break;
        }
    }
error:
    return;
}

static uint16_t get_next_dir_len_from_path(char *path, uint16_t path_len, uint16_t path_pos)
{
    uint16_t ret = 0;
    for (int i = path_pos; i < path_len; ++i) {
        if (path[i] == '/' || path[i] == '\0') {
            break;
        }
        else {
            ++ret;
        }
    }
    return ret;
}

void btc_pba_client_cb_handler(btc_msg_t *msg)
{
    uint16_t event = msg->act;
    tBTA_PBA_CLIENT *p_data = (tBTA_PBA_CLIENT *)msg->arg;
    btc_pba_client_ccb_t *p_ccb = NULL;
    esp_pbac_param_t param = {0};

    switch (event) {
    case BTA_PBA_CLIENT_CONN_OPEN_EVT:
        if (p_data->conn.error == BTA_PBA_CLIENT_NO_ERROR) {
            /* allocate ccb */
            p_ccb = &btc_pba_client_cb.ccb[p_data->conn.handle - 1];
            p_ccb->handle = p_data->conn.handle;
            bdcpy(p_ccb->bd_addr.address, p_data->conn.bd_addr);
            p_ccb->busy = false;
            param.conn_stat.connected = true;
            param.conn_stat.peer_supported_repo = p_data->conn.peer_supported_repo;
            param.conn_stat.peer_supported_feat = p_data->conn.peer_supported_feat;
            param.conn_stat.reason = BTA_PBA_CLIENT_NO_ERROR;
        }
        else {
            param.conn_stat.connected = false;
            /* error codes are compatible */
            param.conn_stat.reason = p_data->conn.error;
        }
        bdcpy(param.conn_stat.remote_bda, p_data->conn.bd_addr);
        param.conn_stat.handle = p_data->conn.handle;
        btc_pba_client_callback_to_app(ESP_PBAC_CONNECTION_STATE_EVT, &param);
        btc_queue_advance();
        break;
    case BTA_PBA_CLIENT_CONN_CLOSE_EVT:
        if (p_data->conn.handle != 0) {
            /* clear ccb */
            p_ccb = &btc_pba_client_cb.ccb[p_data->conn.handle - 1];
            if (p_ccb->path) {
                osi_free(p_ccb->path);
            }
            memset(p_ccb, 0, sizeof(btc_pba_client_ccb_t));
        }
        param.conn_stat.connected = false;
        bdcpy(param.conn_stat.remote_bda, p_data->conn.bd_addr);
        param.conn_stat.handle = p_data->conn.handle;
        /* error codes are compatible */
        param.conn_stat.reason = p_data->conn.error;
        btc_pba_client_callback_to_app(ESP_PBAC_CONNECTION_STATE_EVT, &param);
        btc_queue_advance();
        break;
    case BTA_PBA_CLIENT_PULL_PHONE_BOOK_RSP_EVT:
        if (p_data->response.final) {
            p_ccb = &btc_pba_client_cb.ccb[p_data->response.handle - 1];
            p_ccb->busy = false;
        }
        param.pull_phone_book_rsp.handle = p_data->response.handle;
        param.pull_phone_book_rsp.final = p_data->response.final;
        param.pull_phone_book_rsp.result = p_data->response.status;
        param.pull_phone_book_rsp.data = p_data->response.data;
        param.pull_phone_book_rsp.data_len = p_data->response.data_len;
        parse_pull_phone_book_app_param(&param, p_data->response.app_param, p_data->response.app_param_len);
        btc_pba_client_callback_to_app(ESP_PBAC_PULL_PHONE_BOOK_RESPONSE_EVT, &param);
        if (p_data->response.pkt != NULL) {
            osi_free(p_data->response.pkt);
        }
        break;
    case BTA_PBA_CLIENT_SET_PHONE_BOOK_RSP_EVT:
        p_ccb = &btc_pba_client_cb.ccb[p_data->response.handle - 1];
        if (p_data->response.status == BTA_PBA_CLIENT_NO_ERROR && p_ccb->path_pos < p_ccb->path_len) {
            /* since path_len is not zero, path should not be NULL, use asset to check */
            assert(p_ccb->path != NULL);
            uint16_t dir_name_len = get_next_dir_len_from_path(p_ccb->path, p_ccb->path_len, p_ccb->path_pos);
            if (dir_name_len > 0) {
                char *dir_name = osi_malloc(dir_name_len + 1);
                assert(dir_name != NULL);
                memcpy(dir_name, p_ccb->path + p_ccb->path_pos, dir_name_len);
                dir_name[dir_name_len] = '\0';
                p_ccb->path_pos += dir_name_len + 1;
                BTA_PbaClientSetPhoneBook(p_data->response.handle, ESP_PBAC_SET_PHONE_BOOK_FLAGS_DOWN, dir_name);
                /* break here, don't report event to upper */
                break;
            }
        }
        /* set path done or failed, clear status */
        p_ccb->path_len = 0;
        p_ccb->path_pos = 0;
        if (p_ccb->path) {
            osi_free(p_ccb->path);
            p_ccb->path = NULL;
        }
        p_ccb->busy = false;
        param.set_phone_book_rsp.handle = p_data->response.handle;
        param.set_phone_book_rsp.result = p_data->response.status;
        btc_pba_client_callback_to_app(ESP_PBAC_SET_PHONE_BOOK_RESPONSE_EVT, &param);
        if (p_data->response.pkt != NULL) {
            osi_free(p_data->response.pkt);
        }
        break;
    case BTA_PBA_CLIENT_PULL_VCARD_LISTING_RSP_EVT:
        if (p_data->response.final) {
            p_ccb = &btc_pba_client_cb.ccb[p_data->response.handle - 1];
            p_ccb->busy = false;
        }
        param.pull_vcard_listing_rsp.handle = p_data->response.handle;
        param.pull_vcard_listing_rsp.final = p_data->response.final;
        param.pull_vcard_listing_rsp.result = p_data->response.status;
        param.pull_vcard_listing_rsp.data = p_data->response.data;
        param.pull_vcard_listing_rsp.data_len = p_data->response.data_len;
        parse_pull_vcard_listing_app_param(&param, p_data->response.app_param, p_data->response.app_param_len);
        btc_pba_client_callback_to_app(ESP_PBAC_PULL_VCARD_LISTING_RESPONSE_EVT, &param);
        if (p_data->response.pkt != NULL) {
            osi_free(p_data->response.pkt);
        }
        break;
    case BTA_PBA_CLIENT_PULL_VCARD_ENTRY_RSP_EVT:
        if (p_data->response.final) {
            p_ccb = &btc_pba_client_cb.ccb[p_data->response.handle - 1];
            p_ccb->busy = false;
        }
        param.pull_vcard_entry_rsp.handle = p_data->response.handle;
        param.pull_vcard_entry_rsp.final = p_data->response.final;
        param.pull_vcard_entry_rsp.result = p_data->response.status;
        param.pull_vcard_entry_rsp.data = p_data->response.data;
        param.pull_vcard_entry_rsp.data_len = p_data->response.data_len;
        parse_pull_vcard_entry_app_param(&param, p_data->response.app_param, p_data->response.app_param_len);
        btc_pba_client_callback_to_app(ESP_PBAC_PULL_VCARD_ENTRY_RESPONSE_EVT, &param);
        if (p_data->response.pkt != NULL) {
            osi_free(p_data->response.pkt);
        }
        break;
    case BTA_PBA_CLIENT_REGISTER_EVT:
        /* init process: Enable -> Register */
        btc_pba_client_callback_to_app(ESP_PBAC_INIT_EVT, NULL);
        break;
    case BTA_PBA_CLIENT_DISABLE_EVT:
        /* deinit process: Deregister -> Disable */
        btc_pba_client_callback_to_app(ESP_PBAC_DEINIT_EVT, NULL);
        break;
    default:
        BTC_TRACE_WARNING("%s: unknown event (%d)", __func__, event);
        break;
    }
}

#endif
