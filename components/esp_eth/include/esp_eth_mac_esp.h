/*
 * SPDX-FileCopyrightText: 2019-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"
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
     * @brief Input RMII Clock from external. EMAC Clock GPIO number needs to be configured when this option is selected.
     *
     * @note See components/esp_hal_emac/esp32(*)/emac_periph.c for available GPIO numbers.
     *
     */
    EMAC_CLK_EXT_IN,

    /**
     * @brief Output RMII Clock from internal (A/M)PLL Clock. EMAC Clock GPIO number needs to be configured when this option is selected.
     *
     * @warning ESP32 Errata: If you want the Ethernet to work with Wi-Fi or BT, don’t select ESP32 as RMII CLK output as it would result in clock instability.
     *                        Applicable only to ESP32, other ESP32 SoCs are not affected.
     *
     * @note See components/esp_hal_emac/esp32(*)/emac_periph.c for available GPIO numbers.
     *
     */
    EMAC_CLK_OUT
} emac_rmii_clock_mode_t;

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
        emac_rmii_clock_mode_t clock_mode;  /*!< RMII Clock Mode Configuration */
        int                    clock_gpio;  /*!< RMII Clock GPIO Configuration */
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
    emac_esp_smi_gpio_config_t smi_gpio;            /*!< SMI GPIO numbers */
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
    int32_t mdc_freq_hz;                            /*!< EMAC MDC frequency range limit, if set to 0 or a negative value, the driver will set the CSR clock range up to 2.5 MHz */
} eth_esp32_emac_config_t;

/**
 * @brief List of ESP EMAC specific commands for ioctl API
 *
 */
typedef enum {
    ETH_MAC_ESP_CMD_SET_TDES0_CFG_BITS = ETH_CMD_CUSTOM_MAC_CMDS_OFFSET,    /*!< Set Transmit Descriptor Word 0 control bit mask (debug option)*/
    ETH_MAC_ESP_CMD_CLEAR_TDES0_CFG_BITS,                                   /*!< Clear Transmit Descriptor Word 0 control bit mask (debug option)*/
    ETH_MAC_ESP_CMD_DUMP_REGS,                                              /*!< Dump EMAC registers (debug option) */
} eth_mac_esp_io_cmd_t;

#ifdef SOC_EMAC_IEEE1588V2_SUPPORTED
/**
 * @brief Configuration of PTP module
 *
 * @warning Time stamping is currently Experimental Feature! Be aware that API may change.
 *
 */
typedef struct {
    soc_periph_emac_ptp_clk_src_t clk_src;  /*!< Clock source for PTP */
    float clk_src_period_ns;                /*!< Period of the clock source for PTP in nanoseconds*/
    float required_accuracy_ns;             /*!< Required accuracy for PTP in nanoseconds (must be worse than clock source for PTP)*/
    eth_mac_ptp_roll_type_t roll_type;      /*!< Rollover mode (digital or binary) for subseconds register */
} eth_mac_ptp_config_t;

/**
 * @brief Type of callback function invoked under Time Stamp target time exceeded interrupt
 *
 * @warning Time stamping is currently Experimental Feature! Be aware that API may change.
 *
 * @param eth: mediator of Ethernet driver
 * @param user_args user specific arguments (placeholder - IDF-11429)
 *
 * @return
 *          - TRUE when high priority task has been woken by this function
 *          - FALSE no high priority task was woken by this function
 */
typedef bool (*ts_target_exceed_cb_from_isr_t)(esp_eth_mediator_t *eth, void *user_args);

/**
 * @brief Default configuration for PTP module
 *
 */
#define ETH_MAC_ESP_PTP_DEFAULT_CONFIG()                                      \
    {                                                                         \
        .clk_src = EMAC_PTP_CLK_SRC_XTAL,                                     \
        .clk_src_period_ns = 25,                                              \
        .required_accuracy_ns = 40,                                           \
        .roll_type = ETH_PTP_BINARY_ROLLOVER,                                 \
    }
#endif // SOC_EMAC_IEEE1588V2_SUPPORTED

/**
 * @brief Default ESP32's EMAC specific configuration
 *
 */
#if CONFIG_IDF_TARGET_ESP32
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
                .clock_mode = EMAC_CLK_EXT_IN,                                \
                .clock_gpio = 0                                               \
            }                                                                 \
        },                                                                    \
        .dma_burst_len = ETH_DMA_BURST_LEN_32,                                \
        .intr_priority = 0,                                                   \
        .mdc_freq_hz = 0,                                                     \
    }
#elif CONFIG_IDF_TARGET_ESP32P4
#define ETH_ESP32_EMAC_DEFAULT_CONFIG()                                       \
    {                                                                         \
        .smi_gpio =                                                           \
        {                                                                     \
            .mdc_num = 31,                                                    \
            .mdio_num = 52                                                    \
        },                                                                    \
        .interface = EMAC_DATA_INTERFACE_RMII,                                \
        .clock_config =                                                       \
        {                                                                     \
            .rmii =                                                           \
            {                                                                 \
                .clock_mode = EMAC_CLK_EXT_IN,                                \
                .clock_gpio = 50                                              \
            }                                                                 \
        },                                                                    \
        .dma_burst_len = ETH_DMA_BURST_LEN_32,                                \
        .intr_priority = 0,                                                   \
        .mdc_freq_hz = 0,                                                     \
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
        .clock_config_out_in =                                                \
        {                                                                     \
            .rmii =                                                           \
            {                                                                 \
                .clock_mode = EMAC_CLK_EXT_IN,                                \
                .clock_gpio = -1                                              \
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

#ifdef SOC_EMAC_IEEE1588V2_SUPPORTED
/**
 * @brief Enable/Disable PTP module
 *
 * @param mac: Ethernet MAC instance
 * @param config: PTP configuration
 *
 * @return
 *      - ESP_OK: success
 *      - ESP_ERR_INVALID_ARG: invalid argument
 *      - ESP_FAIL: failure
 */
esp_err_t esp_eth_mac_ptp_enable(esp_eth_mac_t *mac, const eth_mac_ptp_config_t *config);

/**
 * @brief Disable PTP module
 *
 * @param mac: Ethernet MAC instance
 *
 * @return
 *      - ESP_OK: success
 *      - ESP_ERR_INVALID_ARG: invalid argument
 *      - ESP_FAIL: failure
 */
esp_err_t esp_eth_mac_ptp_disable(esp_eth_mac_t *mac);

/**
 * @brief Set PTP time
 *
 * @param mac: Ethernet MAC instance
 * @param time: PTP time
 *
 * @return
 *      - ESP_OK: success
 *      - ESP_ERR_INVALID_ARG: invalid argument
 *      - ESP_FAIL: failure
 */
esp_err_t esp_eth_mac_set_ptp_time(esp_eth_mac_t *mac, const eth_mac_time_t *time);

/**
 * @brief Get PTP time
 *
 * @param mac: Ethernet MAC instance
 * @param time: PTP time
 *
 * @return
 *      - ESP_OK: success
 *      - ESP_ERR_INVALID_ARG: invalid argument
 *      - ESP_FAIL: failure
 */
esp_err_t esp_eth_mac_get_ptp_time(esp_eth_mac_t *mac, eth_mac_time_t *time);

/**
 * @brief Adjust PTP time frequency increment by scale factor
 *
 * @param mac: Ethernet MAC instance
 * @param scale_factor: frequency scale factor
 *
 * @return
 *      - ESP_OK: success
 *      - ESP_ERR_INVALID_ARG: invalid argument
 *      - ESP_FAIL: failure
 */
esp_err_t esp_eth_mac_adj_ptp_freq(esp_eth_mac_t *mac, double scale_factor);

/**
 * @brief Adjust base PTP time frequency increment by PPS
 *
 * @param mac: Ethernet MAC instance
 * @param adj_ppb: adjustment in ppb
 *
 * @return
 *      - ESP_OK: success
 *      - ESP_ERR_INVALID_ARG: invalid argument
 *      - ESP_FAIL: failure
 */
esp_err_t esp_eth_mac_adj_ptp_time(esp_eth_mac_t *mac, int32_t adj_ppb);

/**
 * @brief Set Target Time at which interrupt is invoked when PTP time exceeds this value
 *
 * @param mac: Ethernet MAC instance
 * @param target: target time
 *
 * @return
 *      - ESP_OK: success
 *      - ESP_ERR_INVALID_ARG: invalid argument
 *      - ESP_FAIL: failure
 */
esp_err_t esp_eth_mac_set_target_time(esp_eth_mac_t *mac, const eth_mac_time_t *target);

/**
 * @brief Set pointer to a callback function invoked when PTP time exceeds Target Time
 *
 * @param mac: Ethernet MAC instance
 * @param cb: callback function
 *
 * @return
 *      - ESP_OK: success
 *      - ESP_ERR_INVALID_ARG: invalid argument
 *      - ESP_FAIL: failure
 */
esp_err_t esp_eth_mac_set_target_time_cb(esp_eth_mac_t *mac, ts_target_exceed_cb_from_isr_t cb);

/**
 * @brief Enable timestamp for all received frames
 *
 * @param mac: Ethernet MAC instance
 * @param enable: enable or disable
 *
 * @return
 *      - ESP_OK: success
 *      - ESP_ERR_INVALID_ARG: invalid argument
 *      - ESP_FAIL: failure
 */
esp_err_t esp_eth_mac_enable_ts4all(esp_eth_mac_t *mac, bool enable);

/**
 * @brief Set PPS0 output at GPIO
 *
 * @param mac: Ethernet MAC instance
 * @param gpio_num: GPIO number
 *
 * @return
 *      - ESP_OK: success
 *      - ESP_ERR_INVALID_ARG: invalid argument
 *      - ESP_FAIL: failure
 */
esp_err_t esp_eth_mac_set_pps_out_gpio(esp_eth_mac_t *mac, int gpio_num);

/**
 * @brief Set PPS0 output frequency
 *
 * @param mac: Ethernet MAC instance
 * @param freq_hz: Supported frequencies: 0 = 1PPS (narrow pulse), other values generate square clock signal.
 *                 The clock frequency must be power of two and less than or equal to 16384 Hz.
 *
 * @return
 *      - ESP_OK: success
 *      - ESP_ERR_INVALID_ARG: invalid argument
 *      - ESP_FAIL: failure
 */
esp_err_t esp_eth_mac_set_pps_out_freq(esp_eth_mac_t *mac, uint32_t freq_hz);
#endif // SOC_EMAC_IEEE1588V2_SUPPORTED

#endif // CONFIG_ETH_USE_ESP32_EMAC

#ifdef __cplusplus
}
#endif
