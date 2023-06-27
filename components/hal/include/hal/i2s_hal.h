/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The HAL layer for I2S.
// There is no parameter check in the hal layer, so the caller must ensure the correctness of the parameters.

#pragma once

#include "soc/soc_caps.h"
#include "hal/i2s_types.h"
#include "hal/i2s_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief General slot configuration information
 * @note It is a general purpose struct, not supposed to be used directly by user
 */
typedef struct {
    i2s_data_bit_width_t    data_bit_width;     /*!< I2S sample data bit width (valid data bits per sample) */
    i2s_slot_bit_width_t    slot_bit_width;     /*!< I2S slot bit width (total bits per slot) */
    i2s_slot_mode_t         slot_mode;          /*!< Set mono or stereo mode with I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO */
    union {
        /* STD configurations */
        struct {
            i2s_std_slot_mask_t     slot_mask;          /*!< Select the left, right or both slot */
            uint32_t                ws_width;           /*!< WS signal width (i.e. the number of bclk ticks that ws signal is high) */
            bool                    ws_pol;             /*!< WS signal polarity, set true to enable high lever first */
            bool                    bit_shift;          /*!< Set to enbale bit shift in Philips mode */
#if SOC_I2S_HW_VERSION_1    // For esp32/esp32-s2
            bool                    msb_right;          /*!< Set to place right channel data at the MSB in the FIFO */
#else
            bool                    left_align;         /*!< Set to enable left alignment */
            bool                    big_endian;         /*!< Set to enable big endian */
            bool                    bit_order_lsb;      /*!< Set to enable lsb first */
#endif
        } std;                                          /*!< Specific configurations for standard mode */

#if SOC_I2S_SUPPORTS_TDM
        /* TDM configurations */
        struct {
            i2s_tdm_slot_mask_t     slot_mask;          /*!< Slot mask. Activating slots by setting 1 to corresponding bits. When the activated slots is not consecutive, those data in unactivated slots will be ignored */
            uint32_t                ws_width;           /*!< WS signal width ((i.e. the number of bclk ticks that ws signal is high)) */
            bool                    ws_pol;             /*!< WS signal polarity, set true to enable high lever first */
            bool                    bit_shift;          /*!< Set true to enable bit shift in Philips mode */

            bool                    left_align;         /*!< Set true to enable left alignment */
            bool                    big_endian;         /*!< Set true to enable big endian */
            bool                    bit_order_lsb;      /*!< Set true to enable lsb first */

            bool                    skip_mask;          /*!< Set true to enable skip mask. If it is enabled, only the data of the enabled channels will be sent, otherwise all data stored in DMA TX buffer will be sent */
            uint32_t                total_slot;         /*!< I2S total number of slots. If it is smaller than the biggest activated channel number, it will be set to this number automatically. */
        } tdm;                                          /*!< Specific configurations for TDM mode */
#endif

#if SOC_I2S_SUPPORTS_PDM_TX
        /* PDM TX configurations */
        struct {
#if SOC_I2S_HW_VERSION_1
            i2s_pdm_slot_mask_t     slot_mask;          /*!< Slot mask to choose left or right slot */
#endif
            uint32_t                sd_prescale;        /*!< Sigma-delta filter prescale */
            i2s_pdm_sig_scale_t     sd_scale;           /*!< Sigma-delta filter scaling value */
            i2s_pdm_sig_scale_t     hp_scale;           /*!< High pass filter scaling value */
            i2s_pdm_sig_scale_t     lp_scale;           /*!< Low pass filter scaling value */
            i2s_pdm_sig_scale_t     sinc_scale;         /*!< Sinc filter scaling value */
#if SOC_I2S_HW_VERSION_2
            i2s_pdm_tx_line_mode_t  line_mode;          /*!< PDM TX line mode, on-line codec, one-line dac, two-line dac mode can be selected */
            bool                    hp_en;              /*!< High pass filter enable */
            float                   hp_cut_off_freq_hz; /*!< High pass filter cut-off frequency, range 23.3Hz ~ 185Hz, see cut-off frequency sheet above */
            uint32_t                sd_dither;          /*!< Sigma-delta filter dither */
            uint32_t                sd_dither2;         /*!< Sigma-delta filter dither2 */
#endif // SOC_I2S_HW_VERSION_2
        } pdm_tx;                                       /*!< Specific configurations for PDM TX mode */
#endif
#if SOC_I2S_SUPPORTS_PDM_RX
        /* PDM TX configurations */
        struct {
            i2s_pdm_slot_mask_t     slot_mask;          /*!< Choose the slots to activate */
        } pdm_rx;                                       /*!< Specific configurations for PDM TX mode */
#endif
    };

} i2s_hal_slot_config_t;

/**
 * @brief I2S clock configuration
 */
typedef struct {
    uint32_t sclk;                              /*!< I2S module clock */
    uint32_t mclk;                              /*!< I2S master clock */
    uint32_t bclk;                              /*!< I2S bit clock */
    uint16_t mclk_div;                          /*!< I2S master clock division */
    uint16_t bclk_div;                          /*!< I2S bit clock division*/
} i2s_hal_clock_info_t;

/**
 * Context that should be maintained by both the driver and the HAL
 */
typedef struct {
    i2s_dev_t *dev;                             /*!< I2S instance address */
} i2s_hal_context_t;

/**
 * @brief Init I2S hal context
 *
 * @param hal Context of the HAL layer
 * @param port_id The I2S port number, the max port number is (SOC_I2S_NUM -1)
 */
void i2s_hal_init(i2s_hal_context_t *hal, int port_id);

/**
 * @brief Helper function for calculating the precise mclk division by sclk and mclk
 *
 * @param sclk      system clock
 * @param mclk      module clock
 * @param mclk_div  mclk division coefficients, including integer part and decimal part
 */
void i2s_hal_calc_mclk_precise_division(uint32_t sclk, uint32_t mclk, i2s_ll_mclk_div_t *mclk_div);

/**
 * @brief Set tx channel clock
 *
 * @param hal Context of the HAL layer
 * @param clk_info clock information
 * @param clk_src clock source
 */
void i2s_hal_set_tx_clock(i2s_hal_context_t *hal, const i2s_hal_clock_info_t *clk_info, i2s_clock_src_t clk_src);

/**
 * @brief Set rx channel clock
 *
 * @param hal Context of the HAL layer
 * @param clk_info clock information
 * @param clk_src clock source
 */
void i2s_hal_set_rx_clock(i2s_hal_context_t *hal, const i2s_hal_clock_info_t *clk_info, i2s_clock_src_t clk_src);


/*-------------------------------------------------------------------------
 |                           STD configuration                            |
 -------------------------------------------------------------------------*/
/**
 * @brief Set tx slot to standard mode
 *
 * @param hal Context of the HAL layer
 * @param is_slave If is slave role
 * @param slot_config General slot configuration pointer, but will specified to i2s standard mode
 */
void i2s_hal_std_set_tx_slot(i2s_hal_context_t *hal, bool is_slave, const i2s_hal_slot_config_t *slot_cfg);

/**
 * @brief Set rx slot to standard mode
 *
 * @param hal Context of the HAL layer
 * @param is_slave If is slave role
 * @param slot_config General slot configuration pointer, but will specified to i2s standard mode
 */
void i2s_hal_std_set_rx_slot(i2s_hal_context_t *hal, bool is_slave, const i2s_hal_slot_config_t *slot_cfg);

/**
 * @brief Enable tx channel as standard mode
 *
 * @param hal Context of the HAL layer
 */
void i2s_hal_std_enable_tx_channel(i2s_hal_context_t *hal);

/**
 * @brief Enable rx channel as standard mode
 *
 * @param hal Context of the HAL layer
 */
void i2s_hal_std_enable_rx_channel(i2s_hal_context_t *hal);

/*-------------------------------------------------------------------------
 |                           PDM configuration                            |
 -------------------------------------------------------------------------*/
#if SOC_I2S_SUPPORTS_PDM

#if SOC_I2S_SUPPORTS_PDM_TX
/**
 * @brief Set tx slot to pdm mode
 *
 * @param hal Context of the HAL layer
 * @param is_slave If is slave role
 * @param slot_config General slot configuration pointer, but will specified to i2s pdm tx mode
 */
void i2s_hal_pdm_set_tx_slot(i2s_hal_context_t *hal, bool is_slave, const i2s_hal_slot_config_t *slot_cfg);

/**
 * @brief Enable tx channel as pdm mode
 *
 * @param hal Context of the HAL layer
 */
void i2s_hal_pdm_enable_tx_channel(i2s_hal_context_t *hal);
#endif // SOC_I2S_SUPPORTS_PDM_TX

#if SOC_I2S_SUPPORTS_PDM_RX
/**
 * @brief Set rx slot to pdm mode
 *
 * @param hal Context of the HAL layer
 * @param is_slave If is slave role
 * @param slot_config General slot configuration pointer, but will specified to i2s pdm rx mode
 */
void i2s_hal_pdm_set_rx_slot(i2s_hal_context_t *hal, bool is_slave, const i2s_hal_slot_config_t *slot_cfg);

/**
 * @brief Enable rx channel as pdm mode
 *
 * @param hal Context of the HAL layer
 */
void i2s_hal_pdm_enable_rx_channel(i2s_hal_context_t *hal);
#endif // SOC_I2S_SUPPORTS_PDM_RX
#endif // SOC_I2S_SUPPORTS_PDM

/*-------------------------------------------------------------------------
 |                           TDM configuration                            |
 -------------------------------------------------------------------------*/
#if SOC_I2S_SUPPORTS_TDM
/**
 * @brief Set tx slot to tdm mode
 *
 * @param hal Context of the HAL layer
 * @param is_slave If is slave role
 * @param slot_config General slot configuration pointer, but will specified to i2s tdm mode
 */
void i2s_hal_tdm_set_tx_slot(i2s_hal_context_t *hal, bool is_slave, const i2s_hal_slot_config_t *slot_cfg);

/**
 * @brief Set rx slot to tdm mode
 *
 * @param hal Context of the HAL layer
 * @param is_slave If is slave role
 * @param slot_config General slot configuration pointer, but will specified to i2s tdm mode
 */
void i2s_hal_tdm_set_rx_slot(i2s_hal_context_t *hal, bool is_slave, const i2s_hal_slot_config_t *slot_cfg);

/**
 * @brief Enable tx channel as tdm mode
 *
 * @param hal Context of the HAL layer
 */
void i2s_hal_tdm_enable_tx_channel(i2s_hal_context_t *hal);

/**
 * @brief Enable rx channel as tdm mode
 *
 * @param hal Context of the HAL layer
 */
void i2s_hal_tdm_enable_rx_channel(i2s_hal_context_t *hal);
#endif

/**
 * @brief Start I2S TX channel
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_tx_start(hal)                   i2s_ll_tx_start((hal)->dev)

/**
 * @brief Stop I2S TX channel
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_tx_stop(hal)                   i2s_ll_tx_stop((hal)->dev)

/**
 * @brief Reset I2S TX channel
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_tx_reset(hal)                   i2s_ll_tx_reset((hal)->dev)

/**
 * @brief Reset I2S TX fifo
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_tx_reset_fifo(hal)              i2s_ll_tx_reset_fifo((hal)->dev)

/**
 * @brief Start I2S RX channel
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_rx_start(hal)                   i2s_ll_rx_start((hal)->dev)

/**
 * @brief Stop I2S RX channel
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_rx_stop(hal)                   i2s_ll_rx_stop((hal)->dev)

/**
 * @brief Reset I2S RX channel
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_rx_reset(hal)                   i2s_ll_rx_reset((hal)->dev)

/**
 * @brief Reset I2S RX fifo
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_rx_reset_fifo(hal)              i2s_ll_rx_reset_fifo((hal)->dev)


#if !SOC_I2S_SUPPORTS_GDMA
/**
 * @brief Enable I2S TX DMA
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_tx_enable_dma(hal) i2s_ll_enable_dma((hal)->dev,true)

/**
 * @brief Enable I2S RX DMA
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_rx_enable_dma(hal) i2s_ll_enable_dma((hal)->dev,true)

/**
 * @brief Disable I2S TX DMA
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_tx_disable_dma(hal) i2s_ll_enable_dma((hal)->dev,false)

/**
 * @brief Disable I2S RX DMA
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_rx_disable_dma(hal) i2s_ll_enable_dma((hal)->dev,false)

/**
 * @brief Get I2S interrupt status
 *
 * @param hal Context of the HAL layer
 * @return
 *        - module interrupt status
 */
#define i2s_hal_get_intr_status(hal) i2s_ll_get_intr_status((hal)->dev)

/**
 * @brief Get I2S interrupt status
 *
 * @param hal Context of the HAL layer
 * @param mask Interrupt mask to be cleared.
 */
#define i2s_hal_clear_intr_status(hal, mask) i2s_ll_clear_intr_status((hal)->dev, mask)

/**
 * @brief Enable I2S RX interrupt
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_rx_enable_intr(hal) i2s_ll_rx_enable_intr((hal)->dev)

/**
 * @brief Disable I2S RX interrupt
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_rx_disable_intr(hal) i2s_ll_rx_disable_intr((hal)->dev)

/**
 * @brief Disable I2S TX interrupt
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_tx_disable_intr(hal) i2s_ll_tx_disable_intr((hal)->dev)

/**
 * @brief Enable I2S TX interrupt
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_tx_enable_intr(hal) i2s_ll_tx_enable_intr((hal)->dev)

/**
 * @brief Configure TX DMA descriptor address and start TX DMA
 *
 * @param hal Context of the HAL layer
 * @param link_addr DMA descriptor link address.
 */
#define i2s_hal_tx_start_link(hal, link_addr) i2s_ll_tx_start_link((hal)->dev, link_addr)

/**
 * @brief Configure RX DMA descriptor address and start RX DMA
 *
 * @param hal Context of the HAL layer
 * @param link_addr DMA descriptor link address.
 */
#define i2s_hal_rx_start_link(hal, link_addr) i2s_ll_rx_start_link((hal)->dev, link_addr)

/**
 * @brief Stop TX DMA link
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_tx_stop_link(hal) i2s_ll_tx_stop_link((hal)->dev)

/**
 * @brief Stop RX DMA link
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_rx_stop_link(hal) i2s_ll_rx_stop_link((hal)->dev)

/**
 * @brief Reset RX DMA
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_rx_reset_dma(hal) i2s_ll_rx_reset_dma((hal)->dev)

/**
 * @brief Reset TX DMA
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_tx_reset_dma(hal) i2s_ll_tx_reset_dma((hal)->dev)

/**
 * @brief Get I2S out eof descriptor address
 *
 * @param hal Context of the HAL layer
 * @param addr Pointer to accept out eof des address
 */
#define i2s_hal_get_out_eof_des_addr(hal, addr) i2s_ll_tx_get_eof_des_addr((hal)->dev, addr)

/**
 * @brief Get I2S in suc eof descriptor address
 *
 * @param hal Context of the HAL layer
 * @param addr Pointer to accept in suc eof des address
 */
#define i2s_hal_get_in_eof_des_addr(hal, addr) i2s_ll_rx_get_eof_des_addr((hal)->dev, addr)
#endif

#ifdef __cplusplus
}
#endif
