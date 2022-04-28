/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#if __cpp_exceptions

#include <exception>
#include <memory>
#include <chrono>
#include <vector>
#include <list>
#include <future>

#include "system_cxx.hpp"
#include "spi_cxx.hpp"

namespace idf {

/**
 * @brief Exception which is thrown in the context of SPI Transactions.
 */
struct SPITransferException : public SPIException {
    SPITransferException(esp_err_t error);
};

class SPIDevice;
class SPIDeviceHandle;

/**
 * @brief Describes and encapsulates the transaction.
 *
 * @note This class is intended to be used internally by the SPI C++ classes, but not publicly.
 *      Furthermore, currently only one transaction per time can be handled. If you need to
 *      send several transactions in parallel, you need to build your own mechanism around a
 *      FreeRTOS task and a queue.
 */
class SPITransactionDescriptor {
    friend class SPIDeviceHandle;
public:
    /**
     * @brief Create a SPITransactionDescriptor object, describing a full duplex transaction.
     *
     * @param data_to_send The data sent to the SPI device. It can be dummy data if a read-only
     *      transaction is intended. Its length determines the length of both write and read operation.
     * @param handle to the internal driver handle
     * @param pre_callback If non-empty, this callback will be called directly before the transaction.
     * @param post_callback If non-empty, this callback will be called directly after the transaction.
     * @param user_data optional data which will be accessible in the callbacks declared above
     */
    SPITransactionDescriptor(const std::vector<uint8_t> &data_to_send,
            SPIDeviceHandle *handle,
            std::function<void(void *)> pre_callback = nullptr,
            std::function<void(void *)> post_callback = nullptr,
            void* user_data = nullptr);

    /**
     * @brief Deinitialize and delete all data of the transaction.
     *
     * @note This destructor must not becalled before the transaction is finished by the driver.
     */
    ~SPITransactionDescriptor();

    SPITransactionDescriptor(const SPITransactionDescriptor&) = delete;
    SPITransactionDescriptor operator=(const SPITransactionDescriptor&) = delete;

    /**
     * @brief Queue the transaction asynchronously.
     */
    void start();

    /**
     * @brief Synchronously (blocking) wait for the result and return the result data or throw an exception.
     *
     * @return The data read from the SPI device. Its length is the length of \c data_to_send passed in the
     *      constructor.
     * @throws SPIException in case of an error of the underlying driver or if the driver returns a wrong
     *      transaction descriptor for some reason. In the former case, the error code is the one from the
     *      underlying driver, in the latter case, the error code is ESP_ERR_INVALID_STATE.
     */
    std::vector<uint8_t> get();

    /**
     * @brief Wait until the asynchronous operation is done.
     *
     * @throws SPIException in case of an error of the underlying driver or if the driver returns a wrong
     *      transaction descriptor for some reason. In the former case, the error code is the one from the
     *      underlying driver, in the latter case, the error code is ESP_ERR_INVALID_STATE.
     */
    void wait();


    /**
     * @brief Wait for a result of the transaction up to timeout ms.
     *
     * @param timeout Maximum timeout value for waiting
     *
     * @return true if result is available, false if wait timed out
     *
     * @throws SPIException in case of an error of the underlying driver or if the driver returns a wrong
     *      transaction descriptor for some reason. In the former case, the error code is the one from the
     *      underlying driver, in the latter case, the error code is ESP_ERR_INVALID_STATE.
     */
    bool wait_for(const std::chrono::milliseconds &timeout);

private:
    /**
     * Private descriptor data.
     */
    void *private_transaction_desc;

    /**
     * Private device data.
     */
    SPIDeviceHandle *device_handle;

    /**
     * @brief If non-empty, this callback will be called directly before the transaction.
     */
    std::function<void(void *)> pre_callback;

    /**
     * @brief If non-empty, this callback will be called directly after the transaction.
     */
    std::function<void(void *)> post_callback;

    /**
     * Buffer in spi_transaction_t is const, so we have to declare it here because we want to
     * allocate and delete it.
     */
    uint8_t *tx_buffer;

    /**
     * @brief User data which will be provided in the callbacks.
     */
    void *user_data;

    /**
     * Tells if data has been received, i.e. the transaction has finished and the result can be acquired.
     */
    bool received_data;

    /**
     * Tells if the transaction has been initiated and is at least in-flight, if not finished.
     */
    bool started;
};

/**
 * @brief SPIFuture for asynchronous transaction results, mostly equivalent to std::future.
 *
 * This re-implementation is necessary as std::future is incompatible with the IDF SPI driver interface.
 */
class SPIFuture {
public:
    /**
     * @brief Create an invalid future.
     */
    SPIFuture();

    /**
     * @brief Create a valid future with \c transaction as shared state.
     *
     * @param transaction the shared transaction state
     */
    SPIFuture(std::shared_ptr<SPITransactionDescriptor> transaction);

    SPIFuture(const SPIFuture &other) = delete;

    /**
     * @brief Move constructor as in std::future, leaves \c other invalid.
     *
     * @param other object to move from, will become invalid during this constructor
     */
    SPIFuture(SPIFuture &&other) noexcept;

    /**
     * @brief Move assignment as in std::future, leaves \c other invalid.
     *
     * @param other object to move from, will become invalid during this constructor
     * @return A reference to the newly created SPIFuture object
     */
    SPIFuture &operator=(SPIFuture&& other) noexcept;

    /**
     * @brief Wait until the asynchronous operation is done and return the result or throw and exception.
     *
     * @throws std::future_error if this future is not valid.
     * @throws SPIException in case of an error of the underlying driver or if the driver returns a wrong
     *      transaction descriptor for some reason. In the former case, the error code is the one from the
     *      underlying driver, in the latter case, the error code is ESP_ERR_INVALID_STATE.
     * @return The result of the asynchronous SPI transaction.
     */
    std::vector<uint8_t> get();

    /**
     * @brief Wait for a result up to timeout ms.
     *
     * @param timeout Maximum timeout value for waiting
     *
     * @return std::future_status::ready if result is available, std::future_status::timeout if wait timed out
     */
    std::future_status wait_for(std::chrono::milliseconds timeout);

    /**
     * @brief Wait for a result indefinitely.
     */
    void wait();

    /**
     * @return true if this future is valid, otherwise false.
     */
    bool valid() const noexcept;

private:
    /**
     * The SPITransactionDescriptor, which is the shared state of this future.
     */
    std::shared_ptr<SPITransactionDescriptor> transaction;

    /**
     * Indicates if this future is valid.
     */
    bool is_valid;
};

/**
 * @brief Represents an device on an initialized Master Bus.
 */
class SPIDevice {
public:
    /**
     * @brief Create and initialize a device on the master bus corresponding to spi_host.
     *
     * @param cs The pin number of the chip select signal for the device to create.
     * @param spi_host the spi_host (bus) to which the device shall be attached.
     * @param frequency The devices frequency. this frequency will be set during transactions to the device which will be
     *      created.
     * @param transaction_queue_size The of the transaction queue of this device. This determines how many
     *      transactions can be queued at the same time. Currently, it is set to 1 since the
     *      implementation exclusively acquires the bus for each transaction. This may change in the future.
     */
    SPIDevice(SPINum spi_host,
            CS cs,
            Frequency frequency = Frequency::MHz(1),
            QueueSize transaction_queue_size = QueueSize(1u));

    SPIDevice(const SPIDevice&) = delete;
    SPIDevice operator=(const SPIDevice&) = delete;

    /**
     * @brief De-initializes and destroys the device.
     *
     * @warning Behavior is undefined if a device is destroyed while there is still an ongoing transaction
     *      from that device.
     */
    ~SPIDevice();

    /**
     * @brief Queue a transfer to this device.
     *
     * This method creates a full-duplex transfer to the device represented by the current instance of this class.
     * It then queues that transfer and returns a "future" object. The future object will become ready once
     * the transfer finishes.
     *
     * @param data_to_send Data which will be sent to the device. The length of the data determines the length
     *      of the full-deplex transfer. I.e., the same amount of bytes will be received from the device.
     * @param pre_callback If non-empty, this callback will be called directly before the transaction.
     *      If empty, it will be ignored.
     * @param post_callback If non-empty, this callback will be called directly after the transaction.
     *      If empty, it will be ignored.
     * @param user_data This pointer will be sent to pre_callback and/or pre_callback, if any of them is non-empty.
     *
     * @return a future object which will become ready once the transfer has finished. See also \c SPIFuture.
     */
    SPIFuture transfer(const std::vector<uint8_t> &data_to_send,
            std::function<void(void *)> pre_callback = nullptr,
            std::function<void(void *)> post_callback = nullptr,
            void* user_data = nullptr);

    /**
     * @brief Queue a transfer to this device like \c transfer, but using begin/end iterators instead of a
     *      data vector.
     *
     * This method is equivalent to \c transfer(), except for the parameters.
     *
     * @param begin Iterator to the begin of the data which will be sent to the device.
     * @param end Iterator to the end of the data which will be sent to the device.
     *      This iterator determines the length of the data and hence the length  of the full-deplex transfer.
     *      I.e., the same amount of bytes will be received from the device.
     * @param pre_callback If non-empty, this callback will be called directly before the transaction.
     *      If empty, it will be ignored.
     * @param post_callback If non-empty, this callback will be called directly after the transaction.
     *      If empty, it will be ignored.
     * @param user_data This pointer will be sent to pre_callback and/or pre_callback, if any of them is non-empty.
     *
     * @return a future object which will become ready once the transfer has finished. See also \c SPIFuture.
     */
    template<typename IteratorT>
    SPIFuture transfer(IteratorT begin,
            IteratorT end,
            std::function<void(void *)> pre_callback = nullptr,
            std::function<void(void *)> post_callback = nullptr,
            void* user_data = nullptr);

private:
    /**
     * Private device data.
     */
    SPIDeviceHandle *device_handle;

    /**
     * Saves the current transaction descriptor in case the user's loses its future with the other
     * reference to the transaction.
     */
    std::shared_ptr<SPITransactionDescriptor> current_transaction;
};

/**
 * @brief Represents an SPI Master Bus.
 */
class SPIMaster {
public:
    /*
     * @brief Create an SPI Master Bus.
     *
     * @param host The SPI host (bus) which should be used. ESP chips have a number of different possible SPI hosts,
     *      each of which will create its own bus. Consult the datasheet and TRM on which host to choose.
     * @param mosi The pin number for the MOSI signal of this bus.
     * @param miso The pin number for the MISO signal of this bus.
     * @param sclk The pin number for the clock signal of this bus.
     * @param dma_config The DMA configuration for this bus, see \c DMAConfig.
     * @param max_transfer_size The maximum transfer size in bytes.
     *
     * @throws SPIException with IDF error code if the underlying driver fails.
     */
    explicit SPIMaster(SPINum host,
            const MOSI &mosi,
            const MISO &miso,
            const SCLK &sclk,
            SPI_DMAConfig dma_config = SPI_DMAConfig::AUTO(),
            SPITransferSize max_transfer_size = SPITransferSize::default_size());

    /*
     * @brief Create an SPI Master Bus.
     *
     * @param host The SPI host (bus) which should be used. ESP chips have a number of different possible SPI hosts,
     *      each of which will create its own bus. Consult the datasheet and TRM on which host to choose.
     * @param mosi The pin number for the MOSI signal of this bus.
     * @param miso The pin number for the MISO signal of this bus.
     * @param sclk The pin number for the clock signal of this bus.
     * @param qspiwp The pin number for the QSPIWP signal of this bus.
     * @param qspihd The pin number for the QSPIHD signal of this bus.
     * @param dma_config The DMA configuration for this bus, see \c DMAConfig.
     * @param max_transfer_size The maximum transfer size in bytes.
     *
     * @throws SPIException with IDF error code if the underlying driver fails.
     */
    explicit SPIMaster(SPINum host,
            const MOSI &mosi,
            const MISO &miso,
            const SCLK &sclk,
            const QSPIWP &qspiwp,
            const QSPIHD &qspihd,
            SPI_DMAConfig dma_config = SPI_DMAConfig::AUTO(),
            SPITransferSize max_transfer_size = SPITransferSize::default_size());

    SPIMaster(const SPIMaster&) = delete;
    SPIMaster operator=(const SPIMaster&) = delete;

    SPIMaster(SPIMaster&&) = default;
    SPIMaster &operator=(SPIMaster&&) = default;

    /*
     * @brief De-initializes and destroys the SPI Master Bus.
     *
     * @note Devices created before which try to initialize an exception after the bus is destroyed will throw
     *      and exception.
     */
    virtual ~SPIMaster();

    /**
     * @brief Create a representation of a device on this bus.
     *
     * @param cs The pin number for the CS (chip select) signal to talk to the device.
     * @param f The frequency used to talk to the device.
     */
    std::shared_ptr<SPIDevice> create_dev(CS cs, Frequency frequency = Frequency::MHz(1));

private:
    /**
     * @brief Host identifier for internal use.
     */
    SPINum spi_host;
};

template<typename IteratorT>
SPIFuture SPIDevice::transfer(IteratorT begin,
        IteratorT end,
        std::function<void(void *)> pre_callback,
        std::function<void(void *)> post_callback,
        void* user_data)
{
    std::vector<uint8_t> write_data;
    write_data.assign(begin, end);
    return transfer(write_data, pre_callback, post_callback, user_data);
}

}

#endif
