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
#include "esp_eth_mac.h"

/**
 * @brief EMAC SMI GPIO configuration
 */
typedef struct {
    int mdc_num;
    int mdio_num;
} emac_esp_smi_gpio_config_t;

/**
 * @brief EMAC MII data interface GPIO configuration
 */
typedef struct {
    uint8_t tx_clk_num;
    uint8_t tx_en_num;
    uint8_t txd0_num;
    uint8_t txd1_num;
    uint8_t txd2_num;
    uint8_t txd3_num;
    uint8_t rx_clk_num;
    uint8_t rx_dv_num;
    uint8_t rxd0_num;
    uint8_t rxd1_num;
    uint8_t rxd2_num;
    uint8_t rxd3_num;
} emac_esp_mii_gpio_config_t;

/**
 * @brief EMAC RMII data interface GPIO configuration
 */
typedef struct {
    uint8_t tx_en_num;
    uint8_t txd0_num;
    uint8_t txd1_num;
    uint8_t crs_dv_num;
    uint8_t rxd0_num;
    uint8_t rxd1_num;
} emac_esp_rmii_gpio_config_t;

esp_err_t emac_esp_iomux_init_mii(emac_esp_mii_gpio_config_t *mii_gpio);
esp_err_t emac_esp_iomux_init_rmii(emac_esp_rmii_gpio_config_t *rmii_gpio);
esp_err_t emac_esp_iomux_rmii_clk_input(int num);
esp_err_t emac_esp_iomux_rmii_clk_ouput(int num);
esp_err_t emac_esp_iomux_init_tx_er(int num);
esp_err_t emac_esp_iomux_init_rx_er(int num);
void emac_esp32_gpio_init_smi(emac_esp_smi_gpio_config_t *smi_gpio);

#ifdef __cplusplus
}
#endif
