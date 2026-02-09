/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include <sys/param.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_intr_alloc.h"
#include "hal/mspi_ll.h"
#include "hal/mspi_periph.h"
#include "esp_private/startup_internal.h"
#include "esp_private/mspi_intr.h"

#if MSPI_LL_INTR_EVENT_SUPPORTED && MSPI_LL_INTR_SHARED

#if CONFIG_ESP_PANIC_HANDLER_IRAM
#define MSPI_ISR_ATTR  IRAM_ATTR
#define MSPI_ISR_FLAGS ESP_INTR_FLAG_IRAM
#else
#define MSPI_ISR_ATTR
#define MSPI_ISR_FLAGS 0
#endif

ESP_LOG_ATTR_TAG_DRAM(TAG, "mspi_intr");
static intr_handle_t s_intr_handle = NULL;
static volatile mspi_isr_t s_isr = {
    NULL,
    NULL,
};

static void MSPI_ISR_ATTR mspi_isr_handler(void *arg)
{
    uint32_t intr_events = mspi_ll_get_intr_raw(MSPI_TIMING_LL_MSPI_ID_0);
    mspi_ll_clear_intr(MSPI_TIMING_LL_MSPI_ID_0, intr_events);

    ESP_DRAM_LOGE(TAG, "MSPI error");
    ESP_DRAM_LOGD(TAG, "intr_events: 0x%" PRIx32, intr_events);

    bool is_ecc_error = false;

#if MSPI_LL_ECC_INT_SUPPORTED
    if (intr_events & MSPI_LL_EVENT_ECC_ERR) {
        ESP_DRAM_LOGD(TAG, "ecc error");
        is_ecc_error = true;
    }
#endif
#if MSPI_LL_PMS_INT_SUPPORTED
    if (intr_events & MSPI_LL_EVENT_PMS_REJECT) {
        ESP_DRAM_LOGE(TAG, "pms reject");
    }
#endif
#if MSPI_LL_ADDR_INT_SUPPORTED
    if (intr_events & MSPI_LL_EVENT_AXI_RADDR_ERR) {
        ESP_DRAM_LOGE(TAG, "read addr error");
    }
    if (intr_events & MSPI_LL_EVENT_AXI_WADDR_ERR) {
        ESP_DRAM_LOGE(TAG, "write addr error");
    }
    if (intr_events & MSPI_LL_EVENT_AXI_WR_FLASH_ERR) {
        ESP_DRAM_LOGE(TAG, "write flash error");
    }
#endif
#if MSPI_LL_THRESH_INT_SUPPORTED
    if (intr_events & MSPI_LL_EVENT_RX_TRANS_OVF) {
        ESP_DRAM_LOGE(TAG, "rx trans overflow");
    }
    if (intr_events & MSPI_LL_EVENT_TX_TRANS_UDF) {
        ESP_DRAM_LOGE(TAG, "tx trans underflow");
    }
#endif

    if (s_isr.psram_isr) {
        s_isr.psram_isr(arg, intr_events);
    }

    if (s_isr.flash_isr) {
        s_isr.flash_isr(arg, intr_events);
    }

    // For ecc error, will handle in the flash/psram isr
    if (!is_ecc_error) {
        abort();
    }

    //no yield for now
}

esp_err_t esp_mspi_register_isr(mspi_isr_t *isr)
{
    esp_err_t ret = ESP_FAIL;

    if (isr && isr->psram_isr) {
        s_isr.psram_isr = isr->psram_isr;
    }

    if (isr && isr->flash_isr) {
        s_isr.flash_isr = isr->flash_isr;
    }

    if (!s_intr_handle) {
        ret = esp_intr_alloc(mspi_hw_info.instances[MSPI_TIMING_LL_MSPI_ID_0].irq,
                            MSPI_ISR_FLAGS,
                            mspi_isr_handler,
                            NULL,
                            &s_intr_handle);

        ESP_RETURN_ON_ERROR(ret, TAG, "Failed to allocate MSPI flash interrupt");

        mspi_ll_clear_intr(MSPI_TIMING_LL_MSPI_ID_0, MSPI_LL_EVENT_MASK);
        mspi_ll_enable_intr(MSPI_TIMING_LL_MSPI_ID_0, MSPI_LL_EVENT_MASK, true);
    }

    return ESP_OK;
}

esp_err_t esp_mspi_unregister_isr(void)
{
    esp_err_t ret = ESP_FAIL;

    if (s_intr_handle == NULL) {
        ESP_EARLY_LOGE(TAG, "MSPI interrupt not registered");
        return ESP_ERR_INVALID_STATE;
    }

    ret = esp_intr_free(s_intr_handle);
    ESP_RETURN_ON_ERROR(ret, TAG, "Failed to free MSPI interrupt");

    s_isr.psram_isr = NULL;
    s_isr.flash_isr = NULL;

    return ret;
}
#endif  //#if MSPI_LL_INTR_EVENT_SUPPORTED && MSPI_LL_INTR_SHARED
