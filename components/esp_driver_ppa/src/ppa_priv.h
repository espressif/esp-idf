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
#include "hal/ppa_hal.h"
#include "esp_pm.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PPA_MEM_ALLOC_CAPS       (MALLOC_CAP_DEFAULT)

#define PPA_PM_LOCK_NAME_LEN_MAX 16

#define PPA_CHECK_CM_SUPPORT_BYTE_SWAP(str, color_type_id) \
            ESP_RETURN_ON_FALSE(color_type_id == COLOR_TYPE_ID(COLOR_SPACE_ARGB, COLOR_PIXEL_ARGB8888) || color_type_id == COLOR_TYPE_ID(COLOR_SPACE_RGB, COLOR_PIXEL_RGB565), \
                                ESP_ERR_INVALID_ARG, TAG, str "_cm does not support byte_swap");

#define PPA_CHECK_CM_SUPPORT_RGB_SWAP(str, color_type_id) \
            ESP_RETURN_ON_FALSE(COLOR_SPACE_TYPE(color_type_id) == COLOR_SPACE_ARGB || COLOR_SPACE_TYPE(color_type_id) == COLOR_SPACE_RGB, \
                                ESP_ERR_INVALID_ARG, TAG, str "_cm does not support rgb_swap");

#define PPA_ALIGN_UP(num, align)    (((num) + ((align) - 1)) & ~((align) - 1))
#define PPA_ALIGN_DOWN(num, align)  ((num) & ~((align) - 1))

typedef struct ppa_platform_t ppa_platform_t;

/******************************** ENGINE *************************************/
// PPA module contains SRM engine and Blending engine

typedef struct ppa_engine_t ppa_engine_t;

struct ppa_engine_t {
    ppa_platform_t *platform;                     // PPA driver platform
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

typedef struct ppa_client_t ppa_client_t;

struct ppa_client_t {
    ppa_operation_t oper_type;                    // The PPA operation type that the client wants to do in speciality
    ppa_engine_t *engine;                         // Pointer to the PPA engine that in charge of performing the PPA operation
    uint32_t trans_cnt;                           // Number of pending PPA transactions
    portMUX_TYPE spinlock;                        // Client level spinlock
    ppa_event_callback_t done_cb;                 // Transaction done callback
    QueueHandle_t trans_elm_ptr_queue;            // Queue that contains the pointers to the allocated memory to save the transaction contexts
    ppa_data_burst_length_t data_burst_length;    // The desired data burst length for all the transactions of the client
};

/****************************** OPERATION ************************************/

// The elements in this structure listed first are identical to the elements in structure `ppa_srm_oper_config_t`
// With adding a few extra elements at the end
// This allows memcpy
typedef struct {
    ppa_in_pic_blk_config_t in;
    ppa_out_pic_blk_config_t out;

    // scale-rotate-mirror manipulation
    ppa_srm_rotation_angle_t rotation_angle;
    float scale_x;
    float scale_y;
    bool mirror_x;
    bool mirror_y;

    // input data manipulation
    bool rgb_swap;
    bool byte_swap;
    ppa_alpha_update_mode_t alpha_update_mode;
    union {
        uint32_t alpha_fix_val;
        float alpha_scale_ratio;
    };

    ppa_trans_mode_t mode;
    void *user_data;

    uint32_t scale_x_int;                         // Calculation result for the integral part of the scale_x to be directly written to register
    uint32_t scale_x_frag;                        // Calculation result for the fractional part of the scale_x to be directly written to register
    uint32_t scale_y_int;                         // Calculation result for the integral part of the scale_y to be directly written to register
    uint32_t scale_y_frag;                        // Calculation result for the fractional part of the scale_y to be directly written to register
    uint32_t alpha_value;                         // Calculation result for the fix alpha value to be directly written to register
    ppa_data_burst_length_t data_burst_length;    // Data burst length for the transaction, information passed from the client
} ppa_srm_oper_t;

// The elements in this structure listed first are identical to the elements in structure `ppa_blend_oper_config_t`
// With adding a few extra elements at the end
// This allows memcpy
typedef struct {
    ppa_in_pic_blk_config_t in_bg;
    ppa_in_pic_blk_config_t in_fg;
    ppa_out_pic_blk_config_t out;

    // input data manipulation
    bool bg_rgb_swap;
    bool bg_byte_swap;
    ppa_alpha_update_mode_t bg_alpha_update_mode;
    union {
        uint32_t bg_alpha_fix_val;
        float bg_alpha_scale_ratio;
    };
    bool fg_rgb_swap;
    bool fg_byte_swap;
    ppa_alpha_update_mode_t fg_alpha_update_mode;
    union {
        uint32_t fg_alpha_fix_val;
        float fg_alpha_scale_ratio;
    };
    color_pixel_rgb888_data_t fg_fix_rgb_val;

    // color-keying
    bool bg_ck_en;
    color_pixel_rgb888_data_t bg_ck_rgb_low_thres;
    color_pixel_rgb888_data_t bg_ck_rgb_high_thres;
    bool fg_ck_en;
    color_pixel_rgb888_data_t fg_ck_rgb_low_thres;
    color_pixel_rgb888_data_t fg_ck_rgb_high_thres;
    color_pixel_rgb888_data_t ck_rgb_default_val;
    bool ck_reverse_bg2fg;

    ppa_trans_mode_t mode;
    void *user_data;

    uint32_t bg_alpha_value;                      // Calculation result for the fix alpha value for BG to be directly written to register
    uint32_t fg_alpha_value;                      // Calculation result for the fix alpha value for FG to be directly written to register
    ppa_data_burst_length_t data_burst_length;    // Data burst length for the transaction, information passed from the client
} ppa_blend_oper_t;

// The elements in this structure listed first are identical to the elements in structure `ppa_fill_oper_config_t`
// With adding a few extra elements at the end
// This allows memcpy
typedef struct {
    ppa_out_pic_blk_config_t out;

    uint32_t fill_block_w;
    uint32_t fill_block_h;
    color_pixel_argb8888_data_t fill_argb_color;

    ppa_trans_mode_t mode;
    void *user_data;

    ppa_data_burst_length_t data_burst_length;    // Data burst length for the transaction, information passed from the client
} ppa_fill_oper_t;

/***************************** TRANSACTION ***********************************/

// PPA transaction element
typedef struct ppa_trans_s {
    STAILQ_ENTRY(ppa_trans_s) entry;              // Link entry
    dma2d_trans_config_t *trans_desc;             // Pointer to the structure containing the configurations for a 2D-DMA transaction
    dma2d_trans_t *dma_trans_placeholder;         // Pointer to the memory to store the 2D-DMA transaction context
    SemaphoreHandle_t sem;                        // Semaphore to block when the transaction has not finished
    ppa_client_t *client;                         // Pointer to the client who requested the transaction
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

bool ppa_srm_transaction_on_picked(uint32_t num_chans, const dma2d_trans_channel_info_t *dma2d_chans, void *user_config);
bool ppa_blend_transaction_on_picked(uint32_t num_chans, const dma2d_trans_channel_info_t *dma2d_chans, void *user_config);
bool ppa_fill_transaction_on_picked(uint32_t num_chans, const dma2d_trans_channel_info_t *dma2d_chans, void *user_config);

esp_err_t ppa_do_operation(ppa_client_handle_t ppa_client, ppa_engine_t *ppa_engine_base, ppa_trans_t *trans_elm, ppa_trans_mode_t mode);

bool ppa_transaction_done_cb(dma2d_channel_handle_t dma2d_chan, dma2d_event_data_t *event_data, void *user_data);

bool ppa_recycle_transaction(ppa_client_handle_t ppa_client, ppa_trans_t *trans_elm);

/****************************** PPA DRIVER ***********************************/

struct ppa_platform_t {
    _lock_t mutex;                              // Platform level mutex lock to protect the ppa_engine_acquire/ppa_engine_release process
    portMUX_TYPE spinlock;                      // Platform level spinlock
    ppa_hal_context_t hal;                      // PPA HAL context
    dma2d_pool_handle_t dma2d_pool_handle;      // Pointer to the acquired 2D-DMA pool
    ppa_srm_engine_t *srm;                      // Pointer to the PPA SRM engine
    ppa_blend_engine_t *blending;               // Pointer to the PPA blending engine
    uint32_t srm_engine_ref_count;              // Reference count used to protect PPA SRM engine acquire and release
    uint32_t blend_engine_ref_count;            // Reference count used to protect PPA blending engine acquire and release
    size_t buf_alignment_size;                  // Alignment requirement for the outgoing buffer addr and size to satisfy cache line size
    uint32_t dma_desc_mem_size;                 // Alignment requirement for the 2D-DMA descriptor to satisfy cache line size
};

#ifdef __cplusplus
}
#endif
