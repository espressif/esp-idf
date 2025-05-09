/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "ulp_lp_core_i2c.h"
#include "ulp_lp_core_utils.h"
#include "soc/lp_i2c_reg.h"
#include "soc/i2c_struct.h"
#include "hal/i2c_ll.h"

#if SOC_LP_I2C_SUPPORTED

#define LP_I2C_FIFO_LEN     SOC_LP_I2C_FIFO_LEN
#define LP_I2C_READ_MODE    I2C_MASTER_READ
#define LP_I2C_WRITE_MODE   I2C_MASTER_WRITE
#define LP_I2C_ACK          I2C_MASTER_ACK
#define LP_I2C_NACK         I2C_MASTER_NACK

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

/* I2C LL context */

i2c_dev_t *dev = I2C_LL_GET_HW(LP_I2C_NUM_0);

/* ACK check enable control variable. Enabled by default */
static bool s_ack_check_en = true;

/*
 * The LP I2C controller uses the LP I2C HW command registers to perform read/write operations.
 * The cmd registers have the following format:
 *
 *      31        30:14     13:11      10         9           8         7:0
 * |----------|----------|---------|---------|----------|------------|---------|
 * | CMD_DONE | Reserved |  OPCODE |ACK Value|ACK Expect|ACK Check En|Byte Num |
 * |----------|----------|---------|---------|----------|------------|---------|
 */
static void lp_core_i2c_format_cmd(uint32_t cmd_idx, uint8_t op_code, uint8_t ack_val,
                                   uint8_t ack_expected, uint8_t ack_check_en, uint8_t byte_num)
{
    if (cmd_idx >= sizeof(dev->command)) {
        /* We only have limited HW command registers.
         * Although unlikely, make sure that we do not write to an out of bounds index.
         */
        return;
    }

    /* Form new command */
    i2c_ll_hw_cmd_t hw_cmd = {
        .done = 0,                // CMD Done
        .op_code = op_code,       // Opcode
        .ack_val = ack_val,       // ACK bit sent by I2C controller during READ.
        // Ignored during RSTART, STOP, END and WRITE cmds.
        .ack_exp = ack_expected,  // ACK bit expected by I2C controller during WRITE.
        // Ignored during RSTART, STOP, END and READ cmds.
        .ack_en = ack_check_en,   // I2C controller verifies that the ACK bit sent by the
        // slave device matches the ACK expected bit during WRITE.
        // Ignored during RSTART, STOP, END and READ cmds.
        .byte_num = byte_num,     // Byte Num
    };

    /* Write new command to cmd register */
    i2c_ll_master_write_cmd_reg(dev, hw_cmd, cmd_idx);
}

static inline esp_err_t lp_core_i2c_wait_for_interrupt(uint32_t intr_mask, int32_t ticks_to_wait)
{
    uint32_t intr_status = 0;
    uint32_t to = 0;

    while (1) {
        i2c_ll_get_intr_raw_mask(dev, &intr_status);
        if (intr_status & intr_mask) {
            if (intr_status & LP_I2C_NACK_INT_ST) {
                /* The ACK/NACK received during a WRITE operation does not match the expected ACK/NACK level
                 * Abort and return an error.
                 */
                i2c_ll_clear_intr_mask(dev, intr_mask);
                return ESP_ERR_INVALID_RESPONSE;
            } else if (intr_status & LP_I2C_TRANS_COMPLETE_INT_ST_M) {
                /* Transaction complete.
                 * Clear interrupt bits and break
                 */
                i2c_ll_clear_intr_mask(dev, intr_mask);
                break;
            } else {
                /* We received an I2C_END_DETECT_INT.
                 * This means we are not yet done with the transaction.
                 * Simply clear the interrupt bit and break.
                 */
                i2c_ll_clear_intr_mask(dev, intr_mask);
                break;
            }
            break;
        }

        if (ticks_to_wait > -1) {
            /* If the ticks_to_wait value is not -1, keep track of ticks and
             * break from the loop once the timeout is reached.
             */
            ulp_lp_core_delay_cycles(1);
            to++;
            if (to >= ticks_to_wait) {
                /* Timeout. Clear interrupt bits and return an error */
                i2c_ll_clear_intr_mask(dev, intr_mask);
                return ESP_ERR_TIMEOUT;
            }
        }
    }

    /* We reach here only if we are in a good state */
    return ESP_OK;
}

static inline void lp_core_i2c_config_device_addr(uint32_t cmd_idx, uint16_t device_addr,  uint32_t rw_mode, uint8_t *addr_len)
{
    uint8_t data_byte = 0;
    uint8_t data_len = 0;

    /* 7-bit addressing mode. We do not support 10-bit addressing mode yet (IDF-7364) */

    // Write the device address + R/W mode in the first Tx FIFO slot
    data_byte = (uint8_t)(((device_addr & 0xFF) << 1) | (rw_mode << 0));
    i2c_ll_write_txfifo(dev, &data_byte, 1);
    data_len++;

    /* Update the HW command register. Expect an ACK from the device */
    lp_core_i2c_format_cmd(cmd_idx, I2C_LL_CMD_WRITE, 0, LP_I2C_ACK, s_ack_check_en, data_len);

    /* Return the address length in bytes */
    *addr_len = data_len;
}

void lp_core_i2c_master_set_ack_check_en(i2c_port_t lp_i2c_num, bool ack_check_en)
{
    (void)lp_i2c_num;

    s_ack_check_en = ack_check_en;
}

esp_err_t lp_core_i2c_master_read_from_device(i2c_port_t lp_i2c_num, uint16_t device_addr,
                                              uint8_t *data_rd, size_t size,
                                              int32_t ticks_to_wait)
{
    (void)lp_i2c_num;

    esp_err_t ret = ESP_OK;
    uint32_t cmd_idx = 0;

    if (size == 0) {
        // Quietly return
        return ESP_OK;
    } else if (size > UINT8_MAX) {
        // HW register only has an 8-bit byte-num field
        return ESP_ERR_INVALID_SIZE;
    }

    /* Execute RSTART command to send the START bit */
    lp_core_i2c_format_cmd(cmd_idx++, I2C_LL_CMD_RESTART, 0, 0, 0, 0);

    /* Write device addr and update the HW command register */
    uint8_t addr_len = 0;
    lp_core_i2c_config_device_addr(cmd_idx++, device_addr, LP_I2C_READ_MODE, &addr_len);

    /* Enable trans complete interrupt and end detect interrupt for read/write operation */
    uint32_t intr_mask = (1 << LP_I2C_TRANS_COMPLETE_INT_ST_S) | (1 << LP_I2C_END_DETECT_INT_ST_S);
    i2c_ll_clear_intr_mask(dev, intr_mask);

    /* Read data */
    uint32_t fifo_size = 0;
    uint32_t data_idx = 0;
    int32_t remaining_bytes = size;

    /* The data is received in sequential slots of the Rx FIFO.
     * We must account for FIFO wraparound in case the length of data being received is greater than LP_I2C_FIFO_LEN.
     */
    while (remaining_bytes > 0) {
        /* Select the amount of data that fits in the Rx FIFO */
        fifo_size = MIN(remaining_bytes, LP_I2C_FIFO_LEN);

        /* Update the number of bytes remaining to be read */
        remaining_bytes -= fifo_size;

        /* Update HW command register to read bytes */
        if (fifo_size == 1) {
            /* Read 1 byte and send NACK */
            lp_core_i2c_format_cmd(cmd_idx++, I2C_LL_CMD_READ, LP_I2C_NACK, 0, 0, 1);

            /* STOP */
            lp_core_i2c_format_cmd(cmd_idx++, I2C_LL_CMD_STOP, 0, 0, 0, 0);
        } else if ((fifo_size > 1) && (remaining_bytes == 0)) {
            /* This means it is the last transaction.
             * Read fifo_size - 1 bytes and send ACKs
             */
            lp_core_i2c_format_cmd(cmd_idx++, I2C_LL_CMD_READ, LP_I2C_ACK, 0, 0, fifo_size - 1);

            /* Read last byte and send NACK */
            lp_core_i2c_format_cmd(cmd_idx++, I2C_LL_CMD_READ, LP_I2C_NACK, 0, 0, 1);

            /* STOP */
            lp_core_i2c_format_cmd(cmd_idx++, I2C_LL_CMD_STOP, 0, 0, 0, 0);
        } else {
            /* This means we have to read data more than what can fit in the Rx FIFO.
             * Read fifo_size bytes and send ACKs
             */
            lp_core_i2c_format_cmd(cmd_idx++, I2C_LL_CMD_READ, LP_I2C_ACK, 0, 0, fifo_size);
            lp_core_i2c_format_cmd(cmd_idx++, I2C_LL_CMD_END, 0, 0, 0, 0);
            cmd_idx = 0;
        }

        /* Initiate I2C transfer */
        i2c_ll_update(dev);
        i2c_ll_start_trans(dev);

        /* Wait for the transfer to complete */
        ret = lp_core_i2c_wait_for_interrupt(intr_mask, ticks_to_wait);
        if (ret != ESP_OK) {
            /* Transaction error. Abort. */
            return ret;
        }

        /* Read Rx FIFO */
        i2c_ll_read_rxfifo(dev, &data_rd[data_idx], fifo_size);

        /* Update data_idx */
        data_idx += fifo_size;
    }

    return ret;
}

esp_err_t lp_core_i2c_master_write_to_device(i2c_port_t lp_i2c_num, uint16_t device_addr,
                                             const uint8_t *data_wr, size_t size,
                                             int32_t ticks_to_wait)
{
    (void)lp_i2c_num;

    esp_err_t ret = ESP_OK;
    uint32_t cmd_idx = 0;

    if (size == 0) {
        // Quietly return
        return ESP_OK;
    } else if (size > UINT8_MAX) {
        // HW register only has an 8-bit byte-num field
        return ESP_ERR_INVALID_SIZE;
    }

    /* If SCL is busy, reset the Master FSM */
    if (i2c_ll_is_bus_busy(dev)) {
        i2c_ll_master_fsm_rst(dev);
    }

    /* Reset the Tx and Rx FIFOs */
    i2c_ll_txfifo_rst(dev);
    i2c_ll_rxfifo_rst(dev);

    /* Execute RSTART command to send the START bit */
    lp_core_i2c_format_cmd(cmd_idx++, I2C_LL_CMD_RESTART, 0, 0, 0, 0);

    /* Write device addr and update the HW command register */
    uint8_t addr_len = 0;
    lp_core_i2c_config_device_addr(cmd_idx++, device_addr, LP_I2C_WRITE_MODE, &addr_len);

    /* Enable trans complete interrupt and end detect interrupt for read/write operation */
    uint32_t intr_mask = (1 << LP_I2C_TRANS_COMPLETE_INT_ST_S) | (1 << LP_I2C_END_DETECT_INT_ST_S);
    if (s_ack_check_en) {
        /* Enable LP_I2C_NACK_INT to check for ACK errors */
        intr_mask |= (1 << LP_I2C_NACK_INT_ST_S);
    }
    i2c_ll_clear_intr_mask(dev, intr_mask);

    /* Write data */
    uint32_t fifo_available = LP_I2C_FIFO_LEN - addr_len; // Initially, 1 or 2 fifo slots are taken by the device address
    uint32_t fifo_size = 0;
    uint32_t data_idx = 0;
    int32_t remaining_bytes = size;

    /* The data to be sent must occupy sequential slots of the Tx FIFO.
     * We must account for FIFO wraparound in case the length of data being sent is greater than LP_I2C_FIFO_LEN.
     */
    while (remaining_bytes > 0) {
        /* Select the amount of data that fits in the Tx FIFO */
        fifo_size = MIN(remaining_bytes, fifo_available);

        /* Update the number of bytes remaining to be sent */
        remaining_bytes -= fifo_size;

        /* Write data to the Tx FIFO and update the HW command register. Expect ACKs from the device */
        i2c_ll_write_txfifo(dev, &data_wr[data_idx], fifo_size);
        lp_core_i2c_format_cmd(cmd_idx++, I2C_LL_CMD_WRITE, 0, LP_I2C_ACK, s_ack_check_en, fifo_size);

        if (remaining_bytes == 0) {
            /* This means it is the last transaction. Insert a Stop command. */
            lp_core_i2c_format_cmd(cmd_idx++, I2C_LL_CMD_STOP, 0, 0, 0, 0);
        } else {
            /* This means we have to send more than what can fit in the Tx FIFO. Insert an End command. */
            lp_core_i2c_format_cmd(cmd_idx++, I2C_LL_CMD_END, 0, 0, 0, 0);
            cmd_idx = 0;
        }

        /* Initiate I2C transfer */
        i2c_ll_update(dev);
        i2c_ll_start_trans(dev);

        /* Wait for the transfer to complete */
        ret = lp_core_i2c_wait_for_interrupt(intr_mask, ticks_to_wait);
        if (ret != ESP_OK) {
            /* Transaction error. Abort. */
            return ret;
        }

        /* Update data_idx */
        data_idx += fifo_size;

        /* We now have the full fifo available for writing */
        fifo_available = LP_I2C_FIFO_LEN;
    }

    return ret;
}

esp_err_t lp_core_i2c_master_write_read_device(i2c_port_t lp_i2c_num, uint16_t device_addr,
                                               const uint8_t *data_wr, size_t write_size,
                                               uint8_t *data_rd, size_t read_size,
                                               int32_t ticks_to_wait)
{
    (void)lp_i2c_num;

    esp_err_t ret = ESP_OK;
    uint32_t cmd_idx = 0;

    if ((write_size == 0) || (read_size == 0)) {
        // Quietly return
        return ESP_OK;
    } else if ((write_size > UINT8_MAX) || (read_size > UINT8_MAX)) {
        // HW register only has an 8-bit byte-num field
        return ESP_ERR_INVALID_SIZE;
    }

    /* If SCL is busy, reset the Master FSM */
    if (i2c_ll_is_bus_busy(dev)) {
        i2c_ll_master_fsm_rst(dev);
    }

    /* Reset the Tx and Rx FIFOs */
    i2c_ll_txfifo_rst(dev);
    i2c_ll_rxfifo_rst(dev);

    /* Enable trans complete interrupt and end detect interrupt for read/write operation */
    uint32_t intr_mask = (1 << LP_I2C_TRANS_COMPLETE_INT_ST_S) | (1 << LP_I2C_END_DETECT_INT_ST_S);
    if (s_ack_check_en) {
        /* Enable LP_I2C_NACK_INT to check for ACK errors */
        intr_mask |= (1 << LP_I2C_NACK_INT_ST_S);
    }
    i2c_ll_clear_intr_mask(dev, intr_mask);

    /* Execute RSTART command to send the START bit */
    lp_core_i2c_format_cmd(cmd_idx++, I2C_LL_CMD_RESTART, 0, 0, 0, 0);

    /* Write device addr and update the HW command register */
    uint8_t addr_len = 0;
    lp_core_i2c_config_device_addr(cmd_idx++, device_addr, LP_I2C_WRITE_MODE, &addr_len);

    /* Write data */
    uint32_t fifo_available = LP_I2C_FIFO_LEN - addr_len; // Initially, 1 or 2 fifo slots are taken by the device address
    uint32_t fifo_size = 0;
    uint32_t data_idx = 0;
    int32_t remaining_bytes = write_size;

    /* The data to be sent must occupy sequential slots of the Tx FIFO.
     * We must account for FIFO wraparound in case the length of data being sent is greater than LP_I2C_FIFO_LEN.
     */
    while (remaining_bytes > 0) {
        /* Select the amount of data that fits in the Tx FIFO */
        fifo_size = MIN(remaining_bytes, fifo_available);

        /* Update the number of bytes remaining to be sent */
        remaining_bytes -= fifo_size;

        /* Write data to the Tx FIFO and update the HW command register. Expect ACKs from the device */
        i2c_ll_write_txfifo(dev, &data_wr[data_idx], fifo_size);
        lp_core_i2c_format_cmd(cmd_idx++, I2C_LL_CMD_WRITE, 0, LP_I2C_ACK, s_ack_check_en, fifo_size);

        /* Insert an End command to signal the end of the write transaction to the HW */
        lp_core_i2c_format_cmd(cmd_idx++, I2C_LL_CMD_END, 0, 0, 0, 0);
        cmd_idx = 0;

        /* Initiate I2C transfer */
        i2c_ll_update(dev);
        i2c_ll_start_trans(dev);

        /* Wait for the transfer to complete */
        ret = lp_core_i2c_wait_for_interrupt(intr_mask, ticks_to_wait);
        if (ret != ESP_OK) {
            /* Transaction error. Abort. */
            return ret;
        }

        /* Update data_idx */
        data_idx += fifo_size;

        /* We now have the full fifo available for writing */
        fifo_available = LP_I2C_FIFO_LEN;
    }

    /* Reset command index */
    cmd_idx = 0;

    /* Execute RSTART command again to send a START condition for the read operation */
    lp_core_i2c_format_cmd(cmd_idx++, I2C_LL_CMD_RESTART, 0, 0, 0, 0);

    /* Write device addr again in read mode */
    lp_core_i2c_config_device_addr(cmd_idx++, device_addr, LP_I2C_READ_MODE, &addr_len);

    /* Read data */
    fifo_size = 0;
    data_idx = 0;
    remaining_bytes = read_size;

    /* The data is received in sequential slots of the Rx FIFO.
     * We must account for FIFO wraparound in case the length of data being received is greater than LP_I2C_FIFO_LEN.
     */
    while (remaining_bytes > 0) {
        /* Select the amount of data that fits in the Rx FIFO */
        fifo_size = MIN(remaining_bytes, LP_I2C_FIFO_LEN);

        /* Update the number of bytes remaining to be read */
        remaining_bytes -= fifo_size;

        /* Update HW command register to read bytes */
        if (fifo_size == 1) {
            /* Read 1 byte and send NACK */
            lp_core_i2c_format_cmd(cmd_idx++, I2C_LL_CMD_READ, LP_I2C_NACK, 0, 0, 1);

            /* STOP */
            lp_core_i2c_format_cmd(cmd_idx++, I2C_LL_CMD_STOP, 0, 0, 0, 0);
        } else if ((fifo_size > 1) && (remaining_bytes == 0)) {
            /* This means it is the last transaction.
             * Read fifo_size - 1 bytes and send ACKs
             */
            lp_core_i2c_format_cmd(cmd_idx++, I2C_LL_CMD_READ, LP_I2C_ACK, 0, 0, fifo_size - 1);

            /* Read last byte and send NACK */
            lp_core_i2c_format_cmd(cmd_idx++, I2C_LL_CMD_READ, LP_I2C_NACK, 0, 0, 1);

            /* STOP */
            lp_core_i2c_format_cmd(cmd_idx++, I2C_LL_CMD_STOP, 0, 0, 0, 0);
        } else {
            /* This means we have to read data more than what can fit in the Rx FIFO.
             * Read fifo_size bytes and send ACKs
             */
            lp_core_i2c_format_cmd(cmd_idx++, I2C_LL_CMD_READ, LP_I2C_ACK, 0, 0, fifo_size);
            lp_core_i2c_format_cmd(cmd_idx++, I2C_LL_CMD_END, 0, 0, 0, 0);
            cmd_idx = 0;
        }

        /* Initiate I2C transfer */
        i2c_ll_update(dev);
        i2c_ll_start_trans(dev);

        /* Wait for the transfer to complete */
        ret = lp_core_i2c_wait_for_interrupt(intr_mask, ticks_to_wait);
        if (ret != ESP_OK) {
            /* Transaction error. Abort. */
            return ret;
        }

        /* Read Rx FIFO */
        i2c_ll_read_rxfifo(dev, &data_rd[data_idx], fifo_size);

        /* Update data_idx */
        data_idx += fifo_size;
    }

    return ret;
}

#endif /* SOC_LP_I2C_SUPPORTED */
