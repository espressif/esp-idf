/*
 * SPDX-FileCopyrightText: 2017-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_cpu.h"
#include "esp_attr.h"
#include "hal/uart_hal.h"
#include "hal/gpio_hal.h"
#include "driver/uart.h"
#include "hal/uart_periph.h"
#include "esp_clk_tree.h"
#include "soc/gpio_periph.h"
#include "esp_rom_gpio.h"
#include "hal/uart_ll.h"
#include "esp_intr_alloc.h"
#include "esp_heap_caps.h"
#include "esp_private/esp_clk_tree_common.h"
#include "esp_private/esp_gpio_reserve.h"
#include "esp_private/periph_ctrl.h"

#include "esp_app_trace_port.h"
#include "esp_app_trace_util.h"
#include "esp_app_trace_types.h"

static const char *TAG = "esp_apptrace_uart";

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

typedef struct {
    uint8_t *buffer;            ///< Ring buffer data
    uint32_t max_size;          ///< Ring buffer maximum size (must be power of 2)
    volatile uint32_t count;    ///< Number of bytes currently in the buffer
    volatile uint32_t head;     ///< Write pointer index
    volatile uint32_t tail;     ///< Read pointer index
} esp_apptrace_uart_rb_t;

typedef struct {
    int inited;
    volatile bool tx_busy;              ///< TX busy flag
    uart_hal_context_t hal_ctx;         ///< UART HAL context
    esp_apptrace_uart_rb_t tx_ring;     ///< TX ring buffer
    intr_handle_t intr_handle;          ///< Interrupt handle

    /* TX message buffer */
    uint8_t *tx_msg_buff;               ///< TX message buffer to provide with get_up_buffer
    uint32_t tx_msg_buff_size;          ///< TX message buffer size & maximum size of the single message to transfer.
    uint32_t tx_pending_msg_size;       ///< Pending message size to send with put_up_buffer

    /* RX message buffer */
    uint8_t *rx_msg_buff;               ///< RX message buffer provided with down_buffer_config function
    uint32_t rx_msg_buff_size;          ///< RX message buffer size provided with down_buffer_config function

#if CONFIG_APPTRACE_LOCK_ENABLE
    esp_apptrace_lock_t lock;           ///< Sync lock
#endif

} esp_apptrace_uart_data_t;

static inline bool is_power_of_two(uint32_t n)
{
    return n != 0 && (n & (n - 1)) == 0;
}

static inline uint32_t ring_buffer_mask(const esp_apptrace_uart_rb_t *rb)
{
    return rb->max_size - 1;
}

/* Get the length of the data in the ring buffer */
static inline uint32_t ring_buffer_data_len(const esp_apptrace_uart_rb_t *rb)
{
    return rb->count;
}

/* Get the length of the free space in the ring buffer */
static inline uint32_t ring_buffer_free_len(const esp_apptrace_uart_rb_t *rb)
{
    return rb->max_size - rb->count;
}

static inline void ring_buffer_advance_tail(esp_apptrace_uart_rb_t *rb, uint32_t count)
{
    rb->tail = (rb->tail + count) & ring_buffer_mask(rb);
    rb->count -= count;
}

static inline void ring_buffer_advance_head(esp_apptrace_uart_rb_t *rb, uint32_t count)
{
    rb->head = (rb->head + count) & ring_buffer_mask(rb);
    rb->count += count;
}

static inline uint32_t ring_buffer_calc_to_send(const esp_apptrace_uart_rb_t *rb, uint32_t tx_msg_size)
{
    uint32_t used = ring_buffer_data_len(rb);
    if (used == 0) {
        return 0;
    }

    uint32_t cont = rb->max_size - rb->tail;
    uint32_t n = MIN(used, cont);

    /* Apply message size limit if specified */
    if (tx_msg_size && tx_msg_size < n) {
        return tx_msg_size;
    }
    return n;
}

static esp_err_t esp_apptrace_uart_lock(void *hw_data, esp_apptrace_tmo_t *tmo)
{
#if CONFIG_APPTRACE_LOCK_ENABLE
    esp_apptrace_uart_data_t *uart_data = hw_data;
    esp_err_t ret = esp_apptrace_lock_take(&uart_data->lock, tmo);
    if (ret != ESP_OK) {
        return ESP_FAIL;
    }
#endif
    return ESP_OK;
}

static esp_err_t esp_apptrace_uart_unlock(void *hw_data)
{
    esp_err_t ret = ESP_OK;
#if CONFIG_APPTRACE_LOCK_ENABLE
    esp_apptrace_uart_data_t *uart_data = hw_data;
    ret = esp_apptrace_lock_give(&uart_data->lock);
    assert(ret == ESP_OK && "Failed to unlock apptrace uart lock!");
#endif
    return ret;
}

static esp_err_t ring_buffer_put(esp_apptrace_uart_rb_t *rb, const uint8_t *data, uint32_t len)
{
    /* Drop oldest. Make available space if needed */
    uint32_t free_len = ring_buffer_free_len(rb);
    if (len > free_len) {
        uint32_t need = len - free_len;
        ring_buffer_advance_tail(rb, need);
    }

    uint32_t head = rb->head;
    uint32_t space_to_end = rb->max_size - head;

    if (len <= space_to_end) {
        memcpy(&rb->buffer[head], data, len);
    } else {
        memcpy(&rb->buffer[head], data, space_to_end);
        memcpy(&rb->buffer[0], &data[space_to_end], len - space_to_end);
    }

    ring_buffer_advance_head(rb, len);

    return ESP_OK;
}

static esp_err_t ring_buffer_init(esp_apptrace_uart_rb_t *rb, uint32_t size)
{
    rb->buffer = heap_caps_malloc(size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    if (!rb->buffer) {
        return ESP_ERR_NO_MEM;
    }

    rb->max_size = size;
    rb->count = 0;
    rb->head = 0;
    rb->tail = 0;

    return ESP_OK;
}

static void IRAM_ATTR esp_apptrace_uart_isr_handler(void *arg)
{
    esp_apptrace_uart_data_t *uart_data = arg;
    esp_apptrace_uart_rb_t *rb = &uart_data->tx_ring;

    uint32_t intr_status = uart_hal_get_intsts_mask(&uart_data->hal_ctx);

    if (intr_status & UART_INTR_TXFIFO_EMPTY) {
        uart_hal_clr_intsts_mask(&uart_data->hal_ctx, UART_INTR_TXFIFO_EMPTY);

        uint32_t to_send = ring_buffer_calc_to_send(rb, uart_data->tx_msg_buff_size);
        if (to_send > 0) {
            uint32_t written = 0;
            uart_hal_write_txfifo(&uart_data->hal_ctx, &rb->buffer[rb->tail], to_send, &written);
            ring_buffer_advance_tail(rb, written);
        }

        /* If ring buffer is empty, disable TX interrupt */
        if (ring_buffer_data_len(rb) == 0) {
            uart_ll_disable_intr_mask(uart_data->hal_ctx.dev, UART_INTR_TXFIFO_EMPTY);
            uart_data->tx_busy = false;
        }
    }
}

static esp_err_t esp_apptrace_uart_init(void *hw_data, const esp_apptrace_config_t *config)
{
    esp_err_t ret = ESP_ERR_INVALID_ARG;
    uint64_t gpio_mask = 0;
    esp_apptrace_uart_data_t *uart_data = hw_data;
    const esp_apptrace_uart_config_t *uart_config = &config->dest_cfg.uart;

    /* Init function is called on every core, so ensure to do main setup only once */
    int core_id = esp_cpu_get_core_id();
    if (core_id == 0) {

        if (uart_config->uart_num == CONFIG_ESP_CONSOLE_UART_NUM) {
            ESP_APPTRACE_LOGE("Application trace UART and console UART cannot use the same port number");
            return ESP_ERR_INVALID_ARG;
        }

        if (uart_config->uart_num >= SOC_UART_HP_NUM) {
            ESP_APPTRACE_LOGE("UART port number %d is not supported!", uart_config->uart_num);
            return ESP_ERR_NOT_SUPPORTED;
        }

        if (GPIO_IS_VALID_GPIO(uart_config->tx_pin_num)) {
            gpio_mask |= BIT64(uart_config->tx_pin_num);
        }
        if (GPIO_IS_VALID_GPIO(uart_config->rx_pin_num)) {
            gpio_mask |= BIT64(uart_config->rx_pin_num);
        }

        if (gpio_mask == 0) {
            ESP_LOGE(TAG, "No valid GPIOs to reserve");
            return ESP_ERR_INVALID_STATE;
        }

        uint64_t r = esp_gpio_reserve(gpio_mask);
        if (r & gpio_mask) {
            ESP_LOGE(TAG, "GPIO(s) are already reserved: 0x%"PRIx64, r & gpio_mask);
            return ESP_ERR_INVALID_STATE;
        }

        uart_data->hal_ctx.dev = UART_LL_GET_HW(uart_config->uart_num);

        PERIPH_RCC_ATOMIC() {
            uart_ll_enable_bus_clock(uart_config->uart_num, true);
            uart_ll_reset_register(uart_config->uart_num);
        }
        PERIPH_RCC_ATOMIC() {
            uart_ll_sclk_enable(uart_data->hal_ctx.dev);
        }

        uint32_t sclk_hz;
        esp_clk_tree_src_get_freq_hz(UART_SCLK_DEFAULT, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &sclk_hz);
        /* Enable the default clock source */
        esp_clk_tree_enable_src(UART_SCLK_DEFAULT, true);

        /* Initialize UART HAL (sets default 8N1 mode) */
        uart_hal_init(&uart_data->hal_ctx, uart_config->uart_num);

        PERIPH_RCC_ATOMIC() {
            uart_hal_set_sclk(&uart_data->hal_ctx, UART_SCLK_DEFAULT);
            uart_hal_set_baudrate(&uart_data->hal_ctx, uart_config->baud_rate, sclk_hz);
        }

        /* Configure FIFO thresholds */
        uart_hal_set_txfifo_empty_thr(&uart_data->hal_ctx, 16); /* Slow down IRQ rate */
        uart_hal_set_rxfifo_full_thr(&uart_data->hal_ctx, 1);

        /* Initialize TX ring buffer */
        if (uart_config->tx_buff_size == 0 || !is_power_of_two(uart_config->tx_buff_size)) {
            ESP_APPTRACE_LOGE("TX ring buffer size (%u) must be a power of two and greater than 0",
                              uart_config->tx_buff_size);
            goto err_init_ring_buff;
        }
        ret = ring_buffer_init(&uart_data->tx_ring, uart_config->tx_buff_size);
        if (ret != ESP_OK) {
            ESP_APPTRACE_LOGE("Failed to initialize TX ring buffer");
            goto err_init_ring_buff;
        }

        /* Initialize TX message buffer for providing with get_up_buffer */
        uart_data->tx_msg_buff_size = uart_config->tx_msg_size;
        uart_data->tx_msg_buff = heap_caps_malloc(uart_data->tx_msg_buff_size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        if (uart_data->tx_msg_buff == NULL) {
            ESP_APPTRACE_LOGE("Failed to initialize TX message buffer");
            ret = ESP_ERR_NO_MEM;
            goto err_alloc_msg_buff;
        }

        /* Disable all interrupts and clear status */
        uart_ll_disable_intr_mask(uart_data->hal_ctx.dev, UART_LL_INTR_MASK);
        uart_ll_clr_intsts_mask(uart_data->hal_ctx.dev, UART_LL_INTR_MASK);

        /* Install interrupt handler */
        int intr_alloc_flags = 0;
        ret = esp_intr_alloc(uart_periph_signal[uart_config->uart_num].irq, intr_alloc_flags,
                             esp_apptrace_uart_isr_handler, uart_data, &uart_data->intr_handle);
        if (ret != ESP_OK) {
            ESP_APPTRACE_LOGE("Failed to allocate interrupt: %s", esp_err_to_name(ret));
            goto err_alloc_intr;
        }

        /* Reset FIFOs */
        uart_hal_rxfifo_rst(&uart_data->hal_ctx);
        uart_hal_txfifo_rst(&uart_data->hal_ctx);

        /* Configure GPIO pins for RX and TX */
        const uint32_t tx_idx = UART_PERIPH_SIGNAL(uart_config->uart_num, SOC_UART_PERIPH_SIGNAL_TX);
        const uint32_t rx_idx = UART_PERIPH_SIGNAL(uart_config->uart_num, SOC_UART_PERIPH_SIGNAL_RX);

        /* Configure TX pin */
        gpio_ll_func_sel(&GPIO, uart_config->tx_pin_num, PIN_FUNC_GPIO);
        esp_rom_gpio_pad_pullup_only(uart_config->tx_pin_num);
        esp_rom_gpio_connect_out_signal(uart_config->tx_pin_num, tx_idx, 0, 0);

        /* Configure RX pin */
        gpio_ll_input_enable(&GPIO, uart_config->rx_pin_num);
        esp_rom_gpio_pad_pullup_only(uart_config->rx_pin_num);
        esp_rom_gpio_connect_in_signal(uart_config->rx_pin_num, rx_idx, 0);

#if CONFIG_APPTRACE_LOCK_ENABLE
        esp_apptrace_lock_init(&uart_data->lock);
#endif

    }

    uart_data->inited |= 1 << core_id;
    uart_data->tx_busy = false;

    return ESP_OK;

err_alloc_intr:
    heap_caps_free(uart_data->tx_msg_buff);
err_alloc_msg_buff:
    heap_caps_free(uart_data->tx_ring.buffer);
err_init_ring_buff:
    esp_clk_tree_enable_src(UART_SCLK_DEFAULT, false);
    PERIPH_RCC_ATOMIC() {
        uart_ll_sclk_disable(uart_data->hal_ctx.dev);
    }
    PERIPH_RCC_ATOMIC() {
        uart_ll_enable_bus_clock(uart_config->uart_num, false);
    }
    esp_gpio_revoke(gpio_mask);
    return ret;
}

static uint8_t *esp_apptrace_uart_up_buffer_get(void *hw_data, uint32_t size, esp_apptrace_tmo_t *tmo)
{
    esp_apptrace_uart_data_t *uart_data = hw_data;

    if (size == 0 || size > uart_data->tx_msg_buff_size) {
        return NULL;
    }

    if (esp_apptrace_uart_lock(uart_data, tmo) != ESP_OK) {
        return NULL;
    }

    if (uart_data->tx_pending_msg_size != 0) {
        // A previous message was not sent.
        esp_apptrace_uart_unlock(uart_data);
        return NULL;
    }

    uart_data->tx_pending_msg_size = size;

    esp_apptrace_uart_unlock(uart_data);

    return uart_data->tx_msg_buff;
}

static esp_err_t esp_apptrace_uart_up_buffer_put(void *hw_data, uint8_t *ptr, esp_apptrace_tmo_t *tmo)
{
    esp_apptrace_uart_data_t *uart_data = hw_data;
    esp_apptrace_uart_rb_t *rb = &uart_data->tx_ring;

    esp_err_t res = esp_apptrace_uart_lock(uart_data, tmo);
    if (res != ESP_OK) {
        return res;
    }

    /* Add data to ring buffer */
    ring_buffer_put(rb, ptr, uart_data->tx_pending_msg_size);
    uart_data->tx_pending_msg_size = 0;

    esp_apptrace_uart_unlock(uart_data);

    // Trigger transmission if not already in progress
    if (!uart_data->tx_busy) {
        uart_data->tx_busy = true;
        /* Enable TX interrupt */
        uart_ll_clr_intsts_mask(uart_data->hal_ctx.dev, UART_INTR_TXFIFO_EMPTY);
        uart_ll_ena_intr_mask(uart_data->hal_ctx.dev, UART_INTR_TXFIFO_EMPTY);
    }

    return ESP_OK;
}

static void esp_apptrace_uart_down_buffer_config(void *hw_data, uint8_t *buf, uint32_t size)
{
    esp_apptrace_uart_data_t *uart_data = hw_data;

    assert(buf != NULL && "Down buffer cannot be NULL");
    assert(size > 0 && "Down buffer size must be greater than 0");

    uart_data->rx_msg_buff = buf;
    uart_data->rx_msg_buff_size = size;
}

static uint8_t *esp_apptrace_uart_down_buffer_get(void *hw_data, uint32_t *size, esp_apptrace_tmo_t *tmo)
{
    esp_apptrace_uart_data_t *uart_data = hw_data;
    if (!size || *size == 0) {
        return NULL;
    }

    if (!uart_data->rx_msg_buff) {
        ESP_APPTRACE_LOGE("RX message buffer is not configured. Call down_buffer_config() first.");
        return NULL;
    }

    if (esp_apptrace_uart_lock(uart_data, tmo) != ESP_OK) {
        return NULL;
    }

    /* Read until we get the requested number of bytes (or timeout) */
    const uint32_t req_size = MIN(uart_data->rx_msg_buff_size, *size);
    uint32_t total_read = 0;
    while (total_read < req_size) {
        uint32_t rx_len = uart_hal_get_rxfifo_len(&uart_data->hal_ctx);
        int to_read = MIN((uint32_t)(req_size - total_read), rx_len);
        if (to_read > 0) {
            uart_hal_read_rxfifo(&uart_data->hal_ctx, uart_data->rx_msg_buff + total_read, &to_read);
            total_read += to_read;
            continue;
        }

        if (esp_apptrace_tmo_check(tmo) != ESP_OK) {
            break;
        }
        esp_rom_delay_us(50);
    }

    *size = total_read;
    esp_apptrace_uart_unlock(uart_data);

    return (*size > 0) ? uart_data->rx_msg_buff : NULL;
}

static esp_err_t esp_apptrace_uart_down_buffer_put(void *hw_data, uint8_t *ptr, esp_apptrace_tmo_t *tmo)
{
    (void)hw_data;
    (void)ptr;
    (void)tmo;

    /* No action needed - data was already read in get function */
    return ESP_OK;
}

static bool esp_apptrace_uart_host_is_connected(void *hw_data)
{
    esp_apptrace_uart_data_t *uart_data = hw_data;

    return uart_data->inited & 1;
}

static esp_err_t esp_apptrace_uart_flush_nolock(void *hw_data, uint32_t min_sz, esp_apptrace_tmo_t *tmo)
{
    esp_apptrace_uart_data_t *uart_data = hw_data;
    esp_apptrace_uart_rb_t *rb = &uart_data->tx_ring;

    uint32_t pending = ring_buffer_data_len(rb);
    if (pending < min_sz) {
        ESP_APPTRACE_LOGD("Ignore UART flush request for min %" PRIu32 " bytes. Pending bytes: %" PRIu32, min_sz, pending);
        return ESP_OK;
    }

    /* Trigger transmission if there's data but not busy */
    if (pending > 0 && !uart_data->tx_busy) {
        uart_data->tx_busy = true;
        uart_ll_clr_intsts_mask(uart_data->hal_ctx.dev, UART_INTR_TXFIFO_EMPTY);
        uart_ll_ena_intr_mask(uart_data->hal_ctx.dev, UART_INTR_TXFIFO_EMPTY);
    }

    while (uart_data->tx_busy || ring_buffer_data_len(rb) > 0) {
        if (esp_apptrace_tmo_check(tmo) != ESP_OK) {
            return ESP_ERR_TIMEOUT;
        }
        esp_rom_delay_us(100);
    }

    return ESP_OK;
}

static esp_err_t esp_apptrace_uart_flush(void *hw_data, esp_apptrace_tmo_t *tmo)
{
    esp_apptrace_uart_data_t *uart_data = hw_data;
    esp_err_t res = esp_apptrace_uart_lock(uart_data, tmo);
    if (res != ESP_OK) {
        return res;
    }

    esp_err_t ret = esp_apptrace_uart_flush_nolock(hw_data, 0, tmo);

    esp_apptrace_uart_unlock(uart_data);

    return ret;
}

esp_apptrace_hw_t *esp_apptrace_uart_hw_get(int num, void **data)
{
    ESP_APPTRACE_LOGD("esp_apptrace_uart_hw_get - %i", num);

    static esp_apptrace_uart_data_t s_uart_hw_data;
    static esp_apptrace_hw_t s_uart_hw = {
        .init = esp_apptrace_uart_init,
        .get_up_buffer = esp_apptrace_uart_up_buffer_get,
        .put_up_buffer = esp_apptrace_uart_up_buffer_put,
        .flush_up_buffer_nolock = esp_apptrace_uart_flush_nolock,
        .flush_up_buffer = esp_apptrace_uart_flush,
        .down_buffer_config = esp_apptrace_uart_down_buffer_config,
        .get_down_buffer = esp_apptrace_uart_down_buffer_get,
        .put_down_buffer = esp_apptrace_uart_down_buffer_put,
        .host_is_connected = esp_apptrace_uart_host_is_connected,
    };
    *data = &s_uart_hw_data;
    return &s_uart_hw;
}
