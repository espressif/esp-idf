/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include "esp_eth_com.h"
#include "sdkconfig.h"
#if CONFIG_ETH_USE_SPI_ETHERNET
#include "driver/spi_master.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief Ethernet MAC
*
*/
typedef struct esp_eth_mac_s esp_eth_mac_t;

/**
* @brief Ethernet MAC
*
*/
struct esp_eth_mac_s {
    /**
    * @brief Set mediator for Ethernet MAC
    *
    * @param[in] mac: Ethernet MAC instance
    * @param[in] eth: Ethernet mediator
    *
    * @return
    *      - ESP_OK: set mediator for Ethernet MAC successfully
    *      - ESP_ERR_INVALID_ARG: set mediator for Ethernet MAC failed because of invalid argument
    *
    */
    esp_err_t (*set_mediator)(esp_eth_mac_t *mac, esp_eth_mediator_t *eth);

    /**
    * @brief Initialize Ethernet MAC
    *
    * @param[in] mac: Ethernet MAC instance
    *
    * @return
    *      - ESP_OK: initialize Ethernet MAC successfully
    *      - ESP_ERR_TIMEOUT: initialize Ethernet MAC failed because of timeout
    *      - ESP_FAIL: initialize Ethernet MAC failed because some other error occurred
    *
    */
    esp_err_t (*init)(esp_eth_mac_t *mac);

    /**
    * @brief Deinitialize Ethernet MAC
    *
    * @param[in] mac: Ethernet MAC instance
    *
    * @return
    *      - ESP_OK: deinitialize Ethernet MAC successfully
    *      - ESP_FAIL: deinitialize Ethernet MAC failed because some error occurred
    *
    */
    esp_err_t (*deinit)(esp_eth_mac_t *mac);

    /**
    * @brief Start Ethernet MAC
    *
    * @param[in] mac: Ethernet MAC instance
    *
    * @return
    *      - ESP_OK: start Ethernet MAC successfully
    *      - ESP_FAIL: start Ethernet MAC failed because some other error occurred
    *
    */
    esp_err_t (*start)(esp_eth_mac_t *mac);

    /**
    * @brief Stop Ethernet MAC
    *
    * @param[in] mac: Ethernet MAC instance
    *
    * @return
    *      - ESP_OK: stop Ethernet MAC successfully
    *      - ESP_FAIL: stop Ethernet MAC failed because some error occurred
    *
    */
    esp_err_t (*stop)(esp_eth_mac_t *mac);

    /**
    * @brief Transmit packet from Ethernet MAC
    *
    * @param[in] mac: Ethernet MAC instance
    * @param[in] buf: packet buffer to transmit
    * @param[in] length: length of packet
    *
    * @return
    *      - ESP_OK: transmit packet successfully
    *      - ESP_ERR_INVALID_SIZE: number of actually sent bytes differs to expected
    *      - ESP_FAIL: transmit packet failed because some other error occurred
    *
    * @note Returned error codes may differ for each specific MAC chip.
    *
    */
    esp_err_t (*transmit)(esp_eth_mac_t *mac, uint8_t *buf, uint32_t length);

    /**
    * @brief Transmit packet from Ethernet MAC constructed with special parameters at Layer2.
    *
    * @param[in] mac: Ethernet MAC instance
    * @param[in] argc: number variable arguments
    * @param[in] args: variable arguments
    *
    * @note Typical intended use case is to make possible to construct a frame from multiple higher layer
    *       buffers without a need of buffer reallocations. However, other use cases are not limited.
    *
    * @return
    *      - ESP_OK: transmit packet successfully
    *      - ESP_ERR_INVALID_SIZE: number of actually sent bytes differs to expected
    *      - ESP_FAIL: transmit packet failed because some other error occurred
    *
    * @note Returned error codes may differ for each specific MAC chip.
    *
    */
    esp_err_t (*transmit_vargs)(esp_eth_mac_t *mac, uint32_t argc, va_list args);

    /**
    * @brief Receive packet from Ethernet MAC
    *
    * @param[in] mac: Ethernet MAC instance
    * @param[out] buf: packet buffer which will preserve the received frame
    * @param[out] length: length of the received packet
    *
    * @note Memory of buf is allocated in the Layer2, make sure it get free after process.
    * @note Before this function got invoked, the value of "length" should set by user, equals the size of buffer.
    *       After the function returned, the value of "length" means the real length of received data.
    *
    * @return
    *      - ESP_OK: receive packet successfully
    *      - ESP_ERR_INVALID_ARG: receive packet failed because of invalid argument
    *      - ESP_ERR_INVALID_SIZE: input buffer size is not enough to hold the incoming data.
    *                              in this case, value of returned "length" indicates the real size of incoming data.
    *      - ESP_FAIL: receive packet failed because some other error occurred
    *
    */
    esp_err_t (*receive)(esp_eth_mac_t *mac, uint8_t *buf, uint32_t *length);

    /**
    * @brief Read PHY register
    *
    * @param[in] mac: Ethernet MAC instance
    * @param[in] phy_addr: PHY chip address (0~31)
    * @param[in] phy_reg: PHY register index code
    * @param[out] reg_value: PHY register value
    *
    * @return
    *      - ESP_OK: read PHY register successfully
    *      - ESP_ERR_INVALID_ARG: read PHY register failed because of invalid argument
    *      - ESP_ERR_INVALID_STATE: read PHY register failed because of wrong state of MAC
    *      - ESP_ERR_TIMEOUT: read PHY register failed because of timeout
    *      - ESP_FAIL: read PHY register failed because some other error occurred
    *
    */
    esp_err_t (*read_phy_reg)(esp_eth_mac_t *mac, uint32_t phy_addr, uint32_t phy_reg, uint32_t *reg_value);

    /**
    * @brief Write PHY register
    *
    * @param[in] mac: Ethernet MAC instance
    * @param[in] phy_addr: PHY chip address (0~31)
    * @param[in] phy_reg: PHY register index code
    * @param[in] reg_value: PHY register value
    *
    * @return
    *      - ESP_OK: write PHY register successfully
    *      - ESP_ERR_INVALID_STATE: write PHY register failed because of wrong state of MAC
    *      - ESP_ERR_TIMEOUT: write PHY register failed because of timeout
    *      - ESP_FAIL: write PHY register failed because some other error occurred
    *
    */
    esp_err_t (*write_phy_reg)(esp_eth_mac_t *mac, uint32_t phy_addr, uint32_t phy_reg, uint32_t reg_value);

    /**
    * @brief Set MAC address
    *
    * @param[in] mac: Ethernet MAC instance
    * @param[in] addr: MAC address
    *
    * @return
    *      - ESP_OK: set MAC address successfully
    *      - ESP_ERR_INVALID_ARG: set MAC address failed because of invalid argument
    *      - ESP_FAIL: set MAC address failed because some other error occurred
    *
    */
    esp_err_t (*set_addr)(esp_eth_mac_t *mac, uint8_t *addr);

    /**
    * @brief Get MAC address
    *
    * @param[in] mac: Ethernet MAC instance
    * @param[out] addr: MAC address
    *
    * @return
    *      - ESP_OK: get MAC address successfully
    *      - ESP_ERR_INVALID_ARG: get MAC address failed because of invalid argument
    *      - ESP_FAIL: get MAC address failed because some other error occurred
    *
    */
    esp_err_t (*get_addr)(esp_eth_mac_t *mac, uint8_t *addr);

    /**
    * @brief Set speed of MAC
    *
    * @param[in] ma:c Ethernet MAC instance
    * @param[in] speed: MAC speed
    *
    * @return
    *      - ESP_OK: set MAC speed successfully
    *      - ESP_ERR_INVALID_ARG: set MAC speed failed because of invalid argument
    *      - ESP_FAIL: set MAC speed failed because some other error occurred
    *
    */
    esp_err_t (*set_speed)(esp_eth_mac_t *mac, eth_speed_t speed);

    /**
    * @brief Set duplex mode of MAC
    *
    * @param[in] mac: Ethernet MAC instance
    * @param[in] duplex: MAC duplex
    *
    * @return
    *      - ESP_OK: set MAC duplex mode successfully
    *      - ESP_ERR_INVALID_ARG: set MAC duplex failed because of invalid argument
    *      - ESP_FAIL: set MAC duplex failed because some other error occurred
    *
    */
    esp_err_t (*set_duplex)(esp_eth_mac_t *mac, eth_duplex_t duplex);

    /**
    * @brief Set link status of MAC
    *
    * @param[in] mac: Ethernet MAC instance
    * @param[in] link: Link status
    *
    * @return
    *      - ESP_OK: set link status successfully
    *      - ESP_ERR_INVALID_ARG: set link status failed because of invalid argument
    *      - ESP_FAIL: set link status failed because some other error occurred
    *
    */
    esp_err_t (*set_link)(esp_eth_mac_t *mac, eth_link_t link);

    /**
    * @brief Set promiscuous of MAC
    *
    * @param[in] mac: Ethernet MAC instance
    * @param[in] enable: set true to enable promiscuous mode; set false to disable promiscuous mode
    *
    * @return
    *      - ESP_OK: set promiscuous mode successfully
    *      - ESP_FAIL: set promiscuous mode failed because some error occurred
    *
    */
    esp_err_t (*set_promiscuous)(esp_eth_mac_t *mac, bool enable);

    /**
    * @brief Enable flow control on MAC layer or not
    *
    * @param[in] mac: Ethernet MAC instance
    * @param[in] enable: set true to enable flow control; set false to disable flow control
    *
    * @return
    *      - ESP_OK: set flow control successfully
    *      - ESP_FAIL: set flow control failed because some error occurred
    *
    */
    esp_err_t (*enable_flow_ctrl)(esp_eth_mac_t *mac, bool enable);

    /**
    * @brief Set the PAUSE ability of peer node
    *
    * @param[in] mac: Ethernet MAC instance
    * @param[in] ability: zero indicates that pause function is supported by link partner; non-zero indicates that pause function is not supported by link partner
    *
    * @return
    *      - ESP_OK: set peer pause ability successfully
    *      - ESP_FAIL: set peer pause ability failed because some error occurred
    */
    esp_err_t (*set_peer_pause_ability)(esp_eth_mac_t *mac, uint32_t ability);

    /**
    * @brief Custom IO function of MAC driver. This function is intended to extend common options of esp_eth_ioctl to cover specifics of MAC chip.
    *
    * @note This function may not be assigned when the MAC chip supports only most common set of configuration options.
    *
    * @param[in] mac: Ethernet MAC instance
    * @param[in] cmd: IO control command
    * @param[in, out] data: address of data for `set` command or address where to store the data when used with `get` command
    *
    * @return
    *       - ESP_OK: process io command successfully
    *       - ESP_ERR_INVALID_ARG: process io command failed because of some invalid argument
    *       - ESP_FAIL: process io command failed because some other error occurred
    *       - ESP_ERR_NOT_SUPPORTED: requested feature is not supported
    */
    esp_err_t (*custom_ioctl)(esp_eth_mac_t *mac, uint32_t cmd, void *data);

    /**
    * @brief Free memory of Ethernet MAC
    *
    * @param[in] mac: Ethernet MAC instance
    *
    * @return
    *      - ESP_OK: free Ethernet MAC instance successfully
    *      - ESP_FAIL: free Ethernet MAC instance failed because some error occurred
    *
    */
    esp_err_t (*del)(esp_eth_mac_t *mac);
};

/**
 * @brief RMII Clock Mode Options
 *
 */
typedef enum {
    /**
     * @brief Default values configured using Kconfig are going to be used when "Default" selected.
     *
     */
    EMAC_CLK_DEFAULT,

    /**
     * @brief Input RMII Clock from external. EMAC Clock GPIO number needs to be configured when this option is selected.
     *
     * @note MAC will get RMII clock from outside. Note that ESP32 only supports GPIO0 to input the RMII clock.
     *
     */
    EMAC_CLK_EXT_IN,

    /**
     * @brief Output RMII Clock from internal APLL Clock. EMAC Clock GPIO number needs to be configured when this option is selected.
     *
     */
    EMAC_CLK_OUT
} emac_rmii_clock_mode_t;

/**
 * @brief RMII Clock GPIO number Options
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
     * @note GPIO0 can be set to output a pre-divided PLL clock (test only!). Enabling this option will configure GPIO0 to output a 50MHz clock.
     * In fact this clock doesn’t have directly relationship with EMAC peripheral. Sometimes this clock won’t work well with your PHY chip.
     * You might need to add some extra devices after GPIO0 (e.g. inverter). Note that outputting RMII clock on GPIO0 is an experimental practice.
     * If you want the Ethernet to work with WiFi, don’t select GPIO0 output mode for stability.
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
* @brief Configuration of Ethernet MAC object
*
*/
typedef struct {
    uint32_t sw_reset_timeout_ms;        /*!< Software reset timeout value (Unit: ms) */
    uint32_t rx_task_stack_size;         /*!< Stack size of the receive task */
    uint32_t rx_task_prio;               /*!< Priority of the receive task */
    uint32_t flags;                      /*!< Flags that specify extra capability for mac driver */
} eth_mac_config_t;

#define ETH_MAC_FLAG_WORK_WITH_CACHE_DISABLE (1 << 0) /*!< MAC driver can work when cache is disabled */
#define ETH_MAC_FLAG_PIN_TO_CORE (1 << 1)             /*!< Pin MAC task to the CPU core where driver installation happened */

/**
 * @brief Default configuration for Ethernet MAC object
 *
 */
#define ETH_MAC_DEFAULT_CONFIG()                          \
    {                                                     \
        .sw_reset_timeout_ms = 100,                       \
        .rx_task_stack_size = 2048,                       \
        .rx_task_prio = 15,                               \
        .flags = 0,                                       \
    }

#if CONFIG_ETH_USE_ESP32_EMAC
/**
* @brief EMAC specific configuration
*
*/
typedef struct {
    int smi_mdc_gpio_num;                   /*!< SMI MDC GPIO number, set to -1 could bypass the SMI GPIO configuration */
    int smi_mdio_gpio_num;                  /*!< SMI MDIO GPIO number, set to -1 could bypass the SMI GPIO configuration */
    eth_data_interface_t interface;         /*!< EMAC Data interface to PHY (MII/RMII) */
    eth_mac_clock_config_t clock_config;    /*!< EMAC Interface clock configuration */
    eth_mac_dma_burst_len_t dma_burst_len;  /*!< EMAC DMA burst length for both Tx and Rx */
} eth_esp32_emac_config_t;

/**
 * @brief Default ESP32's EMAC specific configuration
 *
 */
#define ETH_ESP32_EMAC_DEFAULT_CONFIG()               \
    {                                                 \
        .smi_mdc_gpio_num = 23,                       \
        .smi_mdio_gpio_num = 18,                      \
        .interface = EMAC_DATA_INTERFACE_RMII,        \
        .clock_config =                               \
        {                                             \
            .rmii =                                   \
            {                                         \
                .clock_mode = EMAC_CLK_DEFAULT,       \
                .clock_gpio = EMAC_CLK_IN_GPIO        \
            }                                         \
        },                                            \
        .dma_burst_len = ETH_DMA_BURST_LEN_32         \
    }

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

#if CONFIG_ETH_USE_SPI_ETHERNET
/**
 * @brief Custom SPI Driver Configuration.
 * This structure declares configuration and callback functions to access Ethernet SPI module via
 * user's custom SPI driver.
 *
 */
typedef struct
{
    /**
     * @brief Custom driver specific configuration data used by `init()` function.
     *
     * @note Type and its content is fully under user's control
     *
     */
    void *config;

    /**
     * @brief Custom driver SPI Initialization
     *
     * @param[in] spi_config: Custom driver specific configuration
     *
     * @return
     *      - spi_ctx: when initialization is successful, a pointer to context structure holding all variables
     *          needed for subsequent SPI access operations (e.g. SPI bus identification, mutexes, etc.)
     *      - NULL: driver initialization failed
     *
     * @note return type and its content is fully under user's control
     */
    void *(*init)(const void *spi_config);

    /**
     * @brief Custom driver De-initialization
     *
     * @param[in] spi_ctx: a pointer to driver specific context structure
     *
     * @return
     *      - ESP_OK: driver de-initialization was successful
     *      - ESP_FAIL: driver de-initialization failed
     *      - any other failure codes are allowed to be used to provide failure isolation
     */
    esp_err_t (*deinit)(void *spi_ctx);

    /**
     * @brief Custom driver SPI read
     *
     * @note The read function is responsible to construct command, address and data fields
     * of the SPI frame in format expected by particular SPI Ethernet module
     *
     * @param[in] spi_ctx: a pointer to driver specific context structure
     * @param[in] cmd: command
     * @param[in] addr: register address
     * @param[out] data: read data
     * @param[in] data_len: read data length in bytes
     *
     * @return
     *      - ESP_OK: read was successful
     *      - ESP_FAIL: read failed
     *      - any other failure codes are allowed to be used to provide failure isolation
     */
    esp_err_t (*read)(void *spi_ctx, uint32_t cmd, uint32_t addr, void *data, uint32_t data_len);

    /**
     * @brief Custom driver SPI write
     *
     * @note The write function is responsible to construct command, address and data fields
     * of the SPI frame in format expected by particular SPI Ethernet module
     *
     * @param[in] spi_ctx: a pointer to driver specific context structure
     * @param[in] cmd: command
     * @param[in] addr: register address
     * @param[in] data: data to write
     * @param[in] data_len: length of data to write in bytes
     *
     * @return
     *      - ESP_OK: write was successful
     *      - ESP_FAIL: write failed
     *      - any other failure codes are allowed to be used to provide failure isolation
     */
    esp_err_t (*write)(void *spi_ctx, uint32_t cmd, uint32_t addr, const void *data, uint32_t data_len);
} eth_spi_custom_driver_config_t;

/**
 * @brief Default configuration of the custom SPI driver.
 * Internal ESP-IDF SPI Master driver is used by default.
 *
 */
#define ETH_DEFAULT_SPI  \
    {                    \
        .config = NULL,  \
        .init = NULL,    \
        .deinit = NULL,  \
        .read = NULL,    \
        .write = NULL    \
    }
#endif // CONFIG_ETH_USE_SPI_ETHERNET

#if CONFIG_ETH_SPI_ETHERNET_DM9051
/**
 * @brief DM9051 specific configuration
 *
 */
typedef struct {
    int int_gpio_num;                                   /*!< Interrupt GPIO number, set -1 to not use interrupt and to poll rx status periodically */
    uint32_t poll_period_ms;                            /*!< Period in ms to poll rx status when interrupt mode is not used */
    spi_host_device_t spi_host_id;                      /*!< SPI peripheral (this field is invalid when custom SPI driver is defined) */
    spi_device_interface_config_t *spi_devcfg;          /*!< SPI device configuration (this field is invalid when custom SPI driver is defined) */
    eth_spi_custom_driver_config_t custom_spi_driver;   /*!< Custom SPI driver definitions */
} eth_dm9051_config_t;

/**
 * @brief Default DM9051 specific configuration
 *
 */
#define ETH_DM9051_DEFAULT_CONFIG(spi_host, spi_devcfg_p) \
    {                                           \
        .int_gpio_num = 4,                      \
        .poll_period_ms = 0,                    \
        .spi_host_id = spi_host,                \
        .spi_devcfg = spi_devcfg_p,             \
        .custom_spi_driver = ETH_DEFAULT_SPI,   \
    }

/**
* @brief Create DM9051 Ethernet MAC instance
*
* @param dm9051_config: DM9051 specific configuration
* @param mac_config: Ethernet MAC configuration
*
* @return
*      - instance: create MAC instance successfully
*      - NULL: create MAC instance failed because some error occurred
*/
esp_eth_mac_t *esp_eth_mac_new_dm9051(const eth_dm9051_config_t *dm9051_config, const eth_mac_config_t *mac_config);
#endif // CONFIG_ETH_SPI_ETHERNET_DM9051

#if CONFIG_ETH_SPI_ETHERNET_W5500
/**
 * @brief W5500 specific configuration
 *
 */
typedef struct {
    int int_gpio_num;                                   /*!< Interrupt GPIO number, set -1 to not use interrupt and to poll rx status periodically */
    uint32_t poll_period_ms;                            /*!< Period in ms to poll rx status when interrupt mode is not used */
    spi_host_device_t spi_host_id;                      /*!< SPI peripheral (this field is invalid when custom SPI driver is defined)*/
    spi_device_interface_config_t *spi_devcfg;          /*!< SPI device configuration (this field is invalid when custom SPI driver is defined)*/
    eth_spi_custom_driver_config_t custom_spi_driver;   /*!< Custom SPI driver definitions */
} eth_w5500_config_t;

/**
 * @brief Default W5500 specific configuration
 *
 */
#define ETH_W5500_DEFAULT_CONFIG(spi_host, spi_devcfg_p) \
    {                                           \
        .int_gpio_num = 4,                      \
        .poll_period_ms = 0,                    \
        .spi_host_id = spi_host,                \
        .spi_devcfg = spi_devcfg_p,             \
        .custom_spi_driver = ETH_DEFAULT_SPI,   \
    }

/**
* @brief Create W5500 Ethernet MAC instance
*
* @param w5500_config: W5500 specific configuration
* @param mac_config: Ethernet MAC configuration
*
* @return
*      - instance: create MAC instance successfully
*      - NULL: create MAC instance failed because some error occurred
*/
esp_eth_mac_t *esp_eth_mac_new_w5500(const eth_w5500_config_t *w5500_config, const eth_mac_config_t *mac_config);
#endif // CONFIG_ETH_SPI_ETHERNET_W5500

#if CONFIG_ETH_SPI_ETHERNET_KSZ8851SNL
/**
 * @brief KSZ8851SNL specific configuration
 *
 */
typedef struct {
    int int_gpio_num;                                   /*!< Interrupt GPIO number, set -1 to not use interrupt and to poll rx status periodically */
    uint32_t poll_period_ms;                            /*!< Period in ms to poll rx status when interrupt mode is not used */
    spi_host_device_t spi_host_id;                      /*!< SPI peripheral (this field is invalid when custom SPI driver is defined) */
    spi_device_interface_config_t *spi_devcfg;          /*!< SPI device configuration (this field is invalid when custom SPI driver is defined) */
    eth_spi_custom_driver_config_t custom_spi_driver;   /*!< Custom SPI driver definitions */
} eth_ksz8851snl_config_t;

/**
 * @brief Default KSZ8851SNL specific configuration
 *
 */
#define ETH_KSZ8851SNL_DEFAULT_CONFIG(spi_host, spi_devcfg_p) \
    {                                               \
        .int_gpio_num = 4,                          \
        .poll_period_ms = 0,                        \
        .spi_host_id = spi_host,                    \
        .spi_devcfg = spi_devcfg_p,                 \
        .custom_spi_driver = ETH_DEFAULT_SPI,       \
    }

/**
* @brief Create KSZ8851SNL Ethernet MAC instance
*
* @param ksz8851snl_config: KSZ8851SNL specific configuration
* @param mac_config: Ethernet MAC configuration
*
* @return
*      - instance: create MAC instance successfully
*      - NULL: create MAC instance failed because some error occurred
*/
esp_eth_mac_t *esp_eth_mac_new_ksz8851snl(const eth_ksz8851snl_config_t *ksz8851snl_config, const eth_mac_config_t *mac_config);
#endif // CONFIG_ETH_SPI_ETHERNET_KSZ8851

#if CONFIG_ETH_USE_OPENETH
/**
* @brief Create OpenCores Ethernet MAC instance
*
* @param config: Ethernet MAC configuration
*
* @return
*      - instance: create MAC instance successfully
*      - NULL: create MAC instance failed because some error occurred
*/
esp_eth_mac_t *esp_eth_mac_new_openeth(const eth_mac_config_t *config);
#endif // CONFIG_ETH_USE_OPENETH

#ifdef __cplusplus
}
#endif
