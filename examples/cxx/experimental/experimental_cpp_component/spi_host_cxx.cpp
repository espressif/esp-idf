/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#if __cpp_exceptions

#include <stdint.h>
#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "hal/spi_types.h"
#include "driver/spi_master.h"
#include "spi_host_cxx.hpp"
#include "spi_host_private_cxx.hpp"

using namespace std;

namespace idf {

SPIException::SPIException(esp_err_t error) : ESPException(error) { }

SPITransferException::SPITransferException(esp_err_t error) : SPIException(error) { }

SPIMaster::SPIMaster(SPINum host,
        const MOSI &mosi,
        const MISO &miso,
        const SCLK &sclk,
        SPI_DMAConfig dma_config,
        SPITransferSize transfer_size)
    : spi_host(host)
{
    spi_bus_config_t bus_config = {};
    bus_config.mosi_io_num = mosi.get_num();
    bus_config.miso_io_num = miso.get_num();
    bus_config.sclk_io_num = sclk.get_num();
    bus_config.quadwp_io_num = -1;
    bus_config.quadhd_io_num = -1;
    bus_config.max_transfer_sz = transfer_size.get_value();

    SPI_CHECK_THROW(spi_bus_initialize(spi_num_to_driver_type(spi_host), &bus_config, dma_config.get_num()));
}

SPIMaster::SPIMaster(SPINum host,
        const MOSI &mosi,
        const MISO &miso,
        const SCLK &sclk,
        const QSPIWP &qspiwp,
        const QSPIHD &qspihd,
        SPI_DMAConfig dma_config,
        SPITransferSize transfer_size)
    : spi_host(host)
{
    spi_bus_config_t bus_config = {};
    bus_config.mosi_io_num = mosi.get_num();
    bus_config.miso_io_num = miso.get_num();
    bus_config.sclk_io_num = sclk.get_num();
    bus_config.quadwp_io_num = qspiwp.get_num();
    bus_config.quadhd_io_num = qspihd.get_num();
    bus_config.max_transfer_sz = transfer_size.get_value();

    SPI_CHECK_THROW(spi_bus_initialize(spi_num_to_driver_type(spi_host), &bus_config, dma_config.get_num()));
}

SPIMaster::~SPIMaster()
{
    spi_bus_free(spi_num_to_driver_type(spi_host));
}

shared_ptr<SPIDevice> SPIMaster::create_dev(CS cs, Frequency frequency)
{
    return make_shared<SPIDevice>(spi_host, cs, frequency);
}

SPIFuture::SPIFuture()
    : transaction(), is_valid(false)
{
}

SPIFuture::SPIFuture(shared_ptr<SPITransactionDescriptor> transaction)
    : transaction(transaction), is_valid(true)
{
}

SPIFuture::SPIFuture(SPIFuture &&other) noexcept
    : transaction(std::move(other.transaction)), is_valid(true)
{
    other.is_valid = false;
}

SPIFuture &SPIFuture::operator=(SPIFuture &&other) noexcept
{
    if (this != &other) {
        transaction = std::move(other.transaction);
        is_valid = other.is_valid;
        other.is_valid = false;
    }
    return *this;
}

vector<uint8_t> SPIFuture::get()
{
    if (!is_valid) {
        throw std::future_error(future_errc::no_state);
    }

    return transaction->get();
}

future_status SPIFuture::wait_for(chrono::milliseconds timeout)
{
    if (transaction->wait_for(timeout)) {
        return std::future_status::ready;
    } else {
        return std::future_status::timeout;
    }
}

void SPIFuture::wait()
{
    transaction->wait();
}

bool SPIFuture::valid() const noexcept
{
    return is_valid;
}

SPIDevice::SPIDevice(SPINum spi_host, CS cs, Frequency frequency, QueueSize q_size) : device_handle()
{
    device_handle = new SPIDeviceHandle(spi_host, cs, frequency, q_size);
}

SPIDevice::~SPIDevice()
{
    delete device_handle;
}

SPIFuture SPIDevice::transfer(const vector<uint8_t> &data_to_send,
            std::function<void(void *)> pre_callback,
            std::function<void(void *)> post_callback,
            void* user_data)
{
    current_transaction = make_shared<SPITransactionDescriptor>(data_to_send,
            device_handle,
            std::move(pre_callback),
            std::move(post_callback),
            user_data);
    current_transaction->start();
    return SPIFuture(current_transaction);
}

SPITransactionDescriptor::SPITransactionDescriptor(const std::vector<uint8_t> &data_to_send,
        SPIDeviceHandle *handle,
        std::function<void(void *)> pre_callback,
        std::function<void(void *)> post_callback,
        void* user_data_arg)
    : device_handle(handle),
    pre_callback(std::move(pre_callback)),
    post_callback(std::move(post_callback)),
    user_data(user_data_arg),
    received_data(false),
    started(false)
{
    // C++11 vectors don't have size() or empty() members yet
    if (data_to_send.begin() == data_to_send.end()) {
        throw SPITransferException(ESP_ERR_INVALID_ARG);
    }
    if (handle == nullptr) {
        throw SPITransferException(ESP_ERR_INVALID_ARG);
    }

    size_t trans_size = data_to_send.size();
    spi_transaction_t *trans_desc;
    trans_desc = new spi_transaction_t;
    memset(trans_desc, 0, sizeof(spi_transaction_t));
    trans_desc->rx_buffer = new uint8_t [trans_size];
    tx_buffer = new uint8_t [trans_size];
    for (size_t i = 0; i < trans_size; i++) {
        tx_buffer[i] = data_to_send[i];
    }
    trans_desc->length = trans_size * 8;
    trans_desc->tx_buffer = tx_buffer;
    trans_desc->user = this;

    private_transaction_desc = trans_desc;
}

SPITransactionDescriptor::~SPITransactionDescriptor()
{
    if (started) {
        assert(received_data);  // We need to make sure that trans_desc has been received, otherwise the
                                // driver may still write into it afterwards.
    }

    spi_transaction_t *trans_desc = reinterpret_cast<spi_transaction_t*>(private_transaction_desc);
    delete [] tx_buffer;
    delete [] static_cast<uint8_t*>(trans_desc->rx_buffer);
    delete trans_desc;
}

void SPITransactionDescriptor::start()
{
    spi_transaction_t *trans_desc = reinterpret_cast<spi_transaction_t*>(private_transaction_desc);
    SPI_CHECK_THROW(device_handle->acquire_bus(portMAX_DELAY));
    SPI_CHECK_THROW(device_handle->queue_trans(trans_desc, 0));
    started = true;
}

void SPITransactionDescriptor::wait()
{
    while (wait_for(chrono::milliseconds(portMAX_DELAY)) == false) { }
}

bool SPITransactionDescriptor::wait_for(const chrono::milliseconds &timeout_duration)
{
    if (received_data) {
        return true;
    }

    if (!started) {
        throw SPITransferException(ESP_ERR_INVALID_STATE);
    }

    spi_transaction_t *acquired_trans_desc;
    esp_err_t err = device_handle->get_trans_result(&acquired_trans_desc,
            (TickType_t) timeout_duration.count() / portTICK_PERIOD_MS);

    if (err == ESP_ERR_TIMEOUT) {
        return false;
    }

    if (err != ESP_OK) {
        throw SPITransferException(err);
    }

    if (acquired_trans_desc != reinterpret_cast<spi_transaction_t*>(private_transaction_desc)) {
        throw SPITransferException(ESP_ERR_INVALID_STATE);
    }

    received_data = true;
    device_handle->release_bus();

    return true;
}

std::vector<uint8_t> SPITransactionDescriptor::get()
{
    if (!received_data) {
        wait();
    }

    spi_transaction_t *trans_desc = reinterpret_cast<spi_transaction_t*>(private_transaction_desc);
    const size_t TRANSACTION_LENGTH = trans_desc->length / 8;
    vector<uint8_t> result(TRANSACTION_LENGTH);

    for (int i = 0; i < TRANSACTION_LENGTH; i++) {
        result[i] = static_cast<uint8_t*>(trans_desc->rx_buffer)[i];
    }

    return result;
}

} // idf

#endif // __cpp_exceptions
