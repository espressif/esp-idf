//
// SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
//
// SPDX-License-Identifier: BSL-1.0
//
#pragma once

#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/esp_debug.h"
#include "esp_log.h"

namespace asio {
namespace ssl {
namespace mbedtls {

const char *error_message(int error_code)
{
    static char error_buf[100];
    mbedtls_strerror(error_code, error_buf, sizeof(error_buf));
    return error_buf;
}

void throw_alloc_failure(const char* location)
{
    asio::error_code ec( MBEDTLS_ERR_SSL_ALLOC_FAILED, asio::error::get_mbedtls_category());
    asio::detail::throw_error(ec, location);
}

namespace error_codes {

bool is_error(int ret)
{
    return  ret < 0 && ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE;
}

static bool want_write(int ret)
{
    return  ret == MBEDTLS_ERR_SSL_WANT_WRITE;
}

static bool want_read(int ret)
{
    return  ret == MBEDTLS_ERR_SSL_WANT_READ;
}

} // namespace error_codes

enum rw_state {
    IDLE, READING, WRITING, CLOSED
};

class engine {
public:
    explicit engine(std::shared_ptr<context> ctx): ctx_(std::move(ctx)),
        bio_(bio::new_pair("mbedtls-engine")), state_(IDLE), verify_mode_(0) {}

    void set_verify_mode(asio::ssl::verify_mode mode)
    {
        verify_mode_ = mode;
    }

    bio* ext_bio() const
    {
        return bio_.second.get();
    }

    rw_state get_state() const
    {
        return state_;
    }

    int shutdown()
    {
        int ret = mbedtls_ssl_close_notify(&impl_.ssl_);
        if (ret) {
            impl::print_error("mbedtls_ssl_close_notify", ret);
        }
        state_ = CLOSED;
        return ret;
    }

    int connect()
    {
        return handshake(true);
    }

    int accept()
    {
        return handshake(false);
    }

    int write(const void *buffer, int len)
    {
        int ret = impl_.write(buffer, len);
        state_ = ret == len ? IDLE: WRITING;
        return ret;
    }

    int read(void *buffer, int len)
    {
        int ret = impl_.read(buffer, len);
        state_ = ret == len ? IDLE: READING;
        return ret;
    }

private:
    int handshake(bool is_client_not_server)
    {
        if (impl_.before_handshake()) {
            impl_.configure(ctx_.get(), is_client_not_server, impl_verify_mode(is_client_not_server));
        }
        return do_handshake();
    }

    static int bio_read(void *ctx, unsigned char *buf, size_t len)
    {
        auto bio = static_cast<BIO*>(ctx);
        int read = bio->read(buf, len);
        if (read <= 0 && bio->should_read()) {
            return MBEDTLS_ERR_SSL_WANT_READ;
        }
        return read;
    }

    static int bio_write(void *ctx, const unsigned char *buf, size_t len)
    {
        auto bio = static_cast<BIO*>(ctx);
        int written = bio->write(buf, len);
        if (written <= 0 && bio->should_write()) {
            return MBEDTLS_ERR_SSL_WANT_WRITE;
        }
        return written;
    }

    int do_handshake()
    {
        int ret = 0;
        mbedtls_ssl_set_bio(&impl_.ssl_, bio_.first.get(), bio_write, bio_read, nullptr);

        while (impl_.ssl_.MBEDTLS_PRIVATE(state) != MBEDTLS_SSL_HANDSHAKE_OVER) {
            ret = mbedtls_ssl_handshake_step(&impl_.ssl_);

            if (ret != 0) {
                if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
                    impl::print_error("mbedtls_ssl_handshake_step", ret);
                }
                if (ret == MBEDTLS_ERR_SSL_WANT_READ) {
                    state_ = READING;
                } else if (ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
                    state_ = WRITING;
                }
                break;
            }
        }
        return ret;
    }

    // Converts OpenSSL verification mode to mbedtls enum
    int impl_verify_mode(bool is_client_not_server) const
    {
        int mode = MBEDTLS_SSL_VERIFY_UNSET;
        if (is_client_not_server) {
            if (verify_mode_ & SSL_VERIFY_PEER)
                mode = MBEDTLS_SSL_VERIFY_REQUIRED;
            else if (verify_mode_ == SSL_VERIFY_NONE)
                mode = MBEDTLS_SSL_VERIFY_NONE;
        } else {
            if (verify_mode_ & SSL_VERIFY_FAIL_IF_NO_PEER_CERT)
                mode = MBEDTLS_SSL_VERIFY_REQUIRED;
            else if (verify_mode_ & SSL_VERIFY_PEER)
                mode = MBEDTLS_SSL_VERIFY_OPTIONAL;
            else if (verify_mode_ == SSL_VERIFY_NONE)
                mode = MBEDTLS_SSL_VERIFY_NONE;
        }
        return mode;
    }

    struct impl {
        static void print_error(const char* function, int error_code)
        {
            constexpr const char *TAG="mbedtls-engine-impl";
            ESP_LOGE(TAG, "%s() returned -0x%04X", function, -error_code);
            ESP_LOGI(TAG, "-0x%04X: %s", -error_code, error_message(error_code));
        }

        bool before_handshake() const
        {
            return ssl_.MBEDTLS_PRIVATE(state) == 0;
        }

        int write(const void *buffer, int len)
        {
            int ret = mbedtls_ssl_write(&ssl_, static_cast<const unsigned char *>(buffer), len);
            if (ret < 0 && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
                print_error("mbedtls_ssl_write", ret);
            }
            return ret;
        }

        int read(void *buffer, int len)
        {
            int ret = mbedtls_ssl_read(&ssl_, static_cast<unsigned char *>(buffer), len);
            if (ret < 0 && ret != MBEDTLS_ERR_SSL_WANT_READ) {
                print_error("mbedtls_ssl_read", ret);
            }
            return ret;
        }

        impl()
        {
            const unsigned char pers[] = "asio ssl";
            mbedtls_ssl_init(&ssl_);
            mbedtls_ssl_config_init(&conf_);
            mbedtls_ctr_drbg_init(&ctr_drbg_);
#ifdef CONFIG_MBEDTLS_DEBUG
            mbedtls_esp_enable_debug_log(&conf_, CONFIG_MBEDTLS_DEBUG_LEVEL);
#endif
            mbedtls_entropy_init(&entropy_);
            mbedtls_ctr_drbg_seed(&ctr_drbg_, mbedtls_entropy_func, &entropy_, pers, sizeof(pers));
            mbedtls_x509_crt_init(&public_cert_);
            mbedtls_pk_init(&pk_key_);
            mbedtls_x509_crt_init(&ca_cert_);
        }

        bool configure(context *ctx, bool is_client_not_server, int mbedtls_verify_mode)
        {
            mbedtls_x509_crt_init(&public_cert_);
            mbedtls_pk_init(&pk_key_);
            mbedtls_x509_crt_init(&ca_cert_);
            int ret = mbedtls_ssl_config_defaults(&conf_, is_client_not_server ? MBEDTLS_SSL_IS_CLIENT: MBEDTLS_SSL_IS_SERVER,
                                                  MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
            if (ret) {
                print_error("mbedtls_ssl_config_defaults", ret);
                return false;
            }
            mbedtls_ssl_conf_rng(&conf_, mbedtls_ctr_drbg_random, &ctr_drbg_);
            mbedtls_ssl_conf_authmode(&conf_, mbedtls_verify_mode);
            if (ctx->cert_chain_.size() > 0 && ctx->private_key_.size() > 0) {
                ret = mbedtls_x509_crt_parse(&public_cert_, ctx->data(container::CERT), ctx->size(container::CERT));
                if (ret < 0) {
                    print_error("mbedtls_x509_crt_parse", ret);
                    return false;
                }
                ret = mbedtls_pk_parse_key(&pk_key_, ctx->data(container::PRIVKEY), ctx->size(container::PRIVKEY),
                                           nullptr, 0, mbedtls_ctr_drbg_random, &ctr_drbg_);
                if (ret < 0) {
                    print_error("mbedtls_pk_parse_keyfile", ret);
                    return false;
                }
                ret = mbedtls_ssl_conf_own_cert(&conf_, &public_cert_, &pk_key_);
                if (ret) {
                    print_error("mbedtls_ssl_conf_own_cert", ret);
                    return false;
                }
            }

            if (ctx->ca_cert_.size() > 0) {
                ret = mbedtls_x509_crt_parse(&ca_cert_, ctx->data(container::CA_CERT), ctx->size(container::CA_CERT));
                if (ret < 0) {
                    print_error("mbedtls_x509_crt_parse", ret);
                    return false;
                }
                mbedtls_ssl_conf_ca_chain(&conf_, &ca_cert_, nullptr);
            } else {
                mbedtls_ssl_conf_ca_chain(&conf_, nullptr, nullptr);
            }
            ret = mbedtls_ssl_setup(&ssl_, &conf_);
            if (ret) {
                print_error("mbedtls_ssl_setup", ret);
                return false;
            }
            return true;
        }
        mbedtls_ssl_context ssl_{};
        mbedtls_entropy_context entropy_{};
        mbedtls_ctr_drbg_context ctr_drbg_{};
        mbedtls_ssl_config conf_{};
        mbedtls_x509_crt public_cert_{};
        mbedtls_pk_context pk_key_{};
        mbedtls_x509_crt ca_cert_{};
    };

    impl impl_{};
    std::shared_ptr<context> ctx_;
    std::pair<std::shared_ptr<bio>, std::shared_ptr<bio>> bio_;
    enum rw_state state_;
    asio::ssl::verify_mode verify_mode_;
};

} } } // namespace asio::ssl::mbedtls
