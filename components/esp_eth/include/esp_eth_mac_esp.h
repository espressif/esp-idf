/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include "soc/soc_caps.h"
#include "esp_eth_com.h"
#include "esp_eth_mac.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_ETH_USE_ESP32_EMAC

/**
 * @brief RMII Clock Mode Options
 *
 */
typedef enum {
    /**
     * @brief Default values configured using Kconfig are going to be used when "Default" selected.
     *
     * @warning Deprecated option. Clock configuration using Kconfig is limitedly supported only for ESP32 SoC via @c ETH_ESP32_EMAC_DEFAULT_CONFIG
     * and is going to be reevaluated in the next major release.
     * Clock mode and clock GPIO number is supposed to be defined in `EMAC specific configuration` structure from user's code.
     *
     */
    EMAC_CLK_DEFAULT __attribute__((deprecated)), // IDF-9724

    /**
     * @brief Input RMII Clock from external. EMAC Clock GPIO number needs to be configured when this option is selected.
     *
     * @note MAC will get RMII clock from outside. Note that ESP32 only supports GPIO0 to input the RMII clock.
     *
     */
    EMAC_CLK_EXT_IN,

    /**
     * @brief Output RMII Clock from internal (A/M)PLL Clock. EMAC Clock GPIO number needs to be configured when this option is selected.
     *
     */
    EMAC_CLK_OUT
} emac_rmii_clock_mode_t;

#if CONFIG_IDF_TARGET_ESP32
/**
 * @brief RMII Clock GPIO number Options for ESP32
 *
 * @warning If you want the Ethernet to work with WiFi, don’t select ESP32 as RMII CLK output as it would result in clock instability.
 *
 */
typedef enum {
    /**
     * @brief MAC will get RMII clock from outside at this GPIO.
     *
     * @note ESP32 only supports GPIO0 to input the RMII clock.
     *
     */
    EMAC_CLK_IN_GPIO = 0,

    /**
     * @brief Output RMII Clock from internal APLL Clock available at GPIO0
     *
     * @note GPIO0 can be set to output a pre-divided PLL clock. Enabling this option will configure GPIO0 to output a 50MHz clock.
     * In fact this clock doesn’t have directly relationship with EMAC peripheral. Sometimes this clock may not work well with your PHY chip.
     *
     */
    EMAC_APPL_CLK_OUT_GPIO = 0,

    /**
     * @brief Output RMII Clock from internal APLL Clock available at GPIO16
     *
     */
    EMAC_CLK_OUT_GPIO = 16,

    /**
     * @brief Inverted Output RMII Clock from internal APLL Clock available at GPIO17
     *
     */
    EMAC_CLK_OUT_180_GPIO = 17
} emac_rmii_clock_gpio_t;
#else
/**
 * @brief RMII Clock GPIO number
 *
 */
typedef int emac_rmii_clock_gpio_t;
#endif // CONFIG_IDF_TARGET_ESP32

/**
 * @brief Ethernet MAC Clock Configuration
 *
 */
typedef union {
    struct {
        // MII interface is not fully implemented...
        // Reserved for GPIO number, clock source, etc. in MII mode
    } mii; /*!< EMAC MII Clock Configuration */
    struct {
        emac_rmii_clock_mode_t clock_mode; /*!< RMII Clock Mode Configuration */
        emac_rmii_clock_gpio_t clock_gpio; /*!< RMII Clock GPIO Configuration */
    } rmii; /*!< EMAC RMII Clock Configuration */
} eth_mac_clock_config_t;

/**
 * @brief EMAC SMI GPIO configuration
 */
typedef struct {
    int mdc_num;    /*!< SMI MDC GPIO number, set to -1 could bypass the SMI GPIO configuration */
    int mdio_num;   /*!< SMI MDIO GPIO number, set to -1 could bypass the SMI GPIO configuration */
} emac_esp_smi_gpio_config_t;

/**
 * @brief EMAC MII data interface GPIO configuration
 */
typedef struct {
    int tx_clk_num; /*!< TX_CLK GPIO number */
    int tx_en_num;  /*!< TX_EN GPIO number */
    int txd0_num;   /*!< TXD0 GPIO number */
    int txd1_num;   /*!< TXD1 GPIO number */
    int txd2_num;   /*!< TXD2 GPIO number */
    int txd3_num;   /*!< TXD3 GPIO number */
    int rx_clk_num; /*!< RX_CLK GPIO number */
    int rx_dv_num;  /*!< RX_DV GPIO number */
    int rxd0_num;   /*!< RXD0 GPIO number */
    int rxd1_num;   /*!< RXD1 GPIO number */
    int rxd2_num;   /*!< RXD2 GPIO number */
    int rxd3_num;   /*!< RXD3 GPIO number */
    int col_in_num; /*!< COL_IN GPIO number */
    int crs_in_num; /*!< CRS_IN GPIO number */
    int tx_er_num;  /*!< TX_ER GPIO number */
    int rx_er_num;  /*!< RX_ER GPIO number */
} eth_mac_mii_gpio_config_t;

/**
 * @brief EMAC RMII data interface GPIO configuration
 */
typedef struct {
    int tx_en_num;  /*!< TX_EN GPIO number */
    int txd0_num;   /*!< TXD0 GPIO number */
    int txd1_num;   /*!< TXD1 GPIO number */
    int crs_dv_num; /*!< CRS_DV GPIO number */
    int rxd0_num;   /*!< RXD0 GPIO number */
    int rxd1_num;   /*!< RXD1 GPIO number */
} eth_mac_rmii_gpio_config_t;

#if SOC_EMAC_USE_MULTI_IO_MUX || SOC_EMAC_MII_USE_GPIO_MATRIX
/**
 * @brief Ethernet MAC MII/RMII data plane GPIO configuration
 *
 */
typedef union {
    eth_mac_mii_gpio_config_t mii; /*!< EMAC MII Data GPIO Configuration */
    eth_mac_rmii_gpio_config_t rmii; /*!< EMAC RMII Data GPIO Configuration */
} eth_mac_dataif_gpio_config_t;
#endif // SOC_EMAC_USE_MULTI_IO_MUX

/**
* @brief EMAC specific configuration
*
*/
typedef struct {
    union {
        emac_esp_smi_gpio_config_t smi_gpio;            /*!< SMI GPIO numbers */
        struct {
            int smi_mdc_gpio_num __attribute__((deprecated("Please use smi_gpio instead")));  /*!< SMI MDC GPIO number, set to -1 could bypass the SMI GPIO configuration */
            int smi_mdio_gpio_num __attribute__((deprecated("Please use smi_gpio instead"))); /*!< SMI MDIO GPIO number, set to -1 could bypass the SMI GPIO configuration */
        };
    };
    eth_data_interface_t interface;                 /*!< EMAC Data interface to PHY (MII/RMII) */
    eth_mac_clock_config_t clock_config;            /*!< EMAC Interface clock configuration */
    eth_mac_dma_burst_len_t dma_burst_len;          /*!< EMAC DMA burst length for both Tx and Rx */
    int intr_priority;                              /*!< EMAC interrupt priority, if set to 0 or a negative value, the driver will try to allocate an interrupt with a default priority */
#if SOC_EMAC_USE_MULTI_IO_MUX || SOC_EMAC_MII_USE_GPIO_MATRIX
    eth_mac_dataif_gpio_config_t emac_dataif_gpio;  /*!< EMAC MII/RMII data plane GPIO configuration */
#endif // SOC_EMAC_USE_MULTI_IO_MUX
#if !SOC_EMAC_RMII_CLK_OUT_INTERNAL_LOOPBACK
    eth_mac_clock_config_t clock_config_out_in;     /*!< EMAC input clock configuration for internally generated output clock (when output clock is looped back externally) */
#endif //SOC_EMAC_RMII_CLK_OUT_INTERNAL_LOOPBACK
} eth_esp32_emac_config_t;

/**
 * @brief List of ESP EMAC specific commands for ioctl API
 *
 */
typedef enum {
    ETH_MAC_ESP_CMD_SET_TDES0_CFG_BITS = ETH_CMD_CUSTOM_MAC_CMDS_OFFSET,    /*!< Set Transmit Descriptor Word 0 control bit mask (debug option)*/
    ETH_MAC_ESP_CMD_CLEAR_TDES0_CFG_BITS,                                   /*!< Clear Transmit Descriptor Word 0 control bit mask (debug option)*/
    ETH_MAC_ESP_CMD_PTP_ENABLE,                                             /*!< Enable IEEE1588 Time stamping */
} eth_mac_esp_io_cmd_t;

/**
 * @brief Default ESP32's EMAC specific configuration
 *
 */
#if CONFIG_IDF_TARGET_ESP32
#if CONFIG_ETH_RMII_CLK_INPUT // IDF-9724
    #define DEFAULT_RMII_CLK_MODE EMAC_CLK_EXT_IN
#if CONFIG_ETH_RMII_CLK_IN_GPIO == 0
    #define DEFAULT_RMII_CLK_GPIO CONFIG_ETH_RMII_CLK_IN_GPIO
#else
    #error "ESP32 EMAC only support input RMII clock to GPIO0"
#endif // CONFIG_ETH_RMII_CLK_IN_GPIO == 0
#elif CONFIG_ETH_RMII_CLK_OUTPUT
    #define DEFAULT_RMII_CLK_MODE EMAC_CLK_OUT
#if CONFIG_ETH_RMII_CLK_OUTPUT_GPIO0
    #define DEFAULT_RMII_CLK_GPIO EMAC_APPL_CLK_OUT_GPIO
#else
    #define DEFAULT_RMII_CLK_GPIO CONFIG_ETH_RMII_CLK_OUT_GPIO
#endif // CONFIG_ETH_RMII_CLK_OUTPUT_GPIO0
#else
#error "Unsupported RMII clock mode"
#endif // CONFIG_ETH_RMII_CLK_INPUT

#define ETH_ESP32_EMAC_DEFAULT_CONFIG()                                       \
    {                                                                         \
        .smi_gpio =                                                           \
        {                                                                     \
            .mdc_num = 23,                                                    \
            .mdio_num = 18                                                    \
        },                                                                    \
        .interface = EMAC_DATA_INTERFACE_RMII,                                \
        .clock_config =                                                       \
        {                                                                     \
            .rmii =                                                           \
            {                                                                 \
                .clock_mode = DEFAULT_RMII_CLK_MODE,                          \
                .clock_gpio = (emac_rmii_clock_gpio_t) DEFAULT_RMII_CLK_GPIO  \
            }                                                                 \
        },                                                                    \
        .dma_burst_len = ETH_DMA_BURST_LEN_32,                                \
        .intr_priority = 0,                                                   \
    }
#elif CONFIG_IDF_TARGET_ESP32P4
#define ETH_ESP32_EMAC_DEFAULT_CONFIG()                                       \
    {                                                                         \
        .smi_gpio =                                                           \
        {                                                                     \
            .mdc_num = 31,                                                    \
            .mdio_num = 27                                                    \
        },                                                                    \
        .interface = EMAC_DATA_INTERFACE_RMII,                                \
        .clock_config =                                                       \
        {                                                                     \
            .rmii =                                                           \
            {                                                                 \
                .clock_mode = EMAC_CLK_EXT_IN,                                \
                .clock_gpio = (emac_rmii_clock_gpio_t) 50                     \
            }                                                                 \
        },                                                                    \
        .clock_config_out_in =                                                \
        {                                                                     \
            .rmii =                                                           \
            {                                                                 \
                .clock_mode = EMAC_CLK_EXT_IN,                                \
                .clock_gpio = (emac_rmii_clock_gpio_t) -1                     \
            }                                                                 \
        },                                                                    \
        .dma_burst_len = ETH_DMA_BURST_LEN_32,                                \
        .intr_priority = 0,                                                   \
        .emac_dataif_gpio =                                                   \
        {                                                                     \
            .rmii =                                                           \
            {                                                                 \
                .tx_en_num = 49,                                              \
                .txd0_num = 34,                                               \
                .txd1_num = 35,                                               \
                .crs_dv_num = 28,                                             \
                .rxd0_num = 29,                                               \
                .rxd1_num = 30                                                \
            }                                                                 \
        },                                                                    \
    }
#endif // CONFIG_IDF_TARGET_ESP32P4

/**
* @brief Create ESP32 Ethernet MAC instance
*
* @param esp32_config: EMAC specific configuration
* @param config:       Ethernet MAC configuration
*
* @return
*      - instance: create MAC instance successfully
*      - NULL: create MAC instance failed because some error occurred
*/
esp_eth_mac_t *esp_eth_mac_new_esp32(const eth_esp32_emac_config_t *esp32_config, const eth_mac_config_t *config);
#endif // CONFIG_ETH_USE_ESP32_EMAC

#ifdef __cplusplus
}
#endif
