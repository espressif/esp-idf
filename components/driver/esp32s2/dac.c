/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_pm.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/rtc_io.h"
#include "driver/dac.h"
#include "soc/dac_periph.h"
#include "soc/lldesc.h"
#include "soc/system_reg.h"
#include "soc/periph_defs.h"
#include "soc/cp_dma_reg.h"
#include "hal/dac_hal.h"
#include "periph_ctrl.h"
#include "sys/queue.h"

static __attribute__((unused)) const char *TAG = "DAC";

extern portMUX_TYPE rtc_spinlock; //TODO: Will be placed in the appropriate position after the rtc module is finished.
#define DAC_ENTER_CRITICAL()  portENTER_CRITICAL(&rtc_spinlock)
#define DAC_EXIT_CRITICAL()  portEXIT_CRITICAL(&rtc_spinlock)
portMUX_TYPE dac_isr_handler_list_lock = portMUX_INITIALIZER_UNLOCKED;
#define DAC_ENTER_CRITICAL_ISR()    portENTER_CRITICAL_ISR(&dac_isr_handler_list_lock)
#define DAC_EXIT_CRITICAL_ISR()     portEXIT_CRITICAL_ISR(&dac_isr_handler_list_lock)

typedef struct {
    uint32_t int_msk;
    uint8_t *data;
    uint32_t data_len;
} dac_dma_event_t;

typedef struct {
    QueueHandle_t que_dac_hdl;                 /*!< DAC queue handler */
    uint32_t dma_buffer_cnt;                   /*!< DMA buffer count, number of buffer. */
    uint32_t dma_buffer_length;                /*!< DMA buffer length, length of each buffer. */
    lldesc_t **desc;                           /*!< Pointer to DMA descriptor*/
    bool dac_start_en;                         /*!< The status of the DAC, 0: stop, 1: start */
    dac_dma_link_type_t dac_dma_link_type;     /*!< The type of the link, see `dac_dma_link_type_t` */
    esp_pm_lock_handle_t pm_lock;              /*!< Spinlock for DAC */
    intr_handle_t dac_isr_handle;              /*!< DAC interrupt handler */
    uint32_t dac_isr;                          /*!< DAC interrupt mask */
} dac_digi_context_t;

dac_digi_context_t *s_dac_digi_ctx = NULL;

/*---------------------------------------------------------------
                        INTERRUPT HANDLER
---------------------------------------------------------------*/

typedef struct dac_dma_isr_handler {
    uint32_t mask;
    intr_handler_t handler;
    void* handler_arg;
    SLIST_ENTRY(dac_dma_isr_handler) next;
} dac_dma_isr_handler_t;

static SLIST_HEAD(dac_dma_isr_handler_list_, dac_dma_isr_handler) s_dac_dma_isr_handler_list =
        SLIST_HEAD_INITIALIZER(s_dac_dma_isr_handler_list);

static IRAM_ATTR void dac_dma_isr(void * arg)
{
    uint32_t int_st = dac_hal_digi_read_intr_status();
    int task_awoken = pdFALSE;
    dac_dma_event_t dac_evt;
    dac_evt.int_msk = int_st;
    REG_WRITE(SPI_DMA_INT_CLR_REG(3), int_st);
    xQueueSendFromISR(s_dac_digi_ctx->que_dac_hdl, &dac_evt, &task_awoken);

    if (task_awoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

static IRAM_ATTR void dac_dma_isr_default(void* arg)
{
    uint32_t status = dac_hal_digi_read_intr_status();
    dac_dma_isr_handler_t* it;
    DAC_ENTER_CRITICAL_ISR();
    SLIST_FOREACH(it, &s_dac_dma_isr_handler_list, next) {
        if (it->mask & status) {
            DAC_EXIT_CRITICAL_ISR();
            (*it->handler)(it->handler_arg);
            DAC_ENTER_CRITICAL_ISR();
        }
    }
    DAC_EXIT_CRITICAL_ISR();
    dac_hal_digi_clear_intr(status);
}

static esp_err_t dac_dma_isr_ensure_installed(void)
{
    esp_err_t err = ESP_OK;
    DAC_ENTER_CRITICAL_ISR();
    dac_hal_digi_clear_intr(UINT32_MAX);
    dac_hal_digi_enable_intr(0);
    err = esp_intr_alloc(ETS_SPI3_DMA_INTR_SOURCE, 0, &dac_dma_isr_default, NULL, &s_dac_digi_ctx->dac_isr_handle);
    if (err != ESP_OK) {
        goto out;
    }

out:
    DAC_EXIT_CRITICAL_ISR();
    return err;
}

esp_err_t dac_dma_isr_register(intr_handler_t handler, void* handler_arg, uint32_t intr_mask)
{
    esp_err_t err = dac_dma_isr_ensure_installed();
    if (err != ESP_OK) {
        return err;
    }

    dac_dma_isr_handler_t* item = malloc(sizeof(*item));
    if (item == NULL) {
        return ESP_ERR_NO_MEM;
    }
    item->handler = handler;
    item->handler_arg = handler_arg;
    item->mask = intr_mask;
    DAC_ENTER_CRITICAL_ISR();
    SLIST_INSERT_HEAD(&s_dac_dma_isr_handler_list, item, next);
    DAC_EXIT_CRITICAL_ISR();
    return ESP_OK;
}

esp_err_t dac_dma_isr_deregister(intr_handler_t handler, void* handler_arg)
{
    dac_dma_isr_handler_t* it;
    dac_dma_isr_handler_t* prev = NULL;
    bool found = false;
    esp_intr_free(s_dac_digi_ctx->dac_isr_handle);

    DAC_ENTER_CRITICAL_ISR();
    SLIST_FOREACH(it, &s_dac_dma_isr_handler_list, next) {
        if (it->handler == handler && it->handler_arg == handler_arg) {
            if (it == SLIST_FIRST(&s_dac_dma_isr_handler_list)) {
                SLIST_REMOVE_HEAD(&s_dac_dma_isr_handler_list, next);
            } else {
                SLIST_REMOVE_AFTER(prev, next);
            }
            found = true;
            free(it);
            break;
        }
        prev = it;
    }
    DAC_EXIT_CRITICAL_ISR();
    return found ? ESP_OK : ESP_ERR_INVALID_STATE;
}

void dac_dma_linker_stop(void)
{
    dac_hal_dma_disable();
}

void dac_dma_linker_deinit(void)
{
    dac_dma_linker_stop();
    dac_hal_digi_clear_intr(UINT32_MAX);
    dac_hal_digi_enable_intr(0);
}

/*---------------------------------------------------------------
                    Digital controller setting
---------------------------------------------------------------*/

esp_err_t __attribute__((unused)) dac_digi_init(void)
{
    s_dac_digi_ctx = calloc(1, sizeof(dac_digi_context_t));
    s_dac_digi_ctx->dac_start_en = false;
    DAC_ENTER_CRITICAL();
    dac_hal_digi_init();
    DAC_EXIT_CRITICAL();

    return ESP_OK;
}

static lldesc_t** dac_dma_desc_buf_create(int desc_cnt, size_t buf_size, const void *data)
{
    lldesc_t** pdesc = (lldesc_t**)heap_caps_calloc(1, sizeof(lldesc_t*) * desc_cnt, MALLOC_CAP_DMA);
    if (pdesc == NULL) {
        goto _exit;
    }
    for (int i = 0; i < desc_cnt; i++) {
        pdesc[i] = (lldesc_t*)malloc(sizeof(lldesc_t));
        if (pdesc[i] == NULL) {
            goto _exit;
        }
        memset(pdesc[i], 0, sizeof(lldesc_t));
    }
    for (int bux_idx = 0; bux_idx < desc_cnt; bux_idx++) {
        pdesc[bux_idx]->owner = 1;
        pdesc[bux_idx]->eof = 1;
        pdesc[bux_idx]->length = buf_size;
        pdesc[bux_idx]->size = buf_size;
        pdesc[bux_idx]->buf = (uint8_t *) data;
        data += buf_size;
        if (s_dac_digi_ctx->dac_dma_link_type == DAC_DMA_LINK_RECURSIVE) {
            WRITE_PERI_REG(CP_DMA_OUTLINK_START, 1);
            pdesc[bux_idx]->qe.stqe_next = ((bux_idx < (desc_cnt - 1)) ? (pdesc[bux_idx + 1]) : pdesc[0]);
        } else {
            pdesc[bux_idx]->qe.stqe_next = ((bux_idx < (desc_cnt - 1)) ? (pdesc[bux_idx + 1]) : NULL);
        }

    }
    return pdesc;
_exit:
    for (int i = 0; i < desc_cnt; i++) {
        free(pdesc[i]);
    }
    free(pdesc);
    return NULL;
}

esp_err_t dac_digi_initialize(const dac_digi_config_t *init_cfg)
{
    DAC_CHECK(init_cfg->mode < DAC_CONV_MAX, "DAC mode error", ESP_ERR_INVALID_ARG);
    DAC_CHECK(init_cfg->interval > 0 && init_cfg->interval < 4096, "DAC interval error", ESP_ERR_INVALID_ARG);
    DAC_CHECK(init_cfg->dig_clk.div_num < 256, "DAC clk div_num error", ESP_ERR_INVALID_ARG);
    DAC_CHECK(init_cfg->dig_clk.div_b > 0 && init_cfg->dig_clk.div_b < 64, "DAC clk div_b error", ESP_ERR_INVALID_ARG);
    DAC_CHECK(init_cfg->dig_clk.div_a < 64, "DAC clk div_a error", ESP_ERR_INVALID_ARG);

    esp_err_t err = ESP_OK;
    if (s_dac_digi_ctx != NULL) {
        ESP_LOGE(DAC_TAG, "DAC has been installed");
        err =  ESP_FAIL;
        goto _exit;
    }

    s_dac_digi_ctx = calloc(1, sizeof(dac_digi_context_t));
    s_dac_digi_ctx->dac_start_en = false;
    if(s_dac_digi_ctx == NULL){
        err = ESP_ERR_NO_MEM;
        goto _exit;
    }

#ifdef CONFIG_PM_ENABLE
    if (s_dac_digi_ctx->pm_lock == NULL) {
        if (init_cfg->dig_clk.use_apll) {
            err = esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "dac_dma", &s_dac_digi_ctx->pm_lock);
        } else {
            err = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "dac_dma", &s_dac_digi_ctx->pm_lock);
        }
        if (err != ESP_OK) {
            s_dac_digi_ctx->pm_lock = NULL;
            ESP_LOGE(DAC_TAG, "DAC-DMA pm lock error");
            goto _exit;
        }
    }
#endif //CONFIG_PM_ENABLE

    if (s_dac_digi_ctx->que_dac_hdl == NULL) {
        s_dac_digi_ctx->que_dac_hdl = xQueueCreate(5, sizeof(dac_dma_event_t));
    } else {
        xQueueReset(s_dac_digi_ctx->que_dac_hdl);
    }
    periph_module_enable(PERIPH_SPI3_DMA_MODULE);
    periph_module_enable(PERIPH_HSPI_MODULE);
    periph_module_enable(PERIPH_SARADC_MODULE);

    dac_hal_digi_controller_config(init_cfg);
    s_dac_digi_ctx->dma_buffer_cnt = init_cfg->dac_dma_cnt;
    s_dac_digi_ctx->dma_buffer_length = init_cfg->dac_dma_length;
    s_dac_digi_ctx->dac_dma_link_type = init_cfg->dac_dma_link_type;
    s_dac_digi_ctx->dac_isr = SPI_OUT_EOF_INT_ENA | SPI_OUT_TOTAL_EOF_INT_ENA;
    dac_dma_isr_register(dac_dma_isr, NULL, s_dac_digi_ctx->dac_isr);

    return err;
_exit:
    dac_digi_deinitialize();
    return err;
}

esp_err_t dac_digi_write_bytes(const void *buffer)
{
    dac_hal_digi_clear_intr(UINT32_MAX);
    dac_hal_digi_enable_intr(SPI_LL_INTR_OUT_TOTAL_EOF | SPI_LL_INTR_OUT_EOF);
    dac_dma_event_t dac_evt;
    s_dac_digi_ctx->desc = dac_dma_desc_buf_create(s_dac_digi_ctx->dma_buffer_cnt, s_dac_digi_ctx->dma_buffer_length, (uint32_t*)buffer);

    dac_hal_dma_reset(0);
    dac_hal_dma_fifo_reset();

    dac_hal_dma_start(0, s_dac_digi_ctx->desc[0]);
    while (s_dac_digi_ctx->dac_isr) {
        xQueueReceive(s_dac_digi_ctx->que_dac_hdl, &dac_evt, 2000 / portTICK_RATE_MS);
        ESP_LOGV(DAC_TAG, "DAC-DMA intr type 0x%x", dac_evt.int_msk);
        if (dac_evt.int_msk & s_dac_digi_ctx->dac_isr) {
            s_dac_digi_ctx->dac_isr &= (~dac_evt.int_msk);
        }
    }
    return ESP_OK;
}

esp_err_t dac_digi_deinitialize(void)
{
    if (!s_dac_digi_ctx) {
        return ESP_ERR_INVALID_STATE;
    }

    if (s_dac_digi_ctx->dac_start_en == true) {
        ESP_LOGE(DAC_TAG, "DAC is still working");
        return ESP_ERR_INVALID_STATE;
    }
    periph_module_disable(PERIPH_SPI3_DMA_MODULE);
    periph_module_disable(PERIPH_HSPI_MODULE);
    periph_module_disable(PERIPH_SARADC_MODULE);

    dac_dma_linker_deinit();
    dac_dma_isr_deregister(dac_dma_isr, NULL);

    if (s_dac_digi_ctx->que_dac_hdl) {
        vQueueDelete(s_dac_digi_ctx->que_dac_hdl);
        s_dac_digi_ctx->que_dac_hdl = NULL;
    }

    for (int i = 0; i < s_dac_digi_ctx->dma_buffer_cnt; i++) {
        free(s_dac_digi_ctx->desc[i]);
    }

    free(s_dac_digi_ctx);
    s_dac_digi_ctx = NULL;
#ifdef CONFIG_PM_ENABLE
    if (s_dac_digi_ctx->pm_lock) {
        esp_pm_lock_delete(s_dac_digi_ctx->pm_lock);
        s_dac_digi_ctx->pm_lock = NULL;
    }
#endif

    DAC_ENTER_CRITICAL();
    dac_hal_digi_deinit();
    DAC_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t __attribute__((unused)) dac_digi_deinit(void)
{
#ifdef CONFIG_PM_ENABLE
    if (s_dac_digi_ctx->pm_lock) {
        esp_pm_lock_delete(s_dac_digi_ctx->pm_lock);
        s_dac_digi_ctx->pm_lock = NULL;
    }
#endif
    free(s_dac_digi_ctx);
    DAC_ENTER_CRITICAL();
    dac_hal_digi_deinit();
    DAC_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t __attribute__((unused)) dac_digi_controller_config(const dac_digi_config_t *cfg)
{
    ESP_RETURN_ON_FALSE(cfg->mode < DAC_CONV_MAX, ESP_ERR_INVALID_ARG, TAG, "DAC mode error");
    ESP_RETURN_ON_FALSE(cfg->interval > 0 && cfg->interval < 4096, ESP_ERR_INVALID_ARG, TAG, "DAC interval error");
    ESP_RETURN_ON_FALSE(cfg->dig_clk.div_num < 256, ESP_ERR_INVALID_ARG, TAG, "DAC clk div_num error");
    ESP_RETURN_ON_FALSE(cfg->dig_clk.div_b > 0 && cfg->dig_clk.div_b < 64, ESP_ERR_INVALID_ARG, TAG, "DAC clk div_b error");
    ESP_RETURN_ON_FALSE(cfg->dig_clk.div_a < 64, ESP_ERR_INVALID_ARG, TAG, "DAC clk div_a error");
#ifdef CONFIG_PM_ENABLE
    esp_err_t err;
    if (s_dac_digi_ctx->pm_lock == NULL) {
        if (cfg->dig_clk.use_apll) {
            err = esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "dac_dma", &s_dac_digi_ctx->pm_lock);
        } else {
            err = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "dac_dma", &s_dac_digi_ctx->pm_lock);
        }
        if (err != ESP_OK) {
            s_dac_digi_ctx->pm_lock = NULL;
            ESP_LOGE(DAC_TAG, "DAC-DMA pm lock error");
            return err;
        }
    }
#endif //CONFIG_PM_ENABLE

    DAC_ENTER_CRITICAL();
    dac_hal_digi_controller_config(cfg);
    DAC_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t dac_digi_start(void)
{
    if (s_dac_digi_ctx->dac_start_en == true) {
        ESP_LOGE(DAC_TAG, "DAC is already started");
        return ESP_ERR_INVALID_STATE;
    }
    s_dac_digi_ctx->dac_start_en = true;
#ifdef CONFIG_PM_ENABLE
    DAC_CHECK((s_dac_digi_ctx->pm_lock), "Should start after call `dac_digi_controller_config`", ESP_FAIL);
    esp_pm_lock_acquire(s_dac_digi_ctx->pm_lock);
#endif
    DAC_ENTER_CRITICAL();
    dac_hal_digi_start();
    DAC_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t dac_digi_stop(void)
{
    if (s_dac_digi_ctx->dac_start_en == false) {
        ESP_LOGE(DAC_TAG, "DAC is already stopped");
        return ESP_ERR_INVALID_STATE;
    }
    s_dac_digi_ctx->dac_start_en = false;
#ifdef CONFIG_PM_ENABLE
    if (s_dac_digi_ctx->pm_lock) {
        esp_pm_lock_release(s_dac_digi_ctx->pm_lock);
    }
#endif
    DAC_ENTER_CRITICAL();
    dac_hal_digi_stop();
    DAC_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t dac_digi_fifo_reset(void)
{
    DAC_ENTER_CRITICAL();
    dac_hal_digi_fifo_reset();
    DAC_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t dac_digi_reset(void)
{
    DAC_ENTER_CRITICAL();
    dac_hal_digi_reset();
    DAC_EXIT_CRITICAL();

    return ESP_OK;
}
