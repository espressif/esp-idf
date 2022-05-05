/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
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

#include "sdkconfig.h"
#include "esp_exception.hpp"
#include "system_cxx.hpp"
#include "gpio_cxx.hpp"

namespace idf {

/**
 * @brief Check if the provided numerical value is a valid I2C address.
 *
 * @param addr raw number to be checked.
 * @return ESP_OK if \c addr is a valid I2C address, otherwise ESP_ERR_INVALID_ARG.
 */
esp_err_t check_i2c_addr(uint32_t addr) noexcept;

struct I2CException : public ESPException {
    I2CException(esp_err_t error);
};

struct I2CTransferException : public I2CException {
    I2CTransferException(esp_err_t error);
};

/**
 * @brief Represents a valid SDA signal pin number.
 */
class SDA_type;
using SDA_GPIO = GPIONumBase<class SDA_type>;

/**
 * @brief Represents a valid SCL signal pin number.
 */
class SCL_type;
using SCL_GPIO = GPIONumBase<class SCL_type>;

/**
 * @brief Valid representation of I2C number.
 *
 * A chip can have multiple I2C interfaces, each identified by a bus number, subsequently called I2C number.
 * Instances of this class are guaranteed to always contain a valid I2C number.
 */
class I2CNumber : public StrongValueComparable<uint32_t> {
    /**
     * Construct a valid representation of the I2C number.
     *
     * This constructor is private because the it can only be accessed but the static creation methods below.
     * This guarantees that an instance of I2CNumber always carries a valid number.
     */
    constexpr explicit I2CNumber(uint32_t number) : StrongValueComparable<uint32_t>(number) { }

public:
    /**
     * @brief create an I2C number representing the first I2C bus of the chip.
     */
    constexpr static I2CNumber I2C0() {
        return I2CNumber(0);
    }

#if CONFIG_SOC_I2C_NUM == 2
    /**
     * @brief create an I2C number representing the second I2C bus of the chip.
     */
    constexpr static I2CNumber I2C1() {
        return I2CNumber(1);
    }
#endif

    /**
     * Retrieves the valid numerical representation of the I2C number.
     */
    uint32_t get_num();
};

/**
 * @brief Valid representation of I2C address.
 *
 * Instances of this class are guaranteed to always contain a valid I2C address.
 */
class I2CAddress : public StrongValueComparable<uint8_t> {
public:
    /**
     *
     */
    explicit I2CAddress(uint8_t addr);

    /**
     * Retrieves the valid numerical representation of the I2C adress.
     */
    uint8_t get_addr();
};

/**
 * @brief Low-level I2C transaction descriptor
 *
 * This class records and decribes a low-level transaction. Users use the methods (except \c execute_transfer)
 * to record the transaction. Afterwards, the transaction will be executed by calling \c execute_transfer,
 * which blocks until the transaction is finished.
 *
 * @note This is a low-level class, which only exists due to the underlying I2C driver. All data referenced in
 *      read and write calls must not be changed and must stay allocated until at least \c execute_transfer
 *      has finished.
 */
class I2CCommandLink {
public:
    /**
     * @brief Allocate and create the transaction descriptor.
     */
    I2CCommandLink();

    /**
     * @brief Delete the transaction descriptor, de-allocate all resources.
     */
    ~I2CCommandLink();

    I2CCommandLink(const I2CCommandLink&) = delete;
    I2CCommandLink operator=(const I2CCommandLink&) = delete;

    /**
     * @brief Record a start signal on the I2C bus.
     */
    void start();

    /**
     * @brief Record a write of the vector \c bytes on the I2C bus.
     *
     * @param[in] bytes The data to be written. Must stay allocated until execute_transfer has finished or
     *          destructor of this class has been called.
     * @param[in] expect_ack If acknowledgement shall be requested after each written byte, pass true,
     *          otherwise false.
     */
    void write(const std::vector<uint8_t> &bytes, bool expect_ack = true);

    /**
     * @brief Record a one-byte-write on the I2C bus.
     *
     * @param[in] byte The data to be written. No restrictions apply.
     * @param[in] expect_ack If acknowledgement shall be requested after writing the byte, pass true,
     *          otherwise false.
     */
    void write_byte(uint8_t byte, bool expect_ack = true);

    /**
     * @brief Record a read of the size of vector \c bytes on the I2C bus.
     *
     * @param[in] bytes Vector with the size of the data to be read (in bytes). Must stay allocated until
     *          execute_transfer has finished or destructor of this class has been called.
     * @param[in] expect_ack If acknowledgement shall be requested after each written byte, pass true,
     *          otherwise false.
     */
    void read(std::vector<uint8_t> &bytes);

    /**
     * @brief Record a stop command on the I2C bus.
     */
    void stop();

    /**
     * @brief Execute the transaction and wait until it has finished.
     *
     * This method will issue the transaction with the operations in the order in which they have been recorded
     * before.
     *
     * @param i2c_num I2C bus number on the chip.
     * @param driver_timeout Timeout for this transaction.
     */
    void execute_transfer(I2CNumber i2c_num, std::chrono::milliseconds driver_timeout);

private:
    /**
     * @brief Internal driver data.
     */
    void *handle;
};

/**
 * Superclass for all transfer objects which are accepted by \c I2CMaster::transfer().
 */
template<typename TReturn>
class I2CTransfer {
public:
    /**
     * Helper typedef to facilitate type resolution during calls to I2CMaster::transfer().
     */
    typedef TReturn TransferReturnT;

    /**
     * @param driver_timeout The timeout used for calls like i2c_master_cmd_begin() to the underlying driver.
     */
    I2CTransfer(std::chrono::milliseconds driver_timeout_arg = std::chrono::milliseconds(1000));

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
    TReturn do_transfer(I2CNumber i2c_num, I2CAddress i2c_addr);

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
    virtual void queue_cmd(I2CCommandLink &handle, I2CAddress i2c_addr) = 0;

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
    std::chrono::milliseconds driver_timeout;
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
    explicit I2CBus(I2CNumber i2c_number);

    /**
     * @brief uninstall the bus driver.
     */
    virtual ~I2CBus();

    /**
     * The I2C port number.
     */
    const I2CNumber i2c_num;
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
    explicit I2CMaster(I2CNumber i2c_number,
              SCL_GPIO scl_gpio,
              SDA_GPIO sda_gpio,
              Frequency clock_speed,
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
    std::future<typename TransferT::TransferReturnT> transfer(I2CAddress i2c_addr, std::shared_ptr<TransferT> xfer);

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
    void sync_write(I2CAddress i2c_addr, const std::vector<uint8_t> &data);

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
    std::vector<uint8_t> sync_read(I2CAddress i2c_addr, size_t n_bytes);

    /**
     * Do a simple synchronous write-read transfer.
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
    std::vector<uint8_t> sync_transfer(I2CAddress i2c_addr,
            const std::vector<uint8_t> &write_data,
            size_t read_n_bytes);
};

#if CONFIG_SOC_I2C_SUPPORT_SLAVE
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
    I2CSlave(I2CNumber i2c_number,
        SCL_GPIO scl_gpio,
        SDA_GPIO sda_gpio,
        I2CAddress slave_addr,
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
#endif // CONFIG_SOC_I2C_SUPPORT_SLAVE

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
    void queue_cmd(I2CCommandLink &handle, I2CAddress i2c_addr) override;

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
    void queue_cmd(I2CCommandLink &handle, I2CAddress i2c_addr) override;

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
    void queue_cmd(I2CCommandLink &handle, I2CAddress i2c_addr) override;

    /**
     * Creates the vector with the vectors from all reads.
     */
    std::vector<std::vector<uint8_t> > process_result() override;

private:
    class CompTransferNode {
    public:
        virtual ~CompTransferNode() { }
        virtual void queue_cmd(I2CCommandLink &handle, I2CAddress i2c_addr) = 0;
        virtual void process_result(std::vector<std::vector<uint8_t> > &read_results) { }
    };

    class CompTransferNodeRead : public CompTransferNode {
    public:
        CompTransferNodeRead(size_t size) : bytes(size) { }
        void queue_cmd(I2CCommandLink &handle, I2CAddress i2c_addr) override;

        void process_result(std::vector<std::vector<uint8_t> > &read_results) override;
    private:
        std::vector<uint8_t> bytes;
    };

    class CompTransferNodeWrite : public CompTransferNode {
    public:
        CompTransferNodeWrite(std::vector<uint8_t> bytes) : bytes(bytes) { }
        void queue_cmd(I2CCommandLink &handle, I2CAddress i2c_addr) override;
    private:
        std::vector<uint8_t> bytes;
    };

    /**
     * The chained transfers.
     */
    std::list<std::shared_ptr<CompTransferNode> > transfer_list;
};

template<typename TReturn>
I2CTransfer<TReturn>::I2CTransfer(std::chrono::milliseconds driver_timeout_arg)
        : driver_timeout(driver_timeout_arg) { }

template<typename TReturn>
TReturn I2CTransfer<TReturn>::do_transfer(I2CNumber i2c_num, I2CAddress i2c_addr)
{
    I2CCommandLink cmd_link;

    queue_cmd(cmd_link, i2c_addr);

    cmd_link.stop();

    cmd_link.execute_transfer(i2c_num, driver_timeout);

    return process_result();
}

template<typename TransferT>
std::future<typename TransferT::TransferReturnT> I2CMaster::transfer(I2CAddress i2c_addr, std::shared_ptr<TransferT> xfer)
{
    if (!xfer) throw I2CException(ESP_ERR_INVALID_ARG);

    return std::async(std::launch::async, [this](std::shared_ptr<TransferT> xfer, I2CAddress i2c_addr) {
        return xfer->do_transfer(i2c_num, i2c_addr);
    }, xfer, i2c_addr);
}

} // idf
