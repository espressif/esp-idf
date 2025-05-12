/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <esp_types.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "esp_cam_ctlr_csi.h"
#include "hal/mipi_csi_hal.h"
#include "hal/mipi_csi_types.h"
#include "soc/soc_caps.h"
#include "esp_private/dw_gdma.h"

#if CONFIG_PM_ENABLE
#include "esp_pm.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CSI_FSM_INIT = 1,
    CSI_FSM_ENABLED,
    CSI_FSM_STARTED,
} csi_fsm_t;

/*---------------------------------------------------------------
            Driver Context
---------------------------------------------------------------*/
typedef struct csi_controller_t csi_controller_t;

struct csi_controller_t {
    int                         csi_id;             //csi id
#if SOC_ISP_SHARE_CSI_BRG
    int                         csi_brg_id;         //csi bridge id
    void                        *csi_brg_hw;        //csi bridge hardware context
    bool                        csi_brg_in_use;     //csi bridge is in use
#endif
    mipi_csi_hal_context_t      hal;                //hal context
    csi_fsm_t                   csi_fsm;            //driver fsm
    portMUX_TYPE                spinlock;           //spinlock
    color_space_pixel_format_t  in_color_format;    //input color format
    color_space_pixel_format_t  out_color_format;   //output color format
    uint32_t                    h_res;              //input horizontal resolution
    uint32_t                    v_res;              //input vertical resolution
    int                         in_bpp;             //input data type, bit per pixel
    int                         out_bpp;            //output data type, bit per pixel
    size_t                      fb_size_in_bytes;   //Frame buffer size, in bytes
    esp_cam_ctlr_trans_t        trans;              //Saved done transaction to be given out to callers
    void                        *backup_buffer;     //backup buffer to make csi bridge can work to avoid wrong state
    bool                        bk_buffer_exposed;  //status of if back_buffer is exposed to users
    bool                        bk_buffer_dis;      //Allow to not malloc backup_buffer
    QueueHandle_t               trans_que;          //transaction queue
    esp_cam_ctlr_evt_cbs_t      cbs;                //user callbacks
    void                        *cbs_user_data;     //callback userdata
    dw_gdma_channel_handle_t    dma_chan;           //dwgdma channel handle
    size_t                      csi_transfer_size;  //csi transfer size for dwgdma
#if CONFIG_PM_ENABLE
    esp_pm_lock_handle_t        pm_lock;            //Power management lock
#endif
    esp_cam_ctlr_t base;
};

#ifdef __cplusplus
}
#endif
