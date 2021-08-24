// Copyright 2015-2021 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "catch.hpp"
#include "gpio_cxx.hpp"
extern "C" {
#include "Mockgpio.h"
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
 * which use generated CMock mock functions.
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

struct GPIOFixture {
    GPIOFixture(idf::GPIONum gpio_num = idf::GPIONum(18), gpio_mode_t mode = GPIO_MODE_OUTPUT) : num(gpio_num)
    {
        CMOCK_SETUP();
        gpio_reset_pin_ExpectAndReturn(static_cast<gpio_num_t>(num.get_num()), ESP_OK); gpio_set_direction_ExpectAndReturn(static_cast<gpio_num_t>(num.get_num()), mode, ESP_OK);
    }

    ~GPIOFixture()
    {
        // Verify that all expected methods have been called.
        Mockgpio_Verify();
    }

    idf::GPIONum num;
};
