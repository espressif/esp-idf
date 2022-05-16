/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifndef __cpp_exceptions
#error I2C class can only be used when __cpp_exceptions is enabled. Enable CONFIG_COMPILER_CXX_EXCEPTIONS in Kconfig
#endif

#include <exception>
#include <memory>
#include <chrono>
#include <vector>
#include <list>
#include <future>

#include "driver/i2c.h"
#include "esp_exception.hpp"

namespace idf {

struct I2CException : public ESPException {
    I2CException(esp_err_t error);
};

struct I2CTransferException : public I2CException {
    I2CTransferException(esp_err_t error);
};

/**
 * Superclass for all transfer objects which are accepted by \c I2CMaster::transfer().
 */
template<typename TReturn>
class I2CTransfer {
protected:
    /**
     * Wrapper around i2c_cmd_handle_t, makes it exception-safe.
     */
    struct I2CCommandLink {
        I2CCommandLink();
        ~I2CCommandLink();

        i2c_cmd_handle_t handle;
    };

public:
    /**
     * Helper typedef to facilitate type resolution during calls to I2CMaster::transfer().
     */
    typedef TReturn TransferReturnT;

    /**
     * @param driver_timeout The timeout used for calls like i2c_master_cmd_begin() to the underlying driver.
     */
    I2CTransfer(std::chrono::milliseconds driver_timeout = std::chrono::milliseconds(1000));

    virtual ~I2CTransfer() { }

    /**
     * Do all general parts of the I2C transfer:
     *  - initialize the command link
     *  - issuing a start to the command link queue
     *  - calling \c queue_cmd() in the subclass to issue specific commands to the command link queue
     *  - issuing a stop to the command link queue
     *  - executing the assembled commands on the I2C bus
     *  - calling \c process_result() to process the results of the commands or calling process_exception() if
     *    there was an exception
     *  - deleting the command link
     * This method is normally called by I2CMaster, but can also be used stand-alone if the bus corresponding to
     * \c i2c_num has be initialized.
     *
     * @throws I2CException for any particular I2C error
     */
    TReturn do_transfer(i2c_port_t i2c_num, uint8_t i2c_addr);

protected:
    /**
     * Implementation of the I2C command is implemented by subclasses.
     * The I2C command handle is initialized already at this stage.
     * The first action is issuing the I2C address and the read/write bit, depending on what the subclass implements.
     * On error, this  method has to throw an instance of I2CException.
     *
     * @param handle the initialized command handle of the I2C driver.
     * @param i2c_addr The slave's I2C address.
     *
     * @throw I2CException
     */
    virtual void queue_cmd(i2c_cmd_handle_t handle, uint8_t i2c_addr) = 0;

    /**
     * Implementation of whatever neccessary action after successfully sending the I2C command.
     * On error, this  method has to throw an instance of I2CException.
     *
     * @throw I2CException
     */
    virtual TReturn process_result() = 0;

    /**
     * For some calls to the underlying driver (e.g. \c i2c_master_cmd_begin() ), this general timeout will be passed.
     */
    const TickType_t driver_timeout;
};

/**
 * @brief Super class for any I2C master or slave
 */
class I2CBus {
public:
    /*
     * @brief Initialize I2C master bus.
     *
     * Initialize and install the bus driver in master mode.
     *
     * @param i2c_number The I2C port number.
     */
    I2CBus(i2c_port_t i2c_number);

    /**
     * @brief uninstall the bus driver.
     */
    virtual ~I2CBus();

    /**
     * The I2C port number.
     */
    const i2c_port_t i2c_num;
};

/**
 * @brief Simple I2C Master object
 *
 * This class provides to ways to issue I2C read and write requests. The simplest way is to use \c sync_write() and
 * sync_read() to write and read, respectively. As the name suggests, they block during the whole transfer.
 * For all asynchrounous transfers as well as combined write-read transfers, use \c transfer().
 */
class I2CMaster : public I2CBus {
public:
    /**
     * Initialize and install the driver of an I2C master peripheral.
     *
     * Initialize and install the bus driver in master mode. Pullups will be enabled for both pins. If you want a
     * different configuration, use configure() and i2c_set_pin() of the underlying driver to disable one or both
     * pullups.
     *
     * @param i2c_number The number of the I2C device.
     * @param scl_gpio GPIO number of the SCL line.
     * @param sda_gpio GPIO number of the SDA line.
     * @param clock_speed The master clock speed.
     * @param scl_pullup Enable SCL pullup.
     * @param sda_pullup Enable SDA pullup.
     *
     * @throws I2CException with the corrsponding esp_err_t return value if something goes wrong
     */
    I2CMaster(i2c_port_t i2c_number,
              int scl_gpio,
              int sda_gpio,
              uint32_t clock_speed,
              bool scl_pullup = true,
              bool sda_pullup = true);

    /**
     * Delete the driver.
     */
    virtual ~I2CMaster();

    /**
     * Issue an asynchronous I2C transfer which is executed in the background.
     *
     * This method uses a C++ \c std::future as mechanism to wait for the asynchronous return value.
     * The return value can be accessed with \c future::get(). \c future::get() also synchronizes with the thread
     * doing the work in the background, i.e. it waits until the return value has been issued.
     *
     * The actual implementation is delegated to the TransferT object. It will be given the I2C number to work
     * with.
     *
     * Requirements for TransferT: It should implement or imitate the interface of I2CTransfer.
     *
     * @param xfer The transfer to execute. What the transfer does, depends on it's implementation in
     *      \c TransferT::do_transfer(). It also determines the future template of this function, indicated by
     *      \c TransferT::TransferReturnT.
     *
     * @param i2c_addr The address of the I2C slave device targeted by the transfer.
     *
     * @return A future with \c TransferT::TransferReturnT. It depends on which template type is used for xfer.
     *         In case of a simple write (I2CWrite), it's future<void>.
     *         In case of a read (I2CRead), it's future<vector<uint8_t> > corresponding to the length of the read
     *         operation.
     *         If TransferT is a combined transfer with repeated reads (I2CComposed), then the return type is
     *         future<vector<vector<uint8_t> > >, a vector of results corresponding to the queued read operations.
     *
     * @throws I2CException with the corrsponding esp_err_t return value if something goes wrong
     * @throws std::exception for failures in libstdc++
     */
    template<typename TransferT>
    std::future<typename TransferT::TransferReturnT> transfer(std::shared_ptr<TransferT> xfer, uint8_t i2c_addr);

    /**
     * Do a synchronous write.
     *
     * All data in data will be written to the I2C device with i2c_addr at once.
     * This method will block until the I2C write is complete.
     *
     * @param i2c_addr The address of the I2C device to which the data shall be sent.
     * @param data The data to send (size to be sent is determined by data.size()).
     *
     * @throws I2CException with the corrsponding esp_err_t return value if something goes wrong
     * @throws std::exception for failures in libstdc++
     */
    void sync_write(uint8_t i2c_addr, const std::vector<uint8_t> &data);

    /**
     * Do a synchronous read.
     * This method will block until the I2C read is complete.
     *
     * n_bytes bytes of data will be read from the I2C device with i2c_addr.
     * While reading the last byte, the master finishes the reading by sending a NACK, before issuing a stop.
     *
     * @param i2c_addr The address of the I2C device from which to read.
     * @param n_bytes The number of bytes to read.
     *
     * @return the read bytes
     *
     * @throws I2CException with the corrsponding esp_err_t return value if something goes wrong
     * @throws std::exception for failures in libstdc++
     */
    std::vector<uint8_t> sync_read(uint8_t i2c_addr, size_t n_bytes);

    /**
     * Do a simple asynchronous write-read transfer.
     *
     * First, \c write_data will be written to the bus, then a number of \c read_n_bytes will be read from the bus
     * with a repeated start condition. The slave device is determined by \c i2c_addr.
     * While reading the last byte, the master finishes the reading by sending a NACK, before issuing a stop.
     * This method will block until the I2C transfer is complete.
     *
     * @param i2c_addr The address of the I2C device from which to read.
     * @param write_data The data to write to the bus before reading.
     * @param read_n_bytes The number of bytes to read.
     *
     * @return the read bytes
     *
     * @throws I2CException with the corrsponding esp_err_t return value if something goes wrong
     * @throws std::exception for failures in libstdc++
     */
    std::vector<uint8_t> sync_transfer(uint8_t i2c_addr,
            const std::vector<uint8_t> &write_data,
            size_t read_n_bytes);
};

/**
 * @brief Responsible for initialization and de-initialization of an I2C slave peripheral.
 */
class I2CSlave : public I2CBus {
public:
    /**
     * Initialize and install the driver of an I2C slave peripheral.
     *
     * Initialize and install the bus driver in slave mode. Pullups will be enabled for both pins. If you want a
     * different configuration, use configure() and i2c_set_pin() of the underlying driver to disable one or both
     * pullups.
     *
     * @param i2c_number The number of the I2C device.
     * @param scl_gpio GPIO number of the SCL line.
     * @param sda_gpio GPIO number of the SDA line.
     * @param slave_addr The address of the slave device on the I2C bus.
     * @param rx_buf_len Receive buffer length.
     * @param tx_buf_len Transmit buffer length.
     * @param scl_pullup Enable SCL pullup.
     * @param sda_pullup Enable SDA pullup.
     *
     * @throws
     */
    I2CSlave(i2c_port_t i2c_number,
        int scl_gpio,
        int sda_gpio,
        uint8_t slave_addr,
        size_t rx_buf_len,
        size_t tx_buf_len,
        bool scl_pullup = true,
        bool sda_pullup = true);

    /**
     * Delete the driver.
     */
    virtual ~I2CSlave();

    /**
     * Schedule a raw data write once master is ready.
     *
     * The data is saved in a buffer, waiting for the master to pick it up.
     */
    virtual int write_raw(const uint8_t* data, size_t data_len, std::chrono::milliseconds timeout);

    /**
     * Read raw data from the bus.
     *
     * The data is read directly from the buffer. Hence, it has to be written already by master.
     */
    virtual int read_raw(uint8_t* buffer, size_t buffer_len, std::chrono::milliseconds timeout);
};

/**
 * Implementation for simple I2C writes, which can be executed by \c I2CMaster::transfer().
 * It stores the bytes to be written as a vector.
 */
class I2CWrite : public I2CTransfer<void> {
public:
    /**
     * @param bytes The bytes which should be written.
     * @param driver_timeout The timeout used for calls like i2c_master_cmd_begin() to the underlying driver.
     */
    I2CWrite(const std::vector<uint8_t> &bytes, std::chrono::milliseconds driver_timeout = std::chrono::milliseconds(1000));

protected:
    /**
     * Write the address and set the read bit to 0 to issue the address and request a write.
     * Then write the bytes.
     *
     * @param handle The initialized I2C command handle.
     * @param i2c_addr The I2C address of the slave.
     */
    void queue_cmd(i2c_cmd_handle_t handle, uint8_t i2c_addr) override;

    /**
     * Set the value of the promise to unblock any callers waiting on it.
     */
    void process_result() override;

private:
    /**
     * The bytes to write.
     */
    std::vector<uint8_t> bytes;
};

/**
 * Implementation for simple I2C reads, which can be executed by \c I2CMaster::transfer().
 * It stores the bytes to be read as a vector to be returned later via a future.
 */
class I2CRead : public I2CTransfer<std::vector<uint8_t> > {
public:
    /**
     * @param The number of bytes to read.
     * @param driver_timeout The timeout used for calls like i2c_master_cmd_begin() to the underlying driver.
     */
    I2CRead(size_t size, std::chrono::milliseconds driver_timeout = std::chrono::milliseconds(1000));

protected:
    /**
     * Write the address and set the read bit to 1 to issue the address and request a read.
     * Then read into bytes.
     *
     * @param handle The initialized I2C command handle.
     * @param i2c_addr The I2C address of the slave.
     */
    void queue_cmd(i2c_cmd_handle_t handle, uint8_t i2c_addr) override;

    /**
     * Set the return value of the promise to unblock any callers waiting on it.
     */
    std::vector<uint8_t> process_result() override;

private:
    /**
     * The bytes to read.
     */
    std::vector<uint8_t> bytes;
};

/**
 * This kind of transfer uses repeated start conditions to chain transfers coherently.
 * In particular, this can be used to chain multiple single write and read transfers into a single transfer with
 * repeated starts as it is commonly done for I2C devices.
 * The result is a vector of vectors representing the reads in the order of how they were added using add_read().
 */
class I2CComposed : public  I2CTransfer<std::vector<std::vector<uint8_t> > > {
public:
    I2CComposed(std::chrono::milliseconds driver_timeout = std::chrono::milliseconds(1000));

    /**
     * Add a read to the chain.
     *
     * @param size The size of the read in bytes.
     */
    void add_read(size_t size);

    /**
     * Add a write to the chain.
     *
     * @param bytes The bytes to write; size will be bytes.size()
     */
    void add_write(std::vector<uint8_t> bytes);

protected:
    /**
     * Write all chained transfers, including a repeated start issue after each but the last transfer.
     *
     * @param handle The initialized I2C command handle.
     * @param i2c_addr The I2C address of the slave.
     */
    void queue_cmd(i2c_cmd_handle_t handle, uint8_t i2c_addr) override;

    /**
     * Creates the vector with the vectors from all reads.
     */
    std::vector<std::vector<uint8_t> > process_result() override;

private:
    class CompTransferNode {
    public:
        virtual ~CompTransferNode() { }
        virtual void queue_cmd(i2c_cmd_handle_t handle, uint8_t i2c_addr) = 0;
        virtual void process_result(std::vector<std::vector<uint8_t> > &read_results) { }
    };

    class CompTransferNodeRead : public CompTransferNode {
    public:
        CompTransferNodeRead(size_t size) : bytes(size) { }
        void queue_cmd(i2c_cmd_handle_t handle, uint8_t i2c_addr) override;

        void process_result(std::vector<std::vector<uint8_t> > &read_results) override;
    private:
        std::vector<uint8_t> bytes;
    };

    class CompTransferNodeWrite : public CompTransferNode {
    public:
        CompTransferNodeWrite(std::vector<uint8_t> bytes) : bytes(bytes) { }
        void queue_cmd(i2c_cmd_handle_t handle, uint8_t i2c_addr) override;
    private:
        std::vector<uint8_t> bytes;
    };

    /**
     * The chained transfers.
     */
    std::list<std::shared_ptr<CompTransferNode> > transfer_list;
};

template<typename TReturn>
I2CTransfer<TReturn>::I2CTransfer(std::chrono::milliseconds driver_timeout)
        : driver_timeout(driver_timeout.count()) { }

template<typename TReturn>
I2CTransfer<TReturn>::I2CCommandLink::I2CCommandLink()
{
    handle = i2c_cmd_link_create();
    if (!handle) {
        throw I2CException(ESP_ERR_NO_MEM);
    }
}

template<typename TReturn>
I2CTransfer<TReturn>::I2CCommandLink::~I2CCommandLink()
{
    i2c_cmd_link_delete(handle);
}

template<typename TReturn>
TReturn I2CTransfer<TReturn>::do_transfer(i2c_port_t i2c_num, uint8_t i2c_addr)
{
    I2CCommandLink cmd_link;

    queue_cmd(cmd_link.handle, i2c_addr);

    CHECK_THROW_SPECIFIC(i2c_master_stop(cmd_link.handle), I2CException);

    CHECK_THROW_SPECIFIC(i2c_master_cmd_begin(i2c_num, cmd_link.handle, driver_timeout / portTICK_RATE_MS), I2CTransferException);

    return process_result();
}

template<typename TransferT>
std::future<typename TransferT::TransferReturnT> I2CMaster::transfer(std::shared_ptr<TransferT> xfer, uint8_t i2c_addr)
{
    if (!xfer) throw I2CException(ESP_ERR_INVALID_ARG);

    return std::async(std::launch::async, [this](std::shared_ptr<TransferT> xfer, uint8_t i2c_addr) {
        return xfer->do_transfer(i2c_num, i2c_addr);
    }, xfer, i2c_addr);
}

} // idf
