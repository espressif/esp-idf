/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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
#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/idf_additions.h"
#include "driver/isp_types.h"
#include "soc/soc_caps.h"
#if SOC_ISP_SUPPORTED
#include "hal/isp_hal.h"
#include "hal/isp_ll.h"
#include "hal/isp_types.h"
#include "soc/isp_periph.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_ISP_ISR_IRAM_SAFE || CONFIG_ISP_CTRL_FUNC_IN_IRAM
#define ISP_INTR_ALLOC_FLAGS    (ESP_INTR_FLAG_INTRDISABLED | ESP_INTR_FLAG_IRAM)
#define ISP_MEM_ALLOC_CAPS      (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define ISP_INTR_ALLOC_FLAGS    (ESP_INTR_FLAG_INTRDISABLED)
#define ISP_MEM_ALLOC_CAPS      MALLOC_CAP_DEFAULT
#endif

typedef enum {
    ISP_FSM_INIT,           // Controller is initialized, but not enabled
    ISP_FSM_ENABLE,         // Controller is enabled, but is not running
    ISP_FSM_START,          // Controller is in running
    ISP_FSM_ONESHOT,        // Controller is in oneshot sampling
    ISP_FSM_CONTINUOUS,     // Controller is in continuous sampling
} isp_fsm_t;

#if SOC_ISP_SUPPORTED
/*---------------------------------------------------------------
            Driver Context
---------------------------------------------------------------*/
typedef struct isp_processor_t {
    int                         proc_id;
    isp_hal_context_t           hal;
#if SOC_ISP_SHARE_CSI_BRG
    int                         csi_brg_id;
    void                        *csi_brg_hw;
#endif
    isp_fsm_t                   isp_fsm;
    portMUX_TYPE                spinlock;
    color_space_pixel_format_t  in_color_format;
    color_space_pixel_format_t  out_color_format;
    uint32_t                    h_res;
    uint32_t                    v_res;
    color_raw_element_order_t   bayer_order;
    /* sub module contexts */
    isp_af_ctlr_t               af_ctlr[SOC_ISP_AF_CTLR_NUMS];
    isp_awb_ctlr_t              awb_ctlr;
    isp_ae_ctlr_t               ae_ctlr;
    isp_hist_ctlr_t             hist_ctlr;
    isp_fsm_t                   bf_fsm;
    isp_fsm_t                   demosaic_fsm;
    isp_fsm_t                   sharpen_fsm;
    isp_fsm_t                   color_fsm;
    isp_fsm_t                   lsc_fsm;
    esp_isp_evt_cbs_t           cbs;
    void                        *user_data;

    /* ISR */
    intr_handle_t               intr_hdl;
    int                         intr_priority;
    int                         isr_ref_counts;
    struct {
        uint32_t                af_isr_added:    1;
        uint32_t                ae_isr_added:    1;
        uint32_t                awb_isr_added:   1;
        uint32_t                sharp_isr_added: 1;
        uint32_t                hist_isr_added:  1;
    } isr_users;

} isp_processor_t;
#endif

typedef enum {
    ISP_SUBMODULE_AF,
    ISP_SUBMODULE_AE,
    ISP_SUBMODULE_AWB,
    ISP_SUBMODULE_SHARPEN,
    ISP_SUBMODULE_HIST,
} isp_submodule_t;

/*---------------------------------------------------------------
                      INTR
---------------------------------------------------------------*/
esp_err_t esp_isp_register_isr(isp_proc_handle_t proc, isp_submodule_t submodule);
esp_err_t esp_isp_deregister_isr(isp_proc_handle_t proc, isp_submodule_t submodule);
bool esp_isp_af_isr(isp_proc_handle_t proc, uint32_t af_events);
bool esp_isp_ae_isr(isp_proc_handle_t proc, uint32_t ae_events);
bool esp_isp_awb_isr(isp_proc_handle_t proc, uint32_t awb_events);
bool esp_isp_sharpen_isr(isp_proc_handle_t proc, uint32_t sharp_events);
bool esp_isp_hist_isr(isp_proc_handle_t proc, uint32_t hist_events);

#ifdef __cplusplus
}
#endif
