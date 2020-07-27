// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//         http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifdef __cpp_exceptions

#include "i2c_cxx.hpp"

using namespace std;

namespace idf {

#define I2C_CHECK_THROW(err) CHECK_THROW_SPECIFIC((err), I2CException)

I2CException::I2CException(esp_err_t error) : ESPException(error) { }

I2CTransferException::I2CTransferException(esp_err_t error) : I2CException(error) { }

I2CBus::I2CBus(i2c_port_t i2c_number) : i2c_num(i2c_number) { }

I2CBus::~I2CBus() { }

I2CMaster::I2CMaster(i2c_port_t i2c_number,
                     int scl_gpio,
                     int sda_gpio,
                     uint32_t clock_speed,
                     bool scl_pullup,
                     bool sda_pullup)
    : I2CBus(i2c_number)
{
    i2c_config_t conf = {};
    conf.mode = I2C_MODE_MASTER;
    conf.scl_io_num = scl_gpio;
    conf.scl_pullup_en = scl_pullup;
    conf.sda_io_num = sda_gpio;
    conf.sda_pullup_en = sda_pullup;
    conf.master.clk_speed = clock_speed;
    I2C_CHECK_THROW(i2c_param_config(i2c_num, &conf));
    I2C_CHECK_THROW(i2c_driver_install(i2c_num, conf.mode, 0, 0, 0));
}

I2CMaster::~I2CMaster()
{
    i2c_driver_delete(i2c_num);
}

void I2CMaster::sync_write(uint8_t i2c_addr, const vector<uint8_t> &data)
{
    I2CWrite writer(data);

    writer.do_transfer(i2c_num, i2c_addr);
}

std::vector<uint8_t> I2CMaster::sync_read(uint8_t i2c_addr, size_t n_bytes)
{
    I2CRead reader(n_bytes);

    return reader.do_transfer(i2c_num, i2c_addr);
}

vector<uint8_t> I2CMaster::sync_transfer(uint8_t i2c_addr,
        const std::vector<uint8_t> &write_data,
        size_t read_n_bytes)
{
    if (!read_n_bytes) throw I2CException(ESP_ERR_INVALID_ARG);

    I2CComposed composed_transfer;
    composed_transfer.add_write(write_data);
    composed_transfer.add_read(read_n_bytes);

    return composed_transfer.do_transfer(i2c_num, i2c_addr)[0];
}

I2CSlave::I2CSlave(i2c_port_t i2c_number,
        int scl_gpio,
        int sda_gpio,
        uint8_t slave_addr,
        size_t rx_buf_len,
        size_t tx_buf_len,
        bool scl_pullup,
        bool sda_pullup)
    : I2CBus(i2c_number)
{
    i2c_config_t conf = {};
    conf.mode = I2C_MODE_SLAVE;
    conf.scl_io_num = scl_gpio;
    conf.scl_pullup_en = scl_pullup;
    conf.sda_io_num = sda_gpio;
    conf.sda_pullup_en = sda_pullup;
    conf.slave.addr_10bit_en = 0;
    conf.slave.slave_addr = slave_addr;
    I2C_CHECK_THROW(i2c_param_config(i2c_num, &conf));
    I2C_CHECK_THROW(i2c_driver_install(i2c_num, conf.mode, rx_buf_len, tx_buf_len, 0));
}

I2CSlave::~I2CSlave()
{
    i2c_driver_delete(i2c_num);
}

int I2CSlave::write_raw(const uint8_t *data, size_t data_len, chrono::milliseconds timeout)
{
    return i2c_slave_write_buffer(i2c_num, data, data_len, (TickType_t) timeout.count() / portTICK_RATE_MS);
}

int I2CSlave::read_raw(uint8_t *buffer, size_t buffer_len, chrono::milliseconds timeout)
{
    return i2c_slave_read_buffer(i2c_num, buffer, buffer_len, (TickType_t) timeout.count() / portTICK_RATE_MS);
}

I2CWrite::I2CWrite(const vector<uint8_t> &bytes, chrono::milliseconds driver_timeout)
    : I2CTransfer<void>(driver_timeout), bytes(bytes) { }

void I2CWrite::queue_cmd(i2c_cmd_handle_t handle, uint8_t i2c_addr)
{
    I2C_CHECK_THROW(i2c_master_start(handle));
    I2C_CHECK_THROW(i2c_master_write_byte(handle, i2c_addr << 1 | I2C_MASTER_WRITE, true));
    I2C_CHECK_THROW(i2c_master_write(handle, bytes.data(), bytes.size(), true));
}

void I2CWrite::process_result() { }

I2CRead::I2CRead(size_t size, chrono::milliseconds driver_timeout)
    : I2CTransfer<vector<uint8_t> >(driver_timeout), bytes(size) { }

void I2CRead::queue_cmd(i2c_cmd_handle_t handle, uint8_t i2c_addr)
{
    I2C_CHECK_THROW(i2c_master_start(handle));
    I2C_CHECK_THROW(i2c_master_write_byte(handle, i2c_addr << 1 | I2C_MASTER_READ, true));
    I2C_CHECK_THROW(i2c_master_read(handle, bytes.data(), bytes.size(), I2C_MASTER_LAST_NACK));
}

vector<uint8_t> I2CRead::process_result()
{
    return bytes;
}

I2CComposed::I2CComposed(chrono::milliseconds driver_timeout)
    : I2CTransfer<vector<vector<uint8_t> > >(driver_timeout), transfer_list() { }

void I2CComposed::CompTransferNodeRead::queue_cmd(i2c_cmd_handle_t handle, uint8_t i2c_addr)
{
    I2C_CHECK_THROW(i2c_master_write_byte(handle, i2c_addr << 1 | I2C_MASTER_READ, true));
    I2C_CHECK_THROW(i2c_master_read(handle, bytes.data(), bytes.size(), I2C_MASTER_LAST_NACK));
}

void I2CComposed::CompTransferNodeRead::process_result(std::vector<std::vector<uint8_t> > &read_results)
{
    read_results.push_back(bytes);
}

void I2CComposed::CompTransferNodeWrite::queue_cmd(i2c_cmd_handle_t handle, uint8_t i2c_addr)
{
    I2C_CHECK_THROW(i2c_master_write_byte(handle, i2c_addr << 1 | I2C_MASTER_WRITE, true));
    I2C_CHECK_THROW(i2c_master_write(handle, bytes.data(), bytes.size(), true));
}

void I2CComposed::add_read(size_t size)
{
    if (!size) throw I2CException(ESP_ERR_INVALID_ARG);

    transfer_list.push_back(make_shared<CompTransferNodeRead>(size));
}

void I2CComposed::add_write(std::vector<uint8_t> bytes)
{
    if (bytes.empty()) throw I2CException(ESP_ERR_INVALID_ARG);

    transfer_list.push_back(make_shared<CompTransferNodeWrite>(bytes));
}

void I2CComposed::queue_cmd(i2c_cmd_handle_t handle, uint8_t i2c_addr)
{
    for (auto it = transfer_list.begin(); it != transfer_list.end(); it++) {
        I2C_CHECK_THROW(i2c_master_start(handle));
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
