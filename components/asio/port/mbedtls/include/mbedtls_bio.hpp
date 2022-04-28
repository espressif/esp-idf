//
// SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
//
// SPDX-License-Identifier: BSL-1.0
//
#pragma once

#include "asio/ssl/context_base.hpp"
#include "asio/ssl/context.hpp"
#include "sdkconfig.h"

namespace asio {
namespace ssl {
namespace mbedtls {

class bio {
    static constexpr int BIO_SIZE = CONFIG_ASIO_SSL_BIO_SIZE;
    static constexpr int BIO_FLAGS_READ = 1;
    static constexpr int BIO_FLAGS_WRITE = 2;

public:
    int write(const void *buf, int len)
    {
        if (buf == nullptr || len <= 0) {
            // not an error, just empty operation (as in openssl/bio)
            return 0;
        }
        int remaining = size_ - offset_;
        if (remaining <= 0) {
            flags_ |= BIO_FLAGS_WRITE;
            return -1;
        }
        int len_to_write = len > remaining ? remaining : len;
        std::memcpy(&data_[offset_], buf, len_to_write);
        offset_ += len_to_write;
        dlen_ = offset_;
        if (len_to_write == len) {
            flags_ &= ~BIO_FLAGS_WRITE;
        }
        return len_to_write;
    }

    int read(void *buf, int len)
    {
        if (buf == nullptr || len <= 0) {
            // not an error, just empty operation (as in openssl/bio)
            return 0;
        }
        int remaining = peer_->dlen_ - peer_->roffset_;
        if (remaining <= 0) {
            flags_ |= BIO_FLAGS_READ;
            return -1;
        }
        int len_to_read = remaining > len ? len : remaining;
        std::memcpy(buf, &peer_->data_[peer_->roffset_], len_to_read);
        peer_->roffset_ += len_to_read;
        if (len_to_read == len) {
            flags_ &= ~BIO_FLAGS_READ;
        }
        if (peer_->offset_) {
            // shift data back to the beginning of the buffer
            std::memmove(&peer_->data_[0], &peer_->data_[peer_->roffset_], peer_->offset_ - peer_->roffset_);
            peer_->offset_ -= peer_->roffset_;
            peer_->roffset_ = 0;
            peer_->dlen_ = peer_->offset_;
        }
        return len_to_read;
    }

    size_t wpending() const
    {
        return dlen_ - roffset_;
    }

    size_t ctrl_pending()
    {
        return peer_->dlen_ - peer_->roffset_;
    }

    bool should_write() const
    {
        return flags_ & BIO_FLAGS_WRITE;
    }

    bool should_read() const
    {
        return flags_ & BIO_FLAGS_READ;
    }

    static std::pair<std::shared_ptr<bio>, std::shared_ptr<bio>>  new_pair(const char* error_location)
    {
        auto b1 = std::shared_ptr<bio>(new (std::nothrow) bio);
        auto b2 = std::shared_ptr<bio>(new (std::nothrow) bio);
        if (b1 == nullptr || b2 == nullptr) {
            throw_alloc_failure(error_location);
        } else {
            b1->peer_ = b2;
            b2->peer_ = b1;
        }
        return std::make_pair(b1, b2);
    }

private:
    std::array<uint8_t, BIO_SIZE> data_ {};
    size_t size_ {BIO_SIZE};
    std::shared_ptr<bio> peer_ {nullptr};
    int dlen_ {0};
    size_t offset_ {0};
    size_t roffset_ {0};
    size_t flags_ {0};
};

} } } // namespace asio::ssl::mbedtls
