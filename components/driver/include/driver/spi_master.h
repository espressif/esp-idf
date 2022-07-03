/*
 * SPDX-FileCopyrightText: 2010-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
//for spi_bus_initialization funcions. to be back-compatible
#include "driver/spi_common.h"

/** SPI master clock is divided by 80MHz apb clock. Below defines are example frequencies, and are accurate. Be free to specify a random frequency, it will be rounded to closest frequency (to macros below if above 8MHz).
  * 8MHz
  */
#define SPI_MASTER_FREQ_8M      (APB_CLK_FREQ/10)
#define SPI_MASTER_FREQ_9M      (APB_CLK_FREQ/9)    ///< 8.89MHz
#define SPI_MASTER_FREQ_10M     (APB_CLK_FREQ/8)    ///< 10MHz
#define SPI_MASTER_FREQ_11M     (APB_CLK_FREQ/7)    ///< 11.43MHz
#define SPI_MASTER_FREQ_13M     (APB_CLK_FREQ/6)    ///< 13.33MHz
#define SPI_MASTER_FREQ_16M     (APB_CLK_FREQ/5)    ///< 16MHz
#define SPI_MASTER_FREQ_20M     (APB_CLK_FREQ/4)    ///< 20MHz
#define SPI_MASTER_FREQ_26M     (APB_CLK_FREQ/3)    ///< 26.67MHz
#define SPI_MASTER_FREQ_40M     (APB_CLK_FREQ/2)    ///< 40MHz
#define SPI_MASTER_FREQ_80M     (APB_CLK_FREQ/1)    ///< 80MHz
#ifdef __cplusplus
extern "C"
{
#endif

#define SPI_DEVICE_TXBIT_LSBFIRST          (1<<0)  ///< Transmit command/address/data LSB first instead of the default MSB first
#define SPI_DEVICE_RXBIT_LSBFIRST          (1<<1)  ///< Receive data LSB first instead of the default MSB first
#define SPI_DEVICE_BIT_LSBFIRST            (SPI_DEVICE_TXBIT_LSBFIRST|SPI_DEVICE_RXBIT_LSBFIRST) ///< Transmit and receive LSB first
#define SPI_DEVICE_3WIRE                   (1<<2)  ///< Use MOSI (=spid) for both sending and receiving data
#define SPI_DEVICE_POSITIVE_CS             (1<<3)  ///< Make CS positive during a transaction instead of negative
#define SPI_DEVICE_HALFDUPLEX              (1<<4)  ///< Transmit data before receiving it, instead of simultaneously
#define SPI_DEVICE_CLK_AS_CS               (1<<5)  ///< Output clock on CS line if CS is active
/** There are timing issue when reading at high frequency (the frequency is related to whether iomux pins are used, valid time after slave sees the clock).
  *     - In half-duplex mode, the driver automatically inserts dummy bits before reading phase to fix the timing issue. Set this flag to disable this feature.
  *     - In full-duplex mode, however, the hardware cannot use dummy bits, so there is no way to prevent data being read from getting corrupted.
  *       Set this flag to confirm that you're going to work with output only, or read without dummy bits at your own risk.
  */
#define SPI_DEVICE_NO_DUMMY                (1<<6)
#define SPI_DEVICE_DDRCLK                  (1<<7)


typedef struct spi_transaction_t spi_transaction_t;
typedef void(*transaction_cb_t)(spi_transaction_t *trans);

/**
 * @brief This is a configuration for a SPI slave device that is connected to one of the SPI buses.
 */
typedef struct {
    uint8_t command_bits;           ///< Default amount of bits in command phase (0-16), used when ``SPI_TRANS_VARIABLE_CMD`` is not used, otherwise ignored.
    uint8_t address_bits;           ///< Default amount of bits in address phase (0-64), used when ``SPI_TRANS_VARIABLE_ADDR`` is not used, otherwise ignored.
    uint8_t dummy_bits;             ///< Amount of dummy bits to insert between address and data phase
    uint8_t mode;                   /**< SPI mode, representing a pair of (CPOL, CPHA) configuration:
                                         - 0: (0, 0)
                                         - 1: (0, 1)
                                         - 2: (1, 0)
                                         - 3: (1, 1)
                                     */
    uint16_t duty_cycle_pos;         ///< Duty cycle of positive clock, in 1/256th increments (128 = 50%/50% duty). Setting this to 0 (=not setting it) is equivalent to setting this to 128.
    uint16_t cs_ena_pretrans;        ///< Amount of SPI bit-cycles the cs should be activated before the transmission (0-16). This only works on half-duplex transactions.
    uint8_t cs_ena_posttrans;       ///< Amount of SPI bit-cycles the cs should stay active after the transmission (0-16)
    int clock_speed_hz;             ///< Clock speed, divisors of 80MHz, in Hz. See ``SPI_MASTER_FREQ_*``.
    int input_delay_ns;             /**< Maximum data valid time of slave. The time required between SCLK and MISO
        valid, including the possible clock delay from slave to master. The driver uses this value to give an extra
        delay before the MISO is ready on the line. Leave at 0 unless you know you need a delay. For better timing
        performance at high frequency (over 8MHz), it's suggest to have the right value.
        */
    int spics_io_num;               ///< CS GPIO pin for this device, or -1 if not used
    uint32_t flags;                 ///< Bitwise OR of SPI_DEVICE_* flags
    int queue_size;                 ///< Transaction queue size. This sets how many transactions can be 'in the air' (queued using spi_device_queue_trans but not yet finished using spi_device_get_trans_result) at the same time
    transaction_cb_t pre_cb;   /**< Callback to be called before a transmission is started.
                                 *
                                 *  This callback is called within interrupt
                                 *  context should be in IRAM for best
                                 *  performance, see "Transferring Speed"
                                 *  section in the SPI Master documentation for
                                 *  full details. If not, the callback may crash
                                 *  during flash operation when the driver is
                                 *  initialized with ESP_INTR_FLAG_IRAM.
                                 */
    transaction_cb_t post_cb;  /**< Callback to be called after a transmission has completed.
                                 *
                                 *  This callback is called within interrupt
                                 *  context should be in IRAM for best
                                 *  performance, see "Transferring Speed"
                                 *  section in the SPI Master documentation for
                                 *  full details. If not, the callback may crash
                                 *  during flash operation when the driver is
                                 *  initialized with ESP_INTR_FLAG_IRAM.
                                 */
} spi_device_interface_config_t;


#define SPI_TRANS_MODE_DIO            (1<<0)  ///< Transmit/receive data in 2-bit mode
#define SPI_TRANS_MODE_QIO            (1<<1)  ///< Transmit/receive data in 4-bit mode
#define SPI_TRANS_USE_RXDATA          (1<<2)  ///< Receive into rx_data member of spi_transaction_t instead into memory at rx_buffer.
#define SPI_TRANS_USE_TXDATA          (1<<3)  ///< Transmit tx_data member of spi_transaction_t instead of data at tx_buffer. Do not set tx_buffer when using this.
#define SPI_TRANS_MODE_DIOQIO_ADDR    (1<<4)  ///< Also transmit address in mode selected by SPI_MODE_DIO/SPI_MODE_QIO
#define SPI_TRANS_VARIABLE_CMD        (1<<5)  ///< Use the ``command_bits`` in ``spi_transaction_ext_t`` rather than default value in ``spi_device_interface_config_t``.
#define SPI_TRANS_VARIABLE_ADDR       (1<<6)  ///< Use the ``address_bits`` in ``spi_transaction_ext_t`` rather than default value in ``spi_device_interface_config_t``.
#define SPI_TRANS_VARIABLE_DUMMY      (1<<7)  ///< Use the ``dummy_bits`` in ``spi_transaction_ext_t`` rather than default value in ``spi_device_interface_config_t``.
#define SPI_TRANS_CS_KEEP_ACTIVE      (1<<8)  ///< Keep CS active after data transfer
#define SPI_TRANS_MULTILINE_CMD       (1<<9)  ///< The data lines used at command phase is the same as data phase (otherwise, only one data line is used at command phase)
#define SPI_TRANS_MODE_OCT            (1<<10) ///< Transmit/receive data in 8-bit mode
#define SPI_TRANS_MULTILINE_ADDR      SPI_TRANS_MODE_DIOQIO_ADDR ///< The data lines used at address phase is the same as data phase (otherwise, only one data line is used at address phase)

/**
 * This structure describes one SPI transaction. The descriptor should not be modified until the transaction finishes.
 */
struct spi_transaction_t {
    uint32_t flags;                 ///< Bitwise OR of SPI_TRANS_* flags
    uint16_t cmd;                   /**< Command data, of which the length is set in the ``command_bits`` of spi_device_interface_config_t.
                                      *
                                      *  <b>NOTE: this field, used to be "command" in ESP-IDF 2.1 and before, is re-written to be used in a new way in ESP-IDF 3.0.</b>
                                      *
                                      *  Example: write 0x0123 and command_bits=12 to send command 0x12, 0x3_ (in previous version, you may have to write 0x3_12).
                                      */
    uint64_t addr;                  /**< Address data, of which the length is set in the ``address_bits`` of spi_device_interface_config_t.
                                      *
                                      *  <b>NOTE: this field, used to be "address" in ESP-IDF 2.1 and before, is re-written to be used in a new way in ESP-IDF3.0.</b>
                                      *
                                      *  Example: write 0x123400 and address_bits=24 to send address of 0x12, 0x34, 0x00 (in previous version, you may have to write 0x12340000).
                                      */
    size_t length;                  ///< Total data length, in bits
    size_t rxlength;                ///< Total data length received, should be not greater than ``length`` in full-duplex mode (0 defaults this to the value of ``length``).
    void *user;                     ///< User-defined variable. Can be used to store eg transaction ID.
    union {
        const void *tx_buffer;      ///< Pointer to transmit buffer, or NULL for no MOSI phase
        uint8_t tx_data[4];         ///< If SPI_TRANS_USE_TXDATA is set, data set here is sent directly from this variable.
    };
    union {
        void *rx_buffer;            ///< Pointer to receive buffer, or NULL for no MISO phase. Written by 4 bytes-unit if DMA is used.
        uint8_t rx_data[4];         ///< If SPI_TRANS_USE_RXDATA is set, data is received directly to this variable
    };
} ;        //the rx data should start from a 32-bit aligned address to get around dma issue.

/**
 * This struct is for SPI transactions which may change their address and command length.
 * Please do set the flags in base to ``SPI_TRANS_VARIABLE_CMD_ADR`` to use the bit length here.
 */
typedef struct {
    struct spi_transaction_t base;  ///< Transaction data, so that pointer to spi_transaction_t can be converted into spi_transaction_ext_t
    uint8_t command_bits;           ///< The command length in this transaction, in bits.
    uint8_t address_bits;           ///< The address length in this transaction, in bits.
    uint8_t dummy_bits;             ///< The dummy length in this transaction, in bits.
} spi_transaction_ext_t ;


typedef struct spi_device_t *spi_device_handle_t;  ///< Handle for a device on a SPI bus
/**
 * @brief Allocate a device on a SPI bus
 *
 * This initializes the internal structures for a device, plus allocates a CS pin on the indicated SPI master
 * peripheral and routes it to the indicated GPIO. All SPI master devices have three CS pins and can thus control
 * up to three devices.
 *
 * @note While in general, speeds up to 80MHz on the dedicated SPI pins and 40MHz on GPIO-matrix-routed pins are
 *       supported, full-duplex transfers routed over the GPIO matrix only support speeds up to 26MHz.
 *
 * @param host_id SPI peripheral to allocate device on
 * @param dev_config SPI interface protocol config for the device
 * @param handle Pointer to variable to hold the device handle
 * @return
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP_ERR_NOT_FOUND     if host doesn't have any free CS slots
 *         - ESP_ERR_NO_MEM        if out of memory
 *         - ESP_OK                on success
 */
esp_err_t spi_bus_add_device(spi_host_device_t host_id, const spi_device_interface_config_t *dev_config, spi_device_handle_t *handle);


/**
 * @brief Remove a device from the SPI bus
 *
 * @param handle Device handle to free
 * @return
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP_ERR_INVALID_STATE if device already is freed
 *         - ESP_OK                on success
 */
esp_err_t spi_bus_remove_device(spi_device_handle_t handle);


/**
 * @brief Queue a SPI transaction for interrupt transaction execution. Get the result by ``spi_device_get_trans_result``.
 *
 * @note Normally a device cannot start (queue) polling and interrupt
 *      transactions simultaneously.
 *
 * @param handle Device handle obtained using spi_host_add_dev
 * @param trans_desc Description of transaction to execute
 * @param ticks_to_wait Ticks to wait until there's room in the queue; use portMAX_DELAY to
 *                      never time out.
 * @return
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid. This can happen if SPI_TRANS_CS_KEEP_ACTIVE flag is specified while
 *                                 the bus was not acquired (`spi_device_acquire_bus()` should be called first)
 *         - ESP_ERR_TIMEOUT       if there was no room in the queue before ticks_to_wait expired
 *         - ESP_ERR_NO_MEM        if allocating DMA-capable temporary buffer failed
 *         - ESP_ERR_INVALID_STATE if previous transactions are not finished
 *         - ESP_OK                on success
 */
esp_err_t spi_device_queue_trans(spi_device_handle_t handle, spi_transaction_t *trans_desc, TickType_t ticks_to_wait);


/**
 * @brief Get the result of a SPI transaction queued earlier by ``spi_device_queue_trans``.
 *
 * This routine will wait until a transaction to the given device
 * succesfully completed. It will then return the description of the
 * completed transaction so software can inspect the result and e.g. free the memory or
 * re-use the buffers.
 *
 * @param handle Device handle obtained using spi_host_add_dev
 * @param trans_desc Pointer to variable able to contain a pointer to the description of the transaction
        that is executed. The descriptor should not be modified until the descriptor is returned by
        spi_device_get_trans_result.
 * @param ticks_to_wait Ticks to wait until there's a returned item; use portMAX_DELAY to never time
                        out.
 * @return
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP_ERR_TIMEOUT       if there was no completed transaction before ticks_to_wait expired
 *         - ESP_OK                on success
 */
esp_err_t spi_device_get_trans_result(spi_device_handle_t handle, spi_transaction_t **trans_desc, TickType_t ticks_to_wait);


/**
 * @brief Send a SPI transaction, wait for it to complete, and return the result
 *
 * This function is the equivalent of calling spi_device_queue_trans() followed by spi_device_get_trans_result().
 * Do not use this when there is still a transaction separately queued (started) from spi_device_queue_trans() or polling_start/transmit that hasn't been finalized.
 *
 * @note This function is not thread safe when multiple tasks access the same SPI device.
 *      Normally a device cannot start (queue) polling and interrupt
 *      transactions simutanuously.
 *
 * @param handle Device handle obtained using spi_host_add_dev
 * @param trans_desc Description of transaction to execute
 * @return
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP_OK                on success
 */
esp_err_t spi_device_transmit(spi_device_handle_t handle, spi_transaction_t *trans_desc);


/**
 * @brief Immediately start a polling transaction.
 *
 * @note Normally a device cannot start (queue) polling and interrupt
 *      transactions simutanuously. Moreover, a device cannot start a new polling
 *      transaction if another polling transaction is not finished.
 *
 * @param handle Device handle obtained using spi_host_add_dev
 * @param trans_desc Description of transaction to execute
 * @param ticks_to_wait Ticks to wait until there's room in the queue;
 *              currently only portMAX_DELAY is supported.
 *
 * @return
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid. This can happen if SPI_TRANS_CS_KEEP_ACTIVE flag is specified while
 *                                 the bus was not acquired (`spi_device_acquire_bus()` should be called first)
 *         - ESP_ERR_TIMEOUT       if the device cannot get control of the bus before ``ticks_to_wait`` expired
 *         - ESP_ERR_NO_MEM        if allocating DMA-capable temporary buffer failed
 *         - ESP_ERR_INVALID_STATE if previous transactions are not finished
 *         - ESP_OK                on success
 */
esp_err_t spi_device_polling_start(spi_device_handle_t handle, spi_transaction_t *trans_desc, TickType_t ticks_to_wait);


/**
 * @brief Poll until the polling transaction ends.
 *
 * This routine will not return until the transaction to the given device has
 * succesfully completed. The task is not blocked, but actively busy-spins for
 * the transaction to be completed.
 *
 * @param handle Device handle obtained using spi_host_add_dev
 * @param ticks_to_wait Ticks to wait until there's a returned item; use portMAX_DELAY to never time
                        out.
 * @return
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP_ERR_TIMEOUT       if the transaction cannot finish before ticks_to_wait expired
 *         - ESP_OK                on success
 */
esp_err_t spi_device_polling_end(spi_device_handle_t handle, TickType_t ticks_to_wait);


/**
 * @brief Send a polling transaction, wait for it to complete, and return the result
 *
 * This function is the equivalent of calling spi_device_polling_start() followed by spi_device_polling_end().
 * Do not use this when there is still a transaction that hasn't been finalized.
 *
 * @note This function is not thread safe when multiple tasks access the same SPI device.
 *      Normally a device cannot start (queue) polling and interrupt
 *      transactions simutanuously.
 *
 * @param handle Device handle obtained using spi_host_add_dev
 * @param trans_desc Description of transaction to execute
 * @return
 *         - ESP_ERR_INVALID_ARG   if parameter is invalid
 *         - ESP_OK                on success
 */
esp_err_t spi_device_polling_transmit(spi_device_handle_t handle, spi_transaction_t *trans_desc);


/**
 * @brief Occupy the SPI bus for a device to do continuous transactions.
 *
 * Transactions to all other devices will be put off until ``spi_device_release_bus`` is called.
 *
 * @note The function will wait until all the existing transactions have been sent.
 *
 * @param device The device to occupy the bus.
 * @param wait Time to wait before the the bus is occupied by the device. Currently MUST set to portMAX_DELAY.
 *
 * @return
 *      - ESP_ERR_INVALID_ARG : ``wait`` is not set to portMAX_DELAY.
 *      - ESP_OK : Success.
 */
esp_err_t spi_device_acquire_bus(spi_device_handle_t device, TickType_t wait);

/**
 * @brief Release the SPI bus occupied by the device. All other devices can start sending transactions.
 *
 * @param dev The device to release the bus.
 */
void spi_device_release_bus(spi_device_handle_t dev);

/**
 * @brief Calculate the working frequency that is most close to desired frequency.
 *
 * @param fapb The frequency of apb clock, should be ``APB_CLK_FREQ``.
 * @param hz Desired working frequency
 * @param duty_cycle Duty cycle of the spi clock
 *
 * @return Actual working frequency that most fit.
 */
int spi_get_actual_clock(int fapb, int hz, int duty_cycle);

/**
  * @brief Calculate the timing settings of specified frequency and settings.
  *
  * @param gpio_is_used True if using GPIO matrix, or False if iomux pins are used.
  * @param input_delay_ns Input delay from SCLK launch edge to MISO data valid.
  * @param eff_clk Effective clock frequency (in Hz) from `spi_get_actual_clock()`.
  * @param dummy_o Address of dummy bits used output. Set to NULL if not needed.
  * @param cycles_remain_o Address of cycles remaining (after dummy bits are used) output.
  *         - -1 If too many cycles remaining, suggest to compensate half a clock.
  *         - 0 If no remaining cycles or dummy bits are not used.
  *         - positive value: cycles suggest to compensate.
  *
  * @note If **dummy_o* is not zero, it means dummy bits should be applied in half duplex mode, and full duplex mode may not work.
  */
void spi_get_timing(bool gpio_is_used, int input_delay_ns, int eff_clk, int *dummy_o, int *cycles_remain_o);

/**
  * @brief Get the frequency limit of current configurations.
  *         SPI master working at this limit is OK, while above the limit, full duplex mode and DMA will not work,
  *         and dummy bits will be aplied in the half duplex mode.
  *
  * @param gpio_is_used True if using GPIO matrix, or False if native pins are used.
  * @param input_delay_ns Input delay from SCLK launch edge to MISO data valid.
  * @return Frequency limit of current configurations.
  */
int spi_get_freq_limit(bool gpio_is_used, int input_delay_ns);

#ifdef __cplusplus
}
#endif
