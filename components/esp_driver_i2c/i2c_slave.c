/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <sys/param.h>
#include "esp_types.h"
#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"
#include "freertos/queue.h"
#include "hal/i2c_hal.h"
#include "soc/i2c_periph.h"
#include "esp_rom_gpio.h"
#include "driver/gpio.h"
#include "hal/gpio_ll.h"
#include "clk_ctrl_os.h"
#include "esp_private/esp_clk.h"
#include "driver/i2c_slave.h"
#include "i2c_private.h"
#include "esp_memory_utils.h"
#include "freertos/idf_additions.h"

#if CONFIG_I2C_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "esp_log.h"
#include "esp_check.h"

#define I2C_SLAVE_TIMEOUT_DEFAULT           (32000)     /* I2C slave timeout value, APB clock cycle number */
#define I2C_FIFO_EMPTY_THRESH_VAL_DEFAULT   (SOC_I2C_FIFO_LEN/2)
#define I2C_FIFO_FULL_THRESH_VAL_DEFAULT    (SOC_I2C_FIFO_LEN/2)

static const char *TAG = "i2c.slave";

static esp_err_t i2c_slave_bus_destroy(i2c_slave_dev_handle_t i2c_slave);

#if SOC_I2C_SLAVE_CAN_GET_STRETCH_CAUSE
static IRAM_ATTR void s_i2c_handle_clock_stretch(i2c_slave_dev_handle_t i2c_slave)
{
    i2c_hal_context_t *hal = &i2c_slave->base->hal;
    if (i2c_slave->callbacks.on_stretch_occur) {
        i2c_slave_stretch_event_data_t evt = { 0 };
        i2c_hal_context_t *hal = &i2c_slave->base->hal;
        i2c_ll_slave_get_stretch_cause(hal->dev, &evt.stretch_cause);
        i2c_slave->callbacks.on_stretch_occur(i2c_slave, &evt, i2c_slave->user_ctx);
    }
    i2c_ll_slave_clear_stretch(hal->dev);
}
#endif

static IRAM_ATTR void s_i2c_handle_rx_fifo_wm(i2c_slave_dev_handle_t i2c_slave, i2c_slave_receive_t *t)
{
    i2c_hal_context_t *hal = &i2c_slave->base->hal;
    uint32_t rx_fifo_cnt;
    i2c_ll_get_rxfifo_cnt(hal->dev, &rx_fifo_cnt);
    uint32_t fifo_cnt_rd = MIN(t->rcv_fifo_cnt, rx_fifo_cnt);
    i2c_ll_read_rxfifo(hal->dev, i2c_slave->data_buf, fifo_cnt_rd);
    memcpy(t->buffer + i2c_slave->already_receive_len, i2c_slave->data_buf, fifo_cnt_rd);
    i2c_slave->already_receive_len += fifo_cnt_rd;
    t->rcv_fifo_cnt -= fifo_cnt_rd;
}

static IRAM_ATTR void s_i2c_handle_complete(i2c_slave_dev_handle_t i2c_slave, i2c_slave_receive_t *t, BaseType_t *do_yield)
{
    i2c_hal_context_t *hal = &i2c_slave->base->hal;
    uint32_t rx_fifo_cnt;
    i2c_ll_get_rxfifo_cnt(hal->dev, &rx_fifo_cnt);
    if (rx_fifo_cnt != 0) {
        i2c_ll_read_rxfifo(hal->dev, i2c_slave->data_buf, t->rcv_fifo_cnt);
        memcpy(t->buffer + i2c_slave->already_receive_len, i2c_slave->data_buf, t->rcv_fifo_cnt);
        i2c_slave->already_receive_len += t->rcv_fifo_cnt;
        t->rcv_fifo_cnt -= t->rcv_fifo_cnt;
    }
    if (i2c_slave->callbacks.on_recv_done) {

        i2c_slave_rx_done_event_data_t edata = {
            .buffer = t->buffer,
        };
        i2c_slave->callbacks.on_recv_done(i2c_slave, &edata, i2c_slave->user_ctx);
        xSemaphoreGiveFromISR(i2c_slave->slv_rx_mux, do_yield);
        i2c_ll_disable_intr_mask(hal->dev, I2C_LL_SLAVE_RX_EVENT_INTR);
    }
}

static IRAM_ATTR void s_i2c_handle_tx_fifo_wm(i2c_slave_dev_handle_t i2c_slave, BaseType_t *do_yield)
{
    i2c_hal_context_t *hal = &i2c_slave->base->hal;
    uint32_t tx_fifo_rem;
    i2c_ll_get_txfifo_len(hal->dev, &tx_fifo_rem);
    size_t size = 0;
    uint8_t *data = (uint8_t *) xRingbufferReceiveUpToFromISR(i2c_slave->tx_ring_buf, &size, tx_fifo_rem);
    if (data) {
        i2c_ll_write_txfifo(hal->dev, data, size);
        vRingbufferReturnItemFromISR(i2c_slave->tx_ring_buf, data, do_yield);
    }
    if (size <= i2c_slave->trans_data_length) {
        portENTER_CRITICAL_ISR(&i2c_slave->base->spinlock);
        i2c_slave->trans_data_length -= size;
        portEXIT_CRITICAL_ISR(&i2c_slave->base->spinlock);
        if (i2c_slave->trans_data_length == 0) {
            i2c_ll_slave_disable_tx_it(hal->dev);
        }
    } else {
        ESP_DRAM_LOGE(TAG, "I2C TX BUFFER SIZE ERROR");
    }
}

static IRAM_ATTR void s_slave_fifo_isr_handler(uint32_t int_mask, void *arg, BaseType_t *do_yield)
{
    i2c_slave_dev_handle_t i2c_slave = (i2c_slave_dev_handle_t) arg;
#if SOC_I2C_SLAVE_CAN_GET_STRETCH_CAUSE
    if (int_mask & I2C_INTR_STRETCH) {
        s_i2c_handle_clock_stretch(i2c_slave);
    }
#endif
    i2c_slave_receive_t *t = &i2c_slave->receive_desc;
    if (int_mask & I2C_INTR_SLV_RXFIFO_WM) {
        s_i2c_handle_rx_fifo_wm(i2c_slave, t);
    }
    if (int_mask & I2C_INTR_SLV_COMPLETE) {
        s_i2c_handle_complete(i2c_slave, t, do_yield);
    }
    if (int_mask & I2C_INTR_SLV_TXFIFO_WM) {
        s_i2c_handle_tx_fifo_wm(i2c_slave, do_yield);
    }
}

static IRAM_ATTR void s_slave_nonfifo_isr_handler(uint32_t int_mask, void *arg, BaseType_t *do_yield)
{
    i2c_slave_dev_handle_t i2c_slave = (i2c_slave_dev_handle_t) arg;
    i2c_hal_context_t *hal = &i2c_slave->base->hal;
    if (int_mask & I2C_INTR_STRETCH) {
        i2c_slave->slave_evt.slave_stretch = 1;
        i2c_ll_slave_clear_stretch(hal->dev);
    }

    if (int_mask & I2C_INTR_SLV_COMPLETE) {
        i2c_slave->slave_evt.trans_complete = 1;
    }
    xQueueSendFromISR(i2c_slave->slv_evt_queue, &i2c_slave->slave_evt, do_yield);
}

static IRAM_ATTR void s_slave_isr_handle_default(void *arg)
{
    i2c_slave_dev_handle_t i2c_slave = (i2c_slave_dev_handle_t) arg;
    i2c_hal_context_t *hal = &i2c_slave->base->hal;
    portBASE_TYPE HPTaskAwoken = pdFALSE;
    uint32_t int_mask = 0;

    i2c_ll_get_intr_mask(hal->dev, &int_mask);
    i2c_ll_clear_intr_mask(hal->dev, int_mask);
    if (int_mask == 0) {
        return;
    }
#if SOC_I2C_SLAVE_SUPPORT_SLAVE_UNMATCH
    if (int_mask & I2C_INTR_UNMATCH) {
        i2c_slave->slave_evt.addr_unmatch = 1;
        ESP_DRAM_LOGE(TAG, "I2C address not match, trans failed");
    }
#endif

    if (i2c_slave->fifo_mode == I2C_SLAVE_NONFIFO) {
        s_slave_nonfifo_isr_handler(int_mask, i2c_slave, &HPTaskAwoken);
    } else {
        s_slave_fifo_isr_handler(int_mask, i2c_slave, &HPTaskAwoken);
    }

    //We only need to check here if there is a high-priority task needs to be switched.
    if (HPTaskAwoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }

}

esp_err_t i2c_new_slave_device(const i2c_slave_config_t *slave_config, i2c_slave_dev_handle_t *ret_handle)
{
#if CONFIG_I2C_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    i2c_slave_dev_t *i2c_slave = NULL;
    ESP_RETURN_ON_FALSE(slave_config && ret_handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(slave_config->sda_io_num) && GPIO_IS_VALID_GPIO(slave_config->scl_io_num), ESP_ERR_INVALID_ARG, TAG, "invalid SDA/SCL pin number");
#if SOC_LP_I2C_SUPPORTED
    ESP_RETURN_ON_FALSE(slave_config->i2c_port != (SOC_I2C_NUM - 1), ESP_ERR_NOT_SUPPORTED, TAG, "LP i2c is not supported in I2C slave");
#endif
    ESP_RETURN_ON_FALSE(slave_config->i2c_port < SOC_HP_I2C_NUM || slave_config->i2c_port == -1, ESP_ERR_INVALID_ARG, TAG, "invalid i2c port number");
    ESP_RETURN_ON_FALSE((slave_config->send_buf_depth > 0), ESP_ERR_INVALID_ARG, TAG, "invalid SCL speed");
#if SOC_I2C_SLAVE_SUPPORT_BROADCAST
    ESP_RETURN_ON_FALSE(((slave_config->addr_bit_len != I2C_ADDR_BIT_LEN_10) || (!slave_config->flags.broadcast_en)), ESP_ERR_INVALID_STATE, TAG, "10bits address cannot used together with broadcast");
#endif

    int i2c_port_num = slave_config->i2c_port;
    i2c_slave = heap_caps_calloc(1, sizeof(i2c_slave_dev_t), I2C_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(i2c_slave, ESP_ERR_NO_MEM, TAG, "no memory for i2c slave bus");

    ESP_GOTO_ON_ERROR(i2c_acquire_bus_handle(i2c_port_num, &i2c_slave->base, I2C_BUS_MODE_SLAVE), err, TAG, "I2C bus acquire failed");

    i2c_hal_context_t *hal = &i2c_slave->base->hal;
    i2c_slave->base->scl_num = slave_config->scl_io_num;
    i2c_slave->base->sda_num = slave_config->sda_io_num;
#if SOC_I2C_SLAVE_SUPPORT_I2CRAM_ACCESS
    i2c_slave->fifo_mode = (slave_config->flags.access_ram_en == true) ? I2C_SLAVE_NONFIFO : I2C_SLAVE_FIFO;
#endif

#if SOC_I2C_SLAVE_SUPPORT_SLAVE_UNMATCH
    if (slave_config->flags.slave_unmatch_en) {
        i2c_ll_enable_intr_mask(hal->dev, I2C_SLAVE_ADDR_UNMATCH_INT_ENA_M);
    }
#endif

    ESP_GOTO_ON_ERROR(i2c_common_set_pins(i2c_slave->base), err, TAG, "i2c slave set pins failed");

    i2c_slave->tx_ring_buf = xRingbufferCreateWithCaps(slave_config->send_buf_depth, RINGBUF_TYPE_BYTEBUF, I2C_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(i2c_slave->tx_ring_buf != NULL, ESP_ERR_INVALID_STATE, err, TAG, "ringbuffer create failed");

    i2c_slave->slv_rx_mux = xSemaphoreCreateBinaryWithCaps(I2C_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(i2c_slave->slv_rx_mux, ESP_ERR_NO_MEM, err, TAG, "No memory for binary semaphore");
    i2c_slave->slv_tx_mux = xSemaphoreCreateBinaryWithCaps(I2C_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(i2c_slave->slv_tx_mux, ESP_ERR_NO_MEM, err, TAG, "No memory for binary semaphore");
    i2c_slave->slv_evt_queue = xQueueCreateWithCaps(1, sizeof(i2c_slave_evt_t), I2C_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE((i2c_slave->slv_evt_queue != NULL), ESP_ERR_INVALID_STATE, err, TAG, "queue create failed");

    int isr_flags = I2C_INTR_ALLOC_FLAG;
    if (slave_config->intr_priority) {
        isr_flags |= 1 << (slave_config->intr_priority);
    }
    ret = esp_intr_alloc_intrstatus(i2c_periph_signal[i2c_port_num].irq, isr_flags, (uint32_t)i2c_ll_get_interrupt_status_reg(hal->dev), I2C_LL_SLAVE_EVENT_INTR, s_slave_isr_handle_default, i2c_slave, &i2c_slave->base->intr_handle);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "install i2c slave interrupt failed");

#if I2C_USE_RETENTION_LINK
    if (slave_config->flags.allow_pd != 0) {
        i2c_create_retention_module(i2c_slave->base);
    }
#endif // I2C_USE_RETENTION_LINK

    portENTER_CRITICAL(&i2c_slave->base->spinlock);
    i2c_ll_clear_intr_mask(hal->dev, I2C_LL_SLAVE_EVENT_INTR);
    i2c_hal_slave_init(hal);

#if SOC_I2C_SLAVE_SUPPORT_I2CRAM_ACCESS
    if (i2c_slave->fifo_mode == I2C_SLAVE_NONFIFO) {
        i2c_ll_enable_fifo_mode(hal->dev, false);
        i2c_ll_slave_enable_dual_addressing_mode(hal->dev, true);
    } else {
        i2c_ll_enable_fifo_mode(hal->dev, true);
        i2c_ll_slave_enable_dual_addressing_mode(hal->dev, false);
    }
#endif

    //Default, we enable hardware filter
    I2C_CLOCK_SRC_ATOMIC() {
        i2c_ll_set_source_clk(hal->dev, slave_config->clk_source);
    }
    bool addr_10bit_en = slave_config->addr_bit_len != I2C_ADDR_BIT_LEN_7;
    i2c_ll_set_slave_addr(hal->dev, slave_config->slave_addr, addr_10bit_en);
#if SOC_I2C_SLAVE_SUPPORT_BROADCAST
    i2c_ll_slave_broadcast_enable(hal->dev, slave_config->flags.broadcast_en);
#endif
    i2c_ll_set_txfifo_empty_thr(hal->dev, I2C_FIFO_EMPTY_THRESH_VAL_DEFAULT);
    i2c_ll_set_rxfifo_full_thr(hal->dev, I2C_FIFO_FULL_THRESH_VAL_DEFAULT);
    // set timing for data
    i2c_ll_set_sda_timing(hal->dev, 10, 10);
    i2c_ll_set_tout(hal->dev, I2C_SLAVE_TIMEOUT_DEFAULT);

#if SOC_I2C_SLAVE_CAN_GET_STRETCH_CAUSE
    i2c_ll_slave_enable_scl_stretch(hal->dev, slave_config->flags.stretch_en);
#endif
    i2c_ll_slave_enable_auto_start(hal->dev, true);

    i2c_ll_update(hal->dev);
    portEXIT_CRITICAL(&i2c_slave->base->spinlock);

    xSemaphoreGive(i2c_slave->slv_rx_mux);
    xSemaphoreGive(i2c_slave->slv_tx_mux);
    *ret_handle = i2c_slave;
    return ESP_OK;

err:
    if (i2c_slave) {
        i2c_slave_bus_destroy(i2c_slave);
    }
    return ret;
}

static esp_err_t i2c_slave_bus_destroy(i2c_slave_dev_handle_t i2c_slave)
{
    if (i2c_slave) {
        i2c_ll_disable_intr_mask(i2c_slave->base->hal.dev, I2C_LL_SLAVE_EVENT_INTR);
        i2c_common_deinit_pins(i2c_slave->base);
        if (i2c_slave->slv_rx_mux) {
            vSemaphoreDeleteWithCaps(i2c_slave->slv_rx_mux);
            i2c_slave->slv_rx_mux = NULL;
        }
        if (i2c_slave->slv_tx_mux) {
            vSemaphoreDeleteWithCaps(i2c_slave->slv_tx_mux);
            i2c_slave->slv_tx_mux = NULL;
        }
        if (i2c_slave->tx_ring_buf) {
            vRingbufferDeleteWithCaps(i2c_slave->tx_ring_buf);
            i2c_slave->tx_ring_buf = NULL;
        }
        if (i2c_slave->slv_evt_queue) {
            vQueueDeleteWithCaps(i2c_slave->slv_evt_queue);
        }
        i2c_release_bus_handle(i2c_slave->base);
    }

    free(i2c_slave);
    return ESP_OK;
}

esp_err_t i2c_del_slave_device(i2c_slave_dev_handle_t i2c_slave)
{
    ESP_RETURN_ON_FALSE(i2c_slave, ESP_ERR_INVALID_ARG, TAG, "i2c slave not initialized");
    int port_id = i2c_slave->base->port_num;
    ESP_LOGD(TAG, "del i2c bus(%d)", port_id);
    ESP_RETURN_ON_ERROR(i2c_slave_bus_destroy(i2c_slave), TAG, "destroy i2c bus failed");
    return ESP_OK;
}

esp_err_t i2c_slave_transmit(i2c_slave_dev_handle_t i2c_slave, const uint8_t *data, int size, int xfer_timeout_ms)
{
    ESP_RETURN_ON_FALSE(i2c_slave, ESP_ERR_INVALID_ARG, TAG, "i2c slave not initialized");
    ESP_RETURN_ON_FALSE(data, ESP_ERR_INVALID_ARG, TAG, "invalid data buffer");
    ESP_RETURN_ON_FALSE((i2c_slave->fifo_mode == I2C_SLAVE_FIFO), ESP_ERR_NOT_SUPPORTED, TAG, "non-fifo mode is not supported in this API, please set access_ram_en to false");
    esp_err_t ret = ESP_OK;
    i2c_hal_context_t *hal = &i2c_slave->base->hal;
    TickType_t wait_ticks = (xfer_timeout_ms == -1) ? portMAX_DELAY : pdMS_TO_TICKS(xfer_timeout_ms);

    ESP_RETURN_ON_FALSE(xSemaphoreTake(i2c_slave->slv_tx_mux, wait_ticks) == pdTRUE, ESP_ERR_TIMEOUT, TAG, "transmit timeout");
    ESP_GOTO_ON_FALSE(xRingbufferSend(i2c_slave->tx_ring_buf, data, size, wait_ticks) == pdTRUE, ESP_ERR_INVALID_STATE, err, TAG, "no space in ringbuffer");
    portENTER_CRITICAL(&i2c_slave->base->spinlock);
    i2c_slave->trans_data_length += size;
    i2c_ll_enable_intr_mask(hal->dev, I2C_LL_SLAVE_TX_EVENT_INTR);
    portEXIT_CRITICAL(&i2c_slave->base->spinlock);
err:
    xSemaphoreGive(i2c_slave->slv_tx_mux);
    return ret;
}

esp_err_t i2c_slave_receive(i2c_slave_dev_handle_t i2c_slave, uint8_t *data, size_t receive_size)
{
    ESP_RETURN_ON_FALSE(i2c_slave, ESP_ERR_INVALID_ARG, TAG, "i2c slave not initialized");
    ESP_RETURN_ON_FALSE(data, ESP_ERR_INVALID_ARG, TAG, "invalid data buffer");
    ESP_RETURN_ON_FALSE((i2c_slave->fifo_mode == I2C_SLAVE_FIFO), ESP_ERR_NOT_SUPPORTED, TAG, "non-fifo mode is not supported in this API, please set access_ram_en to false");
#if CONFIG_I2C_ISR_IRAM_SAFE
    ESP_RETURN_ON_FALSE(esp_ptr_internal(data), ESP_ERR_INVALID_ARG, TAG, "buffer must locate in internal RAM if IRAM_SAFE is enabled");
#endif
    i2c_hal_context_t *hal = &i2c_slave->base->hal;

    xSemaphoreTake(i2c_slave->slv_rx_mux, portMAX_DELAY);
    i2c_slave_receive_t *t = &i2c_slave->receive_desc;
    t->buffer = data;
    t->rcv_fifo_cnt = receive_size;
    i2c_slave->already_receive_len = 0;
    // Clear all interrupt raw bits before enable, avoid previous bus data affects interrupt.
    i2c_ll_clear_intr_mask(hal->dev, I2C_LL_SLAVE_RX_EVENT_INTR);
    i2c_ll_enable_intr_mask(hal->dev, I2C_LL_SLAVE_RX_EVENT_INTR);

    return ESP_OK;
}

#if SOC_I2C_SLAVE_SUPPORT_I2CRAM_ACCESS

esp_err_t i2c_slave_read_ram(i2c_slave_dev_handle_t i2c_slave, uint8_t ram_offset, uint8_t *data, size_t receive_size)
{
    ESP_RETURN_ON_FALSE(i2c_slave, ESP_ERR_INVALID_ARG, TAG, "i2c slave not initialized");
    ESP_RETURN_ON_FALSE(data, ESP_ERR_INVALID_ARG, TAG, "invalid data buffer");
    ESP_RETURN_ON_FALSE((ram_offset + receive_size <=  SOC_I2C_FIFO_LEN), ESP_ERR_INVALID_SIZE, TAG, "don't read data cross fifo boundary, see `SOC_I2C_FIFO_LEN`");
    ESP_RETURN_ON_FALSE((i2c_slave->fifo_mode == I2C_SLAVE_NONFIFO), ESP_ERR_NOT_SUPPORTED, TAG, "fifo mode is not supported in this API, please set access_ram_en to true");
    i2c_hal_context_t *hal = &i2c_slave->base->hal;

    uint32_t fifo_size = 0;
    portENTER_CRITICAL(&i2c_slave->base->spinlock);
    i2c_ll_get_rxfifo_cnt(hal->dev, &fifo_size);
    if (receive_size > fifo_size) {
        ESP_LOGE(TAG, "receive size is so large that fifo has not so much data to get");
        portEXIT_CRITICAL(&i2c_slave->base->spinlock);
        return ESP_ERR_INVALID_SIZE;
    }
    i2c_ll_read_rx_by_nonfifo(hal->dev, ram_offset, data, fifo_size);
    portEXIT_CRITICAL(&i2c_slave->base->spinlock);

    return ESP_OK;
}

esp_err_t i2c_slave_write_ram(i2c_slave_dev_handle_t i2c_slave, uint8_t ram_offset, const uint8_t *data, size_t size)
{
    ESP_RETURN_ON_FALSE(i2c_slave, ESP_ERR_INVALID_ARG, TAG, "i2c slave not initialized");
    ESP_RETURN_ON_FALSE(data, ESP_ERR_INVALID_ARG, TAG, "invalid data buffer");
    ESP_RETURN_ON_FALSE((i2c_slave->fifo_mode == I2C_SLAVE_NONFIFO), ESP_ERR_NOT_SUPPORTED, TAG, "fifo mode is not supported in this API, please set access_ram_en to true");

    i2c_hal_context_t *hal = &i2c_slave->base->hal;
    ESP_RETURN_ON_FALSE(xSemaphoreTake(i2c_slave->slv_tx_mux, portMAX_DELAY) == pdTRUE, ESP_ERR_TIMEOUT, TAG, "write to ram lock timeout");
    uint32_t fifo_size = 0;
    i2c_ll_txfifo_rst(hal->dev);
    i2c_ll_get_txfifo_len(hal->dev, &fifo_size);
    if (ram_offset + fifo_size < size) {
        ESP_EARLY_LOGE(TAG, "No extra fifo to fill your buffer, please split your buffer");
        return ESP_ERR_INVALID_SIZE;
    }
    i2c_ll_write_tx_by_nonfifo(hal->dev, ram_offset, data, size);
    xSemaphoreGive(i2c_slave->slv_tx_mux);
    return ESP_OK;
}

#endif

esp_err_t i2c_slave_register_event_callbacks(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_event_callbacks_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(i2c_slave != NULL, ESP_ERR_INVALID_ARG, TAG, "i2c slave handle not initialized");
    ESP_RETURN_ON_FALSE(cbs, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

#if CONFIG_I2C_ISR_IRAM_SAFE
#if SOC_I2C_SLAVE_CAN_GET_STRETCH_CAUSE
    if (cbs->on_stretch_occur) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_stretch_occur), ESP_ERR_INVALID_ARG, TAG, "i2c stretch occur callback not in IRAM");
    }
#endif // SOC_I2C_SLAVE_CAN_GET_STRETCH_CAUSE
    if (cbs->on_recv_done) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_recv_done), ESP_ERR_INVALID_ARG, TAG, "i2c receive done callback not in IRAM");
    }
    if (user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_data), ESP_ERR_INVALID_ARG, TAG, "user context not in internal RAM");
    }
#endif // CONFIG_I2C_ISR_IRAM_SAFE

    memcpy(&(i2c_slave->callbacks), cbs, sizeof(i2c_slave_event_callbacks_t));
    i2c_slave->user_ctx = user_data;
    return ESP_OK;
}
