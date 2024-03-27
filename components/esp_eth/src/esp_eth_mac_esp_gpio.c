/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_check.h"
#include "sdkconfig.h"
#include "esp_rom_gpio.h"
#include "driver/gpio.h"
#include "soc/soc_caps.h"
#include "soc/gpio_sig_map.h"
#include "soc/io_mux_reg.h"
#include "soc/gpio_periph.h"
#include "esp_private/gpio.h"
#include "esp_private/eth_mac_esp_gpio.h"
#include "esp_log.h"

static const char *TAG = "esp.emac.gpio";

void emac_esp32_gpio_init_smi(emac_esp_smi_gpio_config_t *smi_gpio)
{
    if (smi_gpio->mdc_num >= 0) {
        /* Setup SMI MDC GPIO */
        gpio_set_direction(smi_gpio->mdc_num, GPIO_MODE_OUTPUT);
#if CONFIG_IDF_TARGET_ESP32
        esp_rom_gpio_connect_out_signal(smi_gpio->mdc_num, EMAC_MDC_O_IDX, false, false);
#elif CONFIG_IDF_TARGET_ESP32P4
        esp_rom_gpio_connect_out_signal(smi_gpio->mdc_num, GMII_MDC_PAD_OUT_IDX, false, false);
#endif
        gpio_func_sel(smi_gpio->mdc_num, PIN_FUNC_GPIO);
    }
    if (smi_gpio->mdio_num >= 0) {
        /* Setup SMI MDIO GPIO */
        gpio_set_direction(smi_gpio->mdio_num, GPIO_MODE_INPUT_OUTPUT);
#if CONFIG_IDF_TARGET_ESP32
        esp_rom_gpio_connect_out_signal(smi_gpio->mdio_num, EMAC_MDO_O_IDX, false, false);
        esp_rom_gpio_connect_in_signal(smi_gpio->mdio_num, EMAC_MDI_I_IDX, false);
#elif CONFIG_IDF_TARGET_ESP32P4
        esp_rom_gpio_connect_out_signal(smi_gpio->mdio_num, GMII_MDO_PAD_OUT_IDX, false, false);
        esp_rom_gpio_connect_in_signal(smi_gpio->mdio_num, GMII_MDI_PAD_IN_IDX, false);
#endif
        gpio_func_sel(smi_gpio->mdio_num, PIN_FUNC_GPIO);
    }
}

esp_err_t emac_esp_iomux_init_mii(emac_esp_mii_gpio_config_t *mii_gpio)
{
    (void)mii_gpio;
#if CONFIG_IDF_TARGET_ESP32
    /* TX_CLK to GPIO0 */
    gpio_func_sel(0, FUNC_GPIO0_EMAC_TX_CLK);
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[0]);
    /* TX_EN to GPIO21 */
    gpio_func_sel(21, FUNC_GPIO21_EMAC_TX_EN);
    PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[21]);
    /* TXD0 to GPIO19 */
    gpio_func_sel(19, FUNC_GPIO19_EMAC_TXD0);
    PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[19]);
    /* TXD1 to GPIO22 */
    gpio_func_sel(22, FUNC_GPIO22_EMAC_TXD1);
    PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[22]);
    /* TXD2 to MTMS */
    gpio_func_sel(14, FUNC_MTMS_EMAC_TXD2);
    PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[14]);
    /* TXD3 to MTDI */
    gpio_func_sel(12, FUNC_MTDI_EMAC_TXD3);
    PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[12]);

    /* RX_CLK to GPIO5 */
    gpio_func_sel(5, FUNC_GPIO5_EMAC_RX_CLK);
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[5]);
    /* RX_DV to GPIO27 */
    gpio_func_sel(27, FUNC_GPIO27_EMAC_RX_DV);
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[27]);
    /* RXD0 to GPIO25 */
    gpio_func_sel(25, FUNC_GPIO25_EMAC_RXD0);
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[25]);
    /* RXD1 to GPIO26 */
    gpio_func_sel(26, FUNC_GPIO26_EMAC_RXD1);
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[26]);
    /* RXD2 to U0TXD */
    gpio_func_sel(1, FUNC_U0TXD_EMAC_RXD2);
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[1]);
    /* RXD3 to MTDO */
    gpio_func_sel(15, FUNC_MTDO_EMAC_RXD3);
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[15]);
    return ESP_OK;
#elif CONFIG_IDF_TARGET_ESP32P4
    ESP_LOGW(TAG, "MII is currently not supported");
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

esp_err_t emac_esp_iomux_rmii_clk_input(int num)
{
#if CONFIG_IDF_TARGET_ESP32
    if (num != 0) {
        return ESP_ERR_INVALID_ARG;
    }
    /* REF_CLK(RMII mode) to GPIO0 */
    gpio_func_sel(0, FUNC_GPIO0_EMAC_TX_CLK);
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[0]);
#elif CONFIG_IDF_TARGET_ESP32P4
    /* REF_CLK(RMII mode) to `num` */
    switch(num) {
    case 32:
        gpio_func_sel(num, FUNC_GPIO32_GMAC_RMII_CLK_PAD);
        PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[32]);
        break;
    case 44:
        gpio_func_sel(num, FUNC_GPIO44_GMAC_RMII_CLK_PAD);
        PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[44]);
        break;
    case 50:
        gpio_func_sel(num, FUNC_GPIO50_GMAC_RMII_CLK_PAD);
        PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[50]);
        break;
    default:
        ESP_LOGE(TAG, "invalid RMII CLK input GPIO number. Expected [32, 44, 50], actual %i", num);
        return ESP_ERR_INVALID_ARG;
    }
#endif
    return ESP_OK;
}

esp_err_t emac_esp_iomux_rmii_clk_ouput(int num)
{
#if CONFIG_IDF_TARGET_ESP32
    switch (num) {
    case 0:
        /* APLL clock output to GPIO0 (must be configured to 50MHz!) */
        gpio_func_sel(num, FUNC_GPIO0_CLK_OUT1);
        PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[0]);
        break;
    case 16:
        /* RMII CLK (50MHz) output to GPIO16 */
        gpio_func_sel(num, FUNC_GPIO16_EMAC_CLK_OUT);
        PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[16]);
        break;
    case 17:
        /* RMII CLK (50MHz) output to GPIO17 */
        gpio_func_sel(num, FUNC_GPIO17_EMAC_CLK_OUT_180);
        PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[17]);
        break;
    default:
        ESP_LOGE(TAG, "invalid RMII CLK output GPIO number. Expected [0, 16, 17], actual %i", num);
        return ESP_ERR_INVALID_ARG;
    }
#elif CONFIG_IDF_TARGET_ESP32P4
    /*RMII CLK output to num */
    switch (num) {
    case 23:
        gpio_func_sel(num, FUNC_GPIO23_REF_50M_CLK_PAD);
        PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[23]);
        break;
    case 39:
        gpio_func_sel(num, FUNC_GPIO39_REF_50M_CLK_PAD);
        PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[39]);
        break;
    default:
        ESP_LOGE(TAG, "invalid RMII CLK input GPIO number. Expected [23, 39], actual %i", num);
        return ESP_ERR_INVALID_ARG;
    }
#endif
    return ESP_OK;
}

esp_err_t emac_esp_iomux_init_rmii(emac_esp_rmii_gpio_config_t *rmii_gpio)
{
#if CONFIG_IDF_TARGET_ESP32
    (void)rmii_gpio;
    /* TX_EN to GPIO21 */
    gpio_func_sel(21, FUNC_GPIO21_EMAC_TX_EN);
    PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[21]);
    /* TXD0 to GPIO19 */
    gpio_func_sel(19, FUNC_GPIO19_EMAC_TXD0);
    PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[19]);
    /* TXD1 to GPIO22 */
    gpio_func_sel(22, FUNC_GPIO22_EMAC_TXD1);
    PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[22]);

    /* CRS_DV to GPIO27 */
    gpio_func_sel(27, FUNC_GPIO27_EMAC_RX_DV);
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[27]);
    /* RXD0 to GPIO25 */
    gpio_func_sel(25, FUNC_GPIO25_EMAC_RXD0);
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[25]);
    /* RXD1 to GPIO26 */
    gpio_func_sel(26, FUNC_GPIO26_EMAC_RXD1);
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[26]);
    return ESP_OK;
#elif CONFIG_IDF_TARGET_ESP32P4
    if (rmii_gpio == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    /* TX_EN */
    switch(rmii_gpio->tx_en_num) {
    case 33:
        gpio_func_sel(rmii_gpio->tx_en_num, FUNC_GPIO33_GMAC_PHY_TXEN_PAD);
        PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[33]);
        break;
    case 40:
        gpio_func_sel(rmii_gpio->tx_en_num, FUNC_GPIO40_GMAC_PHY_TXEN_PAD);
        PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[40]);
        break;
    case 49:
        gpio_func_sel(rmii_gpio->tx_en_num, FUNC_GPIO49_GMAC_PHY_TXEN_PAD);
        PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[49]);
        break;
    default:
        ESP_LOGE(TAG, "invalid TX_EN GPIO number. Expected [33, 40, 49], actual %" PRIu8, rmii_gpio->tx_en_num);
        return ESP_ERR_INVALID_ARG;
    }
    /* TXD0 */
    switch(rmii_gpio->txd0_num) {
    case 34:
        gpio_func_sel(rmii_gpio->txd0_num, FUNC_GPIO34_GMAC_PHY_TXD0_PAD);
        PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[34]);
        break;
    case 41:
        gpio_func_sel(rmii_gpio->txd0_num, FUNC_GPIO41_GMAC_PHY_TXD0_PAD);
        PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[41]);
        break;
    default:
        ESP_LOGE(TAG, "invalid TXD0 GPIO number. Expected [34, 41], actual %" PRIu8, rmii_gpio->txd0_num);
        return ESP_ERR_INVALID_ARG;
    }
    /* TXD1 */
    switch(rmii_gpio->txd1_num) {
    case 35:
        gpio_func_sel(rmii_gpio->txd1_num, FUNC_GPIO35_GMAC_PHY_TXD1_PAD );
        PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[35]);
        break;
    case 42:
        gpio_func_sel(rmii_gpio->txd1_num, FUNC_GPIO42_GMAC_PHY_TXD1_PAD );
        PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[42]);
        break;
    default:
        ESP_LOGE(TAG, "invalid TXD1 GPIO number. Expected [35, 42], actual %" PRIu8, rmii_gpio->txd1_num);
        return ESP_ERR_INVALID_ARG;
    }

    /* CRS_DV */
    switch(rmii_gpio->crs_dv_num) {
    case 28:
        gpio_func_sel(rmii_gpio->crs_dv_num, FUNC_GPIO28_GMAC_PHY_RXDV_PAD);
        PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[28]);
        break;
    case 45:
        gpio_func_sel(rmii_gpio->crs_dv_num, FUNC_GPIO45_GMAC_PHY_RXDV_PAD);
        PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[45]);
        break;
    case 51:
        gpio_func_sel(rmii_gpio->crs_dv_num, FUNC_GPIO51_GMAC_PHY_RXDV_PAD);
        PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[51]);
        break;
    default:
        ESP_LOGE(TAG, "invalid CRS_DV GPIO number. Expected [28, 45, 51], actual %" PRIu8, rmii_gpio->crs_dv_num);
        return ESP_ERR_INVALID_ARG;
    }
    /* RXD0 */
    switch(rmii_gpio->rxd0_num) {
    case 29:
        gpio_func_sel(rmii_gpio->rxd0_num, FUNC_GPIO29_GMAC_PHY_RXD0_PAD);
        PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[29]);
        break;
    case 46:
        gpio_func_sel(rmii_gpio->rxd0_num, FUNC_GPIO46_GMAC_PHY_RXD0_PAD);
        PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[46]);
        break;
    case 52:
        gpio_func_sel(rmii_gpio->rxd0_num, FUNC_GPIO52_GMAC_PHY_RXD0_PAD);
        PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[52]);
        break;
    default:
        ESP_LOGE(TAG, "invalid RXD0 GPIO number. Expected [29, 46, 52], actual %" PRIu8, rmii_gpio->rxd0_num);
        return ESP_ERR_INVALID_ARG;
    }
    /* RXD1 */
    switch(rmii_gpio->rxd1_num) {
    case 30:
        gpio_func_sel(rmii_gpio->rxd1_num, FUNC_GPIO30_GMAC_PHY_RXD1_PAD);
        PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[30]);
        break;
    case 47:
        gpio_func_sel(rmii_gpio->rxd1_num, FUNC_GPIO47_GMAC_PHY_RXD1_PAD);
        PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[47]);
        break;
    case 53:
        gpio_func_sel(rmii_gpio->rxd1_num, FUNC_GPIO53_GMAC_PHY_RXD1_PAD);
        PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[53]);
        break;
    default:
        ESP_LOGE(TAG, "invalid RXD1 GPIO number. Expected [30, 47, 53], actual %" PRIu8, rmii_gpio->rxd1_num);
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
#endif
}

esp_err_t emac_esp_iomux_init_tx_er(int num)
{
#if CONFIG_IDF_TARGET_ESP32
    (void)num;
    /* TX_ER to GPIO4 */
    gpio_func_sel(4, FUNC_GPIO4_EMAC_TX_ER);
    PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[4]);
#elif CONFIG_IDF_TARGET_ESP32P4
    /* TX_ER */
    switch (num)
    {
    case 36:
        gpio_func_sel(num, FUNC_GPIO36_GMAC_PHY_TXER_PAD);
        PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[36]);
        break;
    case 43:
        gpio_func_sel(num, FUNC_GPIO43_GMAC_PHY_TXER_PAD);
        PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[43]);
        break;
    default:
        ESP_LOGE(TAG, "invalid TX_ER GPIO number. Expected [36, 43], actual %i", num);
        return ESP_ERR_INVALID_ARG;
    }
#endif
    return ESP_OK;
}

esp_err_t emac_esp_iomux_init_rx_er(int num)
{
#if CONFIG_IDF_TARGET_ESP32
    (void)num;
    /* RX_ER to MTCK */
    gpio_func_sel(13, FUNC_MTCK_EMAC_RX_ER);
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[13]);
#elif CONFIG_IDF_TARGET_ESP32P4
    /* RX_ER */
    switch (num)
    {
    case 31:
        gpio_func_sel(num, FUNC_GPIO31_GMAC_PHY_RXER_PAD);
        PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[31]);
        break;
    case 48:
        gpio_func_sel(num, FUNC_GPIO48_GMAC_PHY_RXER_PAD);
        PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[48]);
        break;
    case 54:
        gpio_func_sel(num, FUNC_GPIO54_GMAC_PHY_RXER_PAD);
        PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[54]);
        break;
    default:
        ESP_LOGE(TAG, "invalid RX_ER GPIO number. Expected [31, 48, 54], actual %i", num);
        return ESP_ERR_INVALID_ARG;
    }
#endif
    return ESP_OK;
}
