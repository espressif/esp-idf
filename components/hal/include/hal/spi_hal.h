// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The HAL layer for SPI master (common part)

// SPI HAL usages:
// 1. initialize the bus
// 2. initialize the DMA descriptors if DMA used
// 3. setup the clock speed (since this takes long time)
// 4. call setup_device to update parameters for the specific device
// 5. call setup_trans to update parameters for the specific transaction
// 6. prepare data to send, and prepare the receiving buffer
// 7. trigger user defined SPI transaction to start
// 8. wait until the user transaction is done
// 9. fetch the received data
// Parameter to be updated only during ``setup_device`` will be highlighted in the
// field comments.

#pragma once
#include "hal/spi_ll.h"
#include <esp_err.h>
#include "soc/lldesc.h"
#include "soc/soc_caps.h"
#include "hal/spi_types.h"

/**
 * Input parameters to the ``spi_hal_cal_clock_conf`` to calculate the timing configuration
 */
typedef struct {
    uint32_t half_duplex;               ///< Whether half duplex mode is used, device specific
    uint32_t no_compensate;             ///< No need to add dummy to compensate the timing, device specific
    uint32_t clock_speed_hz;            ///< Desired frequency.
    uint32_t duty_cycle;                ///< Desired duty cycle of SPI clock
    uint32_t input_delay_ns;            /**< Maximum delay between SPI launch clock and the data to be valid.
                                         *   This is used to compensate/calculate the maximum frequency allowed.
                                         *   Left 0 if not known.
                                         */
    bool use_gpio;                      ///< True if the GPIO matrix is used, otherwise false
} spi_hal_timing_param_t;

/**
 * Timing configuration structure that should be calculated by
 * ``spi_hal_cal_clock_conf`` at initialization and hold. Filled into the
 * ``timing_conf`` member of the context of HAL before setup a device.
 */
typedef struct {
    spi_ll_clock_val_t clock_reg;       ///< Register value used by the LL layer
    int timing_dummy;                   ///< Extra dummy needed to compensate the timing
    int timing_miso_delay;              ///< Extra miso delay clocks to compensate the timing
} spi_hal_timing_conf_t;

/**
 * DMA configuration structure
 * Should be set by driver at initialization
 */
typedef struct {
    spi_dma_dev_t *dma_in;              ///< Input  DMA(DMA -> RAM) peripheral register address
    spi_dma_dev_t *dma_out;             ///< Output DMA(RAM -> DMA) peripheral register address
    bool dma_enabled;                   ///< Whether the DMA is enabled, do not update after initialization
    lldesc_t  *dmadesc_tx;              /**< Array of DMA descriptor used by the TX DMA.
                                         *   The amount should be larger than dmadesc_n. The driver should ensure that
                                         *   the data to be sent is shorter than the descriptors can hold.
                                         */
    lldesc_t *dmadesc_rx;               /**< Array of DMA descriptor used by the RX DMA.
                                         *   The amount should be larger than dmadesc_n. The driver should ensure that
                                         *   the data to be sent is shorter than the descriptors can hold.
                                         */
    uint32_t tx_dma_chan;               ///< TX DMA channel
    uint32_t rx_dma_chan;               ///< RX DMA channel
    int dmadesc_n;                      ///< The amount of descriptors of both ``dmadesc_tx`` and ``dmadesc_rx`` that the HAL can use.
} spi_hal_config_t;

/**
 * Transaction configuration structure, this should be assigned by driver each time.
 * All these parameters will be updated to the peripheral every transaction.
 */
typedef struct {
    uint16_t cmd;                       ///< Command value to be sent
    int cmd_bits;                       ///< Length (in bits) of the command phase
    int addr_bits;                      ///< Length (in bits) of the address phase
    int dummy_bits;                     ///< Base length (in bits) of the dummy phase. Note when the compensation is enabled, some extra dummy bits may be appended.
    int tx_bitlen;                      ///< TX length, in bits
    int rx_bitlen;                      ///< RX length, in bits
    uint64_t addr;                      ///< Address value to be sent
    uint8_t *send_buffer;               ///< Data to be sent
    uint8_t *rcv_buffer;                ///< Buffer to hold the receive data.
    spi_line_mode_t line_mode;          ///< SPI line mode of this transaction
    int cs_keep_active;                 ///< Keep CS active after transaction
} spi_hal_trans_config_t;

/**
 * Context that should be maintained by both the driver and the HAL.
 */
typedef struct {
    /* These two need to be malloced by the driver first */
    lldesc_t  *dmadesc_tx;              /**< Array of DMA descriptor used by the TX DMA.
                                         *   The amount should be larger than dmadesc_n. The driver should ensure that
                                         *   the data to be sent is shorter than the descriptors can hold.
                                         */
    lldesc_t *dmadesc_rx;               /**< Array of DMA descriptor used by the RX DMA.
                                         *   The amount should be larger than dmadesc_n. The driver should ensure that
                                         *   the data to be sent is shorter than the descriptors can hold.
                                         */

    /* Configured by driver at initialization, don't touch */
    spi_dev_t     *hw;                  ///< Beginning address of the peripheral registers.
    spi_dma_dev_t *dma_in;              ///< Address of the DMA peripheral registers which stores the data received from a peripheral into RAM (DMA -> RAM).
    spi_dma_dev_t *dma_out;             ///< Address of the DMA peripheral registers which transmits the data from RAM to a peripheral (RAM -> DMA).
    bool  dma_enabled;                  ///< Whether the DMA is enabled, do not update after initialization
    uint32_t tx_dma_chan;               ///< TX DMA channel
    uint32_t rx_dma_chan;               ///< RX DMA channel
    int dmadesc_n;                      ///< The amount of descriptors of both ``dmadesc_tx`` and ``dmadesc_rx`` that the HAL can use.

    /* Internal parameters, don't touch */
    spi_hal_trans_config_t trans_config; ///< Transaction configuration
} spi_hal_context_t;

/**
 * Device configuration structure, this should be initialised by driver based on different devices respectively.
 * All these parameters will be updated to the peripheral only when ``spi_hal_setup_device``.
 * They may not get updated when ``spi_hal_setup_trans``.
 */
typedef struct {
    int mode;                           ///< SPI mode, device specific
    int cs_setup;                       ///< Setup time of CS active edge before the first SPI clock, device specific
    int cs_hold;                        ///< Hold time of CS inactive edge after the last SPI clock, device specific
    int cs_pin_id;                      ///< CS pin to use, 0-2, otherwise all the CS pins are not used. Device specific
    spi_hal_timing_conf_t timing_conf;  /**< This structure holds the pre-calculated timing configuration for the device
                                         *   at initialization, device specific
                                         */
    struct {
        uint32_t sio : 1;               ///< Whether to use SIO mode, device specific
        uint32_t half_duplex : 1;       ///< Whether half duplex mode is used, device specific
        uint32_t tx_lsbfirst : 1;       ///< Whether LSB is sent first for TX data, device specific
        uint32_t rx_lsbfirst : 1;       ///< Whether LSB is received first for RX data, device specific
        uint32_t no_compensate : 1;     ///< No need to add dummy to compensate the timing, device specific
#if SOC_SPI_SUPPORT_AS_CS
        uint32_t as_cs  : 1;            ///< Whether to toggle the CS while the clock toggles, device specific
#endif
        uint32_t positive_cs : 1;       ///< Whether the postive CS feature is abled, device specific
    };//boolean configurations
} spi_hal_dev_config_t;

/**
 * Init the peripheral and the context.
 *
 * @param hal        Context of the HAL layer.
 * @param host_id    Index of the SPI peripheral. 0 for SPI1, 1 for SPI2 and 2 for SPI3.
 * @param hal_config Configuration of the hal defined by the upper layer.
 */
void spi_hal_init(spi_hal_context_t *hal, uint32_t host_id, const spi_hal_config_t *hal_config);

/**
 * Deinit the peripheral (and the context if needed).
 *
 * @param hal Context of the HAL layer.
 */
void spi_hal_deinit(spi_hal_context_t *hal);

/**
 * Setup device-related configurations according to the settings in the context.
 *
 * @param hal       Context of the HAL layer.
 * @param hal_dev   Device configuration
 */
void spi_hal_setup_device(spi_hal_context_t *hal, const spi_hal_dev_config_t *hal_dev);

/**
 * Setup transaction related configurations according to the settings in the context.
 *
 * @param hal       Context of the HAL layer.
 * @param hal_dev   Device configuration
 * @param hal_trans Transaction configuration
 */
void spi_hal_setup_trans(spi_hal_context_t *hal, const spi_hal_dev_config_t *hal_dev, const spi_hal_trans_config_t *hal_trans);

/**
 * Prepare the data for the current transaction.
 *
 * @param hal            Context of the HAL layer.
 * @param hal_dev        Device configuration
 * @param hal_trans      Transaction configuration
 */
void spi_hal_prepare_data(spi_hal_context_t *hal, const spi_hal_dev_config_t *hal_dev, const spi_hal_trans_config_t *hal_trans);

/**
 * Trigger start a user-defined transaction.
 *
 * @param hal Context of the HAL layer.
 */
void spi_hal_user_start(const spi_hal_context_t *hal);

/**
 * Check whether the transaction is done (trans_done is set).
 *
 * @param hal Context of the HAL layer.
 */
bool spi_hal_usr_is_done(const spi_hal_context_t *hal);

/**
 * Post transaction operations, mainly fetch data from the buffer.
 *
 * @param hal       Context of the HAL layer.
 */
void spi_hal_fetch_result(const spi_hal_context_t *hal);

/*----------------------------------------------------------
 * Utils
 * ---------------------------------------------------------*/
/**
 * Calculate the configuration of clock and timing. The configuration will be used when ``spi_hal_setup_device``.
 *
 * It is highly suggested to do this at initialization, since it takes long time.
 *
 * @param timing_param   Input parameters to calculate timing configuration
 * @param out_freq       Output of the actual frequency, left NULL if not required.
 * @param timing_conf    Output of the timing configuration.
 *
 * @return ESP_OK if desired is available, otherwise fail.
 */
esp_err_t spi_hal_cal_clock_conf(const spi_hal_timing_param_t *timing_param, int *out_freq, spi_hal_timing_conf_t *timing_conf);

/**
 * Get the frequency actual used.
 *
 * @param hal            Context of the HAL layer.
 * @param fapb           APB clock frequency.
 * @param hz             Desired frequencyc.
 * @param duty_cycle     Desired duty cycle.
 */
int spi_hal_master_cal_clock(int fapb, int hz, int duty_cycle);

/**
 * Get the timing configuration for given parameters.
 *
 * @param eff_clk        Actual SPI clock frequency
 * @param gpio_is_used   true if the GPIO matrix is used, otherwise false.
 * @param input_delay_ns Maximum delay between SPI launch clock and the data to
 *                       be valid. This is used to compensate/calculate the maximum frequency
 *                       allowed. Left 0 if not known.
 * @param dummy_n        Dummy cycles required to correctly read the data.
 * @param miso_delay_n   suggested delay on the MISO line, in APB clocks.
 */
void spi_hal_cal_timing(int eff_clk, bool gpio_is_used, int input_delay_ns, int *dummy_n, int *miso_delay_n);

/**
 * Get the maximum frequency allowed to read if no compensation is used.
 *
 * @param gpio_is_used   true if the GPIO matrix is used, otherwise false.
 * @param input_delay_ns Maximum delay between SPI launch clock and the data to
 *                       be valid. This is used to compensate/calculate the maximum frequency
 *                       allowed. Left 0 if not known.
 */
int spi_hal_get_freq_limit(bool gpio_is_used, int input_delay_ns);
