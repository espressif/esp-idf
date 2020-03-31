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

#pragma once

#ifdef __cpp_exceptions

#include "esp_err.h"
#include <exception>

namespace idf {

/**
 * @brief
 * General exception class for all C++ exceptions in IDF.
 *
 * All throwing code in IDF should use either this exception directly or a sub-classes.
 * An error from the underlying IDF function is mandatory. The idea is to wrap the orignal IDF error code to keep
 * the error scheme partially compatible. If an exception occurs in a higher level C++ code not directly wrapping
 * IDF functions, an appropriate error code reflecting the cause must be chosen or newly created.
 */
class ESPException : public std::exception {
public:
    /**
     * @param error Error from underlying IDF functions.
     */
    ESPException(esp_err_t error);

    virtual ~ESPException() { }

    /**
     * @return A textual representation of the contained error. This method only wraps \c esp_err_to_name.
     */
    virtual const char *what() const noexcept;

    /**
     * Error from underlying IDF functions. If an exception occurs in a higher level C++ code not directly wrapping
     * IDF functions, an appropriate error code reflecting the cause must be chosen or newly created.
     */
    const esp_err_t error;
};

/**
 * Convenience macro to help converting IDF error codes into ESPException.
 */
#define CHECK_THROW(error_)                                         \
    do {                                                            \
        esp_err_t result = error_;                                  \
        if (result != ESP_OK) throw idf::ESPException(result);      \
    } while (0)

/**
 * Convenience macro to help converting IDF error codes into a child of ESPException.
 */
#define CHECK_THROW_SPECIFIC(error_, exception_type_)               \
    do {                                                            \
        esp_err_t result = error_;                                  \
        if (result != ESP_OK) throw idf::exception_type_(result);   \
    } while (0)

} // namespace idf

#endif // __cpp_exceptions
