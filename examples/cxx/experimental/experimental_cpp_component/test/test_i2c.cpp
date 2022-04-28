/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_cxx.hpp"
#include <limits>
#include <stdio.h>

#include <iostream>
#include "test_utils.h" // unity_send_signal

#ifdef __cpp_exceptions
#include "i2c_cxx.hpp"
#include "driver/i2c.h"

using namespace std;
using namespace idf;

#define ADDR 0x47

#define MAGIC_TEST_NUMBER 47

static constexpr I2CNumber I2C_SLAVE_NUM(I2CNumber::I2C0()); /*!<I2C port number for slave dev */
#if CONFIG_IDF_TARGET_ESP32C3
#define I2C_SLAVE_SCL_IO     5     /*!<gpio number for i2c slave clock  */
#define I2C_SLAVE_SDA_IO     6     /*!<gpio number for i2c slave data */
#else
#define I2C_SLAVE_SCL_IO     19    /*!<gpio number for i2c slave clock  */
#define I2C_SLAVE_SDA_IO     18    /*!<gpio number for i2c slave data */
#endif

#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C2
static constexpr I2CNumber I2C_MASTER_NUM(I2CNumber::I2C0()); /*!< I2C port number for master dev */
#define I2C_MASTER_SCL_IO     5     /*!<gpio number for i2c master clock  */
#define I2C_MASTER_SDA_IO     6     /*!<gpio number for i2c master data */
#else
static constexpr I2CNumber I2C_MASTER_NUM(I2CNumber::I2C1()); /*!< I2C port number for master dev */
#define I2C_MASTER_SCL_IO    19    /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO    18   /*!< gpio number for I2C master data  */
#endif

struct MasterFixture {
    MasterFixture(const vector<uint8_t> &data_arg = {47u}) :
        master(new I2CMaster(I2CNumber(I2C_MASTER_NUM),
                SCL_GPIO(I2C_MASTER_SCL_IO),
                SDA_GPIO(I2C_MASTER_SDA_IO),
                Frequency(400000))),
        data(data_arg) { }

    std::shared_ptr<I2CMaster> master;
    vector<uint8_t> data;
};

// TODO The I2C driver tests are disabled, so disable them here, too. Probably due to no runners.
#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32S2, ESP32S3, ESP32C2)

static void i2c_slave_read_raw_byte(void)
{
    I2CSlave slave(I2CNumber(I2C_SLAVE_NUM), SCL_GPIO(I2C_SLAVE_SCL_IO), SDA_GPIO(I2C_SLAVE_SDA_IO), I2CAddress(ADDR), 512, 512);
    uint8_t buffer = 0;

    unity_send_signal("slave init");
    unity_wait_for_signal("master write");

    TEST_ASSERT_EQUAL(1, slave.read_raw(&buffer, 1, chrono::milliseconds(1000)));

    TEST_ASSERT_EQUAL(MAGIC_TEST_NUMBER, buffer);
}

static void i2c_slave_write_raw_byte(void)
{
    I2CSlave slave(I2CNumber(I2C_SLAVE_NUM), SCL_GPIO(I2C_SLAVE_SCL_IO), SDA_GPIO(I2C_SLAVE_SDA_IO), I2CAddress(ADDR), 512, 512);
    uint8_t WRITE_BUFFER = MAGIC_TEST_NUMBER;

    unity_wait_for_signal("master init");

    TEST_ASSERT_EQUAL(1, slave.write_raw(&WRITE_BUFFER, 1, chrono::milliseconds(1000)));

    unity_send_signal("slave write");

    // This last synchronization is necessary to prevent slave from going out of scope hence de-initializing already
    // before master has read
    unity_wait_for_signal("master read done");
}

static void i2c_slave_read_multiple_raw_bytes(void)
{
    I2CSlave slave(I2CNumber(I2C_SLAVE_NUM), SCL_GPIO(I2C_SLAVE_SCL_IO), SDA_GPIO(I2C_SLAVE_SDA_IO), I2CAddress(ADDR), 512, 512);
    uint8_t buffer [8] = {};

    unity_send_signal("slave init");
    unity_wait_for_signal("master write");

    TEST_ASSERT_EQUAL(8, slave.read_raw(buffer, 8, chrono::milliseconds(1000)));

    for (int i = 0; i < 8; i++) {
        TEST_ASSERT_EQUAL(i, buffer[i]);
    }
}

static void i2c_slave_write_multiple_raw_bytes(void)
{
    I2CSlave slave(I2CNumber(I2C_SLAVE_NUM), SCL_GPIO(I2C_SLAVE_SCL_IO), SDA_GPIO(I2C_SLAVE_SDA_IO), I2CAddress(ADDR), 512, 512);
    uint8_t WRITE_BUFFER [8] = {0, 1, 2, 3, 4, 5, 6, 7};

    unity_wait_for_signal("master init");

    TEST_ASSERT_EQUAL(8, slave.write_raw(WRITE_BUFFER, 8, chrono::milliseconds(1000)));

    unity_send_signal("slave write");
    unity_wait_for_signal("master read done");
}

static void i2c_slave_composed_trans(void)
{
    I2CSlave slave(I2CNumber(I2C_SLAVE_NUM), SCL_GPIO(I2C_SLAVE_SCL_IO), SDA_GPIO(I2C_SLAVE_SDA_IO), I2CAddress(ADDR), 512, 512);
    size_t BUF_SIZE = 2;
    const uint8_t SLAVE_WRITE_BUFFER [BUF_SIZE] = {0xde, 0xad};
    uint8_t slave_read_buffer = 0;

    unity_send_signal("slave init");

    TEST_ASSERT_EQUAL(BUF_SIZE, slave.write_raw(SLAVE_WRITE_BUFFER, BUF_SIZE, chrono::milliseconds(1000)));

    unity_wait_for_signal("master transfer");

    TEST_ASSERT_EQUAL(1, slave.read_raw(&slave_read_buffer, 1, chrono::milliseconds(1000)));

    TEST_ASSERT_EQUAL(MAGIC_TEST_NUMBER, slave_read_buffer);
}

static void i2c_master_read_raw_byte(void)
{
    MasterFixture fix;

    unity_send_signal("master init");
    unity_wait_for_signal("slave write");

    std::shared_ptr<I2CRead> reader(new I2CRead(1));

    future<vector<uint8_t> > fut = fix.master->transfer(I2CAddress(ADDR), reader);

    vector<uint8_t> data;
    data = fut.get();
    unity_send_signal("master read done");

    TEST_ASSERT_EQUAL(1, data.size());
    TEST_ASSERT_EQUAL(MAGIC_TEST_NUMBER, data[0]);
}

TEST_CASE_MULTIPLE_DEVICES("I2CMaster read one byte", "[cxx i2c][test_env=UT_T2_I2C][timeout=150]",
        i2c_master_read_raw_byte, i2c_slave_write_raw_byte);

static void i2c_master_write_raw_byte(void)
{
    MasterFixture fix;

    unity_wait_for_signal("slave init");

    std::shared_ptr<I2CWrite> writer(new I2CWrite(fix.data));
    future<void> fut = fix.master->transfer(I2CAddress(ADDR), writer);

    fut.get();
    unity_send_signal("master write");
}

TEST_CASE_MULTIPLE_DEVICES("I2CMaster write one byte", "[cxx i2c][test_env=UT_T2_I2C][timeout=150]",
        i2c_master_write_raw_byte, i2c_slave_read_raw_byte);

static void i2c_master_read_multiple_raw_bytes(void)
{
    MasterFixture fix;

    unity_send_signal("master init");
    unity_wait_for_signal("slave write");

    std::shared_ptr<I2CRead> reader(new I2CRead(8));

    future<vector<uint8_t> > fut = fix.master->transfer(I2CAddress(ADDR), reader);

    vector<uint8_t> data = fut.get();
    unity_send_signal("master read done");

    TEST_ASSERT_EQUAL(8, data.size());
    for (int i = 0; i < 8; i++) {
        TEST_ASSERT_EQUAL(i, data[i]);
    }
}

TEST_CASE_MULTIPLE_DEVICES("I2CMaster read multiple bytes", "[cxx i2c][test_env=UT_T2_I2C][timeout=150]",
        i2c_master_read_multiple_raw_bytes, i2c_slave_write_multiple_raw_bytes);

static void i2c_master_write_multiple_raw_bytes(void)
{
    MasterFixture fix({0, 1, 2, 3, 4, 5, 6, 7});

    unity_wait_for_signal("slave init");

    std::shared_ptr<I2CWrite> writer(new I2CWrite(fix.data));
    future<void> fut = fix.master->transfer(I2CAddress(ADDR), writer);

    fut.get();
    unity_send_signal("master write");
}

TEST_CASE_MULTIPLE_DEVICES("I2CMaster write multiple bytes", "[cxx i2c][test_env=UT_T2_I2C][timeout=150]",
        i2c_master_write_multiple_raw_bytes, i2c_slave_read_multiple_raw_bytes);

static void i2c_master_sync_transfer(void)
{
    MasterFixture fix;
    size_t READ_SIZE = 2;
    const uint8_t DESIRED_READ [READ_SIZE] = {0xde, 0xad};

    unity_wait_for_signal("slave init");

    vector<uint8_t> read_data = fix.master->sync_transfer(I2CAddress(ADDR), fix.data, READ_SIZE);

    unity_send_signal("master transfer");
    TEST_ASSERT_EQUAL(READ_SIZE, read_data.size());
    for (int i = 0; i < READ_SIZE; i++) {
        TEST_ASSERT_EQUAL(DESIRED_READ[i], read_data[i]);
    }
}

TEST_CASE_MULTIPLE_DEVICES("I2CMaster sync transfer", "[cxx i2c][test_env=UT_T2_I2C][timeout=150]",
        i2c_master_sync_transfer, i2c_slave_composed_trans);

static void i2c_master_composed_trans(void)
{
    MasterFixture fix;
    size_t BUF_SIZE = 2;
    const uint8_t SLAVE_WRITE_BUFFER [BUF_SIZE] = {0xde, 0xad};

    std::shared_ptr<I2CComposed> composed_transfer(new I2CComposed);
    composed_transfer->add_write({47u});
    composed_transfer->add_read(BUF_SIZE);

    unity_wait_for_signal("slave init");

    future<vector<vector<uint8_t> > > result = fix.master->transfer(I2CAddress(ADDR), composed_transfer);

    unity_send_signal("master transfer");

    vector<vector<uint8_t> > read_data = result.get();

    TEST_ASSERT_EQUAL(1, read_data.size());
    TEST_ASSERT_EQUAL(2, read_data[0].size());
    for (int i = 0; i < BUF_SIZE; i++) {
        TEST_ASSERT_EQUAL(SLAVE_WRITE_BUFFER[i], read_data[0][i]);
    }
}

TEST_CASE_MULTIPLE_DEVICES("I2CMaster Composed transfer", "[cxx i2c][test_env=UT_T2_I2C][timeout=150]",
        i2c_master_composed_trans, i2c_slave_composed_trans);

#endif  //TEMPORARY_DISABLED_FOR_TARGETS(ESP32S2, ESP32S3)
#endif // __cpp_exceptions
