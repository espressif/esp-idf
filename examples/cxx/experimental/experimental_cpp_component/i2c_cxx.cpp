/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef __cpp_exceptions

#include "driver/i2c.h"
#include "i2c_cxx.hpp"

using namespace std;

namespace idf {

#define I2C_CHECK_THROW(err) CHECK_THROW_SPECIFIC((err), I2CException)

esp_err_t check_i2c_num(uint32_t i2c_num) noexcept
{
    if (i2c_num >= I2C_NUM_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

esp_err_t check_i2c_addr(uint32_t addr) noexcept
{
    // maximum I2C address currently supported in the C++ classes is 127
    if (addr > 0x7f) {
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

I2CException::I2CException(esp_err_t error) : ESPException(error) { }

I2CTransferException::I2CTransferException(esp_err_t error) : I2CException(error) { }

uint32_t I2CNumber::get_num()
{
    return get_value();
}

I2CAddress::I2CAddress(uint8_t addr) : StrongValueComparable<uint8_t> (addr)
{
    esp_err_t error = check_i2c_addr(addr);
    if (error != ESP_OK) {
        throw I2CException(error);
    }
}

uint8_t I2CAddress::get_addr()
{
    return get_value();
}

I2CCommandLink::I2CCommandLink()
{
    handle = i2c_cmd_link_create();
    if (!handle) {
        throw I2CException(ESP_ERR_NO_MEM);
    }
}

I2CCommandLink::~I2CCommandLink()
{
    i2c_cmd_link_delete(handle);
}

void I2CCommandLink::start()
{
    I2C_CHECK_THROW(i2c_master_start(handle));
}

void I2CCommandLink::write(const std::vector<uint8_t> &bytes, bool expect_ack)
{
    I2C_CHECK_THROW(i2c_master_write(handle, bytes.data(), bytes.size(), expect_ack));
}

void I2CCommandLink::write_byte(uint8_t byte, bool expect_ack)
{
    I2C_CHECK_THROW(i2c_master_write_byte(handle, byte, expect_ack));
}

void I2CCommandLink::read(std::vector<uint8_t> &bytes)
{
    I2C_CHECK_THROW(i2c_master_read(handle, bytes.data(), bytes.size(), I2C_MASTER_LAST_NACK));
}

void I2CCommandLink::stop()
{
    I2C_CHECK_THROW(i2c_master_stop(handle));
}

void I2CCommandLink::execute_transfer(I2CNumber i2c_num, chrono::milliseconds driver_timeout)
{
    esp_err_t err = i2c_master_cmd_begin(i2c_num.get_num(), handle, driver_timeout.count() / portTICK_PERIOD_MS);
    if (err != ESP_OK) {
        throw I2CTransferException(err);
    }
}

I2CBus::I2CBus(I2CNumber i2c_number) : i2c_num(std::move(i2c_number)) { }

I2CBus::~I2CBus() { }

I2CMaster::I2CMaster(I2CNumber i2c_number,
                     SCL_GPIO scl_gpio,
                     SDA_GPIO sda_gpio,
                     Frequency clock_speed,
                     bool scl_pullup,
                     bool sda_pullup)
    : I2CBus(std::move(i2c_number))
{
    i2c_config_t conf = {};
    conf.mode = I2C_MODE_MASTER;
    conf.scl_io_num = scl_gpio.get_num();
    conf.scl_pullup_en = scl_pullup;
    conf.sda_io_num = sda_gpio.get_num();
    conf.sda_pullup_en = sda_pullup;
    conf.master.clk_speed = clock_speed.get_value();
    I2C_CHECK_THROW(i2c_param_config(i2c_num.get_value(), &conf));
    I2C_CHECK_THROW(i2c_driver_install(i2c_num.get_value(), conf.mode, 0, 0, 0));
}

I2CMaster::~I2CMaster()
{
    i2c_driver_delete(i2c_num.get_value());
}

void I2CMaster::sync_write(I2CAddress i2c_addr, const vector<uint8_t> &data)
{
    I2CWrite writer(data);

    writer.do_transfer(i2c_num, i2c_addr);
}

std::vector<uint8_t> I2CMaster::sync_read(I2CAddress i2c_addr, size_t n_bytes)
{
    I2CRead reader(n_bytes);

    return reader.do_transfer(i2c_num, i2c_addr);
}

vector<uint8_t> I2CMaster::sync_transfer(I2CAddress i2c_addr,
        const std::vector<uint8_t> &write_data,
        size_t read_n_bytes)
{
    I2CComposed composed_transfer;
    composed_transfer.add_write(write_data);
    composed_transfer.add_read(read_n_bytes);

    return composed_transfer.do_transfer(i2c_num, i2c_addr)[0];
}

#if CONFIG_SOC_I2C_SUPPORT_SLAVE
I2CSlave::I2CSlave(I2CNumber i2c_number,
        SCL_GPIO scl_gpio,
        SDA_GPIO sda_gpio,
        I2CAddress slave_addr,
        size_t rx_buf_len,
        size_t tx_buf_len,
        bool scl_pullup,
        bool sda_pullup)
    : I2CBus(std::move(i2c_number))
{
    i2c_config_t conf = {};
    conf.mode = I2C_MODE_SLAVE;
    conf.scl_io_num = scl_gpio.get_value();
    conf.scl_pullup_en = scl_pullup;
    conf.sda_io_num = sda_gpio.get_value();
    conf.sda_pullup_en = sda_pullup;
    conf.slave.addr_10bit_en = 0;
    conf.slave.slave_addr = slave_addr.get_addr();
    I2C_CHECK_THROW(i2c_param_config(i2c_num.get_value(), &conf));
    I2C_CHECK_THROW(i2c_driver_install(i2c_num.get_value(), conf.mode, rx_buf_len, tx_buf_len, 0));
}

I2CSlave::~I2CSlave()
{
    i2c_driver_delete(i2c_num.get_value());
}

int I2CSlave::write_raw(const uint8_t *data, size_t data_len, chrono::milliseconds timeout)
{
    return i2c_slave_write_buffer(i2c_num.get_value(), data, data_len, (TickType_t) timeout.count() / portTICK_PERIOD_MS);
}

int I2CSlave::read_raw(uint8_t *buffer, size_t buffer_len, chrono::milliseconds timeout)
{
    return i2c_slave_read_buffer(i2c_num.get_value(), buffer, buffer_len, (TickType_t) timeout.count() / portTICK_PERIOD_MS);
}
#endif // CONFIG_SOC_I2C_SUPPORT_SLAVE

I2CWrite::I2CWrite(const vector<uint8_t> &bytes, chrono::milliseconds driver_timeout)
    : I2CTransfer<void>(driver_timeout), bytes(bytes)
{
    if (bytes.empty()) {
        throw I2CException(ESP_ERR_INVALID_ARG);
    }
}

void I2CWrite::queue_cmd(I2CCommandLink &handle, I2CAddress i2c_addr)
{
    handle.start();
    handle.write_byte(i2c_addr.get_value() << 1 | I2C_MASTER_WRITE);
    handle.write(bytes);
}

void I2CWrite::process_result() { }

I2CRead::I2CRead(size_t size, chrono::milliseconds driver_timeout)
    : I2CTransfer<vector<uint8_t> >(driver_timeout), bytes(size)
{
    if (size == 0) {
        throw I2CException(ESP_ERR_INVALID_ARG);
    }
}

void I2CRead::queue_cmd(I2CCommandLink &handle, I2CAddress i2c_addr)
{
    handle.start();
    handle.write_byte(i2c_addr.get_value() << 1 | I2C_MASTER_READ);
    handle.read(bytes);
}

vector<uint8_t> I2CRead::process_result()
{
    return bytes;
}

I2CComposed::I2CComposed(chrono::milliseconds driver_timeout)
    : I2CTransfer<vector<vector<uint8_t> > >(driver_timeout), transfer_list() { }

void I2CComposed::CompTransferNodeRead::queue_cmd(I2CCommandLink &handle, I2CAddress i2c_addr)
{
    handle.write_byte(i2c_addr.get_value() << 1 | I2C_MASTER_READ);
    handle.read(bytes);
}

void I2CComposed::CompTransferNodeRead::process_result(std::vector<std::vector<uint8_t> > &read_results)
{
    read_results.push_back(bytes);
}

void I2CComposed::CompTransferNodeWrite::queue_cmd(I2CCommandLink &handle, I2CAddress i2c_addr)
{
    handle.write_byte(i2c_addr.get_value() << 1 | I2C_MASTER_WRITE);
    handle.write(bytes);
}

void I2CComposed::add_read(size_t size)
{
    if (!size) {
        throw I2CException(ESP_ERR_INVALID_ARG);
    }

    transfer_list.push_back(make_shared<CompTransferNodeRead>(size));
}

void I2CComposed::add_write(std::vector<uint8_t> bytes)
{
    if (bytes.empty()) {
        throw I2CException(ESP_ERR_INVALID_ARG);
    }

    transfer_list.push_back(make_shared<CompTransferNodeWrite>(bytes));
}

void I2CComposed::queue_cmd(I2CCommandLink &handle, I2CAddress i2c_addr)
{
    for (auto it = transfer_list.begin(); it != transfer_list.end(); it++) {
        handle.start();
        (*it)->queue_cmd(handle, i2c_addr);
    }
}

std::vector<std::vector<uint8_t> > I2CComposed::process_result()
{
    std::vector<std::vector<uint8_t> > results;
    for (auto it = transfer_list.begin(); it != transfer_list.end(); it++) {
        (*it)->process_result(results);
    }
    return results;
}

} // idf

#endif // __cpp_exceptions
