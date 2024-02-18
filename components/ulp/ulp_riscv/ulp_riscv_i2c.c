/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ulp_riscv_i2c.h"
#include "esp_check.h"
#include "soc/rtc_i2c_reg.h"
#include "soc/rtc_i2c_struct.h"
#include "soc/rtc_io_struct.h"
#include "soc/sens_reg.h"
#include "soc/clk_tree_defs.h"
#include "hal/i2c_ll.h"
#include "hal/misc.h"
#include "driver/rtc_io.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

static const char *RTCI2C_TAG = "ulp_riscv_i2c";

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

/* Use the register structure to access RTC_I2C and RTCIO module registers */
rtc_i2c_dev_t *i2c_dev = &RTC_I2C;
rtc_io_dev_t *rtc_io_dev = &RTCIO;

#define MICROSEC_TO_RTC_FAST_CLK(period)    (period) * ((float)(SOC_CLK_RC_FAST_FREQ_APPROX) / (1000000.0))

/* Read/Write timeout (number of iterations)*/
#define ULP_RISCV_I2C_RW_TIMEOUT            CONFIG_ULP_RISCV_I2C_RW_TIMEOUT

static esp_err_t i2c_gpio_is_cfg_valid(gpio_num_t sda_io_num, gpio_num_t scl_io_num)
{
    /* Verify that the SDA and SCL GPIOs are valid RTC I2C io pins */
    ESP_RETURN_ON_ERROR(!rtc_gpio_is_valid_gpio(sda_io_num), RTCI2C_TAG, "RTC I2C SDA GPIO invalid");
    ESP_RETURN_ON_ERROR(!rtc_gpio_is_valid_gpio(scl_io_num), RTCI2C_TAG, "RTC I2C SCL GPIO invalid");

    /* Verify that the SDA and SCL line belong to the RTC IO I2C function group */
    if ((sda_io_num != GPIO_NUM_1) && (sda_io_num != GPIO_NUM_3)) {
        ESP_LOGE(RTCI2C_TAG, "SDA pin can only be configured as GPIO#1 or GPIO#3");
        return ESP_ERR_INVALID_ARG;
    }

    if ((scl_io_num != GPIO_NUM_0) && (scl_io_num != GPIO_NUM_2)) {
        ESP_LOGE(RTCI2C_TAG, "SCL pin can only be configured as GPIO#0 or GPIO#2");
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

static esp_err_t i2c_configure_io(gpio_num_t io_num, bool pullup_en)
{
    /* Initialize IO Pin */
    ESP_RETURN_ON_ERROR(rtc_gpio_init(io_num), RTCI2C_TAG, "RTC GPIO Init failed for GPIO %d", io_num);
    /* Set direction to input+output */
    ESP_RETURN_ON_ERROR(rtc_gpio_set_direction(io_num, RTC_GPIO_MODE_INPUT_OUTPUT_OD), RTCI2C_TAG, "RTC GPIO Set direction failed for %d", io_num);
    /* Disable pulldown on the io pin */
    ESP_RETURN_ON_ERROR(rtc_gpio_pulldown_dis(io_num), RTCI2C_TAG, "RTC GPIO pulldown disable failed for %d", io_num);
    /* Enable pullup based on pullup_en flag */
    if (pullup_en) {
        ESP_RETURN_ON_ERROR(rtc_gpio_pullup_en(io_num), RTCI2C_TAG, "RTC GPIO pullup enable failed for %d", io_num);
    } else {
        ESP_RETURN_ON_ERROR(rtc_gpio_pullup_dis(io_num), RTCI2C_TAG, "RTC GPIO pullup disable failed for %d", io_num);
    }

    return ESP_OK;
}

static esp_err_t i2c_set_pin(const ulp_riscv_i2c_cfg_t *cfg)
{
    gpio_num_t sda_io_num = cfg->i2c_pin_cfg.sda_io_num;
    gpio_num_t scl_io_num = cfg->i2c_pin_cfg.scl_io_num;
    bool sda_pullup_en = cfg->i2c_pin_cfg.sda_pullup_en;
    bool scl_pullup_en = cfg->i2c_pin_cfg.scl_pullup_en;

    /* Verify that the I2C GPIOs are valid */
    ESP_RETURN_ON_ERROR(i2c_gpio_is_cfg_valid(sda_io_num, scl_io_num), RTCI2C_TAG, "RTC I2C GPIO config invalid");

    /* Initialize SDA Pin */
    ESP_RETURN_ON_ERROR(i2c_configure_io(sda_io_num, sda_pullup_en), RTCI2C_TAG, "RTC I2C SDA pin config failed");

    /* Initialize SCL Pin */
    ESP_RETURN_ON_ERROR(i2c_configure_io(scl_io_num, scl_pullup_en), RTCI2C_TAG, "RTC I2C SCL pin config failed");

    /* Route SDA IO signal to the RTC subsystem */
    rtc_io_dev->touch_pad[sda_io_num].mux_sel = 1;

    /* Route SCL IO signal to the RTC subsystem */
    rtc_io_dev->touch_pad[scl_io_num].mux_sel = 1;

    /* Select RTC I2C function for SDA pin */
    rtc_io_dev->touch_pad[sda_io_num].fun_sel = 3;

    /* Select RTC I2C function for SCL pin */
    rtc_io_dev->touch_pad[scl_io_num].fun_sel = 3;

    /* Map the SDA and SCL signals to the RTC I2C controller */
    if (sda_io_num == GPIO_NUM_1) {
        rtc_io_dev->sar_i2c_io.sda_sel = 0;
    } else {
        rtc_io_dev->sar_i2c_io.sda_sel = 1;
    }

    if (scl_io_num == GPIO_NUM_0) {
        rtc_io_dev->sar_i2c_io.scl_sel = 0;
    } else {
        rtc_io_dev->sar_i2c_io.scl_sel = 1;
    }

    return ESP_OK;
}

static esp_err_t i2c_set_timing(const ulp_riscv_i2c_cfg_t *cfg)
{
    /* Convert all timing parameters from micro-seconds to period in RTC_FAST_CLK cycles.
     * RTC_FAST_CLK = 8.5 MHz for esp32s2 and 17.5 MHz for esp32s3.
     * The following calculations approximate the period for each parameter.
     */
    float scl_low_period = MICROSEC_TO_RTC_FAST_CLK(cfg->i2c_timing_cfg.scl_low_period);
    float scl_high_period = MICROSEC_TO_RTC_FAST_CLK(cfg->i2c_timing_cfg.scl_high_period);
    float sda_duty_period = MICROSEC_TO_RTC_FAST_CLK(cfg->i2c_timing_cfg.sda_duty_period);
    float scl_start_period = MICROSEC_TO_RTC_FAST_CLK(cfg->i2c_timing_cfg.scl_start_period);
    float scl_stop_period = MICROSEC_TO_RTC_FAST_CLK(cfg->i2c_timing_cfg.scl_stop_period);
    float i2c_trans_timeout = MICROSEC_TO_RTC_FAST_CLK(cfg->i2c_timing_cfg.i2c_trans_timeout);
    float setup_time_start = (cfg->i2c_timing_cfg.scl_high_period + cfg->i2c_timing_cfg.sda_duty_period);
    float hold_time_start = (cfg->i2c_timing_cfg.scl_start_period - cfg->i2c_timing_cfg.sda_duty_period);
    float setup_time_data = (cfg->i2c_timing_cfg.scl_low_period - cfg->i2c_timing_cfg.sda_duty_period);

    /* Verify timing constraints */
    ESP_RETURN_ON_FALSE(cfg->i2c_timing_cfg.scl_low_period >= 1.3f, ESP_ERR_INVALID_ARG, RTCI2C_TAG, "SCL low period cannot be less than 1.3 micro seconds");
    // TODO: As per specs, SCL high period must be greater than 0.6 micro seconds but after tests it is found that we can have a the period as 0.3 micro seconds to
    // achieve performance close to I2C fast mode. Therefore, this criteria is relaxed.
    ESP_RETURN_ON_FALSE(cfg->i2c_timing_cfg.scl_high_period >= 0.3f, ESP_ERR_INVALID_ARG, RTCI2C_TAG, "SCL high period cannot be less than 0.3 micro seconds");
    ESP_RETURN_ON_FALSE(setup_time_start >= 0.6f, ESP_ERR_INVALID_ARG, RTCI2C_TAG, "Setup time cannot be less than 0.6 micro seconds");
    ESP_RETURN_ON_FALSE(hold_time_start >= 0.6f, ESP_ERR_INVALID_ARG, RTCI2C_TAG, "Data hold time cannot be less than 0.6 micro seconds");
    ESP_RETURN_ON_FALSE(cfg->i2c_timing_cfg.scl_stop_period >= 0.6f, ESP_ERR_INVALID_ARG, RTCI2C_TAG, "Setup time cannot be less than 0.6 micro seconds");
    ESP_RETURN_ON_FALSE(cfg->i2c_timing_cfg.sda_duty_period <= 3.45f, ESP_ERR_INVALID_ARG, RTCI2C_TAG, "Data hold time cannot be greater than 3.45 micro seconds");
    ESP_RETURN_ON_FALSE((setup_time_data * 1000) >= 250, ESP_ERR_INVALID_ARG, RTCI2C_TAG, "Data setup time cannot be less than 250 nano seconds");

    /* Verify filtering constrains
     *
     * I2C may have glitches on the transition edge, so the edge will be filtered in the design,
     * which will also affect the value of the timing parameter register.
     * Therefore, the following filtering constraints must be followed:
     */
    ESP_RETURN_ON_FALSE(scl_stop_period > scl_high_period, ESP_ERR_INVALID_ARG, RTCI2C_TAG, "SCL Stop period cannot be greater than SCL high period");
    ESP_RETURN_ON_FALSE(sda_duty_period < scl_low_period, ESP_ERR_INVALID_ARG, RTCI2C_TAG, "SDA duty period cannot be less than the SCL low period");
    ESP_RETURN_ON_FALSE(scl_start_period > 8, ESP_ERR_INVALID_ARG, RTCI2C_TAG, "SCL start period must be greater than 8 RTC_FAST_CLK cycles");
    ESP_RETURN_ON_FALSE((scl_low_period + scl_high_period - sda_duty_period) > 8, ESP_ERR_INVALID_ARG, RTCI2C_TAG, "SCL low + SCL high - SDA duty must be greater than 8 RTC_FAST_CLK cycles");

    /* Verify SDA duty num constraints */
    ESP_RETURN_ON_FALSE(sda_duty_period > 14, ESP_ERR_INVALID_ARG, RTCI2C_TAG, "SDA duty period must be greater than 14 RTC_FAST_CLK cycles");

    /* Set the RTC I2C timing parameters */
#if CONFIG_IDF_TARGET_ESP32S2
    i2c_dev->scl_low.val = scl_low_period;                  // SCL low period
    i2c_dev->scl_high.val = scl_high_period;                // SCL high period
    i2c_dev->sda_duty.val = sda_duty_period;                // SDA duty cycle
    i2c_dev->scl_start_period.val = scl_start_period;       // Wait time after START condition
    i2c_dev->scl_stop_period.val = scl_stop_period;         // Wait time before END condition
    i2c_dev->timeout.val = i2c_trans_timeout;               // I2C transaction timeout
#elif CONFIG_IDF_TARGET_ESP32S3
    i2c_dev->i2c_scl_low.val = scl_low_period;              // SCL low period
    i2c_dev->i2c_scl_high.val = scl_high_period;            // SCL high period
    i2c_dev->i2c_sda_duty.val = sda_duty_period;            // SDA duty cycle
    i2c_dev->i2c_scl_start_period.val = scl_start_period;   // Wait time after START condition
    i2c_dev->i2c_scl_stop_period.val = scl_stop_period;     // Wait time before END condition
    i2c_dev->i2c_to.val = i2c_trans_timeout;                // I2C transaction timeout
#endif // CONFIG_IDF_TARGET_ESP32S2

    return ESP_OK;
}

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
#if CONFIG_IDF_TARGET_ESP32S2
    /* Reset cmd register */
    i2c_dev->command[cmd_idx].val = 0;

    /* Write new command to cmd register */
    i2c_dev->command[cmd_idx].done = 0;                     // CMD Done
    i2c_dev->command[cmd_idx].op_code = op_code;            // Opcode
    i2c_dev->command[cmd_idx].ack_val = ack_val;            // ACK bit sent by I2C controller during READ.
                                                            // Ignored during RSTART, STOP, END and WRITE cmds.
    i2c_dev->command[cmd_idx].ack_exp = ack_expected;       // ACK bit expected by I2C controller during WRITE.
                                                            // Ignored during RSTART, STOP, END and READ cmds.
    i2c_dev->command[cmd_idx].ack_en = ack_check_en;        // I2C controller verifies that the ACK bit sent by the
                                                            // slave device matches the ACK expected bit during WRITE.
                                                            // Ignored during RSTART, STOP, END and READ cmds.
    HAL_FORCE_MODIFY_U32_REG_FIELD(i2c_dev->command[cmd_idx], byte_num, byte_num);  // Byte Num

#elif CONFIG_IDF_TARGET_ESP32S3
    /* Reset cmd register */
    i2c_dev->i2c_cmd[cmd_idx].val = 0;

    /* Write new command to cmd register */
    i2c_dev->i2c_cmd[cmd_idx].i2c_command_done = 0;         // CMD Done
    i2c_dev->i2c_cmd[cmd_idx].i2c_op_code = op_code;        // Opcode
    i2c_dev->i2c_cmd[cmd_idx].i2c_ack_val = ack_val;        // ACK bit sent by I2C controller during READ.
                                                            // Ignored during RSTART, STOP, END and WRITE cmds.
    i2c_dev->i2c_cmd[cmd_idx].i2c_ack_exp = ack_expected;   // ACK bit expected by I2C controller during WRITE.
                                                            // Ignored during RSTART, STOP, END and READ cmds.
    i2c_dev->i2c_cmd[cmd_idx].i2c_ack_en = ack_check_en;    // I2C controller verifies that the ACK bit sent by the
                                                            // slave device matches the ACK expected bit during WRITE.
                                                            // Ignored during RSTART, STOP, END and READ cmds.
    HAL_FORCE_MODIFY_U32_REG_FIELD(i2c_dev->i2c_cmd[cmd_idx], i2c_byte_num, byte_num);  // Byte Num
#endif // CONFIG_IDF_TARGET_ESP32S2
}

static inline esp_err_t ulp_riscv_i2c_wait_for_interrupt(int32_t ticks_to_wait)
{
    uint32_t status = 0;
    uint32_t to = 0;
    esp_err_t ret = ESP_OK;

    while (1) {
        status = READ_PERI_REG(RTC_I2C_INT_ST_REG);

        /* Return ESP_OK if Tx or Rx data interrupt bits are set. */
        if ((status & RTC_I2C_TX_DATA_INT_ST) ||
            (status & RTC_I2C_RX_DATA_INT_ST)) {
            ret = ESP_OK;
            break;
        /* In case of error status, break and return ESP_FAIL */
#if CONFIG_IDF_TARGET_ESP32S2
        } else if ((status & RTC_I2C_TIMEOUT_INT_ST) ||
#elif CONFIG_IDF_TARGET_ESP32S3
        } else if ((status & RTC_I2C_TIME_OUT_INT_ST) ||
#endif // CONFIG_IDF_TARGET_ESP32S2
                   (status & RTC_I2C_ACK_ERR_INT_ST) ||
                   (status & RTC_I2C_ARBITRATION_LOST_INT_ST)) {
            ret = ESP_FAIL;
            break;
        }

        if (ticks_to_wait > -1) {
            /* If the ticks_to_wait value is not -1, keep track of ticks and
             * break from the loop once the timeout is reached.
             */
            vTaskDelay(1);
            to++;
            if (to >= ticks_to_wait) {
                ret = ESP_ERR_TIMEOUT;
                break;
            }
        }
    }

    return ret;
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
    esp_err_t ret = ESP_OK;

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

    /* Start RTC I2C transmission */
    SET_PERI_REG_MASK(SENS_SAR_I2C_CTRL_REG, SENS_SAR_I2C_START_FORCE);
    SET_PERI_REG_MASK(SENS_SAR_I2C_CTRL_REG, SENS_SAR_I2C_START);

    for (i = 0; i < size; i++) {
        /* Poll for RTC I2C Rx Data interrupt bit to be set */
        ret = ulp_riscv_i2c_wait_for_interrupt(ULP_RISCV_I2C_RW_TIMEOUT);

        if (ret == ESP_OK) {
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
            ESP_LOGE(RTCI2C_TAG, "Read Failed!");
            uint32_t status = READ_PERI_REG(RTC_I2C_INT_RAW_REG);
            ESP_LOGE(RTCI2C_TAG, "RTC I2C Interrupt Raw Reg 0x%"PRIx32"", status);
            ESP_LOGE(RTCI2C_TAG, "RTC I2C Status Reg 0x%"PRIx32"", READ_PERI_REG(RTC_I2C_STATUS_REG));
            SET_PERI_REG_MASK(RTC_I2C_INT_CLR_REG, status);
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
    esp_err_t ret = ESP_OK;

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
        ret = ulp_riscv_i2c_wait_for_interrupt(ULP_RISCV_I2C_RW_TIMEOUT);

        if (ret == ESP_OK) {
            /* Clear the Tx data interrupt bit */
            SET_PERI_REG_MASK(RTC_I2C_INT_CLR_REG, RTC_I2C_TX_DATA_INT_CLR);
        } else {
            ESP_LOGE(RTCI2C_TAG, "Write Failed!");
            uint32_t status = READ_PERI_REG(RTC_I2C_INT_RAW_REG);
            ESP_LOGE(RTCI2C_TAG, "RTC I2C Interrupt Raw Reg 0x%"PRIx32"", status);
            ESP_LOGE(RTCI2C_TAG, "RTC I2C Status Reg 0x%"PRIx32"", READ_PERI_REG(RTC_I2C_STATUS_REG));
            SET_PERI_REG_MASK(RTC_I2C_INT_CLR_REG, status);
            break;
        }
    }

    /* Clear the RTC I2C transmission bits */
    CLEAR_PERI_REG_MASK(SENS_SAR_I2C_CTRL_REG, SENS_SAR_I2C_START_FORCE);
    CLEAR_PERI_REG_MASK(SENS_SAR_I2C_CTRL_REG, SENS_SAR_I2C_START);
}

esp_err_t ulp_riscv_i2c_master_init(const ulp_riscv_i2c_cfg_t *cfg)
{
    /* Clear any stale config registers */
    WRITE_PERI_REG(RTC_I2C_CTRL_REG, 0);
    WRITE_PERI_REG(SENS_SAR_I2C_CTRL_REG, 0);

    /* Reset RTC I2C */
#if CONFIG_IDF_TARGET_ESP32S2
    i2c_dev->ctrl.i2c_reset = 1;
    esp_rom_delay_us(20);
    i2c_dev->ctrl.i2c_reset = 0;
#elif CONFIG_IDF_TARGET_ESP32S3
    SET_PERI_REG_MASK(SENS_SAR_PERI_RESET_CONF_REG, SENS_RTC_I2C_RESET);
    i2c_dev->i2c_ctrl.i2c_i2c_reset = 1;
    esp_rom_delay_us(20);
    i2c_dev->i2c_ctrl.i2c_i2c_reset = 0;
    CLEAR_PERI_REG_MASK(SENS_SAR_PERI_RESET_CONF_REG, SENS_RTC_I2C_RESET);
#endif // CONFIG_IDF_TARGET_ESP32S2

    /* Verify that the input cfg param is valid */
    ESP_RETURN_ON_FALSE(cfg, ESP_ERR_INVALID_ARG, RTCI2C_TAG, "RTC I2C configuration is NULL");

    /* Configure RTC I2C GPIOs */
    ESP_RETURN_ON_ERROR(i2c_set_pin(cfg), RTCI2C_TAG, "Failed to configure RTC I2C GPIOs");

    /* Enable internal open-drain mode for SDA and SCL lines */
#if CONFIG_IDF_TARGET_ESP32S2
    i2c_dev->ctrl.sda_force_out = 0;
    i2c_dev->ctrl.scl_force_out = 0;
#elif CONFIG_IDF_TARGET_ESP32S3
    i2c_dev->i2c_ctrl.i2c_sda_force_out = 0;
    i2c_dev->i2c_ctrl.i2c_scl_force_out = 0;
#endif // CONFIG_IDF_TARGET_ESP32S2

#if CONFIG_IDF_TARGET_ESP32S2
    /* Configure the RTC I2C controller in master mode */
    i2c_dev->ctrl.ms_mode = 1;

    /* Enable RTC I2C Clock gate */
    i2c_dev->ctrl.i2c_ctrl_clk_gate_en = 1;
#elif CONFIG_IDF_TARGET_ESP32S3
    /* For esp32s3, we need to enable the rtc_i2c clock gate before accessing rtc i2c registers */
    SET_PERI_REG_MASK(SENS_SAR_PERI_CLK_GATE_CONF_REG, SENS_RTC_I2C_CLK_EN);

    /* Configure the RTC I2C controller in master mode */
    i2c_dev->i2c_ctrl.i2c_ms_mode = 1;

    /* Enable RTC I2C Clock gate */
    i2c_dev->i2c_ctrl.i2c_i2c_ctrl_clk_gate_en = 1;
#endif // CONFIG_IDF_TARGET_ESP32S2

    /* Configure RTC I2C timing paramters */
    ESP_RETURN_ON_ERROR(i2c_set_timing(cfg), RTCI2C_TAG, "Failed to configure RTC I2C timing");

    /* Enable RTC I2C interrupts */
    SET_PERI_REG_MASK(RTC_I2C_INT_ENA_REG, RTC_I2C_RX_DATA_INT_ENA |
                                           RTC_I2C_TX_DATA_INT_ENA |
                                           RTC_I2C_ARBITRATION_LOST_INT_ENA |
                                           RTC_I2C_ACK_ERR_INT_ENA |
#if CONFIG_IDF_TARGET_ESP32S2
                                           RTC_I2C_TIMEOUT_INT_ENA);
#elif CONFIG_IDF_TARGET_ESP32S3
                                           RTC_I2C_TIME_OUT_INT_ENA);
#endif // CONFIG_IDF_TARGET_ESP32S2

    return ESP_OK;
}
