/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "ulp_riscv_i2c_ulp_core.h"
#include "ulp_riscv_utils.h"
#include "soc/rtc_i2c_reg.h"
#include "soc/rtc_i2c_struct.h"
#include "soc/rtc_io_reg.h"
#include "soc/sens_reg.h"
#include "hal/i2c_ll.h"
#include "sdkconfig.h"

#define I2C_CTRL_SLAVE_ADDR_MASK        (0xFF << 0)
#define I2C_CTRL_SLAVE_REG_ADDR_MASK    (0xFF << 11)
#define I2C_CTRL_MASTER_TX_DATA_MASK    (0xFF << 19)

#if CONFIG_IDF_TARGET_ESP32S3
#define ULP_I2C_CMD_RESTART 0                   /*!<I2C restart command */
#define ULP_I2C_CMD_WRITE   1                   /*!<I2C write command */
#define ULP_I2C_CMD_READ    2                   /*!<I2C read command */
#define ULP_I2C_CMD_STOP    3                   /*!<I2C stop command */
#define ULP_I2C_CMD_END     4                   /*!<I2C end command */
#else
#define ULP_I2C_CMD_RESTART I2C_LL_CMD_RESTART  /*!<I2C restart command */
#define ULP_I2C_CMD_WRITE   I2C_LL_CMD_WRITE    /*!<I2C write command */
#define ULP_I2C_CMD_READ    I2C_LL_CMD_READ     /*!<I2C read command */
#define ULP_I2C_CMD_STOP    I2C_LL_CMD_STOP     /*!<I2C stop command */
#define ULP_I2C_CMD_END     I2C_LL_CMD_END      /*!<I2C end command */
#endif // CONFIG_IDF_TARGET_ESP32S3

/* Read/Write timeout (number of iterationis) */
#define ULP_RISCV_I2C_RW_TIMEOUT            CONFIG_ULP_RISCV_I2C_RW_TIMEOUT

/*
 * The RTC I2C controller follows the I2C command registers to perform read/write operations.
 * The cmd registers have the following format:
 *
 *      31        30:14     13:11      10         9           8         7:0
 * |----------|----------|---------|---------|----------|------------|---------|
 * | CMD_DONE | Reserved |  OPCODE |ACK Value|ACK Expect|ACK Check En|Byte Num |
 * |----------|----------|---------|---------|----------|------------|---------|
 */
static void ulp_riscv_i2c_format_cmd(uint32_t cmd_idx, uint8_t op_code, uint8_t ack_val,
        uint8_t ack_expected, uint8_t ack_check_en, uint8_t byte_num)
{
    uint32_t reg_addr = RTC_I2C_CMD0_REG + 4*cmd_idx;

    CLEAR_PERI_REG_MASK(reg_addr, 0xFFFFFFFF);

    WRITE_PERI_REG(reg_addr,
            (0 << 31) |                     // CMD Done

            ((op_code & 0x3) << 11) |       // Opcode

            ((ack_val & 0x1) << 10) |       // ACK bit sent by I2C controller during READ.
                                            // Ignored during RSTART, STOP, END and WRITE cmds.

            ((ack_expected & 0x1) << 9) |   // ACK bit expected by I2C controller during WRITE.
                                            // Ignored during RSTART, STOP, END and READ cmds.

            ((ack_check_en & 0x1) << 8) |   // I2C controller verifies that the ACK bit sent by the slave device matches
                                            // the ACK expected bit during WRITE.
                                            // Ignored during RSTART, STOP, END and READ cmds.
            ((byte_num & 0xFF) << 0));      // Byte Num
}

static inline int32_t ulp_riscv_i2c_wait_for_interrupt(int32_t ticks_to_wait)
{
    uint32_t status = 0;
    uint32_t to = 0;

    while (1) {
        status = READ_PERI_REG(RTC_I2C_INT_ST_REG);

        /* Return 0 if Tx or Rx data interrupt bits are set. */
        if ((status & RTC_I2C_TX_DATA_INT_ST) ||
            (status & RTC_I2C_RX_DATA_INT_ST)) {
            return 0;
        /* In case of error status, break and return -1 */
#if CONFIG_IDF_TARGET_ESP32S2
        } else if ((status & RTC_I2C_TIMEOUT_INT_ST) ||
#elif CONFIG_IDF_TARGET_ESP32S3
        } else if ((status & RTC_I2C_TIME_OUT_INT_ST) ||
#endif // CONFIG_IDF_TARGET_ESP32S2
                   (status & RTC_I2C_ACK_ERR_INT_ST) ||
                   (status & RTC_I2C_ARBITRATION_LOST_INT_ST)) {
            return -1;
        }

        if (ticks_to_wait > -1) {
            /* If the ticks_to_wait value is not -1, keep track of ticks and
             * break from the loop once the timeout is reached.
             */
            ulp_riscv_delay_cycles(1);
            to++;
            if (to >= ticks_to_wait) {
                return -1;
            }
        }
    }
}

void ulp_riscv_i2c_master_set_slave_addr(uint8_t slave_addr)
{
    CLEAR_PERI_REG_MASK(SENS_SAR_I2C_CTRL_REG, I2C_CTRL_SLAVE_ADDR_MASK);
    SET_PERI_REG_BITS(SENS_SAR_I2C_CTRL_REG, 0xFF, slave_addr, 0);
}

void ulp_riscv_i2c_master_set_slave_reg_addr(uint8_t slave_reg_addr)
{
    CLEAR_PERI_REG_MASK(SENS_SAR_I2C_CTRL_REG, I2C_CTRL_SLAVE_REG_ADDR_MASK);
    SET_PERI_REG_BITS(SENS_SAR_I2C_CTRL_REG, 0xFF, slave_reg_addr, 11);
}

/*
 * I2C transactions when master reads one byte of data from the slave device:
 *
 * |--------|--------|---------|--------|--------|--------|--------|---------|--------|--------|--------|--------|
 * | Master | START  | SAD + W |        |  SUB   |        |   SR   | SAD + R |        |        |  NACK  |  STOP  |
 * |--------|--------|---------|--------|--------|--------|--------|---------|--------|--------|--------|--------|
 * | Slave  |        |         |  ACK   |        |   ACK  |        |         |   ACK  |  DATA  |        |        |
 * |--------|--------|---------|--------|--------|--------|--------|---------|--------|--------|--------|--------|
 *
 * I2C transactions when master reads multiple bytes of data from the slave device:
 *
 * |--------|--------|---------|--------|--------|--------|--------|---------|--------|--------|--------|--------|--------|--------|
 * | Master | START  | SAD + W |        |  SUB   |        |   SR   | SAD + R |        |        |   ACK  |        |   NACK |  STOP  |
 * |--------|--------|---------|--------|--------|--------|--------|---------|--------|--------|--------|--------|--------|--------|
 * | Slave  |        |         |  ACK   |        |   ACK  |        |         |   ACK  |  DATA  |        |  DATA  |        |        |
 * |--------|--------|---------|--------|--------|--------|--------|---------|--------|--------|--------|--------|--------|--------|
 */
void ulp_riscv_i2c_master_read_from_device(uint8_t *data_rd, size_t size)
{
    uint32_t i = 0;
    uint32_t cmd_idx = 0;

    if (size == 0) {
        // Quietly return
        return;
    }

    /* By default, RTC I2C controller is hard wired to use CMD2 register onwards for read operations */
    cmd_idx = 2;

    /* Write slave addr */
    ulp_riscv_i2c_format_cmd(cmd_idx++, ULP_I2C_CMD_WRITE, 0, 0, 1, 2);

    /* Repeated START */
    ulp_riscv_i2c_format_cmd(cmd_idx++, ULP_I2C_CMD_RESTART, 0, 0, 0, 0);

    /* Write slave register addr */
    ulp_riscv_i2c_format_cmd(cmd_idx++, ULP_I2C_CMD_WRITE, 0, 0, 1, 1);

    if (size > 1) {
        /* Read n - 1 bytes */
        ulp_riscv_i2c_format_cmd(cmd_idx++, ULP_I2C_CMD_READ, 0, 0, 1, size - 1);
    }

    /* Read last byte + NACK */
    ulp_riscv_i2c_format_cmd(cmd_idx++, ULP_I2C_CMD_READ, 1, 1, 1, 1);

    /* STOP */
    ulp_riscv_i2c_format_cmd(cmd_idx++, ULP_I2C_CMD_STOP, 0, 0, 0, 0);

    /* Configure the RTC I2C controller in read mode */
    SET_PERI_REG_BITS(SENS_SAR_I2C_CTRL_REG, 0x1, 0, 27);

    /* Enable Rx data interrupt */
    SET_PERI_REG_MASK(RTC_I2C_INT_ENA_REG, RTC_I2C_RX_DATA_INT_ENA);

    /* Start RTC I2C transmission */
    SET_PERI_REG_MASK(SENS_SAR_I2C_CTRL_REG, SENS_SAR_I2C_START_FORCE);
    SET_PERI_REG_MASK(SENS_SAR_I2C_CTRL_REG, SENS_SAR_I2C_START);

    for (i = 0; i < size; i++) {
        /* Poll for RTC I2C Rx Data interrupt bit to be set */
        if(!ulp_riscv_i2c_wait_for_interrupt(ULP_RISCV_I2C_RW_TIMEOUT)) {
            /* Read the data
             *
             * Unfortunately, the RTC I2C has no fifo buffer to help us with reading and storing
             * multiple bytes of data. Therefore, we need to read one byte at a time and clear the
             * Rx interrupt to get ready for the next byte.
             */
#if CONFIG_IDF_TARGET_ESP32S2
            data_rd[i] = REG_GET_FIELD(RTC_I2C_DATA_REG, RTC_I2C_RDATA);
#elif CONFIG_IDF_TARGET_ESP32S3
            data_rd[i] = REG_GET_FIELD(RTC_I2C_DATA_REG, RTC_I2C_I2C_RDATA);
#endif // CONFIG_IDF_TARGET_ESP32S2

            /* Clear the Rx data interrupt bit */
            SET_PERI_REG_MASK(RTC_I2C_INT_CLR_REG, RTC_I2C_RX_DATA_INT_CLR);
        } else {
            /* Error in transaction */
            CLEAR_PERI_REG_MASK(RTC_I2C_INT_CLR_REG, READ_PERI_REG(RTC_I2C_INT_ST_REG));
            break;
        }
    }

    /* Clear the RTC I2C transmission bits */
    CLEAR_PERI_REG_MASK(SENS_SAR_I2C_CTRL_REG, SENS_SAR_I2C_START_FORCE);
    CLEAR_PERI_REG_MASK(SENS_SAR_I2C_CTRL_REG, SENS_SAR_I2C_START);
}

/*
 * I2C transactions when master writes one byte of data to the slave device:
 *
 * |--------|--------|---------|--------|--------|--------|--------|--------|--------|
 * | Master | START  | SAD + W |        |  SUB   |        |  DATA  |        |  STOP  |
 * |--------|--------|---------|--------|--------|--------|--------|--------|--------|
 * | Slave  |        |         |  ACK   |        |   ACK  |        |   ACK  |        |
 * |--------|--------|---------|--------|--------|--------|--------|--------|--------|
 *
 * I2C transactions when master writes multiple bytes of data to the slave device:
 *
 * |--------|--------|---------|--------|--------|--------|--------|--------|--------|--------|--------|
 * | Master | START  | SAD + W |        |  SUB   |        |  DATA  |        |  DATA  |        |  STOP  |
 * |--------|--------|---------|--------|--------|--------|--------|--------|--------|--------|--------|
 * | Slave  |        |         |  ACK   |        |   ACK  |        |   ACK  |        |   ACK  |        |
 * |--------|--------|---------|--------|--------|--------|--------|--------|--------|--------|--------|
 */
void ulp_riscv_i2c_master_write_to_device(uint8_t *data_wr, size_t size)
{
    uint32_t i = 0;
    uint32_t cmd_idx = 0;

    if (size == 0) {
        // Quietly return
        return;
    }

    /* By default, RTC I2C controller is hard wired to use CMD0 and CMD1 registers for write operations */
    cmd_idx = 0;

    /* Write slave addr + reg addr + data */
    ulp_riscv_i2c_format_cmd(cmd_idx++, ULP_I2C_CMD_WRITE, 0, 0, 1, 2 + size);

    /* Stop */
    ulp_riscv_i2c_format_cmd(cmd_idx++, ULP_I2C_CMD_STOP, 0, 0, 0, 0);

    /* Configure the RTC I2C controller in write mode */
    SET_PERI_REG_BITS(SENS_SAR_I2C_CTRL_REG, 0x1, 1, 27);

    /* Enable Tx data interrupt */
    SET_PERI_REG_MASK(RTC_I2C_INT_ENA_REG, RTC_I2C_TX_DATA_INT_ENA);

    for (i = 0; i < size; i++) {
        /* Write the data to be transmitted */
        CLEAR_PERI_REG_MASK(SENS_SAR_I2C_CTRL_REG, I2C_CTRL_MASTER_TX_DATA_MASK);
        SET_PERI_REG_BITS(SENS_SAR_I2C_CTRL_REG, 0xFF, data_wr[i], 19);

        if (i == 0) {
            /* Start RTC I2C transmission. (Needn't do it for every byte) */
            SET_PERI_REG_MASK(SENS_SAR_I2C_CTRL_REG, SENS_SAR_I2C_START_FORCE);
            SET_PERI_REG_MASK(SENS_SAR_I2C_CTRL_REG, SENS_SAR_I2C_START);
        }

        /* Poll for RTC I2C Tx Data interrupt bit to be set */
        if (!ulp_riscv_i2c_wait_for_interrupt(ULP_RISCV_I2C_RW_TIMEOUT)) {
            /* Clear the Tx data interrupt bit */
            SET_PERI_REG_MASK(RTC_I2C_INT_CLR_REG, RTC_I2C_TX_DATA_INT_CLR);
        } else {
            SET_PERI_REG_MASK(RTC_I2C_INT_CLR_REG, READ_PERI_REG(RTC_I2C_INT_ST_REG));
            break;
        }
    }

    /* Clear the RTC I2C transmission bits */
    CLEAR_PERI_REG_MASK(SENS_SAR_I2C_CTRL_REG, SENS_SAR_I2C_START_FORCE);
    CLEAR_PERI_REG_MASK(SENS_SAR_I2C_CTRL_REG, SENS_SAR_I2C_START);
}
