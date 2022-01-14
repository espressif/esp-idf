/*
 * ESP BLE Mesh Example
 *
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef __SYNC_H__
#define __SYNC_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "esp_log.h"
#include "esp_timer.h"
#include "ble_unit.h"
#include "test_env.h"

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "mesh_util.h"
#define PARAM_MAX           7

#define SYNC_TIMEOUT   500000 //500ms
#define PARAMTER(value) (((uint32_t)value) << 7)

#define WIFI_TCP_TX_CASE    0x0
#define WIFI_TCP_RX_CASE    0x1
#define BLE_ADV_CASE        0x2
#define BLE_SCAN_CASE       0x3
#define NOT_CASE            0xff

#define MSG_HEAD                     0xcbb3
#define MSG_TYPE                     0xff
#define MSG_CONTINUE                 0x0
#define MSG_END                      0x1
#define MSG_DATA_BASE                0x8
#define MSG_ID_ASSIGN_CASE           0x1
#define MSG_ID_WIFI_DEV_INIT_FINISH  0x2
#define MSG_ID_BT_DEV_INIT_FINISH    0x3
#define MSG_ID_START_CASE            0x4

#define BLE_CASE_ID         0b100000 << PARAM_MAX
#define WIFI_CASE_ID        0b010000 << PARAM_MAX
#define SERVER_IP           0b001000 << PARAM_MAX
#define START_TIME          0b000100 << PARAM_MAX
#define WIFI_SSID           0b000010 << PARAM_MAX
#define WIFI_PASSWIRD       0b000001 << PARAM_MAX


#define GET_PARAM(value,bit)    (((value) & (bit)) ? bit:0x0)

#define MSG_MIN_LEN         8

typedef void (*sync_recv)(uint8_t *raw_data, uint32_t raw_data_len);

typedef struct  {
    uint8_t length;
    uint8_t  type;
    uint16_t head;
    uint8_t  msg_id;
    uint16_t ctl: 3,
             param_bit: 13;
} __attribute__((packed)) sync_msg_head;

typedef struct  {
    uint8_t length;
    uint8_t  type;
    uint16_t head;
    uint8_t  msg_id;
    uint16_t ctl: 3,
             param_bit: 13;
    uint8_t  reserve;       //reserved for extend param_bit
    uint8_t  data[23];
} __attribute__((packed)) sync_msg;

typedef struct {
    uint8_t  case_id;
    const uint16_t excpet_param_bit;
} auto_tc;
extern auto_tc auto_tb[6];


typedef enum {
#if defined(CONFIG_EXAMPLE_COEX_ROLE)
    ASSIGN_CASE,
#else
    WAIT_CASE,
    WAIT_START,
#endif
    START_CASE,
} sync_state;

struct sync_t {
    sync_state    state;
    uint8_t       own_wifi_case;
    uint8_t       own_ble_case;
    uint8_t       start_time;

    uint16_t      recv_param_bit;

    bool          except_recv_wifi_id;
    bool          except_recv_bt_id;

    esp_timer_handle_t sync_timer;
    sync_recv     cmd_recv;
};
extern struct sync_t sync_obj;

extern SemaphoreHandle_t client_mutex;
void sync_init(void);

#endif
