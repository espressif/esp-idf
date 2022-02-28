/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 *
 * I2C C++ unit tests
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
*/
#define CATCH_CONFIG_MAIN
#include <stdio.h>
#include "unity.h"
#include "freertos/portmacro.h"
#include "driver/i2c.h"
#include "i2c_cxx.hpp"
#include "system_cxx.hpp"
#include "test_fixtures.hpp"

#include "catch.hpp"

extern "C" {
#include "Mocki2c.h"
}

// TODO: IDF-2693, function definition just to satisfy linker, mock esp_common instead
const char *esp_err_to_name(esp_err_t code) {
    return "host_test error";
}

using namespace std;
using namespace idf;

TEST_CASE("I2CNumber")
{
    CMockFixture fix;
    CHECK(I2CNumber::I2C0().get_num() == 0);
}

TEST_CASE("I2CAddr")
{
    CMockFixture fix;
    CHECK_THROWS_AS(I2CAddress(-1), I2CException&);
    I2CAddress(0);
    I2CAddress(127);
    CHECK_THROWS_AS(I2CAddress(128), I2CException&);

    I2CAddress addr(47);
    CHECK(addr.get_addr() == 47);
}

TEST_CASE("I2CMaster parameter configuration fails")
{
    CMockFixture fix;
    i2c_param_config_ExpectAnyArgsAndReturn(ESP_FAIL);

    CHECK_THROWS_AS(I2CMaster(I2CNumber::I2C0(), SCL_GPIO(1), SDA_GPIO(2), Frequency(400000)), I2CException&);
}

TEST_CASE("I2CMaster driver install failure")
{
    CMockFixture fix;
    i2c_param_config_ExpectAnyArgsAndReturn(ESP_OK);
    i2c_driver_install_ExpectAnyArgsAndReturn(ESP_FAIL);

    CHECK_THROWS_AS(I2CMaster(I2CNumber::I2C0(), SCL_GPIO(1), SDA_GPIO(2), Frequency(400000)), I2CException&);
}

TEST_CASE("I2CMaster success")
{
    CMockFixture fix;
    I2CMasterFix master_fix;

    I2CMaster(I2CNumber::I2C0(), SCL_GPIO(1), SDA_GPIO(2), Frequency(400000));
}

TEST_CASE("I2CWrite empty data throws")
{
    CMockFixture fix;
    std::vector<uint8_t> empty;
    CHECK_THROWS_AS(I2CWrite writer(empty), I2CException&);
}

TEST_CASE("I2CRead zero length throws")
{
    CMockFixture fix;
    std::vector<uint8_t> empty;
    CHECK_THROWS_AS(I2CRead reader(0), I2CException&);
}

TEST_CASE("I2CWrite do_transfer fails at link creation")
{
    CMockFixture fix;
    i2c_cmd_link_create_ExpectAndReturn(nullptr);
    i2c_cmd_link_delete_Ignore();
    I2CWrite writer({47});

    CHECK_THROWS_AS(writer.do_transfer(I2CNumber::I2C0(), I2CAddress(0x47)), I2CException&);
}

TEST_CASE("I2CWrite do_transfer fails at start")
{
    CMockFixture fix;
    i2c_cmd_handle_t dummy_handle = reinterpret_cast<i2c_cmd_handle_t>(0xbeef);
    i2c_cmd_link_create_IgnoreAndReturn(&dummy_handle);
    i2c_master_start_ExpectAnyArgsAndReturn(ESP_FAIL);
    i2c_cmd_link_delete_Ignore();
    I2CWrite writer({47});

    CHECK_THROWS_AS(writer.do_transfer(I2CNumber::I2C0(), I2CAddress(0x47)), I2CException&);
}

TEST_CASE("I2CWrite do_transfer fails at write byte")
{
    CMockFixture fix;
    i2c_cmd_handle_t dummy_handle = reinterpret_cast<i2c_cmd_handle_t>(0xbeef);
    i2c_cmd_link_create_IgnoreAndReturn(&dummy_handle);
    i2c_master_start_IgnoreAndReturn(ESP_OK);
    i2c_master_write_byte_ExpectAnyArgsAndReturn(ESP_FAIL);
    i2c_cmd_link_delete_Ignore();
    I2CWrite writer({47});

    CHECK_THROWS_AS(writer.do_transfer(I2CNumber::I2C0(), I2CAddress(0x47)), I2CException&);
}

TEST_CASE("I2CWrite do_transfer fails at write")
{
    CMockFixture fix;
    i2c_cmd_handle_t dummy_handle = reinterpret_cast<i2c_cmd_handle_t>(0xbeef);
    i2c_cmd_link_create_IgnoreAndReturn(&dummy_handle);
    i2c_master_start_IgnoreAndReturn(ESP_OK);
    i2c_master_write_byte_IgnoreAndReturn(ESP_OK);
    i2c_master_write_ExpectAnyArgsAndReturn(ESP_FAIL);
    i2c_cmd_link_delete_Ignore();
    I2CWrite writer({47});

    CHECK_THROWS_AS(writer.do_transfer(I2CNumber::I2C0(), I2CAddress(0x47)), I2CException&);
}

TEST_CASE("I2CWrite do_transfer fails at stop")
{
    CMockFixture fix;
    i2c_cmd_handle_t dummy_handle = reinterpret_cast<i2c_cmd_handle_t>(0xbeef);
    i2c_cmd_link_create_IgnoreAndReturn(&dummy_handle);
    i2c_master_start_IgnoreAndReturn(ESP_OK);
    i2c_master_write_byte_IgnoreAndReturn(ESP_OK);
    i2c_master_write_IgnoreAndReturn(ESP_OK);
    i2c_master_stop_ExpectAnyArgsAndReturn(ESP_FAIL);
    i2c_cmd_link_delete_Ignore();
    I2CWrite writer({47});

    CHECK_THROWS_AS(writer.do_transfer(I2CNumber::I2C0(), I2CAddress(0x47)), I2CException&);
}

TEST_CASE("I2CWrite do_transfer execution times out")
{
    CMockFixture fix;
    i2c_cmd_handle_t dummy_handle = reinterpret_cast<i2c_cmd_handle_t>(0xbeef);
    i2c_cmd_link_create_IgnoreAndReturn(&dummy_handle);
    i2c_master_start_IgnoreAndReturn(ESP_OK);
    i2c_master_write_byte_IgnoreAndReturn(ESP_OK);
    i2c_master_write_IgnoreAndReturn(ESP_OK);
    i2c_master_stop_IgnoreAndReturn(ESP_OK);
    i2c_master_cmd_begin_ExpectAnyArgsAndReturn(ESP_ERR_TIMEOUT);
    i2c_cmd_link_delete_Ignore();
    I2CWrite writer({47});

    CHECK_THROWS_AS(writer.do_transfer(I2CNumber::I2C0(), I2CAddress(0x47)), I2CTransferException&);
}

TEST_CASE("I2CWrite calls driver correctly")
{
    CMockFixture fix;
    I2CCmdLinkFix cmd_fix(0x47, I2C_MASTER_WRITE);
    uint8_t expected_write [] = {0xAB, 0xBA};
    const size_t WRITE_SIZE = sizeof(expected_write);
    const size_t EXPECTED_DATA_LEN = WRITE_SIZE;

    // note that this behavior is not entirely correct, in th real driver, only i2c_master_cmd_begin()
    // will actually write the data but for the tests it is enough for now
    i2c_master_write_ExpectWithArrayAndReturn(&cmd_fix.dummy_handle, expected_write, WRITE_SIZE, EXPECTED_DATA_LEN, true, ESP_OK);
    i2c_master_stop_ExpectAndReturn(&cmd_fix.dummy_handle, ESP_OK);
    i2c_master_cmd_begin_ExpectAndReturn(0, &cmd_fix.dummy_handle, 1000 / portTICK_PERIOD_MS, ESP_OK);

    std::vector<uint8_t> WRITE_BYTES = {0xAB, 0xBA};
    I2CWrite write(WRITE_BYTES);
    write.do_transfer(I2CNumber::I2C0(), I2CAddress(0x47));
}

TEST_CASE("I2CRead do_transfer fails at read")
{
    CMockFixture fix;
    i2c_cmd_handle_t dummy_handle = reinterpret_cast<i2c_cmd_handle_t>(0xbeef);
    i2c_cmd_link_create_ExpectAndReturn(&dummy_handle);
    i2c_master_start_ExpectAnyArgsAndReturn(ESP_OK);
    i2c_master_write_byte_ExpectAnyArgsAndReturn(ESP_OK);
    i2c_master_read_ExpectAnyArgsAndReturn(ESP_FAIL);
    i2c_cmd_link_delete_Ignore();
    I2CRead reader(2);

    CHECK_THROWS_AS(reader.do_transfer(I2CNumber::I2C0(), I2CAddress(0x47)), I2CException&);
}

TEST_CASE("I2CRead calls driver correctly")
{
    CMockFixture fix;
    I2CCmdLinkFix cmd_fix(0x47, I2C_MASTER_READ);
    uint8_t READ_DATA [] = {0xAB, 0xBA};
    const size_t READ_SIZE = sizeof(READ_DATA);

    i2c_master_read_ExpectAndReturn(&cmd_fix.dummy_handle, nullptr, READ_SIZE, i2c_ack_type_t::I2C_MASTER_LAST_NACK, ESP_OK);
    i2c_master_read_IgnoreArg_data();

    // note that this behavior is not entirely correct, in th real driver, only i2c_master_cmd_begin()
    // will actually read the data but for the tests it is enough for now
    i2c_master_read_ReturnArrayThruPtr_data(READ_DATA, READ_SIZE);
    i2c_master_stop_ExpectAndReturn(&cmd_fix.dummy_handle, ESP_OK);
    i2c_master_cmd_begin_ExpectAndReturn(0, &cmd_fix.dummy_handle, 1000 / portTICK_PERIOD_MS, ESP_OK);

    I2CRead reader(READ_SIZE);
    std::vector<uint8_t> result = reader.do_transfer(I2CNumber::I2C0(), I2CAddress(0x47));
    CHECK(result[0] == 0xAB);
    CHECK(result[1] == 0xBA);
}

TEST_CASE("I2CComposed try to read size 0 throws")
{
    CMockFixture fix;
    I2CComposed composed_transfer;
    CHECK_THROWS_AS(composed_transfer.add_read(0), I2CException&);
}

TEST_CASE("I2CComposed try to write empy vector throws")
{
    CMockFixture fix;
    I2CComposed composed_transfer;
    CHECK_THROWS_AS(composed_transfer.add_write({}), I2CException&);
}

TEST_CASE("I2CComposed calls driver correctly")
{
    CMockFixture fix;
    I2CCmdLinkFix cmd_fix(0x47, I2C_MASTER_WRITE);
    uint8_t expected_write [] = {0x47, 0x48, 0x49};
    const size_t WRITE_SIZE = sizeof(expected_write);
    const size_t EXPECTED_DATA_LEN = WRITE_SIZE;
    uint8_t READ_DATA [] = {0xAB, 0xBA};
    const size_t READ_SIZE = sizeof(READ_DATA);

    // the write-read transaction with repeated start:
    i2c_master_write_ExpectWithArrayAndReturn(&cmd_fix.dummy_handle, expected_write, WRITE_SIZE, EXPECTED_DATA_LEN, true, ESP_OK);
    i2c_master_start_ExpectAndReturn(&cmd_fix.dummy_handle, ESP_OK);
    i2c_master_write_byte_ExpectAndReturn(&cmd_fix.dummy_handle, 0x47 << 1 | I2C_MASTER_READ, true, ESP_OK);
    i2c_master_read_ExpectAndReturn(&cmd_fix.dummy_handle, nullptr, 2, i2c_ack_type_t::I2C_MASTER_LAST_NACK, ESP_OK);
    i2c_master_read_IgnoreArg_data();

    // note that this behavior is not entirely correct, in th real driver, only i2c_master_cmd_begin()
    // will actually read the data but for the tests it is enough for now
    i2c_master_read_ReturnArrayThruPtr_data(READ_DATA, READ_SIZE);
    i2c_master_stop_ExpectAndReturn(&cmd_fix.dummy_handle, ESP_OK);
    i2c_master_cmd_begin_ExpectAndReturn(0, &cmd_fix.dummy_handle, 1000 / portTICK_PERIOD_MS, ESP_OK);

    I2CComposed composed_transfer;
    composed_transfer.add_write({0x47, 0x48, 0x49});
    composed_transfer.add_read(READ_SIZE);

    vector<vector<uint8_t> > read_result = composed_transfer.do_transfer(I2CNumber::I2C0(), I2CAddress(0x47));

    TEST_ASSERT_EQUAL(1, read_result.size());
    TEST_ASSERT_EQUAL(READ_SIZE, read_result[0].size());
    for (int i = 0; i < READ_SIZE; i++) {
        TEST_ASSERT_EQUAL(READ_DATA[i], read_result[0][i]);
    }
}

TEST_CASE("I2CWrite transfer calls driver correctly")
{
    CMockFixture fix;
    I2CMasterFix master_fix;
    I2CCmdLinkFix cmd_fix(0x47, I2C_MASTER_WRITE);
    uint8_t expected_write [] = {0xAB, 0xBA};
    const size_t WRITE_SIZE = sizeof(expected_write);
    const size_t EXPECTED_DATA_LEN = WRITE_SIZE;

    // note that this behavior is not entirely correct, in th real driver, only i2c_master_cmd_begin()
    // will actually write the data but for the tests it is enough for now
    i2c_master_write_ExpectWithArrayAndReturn(&cmd_fix.dummy_handle, expected_write, WRITE_SIZE, EXPECTED_DATA_LEN, true, ESP_OK);
    i2c_master_stop_ExpectAndReturn(&cmd_fix.dummy_handle, ESP_OK);
    i2c_master_cmd_begin_ExpectAndReturn(0, &cmd_fix.dummy_handle, 1000 / portTICK_PERIOD_MS, ESP_OK);

    I2CMaster master(I2CNumber::I2C0(), SCL_GPIO(1), SDA_GPIO(2), Frequency(400000));
    std::vector<uint8_t> WRITE_BYTES = {0xAB, 0xBA};
    auto writer = make_shared<I2CWrite>(WRITE_BYTES);
    master.transfer(I2CAddress(0x47), writer);
}

TEST_CASE("I2CMaster synchronous write")
{
    CMockFixture fix;
    I2CMasterFix master_fix;
    I2CCmdLinkFix cmd_fix(0x47, I2C_MASTER_WRITE);
    uint8_t expected_write [] = {0xAB, 0xBA};
    const size_t WRITE_SIZE = sizeof(expected_write);
    const size_t EXPECTED_DATA_LEN = WRITE_SIZE;

    // note that this behavior is not entirely correct, in th real driver, only i2c_master_cmd_begin()
    // will actually write the data but for the tests it is enough for now
    i2c_master_write_ExpectWithArrayAndReturn(&cmd_fix.dummy_handle, expected_write, WRITE_SIZE, EXPECTED_DATA_LEN, true, ESP_OK);
    i2c_master_stop_ExpectAndReturn(&cmd_fix.dummy_handle, ESP_OK);
    i2c_master_cmd_begin_ExpectAndReturn(0, &cmd_fix.dummy_handle, 1000 / portTICK_PERIOD_MS, ESP_OK);

    I2CMaster master(I2CNumber::I2C0(), SCL_GPIO(1), SDA_GPIO(2), Frequency(400000));
    std::vector<uint8_t> WRITE_BYTES = {0xAB, 0xBA};
    master.sync_write(I2CAddress(0x47), WRITE_BYTES);
}

TEST_CASE("I2CMaster synchronous read")
{
    CMockFixture fix;
    I2CMasterFix master_fix;
    I2CCmdLinkFix cmd_fix(0x47, I2C_MASTER_READ);
    uint8_t READ_DATA [] = {0xAB, 0xBA};
    const size_t READ_SIZE = sizeof(READ_DATA);

    i2c_master_read_ExpectAndReturn(&cmd_fix.dummy_handle, nullptr, READ_SIZE, i2c_ack_type_t::I2C_MASTER_LAST_NACK, ESP_OK);
    i2c_master_read_IgnoreArg_data();

    // note that this behavior is not entirely correct, in th real driver, only i2c_master_cmd_begin()
    // will actually read the data but for the tests it is enough for now
    i2c_master_read_ReturnArrayThruPtr_data(READ_DATA, READ_SIZE);
    i2c_master_stop_ExpectAndReturn(&cmd_fix.dummy_handle, ESP_OK);
    i2c_master_cmd_begin_ExpectAndReturn(0, &cmd_fix.dummy_handle, 1000 / portTICK_PERIOD_MS, ESP_OK);

    I2CMaster master(I2CNumber::I2C0(), SCL_GPIO(1), SDA_GPIO(2), Frequency(400000));
    std::vector<uint8_t> result = master.sync_read(I2CAddress(0x47), READ_SIZE);

    REQUIRE(result.size() == READ_SIZE);
    CHECK(result[0] == 0xAB);
    CHECK(result[1] == 0xBA);
}

TEST_CASE("I2CMaster syncronous transfer (read and write)")
{
    CMockFixture fix;
    I2CMasterFix master_fix;
    I2CCmdLinkFix cmd_fix(0x47, I2C_MASTER_WRITE);
    i2c_cmd_handle_t dummy_handle = reinterpret_cast<i2c_cmd_handle_t>(0xbeef);
    uint8_t expected_write [] = {0x47, 0x48, 0x49};
    const size_t WRITE_SIZE = sizeof(expected_write);
    const size_t EXPECTED_DATA_LEN = WRITE_SIZE;
    uint8_t READ_DATA [] = {0xAB, 0xBA};
    const size_t READ_SIZE = sizeof(READ_DATA);

    // the write-read transaction with repeated start:
    i2c_master_write_ExpectWithArrayAndReturn(&cmd_fix.dummy_handle, expected_write, WRITE_SIZE, EXPECTED_DATA_LEN, true, ESP_OK);
    i2c_master_start_ExpectAndReturn(&cmd_fix.dummy_handle, ESP_OK);
    i2c_master_write_byte_ExpectAndReturn(&cmd_fix.dummy_handle, 0x47 << 1 | I2C_MASTER_READ, true, ESP_OK);
    i2c_master_read_ExpectAndReturn(&cmd_fix.dummy_handle, nullptr, 2, i2c_ack_type_t::I2C_MASTER_LAST_NACK, ESP_OK);
    i2c_master_read_IgnoreArg_data();

    // note that this behavior is not entirely correct, in th real driver, only i2c_master_cmd_begin()
    // will actually read the data but for the tests it is enough for now
    i2c_master_read_ReturnArrayThruPtr_data(READ_DATA, READ_SIZE);
    i2c_master_stop_ExpectAndReturn(&cmd_fix.dummy_handle, ESP_OK);
    i2c_master_cmd_begin_ExpectAndReturn(0, &cmd_fix.dummy_handle, 1000 / portTICK_PERIOD_MS, ESP_OK);

    I2CMaster master(I2CNumber::I2C0(), SCL_GPIO(1), SDA_GPIO(2), Frequency(400000));
    vector<uint8_t> read_result = master.sync_transfer(I2CAddress(0x47), {0x47, 0x48, 0x49}, READ_SIZE);

    CHECK(read_result.size() == READ_SIZE);
    for (int i = 0; i < READ_SIZE; i++) {
        CHECK(read_result[i] == READ_DATA[i]);
    }
}

#if SOC_I2C_SUPPORT_SLAVE
TEST_CASE("I2CSlave parameter configuration fails")
{
    CMockFixture fix;
    i2c_param_config_ExpectAnyArgsAndReturn(ESP_FAIL);

    CHECK_THROWS_AS(I2CSlave(I2CNumber::I2C0(), SCL_GPIO(1), SDA_GPIO(2), I2CAddress(0x47), 64, 64), I2CException&);
}

TEST_CASE("I2CSlave driver installation fails")
{
    CMockFixture fix;
    i2c_param_config_IgnoreAndReturn(ESP_OK);
    i2c_driver_install_IgnoreAndReturn(ESP_FAIL);

    CHECK_THROWS_AS(I2CSlave (I2CNumber::I2C0(), SCL_GPIO(1), SDA_GPIO(2), I2CAddress(0x47), 64, 64), I2CException&);
}

TEST_CASE("I2CSlave calls driver functions correctly")
{
    CMockFixture fix;
    I2CSlaveFix slave_fix(CreateAnd::SUCCEED);

    I2CSlave slave(I2CNumber::I2C0(), SCL_GPIO(1), SDA_GPIO(2), I2CAddress(0x47), 64, 64);
}

TEST_CASE("I2CSlave write fails")
{
    CMockFixture fix;
    I2CSlaveFix slave_fix(CreateAnd::IGNORE);
    const uint8_t WRITE_BUFFER[] = {0xAB, 0xCD};
    const size_t WRITE_BUFFER_LEN = sizeof(WRITE_BUFFER);
    i2c_slave_write_buffer_ExpectAnyArgsAndReturn(-1);

    I2CSlave slave(I2CNumber::I2C0(), SCL_GPIO(3), SDA_GPIO(4), I2CAddress(0x47), 64, 64);
    CHECK(slave.write_raw(WRITE_BUFFER, WRITE_BUFFER_LEN, chrono::milliseconds(0)) == -1);
}

TEST_CASE("I2CSlave write calls driver functions correctly")
{
    CMockFixture fix;
    I2CSlaveFix slave_fix(CreateAnd::IGNORE);
    const uint8_t WRITE_BUFFER[] = {0xAB, 0xCD};
    const size_t WRITE_BUFFER_LEN = sizeof(WRITE_BUFFER);
    i2c_slave_write_buffer_ExpectWithArrayAndReturn(0,
            WRITE_BUFFER,
            WRITE_BUFFER_LEN,
            WRITE_BUFFER_LEN,
            500 / portTICK_PERIOD_MS,
            WRITE_BUFFER_LEN);

    I2CSlave slave(I2CNumber::I2C0(), SCL_GPIO(3), SDA_GPIO(4), I2CAddress(0x47), 64, 64);
    CHECK(slave.write_raw(WRITE_BUFFER, WRITE_BUFFER_LEN, chrono::milliseconds(500)) == WRITE_BUFFER_LEN);
}

TEST_CASE("I2CSlave read fails")
{
    CMockFixture fix;
    I2CSlaveFix slave_fix(CreateAnd::IGNORE);
    const size_t READ_BUFFER_LEN = 2;
    uint8_t read_buffer[READ_BUFFER_LEN];
    i2c_slave_read_buffer_ExpectAnyArgsAndReturn(-1);

    I2CSlave slave(I2CNumber::I2C0(), SCL_GPIO(3), SDA_GPIO(4), I2CAddress(0x47), 64, 64);
    CHECK(slave.read_raw(read_buffer, READ_BUFFER_LEN, chrono::milliseconds(0)) == -1);
}

TEST_CASE("I2CSlave read calls driver functions correctly")
{
    CMockFixture fix;
    I2CSlaveFix slave_fix(CreateAnd::IGNORE);
    uint8_t WRITE_BUFFER[] = {0xAB, 0xCD};
    const size_t BUFFER_LEN = sizeof(WRITE_BUFFER);
    uint8_t read_buffer[BUFFER_LEN];
    i2c_slave_read_buffer_ExpectAndReturn(0, read_buffer, BUFFER_LEN, 500 / portTICK_PERIOD_MS, BUFFER_LEN);
    i2c_slave_read_buffer_ReturnArrayThruPtr_data(WRITE_BUFFER, BUFFER_LEN);

    I2CSlave slave(I2CNumber::I2C0(), SCL_GPIO(3), SDA_GPIO(4), I2CAddress(0x47), 64, 64);
    CHECK(slave.read_raw(read_buffer, BUFFER_LEN, chrono::milliseconds(500)) == BUFFER_LEN);
    for (size_t i = 0; i < BUFFER_LEN; i++) {
        CHECK(read_buffer[i] == WRITE_BUFFER[i]);
    }
}
#endif // SOC_I2C_SUPPORT_SLAVE
