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
#include "soc/sdmmc_struct.h"
#include "soc/sdmmc_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/gpio_sig_map.h"
#include "rom/gpio.h"
#include "driver/gpio.h"
#include "driver/sdmmc_host.h"
#include "sdmmc_private.h"

#define SDMMC_EVENT_QUEUE_LENGTH 32

typedef struct {
    uint32_t clk;
    uint32_t cmd;
    uint32_t d0;
    uint32_t d1;
    uint32_t d2;
    uint32_t d3;
    uint32_t d4;
    uint32_t d5;
    uint32_t d6;
    uint32_t d7;
    uint8_t card_detect;
    uint8_t write_protect;
    uint8_t width;
} sdmmc_slot_info_t;


static void sdmmc_isr(void* arg);
static void sdmmc_host_dma_init();

static const sdmmc_slot_info_t s_slot_info[2]  = {
    {
        .clk = PERIPHS_IO_MUX_SD_CLK_U,
        .cmd = PERIPHS_IO_MUX_SD_CMD_U,
        .d0 = PERIPHS_IO_MUX_SD_DATA0_U,
        .d1 = PERIPHS_IO_MUX_SD_DATA1_U,
        .d2 = PERIPHS_IO_MUX_SD_DATA2_U,
        .d3 = PERIPHS_IO_MUX_SD_DATA3_U,
        .d4 = PERIPHS_IO_MUX_GPIO16_U,
        .d5 = PERIPHS_IO_MUX_GPIO17_U,
        .d6 = PERIPHS_IO_MUX_GPIO5_U,
        .d7 = PERIPHS_IO_MUX_GPIO18_U,
        .card_detect = HOST_CARD_DETECT_N_1_IDX,
        .write_protect = HOST_CARD_WRITE_PRT_1_IDX,
        .width = 8
    },
    {
        .clk = PERIPHS_IO_MUX_MTMS_U,
        .cmd = PERIPHS_IO_MUX_MTDO_U,
        .d0 = PERIPHS_IO_MUX_GPIO2_U,
        .d1 = PERIPHS_IO_MUX_GPIO4_U,
        .d2 = PERIPHS_IO_MUX_MTDI_U,
        .d3 = PERIPHS_IO_MUX_MTCK_U,
        .card_detect = HOST_CARD_DETECT_N_2_IDX,
        .write_protect = HOST_CARD_WRITE_PRT_2_IDX,
        .width = 4
    }
};

static const char* TAG = "sdmmc_periph";
static intr_handle_t s_intr_handle;
static QueueHandle_t s_event_queue;


void sdmmc_host_reset()
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
 * For now set the first stage divider to generate 40MHz, and then configure
 * the second stage dividers to generate the frequency requested.
 *
 * Of the second stage dividers, div0 is used for card 0, and div1 is used
 * for card 1.
 */

static void sdmmc_host_input_clk_enable()
{
    // Set frequency to 160MHz / (p + 1) = 40MHz, duty cycle (h + 1)/(p + 1) = 1/2
    SDMMC.clock.div_factor_p = 3;
    SDMMC.clock.div_factor_h = 1;
    SDMMC.clock.div_factor_m = 3;
    // Set phases for in/out clocks
    SDMMC.clock.phase_dout = 4;
    SDMMC.clock.phase_din = 4;
    SDMMC.clock.phase_core = 0;
    // Wait for the clock to propagate
    ets_delay_us(10);
}

static void sdmmc_host_input_clk_disable()
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

    // Calculate new dividers
    int div = 0;
    if (freq_khz < clk40m) {
        // round up; extra *2 is because clock divider divides by 2*n
        div = (clk40m + freq_khz * 2 - 1) / (freq_khz * 2);
    }
    ESP_LOGD(TAG, "slot=%d div=%d freq=%dkHz", slot, div,
            (div == 0) ? clk40m : clk40m / (2 * div));

    // Program CLKDIV and CLKSRC, send them to the CIU
    switch(slot) {
        case 0:
            SDMMC.clksrc.card0 = 0;
            SDMMC.clkdiv.div0 = div;
            break;
        case 1:
            SDMMC.clksrc.card1 = 1;
            SDMMC.clkdiv.div1 = div;
            break;
    }
    sdmmc_host_clock_update_command(slot);

    // Re-enable clocks
    SDMMC.clkena.cclk_enable |= BIT(slot);
    SDMMC.clkena.cclk_low_power |= BIT(slot);
    sdmmc_host_clock_update_command(slot);
    return ESP_OK;
}

esp_err_t sdmmc_host_start_command(int slot, sdmmc_hw_cmd_t cmd, uint32_t arg) {
    if (!(slot == 0 || slot == 1)) {
        return ESP_ERR_INVALID_ARG;
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

esp_err_t sdmmc_host_init()
{
    if (s_intr_handle) {
        return ESP_ERR_INVALID_STATE;
    }

    // Enable clock to peripheral
    sdmmc_host_input_clk_enable();

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
    // Attach interrupt handler
    esp_err_t ret = esp_intr_alloc(ETS_SDIO_HOST_INTR_SOURCE, 0, &sdmmc_isr, s_event_queue, &s_intr_handle);
    if (ret != ESP_OK) {
        vQueueDelete(s_event_queue);
        s_event_queue = NULL;
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
            SDMMC_INTMASK_RESP_ERR | SDMMC_INTMASK_HLE;
    SDMMC.ctrl.int_enable = 1;

    // Enable DMA
    sdmmc_host_dma_init();

    // Initialize transaction handler
    ret = sdmmc_host_transaction_handler_init();
    if (ret != ESP_OK) {
        vQueueDelete(s_event_queue);
        s_event_queue = NULL;
        esp_intr_free(s_intr_handle);
        s_intr_handle = NULL;
        return ret;
    }

    return ESP_OK;
}


static inline void configure_pin(uint32_t io_mux_reg)
{
    const int sdmmc_func = 3;
    const int drive_strength = 3;
    PIN_INPUT_ENABLE(io_mux_reg);
    PIN_FUNC_SELECT(io_mux_reg, sdmmc_func);
    PIN_SET_DRV(io_mux_reg, drive_strength);
}

esp_err_t sdmmc_host_init_slot(int slot, const sdmmc_slot_config_t* slot_config)
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
    int gpio_cd = slot_config->gpio_cd;
    int gpio_wp = slot_config->gpio_wp;

    // Configure pins
    const sdmmc_slot_info_t* pslot = &s_slot_info[slot];
    configure_pin(pslot->clk);
    configure_pin(pslot->cmd);
    configure_pin(pslot->d0);
    configure_pin(pslot->d1);
    configure_pin(pslot->d2);
    configure_pin(pslot->d3);
    if (pslot->width == 8) {
        configure_pin(pslot->d4);
        configure_pin(pslot->d5);
        configure_pin(pslot->d6);
        configure_pin(pslot->d7);
    }
    if (gpio_cd != -1) {
        gpio_set_direction(gpio_cd, GPIO_MODE_INPUT);
        gpio_matrix_in(gpio_cd, pslot->card_detect, 0);
    }
    if (gpio_wp != -1) {
        gpio_set_direction(gpio_wp, GPIO_MODE_INPUT);
        gpio_matrix_in(gpio_wp, pslot->write_protect, 0);
    }
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

esp_err_t sdmmc_host_deinit()
{
    if (!s_intr_handle) {
        return ESP_ERR_INVALID_STATE;
    }
    esp_intr_free(s_intr_handle);
    s_intr_handle = NULL;
    vQueueDelete(s_event_queue);
    s_event_queue = NULL;
    sdmmc_host_input_clk_disable();
    sdmmc_host_transaction_handler_deinit();
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
    if (s_slot_info[slot].width < width) {
        return ESP_ERR_INVALID_ARG;
    }
    const uint16_t mask = BIT(slot);
    if (width == 1) {
        SDMMC.ctype.card_width_8 &= ~mask;
        SDMMC.ctype.card_width &= ~mask;
    } else if (width == 4) {
        SDMMC.ctype.card_width_8 &= ~mask;
        SDMMC.ctype.card_width |= mask;
    } else if (width == 8){
        SDMMC.ctype.card_width_8 |= mask;
    } else {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_LOGD(TAG, "slot=%d width=%d", slot, width);
    return ESP_OK;
}

static void sdmmc_host_dma_init()
{
    SDMMC.ctrl.dma_enable = 1;
    SDMMC.bmod.val = 0;
    SDMMC.bmod.sw_reset = 1;
    SDMMC.idinten.ni = 1;
    SDMMC.idinten.ri = 1;
    SDMMC.idinten.ti = 1;
}


void sdmmc_host_dma_stop()
{
    SDMMC.ctrl.use_internal_dma = 0;
    SDMMC.ctrl.dma_reset = 1;
    SDMMC.bmod.fb = 0;
    SDMMC.bmod.enable = 0;
}

void sdmmc_host_dma_prepare(sdmmc_desc_t* desc, size_t block_size, size_t data_size)
{
    // TODO: set timeout depending on data size
    SDMMC.tmout.val = 0xffffffff;

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

void sdmmc_host_dma_resume()
{
    SDMMC.pldmnd = 1;
}

/**
 * @brief SDMMC interrupt handler
 *
 * Ignoring SDIO and streaming read/writes for now (and considering just SD memory cards),
 * all communication is driven by the master, and the hardware handles things like stop
 * commands automatically. So the interrupt handler doesn't need to do much, we just push
 * interrupt status into a queue, clear interrupt flags, and let the task currently doing
 * communication figure out what to do next.
 *
 * Card detect interrupts pose a small issue though, because if a card is plugged in and
 * out a few times, while there is no task to process the events, event queue can become
 * full and some card detect events may be dropped. We ignore this problem for now, since
 * the there are no other interesting events which can get lost due to this.
 */
static void sdmmc_isr(void* arg) {
    QueueHandle_t queue = (QueueHandle_t) arg;
    sdmmc_event_t event;
    uint32_t pending = SDMMC.mintsts.val;
    SDMMC.rintsts.val = pending;
    event.sdmmc_status = pending;

    uint32_t dma_pending = SDMMC.idsts.val;
    SDMMC.idsts.val = dma_pending;
    event.dma_status = dma_pending & 0x1f;

    int higher_priority_task_awoken = pdFALSE;
    xQueueSendFromISR(queue, &event, &higher_priority_task_awoken);
    if (higher_priority_task_awoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

