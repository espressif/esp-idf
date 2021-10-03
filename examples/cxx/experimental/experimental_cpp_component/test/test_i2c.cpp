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

#include "unity.h"
#include "unity_cxx.hpp"
#include <limits>
#include <stdio.h>

#include <iostream>
#include "test_utils.h" // unity_send_signal

#ifdef __cpp_exceptions
#include "i2c_cxx.hpp"

using namespace std;
using namespace idf;

#define TAG "I2C Test"
#define ADDR 0x47

#define MAGIC_TEST_NUMBER 47

#define I2C_SLAVE_NUM I2C_NUM_0    /*!<I2C port number for slave dev */
#if CONFIG_IDF_TARGET_ESP32C3
#define I2C_SLAVE_SCL_IO     5     /*!<gpio number for i2c slave clock  */
#define I2C_SLAVE_SDA_IO     6     /*!<gpio number for i2c slave data */
#else
#define I2C_SLAVE_SCL_IO     19    /*!<gpio number for i2c slave clock  */
#define I2C_SLAVE_SDA_IO     18    /*!<gpio number for i2c slave data */
#endif

#if CONFIG_IDF_TARGET_ESP32C3
#define I2C_MASTER_NUM I2C_NUM_0   /*!< I2C port number for master dev */
#define I2C_MASTER_SCL_IO     5     /*!<gpio number for i2c master clock  */
#define I2C_MASTER_SDA_IO     6     /*!<gpio number for i2c master data */
#else
#define I2C_MASTER_NUM I2C_NUM_1   /*!< I2C port number for master dev */
#define I2C_MASTER_SCL_IO    19    /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO    18   /*!< gpio number for I2C master data  */
#endif

struct MasterFixture {
    MasterFixture(const vector<uint8_t> &data_arg = {47u}) :
        master(new I2CMaster(I2C_MASTER_NUM, I2C_MASTER_SCL_IO, I2C_MASTER_SDA_IO, 400000)),
        data(data_arg) { }

    std::shared_ptr<I2CMaster> master;
    vector<uint8_t> data;
};

TEST_CASE("I2CMaster GPIO out of range", "[cxx i2c][leaks=300]")
{
    TEST_THROW(I2CMaster(0, 255, 255, 400000), I2CException);
}

TEST_CASE("I2CMaster SDA and SCL equal", "[cxx i2c][leaks=300]")
{
    TEST_THROW(I2CMaster(0, 0, 0, 400000), I2CException);
}

TEST_CASE("I2Transfer timeout", "[cxx i2c][leaks=300]")
{
    std::vector<uint8_t> data = {MAGIC_TEST_NUMBER};

    // I2CWrite directly inherits from I2CTransfer; it's representative for I2CRead and I2CComposed, too.
    I2CWrite writer(data, chrono::milliseconds(50));

    TEST_THROW(writer.do_transfer(I2C_MASTER_NUM, ADDR), I2CTransferException);
}

// TODO The I2C driver tests are disabled, so disable them here, too. Probably due to no runners.
#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32S2, ESP32S3)

static void i2c_slave_read_raw_byte(void)
{
    I2CSlave slave(I2C_SLAVE_NUM, I2C_SLAVE_SCL_IO, I2C_SLAVE_SDA_IO, ADDR, 512, 512);
    uint8_t buffer = 0;

    unity_send_signal("slave init");
    unity_wait_for_signal("master write");

    TEST_ASSERT_EQUAL(1, slave.read_raw(&buffer, 1, chrono::milliseconds(1000)));

    TEST_ASSERT_EQUAL(MAGIC_TEST_NUMBER, buffer);
}

static void i2c_slave_write_raw_byte(void)
{
    I2CSlave slave(I2C_SLAVE_NUM, I2C_SLAVE_SCL_IO, I2C_SLAVE_SDA_IO, ADDR, 512, 512);
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
    I2CSlave slave(I2C_SLAVE_NUM, I2C_SLAVE_SCL_IO, I2C_SLAVE_SDA_IO, ADDR, 512, 512);
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
    I2CSlave slave(I2C_SLAVE_NUM, I2C_SLAVE_SCL_IO, I2C_SLAVE_SDA_IO, ADDR, 512, 512);
    uint8_t WRITE_BUFFER [8] = {0, 1, 2, 3, 4, 5, 6, 7};

    unity_wait_for_signal("master init");

    TEST_ASSERT_EQUAL(8, slave.write_raw(WRITE_BUFFER, 8, chrono::milliseconds(1000)));

    unity_send_signal("slave write");
    unity_wait_for_signal("master read done");
}

static void i2c_slave_composed_trans(void)
{
    I2CSlave slave(I2C_SLAVE_NUM, I2C_SLAVE_SCL_IO, I2C_SLAVE_SDA_IO, ADDR, 512, 512);
    size_t BUF_SIZE = 2;
    const uint8_t SLAVE_WRITE_BUFFER [BUF_SIZE] = {0xde, 0xad};
    uint8_t slave_read_buffer = 0;

    unity_send_signal("slave init");

    TEST_ASSERT_EQUAL(BUF_SIZE, slave.write_raw(SLAVE_WRITE_BUFFER, BUF_SIZE, chrono::milliseconds(1000)));

    unity_wait_for_signal("master transfer");

    TEST_ASSERT_EQUAL(1, slave.read_raw(&slave_read_buffer, 1, chrono::milliseconds(1000)));

    TEST_ASSERT_EQUAL(MAGIC_TEST_NUMBER, slave_read_buffer);
}

static void i2c_I2CRead(void)
{
    // here only to install/uninstall driver
    MasterFixture fix;

    unity_send_signal("master init");
    unity_wait_for_signal("slave write");

    I2CRead reader(1);
    vector<uint8_t> data = reader.do_transfer(I2C_MASTER_NUM, ADDR);
    unity_send_signal("master read done");

    TEST_ASSERT_EQUAL(1, data.size());
    TEST_ASSERT_EQUAL(MAGIC_TEST_NUMBER, data[0]);
}

TEST_CASE_MULTIPLE_DEVICES("I2CRead do_transfer", "[cxx i2c][test_env=UT_T2_I2C][timeout=150]",
        i2c_I2CRead, i2c_slave_write_raw_byte);

static void i2c_I2CWrite(void)
{
    MasterFixture fix;

    I2CWrite writer(fix.data);

    unity_wait_for_signal("slave init");

    writer.do_transfer(I2C_MASTER_NUM, ADDR);

    unity_send_signal("master write");
}

TEST_CASE_MULTIPLE_DEVICES("I2CWrite do_transfer", "[cxx i2c][test_env=UT_T2_I2C][timeout=150]",
        i2c_I2CWrite, i2c_slave_read_raw_byte);

static void i2c_master_read_raw_byte(void)
{
    MasterFixture fix;

    unity_send_signal("master init");
    unity_wait_for_signal("slave write");

    std::shared_ptr<I2CRead> reader(new I2CRead(1));

    future<vector<uint8_t> > fut = fix.master->transfer(reader, ADDR);

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
    future<void> fut = fix.master->transfer(writer, ADDR);

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

    future<vector<uint8_t> > fut = fix.master->transfer(reader, ADDR);

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
    future<void> fut = fix.master->transfer(writer, ADDR);

    fut.get();
    unity_send_signal("master write");
}

TEST_CASE_MULTIPLE_DEVICES("I2CMaster write multiple bytes", "[cxx i2c][test_env=UT_T2_I2C][timeout=150]",
        i2c_master_write_multiple_raw_bytes, i2c_slave_read_multiple_raw_bytes);

static void i2c_master_sync_read(void)
{
    MasterFixture fix;

    unity_send_signal("master init");
    unity_wait_for_signal("slave write");

    vector<uint8_t> data = fix.master->sync_read(ADDR, 1);

    unity_send_signal("master read done");

    TEST_ASSERT_EQUAL(1, data.size());
    TEST_ASSERT_EQUAL(MAGIC_TEST_NUMBER, data[0]);
}

TEST_CASE_MULTIPLE_DEVICES("I2CMaster sync read", "[cxx i2c][test_env=UT_T2_I2C][timeout=150]",
        i2c_master_sync_read, i2c_slave_write_raw_byte);

static void i2c_master_sync_write(void)
{
    MasterFixture fix;

    unity_wait_for_signal("slave init");

    fix.master->sync_write(ADDR, fix.data);

    unity_send_signal("master write");
}

TEST_CASE_MULTIPLE_DEVICES("I2CMaster sync write", "[cxx i2c][test_env=UT_T2_I2C][timeout=150]",
        i2c_master_sync_write, i2c_slave_read_raw_byte);

static void i2c_master_sync_transfer(void)
{
    MasterFixture fix;
    size_t READ_SIZE = 2;
    const uint8_t DESIRED_READ [READ_SIZE] = {0xde, 0xad};

    unity_wait_for_signal("slave init");

    vector<uint8_t> read_data = fix.master->sync_transfer(ADDR, fix.data, READ_SIZE);

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

    future<vector<vector<uint8_t> > > result = fix.master->transfer(composed_transfer, ADDR);

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

static void i2c_slave_write_multiple_raw_bytes_twice(void)
{
    I2CSlave slave(I2C_SLAVE_NUM, I2C_SLAVE_SCL_IO, I2C_SLAVE_SDA_IO, ADDR, 512, 512);
    const size_t BUF_SIZE = 8;
    uint8_t WRITE_BUFFER [BUF_SIZE] = {0, 1, 2, 3, 4, 5, 6, 7};

    unity_wait_for_signal("master init");

    TEST_ASSERT_EQUAL(BUF_SIZE, slave.write_raw(WRITE_BUFFER, BUF_SIZE, chrono::milliseconds(1000)));
    TEST_ASSERT_EQUAL(BUF_SIZE, slave.write_raw(WRITE_BUFFER, BUF_SIZE, chrono::milliseconds(1000)));

    unity_send_signal("slave write");
    unity_wait_for_signal("master read done");
}

static void i2c_master_reuse_read_multiple_raw_bytes(void)
{
    MasterFixture fix;

    unity_send_signal("master init");
    unity_wait_for_signal("slave write");
    const size_t BUF_SIZE = 8;
#if !CONFIG_IDF_TARGET_ESP32C3
    std::shared_ptr<I2CRead> reader(new I2CRead(BUF_SIZE));

    future<vector<uint8_t> > fut;
    fut = fix.master->transfer(reader, ADDR);
    vector<uint8_t> data1 = fut.get();

    fut = fix.master->transfer(reader, ADDR);
    vector<uint8_t> data2 = fut.get();

    unity_send_signal("master read done");

    TEST_ASSERT_EQUAL(BUF_SIZE, data1.size());
    TEST_ASSERT_EQUAL(BUF_SIZE, data2.size());
    for (int i = 0; i < BUF_SIZE; i++) {
        TEST_ASSERT_EQUAL(i, data1[i]);
        TEST_ASSERT_EQUAL(i, data2[i]);
    }
#else // Cannot read twice because the `prefetch` behaviour on C3.
    std::shared_ptr<I2CRead> reader(new I2CRead(BUF_SIZE * 2));

    future<vector<uint8_t> > fut;
    fut = fix.master->transfer(reader, ADDR);
    vector<uint8_t> data = fut.get();

    unity_send_signal("master read done");

    TEST_ASSERT_EQUAL(BUF_SIZE * 2, data.size());
    for (int i = 0; i < BUF_SIZE; i++) {
        TEST_ASSERT_EQUAL((i % BUF_SIZE), data[i]);
    }

#endif // !CONFIG_IDF_TARGET_ESP32C3

}

TEST_CASE_MULTIPLE_DEVICES("I2CMaster reuse read multiple bytes", "[cxx i2c][test_env=UT_T2_I2C][timeout=150]",
        i2c_master_reuse_read_multiple_raw_bytes, i2c_slave_write_multiple_raw_bytes_twice);

static void i2c_slave_read_multiple_raw_bytes_twice(void)
{
    I2CSlave slave(I2C_SLAVE_NUM, I2C_SLAVE_SCL_IO, I2C_SLAVE_SDA_IO, ADDR, 512, 512);
    const size_t BUF_SIZE = 8;
    uint8_t buffer1 [BUF_SIZE] = {};
    uint8_t buffer2 [BUF_SIZE] = {};

    unity_send_signal("slave init");
    unity_wait_for_signal("master write");

    TEST_ASSERT_EQUAL(BUF_SIZE, slave.read_raw(buffer1, BUF_SIZE, chrono::milliseconds(1000)));
    TEST_ASSERT_EQUAL(BUF_SIZE, slave.read_raw(buffer2, BUF_SIZE, chrono::milliseconds(1000)));

    for (int i = 0; i < BUF_SIZE; i++) {
        TEST_ASSERT_EQUAL(i, buffer1[i]);
        TEST_ASSERT_EQUAL(i, buffer2[i]);
    }
}

static void i2c_master_reuse_write_multiple_raw_bytes(void)
{
    MasterFixture fix({0, 1, 2, 3, 4, 5, 6, 7});

    unity_wait_for_signal("slave init");

    std::shared_ptr<I2CWrite> writer(new I2CWrite(fix.data));
    future<void> fut;
    fut = fix.master->transfer(writer, ADDR);
    fut.get();

    fut = fix.master->transfer(writer, ADDR);
    fut.get();

    unity_send_signal("master write");
}

TEST_CASE_MULTIPLE_DEVICES("I2CMaster reuse write multiple bytes", "[cxx i2c][test_env=UT_T2_I2C][timeout=150]",
        i2c_master_reuse_write_multiple_raw_bytes, i2c_slave_read_multiple_raw_bytes_twice);

static void i2c_slave_composed_trans_twice(void)
{
    I2CSlave slave(I2C_SLAVE_NUM, I2C_SLAVE_SCL_IO, I2C_SLAVE_SDA_IO, ADDR, 512, 512);
    size_t BUF_SIZE = 2;
    const uint8_t SLAVE_WRITE_BUFFER1 [BUF_SIZE] = {0xde, 0xad};
    const uint8_t SLAVE_WRITE_BUFFER2 [BUF_SIZE] = {0xbe, 0xef};
    uint8_t slave_read_buffer = 0;

    unity_send_signal("slave init");

    TEST_ASSERT_EQUAL(BUF_SIZE, slave.write_raw(SLAVE_WRITE_BUFFER1, BUF_SIZE, chrono::milliseconds(1000)));
    TEST_ASSERT_EQUAL(BUF_SIZE, slave.write_raw(SLAVE_WRITE_BUFFER2, BUF_SIZE, chrono::milliseconds(1000)));

    unity_wait_for_signal("master transfer");

    TEST_ASSERT_EQUAL(1, slave.read_raw(&slave_read_buffer, 1, chrono::milliseconds(1000)));
    TEST_ASSERT_EQUAL(MAGIC_TEST_NUMBER, slave_read_buffer);
#if !CONFIG_IDF_TARGET_ESP32C3
    TEST_ASSERT_EQUAL(1, slave.read_raw(&slave_read_buffer, 1, chrono::milliseconds(1000)));
    TEST_ASSERT_EQUAL(MAGIC_TEST_NUMBER, slave_read_buffer);
#endif // !CONFIG_IDF_TARGET_ESP32C3
}

static void i2c_master_reuse_composed_trans(void)
{
    MasterFixture fix;
    size_t BUF_SIZE = 2;
    const uint8_t SLAVE_WRITE_BUFFER1 [BUF_SIZE] = {0xde, 0xad};
    const uint8_t SLAVE_WRITE_BUFFER2 [BUF_SIZE] = {0xbe, 0xef};

    std::shared_ptr<I2CComposed> composed_transfer(new I2CComposed);
    composed_transfer->add_write({47u});
#if !CONFIG_IDF_TARGET_ESP32C3
    composed_transfer->add_read(BUF_SIZE);

    unity_wait_for_signal("slave init");

    vector<vector<uint8_t> > read_data1 = fix.master->transfer(composed_transfer, ADDR).get();

    vector<vector<uint8_t> > read_data2 = fix.master->transfer(composed_transfer, ADDR).get();

    unity_send_signal("master transfer");

    TEST_ASSERT_EQUAL(1, read_data1.size());
    TEST_ASSERT_EQUAL(2, read_data1[0].size());
    TEST_ASSERT_EQUAL(1, read_data2.size());
    TEST_ASSERT_EQUAL(2, read_data2[0].size());

    for (int i = 0; i < BUF_SIZE; i++) {
        TEST_ASSERT_EQUAL(SLAVE_WRITE_BUFFER1[i], read_data1[0][i]);
        TEST_ASSERT_EQUAL(SLAVE_WRITE_BUFFER2[i], read_data2[0][i]);
    }
#else // Cannot read twice because the `prefetch` behaviour on C3.
    composed_transfer->add_read(BUF_SIZE * 2);

    unity_wait_for_signal("slave init");

    vector<vector<uint8_t> > read_data = fix.master->transfer(composed_transfer, ADDR).get();

    unity_send_signal("master transfer");

    TEST_ASSERT_EQUAL(1, read_data.size());
    TEST_ASSERT_EQUAL(4, read_data[0].size());


    for (int i = 0; i < BUF_SIZE; i++) {
        TEST_ASSERT_EQUAL(SLAVE_WRITE_BUFFER1[i], read_data[0][i]);
    }
    for (int i = BUF_SIZE; i < BUF_SIZE * 2; i++) {
        TEST_ASSERT_EQUAL(SLAVE_WRITE_BUFFER2[i - BUF_SIZE], read_data[0][i]);
    }

#endif //!CONFIG_IDF_TARGET_ESP32C3
}

TEST_CASE_MULTIPLE_DEVICES("I2CMaster reuse composed transfer", "[cxx i2c][test_env=UT_T2_I2C][timeout=150]",
        i2c_master_reuse_composed_trans, i2c_slave_composed_trans_twice);

#endif  //TEMPORARY_DISABLED_FOR_TARGETS(ESP32S2, ESP32S3)
#endif // __cpp_exceptions
