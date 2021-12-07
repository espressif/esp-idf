//
// SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
//
// SPDX-License-Identifier: Apache-2.0
//

#include "asio/detail/config.hpp"
#include "asio/ssl/detail/openssl_init.hpp"

namespace asio {
namespace ssl {
namespace detail {

// No OpenSSL in this implementation, instance is nullptr
asio::detail::shared_ptr<openssl_init_base::do_init> openssl_init_base::instance()
{
    return nullptr;
}

} } } // namespace asio::ssl::detail
