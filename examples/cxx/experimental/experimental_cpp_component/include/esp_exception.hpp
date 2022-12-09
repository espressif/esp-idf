// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#ifndef ESP_EXCEPTION_HPP_
#define ESP_EXCEPTION_HPP_

#ifdef __cpp_exceptions

#include "esp_err.h"
#include <exception>

namespace idf {

/**
 * General exception class for exceptions on the ESP chips.
 * All throwing code in IDF should use either this exception directly or a sub-classes.
 */
struct ESPException : public std::exception {
    ESPException(esp_err_t error);

    esp_err_t error;
};

/**
 * Convenience macro to help converting IDF error codes into ESPException.
 */
#define CHECK_THROW(error_) if (error_ != ESP_OK) throw idf::ESPException(error_);

} // namespace idf

#endif // __cpp_exceptions

#endif // ESP_EXCEPTION_HPP_
