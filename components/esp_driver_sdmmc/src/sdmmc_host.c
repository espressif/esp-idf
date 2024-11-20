/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <sys/param.h>
#include "esp_log.h"
#include "esp_intr_alloc.h"
#include "esp_timer.h"
#include "esp_check.h"
#include "soc/soc_caps.h"
#include "soc/gpio_periph.h"
#include "esp_rom_gpio.h"
#include "esp_rom_sys.h"
#include "driver/gpio.h"
#include "driver/sdmmc_host.h"
#include "esp_private/esp_clk_tree_common.h"
#include "esp_private/periph_ctrl.h"
#include "sdmmc_internal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_clk_tree.h"
#include "soc/sdmmc_periph.h"
#include "soc/soc_caps.h"
#include "hal/gpio_hal.h"
#include "hal/sdmmc_hal.h"
#include "hal/sd_types.h"
#include "hal/sdmmc_ll.h"

#define SDMMC_EVENT_QUEUE_LENGTH    32

#define SDMMC_FREQ_SDR104           208000      /*!< MMC 208MHz speed */

#if !SOC_RCC_IS_INDEPENDENT
// Reset and Clock Control registers are mixing with other peripherals, so we need to use a critical section
#define SDMMC_RCC_ATOMIC()          PERIPH_RCC_ATOMIC()
#else
#define SDMMC_RCC_ATOMIC()
#endif

#if SOC_PERIPH_CLK_CTRL_SHARED
// Clock source and related clock settings are mixing with other peripherals, so we need to use a critical section
#define SDMMC_CLK_SRC_ATOMIC()      PERIPH_RCC_ATOMIC()
#else
#define SDMMC_CLK_SRC_ATOMIC()
#endif

static const char *TAG = "sdmmc_periph";

#define SLOT_CHECK(slot_num) \
if (slot_num < 0 || slot_num >= SOC_SDMMC_NUM_SLOTS) { \
    return ESP_ERR_INVALID_ARG; \
}

#define GPIO_NUM_CHECK(_gpio_num) \
if (!GPIO_IS_VALID_GPIO(_gpio_num)) { \
    esp_err_t _err = ESP_ERR_INVALID_ARG; \
    ESP_LOGE(TAG, "%s: Invalid GPIO number %d, returned 0x%x", __func__, _gpio_num, _err); \
    return _err; \
}

/**
 * Slot contexts
 */
typedef struct slot_ctx_t {
    int slot_id;
    size_t slot_width;
    sdmmc_slot_io_info_t slot_gpio_num;
    bool use_gpio_matrix;
    bool is_uhs1;
#if SOC_SDMMC_NUM_SLOTS >= 2
    int slot_host_div;
    uint32_t slot_freq_khz;
    sdmmc_ll_delay_phase_t slot_ll_delay_phase;
#endif
} slot_ctx_t;

/**
 * Host contexts
 */
typedef struct host_ctx_t {
    intr_handle_t              intr_handle;
    QueueHandle_t              event_queue;
    SemaphoreHandle_t          io_intr_event;
    sdmmc_hal_context_t        hal;
    soc_periph_sdmmc_clk_src_t clk_src;
    slot_ctx_t                 slot_ctx[SOC_SDMMC_NUM_SLOTS];
#if SOC_SDMMC_NUM_SLOTS >= 2
    uint8_t                    num_of_init_slots;
    int8_t                     active_slot_num;
#endif
} host_ctx_t;

#if SOC_SDMMC_NUM_SLOTS >= 2
static host_ctx_t s_host_ctx = {.active_slot_num = -1};
#else
static host_ctx_t s_host_ctx = {0};
#endif

static void sdmmc_isr(void *arg);
static esp_err_t sdmmc_host_pullup_en_internal(int slot, int width);
static bool sdmmc_host_slot_initialized(int slot);
#if SOC_SDMMC_NUM_SLOTS >= 2
static void sdmmc_host_change_to_slot(int slot);
#endif

static void s_module_reset(void)
{
    // reset module
    sdmmc_ll_reset_controller(s_host_ctx.hal.dev);
    sdmmc_ll_reset_dma(s_host_ctx.hal.dev);
    sdmmc_ll_reset_fifo(s_host_ctx.hal.dev);
}

static bool s_is_module_reset_done(void)
{
    bool is_done = sdmmc_ll_is_controller_reset_done(s_host_ctx.hal.dev) && sdmmc_ll_is_dma_reset_done(s_host_ctx.hal.dev) && sdmmc_ll_is_fifo_reset_done(s_host_ctx.hal.dev);
    return is_done;
}

esp_err_t sdmmc_host_reset(void)
{
    s_module_reset();

    // Wait for the reset bits to be cleared by hardware
    int64_t yield_delay_us = 100 * 1000; // initially 100ms
    int64_t t0 = esp_timer_get_time();
    int64_t t1 = 0;
    while (!s_is_module_reset_done()) {
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
static void sdmmc_host_set_clk_div(soc_periph_sdmmc_clk_src_t src, int div)
{
    esp_clk_tree_enable_src((soc_module_clk_t)src, true);
    SDMMC_CLK_SRC_ATOMIC() {
        sdmmc_ll_set_clock_div(s_host_ctx.hal.dev, div);
        sdmmc_ll_select_clk_source(s_host_ctx.hal.dev, src);
        sdmmc_ll_init_phase_delay(s_host_ctx.hal.dev);
#if SOC_CLK_SDIO_PLL_SUPPORTED
        if (src == SDMMC_CLK_SRC_SDIO_200M) {
            sdmmc_ll_enable_sdio_pll(s_host_ctx.hal.dev, true);
        }
#endif
    }

    // Wait for the clock to propagate
    esp_rom_delay_us(10);
}

static esp_err_t sdmmc_host_clock_update_command(int slot, bool is_cmd11)
{
    // Clock update command (not a real command; just updates CIU registers)
    sdmmc_hw_cmd_t cmd_val = {
        .card_num = slot,
        .update_clk_reg = 1,
        .wait_complete = 1
    };
    if (is_cmd11) {
        cmd_val.volt_switch = 1;
    }
    ESP_RETURN_ON_ERROR(sdmmc_host_start_command(slot, cmd_val, 0), TAG, "sdmmc_host_start_command returned 0x%x", err_rc_);

    return ESP_OK;
}

void sdmmc_host_get_clk_dividers(uint32_t freq_khz, int *host_div, int *card_div, soc_periph_sdmmc_clk_src_t *src)
{
    uint32_t clk_src_freq_hz = 0;
    soc_periph_sdmmc_clk_src_t clk_src = 0;
#if SOC_SDMMC_UHS_I_SUPPORTED
    if (freq_khz > SDMMC_FREQ_HIGHSPEED) {
        clk_src = SDMMC_CLK_SRC_SDIO_200M;
    } else
#endif
    {
        clk_src = SDMMC_CLK_SRC_DEFAULT;
    }
    s_host_ctx.clk_src = clk_src;

    esp_err_t ret = esp_clk_tree_src_get_freq_hz(clk_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &clk_src_freq_hz);
    assert(ret == ESP_OK);
    ESP_LOGD(TAG, "clk_src_freq_hz: %"PRId32" hz", clk_src_freq_hz);

#if SDMMC_LL_MAX_FREQ_KHZ_FPGA
    if (freq_khz >= SDMMC_LL_MAX_FREQ_KHZ_FPGA) {
        ESP_LOGW(TAG, "working on FPGA, fallback to use the %d KHz", SDMMC_LL_MAX_FREQ_KHZ_FPGA);
        freq_khz = SDMMC_LL_MAX_FREQ_KHZ_FPGA;
    }
#endif

    // Calculate new dividers
#if SOC_SDMMC_UHS_I_SUPPORTED
    if (freq_khz == SDMMC_FREQ_SDR104) {
        *host_div = 1;       // 200 MHz / 1 = 200 MHz
        *card_div = 0;
    } else if (freq_khz == SDMMC_FREQ_SDR50) {
        *host_div = 2;       // 200 MHz / 2 = 100 MHz
        *card_div = 0;
    } else
#endif
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
            if (*host_div > 15) {
                *host_div = 2;
                *card_div = (clk_src_freq_hz / 2) / (2 * freq_khz * 1000);
                if (((clk_src_freq_hz / 2) % (2 * freq_khz * 1000)) > 0) {
                    (*card_div)++;
                }
            } else if ((clk_src_freq_hz % (freq_khz * 1000)) > 0) {
                (*host_div)++;
            }
        }

    *src = clk_src;
}

static int sdmmc_host_calc_freq(soc_periph_sdmmc_clk_src_t src, const int host_div, const int card_div)
{
    uint32_t clk_src_freq_hz = 0;
    esp_err_t ret = esp_clk_tree_src_get_freq_hz(src, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &clk_src_freq_hz);
    assert(ret == ESP_OK);

    return clk_src_freq_hz / host_div / ((card_div == 0) ? 1 : card_div * 2) / 1000;
}

static void sdmmc_host_set_data_timeout(uint32_t freq_khz)
{
    const uint32_t data_timeout_ms = 100;
    uint32_t data_timeout_cycles = data_timeout_ms * freq_khz;
    sdmmc_ll_set_data_timeout(s_host_ctx.hal.dev, data_timeout_cycles);
}

esp_err_t sdmmc_host_set_card_clk(int slot, uint32_t freq_khz)
{
    SLOT_CHECK(slot);

    // Disable clock first
    sdmmc_ll_enable_card_clock(s_host_ctx.hal.dev, slot, false);
    esp_err_t err = sdmmc_host_clock_update_command(slot, false);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "disabling clk failed");
        ESP_LOGE(TAG, "%s: sdmmc_host_clock_update_command returned 0x%x", __func__, err);
        return err;
    }

    soc_periph_sdmmc_clk_src_t clk_src = 0;
    int host_div = 0;   /* clock divider of the host (SDMMC.clock) */
    int card_div = 0;   /* 1/2 of card clock divider (SDMMC.clkdiv) */
    sdmmc_host_get_clk_dividers(freq_khz, &host_div, &card_div, &clk_src);

    int real_freq = sdmmc_host_calc_freq(clk_src, host_div, card_div);
    ESP_LOGD(TAG, "slot=%d clk_src=%d host_div=%d card_div=%d freq=%dkHz (max %" PRIu32 "kHz)", slot, clk_src, host_div, card_div, real_freq, freq_khz);

    // Program card clock settings, send them to the CIU
    sdmmc_ll_set_card_clock_div(s_host_ctx.hal.dev, slot, card_div);
    sdmmc_host_set_clk_div(clk_src, host_div);
    err = sdmmc_host_clock_update_command(slot, false);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "setting clk div failed");
        ESP_LOGE(TAG, "%s: sdmmc_host_clock_update_command returned 0x%x", __func__, err);
        return err;
    }

    // Re-enable clocks
    sdmmc_ll_enable_card_clock(s_host_ctx.hal.dev, slot, true);
    sdmmc_ll_enable_card_clock_low_power(s_host_ctx.hal.dev, slot, true);
    err = sdmmc_host_clock_update_command(slot, false);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "re-enabling clk failed");
        ESP_LOGE(TAG, "%s: sdmmc_host_clock_update_command returned 0x%x", __func__, err);
        return err;
    }

    sdmmc_host_set_data_timeout(freq_khz);
    // always set response timeout to highest value, it's small enough anyway
    sdmmc_ll_set_response_timeout(s_host_ctx.hal.dev, 255);
#if SOC_SDMMC_NUM_SLOTS >= 2
    // save the current frequency
    s_host_ctx.slot_ctx[slot].slot_freq_khz = freq_khz;
    // save host_div value
    s_host_ctx.slot_ctx[slot].slot_host_div = host_div;
#endif
    return ESP_OK;
}

esp_err_t sdmmc_host_get_real_freq(int slot, int *real_freq_khz)
{
    SLOT_CHECK(slot);

    if (real_freq_khz == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    int host_div = sdmmc_ll_get_clock_div(s_host_ctx.hal.dev);
    int card_div = sdmmc_ll_get_card_clock_div(s_host_ctx.hal.dev, slot);
    *real_freq_khz = sdmmc_host_calc_freq(s_host_ctx.clk_src, host_div, card_div);

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
    ESP_RETURN_ON_ERROR(esp_clk_tree_src_get_freq_hz(s_host_ctx.clk_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &clk_src_freq_hz),
                        TAG, "get source clock frequency failed");

    //Now we're in high speed. Note ESP SDMMC Host HW only supports integer divider.
    int delay_phase_num = 0;
    sdmmc_ll_delay_phase_t phase = SDMMC_LL_DELAY_PHASE_0;
    switch (delay_phase) {
    case SDMMC_DELAY_PHASE_1:
        phase = SDMMC_LL_DELAY_PHASE_1;
        delay_phase_num = 1;
        break;
    case SDMMC_DELAY_PHASE_2:
        phase = SDMMC_LL_DELAY_PHASE_2;
        delay_phase_num = 2;
        break;
    case SDMMC_DELAY_PHASE_3:
        phase = SDMMC_LL_DELAY_PHASE_3;
        delay_phase_num = 3;
        break;
    default:
        phase = SDMMC_LL_DELAY_PHASE_0;
        delay_phase_num = 0;
        break;
    }
    SDMMC_CLK_SRC_ATOMIC() {
        sdmmc_ll_set_din_delay(s_host_ctx.hal.dev, phase);
    }

    int src_clk_period_ps = (1 * 1000 * 1000) / (clk_src_freq_hz / (1 * 1000 * 1000));
    int phase_diff_ps = src_clk_period_ps * sdmmc_ll_get_clock_div(s_host_ctx.hal.dev) / SOC_SDMMC_DELAY_PHASE_NUM;
    ESP_LOGD(TAG, "difference between input delay phases is %d ps", phase_diff_ps);
    ESP_LOGI(TAG, "host sampling edge is delayed by %d ps", phase_diff_ps * delay_phase_num);
#if SOC_SDMMC_NUM_SLOTS >= 2
    // save the current phase delay setting
    s_host_ctx.slot_ctx[slot].slot_ll_delay_phase = phase;
#endif
#endif

    return ESP_OK;
}

esp_err_t sdmmc_host_start_command(int slot, sdmmc_hw_cmd_t cmd, uint32_t arg)
{
    SLOT_CHECK(slot);

#if SOC_SDMMC_NUM_SLOTS >= 2
    // Change the host settings to the appropriate slot before starting the transaction
    // If the slot is not initialized (slot_host_div not set) or already active, do nothing
    if (s_host_ctx.active_slot_num != slot) {
        s_host_ctx.active_slot_num = slot;
        if (sdmmc_host_slot_initialized(slot)) {
            sdmmc_host_change_to_slot(slot);
        } else {
            ESP_LOGD(TAG, "Slot %d is not initialized yet, skipping sdmmc_host_change_to_slot", slot);
        }
    }
#endif

    // if this isn't a clock update command, check the card detect status
    if (!sdmmc_ll_is_card_detected(s_host_ctx.hal.dev, slot) && !cmd.update_clk_reg) {
        return ESP_ERR_NOT_FOUND;
    }
    if (cmd.data_expected && cmd.rw && sdmmc_ll_is_card_write_protected(s_host_ctx.hal.dev, slot)) {
        return ESP_ERR_INVALID_STATE;
    }
    /* Outputs should be synchronized to cclk_out */
    cmd.use_hold_reg = 1;

    int64_t yield_delay_us = 100 * 1000; // initially 100ms
    int64_t t0 = esp_timer_get_time();
    int64_t t1 = 0;
    bool skip_wait = (cmd.volt_switch && cmd.update_clk_reg);
    if (!skip_wait) {
        while (!(sdmmc_ll_is_command_taken(s_host_ctx.hal.dev))) {
            t1 = esp_timer_get_time();
            if (t1 - t0 > SDMMC_HOST_START_CMD_TIMEOUT_US) {
                return ESP_ERR_TIMEOUT;
            }
            if (t1 - t0 > yield_delay_us) {
                yield_delay_us *= 2;
                vTaskDelay(1);
            }
        }
    }
    sdmmc_ll_set_command_arg(s_host_ctx.hal.dev, arg);
    cmd.card_num = slot;
    cmd.start_command = 1;
    sdmmc_ll_set_command(s_host_ctx.hal.dev, cmd);

    while (!(sdmmc_ll_is_command_taken(s_host_ctx.hal.dev))) {
        t1 = esp_timer_get_time();
        if (t1 - t0 > SDMMC_HOST_START_CMD_TIMEOUT_US) {
            return ESP_ERR_TIMEOUT;
        }
        if (t1 - t0 > yield_delay_us) {
            yield_delay_us *= 2;
            vTaskDelay(1);
        }
    }
    return ESP_OK;
}

static void sdmmc_host_intmask_clear_disable(void)
{
    sdmmc_ll_clear_interrupt(s_host_ctx.hal.dev, 0xffffffff);
    sdmmc_ll_enable_interrupt(s_host_ctx.hal.dev, 0xffffffff, false);
    sdmmc_ll_enable_global_interrupt(s_host_ctx.hal.dev, false);
}

static void sdmmc_host_intmask_set_enable(void)
{
    sdmmc_ll_enable_interrupt(s_host_ctx.hal.dev, 0xffffffff, false);
    sdmmc_ll_enable_interrupt(s_host_ctx.hal.dev, SDMMC_LL_EVENT_DEFAULT, true);
    sdmmc_ll_enable_global_interrupt(s_host_ctx.hal.dev, true);
}

esp_err_t sdmmc_host_init(void)
{
    if (s_host_ctx.intr_handle) {
        ESP_LOGI(TAG, "%s: SDMMC host already initialized, skipping init flow", __func__);
        return ESP_OK;
    }

    //enable bus clock for registers
    SDMMC_RCC_ATOMIC() {
        sdmmc_ll_enable_bus_clock(s_host_ctx.hal.dev, true);
        sdmmc_ll_reset_register(s_host_ctx.hal.dev);
    }

    //hal context init
    sdmmc_hal_init(&s_host_ctx.hal);

    // Enable clock to peripheral. Use smallest divider first.
    sdmmc_host_set_clk_div(SDMMC_CLK_SRC_DEFAULT, 2);

    // Reset
    esp_err_t err = sdmmc_host_reset();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: sdmmc_host_reset returned 0x%x", __func__, err);
        return err;
    }

    ESP_LOGD(TAG, "peripheral version %"PRIx32", hardware config %08"PRIx32, sdmmc_ll_get_version_id(s_host_ctx.hal.dev), sdmmc_ll_get_hw_config_info(s_host_ctx.hal.dev));

    // Clear interrupt status and set interrupt mask to known state
    sdmmc_host_intmask_clear_disable();

    // Allocate event queue
    s_host_ctx.event_queue = xQueueCreate(SDMMC_EVENT_QUEUE_LENGTH, sizeof(sdmmc_event_t));
    if (!s_host_ctx.event_queue) {
        return ESP_ERR_NO_MEM;
    }
    s_host_ctx.io_intr_event = xSemaphoreCreateBinary();
    if (!s_host_ctx.io_intr_event) {
        vQueueDelete(s_host_ctx.event_queue);
        s_host_ctx.event_queue = NULL;
        return ESP_ERR_NO_MEM;
    }
    // Attach interrupt handler
    esp_err_t ret = esp_intr_alloc(ETS_SDIO_HOST_INTR_SOURCE, 0, &sdmmc_isr, s_host_ctx.event_queue, &s_host_ctx.intr_handle);
    if (ret != ESP_OK) {
        vQueueDelete(s_host_ctx.event_queue);
        s_host_ctx.event_queue = NULL;
        vSemaphoreDelete(s_host_ctx.io_intr_event);
        s_host_ctx.io_intr_event = NULL;
        return ret;
    }
    // Enable interrupts
    sdmmc_host_intmask_set_enable();

    // Disable generation of Busy Clear Interrupt
    sdmmc_ll_enable_busy_clear_interrupt(s_host_ctx.hal.dev, false);

    // Init DMA
    sdmmc_ll_init_dma(s_host_ctx.hal.dev);

    // Initialize transaction handler
    ret = sdmmc_host_transaction_handler_init();
    if (ret != ESP_OK) {
        vQueueDelete(s_host_ctx.event_queue);
        s_host_ctx.event_queue = NULL;
        vSemaphoreDelete(s_host_ctx.io_intr_event);
        s_host_ctx.io_intr_event = NULL;
        esp_intr_free(s_host_ctx.intr_handle);
        s_host_ctx.intr_handle = NULL;
        return ret;
    }

    return ESP_OK;
}

static void configure_pin_iomux(uint8_t gpio_num)
{
    const int sdmmc_func = SDMMC_LL_IOMUX_FUNC;
    const int drive_strength = 3;
    assert(gpio_num != (uint8_t) GPIO_NUM_NC);
    gpio_pulldown_dis(gpio_num);

    uint32_t reg = GPIO_PIN_MUX_REG[gpio_num];
    assert(reg != UINT32_MAX);
    PIN_INPUT_ENABLE(reg);
    gpio_hal_iomux_func_sel(reg, sdmmc_func);
    PIN_SET_DRV(reg, drive_strength);
}

static void configure_pin_gpio_matrix(uint8_t gpio_num, uint8_t gpio_matrix_sig, gpio_mode_t mode, const char *name)
{
    assert(gpio_num != (uint8_t) GPIO_NUM_NC);
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

static void configure_pin(uint8_t gpio_num, uint8_t gpio_matrix_sig, gpio_mode_t mode, const char *name, bool use_gpio_matrix)
{
    if (use_gpio_matrix) {
        configure_pin_gpio_matrix(gpio_num, gpio_matrix_sig, mode, name);
    } else {
        configure_pin_iomux(gpio_num);
    }
}

//True: pins are all not set; False: one or more pins are set
static bool s_check_pin_not_set(const sdmmc_slot_config_t *slot_config)
{
#if SOC_SDMMC_USE_GPIO_MATRIX
    bool pin_not_set = !slot_config->clk && !slot_config->cmd && !slot_config->d0 && !slot_config->d1 && !slot_config->d2 &&
                       !slot_config->d3 && !slot_config->d4 && !slot_config->d5 && !slot_config->d6 && !slot_config->d7;
    return pin_not_set;
#else
    return true;
#endif
}

esp_err_t sdmmc_host_is_slot_set_to_uhs1(int slot, bool *is_uhs1)
{
    if (s_host_ctx.slot_ctx[slot].slot_id != slot) {
        ESP_LOGE(TAG, "%s: slot %d isn't initialized", __func__, slot);
        return ESP_ERR_INVALID_STATE;
    }

    *is_uhs1 = s_host_ctx.slot_ctx[slot].is_uhs1;

    return ESP_OK;
}

esp_err_t sdmmc_host_init_slot(int slot, const sdmmc_slot_config_t *slot_config)
{
    if (!s_host_ctx.intr_handle) {
        return ESP_ERR_INVALID_STATE;
    }

    SLOT_CHECK(slot);

    if (slot_config == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (slot_config->flags & SDMMC_SLOT_FLAG_UHS1) {
        s_host_ctx.slot_ctx[slot].is_uhs1 = true;
    }

    int gpio_cd = slot_config->cd;
    int gpio_wp = slot_config->wp;
    bool gpio_wp_polarity = slot_config->flags & SDMMC_SLOT_FLAG_WP_ACTIVE_HIGH;
    uint8_t slot_width = slot_config->width;

    // Configure pins
    const sdmmc_slot_info_t *slot_info = &sdmmc_slot_info[slot];
    sdmmc_slot_io_info_t *slot_gpio = &s_host_ctx.slot_ctx[slot].slot_gpio_num;

    if (slot_width == SDMMC_SLOT_WIDTH_DEFAULT) {
        slot_width = slot_info->width;
    } else if (slot_width > slot_info->width) {
        return ESP_ERR_INVALID_ARG;
    }
    s_host_ctx.slot_ctx[slot].slot_width = slot_width;
    slot_gpio->cd = gpio_cd;
    slot_gpio->wp = gpio_wp;

    bool pin_not_set = s_check_pin_not_set(slot_config);
    //SD driver behaviour is: all pins not defined == using iomux
    bool use_gpio_matrix = !pin_not_set;

    if (slot == 0) {
#if !SDMMC_LL_SLOT_SUPPORT_GPIO_MATRIX(0)
        if (use_gpio_matrix &&
                SDMMC_SLOT0_IOMUX_PIN_NUM_CLK == slot_config->clk &&
                SDMMC_SLOT0_IOMUX_PIN_NUM_CMD == slot_config->cmd &&
                SDMMC_SLOT0_IOMUX_PIN_NUM_D0 == slot_config->d0 &&
                SDMMC_SLOT0_IOMUX_PIN_NUM_D1 == slot_config->d1 &&
                SDMMC_SLOT0_IOMUX_PIN_NUM_D2 == slot_config->d2 &&
                SDMMC_SLOT0_IOMUX_PIN_NUM_D3 == slot_config->d3) {
            use_gpio_matrix = false;
        } else {
            ESP_RETURN_ON_FALSE(!use_gpio_matrix, ESP_ERR_INVALID_ARG, TAG, "doesn't support routing from GPIO matrix, driver uses dedicated IOs");
        }
#endif
    } else {
#if !SDMMC_LL_SLOT_SUPPORT_GPIO_MATRIX(1)
        ESP_RETURN_ON_FALSE(!use_gpio_matrix, ESP_ERR_INVALID_ARG, TAG, "doesn't support routing from GPIO matrix, driver uses dedicated IOs");
#endif
    }
    s_host_ctx.slot_ctx[slot].use_gpio_matrix = use_gpio_matrix;

#if SOC_SDMMC_USE_GPIO_MATRIX
    if (use_gpio_matrix) {
        /* Save pin configuration for this slot */
        slot_gpio->clk = slot_config->clk;
        slot_gpio->cmd = slot_config->cmd;
        slot_gpio->d0 = slot_config->d0;
        /* Save d1 even in 1-line mode, it might be needed for SDIO INT line */
        slot_gpio->d1 = slot_config->d1;
        if (slot_width >= 4) {
            slot_gpio->d2 = slot_config->d2;
        }
        /* Save d3 even for 1-line mode, as it needs to be set high */
        slot_gpio->d3 = slot_config->d3;
        if (slot_width >= 8) {
            slot_gpio->d4 = slot_config->d4;
            slot_gpio->d5 = slot_config->d5;
            slot_gpio->d6 = slot_config->d6;
            slot_gpio->d7 = slot_config->d7;
        }
    } else
#endif  //#if SOC_SDMMC_USE_GPIO_MATRIX
    {
        /* init pin configuration for this slot */
        slot_gpio->clk = sdmmc_slot_gpio_num[slot].clk;
        slot_gpio->cmd = sdmmc_slot_gpio_num[slot].cmd;
        slot_gpio->d0 = sdmmc_slot_gpio_num[slot].d0;
        slot_gpio->d1 = sdmmc_slot_gpio_num[slot].d1;
        slot_gpio->d2 = sdmmc_slot_gpio_num[slot].d2;
        slot_gpio->d3 = sdmmc_slot_gpio_num[slot].d3;
        slot_gpio->d4 = sdmmc_slot_gpio_num[slot].d4;
        slot_gpio->d5 = sdmmc_slot_gpio_num[slot].d5;
        slot_gpio->d6 = sdmmc_slot_gpio_num[slot].d6;
        slot_gpio->d7 = sdmmc_slot_gpio_num[slot].d7;
    }

    bool pullup = slot_config->flags & SDMMC_SLOT_FLAG_INTERNAL_PULLUP;
    if (pullup) {
        sdmmc_host_pullup_en_internal(slot, s_host_ctx.slot_ctx[slot].slot_width);
    }

    if (slot_width >= 1) {
        GPIO_NUM_CHECK(slot_gpio->clk);
        GPIO_NUM_CHECK(slot_gpio->cmd);
        GPIO_NUM_CHECK(slot_gpio->d0);
    }
    if (slot_width >= 4) {
        GPIO_NUM_CHECK(slot_gpio->d1);
        GPIO_NUM_CHECK(slot_gpio->d2);
        GPIO_NUM_CHECK(slot_gpio->d3);
    }
    if (slot_width == 8) {
        GPIO_NUM_CHECK(slot_gpio->d4);
        GPIO_NUM_CHECK(slot_gpio->d5);
        GPIO_NUM_CHECK(slot_gpio->d6);
        GPIO_NUM_CHECK(slot_gpio->d7);
    }

    configure_pin(slot_gpio->clk, sdmmc_slot_gpio_sig[slot].clk, GPIO_MODE_OUTPUT, "clk", use_gpio_matrix);
    configure_pin(slot_gpio->cmd, sdmmc_slot_gpio_sig[slot].cmd, GPIO_MODE_INPUT_OUTPUT, "cmd", use_gpio_matrix);
    configure_pin(slot_gpio->d0, sdmmc_slot_gpio_sig[slot].d0, GPIO_MODE_INPUT_OUTPUT, "d0", use_gpio_matrix);

    if (slot_width >= 4) {
        configure_pin(slot_gpio->d1, sdmmc_slot_gpio_sig[slot].d1, GPIO_MODE_INPUT_OUTPUT, "d1", use_gpio_matrix);
        configure_pin(slot_gpio->d2, sdmmc_slot_gpio_sig[slot].d2, GPIO_MODE_INPUT_OUTPUT, "d2", use_gpio_matrix);
        if (s_host_ctx.slot_ctx[slot].is_uhs1) {
            configure_pin(slot_gpio->d3, sdmmc_slot_gpio_sig[slot].d3, GPIO_MODE_INPUT_OUTPUT, "d3", use_gpio_matrix);
        } else {
            // Force D3 high to make slave enter SD mode.
            // Connect to peripheral after width configuration.
            if (slot_gpio->d3 > GPIO_NUM_NC) {
                gpio_config_t gpio_conf = {
                    .pin_bit_mask = BIT64(slot_gpio->d3),
                    .mode = GPIO_MODE_OUTPUT,
                    .pull_up_en = 0,
                    .pull_down_en = 0,
                    .intr_type = GPIO_INTR_DISABLE,
                };
                gpio_config(&gpio_conf);
                gpio_set_level(slot_gpio->d3, 1);
            }
        }
    }
    if (slot_width == 8) {
        configure_pin(slot_gpio->d4, sdmmc_slot_gpio_sig[slot].d4, GPIO_MODE_INPUT_OUTPUT, "d4", use_gpio_matrix);
        configure_pin(slot_gpio->d5, sdmmc_slot_gpio_sig[slot].d5, GPIO_MODE_INPUT_OUTPUT, "d5", use_gpio_matrix);
        configure_pin(slot_gpio->d6, sdmmc_slot_gpio_sig[slot].d6, GPIO_MODE_INPUT_OUTPUT, "d6", use_gpio_matrix);
        configure_pin(slot_gpio->d7, sdmmc_slot_gpio_sig[slot].d7, GPIO_MODE_INPUT_OUTPUT, "d7", use_gpio_matrix);
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
    esp_rom_gpio_connect_in_signal(matrix_in_wp, slot_info->write_protect, (gpio_wp_polarity ? false : true));

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

    s_host_ctx.slot_ctx[slot].slot_id = slot;

#if SOC_SDMMC_NUM_SLOTS >= 2
    if (s_host_ctx.num_of_init_slots < SOC_SDMMC_NUM_SLOTS && s_host_ctx.active_slot_num != slot) {
        s_host_ctx.num_of_init_slots += 1;
    }
    s_host_ctx.active_slot_num = slot;
#endif
    return ESP_OK;
}

static void sdmmc_host_deinit_internal(void)
{
    esp_intr_free(s_host_ctx.intr_handle);
    s_host_ctx.intr_handle = NULL;
    vQueueDelete(s_host_ctx.event_queue);
    s_host_ctx.event_queue = NULL;
    vQueueDelete(s_host_ctx.io_intr_event);
    s_host_ctx.io_intr_event = NULL;
    sdmmc_ll_deinit_clk(s_host_ctx.hal.dev);
    sdmmc_host_transaction_handler_deinit();
    //disable bus clock for registers
    SDMMC_RCC_ATOMIC() {
        sdmmc_ll_enable_bus_clock(s_host_ctx.hal.dev, false);
    }
    ESP_LOGD(TAG, "SDMMC host deinitialized");
}

static int sdmmc_host_decrease_init_slot_num(void)
{
#if SOC_SDMMC_NUM_SLOTS >= 2
    s_host_ctx.active_slot_num = -1; // Reset the active slot number, will be set again before the next transaction
    if (s_host_ctx.num_of_init_slots > 0) {
        s_host_ctx.num_of_init_slots -= 1;
    }
    return s_host_ctx.num_of_init_slots;
#else
    return 0;
#endif
}

static void sdmmc_host_deinit_slot_internal(int slot)
{
    int8_t gpio_pin_num;
    sdmmc_slot_io_info_t* gpio = &s_host_ctx.slot_ctx[slot].slot_gpio_num;
    // Disconnect signals and reset used GPIO pins
    for (size_t i = 0; i < (sizeof(gpio->val) / (sizeof(gpio->val[0]))); i++) {
        gpio_pin_num = gpio->val[i];
        if (gpio_pin_num != GPIO_NUM_NC && GPIO_IS_VALID_GPIO(gpio_pin_num)) {
            gpio_reset_pin(gpio_pin_num);
        }
    }
    // Reset the slot context
    memset(&(s_host_ctx.slot_ctx[slot]), 0, sizeof(slot_ctx_t));
}

esp_err_t sdmmc_host_deinit_slot(int slot)
{
    if (!(slot == 0 || slot == 1)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!s_host_ctx.intr_handle) {
        return ESP_ERR_INVALID_STATE;
    }
    sdmmc_host_deinit_slot_internal(slot);
    int num_of_init_slots = sdmmc_host_decrease_init_slot_num();
    if (num_of_init_slots != 0) {
        ESP_LOGD(TAG, "SDMMC host not deinitialized yet, number of initialized slots: %d",
                 num_of_init_slots);
        return ESP_OK;
    }
    sdmmc_host_deinit_internal();

    return ESP_OK;
}

esp_err_t sdmmc_host_deinit(void)
{
    if (!s_host_ctx.intr_handle) {
        return ESP_ERR_INVALID_STATE;
    }
    for (int slot = 0; slot < SOC_SDMMC_NUM_SLOTS; slot++) {
        sdmmc_host_deinit_slot_internal(slot);
    }
    sdmmc_host_deinit_internal();

    return ESP_OK;
}

static bool sdmmc_host_slot_initialized(int slot)
{
    // slot_host_div is initialized to 0 and is set in sdmmc_host_set_card_clk during card initialization
    // during card deinitialization it is set back to 0
    // should not be 0 if the slot is initialized
    if (s_host_ctx.slot_ctx[slot].slot_host_div == 0) {
        return false;
    }
    return true;
}

#if SOC_SDMMC_NUM_SLOTS >= 2
static void sdmmc_host_change_to_slot(int slot)
{
    // Apply the appropriate saved host settings for the new slot before starting the transaction
    SDMMC_CLK_SRC_ATOMIC() {
        sdmmc_ll_set_clock_div(s_host_ctx.hal.dev, s_host_ctx.slot_ctx[slot].slot_host_div);
#if !CONFIG_IDF_TARGET_ESP32
        sdmmc_ll_set_din_delay(s_host_ctx.hal.dev, s_host_ctx.slot_ctx[slot].slot_ll_delay_phase);
#endif
    }
    sdmmc_host_set_data_timeout(s_host_ctx.slot_ctx[slot].slot_freq_khz);

    // Wait for the clock to propagate
    esp_rom_delay_us(10);
}
#endif  // SOC_SDMMC_NUM_SLOTS >= 2

esp_err_t sdmmc_host_wait_for_event(int tick_count, sdmmc_event_t *out_event)
{
    if (!out_event) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!s_host_ctx.event_queue) {
        return ESP_ERR_INVALID_STATE;
    }
    int ret = xQueueReceive(s_host_ctx.event_queue, out_event, tick_count);
    if (ret == pdFALSE) {
        return ESP_ERR_TIMEOUT;
    }
    return ESP_OK;
}

esp_err_t sdmmc_host_set_bus_width(int slot, size_t width)
{
    SLOT_CHECK(slot);

    if (sdmmc_slot_info[slot].width < width) {
        return ESP_ERR_INVALID_ARG;
    }
    if (width == 1) {
        sdmmc_ll_set_card_width(s_host_ctx.hal.dev, slot, SD_BUS_WIDTH_1_BIT);
    } else if (width == 4) {
        sdmmc_ll_set_card_width(s_host_ctx.hal.dev, slot, SD_BUS_WIDTH_4_BIT);
        // D3 was set to GPIO high to force slave into SD mode, until 4-bit mode is set
        configure_pin(s_host_ctx.slot_ctx[slot].slot_gpio_num.d3, sdmmc_slot_gpio_sig[slot].d3, GPIO_MODE_INPUT_OUTPUT, "d3", s_host_ctx.slot_ctx[slot].use_gpio_matrix);
    } else if (width == 8) {
        sdmmc_ll_set_card_width(s_host_ctx.hal.dev, slot, SD_BUS_WIDTH_8_BIT);
        // D3 was set to GPIO high to force slave into SD mode, until 4-bit mode is set
        configure_pin(s_host_ctx.slot_ctx[slot].slot_gpio_num.d3, sdmmc_slot_gpio_sig[slot].d3, GPIO_MODE_INPUT_OUTPUT, "d3", s_host_ctx.slot_ctx[slot].use_gpio_matrix);
    } else {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_LOGD(TAG, "slot=%d width=%d", slot, width);
    return ESP_OK;
}

size_t sdmmc_host_get_slot_width(int slot)
{
    assert(slot == 0 || slot == 1);
    return s_host_ctx.slot_ctx[slot].slot_width;
}

esp_err_t sdmmc_host_set_bus_ddr_mode(int slot, bool ddr_enabled)
{
    SLOT_CHECK(slot);

    if (s_host_ctx.slot_ctx[slot].slot_width == 8 && ddr_enabled) {
        ESP_LOGW(TAG, "DDR mode with 8-bit bus width is not supported yet");
        // requires reconfiguring controller clock for 2x card frequency
        return ESP_ERR_NOT_SUPPORTED;
    }

    sdmmc_ll_enable_ddr_mode(s_host_ctx.hal.dev, slot, ddr_enabled);
    ESP_LOGD(TAG, "slot=%d ddr=%d", slot, ddr_enabled ? 1 : 0);
    return ESP_OK;
}

esp_err_t sdmmc_host_set_cclk_always_on(int slot, bool cclk_always_on)
{
    SLOT_CHECK(slot);

    // During initialization this is not protected by a mutex
    if (cclk_always_on) {
        sdmmc_ll_enable_card_clock_low_power(s_host_ctx.hal.dev, slot, false);
    } else {
        sdmmc_ll_enable_card_clock_low_power(s_host_ctx.hal.dev, slot, true);
    }
    sdmmc_host_clock_update_command(slot, false);
    return ESP_OK;
}

void sdmmc_host_enable_clk_cmd11(int slot, bool enable)
{
    sdmmc_ll_enable_card_clock(s_host_ctx.hal.dev, slot, enable);
    sdmmc_host_clock_update_command(slot, true);
    sdmmc_ll_enable_1v8_mode(s_host_ctx.hal.dev, slot, enable);
}

void sdmmc_host_dma_stop(void)
{
    sdmmc_ll_stop_dma(s_host_ctx.hal.dev);
}

void sdmmc_host_dma_prepare(sdmmc_desc_t *desc, size_t block_size, size_t data_size)
{
    // Set size of data and DMA descriptor pointer
    sdmmc_ll_set_data_transfer_len(s_host_ctx.hal.dev, data_size);
    sdmmc_ll_set_block_size(s_host_ctx.hal.dev, block_size);
    sdmmc_ll_set_desc_addr(s_host_ctx.hal.dev, (uint32_t)desc);

    // Enable everything needed to use DMA
    sdmmc_ll_enable_dma(s_host_ctx.hal.dev, true);
    sdmmc_host_dma_resume();
}

void sdmmc_host_dma_resume(void)
{
    sdmmc_ll_poll_demand(s_host_ctx.hal.dev);
}

bool sdmmc_host_card_busy(void)
{
    return sdmmc_ll_is_card_data_busy(s_host_ctx.hal.dev);
}

esp_err_t sdmmc_host_io_int_enable(int slot)
{
    configure_pin(s_host_ctx.slot_ctx[slot].slot_gpio_num.d1, sdmmc_slot_gpio_sig[slot].d1, GPIO_MODE_INPUT_OUTPUT, "d1", s_host_ctx.slot_ctx[slot].use_gpio_matrix);
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
    assert(slot == 0 || slot == 1);

    /* Disable SDIO interrupt */
    if (slot == 0) {
        sdmmc_ll_enable_interrupt(s_host_ctx.hal.dev, SDMMC_INTMASK_IO_SLOT0, false);
        sdmmc_ll_clear_interrupt(s_host_ctx.hal.dev, SDMMC_INTMASK_IO_SLOT0);
    } else {
        sdmmc_ll_enable_interrupt(s_host_ctx.hal.dev, SDMMC_INTMASK_IO_SLOT1, false);
        sdmmc_ll_clear_interrupt(s_host_ctx.hal.dev, SDMMC_INTMASK_IO_SLOT1);
    }

    if (gpio_get_level(s_host_ctx.slot_ctx[slot].slot_gpio_num.d1) == 0) {
        return ESP_OK;
    }
    /* Otherwise, need to wait for an interrupt. Since D1 was high,
     * SDMMC peripheral interrupt is guaranteed to trigger on negedge.
     */
    xSemaphoreTake(s_host_ctx.io_intr_event, 0);
    /* Re-enable SDIO interrupt */
    if (slot == 0) {
        sdmmc_ll_enable_interrupt(s_host_ctx.hal.dev, SDMMC_INTMASK_IO_SLOT0, true);
    } else {
        sdmmc_ll_enable_interrupt(s_host_ctx.hal.dev, SDMMC_INTMASK_IO_SLOT1, true);
    }

    if (xSemaphoreTake(s_host_ctx.io_intr_event, timeout_ticks) == pdTRUE) {
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

    uint32_t pending = sdmmc_ll_get_intr_status(s_host_ctx.hal.dev) & SDMMC_LL_SD_EVENT_MASK;
    sdmmc_ll_clear_interrupt(s_host_ctx.hal.dev, pending);
    event.sdmmc_status = pending;

    uint32_t dma_pending = sdmmc_ll_get_idsts_interrupt_raw(s_host_ctx.hal.dev);
    sdmmc_ll_clear_idsts_interrupt(s_host_ctx.hal.dev, dma_pending);
    event.dma_status = dma_pending & 0x1f;

    if (pending != 0 || dma_pending != 0) {
        xQueueSendFromISR(queue, &event, &higher_priority_task_awoken);
    }

    uint32_t sdio_pending = (sdmmc_ll_get_intr_status(s_host_ctx.hal.dev) & (SDMMC_INTMASK_IO_SLOT1 | SDMMC_INTMASK_IO_SLOT0));
    if (sdio_pending) {
        // disable the interrupt (no need to clear here, this is done in sdmmc_host_io_int_wait)
        sdmmc_ll_enable_interrupt(s_host_ctx.hal.dev, sdio_pending, false);
        xSemaphoreGiveFromISR(s_host_ctx.io_intr_event, &higher_priority_task_awoken);
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
    gpio_pullup_en(s_host_ctx.slot_ctx[slot].slot_gpio_num.cmd);
    gpio_pullup_en(s_host_ctx.slot_ctx[slot].slot_gpio_num.d0);
    if (width >= 4) {
        gpio_pullup_en(s_host_ctx.slot_ctx[slot].slot_gpio_num.d1);
        gpio_pullup_en(s_host_ctx.slot_ctx[slot].slot_gpio_num.d2);
        gpio_pullup_en(s_host_ctx.slot_ctx[slot].slot_gpio_num.d3);
    }
    if (width == 8) {
        gpio_pullup_en(s_host_ctx.slot_ctx[slot].slot_gpio_num.d4);
        gpio_pullup_en(s_host_ctx.slot_ctx[slot].slot_gpio_num.d5);
        gpio_pullup_en(s_host_ctx.slot_ctx[slot].slot_gpio_num.d6);
        gpio_pullup_en(s_host_ctx.slot_ctx[slot].slot_gpio_num.d7);
    }
    return ESP_OK;
}

esp_err_t sdmmc_host_get_dma_info(int slot, esp_dma_mem_info_t *dma_mem_info)
{
    SLOT_CHECK(slot);

    if (dma_mem_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    dma_mem_info->extra_heap_caps = MALLOC_CAP_DMA;
    dma_mem_info->dma_alignment_bytes = 4;
    return ESP_OK;
}

esp_err_t sdmmc_host_get_state(sdmmc_host_state_t* state)
{
    if (state == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (s_host_ctx.intr_handle) {
        state->host_initialized = true;
        state->num_of_init_slots = 1;
    } else {
        state->host_initialized = false;
        state->num_of_init_slots = 0;
    }
#if SOC_SDMMC_NUM_SLOTS >= 2
    state->num_of_init_slots = s_host_ctx.num_of_init_slots;
#endif
    return ESP_OK;
}
