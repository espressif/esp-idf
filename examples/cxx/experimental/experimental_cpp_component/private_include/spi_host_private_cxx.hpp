/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * The code in this file includes driver headers directly, hence it's a private include.
 * It should only be used in C++ source files, while header files use forward declarations of the types.
 * This way, public headers don't need to depend on (i.e. include) driver headers.
 */

#ifdef __cpp_exceptions

#include "hal/spi_types.h"
#include "driver/spi_master.h"

using namespace std;

namespace idf {

#define SPI_CHECK_THROW(err) CHECK_THROW_SPECIFIC((err), SPIException)

namespace {

/**
 * @brief Convenience method to convert a SPINum object into the driver type. Avoids long static casts.
 */
spi_host_device_t spi_num_to_driver_type(const SPINum &num) noexcept {
    return static_cast<spi_host_device_t>(num.get_spi_num());
}

}

/**
 * This class wraps closely around the SPI master device driver functions.
 * It is used to hide the implementation, in particular the dependencies on the driver and HAL layer headers.
 * Public header files only use a pointer to this class which is forward declared in spi_host_cxx.hpp.
 * Implementations (source files) can include this private header and use the class definitions.
 *
 * Furthermore, this class ensures RAII-capabilities of an SPI master device allocation and initiates pre- and
 * post-transaction callback for each transfer. In constrast to the IDF driver, the callbacks are not per-device
 * but per transaction in the C++ wrapper framework.
 *
 * For information on the public member functions, refer to the corresponding driver functions in spi_master.h
 */
class SPIDeviceHandle {
public:
    /**
     * Create a device instance on the SPI bus identified by spi_host, allocate all corresponding resources.
     */
    SPIDeviceHandle(SPINum spi_host, CS cs, Frequency frequency, QueueSize q_size)
    {
        spi_device_interface_config_t dev_config = {};
        dev_config.clock_speed_hz = frequency.get_value();
        dev_config.spics_io_num = cs.get_num();
        dev_config.pre_cb = pr_cb;
        dev_config.post_cb = post_cb;
        dev_config.queue_size = q_size.get_size();
        SPI_CHECK_THROW(spi_bus_add_device(spi_num_to_driver_type(spi_host), &dev_config, &handle));
    }

    SPIDeviceHandle(const SPIDeviceHandle &other) = delete;

    SPIDeviceHandle(SPIDeviceHandle &&other) noexcept : handle(std::move(other.handle))
    {
        // Only to indicate programming errors where users use an instance after moving it.
        other.handle = nullptr;
    }

    /**
     * Remove device instance from the SPI bus, deallocate all corresponding resources.
     */
    ~SPIDeviceHandle()
    {
        // We ignore the return value here.
        // Only possible errors are wrong handle (impossible by object invariants) and
        // handle already freed, which we can ignore.
        spi_bus_remove_device(handle);
    }

    SPIDeviceHandle &operator=(SPIDeviceHandle&& other) noexcept
    {
        if (this != &other) {
            handle = std::move(other.handle);

            // Only to indicate programming errors where users use an instance after moving it.
            other.handle = nullptr;
        }
        return *this;
    }

    esp_err_t acquire_bus(TickType_t wait)
    {
        return spi_device_acquire_bus(handle, portMAX_DELAY);
    }

    esp_err_t queue_trans(spi_transaction_t *trans_desc, TickType_t wait)
    {
        return spi_device_queue_trans(handle, trans_desc, wait);
    }

    esp_err_t get_trans_result(spi_transaction_t **trans_desc, TickType_t ticks_to_wait)
    {
        return spi_device_get_trans_result(handle, trans_desc, ticks_to_wait);
    }

    void release_bus()
    {
        spi_device_release_bus(handle);
    }

private:
    /**
     * Route the callback to the callback in the specific SPITransactionDescriptor instance.
     */
    static void pr_cb(spi_transaction_t *driver_transaction)
    {
        SPITransactionDescriptor *transaction = static_cast<SPITransactionDescriptor*>(driver_transaction->user);
        if (transaction->pre_callback) {
            transaction->pre_callback(transaction->user_data);
        }
    }

    /**
     * Route the callback to the callback in the specific SPITransactionDescriptor instance.
     */
    static void post_cb(spi_transaction_t *driver_transaction)
    {
        SPITransactionDescriptor *transaction = static_cast<SPITransactionDescriptor*>(driver_transaction->user);
        if (transaction->post_callback) {
            transaction->post_callback(transaction->user_data);
        }
    }

    spi_device_handle_t handle;
};

}

#endif
