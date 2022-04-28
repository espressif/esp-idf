//
// SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
//
// SPDX-License-Identifier: BSL-1.0
//
#pragma once

#include "asio/detail/config.hpp"
#include "asio/ssl/error.hpp"
#include "asio/ssl/detail/openssl_init.hpp"
#include "mbedtls_context.hpp"

namespace asio {
namespace error {
namespace detail {

class mbedtls_category : public asio::error_category
{
public:
    const char* name() const ASIO_ERROR_CATEGORY_NOEXCEPT
    {
        return "asio.ssl";
    }

    std::string message(int value) const
    {
        const char* s = asio::ssl::mbedtls::error_message(value);
        return s ? s : "asio.mbedtls error";
    }
};

} // namespace detail

const asio::error_category& get_mbedtls_category()
{
    static detail::mbedtls_category instance;
    return instance;
}

const asio::error_category& get_ssl_category()
{
    return asio::error::get_mbedtls_category();
}

} // namespace error

namespace ssl {
namespace error {

const asio::error_category& get_stream_category()
{
    return asio::error::get_mbedtls_category();
}

} } } // namespace asio::ssl::error
