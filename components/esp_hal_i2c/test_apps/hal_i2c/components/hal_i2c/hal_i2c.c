/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include "esp_log.h"
#include "soc/io_mux_reg.h"
#include "soc/periph_defs.h"
#include "soc/gpio_sig_map.h"
#include "hal/gpio_ll.h"
#include "hal/gpio_hal.h"
#include "hal/i2c_ll.h"
#include "esp_rom_gpio.h"
#include "esp_cpu.h"
#include "esp_check.h"
#include "esp_private/periph_ctrl.h"
#include "hal/clk_tree_ll.h"
#include "hal/clk_tree_hal.h"
#include "hal_i2c.h"

// Get current CPU tick count
#define RECORD_TIME_PREPARE()   uint32_t __t1, __t2
#define RECORD_TIME_START()     do {__t1 = esp_cpu_get_cycle_count();} while(0)
#define RECORD_TIME_ELAPSED(p_time) do{__t2 = esp_cpu_get_cycle_count(); p_time = (__t2 - __t1);} while(0)

static inline uint32_t time_get_us_by_ccount(uint32_t counter)
{
    return counter / CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ;
}

#define ACK_VALUE            (0)
#define NACK_VALUE           (1)
#define NOT_CHECK_ACK_VALUE  (0)
#define CHECK_ACK_VALUE      (1)

ESP_LOG_ATTR_TAG(TAG, "hal-i2c");

// If Reset and Clock Control is independent, we need this macro to avoid concurrency issue
#if !SOC_RCC_IS_INDEPENDENT
#define I2C_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define I2C_RCC_ATOMIC()
#endif

#if SOC_PERIPH_CLK_CTRL_SHARED
#define I2C_CLOCK_SRC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define I2C_CLOCK_SRC_ATOMIC()
#endif

typedef struct {
    i2c_dev_t *i2c_dev;
    bool initialized;
} hal_i2c_context;

hal_i2c_context i2c_context[I2C_NUM_MAX] = {
    {
        .i2c_dev = I2C_LL_GET_HW(0),
        .initialized = false,
    },
#if SOC_I2C_NUM > 1
    {
        .i2c_dev = I2C_LL_GET_HW(1),
        .initialized = false,
    },
#endif
};

esp_err_t hal_i2c_init(hal_i2c_config *cfg)
{
    ESP_RETURN_ON_FALSE(cfg->i2c_port < I2C_NUM_MAX, ESP_ERR_INVALID_ARG, TAG, "No such I2C port on this chip");
    int sda_io = cfg->sda_pin;
    int scl_io = cfg->scl_pin;
    uint32_t freq = cfg->freq;
    i2c_dev_t *dev = i2c_context[cfg->i2c_port].i2c_dev;

    I2C_RCC_ATOMIC() {
        i2c_ll_enable_bus_clock(0, true);
        i2c_ll_reset_register(0);
    }

    i2c_ll_enable_controller_clock(dev, true);

    // Note: Both SCL and SDA pins must be in open-drain configuration and the lines must be pulled-up.
    // The internal pull-ups are very weak, so we strongly recommend you use an external pull-up.

    // In io pin configurations, we:
    // 1. Set SCL and SDA to high level, because SCL/SDA valid when logic voltage change from 1 to 0.
    // 2. Set both SCL and SDA open-drain
    // 3. Set both SCL and SDA pullup enable and pulldown disable. (If you use external pullup, this can be ignored)
    // 4. io mux function select
    // 5. We connect out/in signal. As I2C master, out/in signal is necessary for both SCL and SDA according to esp hardware.

    // SDA pin configurations
    if (sda_io != -1) {
        assert(sda_io < SOC_GPIO_PIN_COUNT);
        gpio_ll_set_level(&GPIO, sda_io, 1);
        gpio_ll_od_enable(&GPIO, sda_io);
        gpio_ll_pullup_en(&GPIO, sda_io);
        gpio_ll_pulldown_dis(&GPIO, sda_io);
        gpio_ll_func_sel(&GPIO, sda_io, PIN_FUNC_GPIO);
        esp_rom_gpio_connect_out_signal(sda_io, i2c_periph_signal[cfg->i2c_port].sda_out_sig, 0, 0);
        esp_rom_gpio_connect_in_signal(sda_io, i2c_periph_signal[cfg->i2c_port].sda_in_sig, 0);
    }
    // SCL pin configurations
    if (scl_io != -1) {
        assert(scl_io < SOC_GPIO_PIN_COUNT);
        gpio_ll_set_level(&GPIO, scl_io, 1);
        gpio_ll_od_enable(&GPIO, scl_io);
        gpio_ll_pullup_en(&GPIO, scl_io);
        gpio_ll_pulldown_dis(&GPIO, scl_io);
        gpio_ll_func_sel(&GPIO, scl_io, PIN_FUNC_GPIO);
        esp_rom_gpio_connect_out_signal(scl_io, i2c_periph_signal[cfg->i2c_port].scl_out_sig, 0, 0);
        esp_rom_gpio_connect_in_signal(scl_io, i2c_periph_signal[cfg->i2c_port].scl_out_sig, 0);
    }
    // Initialize I2C master bus. Including enable its clock, choose its mode, etc.
    i2c_ll_set_mode(dev, I2C_BUS_MODE_MASTER);
    i2c_ll_enable_pins_open_drain(dev, true);
    i2c_ll_enable_arbitration(dev, false);
    //MSB (I2C standard require the data to be sent with most MSB)
    i2c_ll_set_data_mode(dev, I2C_DATA_MODE_MSB_FIRST, I2C_DATA_MODE_MSB_FIRST);
    //Reset fifo
    i2c_ll_txfifo_rst(dev);
    i2c_ll_rxfifo_rst(dev);

    //disable intr
    i2c_ll_disable_intr_mask(dev, I2C_LL_INTR_MASK);

    // init clock, always use xtal in hal driver.
    i2c_hal_clk_config_t clk_cal = {0};
    I2C_CLOCK_SRC_ATOMIC() {
        i2c_ll_set_source_clk(dev, SOC_MOD_CLK_XTAL);
    }
    uint32_t xtal_freq = 0;
#if SOC_CLK_TREE_SUPPORTED
    xtal_freq = clk_hal_xtal_get_freq_mhz();
#else
    xtal_freq = clk_ll_xtal_get_freq_mhz();
#endif
    i2c_ll_master_cal_bus_clk(xtal_freq * MHZ, freq, &clk_cal);
    i2c_ll_master_set_bus_timing(dev, &clk_cal);

    i2c_ll_update(dev);
    i2c_context[cfg->i2c_port].initialized = true;
    return ESP_OK;
}

static esp_err_t i2c_wait_done(i2c_port_t port_num, int cmd_idx, uint32_t timeout_ms)
{
    uint32_t timeout_us = timeout_ms * 1000;
    uint32_t wait_time = 0;
    RECORD_TIME_PREPARE();
    RECORD_TIME_START();
    i2c_dev_t *dev = i2c_context[port_num].i2c_dev;
    while (i2c_ll_master_is_cmd_done(dev, cmd_idx) == 0) {
        RECORD_TIME_ELAPSED(wait_time);
        if (time_get_us_by_ccount(wait_time) > timeout_us) {
            return ESP_ERR_TIMEOUT;
        }
    }
    return ESP_OK;
}

static void i2c_format_cmd(i2c_port_t port_num, uint32_t cmd_idx, uint8_t op_code, uint8_t ack_val, uint8_t ack_expected, uint8_t ack_check_en, uint8_t byte_num)
{
    i2c_dev_t *dev = i2c_context[port_num].i2c_dev;
    /* Form new command */
    i2c_ll_hw_cmd_t hw_cmd = {
        .done = 0,                // CMD Done
        // Different ESP target may have different op code, please
        // refer to '{target}/hal/i2c_ll.h', they are
        // #define I2C_LL_CMD_RESTART
        // #define I2C_LL_CMD_WRITE
        // #define I2C_LL_CMD_READ
        // #define I2C_LL_CMD_STOP
        // #define I2C_LL_CMD_END
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

esp_err_t hal_i2c_write(i2c_port_t port_num, uint16_t addr, const uint8_t *txdata, uint32_t txlength, uint32_t timeout_ms)
{
    ESP_RETURN_ON_FALSE(i2c_context[port_num].initialized == true, ESP_ERR_INVALID_STATE, TAG, "This I2C port has not been initialized");
    ESP_RETURN_ON_FALSE(port_num < I2C_NUM_MAX, ESP_ERR_INVALID_STATE, TAG, "This ESP target does not have this port");
    ESP_RETURN_ON_FALSE(txdata, ESP_ERR_INVALID_STATE, TAG, "txdata pointer is null");
    i2c_dev_t *dev = i2c_context[port_num].i2c_dev;
    uint32_t cmd_idx = 0;
    if (i2c_ll_is_bus_busy(dev)) {
        i2c_ll_master_fsm_rst(dev);
    }

    /* Reset the Tx and Rx FIFOs */
    i2c_ll_txfifo_rst(dev);
    i2c_ll_rxfifo_rst(dev);

    /* Execute RSTART command to send the START bit he I2C controller is based on a command-list where each cell describe the
    next action to perform. The first action we want the controller to perform is a start, so create a START command and enqueue
    it in the hardware commands list*/
    i2c_format_cmd(port_num, cmd_idx++, I2C_LL_CMD_RESTART, ACK_VALUE, 0, NOT_CHECK_ACK_VALUE, 0);

    /* Write device addr and update the HW command register. The 7-bit device address must be shift left once and the WRITE bit (0) must be appended */
    uint8_t addr_byte = (uint8_t)(((addr & 0xFF) << 1) | (0 << 0));
    /* The controller also has a RAM/FIFO that contains the data to send or receive whenever a READ or WRITE command is issued through list previously described */
    i2c_ll_write_txfifo(dev, &addr_byte, 1);
    /* Update the HW command register. Expect an ACK from the device */
    i2c_format_cmd(port_num, cmd_idx++, I2C_LL_CMD_WRITE, ACK_VALUE, I2C_MASTER_ACK, NOT_CHECK_ACK_VALUE, 1);

    uint32_t remaining_byte = txlength;
    while (remaining_byte) {
        uint32_t tx_len_tmp = remaining_byte > I2C_LL_GET(FIFO_LEN) - 1 ? I2C_LL_GET(FIFO_LEN) - 1 : remaining_byte;
        i2c_ll_write_txfifo(dev, txdata, tx_len_tmp);
        /*The I2C controller support up to  I2C commands, as such, if we need to enqueue more commands than that,
        we can tell the hardware to start with the enqueued commands first and wait for the next commands to send afterwards.
        To specify this behavior, we must use the END command. */
        i2c_format_cmd(port_num, cmd_idx++, I2C_LL_CMD_WRITE, ACK_VALUE, 0, NOT_CHECK_ACK_VALUE, tx_len_tmp);
        i2c_format_cmd(port_num, cmd_idx++, I2C_LL_CMD_END, ACK_VALUE, 0, NOT_CHECK_ACK_VALUE, 0);
        i2c_ll_update(dev);
        i2c_ll_start_trans(dev);
        ESP_RETURN_ON_ERROR(i2c_wait_done(port_num, cmd_idx - 1, timeout_ms), TAG, "wait done failed");
        cmd_idx = 0;
        txdata += tx_len_tmp;
        remaining_byte -= tx_len_tmp;
    }

    i2c_format_cmd(port_num, cmd_idx++, I2C_LL_CMD_STOP, ACK_VALUE, 0, NOT_CHECK_ACK_VALUE, 0);
    i2c_ll_update(dev);
    i2c_ll_start_trans(dev);
    ESP_RETURN_ON_ERROR(i2c_wait_done(port_num, cmd_idx - 1, timeout_ms), TAG, "wait done failed");
    return ESP_OK;
}

esp_err_t hal_i2c_read(i2c_port_t port_num, uint16_t addr, uint8_t *rxdata, uint32_t rxlength, uint32_t timeout_ms)
{
    ESP_RETURN_ON_FALSE(i2c_context[port_num].initialized == true, ESP_ERR_INVALID_STATE, TAG, "This I2C port has not been initialized");
    i2c_dev_t *dev = i2c_context[port_num].i2c_dev;
    uint32_t cmd_idx = 0;
    uint32_t data_idx = 0;
    /* Reset the Tx and Rx FIFOs */
    i2c_ll_txfifo_rst(dev);
    i2c_ll_rxfifo_rst(dev);

    /* Execute RSTART command to send the START bit */
    i2c_format_cmd(port_num, cmd_idx++, I2C_LL_CMD_RESTART, ACK_VALUE, 0, NOT_CHECK_ACK_VALUE, 0);
    uint8_t addr_byte = (uint8_t)(((addr & 0xFF) << 1) | 1);
    i2c_ll_write_txfifo(dev, &addr_byte, 1);
    /* Update the HW command register. Expect an ACK from the device */
    i2c_format_cmd(port_num, cmd_idx++, I2C_LL_CMD_WRITE, ACK_VALUE, 0, NOT_CHECK_ACK_VALUE, 1);

    uint32_t remaining_byte = rxlength;

    while (remaining_byte) {
        uint32_t tmp_rx_length = (remaining_byte > I2C_LL_GET(FIFO_LEN)) ? I2C_LL_GET(FIFO_LEN) : remaining_byte;
        remaining_byte -= tmp_rx_length;
        if (tmp_rx_length == 1) {
            i2c_format_cmd(port_num, cmd_idx++, I2C_LL_CMD_READ, NACK_VALUE, 0, NOT_CHECK_ACK_VALUE, 1);
            i2c_format_cmd(port_num, cmd_idx++, I2C_LL_CMD_STOP, ACK_VALUE, 0, NOT_CHECK_ACK_VALUE, 0);
        } else if ((tmp_rx_length > 1) && (remaining_byte == 0)) {
            i2c_format_cmd(port_num, cmd_idx++, I2C_LL_CMD_READ, ACK_VALUE, 0, NOT_CHECK_ACK_VALUE, tmp_rx_length - 1);
            i2c_format_cmd(port_num, cmd_idx++, I2C_LL_CMD_READ, NACK_VALUE, 0, NOT_CHECK_ACK_VALUE, 1);
            i2c_format_cmd(port_num, cmd_idx++, I2C_LL_CMD_STOP, ACK_VALUE, 0, NOT_CHECK_ACK_VALUE, 0);
        } else {
            i2c_format_cmd(port_num, cmd_idx++, I2C_LL_CMD_READ, ACK_VALUE, 0, NOT_CHECK_ACK_VALUE, tmp_rx_length);
            i2c_format_cmd(port_num, cmd_idx++, I2C_LL_CMD_END, ACK_VALUE, 0, NOT_CHECK_ACK_VALUE, 0);
        }

        i2c_ll_update(dev);
        i2c_ll_start_trans(dev);
        ESP_RETURN_ON_ERROR(i2c_wait_done(port_num, cmd_idx - 1, timeout_ms), TAG, "wait done failed");
        cmd_idx = 0;
        i2c_ll_read_rxfifo(dev, &rxdata[data_idx], 1);
        data_idx += tmp_rx_length;
    }

    return ESP_OK;
}

esp_err_t hal_i2c_write_read(i2c_port_t port_num, uint16_t addr, const uint8_t *txdata, uint32_t txlength, uint8_t *rxdata, uint32_t rxlength, uint32_t timeout_ms)
{
    ESP_RETURN_ON_FALSE(i2c_context[port_num].initialized == true, ESP_ERR_INVALID_STATE, TAG, "This I2C port has not been initialized");
    i2c_dev_t *dev = i2c_context[port_num].i2c_dev;
    uint32_t cmd_idx = 0;
    uint32_t data_idx = 0;
    /* Reset the Tx and Rx FIFOs */
    i2c_ll_txfifo_rst(dev);
    i2c_ll_rxfifo_rst(dev);

    /* Execute RSTART command to send the START bit */
    i2c_format_cmd(port_num, cmd_idx++, I2C_LL_CMD_RESTART, 0, 0, NOT_CHECK_ACK_VALUE, 0);
    uint8_t addr_byte = (uint8_t)(((addr & 0xFF) << 1) | 0);
    i2c_ll_write_txfifo(dev, &addr_byte, 1);
    /* Update the HW command register. Expect an ACK from the device */
    i2c_format_cmd(port_num, cmd_idx++, I2C_LL_CMD_WRITE, 0, 0, NOT_CHECK_ACK_VALUE, 1);

    // Write first
    uint32_t remaining_byte = txlength;
    while (remaining_byte) {
        uint32_t tx_len_tmp = remaining_byte > I2C_LL_GET(FIFO_LEN) - 1 ? I2C_LL_GET(FIFO_LEN) - 1 : remaining_byte;
        i2c_ll_write_txfifo(dev, txdata, tx_len_tmp);
        /*The I2C controller support up to  I2C commands, as such, if we need to enqueue more commands than that,
        we can tell the hardware to start with the enqueued commands first and wait for the next commands to send afterwards.
        To specify this behavior, we must use the END command. */
        i2c_format_cmd(port_num, cmd_idx++, I2C_LL_CMD_WRITE, ACK_VALUE, 0, NOT_CHECK_ACK_VALUE, tx_len_tmp);
        i2c_format_cmd(port_num, cmd_idx++, I2C_LL_CMD_END, ACK_VALUE, 0, NOT_CHECK_ACK_VALUE, 0);
        /* Initiate I2C transfer */
        i2c_ll_update(dev);
        i2c_ll_start_trans(dev);
        ESP_RETURN_ON_ERROR(i2c_wait_done(port_num, cmd_idx - 1, timeout_ms), TAG, "wait done failed");
        cmd_idx = 0;
        txdata += tx_len_tmp;
        remaining_byte -= tx_len_tmp;
    }

    // Then, read
    i2c_format_cmd(port_num, cmd_idx++, I2C_LL_CMD_RESTART, ACK_VALUE, 0, NOT_CHECK_ACK_VALUE, 0);
    addr_byte = (uint8_t)(((addr & 0xFF) << 1) | 1);
    i2c_ll_write_txfifo(dev, &addr_byte, 1);
    /* Update the HW command register. Expect an ACK from the device */
    i2c_format_cmd(port_num, cmd_idx++, I2C_LL_CMD_WRITE, ACK_VALUE, 0, NOT_CHECK_ACK_VALUE, 1);

    remaining_byte = rxlength;

    while (remaining_byte) {
        uint32_t tmp_rx_length = (remaining_byte > I2C_LL_GET(FIFO_LEN)) ? I2C_LL_GET(FIFO_LEN) : remaining_byte;
        remaining_byte -= tmp_rx_length;
        if (tmp_rx_length == 1) {
            i2c_format_cmd(port_num, cmd_idx++, I2C_LL_CMD_READ, NACK_VALUE, 0, NOT_CHECK_ACK_VALUE, 1);
            i2c_format_cmd(port_num, cmd_idx++, I2C_LL_CMD_STOP, ACK_VALUE, 0, NOT_CHECK_ACK_VALUE, 0);
        } else if ((tmp_rx_length > 1) && (remaining_byte == 0)) {
            i2c_format_cmd(port_num, cmd_idx++, I2C_LL_CMD_READ, ACK_VALUE, 0, NOT_CHECK_ACK_VALUE, tmp_rx_length - 1);
            i2c_format_cmd(port_num, cmd_idx++, I2C_LL_CMD_READ, NACK_VALUE, 0, NOT_CHECK_ACK_VALUE, 1);
            i2c_format_cmd(port_num, cmd_idx++, I2C_LL_CMD_STOP, ACK_VALUE, 0, NOT_CHECK_ACK_VALUE, 0);
        } else {
            i2c_format_cmd(port_num, cmd_idx++, I2C_LL_CMD_READ, ACK_VALUE, 0, NOT_CHECK_ACK_VALUE, tmp_rx_length);
            i2c_format_cmd(port_num, cmd_idx++, I2C_LL_CMD_END, ACK_VALUE, 0, NOT_CHECK_ACK_VALUE, 0);
        }

        i2c_ll_update(dev);
        i2c_ll_start_trans(dev);
        ESP_RETURN_ON_ERROR(i2c_wait_done(port_num, cmd_idx - 1, timeout_ms), TAG, "wait done failed");
        i2c_ll_read_rxfifo(dev, &rxdata[data_idx], tmp_rx_length);
        cmd_idx = 0;
        data_idx += tmp_rx_length;

    }

    return ESP_OK;

}

// I2C pure hal example cannot work with esp_driver_i2c.
__attribute__((constructor))
static void check_hal_i2c_driver_conflict(void)
{
    // This function was declared as weak here. The new I2C driver has the implementation.
    // So if the new I2C driver is not linked in, then `i2c_acquire_bus_handle()` should be NULL at runtime.
    extern __attribute__((weak)) esp_err_t i2c_acquire_bus_handle(int port_num, void *i2c_new_bus, int mode);
    if ((void *)i2c_acquire_bus_handle != NULL) {
        // printf("CONFLICT! esp_driver cannot work with this pure hal driver\n");
        abort();
    }
}
