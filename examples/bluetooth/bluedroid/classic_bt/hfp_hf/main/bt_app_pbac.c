/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_pbac_api.h"
#include "bt_app_core.h"
#include "bt_app_pbac.h"

#define BT_PBAC_TAG       "BT_PBAC"

esp_pbac_conn_hdl_t pba_conn_handle;

void bt_app_pbac_cb(esp_pbac_event_t event, esp_pbac_param_t *param)
{
    switch (event)
    {
    case ESP_PBAC_CONNECTION_STATE_EVT:
        ESP_LOGI(BT_PBAC_TAG, "PBA client connection event, state: %s, reason: 0x%x", (param->conn_stat.connected ? "Connected" : "Disconnected"), param->conn_stat.reason);
        ESP_LOGI(BT_PBAC_TAG, "Peer supported repositories: 0x%x, supported features: 0x%lx", param->conn_stat.peer_supported_repo, param->conn_stat.peer_supported_feat);
        if (param->conn_stat.connected) {
            pba_conn_handle = param->conn_stat.handle;
            /* set phone book to "telecom" folder, just to test set phone book function */
            esp_pbac_set_phone_book(pba_conn_handle, ESP_PBAC_SET_PHONE_BOOK_FLAGS_DOWN, "telecom");
        }
        break;
    case ESP_PBAC_PULL_PHONE_BOOK_RESPONSE_EVT:
        /* if multiple PBA connection, we should check param->pull_phone_book_rsp.handle */
        ESP_LOGI(BT_PBAC_TAG, "PBA client pull phone book response, handle:%d, result: 0x%x", param->pull_phone_book_rsp.handle, param->pull_phone_book_rsp.result);
        if (param->pull_phone_book_rsp.result == ESP_PBAC_SUCCESS && param->pull_phone_book_rsp.data_len > 0) {
            printf("%.*s\n", param->pull_phone_book_rsp.data_len, param->pull_phone_book_rsp.data);
            /* copy data to other buff before return, if phone book size is too large, it will be sent in multiple response event */
        }
        if (param->pull_phone_book_rsp.final) {
            ESP_LOGI(BT_PBAC_TAG, "PBA client pull phone book final response");
            /* pull phone book done, now we can perform other operation */
            if (param->pull_phone_book_rsp.result == ESP_PBAC_SUCCESS && param->pull_phone_book_rsp.include_phone_book_size) {
                ESP_LOGI(BT_PBAC_TAG, "Phone Book Size:%d", param->pull_phone_book_rsp.phone_book_size);
                esp_pbac_pull_phone_book_app_param_t app_param = {0};
                app_param.include_property_selector = 1;
                /* property bit mask, filter out photo, refer to Phone Book Access Profile */
                app_param.property_selector = 0xFFFFFFF7;
                /* pull again, without 'max_list_count = 0', then we can get the entire phone book */
                esp_pbac_pull_phone_book(pba_conn_handle, "telecom/pb.vcf", &app_param);
            }
        }
        break;
    case ESP_PBAC_SET_PHONE_BOOK_RESPONSE_EVT:
        ESP_LOGI(BT_PBAC_TAG, "PBA client set phone book response, handle:%d, result: 0x%x", param->set_phone_book_rsp.handle, param->set_phone_book_rsp.result);
        /* done, set phone book response will always be a final response */
        if (param->set_phone_book_rsp.result == ESP_PBAC_SUCCESS) {
            esp_pbac_pull_phone_book_app_param_t app_param = {0};
            app_param.include_max_list_count = 1;
            /* set max_list_count to zero, then we can get phone book size in peer response */
            app_param.max_list_count = 0;
            /* pull phone book use a absolute path; if no app param, we can pass a NULL to API */
            esp_pbac_pull_phone_book(pba_conn_handle, "telecom/pb.vcf", &app_param);
        }
        break;
    case ESP_PBAC_PULL_VCARD_LISTING_RESPONSE_EVT:
        ESP_LOGI(BT_PBAC_TAG, "PBA client pull vCard listing response, handle:%d, result: 0x%x", param->pull_vcard_listing_rsp.handle, param->pull_vcard_listing_rsp.result);
        if (param->pull_vcard_listing_rsp.result == ESP_PBAC_SUCCESS) {
            printf("%.*s\n", param->pull_vcard_listing_rsp.data_len, param->pull_vcard_listing_rsp.data);
        }
        if (param->pull_vcard_listing_rsp.final) {
            ESP_LOGI(BT_PBAC_TAG, "PBA client pull vCard listing final response");
        }
        break;
    case ESP_PBAC_PULL_VCARD_ENTRY_RESPONSE_EVT:
        ESP_LOGI(BT_PBAC_TAG, "PBA client pull vCard entry response, handle:%d, result: 0x%x", param->pull_vcard_entry_rsp.handle, param->pull_vcard_entry_rsp.result);
        if (param->pull_vcard_entry_rsp.result == ESP_PBAC_SUCCESS) {
            printf("%.*s\n", param->pull_vcard_entry_rsp.data_len, param->pull_vcard_entry_rsp.data);
        }
        if (param->pull_vcard_entry_rsp.final) {
            ESP_LOGI(BT_PBAC_TAG, "PBA client pull vCard entry final response");
        }
        break;
    default:
        break;
    }
}
