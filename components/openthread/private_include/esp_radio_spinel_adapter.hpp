/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

namespace esp {
namespace radio_spinel {

/**
 * This class defines an template to adapt both UartSpinelInterface and SpiSpinelInterface.
 *
 */
template <typename InterfaceType> class SpinelInterfaceAdapter {
public:
    SpinelInterfaceAdapter(void) {}

    ~SpinelInterfaceAdapter(void) {}

    InterfaceType &GetSpinelInterface(void) { return mSpinelInterface; }

private:
    InterfaceType mSpinelInterface;
};

} // namespace radio_spinel
} // namespace esp
