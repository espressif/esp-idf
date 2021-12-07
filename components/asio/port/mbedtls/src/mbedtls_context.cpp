//
// SPDX-FileCopyrightText: 2005 Voipster / Indrek dot Juhani at voipster dot com
// SPDX-FileCopyrightText: 2005-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// SPDX-License-Identifier: BSL-1.0
//
// SPDX-FileContributor: 2021 Espressif Systems (Shanghai) CO LTD
//

#include "asio/detail/config.hpp"
#include "openssl_stub.hpp"
#include <cstring>
#include "asio/detail/throw_error.hpp"
#include "asio/error.hpp"
#include "asio/ssl/context.hpp"
#include "asio/ssl/error.hpp"
#include "mbedtls_context.hpp"


namespace asio {
namespace ssl {


context::context(context::method m)
        : handle_(0)
{
    handle_ = mbedtls::shared_ctx::create("mbedtls-context", m);
    set_options(no_compression);
}

context::context(context&& other)
{
    handle_ = other.handle_;
    other.handle_ = 0;
}

context& context::operator=(context&& other)
{
    context tmp(ASIO_MOVE_CAST(context)(*this));
    handle_ = other.handle_;
    other.handle_ = 0;
    return *this;
}

context::~context()
{
    delete handle_;
}

context::native_handle_type context::native_handle()
{
    return handle_;
}

void context::set_options(context::options o)
{
    asio::error_code ec;
    set_options(o, ec);
    asio::detail::throw_error(ec, "set_options");
}

ASIO_SYNC_OP_VOID context::set_options(
        context::options o, asio::error_code& ec)
{
    handle_->get()->options_ = o;
    ec = asio::error_code();
    ASIO_SYNC_OP_VOID_RETURN(ec);
}

void context::add_certificate_authority(const const_buffer& ca)
{
    asio::error_code ec;
    add_certificate_authority(ca, ec);
    asio::detail::throw_error(ec, "add_certificate_authority");
}

ASIO_SYNC_OP_VOID context::add_certificate_authority(
        const const_buffer& ca, asio::error_code& ec)
{
    handle_->get()->ca_cert_ = ca;
    ASIO_SYNC_OP_VOID_RETURN(asio::error_code());
}

void context::use_certificate_chain(const const_buffer& chain)
{
    asio::error_code ec;
    use_certificate_chain(chain, ec);
    asio::detail::throw_error(ec, "use_certificate_chain");
}

ASIO_SYNC_OP_VOID context::use_certificate_chain(
        const const_buffer& chain, asio::error_code& ec)
{
    handle_->get()->cert_chain_ = chain;
    ASIO_SYNC_OP_VOID_RETURN(asio::error_code());
}

void context::use_private_key(
        const const_buffer& private_key, context::file_format format)
{
    asio::error_code ec;
    use_private_key(private_key, format, ec);
    asio::detail::throw_error(ec, "use_private_key");
}

ASIO_SYNC_OP_VOID context::use_private_key(
        const const_buffer& private_key, context::file_format format,
        asio::error_code& ec)
{
    handle_->get()->private_key_ = private_key;
    ASIO_SYNC_OP_VOID_RETURN(asio::error_code());
}

} // namespace ssl
} // namespace asio
