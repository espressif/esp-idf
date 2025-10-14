/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "common/bt_defs.h"
#include "esp_pbac_api.h"

#if BTC_PBA_CLIENT_INCLUDED

typedef enum {
    BTC_PBA_CLIENT_INIT_EVT = 0,
    BTC_PBA_CLIENT_DEINIT_EVT,
    BTC_PBA_CLIENT_CONNECT_EVT,
    BTC_PBA_CLIENT_DISCONNECT_EVT,
    BTC_PBA_CLIENT_PULL_PHONE_BOOK_EVT,
    BTC_PBA_CLIENT_SET_PHONE_BOOK_EVT,
    BTC_PBA_CLIENT_SET_PHONE_BOOK2_EVT,
    BTC_PBA_CLIENT_PULL_VCARD_LISTING_EVT,
    BTC_PBA_CLIENT_PULL_VCARD_ENTRY_EVT
} BTC_PBA_CLIENT_EVT;

/* equal to BTA max conn */
#define BTC_PBA_CLIENT_MAX_CONN_NUM     2

typedef struct {
    uint16_t                            handle;
    bt_bdaddr_t                         bd_addr;
    char                               *path;
    uint16_t                            path_len;
    uint16_t                            path_pos;
    bool                                busy;
} btc_pba_client_ccb_t;

typedef struct {
    btc_pba_client_ccb_t                ccb[BTC_PBA_CLIENT_MAX_CONN_NUM];
} btc_pba_client_cb_t;

typedef union {
    // BTC_PBA_CLIENT_CONNECT_EVT
    struct pba_client_connect_arg {
        bt_bdaddr_t bd_addr;
    } connect;

    // BTC_PBA_CLIENT_DISCONNECT_EVT
    struct pba_client_disconnect_arg {
        uint16_t handle;
    } disconnect;

    // BTC_PBA_CLIENT_PULL_PHONE_BOOK_EVT
    struct pba_client_pull_phone_book_arg {
        uint16_t handle;
        char *name;
        bool include_app_param;
        esp_pbac_pull_phone_book_app_param_t app_param;
    } pull_phone_book;

    // BTC_PBA_CLIENT_SET_PHONE_BOOK_EVT
    struct pba_client_set_phone_book_arg {
        uint16_t handle;
        uint8_t flags;
        char *name;
    } set_phone_book;

    // BTC_PBA_CLIENT_PULL_VCARD_LISTING_EVT
    struct pba_client_pull_vcard_listing_arg {
        uint16_t handle;
        char *name;
        bool include_app_param;
        esp_pbac_pull_vcard_listing_app_param_t app_param;
    } pull_vcard_listing;

    // BTC_PBA_CLIENT_PULL_VCARD_ENTRY_EVT
    struct pba_client_pull_vcard_entry_arg {
        uint16_t handle;
        char *name;
        bool include_app_param;
        esp_pbac_pull_vcard_entry_app_param_t app_param;
    } pull_vcard_entry;
} btc_pba_client_args_t;

void btc_pba_client_call_handler(btc_msg_t *msg);
void btc_pba_client_cb_handler(btc_msg_t *msg);
void btc_pba_client_args_deep_free(btc_msg_t *msg);
void btc_pba_client_args_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);

#endif
