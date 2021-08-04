/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#if __cpp_exceptions

#include "esp_exception.hpp"
#include "system_cxx.hpp"

namespace idf {

/**
 * @brief Exception thrown for errors in the GPIO C++ API.
 */
struct GPIOException : public ESPException {
    /**
     * @param error The IDF error representing the error class of the error to throw.
     */
    GPIOException(esp_err_t error);
};

/**
 * Check if the numeric pin number is valid on the current hardware.
 */
esp_err_t check_gpio_pin_num(uint32_t pin_num) noexcept;

/**
 * Check if the numeric value of a drive strength is valid on the current hardware.
 */
esp_err_t check_gpio_drive_strength(uint32_t strength) noexcept;

/**
 * This is a "Strong Value Type" class for GPIO. The GPIO pin number is checked during construction according to
 * the hardware capabilities. This means that any GPIONumBase object is guaranteed to contain a valid GPIO number.
 * See also the template class \c StrongValue.
 */
template<typename GPIONumFinalType>
class GPIONumBase final : public StrongValueComparable<uint32_t> {
public:
    /**
     * @brief Create a numerical pin number representation and make sure it's correct.
     *
     * @throw GPIOException if the number does not reflect a valid GPIO number on the current hardware.
     */
    GPIONumBase(uint32_t pin) : StrongValueComparable<uint32_t>(pin)
    {
        esp_err_t pin_check_result = check_gpio_pin_num(pin);
        if (pin_check_result != ESP_OK) {
            throw GPIOException(pin_check_result);
        }
    }

    using StrongValueComparable<uint32_t>::operator==;
    using StrongValueComparable<uint32_t>::operator!=;

    /**
     * Retrieves the valid numerical representation of the GPIO number.
     */
    uint32_t get_num() const { return get_value(); };
};

/**
 * This is a TAG type whose sole purpose is to create a distinct type from GPIONumBase.
 */
class GPIONumType;

/**
 * A GPIO number type used for general GPIOs, in contrast to specific GPIO pins like e.g. SPI_SCLK.
 */
using GPIONum = GPIONumBase<class GPIONumType>;

/**
 * Level of an input GPIO.
 */
enum class GPIOLevel {
    HIGH,
    LOW
};

/**
 * Represents a valid pull up configuration for GPIOs.
 * It is supposed to resemble an enum type, hence it has static creation methods and a private constructor.
 * This class is a "Strong Value Type", see also the template class \c StrongValue for more properties.
 */
class GPIOPullMode final : public StrongValueComparable<uint32_t> {
private:
    /**
     * Constructor is private since it should only be accessed by the static creation methods.
     *
     * @param pull_mode A valid numerical respresentation of the pull up configuration. Must be valid!
     */
    GPIOPullMode(uint32_t pull_mode) : StrongValueComparable<uint32_t>(pull_mode) { }

public:
    /**
     * Create a representation of a floating pin configuration.
     * For more information, check the driver and HAL files.
     */
    static GPIOPullMode FLOATING();

    /**
     * Create a representation of a pullup configuration.
     * For more information, check the driver and HAL files.
     */
    static GPIOPullMode PULLUP();

    /**
     * Create a representation of a pulldown configuration.
     * For more information, check the driver and HAL files.
     */
    static GPIOPullMode PULLDOWN();

    using StrongValueComparable<uint32_t>::operator==;
    using StrongValueComparable<uint32_t>::operator!=;

    /**
     * Retrieves the valid numerical representation of the pull mode.
     */
    uint32_t get_pull_mode() const { return get_value(); };
};

/**
 * @brief Represents a valid wakup interrupt type for GPIO inputs.
 *
 * This class is a "Strong Value Type", see also the template class \c StrongValue for more properties.
 * It is supposed to resemble an enum type, hence it has static creation methods and a private constructor.
 * For a detailed mapping of interrupt types to numeric values, please refer to the driver types and implementation.
 */
class GPIOWakeupIntrType final: public StrongValueComparable<uint32_t> {
private:
    /**
     * Constructor is private since it should only be accessed by the static creation methods.
     *
     * @param pull_mode A valid numerical respresentation of a possible interrupt level to wake up. Must be valid!
     */
    GPIOWakeupIntrType(uint32_t interrupt_level) : StrongValueComparable<uint32_t>(interrupt_level) { }

public:
    static GPIOWakeupIntrType LOW_LEVEL();
    static GPIOWakeupIntrType HIGH_LEVEL();

    /**
     * Retrieves the valid numerical representation of the pull mode.
     */
    uint32_t get_level() const noexcept { return get_value(); };
};

/**
 * Class representing a valid drive strength for GPIO outputs.
 * This class is a "Strong Value Type", see also the template class \c StrongValue for more properties.
 * For a detailed mapping for values to drive strengths, please refer to the datasheet of the chip you are using.
 * E.g. for ESP32, the values in general are the following:
 *  - WEAK:             5mA
 *  - STRONGER:        10mA
 *  - DEFAULT/MEDIUM:  20mA
 *  - STRONGEST:       40mA
 */
class GPIODriveStrength final : public StrongValueComparable<uint32_t> {
public:
    /**
     * @brief Create a drive strength representation and checks its validity.
     *
     * After construction, this class should have a guaranteed valid strength representation.
     *
     * @param strength the numeric value mapping for a particular strength. For possible ranges, look at the
     *                  static creation functions below.
     * @throws GPIOException if the supplied number is out of the hardware capable range.
     */
    GPIODriveStrength(uint32_t strength) : StrongValueComparable<uint32_t>(strength)
    {
        esp_err_t strength_check_result = check_gpio_drive_strength(strength);
        if (strength_check_result != ESP_OK) {
            throw GPIOException(strength_check_result);
        }
    }

    /**
     * Create a representation of the default drive strength.
     * For more information, check the datasheet and driver and HAL files.
     */
    static GPIODriveStrength DEFAULT();

    /**
     * Create a representation of the weak drive strength.
     * For more information, check the datasheet and driver and HAL files.
     */
    static GPIODriveStrength WEAK();

    /**
     * Create a representation of the less weak drive strength.
     * For more information, check the datasheet and driver and HAL files.
     */
    static GPIODriveStrength LESS_WEAK();

    /**
     * Create a representation of the medium drive strength.
     * For more information, check the datasheet and driver and HAL files.
     */
    static GPIODriveStrength MEDIUM();

    /**
     * Create a representation of the strong drive strength.
     */
    static GPIODriveStrength STRONGEST();

    using StrongValueComparable<uint32_t>::operator==;
    using StrongValueComparable<uint32_t>::operator!=;

    /**
     * Retrieves the valid numerical representation of the drive strength.
     */
    uint32_t get_strength() const { return get_value(); };

};

/**
 * @brief Implementations commonly used functionality for all GPIO configurations.
 *
 * Some functionality is only for specific configurations (set and get drive strength) but is necessary here
 * to avoid complicating the inheritance hierarchy of the GPIO classes.
 * Child classes implementing any GPIO configuration (output, input, etc.) are meant to intherit from this class
 * and possibly make some of the functionality publicly available.
 */
class GPIOBase {
protected:
    /**
     * @brief Construct a GPIO.
     *
     * This constructor will only reset the GPIO but leaves the actual configuration (input, output, etc.) to
     * the sub class.
     *
     * @param num GPIO pin number of the GPIO to be configured.
     *
     * @throws GPIOException
     *              - if the underlying driver function fails
     */
    GPIOBase(GPIONum num);

    /**
     * @brief Enable gpio pad hold function.
     *
     * The gpio pad hold function works in both input and output modes, but must be output-capable gpios.
     * If pad hold enabled:
     *   in output mode: the output level of the pad will be force locked and can not be changed.
     *   in input mode: the input value read will not change, regardless the changes of input signal.
     *
     * @throws GPIOException if the underlying driver function fails.
     */
    void hold_en();

    /**
     * @brief Disable gpio pad hold function.
     *
     * @throws GPIOException if the underlying driver function fails.
     */
    void hold_dis();

    /**
     * @brief Configure the drive strength of the GPIO.
     *
     * @param strength The drive strength. Refer to \c GPIODriveStrength for more details.
     *
     * @throws GPIOException if the underlying driver function fails.
     */
    void set_drive_strength(GPIODriveStrength strength);

    /**
     * @brief Return the current drive strength of the GPIO.
     *
     * @return The currently configured drive strength. Refer to \c GPIODriveStrength for more details.
     *
     * @throws GPIOException if the underlying driver function fails.
     */
    GPIODriveStrength get_drive_strength();

    /**
     * @brief The number of the configured GPIO pin.
     */
    GPIONum gpio_num;
};

/**
 * @brief This class represents a GPIO which is configured as output.
 */
class GPIO_Output : public GPIOBase {
public:
    /**
     * @brief Construct and configure a GPIO as output.
     *
     * @param num GPIO pin number of the GPIO to be configured.
     *
     * @throws GPIOException
     *              - if the underlying driver function fails
     */
    GPIO_Output(GPIONum num);

    /**
     * @brief Set GPIO to high level.
     *
     * @throws GPIOException if the underlying driver function fails.
     */
    void set_high();

    /**
     * @brief Set GPIO to low level.
     *
     * @throws GPIOException if the underlying driver function fails.
     */
    void set_low();

    using GPIOBase::set_drive_strength;
    using GPIOBase::get_drive_strength;
};

/**
 * @brief This class represents a GPIO which is configured as input.
 */
class GPIOInput : public GPIOBase {
public:
    /**
     * @brief Construct and configure a GPIO as input.
     *
     * @param num GPIO pin number of the GPIO to be configured.
     *
     * @throws GPIOException
     *              - if the underlying driver function fails
     */
    GPIOInput(GPIONum num);

    /**
     * @brief Read the current level of the GPIO.
     *
     * @return The GPIO current level of the GPIO.
     */
    GPIOLevel get_level() const noexcept;

    /**
     * @brief Configure the internal pull-up and pull-down restors.
     *
     * @param mode The pull-up/pull-down configuration see \c GPIOPullMode.
     *
     * @throws GPIOException if the underlying driver function fails.
     */
    void set_pull_mode(GPIOPullMode mode);

    /**
     * @brief Configure the pin as wake up pin.
     *
     * @throws GPIOException if the underlying driver function fails.
     */
    void wakeup_enable(GPIOWakeupIntrType interrupt_type);

    /**
     * @brief Disable wake up functionality for this pin if it was enabled before.
     *
     * @throws GPIOException if the underlying driver function fails.
     */
    void wakeup_disable();
};

/**
 * @brief This class represents a GPIO which is configured as open drain output and input at the same time.
 *
 * This class facilitates bit-banging for single wire protocols.
 */
class GPIO_OpenDrain : public GPIOInput {
public:
    /**
     * @brief Construct and configure a GPIO as open drain output as well as input.
     *
     * @param num GPIO pin number of the GPIO to be configured.
     *
     * @throws GPIOException
     *              - if the underlying driver function fails
     */
    GPIO_OpenDrain(GPIONum num);

    /**
     * @brief Set GPIO to floating level.
     *
     * @throws GPIOException if the underlying driver function fails.
     */
    void set_floating();

    /**
     * @brief Set GPIO to low level.
     *
     * @throws GPIOException if the underlying driver function fails.
     */
    void set_low();

    using GPIOBase::set_drive_strength;
    using GPIOBase::get_drive_strength;
};

}

#endif
