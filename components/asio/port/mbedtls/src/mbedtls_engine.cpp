//
// SPDX-FileCopyrightText: 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// SPDX-License-Identifier: BSL-1.0
//
// SPDX-FileContributor: 2021 Espressif Systems (Shanghai) CO LTD
//

#include "asio/detail/config.hpp"
#include "openssl_stub.hpp"
#include "asio/detail/throw_error.hpp"
#include "asio/error.hpp"
#include "asio/ssl/detail/engine.hpp"
#include "asio/ssl/error.hpp"
#include "asio/ssl/verify_context.hpp"
#include "mbedtls_context.hpp"
#include "mbedtls_bio.hpp"
#include "mbedtls_error.hpp"
#include "mbedtls_engine.hpp"


namespace asio {
namespace ssl {
namespace detail {


engine::engine(SSL_CTX* context)
        : ssl_(nullptr)
{
    ssl_ = mbedtls::create<mbedtls::engine>("mbedtls-engine", context->get());
}

engine::~engine()
{
    delete ssl_;
}

SSL* engine::native_handle()
{
    return ssl_;
}

asio::error_code engine::set_verify_mode(
        verify_mode v, asio::error_code& ec)
{
    ssl_->set_verify_mode(v);
    return {};
}

engine::want engine::handshake(
        stream_base::handshake_type type, asio::error_code& ec)
{
    return perform((type == asio::ssl::stream_base::client)
                   ? &engine::do_connect : &engine::do_accept, 0, 0, ec, 0);
}

engine::want engine::shutdown(asio::error_code& ec)
{
    return perform(&engine::do_shutdown, 0, 0, ec, 0);
}

engine::want engine::write(const asio::const_buffer& data,
                           asio::error_code& ec, std::size_t& bytes_transferred)
{
    if (data.size() == 0)
    {
        ec = asio::error_code();
        return engine::want_nothing;
    }

    return perform(&engine::do_write,
                   const_cast<void*>(data.data()),
                   data.size(), ec, &bytes_transferred);
}

engine::want engine::read(const asio::mutable_buffer& data,
                          asio::error_code& ec, std::size_t& bytes_transferred)
{
    if (data.size() == 0)
    {
        ec = asio::error_code();
        return engine::want_nothing;
    }

    return perform(&engine::do_read, data.data(),
                   data.size(), ec, &bytes_transferred);
}

asio::mutable_buffer engine::get_output(
        const asio::mutable_buffer& data)
{
    int length = ssl_->ext_bio()->read(data.data(), static_cast<int>(data.size()));

    return asio::buffer(data,
                        length > 0 ? static_cast<std::size_t>(length) : 0);
}

asio::const_buffer engine::put_input(
        const asio::const_buffer& data)
{
    int length = ssl_->ext_bio()->write(data.data(), static_cast<int>(data.size()));

    return asio::buffer(data +
                        (length > 0 ? static_cast<std::size_t>(length) : 0));
}

const asio::error_code& engine::map_error_code(
        asio::error_code& ec) const
{
    // We only want to map the error::eof code.
    if (ec != asio::error::eof)
        return ec;

    // If there's data yet to be read, it's an error.
    if (ssl_->ext_bio()->wpending())
    {
        ec = asio::ssl::error::stream_truncated;
        return ec;
    }

    // Otherwise, the peer should have negotiated a proper shutdown.
    if (ssl_->shutdown() != 0)
    {
        ec = asio::ssl::error::stream_truncated;
    }

    return ec;
}

// This is a simplified implementation of a generic ssl io operation
// original implementation using openssl's SSL object is in asio/include/asio/ssl/detail/impl/engine.ipp
engine::want engine::perform(int (engine::* op)(void*, std::size_t),
                             void* data, std::size_t length, asio::error_code& ec,
                             std::size_t* bytes_transferred)
{
    std::size_t pending_output_before = ssl_->ext_bio()->ctrl_pending();
    int result = (this->*op)(data, length);

    std::size_t pending_output_after = ssl_->ext_bio()->ctrl_pending();

    if (mbedtls::error_codes::is_error(result))
    {
        ec = asio::error_code(result, asio::error::get_mbedtls_category());
        return pending_output_after > pending_output_before ? want_output : want_nothing;
    }

    if (result == 0)
    {
        return pending_output_after > pending_output_before
               ? want_output : want_nothing;
    }

    if (result > 0 && bytes_transferred)
        *bytes_transferred = static_cast<std::size_t>(result);

    if (mbedtls::error_codes::want_write(result))
    {
        ec = asio::error_code();
        return want_output_and_retry;
    }
    else if (pending_output_after > pending_output_before)
    {
        ec = asio::error_code();
        return result > 0 ? want_output : want_output_and_retry;
    }
    else if (mbedtls::error_codes::want_read(result))
    {
        ec = asio::error_code();
        return want_input_and_retry;
    }
    else if (ssl_->get_state() == mbedtls::CLOSED)
    {
        ec = asio::error::eof;
        return want_nothing;
    }

    ec = asio::error_code();
    return want_nothing;
}

int engine::do_accept(void*, std::size_t)
{
    return ssl_->accept();
}

int engine::do_connect(void*, std::size_t)
{
    return ssl_->connect();
}

int engine::do_shutdown(void*, std::size_t)
{
    return ssl_->shutdown();
}

int engine::do_read(void* data, std::size_t length)
{
    return ssl_->read(data, length < INT_MAX ? static_cast<int>(length) : INT_MAX);
}

int engine::do_write(void* data, std::size_t length)
{
    return ssl_->write(data, length < INT_MAX ? static_cast<int>(length) : INT_MAX);
}

} // namespace detail
} // namespace ssl
} // namespace asio
