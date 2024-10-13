/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include "esp_eth_mac_esp.h"

#if CONFIG_ETH_USE_ESP32_EMAC

esp_err_t emac_esp_gpio_matrix_init_mii(const eth_mac_mii_gpio_config_t *mii_gpio);
esp_err_t emac_esp_iomux_init_mii(const eth_mac_mii_gpio_config_t *mii_gpio);
esp_err_t emac_esp_iomux_init_rmii(const eth_mac_rmii_gpio_config_t *rmii_gpio);
esp_err_t emac_esp_iomux_rmii_clk_input(int num);
esp_err_t emac_esp_iomux_rmii_clk_ouput(int num);
esp_err_t emac_esp_iomux_rmii_init_tx_er(int num);
esp_err_t emac_esp_iomux_rmii_init_rx_er(int num);
esp_err_t emac_esp_iomux_mii_init_tx_er(int num);
esp_err_t emac_esp_iomux_mii_init_rx_er(int num);
esp_err_t emac_esp_gpio_init_smi(const emac_esp_smi_gpio_config_t *smi_gpio);
esp_err_t emac_esp_gpio_deinit_all(void);

#endif // CONFIG_ETH_USE_ESP32_EMAC

#ifdef __cplusplus
}
#endif
