/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>
#include <sys/param.h>
#include <sys/queue.h>
#include "sdkconfig.h"
#if CONFIG_LCD_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_attr.h"
#include "esp_check.h"
#include "esp_pm.h"
#include "esp_lcd_panel_io_interface.h"
#include "esp_lcd_panel_io.h"
#include "esp_rom_gpio.h"
#include "soc/soc_caps.h"
#include "esp_clk_tree.h"
#include "esp_memory_utils.h"
#include "hal/dma_types.h"
#include "hal/gpio_hal.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"
#include "esp_private/gdma.h"
#include "driver/gpio.h"
#include "esp_private/periph_ctrl.h"
#include "esp_lcd_common.h"
#include "soc/lcd_periph.h"
#include "hal/lcd_ll.h"
#include "hal/lcd_hal.h"
#include "esp_cache.h"

#define ALIGN_UP(size, align)    (((size) + (align) - 1) & ~((align) - 1))
#define ALIGN_DOWN(size, align)  ((size) & ~((align) - 1))

static const char *TAG = "lcd_panel.io.i80";

typedef struct esp_lcd_i80_bus_t esp_lcd_i80_bus_t;
typedef struct lcd_panel_io_i80_t lcd_panel_io_i80_t;
typedef struct lcd_i80_trans_descriptor_t lcd_i80_trans_descriptor_t;

static esp_err_t panel_io_i80_tx_param(esp_lcd_panel_io_t *io, int lcd_cmd, const void *param, size_t param_size);
static esp_err_t panel_io_i80_tx_color(esp_lcd_panel_io_t *io, int lcd_cmd, const void *color, size_t color_size);
static esp_err_t panel_io_i80_del(esp_lcd_panel_io_t *io);
static esp_err_t lcd_i80_init_dma_link(esp_lcd_i80_bus_handle_t bus);
static void lcd_periph_trigger_quick_trans_done_event(esp_lcd_i80_bus_handle_t bus);
static esp_err_t lcd_i80_select_periph_clock(esp_lcd_i80_bus_handle_t bus, lcd_clock_source_t clk_src);
static esp_err_t lcd_i80_bus_configure_gpio(esp_lcd_i80_bus_handle_t bus, const esp_lcd_i80_bus_config_t *bus_config);
static void lcd_i80_switch_devices(lcd_panel_io_i80_t *cur_device, lcd_panel_io_i80_t *next_device);
static void lcd_start_transaction(esp_lcd_i80_bus_t *bus, lcd_i80_trans_descriptor_t *trans_desc);
static void lcd_default_isr_handler(void *args);
static esp_err_t panel_io_i80_register_event_callbacks(esp_lcd_panel_io_handle_t io, const esp_lcd_panel_io_callbacks_t *cbs, void *user_ctx);

struct esp_lcd_i80_bus_t {
    int bus_id;            // Bus ID, index from 0
    portMUX_TYPE spinlock; // spinlock used to protect i80 bus members(hal, device_list, cur_trans)
    lcd_hal_context_t hal; // Hal object
    size_t bus_width;      // Number of data lines
    intr_handle_t intr;    // LCD peripheral interrupt handle
    esp_pm_lock_handle_t pm_lock; // Power management lock
    size_t num_dma_nodes;  // Number of DMA descriptors
    uint8_t *format_buffer;  // The driver allocates an internal buffer for DMA to do data format transformer
    size_t resolution_hz;    // LCD_CLK resolution, determined by selected clock source
    gdma_channel_handle_t dma_chan; // DMA channel handle
    size_t psram_trans_align; // DMA transfer alignment for data allocated from PSRAM
    size_t sram_trans_align;  // DMA transfer alignment for data allocated from SRAM
    lcd_i80_trans_descriptor_t *cur_trans; // Current transaction
    lcd_panel_io_i80_t *cur_device; // Current working device
    LIST_HEAD(i80_device_list, lcd_panel_io_i80_t) device_list; // Head of i80 device list
    struct {
        unsigned int exclusive: 1; // Indicate whether the I80 bus is owned by one device (whose CS GPIO is not assigned) exclusively
    } flags;
    dma_descriptor_t dma_nodes[]; // DMA descriptor pool, the descriptors are shared by all i80 devices
};

struct lcd_i80_trans_descriptor_t {
    lcd_panel_io_i80_t *i80_device; // i80 device issuing this transaction
    int cmd_value;        // Command value
    uint32_t cmd_cycles;  // Command cycles
    const void *data;     // Data buffer
    uint32_t data_length; // Data buffer size
    void *user_ctx;   // private data used by trans_done_cb
    esp_lcd_panel_io_color_trans_done_cb_t trans_done_cb; // transaction done callback
};

struct lcd_panel_io_i80_t {
    esp_lcd_panel_io_t base;   // Base class of generic lcd panel io
    esp_lcd_i80_bus_t *bus;    // Which bus the device is attached to
    int cs_gpio_num;           // GPIO used for CS line
    unsigned int pclk_hz;      // PCLK clock frequency
    size_t clock_prescale;     // Prescaler coefficient, determined by user's configured PCLK frequency
    QueueHandle_t trans_queue; // Transaction queue, transactions in this queue are pending for scheduler to dispatch
    QueueHandle_t done_queue;  // Transaction done queue, transactions in this queue are finished but not recycled by the caller
    size_t queue_size;         // Size of transaction queue
    size_t num_trans_inflight; // Number of transactions that are undergoing (the descriptor not recycled yet)
    int lcd_cmd_bits;          // Bit width of LCD command
    int lcd_param_bits;        // Bit width of LCD parameter
    void *user_ctx;            // private data used when transfer color data
    esp_lcd_panel_io_color_trans_done_cb_t on_color_trans_done; // color data trans done callback
    LIST_ENTRY(lcd_panel_io_i80_t) device_list_entry; // Entry of i80 device list
    struct {
        unsigned int dc_idle_level: 1;  // Level of DC line in IDLE phase
        unsigned int dc_cmd_level: 1;   // Level of DC line in CMD phase
        unsigned int dc_dummy_level: 1; // Level of DC line in DUMMY phase
        unsigned int dc_data_level: 1;  // Level of DC line in DATA phase
    } dc_levels;
    struct {
        unsigned int cs_active_high: 1;     // Whether the CS line is active on high level
        unsigned int reverse_color_bits: 1; // Reverse the data bits, D[N:0] -> D[0:N]
        unsigned int swap_color_bytes: 1;   // Swap adjacent two data bytes before sending out
        unsigned int pclk_active_neg: 1;    // The display will write data lines when there's a falling edge on WR line
        unsigned int pclk_idle_low: 1;      // The WR line keeps at low level in IDLE phase
    } flags;
    lcd_i80_trans_descriptor_t trans_pool[]; // Transaction pool
};

esp_err_t esp_lcd_new_i80_bus(const esp_lcd_i80_bus_config_t *bus_config, esp_lcd_i80_bus_handle_t *ret_bus)
{
#if CONFIG_LCD_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    esp_lcd_i80_bus_t *bus = NULL;
    ESP_RETURN_ON_FALSE(bus_config && ret_bus, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    // although LCD_CAM can support up to 24 data lines, we restrict users to only use 8 or 16 bit width
    ESP_RETURN_ON_FALSE(bus_config->bus_width == 8 || bus_config->bus_width == 16, ESP_ERR_INVALID_ARG,
                        TAG, "invalid bus width:%d", bus_config->bus_width);
    size_t num_dma_nodes = bus_config->max_transfer_bytes / DMA_DESCRIPTOR_BUFFER_MAX_SIZE + 1;
    // DMA descriptors must be placed in internal SRAM
    bus = heap_caps_calloc(1, sizeof(esp_lcd_i80_bus_t) + num_dma_nodes * sizeof(dma_descriptor_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
    ESP_GOTO_ON_FALSE(bus, ESP_ERR_NO_MEM, err, TAG, "no mem for i80 bus");
    bus->num_dma_nodes = num_dma_nodes;
    bus->bus_id = -1;
    bus->format_buffer = heap_caps_calloc(1, CONFIG_LCD_PANEL_IO_FORMAT_BUF_SIZE, MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
    ESP_GOTO_ON_FALSE(bus->format_buffer, ESP_ERR_NO_MEM, err, TAG, "no mem for format buffer");
    // register to platform
    int bus_id = lcd_com_register_device(LCD_COM_DEVICE_TYPE_I80, bus);
    ESP_GOTO_ON_FALSE(bus_id >= 0, ESP_ERR_NOT_FOUND, err, TAG, "no free i80 bus slot");
    bus->bus_id = bus_id;
    // enable APB to access LCD registers
    PERIPH_RCC_ACQUIRE_ATOMIC(lcd_periph_signals.panels[bus_id].module, ref_count) {
        if (ref_count == 0) {
            lcd_ll_enable_bus_clock(bus_id, true);
            lcd_ll_reset_register(bus_id);
        }
    }
    // initialize HAL layer, so we can call LL APIs later
    lcd_hal_init(&bus->hal, bus_id);
    LCD_CLOCK_SRC_ATOMIC() {
        lcd_ll_enable_clock(bus->hal.dev, true);
    }
    // set peripheral clock resolution
    ret = lcd_i80_select_periph_clock(bus, bus_config->clk_src);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "select periph clock %d failed", bus_config->clk_src);
    // reset peripheral and FIFO after we select a correct clock source
    lcd_ll_reset(bus->hal.dev);
    lcd_ll_fifo_reset(bus->hal.dev);
    // install interrupt service, (LCD peripheral shares the same interrupt source with Camera peripheral with different mask)
    // interrupt is disabled by default
    int isr_flags = LCD_I80_INTR_ALLOC_FLAGS | ESP_INTR_FLAG_SHARED | ESP_INTR_FLAG_LOWMED;
    ret = esp_intr_alloc_intrstatus(lcd_periph_signals.buses[bus_id].irq_id, isr_flags,
                                    (uint32_t)lcd_ll_get_interrupt_status_reg(bus->hal.dev),
                                    LCD_LL_EVENT_TRANS_DONE, lcd_default_isr_handler, bus, &bus->intr);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "install interrupt failed");
    lcd_ll_enable_interrupt(bus->hal.dev, LCD_LL_EVENT_TRANS_DONE, false); // disable all interrupts
    lcd_ll_clear_interrupt_status(bus->hal.dev, UINT32_MAX); // clear pending interrupt
    // install DMA service
    bus->psram_trans_align = bus_config->psram_trans_align;
    bus->sram_trans_align = bus_config->sram_trans_align;
    bus->bus_width = bus_config->bus_width;
    ret = lcd_i80_init_dma_link(bus);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "install DMA failed");
    // disable RGB-LCD mode
    lcd_ll_enable_rgb_mode(bus->hal.dev, false);
    // disable YUV-RGB converter
    lcd_ll_enable_rgb_yuv_convert(bus->hal.dev, false);
    // set how much data to read from DMA each time
    lcd_ll_set_dma_read_stride(bus->hal.dev, bus->bus_width);
    // sometime, we need to change the output data order: ABAB->BABA
    lcd_ll_set_swizzle_mode(bus->hal.dev, LCD_LL_SWIZZLE_AB2BA);
    // number of data cycles is controlled by DMA buffer size
    lcd_ll_enable_output_always_on(bus->hal.dev, true);
    // enable trans done interrupt
    lcd_ll_enable_interrupt(bus->hal.dev, LCD_LL_EVENT_TRANS_DONE, true);
    // trigger a quick "trans done" event, and wait for the interrupt line goes active
    // this could ensure we go into ISR handler next time we call `esp_intr_enable`
    lcd_periph_trigger_quick_trans_done_event(bus);
    // configure GPIO
    ret = lcd_i80_bus_configure_gpio(bus, bus_config);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "configure GPIO failed");
    // fill other i80 bus runtime parameters
    LIST_INIT(&bus->device_list); // initialize device list head
    bus->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    *ret_bus = bus;
    ESP_LOGD(TAG, "new i80 bus(%d) @%p, %zu dma nodes", bus_id, bus, bus->num_dma_nodes);
    return ESP_OK;

err:
    if (bus) {
        if (bus->intr) {
            esp_intr_free(bus->intr);
        }
        if (bus->dma_chan) {
            gdma_disconnect(bus->dma_chan);
            gdma_del_channel(bus->dma_chan);
        }
        if (bus->bus_id >= 0) {
            PERIPH_RCC_RELEASE_ATOMIC(lcd_periph_signals.panels[bus->bus_id].module, ref_count) {
                if (ref_count == 0) {
                    lcd_ll_enable_bus_clock(bus->bus_id, false);
                }
            }
            lcd_com_remove_device(LCD_COM_DEVICE_TYPE_I80, bus->bus_id);
        }
        if (bus->format_buffer) {
            free(bus->format_buffer);
        }
        if (bus->pm_lock) {
            esp_pm_lock_delete(bus->pm_lock);
        }
        free(bus);
    }
    return ret;
}

esp_err_t esp_lcd_del_i80_bus(esp_lcd_i80_bus_handle_t bus)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(bus, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(LIST_EMPTY(&bus->device_list), ESP_ERR_INVALID_STATE, err, TAG, "device list not empty");
    int bus_id = bus->bus_id;
    lcd_com_remove_device(LCD_COM_DEVICE_TYPE_I80, bus_id);
    PERIPH_RCC_RELEASE_ATOMIC(lcd_periph_signals.panels[bus_id].module, ref_count) {
        if (ref_count == 0) {
            lcd_ll_enable_bus_clock(bus_id, false);
        }
    }
    gdma_disconnect(bus->dma_chan);
    gdma_del_channel(bus->dma_chan);
    esp_intr_free(bus->intr);
    free(bus->format_buffer);
    if (bus->pm_lock) {
        esp_pm_lock_delete(bus->pm_lock);
    }
    free(bus);
    ESP_LOGD(TAG, "del i80 bus(%d)", bus_id);
err:
    return ret;
}

esp_err_t esp_lcd_new_panel_io_i80(esp_lcd_i80_bus_handle_t bus, const esp_lcd_panel_io_i80_config_t *io_config, esp_lcd_panel_io_handle_t *ret_io)
{
    esp_err_t ret = ESP_OK;
    lcd_panel_io_i80_t *i80_device = NULL;
    bool bus_exclusive = false;
    ESP_GOTO_ON_FALSE(bus && io_config && ret_io, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    // check if the bus has been configured as exclusive
    portENTER_CRITICAL(&bus->spinlock);
    if (!bus->flags.exclusive) {
        bus->flags.exclusive = io_config->cs_gpio_num < 0;
    } else {
        bus_exclusive = true;
    }
    portEXIT_CRITICAL(&bus->spinlock);
    ESP_GOTO_ON_FALSE(!bus_exclusive, ESP_ERR_INVALID_STATE, err, TAG, "bus has been exclusively owned by device");
    // check if pixel clock setting is valid
    uint32_t pclk_prescale = bus->resolution_hz / io_config->pclk_hz;
    ESP_GOTO_ON_FALSE(pclk_prescale > 0 && pclk_prescale <= LCD_LL_PCLK_DIV_MAX, ESP_ERR_NOT_SUPPORTED, err, TAG,
                      "prescaler can't satisfy PCLK clock %"PRIu32"Hz", io_config->pclk_hz);
    i80_device = heap_caps_calloc(1, sizeof(lcd_panel_io_i80_t) + io_config->trans_queue_depth * sizeof(lcd_i80_trans_descriptor_t), LCD_I80_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(i80_device, ESP_ERR_NO_MEM, err, TAG, "no mem for i80 panel io");
    // create two queues for i80 device
    i80_device->trans_queue = xQueueCreate(io_config->trans_queue_depth, sizeof(lcd_i80_trans_descriptor_t *));
    ESP_GOTO_ON_FALSE(i80_device->trans_queue, ESP_ERR_NO_MEM, err, TAG, "create trans queue failed");
    i80_device->done_queue = xQueueCreate(io_config->trans_queue_depth, sizeof(lcd_i80_trans_descriptor_t *));
    ESP_GOTO_ON_FALSE(i80_device->done_queue, ESP_ERR_NO_MEM, err, TAG, "create done queue failed");
    // adding device to list
    portENTER_CRITICAL(&bus->spinlock);
    LIST_INSERT_HEAD(&bus->device_list, i80_device, device_list_entry);
    portEXIT_CRITICAL(&bus->spinlock);
    // we don't initialize the i80 bus at the memont, but initialize the bus when start a transaction for a new device
    // so save these as i80 device runtime parameters
    i80_device->bus = bus;
    i80_device->lcd_cmd_bits = io_config->lcd_cmd_bits;
    i80_device->lcd_param_bits = io_config->lcd_param_bits;
    i80_device->queue_size = io_config->trans_queue_depth;
    i80_device->clock_prescale = pclk_prescale;
    i80_device->pclk_hz = bus->resolution_hz / pclk_prescale;
    i80_device->dc_levels.dc_cmd_level = io_config->dc_levels.dc_cmd_level;
    i80_device->dc_levels.dc_data_level = io_config->dc_levels.dc_data_level;
    i80_device->dc_levels.dc_dummy_level = io_config->dc_levels.dc_dummy_level;
    i80_device->dc_levels.dc_idle_level = io_config->dc_levels.dc_idle_level;
    i80_device->cs_gpio_num = io_config->cs_gpio_num;
    i80_device->flags.reverse_color_bits = io_config->flags.reverse_color_bits;
    i80_device->flags.swap_color_bytes = io_config->flags.swap_color_bytes;
    i80_device->flags.cs_active_high = io_config->flags.cs_active_high;
    i80_device->flags.pclk_idle_low = io_config->flags.pclk_idle_low;
    i80_device->flags.pclk_active_neg = io_config->flags.pclk_active_neg;
    i80_device->on_color_trans_done = io_config->on_color_trans_done;
    i80_device->user_ctx = io_config->user_ctx;
    // fill panel io function table
    i80_device->base.del = panel_io_i80_del;
    i80_device->base.tx_param = panel_io_i80_tx_param;
    i80_device->base.tx_color = panel_io_i80_tx_color;
    i80_device->base.register_event_callbacks = panel_io_i80_register_event_callbacks;
    // we only configure the CS GPIO as output, don't connect to the peripheral signal at the moment
    // we will connect the CS GPIO to peripheral signal when switching devices in lcd_i80_switch_devices()
    if (io_config->cs_gpio_num >= 0) {
        gpio_set_level(io_config->cs_gpio_num, !io_config->flags.cs_active_high);
        gpio_set_direction(io_config->cs_gpio_num, GPIO_MODE_OUTPUT);
        gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[io_config->cs_gpio_num], PIN_FUNC_GPIO);
    }
    *ret_io = &(i80_device->base);
    ESP_LOGD(TAG, "new i80 lcd panel io @%p on bus(%d)", i80_device, bus->bus_id);
    return ESP_OK;

err:
    if (i80_device) {
        if (i80_device->trans_queue) {
            vQueueDelete(i80_device->trans_queue);
        }
        if (i80_device->done_queue) {
            vQueueDelete(i80_device->done_queue);
        }
        free(i80_device);
    }
    return ret;
}

static esp_err_t panel_io_i80_del(esp_lcd_panel_io_t *io)
{
    lcd_panel_io_i80_t *i80_device = __containerof(io, lcd_panel_io_i80_t, base);
    esp_lcd_i80_bus_t *bus = i80_device->bus;
    lcd_i80_trans_descriptor_t *trans_desc = NULL;
    // wait all pending transaction to finish
    size_t num_trans_inflight = i80_device->num_trans_inflight;
    for (size_t i = 0; i < num_trans_inflight; i++) {
        ESP_RETURN_ON_FALSE(xQueueReceive(i80_device->done_queue, &trans_desc, portMAX_DELAY) == pdTRUE,
                            ESP_FAIL, TAG, "recycle inflight transactions failed");
        i80_device->num_trans_inflight--;
    }
    // remove from device list
    portENTER_CRITICAL(&bus->spinlock);
    LIST_REMOVE(i80_device, device_list_entry);
    portEXIT_CRITICAL(&bus->spinlock);

    // reset CS to normal GPIO
    if (i80_device->cs_gpio_num >= 0) {
        gpio_reset_pin(i80_device->cs_gpio_num);
    }

    ESP_LOGD(TAG, "del i80 lcd panel io @%p", i80_device);
    vQueueDelete(i80_device->trans_queue);
    vQueueDelete(i80_device->done_queue);
    free(i80_device);
    return ESP_OK;
}

static esp_err_t panel_io_i80_register_event_callbacks(esp_lcd_panel_io_handle_t io, const esp_lcd_panel_io_callbacks_t *cbs, void *user_ctx)
{
    lcd_panel_io_i80_t *i80_device = __containerof(io, lcd_panel_io_i80_t, base);

    if (i80_device->on_color_trans_done != NULL) {
        ESP_LOGW(TAG, "Callback on_color_trans_done was already set and now it was overwritten!");
    }

    i80_device->on_color_trans_done = cbs->on_color_trans_done;
    i80_device->user_ctx = user_ctx;

    return ESP_OK;
}

static void i80_lcd_prepare_cmd_buffer(esp_lcd_i80_bus_t *bus, lcd_panel_io_i80_t *i80_device, void *lcd_cmd)
{
    uint8_t *from = (uint8_t *)lcd_cmd;
    if (bus->bus_width < i80_device->lcd_cmd_bits) {
        // LCD is big-endian, e.g. to send command 0x1234, byte 0x12 should appear on the bus first
        // However, the i80 peripheral will send 0x34 first, so we reversed the order below
        int start = 0;
        int end = i80_device->lcd_cmd_bits / 8 - 1;
        lcd_com_reverse_buffer_bytes(from, start, end);
    }
}

static uint32_t i80_lcd_prepare_param_buffer(esp_lcd_i80_bus_t *bus, lcd_panel_io_i80_t *i80_device, const void *lcd_param, size_t param_size)
{
    int param_per_size = i80_device->lcd_param_bits / 8;
    int param_num = param_size / param_per_size;
    const uint8_t *from = (const uint8_t *)lcd_param;
    uint8_t *to = bus->format_buffer;
    uint8_t step = bus->bus_width / 8;
    int param_cycle = i80_device->lcd_param_bits / bus->bus_width;
    // in case bus_width=16 and param_bits=8, we still need 1 param_cycle
    if (param_cycle * bus->bus_width < i80_device->lcd_param_bits) {
        param_cycle++;
    }
    int ele_cycles = param_cycle * param_num;
    int bytes_to_copy = MIN(bus->bus_width, i80_device->lcd_param_bits) / 8;
    int cnt_from = 0;
    // expand the width of parameters when necessary
    for (int i = 0; i < ele_cycles; i++) {
        for (int j = 0; j < bytes_to_copy; j++) {
            to[j] = from[cnt_from++];
        }
        to += step;
    }
    return to - bus->format_buffer;
}

static esp_err_t panel_io_i80_tx_param(esp_lcd_panel_io_t *io, int lcd_cmd, const void *param, size_t param_size)
{
    lcd_panel_io_i80_t *next_device = __containerof(io, lcd_panel_io_i80_t, base);
    esp_lcd_i80_bus_t *bus = next_device->bus;
    lcd_panel_io_i80_t *cur_device = bus->cur_device;
    lcd_i80_trans_descriptor_t *trans_desc = NULL;
    assert(param_size <= (bus->num_dma_nodes * DMA_DESCRIPTOR_BUFFER_MAX_SIZE) && "parameter bytes too long, enlarge max_transfer_bytes");
    assert(param_size <= CONFIG_LCD_PANEL_IO_FORMAT_BUF_SIZE && "format buffer too small, increase CONFIG_LCD_PANEL_IO_FORMAT_BUF_SIZE");
    uint32_t cmd_cycles = next_device->lcd_cmd_bits / bus->bus_width;
    // in case bus_width=16 and cmd_bits=8, we still need 1 cmd_cycle
    if (cmd_cycles * bus->bus_width < next_device->lcd_cmd_bits) {
        cmd_cycles++;
    }
    i80_lcd_prepare_cmd_buffer(bus, next_device, &lcd_cmd);
    uint32_t param_len = i80_lcd_prepare_param_buffer(bus, next_device, param, param_size);
    // wait all pending transaction in the queue to finish
    size_t num_trans_inflight = next_device->num_trans_inflight;
    for (size_t i = 0; i < num_trans_inflight; i++) {
        ESP_RETURN_ON_FALSE(xQueueReceive(next_device->done_queue, &trans_desc, portMAX_DELAY) == pdTRUE,
                            ESP_FAIL, TAG, "recycle inflight transactions failed");
        next_device->num_trans_inflight--;
    }

    uint32_t intr_status = lcd_ll_get_interrupt_status(bus->hal.dev);
    lcd_ll_clear_interrupt_status(bus->hal.dev, intr_status);
    // switch devices if necessary
    lcd_i80_switch_devices(cur_device, next_device);
    // set data format
    lcd_ll_reverse_dma_data_bit_order(bus->hal.dev, false);
    // whether to swap the adjacent data bytes
    lcd_ll_enable_swizzle(bus->hal.dev, next_device->lcd_param_bits > bus->bus_width);
    bus->cur_trans = NULL;
    bus->cur_device = next_device;
    // package a transaction
    trans_desc = &next_device->trans_pool[0];
    trans_desc->i80_device = next_device;
    trans_desc->cmd_cycles = cmd_cycles;
    trans_desc->cmd_value = lcd_cmd;
    // either the param is NULL or the param_size is zero, means there isn't a data phase in this transaction
    trans_desc->data = (param && param_len) ? bus->format_buffer : NULL;
    trans_desc->data_length = trans_desc->data ? param_len : 0;
    trans_desc->trans_done_cb = NULL; // no callback for parameter transaction
    // mount data to DMA links
    lcd_com_mount_dma_data(bus->dma_nodes, trans_desc->data, trans_desc->data_length);
    // increase the pm lock reference count before starting a new transaction
    if (bus->pm_lock) {
        esp_pm_lock_acquire(bus->pm_lock);
    }
    lcd_start_transaction(bus, trans_desc);
    // polling the trans done event, but don't clear the event status
    while (!(lcd_ll_get_interrupt_status(bus->hal.dev) & LCD_LL_EVENT_TRANS_DONE)) {}
    // decrease pm lock reference count
    if (bus->pm_lock) {
        esp_pm_lock_release(bus->pm_lock);
    }
    return ESP_OK;
}

static esp_err_t panel_io_i80_tx_color(esp_lcd_panel_io_t *io, int lcd_cmd, const void *color, size_t color_size)
{
    lcd_panel_io_i80_t *i80_device = __containerof(io, lcd_panel_io_i80_t, base);
    esp_lcd_i80_bus_t *bus = i80_device->bus;
    lcd_i80_trans_descriptor_t *trans_desc = NULL;
    assert(color_size <= (bus->num_dma_nodes * DMA_DESCRIPTOR_BUFFER_MAX_SIZE) && "color bytes too long, enlarge max_transfer_bytes");
    // in case bus_width=16 and cmd_bits=8, we still need 1 cmd_cycle
    uint32_t cmd_cycles = i80_device->lcd_cmd_bits / bus->bus_width;
    if (cmd_cycles * bus->bus_width < i80_device->lcd_cmd_bits) {
        cmd_cycles++;
    }
    i80_lcd_prepare_cmd_buffer(bus, i80_device, &lcd_cmd);
    if (i80_device->num_trans_inflight < i80_device->queue_size) {
        trans_desc = &i80_device->trans_pool[i80_device->num_trans_inflight];
    } else {
        // transaction pool has used up, recycle one from done_queue
        ESP_RETURN_ON_FALSE(xQueueReceive(i80_device->done_queue, &trans_desc, portMAX_DELAY) == pdTRUE,
                            ESP_FAIL, TAG, "recycle inflight transactions failed");
        i80_device->num_trans_inflight--;
    }
    trans_desc->i80_device = i80_device;
    trans_desc->cmd_cycles = cmd_cycles;
    trans_desc->cmd_value = lcd_cmd;
    trans_desc->data = color;
    trans_desc->data_length = color_size;
    trans_desc->trans_done_cb = i80_device->on_color_trans_done;
    trans_desc->user_ctx = i80_device->user_ctx;

    if (esp_ptr_external_ram(color)) {
        uint32_t dcache_line_size = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_DATA);
        // flush frame buffer from cache to the physical PSRAM
        // note the esp_cache_msync function will check the alignment of the address and size, make sure they're aligned to current cache line size
        esp_cache_msync((void *)ALIGN_DOWN((intptr_t)color, dcache_line_size), ALIGN_UP(color_size, dcache_line_size), 0);
    }

    // send transaction to trans_queue
    xQueueSend(i80_device->trans_queue, &trans_desc, portMAX_DELAY);
    i80_device->num_trans_inflight++;
    // enable interrupt and go into isr handler, where we fetch the transactions from trans_queue and start it
    // we will go into `lcd_default_isr_handler` almost at once, because the "trans done" event is active at the moment
    esp_intr_enable(bus->intr);
    return ESP_OK;
}

static esp_err_t lcd_i80_select_periph_clock(esp_lcd_i80_bus_handle_t bus, lcd_clock_source_t clk_src)
{
    // get clock source frequency
    uint32_t src_clk_hz = 0;
    ESP_RETURN_ON_ERROR(esp_clk_tree_src_get_freq_hz((soc_module_clk_t)clk_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &src_clk_hz),
                        TAG, "get clock source frequency failed");

    LCD_CLOCK_SRC_ATOMIC() {
        lcd_ll_select_clk_src(bus->hal.dev, clk_src);
        // force to use integer division, as fractional division might lead to clock jitter
        lcd_ll_set_group_clock_coeff(bus->hal.dev, LCD_PERIPH_CLOCK_PRE_SCALE, 0, 0);
    }

    // save the resolution of the i80 bus
    bus->resolution_hz = src_clk_hz / LCD_PERIPH_CLOCK_PRE_SCALE;
    // create pm lock based on different clock source
    // clock sources like PLL and XTAL will be turned off in light sleep
#if CONFIG_PM_ENABLE
    ESP_RETURN_ON_ERROR(esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "i80_bus_lcd", &bus->pm_lock), TAG, "create pm lock failed");
#endif
    return ESP_OK;
}

static esp_err_t lcd_i80_init_dma_link(esp_lcd_i80_bus_handle_t bus)
{
    esp_err_t ret = ESP_OK;
    // chain DMA descriptors
    for (int i = 0; i < bus->num_dma_nodes; i++) {
        bus->dma_nodes[i].dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_CPU;
        bus->dma_nodes[i].next = &bus->dma_nodes[i + 1];
    }
    bus->dma_nodes[bus->num_dma_nodes - 1].next = NULL; // one-off DMA chain
    // alloc DMA channel and connect to LCD peripheral
    gdma_channel_alloc_config_t dma_chan_config = {
        .direction = GDMA_CHANNEL_DIRECTION_TX,
    };
#if SOC_GDMA_TRIG_PERIPH_LCD0_BUS == SOC_GDMA_BUS_AHB
    ret = gdma_new_ahb_channel(&dma_chan_config, &bus->dma_chan);
#elif SOC_GDMA_TRIG_PERIPH_LCD0_BUS == SOC_GDMA_BUS_AXI
    ret = gdma_new_axi_channel(&dma_chan_config, &bus->dma_chan);
#endif
    ESP_GOTO_ON_ERROR(ret, err, TAG, "alloc DMA channel failed");
    gdma_connect(bus->dma_chan, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_LCD, 0));
    gdma_strategy_config_t strategy_config = {
        .auto_update_desc = true,
        .owner_check = true
    };
    gdma_apply_strategy(bus->dma_chan, &strategy_config);
    // set DMA transfer ability
    gdma_transfer_ability_t ability = {
        .psram_trans_align = bus->psram_trans_align,
        .sram_trans_align = bus->sram_trans_align,
    };
    gdma_set_transfer_ability(bus->dma_chan, &ability);
    return ESP_OK;
err:
    if (bus->dma_chan) {
        gdma_del_channel(bus->dma_chan);
    }
    return ret;
}

static esp_err_t lcd_i80_bus_configure_gpio(esp_lcd_i80_bus_handle_t bus, const esp_lcd_i80_bus_config_t *bus_config)
{
    int bus_id = bus->bus_id;
    // check validation of GPIO number
    bool valid_gpio = (bus_config->wr_gpio_num >= 0) && (bus_config->dc_gpio_num >= 0);
    for (size_t i = 0; i < bus_config->bus_width; i++) {
        valid_gpio = valid_gpio && (bus_config->data_gpio_nums[i] >= 0);
    }
    if (!valid_gpio) {
        return ESP_ERR_INVALID_ARG;
    }
    // Set the number of output data lines
    lcd_ll_set_data_wire_width(bus->hal.dev, bus_config->bus_width);
    // connect peripheral signals via GPIO matrix
    for (size_t i = 0; i < bus_config->bus_width; i++) {
        gpio_set_direction(bus_config->data_gpio_nums[i], GPIO_MODE_OUTPUT);
        esp_rom_gpio_connect_out_signal(bus_config->data_gpio_nums[i], lcd_periph_signals.buses[bus_id].data_sigs[i], false, false);
        gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[bus_config->data_gpio_nums[i]], PIN_FUNC_GPIO);
    }
    gpio_set_direction(bus_config->dc_gpio_num, GPIO_MODE_OUTPUT);
    esp_rom_gpio_connect_out_signal(bus_config->dc_gpio_num, lcd_periph_signals.buses[bus_id].dc_sig, false, false);
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[bus_config->dc_gpio_num], PIN_FUNC_GPIO);
    gpio_set_direction(bus_config->wr_gpio_num, GPIO_MODE_OUTPUT);
    esp_rom_gpio_connect_out_signal(bus_config->wr_gpio_num, lcd_periph_signals.buses[bus_id].wr_sig, false, false);
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[bus_config->wr_gpio_num], PIN_FUNC_GPIO);
    return ESP_OK;
}

static void lcd_periph_trigger_quick_trans_done_event(esp_lcd_i80_bus_handle_t bus)
{
    // trigger a quick interrupt event by a dummy transaction, wait the LCD interrupt line goes active
    // next time when esp_intr_enable is invoked, we can go into interrupt handler immediately
    // where we dispatch transactions for i80 devices
    lcd_ll_set_phase_cycles(bus->hal.dev, 0, 1, 0);
    lcd_ll_start(bus->hal.dev);
    while (!(lcd_ll_get_interrupt_status(bus->hal.dev) & LCD_LL_EVENT_TRANS_DONE)) {}
}

static void lcd_start_transaction(esp_lcd_i80_bus_t *bus, lcd_i80_trans_descriptor_t *trans_desc)
{
    // by default, the dummy phase is disabled because it's not common for most LCDs
    uint32_t dummy_cycles = 0;
    uint32_t cmd_cycles = trans_desc->cmd_value >= 0 ? trans_desc->cmd_cycles : 0;
    // Number of data phase cycles are controlled by DMA buffer length, we only need to enable/disable the phase here
    uint32_t data_cycles = trans_desc->data ? 1 : 0;
    if (trans_desc->cmd_value >= 0) {
        lcd_ll_set_command(bus->hal.dev, bus->bus_width, trans_desc->cmd_value);
    }
    lcd_ll_set_phase_cycles(bus->hal.dev, cmd_cycles, dummy_cycles, data_cycles);
    lcd_ll_set_blank_cycles(bus->hal.dev, 1, 1);

    if (trans_desc->data) { // some specific LCD commands can have no parameters
        gdma_start(bus->dma_chan, (intptr_t)(bus->dma_nodes));
        // delay 1us is sufficient for DMA to pass data to LCD FIFO
        // in fact, this is only needed when LCD pixel clock is set too high
        esp_rom_delay_us(1);
    }
    lcd_ll_start(bus->hal.dev);
}

static void lcd_i80_switch_devices(lcd_panel_io_i80_t *cur_device, lcd_panel_io_i80_t *next_device)
{
    // we assume the next_device and cur_device are attached to the same bus
    esp_lcd_i80_bus_t *bus = next_device->bus;
    if (next_device != cur_device) {
        // reconfigure PCLK for the new device
        lcd_ll_set_pixel_clock_prescale(bus->hal.dev, next_device->clock_prescale);
        lcd_ll_set_clock_idle_level(bus->hal.dev, !next_device->flags.pclk_idle_low);
        lcd_ll_set_pixel_clock_edge(bus->hal.dev, next_device->flags.pclk_active_neg);
        // configure DC line level for the new device
        lcd_ll_set_dc_level(bus->hal.dev, next_device->dc_levels.dc_idle_level, next_device->dc_levels.dc_cmd_level,
                            next_device->dc_levels.dc_dummy_level, next_device->dc_levels.dc_data_level);
        if (cur_device && cur_device->cs_gpio_num >= 0) {
            // disconnect current CS GPIO from peripheral signal
            esp_rom_gpio_connect_out_signal(cur_device->cs_gpio_num, SIG_GPIO_OUT_IDX, false, false);
        }
        if (next_device->cs_gpio_num >= 0) {
            // connect CS signal to the new device
            esp_rom_gpio_connect_out_signal(next_device->cs_gpio_num, lcd_periph_signals.buses[bus->bus_id].cs_sig,
                                            next_device->flags.cs_active_high, false);
        }
    }
}

IRAM_ATTR static void lcd_default_isr_handler(void *args)
{
    esp_lcd_i80_bus_t *bus = (esp_lcd_i80_bus_t *)args;
    lcd_i80_trans_descriptor_t *trans_desc = NULL;
    lcd_panel_io_i80_t *cur_device = NULL;
    lcd_panel_io_i80_t *next_device = NULL;
    BaseType_t high_task_woken = pdFALSE;
    bool need_yield = false;
    uint32_t intr_status = lcd_ll_get_interrupt_status(bus->hal.dev);
    if (intr_status & LCD_LL_EVENT_TRANS_DONE) {
        // disable interrupt temporarily, only re-enable when there be remained transaction in the queue
        esp_intr_disable(bus->intr);
        trans_desc = bus->cur_trans; // the finished transaction
        cur_device = bus->cur_device;// the working device
        // process finished transaction
        if (trans_desc) {
            assert(trans_desc->i80_device == cur_device && "transaction device mismatch");
            // decrease pm lock reference count
            if (bus->pm_lock) {
                esp_pm_lock_release(bus->pm_lock);
            }
            // device callback
            if (trans_desc->trans_done_cb) {
                if (trans_desc->trans_done_cb(&cur_device->base, NULL, trans_desc->user_ctx)) {
                    need_yield = true;
                }
            }
            // move transaction to done_queue
            // there won't be case that will overflow the queue, so skip checking the return value
            high_task_woken = pdFALSE;
            xQueueSendFromISR(cur_device->done_queue, &trans_desc, &high_task_woken);
            if (high_task_woken == pdTRUE) {
                need_yield = true;
            }
            bus->cur_trans = NULL;
        }
        // fetch transactions from devices' trans_queue
        // Note: the first registered device will have the highest priority to be scheduled
        LIST_FOREACH(next_device, &bus->device_list, device_list_entry) {
            high_task_woken = pdFALSE;
            if (xQueueReceiveFromISR(next_device->trans_queue, &trans_desc, &high_task_woken) == pdTRUE) {
                if (high_task_woken == pdTRUE) {
                    need_yield = true;
                }
                // sanity check
                assert(trans_desc);
                // only clear the interrupt status when we're sure there still remains transaction to handle
                lcd_ll_clear_interrupt_status(bus->hal.dev, intr_status);
                // switch devices if necessary
                lcd_i80_switch_devices(cur_device, next_device);
                // only reverse data bit/bytes for color data
                lcd_ll_reverse_dma_data_bit_order(bus->hal.dev, next_device->flags.reverse_color_bits);
                lcd_ll_enable_swizzle(bus->hal.dev, next_device->flags.swap_color_bytes);
                bus->cur_trans = trans_desc;
                bus->cur_device = next_device;
                // mount data to DMA links
                lcd_com_mount_dma_data(bus->dma_nodes, trans_desc->data, trans_desc->data_length);
                // enable interrupt again, because the new transaction can trigger new trans done event
                esp_intr_enable(bus->intr);
                // increase the pm lock reference count before starting a new transaction
                if (bus->pm_lock) {
                    esp_pm_lock_acquire(bus->pm_lock);
                }
                lcd_start_transaction(bus, trans_desc);
                break; // exit for-each loop
            }
        }
    }
    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}
