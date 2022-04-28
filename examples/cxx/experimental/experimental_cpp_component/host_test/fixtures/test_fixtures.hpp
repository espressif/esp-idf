/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
*/

#include "catch.hpp"
#include "gpio_cxx.hpp"
#include "driver/spi_master.h"
#include "spi_cxx.hpp"
#include "i2c_cxx.hpp"
extern "C" {
#include "Mockgpio.h"
#include "Mockspi_master.h"
#include "Mockspi_common.h"
#include "Mocki2c.h"
}

static const idf::GPIONum VALID_GPIO(18);

/**
 * Exception which is thrown if there is some internal cmock error which results in a
 * longjump to the location of a TEST_PROTECT() call.
 *
 * @note This is a temporary solution until there is a better integration of CATCH into CMock.
 *      Note also that usually there will be a segfault when cmock fails a second time.
 *      This means paying attention to the first error message is crucial for removing errors.
 */
class CMockException : public std::exception {
public:
    virtual ~CMockException() { }

    /**
     * @return A reminder to look at the actual cmock log.
     */
    virtual const char *what() const noexcept
    {
        return "CMock encountered an error. Look at the CMock log";
    }
};

/**
 * Helper macro for setting up a test protect call for CMock.
 *
 * This macro should be used at the beginning of any test cases
 * that uses generated CMock mock functions.
 * This is necessary because CMock uses longjmp which screws up C++ stacks and
 * also the CATCH mechanisms.
 *
 * @note This is a temporary solution until there is a better integration of CATCH into CMock.
 *      Note also that usually there will be a segfault when cmock fails a second time.
 *      This means paying attention to the first error message is crucial for removing errors.
 */
#define CMOCK_SETUP() \
    do { \
        if (!TEST_PROTECT()) {      \
            throw CMockException(); \
        }                           \
    } \
    while (0)

struct CMockFixture {
    CMockFixture()
    {
        CMOCK_SETUP();
    }

    ~CMockFixture()
    {
        // Verify that all expected methods have been called.
        Mockgpio_Verify();
        Mockspi_master_Verify();
        Mockspi_common_Verify();
    }
};

struct GPIOFixture : public CMockFixture {
    GPIOFixture(idf::GPIONum gpio_num = idf::GPIONum(18), gpio_mode_t mode = GPIO_MODE_OUTPUT)
        : CMockFixture(), num(gpio_num)
    {
        gpio_reset_pin_ExpectAndReturn(static_cast<gpio_num_t>(num.get_num()), ESP_OK);
        gpio_set_direction_ExpectAndReturn(static_cast<gpio_num_t>(num.get_num()), mode, ESP_OK);
    }

    idf::GPIONum num;
};

struct SPIFix;
struct SPIDevFix;
struct SPITransactionDescriptorFix;
struct SPITransactionTimeoutFix;
struct SPITransactionFix;

static SPIFix *g_fixture;
static SPIDevFix *g_dev_fixture;
static SPITransactionDescriptorFix *g_trans_desc_fixture;
static SPITransactionTimeoutFix *g_trans_timeout_fixture;
static SPITransactionFix *g_trans_fixture;

struct SPIFix : public CMockFixture {
    SPIFix(spi_host_device_t host_id = spi_host_device_t(1),
           uint32_t mosi = 1,
           uint32_t miso = 2,
           uint32_t sclk = 3) : CMockFixture(), bus_config() {
        bus_config.mosi_io_num = mosi;
        bus_config.miso_io_num = miso;
        bus_config.sclk_io_num = sclk;
        bus_config.quadwp_io_num = -1;
        bus_config.quadhd_io_num = -1;

        spi_bus_initialize_ExpectWithArrayAndReturn(host_id, &bus_config, 1, spi_common_dma_t::SPI_DMA_CH_AUTO, ESP_OK);
        spi_bus_free_ExpectAnyArgsAndReturn(ESP_OK);

        g_fixture = this;
    }

    ~SPIFix() {
        g_fixture = nullptr;
    }

    spi_bus_config_t bus_config;
};

struct QSPIFix : public SPIFix {
    QSPIFix(spi_host_device_t host_id = spi_host_device_t(1),
           uint32_t mosi = 1,
           uint32_t miso = 2,
           uint32_t sclk = 3,
            uint32_t wp = 4,
            uint32_t hd = 5) : SPIFix(host_id, mosi, miso, sclk)
    {
        bus_config.quadwp_io_num = wp;
        bus_config.quadhd_io_num = hd;
    }
};

enum class CreateAnd {
    FAIL,
    SUCCEED,
    IGNORE
};

struct SPIDevFix {
    SPIDevFix(CreateAnd flags)
        : dev_handle(reinterpret_cast<spi_device_handle_t>(47)),
        dev_config()
    {
        dev_config.spics_io_num = 4;
        if (flags == CreateAnd::FAIL) {
            spi_bus_add_device_ExpectAnyArgsAndReturn(ESP_FAIL);
        } else if (flags == CreateAnd::IGNORE) {
            spi_bus_add_device_IgnoreAndReturn(ESP_OK);
            spi_bus_remove_device_IgnoreAndReturn(ESP_OK);
        } else {
            spi_bus_add_device_AddCallback(add_dev_cb);
            spi_bus_add_device_ExpectAnyArgsAndReturn(ESP_OK);
            spi_bus_remove_device_ExpectAndReturn(dev_handle, ESP_OK);
        }

        g_dev_fixture = this;
    }

    ~SPIDevFix()
    {
        spi_bus_add_device_AddCallback(nullptr);
        g_dev_fixture = nullptr;
    }

    spi_device_handle_t dev_handle;
    spi_device_interface_config_t dev_config;

    static esp_err_t add_dev_cb(spi_host_device_t host_id,
            const spi_device_interface_config_t* dev_config,
            spi_device_handle_t* handle,
            int cmock_num_calls)
    {
        SPIDevFix *fix = static_cast<SPIDevFix*>(g_dev_fixture);
        *handle = fix->dev_handle;
        fix->dev_config = *dev_config;
        return ESP_OK;
    }
};

struct SPITransactionFix {
    SPITransactionFix(esp_err_t get_trans_return = ESP_OK) : get_transaction_return(get_trans_return)
    {
        spi_device_queue_trans_AddCallback(queue_trans_cb);
        spi_device_get_trans_result_AddCallback(get_trans_result_cb);

        spi_device_queue_trans_ExpectAnyArgsAndReturn(ESP_OK);
        spi_device_get_trans_result_ExpectAnyArgsAndReturn(get_trans_return);

        g_trans_fixture = this;
    }

    ~SPITransactionFix()
    {
        spi_device_get_trans_result_AddCallback(nullptr);
        spi_device_queue_trans_AddCallback(nullptr);
        g_trans_fixture = nullptr;
    }

    static esp_err_t queue_trans_cb(spi_device_handle_t handle,
            spi_transaction_t* trans_desc,
            TickType_t ticks_to_wait,
            int cmock_num_calls)
    {
        SPITransactionFix *fix = static_cast<SPITransactionFix*> (g_trans_fixture);
        fix->orig_trans = trans_desc;
        return ESP_OK;
    }

    static esp_err_t get_trans_result_cb(spi_device_handle_t handle,
            spi_transaction_t** trans_desc,
            TickType_t ticks_to_wait,
            int cmock_num_calls)
    {
        SPITransactionFix *fix = static_cast<SPITransactionFix*> (g_trans_fixture);

        *trans_desc = fix->orig_trans;

        return fix->get_transaction_return;
    }

    esp_err_t get_transaction_return;
    spi_transaction_t *orig_trans;
};

struct SPITransactionDescriptorFix {
    SPITransactionDescriptorFix(size_t size = 1, bool ignore_handle = false, TickType_t wait_time = portMAX_DELAY)
        : size(size), handle(reinterpret_cast<spi_device_handle_t>(0x01020304))
    {
        spi_device_queue_trans_AddCallback(queue_trans_cb);
        spi_device_get_trans_result_AddCallback(get_trans_result_cb);

        spi_device_acquire_bus_ExpectAndReturn(handle, portMAX_DELAY, ESP_OK);
        if (ignore_handle) {
            spi_device_acquire_bus_IgnoreArg_device();
        }
        spi_device_queue_trans_ExpectAndReturn(handle, nullptr, 0, ESP_OK);
        spi_device_queue_trans_IgnoreArg_trans_desc();
        if (ignore_handle) {
            spi_device_queue_trans_IgnoreArg_handle();
        }

        spi_device_get_trans_result_ExpectAndReturn(handle, nullptr, wait_time, ESP_OK);
        spi_device_get_trans_result_IgnoreArg_trans_desc();
        if (ignore_handle) {
            spi_device_get_trans_result_IgnoreArg_handle();
        }
        spi_device_release_bus_ExpectAnyArgs();

        g_trans_desc_fixture = this;
    }

    ~SPITransactionDescriptorFix()
    {
        spi_device_get_trans_result_AddCallback(nullptr);
        spi_device_queue_trans_AddCallback(nullptr);
        g_trans_desc_fixture = nullptr;
    }

    static esp_err_t queue_trans_cb(spi_device_handle_t handle,
            spi_transaction_t* trans_desc,
            TickType_t ticks_to_wait,
            int cmock_num_calls)
    {
        SPITransactionDescriptorFix *fix = static_cast<SPITransactionDescriptorFix*> (g_trans_desc_fixture);
        fix->orig_trans = trans_desc;
        return ESP_OK;
    }

    static esp_err_t get_trans_result_cb(spi_device_handle_t handle,
            spi_transaction_t** trans_desc,
            TickType_t ticks_to_wait,
            int cmock_num_calls)
    {
        SPITransactionDescriptorFix *fix = static_cast<SPITransactionDescriptorFix*> (g_trans_desc_fixture);

        for (int i = 0; i < fix->size; i++) {
            static_cast<uint8_t*>(fix->orig_trans->rx_buffer)[i] = fix->rx_data[i];
        }

        *trans_desc = fix->orig_trans;

        return ESP_OK;
    }

    size_t size;
    spi_transaction_t *orig_trans;
    spi_device_handle_t handle;
    std::vector<uint8_t> tx_data;
    std::vector<uint8_t> rx_data;
};

struct I2CMasterFix {
    I2CMasterFix(i2c_port_t port_arg = 0) : i2c_conf(), port(port_arg)
    {
        i2c_conf.mode = i2c_mode_t::I2C_MODE_MASTER;
        i2c_conf.sda_io_num = 2;
        i2c_conf.scl_io_num = 1;
        i2c_conf.sda_pullup_en = true;
        i2c_conf.scl_pullup_en = true;
        i2c_conf.master.clk_speed = 400000;
        i2c_conf.clk_flags = 0;
        i2c_param_config_ExpectWithArrayAndReturn(i2c_port_t(0), &i2c_conf, 1, ESP_OK);
        i2c_driver_install_ExpectAndReturn(i2c_port_t(0), i2c_mode_t::I2C_MODE_MASTER, 0, 0, 0, ESP_OK);
        i2c_driver_delete_ExpectAndReturn(i2c_port_t(0), ESP_OK);
    }

    i2c_config_t i2c_conf;
    i2c_port_t port;
};

#if CONFIG_SOC_I2C_SUPPORT_SLAVE
struct I2CSlaveFix {
    I2CSlaveFix(CreateAnd flags, i2c_port_t port_arg = 0, size_t buffer_size = 64) : i2c_conf(), port(port_arg)
    {
        if (flags == CreateAnd::SUCCEED) {
            i2c_conf.mode = i2c_mode_t::I2C_MODE_SLAVE;
            i2c_conf.sda_io_num = 2;
            i2c_conf.scl_io_num = 1;
            i2c_conf.sda_pullup_en = true;
            i2c_conf.scl_pullup_en = true;
            i2c_conf.slave.addr_10bit_en = 0;
            i2c_conf.slave.slave_addr = 0x47;
            i2c_param_config_ExpectWithArrayAndReturn(port, &i2c_conf, 1, ESP_OK);
            i2c_driver_install_ExpectAndReturn(port, i2c_mode_t::I2C_MODE_SLAVE, buffer_size, buffer_size, 0, ESP_OK);
            i2c_driver_delete_ExpectAndReturn(port, ESP_OK);
        } else if (flags == CreateAnd::IGNORE) {
            i2c_param_config_IgnoreAndReturn(ESP_OK);
            i2c_driver_install_IgnoreAndReturn(ESP_OK);
            i2c_driver_delete_IgnoreAndReturn(ESP_OK);
        } else {
            throw idf::I2CException(ESP_ERR_INVALID_ARG);
        }
    }

    i2c_config_t i2c_conf;
    i2c_port_t port;
};
#endif // CONFIG_SOC_I2C_SUPPORT_SLAVE

struct I2CCmdLinkFix
{
    I2CCmdLinkFix(uint8_t expected_addr, i2c_rw_t type = I2C_MASTER_WRITE) : dummy_handle(reinterpret_cast<i2c_cmd_handle_t>(0xbeef))
    {
        i2c_cmd_link_create_ExpectAndReturn(&dummy_handle);
        i2c_master_start_ExpectAndReturn(&dummy_handle, ESP_OK);
        i2c_master_write_byte_ExpectAndReturn(&dummy_handle, expected_addr << 1 | type, true, ESP_OK);
        i2c_cmd_link_delete_Expect(&dummy_handle);
    }

    i2c_cmd_handle_t dummy_handle;
};
