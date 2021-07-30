/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// #define LOG_LOCAL_LEVEL ESP_LOG_DEBUG

#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>
#include <sys/queue.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_attr.h"
#include "esp_check.h"
#include "esp_intr_alloc.h"
#include "esp_heap_caps.h"
#include "esp_lcd_panel_io_interface.h"
#include "esp_lcd_panel_io.h"
#include "esp_rom_gpio.h"
#include "soc/soc_caps.h"
#include "hal/dma_types.h"
#include "hal/gpio_hal.h"
#include "esp_private/gdma.h"
#include "driver/gpio.h"
#include "driver/periph_ctrl.h"
#if SOC_LCDCAM_SUPPORTED
#include "esp_lcd_common.h"
#include "soc/lcd_periph.h"
#include "hal/lcd_ll.h"
#include "hal/lcd_hal.h"

static const char *TAG = "lcd_panel.io.i80";

typedef struct esp_lcd_i80_bus_t esp_lcd_i80_bus_t;
typedef struct lcd_panel_io_i80_t lcd_panel_io_i80_t;
typedef struct lcd_i80_trans_descriptor_t lcd_i80_trans_descriptor_t;

static esp_err_t panel_io_i80_tx_param(esp_lcd_panel_io_t *io, int lcd_cmd, int lcd_cmd_bits, const void *param, size_t param_size);
static esp_err_t panel_io_i80_tx_color(esp_lcd_panel_io_t *io, int lcd_cmd, int lcd_cmd_bits, const void *color, size_t color_size);
static esp_err_t panel_io_i80_del(esp_lcd_panel_io_t *io);
static esp_err_t lcd_i80_bus_create_trans_link(esp_lcd_i80_bus_handle_t bus);
static void lcd_periph_trigger_quick_trans_done_event(esp_lcd_i80_bus_handle_t bus);
static esp_err_t lcd_i80_bus_configure_gpio(esp_lcd_i80_bus_handle_t bus, const esp_lcd_i80_bus_config_t *bus_config);
static void lcd_i80_switch_devices(lcd_panel_io_i80_t *cur_device, lcd_panel_io_i80_t *next_device);
static void lcd_start_transaction(esp_lcd_i80_bus_t *bus, lcd_i80_trans_descriptor_t *trans_desc);
static IRAM_ATTR void lcd_default_isr_handler(void *args);

struct esp_lcd_i80_bus_t {
    int bus_id;            // Bus ID, index from 0
    portMUX_TYPE spinlock; // spinlock used to protect i80 bus members(hal, device_list, cur_trans)
    lcd_hal_context_t hal; // Hal object
    size_t data_width;   // Number of data lines
    intr_handle_t intr;    // LCD peripheral interrupt handle
    size_t num_dma_nodes;  // Number of DMA descriptors
    size_t resolution_hz;    // LCD_CLK resolution, determined by selected clock source
    gdma_channel_handle_t dma_chan; // DMA channel handle
    lcd_i80_trans_descriptor_t *cur_trans; // Current transaction
    lcd_panel_io_i80_t *cur_device; // Current working device
    LIST_HEAD(i80_device_list, lcd_panel_io_i80_t) device_list; // Head of i80 device list
    dma_descriptor_t dma_nodes[0]; // DMA descriptor pool, the descriptors are shared by all i80 devices
};

struct lcd_i80_trans_descriptor_t {
    lcd_panel_io_i80_t *i80_device; // i80 device issuing this transaction
    int cmd_value;        // Command value
    uint32_t cmd_cycles;  // Command cycles
    const void *data;     // Data buffer
    uint32_t data_length; // Data buffer size
    void *cb_user_data;   // private data used by trans_done_cb
    bool (*trans_done_cb)(esp_lcd_panel_io_handle_t panel_io, void *user_data, void *event_data); // transaction done callback
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
    size_t num_trans_working;  // Number of transactions that are undergoing (the descriptor not recycled yet)
    void *cb_user_data;        // private data used when transfer color data
    bool (*on_color_trans_done)(esp_lcd_panel_io_handle_t panel_io, void *user_data, void *event_data); // color data trans done callback
    LIST_ENTRY(lcd_panel_io_i80_t) device_list_entry; // Entry of i80 device list
    struct {
        int dc_idle_level: 1;  // Level of DC line in IDLE phase
        int dc_cmd_level: 1;   // Level of DC line in CMD phase
        int dc_dummy_level: 1; // Level of DC line in DUMMY phase
        int dc_data_level: 1;  // Level of DC line in DATA phase
    } dc_levels;
    struct {
        int invert_cs: 1;          // Whether to invert the CS line
        int reverse_color_bits: 1; // Reverse the data bits, D[N:0] -> D[0:N]
        int swap_color_bytes: 1;   // Swap adjacent two data bytes before sending out
        int pclk_active_neg: 1;    // The display will write data lines when there's a falling edge on WR line
        int pclk_idle_low: 1;      // The WR line keeps at low level in IDLE phase
    } flags;
    lcd_i80_trans_descriptor_t trans_pool[0]; // Transaction pool
};

esp_err_t esp_lcd_new_i80_bus(const esp_lcd_i80_bus_config_t *bus_config, esp_lcd_i80_bus_handle_t *ret_bus)
{
    esp_err_t ret = ESP_OK;
    esp_lcd_i80_bus_t *bus = NULL;
    ESP_GOTO_ON_FALSE(bus_config && ret_bus, ESP_ERR_INVALID_ARG, err_arg, TAG, "invalid argument");
    size_t num_dma_nodes = bus_config->max_transfer_bytes / DMA_DESCRIPTOR_BUFFER_MAX_SIZE + 1;
    // DMA descriptors must be placed in internal SRAM
    bus = heap_caps_calloc(1, sizeof(esp_lcd_i80_bus_t) + num_dma_nodes * sizeof(dma_descriptor_t), MALLOC_CAP_DMA);
    ESP_GOTO_ON_FALSE(bus, ESP_ERR_NO_MEM, no_mem_bus, TAG, "no mem for i80 bus");
    bus->num_dma_nodes = num_dma_nodes;
    // register to platform
    int bus_id = lcd_com_register_device(LCD_COM_DEVICE_TYPE_I80, bus);
    ESP_GOTO_ON_FALSE(bus_id >= 0, ESP_ERR_NOT_FOUND, no_slot, TAG, "no free i80 bus slot");
    bus->bus_id = bus_id;
    // enable APB to access LCD registers
    periph_module_enable(lcd_periph_signals.buses[bus_id].module);
    // initialize HAL layer, so we can call LL APIs later
    lcd_hal_init(&bus->hal, bus_id);
    // reset peripheral and FIFO
    lcd_ll_reset(bus->hal.dev);
    lcd_ll_fifo_reset(bus->hal.dev);
    lcd_ll_enable_clock(bus->hal.dev, true);
    // install interrupt service, (LCD peripheral shares the same interrupt source with Camera peripheral with different mask)
    // interrupt is disabled by default
    int isr_flags = ESP_INTR_FLAG_INTRDISABLED;
    ret = esp_intr_alloc_intrstatus(lcd_periph_signals.buses[bus_id].irq_id, isr_flags,
                                    (uint32_t)lcd_ll_get_interrupt_status_reg(bus->hal.dev),
                                    LCD_LL_EVENT_TRANS_DONE, lcd_default_isr_handler, bus, &bus->intr);
    ESP_GOTO_ON_ERROR(ret, no_int, TAG, "install interrupt failed");
    lcd_ll_enable_interrupt(bus->hal.dev, LCD_LL_EVENT_TRANS_DONE, false); // disable all interrupts
    lcd_ll_clear_interrupt_status(bus->hal.dev, UINT32_MAX); // clear pending interrupt
    // install DMA service
    ret = lcd_i80_bus_create_trans_link(bus);
    ESP_GOTO_ON_ERROR(ret, no_dma, TAG, "install DMA failed");
    // set peripheral clock resolution
    bus->resolution_hz = lcd_com_select_periph_clock(&bus->hal);
    // enable 8080 mode and set data width
    lcd_ll_enable_rgb_mode(bus->hal.dev, false);
    lcd_ll_set_data_width(bus->hal.dev, bus_config->data_width);
    // number of data cycles is controlled by DMA buffer size
    lcd_ll_enable_output_always_on(bus->hal.dev, true);
    // enable trans done interrupt
    lcd_ll_enable_interrupt(bus->hal.dev, LCD_LL_EVENT_TRANS_DONE, true);
    // trigger a quick "trans done" event, and wait for the interrupt line goes active
    // this could ensure we go into ISR handler next time we call `esp_intr_enable`
    lcd_periph_trigger_quick_trans_done_event(bus);
    // configure GPIO
    ret = lcd_i80_bus_configure_gpio(bus, bus_config);
    ESP_GOTO_ON_ERROR(ret, no_gpio, TAG, "configure GPIO failed");
    // fill other i80 bus runtime parameters
    LIST_INIT(&bus->device_list); // initialize device list head
    bus->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    bus->data_width = lcd_ll_get_data_width(bus->hal.dev);
    *ret_bus = bus;
    ESP_LOGD(TAG, "new i80 bus(%d) @%p, %zu dma nodes", bus_id, bus, bus->num_dma_nodes);
    return ESP_OK;

no_gpio:
    gdma_disconnect(bus->dma_chan);
    gdma_del_channel(bus->dma_chan);
no_dma:
    esp_intr_free(bus->intr);
no_int:
    periph_module_disable(lcd_periph_signals.buses[bus_id].module);
    lcd_com_remove_device(LCD_COM_DEVICE_TYPE_I80, bus->bus_id);
no_slot:
    free(bus);
no_mem_bus:
err_arg:
    return ret;
}

esp_err_t esp_lcd_del_i80_bus(esp_lcd_i80_bus_handle_t bus)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(bus, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(LIST_EMPTY(&bus->device_list), ESP_ERR_INVALID_STATE, err, TAG, "device list not empty");
    int bus_id = bus->bus_id;
    gdma_disconnect(bus->dma_chan);
    gdma_del_channel(bus->dma_chan);
    esp_intr_free(bus->intr);
    periph_module_disable(lcd_periph_signals.buses[bus_id].module);
    lcd_com_remove_device(LCD_COM_DEVICE_TYPE_I80, bus_id);
    free(bus);
    ESP_LOGD(TAG, "del i80 bus(%d)", bus_id);
err:
    return ret;
}

esp_err_t esp_lcd_new_panel_io_i80(esp_lcd_i80_bus_handle_t bus, const esp_lcd_panel_io_i80_config_t *io_config, esp_lcd_panel_io_handle_t *ret_io)
{
    esp_err_t ret = ESP_OK;
    lcd_panel_io_i80_t *i80_device = NULL;
    ESP_GOTO_ON_FALSE(bus && io_config && ret_io, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    // check if pixel clock setting is valid
    uint32_t pclk_prescale = bus->resolution_hz / io_config->pclk_hz;
    ESP_GOTO_ON_FALSE(pclk_prescale <= LCD_LL_CLOCK_PRESCALE_MAX, ESP_ERR_NOT_SUPPORTED, err, TAG,
                      "prescaler can't satisfy PCLK clock %u", io_config->pclk_hz);
    i80_device = calloc(1, sizeof(lcd_panel_io_i80_t) + io_config->trans_queue_depth * sizeof(lcd_i80_trans_descriptor_t));
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
    i80_device->pclk_hz = bus->resolution_hz / pclk_prescale;
    i80_device->dc_levels.dc_cmd_level = io_config->dc_levels.dc_cmd_level;
    i80_device->dc_levels.dc_data_level = io_config->dc_levels.dc_data_level;
    i80_device->dc_levels.dc_dummy_level = io_config->dc_levels.dc_dummy_level;
    i80_device->dc_levels.dc_idle_level = io_config->dc_levels.dc_idle_level;
    i80_device->cs_gpio_num = io_config->cs_gpio_num;
    i80_device->flags.reverse_color_bits = io_config->flags.reverse_color_bits;
    i80_device->flags.swap_color_bytes = io_config->flags.swap_color_bytes;
    i80_device->flags.invert_cs = io_config->flags.invert_cs;
    i80_device->flags.pclk_idle_low = io_config->flags.pclk_idle_low;
    i80_device->flags.pclk_active_neg = io_config->flags.pclk_active_neg;
    i80_device->on_color_trans_done = io_config->on_color_trans_done;
    i80_device->cb_user_data = io_config->user_data;
    // fill panel io function table
    i80_device->base.del = panel_io_i80_del;
    i80_device->base.tx_param = panel_io_i80_tx_param;
    i80_device->base.tx_color = panel_io_i80_tx_color;
    // we only configure the CS GPIO as output, don't connect to the peripheral signal at the moment
    // we will connect the CS GPIO to peripheral signal when switching devices in lcd_i80_switch_devices()
    gpio_set_level(io_config->cs_gpio_num, !io_config->flags.invert_cs);
    gpio_set_direction(io_config->cs_gpio_num, GPIO_MODE_OUTPUT);
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[io_config->cs_gpio_num], PIN_FUNC_GPIO);
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
    for (size_t i = 0; i < i80_device->num_trans_working; i++) {
        xQueueReceive(i80_device->done_queue, &trans_desc, portMAX_DELAY);
    }
    // remove from device list
    portENTER_CRITICAL(&bus->spinlock);
    LIST_REMOVE(i80_device, device_list_entry);
    portEXIT_CRITICAL(&bus->spinlock);

    ESP_LOGD(TAG, "del i80 lcd panel io @%p", i80_device);
    vQueueDelete(i80_device->trans_queue);
    vQueueDelete(i80_device->done_queue);
    free(i80_device);
    return ESP_OK;
}

static esp_err_t panel_io_i80_tx_param(esp_lcd_panel_io_t *io, int lcd_cmd, int lcd_cmd_bits, const void *param, size_t param_size)
{
    lcd_panel_io_i80_t *next_device = __containerof(io, lcd_panel_io_i80_t, base);
    esp_lcd_i80_bus_t *bus = next_device->bus;
    lcd_panel_io_i80_t *cur_device = bus->cur_device;
    lcd_i80_trans_descriptor_t *trans_desc = NULL;
    assert(param_size <= (bus->num_dma_nodes * DMA_DESCRIPTOR_BUFFER_MAX_SIZE) && "parameter bytes too long, enlarge max_transfer_bytes");
    uint32_t cmd_cycles = lcd_cmd_bits / bus->data_width;
    // in case data_width=16 and cmd_bits=8, we still need 1 cmd_cycle
    if (cmd_cycles * bus->data_width < lcd_cmd_bits) {
        cmd_cycles++;
    }
    // wait all pending transaction in the queue to finish
    for (size_t i = 0; i < next_device->num_trans_working; i++) {
        xQueueReceive(next_device->done_queue, &trans_desc, portMAX_DELAY);
    }
    next_device->num_trans_working = 0;

    uint32_t intr_status = lcd_ll_get_interrupt_status(bus->hal.dev);
    lcd_ll_clear_interrupt_status(bus->hal.dev, intr_status);
    // switch devices if necessary
    lcd_i80_switch_devices(cur_device, next_device);
    // don't reverse bit/bytes for parameters
    lcd_ll_reverse_data_bit_order(bus->hal.dev, false);
    lcd_ll_reverse_data_byte_order(bus->hal.dev, bus->data_width, false);
    bus->cur_trans = NULL;
    bus->cur_device = next_device;
    // package a transaction
    trans_desc = &next_device->trans_pool[0];
    trans_desc->i80_device = next_device;
    trans_desc->cmd_cycles = cmd_cycles;
    trans_desc->cmd_value = lcd_cmd;
    trans_desc->data = param;
    trans_desc->data_length = param_size;
    trans_desc->trans_done_cb = NULL; // no callback for parameter transaction
    // mount data to DMA links
    lcd_com_mount_dma_data(bus->dma_nodes, trans_desc->data, trans_desc->data_length);
    lcd_start_transaction(bus, trans_desc);
    // polling the trans done event, but don't clear the event status
    while (!(lcd_ll_get_interrupt_status(bus->hal.dev) & LCD_LL_EVENT_TRANS_DONE));
    return ESP_OK;
}

static esp_err_t panel_io_i80_tx_color(esp_lcd_panel_io_t *io, int lcd_cmd, int lcd_cmd_bits, const void *color, size_t color_size)
{
    lcd_panel_io_i80_t *i80_device = __containerof(io, lcd_panel_io_i80_t, base);
    esp_lcd_i80_bus_t *bus = i80_device->bus;
    lcd_i80_trans_descriptor_t *trans_desc = NULL;
    assert(color_size <= (bus->num_dma_nodes * DMA_DESCRIPTOR_BUFFER_MAX_SIZE) && "color bytes too long, enlarge max_transfer_bytes");
    // in case data_width=16 and cmd_bits=8, we still need 1 cmd_cycle
    uint32_t cmd_cycles = lcd_cmd_bits / bus->data_width;
    if (cmd_cycles * bus->data_width < lcd_cmd_bits) {
        cmd_cycles++;
    }
    if (i80_device->num_trans_working < i80_device->queue_size) {
        trans_desc = &i80_device->trans_pool[i80_device->num_trans_working];
    } else {
        // transaction pool has used up, recycle one from done_queue
        xQueueReceive(i80_device->done_queue, &trans_desc, portMAX_DELAY);
        i80_device->num_trans_working--;
    }
    trans_desc->i80_device = i80_device;
    trans_desc->cmd_cycles = cmd_cycles;
    trans_desc->cmd_value = lcd_cmd;
    trans_desc->data = color;
    trans_desc->data_length = color_size;
    trans_desc->trans_done_cb = i80_device->on_color_trans_done;
    trans_desc->cb_user_data = i80_device->cb_user_data;
    // send transaction to trans_queue
    xQueueSend(i80_device->trans_queue, &trans_desc, portMAX_DELAY);
    i80_device->num_trans_working++;
    // enable interrupt and go into isr handler, where we fetch the transactions from trans_queue and start it
    // we will go into `lcd_default_isr_handler` almost at once, because the "trans done" event is active at the moment
    esp_intr_enable(bus->intr);
    return ESP_OK;
}

static esp_err_t lcd_i80_bus_create_trans_link(esp_lcd_i80_bus_handle_t bus)
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
    ret = gdma_new_channel(&dma_chan_config, &bus->dma_chan);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "alloc DMA channel failed");
    gdma_connect(bus->dma_chan, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_LCD, 0));
    gdma_strategy_config_t strategy_config = {
        .auto_update_desc = true,
        .owner_check = true
    };
    gdma_apply_strategy(bus->dma_chan, &strategy_config);
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
    for (size_t i = 0; i < bus_config->data_width; i++) {
        valid_gpio = valid_gpio && (bus_config->data_gpio_nums[i] >= 0);
    }
    if (!valid_gpio) {
        return ESP_ERR_INVALID_ARG;
    }
    // connect peripheral signals via GPIO matrix
    for (size_t i = 0; i < bus_config->data_width; i++) {
        gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[bus_config->data_gpio_nums[i]], PIN_FUNC_GPIO);
        gpio_set_direction(bus_config->data_gpio_nums[i], GPIO_MODE_OUTPUT);
        esp_rom_gpio_connect_out_signal(bus_config->data_gpio_nums[i], lcd_periph_signals.buses[bus_id].data_sigs[i], false, false);
    }
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[bus_config->dc_gpio_num], PIN_FUNC_GPIO);
    gpio_set_direction(bus_config->dc_gpio_num, GPIO_MODE_OUTPUT);
    esp_rom_gpio_connect_out_signal(bus_config->dc_gpio_num, lcd_periph_signals.buses[bus_id].dc_sig, false, false);
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[bus_config->wr_gpio_num], PIN_FUNC_GPIO);
    gpio_set_direction(bus_config->wr_gpio_num, GPIO_MODE_OUTPUT);
    esp_rom_gpio_connect_out_signal(bus_config->wr_gpio_num, lcd_periph_signals.buses[bus_id].wr_sig, false, false);
    return ESP_OK;
}

static void lcd_periph_trigger_quick_trans_done_event(esp_lcd_i80_bus_handle_t bus)
{
    // trigger a quick interrupt event by a dummy transaction, wait the LCD interrupt line goes active
    // next time when esp_intr_enable is invoked, we can go into interrupt handler immediately
    // where we dispatch transactions for i80 devices
    lcd_ll_set_phase_cycles(bus->hal.dev, 0, 1, 0);
    lcd_ll_start(bus->hal.dev);
    while (!(lcd_ll_get_interrupt_status(bus->hal.dev) & LCD_LL_EVENT_TRANS_DONE));
}

static void lcd_start_transaction(esp_lcd_i80_bus_t *bus, lcd_i80_trans_descriptor_t *trans_desc)
{
    // by default, the dummy phase is disabled because it's not common for most LCDs
    // Number of data phase cycles are controlled by DMA buffer length, we only need to enable/disable the phase here
    lcd_ll_set_phase_cycles(bus->hal.dev, trans_desc->cmd_cycles, 0, trans_desc->data ? 1 : 0);
    lcd_ll_set_command(bus->hal.dev, bus->data_width, trans_desc->cmd_value);
    if (trans_desc->data) { // some specific LCD commands can have no parameters
        gdma_start(bus->dma_chan, (intptr_t)(bus->dma_nodes));
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
        if (cur_device) {
            // disconnect current CS GPIO from peripheral signal
            esp_rom_gpio_connect_out_signal(cur_device->cs_gpio_num, SIG_GPIO_OUT_IDX, false, false);
        }
        // connect CS signal to the new device
        esp_rom_gpio_connect_out_signal(next_device->cs_gpio_num, lcd_periph_signals.buses[bus->bus_id].cs_sig,
                                        next_device->flags.invert_cs, false);
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
            // device callback
            if (trans_desc->trans_done_cb) {
                if (trans_desc->trans_done_cb(&cur_device->base, trans_desc->cb_user_data, NULL)) {
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
                // only clear the interrupt status when we're sure there still remains transaction to handle
                lcd_ll_clear_interrupt_status(bus->hal.dev, intr_status);
                // switch devices if necessary
                lcd_i80_switch_devices(cur_device, next_device);
                // only reverse data bit/bytes for color data
                lcd_ll_reverse_data_bit_order(bus->hal.dev, next_device->flags.reverse_color_bits);
                lcd_ll_reverse_data_byte_order(bus->hal.dev, bus->data_width, next_device->flags.swap_color_bytes);
                bus->cur_trans = trans_desc;
                bus->cur_device = next_device;
                // mount data to DMA links
                lcd_com_mount_dma_data(bus->dma_nodes, trans_desc->data, trans_desc->data_length);
                // enable interrupt again, because the new transaction can trigger new trans done event
                esp_intr_enable(bus->intr);
                lcd_start_transaction(bus, trans_desc);
                break; // exit for-each loop
            }
        }
    }
    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}

#endif // SOC_LCDCAM_SUPPORTED
