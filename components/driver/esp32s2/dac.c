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
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
#include "driver/rtc_io.h"
#include "driver/dac.h"
#include "soc/dac_periph.h"
#include "soc/lldesc.h"
#include "hal/dac_hal.h"
#include "periph_ctrl.h"
#include "driver/spi_common_internal.h"

static const char *DAC_TAG = "DAC";

/* On ESP32-S2, DAC-DMA shares the SPI3-DMA channel */
#define DAC_USE_SPI3_DMA_CHANNEL  SPI3_HOST

extern portMUX_TYPE rtc_spinlock; //TODO: Will be placed in the appropriate position after the rtc module is finished.
#define DAC_ENTER_CRITICAL()  portENTER_CRITICAL(&rtc_spinlock)
#define DAC_EXIT_CRITICAL()  portEXIT_CRITICAL(&rtc_spinlock)

typedef struct {
    bool intr_trigger;
    uint8_t *data;
    uint32_t data_len;
} dac_dma_event_t;

typedef struct {
    dac_hal_context_t *hal;                    /*!< HAL pointer of DAC */
    QueueHandle_t que_dac_hdl;                 /*!< DAC queue handler */
    uint32_t dma_buffer_cnt;                   /*!< DMA buffer count, number of buffer. */
    uint32_t dma_buffer_length;                /*!< DMA buffer length, length of each buffer. */
    lldesc_t **desc;                           /*!< Pointer to DMA descriptor*/
    bool dac_start_en;                         /*!< The status of the DAC, 0: stop, 1: start */
    dac_dma_link_type_t dac_dma_link_type;     /*!< The type of the link, see `dac_dma_link_type_t` */
    esp_pm_lock_handle_t pm_lock;              /*!< Spinlock for DAC */
    spi_host_device_t spi_host;                /*!< spi host */
    intr_handle_t dac_isr_handle;              /*!< DAC interrupt handler */
} dac_digi_context_t;

static dac_digi_context_t *s_dac_digi_ctx = NULL;

/*---------------------------------------------------------------
                        INTERRUPT HANDLER
---------------------------------------------------------------*/

static IRAM_ATTR void dac_dma_isr_default(void *arg)
{
    dac_digi_context_t *p_dac = (dac_digi_context_t *) arg;
    bool status =  dac_dma_hal_get_intr_status(p_dac->hal);
    dac_dma_hal_clr_intr(p_dac->hal);
    int task_awoken = pdFALSE;
    dac_dma_event_t dac_evt;
    dac_evt.intr_trigger = status;
    xQueueSendFromISR(s_dac_digi_ctx->que_dac_hdl, &dac_evt, &task_awoken);
}

/*---------------------------------------------------------------
                    Digital controller setting
---------------------------------------------------------------*/
static lldesc_t** dac_dma_desc_buf_create(int desc_cnt, size_t buf_size)
{
    lldesc_t** pdesc = (lldesc_t**)heap_caps_calloc(1, sizeof(lldesc_t*) * desc_cnt, MALLOC_CAP_DMA);
    if (pdesc == NULL) {
        goto _exit;
    }
    for (int i = 0; i < desc_cnt; i++) {
        pdesc[i] = (lldesc_t*)heap_caps_malloc(sizeof(lldesc_t), MALLOC_CAP_DMA);
        if (pdesc[i] == NULL) {
            goto _exit;
        }
        memset(pdesc[i], 0, sizeof(lldesc_t));
    }
    return pdesc;
_exit:
    for (int i = 0; i < desc_cnt; i++) {
        free(pdesc[i]);
    }
    free(pdesc);
    return NULL;
}

static void dac_dma_write(lldesc_t** pdesc, int desc_cnt, size_t buf_size, const void *buffer)
{
    for (int bux_idx = 0; bux_idx < desc_cnt; bux_idx++) {
        pdesc[bux_idx]->owner = 1;
        pdesc[bux_idx]->eof = 1;
        pdesc[bux_idx]->length = buf_size;
        pdesc[bux_idx]->size = buf_size;
        pdesc[bux_idx]->buf = (uint8_t *) buffer;
        buffer += buf_size;
        if (s_dac_digi_ctx->dac_dma_link_type == DAC_DMA_LINK_RECURSIVE) {
            pdesc[bux_idx]->qe.stqe_next = ((bux_idx < (desc_cnt - 1)) ? (pdesc[bux_idx + 1]) : pdesc[0]);
        } else {
            pdesc[bux_idx]->qe.stqe_next = ((bux_idx < (desc_cnt - 1)) ? (pdesc[bux_idx + 1]) : NULL);
        }
    }
}

esp_err_t dac_digi_initialize(const dac_digi_init_config_t *init_cfg)
{
    ESP_RETURN_ON_FALSE(init_cfg->mode < DAC_CONV_MAX, ESP_ERR_INVALID_ARG, DAC_TAG, "DAC mode error");
    ESP_RETURN_ON_FALSE((init_cfg->interval > 0) && (init_cfg->interval < 4096), ESP_ERR_INVALID_ARG, DAC_TAG, "DAC interval error");

    esp_err_t err = ESP_OK;
    uint32_t dma_chan = 0;
    uint32_t dac_chan = 0;
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
        err = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "dac_dma", &s_dac_digi_ctx->pm_lock);
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

    periph_module_enable(PERIPH_SARADC_MODULE);

    do {
        if ((init_cfg->dac_chan_msk >> dac_chan) & BIT(0)) {
            dac_output_enable(dac_chan);
            dac_chan++;
        }
    } while (dac_chan < DAC_CHANNEL_MAX);

    dac_hal_ctrl_config_t ctrl_hal_cfg = {
        .mode = init_cfg->mode,
        .interval = init_cfg->interval,
    };

    dac_hal_digi_controller_configure(&ctrl_hal_cfg);
    s_dac_digi_ctx->dma_buffer_cnt = init_cfg->dac_dma_cnt;
    s_dac_digi_ctx->dma_buffer_length = init_cfg->dac_dma_length;
    s_dac_digi_ctx->dac_dma_link_type = init_cfg->dac_dma_link_type;
    s_dac_digi_ctx->spi_host = DAC_USE_SPI3_DMA_CHANNEL;

    spicommon_periph_claim(s_dac_digi_ctx->spi_host, "dac");
    err = spicommon_slave_dma_chan_alloc(s_dac_digi_ctx->spi_host, SPI_DMA_CH_AUTO, &dma_chan, &dma_chan);
    if (err != ESP_OK) {
        goto _exit;
    }

    s_dac_digi_ctx->hal = calloc(1, sizeof(dac_hal_context_t));
    s_dac_digi_ctx->hal->dev = (void *)SPI_LL_GET_HW(s_dac_digi_ctx->spi_host);
    s_dac_digi_ctx->hal->dma_chan = dma_chan;

    dac_dma_hal_init(s_dac_digi_ctx->hal);
    err = esp_intr_alloc(spicommon_irqdma_source_for_host(s_dac_digi_ctx->spi_host), 0, dac_dma_isr_default, s_dac_digi_ctx, &s_dac_digi_ctx->dac_isr_handle);
    if(err != ESP_OK){
        goto _exit;
    }
    s_dac_digi_ctx->desc = dac_dma_desc_buf_create(s_dac_digi_ctx->dma_buffer_cnt, s_dac_digi_ctx->dma_buffer_length);

    return err;
_exit:
    dac_digi_deinitialize();
    return err;
}

esp_err_t dac_digi_write_bytes(uint32_t length, const void *buffer, TickType_t ticks_to_wait)
{
    ESP_RETURN_ON_FALSE(length <= (s_dac_digi_ctx->dma_buffer_cnt * s_dac_digi_ctx->dma_buffer_length), ESP_ERR_INVALID_ARG, DAC_TAG, "DAC DMA buffer length is larger than DMA buffer.");
    dac_dma_event_t dac_evt;
    bool dac_isr_flag = 1;


    dac_dma_write(s_dac_digi_ctx->desc, s_dac_digi_ctx->dma_buffer_cnt, s_dac_digi_ctx->dma_buffer_length, buffer);

    dac_dma_hal_trans_start(s_dac_digi_ctx->hal, s_dac_digi_ctx->desc[0]);
    while (dac_isr_flag) {
        xQueueReceive(s_dac_digi_ctx->que_dac_hdl, &dac_evt, ticks_to_wait);
        if (dac_evt.intr_trigger & dac_isr_flag) {
            dac_isr_flag &= (!dac_evt.intr_trigger);
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
    periph_module_disable(PERIPH_SARADC_MODULE);

    spicommon_periph_free(s_dac_digi_ctx->spi_host);
    spicommon_slave_free_dma(s_dac_digi_ctx->spi_host);


    if (s_dac_digi_ctx->que_dac_hdl) {
        vQueueDelete(s_dac_digi_ctx->que_dac_hdl);
        s_dac_digi_ctx->que_dac_hdl = NULL;
    }

    dac_dma_hal_deinit(s_dac_digi_ctx->hal);

    for (int i = 0; i < s_dac_digi_ctx->dma_buffer_cnt; i++) {
        free(s_dac_digi_ctx->desc[i]);
    }

    free(s_dac_digi_ctx->hal);
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
    ESP_RETURN_ON_FALSE((s_dac_digi_ctx->pm_lock), ESP_ERR_INVALID_STATE, DAC_TAG, "Should start after call `dac_digi_controller_config`");
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

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
esp_err_t __attribute__((deprecated)) dac_digi_init(void)
{
    s_dac_digi_ctx = calloc(1, sizeof(dac_digi_context_t));
    s_dac_digi_ctx->dac_start_en = false;
    DAC_ENTER_CRITICAL();
    dac_hal_digi_init();
    DAC_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t __attribute__((deprecated)) dac_digi_deinit(void)
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

esp_err_t __attribute__((deprecated)) dac_digi_controller_config(const dac_digi_config_t *cfg)
{
    ESP_RETURN_ON_FALSE(cfg->mode < DAC_CONV_MAX, ESP_ERR_INVALID_ARG, DAC_TAG, "DAC mode error");
    ESP_RETURN_ON_FALSE(cfg->interval > 0 && cfg->interval < 4096, ESP_ERR_INVALID_ARG, DAC_TAG, "DAC interval error");
    ESP_RETURN_ON_FALSE(cfg->dig_clk.div_num < 256, ESP_ERR_INVALID_ARG, DAC_TAG, "DAC clk div_num error");
    ESP_RETURN_ON_FALSE(cfg->dig_clk.div_b > 0 && cfg->dig_clk.div_b < 64, ESP_ERR_INVALID_ARG, DAC_TAG, "DAC clk div_b error");
    ESP_RETURN_ON_FALSE(cfg->dig_clk.div_a < 64, ESP_ERR_INVALID_ARG, DAC_TAG, "DAC clk div_a error");
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
