/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <sys/param.h>
#include <sys/lock.h>
#include "sdkconfig.h"
#include "esp_types.h"
#include "esp_attr.h"
#include "esp_check.h"
#if CONFIG_I2C_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "esp_log.h"
#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "soc/i2c_periph.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/esp_clk.h"
#include "esp_rom_gpio.h"
#include "driver/i2c_master.h"
#include "i2c_private.h"
#include "driver/gpio.h"
#include "clk_ctrl_os.h"
#include "hal/i2c_types.h"
#include "hal/i2c_hal.h"
#include "hal/gpio_hal.h"
#include "esp_memory_utils.h"
#include "freertos/idf_additions.h"

static const char *TAG = "i2c.master";

#define DIM(array)                (sizeof(array)/sizeof(*array))
#define I2C_ADDRESS_TRANS_WRITE(device_address)    (((device_address) << 1) | 0)
#define I2C_ADDRESS_TRANS_READ(device_address)    (((device_address) << 1) | 1)

#if SOC_LP_I2C_SUPPORTED
#define I2C_FIFO_LEN(port_num) (((port_num) < SOC_HP_I2C_NUM) ? SOC_I2C_FIFO_LEN : SOC_LP_I2C_FIFO_LEN)
#else
#define I2C_FIFO_LEN(port_num) (SOC_I2C_FIFO_LEN)
#endif

static esp_err_t s_i2c_master_clear_bus(i2c_bus_handle_t handle)
{
#if !SOC_I2C_SUPPORT_HW_CLR_BUS
    const int scl_half_period = 5; // use standard 100kHz data rate
    int i = 0;
    gpio_set_direction(handle->scl_num, GPIO_MODE_OUTPUT_OD);
    gpio_set_direction(handle->sda_num, GPIO_MODE_INPUT_OUTPUT_OD);
    // If a SLAVE device was in a read operation when the bus was interrupted, the SLAVE device is controlling SDA.
    // The only bit during the 9 clock cycles of a READ byte the MASTER(ESP32) is guaranteed control over is during the ACK bit
    // period. If the slave is sending a stream of ZERO bytes, it will only release SDA during the ACK bit period.
    // So, this reset code needs to synchronous the bit stream with, Either, the ACK bit, Or a 1 bit to correctly generate
    // a STOP condition.
    gpio_set_level(handle->scl_num, 0);
    gpio_set_level(handle->sda_num, 1);
    esp_rom_delay_us(scl_half_period);
    while (!gpio_get_level(handle->sda_num) && (i++ < 9)) {
        gpio_set_level(handle->scl_num, 1);
        esp_rom_delay_us(scl_half_period);
        gpio_set_level(handle->scl_num, 0);
        esp_rom_delay_us(scl_half_period);
    }
    gpio_set_level(handle->sda_num, 0); // setup for STOP
    gpio_set_level(handle->scl_num, 1);
    esp_rom_delay_us(scl_half_period);
    gpio_set_level(handle->sda_num, 1); // STOP, SDA low -> high while SCL is HIGH
    i2c_common_set_pins(handle);
#else
    i2c_hal_context_t *hal = &handle->hal;
    i2c_ll_master_clr_bus(hal->dev, I2C_LL_RESET_SLV_SCL_PULSE_NUM_DEFAULT);
#endif
    return ESP_OK;
}

/**
 * @brief Reset I2C hardware fsm.
 *        1. Store filter and timing stuff(they are I2C hardware configure stuff)
 *           so, the configuration is same after reset.
 *
 * @param[in] i2c_master I2C master handle
 */
static esp_err_t s_i2c_hw_fsm_reset(i2c_master_bus_handle_t i2c_master)
{
    i2c_hal_context_t *hal = &i2c_master->base->hal;
#if !SOC_I2C_SUPPORT_HW_FSM_RST
    i2c_hal_timing_config_t timing_config;
    uint8_t filter_cfg;

    i2c_hal_get_timing_config(hal, &timing_config);
    i2c_ll_master_get_filter(hal->dev, &filter_cfg);

    //to reset the I2C hw module, we need re-enable the hw
    s_i2c_master_clear_bus(i2c_master->base);
    I2C_RCC_ATOMIC() {
        i2c_ll_reset_register(i2c_master->base->port_num);
    }

    i2c_hal_master_init(hal);
    i2c_ll_disable_intr_mask(hal->dev, I2C_LL_INTR_MASK);
    i2c_ll_clear_intr_mask(hal->dev, I2C_LL_INTR_MASK);
    i2c_hal_set_timing_config(hal, &timing_config);
    i2c_ll_master_set_filter(hal->dev, filter_cfg);
#else
    i2c_ll_master_fsm_rst(hal->dev);
    s_i2c_master_clear_bus(i2c_master->base);
#endif
    return ESP_OK;
}

static void s_i2c_err_log_print(i2c_master_event_t event, bool bypass_nack_log)
{
    if (event == I2C_EVENT_TIMEOUT) {
        ESP_LOGE(TAG, "I2C transaction timeout detected");
    }
    if (bypass_nack_log != true) {
        if (event == I2C_EVENT_NACK) {
            ESP_LOGE(TAG, "I2C transaction unexpected nack detected");
        }
    }
}

//////////////////////////////////////I2C operation functions////////////////////////////////////////////
/**
 * @brief This function is used to send I2C write command, which is divided in two parts.
 *        -1. If write buffer is smaller than hardware fifo, it can be sent out in one single time,
 *            so the hardware command(step) is simply like start(1)->write(2)->end(3)
 *        -2. If write buffer is larger than hardware fifo, it cannot be sent out in one time, so it needs
 *            to be separated in to different transactions by interrupt. In this time, the hardware command(step)
 *            simply looks like start(1)->write_part(2)--interrupt--...--write(1)->end(2).
 *
 * @param[in] i2c_master I2C master handle
 * @param[in] i2c_operation Pointer to I2C trans operation structure.
 * @param[in] fifo_fill will be populated with the number of bytes written to the hardware FIFO by this function
 * @param[in] address_fill I2C device address with read or write information.
 */
static bool s_i2c_write_command(i2c_master_bus_handle_t i2c_master, i2c_operation_t *i2c_operation, uint8_t *fifo_fill, uint8_t *address_fill, BaseType_t *do_yield)
{
    i2c_master->async_break = false;
    const size_t remaining_bytes = i2c_operation->total_bytes - i2c_operation->bytes_used;
    const i2c_ll_hw_cmd_t hw_end_cmd = {
        .op_code = I2C_LL_CMD_END
    };
    uint8_t *write_pr = NULL;
    i2c_hal_context_t *hal = &i2c_master->base->hal;
    i2c_bus_handle_t handle = i2c_master->base;
    i2c_ll_hw_cmd_t hw_cmd = i2c_operation->hw_cmd;
    uint8_t data_fill = 0;

    // data_fill refers to the length to fill the data
    data_fill = MIN(remaining_bytes, I2C_FIFO_LEN(i2c_master->base->port_num) - *address_fill);
    write_pr = i2c_operation->data + i2c_operation->bytes_used;
    i2c_operation->bytes_used += data_fill;
    hw_cmd.byte_num = data_fill + *address_fill;
    *address_fill = 0;
    atomic_store(&i2c_master->status, I2C_STATUS_WRITE);
    portENTER_CRITICAL_SAFE(&handle->spinlock);
    i2c_ll_write_txfifo(hal->dev, write_pr, data_fill);
    i2c_ll_master_write_cmd_reg(hal->dev, hw_cmd, i2c_master->cmd_idx);
    portEXIT_CRITICAL_SAFE(&handle->spinlock);
    i2c_master->w_r_size = data_fill;
#if SOC_I2C_STOP_INDEPENDENT
    i2c_ll_master_write_cmd_reg(hal->dev, hw_end_cmd, i2c_master->cmd_idx + 1);
    i2c_master->cmd_idx = 0;
    if (i2c_master->i2c_trans.ops[i2c_master->trans_idx].total_bytes == i2c_master->i2c_trans.ops[i2c_master->trans_idx].bytes_used) {
        i2c_master->i2c_trans.cmd_count--;
        i2c_master->trans_idx++;
    }
    portENTER_CRITICAL_SAFE(&handle->spinlock);
    if (i2c_master->async_trans == false) {
        i2c_hal_master_trans_start(hal);
    } else {
        i2c_master->async_break = true;
    }
    portEXIT_CRITICAL_SAFE(&handle->spinlock);

#else
    // If data cannot be sent in one time, send data out. Otherwise, continue configuring command.
    if ((remaining_bytes - data_fill) != 0) {
        portENTER_CRITICAL_SAFE(&handle->spinlock);
        i2c_ll_master_write_cmd_reg(hal->dev, hw_end_cmd, i2c_master->cmd_idx + 1);
        portEXIT_CRITICAL_SAFE(&handle->spinlock);
        i2c_master->cmd_idx = 0;
        if (i2c_master->async_trans == false) {
            i2c_hal_master_trans_start(hal);
        } else {
            i2c_master->async_break = true;
        }
    } else {
        i2c_master->cmd_idx++;
        i2c_master->trans_idx++;
        i2c_master->i2c_trans.cmd_count--;
        if (i2c_master->async_trans == false) {
            if (xPortInIsrContext()) {
                xSemaphoreGiveFromISR(i2c_master->cmd_semphr, do_yield);
            } else {
                xSemaphoreGive(i2c_master->cmd_semphr);
            }
        }
    }
#endif
    *fifo_fill = data_fill;

    return i2c_master->async_break;
}

/**
 * @brief This function is used to send I2C read command, which is divided in three parts.
 *        -1. If read buffer is smaller than hardware fifo, it can be sent out in one single time,
 *            so the hardware command(step) is simply like start(1)->read_ack(2)->read_nack(3)->end(4)
 *        -2. If read buffer is larger than hardware fifo, it cannot be sent out in one time, so it needs
 *            to be separated in to different transactions by interrupt. In this time, the hardware command(step)
 *            simply looks like start(1)->read_part(2)--interrupt--...--read(1)->end(2).
 *        -3. If only one byte is waiting to be read. only send nack command. like start(1)->read_nack(2)->end(3)
 *
 * @param[in] i2c_master I2C master handle
 * @param[in] i2c_operation Pointer to I2C trans operation structure.
 * @param[out] fifo_fill Pointer to read buffer length
 */
static bool s_i2c_read_command(i2c_master_bus_handle_t i2c_master, i2c_operation_t *i2c_operation, uint8_t *fifo_fill, BaseType_t *do_yield)
{
    i2c_master->async_break = false;
    const size_t remaining_bytes = i2c_operation->total_bytes - i2c_operation->bytes_used;
    const i2c_ll_hw_cmd_t hw_end_cmd = {
        .op_code = I2C_LL_CMD_END
    };
    i2c_hal_context_t *hal = &i2c_master->base->hal;
    i2c_bus_handle_t handle = i2c_master->base;
    i2c_ll_hw_cmd_t hw_cmd = i2c_operation->hw_cmd;

    *fifo_fill = MIN(remaining_bytes, I2C_FIFO_LEN(i2c_master->base->port_num) - i2c_master->read_len_static);
    i2c_master->rx_cnt = *fifo_fill;
    hw_cmd.byte_num = *fifo_fill;

    i2c_master->contains_read = true;
#if !SOC_I2C_STOP_INDEPENDENT
    if (remaining_bytes < I2C_FIFO_LEN(i2c_master->base->port_num) - 1) {
        if (i2c_operation->hw_cmd.ack_val == ACK_VAL) {
            if (remaining_bytes != 0) {
                i2c_ll_master_write_cmd_reg(hal->dev, hw_cmd, i2c_master->cmd_idx);
                i2c_master->read_len_static = i2c_master->rx_cnt;
                i2c_master->cmd_idx++;
            }
            i2c_master->read_buf_pos = i2c_master->trans_idx;
        } else {
            i2c_ll_master_write_cmd_reg(hal->dev, hw_cmd, i2c_master->cmd_idx);
            i2c_master->cmd_idx++;
        }
        i2c_master->trans_idx++;
        i2c_master->i2c_trans.cmd_count--;
        if (i2c_master->async_trans == false) {
            if (xPortInIsrContext()) {
                xSemaphoreGiveFromISR(i2c_master->cmd_semphr, do_yield);
            } else {
                xSemaphoreGive(i2c_master->cmd_semphr);
            }
        }
    } else {
        atomic_store(&i2c_master->status, I2C_STATUS_READ);
        portENTER_CRITICAL_SAFE(&handle->spinlock);
        i2c_ll_master_write_cmd_reg(hal->dev, hw_cmd, i2c_master->cmd_idx);
        i2c_ll_master_write_cmd_reg(hal->dev, hw_end_cmd, i2c_master->cmd_idx + 1);
        if (i2c_master->async_trans == false) {
            i2c_hal_master_trans_start(hal);
        } else {
            i2c_master->async_break = true;
        }
        portEXIT_CRITICAL_SAFE(&handle->spinlock);
    }
#else
    portENTER_CRITICAL_SAFE(&handle->spinlock);
    // If the read command work with ack_val, but no bytes to read, we skip
    // this command, and run next command directly.
    if (hw_cmd.ack_val == ACK_VAL) {
        if (i2c_operation->total_bytes == 0) {
            i2c_master->trans_idx++;
            hw_cmd = i2c_master->i2c_trans.ops[i2c_master->trans_idx].hw_cmd;
            i2c_master->i2c_trans.cmd_count--;
        }
    }
    i2c_ll_master_write_cmd_reg(hal->dev, hw_cmd, i2c_master->cmd_idx);
    i2c_ll_master_write_cmd_reg(hal->dev, hw_end_cmd, i2c_master->cmd_idx + 1);
    portEXIT_CRITICAL_SAFE(&handle->spinlock);
    i2c_master->status = I2C_STATUS_READ;
    portENTER_CRITICAL_SAFE(&handle->spinlock);
    if (i2c_master->async_trans == false) {
        i2c_hal_master_trans_start(hal);
    } else {
        i2c_master->async_break = true;
    }
    portEXIT_CRITICAL_SAFE(&handle->spinlock);
#endif

    return i2c_master->async_break;
}

/**
 * @brief This function is used to send I2C start or stop command, which is divided in two parts.
 *        If start command is accepted, a write address command must be followed. So prepared one address
 *        data here. Send with write or read command.
 *
 * @param[in] i2c_master I2C master handle
 * @param[in] i2c_operation Pointer to I2C trans operation structure.
 * @param[in] address_fill I2C device address with read or write information.
 */
static void s_i2c_start_end_command(i2c_master_bus_handle_t i2c_master, i2c_operation_t *i2c_operation, uint8_t *address_fill, BaseType_t *do_yield)
{
    i2c_hal_context_t *hal = &i2c_master->base->hal;
    i2c_ll_hw_cmd_t hw_cmd = i2c_operation->hw_cmd;

#if SOC_I2C_SUPPORT_10BIT_ADDR
    uint8_t cmd_address = (i2c_master->addr_10bits_bus == I2C_ADDR_BIT_LEN_7) ? i2c_master->i2c_trans.device_address : ((i2c_master->i2c_trans.device_address >> 8) | 0x78);
    uint8_t addr_byte = (i2c_master->addr_10bits_bus == I2C_ADDR_BIT_LEN_7) ? 1 : 2;
#else
    uint8_t cmd_address = i2c_master->i2c_trans.device_address;
    uint8_t addr_byte = 1;
#endif
    uint8_t addr_write[addr_byte];
    uint8_t addr_read[addr_byte];

    addr_write[0] = I2C_ADDRESS_TRANS_WRITE(cmd_address);
    addr_read[0] = I2C_ADDRESS_TRANS_READ(cmd_address);
#if SOC_I2C_SUPPORT_10BIT_ADDR
    if (i2c_master->addr_10bits_bus == I2C_ADDR_BIT_LEN_10) {
        addr_write[1] = i2c_master->i2c_trans.device_address & 0xff;
        addr_read[1] = i2c_master->i2c_trans.device_address & 0xff;
    }
#endif

    portENTER_CRITICAL_SAFE(&i2c_master->base->spinlock);
    i2c_ll_master_write_cmd_reg(hal->dev, hw_cmd, i2c_master->cmd_idx);
    i2c_master->cmd_idx++;
    portEXIT_CRITICAL_SAFE(&i2c_master->base->spinlock);
    if (i2c_operation->hw_cmd.op_code == I2C_LL_CMD_RESTART) {
        i2c_operation_t next_transaction = i2c_master->i2c_trans.ops[i2c_master->trans_idx + 1];

        if (next_transaction.hw_cmd.op_code == I2C_LL_CMD_READ) {
#if SOC_I2C_SUPPORT_10BIT_ADDR
            if (i2c_master->addr_10bits_bus == I2C_ADDR_BIT_LEN_10) {
                i2c_ll_hw_cmd_t hw_write_cmd = {
                    .ack_en = false,
                    .op_code = I2C_LL_CMD_WRITE,
                    .byte_num = 2,
                };
                portENTER_CRITICAL_SAFE(&i2c_master->base->spinlock);
                i2c_ll_write_txfifo(hal->dev, addr_write, sizeof(addr_write));
                i2c_ll_master_write_cmd_reg(hal->dev, hw_write_cmd, i2c_master->cmd_idx);
                i2c_master->cmd_idx++;
                portEXIT_CRITICAL_SAFE(&i2c_master->base->spinlock);
                const i2c_ll_hw_cmd_t hw_start_cmd = {
                    .op_code = I2C_LL_CMD_RESTART,
                };
                portENTER_CRITICAL_SAFE(&i2c_master->base->spinlock);
                i2c_ll_master_write_cmd_reg(hal->dev, hw_start_cmd, i2c_master->cmd_idx);
                i2c_master->cmd_idx++;
                portEXIT_CRITICAL_SAFE(&i2c_master->base->spinlock);

            }
#endif
            i2c_ll_hw_cmd_t hw_write_cmd = {
                .ack_en = i2c_master->ack_check_disable ? false : true,
                .op_code = I2C_LL_CMD_WRITE,
                .byte_num = 1,
            };
            portENTER_CRITICAL_SAFE(&i2c_master->base->spinlock);
            i2c_ll_write_txfifo(hal->dev, addr_read, sizeof(addr_read));
            i2c_ll_master_write_cmd_reg(hal->dev, hw_write_cmd, i2c_master->cmd_idx);
            i2c_master->cmd_idx++;
            portEXIT_CRITICAL_SAFE(&i2c_master->base->spinlock);
        } else if (next_transaction.hw_cmd.op_code == I2C_LL_CMD_STOP) {
            i2c_ll_hw_cmd_t hw_write_cmd = {
                .ack_en = true,
                .op_code = I2C_LL_CMD_WRITE,
                .byte_num = 1,
            };
            portENTER_CRITICAL_SAFE(&i2c_master->base->spinlock);
            i2c_ll_write_txfifo(hal->dev, addr_write, sizeof(addr_write));
            i2c_ll_master_write_cmd_reg(hal->dev, hw_write_cmd, i2c_master->cmd_idx);
            i2c_master->cmd_idx++;
            portEXIT_CRITICAL_SAFE(&i2c_master->base->spinlock);
        } else {
            /* The next transaction is a WRITE, we can merge the device address byte
                * with the next write command. No need to write the `cmd_reg` as the next
                * command will do it for us, we only need to add the device address byte
                * in the TX FIFO. */
            portENTER_CRITICAL_SAFE(&i2c_master->base->spinlock);
            i2c_ll_write_txfifo(hal->dev, addr_write, sizeof(addr_write));
            *address_fill += sizeof(addr_write);
            portEXIT_CRITICAL_SAFE(&i2c_master->base->spinlock);
        }
        if (i2c_master->async_trans == false) {
            if (xPortInIsrContext()) {
                xSemaphoreGiveFromISR(i2c_master->cmd_semphr, do_yield);
            } else {
                xSemaphoreGive(i2c_master->cmd_semphr);
            }
        }
    } else {
        atomic_store(&i2c_master->status, I2C_STATUS_STOP);
    }
    portENTER_CRITICAL_SAFE(&i2c_master->base->spinlock);
    i2c_master->trans_idx++;
    i2c_master->i2c_trans.cmd_count--;
    portEXIT_CRITICAL_SAFE(&i2c_master->base->spinlock);
}

static void s_i2c_send_commands(i2c_master_bus_handle_t i2c_master, TickType_t ticks_to_wait)
{
    i2c_hal_context_t *hal = &i2c_master->base->hal;
    uint8_t fifo_fill = 0;
    uint8_t address_fill = 0;

    // Initialise event queue.
    xQueueReset(i2c_master->event_queue);
    i2c_master->event = I2C_EVENT_ALIVE;
    while (i2c_master->i2c_trans.cmd_count) {
        if (xSemaphoreTake(i2c_master->cmd_semphr, ticks_to_wait) != pdTRUE) {
            // Software timeout, clear the command link and finish this transaction.
            i2c_master->cmd_idx = 0;
            i2c_master->trans_idx = 0;
            atomic_store(&i2c_master->status, I2C_STATUS_TIMEOUT);
            return;
        }

        if (i2c_master->status == I2C_STATUS_TIMEOUT) {
            s_i2c_hw_fsm_reset(i2c_master);
            i2c_master->cmd_idx = 0;
            i2c_master->trans_idx = 0;
            ESP_LOGE(TAG, "I2C hardware timeout detected");
            xSemaphoreGive(i2c_master->cmd_semphr);
            return;
        }

        if (i2c_master->status == I2C_STATUS_ACK_ERROR) {
            ESP_LOGE(TAG, "I2C hardware NACK detected");
            const i2c_ll_hw_cmd_t hw_stop_cmd = {
                .op_code = I2C_LL_CMD_STOP,
            };
            i2c_ll_master_write_cmd_reg(hal->dev, hw_stop_cmd, 0);
            i2c_hal_master_trans_start(hal);
            // The master trans start would start a transaction.
            // Queue wait for the event instead of return directly.
            break;
        }

        i2c_operation_t *i2c_operation = &i2c_master->i2c_trans.ops[i2c_master->trans_idx];

        if (i2c_operation->hw_cmd.op_code == I2C_LL_CMD_WRITE) {
            s_i2c_write_command(i2c_master, i2c_operation, &fifo_fill, &address_fill, NULL);
        } else if (i2c_operation->hw_cmd.op_code == I2C_LL_CMD_READ) {
            s_i2c_read_command(i2c_master, i2c_operation, &fifo_fill, NULL);
        } else {
            s_i2c_start_end_command(i2c_master, i2c_operation, &address_fill, NULL);
        }
    }
    i2c_hal_master_trans_start(hal);

    // For blocking implementation, we must wait event interrupt to update the status.
    // Otherwise, update status to timeout.
    i2c_master_event_t event;
    if (xQueueReceive(i2c_master->event_queue, &event, ticks_to_wait) == pdTRUE) {
        if (event == I2C_EVENT_DONE) {
            atomic_store(&i2c_master->status, I2C_STATUS_DONE);
        }
        s_i2c_err_log_print(event, i2c_master->bypass_nack_log);
    } else {
        i2c_master->cmd_idx = 0;
        i2c_master->trans_idx = 0;
        atomic_store(&i2c_master->status, I2C_STATUS_TIMEOUT);
    }
    xSemaphoreGive(i2c_master->cmd_semphr);
}

static void s_i2c_send_command_async(i2c_master_bus_handle_t i2c_master, BaseType_t *do_yield)
{
    i2c_hal_context_t *hal = &i2c_master->base->hal;
    uint8_t address_fill = 0;
    bool needs_start = false;
    if (i2c_master->i2c_trans.cmd_count == 0) {
        // No extra commands.
        i2c_master->trans_finish = true;
        i2c_master->in_progress = false;
        if (i2c_master->queue_trans) {
            xSemaphoreTakeFromISR(i2c_master->bus_lock_mux, do_yield);
            i2c_master->new_queue = true;
            i2c_master->ops_cur_size--;
            xSemaphoreGiveFromISR(i2c_master->bus_lock_mux, do_yield);
            xQueueSendFromISR(i2c_master->trans_queues[I2C_TRANS_QUEUE_COMPLETE], &i2c_master->i2c_trans, do_yield);
        }
        i2c_master->sent_all = true;
        return;
    }

    // Stop the transaction when invalid event is detected
    if (i2c_master->event == I2C_EVENT_NACK || i2c_master->event == I2C_EVENT_TIMEOUT) {
        i2c_master->sent_all = true;
        i2c_master->trans_finish = true;
        i2c_master->in_progress = false;
        if (i2c_master->queue_trans) {
            i2c_master->ops_cur_size--;
            xQueueSendFromISR(i2c_master->trans_queues[I2C_TRANS_QUEUE_COMPLETE], &i2c_master->i2c_trans, do_yield);
        }
        i2c_master->i2c_trans.cmd_count = 0;
        i2c_master->event = I2C_EVENT_ALIVE;
        return;
    }
    while (i2c_ll_is_bus_busy(hal->dev)) {}

    while (i2c_master->i2c_trans.cmd_count && !needs_start) {
        i2c_master->in_progress = true;
        i2c_master->sent_all = false;
        i2c_operation_t *i2c_operation = &i2c_master->i2c_trans.ops[i2c_master->trans_idx];
        uint8_t fifo_fill = 0;

        if (i2c_operation->hw_cmd.op_code == I2C_LL_CMD_WRITE) {
            needs_start = s_i2c_write_command(i2c_master, i2c_operation, &fifo_fill, &address_fill, do_yield);
        } else if (i2c_operation->hw_cmd.op_code == I2C_LL_CMD_READ) {
            needs_start = s_i2c_read_command(i2c_master, i2c_operation, &fifo_fill, do_yield);
        } else {
            s_i2c_start_end_command(i2c_master, i2c_operation, &address_fill, do_yield);

        }
    }
    i2c_hal_master_trans_start(hal);
}

static esp_err_t s_i2c_transaction_start(i2c_master_dev_handle_t i2c_dev, int xfer_timeout_ms)
{
    i2c_master_bus_handle_t i2c_master = i2c_dev->master_bus;
    i2c_hal_context_t *hal = &i2c_master->base->hal;
    TickType_t ticks_to_wait = (xfer_timeout_ms == -1) ? portMAX_DELAY : pdMS_TO_TICKS(xfer_timeout_ms);
    // Sometimes when the FSM get stuck, the ACK_ERR interrupt will occur endlessly until we reset the FSM and clear bus.
    esp_err_t ret = ESP_OK;
    if (i2c_master->status == I2C_STATUS_TIMEOUT || i2c_ll_is_bus_busy(hal->dev)) {
        s_i2c_hw_fsm_reset(i2c_master);
    }

    if (i2c_master->base->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_acquire(i2c_master->base->pm_lock), TAG, "acquire pm_lock failed");
    }

    portENTER_CRITICAL(&i2c_master->base->spinlock);
    atomic_init(&i2c_master->trans_idx, 0);
    atomic_store(&i2c_master->status, I2C_STATUS_IDLE);
    i2c_master->cmd_idx = 0;
    i2c_master->rx_cnt = 0;
    i2c_master->read_len_static = 0;

    i2c_hal_master_set_scl_timeout_val(hal, i2c_dev->scl_wait_us, i2c_master->base->clk_src_freq_hz);

    if (!i2c_master->base->is_lp_i2c) {
        I2C_CLOCK_SRC_ATOMIC() {
            i2c_ll_set_source_clk(hal->dev, i2c_master->base->clk_src);
        }
    }
#if SOC_LP_I2C_SUPPORTED
    else {
        LP_I2C_SRC_CLK_ATOMIC() {
            lp_i2c_ll_set_source_clk(hal->dev, i2c_master->base->clk_src);
        }
    }
#endif
    I2C_CLOCK_SRC_ATOMIC() {
        i2c_hal_set_bus_timing(hal, i2c_dev->scl_speed_hz, i2c_master->base->clk_src, i2c_master->base->clk_src_freq_hz);
    }
    i2c_ll_master_set_fractional_divider(hal->dev, 0, 0);
    i2c_ll_update(hal->dev);

    i2c_ll_txfifo_rst(hal->dev);
    i2c_ll_rxfifo_rst(hal->dev);
    i2c_ll_enable_intr_mask(hal->dev, I2C_LL_MASTER_EVENT_INTR);
    portEXIT_CRITICAL(&i2c_master->base->spinlock);
    if (i2c_master->async_trans == true) {
        s_i2c_send_command_async(i2c_master, NULL);
    } else {
        s_i2c_send_commands(i2c_master, ticks_to_wait);
        // Wait event bits
        if (i2c_master->status != I2C_STATUS_DONE) {
            ret = ESP_ERR_INVALID_STATE;
        }
        // Interrupt can be disabled when on transaction finishes.
        i2c_ll_disable_intr_mask(hal->dev, I2C_LL_MASTER_EVENT_INTR);
    }

    if (i2c_master->base->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_release(i2c_master->base->pm_lock), TAG, "release pm_lock failed");
    }

    return ret;
}

///////////////////////////////I2C DRIVERS//////////////////////////////////////////////////////////////

IRAM_ATTR static void i2c_isr_receive_handler(i2c_master_bus_t *i2c_master)
{
    i2c_hal_context_t *hal = &i2c_master->base->hal;

    if (i2c_master->status == I2C_STATUS_READ) {
        i2c_operation_t *i2c_operation = &i2c_master->i2c_trans.ops[i2c_master->trans_idx];
        portENTER_CRITICAL_ISR(&i2c_master->base->spinlock);
        i2c_ll_read_rxfifo(hal->dev, i2c_operation->data + i2c_operation->bytes_used, i2c_master->rx_cnt);
        /* rx_cnt bytes have just been read, increment the number of bytes used from the buffer */
        i2c_master->w_r_size = i2c_master->rx_cnt;
        i2c_operation->bytes_used += i2c_master->rx_cnt;
        i2c_master->cmd_idx = 0;

        /* Test if there are still some remaining bytes to send. */
        if (i2c_operation->bytes_used == i2c_operation->total_bytes) {
            i2c_master->i2c_trans.cmd_count--;
            i2c_master->read_buf_pos = i2c_master->trans_idx;
            i2c_master->trans_idx++;
            i2c_operation->bytes_used = 0;
        }
        portEXIT_CRITICAL_ISR(&i2c_master->base->spinlock);
    }
#if !SOC_I2C_STOP_INDEPENDENT
    else {
        i2c_operation_t *i2c_operation = &i2c_master->i2c_trans.ops[i2c_master->read_buf_pos];
        portENTER_CRITICAL_ISR(&i2c_master->base->spinlock);
        i2c_ll_read_rxfifo(hal->dev, i2c_operation->data + i2c_operation->bytes_used, i2c_master->read_len_static);
        i2c_ll_read_rxfifo(hal->dev, i2c_master->i2c_trans.ops[i2c_master->read_buf_pos + 1].data, 1);
        i2c_master->w_r_size = i2c_master->read_len_static + 1;
        i2c_master->contains_read = false;
        portEXIT_CRITICAL_ISR(&i2c_master->base->spinlock);
    }
#endif
}

static void IRAM_ATTR i2c_master_isr_handler_default(void *arg)
{
    i2c_master_bus_handle_t i2c_master = (i2c_master_bus_t*) arg;

    i2c_hal_context_t *hal = &i2c_master->base->hal;
    portBASE_TYPE HPTaskAwoken = pdFALSE;
    uint32_t int_mask;
    BaseType_t ret = pdTRUE;
    i2c_master->trans_done = false;
    i2c_ll_get_intr_mask(hal->dev, &int_mask);
    i2c_ll_clear_intr_mask(hal->dev, int_mask);
    if (int_mask == 0) {
        return;
    }

    if (int_mask & I2C_LL_INTR_NACK) {
        atomic_store(&i2c_master->status, I2C_STATUS_ACK_ERROR);
        i2c_master->event = I2C_EVENT_NACK;
    } else if (int_mask & I2C_LL_INTR_TIMEOUT || int_mask & I2C_LL_INTR_ARBITRATION) {
        atomic_store(&i2c_master->status, I2C_STATUS_TIMEOUT);
        i2c_master->event = I2C_EVENT_TIMEOUT;
    } else if (int_mask & I2C_LL_INTR_MST_COMPLETE) {
        i2c_master->trans_done = true;
        i2c_master->event = I2C_EVENT_DONE;
    }
    if (i2c_master->event != I2C_EVENT_ALIVE) {
        xQueueSendFromISR(i2c_master->event_queue, (void *)&i2c_master->event, &HPTaskAwoken);
    }
    if (i2c_master->contains_read == true) {
        if (int_mask & I2C_LL_INTR_MST_COMPLETE || int_mask & I2C_LL_INTR_END_DETECT) {
            i2c_isr_receive_handler(i2c_master);
        }
    }

    if (i2c_master->async_trans) {
        i2c_master_dev_handle_t i2c_dev = NULL;

        i2c_master_device_list_t *device_item;
        xSemaphoreTakeFromISR(i2c_master->bus_lock_mux, &HPTaskAwoken);
        SLIST_FOREACH(device_item, &i2c_master->device_list, next) {
            if (device_item->device->device_address == i2c_master->i2c_trans.device_address) {
                i2c_dev = device_item->device;
                break;
            }
        }
        xSemaphoreGiveFromISR(i2c_master->bus_lock_mux, &HPTaskAwoken);
        if (i2c_dev == NULL) {
            return;
        }
        i2c_master_event_data_t evt = {
            .event = i2c_master->event,
        };
        s_i2c_send_command_async(i2c_master, &HPTaskAwoken);
        if (i2c_master->trans_done) {
            if (i2c_dev->on_trans_done) {
                i2c_dev->on_trans_done(i2c_dev, &evt, i2c_dev->user_ctx);
            }
            if (i2c_master->new_queue && i2c_master->num_trans_inqueue > 0 && i2c_master->in_progress == false) {
                i2c_transaction_t t = {};
                i2c_master->num_trans_inqueue--;
                i2c_master->new_queue = false;
                t.cmd_count = 0;
                t.device_address = 0;
                t.ops = NULL;
                ret = xQueueReceiveFromISR(i2c_master->trans_queues[I2C_TRANS_QUEUE_PROGRESS], &t, &HPTaskAwoken);
                if (ret == pdTRUE) {
                    i2c_master->queue_trans = true;
                    atomic_init(&i2c_master->trans_idx, 0);
                    atomic_store(&i2c_master->status, I2C_STATUS_IDLE);
                    i2c_master->cmd_idx = 0;
                    i2c_master->rx_cnt = 0;
                    i2c_master->read_len_static = 0;
                    i2c_ll_txfifo_rst(hal->dev);
                    i2c_ll_rxfifo_rst(hal->dev);
                    i2c_master->i2c_trans = t;
                    memcpy(i2c_master->i2c_ops, t.ops, t.cmd_count * sizeof(i2c_operation_t));

                    s_i2c_send_command_async(i2c_master, &HPTaskAwoken);
                }
            }
        }
    } else {
        xSemaphoreGiveFromISR(i2c_master->cmd_semphr, &HPTaskAwoken);
    }

    if (HPTaskAwoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }

}

static esp_err_t i2c_param_master_config(i2c_bus_handle_t handle, const i2c_master_bus_config_t *i2c_conf)
{
    i2c_hal_context_t *hal = &handle->hal;

    ESP_RETURN_ON_ERROR(i2c_common_set_pins(handle), TAG, "i2c master set pin failed");
    soc_module_clk_t clk_source = i2c_conf->clk_source;
    ESP_RETURN_ON_ERROR(i2c_select_periph_clock(handle, clk_source), TAG, "i2c select clock failed");
    handle->clk_src = i2c_conf->clk_source;
    portENTER_CRITICAL(&handle->spinlock);
    i2c_hal_master_init(hal);
    i2c_ll_update(hal->dev);
    portEXIT_CRITICAL(&handle->spinlock);
    return ESP_OK;
}

static esp_err_t i2c_master_bus_destroy(i2c_master_bus_handle_t bus_handle)
{
    ESP_RETURN_ON_FALSE(bus_handle, ESP_ERR_INVALID_ARG, TAG, "no memory for i2c master bus");
    i2c_master_bus_handle_t i2c_master = bus_handle;
    if (i2c_release_bus_handle(i2c_master->base) == ESP_OK) {
        if (i2c_master) {
            if (i2c_master->bus_lock_mux) {
                vSemaphoreDeleteWithCaps(i2c_master->bus_lock_mux);
                i2c_master->bus_lock_mux = NULL;
            }
            if (i2c_master->cmd_semphr) {
                vSemaphoreDeleteWithCaps(i2c_master->cmd_semphr);
                i2c_master->cmd_semphr = NULL;
            }
            if (i2c_master->event_queue) {
                vQueueDeleteWithCaps(i2c_master->event_queue);
            }
            if (i2c_master->queues_storage) {
                free(i2c_master->queues_storage);
            }
            free(i2c_master->i2c_async_ops);
            for (int i = 0; i < I2C_TRANS_QUEUE_MAX; i++) {
                if (i2c_master->trans_queues[i]) {
                    vQueueDelete(i2c_master->trans_queues[i]);
                }
            }
            bus_handle = NULL;
        }

        free(i2c_master);
    } else {
        free(i2c_master);
    }

    return ESP_OK;
}

static esp_err_t s_i2c_asynchronous_transaction(i2c_master_dev_handle_t i2c_dev, i2c_operation_t *i2c_ops, size_t ops_dim, int timeout_ms)
{
    i2c_master_bus_t *i2c_master = i2c_dev->master_bus;
    if (i2c_master->sent_all == true && i2c_master->num_trans_inqueue == 0) {
        memcpy(i2c_master->i2c_ops, i2c_ops, sizeof(i2c_operation_t) * ops_dim);
        i2c_master->addr_10bits_bus = i2c_dev->addr_10bits;
        i2c_master->i2c_trans = (i2c_transaction_t) {
            .device_address = i2c_dev->device_address,
            .ops = i2c_master->i2c_ops,
            .cmd_count = ops_dim,
        };

        i2c_master->sent_all = false;
        i2c_master->trans_finish = false;
        i2c_master->queue_trans = false;
        ESP_RETURN_ON_ERROR(s_i2c_transaction_start(i2c_dev, timeout_ms), TAG, "I2C transaction failed");
    } else {
        xSemaphoreTake(i2c_master->bus_lock_mux, portMAX_DELAY);
        // Check whether operation pool has extra space.
        bool ops_pool = (i2c_master->ops_cur_size != i2c_master->queue_size);
        i2c_operation_t *ops_current;

        if (ops_pool) {
            i2c_master->ops_cur_size++;
            memcpy(&i2c_master->i2c_async_ops[i2c_master->ops_prepare_idx], i2c_ops, sizeof(i2c_operation_t) * ops_dim);
            // Clear unused memory
            uint8_t unused_dim = I2C_STATIC_OPERATION_ARRAY_MAX - ops_dim;
            if (unused_dim != 0) {
                memset(&i2c_master->i2c_async_ops[i2c_master->ops_prepare_idx] + sizeof(i2c_operation_t) * ops_dim, 0, sizeof(i2c_operation_t) * unused_dim);
            }
            // Record current operation and feed to transaction queue.
            ops_current = &i2c_master->i2c_async_ops[i2c_master->ops_prepare_idx][0];
            i2c_master->ops_prepare_idx = (i2c_master->ops_prepare_idx + 1) % i2c_master->queue_size;
        }

        xSemaphoreGive(i2c_master->bus_lock_mux);
        ESP_RETURN_ON_FALSE(ops_pool == true, ESP_ERR_INVALID_STATE, TAG, "ops list is full, please increase your trans_queue_depth");

        i2c_transaction_t i2c_queue_pre;
        if (i2c_master->num_trans_inflight < i2c_master->queue_size) {
            ESP_RETURN_ON_FALSE(xQueueReceive(i2c_master->trans_queues[I2C_TRANS_QUEUE_READY], &i2c_queue_pre, portMAX_DELAY) == pdTRUE, ESP_FAIL, TAG, "no transaction in the ready queue");
        } else {
            ESP_RETURN_ON_FALSE(xQueueReceive(i2c_master->trans_queues[I2C_TRANS_QUEUE_COMPLETE], &i2c_queue_pre, portMAX_DELAY) == pdTRUE, ESP_FAIL, TAG, "recycle transaction from done queue failed");
            i2c_master->num_trans_inflight--;
        }
        i2c_queue_pre = (i2c_transaction_t) {
            .device_address = i2c_dev->device_address,
            .ops = ops_current,
            .cmd_count = ops_dim,
        };
        if (xQueueSend(i2c_master->trans_queues[I2C_TRANS_QUEUE_PROGRESS], &i2c_queue_pre, portMAX_DELAY) == pdTRUE) {
            i2c_master->num_trans_inflight++;
            i2c_master->num_trans_inqueue++;
            if (i2c_master->sent_all == true) {
                // Oh no, you cannot get the queue from ISR, so you get queue here.
                ESP_RETURN_ON_FALSE(xQueueReceive(i2c_master->trans_queues[I2C_TRANS_QUEUE_PROGRESS], &i2c_queue_pre, portMAX_DELAY) == pdTRUE, ESP_FAIL, TAG, "get trans from progress queue failed");
                i2c_master->ops_cur_size--;
                i2c_master->num_trans_inflight--;
                i2c_master->num_trans_inqueue--;
                i2c_master->sent_all = false;
                i2c_master->trans_finish = false;
                i2c_master->queue_trans = false;
                ESP_RETURN_ON_ERROR(s_i2c_transaction_start(i2c_dev, timeout_ms), TAG, "I2C transaction failed");
            }
        } else {
            ESP_RETURN_ON_FALSE(xQueueSend(i2c_master->trans_queues[I2C_TRANS_QUEUE_READY], &i2c_queue_pre, 0) == pdTRUE, ESP_ERR_INVALID_STATE, TAG, "ready queue full");
        }
    }

    return ESP_OK;
}

static esp_err_t s_i2c_synchronous_transaction(i2c_master_dev_handle_t i2c_dev, i2c_operation_t *i2c_ops, size_t ops_dim, int timeout_ms)
{
    esp_err_t ret = ESP_OK;
    i2c_dev->master_bus->trans_done = false;
    TickType_t ticks_to_wait = (timeout_ms == -1) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    if (xSemaphoreTake(i2c_dev->master_bus->bus_lock_mux, ticks_to_wait) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }
    memcpy(i2c_dev->master_bus->i2c_ops, i2c_ops, sizeof(i2c_operation_t) * ops_dim);
    i2c_dev->master_bus->addr_10bits_bus = i2c_dev->addr_10bits;
    i2c_dev->master_bus->i2c_trans = (i2c_transaction_t) {
        .device_address = i2c_dev->device_address,
        .ops = i2c_dev->master_bus->i2c_ops,
        .cmd_count = ops_dim,
    };

    i2c_dev->master_bus->sent_all = false;
    i2c_dev->master_bus->trans_finish = false;
    i2c_dev->master_bus->queue_trans = false;
    i2c_dev->master_bus->ack_check_disable = i2c_dev->ack_check_disable;
    ESP_GOTO_ON_ERROR(s_i2c_transaction_start(i2c_dev, timeout_ms), err, TAG, "I2C transaction failed");
    xSemaphoreGive(i2c_dev->master_bus->bus_lock_mux);
    return ret;

err:
    // When error occurs, reset hardware fsm in case not influence following transactions.
    s_i2c_hw_fsm_reset(i2c_dev->master_bus);
    xSemaphoreGive(i2c_dev->master_bus->bus_lock_mux);
    return ret;
}

esp_err_t i2c_new_master_bus(const i2c_master_bus_config_t *bus_config, i2c_master_bus_handle_t *ret_bus_handle)
{
#if CONFIG_I2C_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    i2c_master_bus_t *i2c_master = NULL;
    i2c_port_num_t i2c_port_num = bus_config->i2c_port;
    ESP_RETURN_ON_FALSE(bus_config, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE((bus_config->i2c_port < SOC_I2C_NUM || bus_config->i2c_port == -1), ESP_ERR_INVALID_ARG, TAG, "invalid i2c port number");
    ESP_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(bus_config->sda_io_num) && GPIO_IS_VALID_GPIO(bus_config->scl_io_num), ESP_ERR_INVALID_ARG, TAG, "invalid SDA/SCL pin number");

    i2c_master = heap_caps_calloc(1, sizeof(i2c_master_bus_t) + 20 * sizeof(i2c_transaction_t), I2C_MEM_ALLOC_CAPS);

    ESP_RETURN_ON_FALSE(i2c_master, ESP_ERR_NO_MEM, TAG, "no memory for i2c master bus");

    ESP_GOTO_ON_ERROR(i2c_acquire_bus_handle(i2c_port_num, &i2c_master->base, I2C_BUS_MODE_MASTER), err, TAG, "I2C bus acquire failed");
    i2c_port_num = i2c_master->base->port_num;

    i2c_hal_context_t *hal = &i2c_master->base->hal;
    i2c_master->base->scl_num = bus_config->scl_io_num;
    i2c_master->base->sda_num = bus_config->sda_io_num;
    i2c_master->base->pull_up_enable = bus_config->flags.enable_internal_pullup;

    if (i2c_master->base->pull_up_enable == false) {
        ESP_LOGW(TAG, "Please check pull-up resistances whether be connected properly. Otherwise unexpected behavior would happen. For more detailed information, please read docs");
    }
    ESP_GOTO_ON_ERROR(i2c_param_master_config(i2c_master->base, bus_config), err, TAG, "i2c configure parameter failed");

    i2c_master->bus_lock_mux = xSemaphoreCreateBinaryWithCaps(I2C_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(i2c_master->bus_lock_mux, ESP_ERR_NO_MEM, err, TAG, "No memory for binary semaphore");
    xSemaphoreGive(i2c_master->bus_lock_mux);

    i2c_master->cmd_semphr = xSemaphoreCreateBinaryWithCaps(I2C_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(i2c_master->cmd_semphr, ESP_ERR_NO_MEM, err, TAG, "no memory for i2c semaphore struct");

    i2c_master->event_queue = xQueueCreateWithCaps(1, sizeof(i2c_master_event_t), I2C_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(i2c_master->event_queue, ESP_ERR_NO_MEM, err, TAG, "no memory for i2c queue struct");

    portENTER_CRITICAL(&i2c_master->base->spinlock);
    i2c_ll_clear_intr_mask(hal->dev, I2C_LL_MASTER_EVENT_INTR);
    portEXIT_CRITICAL(&i2c_master->base->spinlock);

    if (bus_config->intr_priority) {
        ESP_RETURN_ON_FALSE(1 << (bus_config->intr_priority) & I2C_ALLOW_INTR_PRIORITY_MASK, ESP_ERR_INVALID_ARG, TAG, "invalid interrupt priority:%d", bus_config->intr_priority);
    }

    xSemaphoreTake(i2c_master->bus_lock_mux, portMAX_DELAY);
    SLIST_INIT(&i2c_master->device_list);
    xSemaphoreGive(i2c_master->bus_lock_mux);
    // Initialize the queue
    if (bus_config->trans_queue_depth) {
        ESP_LOGW(TAG, "Please note i2c asynchronous is only used for specific scenario currently. It's experimental for other users because user cannot get bus error from API. And It's not compatible with ``i2c_master_probe``. If user makes sure there won't be any error on bus and tested with no problem, this message can be ignored.");
        i2c_master->async_trans = true;
        i2c_master->sent_all = true;
        i2c_master->trans_finish = true;
        i2c_master->new_queue = true;
        i2c_master->queue_size = bus_config->trans_queue_depth;
        i2c_master->queues_storage = (uint8_t*)heap_caps_calloc(bus_config->trans_queue_depth * I2C_TRANS_QUEUE_MAX, sizeof(i2c_transaction_t), I2C_MEM_ALLOC_CAPS);
        ESP_RETURN_ON_FALSE(i2c_master->queues_storage, ESP_ERR_NO_MEM, TAG, "no mem for queue storage");
        i2c_transaction_t **pp_trans_desc = (i2c_transaction_t **)i2c_master->queues_storage;
        for (int i = 0; i < I2C_TRANS_QUEUE_MAX; i++) {
            i2c_master->trans_queues[i] = xQueueCreate(bus_config->trans_queue_depth, sizeof(i2c_transaction_t));

            pp_trans_desc += bus_config->trans_queue_depth;
            // sanity check
            assert(i2c_master->trans_queues[i]);
        }
        i2c_transaction_t trans_pre = {};
        for (int i = 0; i < bus_config->trans_queue_depth ; i++) {
            trans_pre = i2c_master->i2c_trans_pool[i];
            ESP_RETURN_ON_FALSE(xQueueSend(i2c_master->trans_queues[I2C_TRANS_QUEUE_READY], &trans_pre, 0) == pdTRUE,
                                ESP_ERR_INVALID_STATE, TAG, "ready queue full");
        }

        i2c_master->i2c_async_ops = (i2c_operation_t(*)[I2C_STATIC_OPERATION_ARRAY_MAX])heap_caps_calloc(bus_config->trans_queue_depth, sizeof(*i2c_master->i2c_async_ops), I2C_MEM_ALLOC_CAPS);
        ESP_RETURN_ON_FALSE(i2c_master->i2c_async_ops, ESP_ERR_NO_MEM, TAG, "no mem for operations");
        i2c_master->ops_prepare_idx = 0;

    }
    int isr_flags = I2C_INTR_ALLOC_FLAG;
    if (bus_config->intr_priority) {
        isr_flags |= 1 << (bus_config->intr_priority);
    }
    ret = esp_intr_alloc_intrstatus(i2c_periph_signal[i2c_port_num].irq, isr_flags, (uint32_t)i2c_ll_get_interrupt_status_reg(hal->dev), I2C_LL_MASTER_EVENT_INTR, i2c_master_isr_handler_default, i2c_master, &i2c_master->base->intr_handle);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "install i2c master interrupt failed");
    atomic_init(&i2c_master->status, I2C_STATUS_IDLE);

    i2c_ll_master_set_filter(hal->dev, bus_config->glitch_ignore_cnt);

    xSemaphoreGive(i2c_master->cmd_semphr);

    *ret_bus_handle = i2c_master;
    return ESP_OK;

err:
    if (i2c_master) {
        i2c_master_bus_destroy(i2c_master);
    }
    return ret;
}

esp_err_t i2c_master_bus_add_device(i2c_master_bus_handle_t bus_handle, const i2c_device_config_t *dev_config, i2c_master_dev_handle_t *ret_handle)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE((bus_handle != NULL), ESP_ERR_INVALID_ARG, TAG, "this bus is not initialized, please call `i2c_new_master_bus`");
    ESP_RETURN_ON_FALSE(dev_config, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(dev_config->scl_speed_hz > 0, ESP_ERR_INVALID_ARG, TAG, "invalid scl frequency");
    if (bus_handle->base->bus_mode != I2C_BUS_MODE_MASTER) {
        ESP_LOGE(TAG, "This is not master bus!");
        return ESP_ERR_INVALID_ARG;
    }
    i2c_master_bus_t *i2c_master = bus_handle;
    i2c_master_dev_t *i2c_dev = heap_caps_calloc(1, sizeof(i2c_master_dev_t), I2C_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(i2c_dev, ESP_ERR_NO_MEM, err, TAG, "no memory for i2c master device");
    i2c_dev->device_address = dev_config->device_address;
    i2c_dev->scl_speed_hz = dev_config->scl_speed_hz;
    i2c_dev->addr_10bits = dev_config->dev_addr_length;
    i2c_dev->master_bus = i2c_master;
    i2c_dev->ack_check_disable = dev_config->flags.disable_ack_check;
    i2c_dev->scl_wait_us = (dev_config->scl_wait_us == 0) ? I2C_LL_SCL_WAIT_US_VAL_DEFAULT : dev_config->scl_wait_us;

    i2c_master_device_list_t *device_item = (i2c_master_device_list_t *)calloc(1, sizeof(i2c_master_device_list_t));
    ESP_GOTO_ON_FALSE((device_item != NULL), ESP_ERR_NO_MEM, err, TAG, "no memory for i2c device item`");
    device_item->device = i2c_dev;
    xSemaphoreTake(bus_handle->bus_lock_mux, portMAX_DELAY);
    SLIST_INSERT_HEAD(&bus_handle->device_list, device_item, next);
    xSemaphoreGive(bus_handle->bus_lock_mux);

    *ret_handle = i2c_dev;
    return ret;
err:
    if (i2c_dev) {
        i2c_master_bus_rm_device(i2c_dev);
    }
    return ret;
}

esp_err_t i2c_master_bus_rm_device(i2c_master_dev_handle_t handle)
{
    ESP_RETURN_ON_FALSE((handle != NULL), ESP_ERR_INVALID_ARG, TAG, "this device is not initialized");
    ESP_RETURN_ON_FALSE((((int)atomic_load(&handle->master_bus->status) > (int)I2C_STATUS_START)), ESP_ERR_INVALID_STATE, TAG, "Wrong I2C status, cannot delete device");

    i2c_master_bus_handle_t i2c_master = handle->master_bus;
    i2c_master_device_list_t *device_item;
    xSemaphoreTake(handle->master_bus->bus_lock_mux, portMAX_DELAY);
    SLIST_FOREACH(device_item, &i2c_master->device_list, next) {
        if (handle == device_item->device) {
            SLIST_REMOVE(&i2c_master->device_list, device_item, i2c_master_device_list, next);
            free(device_item);
            break;
        }
    }
    xSemaphoreGive(handle->master_bus->bus_lock_mux);

    if (handle) {
        free(handle);
        handle = NULL;
    }
    return ESP_OK;
}

esp_err_t i2c_del_master_bus(i2c_master_bus_handle_t bus_handle)
{
    ESP_LOGD(TAG, "del i2c bus(%d)", bus_handle->base->port_num);
    ESP_RETURN_ON_ERROR(i2c_master_bus_destroy(bus_handle), TAG, "destroy i2c bus failed");
    return ESP_OK;
}

esp_err_t i2c_master_bus_reset(i2c_master_bus_handle_t bus_handle)
{
    ESP_RETURN_ON_FALSE((bus_handle != NULL), ESP_ERR_INVALID_ARG, TAG, "This bus is not initialized");
    // Reset I2C master bus
    ESP_RETURN_ON_ERROR(s_i2c_hw_fsm_reset(bus_handle), TAG, "I2C master bus reset failed");
    // Reset I2C status state
    atomic_store(&bus_handle->status, I2C_STATUS_IDLE);
    return ESP_OK;
}

esp_err_t i2c_master_transmit(i2c_master_dev_handle_t i2c_dev, const uint8_t *write_buffer, size_t write_size, int xfer_timeout_ms)
{
    ESP_RETURN_ON_FALSE(i2c_dev != NULL, ESP_ERR_INVALID_ARG, TAG, "i2c handle not initialized");
    ESP_RETURN_ON_FALSE((write_buffer != NULL) && (write_size > 0), ESP_ERR_INVALID_ARG, TAG, "i2c transmit buffer or size invalid");

    i2c_operation_t i2c_ops[] = {
        {.hw_cmd = I2C_TRANS_START_COMMAND},
        {.hw_cmd = I2C_TRANS_WRITE_COMMAND(i2c_dev->ack_check_disable ? false : true), .data = (uint8_t *)write_buffer, .total_bytes = write_size},
        {.hw_cmd = I2C_TRANS_STOP_COMMAND},
    };

    if (i2c_dev->master_bus->async_trans == false) {
        ESP_RETURN_ON_ERROR(s_i2c_synchronous_transaction(i2c_dev, i2c_ops, DIM(i2c_ops), xfer_timeout_ms), TAG, "I2C transaction failed");
    } else {
        ESP_RETURN_ON_ERROR(s_i2c_asynchronous_transaction(i2c_dev, i2c_ops, DIM(i2c_ops), xfer_timeout_ms), TAG, "I2C transaction failed");
    }
    return ESP_OK;
}

esp_err_t i2c_master_transmit_receive(i2c_master_dev_handle_t i2c_dev, const uint8_t *write_buffer, size_t write_size, uint8_t *read_buffer, size_t read_size, int xfer_timeout_ms)
{
    ESP_RETURN_ON_FALSE(i2c_dev != NULL, ESP_ERR_INVALID_ARG, TAG, "i2c handle not initialized");
    ESP_RETURN_ON_FALSE((write_buffer != NULL) && (write_size > 0), ESP_ERR_INVALID_ARG, TAG, "i2c transmit buffer or size invalid");
    ESP_RETURN_ON_FALSE((read_buffer != NULL) && (read_size > 0), ESP_ERR_INVALID_ARG, TAG, "i2c receive buffer or size invalid");

    i2c_operation_t i2c_ops[] = {
        {.hw_cmd = I2C_TRANS_START_COMMAND},
        {.hw_cmd = I2C_TRANS_WRITE_COMMAND(i2c_dev->ack_check_disable ? false : true), .data = (uint8_t *)write_buffer, .total_bytes = write_size},
        {.hw_cmd = I2C_TRANS_START_COMMAND},
        {.hw_cmd = I2C_TRANS_READ_COMMAND(ACK_VAL), .data = read_buffer, .total_bytes = read_size - 1},
        {.hw_cmd = I2C_TRANS_READ_COMMAND(NACK_VAL), .data = (read_buffer + read_size - 1), .total_bytes = 1},
        {.hw_cmd = I2C_TRANS_STOP_COMMAND},
    };

    if (i2c_dev->master_bus->async_trans == false) {
        ESP_RETURN_ON_ERROR(s_i2c_synchronous_transaction(i2c_dev, i2c_ops, DIM(i2c_ops), xfer_timeout_ms), TAG, "I2C transaction failed");
    } else {
        ESP_RETURN_ON_ERROR(s_i2c_asynchronous_transaction(i2c_dev, i2c_ops, DIM(i2c_ops), xfer_timeout_ms), TAG, "I2C transaction failed");
    }
    return ESP_OK;
}

esp_err_t i2c_master_receive(i2c_master_dev_handle_t i2c_dev, uint8_t *read_buffer, size_t read_size, int xfer_timeout_ms)
{
    ESP_RETURN_ON_FALSE(i2c_dev != NULL, ESP_ERR_INVALID_ARG, TAG, "i2c handle not initialized");
    ESP_RETURN_ON_FALSE((read_buffer != NULL) && (read_size > 0), ESP_ERR_INVALID_ARG, TAG, "i2c receive buffer or size invalid");

    i2c_operation_t i2c_ops[] = {
        {.hw_cmd = I2C_TRANS_START_COMMAND},
        {.hw_cmd = I2C_TRANS_READ_COMMAND(ACK_VAL), .data = read_buffer, .total_bytes = read_size - 1},
        {.hw_cmd = I2C_TRANS_READ_COMMAND(NACK_VAL), .data = (read_buffer + read_size - 1), .total_bytes = 1},
        {.hw_cmd = I2C_TRANS_STOP_COMMAND},
    };

    if (i2c_dev->master_bus->async_trans == false) {
        ESP_RETURN_ON_ERROR(s_i2c_synchronous_transaction(i2c_dev, i2c_ops, DIM(i2c_ops), xfer_timeout_ms), TAG, "I2C transaction failed");
    } else {
        ESP_RETURN_ON_ERROR(s_i2c_asynchronous_transaction(i2c_dev, i2c_ops, DIM(i2c_ops), xfer_timeout_ms), TAG, "I2C transaction failed");
    }
    return ESP_OK;
}

esp_err_t i2c_master_probe(i2c_master_bus_handle_t bus_handle, uint16_t address, int xfer_timeout_ms)
{
    ESP_RETURN_ON_FALSE(bus_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "i2c handle not initialized");
    TickType_t ticks_to_wait = (xfer_timeout_ms == -1) ? portMAX_DELAY : pdMS_TO_TICKS(xfer_timeout_ms);
    if (xSemaphoreTake(bus_handle->bus_lock_mux, ticks_to_wait) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }
    esp_err_t ret = ESP_OK;

    bus_handle->cmd_idx = 0;
    bus_handle->trans_idx = 0;
    bus_handle->trans_done = false;
    bus_handle->status = I2C_STATUS_IDLE;
    bus_handle->bypass_nack_log = true;
    i2c_hal_context_t *hal = &bus_handle->base->hal;
    i2c_operation_t i2c_ops[] = {
        {.hw_cmd = I2C_TRANS_START_COMMAND},
        {.hw_cmd = I2C_TRANS_STOP_COMMAND},
    };

    bus_handle->i2c_trans = (i2c_transaction_t) {
        .device_address = address,
        .ops = i2c_ops,
        .cmd_count = DIM(i2c_ops),
    };

    // I2C probe does not have i2c device module. So set the clock parameter independently
    // This will not influence device transaction.
    I2C_CLOCK_SRC_ATOMIC() {
        i2c_ll_set_source_clk(hal->dev, bus_handle->base->clk_src);
        i2c_hal_set_bus_timing(hal, 100000, bus_handle->base->clk_src, bus_handle->base->clk_src_freq_hz);
    }
    i2c_ll_master_set_fractional_divider(hal->dev, 0, 0);
    i2c_ll_enable_intr_mask(hal->dev, I2C_LL_MASTER_EVENT_INTR);
    i2c_ll_update(hal->dev);

    s_i2c_send_commands(bus_handle, ticks_to_wait);
    if (bus_handle->status == I2C_STATUS_ACK_ERROR) {
        ret = ESP_ERR_NOT_FOUND;
    } else if (bus_handle->status == I2C_STATUS_TIMEOUT) {
        ESP_LOGE(TAG, "probe device timeout. Please check if xfer_timeout_ms and pull-ups are correctly set up");
        ret = ESP_ERR_TIMEOUT;
    }

    // Reset the status to done, in order not influence next time transaction.
    bus_handle->status = I2C_STATUS_DONE;
    bus_handle->bypass_nack_log = false;
    i2c_ll_disable_intr_mask(hal->dev, I2C_LL_MASTER_EVENT_INTR);
    xSemaphoreGive(bus_handle->bus_lock_mux);
    return ret;
}

esp_err_t i2c_master_register_event_callbacks(i2c_master_dev_handle_t i2c_dev, const i2c_master_event_callbacks_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(i2c_dev != NULL, ESP_ERR_INVALID_ARG, TAG, "i2c handle not initialized");

    if (i2c_dev->master_bus->async_trans == false) {
        ESP_LOGE(TAG, "I2C transaction queue is not initialized, so you can't use callback here, please resister the bus again with trans_queue_depth != 0");
        return ESP_ERR_INVALID_STATE;
    }

#if CONFIG_I2C_ISR_IRAM_SAFE
    if (cbs->on_trans_done) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_trans_done), ESP_ERR_INVALID_ARG, TAG, "i2c trans done callback not in IRAM");
    }
    if (user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_data), ESP_ERR_INVALID_ARG, TAG, "user context not in internal RAM");
    }
#endif // CONFIG_I2C_ISR_IRAM_SAFE

    i2c_dev->on_trans_done = cbs->on_trans_done;
    i2c_dev->user_ctx = user_data;
    return ESP_OK;
}

esp_err_t i2c_master_bus_wait_all_done(i2c_master_bus_handle_t bus_handle, int timeout_ms)
{
    ESP_RETURN_ON_FALSE(bus_handle, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    TickType_t wait_ticks = timeout_ms < 0 ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    i2c_transaction_t t;

    size_t cnt = bus_handle->num_trans_inflight;
    for (size_t i = 0; i < cnt; i++) {
        ESP_RETURN_ON_FALSE(xQueueReceive(bus_handle->trans_queues[I2C_TRANS_QUEUE_COMPLETE], &t, wait_ticks) == pdTRUE,
                            ESP_ERR_TIMEOUT, TAG, "flush timeout");
        ESP_RETURN_ON_FALSE(xQueueSend(bus_handle->trans_queues[I2C_TRANS_QUEUE_READY], &t, 0) == pdTRUE,
                            ESP_ERR_INVALID_STATE, TAG, "ready queue full");
        bus_handle->num_trans_inflight--;
    }
    return ESP_OK;

}
