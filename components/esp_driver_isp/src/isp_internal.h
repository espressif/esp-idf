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
#include "hal/isp_hal.h"
#include "hal/isp_ll.h"
#include "hal/isp_types.h"
#include "soc/isp_periph.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_ISP_ISR_IRAM_SAFE
#define ISP_INTR_ALLOC_FLAGS    (ESP_INTR_FLAG_SHARED | ESP_INTR_FLAG_INTRDISABLED | ESP_INTR_FLAG_IRAM)
#define ISP_MEM_ALLOC_CAPS      (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define ISP_INTR_ALLOC_FLAGS    (ESP_INTR_FLAG_SHARED | ESP_INTR_FLAG_INTRDISABLED)
#define ISP_MEM_ALLOC_CAPS      MALLOC_CAP_DEFAULT
#endif

typedef enum {
    ISP_FSM_INIT,
    ISP_FSM_ENABLE,
    ISP_FSM_START,
} isp_fsm_t;

/*---------------------------------------------------------------
            Driver Context
---------------------------------------------------------------*/
typedef struct isp_processor_t {
    int                  proc_id;
    isp_hal_context_t    hal;
#if SOC_ISP_SHARE_CSI_BRG
    int                  csi_brg_id;
    void                 *csi_brg_hw;
#endif
    isp_fsm_t            isp_fsm;
    portMUX_TYPE         spinlock;

    /* sub module contexts */
    isp_af_ctrlr_t af_ctlr[SOC_ISP_AF_CTLR_NUMS];
} isp_processor_t;

#ifdef __cplusplus
}
#endif
