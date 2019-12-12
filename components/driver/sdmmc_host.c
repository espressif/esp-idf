// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdbool.h>
#include <stddef.h>
#include <sys/param.h>
#include "esp_log.h"
#include "esp_intr_alloc.h"
#include "soc/gpio_periph.h"
#include "esp32/rom/gpio.h"
#include "driver/gpio.h"
#include "driver/sdmmc_host.h"
#include "driver/periph_ctrl.h"
#include "sdmmc_private.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "soc/sdmmc_periph.h"

#define SDMMC_EVENT_QUEUE_LENGTH 32


static void sdmmc_isr(void* arg);
static void sdmmc_host_dma_init(void);


static const char* TAG = "sdmmc_periph";
static intr_handle_t s_intr_handle;
static QueueHandle_t s_event_queue;
static SemaphoreHandle_t s_io_intr_event;

size_t s_slot_width[2] = {1,1};

void sdmmc_host_reset(void)
{
    // Set reset bits
    SDMMC.ctrl.controller_reset = 1;
    SDMMC.ctrl.dma_reset = 1;
    SDMMC.ctrl.fifo_reset = 1;
    // Wait for the reset bits to be cleared by hardware
    while (SDMMC.ctrl.controller_reset || SDMMC.ctrl.fifo_reset || SDMMC.ctrl.dma_reset) {
        ;
    }
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
    // Set frequency to 160MHz / div
    // div = p + 1
    // duty cycle = (h + 1)/(p + 1) (should be = 1/2)
    assert (div > 1 && div <= 16);
    int p = div - 1;
    int h = div / 2 - 1;

    SDMMC.clock.div_factor_p = p;
    SDMMC.clock.div_factor_h = h;
    SDMMC.clock.div_factor_m = p;
    // Set phases for in/out clocks
    SDMMC.clock.phase_dout = 4;     // 180 degree phase on the output clock
    SDMMC.clock.phase_din = 4;      // 180 degree phase on the input clock
    SDMMC.clock.phase_core = 0;
    // Wait for the clock to propagate
    ets_delay_us(10);
}

static void sdmmc_host_input_clk_disable(void)
{
    SDMMC.clock.val = 0;
}

static void sdmmc_host_clock_update_command(int slot)
{
    // Clock update command (not a real command; just updates CIU registers)
    sdmmc_hw_cmd_t cmd_val = {
        .card_num = slot,
        .update_clk_reg = 1,
        .wait_complete = 1
    };
    bool repeat = true;
    while(repeat) {
        sdmmc_host_start_command(slot, cmd_val, 0);
        while (true) {
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
        }
    }
}

esp_err_t sdmmc_host_set_card_clk(int slot, uint32_t freq_khz)
{
    if (!(slot == 0 || slot == 1)) {
        return ESP_ERR_INVALID_ARG;
    }
    const int clk40m = 40000;

    // Disable clock first
    SDMMC.clkena.cclk_enable &= ~BIT(slot);
    sdmmc_host_clock_update_command(slot);

    int host_div = 0;   /* clock divider of the host (SDMMC.clock) */
    int card_div = 0;   /* 1/2 of card clock divider (SDMMC.clkdiv) */

    // Calculate new dividers
    if (freq_khz >= SDMMC_FREQ_HIGHSPEED) {
        host_div = 4;       // 160 MHz / 4 = 40 MHz
        card_div = 0;
    } else if (freq_khz == SDMMC_FREQ_DEFAULT) {
        host_div = 8;       // 160 MHz / 8 = 20 MHz
        card_div = 0;
    } else if (freq_khz == SDMMC_FREQ_PROBING) {
        host_div = 10;      // 160 MHz / 10 / (20 * 2) = 400 kHz
        card_div = 20;
    } else {
        host_div = 2;
        card_div = (clk40m + freq_khz * 2 - 1) / (freq_khz * 2); // round up
    }

    ESP_LOGD(TAG, "slot=%d host_div=%d card_div=%d freq=%dkHz",
            slot, host_div, card_div,
            2 * APB_CLK_FREQ / host_div / ((card_div == 0) ? 1 : card_div * 2) / 1000);

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
    sdmmc_host_clock_update_command(slot);

    // Re-enable clocks
    SDMMC.clkena.cclk_enable |= BIT(slot);
    SDMMC.clkena.cclk_low_power |= BIT(slot);
    sdmmc_host_clock_update_command(slot);

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

esp_err_t sdmmc_host_start_command(int slot, sdmmc_hw_cmd_t cmd, uint32_t arg) {
    if (!(slot == 0 || slot == 1)) {
        return ESP_ERR_INVALID_ARG;
    }
    if ((SDMMC.cdetect.cards & BIT(slot)) != 0) {
        return ESP_ERR_NOT_FOUND;
    }
    if (cmd.data_expected && cmd.rw && (SDMMC.wrtprt.cards & BIT(slot)) != 0) {
        return ESP_ERR_INVALID_STATE;
    }
    while (SDMMC.cmd.start_command == 1) {
        ;
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
    sdmmc_host_reset();
    ESP_LOGD(TAG, "peripheral version %x, hardware config %08x", SDMMC.verid, SDMMC.hcon);

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

static void configure_pin(int pin)
{
    const int sdmmc_func = 3;
    const int drive_strength = 3;
    assert(pin!=GPIO_NUM_NC);
    gpio_pulldown_dis(pin);

    uint32_t reg = GPIO_PIN_MUX_REG[pin];
    assert(reg != UINT32_MAX);
    PIN_INPUT_ENABLE(reg);
    PIN_FUNC_SELECT(reg, sdmmc_func);
    PIN_SET_DRV(reg, drive_strength);
}

esp_err_t sdmmc_host_init_slot(int slot, const sdmmc_slot_config_t* slot_config)
{
    bool pullup = slot_config->flags & SDMMC_SLOT_FLAG_INTERNAL_PULLUP;
    if (pullup) {
        sdmmc_host_pullup_en(slot, slot_config->width);
    }
    if (!s_intr_handle) {
        return ESP_ERR_INVALID_STATE;
    }
    if (!(slot == 0 || slot == 1)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (slot_config == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    int gpio_cd = slot_config->gpio_cd;
    int gpio_wp = slot_config->gpio_wp;
    uint8_t slot_width = slot_config->width;

    // Configure pins
    const sdmmc_slot_info_t* pslot = &sdmmc_slot_info[slot];

    if (slot_width == SDMMC_SLOT_WIDTH_DEFAULT) {
        slot_width = pslot->width;
    }
    else if (slot_width > pslot->width) {
        return ESP_ERR_INVALID_ARG;
    }
    s_slot_width[slot] = slot_width;

    configure_pin(pslot->clk_gpio);
    configure_pin(pslot->cmd_gpio);
    configure_pin(pslot->d0_gpio);

    if (slot_width >= 4) {
        configure_pin(pslot->d1_gpio);
        configure_pin(pslot->d2_gpio);
        // Force D3 high to make slave enter SD mode.
        // Connect to peripheral after width configuration.
        gpio_config_t gpio_conf = {
            .pin_bit_mask = BIT(pslot->d3_gpio),
            .mode = GPIO_MODE_OUTPUT ,
            .pull_up_en = 0,
            .pull_down_en = 0,
            .intr_type = GPIO_INTR_DISABLE,
        };
        gpio_config(&gpio_conf);
        gpio_set_level(pslot->d3_gpio, 1);
        if (slot_width == 8) {
            configure_pin(pslot->d4_gpio);
            configure_pin(pslot->d5_gpio);
            configure_pin(pslot->d6_gpio);
            configure_pin(pslot->d7_gpio);
        }
    }

    // SDIO slave interrupt is edge sensitive to ~(int_n | card_int | card_detect)
    // set this and card_detect to high to enable sdio interrupt
    gpio_matrix_in(GPIO_FUNC_IN_HIGH, pslot->card_int, false);

    // Set up Card Detect input
    int matrix_in_cd;
    if (gpio_cd != SDMMC_SLOT_NO_CD) {
        ESP_LOGD(TAG, "using GPIO%d as CD pin", gpio_cd);
        gpio_pad_select_gpio(gpio_cd);
        gpio_set_direction(gpio_cd, GPIO_MODE_INPUT);
        matrix_in_cd = gpio_cd;
    } else {
        // if not set, default to CD low (card present)
        matrix_in_cd = GPIO_FUNC_IN_LOW;
    }
    gpio_matrix_in(matrix_in_cd, pslot->card_detect, false);

    // Set up Write Protect input
    int matrix_in_wp;
    if (gpio_wp != SDMMC_SLOT_NO_WP) {
        ESP_LOGD(TAG, "using GPIO%d as WP pin", gpio_wp);
        gpio_pad_select_gpio(gpio_wp);
        gpio_set_direction(gpio_wp, GPIO_MODE_INPUT);
        matrix_in_wp = gpio_wp;
    } else {
        // if not set, default to WP high (not write protected)
        matrix_in_wp = GPIO_FUNC_IN_HIGH;
    }
    // WP signal is normally active low, but hardware expects
    // an active-high signal, so invert it in GPIO matrix
    gpio_matrix_in(matrix_in_wp, pslot->write_protect, true);

    // By default, set probing frequency (400kHz) and 1-bit bus
    esp_err_t ret = sdmmc_host_set_card_clk(slot, 400);
    if (ret != ESP_OK) {
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

esp_err_t sdmmc_host_wait_for_event(int tick_count, sdmmc_event_t* out_event)
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
        configure_pin(sdmmc_slot_info[slot].d3_gpio);
    } else if (width == 8) {
        SDMMC.ctype.card_width_8 |= mask;
        // D3 was set to GPIO high to force slave into SD mode, until 4-bit mode is set
        configure_pin(sdmmc_slot_info[slot].d3_gpio);
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

void sdmmc_host_dma_prepare(sdmmc_desc_t* desc, size_t block_size, size_t data_size)
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
    configure_pin(sdmmc_slot_info[slot].d1_gpio);
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
    if (gpio_get_level(sdmmc_slot_info[slot].d1_gpio) == 0) {
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
static void sdmmc_isr(void* arg) {
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
        // disable the interrupt (no need to clear here, this is done in sdmmc_host_io_wait_int)
        SDMMC.intmask.sdio &= ~sdio_pending;
        xSemaphoreGiveFromISR(s_io_intr_event, &higher_priority_task_awoken);
    }

    if (higher_priority_task_awoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

esp_err_t sdmmc_host_pullup_en(int slot, int width)
{
    if (width > sdmmc_slot_info[slot].width) {
        //in esp32 we only support 8 bit in slot 0, note this is occupied by the flash by default
        return ESP_ERR_INVALID_ARG;
    }
    //according to the spec, the host control the clk, we don't to pull it up here
    gpio_pullup_en(sdmmc_slot_info[slot].cmd_gpio);
    gpio_pullup_en(sdmmc_slot_info[slot].d0_gpio);
    if (width >= 4) {
        gpio_pullup_en(sdmmc_slot_info[slot].d1_gpio);
        gpio_pullup_en(sdmmc_slot_info[slot].d2_gpio);
        gpio_pullup_en(sdmmc_slot_info[slot].d3_gpio);
    }
    if (width == 8) {
        gpio_pullup_en(sdmmc_slot_info[slot].d4_gpio);
        gpio_pullup_en(sdmmc_slot_info[slot].d5_gpio);
        gpio_pullup_en(sdmmc_slot_info[slot].d6_gpio);
        gpio_pullup_en(sdmmc_slot_info[slot].d7_gpio);
    }
    return ESP_OK;
}

