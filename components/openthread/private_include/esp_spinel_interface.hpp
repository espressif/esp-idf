/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "esp_openthread.h"
#include "esp_openthread_types.h"
#include "hal/uart_types.h"
#include "lib/spinel/spinel_interface.hpp"
#include "lib/hdlc/hdlc.hpp"
#include "openthread/error.h"

namespace esp {
namespace openthread {

/**
 * This class defines an template to adapt both UartSpinelInterface and SpiSpinelInterface.
 *
 */
template <typename InterfaceType> class SpinelInterfaceAdapter {
public:
    /**
     * @brief   This constructor of object.
     */
    SpinelInterfaceAdapter(void) {}

    /**
     * @brief   This destructor of the object.
     *
     */
    ~SpinelInterfaceAdapter(void) {}

    /**
     * @brief   This method return the underlying spinel interface.
     *
     * @return  The underlying spinel interface.
     *
     */
    InterfaceType &GetSpinelInterface(void) { return mSpinelInterface; }

private:
    InterfaceType mSpinelInterface;
};

} // namespace openthread
} // namespace esp
