/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/param.h>
#include "sdkconfig.h"
#include "hal/adc_hal.h"
#include "hal/adc_hal_conf.h"
#include "hal/assert.h"
#include "soc/lldesc.h"
#include "soc/soc_caps.h"

#if CONFIG_IDF_TARGET_ESP32
//ADC utilises I2S0 DMA on ESP32
#include "hal/i2s_ll.h"
#include "hal/i2s_types.h"
#include "soc/i2s_struct.h"
#endif

#if CONFIG_IDF_TARGET_ESP32S2
//ADC utilises SPI3 DMA on ESP32S2
#include "hal/spi_ll.h"
#include "soc/spi_struct.h"
#endif

#if SOC_ADC_DIG_CTRL_SUPPORTED && !SOC_ADC_RTC_CTRL_SUPPORTED
/*---------------------------------------------------------------
                    Single Read
---------------------------------------------------------------*/
/**
 * For chips without RTC controller, Digital controller is used to trigger an ADC single read.
 */
#include "esp_rom_sys.h"
#endif  //SOC_ADC_DIG_CTRL_SUPPORTED && !SOC_ADC_RTC_CTRL_SUPPORTED

/*---------------------------------------------------------------
            Define all ADC DMA required operations here
---------------------------------------------------------------*/
#if SOC_GDMA_SUPPORTED
#define adc_dma_ll_rx_clear_intr(dev, chan, mask)       gdma_ll_rx_clear_interrupt_status(dev, chan, mask)
#define adc_dma_ll_rx_enable_intr(dev, chan, mask)      gdma_ll_rx_enable_interrupt(dev, chan, mask, true)
#define adc_dma_ll_rx_disable_intr(dev, chan, mask)     gdma_ll_rx_enable_interrupt(dev, chan, mask, false)
#define adc_dma_ll_rx_reset_channel(dev, chan)          gdma_ll_rx_reset_channel(dev, chan)
#define adc_dma_ll_rx_stop(dev, chan)                   gdma_ll_rx_stop(dev, chan)
#define adc_dma_ll_rx_start(dev, chan, addr) do { \
            gdma_ll_rx_set_desc_addr(dev, chan, (uint32_t)addr); \
            gdma_ll_rx_start(dev, chan); \
        } while (0)
#define adc_ll_digi_dma_set_eof_num(dev, num)           adc_ll_digi_dma_set_eof_num(num)
#define adc_ll_digi_reset(dev)                          adc_ll_digi_reset()
#define adc_ll_digi_trigger_enable(dev)                 adc_ll_digi_trigger_enable()
#define adc_ll_digi_trigger_disable(dev)                adc_ll_digi_trigger_disable()

//ADC utilises SPI3 DMA on ESP32S2
#elif CONFIG_IDF_TARGET_ESP32S2
#define adc_dma_ll_rx_get_intr(dev, mask)               spi_ll_get_intr(dev, mask)
#define adc_dma_ll_rx_clear_intr(dev, chan, mask)       spi_ll_clear_intr(dev, mask)
#define adc_dma_ll_rx_enable_intr(dev, chan, mask)      spi_ll_enable_intr(dev, mask)
#define adc_dma_ll_rx_disable_intr(dev, chan, mask)     spi_ll_disable_intr(dev, mask)
#define adc_dma_ll_rx_reset_channel(dev, chan)          spi_dma_ll_rx_reset(dev, chan)
#define adc_dma_ll_rx_stop(dev, chan)                   spi_dma_ll_rx_stop(dev, chan)
#define adc_dma_ll_rx_start(dev, chan, addr)            spi_dma_ll_rx_start(dev, chan, addr)
#define adc_dma_ll_get_in_suc_eof_desc_addr(dev, chan)  spi_dma_ll_get_in_suc_eof_desc_addr(dev, chan)
#define adc_ll_digi_dma_set_eof_num(dev, num)           adc_ll_digi_dma_set_eof_num(num)
#define adc_ll_digi_reset(dev)                          adc_ll_digi_reset()
#define adc_ll_digi_trigger_enable(dev)                 adc_ll_digi_trigger_enable()
#define adc_ll_digi_trigger_disable(dev)                adc_ll_digi_trigger_disable()

//ADC utilises I2S0 DMA on ESP32
#else //CONFIG_IDF_TARGET_ESP32
#define adc_dma_ll_rx_get_intr(dev, mask)               ({i2s_ll_get_intr_status(dev) & mask;})
#define adc_dma_ll_rx_clear_intr(dev, chan, mask)       i2s_ll_clear_intr_status(dev, mask)
#define adc_dma_ll_rx_enable_intr(dev, chan, mask)      do {((i2s_dev_t *)(dev))->int_ena.val |= mask;} while (0)
#define adc_dma_ll_rx_disable_intr(dev, chan, mask)     do {((i2s_dev_t *)(dev))->int_ena.val &= ~mask;} while (0)
#define adc_dma_ll_rx_reset_channel(dev, chan)          i2s_ll_rx_reset_dma(dev)
#define adc_dma_ll_rx_stop(dev, chan)                   i2s_ll_rx_stop_link(dev)
#define adc_dma_ll_rx_start(dev, chan, address) do { \
            ((i2s_dev_t *)(dev))->in_link.addr = (uint32_t)(address); \
            i2s_ll_enable_dma(dev, 1); \
            ((i2s_dev_t *)(dev))->in_link.start = 1; \
        } while (0)
#define adc_dma_ll_get_in_suc_eof_desc_addr(dev, chan)  ({uint32_t addr; i2s_ll_rx_get_eof_des_addr(dev, &addr); addr;})
#define adc_ll_digi_dma_set_eof_num(dev, num)            do {((i2s_dev_t *)(dev))->rx_eof_num = num;} while (0)
#define adc_ll_digi_reset(dev) do { \
            i2s_ll_rx_reset(dev); \
            i2s_ll_rx_reset_fifo(dev); \
        } while (0)
#define adc_ll_digi_trigger_enable(dev)                 i2s_ll_rx_start(dev)
#define adc_ll_digi_trigger_disable(dev)                i2s_ll_rx_stop(dev)
#define adc_ll_digi_dma_enable()                        adc_ll_digi_set_data_source(1)  //Will this influence I2S0
#define adc_ll_digi_dma_disable()                       adc_ll_digi_set_data_source(0)

//ESP32 ADC uses the DMA through I2S. The I2S needs to be configured.
#define I2S_BASE_CLK                                    (2*APB_CLK_FREQ)
#define SAMPLE_BITS                                     16
#define ADC_LL_CLKM_DIV_NUM_DEFAULT                     2
#define ADC_LL_CLKM_DIV_B_DEFAULT                       0
#define ADC_LL_CLKM_DIV_A_DEFAULT                       1

#endif



void adc_hal_dma_ctx_config(adc_hal_dma_ctx_t *hal, const adc_hal_dma_config_t *config)
{
    hal->desc_dummy_head.next = hal->rx_desc;
    hal->dev = config->dev;
    hal->desc_max_num = config->desc_max_num;
    hal->dma_chan = config->dma_chan;
    hal->eof_num = config->eof_num;
}

void adc_hal_digi_init(adc_hal_dma_ctx_t *hal)
{
    // Set internal FSM wait time, fixed value.
    adc_ll_digi_set_fsm_time(ADC_HAL_FSM_RSTB_WAIT_DEFAULT, ADC_HAL_FSM_START_WAIT_DEFAULT,
                             ADC_HAL_FSM_STANDBY_WAIT_DEFAULT);
    adc_ll_set_sample_cycle(ADC_HAL_SAMPLE_CYCLE_DEFAULT);
    adc_hal_pwdet_set_cct(ADC_HAL_PWDET_CCT_DEFAULT);
    adc_ll_digi_output_invert(ADC_UNIT_1, ADC_HAL_DIGI_DATA_INVERT_DEFAULT(ADC_UNIT_1));
    adc_ll_digi_output_invert(ADC_UNIT_2, ADC_HAL_DIGI_DATA_INVERT_DEFAULT(ADC_UNIT_2));
    adc_ll_digi_set_clk_div(ADC_HAL_DIGI_SAR_CLK_DIV_DEFAULT);

    adc_dma_ll_rx_clear_intr(hal->dev, hal->dma_chan, ADC_HAL_DMA_INTR_MASK);
    adc_dma_ll_rx_enable_intr(hal->dev, hal->dma_chan, ADC_HAL_DMA_INTR_MASK);
    adc_ll_digi_dma_set_eof_num(hal->dev, hal->eof_num);
#if CONFIG_IDF_TARGET_ESP32
    i2s_ll_rx_set_sample_bit(hal->dev, SAMPLE_BITS, SAMPLE_BITS);
    i2s_ll_rx_enable_mono_mode(hal->dev, 1);
    i2s_ll_rx_force_enable_fifo_mod(hal->dev, 1);
    i2s_ll_enable_builtin_adc(hal->dev, 1);
#endif

    adc_oneshot_ll_disable_all_unit();
}

void adc_hal_digi_deinit(adc_hal_dma_ctx_t *hal)
{
    adc_ll_digi_trigger_disable(hal->dev);
    adc_ll_digi_dma_disable();
    adc_ll_digi_clear_pattern_table(ADC_UNIT_1);
    adc_ll_digi_clear_pattern_table(ADC_UNIT_2);
    adc_ll_digi_reset(hal->dev);
    adc_ll_digi_controller_clk_disable();
}

/*---------------------------------------------------------------
                    DMA read
---------------------------------------------------------------*/
static adc_ll_digi_convert_mode_t get_convert_mode(adc_digi_convert_mode_t convert_mode)
{
#if CONFIG_IDF_TARGET_ESP32
    return ADC_LL_DIGI_CONV_ONLY_ADC1;
#endif
#if (SOC_ADC_DIGI_CONTROLLER_NUM == 1)
    return ADC_LL_DIGI_CONV_ALTER_UNIT;
#elif (SOC_ADC_DIGI_CONTROLLER_NUM >= 2)
    switch (convert_mode) {
        case ADC_CONV_SINGLE_UNIT_1:
            return ADC_LL_DIGI_CONV_ONLY_ADC1;
        case ADC_CONV_SINGLE_UNIT_2:
            return ADC_LL_DIGI_CONV_ONLY_ADC2;
        case ADC_CONV_BOTH_UNIT:
            return ADC_LL_DIGI_CONV_BOTH_UNIT;
        case ADC_CONV_ALTER_UNIT:
            return ADC_LL_DIGI_CONV_ALTER_UNIT;
        default:
            abort();
    }
#endif
}

/**
 * For esp32s2 and later chips
 * - Set ADC digital controller clock division factor. The clock is divided from `APLL` or `APB` clock.
 *   Expression: controller_clk = APLL/APB * (div_num  + div_a / div_b + 1).
 * - Enable clock and select clock source for ADC digital controller.
 * For esp32, use I2S clock
 */
static void adc_hal_digi_sample_freq_config(adc_hal_dma_ctx_t *hal, uint32_t freq)
{
#if !CONFIG_IDF_TARGET_ESP32
    uint32_t interval = APB_CLK_FREQ / (ADC_LL_CLKM_DIV_NUM_DEFAULT + ADC_LL_CLKM_DIV_A_DEFAULT / ADC_LL_CLKM_DIV_B_DEFAULT + 1) / 2 / freq;
    //set sample interval
    adc_ll_digi_set_trigger_interval(interval);
    //Here we set the clock divider factor to make the digital clock to 5M Hz
    adc_ll_digi_controller_clk_div(ADC_LL_CLKM_DIV_NUM_DEFAULT, ADC_LL_CLKM_DIV_B_DEFAULT, ADC_LL_CLKM_DIV_A_DEFAULT);
    adc_ll_digi_clk_sel(0);   //use APB
#else
    i2s_ll_rx_clk_set_src(hal->dev, I2S_CLK_SRC_DEFAULT);    /*!< Clock from PLL_D2_CLK(160M)*/
    uint32_t bck = I2S_BASE_CLK / (ADC_LL_CLKM_DIV_NUM_DEFAULT + ADC_LL_CLKM_DIV_B_DEFAULT / ADC_LL_CLKM_DIV_A_DEFAULT) / 2 / freq;
    i2s_ll_set_raw_mclk_div(hal->dev, ADC_LL_CLKM_DIV_NUM_DEFAULT, ADC_LL_CLKM_DIV_A_DEFAULT, ADC_LL_CLKM_DIV_B_DEFAULT);
    i2s_ll_rx_set_bck_div_num(hal->dev, bck);
#endif
}

void adc_hal_digi_controller_config(adc_hal_dma_ctx_t *hal, const adc_hal_digi_ctrlr_cfg_t *cfg)
{
#if (SOC_ADC_DIGI_CONTROLLER_NUM == 1)
    //Only one pattern table, this variable is for readability
    const int pattern_both = 0;

    adc_ll_digi_clear_pattern_table(pattern_both);
    adc_ll_digi_set_pattern_table_len(pattern_both, cfg->adc_pattern_len);
    for (int i = 0; i < cfg->adc_pattern_len; i++) {
        adc_ll_digi_set_pattern_table(pattern_both, i, cfg->adc_pattern[i]);
    }

#elif (SOC_ADC_DIGI_CONTROLLER_NUM >= 2)
    uint32_t adc1_pattern_idx = 0;
    uint32_t adc2_pattern_idx = 0;

    adc_ll_digi_clear_pattern_table(ADC_UNIT_1);
    adc_ll_digi_clear_pattern_table(ADC_UNIT_2);

    for (int i = 0; i < cfg->adc_pattern_len; i++) {
        if (cfg->adc_pattern[i].unit == ADC_UNIT_1) {
            adc_ll_digi_set_pattern_table(ADC_UNIT_1, adc1_pattern_idx, cfg->adc_pattern[i]);
            adc1_pattern_idx++;
        } else if (cfg->adc_pattern[i].unit == ADC_UNIT_2) {
            adc_ll_digi_set_pattern_table(ADC_UNIT_2, adc2_pattern_idx, cfg->adc_pattern[i]);
            adc2_pattern_idx++;
        } else {
            abort();
        }
    }
    adc_ll_digi_set_pattern_table_len(ADC_UNIT_1, adc1_pattern_idx);
    adc_ll_digi_set_pattern_table_len(ADC_UNIT_2, adc2_pattern_idx);

#endif

    if (cfg->conv_limit_en) {
        adc_ll_digi_set_convert_limit_num(cfg->conv_limit_num);
        adc_ll_digi_convert_limit_enable();
    } else {
        adc_ll_digi_convert_limit_disable();
    }

    adc_ll_digi_set_convert_mode(get_convert_mode(cfg->conv_mode));

    //clock and sample frequency
    adc_hal_digi_sample_freq_config(hal, cfg->sample_freq_hz);
}

static void adc_hal_digi_dma_link_descriptors(dma_descriptor_t *desc, uint8_t *data_buf, uint32_t size, uint32_t num)
{
    HAL_ASSERT(((uint32_t)data_buf % 4) == 0);
    HAL_ASSERT((size % 4) == 0);
    uint32_t n = 0;

    while (num--) {
        desc[n] = (dma_descriptor_t) {
            .dw0.size = size,
            .dw0.length = 0,
            .dw0.suc_eof = 0,
            .dw0.owner = 1,
            .buffer = data_buf,
            .next = &desc[n+1]
        };
        data_buf += size;
        n++;
    }
    desc[n-1].next = NULL;
}

void adc_hal_digi_start(adc_hal_dma_ctx_t *hal, uint8_t *data_buf)
{
    //stop peripheral and DMA
    adc_hal_digi_stop(hal);

    //reset DMA
    adc_dma_ll_rx_reset_channel(hal->dev, hal->dma_chan);
    //reset peripheral
    adc_ll_digi_reset(hal->dev);

    //reset the current descriptor address
    hal->cur_desc_ptr = &hal->desc_dummy_head;
    adc_hal_digi_dma_link_descriptors(hal->rx_desc, data_buf, hal->eof_num * ADC_HAL_DATA_LEN_PER_CONV, hal->desc_max_num);

    //start DMA
    adc_dma_ll_rx_start(hal->dev, hal->dma_chan, (lldesc_t *)hal->rx_desc);
    //connect DMA and peripheral
    adc_ll_digi_dma_enable();
    //start ADC
    adc_ll_digi_trigger_enable(hal->dev);
}

#if !SOC_GDMA_SUPPORTED
intptr_t adc_hal_get_desc_addr(adc_hal_dma_ctx_t *hal)
{
    return adc_dma_ll_get_in_suc_eof_desc_addr(hal->dev, hal->dma_chan);
}

bool adc_hal_check_event(adc_hal_dma_ctx_t *hal, uint32_t mask)
{
    return adc_dma_ll_rx_get_intr(hal->dev, mask);
}
#endif  //#if !SOC_GDMA_SUPPORTED

adc_hal_dma_desc_status_t adc_hal_get_reading_result(adc_hal_dma_ctx_t *hal, const intptr_t eof_desc_addr, dma_descriptor_t **cur_desc)
{
    HAL_ASSERT(hal->cur_desc_ptr);
    if (!hal->cur_desc_ptr->next) {
        return ADC_HAL_DMA_DESC_NULL;
    }
    if ((intptr_t)hal->cur_desc_ptr == eof_desc_addr) {
        return ADC_HAL_DMA_DESC_WAITING;
    }

    hal->cur_desc_ptr = hal->cur_desc_ptr->next;
    *cur_desc = hal->cur_desc_ptr;

    return ADC_HAL_DMA_DESC_VALID;
}

void adc_hal_digi_clr_intr(adc_hal_dma_ctx_t *hal, uint32_t mask)
{
    adc_dma_ll_rx_clear_intr(hal->dev, hal->dma_chan, mask);
}

void adc_hal_digi_dis_intr(adc_hal_dma_ctx_t *hal, uint32_t mask)
{
    adc_dma_ll_rx_disable_intr(hal->dev, hal->dma_chan, mask);
}

void adc_hal_digi_stop(adc_hal_dma_ctx_t *hal)
{
    //stop ADC
    adc_ll_digi_trigger_disable(hal->dev);
    //stop DMA
    adc_dma_ll_rx_stop(hal->dev, hal->dma_chan);
    //disconnect DMA and peripheral
    adc_ll_digi_dma_disable();
}


/*---------------------------------------------------------------
                    Single Read
---------------------------------------------------------------*/
/**
 * For chips without RTC controller, Digital controller is used to trigger an ADC single read.
 */

//--------------------Single Read-------------------------------//
static void adc_hal_onetime_start(adc_unit_t adc_n)
{
#if SOC_ADC_DIG_CTRL_SUPPORTED && !SOC_ADC_RTC_CTRL_SUPPORTED
    (void)adc_n;
    /**
     * There is a hardware limitation. If the APB clock frequency is high, the step of this reg signal: ``onetime_start`` may not be captured by the
     * ADC digital controller (when its clock frequency is too slow). A rough estimate for this step should be at least 3 ADC digital controller
     * clock cycle.
     *
     * This limitation will be removed in hardware future versions.
     *
     */
    uint32_t digi_clk = APB_CLK_FREQ / (ADC_LL_CLKM_DIV_NUM_DEFAULT + ADC_LL_CLKM_DIV_A_DEFAULT / ADC_LL_CLKM_DIV_B_DEFAULT + 1);
    //Convert frequency to time (us). Since decimals are removed by this division operation. Add 1 here in case of the fact that delay is not enough.
    uint32_t delay = (1000 * 1000) / digi_clk + 1;
    //3 ADC digital controller clock cycle
    delay = delay * 3;
    //This coefficient (8) is got from test. When digi_clk is not smaller than ``APB_CLK_FREQ/8``, no delay is needed.
    if (digi_clk >= APB_CLK_FREQ/8) {
        delay = 0;
    }

    adc_oneshot_ll_start(false);
    esp_rom_delay_us(delay);
    adc_oneshot_ll_start(true);

    //No need to delay here. Becuase if the start signal is not seen, there won't be a done intr.
#else
    adc_oneshot_ll_start(adc_n);
#endif
}

esp_err_t adc_hal_convert(adc_unit_t adc_n, int channel, int *out_raw)
{
    uint32_t event = (adc_n == ADC_UNIT_1) ? ADC_LL_EVENT_ADC1_ONESHOT_DONE : ADC_LL_EVENT_ADC2_ONESHOT_DONE;
    adc_oneshot_ll_clear_event(event);
    adc_oneshot_ll_disable_all_unit();
    adc_oneshot_ll_enable(adc_n);
    adc_oneshot_ll_set_channel(adc_n, channel);

    adc_hal_onetime_start(adc_n);
    while (adc_oneshot_ll_get_event(event) != true) {
        ;
    }
    *out_raw = adc_oneshot_ll_get_raw_result(adc_n);
    if (adc_oneshot_ll_raw_check_valid(adc_n, *out_raw) == false) {
        return ESP_ERR_INVALID_STATE;
    }

    //HW workaround: when enabling periph clock, this should be false
    adc_oneshot_ll_disable_all_unit();

    return ESP_OK;
}
