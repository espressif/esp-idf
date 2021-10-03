/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifndef __cpp_exceptions
#error system C++ classes only usable when C++ exceptions enabled. Enable CONFIG_COMPILER_CXX_EXCEPTIONS in Kconfig
#endif

/**
 * This is a "Strong Value Type" base class for types in IDF C++ classes.
 * The idea is that subclasses completely check the contained value during construction.
 * After that, it's trapped and encapsulated inside and cannot be changed anymore.
 * Consequently, the API functions receiving a correctly implemented sub class as parameter
 * don't need to check it anymore. Only at API boundaries the valid value will be retrieved
 * with get_value().
 */
template<typename ValueT>
class StrongValue {
protected:
    StrongValue(ValueT value_arg) : value(value_arg) { }

    ValueT get_value() const {
        return value;
    }

private:
    ValueT value;
};

/**
 * This class adds comparison properties to StrongValue, but no sorting properties.
 */
template<typename ValueT>
class StrongValueComparable : public StrongValue<ValueT> {
protected:
    StrongValueComparable(ValueT value_arg) : StrongValue<ValueT>(value_arg) { }

    using StrongValue<ValueT>::get_value;

    bool operator==(const StrongValueComparable<ValueT> &other_gpio) const
    {
        return get_value() == other_gpio.get_value();
    }

    bool operator!=(const StrongValueComparable<ValueT> &other_gpio) const
    {
        return get_value() != other_gpio.get_value();
    }
};
