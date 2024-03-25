/*
 * SPDX-FileCopyrightText: 2010-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// Internal header, don't use it in the user code

#pragma once

#include <esp_intr_alloc.h>
#include "driver/spi_common.h"
#include "freertos/FreeRTOS.h"
#include "hal/spi_types.h"
#include "hal/dma_types.h"
#include "esp_private/spi_dma.h"
#include "esp_pm.h"
#include "esp_private/spi_share_hw_ctrl.h"
#if SOC_GDMA_SUPPORTED
#include "esp_private/gdma.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef CONFIG_SPI_MASTER_ISR_IN_IRAM
#define SPI_MASTER_ISR_ATTR IRAM_ATTR
#else
#define SPI_MASTER_ISR_ATTR
#endif

#ifdef CONFIG_SPI_MASTER_IN_IRAM
#define SPI_MASTER_ATTR IRAM_ATTR
#else
#define SPI_MASTER_ATTR
#endif

//NOTE!! If both A and B are not defined, '#if (A==B)' is true, because GCC use 0 stand for undefined symbol
#if SOC_GPSPI_SUPPORTED && defined(SOC_GDMA_BUS_AXI) && (SOC_GDMA_TRIG_PERIPH_SPI2_BUS == SOC_GDMA_BUS_AXI)
#define DMA_DESC_MEM_ALIGN_SIZE 8
typedef dma_descriptor_align8_t spi_dma_desc_t;
#else
#define DMA_DESC_MEM_ALIGN_SIZE 4
typedef dma_descriptor_align4_t spi_dma_desc_t;
#endif

/// Attributes of an SPI bus
typedef struct {
    spi_bus_config_t bus_cfg;           ///< Config used to initialize the bus
    uint32_t flags;                     ///< Flags (attributes) of the bus
    int max_transfer_sz;                ///< Maximum length of bytes available to send
    bool dma_enabled;                   ///< To enable DMA or not
    uint16_t internal_mem_align_size;   ///< Buffer align byte requirement for internal memory
    spi_bus_lock_handle_t lock;
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock;       ///< Power management lock
#endif
} spi_bus_attr_t;

typedef struct {
#if SOC_GDMA_SUPPORTED
    gdma_channel_handle_t tx_dma_chan;  ///< GDMA tx channel
    gdma_channel_handle_t rx_dma_chan;  ///< GDMA rx channel
#else
    spi_dma_chan_handle_t tx_dma_chan;  ///< TX DMA channel, on ESP32 and ESP32S2, tx_dma_chan and rx_dma_chan are same
    spi_dma_chan_handle_t rx_dma_chan;  ///< RX DMA channel, on ESP32 and ESP32S2, tx_dma_chan and rx_dma_chan are same
#endif
    int dma_desc_num;               ///< DMA descriptor number of dmadesc_tx or dmadesc_rx.
    spi_dma_desc_t *dmadesc_tx;     ///< DMA descriptor array for TX
    spi_dma_desc_t *dmadesc_rx;     ///< DMA descriptor array for RX
} spi_dma_ctx_t;

/// Destructor called when a bus is deinitialized.
typedef esp_err_t (*spi_destroy_func_t)(void*);

/**
 * @brief Alloc DMA channel for SPI
 *
 * @param host_id                  SPI host ID
 * @param dma_chan                 DMA channel to be used
 * @param out_dma_ctx              Actual DMA channel context (if you choose to assign a specific DMA channel, this will be the channel you assigned before)
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_NO_MEM:        No enough memory
 *        - ESP_ERR_NOT_FOUND:     There is no available DMA channel
 */
esp_err_t spicommon_dma_chan_alloc(spi_host_device_t host_id, spi_dma_chan_t dma_chan, spi_dma_ctx_t **out_dma_ctx);

/**
 * @brief Alloc DMA descriptors for SPI
 *
 * @param dma_ctx                  DMA context returned by `spicommon_dma_chan_alloc`
 * @param[in]  cfg_max_sz          Expected maximum transfer size, in bytes.
 * @param[out] actual_max_sz       Actual max transfer size one transaction can be, in bytes.
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_NO_MEM:        No enough memory
 */
esp_err_t spicommon_dma_desc_alloc(spi_dma_ctx_t *dma_ctx, int cfg_max_sz, int *actual_max_sz);

/**
 * Setupt/Configure dma descriptor link list
 *
 * @param dmadesc start of dma descriptor memory
 * @param data    start of data buffer to be configured in
 * @param len     length of data buffer, in byte
 * @param is_rx   if descriptor is for rx/receive direction
 */
void spicommon_dma_desc_setup_link(spi_dma_desc_t *dmadesc, const void *data, int len, bool is_rx);

/**
 * @brief Free DMA for SPI
 *
 * @param dma_ctx  spi_dma_ctx_t struct pointer
 *
 * @return
 *        - ESP_OK: On success
 */
esp_err_t spicommon_dma_chan_free(spi_dma_ctx_t *dma_ctx);

/**
 * @brief Connect a SPI peripheral to GPIO pins
 *
 * This routine is used to connect a SPI peripheral to the IO-pads and DMA channel given in
 * the arguments. Depending on the IO-pads requested, the routing is done either using the
 * IO_mux or using the GPIO matrix.
 *
 * @param host SPI peripheral to be routed
 * @param bus_config Pointer to a spi_bus_config struct detailing the GPIO pins
 * @param flags Combination of SPICOMMON_BUSFLAG_* flags, set to ensure the pins set are capable with some functions:
 *              - ``SPICOMMON_BUSFLAG_MASTER``: Initialize I/O in master mode
 *              - ``SPICOMMON_BUSFLAG_SLAVE``: Initialize I/O in slave mode
 *              - ``SPICOMMON_BUSFLAG_IOMUX_PINS``: Pins set should match the iomux pins of the controller.
 *              - ``SPICOMMON_BUSFLAG_SCLK``, ``SPICOMMON_BUSFLAG_MISO``, ``SPICOMMON_BUSFLAG_MOSI``:
 *                  Make sure SCLK/MISO/MOSI is/are set to a valid GPIO. Also check output capability according to the mode.
 *              - ``SPICOMMON_BUSFLAG_DUAL``: Make sure both MISO and MOSI are output capable so that DIO mode is capable.
 *              - ``SPICOMMON_BUSFLAG_WPHD`` Make sure WP and HD are set to valid output GPIOs.
 *              - ``SPICOMMON_BUSFLAG_QUAD``: Combination of ``SPICOMMON_BUSFLAG_DUAL`` and ``SPICOMMON_BUSFLAG_WPHD``.
 *              - ``SPICOMMON_BUSFLAG_IO4_IO7``: Make sure spi data4 ~ spi data7 are set to valid output GPIOs.
 *              - ``SPICOMMON_BUSFLAG_OCTAL``: Combination of ``SPICOMMON_BUSFLAG_QUAL`` and ``SPICOMMON_BUSFLAG_IO4_IO7``.
 * @param[out] flags_o A SPICOMMON_BUSFLAG_* flag combination of bus abilities will be written to this address.
 *              Leave to NULL if not needed.
 *              - ``SPICOMMON_BUSFLAG_IOMUX_PINS``: The bus is connected to iomux pins.
 *              - ``SPICOMMON_BUSFLAG_SCLK``, ``SPICOMMON_BUSFLAG_MISO``, ``SPICOMMON_BUSFLAG_MOSI``: The bus has
 *                  CLK/MISO/MOSI connected.
 *              - ``SPICOMMON_BUSFLAG_DUAL``: The bus is capable with DIO mode.
 *              - ``SPICOMMON_BUSFLAG_WPHD`` The bus has WP and HD connected.
 *              - ``SPICOMMON_BUSFLAG_QUAD``: Combination of ``SPICOMMON_BUSFLAG_DUAL`` and ``SPICOMMON_BUSFLAG_WPHD``.
 *              - ``SPICOMMON_BUSFLAG_IO4_IO7``: The bus has spi data4 ~ spi data7 connected.
 *              - ``SPICOMMON_BUSFLAG_OCTAL``: Combination of ``SPICOMMON_BUSFLAG_QUAL`` and ``SPICOMMON_BUSFLAG_IO4_IO7``.
 * @return
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP_OK                on success
 */
esp_err_t spicommon_bus_initialize_io(spi_host_device_t host, const spi_bus_config_t *bus_config, uint32_t flags, uint32_t *flags_o);

/**
 * @brief Free the IO used by a SPI peripheral
 *
 * @param bus_cfg Bus config struct which defines which pins to be used.
 *
 * @return
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP_OK                on success
 */
esp_err_t spicommon_bus_free_io_cfg(const spi_bus_config_t *bus_cfg);

/**
 * @brief Initialize a Chip Select pin for a specific SPI peripheral
 *
 * @param host SPI peripheral
 * @param cs_io_num GPIO pin to route
 * @param cs_num CS id to route
 * @param force_gpio_matrix If true, CS will always be routed through the GPIO matrix. If false,
 *                          if the GPIO number allows it, the routing will happen through the IO_mux.
 */
void spicommon_cs_initialize(spi_host_device_t host, int cs_io_num, int cs_num, int force_gpio_matrix);

/**
 * @brief Free a chip select line
 *
 * @param cs_gpio_num CS gpio num to free
 */
void spicommon_cs_free_io(int cs_gpio_num);

/**
 * @brief Check whether all pins used by a host are through IOMUX.
 *
 * @param host SPI peripheral
 *
 * @return false if any pins are through the GPIO matrix, otherwise true.
 */
bool spicommon_bus_using_iomux(spi_host_device_t host);

/**
 * @brief Get the IRQ source for a specific SPI host
 *
 * @param host The SPI host
 *
 * @return The hosts IRQ source
 */
int spicommon_irqsource_for_host(spi_host_device_t host);

/**
 * @brief Get the IRQ source for a specific SPI DMA
 *
 * @param host The SPI host
 *
 * @return The hosts IRQ source
 */
int spicommon_irqdma_source_for_host(spi_host_device_t host);

/**
 * Callback, to be called when a DMA engine reset is completed
*/
typedef void(*dmaworkaround_cb_t)(void *arg);

#if CONFIG_IDF_TARGET_ESP32
//This workaround is only for esp32
/**
 * @brief Request a reset for a certain DMA channel
 *
 * @note In some (well-defined) cases in the ESP32 (at least rev v.0 and v.1), a SPI DMA channel will get confused. This can be remedied
 * by resetting the SPI DMA hardware in case this happens. Unfortunately, the reset knob used for this will reset _both_ DMA channels, and
 * as such can only done safely when both DMA channels are idle. These functions coordinate this.
 *
 * Essentially, when a reset is needed, a driver can request this using spicommon_dmaworkaround_req_reset. This is supposed to be called
 * with an user-supplied function as an argument. If both DMA channels are idle, this call will reset the DMA subsystem and return true.
 * If the other DMA channel is still busy, it will return false; as soon as the other DMA channel is done, however, it will reset the
 * DMA subsystem and call the callback. The callback is then supposed to be used to continue the SPI drivers activity.
 *
 * @param dmachan DMA channel associated with the SPI host that needs a reset
 * @param cb Callback to call in case DMA channel cannot be reset immediately
 * @param arg Argument to the callback
 *
 * @return True when a DMA reset could be executed immediately. False when it could not; in this
 *         case the callback will be called with the specified argument when the logic can execute
 *         a reset, after that reset.
 */
bool spicommon_dmaworkaround_req_reset(int dmachan, dmaworkaround_cb_t cb, void *arg);

/**
 * @brief Check if a DMA reset is requested but has not completed yet
 *
 * @return True when a DMA reset is requested but hasn't completed yet. False otherwise.
 */
bool spicommon_dmaworkaround_reset_in_progress(void);

/**
 * @brief Mark a DMA channel as idle.
 *
 * A call to this function tells the workaround logic that this channel will
 * not be affected by a global SPI DMA reset.
 */
void spicommon_dmaworkaround_idle(int dmachan);

/**
 * @brief Mark a DMA channel as active.
 *
 * A call to this function tells the workaround logic that this channel will
 * be affected by a global SPI DMA reset, and a reset like that should not be attempted.
 */
void spicommon_dmaworkaround_transfer_active(int dmachan);
#endif  //#if CONFIG_IDF_TARGET_ESP32

/*******************************************************************************
 * Bus attributes
 ******************************************************************************/
/**
 * @brief Set bus lock for the main bus, called by startup code.
 *
 * @param lock The lock to be used by the main SPI bus.
 */
void spi_bus_main_set_lock(spi_bus_lock_handle_t lock);

/**
 * @brief Get the attributes of a specified SPI bus.
 *
 * @param host_id The specified host to get attribute
 * @return (Const) Pointer to the attributes
 */
const spi_bus_attr_t* spi_bus_get_attr(spi_host_device_t host_id);

/**
 * @brief Get the dma context of a specified SPI bus.
 *
 * @param host_id The specified host to get attribute
 * @return (Const) Pointer to the dma context
 */
const spi_dma_ctx_t* spi_bus_get_dma_ctx(spi_host_device_t host_id);

/**
 * @brief Register a function to a initialized bus to make it called when deinitializing the bus.
 *
 * @param host_id   The SPI bus to register the destructor.
 * @param f         Destructor to register
 * @param arg       The argument to call the destructor
 * @return Always ESP_OK.
 */
esp_err_t spi_bus_register_destroy_func(spi_host_device_t host_id,
                                        spi_destroy_func_t f, void *arg);

#ifdef __cplusplus
}
#endif
