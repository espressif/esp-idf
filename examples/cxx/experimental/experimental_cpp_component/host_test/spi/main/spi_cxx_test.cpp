/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0
 *
 * This test code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
*/

#define CATCH_CONFIG_MAIN
#include <stdio.h>
#include "unity.h"
#include "freertos/portmacro.h"
#include "spi_host_cxx.hpp"
#include "spi_host_private_cxx.hpp"
#include "system_cxx.hpp"
#include "test_fixtures.hpp"

#include "catch.hpp"

// TODO: IDF-2693, function definition just to satisfy linker, mock esp_common instead
const char *esp_err_to_name(esp_err_t code) {
    return "host_test error";
}

using namespace std;
using namespace idf;

TEST_CASE("SPITransferSize basic construction")
{
    SPITransferSize transfer_size_0(0);
    CHECK(0 == transfer_size_0.get_value());
    SPITransferSize transfer_size_1(47);
    CHECK(47 == transfer_size_1.get_value());
    SPITransferSize transfer_size_default = SPITransferSize::default_size();
    CHECK(0 == transfer_size_default.get_value());
}

TEST_CASE("SPI gpio numbers work correctly")
{
    GPIONum gpio_num_0(19);
    MOSI mosi_0(18);
    MOSI mosi_1(gpio_num_0.get_num());
    MOSI mosi_2(mosi_0);
    CHECK(mosi_0 != mosi_1);
    CHECK(mosi_2 == mosi_0);
    CHECK(mosi_2.get_num() == 18u);
}

TEST_CASE("SPI_DMAConfig valid")
{
    CHECK(SPI_DMAConfig::AUTO().get_num() == spi_common_dma_t::SPI_DMA_CH_AUTO);
    CHECK(SPI_DMAConfig::DISABLED().get_num() == spi_common_dma_t::SPI_DMA_DISABLED);
}

TEST_CASE("SPINum invalid argument")
{
    CHECK_THROWS_AS(SPINum(-1), SPIException&);
    uint32_t host_raw = spi_host_device_t::SPI_HOST_MAX;
    CHECK_THROWS_AS(SPINum host(host_raw), SPIException&);
}

TEST_CASE("Master init failure")
{
    CMockFixture cmock_fix;
    spi_bus_initialize_ExpectAnyArgsAndReturn(ESP_FAIL);

    CHECK_THROWS_AS(SPIMaster master(SPINum(SPI2_HOST), MOSI(1), MISO(2), SCLK(3)), SPIException&);
}

TEST_CASE("Master invalid state")
{
    CMockFixture cmock_fix;
    spi_bus_initialize_ExpectAnyArgsAndReturn(ESP_ERR_INVALID_STATE);

    CHECK_THROWS_AS(SPIMaster master(SPINum(SPI2_HOST), MOSI(1), MISO(2), SCLK(3)), SPIException&);
}

TEST_CASE("build master")
{
    SPIFix fix;

    SPIMaster master(SPINum(SPI2_HOST),
            MOSI(fix.bus_config.mosi_io_num),
            MISO(fix.bus_config.miso_io_num),
            SCLK(fix.bus_config.sclk_io_num));
}

TEST_CASE("build QSPI master")
{
    QSPIFix fix;

    SPIMaster master(SPINum(SPI2_HOST),
            MOSI(fix.bus_config.mosi_io_num),
            MISO(fix.bus_config.miso_io_num),
            SCLK(fix.bus_config.sclk_io_num),
            QSPIWP(fix.bus_config.quadwp_io_num),
            QSPIHD(fix.bus_config.quadhd_io_num));
}

TEST_CASE("Master build device")
{
    SPIFix fix;
    SPIDevFix dev_fix(CreateAnd::SUCCEED);

    SPIMaster master(SPINum(SPI2_HOST),
            MOSI(fix.bus_config.mosi_io_num),
            MISO(fix.bus_config.miso_io_num),
            SCLK(fix.bus_config.sclk_io_num));

    master.create_dev(CS(4), Frequency::MHz(1));
}

TEST_CASE("SPIDeviceHandle throws on driver error")
{
    CMockFixture cmock_fix;
    SPIDevFix dev_fix(CreateAnd::FAIL);
    CHECK_THROWS_AS(SPIDeviceHandle handle(SPINum(SPI2_HOST), CS(4), Frequency::MHz(1), QueueSize(10)), SPIException&);
}

TEST_CASE("SPIDeviceHandle succeed")
{
    CMockFixture cmock_fix;
    SPIDevFix dev_fix(CreateAnd::SUCCEED);
    SPIDeviceHandle handle(SPINum(SPI2_HOST), CS(4), Frequency::MHz(1), QueueSize(10));
}

TEST_CASE("SPIDevice succeed")
{
    CMockFixture cmock_fix;
    SPIDevFix dev_fix(CreateAnd::SUCCEED);
    SPIDevice dev(SPINum(SPI2_HOST), CS(4), Frequency::MHz(1), QueueSize(10));
}

TEST_CASE("SPI transaction empty data throws")
{
    CHECK_THROWS_AS(SPITransactionDescriptor transaction({}, reinterpret_cast<SPIDeviceHandle*>(4747)), SPIException&);
}

TEST_CASE("SPI transaction device handle nullptr throws")
{
    CHECK_THROWS_AS(SPITransactionDescriptor transaction({47}, nullptr), SPIException&);
}

TEST_CASE("SPI transaction not started wait_for")
{
    CMockFixture cmock_fix;
    SPIDevFix dev_fix(CreateAnd::IGNORE);
    SPIDeviceHandle handle(SPINum(SPI2_HOST), CS(4), Frequency::MHz(1), QueueSize(10));
    SPITransactionDescriptor transaction({47}, &handle);

    CHECK_THROWS_AS(transaction.wait_for(std::chrono::milliseconds(47)), SPITransferException&);
}

TEST_CASE("SPI transaction not started wait")
{
    CMockFixture cmock_fix;
    SPIDevFix dev_fix(CreateAnd::IGNORE);
    SPIDeviceHandle handle(SPINum(SPI2_HOST), CS(4), Frequency::MHz(1), QueueSize(10));
    SPITransactionDescriptor transaction({47}, &handle);

    CHECK_THROWS_AS(transaction.wait(), SPITransferException&);
}

TEST_CASE("SPI transaction not started get")
{
    CMockFixture cmock_fix;
    SPIDevFix dev_fix(CreateAnd::IGNORE);
    SPIDeviceHandle handle(SPINum(SPI2_HOST), CS(4), Frequency::MHz(1), QueueSize(10));
    SPITransactionDescriptor transaction({47}, &handle);

    CHECK_THROWS_AS(transaction.get(), SPITransferException&);
}

TEST_CASE("SPI transaction wait_for timeout")
{
    CMockFixture cmock_fix;
    SPITransactionFix transaction_fix(ESP_ERR_TIMEOUT);
    SPIDevFix dev_fix(CreateAnd::IGNORE);
    spi_device_acquire_bus_IgnoreAndReturn(ESP_OK);
    spi_device_release_bus_Ignore();

    SPIDeviceHandle handle(SPINum(SPI2_HOST), CS(4), Frequency::MHz(1), QueueSize(10));
    SPITransactionDescriptor transaction({47}, &handle);
    transaction.start();

    CHECK(transaction.wait_for(std::chrono::milliseconds(47)) == false);

    // We need to finish the transaction, otherwise it goes out of scope without finishing and cleaning up the
    // allocated transaction descriptor.
    transaction_fix.get_transaction_return = ESP_OK;
    spi_device_get_trans_result_ExpectAnyArgsAndReturn(ESP_OK);
    transaction.wait();
}

TEST_CASE("SPI transaction one byte")
{
    CMockFixture cmock_fix;
    SPITransactionDescriptorFix fix(1, true);
    SPIDevFix dev_fix(CreateAnd::IGNORE);
    fix.rx_data = {0xA6};

    SPIDeviceHandle handle(SPINum(SPI2_HOST), CS(4), Frequency::MHz(1), QueueSize(10));
    SPITransactionDescriptor transaction({47}, &handle);

    transaction.start();
    auto out_data = transaction.get();

    CHECK(1 * 8 == fix.orig_trans->length);
    CHECK(47 == ((uint8_t*) fix.orig_trans->tx_buffer)[0]);
    REQUIRE(out_data.begin() != out_data.end());
    CHECK(0xA6 == out_data[0]);
}

TEST_CASE("SPI transaction two byte")
{
    CMockFixture cmock_fix;
    SPITransactionDescriptorFix fix(2, true);
    SPIDevFix dev_fix(CreateAnd::IGNORE);
    fix.rx_data = {0xA6, 0xA7};

    SPIDeviceHandle handle(SPINum(SPI2_HOST), CS(4), Frequency::MHz(1), QueueSize(10));
    SPITransactionDescriptor transaction({47, 48}, &handle);
    transaction.start();
    auto out_data = transaction.get();

    CHECK(fix.size * 8 == fix.orig_trans->length);
    CHECK(47 == ((uint8_t*) fix.orig_trans->tx_buffer)[0]);
    CHECK(48 == ((uint8_t*) fix.orig_trans->tx_buffer)[1]);
    REQUIRE(out_data.begin() != out_data.end());
    REQUIRE(out_data.size() == 2);
    CHECK(0xA6 == out_data[0]);
    CHECK(0xA7 == out_data[1]);
}

TEST_CASE("SPI transaction future")
{
    CMockFixture cmock_fix;
    SPITransactionDescriptorFix trans_fix(1, true);
    trans_fix.rx_data = {0xA6};
    SPIDevFix dev_fix(CreateAnd::IGNORE);

    SPIDevice dev(SPINum(SPI2_HOST), CS(4), Frequency::MHz(1), QueueSize(10));
    auto result = dev.transfer({47});
    vector<uint8_t> out_data = result.get();

    CHECK(1 * 8 == trans_fix.orig_trans->length);
    CHECK(47 == ((uint8_t*) trans_fix.orig_trans->tx_buffer)[0]);
    REQUIRE(out_data.begin() != out_data.end());
    CHECK(out_data.size() == 1);
    CHECK(0xA6 == out_data[0]);
}

TEST_CASE("SPI transaction with pre_callback")
{
    CMockFixture cmock_fix;
    SPITransactionDescriptorFix trans_fix(1, true);
    trans_fix.rx_data = {0xA6};
    SPIDevFix dev_fix(CreateAnd::SUCCEED);
    bool pre_cb_called = false;

    SPIDevice dev(SPINum(SPI2_HOST), CS(4), Frequency::MHz(1), QueueSize(10));
    auto result = dev.transfer({47}, [&] (void *user) { pre_cb_called = true; });
    vector<uint8_t> out_data = result.get();

    SPITransactionDescriptor *transaction = reinterpret_cast<SPITransactionDescriptor*>(trans_fix.orig_trans->user);
    dev_fix.dev_config.pre_cb(trans_fix.orig_trans);
    CHECK(true == pre_cb_called);
    CHECK(1 * 8 == trans_fix.orig_trans->length);
    CHECK(47 == ((uint8_t*) trans_fix.orig_trans->tx_buffer)[0]);
    REQUIRE(out_data.begin() != out_data.end());
    CHECK(out_data.size() == 1);
    CHECK(0xA6 == out_data[0]);
}

TEST_CASE("SPI transaction with post_callback")
{
    CMockFixture cmock_fix;
    SPITransactionDescriptorFix trans_fix(1, true);
    trans_fix.rx_data = {0xA6};
    SPIDevFix dev_fix(CreateAnd::SUCCEED);
    bool post_cb_called = false;
    SPIDevice dev(SPINum(SPI2_HOST), CS(4), Frequency::MHz(1), QueueSize(10));

    auto result = dev.transfer({47}, [&] (void *user) { }, [&] (void *user) { post_cb_called = true; });
    vector<uint8_t> out_data = result.get();

    dev_fix.dev_config.post_cb(trans_fix.orig_trans);
    CHECK(true == post_cb_called);
    CHECK(1 * 8 == trans_fix.orig_trans->length);
    CHECK(47 == ((uint8_t*) trans_fix.orig_trans->tx_buffer)[0]);
    REQUIRE(out_data.begin() != out_data.end());
    CHECK(out_data.size() == 1);
    CHECK(0xA6 == out_data[0]);
}

TEST_CASE("SPI transaction data routed to pre callback")
{
    CMockFixture cmock_fix;
    SPITransactionDescriptorFix trans_fix(1, true);
    trans_fix.rx_data = {0xA6};
    SPIDevFix dev_fix(CreateAnd::SUCCEED);
    bool pre_cb_called = false;
    SPIDevice dev(SPINum(SPI2_HOST), CS(4), Frequency::MHz(1), QueueSize(10));

    auto result = dev.transfer({47},
            [&] (void *user) { *(static_cast<bool*>(user)) = true; },
            [&] (void *user) { },
            &pre_cb_called);
    result.get();

    dev_fix.dev_config.pre_cb(trans_fix.orig_trans);
    CHECK(true == pre_cb_called);

}

TEST_CASE("SPI transaction data routed to post callback")
{
    CMockFixture cmock_fix;
    SPITransactionDescriptorFix trans_fix(1, true);
    trans_fix.rx_data = {0xA6};
    SPIDevFix dev_fix(CreateAnd::SUCCEED);
    bool post_cb_called = false;
    SPIDevice dev(SPINum(SPI2_HOST), CS(4), Frequency::MHz(1), QueueSize(10));

    auto result = dev.transfer({47},
            [&] (void *user) { },
            [&] (void *user) { *(static_cast<bool*>(user)) = true; },
            &post_cb_called);
    result.get();

    dev_fix.dev_config.post_cb(trans_fix.orig_trans);
    CHECK(true == post_cb_called);
}

TEST_CASE("SPI two transactions")
{
    CMockFixture cmock_fix;
    SPITransactionDescriptorFix trans_fix(1, true);
    trans_fix.rx_data = {0xA6};
    SPIDevFix dev_fix(CreateAnd::SUCCEED);
    bool pre_cb_called = false;
    SPIDevice dev(SPINum(SPI2_HOST), CS(4), Frequency::MHz(1), QueueSize(10));
    std::function<void(void *)> pre_callback = [&] (void *user) {
        pre_cb_called = true;
    };

    auto result = dev.transfer({47}, pre_callback);
    vector<uint8_t> out_data = result.get();

    dev_fix.dev_config.pre_cb(trans_fix.orig_trans);
    CHECK(true == pre_cb_called);

    CHECK(1 * 8 == trans_fix.orig_trans->length);
    CHECK(47 == ((uint8_t*) trans_fix.orig_trans->tx_buffer)[0]);

    REQUIRE(out_data.begin() != out_data.end());
    CHECK(out_data.size() == 1);
    CHECK(0xA6 == out_data[0]);

    // preparing the second transfer
    pre_cb_called = false;
    spi_device_acquire_bus_ExpectAndReturn(trans_fix.handle, portMAX_DELAY, ESP_OK);
    spi_device_acquire_bus_IgnoreArg_device();
    spi_device_queue_trans_ExpectAndReturn(trans_fix.handle, nullptr, 0, ESP_OK);
    spi_device_queue_trans_IgnoreArg_trans_desc();
    spi_device_queue_trans_IgnoreArg_handle();
    spi_device_get_trans_result_ExpectAndReturn(trans_fix.handle, nullptr, portMAX_DELAY, ESP_OK);
    spi_device_get_trans_result_IgnoreArg_trans_desc();
    spi_device_get_trans_result_IgnoreArg_handle();
    spi_device_release_bus_Ignore();


    result = dev.transfer({47}, pre_callback);
    result.get();

    dev_fix.dev_config.pre_cb(trans_fix.orig_trans);
    CHECK(true == pre_cb_called);
}

TEST_CASE("SPIFuture invalid after default construction")
{
    SPIFuture future;
    CHECK(false == future.valid());
}

TEST_CASE("SPIFuture valid")
{
    CMOCK_SETUP();
    SPIDevFix dev_fix(CreateAnd::IGNORE);

    SPIDeviceHandle handle(SPINum(SPI2_HOST), CS(4), Frequency::MHz(1), QueueSize(10));
    shared_ptr<SPITransactionDescriptor> trans(new SPITransactionDescriptor(std::vector<uint8_t>(47), &handle));
    SPIFuture future(trans);

    CHECK(true == future.valid());
}

TEST_CASE("SPIFuture wait_for timeout")
{
    CMockFixture cmock_fix;
    SPITransactionFix transaction_fix(ESP_ERR_TIMEOUT);
    SPIDevFix dev_fix(CreateAnd::IGNORE);
    spi_device_acquire_bus_IgnoreAndReturn(ESP_OK);
    spi_device_release_bus_Ignore();

    SPIDeviceHandle handle(SPINum(SPI2_HOST), CS(4), Frequency::MHz(1), QueueSize(10));
    shared_ptr<SPITransactionDescriptor> transaction(new SPITransactionDescriptor(std::vector<uint8_t>(47), &handle));
    SPIFuture future(transaction);
    transaction->start();

    CHECK(future.wait_for(std::chrono::milliseconds(47)) == std::future_status::timeout);

    // We need to finish the transaction, otherwise it goes out of scope without finishing and cleaning up the
    // allocated transaction descriptor.
    transaction_fix.get_transaction_return = ESP_OK;
    spi_device_get_trans_result_ExpectAnyArgsAndReturn(ESP_OK);

    future.wait();
}

TEST_CASE("SPIFuture wait_for on SPIFuture")
{
    CMockFixture cmock_fix;
    SPITransactionDescriptorFix trans_fix(1, true, 20);
    trans_fix.rx_data = {0xA6};
    SPIDevFix dev_fix(CreateAnd::IGNORE);

    SPIDevice dev(SPINum(SPI2_HOST), CS(4), Frequency::MHz(1), QueueSize(10));
    auto result = dev.transfer({47});

    CHECK(result.wait_for(std::chrono::milliseconds(20)) == std::future_status::ready);
}

TEST_CASE("SPIFuture wait on SPIFuture")
{
    CMockFixture cmock_fix;
    SPITransactionDescriptorFix trans_fix(1, true);
    trans_fix.rx_data = {0xA6};
    SPIDevFix dev_fix(CreateAnd::IGNORE);

    SPIDevice dev(SPINum(SPI2_HOST), CS(4), Frequency::MHz(1), QueueSize(10));
    auto result = dev.transfer({47});

    result.wait();

    vector<uint8_t> out_data = result.get();
    CHECK(out_data.size() == 1);
}
