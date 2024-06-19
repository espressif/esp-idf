/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_check.h"
#include "lp_core_spi.h"
#include "driver/rtc_io.h"
#include "driver/lp_io.h"
#include "hal/rtc_io_types.h"
#include "include/lp_core_spi.h"
#include "soc/lp_spi_struct.h"
#include "soc/lp_gpio_sig_map.h"
#include "soc/lpperi_struct.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/esp_clk_tree_common.h"
#include "hal/spi_ll.h"

static const char *LP_SPI_TAG = "lp_spi";

/* Use the LP SPI register structure to access peripheral registers */
lp_spi_dev_t *lp_spi_dev = &LP_SPI;

static esp_err_t lp_spi_config_io(gpio_num_t pin, rtc_gpio_mode_t direction, uint32_t out_pad_idx, uint32_t in_pad_idx)
{
    esp_err_t ret = ESP_OK;

    /* If pin is -1, then it is not connected to any LP_IO */
    if (pin == -1) {
        return ESP_OK;
    }

    /* Initialize LP_IO */
    ESP_RETURN_ON_ERROR(rtc_gpio_init(pin), LP_SPI_TAG, "LP IO Init failed for GPIO %d", pin);

    /* Set LP_IO direction */
    ESP_RETURN_ON_ERROR(rtc_gpio_set_direction(pin, direction), LP_SPI_TAG, "LP IO Set direction failed for %d", pin);

    /* Connect the LP SPI signals to the LP_IO Matrix */
    ESP_RETURN_ON_ERROR(lp_gpio_connect_out_signal(pin, out_pad_idx, 0, 0), LP_SPI_TAG, "LP IO Matrix connect out signal failed for %d", pin);
    ESP_RETURN_ON_ERROR(lp_gpio_connect_in_signal(pin, in_pad_idx, 0), LP_SPI_TAG, "LP IO Matrix connect in signal failed for %d", pin);

    return ret;
}

static esp_err_t lp_spi_bus_init_io(const lp_spi_bus_config_t *bus_config)
{
    esp_err_t ret = ESP_OK;

    /* Argument sanity check */
#if SOC_LP_GPIO_MATRIX_SUPPORTED
    /* LP SPI signals can be routed to any LP_IO */
    ESP_RETURN_ON_FALSE((rtc_gpio_is_valid_gpio(bus_config->mosi_io_num)), ESP_FAIL, LP_SPI_TAG, "mosi_io_num error");
    ESP_RETURN_ON_FALSE((bus_config->miso_io_num == -1) || (rtc_gpio_is_valid_gpio(bus_config->miso_io_num)), ESP_FAIL, LP_SPI_TAG, "miso_io_num error");
    ESP_RETURN_ON_FALSE((rtc_gpio_is_valid_gpio(bus_config->sclk_io_num)), ESP_FAIL, LP_SPI_TAG, "sclk_io_num error");

    /* Configure miso pin*/
    ret = lp_spi_config_io(bus_config->miso_io_num, RTC_GPIO_MODE_INPUT_OUTPUT, LP_SPI_Q_PAD_OUT_IDX, LP_SPI_Q_PAD_IN_IDX);
    /* Configure mosi pin */
    ret = lp_spi_config_io(bus_config->mosi_io_num, RTC_GPIO_MODE_INPUT_OUTPUT, LP_SPI_D_PAD_OUT_IDX, LP_SPI_D_PAD_IN_IDX);
    /* Configure sclk pin */
    ret = lp_spi_config_io(bus_config->sclk_io_num, RTC_GPIO_MODE_INPUT_OUTPUT, LP_SPI_CK_PAD_OUT_IDX, LP_SPI_CK_PAD_IN_IDX);
#else
#error "LP SPI bus initialization is not supported without LP GPIO Matrix."
#endif  /* SOC_LP_GPIO_MATRIX_SUPPORTED */

    return ret;
}

static esp_err_t lp_spi_cs_pin_init(int cs_io_num)
{
    esp_err_t ret = ESP_OK;

#if SOC_LP_GPIO_MATRIX_SUPPORTED
    /* CS signal can be routed to any LP_IO */
    ESP_RETURN_ON_FALSE((rtc_gpio_is_valid_gpio(cs_io_num)), ESP_FAIL, LP_SPI_TAG, "cs_io_num error");

    /* Configure CS pin */
    ret = lp_spi_config_io(cs_io_num, RTC_GPIO_MODE_INPUT_OUTPUT, LP_SPI_CS_PAD_OUT_IDX, LP_SPI_CS_PAD_IN_IDX);
#else
#error "LP SPI device Chip Select (CS) initialization is not supported without LP GPIO Matrix."
#endif  /* SOC_LP_GPIO_MATRIX_SUPPORTED */

    return ret;
}

static void lp_spi_enable_clock_gate(void)
{
    lpperi_dev_t *lp_peri_dev = &LPPERI;
    PERIPH_RCC_ATOMIC() {
        (void)__DECLARE_RCC_ATOMIC_ENV; // Avoid warnings for unused variable __DECLARE_RCC_ATOMIC_ENV
        lp_peri_dev->clk_en.ck_en_lp_spi = 1;
    }
}

static esp_err_t lp_spi_clock_init(const lp_spi_device_config_t *dev_config)
{
    esp_err_t ret = ESP_OK;

    /* Max requested clock frequency cannot be more than the LP_FAST_CLK frequency */
    uint32_t max_clock_source_hz = esp_clk_tree_lp_fast_get_freq_hz(ESP_CLK_TREE_SRC_FREQ_PRECISION_APPROX);
    ESP_RETURN_ON_FALSE(dev_config->clock_speed_hz <= max_clock_source_hz, ESP_ERR_INVALID_ARG, LP_SPI_TAG, "Invalid clock speed for SPI device. Max allowed = %ld Hz", max_clock_source_hz);

    /* Set the duty cycle. If not specified, use 50% */
    int duty_cycle = dev_config->duty_cycle ? dev_config->duty_cycle : 128;

    /* Calculate the clock pre-div values. We use the HP SPI LL function here for the calculation. */
    spi_ll_clock_val_t spi_clock;
    spi_ll_master_cal_clock(max_clock_source_hz, dev_config->clock_speed_hz, duty_cycle, &spi_clock);
    lp_spi_dev->spi_clock.val = spi_clock;

    return ret;
}

static void lp_spi_master_init(void)
{
    /* Initialize the LP SPI in master mode.
     * (We do not have a HAL/LL layer for LP SPI, yet, so let's use the LP SPI registers directly).
     */

    /* Clear Slave mode to enable Master mode */
    lp_spi_dev->spi_slave.reg_slave_mode = 0;
    lp_spi_dev->spi_slave.reg_clk_mode = 0;

    /* Reset CS timing */
    lp_spi_dev->spi_user1.reg_cs_setup_time = 0;
    lp_spi_dev->spi_user1.reg_cs_hold_time = 0;

    /* Use all 64 bytes of the Tx/Rx buffers in CPU controlled transfer */
    lp_spi_dev->spi_user.reg_usr_mosi_highpart = 0;
    lp_spi_dev->spi_user.reg_usr_miso_highpart = 0;
}

static void lp_spi_slave_init(void)
{
    /* Set Slave mode */
    lp_spi_dev->spi_slave.reg_slave_mode = 1;

    /* Reset the SPI peripheral */
    lp_spi_dev->spi_slave.reg_soft_reset = 1;
    lp_spi_dev->spi_slave.reg_soft_reset = 0;

    /* Configure slave */
    lp_spi_dev->spi_clock.val = 0;
    lp_spi_dev->spi_user.val = 0;
    lp_spi_dev->spi_ctrl.val = 0;
    lp_spi_dev->spi_user.reg_doutdin = 1; //we only support full duplex
    lp_spi_dev->spi_user.reg_sio = 0;

    /* Use all 64 bytes of the Tx/Rx buffers in CPU controlled transfer */
    lp_spi_dev->spi_user.reg_usr_miso_highpart = 0;
    lp_spi_dev->spi_user.reg_usr_mosi_highpart = 0;
}

static void lp_spi_master_setup_device(const lp_spi_device_config_t *dev_config)
{
    /* Configure transmission bit order */
    lp_spi_dev->spi_ctrl.reg_rd_bit_order = dev_config->flags & LP_SPI_DEVICE_RXBIT_LSBFIRST ? 1 : 0;
    lp_spi_dev->spi_ctrl.reg_wr_bit_order = dev_config->flags & LP_SPI_DEVICE_TXBIT_LSBFIRST ? 1 : 0;

    /* Configure SPI mode in master mode */
    if (dev_config->spi_mode == 0) {
        lp_spi_dev->spi_misc.reg_ck_idle_edge = 0;
        lp_spi_dev->spi_user.reg_ck_out_edge = 0;
    } else if (dev_config->spi_mode == 1) {
        lp_spi_dev->spi_misc.reg_ck_idle_edge = 0;
        lp_spi_dev->spi_user.reg_ck_out_edge = 1;
    } else if (dev_config->spi_mode == 2) {
        lp_spi_dev->spi_misc.reg_ck_idle_edge = 1;
        lp_spi_dev->spi_user.reg_ck_out_edge = 1;
    } else if (dev_config->spi_mode == 3) {
        lp_spi_dev->spi_misc.reg_ck_idle_edge = 1;
        lp_spi_dev->spi_user.reg_ck_out_edge = 0;
    }

    /* Configure the polarity of the CS line */
    lp_spi_dev->spi_misc.reg_master_cs_pol = dev_config->flags & LP_SPI_DEVICE_CS_ACTIVE_HIGH ? 1 : 0;

    /* Configure half-duplex (0) or full-duplex (1) mode for LP SPI master */
    lp_spi_dev->spi_user.reg_doutdin = dev_config->flags & LP_SPI_DEVICE_HALF_DUPLEX ? 0 : 1;

    /* Configure 3-Wire half-duplex mode */
    lp_spi_dev->spi_user.reg_sio = dev_config->flags & LP_SPI_DEVICE_3WIRE ? 1 : 0;

    /* Configure CS setup and hold times */
    lp_spi_dev->spi_user1.reg_cs_setup_time = dev_config->cs_ena_pretrans == 0 ? 0 : dev_config->cs_ena_pretrans - 1;
    lp_spi_dev->spi_user.reg_cs_setup = dev_config->cs_ena_pretrans ? 1 : 0;
    lp_spi_dev->spi_user1.reg_cs_hold_time = dev_config->cs_ena_posttrans;
    lp_spi_dev->spi_user.reg_cs_hold = dev_config->cs_ena_posttrans ? 1 : 0;

    /* Select the CS pin */
    lp_spi_dev->spi_misc.reg_cs0_dis = 0;
}

static void lp_spi_slave_setup_device(const lp_spi_slave_config_t *slave_config)
{
    /* Configure transmission bit order */
    lp_spi_dev->spi_ctrl.reg_rd_bit_order = slave_config->flags & LP_SPI_DEVICE_RXBIT_LSBFIRST ? 1 : 0;
    lp_spi_dev->spi_ctrl.reg_wr_bit_order = slave_config->flags & LP_SPI_DEVICE_TXBIT_LSBFIRST ? 1 : 0;

    /* Configure SPI mode in slave mode */
    if (slave_config->spi_mode == 0) {
        lp_spi_dev->spi_misc.reg_ck_idle_edge = 0;
        lp_spi_dev->spi_user.reg_rsck_i_edge = 0;
        lp_spi_dev->spi_user.reg_tsck_i_edge = 0;
        lp_spi_dev->spi_slave.reg_clk_mode_13 = 0;
    } else if (slave_config->spi_mode == 1) {
        lp_spi_dev->spi_misc.reg_ck_idle_edge = 0;
        lp_spi_dev->spi_user.reg_rsck_i_edge = 1;
        lp_spi_dev->spi_user.reg_tsck_i_edge = 1;
        lp_spi_dev->spi_slave.reg_clk_mode_13 = 1;
    } else if (slave_config->spi_mode == 2) {
        lp_spi_dev->spi_misc.reg_ck_idle_edge = 1;
        lp_spi_dev->spi_user.reg_rsck_i_edge = 1;
        lp_spi_dev->spi_user.reg_tsck_i_edge = 1;
        lp_spi_dev->spi_slave.reg_clk_mode_13 = 0;
    } else if (slave_config->spi_mode == 3) {
        lp_spi_dev->spi_misc.reg_ck_idle_edge = 1;
        lp_spi_dev->spi_user.reg_rsck_i_edge = 0;
        lp_spi_dev->spi_user.reg_tsck_i_edge = 0;
        lp_spi_dev->spi_slave.reg_clk_mode_13 = 1;
    }

    if (slave_config->flags & LP_SPI_DEVICE_CS_ACTIVE_HIGH) {
        ESP_LOGW(LP_SPI_TAG, "Active high CS line is not supported in slave mode. Using active low CS line.");
    }
    lp_spi_dev->spi_misc.reg_slave_cs_pol = 0;

    if (slave_config->flags & LP_SPI_DEVICE_HALF_DUPLEX) {
        ESP_LOGW(LP_SPI_TAG, "Half-duplex mode is not supported in slave mode. Using full-duplex mode.");
    }
    lp_spi_dev->spi_user.reg_doutdin = 1;

    /* Configure 3-Wire half-duplex mode */
    lp_spi_dev->spi_user.reg_sio = slave_config->flags & LP_SPI_DEVICE_3WIRE ? 1 : 0;

    /* Select the CS pin */
    lp_spi_dev->spi_misc.reg_cs0_dis = 0;
}

//////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Public APIs ///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

esp_err_t lp_core_lp_spi_bus_initialize(lp_spi_host_t host_id, const lp_spi_bus_config_t *bus_config)
{
    (void)host_id;

    /* Sanity check arguments */
    if (bus_config == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Connect the LP SPI peripheral to a "bus", i.e. a set of
     * GPIO pins defined in the bus_config structure.
     */
    esp_err_t ret = lp_spi_bus_init_io(bus_config);

    return ret;
}

esp_err_t lp_core_lp_spi_bus_add_device(lp_spi_host_t host_id, const lp_spi_device_config_t *dev_config)
{
    (void)host_id;

    esp_err_t ret = ESP_OK;

    /* Configure the CS pin */
    ESP_RETURN_ON_ERROR(lp_spi_cs_pin_init(dev_config->cs_io_num), LP_SPI_TAG, "CS pin initialization failed");

    /* Enable the LP SPI clock gate */
    lp_spi_enable_clock_gate();

    /* Lazy initialize the LP SPI in master mode */
    lp_spi_master_init();

    /* Configure clock */
    ESP_RETURN_ON_ERROR(lp_spi_clock_init(dev_config), LP_SPI_TAG, "Clock initialization failed");

    /* Setup the SPI device */
    lp_spi_master_setup_device(dev_config);

    return ret;
}

esp_err_t lp_core_lp_spi_slave_initialize(lp_spi_host_t host_id, const lp_spi_slave_config_t *slave_config)
{
    (void)host_id;

    esp_err_t ret = ESP_OK;

    /* Configure the CS pin */
    ESP_RETURN_ON_ERROR(lp_spi_cs_pin_init(slave_config->cs_io_num), LP_SPI_TAG, "CS pin initialization failed");

    /* Enable the LP SPI clock gate */
    lp_spi_enable_clock_gate();

    /* Initialize the LP SPI in slave mode */
    lp_spi_slave_init();

    /* Setup the SPI device */
    lp_spi_slave_setup_device(slave_config);

    return ret;
}
