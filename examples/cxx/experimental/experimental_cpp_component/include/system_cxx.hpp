/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * This file contains helper classes for commonly used IDF types. The classes make the use of these types easier and
 * safer.
 * In particular, their usage provides greater type-safety of function arguments and "correctness by construction".
 */
#pragma once

#ifndef __cpp_exceptions
#error system C++ classes only usable when C++ exceptions enabled. Enable CONFIG_COMPILER_CXX_EXCEPTIONS in Kconfig
#endif

#include "esp_exception.hpp"

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
    constexpr StrongValue(ValueT value_arg) : value(value_arg) { }

    ValueT get_value() const {
        return value;
    }

private:
    ValueT value;
};

/**
 * This class adds comparison properties to StrongValue, but no sorting and ordering properties.
 */
template<typename ValueT>
class StrongValueComparable : public StrongValue<ValueT> {
protected:
    constexpr StrongValueComparable(ValueT value_arg) : StrongValue<ValueT>(value_arg) { }

public:
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

namespace idf {

/**
 * This class adds ordering and sorting properties to StrongValue.
 */
template<typename ValueT>
class StrongValueOrdered : public StrongValueComparable<ValueT> {
public:
    StrongValueOrdered(ValueT value) : StrongValueComparable<ValueT>(value) { }

    using StrongValueComparable<ValueT>::get_value;

    bool operator>(const StrongValueOrdered<ValueT> &other) const
    {
        return get_value() > other.get_value();
    }

    bool operator<(const StrongValueOrdered<ValueT> &other) const
    {
        return get_value() < other.get_value();
    }

    bool operator>=(const StrongValueOrdered<ValueT> &other) const
    {
        return get_value() >= other.get_value();
    }

    bool operator<=(const StrongValueOrdered<ValueT> &other) const
    {
        return get_value() <= other.get_value();
    }
};

/**
 * A general frequency class to be used whereever an unbound frequency value is necessary.
 */
class Frequency : public StrongValueOrdered<size_t> {
public:
    explicit Frequency(size_t frequency) : StrongValueOrdered<size_t>(frequency)
    {
        if (frequency == 0) {
            throw ESPException(ESP_ERR_INVALID_ARG);
        }
    }

    Frequency(const Frequency&) = default;
    Frequency &operator=(const Frequency&) = default;

    using StrongValueOrdered<size_t>::get_value;

    static Frequency Hz(size_t frequency)
    {
        return Frequency(frequency);
    }

    static Frequency KHz(size_t frequency)
    {
        return Frequency(frequency * 1000);
    }

    static Frequency MHz(size_t frequency)
    {
        return Frequency(frequency * 1000 * 1000);
    }
};

/**
 * Queue size mainly for operating system queues.
 */
class QueueSize {
public:
    explicit QueueSize(size_t q_size) : queue_size(q_size) { }

    size_t get_size()
    {
        return queue_size;
    }

private:
    size_t queue_size;
};

}
