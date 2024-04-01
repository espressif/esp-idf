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
#include "hal/isp_hal.h"
#include "hal/isp_types.h"
#include "soc/isp_periph.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ISP_FSM_INIT,
    ISP_FSM_ENABLE,
} isp_fsm_t;

/*---------------------------------------------------------------
            Driver Context
---------------------------------------------------------------*/
typedef enum {
    ISP_SUBMODULE_AF,
} isp_submodule_t;

typedef struct isp_af_controller_t isp_af_controller_t;
typedef struct isp_processor_t isp_processor_t;

struct isp_af_controller_t {
    int                                id;
    isp_fsm_t                          fsm;
    portMUX_TYPE                       spinlock;
    isp_processor_t                    *isp_proc;
    esp_isp_af_env_config_t            config;
    esp_isp_af_env_detector_evt_cbs_t  cbs;
    void                               *user_data;
};

struct isp_processor_t {
    int                  proc_id;
    isp_hal_context_t    hal;
#if SOC_ISP_SHARE_CSI_BRG
    int                  csi_brg_id;
    void                 *csi_brg_hw;
#endif
    isp_fsm_t            isp_fsm;
    portMUX_TYPE         spinlock;
    intr_handle_t        intr_hdl;

    /* sub module contexts */
    isp_af_controller_t  *af_ctlr[SOC_ISP_AF_CTLR_NUMS];

    /* should be accessed within isp_processor_t spinlock */
    int                  isr_ref_counts;
    bool                 af_isr_added;
};

/*---------------------------------------------------------------
                      INTR
---------------------------------------------------------------*/
esp_err_t esp_isp_register_isr(isp_proc_handle_t proc, isp_submodule_t submodule);
esp_err_t esp_isp_deregister_isr(isp_proc_handle_t proc, isp_submodule_t submodule);
bool esp_isp_af_isr(isp_proc_handle_t proc, uint32_t af_events);

#ifdef __cplusplus
}
#endif
