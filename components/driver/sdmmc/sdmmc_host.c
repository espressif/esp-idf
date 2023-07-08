/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stddef.h>
#include <sys/param.h>
#include "esp_log.h"
#include "esp_intr_alloc.h"
#include "esp_timer.h"
#include "esp_check.h"
#include "soc/soc_caps.h"
#include "soc/soc_pins.h"
#include "soc/gpio_periph.h"
#include "esp_rom_gpio.h"
#include "esp_rom_sys.h"
#include "driver/gpio.h"
#include "driver/sdmmc_host.h"
#include "esp_private/periph_ctrl.h"
#include "sdmmc_private.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_clk_tree.h"
#include "soc/sdmmc_periph.h"
#include "soc/soc_caps.h"
#include "hal/gpio_hal.h"

#define SDMMC_EVENT_QUEUE_LENGTH 32

static void sdmmc_isr(void *arg);
static void sdmmc_host_dma_init(void);

static const char *TAG = "sdmmc_periph";
static intr_handle_t s_intr_handle;
static QueueHandle_t s_event_queue;
static SemaphoreHandle_t s_io_intr_event;

static size_t s_slot_width[2] = {1, 1};

/* The following definitions are used to simplify GPIO configuration in the driver,
 * whether IOMUX or GPIO Matrix is used by the chip.
 * Two simple "APIs" are provided to the driver code:
 * - configure_pin(name, slot, mode): Configures signal "name" for the given slot and mode.
 * - GPIO_NUM(slot, name): Returns the GPIO number of signal "name" for the given slot.
 *
 * To make this work, configure_pin is defined as a macro that picks the parameters required
 * for configuring GPIO matrix or IOMUX from relevant arrays, and passes them to either of
 * configure_pin_gpio_matrix, configure_pin_iomux functions.
 * Likewise, GPIO_NUM is a macro that picks the pin number from one of the two structures.
 *
 * Macros are used rather than inline functions to look up members of different structures
 * with same names. E.g. the number of pin d3 is obtained either from .d3 member of
 * sdmmc_slot_gpio_num array (for IOMUX) or from .d3 member of s_sdmmc_slot_gpio_num array
 * (for GPIO matrix).
 */
#ifdef SOC_SDMMC_USE_GPIO_MATRIX
static void configure_pin_gpio_matrix(uint8_t gpio_num, uint8_t gpio_matrix_sig, gpio_mode_t mode, const char *name);
#define configure_pin(name, slot, mode) \
    configure_pin_gpio_matrix(s_sdmmc_slot_gpio_num[slot].name, sdmmc_slot_gpio_sig[slot].name, mode, #name)
static sdmmc_slot_io_info_t s_sdmmc_slot_gpio_num[SOC_SDMMC_NUM_SLOTS];
#define GPIO_NUM(slot, name) s_sdmmc_slot_gpio_num[slot].name

#elif SOC_SDMMC_USE_IOMUX
static void configure_pin_iomux(uint8_t gpio_num);
#define configure_pin(name, slot, mode) configure_pin_iomux(sdmmc_slot_gpio_num[slot].name)
#define GPIO_NUM(slot, name) sdmmc_slot_gpio_num[slot].name

#endif // SOC_SDMMC_USE_GPIO_MATRIX

static esp_err_t sdmmc_host_pullup_en_internal(int slot, int width);

esp_err_t sdmmc_host_reset(void)
{
    // Set reset bits
    SDMMC.ctrl.controller_reset = 1;
    SDMMC.ctrl.dma_reset = 1;
    SDMMC.ctrl.fifo_reset = 1;

    // Wait for the reset bits to be cleared by hardware
    int64_t yield_delay_us = 100 * 1000; // initially 100ms
    int64_t t0 = esp_timer_get_time();
    int64_t t1 = 0;
    while (SDMMC.ctrl.controller_reset || SDMMC.ctrl.fifo_reset || SDMMC.ctrl.dma_reset) {
        t1 = esp_timer_get_time();
        if (t1 - t0 > SDMMC_HOST_RESET_TIMEOUT_US) {
            return ESP_ERR_TIMEOUT;
        }
        if (t1 - t0 > yield_delay_us) {
            yield_delay_us *= 2;
            vTaskDelay(1);
        }
    }

    return ESP_OK;
}

/* We have two clock divider stages:
 * - one is the clock generator which drives SDMMC peripheral,
 *   it can be configured using SDMMC.clock register. It can generate
 *   frequencies 160MHz/(N + 1), where 0 < N < 16, I.e. from 10 to 80 MHz.
 * - 4 clock dividers inside SDMMC peripheral, which can divide clock
 *   from the first stage by 2 * M, where 0 < M < 255
 *   (they can also be bypassed).
 *
 * For cards which aren't UHS-1 or UHS-2 cards, which we don't support,
 * maximum bus frequency in high speed (HS) mode is 50 MHz.
 * Note: for non-UHS-1 cards, HS mode is optional.
 * Default speed (DS) mode is mandatory, it works up to 25 MHz.
 * Whether the card supports HS or not can be determined using TRAN_SPEED
 * field of card's CSD register.
 *
 * 50 MHz can not be obtained exactly, closest we can get is 53 MHz.
 *
 * The first stage divider is set to the highest possible value for the given
 * frequency, and the the second stage dividers are used if division factor
 * is >16.
 *
 * Of the second stage dividers, div0 is used for card 0, and div1 is used
 * for card 1.
 */

static void sdmmc_host_set_clk_div(int div)
{
    /**
     * Set frequency to 160MHz / div
     *
     * n: counter resets at div_factor_n.
     * l: negedge when counter equals div_factor_l.
     * h: posedge when counter equals div_factor_h.
     *
     * We set the duty cycle to 1/2
     */
#if CONFIG_IDF_TARGET_ESP32
    assert (div > 1 && div <= 16);
    int h = div - 1;
    int l = div / 2 - 1;

    SDMMC.clock.div_factor_h = h;
    SDMMC.clock.div_factor_l = l;
    SDMMC.clock.div_factor_n = h;

    // Set phases for in/out clocks
    // 180 degree phase on input and output clocks
    SDMMC.clock.phase_dout = 4;
    SDMMC.clock.phase_din = 4;
    SDMMC.clock.phase_core = 0;

#elif CONFIG_IDF_TARGET_ESP32S3
    assert (div > 1 && div <= 16);
    int l = div - 1;
    int h = div / 2 - 1;

    SDMMC.clock.div_factor_h = h;
    SDMMC.clock.div_factor_l = l;
    SDMMC.clock.div_factor_n = l;

    // Make sure SOC_MOD_CLK_PLL_F160M (160 MHz) source clock is used
#if SOC_SDMMC_SUPPORT_XTAL_CLOCK
    SDMMC.clock.clk_sel = 1;
#endif

    SDMMC.clock.phase_core = 0;
    /* 90 deg. delay for cclk_out to satisfy large hold time for SDR12 (up to 25MHz) and SDR25 (up to 50MHz) modes.
     * Whether this delayed clock will be used depends on use_hold_reg bit in CMD structure,
     * determined when sending out the command.
     */
    SDMMC.clock.phase_dout = 1;
    SDMMC.clock.phase_din = 0;
#endif  //CONFIG_IDF_TARGET_ESP32S3

    // Wait for the clock to propagate
    esp_rom_delay_us(10);
}

static inline int s_get_host_clk_div(void)
{
#if CONFIG_IDF_TARGET_ESP32
    return SDMMC.clock.div_factor_h + 1;
#elif CONFIG_IDF_TARGET_ESP32S3
    return SDMMC.clock.div_factor_l + 1;
#endif
}

static void sdmmc_host_input_clk_disable(void)
{
    SDMMC.clock.val = 0;
}

static esp_err_t sdmmc_host_clock_update_command(int slot)
{
    // Clock update command (not a real command; just updates CIU registers)
    sdmmc_hw_cmd_t cmd_val = {
        .card_num = slot,
        .update_clk_reg = 1,
        .wait_complete = 1
    };
    bool repeat = true;
    while (repeat) {

        ESP_RETURN_ON_ERROR(sdmmc_host_start_command(slot, cmd_val, 0), TAG, "sdmmc_host_start_command returned 0x%x", err_rc_);

        int64_t yield_delay_us = 100 * 1000; // initially 100ms
        int64_t t0 = esp_timer_get_time();
        int64_t t1 = 0;
        while (true) {
            t1 = esp_timer_get_time();
            if (t1 - t0  > SDMMC_HOST_CLOCK_UPDATE_CMD_TIMEOUT_US) {
                return ESP_ERR_TIMEOUT;
            }
            // Sending clock update command to the CIU can generate HLE error.
            // According to the manual, this is okay and we must retry the command.
            if (SDMMC.rintsts.hle) {
                SDMMC.rintsts.hle = 1;
                repeat = true;
                break;
            }
            // When the command is accepted by CIU, start_command bit will be
            // cleared in SDMMC.cmd register.
            if (SDMMC.cmd.start_command == 0) {
                repeat = false;
                break;
            }
            if (t1 - t0 > yield_delay_us) {
                yield_delay_us *= 2;
                vTaskDelay(1);
            }
        }
    }

    return ESP_OK;
}

void sdmmc_host_get_clk_dividers(const uint32_t freq_khz, int *host_div, int *card_div)
{
    uint32_t clk_src_freq_hz = 0;
    esp_clk_tree_src_get_freq_hz(SDMMC_CLK_SRC_DEFAULT, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &clk_src_freq_hz);
    assert(clk_src_freq_hz == (160 * 1000 * 1000));

    // Calculate new dividers
    if (freq_khz >= SDMMC_FREQ_HIGHSPEED) {
        *host_div = 4;       // 160 MHz / 4 = 40 MHz
        *card_div = 0;
    } else if (freq_khz == SDMMC_FREQ_DEFAULT) {
        *host_div = 8;       // 160 MHz / 8 = 20 MHz
        *card_div = 0;
    } else if (freq_khz == SDMMC_FREQ_PROBING) {
        *host_div = 10;      // 160 MHz / 10 / (20 * 2) = 400 kHz
        *card_div = 20;
    } else {
        /*
         * for custom frequencies use maximum range of host divider (1-16), find the closest <= div. combination
         * if exceeded, combine with the card divider to keep reasonable precision (applies mainly to low frequencies)
         * effective frequency range: 400 kHz - 32 MHz (32.1 - 39.9 MHz cannot be covered with given divider scheme)
         */
        *host_div = (clk_src_freq_hz) / (freq_khz * 1000);
        if (*host_div > 15 ) {
            *host_div = 2;
            *card_div = (clk_src_freq_hz / 2) / (2 * freq_khz * 1000);
            if ( ((clk_src_freq_hz / 2) % (2 * freq_khz * 1000)) > 0 ) {
                (*card_div)++;
            }
        } else if ((clk_src_freq_hz % (freq_khz * 1000)) > 0) {
            (*host_div)++;
        }
    }
}

static int sdmmc_host_calc_freq(const int host_div, const int card_div)
{
    uint32_t clk_src_freq_hz = 0;
    esp_clk_tree_src_get_freq_hz(SDMMC_CLK_SRC_DEFAULT, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &clk_src_freq_hz);
    assert(clk_src_freq_hz == (160 * 1000 * 1000));
    return clk_src_freq_hz / host_div / ((card_div == 0) ? 1 : card_div * 2) / 1000;
}

esp_err_t sdmmc_host_set_card_clk(int slot, uint32_t freq_khz)
{
    if (!(slot == 0 || slot == 1)) {
        return ESP_ERR_INVALID_ARG;
    }

    // Disable clock first
    SDMMC.clkena.cclk_enable &= ~BIT(slot);
    esp_err_t err = sdmmc_host_clock_update_command(slot);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "disabling clk failed");
        ESP_LOGE(TAG, "%s: sdmmc_host_clock_update_command returned 0x%x", __func__, err);
        return err;
    }

    int host_div = 0;   /* clock divider of the host (SDMMC.clock) */
    int card_div = 0;   /* 1/2 of card clock divider (SDMMC.clkdiv) */
    sdmmc_host_get_clk_dividers(freq_khz, &host_div, &card_div);

    int real_freq = sdmmc_host_calc_freq(host_div, card_div);
    ESP_LOGD(TAG, "slot=%d host_div=%d card_div=%d freq=%dkHz (max %" PRIu32 "kHz)", slot, host_div, card_div, real_freq, freq_khz);

    // Program CLKDIV and CLKSRC, send them to the CIU
    switch(slot) {
        case 0:
            SDMMC.clksrc.card0 = 0;
            SDMMC.clkdiv.div0 = card_div;
            break;
        case 1:
            SDMMC.clksrc.card1 = 1;
            SDMMC.clkdiv.div1 = card_div;
            break;
    }
    sdmmc_host_set_clk_div(host_div);
    err = sdmmc_host_clock_update_command(slot);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "setting clk div failed");
        ESP_LOGE(TAG, "%s: sdmmc_host_clock_update_command returned 0x%x", __func__, err);
        return err;
    }

    // Re-enable clocks
    SDMMC.clkena.cclk_enable |= BIT(slot);
    SDMMC.clkena.cclk_low_power |= BIT(slot);
    err = sdmmc_host_clock_update_command(slot);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "re-enabling clk failed");
        ESP_LOGE(TAG, "%s: sdmmc_host_clock_update_command returned 0x%x", __func__, err);
        return err;
    }

    // set data timeout
    const uint32_t data_timeout_ms = 100;
    uint32_t data_timeout_cycles = data_timeout_ms * freq_khz;
    const uint32_t data_timeout_cycles_max = 0xffffff;
    if (data_timeout_cycles > data_timeout_cycles_max) {
        data_timeout_cycles = data_timeout_cycles_max;
    }
    SDMMC.tmout.data = data_timeout_cycles;
    // always set response timeout to highest value, it's small enough anyway
    SDMMC.tmout.response = 255;
    return ESP_OK;
}

esp_err_t sdmmc_host_get_real_freq(int slot, int *real_freq_khz)
{
    if (real_freq_khz == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!(slot == 0 || slot == 1)) {
        return ESP_ERR_INVALID_ARG;
    }

    int host_div = s_get_host_clk_div();
    int card_div = slot == 0 ? SDMMC.clkdiv.div0 : SDMMC.clkdiv.div1;
    *real_freq_khz = sdmmc_host_calc_freq(host_div, card_div);

    return ESP_OK;
}

esp_err_t sdmmc_host_set_input_delay(int slot, sdmmc_delay_phase_t delay_phase)
{
#if CONFIG_IDF_TARGET_ESP32
    //DIG-217
    ESP_LOGW(TAG, "esp32 doesn't support input phase delay, fallback to 0 delay");
    return ESP_ERR_NOT_SUPPORTED;
#else
    ESP_RETURN_ON_FALSE((slot == 0 || slot == 1), ESP_ERR_INVALID_ARG, TAG, "invalid slot");
    ESP_RETURN_ON_FALSE(delay_phase < SOC_SDMMC_DELAY_PHASE_NUM, ESP_ERR_INVALID_ARG, TAG, "invalid delay phase");

    uint32_t clk_src_freq_hz = 0;
    ESP_RETURN_ON_ERROR(esp_clk_tree_src_get_freq_hz(SDMMC_CLK_SRC_DEFAULT, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &clk_src_freq_hz),
                        TAG, "get source clock frequency failed");

    //Now we're in high speed. Note ESP SDMMC Host HW only supports integer divider.
    int delay_phase_num = 0;
    switch (delay_phase) {
        case SDMMC_DELAY_PHASE_1:
            SDMMC.clock.phase_din = 0x1;
            delay_phase_num = 1;
            break;
        case SDMMC_DELAY_PHASE_2:
            SDMMC.clock.phase_din = 0x4;
            delay_phase_num = 2;
            break;
        case SDMMC_DELAY_PHASE_3:
            SDMMC.clock.phase_din = 0x6;
            delay_phase_num = 3;
            break;
        default:
            SDMMC.clock.phase_din = 0x0;
            break;
    }

    int src_clk_period_ps = (1 * 1000 * 1000) / (clk_src_freq_hz / (1 * 1000 * 1000));
    int phase_diff_ps = src_clk_period_ps * (SDMMC.clock.div_factor_n + 1) / SOC_SDMMC_DELAY_PHASE_NUM;
    ESP_LOGD(TAG, "difference between input delay phases is %d ps", phase_diff_ps);
    ESP_LOGI(TAG, "host sampling edge is delayed by %d ps", phase_diff_ps * delay_phase_num);
#endif

    return ESP_OK;
}

esp_err_t sdmmc_host_start_command(int slot, sdmmc_hw_cmd_t cmd, uint32_t arg)
{
    if (!(slot == 0 || slot == 1)) {
        return ESP_ERR_INVALID_ARG;
    }
    if ((SDMMC.cdetect.cards & BIT(slot)) != 0) {
        return ESP_ERR_NOT_FOUND;
    }
    if (cmd.data_expected && cmd.rw && (SDMMC.wrtprt.cards & BIT(slot)) != 0) {
        return ESP_ERR_INVALID_STATE;
    }
    /* Outputs should be synchronized to cclk_out */
    cmd.use_hold_reg = 1;

    int64_t yield_delay_us = 100 * 1000; // initially 100ms
    int64_t t0 = esp_timer_get_time();
    int64_t t1 = 0;
    while (SDMMC.cmd.start_command == 1) {
        t1 = esp_timer_get_time();
        if (t1 - t0 > SDMMC_HOST_START_CMD_TIMEOUT_US) {
            return ESP_ERR_TIMEOUT;
        }
        if (t1 - t0 > yield_delay_us) {
            yield_delay_us *= 2;
            vTaskDelay(1);
        }
    }
    SDMMC.cmdarg = arg;
    cmd.card_num = slot;
    cmd.start_command = 1;
    SDMMC.cmd = cmd;
    return ESP_OK;
}

esp_err_t sdmmc_host_init(void)
{
    if (s_intr_handle) {
        return ESP_ERR_INVALID_STATE;
    }

    periph_module_reset(PERIPH_SDMMC_MODULE);
    periph_module_enable(PERIPH_SDMMC_MODULE);

    // Enable clock to peripheral. Use smallest divider first.
    sdmmc_host_set_clk_div(2);

    // Reset
    esp_err_t err = sdmmc_host_reset();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: sdmmc_host_reset returned 0x%x", __func__, err);
        return err;
    }

    ESP_LOGD(TAG, "peripheral version %"PRIx32", hardware config %08"PRIx32, SDMMC.verid, SDMMC.hcon);

    // Clear interrupt status and set interrupt mask to known state
    SDMMC.rintsts.val = 0xffffffff;
    SDMMC.intmask.val = 0;
    SDMMC.ctrl.int_enable = 0;

    // Allocate event queue
    s_event_queue = xQueueCreate(SDMMC_EVENT_QUEUE_LENGTH, sizeof(sdmmc_event_t));
    if (!s_event_queue) {
        return ESP_ERR_NO_MEM;
    }
    s_io_intr_event = xSemaphoreCreateBinary();
    if (!s_io_intr_event) {
        vQueueDelete(s_event_queue);
        s_event_queue = NULL;
        return ESP_ERR_NO_MEM;
    }
    // Attach interrupt handler
    esp_err_t ret = esp_intr_alloc(ETS_SDIO_HOST_INTR_SOURCE, 0, &sdmmc_isr, s_event_queue, &s_intr_handle);
    if (ret != ESP_OK) {
        vQueueDelete(s_event_queue);
        s_event_queue = NULL;
        vSemaphoreDelete(s_io_intr_event);
        s_io_intr_event = NULL;
        return ret;
    }
    // Enable interrupts
    SDMMC.intmask.val =
            SDMMC_INTMASK_CD |
            SDMMC_INTMASK_CMD_DONE |
            SDMMC_INTMASK_DATA_OVER |
            SDMMC_INTMASK_RCRC | SDMMC_INTMASK_DCRC |
            SDMMC_INTMASK_RTO | SDMMC_INTMASK_DTO | SDMMC_INTMASK_HTO |
            SDMMC_INTMASK_SBE | SDMMC_INTMASK_EBE |
            SDMMC_INTMASK_RESP_ERR | SDMMC_INTMASK_HLE; //sdio is enabled only when use.
    SDMMC.ctrl.int_enable = 1;

    // Disable generation of Busy Clear Interrupt
    SDMMC.cardthrctl.busy_clr_int_en = 0;

    // Enable DMA
    sdmmc_host_dma_init();

    // Initialize transaction handler
    ret = sdmmc_host_transaction_handler_init();
    if (ret != ESP_OK) {
        vQueueDelete(s_event_queue);
        s_event_queue = NULL;
        vSemaphoreDelete(s_io_intr_event);
        s_io_intr_event = NULL;
        esp_intr_free(s_intr_handle);
        s_intr_handle = NULL;
        return ret;
    }

    return ESP_OK;
}

#ifdef SOC_SDMMC_USE_IOMUX

static void configure_pin_iomux(uint8_t gpio_num)
{
    const int sdmmc_func = 3;
    const int drive_strength = 3;
    assert(gpio_num != (uint8_t) GPIO_NUM_NC);
    gpio_pulldown_dis(gpio_num);

    uint32_t reg = GPIO_PIN_MUX_REG[gpio_num];
    assert(reg != UINT32_MAX);
    PIN_INPUT_ENABLE(reg);
    gpio_hal_iomux_func_sel(reg, sdmmc_func);
    PIN_SET_DRV(reg, drive_strength);
}

#elif SOC_SDMMC_USE_GPIO_MATRIX

static void configure_pin_gpio_matrix(uint8_t gpio_num, uint8_t gpio_matrix_sig, gpio_mode_t mode, const char *name)
{
    assert (gpio_num != (uint8_t) GPIO_NUM_NC);
    ESP_LOGD(TAG, "using GPIO%d as %s pin", gpio_num, name);
    gpio_reset_pin(gpio_num);
    gpio_set_direction(gpio_num, mode);
    gpio_pulldown_dis(gpio_num);
    if (mode == GPIO_MODE_INPUT || mode == GPIO_MODE_INPUT_OUTPUT) {
        esp_rom_gpio_connect_in_signal(gpio_num, gpio_matrix_sig, false);
    }
    if (mode == GPIO_MODE_OUTPUT || mode == GPIO_MODE_INPUT_OUTPUT) {
        esp_rom_gpio_connect_out_signal(gpio_num, gpio_matrix_sig, false, false);
    }
}

#endif // SOC_SDMMC_USE_{IOMUX,GPIO_MATRIX}

esp_err_t sdmmc_host_init_slot(int slot, const sdmmc_slot_config_t *slot_config)
{
    if (!s_intr_handle) {
        return ESP_ERR_INVALID_STATE;
    }
    if (!(slot == 0 || slot == 1)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (slot_config == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    int gpio_cd = slot_config->cd;
    int gpio_wp = slot_config->wp;
    bool gpio_wp_polarity = slot_config->flags & SDMMC_SLOT_FLAG_WP_ACTIVE_HIGH;
    uint8_t slot_width = slot_config->width;

    // Configure pins
    const sdmmc_slot_info_t *slot_info = &sdmmc_slot_info[slot];

    if (slot_width == SDMMC_SLOT_WIDTH_DEFAULT) {
        slot_width = slot_info->width;
    } else if (slot_width > slot_info->width) {
        return ESP_ERR_INVALID_ARG;
    }
    s_slot_width[slot] = slot_width;

#if SOC_SDMMC_USE_GPIO_MATRIX
    /* Save pin configuration for this slot */
    s_sdmmc_slot_gpio_num[slot].clk = slot_config->clk;
    s_sdmmc_slot_gpio_num[slot].cmd = slot_config->cmd;
    s_sdmmc_slot_gpio_num[slot].d0 = slot_config->d0;
    /* Save d1 even in 1-line mode, it might be needed for SDIO INT line */
    s_sdmmc_slot_gpio_num[slot].d1 = slot_config->d1;
    if (slot_width >= 4) {
        s_sdmmc_slot_gpio_num[slot].d2 = slot_config->d2;
    }
    /* Save d3 even for 1-line mode, as it needs to be set high */
    s_sdmmc_slot_gpio_num[slot].d3 = slot_config->d3;
    if (slot_width >= 8) {
        s_sdmmc_slot_gpio_num[slot].d4 = slot_config->d4;
        s_sdmmc_slot_gpio_num[slot].d5 = slot_config->d5;
        s_sdmmc_slot_gpio_num[slot].d6 = slot_config->d6;
        s_sdmmc_slot_gpio_num[slot].d7 = slot_config->d7;
    }
#endif

    bool pullup = slot_config->flags & SDMMC_SLOT_FLAG_INTERNAL_PULLUP;
    if (pullup) {
        sdmmc_host_pullup_en_internal(slot, slot_config->width);
    }

    configure_pin(clk, slot, GPIO_MODE_OUTPUT);
    configure_pin(cmd, slot, GPIO_MODE_INPUT_OUTPUT);
    configure_pin(d0, slot, GPIO_MODE_INPUT_OUTPUT);

    if (slot_width >= 4) {
        configure_pin(d1, slot, GPIO_MODE_INPUT_OUTPUT);
        configure_pin(d2, slot, GPIO_MODE_INPUT_OUTPUT);
        // Force D3 high to make slave enter SD mode.
        // Connect to peripheral after width configuration.
        gpio_config_t gpio_conf = {
            .pin_bit_mask = BIT64(GPIO_NUM(slot, d3)),
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = 0,
            .pull_down_en = 0,
            .intr_type = GPIO_INTR_DISABLE,
        };
        gpio_config(&gpio_conf);
        gpio_set_level(GPIO_NUM(slot, d3), 1);
    }
    if (slot_width == 8) {
        configure_pin(d4, slot, GPIO_MODE_INPUT_OUTPUT);
        configure_pin(d5, slot, GPIO_MODE_INPUT_OUTPUT);
        configure_pin(d6, slot, GPIO_MODE_INPUT_OUTPUT);
        configure_pin(d7, slot, GPIO_MODE_INPUT_OUTPUT);
    }

    // SDIO slave interrupt is edge sensitive to ~(int_n | card_int | card_detect)
    // set this and card_detect to high to enable sdio interrupt
    esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ONE_INPUT, slot_info->card_int, false);

    // Set up Card Detect input
    int matrix_in_cd;
    if (gpio_cd != SDMMC_SLOT_NO_CD) {
        ESP_LOGD(TAG, "using GPIO%d as CD pin", gpio_cd);
        esp_rom_gpio_pad_select_gpio(gpio_cd);
        gpio_set_direction(gpio_cd, GPIO_MODE_INPUT);
        matrix_in_cd = gpio_cd;
    } else {
        // if not set, default to CD low (card present)
        matrix_in_cd = GPIO_MATRIX_CONST_ZERO_INPUT;
    }
    esp_rom_gpio_connect_in_signal(matrix_in_cd, slot_info->card_detect, false);

    // Set up Write Protect input
    int matrix_in_wp;
    if (gpio_wp != SDMMC_SLOT_NO_WP) {
        ESP_LOGD(TAG, "using GPIO%d as WP pin", gpio_wp);
        esp_rom_gpio_pad_select_gpio(gpio_wp);
        gpio_set_direction(gpio_wp, GPIO_MODE_INPUT);
        matrix_in_wp = gpio_wp;
    } else {
        // if not set, default to WP high (not write protected)
        matrix_in_wp = GPIO_MATRIX_CONST_ONE_INPUT;
    }
    // As hardware expects an active-high signal,
    // if WP signal is active low, then invert it in GPIO matrix,
    // else keep it in its default state
    esp_rom_gpio_connect_in_signal(matrix_in_wp, slot_info->write_protect, (gpio_wp_polarity? false : true));

    // By default, set probing frequency (400kHz) and 1-bit bus
    esp_err_t ret = sdmmc_host_set_card_clk(slot, 400);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "setting probing freq and 1-bit bus failed");
        ESP_LOGE(TAG, "%s: sdmmc_host_set_card_clk returned 0x%x", __func__, ret);
        return ret;
    }
    ret = sdmmc_host_set_bus_width(slot, 1);
    if (ret != ESP_OK) {
        return ret;
    }
    return ESP_OK;
}

esp_err_t sdmmc_host_deinit(void)
{
    if (!s_intr_handle) {
        return ESP_ERR_INVALID_STATE;
    }
    esp_intr_free(s_intr_handle);
    s_intr_handle = NULL;
    vQueueDelete(s_event_queue);
    s_event_queue = NULL;
    vQueueDelete(s_io_intr_event);
    s_io_intr_event = NULL;
    sdmmc_host_input_clk_disable();
    sdmmc_host_transaction_handler_deinit();
    periph_module_disable(PERIPH_SDMMC_MODULE);
    return ESP_OK;
}

esp_err_t sdmmc_host_wait_for_event(int tick_count, sdmmc_event_t *out_event)
{
    if (!out_event) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!s_event_queue) {
        return ESP_ERR_INVALID_STATE;
    }
    int ret = xQueueReceive(s_event_queue, out_event, tick_count);
    if (ret == pdFALSE) {
        return ESP_ERR_TIMEOUT;
    }
    return ESP_OK;
}

esp_err_t sdmmc_host_set_bus_width(int slot, size_t width)
{
    if (!(slot == 0 || slot == 1)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (sdmmc_slot_info[slot].width < width) {
        return ESP_ERR_INVALID_ARG;
    }
    const uint16_t mask = BIT(slot);
    if (width == 1) {
        SDMMC.ctype.card_width_8 &= ~mask;
        SDMMC.ctype.card_width &= ~mask;
    } else if (width == 4) {
        SDMMC.ctype.card_width_8 &= ~mask;
        SDMMC.ctype.card_width |= mask;
        // D3 was set to GPIO high to force slave into SD mode, until 4-bit mode is set
        configure_pin(d3, slot, GPIO_MODE_INPUT_OUTPUT);
    } else if (width == 8) {
        SDMMC.ctype.card_width_8 |= mask;
        // D3 was set to GPIO high to force slave into SD mode, until 4-bit mode is set
        configure_pin(d3, slot, GPIO_MODE_INPUT_OUTPUT);
    } else {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_LOGD(TAG, "slot=%d width=%d", slot, width);
    return ESP_OK;
}

size_t sdmmc_host_get_slot_width(int slot)
{
    assert( slot == 0 || slot == 1 );
    return s_slot_width[slot];
}

esp_err_t sdmmc_host_set_bus_ddr_mode(int slot, bool ddr_enabled)
{
    if (!(slot == 0 || slot == 1)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_slot_width[slot] == 8 && ddr_enabled) {
        ESP_LOGW(TAG, "DDR mode with 8-bit bus width is not supported yet");
        // requires reconfiguring controller clock for 2x card frequency
        return ESP_ERR_NOT_SUPPORTED;
    }
    uint32_t mask = BIT(slot);
    if (ddr_enabled) {
        SDMMC.uhs.ddr |= mask;
        SDMMC.emmc_ddr_reg |= mask;
    } else {
        SDMMC.uhs.ddr &= ~mask;
        SDMMC.emmc_ddr_reg &= ~mask;
    }
    ESP_LOGD(TAG, "slot=%d ddr=%d", slot, ddr_enabled ? 1 : 0);
    return ESP_OK;
}

esp_err_t sdmmc_host_set_cclk_always_on(int slot, bool cclk_always_on)
{
    if (!(slot == 0 || slot == 1)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (cclk_always_on) {
        SDMMC.clkena.cclk_low_power &= ~BIT(slot);
    } else {
        SDMMC.clkena.cclk_low_power |= BIT(slot);
    }
    sdmmc_host_clock_update_command(slot);
    return ESP_OK;
}

static void sdmmc_host_dma_init(void)
{
    SDMMC.ctrl.dma_enable = 1;
    SDMMC.bmod.val = 0;
    SDMMC.bmod.sw_reset = 1;
    SDMMC.idinten.ni = 1;
    SDMMC.idinten.ri = 1;
    SDMMC.idinten.ti = 1;
}

void sdmmc_host_dma_stop(void)
{
    SDMMC.ctrl.use_internal_dma = 0;
    SDMMC.ctrl.dma_reset = 1;
    SDMMC.bmod.fb = 0;
    SDMMC.bmod.enable = 0;
}

void sdmmc_host_dma_prepare(sdmmc_desc_t *desc, size_t block_size, size_t data_size)
{
    // Set size of data and DMA descriptor pointer
    SDMMC.bytcnt = data_size;
    SDMMC.blksiz = block_size;
    SDMMC.dbaddr = desc;

    // Enable everything needed to use DMA
    SDMMC.ctrl.dma_enable = 1;
    SDMMC.ctrl.use_internal_dma = 1;
    SDMMC.bmod.enable = 1;
    SDMMC.bmod.fb = 1;
    sdmmc_host_dma_resume();
}

void sdmmc_host_dma_resume(void)
{
    SDMMC.pldmnd = 1;
}

bool sdmmc_host_card_busy(void)
{
    return SDMMC.status.data_busy == 1;
}

esp_err_t sdmmc_host_io_int_enable(int slot)
{
    configure_pin(d1, slot, GPIO_MODE_INPUT_OUTPUT);
    return ESP_OK;
}

esp_err_t sdmmc_host_io_int_wait(int slot, TickType_t timeout_ticks)
{
    /* SDIO interrupts are negedge sensitive ones: the status bit is only set
     * when first interrupt triggered.
     *
     * If D1 GPIO is low when entering this function, we know that interrupt
     * (in SDIO sense) has occurred and we don't need to use SDMMC peripheral
     * interrupt.
     */

    SDMMC.intmask.sdio &= ~BIT(slot);   /* Disable SDIO interrupt */
    SDMMC.rintsts.sdio = BIT(slot);
    if (gpio_get_level(GPIO_NUM(slot, d1)) == 0) {
        return ESP_OK;
    }
    /* Otherwise, need to wait for an interrupt. Since D1 was high,
     * SDMMC peripheral interrupt is guaranteed to trigger on negedge.
     */
    xSemaphoreTake(s_io_intr_event, 0);
    SDMMC.intmask.sdio |= BIT(slot);    /* Re-enable SDIO interrupt */

    if (xSemaphoreTake(s_io_intr_event, timeout_ticks) == pdTRUE) {
        return ESP_OK;
    } else {
        return ESP_ERR_TIMEOUT;
    }
}

/**
 * @brief SDMMC interrupt handler
 *
 * All communication in SD protocol is driven by the master, and the hardware
 * handles things like stop commands automatically.
 * So the interrupt handler doesn't need to do much, we just push interrupt
 * status into a queue, clear interrupt flags, and let the task currently
 * doing communication figure out what to do next.
 * This also applies to SDIO interrupts which are generated by the slave.
 *
 * Card detect interrupts pose a small issue though, because if a card is
 * plugged in and out a few times, while there is no task to process
 * the events, event queue can become full and some card detect events
 * may be dropped. We ignore this problem for now, since the there are no other
 * interesting events which can get lost due to this.
 */
static void sdmmc_isr(void *arg)
{
    QueueHandle_t queue = (QueueHandle_t) arg;
    sdmmc_event_t event;
    int higher_priority_task_awoken = pdFALSE;

    uint32_t pending = SDMMC.mintsts.val & 0xFFFF;
    SDMMC.rintsts.val = pending;
    event.sdmmc_status = pending;

    uint32_t dma_pending = SDMMC.idsts.val;
    SDMMC.idsts.val = dma_pending;
    event.dma_status = dma_pending & 0x1f;

    if (pending != 0 || dma_pending != 0) {
        xQueueSendFromISR(queue, &event, &higher_priority_task_awoken);
    }

    uint32_t sdio_pending = SDMMC.mintsts.sdio;
    if (sdio_pending) {
        // disable the interrupt (no need to clear here, this is done in sdmmc_host_io_int_wait)
        SDMMC.intmask.sdio &= ~sdio_pending;
        xSemaphoreGiveFromISR(s_io_intr_event, &higher_priority_task_awoken);
    }

    if (higher_priority_task_awoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

static esp_err_t sdmmc_host_pullup_en_internal(int slot, int width)
{
    if (width > sdmmc_slot_info[slot].width) {
        //in esp32 we only support 8 bit in slot 0, note this is occupied by the flash by default
        return ESP_ERR_INVALID_ARG;
    }
    // according to the spec, the host controls the clk, we don't to pull it up here
    gpio_pullup_en(GPIO_NUM(slot, cmd));
    gpio_pullup_en(GPIO_NUM(slot, d0));
    if (width >= 4) {
        gpio_pullup_en(GPIO_NUM(slot, d1));
        gpio_pullup_en(GPIO_NUM(slot, d2));
        gpio_pullup_en(GPIO_NUM(slot, d3));
    }
    if (width == 8) {
        gpio_pullup_en(GPIO_NUM(slot, d4));
        gpio_pullup_en(GPIO_NUM(slot, d5));
        gpio_pullup_en(GPIO_NUM(slot, d6));
        gpio_pullup_en(GPIO_NUM(slot, d7));
    }
    return ESP_OK;
}
