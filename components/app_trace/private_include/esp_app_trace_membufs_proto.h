/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_APP_TRACE_MEMBUFS_PROTO_H_
#define ESP_APP_TRACE_MEMBUFS_PROTO_H_

#include "esp_app_trace_util.h"

#ifdef __cplusplus
extern "C" {
#endif

/** TRAX HW transport state */
typedef struct {
    uint32_t                   in_block;     // input block ID
    // TODO: change to uint16_t
    uint32_t                   markers[2];   // block filling level markers
} esp_apptrace_membufs_state_t;

/** memory block parameters,
 * should be packed, because it is read from the host */
typedef struct {
    uint8_t   *start;   // start address
    uint32_t   sz;      // size
} esp_apptrace_mem_block_t;

typedef struct {
    esp_err_t (*swap_start)(uint32_t curr_block_id);
    esp_err_t (*swap)(uint32_t new_block_id);
    esp_err_t (*swap_end)(uint32_t new_block_id, uint32_t prev_block_len);
    bool (*host_data_pending)(void);
} esp_apptrace_membufs_proto_hw_t;

typedef struct {
    esp_apptrace_membufs_proto_hw_t *       hw;
    volatile esp_apptrace_membufs_state_t   state;       // state
    esp_apptrace_mem_block_t                blocks[2];   // memory blocks
#if CONFIG_APPTRACE_PENDING_DATA_SIZE_MAX > 0
    // ring buffer control struct for pending user blocks
    esp_apptrace_rb_t                   rb_pend;
    // storage for pending user blocks
    uint8_t                             pending_data[CONFIG_APPTRACE_PENDING_DATA_SIZE_MAX + 1];
#endif
    // ring buffer control struct for data from host (down buffer)
    esp_apptrace_rb_t                       rb_down;
} esp_apptrace_membufs_proto_data_t;

esp_err_t esp_apptrace_membufs_init(esp_apptrace_membufs_proto_data_t *proto, const esp_apptrace_mem_block_t blocks_cfg[2]);
void esp_apptrace_membufs_down_buffer_config(esp_apptrace_membufs_proto_data_t *data, uint8_t *buf, uint32_t size);
uint8_t *esp_apptrace_membufs_down_buffer_get(esp_apptrace_membufs_proto_data_t *proto, uint32_t *size, esp_apptrace_tmo_t *tmo);
esp_err_t esp_apptrace_membufs_down_buffer_put(esp_apptrace_membufs_proto_data_t *proto, uint8_t *ptr, esp_apptrace_tmo_t *tmo);
uint8_t *esp_apptrace_membufs_up_buffer_get(esp_apptrace_membufs_proto_data_t *proto, uint32_t size, esp_apptrace_tmo_t *tmo);
esp_err_t esp_apptrace_membufs_up_buffer_put(esp_apptrace_membufs_proto_data_t *proto, uint8_t *ptr, esp_apptrace_tmo_t *tmo);
esp_err_t esp_apptrace_membufs_flush_nolock(esp_apptrace_membufs_proto_data_t *proto, uint32_t min_sz, esp_apptrace_tmo_t *tmo);

#ifdef __cplusplus
}
#endif

#endif
