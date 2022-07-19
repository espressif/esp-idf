/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 Tests for the dac device driver on ESP32-S2 only
*/
#include "sdkconfig.h"
#if CONFIG_IDF_TARGET_ESP32S2

#include "esp_system.h"
#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/rtc_io.h"
#include "driver/gpio.h"
#include "unity.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "test_utils.h"
#include "soc/soc.h"
#include "soc/spi_reg.h"
#include "soc/adc_periph.h"
#include "soc/dac_periph.h"
#include "soc/spi_periph.h"
#include "test/test_common_adc.h"
#include "driver/dac.h"
#include "soc/system_reg.h"
#include "esp32s2/rom/lldesc.h"
#include "test/test_adc_dac_dma.h"

static const char *TAG = "test_adc";

#define PLATFORM_SELECT            (1)  //0: pxp; 1: chip
#if (PLATFORM_SELECT == 0)              //PXP platform
#include "soc/syscon_reg.h"
#define SET_BREAK_POINT(flag) REG_WRITE(SYSCON_DATE_REG, flag)
//PXP clk is slower.
#define SYS_DELAY_TIME_MOM    (1/40)
#define RTC_SLOW_CLK_FLAG     1     // Slow clock is 32KHz.
static void test_pxp_deinit_io(void)
{
    for (int i = 0; i < 22; i++) {
        rtc_gpio_init(i);
    }
}
#else
//PXP clk is slower.
#define SET_BREAK_POINT(flag)
#define SYS_DELAY_TIME_MOM    (1)
#define RTC_SLOW_CLK_FLAG     0     // Slow clock is 32KHz.
#endif

#define SAR_SIMPLE_NUM  512  // Set out number of enabled unit.

typedef struct dma_msg {
    uint32_t int_msk;
    uint8_t *data;
    uint32_t data_len;
} dac_dma_event_t;

static QueueHandle_t que_dac = NULL;
static uint8_t link_buf[2][SAR_SIMPLE_NUM*2] = {0};
static lldesc_t dma1 = {0};
static lldesc_t dma2 = {0};

/*******************************************/
/**           DAC-DMA INIT CODE            */
/*******************************************/

/**
 * DMA liner initialization and start.
 * @param is_loop
 *     - true: The two dma linked lists are connected end to end, with no end mark (eof).
 *     - false: The two dma linked lists are connected end to end, with end mark (eof).
 * @param int_mask DMA interrupt types.
 */
uint32_t dac_dma_linker_init(bool is_alter, bool is_loop)
{
    /* The DAC output is a sawtooth wave. */
    if (is_alter) {
        for(int i=0; i<SAR_SIMPLE_NUM*2; i++) {
            if(i%2){
                link_buf[0][i] = i%256;
            }else{
                link_buf[0][i] = 256-i%256;
            }
            if(i%2){
                link_buf[1][i] = i%256;
            }else{
                link_buf[1][i] = 256-i%256;
            }
        }
    } else {
        for(int i=0; i<SAR_SIMPLE_NUM; i++) {
            link_buf[0][i] = i%256;
            link_buf[1][i] = i%256;
        }
    }
    dma1 = (lldesc_t) {
        .size = (is_alter) ? SAR_SIMPLE_NUM*2 : SAR_SIMPLE_NUM,
        .length = (is_alter) ? SAR_SIMPLE_NUM*2 : SAR_SIMPLE_NUM,
        .eof = 0,
        .owner = 1,
        .buf = &link_buf[0][0],
        .qe.stqe_next = &dma2,
    };
    dma2 = (lldesc_t) {
        .size = (is_alter) ? SAR_SIMPLE_NUM*2 : SAR_SIMPLE_NUM,
        .length = (is_alter) ? SAR_SIMPLE_NUM*2 : SAR_SIMPLE_NUM,
        .owner = 1,
        .buf = &link_buf[1][0],
    };
    if (is_loop) {
        dma2.eof = 0;
        dma2.qe.stqe_next = &dma1;
    } else {
        dma2.eof = 1;
        dma2.qe.stqe_next = NULL;
    }
    return (uint32_t)&dma1;
}

/** ADC-DMA ISR handler. */
static IRAM_ATTR void dac_dma_isr(void * arg)
{
    uint32_t int_st = REG_READ(SPI_DMA_INT_ST_REG(3));
    int task_awoken = pdFALSE;
    dac_dma_event_t adc_evt;
    adc_evt.int_msk = int_st;
    REG_WRITE(SPI_DMA_INT_CLR_REG(3), int_st);
    xQueueSendFromISR(que_dac, &adc_evt, &task_awoken);
    ESP_EARLY_LOGV(TAG, "int msk%x, raw%x", int_st, REG_READ(SPI_DMA_INT_RAW_REG(3)));

    if (task_awoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

/**
 * Testcase: Check the interrupt types of DAC-DMA.
 */
void test_dac_dig_dma_intr_check(dac_digi_convert_mode_t mode)
{
    ESP_LOGI(TAG, "  >> %s - dac mode %d<<  ", __func__, mode);

    dac_dma_event_t evt;

    dac_digi_init();
    const dac_digi_config_t cfg = {
        .mode = mode,
        .interval = 100,
        .dig_clk.use_apll = false,  // APB clk
        .dig_clk.div_num = 79,
        .dig_clk.div_b = 1,
        .dig_clk.div_a = 0,
    };
    dac_digi_controller_config(&cfg);
    dac_output_enable(DAC_CHANNEL_1);
    dac_output_enable(DAC_CHANNEL_2);

    /* DAC-DMA linker init */
    if (que_dac == NULL) {
        que_dac = xQueueCreate(5, sizeof(dac_dma_event_t));
    } else {
        xQueueReset(que_dac);
    }
    uint32_t int_mask = SPI_OUT_DONE_INT_ENA | SPI_OUT_EOF_INT_ENA | SPI_OUT_TOTAL_EOF_INT_ENA;
    uint32_t dma_addr = dac_dma_linker_init(mode, false);
    adc_dac_dma_isr_register(dac_dma_isr, NULL, int_mask);
    adc_dac_dma_linker_start(DMA_ONLY_DAC_OUTLINK, (void *)dma_addr, int_mask);

    /* ADC-DMA start output */
    dac_digi_start();

    /* Check interrupt type */
    while (int_mask) {
        TEST_ASSERT_EQUAL( xQueueReceive(que_dac, &evt, 2000 / portTICK_PERIOD_MS), pdTRUE );
        ESP_LOGI(TAG, "DAC-DMA intr type 0x%x", evt.int_msk);
        if (evt.int_msk & int_mask) {
            int_mask &= (~evt.int_msk);
        }
    }

    ESP_LOGI(TAG, "DAC-DMA intr test over");
    adc_dac_dma_linker_deinit();
    adc_dac_dma_isr_deregister(dac_dma_isr, NULL);
    TEST_ESP_OK( dac_digi_deinit() );
}

TEST_CASE("DAC-DMA interrupt test", "[dac]")
{
    test_dac_dig_dma_intr_check(DAC_CONV_NORMAL);
    test_dac_dig_dma_intr_check(DAC_CONV_ALTER);
}

/*******************************************/
/**           SPI DMA INIT CODE            */
/*******************************************/

#include "sys/queue.h"
static bool adc_dac_dma_isr_flag = false;

/*---------------------------------------------------------------
                        INTERRUPT HANDLER
---------------------------------------------------------------*/

typedef struct adc_dac_dma_isr_handler_ {
    uint32_t mask;
    intr_handler_t handler;
    void* handler_arg;
    SLIST_ENTRY(adc_dac_dma_isr_handler_) next;
} adc_dac_dma_isr_handler_t;

static SLIST_HEAD(adc_dac_dma_isr_handler_list_, adc_dac_dma_isr_handler_) s_adc_dac_dma_isr_handler_list =
        SLIST_HEAD_INITIALIZER(s_adc_dac_dma_isr_handler_list);
portMUX_TYPE s_isr_handler_list_lock = portMUX_INITIALIZER_UNLOCKED;
static intr_handle_t s_adc_dac_dma_isr_handle;

static IRAM_ATTR void adc_dac_dma_isr_default(void* arg)
{
    uint32_t status = REG_READ(SPI_DMA_INT_ST_REG(3));
    adc_dac_dma_isr_handler_t* it;
    portENTER_CRITICAL_ISR(&s_isr_handler_list_lock);
    SLIST_FOREACH(it, &s_adc_dac_dma_isr_handler_list, next) {
        if (it->mask & status) {
            portEXIT_CRITICAL_ISR(&s_isr_handler_list_lock);
            (*it->handler)(it->handler_arg);
            portENTER_CRITICAL_ISR(&s_isr_handler_list_lock);
        }
    }
    portEXIT_CRITICAL_ISR(&s_isr_handler_list_lock);
    REG_WRITE(SPI_DMA_INT_CLR_REG(3), status);
}

static esp_err_t adc_dac_dma_isr_ensure_installed(void)
{
    esp_err_t err = ESP_OK;
    portENTER_CRITICAL(&s_isr_handler_list_lock);
    if (s_adc_dac_dma_isr_handle) {
        goto out;
    }
    REG_WRITE(SPI_DMA_INT_ENA_REG(3), 0);
    REG_WRITE(SPI_DMA_INT_CLR_REG(3), UINT32_MAX);
    err = esp_intr_alloc(ETS_SPI3_DMA_INTR_SOURCE, 0, &adc_dac_dma_isr_default, NULL, &s_adc_dac_dma_isr_handle);
    if (err != ESP_OK) {
        goto out;
    }

out:
    portEXIT_CRITICAL(&s_isr_handler_list_lock);
    return err;
}

esp_err_t adc_dac_dma_isr_register(intr_handler_t handler, void* handler_arg, uint32_t intr_mask)
{
    esp_err_t err = adc_dac_dma_isr_ensure_installed();
    if (err != ESP_OK) {
        return err;
    }

    adc_dac_dma_isr_handler_t* item = malloc(sizeof(*item));
    if (item == NULL) {
        return ESP_ERR_NO_MEM;
    }
    item->handler = handler;
    item->handler_arg = handler_arg;
    item->mask = intr_mask;
    portENTER_CRITICAL(&s_isr_handler_list_lock);
    SLIST_INSERT_HEAD(&s_adc_dac_dma_isr_handler_list, item, next);
    portEXIT_CRITICAL(&s_isr_handler_list_lock);
    return ESP_OK;
}

esp_err_t adc_dac_dma_isr_deregister(intr_handler_t handler, void* handler_arg)
{
    adc_dac_dma_isr_handler_t* it;
    adc_dac_dma_isr_handler_t* prev = NULL;
    bool found = false;
    portENTER_CRITICAL(&s_isr_handler_list_lock);
    SLIST_FOREACH(it, &s_adc_dac_dma_isr_handler_list, next) {
        if (it->handler == handler && it->handler_arg == handler_arg) {
            if (it == SLIST_FIRST(&s_adc_dac_dma_isr_handler_list)) {
                SLIST_REMOVE_HEAD(&s_adc_dac_dma_isr_handler_list, next);
            } else {
                SLIST_REMOVE_AFTER(prev, next);
            }
            found = true;
            free(it);
            break;
        }
        prev = it;
    }
    portEXIT_CRITICAL(&s_isr_handler_list_lock);
    return found ? ESP_OK : ESP_ERR_INVALID_STATE;
}

void adc_dac_dma_linker_start(spi_dma_link_type_t type, void *dma_addr, uint32_t int_msk)
{
    REG_SET_BIT(DPORT_PERIP_CLK_EN_REG, DPORT_APB_SARADC_CLK_EN_M);
    REG_SET_BIT(DPORT_PERIP_CLK_EN_REG, DPORT_SPI3_DMA_CLK_EN_M);
    REG_SET_BIT(DPORT_PERIP_CLK_EN_REG, DPORT_SPI3_CLK_EN);
    REG_CLR_BIT(DPORT_PERIP_RST_EN_REG, DPORT_SPI3_DMA_RST_M);
    REG_CLR_BIT(DPORT_PERIP_RST_EN_REG, DPORT_SPI3_RST_M);
    REG_WRITE(SPI_DMA_INT_CLR_REG(3), 0xFFFFFFFF);
    REG_WRITE(SPI_DMA_INT_ENA_REG(3), int_msk | REG_READ(SPI_DMA_INT_ENA_REG(3)));
    if (type & DMA_ONLY_ADC_INLINK) {
        REG_SET_BIT(SPI_DMA_IN_LINK_REG(3), SPI_INLINK_STOP);
        REG_CLR_BIT(SPI_DMA_IN_LINK_REG(3), SPI_INLINK_START);
        SET_PERI_REG_BITS(SPI_DMA_IN_LINK_REG(3), SPI_INLINK_ADDR, (uint32_t)dma_addr, 0);
        REG_SET_BIT(SPI_DMA_CONF_REG(3), SPI_IN_RST);
        REG_CLR_BIT(SPI_DMA_CONF_REG(3), SPI_IN_RST);
        REG_CLR_BIT(SPI_DMA_IN_LINK_REG(3), SPI_INLINK_STOP);
        REG_SET_BIT(SPI_DMA_IN_LINK_REG(3), SPI_INLINK_START);
    }
    if (type & DMA_ONLY_DAC_OUTLINK) {
        REG_SET_BIT(SPI_DMA_OUT_LINK_REG(3), SPI_OUTLINK_STOP);
        REG_CLR_BIT(SPI_DMA_OUT_LINK_REG(3), SPI_OUTLINK_START);
        SET_PERI_REG_BITS(SPI_DMA_OUT_LINK_REG(3), SPI_OUTLINK_ADDR, (uint32_t)dma_addr, 0);
        REG_SET_BIT(SPI_DMA_CONF_REG(3), SPI_OUT_RST);
        REG_CLR_BIT(SPI_DMA_CONF_REG(3), SPI_OUT_RST);
        REG_CLR_BIT(SPI_DMA_OUT_LINK_REG(3), SPI_OUTLINK_STOP);
        REG_SET_BIT(SPI_DMA_OUT_LINK_REG(3), SPI_OUTLINK_START);
    }
}

void adc_dac_dma_linker_stop(spi_dma_link_type_t type)
{
    if (type & DMA_ONLY_ADC_INLINK) {
        REG_SET_BIT(SPI_DMA_IN_LINK_REG(3), SPI_INLINK_STOP);
        REG_CLR_BIT(SPI_DMA_IN_LINK_REG(3), SPI_INLINK_START);
    }
    if (type & DMA_ONLY_DAC_OUTLINK) {
        REG_SET_BIT(SPI_DMA_OUT_LINK_REG(3), SPI_OUTLINK_STOP);
        REG_CLR_BIT(SPI_DMA_OUT_LINK_REG(3), SPI_OUTLINK_START);
    }
}

void adc_dac_dma_linker_deinit(void)
{
    adc_dac_dma_linker_stop(DMA_BOTH_ADC_DAC);
    REG_WRITE(SPI_DMA_INT_CLR_REG(3), 0xFFFFFFFF);
    REG_WRITE(SPI_DMA_INT_ENA_REG(3), 0);
    adc_dac_dma_isr_flag = false;
}

/*******************************************/
/**           SPI DMA INIT CODE END        */
/*******************************************/

#endif // CONFIG_IDF_TARGET_ESP32S2
