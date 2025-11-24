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
#include "esp_private/mspi_intr.h"
#if !CONFIG_IDF_TARGET_ESP32 && !CONFIG_IDF_TARGET_ESP32S2
#include "hal/psram_ctrlr_ll.h"
#endif

#if PSRAM_CTRLR_LL_INTR_EVENT_SUPPORTED

#if CONFIG_ESP_PANIC_HANDLER_IRAM
#define PSRAM_ISR_ATTR   IRAM_ATTR
#define PSRAM_ISR_FLAGS  ESP_INTR_FLAG_IRAM
#else
#define PSRAM_ISR_ATTR
#define PSRAM_ISR_FLAGS  0
#endif

ESP_LOG_ATTR_TAG_DRAM(TAG, "psram_mspi");

static void PSRAM_ISR_ATTR mspi_psram_isr_handler(void *arg, uint32_t intr_events)
{
#if PSRAM_CTRLR_LL_PMS_INT_SUPPORTED
    if (intr_events & PSRAM_CTRLR_LL_EVENT_PMS_REJECT) {
        ESP_DRAM_LOGE(TAG, "psram pms reject");
    }
#endif
#if PSRAM_CTRLR_LL_THRESH_INT_SUPPORTED
    if (intr_events & PSRAM_CTRLR_LL_EVENT_RX_TRANS_OVF) {
        ESP_DRAM_LOGE(TAG, "psram rx trans overflow");
    }
    if (intr_events & PSRAM_CTRLR_LL_EVENT_TX_TRANS_UDF) {
        ESP_DRAM_LOGE(TAG, "psram tx trans underflow");
    }
#endif
}

#if !MSPI_LL_INTR_SHARED
/**
 * For PSRAM/FLASH separate MSPI chips, register a PSRAM standalone ISR
 */
__attribute__((__unused__)) static intr_handle_t s_mspi_psram_intr_handle = NULL;

static void PSRAM_ISR_ATTR mspi_psram_isr_handler_wrapper(void *arg)
{
    uint32_t intr_events = psram_ctrlr_ll_get_intr_raw(PSRAM_CTRLR_LL_MSPI_ID_SYSTEM);
    psram_ctrlr_ll_clear_intr(PSRAM_CTRLR_LL_MSPI_ID_SYSTEM, intr_events);

    ESP_DRAM_LOGE(TAG, "MSPI PSRAM error");

    mspi_psram_isr_handler(arg, intr_events);

    abort();
}
#endif

esp_err_t esp_psram_mspi_register_isr(void)
{
    esp_err_t ret = ESP_FAIL;

#if MSPI_LL_INTR_SHARED
    mspi_isr_t isr = {
        .psram_isr = mspi_psram_isr_handler,
    };
    ret = esp_mspi_register_isr(&isr);
#else
    ret = esp_intr_alloc(mspi_hw_info.instances[PSRAM_CTRLR_LL_MSPI_ID_SYSTEM].irq,
                         PSRAM_ISR_FLAGS,
                         mspi_psram_isr_handler_wrapper,
                         NULL,
                         &s_mspi_psram_intr_handle);

    ESP_RETURN_ON_ERROR(ret, TAG, "Failed to allocate MSPI psram interrupt");

    psram_ctrlr_ll_clear_intr(PSRAM_CTRLR_LL_MSPI_ID_SYSTEM, PSRAM_CTRLR_LL_EVENT_MASK);
    psram_ctrlr_ll_enable_intr(PSRAM_CTRLR_LL_MSPI_ID_SYSTEM, PSRAM_CTRLR_LL_EVENT_MASK, true);
#endif

    return ret;
}

esp_err_t esp_psram_mspi_unregister_isr(void)
{
    esp_err_t ret = ESP_FAIL;

#if MSPI_LL_INTR_SHARED
    ret = esp_mspi_unregister_isr();
    ESP_RETURN_ON_ERROR(ret, TAG, "Failed to unregister MSPI psram interrupt");
#else

    if (s_mspi_psram_intr_handle == NULL) {
        ESP_EARLY_LOGE(TAG, "MSPI psram interrupt not registered");
        return ESP_ERR_INVALID_STATE;
    }

    ret = esp_intr_free(s_mspi_psram_intr_handle);
    ESP_RETURN_ON_ERROR(ret, TAG, "Failed to free MSPI psram interrupt");
#endif

    return ESP_OK;
}
#else
esp_err_t esp_psram_mspi_register_isr(void)
{
    return ESP_OK;
}

esp_err_t esp_psram_mspi_unregister_isr(void)
{
    return ESP_OK;
}
#endif  //#if PSRAM_CTRLR_LL_INTR_EVENT_SUPPORTED
