/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The HAL layer for SPI master (common part)

// SPI HAL usages (without DMA):
// 1. initialize the bus
// 2. setup the clock speed (since this takes long time)
// 3. call setup_device to update parameters for the specific device
// 4. call setup_trans to update parameters for the specific transaction
// 5. prepare data to send into hw registers
// 6. trigger user defined SPI transaction to start
// 7. wait until the user transaction is done
// 8. fetch the received data
// Parameter to be updated only during ``setup_device`` will be highlighted in the
// field comments.

#pragma once
#include "esp_err.h"
#include "soc/soc_caps.h"
#include "hal/spi_types.h"
#include "hal/dma_types.h"
#if SOC_GDMA_SUPPORTED
#include "soc/gdma_channel.h"
#endif
#if SOC_GPSPI_SUPPORTED
#include "hal/spi_ll.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_GPSPI_SUPPORTED

#if SOC_GDMA_TRIG_PERIPH_SPI2_BUS == SOC_GDMA_BUS_AHB
typedef dma_descriptor_align4_t spi_dma_desc_t;
#else
typedef dma_descriptor_align8_t spi_dma_desc_t;
#endif

/**
 * Input parameters to the ``spi_hal_cal_clock_conf`` to calculate the timing configuration
 */
typedef struct {
    uint32_t clk_src_hz;                ///< Selected SPI clock source speed in Hz
    uint32_t half_duplex;               ///< Whether half duplex mode is used, device specific
    uint32_t no_compensate;             ///< No need to add dummy to compensate the timing, device specific
    uint32_t expected_freq;             ///< Expected frequency in Hz.
    uint32_t duty_cycle;                ///< Expected duty cycle of SPI clock
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
    spi_clock_source_t clock_source;    ///< Clock source of each device used by LL layer
    uint32_t source_pre_div;            ///< Pre divider befor enter SPI peripheral
    int real_freq;                      ///< Output of the actual frequency
    int timing_dummy;                   ///< Extra dummy needed to compensate the timing
    int timing_miso_delay;              ///< Extra miso delay clocks to compensate the timing
} spi_hal_timing_conf_t;

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
    /* Configured by driver at initialization, don't touch */
    spi_dev_t     *hw;                  ///< Beginning address of the peripheral registers.
    bool  dma_enabled;                  ///< Whether the DMA is enabled, do not update after initialization
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
#if SOC_SPI_AS_CS_SUPPORTED
        uint32_t as_cs  : 1;            ///< Whether to toggle the CS while the clock toggles, device specific
#endif
        uint32_t positive_cs : 1;       ///< Whether the postive CS feature is abled, device specific
    };//boolean configurations
} spi_hal_dev_config_t;

#if SOC_SPI_SCT_SUPPORTED
/**
 * SCT mode required configurations, per segment
 */
typedef struct {
    /* CONF State */
    bool seg_end;                       ///< True: this segment is the end; False: this segment isn't the end;
    uint32_t seg_gap_len;               ///< spi clock length of CS inactive on config phase for sct
    /* PREP State */
    int cs_setup;                       ///< Setup time of CS active edge before the first SPI clock
    /* CMD State */
    uint16_t cmd;                       ///< Command value to be sent
    int cmd_bits;                       ///< Length (in bits) of the command phase
    /* ADDR State */
    uint64_t addr;                      ///< Address value to be sent
    int addr_bits;                      ///< Length (in bits) of the address phase
    /* DUMMY State */
    int dummy_bits;                     ///< Base length (in bits) of the dummy phase.
    /* DOUT State */
    int tx_bitlen;                      ///< TX length, in bits
    /* DIN State */
    int rx_bitlen;                      ///< RX length, in bits
    /* DONE State */
    int cs_hold;                        ///< Hold time of CS inactive edge after the last SPI clock
} spi_hal_seg_config_t;
#endif  //#if SOC_SPI_SCT_SUPPORTED

/**
 * Init the peripheral and the context.
 *
 * @param hal        Context of the HAL layer.
 * @param host_id    Index of the SPI peripheral. 0 for SPI1, 1 for SPI2 and 2 for SPI3.
 */
void spi_hal_init(spi_hal_context_t *hal, uint32_t host_id);

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
 * Enable/Disable miso/mosi signals on peripheral side
 *
 * @param hw        Beginning address of the peripheral registers.
 * @param mosi_ena  enable/disable mosi line
 * @param miso_ena  enable/disable miso line
 */
void spi_hal_enable_data_line(spi_dev_t *hw, bool mosi_ena, bool miso_ena);

/**
 * Prepare tx hardware for a new DMA trans
 *
 * @param hw Beginning address of the peripheral registers.
 */
void spi_hal_hw_prepare_rx(spi_dev_t *hw);

/**
 * Prepare tx hardware for a new DMA trans
 *
 * @param hw Beginning address of the peripheral registers.
 */
void spi_hal_hw_prepare_tx(spi_dev_t *hw);

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
 * Setup transaction operations, write tx buffer to HW registers
 *
 * @param hal       Context of the HAL layer.
 * @param hal_trans Transaction configuration.
 */
void spi_hal_push_tx_buffer(const spi_hal_context_t *hal, const spi_hal_trans_config_t *hal_trans);

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
 * @param timing_conf    Output of the timing configuration.
 *
 * @return ESP_OK if desired is available, otherwise fail.
 */
esp_err_t spi_hal_cal_clock_conf(const spi_hal_timing_param_t *timing_param, spi_hal_timing_conf_t *timing_conf);

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
 * @param source_freq_hz Clock freq of selected clock source for SPI in Hz.
 * @param eff_clk        Actual SPI clock frequency
 * @param gpio_is_used   true if the GPIO matrix is used, otherwise false.
 * @param input_delay_ns Maximum delay between SPI launch clock and the data to
 *                       be valid. This is used to compensate/calculate the maximum frequency
 *                       allowed. Left 0 if not known.
 * @param dummy_n        Dummy cycles required to correctly read the data.
 * @param miso_delay_n   suggested delay on the MISO line, in APB clocks.
 */
void spi_hal_cal_timing(int source_freq_hz, int eff_clk, bool gpio_is_used, int input_delay_ns, int *dummy_n, int *miso_delay_n);

/**
 * Get the maximum frequency allowed to read if no compensation is used.
 *
 * @param gpio_is_used   true if the GPIO matrix is used, otherwise false.
 * @param input_delay_ns Maximum delay between SPI launch clock and the data to
 *                       be valid. This is used to compensate/calculate the maximum frequency
 *                       allowed. Left 0 if not known.
 */
int spi_hal_get_freq_limit(bool gpio_is_used, int input_delay_ns);

#if SOC_SPI_SCT_SUPPORTED
/*----------------------------------------------------------
 * Segmented-Configure-Transfer (SCT) Mode
 * ---------------------------------------------------------*/
/**
 * Initialise SCT mode required registers and hal states
 *
 * @param hal            Context of the HAL layer.
 */
void spi_hal_sct_init(spi_hal_context_t *hal);

/**
 * Initialise conf buffer, give it an initial value
 *
 * @param hal            Context of the HAL layer.
 */
void spi_hal_sct_init_conf_buffer(spi_hal_context_t *hal, uint32_t conf_buffer[SOC_SPI_SCT_BUFFER_NUM_MAX]);

/**
 * Format the conf buffer
 * According to the `spi_hal_seg_config_t`, update the conf buffer
 *
 * @param hal            Context of the HAL layer.
 * @param config         Conf buffer configuration, per segment. See `spi_hal_seg_config_t` to know what can be configured
 * @param conf_buffer    Conf buffer
 */
void spi_hal_sct_format_conf_buffer(spi_hal_context_t *hal, const spi_hal_seg_config_t *config, const spi_hal_dev_config_t *dev, uint32_t conf_buffer[SOC_SPI_SCT_BUFFER_NUM_MAX]);

/**
 * Deinit SCT mode related registers and hal states
 */
void spi_hal_sct_deinit(spi_hal_context_t *hal);

/**
 * Set conf_bitslen to HW for sct.
 */
void spi_hal_sct_set_conf_bits_len(spi_hal_context_t *hal, uint32_t conf_len);

/**
 * Clear SPI interrupt bits by mask
 */
void spi_hal_clear_intr_mask(spi_hal_context_t *hal, uint32_t mask);

/**
 * Get SPI interrupt bits status by mask
 */
bool spi_hal_get_intr_mask(spi_hal_context_t *hal, uint32_t mask);

/**
 * Set conf_bitslen base to HW for sct, only supported on s2.
 */
#define spi_hal_sct_setup_conf_base(hal, conf_base)     spi_ll_set_conf_base_bitslen((hal)->hw, conf_base)
#endif  //#if SOC_SPI_SCT_SUPPORTED
#endif  //#if SOC_GPSPI_SUPPORTED

#ifdef __cplusplus
}
#endif
