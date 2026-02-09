/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include "esp_check.h"
#include "sdkconfig.h"
#include "esp_rom_gpio.h"
#include "driver/gpio.h"
#include "soc/soc_caps.h"
#include "soc/gpio_sig_map.h"
#include "soc/io_mux_reg.h"
#include "hal/emac_periph.h"
#include "esp_private/gpio.h"
#include "esp_private/eth_mac_esp_gpio.h"
#include "esp_private/esp_gpio_reserve.h"
#include "esp_log.h"

#define GET_GPIO_OR_SINGLE(cfg, num) cfg == NULL ? GPIO_NUM_MAX : cfg->num

static const char *TAG = "esp.emac.gpio";

static uint64_t s_emac_esp_used_gpio_mask = 0x0;

static esp_err_t emac_esp_gpio_matrix_init(gpio_num_t gpio_num, uint32_t signal_in_idx, uint32_t signal_out_idx, gpio_mode_t mode)
{
    // silently skip when user don't want to connect the signal to GPIO pad
    if (gpio_num <= GPIO_NUM_NC) {
        ESP_LOGD(TAG, "%s skipping signal in_idx %" PRIu32 ", out_idx %" PRIu32, __func__, signal_in_idx, signal_out_idx);
        return ESP_OK;
    }
    switch(mode) {
        case GPIO_MODE_INPUT:
            ESP_RETURN_ON_FALSE(signal_in_idx != SIG_GPIO_OUT_IDX, ESP_ERR_NOT_SUPPORTED,
                                TAG, "requested periph signal cannot be connect via GPIO Matrix");
            ESP_RETURN_ON_FALSE(esp_gpio_is_reserved(BIT64(gpio_num)) == false, ESP_ERR_INVALID_STATE,
                                TAG, "GPIO %i is reserved", gpio_num);
            gpio_input_enable(gpio_num);
            esp_rom_gpio_connect_in_signal(gpio_num, signal_in_idx, false);
            break;
        case GPIO_MODE_OUTPUT:
            ESP_RETURN_ON_FALSE(signal_out_idx != SIG_GPIO_OUT_IDX, ESP_ERR_NOT_SUPPORTED,
                                TAG, "requested periph signal cannot be connect via GPIO Matrix");
            ESP_RETURN_ON_FALSE((esp_gpio_reserve(BIT64(gpio_num)) & BIT64(gpio_num)) == 0, ESP_ERR_INVALID_STATE,
                                TAG, "GPIO %i is already reserved", gpio_num);
            esp_rom_gpio_connect_out_signal(gpio_num, signal_out_idx, false, false);
            break;
        case GPIO_MODE_INPUT_OUTPUT:
            ESP_RETURN_ON_FALSE(signal_in_idx != SIG_GPIO_OUT_IDX, ESP_ERR_NOT_SUPPORTED,
                                TAG, "requested periph signal cannot be connect via GPIO Matrix");
            ESP_RETURN_ON_FALSE(signal_out_idx != SIG_GPIO_OUT_IDX, ESP_ERR_NOT_SUPPORTED,
                                TAG, "requested periph signal cannot be connect via GPIO Matrix");
            ESP_RETURN_ON_FALSE((esp_gpio_reserve(BIT64(gpio_num)) & BIT64(gpio_num)) == 0, ESP_ERR_INVALID_STATE,
                                TAG, "GPIO %i is already reserved", gpio_num);
            gpio_input_enable(gpio_num);
            esp_rom_gpio_connect_out_signal(gpio_num, signal_out_idx, false, false);
            esp_rom_gpio_connect_in_signal(gpio_num, signal_in_idx, false);
            break;
        default:
            return ESP_ERR_INVALID_ARG;
    }
    s_emac_esp_used_gpio_mask |= BIT64(gpio_num);
    ESP_RETURN_ON_ERROR(gpio_set_pull_mode(gpio_num, GPIO_FLOATING), TAG, "failed to set pull mode at GPIO %i", gpio_num);
    ESP_RETURN_ON_ERROR(gpio_func_sel(gpio_num, PIN_FUNC_GPIO), TAG, "failed to set GPIO function at GPIO #%i", gpio_num);
    return ESP_OK;
}

static esp_err_t emac_esp_iomux_init(gpio_num_t gpio_num, const emac_iomux_info_t *iomux_info, uint32_t signal_idx, bool is_input)
{
    // silently skip undefined iomux functions (for example, ESP32 does not use MII COL_IN/CRS_IN)
    if (iomux_info == NULL) {
        ESP_LOGD(TAG, "%s skipping target undefined iomux periph function", __func__);
        return ESP_OK;
    }
    // silently skip when user don't want to iomux the function to GPIO pad
    if (gpio_num <= GPIO_NUM_NC) {
        ESP_LOGD(TAG, "%s user defined GPIO not connected - skipping", __func__);
        return ESP_OK;
    }
    // loop over target iomux_info until reached end of list indicated by invalid GPIO num
    while (iomux_info->gpio_num != GPIO_NUM_MAX) {
        // if requested GPIO number can be IO muxed or select the only pad that can be muxed on the target
        if(gpio_num == iomux_info->gpio_num || gpio_num == GPIO_NUM_MAX) {
            ESP_RETURN_ON_FALSE((esp_gpio_reserve(BIT64(iomux_info->gpio_num)) & BIT64(iomux_info->gpio_num)) == 0, ESP_ERR_INVALID_STATE,
                                TAG, "GPIO %i is already reserved", iomux_info->gpio_num);
            s_emac_esp_used_gpio_mask |= BIT64(iomux_info->gpio_num);
            if (is_input) {
                ESP_RETURN_ON_ERROR(gpio_iomux_input(iomux_info->gpio_num, iomux_info->func, signal_idx), TAG, "failed to set perip. input via IOMUX");
            } else {
                ESP_RETURN_ON_ERROR(gpio_iomux_output(iomux_info->gpio_num, iomux_info->func), TAG, "failed to set perip. output via IOMUX");
            }
            ESP_RETURN_ON_ERROR(gpio_set_pull_mode(iomux_info->gpio_num, GPIO_FLOATING),
                                TAG, "failed to set pull mode at GPIO %i", iomux_info->gpio_num);
            return ESP_OK;
        }
        iomux_info++;
    }
    return ESP_FAIL;
}

esp_err_t emac_esp_gpio_init_smi(const emac_esp_smi_gpio_config_t *smi_gpio)
{
    if (smi_gpio->mdc_num >= 0) {
        /* Setup SMI MDC GPIO */
        ESP_RETURN_ON_ERROR(emac_esp_gpio_matrix_init(smi_gpio->mdc_num, 0, emac_io_idx.mdc_idx, GPIO_MODE_OUTPUT),
                            TAG, "MDC GPIO matrix config failed");
    }
    if (smi_gpio->mdio_num >= 0) {
        /* Setup SMI MDIO GPIO */
        ESP_RETURN_ON_ERROR(emac_esp_gpio_matrix_init(smi_gpio->mdio_num, emac_io_idx.mdi_idx, emac_io_idx.mdo_idx, GPIO_MODE_INPUT_OUTPUT),
                            TAG, "MDIO GPIO matrix config failed");
    }
    return ESP_OK;
}

esp_err_t emac_esp_gpio_matrix_init_mii(const eth_mac_mii_gpio_config_t *mii_gpio)
{
    ESP_RETURN_ON_FALSE(mii_gpio != NULL, ESP_ERR_INVALID_ARG, TAG, "MII IO matrix config cannot be NULL");

    ESP_RETURN_ON_ERROR(emac_esp_gpio_matrix_init(mii_gpio->tx_clk_num, emac_io_idx.mii_tx_clk_i_idx, 0, GPIO_MODE_INPUT),
                        TAG, "TX_CLK GPIO matrix config failed");
    ESP_RETURN_ON_ERROR(emac_esp_gpio_matrix_init(mii_gpio->tx_en_num, 0, emac_io_idx.mii_tx_en_o_idx, GPIO_MODE_OUTPUT),
                        TAG, "TX_EN GPIO matrix config failed");
    ESP_RETURN_ON_ERROR(emac_esp_gpio_matrix_init(mii_gpio->txd0_num, 0, emac_io_idx.mii_txd0_o_idx, GPIO_MODE_OUTPUT),
                        TAG, "TDX0 GPIO matrix config failed");
    ESP_RETURN_ON_ERROR(emac_esp_gpio_matrix_init(mii_gpio->txd1_num, 0, emac_io_idx.mii_txd1_o_idx, GPIO_MODE_OUTPUT),
                        TAG, "TDX1 GPIO matrix config failed");
    ESP_RETURN_ON_ERROR(emac_esp_gpio_matrix_init(mii_gpio->txd2_num, 0, emac_io_idx.mii_txd2_o_idx, GPIO_MODE_OUTPUT),
                        TAG, "TDX2 GPIO matrix config failed");
    ESP_RETURN_ON_ERROR(emac_esp_gpio_matrix_init(mii_gpio->txd3_num, 0, emac_io_idx.mii_txd3_o_idx, GPIO_MODE_OUTPUT),
                        TAG, "TDX3 GPIO matrix config failed");
    ESP_RETURN_ON_ERROR(emac_esp_gpio_matrix_init(mii_gpio->rx_clk_num, emac_io_idx.mii_rx_clk_i_idx, 0, GPIO_MODE_INPUT),
                        TAG, "RX_CLK GPIO matrix config failed");
    ESP_RETURN_ON_ERROR(emac_esp_gpio_matrix_init(mii_gpio->rxd0_num, emac_io_idx.mii_rxd0_i_idx, 0, GPIO_MODE_INPUT),
                        TAG, "RXD0 GPIO matrix config failed");
    ESP_RETURN_ON_ERROR(emac_esp_gpio_matrix_init(mii_gpio->rxd1_num, emac_io_idx.mii_rxd1_i_idx, 0, GPIO_MODE_INPUT),
                        TAG, "RXD1 GPIO matrix config failed");
    ESP_RETURN_ON_ERROR(emac_esp_gpio_matrix_init(mii_gpio->rxd2_num, emac_io_idx.mii_rxd2_i_idx, 0, GPIO_MODE_INPUT),
                        TAG, "RXD2 GPIO matrix config failed");
    ESP_RETURN_ON_ERROR(emac_esp_gpio_matrix_init(mii_gpio->rxd3_num, emac_io_idx.mii_rxd3_i_idx, 0, GPIO_MODE_INPUT),
                        TAG, "RXD3 GPIO matrix config failed");
    ESP_RETURN_ON_ERROR(emac_esp_gpio_matrix_init(mii_gpio->col_in_num, emac_io_idx.mii_col_i_idx, 0, GPIO_MODE_INPUT),
                        TAG, "COL_IN GPIO matrix config failed");
    ESP_RETURN_ON_ERROR(emac_esp_gpio_matrix_init(mii_gpio->crs_in_num, emac_io_idx.mii_crs_i_idx, 0, GPIO_MODE_INPUT),
                        TAG, "CRS_IN GPIO matrix config failed");
    ESP_RETURN_ON_ERROR(emac_esp_gpio_matrix_init(mii_gpio->tx_er_num, 0, emac_io_idx.mii_tx_er_o_idx, GPIO_MODE_OUTPUT),
                        TAG, "TX_ER GPIO matrix config failed");
    ESP_RETURN_ON_ERROR(emac_esp_gpio_matrix_init(mii_gpio->rx_er_num, emac_io_idx.mii_rx_er_i_idx, 0, GPIO_MODE_INPUT),
                        TAG, "RX_ER GPIO matrix config failed");

    return ESP_OK;
}

esp_err_t emac_esp_iomux_init_mii(const eth_mac_mii_gpio_config_t *mii_gpio)
{
    ESP_RETURN_ON_FALSE(emac_mii_iomux_pins.clk_tx != NULL, ESP_ERR_NOT_SUPPORTED, TAG, "target does not support MII IOMUX");

    ESP_RETURN_ON_ERROR(emac_esp_iomux_init(GET_GPIO_OR_SINGLE(mii_gpio, tx_clk_num), emac_mii_iomux_pins.clk_tx,
                        emac_io_idx.mii_tx_clk_i_idx, true), TAG, "invalid TX_CLK GPIO number");
    ESP_RETURN_ON_ERROR(emac_esp_iomux_init(GET_GPIO_OR_SINGLE(mii_gpio, tx_en_num), emac_mii_iomux_pins.tx_en,
                        emac_io_idx.mii_tx_en_o_idx, false), TAG, "invalid TX_EN GPIO number");
    ESP_RETURN_ON_ERROR(emac_esp_iomux_init(GET_GPIO_OR_SINGLE(mii_gpio, txd0_num), emac_mii_iomux_pins.txd0,
                        emac_io_idx.mii_txd0_o_idx, false), TAG, "invalid TXD0 GPIO number");
    ESP_RETURN_ON_ERROR(emac_esp_iomux_init(GET_GPIO_OR_SINGLE(mii_gpio, txd1_num), emac_mii_iomux_pins.txd1,
                        emac_io_idx.mii_txd1_o_idx, false), TAG, "invalid TXD1 GPIO number");
    ESP_RETURN_ON_ERROR(emac_esp_iomux_init(GET_GPIO_OR_SINGLE(mii_gpio, txd2_num), emac_mii_iomux_pins.txd2,
                        emac_io_idx.mii_txd2_o_idx, false), TAG, "invalid TXD2 GPIO number");
    ESP_RETURN_ON_ERROR(emac_esp_iomux_init(GET_GPIO_OR_SINGLE(mii_gpio, txd3_num), emac_mii_iomux_pins.txd3,
                        emac_io_idx.mii_txd3_o_idx, false), TAG, "invalid TXD3 GPIO number");
    ESP_RETURN_ON_ERROR(emac_esp_iomux_init(GET_GPIO_OR_SINGLE(mii_gpio, rx_clk_num), emac_mii_iomux_pins.clk_rx,
                        emac_io_idx.mii_rx_clk_i_idx, true), TAG, "invalid RX_CLK GPIO number");
    ESP_RETURN_ON_ERROR(emac_esp_iomux_init(GET_GPIO_OR_SINGLE(mii_gpio, rx_dv_num), emac_mii_iomux_pins.rx_dv,
                        emac_io_idx.mii_rx_dv_i_idx, true), TAG, "invalid RX_DV GPIO number");
    ESP_RETURN_ON_ERROR(emac_esp_iomux_init(GET_GPIO_OR_SINGLE(mii_gpio, rxd0_num), emac_mii_iomux_pins.rxd0,
                        emac_io_idx.mii_rxd0_i_idx, true), TAG, "invalid RXD0 GPIO number");
    ESP_RETURN_ON_ERROR(emac_esp_iomux_init(GET_GPIO_OR_SINGLE(mii_gpio, rxd1_num), emac_mii_iomux_pins.rxd1,
                        emac_io_idx.mii_rxd1_i_idx, true), TAG, "invalid RXD1 GPIO number");
    ESP_RETURN_ON_ERROR(emac_esp_iomux_init(GET_GPIO_OR_SINGLE(mii_gpio, rxd2_num), emac_mii_iomux_pins.rxd2,
                        emac_io_idx.mii_rxd2_i_idx, true), TAG, "invalid RXD2 GPIO number");
    ESP_RETURN_ON_ERROR(emac_esp_iomux_init(GET_GPIO_OR_SINGLE(mii_gpio, rxd3_num), emac_mii_iomux_pins.rxd3,
                        emac_io_idx.mii_rxd3_i_idx, true), TAG, "invalid RXD3 GPIO number");
    ESP_RETURN_ON_ERROR(emac_esp_iomux_init(GET_GPIO_OR_SINGLE(mii_gpio, col_in_num), emac_mii_iomux_pins.col_in,
                        emac_io_idx.mii_col_i_idx, true), TAG, "invalid COL_IN GPIO number");
    ESP_RETURN_ON_ERROR(emac_esp_iomux_init(GET_GPIO_OR_SINGLE(mii_gpio, crs_in_num), emac_mii_iomux_pins.crs_in,
                        emac_io_idx.mii_crs_i_idx, true), TAG, "invalid CRS_IN GPIO number");
    return ESP_OK;
}

esp_err_t emac_esp_iomux_rmii_clk_input(int num)
{
    ESP_RETURN_ON_FALSE(emac_rmii_iomux_pins.clki != NULL, ESP_ERR_NOT_SUPPORTED, TAG, "target does not support RMII CLKI IOMUX");

    ESP_RETURN_ON_ERROR(emac_esp_iomux_init(num, emac_rmii_iomux_pins.clki, emac_io_idx.rmii_refclk_i_idx, true),
                        TAG, "invalid RMII CLK input GPIO number");
    return ESP_OK;
}

esp_err_t emac_esp_iomux_rmii_clk_ouput(int num)
{
    ESP_RETURN_ON_FALSE(emac_rmii_iomux_pins.clko != NULL, ESP_ERR_NOT_SUPPORTED, TAG, "target does not support RMII CLKO IOMUX");

    ESP_RETURN_ON_ERROR(emac_esp_iomux_init(num, emac_rmii_iomux_pins.clko, emac_io_idx.rmii_refclk_o_idx, false),
                        TAG, "invalid RMII CLK output GPIO number");
    return ESP_OK;
}

esp_err_t emac_esp_iomux_init_rmii(const eth_mac_rmii_gpio_config_t *rmii_gpio)
{
    ESP_RETURN_ON_FALSE(emac_rmii_iomux_pins.clki != NULL, ESP_ERR_NOT_SUPPORTED, TAG, "target does not support RMII IOMUX");

    ESP_RETURN_ON_ERROR(emac_esp_iomux_init(GET_GPIO_OR_SINGLE(rmii_gpio, tx_en_num), emac_rmii_iomux_pins.tx_en,
                        emac_io_idx.mii_tx_en_o_idx, false), TAG, "invalid TX_EN GPIO number");
    ESP_RETURN_ON_ERROR(emac_esp_iomux_init(GET_GPIO_OR_SINGLE(rmii_gpio, txd0_num), emac_rmii_iomux_pins.txd0,
                        emac_io_idx.mii_txd0_o_idx, false), TAG, "invalid TXD0 GPIO number");
    ESP_RETURN_ON_ERROR(emac_esp_iomux_init(GET_GPIO_OR_SINGLE(rmii_gpio, txd1_num), emac_rmii_iomux_pins.txd1,
                        emac_io_idx.mii_txd1_o_idx, false), TAG, "invalid TXD1 GPIO number");
    ESP_RETURN_ON_ERROR(emac_esp_iomux_init(GET_GPIO_OR_SINGLE(rmii_gpio, crs_dv_num), emac_rmii_iomux_pins.crs_dv,
                        emac_io_idx.mii_crs_i_idx, true), TAG,"invalid CRS_DV GPIO number");
    ESP_RETURN_ON_ERROR(emac_esp_iomux_init(GET_GPIO_OR_SINGLE(rmii_gpio, rxd0_num), emac_rmii_iomux_pins.rxd0,
                        emac_io_idx.mii_rxd0_i_idx, true), TAG,"invalid RXD0 GPIO number");
    ESP_RETURN_ON_ERROR(emac_esp_iomux_init(GET_GPIO_OR_SINGLE(rmii_gpio, rxd1_num), emac_rmii_iomux_pins.rxd1,
                        emac_io_idx.mii_rxd1_i_idx, true), TAG,"invalid RXD1 GPIO number");

    return ESP_OK;
}

esp_err_t emac_esp_iomux_rmii_init_tx_er(int num)
{
    ESP_RETURN_ON_FALSE(emac_rmii_iomux_pins.tx_er != NULL, ESP_ERR_NOT_SUPPORTED, TAG, "target does not support RMII TX_ER IOMUX");

    ESP_RETURN_ON_ERROR(emac_esp_iomux_init(num, emac_rmii_iomux_pins.tx_er, emac_io_idx.mii_tx_er_o_idx, false),
                        TAG, "invalid TX_ER GPIO number");
    return ESP_OK;
}

esp_err_t emac_esp_iomux_rmii_init_rx_er(int num)
{
    ESP_RETURN_ON_FALSE(emac_rmii_iomux_pins.rx_er != NULL, ESP_ERR_NOT_SUPPORTED, TAG, "target does not support RMII RX_ER IOMUX");

    ESP_RETURN_ON_ERROR(emac_esp_iomux_init(num, emac_rmii_iomux_pins.rx_er, emac_io_idx.mii_rx_er_i_idx, true),
                        TAG, "invalid RX_ER GPIO number");
    return ESP_OK;
}

esp_err_t emac_esp_iomux_mii_init_tx_er(int num)
{
    ESP_RETURN_ON_FALSE(emac_mii_iomux_pins.tx_er != NULL, ESP_ERR_NOT_SUPPORTED, TAG, "target does not support MII TX_ER IOMUX");

    ESP_RETURN_ON_ERROR(emac_esp_iomux_init(num, emac_mii_iomux_pins.tx_er, emac_io_idx.mii_tx_er_o_idx, false),
                        TAG, "invalid TX_ER GPIO number");
    return ESP_OK;
}

esp_err_t emac_esp_iomux_mii_init_rx_er(int num)
{
    ESP_RETURN_ON_FALSE(emac_mii_iomux_pins.rx_er != NULL, ESP_ERR_NOT_SUPPORTED, TAG, "target does not support RMII RX_ER IOMUX");

    ESP_RETURN_ON_ERROR(emac_esp_iomux_init(num, emac_mii_iomux_pins.rx_er, emac_io_idx.mii_rx_er_i_idx, true),
                        TAG, "invalid RX_ER GPIO number");
    return ESP_OK;
}

esp_err_t emac_esp_gpio_matrix_init_ptp_pps(int num)
{
    ESP_RETURN_ON_ERROR(emac_esp_gpio_matrix_init(num, 0, emac_io_idx.ptp_pps_idx, GPIO_MODE_OUTPUT),
                        TAG, "PTP PPS GPIO matrix config failed");
    return ESP_OK;
}

esp_err_t emac_esp_gpio_deinit_all(void)
{
    for (int gpio_num = 0; gpio_num < 64; gpio_num++) {
        if (BIT64(gpio_num) & s_emac_esp_used_gpio_mask) {
            gpio_reset_pin(gpio_num);
            esp_gpio_revoke(BIT64(gpio_num));
        }
        s_emac_esp_used_gpio_mask &= ~BIT64(gpio_num);
    }
    return ESP_OK;
}
