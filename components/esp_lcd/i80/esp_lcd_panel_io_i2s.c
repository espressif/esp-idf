/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Although we're manipulating I2S peripheral (on esp32/s2 target), it has nothing to do with the AUDIO BUS.
// In fact, we're simulating the Intel 8080 bus with I2S peripheral, in a special parallel mode.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
#include "esp_intr_alloc.h"
#include "esp_heap_caps.h"
#include "esp_pm.h"
#include "esp_lcd_panel_io_interface.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_common.h"
#include "esp_rom_gpio.h"
#include "soc/soc_caps.h"
#include "hal/gpio_hal.h"
#include "driver/gpio.h"
#include "esp_clk_tree.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/i2s_platform.h"
#include "esp_private/gdma_link.h"
#include "soc/lcd_periph.h"
#include "hal/i2s_hal.h"
#include "hal/i2s_ll.h"
#include "hal/i2s_types.h"

// the DMA descriptor used by esp32 and esp32s2, each descriptor can carry 4095 bytes at most
#define LCD_DMA_DESCRIPTOR_BUFFER_MAX_SIZE 4095

static const char *TAG = "lcd_panel.io.i80";

typedef struct esp_lcd_i80_bus_t esp_lcd_i80_bus_t;
typedef struct lcd_panel_io_i80_t lcd_panel_io_i80_t;
typedef struct lcd_i80_trans_descriptor_t lcd_i80_trans_descriptor_t;

static esp_err_t panel_io_i80_tx_param(esp_lcd_panel_io_t *io, int lcd_cmd, const void *param, size_t param_size);
static esp_err_t panel_io_i80_tx_color(esp_lcd_panel_io_t *io, int lcd_cmd, const void *color, size_t color_size);
static esp_err_t panel_io_i80_del(esp_lcd_panel_io_t *io);
static esp_err_t i2s_lcd_select_periph_clock(esp_lcd_i80_bus_handle_t bus, lcd_clock_source_t src);
static esp_err_t i2s_lcd_init_dma_link(esp_lcd_i80_bus_handle_t bus);
static esp_err_t i2s_lcd_configure_gpio(esp_lcd_i80_bus_handle_t bus, const esp_lcd_i80_bus_config_t *bus_config);
static void i2s_lcd_trigger_quick_trans_done_event(esp_lcd_i80_bus_handle_t bus);
static void lcd_i80_switch_devices(lcd_panel_io_i80_t *cur_device, lcd_panel_io_i80_t *next_device);
static void lcd_default_isr_handler(void *args);
static esp_err_t panel_io_i80_register_event_callbacks(esp_lcd_panel_io_handle_t io, const esp_lcd_panel_io_callbacks_t *cbs, void *user_ctx);

struct esp_lcd_i80_bus_t {
    int bus_id;            // Bus ID, index from 0
    portMUX_TYPE spinlock; // spinlock used to protect i80 bus members(hal, device_list, cur_trans)
    i2s_hal_context_t hal; // Hal object
    size_t bus_width;      // Number of data lines
    int dc_gpio_num;       // GPIO used for DC line
    int wr_gpio_num;       // GPIO used for WR line
    intr_handle_t intr;    // LCD peripheral interrupt handle
    esp_pm_lock_handle_t pm_lock; // lock APB frequency when necessary
    size_t num_dma_nodes;  // Number of DMA descriptors
    gdma_link_list_handle_t dma_link; // DMA link list handle
    uint8_t *format_buffer;// The driver allocates an internal buffer for DMA to do data format transformer
    unsigned long resolution_hz;  // LCD_CLK resolution, determined by selected clock source
    lcd_i80_trans_descriptor_t *cur_trans; // Current transaction
    lcd_panel_io_i80_t *cur_device; // Current working device
    LIST_HEAD(i80_device_list, lcd_panel_io_i80_t) device_list; // Head of i80 device list
    struct {
        unsigned int exclusive: 1; // Indicate whether the I80 bus is owned by one device (whose CS GPIO is not assigned) exclusively
    } flags;
};

struct lcd_i80_trans_descriptor_t {
    lcd_panel_io_i80_t *i80_device; // i80 device issuing this transaction
    const void *data;     // Data buffer
    uint32_t data_length; // Data buffer size
    esp_lcd_panel_io_color_trans_done_cb_t trans_done_cb; // transaction done callback
    void *user_ctx;   // private data used by trans_done_cb
    struct {
        unsigned int dc_level: 1; // Level of DC line for this transaction
    } flags;
};

struct lcd_panel_io_i80_t {
    esp_lcd_panel_io_t base;   // Base class of generic lcd panel io
    esp_lcd_i80_bus_t *bus;    // Which bus the device is attached to
    int cs_gpio_num;           // GPIO used for CS line
    uint32_t pclk_hz;          // PCLK clock frequency
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
        unsigned int dc_cmd_level: 1;  // Level of DC line in CMD phase
        unsigned int dc_data_level: 1; // Level of DC line in DATA phase
    } dc_levels;
    struct {
        unsigned int cs_active_high: 1;   // Whether the CS line is active on high level
        unsigned int swap_color_bytes: 1; // Swap adjacent two data bytes before sending out
        unsigned int pclk_idle_low: 1;    // The WR line keeps at low level in IDLE phase
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
    ESP_GOTO_ON_FALSE(bus_config && ret_bus, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    // although I2S bus supports up to 24 parallel data lines, we restrict users to only use 8 or 16 bit width
    ESP_GOTO_ON_FALSE(bus_config->bus_width == 8 || bus_config->bus_width == 16, ESP_ERR_INVALID_ARG, err,
                      TAG, "invalid bus width:%d", bus_config->bus_width);
    size_t max_transfer_bytes = (bus_config->max_transfer_bytes + 3) & ~0x03; // align up to 4 bytes
#if SOC_I2S_TRANS_SIZE_ALIGN_WORD
    // double the size of the internal DMA buffer if bus_width is 8,
    // because one I2S FIFO (4 bytes) will only contain two bytes of valid data
    max_transfer_bytes = max_transfer_bytes * 16 / bus_config->bus_width + 4;
#endif
    // allocate i80 bus memory
    bus = heap_caps_calloc(1, sizeof(esp_lcd_i80_bus_t), LCD_I80_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(bus, ESP_ERR_NO_MEM, err, TAG, "no mem for i80 bus");
    size_t num_dma_nodes = max_transfer_bytes / LCD_DMA_DESCRIPTOR_BUFFER_MAX_SIZE + 1;
    // create DMA link list
    gdma_link_list_config_t dma_link_config = {
        .buffer_alignment = 1, // no special buffer alignment for LCD TX buffer
        .item_alignment = 4,  // 4 bytes alignment for each DMA descriptor
        .num_items = num_dma_nodes,
        .flags = {
            .check_owner = true,
        },
    };
    ESP_GOTO_ON_ERROR(gdma_new_link_list(&dma_link_config, &bus->dma_link), err, TAG, "create DMA link list failed");
    bus->bus_id = -1;
    bus->num_dma_nodes = num_dma_nodes;
#if SOC_I2S_TRANS_SIZE_ALIGN_WORD
    // transform format for LCD commands, parameters and color data, so we need a big buffer
    bus->format_buffer = heap_caps_calloc(1, max_transfer_bytes, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_DMA);
#else
    // only transform format for LCD parameters, buffer size depends on specific LCD, set at compile time
    bus->format_buffer = heap_caps_calloc(1, CONFIG_LCD_PANEL_IO_FORMAT_BUF_SIZE, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_DMA);
#endif // SOC_I2S_TRANS_SIZE_ALIGN_WORD
    ESP_GOTO_ON_FALSE(bus->format_buffer, ESP_ERR_NO_MEM, err, TAG, "no mem for format buffer");
    // LCD mode can't work with other modes at the same time, we need to register the driver object to the I2S platform
    int bus_id = -1;
    for (int i = 0; i < SOC_LCD_I80_BUSES; i++) {
        if (i2s_platform_acquire_occupation(I2S_CTLR_HP, i, "esp_lcd_panel_io_i2s") == ESP_OK) {
            bus_id = i;
            break;
        }
    }
    ESP_GOTO_ON_FALSE(bus_id != -1, ESP_ERR_NOT_FOUND, err, TAG, "no free i80 bus slot");
    bus->bus_id = bus_id;
    // initialize HAL layer
    i2s_hal_init(&bus->hal, bus->bus_id);
    // set peripheral clock resolution
    ret = i2s_lcd_select_periph_clock(bus, bus_config->clk_src);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "select periph clock failed");
    // reset peripheral, DMA channel and FIFO
    i2s_ll_tx_reset(bus->hal.dev);
    i2s_ll_tx_reset_dma(bus->hal.dev);
    i2s_ll_tx_reset_fifo(bus->hal.dev);
    // install interrupt service, (I2S LCD mode only uses the "TX Unit", which leaves "RX Unit" for other purpose)
    // So the interrupt should also be able to share with other functionality
    int isr_flags = LCD_I80_INTR_ALLOC_FLAGS | ESP_INTR_FLAG_SHARED | ESP_INTR_FLAG_LOWMED;
    ret = esp_intr_alloc_intrstatus(lcd_periph_signals.buses[bus->bus_id].irq_id, isr_flags,
                                    (uint32_t)i2s_ll_get_intr_status_reg(bus->hal.dev),
                                    I2S_LL_EVENT_TX_EOF, lcd_default_isr_handler, bus, &bus->intr);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "install interrupt failed");
    i2s_ll_enable_intr(bus->hal.dev, I2S_LL_EVENT_TX_EOF, false); // disable interrupt temporarily
    i2s_ll_clear_intr_status(bus->hal.dev, I2S_LL_EVENT_TX_EOF);  // clear pending interrupt
    // initialize DMA link
    i2s_lcd_init_dma_link(bus);
    // enable I2S LCD master mode (refer to I2S TRM)
    i2s_ll_enable_lcd(bus->hal.dev, true);
    i2s_ll_tx_stop_on_fifo_empty(bus->hal.dev, true);
    i2s_ll_tx_set_slave_mod(bus->hal.dev, false);
    i2s_ll_tx_set_bits_mod(bus->hal.dev, bus_config->bus_width);
    i2s_ll_tx_select_std_slot(bus->hal.dev, I2S_STD_SLOT_BOTH, true); // copy mono
    bus->bus_width = bus_config->bus_width;
    i2s_ll_tx_enable_right_first(bus->hal.dev, true);
#if SOC_I2S_SUPPORTS_DMA_EQUAL
    i2s_ll_tx_enable_dma_equal(bus->hal.dev, true);
#endif
    // enable trans done interrupt
    i2s_ll_enable_intr(bus->hal.dev, I2S_LL_EVENT_TX_EOF, true);
    // trigger a quick "trans done" event, and wait for the interrupt line goes active
    // this could ensure we go into ISR handler next time we call `esp_intr_enable`
    i2s_lcd_trigger_quick_trans_done_event(bus);
    // configure GPIO
    ret = i2s_lcd_configure_gpio(bus, bus_config);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "configure GPIO failed");
    // fill other i80 bus runtime parameters
    LIST_INIT(&bus->device_list); // initialize device list head
    bus->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    bus->dc_gpio_num = bus_config->dc_gpio_num;
    bus->wr_gpio_num = bus_config->wr_gpio_num;
    *ret_bus = bus;
    ESP_LOGD(TAG, "new i80 bus(%d) @%p, %zu dma nodes, resolution %luHz", bus->bus_id, bus, num_dma_nodes, bus->resolution_hz);
    return ESP_OK;

err:
    if (bus) {
        if (bus->dma_link) {
            gdma_del_link_list(bus->dma_link);
        }
        if (bus->intr) {
            esp_intr_free(bus->intr);
        }
        if (bus->bus_id >= 0) {
            i2s_platform_release_occupation(I2S_CTLR_HP, bus->bus_id);
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
    i2s_platform_release_occupation(I2S_CTLR_HP, bus_id);
    esp_intr_free(bus->intr);
    if (bus->pm_lock) {
        esp_pm_lock_delete(bus->pm_lock);
    }
    free(bus->format_buffer);
    gdma_del_link_list(bus->dma_link);
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
    // because we set the I2S's left channel data same to right channel, so f_pclk = f_i2s/pclk_div/2
    uint32_t pclk_prescale = bus->resolution_hz / 2 / io_config->pclk_hz;
    ESP_GOTO_ON_FALSE(pclk_prescale > 0 && pclk_prescale <= I2S_LL_BCK_MAX_PRESCALE, ESP_ERR_NOT_SUPPORTED, err, TAG,
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
    i80_device->queue_size = io_config->trans_queue_depth;
    i80_device->clock_prescale = pclk_prescale;
    i80_device->lcd_cmd_bits = io_config->lcd_cmd_bits;
    i80_device->lcd_param_bits = io_config->lcd_param_bits;
    i80_device->pclk_hz = bus->resolution_hz / pclk_prescale / 2;
    i80_device->dc_levels.dc_cmd_level = io_config->dc_levels.dc_cmd_level;
    i80_device->dc_levels.dc_data_level = io_config->dc_levels.dc_data_level;
    i80_device->cs_gpio_num = io_config->cs_gpio_num;
    i80_device->on_color_trans_done = io_config->on_color_trans_done;
    i80_device->user_ctx = io_config->user_ctx;
    i80_device->flags.cs_active_high = io_config->flags.cs_active_high;
    i80_device->flags.swap_color_bytes = io_config->flags.swap_color_bytes;
    i80_device->flags.pclk_idle_low = io_config->flags.pclk_idle_low;
    // fill panel io function table
    i80_device->base.del = panel_io_i80_del;
    i80_device->base.tx_param = panel_io_i80_tx_param;
    i80_device->base.tx_color = panel_io_i80_tx_color;
    i80_device->base.register_event_callbacks = panel_io_i80_register_event_callbacks;
    if (io_config->cs_gpio_num >= 0) {
        // CS signal is controlled by software
        gpio_set_level(io_config->cs_gpio_num, !io_config->flags.cs_active_high); // de-assert by default
        gpio_set_direction(io_config->cs_gpio_num, GPIO_MODE_OUTPUT);
        gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[io_config->cs_gpio_num], PIN_FUNC_GPIO);
    }
    *ret_io = &(i80_device->base);
    ESP_LOGD(TAG, "new i80 lcd panel io @%p on bus(%d), pclk=%"PRIu32"Hz", i80_device, bus->bus_id, i80_device->pclk_hz);
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

void *esp_lcd_i80_alloc_draw_buffer(esp_lcd_panel_io_handle_t io, size_t size, uint32_t caps)
{
    ESP_RETURN_ON_FALSE(io, NULL, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE((caps & MALLOC_CAP_SPIRAM) == 0, NULL, TAG, "external memory is not supported");
    // DMA can only carry internal memory
    return heap_caps_aligned_calloc(4, 1, size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_DMA);
}

static esp_err_t panel_io_i80_del(esp_lcd_panel_io_t *io)
{
    lcd_panel_io_i80_t *i80_device = __containerof(io, lcd_panel_io_i80_t, base);
    esp_lcd_i80_bus_t *bus = i80_device->bus;
    lcd_i80_trans_descriptor_t *trans_desc = NULL;
    size_t num_trans_inflight = i80_device->num_trans_inflight;
    // wait all pending transaction to finish
    for (size_t i = 0; i < num_trans_inflight; i++) {
        ESP_RETURN_ON_FALSE(xQueueReceive(i80_device->done_queue, &trans_desc, portMAX_DELAY) == pdTRUE,
                            ESP_FAIL, TAG, "recycle inflight transactions failed");
        i80_device->num_trans_inflight--;
    }
    // remove from device list
    portENTER_CRITICAL(&bus->spinlock);
    LIST_REMOVE(i80_device, device_list_entry);
    portEXIT_CRITICAL(&bus->spinlock);

    // reset CS GPIO
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
        ESP_LOGW(TAG, "Callback on_color_trans_done was already set and now it was owerwritten!");
    }

    i80_device->on_color_trans_done = cbs->on_color_trans_done;
    i80_device->user_ctx = user_ctx;

    return ESP_OK;
}

static void i2s_lcd_prepare_cmd_buffer(lcd_i80_trans_descriptor_t *trans_desc, const void *cmd)
{
    lcd_panel_io_i80_t *i80_device = trans_desc->i80_device;
    esp_lcd_i80_bus_t *bus = i80_device->bus;
    uint8_t *from = (uint8_t *)cmd;
    // LCD is big-endian, e.g. to send command 0x1234, byte 0x12 should appear on the data bus first
    // However, the I2S peripheral will send 0x34 first, so we reversed the order below
    if (bus->bus_width < i80_device->lcd_cmd_bits) {
        int start = 0;
        int end = i80_device->lcd_cmd_bits / 8 - 1;
        lcd_com_reverse_buffer_bytes(from, start, end);
    }
#if SOC_I2S_TRANS_SIZE_ALIGN_WORD
    uint8_t *to = bus->format_buffer;
    int cmd_cycle = i80_device->lcd_cmd_bits / bus->bus_width;
    if (cmd_cycle * bus->bus_width < i80_device->lcd_cmd_bits) {
        cmd_cycle++;
    }
    int bytes_to_copy = MIN(bus->bus_width, i80_device->lcd_cmd_bits) / 8;
    int cnt_from = 0;
    // format command buffer
    for (int i = 0; i < cmd_cycle; i++) {
        for (int j = 0; j < bytes_to_copy; j++) {
            to[2 + j] = from[cnt_from++];
        }
        to += 4;
    }
    trans_desc->data = bus->format_buffer;
    trans_desc->data_length = cmd_cycle * 4;
#else
    trans_desc->data = cmd;
    trans_desc->data_length = MAX(i80_device->lcd_cmd_bits, bus->bus_width) / 8;
#endif
}

static void i2s_lcd_prepare_param_buffer(lcd_i80_trans_descriptor_t *trans_desc, const void *param, size_t param_num)
{
    lcd_panel_io_i80_t *i80_device = trans_desc->i80_device;
    esp_lcd_i80_bus_t *bus = i80_device->bus;
    uint8_t *from = (uint8_t *)param;
    int param_size = i80_device->lcd_param_bits / 8;
    // LCD is big-endian, e.g. to send param 0x1234, byte 0x12 should appear on the data bus first
    // However, the I2S peripheral will send 0x34 first, so we reversed the order below
    if (bus->bus_width < i80_device->lcd_param_bits) {
        for (size_t i = 0; i < param_num; i++) {
            int start = i * param_size;
            int end = start + param_size - 1;
            lcd_com_reverse_buffer_bytes(from, start, end);
        }
    }
#if SOC_I2S_TRANS_SIZE_ALIGN_WORD
    uint8_t *to = bus->format_buffer;
    int param_cycle = i80_device->lcd_param_bits / bus->bus_width;
    if (param_cycle * bus->bus_width < i80_device->lcd_param_bits) {
        param_cycle++;
    }
    int ele_cycles = param_cycle * param_num;
    int bytes_to_copy = MIN(bus->bus_width, i80_device->lcd_param_bits) / 8;
    int cnt_from = 0;
    // format parameter buffer
    for (int i = 0; i < ele_cycles; i++) {
        for (int j = 0; j < bytes_to_copy; j++) {
            to[2 + j] = from[cnt_from++];
        }
        to += 4;
    }
    trans_desc->data = bus->format_buffer;
    trans_desc->data_length = ele_cycles * 4;
#else
    uint8_t *to = bus->format_buffer;
    uint8_t step = bus->bus_width / 8;
    int param_cycle = i80_device->lcd_param_bits / bus->bus_width;
    if (param_cycle * bus->bus_width < i80_device->lcd_param_bits) {
        param_cycle++;
    }
    int ele_cycles = param_cycle * param_num;
    int bytes_to_copy = MIN(bus->bus_width, i80_device->lcd_param_bits) / 8;
    int cnt_from = 0;
    // format parameter buffer
    for (int i = 0; i < ele_cycles; i++) {
        for (int j = 0; j < bytes_to_copy; j++) {
            to[j] = from[cnt_from++];
        }
        to += step;
    }
    trans_desc->data = bus->format_buffer;
    trans_desc->data_length = to - bus->format_buffer;
#endif
}

static void i2s_lcd_prepare_color_buffer(lcd_i80_trans_descriptor_t *trans_desc, const void *color, size_t color_size)
{
#if SOC_I2S_TRANS_SIZE_ALIGN_WORD
    lcd_panel_io_i80_t *i80_device = trans_desc->i80_device;
    esp_lcd_i80_bus_t *bus = i80_device->bus;
    uint8_t *from = (uint8_t *)color;
    uint8_t *to = bus->format_buffer;
    int bytes_to_copy = bus->bus_width / 8;
    int cnt_from = 0;
    int first_half = i80_device->flags.swap_color_bytes ? 0 : 2;
    int second_half = i80_device->flags.swap_color_bytes ? 2 : 0;
    // format color buffer
    while (cnt_from < color_size) {
        for (int i = 0; i < bytes_to_copy; i++) {
            to[first_half + i] = from[cnt_from++];
        }
        for (int i = 0; i < bytes_to_copy; i++) {
            to[second_half + i] = from[cnt_from++];
        }
        to += 4;
    }
    trans_desc->data = bus->format_buffer;
    trans_desc->data_length = to - bus->format_buffer;
#else
    trans_desc->data = color;
    trans_desc->data_length = color_size;
#endif
}

static esp_err_t panel_io_i80_tx_param(esp_lcd_panel_io_t *io, int lcd_cmd, const void *param, size_t param_size)
{
    lcd_panel_io_i80_t *next_device = __containerof(io, lcd_panel_io_i80_t, base);
    esp_lcd_i80_bus_t *bus = next_device->bus;
    lcd_panel_io_i80_t *cur_device = bus->cur_device;
    lcd_i80_trans_descriptor_t *trans_desc = NULL;
    assert(param_size <= (bus->num_dma_nodes * LCD_DMA_DESCRIPTOR_BUFFER_MAX_SIZE) && "parameter bytes too long, enlarge max_transfer_bytes");
    assert(param_size <= CONFIG_LCD_PANEL_IO_FORMAT_BUF_SIZE && "format buffer too small, increase CONFIG_LCD_PANEL_IO_FORMAT_BUF_SIZE");
    size_t num_trans_inflight = next_device->num_trans_inflight;
    // before issue a polling transaction, need to wait queued transactions finished
    for (size_t i = 0; i < num_trans_inflight; i++) {
        ESP_RETURN_ON_FALSE(xQueueReceive(next_device->done_queue, &trans_desc, portMAX_DELAY) == pdTRUE,
                            ESP_FAIL, TAG, "recycle inflight transactions failed");
        next_device->num_trans_inflight--;
    }

    gdma_buffer_mount_config_t mount_config = {
        .flags = {
            .mark_eof = true,
            .mark_final = true, // singly link list, mark final descriptor
        }
    };

    i2s_ll_clear_intr_status(bus->hal.dev, I2S_LL_EVENT_TX_EOF);
    // switch devices if necessary
    lcd_i80_switch_devices(cur_device, next_device);
    trans_desc = &next_device->trans_pool[0];
    trans_desc->i80_device = next_device;
    trans_desc->trans_done_cb = NULL; // no callback for command transfer
    bus->cur_trans = trans_desc;
#if SOC_I2S_TRANS_SIZE_ALIGN_WORD
    // switch to I2S 32bits mode, one WS cycle <=> one I2S FIFO
    i2s_ll_tx_set_bits_mod(bus->hal.dev, 32);
#endif
    i2s_lcd_prepare_cmd_buffer(trans_desc, &lcd_cmd);
    mount_config.buffer = (void *)trans_desc->data;
    mount_config.length = trans_desc->data_length;
    gdma_link_mount_buffers(bus->dma_link, 0, &mount_config, 1, NULL);
    gpio_set_level(bus->dc_gpio_num, next_device->dc_levels.dc_cmd_level);
    i2s_ll_tx_stop(bus->hal.dev);
    i2s_ll_tx_reset(bus->hal.dev); // reset TX engine first
    i2s_ll_start_out_link(bus->hal.dev);
    // delay a while, wait for DMA data being feed to I2S FIFO
    // in fact, this is only needed when LCD pixel clock is set too high
    esp_rom_delay_us(1);
    // increase the pm lock reference count before starting a new transaction
    if (bus->pm_lock) {
        esp_pm_lock_acquire(bus->pm_lock);
    }
    i2s_ll_tx_start(bus->hal.dev);
    // polling the trans done event
    while (!(i2s_ll_get_intr_status(bus->hal.dev) & I2S_LL_EVENT_TX_EOF)) {}

    // parameter is usually short, using polling mode
    if (param && param_size) {
        i2s_ll_clear_intr_status(bus->hal.dev, I2S_LL_EVENT_TX_EOF);
        i2s_lcd_prepare_param_buffer(trans_desc, param, param_size * 8 / next_device->lcd_param_bits);
        mount_config.buffer = (void *)trans_desc->data;
        mount_config.length = trans_desc->data_length;
        gdma_link_mount_buffers(bus->dma_link, 0, &mount_config, 1, NULL);
        gpio_set_level(bus->dc_gpio_num, next_device->dc_levels.dc_data_level);
        i2s_ll_tx_stop(bus->hal.dev);
        i2s_ll_tx_reset(bus->hal.dev); // reset TX engine first
        i2s_ll_start_out_link(bus->hal.dev);
        esp_rom_delay_us(1);
        i2s_ll_tx_start(bus->hal.dev);
        // polling the trans done event, but don't clear the event status
        while (!(i2s_ll_get_intr_status(bus->hal.dev) & I2S_LL_EVENT_TX_EOF)) {}
    }
    // decrease pm lock reference count
    if (bus->pm_lock) {
        esp_pm_lock_release(bus->pm_lock);
    }
    bus->cur_trans = NULL;
    return ESP_OK;
}

static esp_err_t panel_io_i80_tx_color(esp_lcd_panel_io_t *io, int lcd_cmd, const void *color, size_t color_size)
{
    lcd_panel_io_i80_t *next_device = __containerof(io, lcd_panel_io_i80_t, base);
    esp_lcd_i80_bus_t *bus = next_device->bus;
    lcd_panel_io_i80_t *cur_device = bus->cur_device;
    lcd_i80_trans_descriptor_t *trans_desc = NULL;
    assert(color_size <= (bus->num_dma_nodes * LCD_DMA_DESCRIPTOR_BUFFER_MAX_SIZE) && "color bytes too long, enlarge max_transfer_bytes");
    size_t num_trans_inflight = next_device->num_trans_inflight;
    // before issue a polling transaction, need to wait queued transactions finished
    for (size_t i = 0; i < num_trans_inflight; i++) {
        ESP_RETURN_ON_FALSE(xQueueReceive(next_device->done_queue, &trans_desc, portMAX_DELAY) == pdTRUE,
                            ESP_FAIL, TAG, "recycle inflight transactions failed");
        next_device->num_trans_inflight--;
    }

    gdma_buffer_mount_config_t mount_config = {
        .flags = {
            .mark_eof = true,
            .mark_final = true, // singly link list, mark final descriptor
        }
    };

    i2s_ll_clear_intr_status(bus->hal.dev, I2S_LL_EVENT_TX_EOF);
    // switch devices if necessary
    lcd_i80_switch_devices(cur_device, next_device);
    trans_desc = &next_device->trans_pool[0];
    trans_desc->i80_device = next_device;
    trans_desc->trans_done_cb = NULL; // no callback for command transfer
    bus->cur_trans = trans_desc;
#if SOC_I2S_TRANS_SIZE_ALIGN_WORD
    // switch to I2S 32bits mode, one WS cycle <=> one I2S FIFO
    i2s_ll_tx_set_bits_mod(bus->hal.dev, 32);
#endif
    i2s_lcd_prepare_cmd_buffer(trans_desc, &lcd_cmd);
    mount_config.buffer = (void *)trans_desc->data;
    mount_config.length = trans_desc->data_length;
    gdma_link_mount_buffers(bus->dma_link, 0, &mount_config, 1, NULL);
    gpio_set_level(bus->dc_gpio_num, next_device->dc_levels.dc_cmd_level);
    i2s_ll_tx_stop(bus->hal.dev);
    i2s_ll_tx_reset(bus->hal.dev); // reset TX engine first
    i2s_ll_start_out_link(bus->hal.dev);
    esp_rom_delay_us(1);
    // increase the pm lock reference count before starting a new transaction
    if (bus->pm_lock) {
        esp_pm_lock_acquire(bus->pm_lock);
    }
    i2s_ll_tx_start(bus->hal.dev);
    // polling the trans done event
    while (!(i2s_ll_get_intr_status(bus->hal.dev) & I2S_LL_EVENT_TX_EOF)) {}
    // decrease pm lock reference count
    if (bus->pm_lock) {
        esp_pm_lock_release(bus->pm_lock);
    }
    bus->cur_trans = NULL;

    // sending LCD color data to queue
    trans_desc->trans_done_cb = next_device->on_color_trans_done;
    trans_desc->user_ctx = next_device->user_ctx;
    trans_desc->flags.dc_level = next_device->dc_levels.dc_data_level; // DC level for data transaction
    i2s_lcd_prepare_color_buffer(trans_desc, color, color_size);
    // send transaction to trans_queue
    xQueueSend(next_device->trans_queue, &trans_desc, portMAX_DELAY);
    next_device->num_trans_inflight++;
    // enable interrupt and go into isr handler, where we fetch the transactions from trans_queue and start it
    // we will go into `lcd_default_isr_handler` almost at once, because the "trans done" event is active at the moment
    esp_intr_enable(bus->intr);
    return ESP_OK;
}

static esp_err_t i2s_lcd_select_periph_clock(esp_lcd_i80_bus_handle_t bus, lcd_clock_source_t src)
{
    // get clock source frequency
    uint32_t src_clk_hz = 0;
    ESP_RETURN_ON_ERROR(esp_clk_tree_src_get_freq_hz((soc_module_clk_t)src, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &src_clk_hz),
                        TAG, "get clock source frequency failed");

    // I2S clock source is binary compatible with lcd_clock_source_t
    i2s_ll_tx_clk_set_src(bus->hal.dev, (i2s_clock_src_t)src);
    i2s_ll_set_raw_mclk_div(bus->hal.dev, LCD_PERIPH_CLOCK_PRE_SCALE, 1, 0);
    // save the resolution of the i80 bus
    bus->resolution_hz = src_clk_hz / LCD_PERIPH_CLOCK_PRE_SCALE;

    // create pm lock based on different clock source
    // clock sources like PLL and XTAL will be turned off in light sleep
#if CONFIG_PM_ENABLE
    ESP_RETURN_ON_ERROR(esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "i80_bus_lcd", &bus->pm_lock), TAG, "create pm lock failed");
#endif
    return ESP_OK;
}

static esp_err_t i2s_lcd_init_dma_link(esp_lcd_i80_bus_handle_t bus)
{
    i2s_ll_dma_enable_eof_on_fifo_empty(bus->hal.dev, true);
    i2s_ll_dma_enable_owner_check(bus->hal.dev, true);
    i2s_ll_dma_enable_auto_write_back(bus->hal.dev, true);
    i2s_ll_set_out_link_addr(bus->hal.dev, gdma_link_get_head_addr(bus->dma_link));
    i2s_ll_enable_dma(bus->hal.dev, true);
    return ESP_OK;
}

static esp_err_t i2s_lcd_configure_gpio(esp_lcd_i80_bus_handle_t bus, const esp_lcd_i80_bus_config_t *bus_config)
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
    // connect peripheral signals via GPIO matrix
    // data line
    for (size_t i = 0; i < bus_config->bus_width; i++) {
        gpio_set_direction(bus_config->data_gpio_nums[i], GPIO_MODE_OUTPUT);
#if SOC_I2S_TRANS_SIZE_ALIGN_WORD
        esp_rom_gpio_connect_out_signal(bus_config->data_gpio_nums[i], lcd_periph_signals.buses[bus_id].data_sigs[i + 8], false, false);
#else
        esp_rom_gpio_connect_out_signal(bus_config->data_gpio_nums[i], lcd_periph_signals.buses[bus_id].data_sigs[i + SOC_LCD_I80_BUS_WIDTH - bus_config->bus_width], false, false);
#endif
        gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[bus_config->data_gpio_nums[i]], PIN_FUNC_GPIO);
    }
    // WR signal (pclk)
    gpio_set_direction(bus_config->wr_gpio_num, GPIO_MODE_OUTPUT);
    esp_rom_gpio_connect_out_signal(bus_config->wr_gpio_num, lcd_periph_signals.buses[bus_id].wr_sig, true, false);
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[bus_config->wr_gpio_num], PIN_FUNC_GPIO);
    // DC signal is controlled by software, set as general purpose IO
    gpio_set_direction(bus_config->dc_gpio_num, GPIO_MODE_OUTPUT);
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[bus_config->dc_gpio_num], PIN_FUNC_GPIO);
    return ESP_OK;
}

static void i2s_lcd_trigger_quick_trans_done_event(esp_lcd_i80_bus_handle_t bus)
{
    // trigger a quick interrupt event by a dummy transaction, wait the LCD interrupt line goes active
    // next time when esp_intr_enable is invoked, we can go into interrupt handler immediately
    // where we dispatch transactions for i80 devices
    static uint32_t fake_trigger = 0;
    gdma_buffer_mount_config_t mount_config = {
        .buffer = &fake_trigger,
        .length = 4,
        .flags = {
            .mark_eof = true,   // mark the "EOF" flag to trigger I2S EOF interrupt
            .mark_final = true, // singly link list, mark final descriptor
        }
    };
    gdma_link_mount_buffers(bus->dma_link, 0, &mount_config, 1, NULL);
    i2s_ll_start_out_link(bus->hal.dev);
    i2s_ll_tx_start(bus->hal.dev);
    while (!(i2s_ll_get_intr_status(bus->hal.dev) & I2S_LL_EVENT_TX_EOF)) {}
}

static void lcd_i80_switch_devices(lcd_panel_io_i80_t *cur_device, lcd_panel_io_i80_t *next_device)
{
    // the caller should make sure the next_device and cur_device are attached to the same bus
    esp_lcd_i80_bus_t *bus = next_device->bus;
    if (next_device != cur_device) {
        // reconfigure PCLK for the new device
        i2s_ll_tx_set_bck_div_num(bus->hal.dev, next_device->clock_prescale);
        if (cur_device && cur_device->cs_gpio_num >= 0) { // de-assert current device
            gpio_set_level(cur_device->cs_gpio_num, !cur_device->flags.cs_active_high);
        }
        if (next_device->cs_gpio_num >= 0) {
            gpio_set_level(next_device->cs_gpio_num, next_device->flags.cs_active_high); // select the next device
        }
        // the WR signal (a.k.a the PCLK) generated by I2S is low level in idle stage
        // but most of 8080 LCDs require the WR line to be in high level during idle stage
        esp_rom_gpio_connect_out_signal(bus->wr_gpio_num, lcd_periph_signals.buses[bus->bus_id].wr_sig, !next_device->flags.pclk_idle_low, false);
    }
    bus->cur_device = next_device;
}

static IRAM_ATTR void lcd_default_isr_handler(void *args)
{
    esp_lcd_i80_bus_t *bus = (esp_lcd_i80_bus_t *)args;
    lcd_i80_trans_descriptor_t *trans_desc = NULL;
    lcd_panel_io_i80_t *cur_device = NULL;
    lcd_panel_io_i80_t *next_device = NULL;
    BaseType_t high_task_woken = pdFALSE;
    bool need_yield = false;
    uint32_t intr_status = i2s_ll_get_intr_status(bus->hal.dev);
    if (intr_status & I2S_LL_EVENT_TX_EOF) { // trans done event
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
                i2s_ll_clear_intr_status(bus->hal.dev, I2S_LL_EVENT_TX_EOF);
                // switch devices if necessary
                lcd_i80_switch_devices(cur_device, next_device);
                bus->cur_trans = trans_desc;
                gpio_set_level(bus->dc_gpio_num, trans_desc->flags.dc_level);
                // mount data to DMA links
                gdma_buffer_mount_config_t mount_config = {
                    .buffer = (void *)trans_desc->data,
                    .length = trans_desc->data_length,
                    .flags = {
                        .mark_eof = true,
                        .mark_final = true, // singly link list, mark final descriptor
                    }
                };
                gdma_link_mount_buffers(bus->dma_link, 0, &mount_config, 1, NULL);
#if SOC_I2S_TRANS_SIZE_ALIGN_WORD
                // switch to I2S 16bits mode, two WS cycle <=> one I2S FIFO
                i2s_ll_tx_set_bits_mod(bus->hal.dev, 16);
#endif
                // enable interrupt again, because the new transaction can trigger new trans done event
                esp_intr_enable(bus->intr);
                i2s_ll_tx_stop(bus->hal.dev);
                i2s_ll_tx_reset(bus->hal.dev); // reset TX engine first
                i2s_ll_start_out_link(bus->hal.dev);
                esp_rom_delay_us(1);
                // increase the pm lock reference count before starting a new transaction
                if (bus->pm_lock) {
                    esp_pm_lock_acquire(bus->pm_lock);
                }
                i2s_ll_tx_start(bus->hal.dev);
                break; // exit for-each loop
            }
        }
    }
    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}
