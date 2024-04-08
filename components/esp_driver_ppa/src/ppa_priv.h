/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <sys/queue.h>
#include "sdkconfig.h"
#include "driver/ppa.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_private/dma2d.h"
#include "hal/dma2d_types.h"
#include "hal/ppa_types.h"
#include "esp_pm.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PPA_MEM_ALLOC_CAPS (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT) // TODO...

#define PPA_PM_LOCK_NAME_LEN_MAX 16

/******************************** ENGINE *************************************/
// PPA module contains SRM engine and Blending engine

typedef struct ppa_engine_t ppa_engine_t;

struct ppa_engine_t {
    ppa_engine_type_t type;                       // Type of the PPA engine
    portMUX_TYPE spinlock;                        // Engine level spinlock
    SemaphoreHandle_t sem;                        // Semaphore for whether the engine is processing a transaction
    STAILQ_HEAD(trans, ppa_trans_s) trans_stailq; // link head of pending transactions for the PPA engine
#if CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock;                 // Power management lock
#endif
};

typedef struct ppa_srm_engine_t {
    ppa_engine_t base;                            // PPA engine base structure
    dma2d_descriptor_t *dma_tx_desc;              // Into PPA SRM engine direction 2D-DMA descriptor
    dma2d_descriptor_t *dma_rx_desc;              // Out from PPA SRM engine direction 2D-DMA descriptor
} ppa_srm_engine_t;

typedef struct ppa_blend_engine_t {
    ppa_engine_t base;                            // PPA engine base structure
    dma2d_descriptor_t *dma_tx_bg_desc;           // Into PPA Blending engine direction background channel 2D-DMA descriptor
    dma2d_descriptor_t *dma_tx_fg_desc;           // Into PPA Blending engine direction foreground channel 2D-DMA descriptor
    dma2d_descriptor_t *dma_rx_desc;              // Out from PPA blending engine direction 2D-DMA descriptor
} ppa_blend_engine_t;

typedef struct {
    ppa_engine_type_t engine;                     // Engine type
} ppa_engine_config_t;

/******************************** CLIENT *************************************/

typedef struct ppa_invoker_t ppa_invoker_t;

struct ppa_invoker_t {
    ppa_operation_t oper_type;                    // The PPA operation type that the invoker wants to do in speciality
    ppa_engine_t *engine;                         // Pointer to the PPA engine that in charge of performing the PPA operation
    uint32_t trans_cnt;                           // Number of pending PPA transactions
    portMUX_TYPE spinlock;                        // Client level spinlock
    bool in_accepting_trans_state;                // Indicates whether the invoker can accept new PPA transaction requests now
    ppa_event_callback_t done_cb;                 // Transaction done callback
    QueueHandle_t trans_elm_ptr_queue;            // Queue that contains the pointers to the allocated memory to save the transaction contexts
};

/****************************** OPERATION ************************************/

// The elements in this structure listed first are identical to the elements in structure `ppa_blend_operation_config_t`
// With adding a few extra elements in the end
// This allows memcpy
typedef struct {
    void *in_buffer;
    uint32_t in_pic_w;
    uint32_t in_pic_h;
    uint32_t in_block_w;
    uint32_t in_block_h;
    uint32_t in_block_offset_x;
    uint32_t in_block_offset_y;

    void *out_buffer;
    uint32_t out_buffer_size;
    uint32_t out_pic_w;
    uint32_t out_pic_h;
    uint32_t out_block_offset_x;
    uint32_t out_block_offset_y;

    ppa_srm_rotation_angle_t rotation_angle;
    float scale_x;
    float scale_y;
    bool mirror_x;
    bool mirror_y;

    struct {
        ppa_srm_color_mode_t mode;
        color_range_t yuv_range;
        color_conv_std_rgb_yuv_t yuv_std;
        bool rgb_swap;
        bool byte_swap;
        ppa_alpha_update_mode_t alpha_update_mode;
        uint32_t alpha_value;
    } in_color;

    struct {
        ppa_srm_color_mode_t mode;
        color_range_t yuv_range;
        color_conv_std_rgb_yuv_t yuv_std;
    } out_color;

    uint32_t scale_x_int;
    uint32_t scale_x_frag;
    uint32_t scale_y_int;
    uint32_t scale_y_frag;
} ppa_srm_oper_t;

typedef ppa_blend_operation_config_t ppa_blend_oper_t;

typedef ppa_fill_operation_config_t ppa_fill_oper_t;

/***************************** TRANSACTION ***********************************/

// PPA transaction element
typedef struct ppa_trans_s {
    STAILQ_ENTRY(ppa_trans_s) entry;              // Link entry
    dma2d_trans_config_t *trans_desc;             // Pointer to the structure containing the configurations for a 2D-DMA transaction
    dma2d_trans_t *dma_trans_placeholder;         // Pointer to the memory to store the 2D-DMA transaction context
    SemaphoreHandle_t sem;                        // Semaphore to block when the transaction has not finished
    ppa_invoker_t *invoker;                       // Pointer to the invoker who requested the transaction
    void *user_data;                              // User registered event data (per transaction)
} ppa_trans_t;

typedef struct {
    union {
        ppa_srm_oper_t *srm_desc;                 // Pointer to the structure containing the configurations for a PPA SRM operation transaction
        ppa_blend_oper_t *blend_desc;             // Pointer to the structure containing the configurations for a PPA blend operation transaction
        ppa_fill_oper_t *fill_desc;               // Pointer to the structure containing the configurations for a PPA fill operation transaction
        void *op_desc;                            // General pointer to the structure containing the configurations for a PPA transaction
    };
    ppa_engine_t *ppa_engine;                     // Pointer to the PPA engine
    ppa_trans_t *trans_elm;                       // Pointer to the PPA transaction element
    dma2d_trigger_peripheral_t trigger_periph;    // The 2D-DMA trigger peripheral
} ppa_dma2d_trans_on_picked_config_t;

#ifdef __cplusplus
}
#endif
