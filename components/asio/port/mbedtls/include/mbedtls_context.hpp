//
// SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
//
// SPDX-License-Identifier: BSL-1.0
//
#pragma once

#include "asio/ssl/context_base.hpp"
#include "asio/ssl/context.hpp"

namespace asio {
namespace error {

const asio::error_category& get_mbedtls_category();
} // namespace error

namespace ssl {
namespace mbedtls {

void throw_alloc_failure(const char* location);

const char *error_message(int error_code);

enum class container {
    CERT, CA_CERT, PRIVKEY
};

template <typename T, typename... Args>
inline T* create(const char * location, Args &&... args)
{
    T* t = new (std::nothrow) T(std::forward<Args>(args)...);
    if (t == nullptr)
    {
        throw_alloc_failure(location);
    }
    return t;
}

class context {
public:
    explicit context(context_base::method m): method_(m), options_(0) {}

    const unsigned char *data(container c) const
    {
        switch (c) {
            case container::CERT:
                return static_cast<const unsigned char *>(cert_chain_.data());
            case container::CA_CERT:
                return static_cast<const unsigned char *>(ca_cert_.data());
            case container::PRIVKEY:
                return static_cast<const unsigned char *>(private_key_.data());
        }
        return nullptr;
    }

    std::size_t size(container c) const
    {
        switch (c) {
            case container::CERT:
                return cert_chain_.size();
            case container::CA_CERT:
                return ca_cert_.size();
            case container::PRIVKEY:
                return private_key_.size();
        }
        return 0;
    }

    context_base::method method_;
    asio::ssl::context::options options_;
    const_buffer cert_chain_;
    const_buffer private_key_;
    const_buffer ca_cert_;
};

/**
 * @brief Wrapper class around SSL_CTX so we can easily create
 * a shared pointer to the context without throwing the default exception.
 * This is useful, as we can use asio::detail::throw_error for allocation errors.
 */
class shared_ctx {
public:
    static SSL_CTX *create(const char* location, context_base::method m)
    {
        auto wrapped = asio::ssl::mbedtls::create<shared_ctx>(location, m);
        if (wrapped->ctx_ == nullptr)
        {
            throw_alloc_failure(location);
        }
        return wrapped;
    }

    std::shared_ptr<mbedtls::context> get() const
    {
        return ctx_;
    }

    explicit shared_ctx(context_base::method m)
        :ctx_(std::shared_ptr<context>(new (std::nothrow) context(m))) { }

private:
    std::shared_ptr<mbedtls::context> ctx_;
};

} } } // namespace asio::ssl::mbedtls
