/*
 * SPDX-FileCopyrightText: 2020-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include "esp_mbedtls_dynamic_impl.h"
#include "sdkconfig.h"

#if CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
#include "esp_crt_bundle.h"
#endif

#define COUNTER_SIZE (8)
#define CACHE_IV_SIZE (16)
#define CACHE_BUFFER_SIZE (CACHE_IV_SIZE + COUNTER_SIZE)

#define TX_IDLE_BUFFER_SIZE (MBEDTLS_SSL_HEADER_LEN + CACHE_BUFFER_SIZE)

#define ESP_MBEDTLS_RETURN_IF_RX_BUF_STATIC(ssl) \
    do { \
        if (ssl->MBEDTLS_PRIVATE(in_buf)) { \
            esp_mbedtls_ssl_buf_states state = esp_mbedtls_get_buf_state(ssl->MBEDTLS_PRIVATE(in_buf)); \
            if (state == ESP_MBEDTLS_SSL_BUF_STATIC) { \
                return 0; \
            } \
        } \
    } while(0)


static const char *TAG = "Dynamic Impl";

static void esp_mbedtls_set_buf_state(unsigned char *buf, esp_mbedtls_ssl_buf_states state)
{
    struct esp_mbedtls_ssl_buf *temp = __containerof(buf, struct esp_mbedtls_ssl_buf, buf[0]);
    temp->state = state;
}

static esp_mbedtls_ssl_buf_states esp_mbedtls_get_buf_state(unsigned char *buf)
{
    struct esp_mbedtls_ssl_buf *temp = __containerof(buf, struct esp_mbedtls_ssl_buf, buf[0]);
    return temp->state;
}

void esp_mbedtls_free_buf(unsigned char *buf)
{
    struct esp_mbedtls_ssl_buf *temp = __containerof(buf, struct esp_mbedtls_ssl_buf, buf[0]);
    ESP_LOGV(TAG, "free buffer @ %p", temp);
    mbedtls_free(temp);
}

static void esp_mbedtls_init_ssl_buf(struct esp_mbedtls_ssl_buf *buf, unsigned int len)
{
    if (buf) {
        buf->state = ESP_MBEDTLS_SSL_BUF_CACHED;
        buf->len = len;
    }
}

static void esp_mbedtls_parse_record_header(mbedtls_ssl_context *ssl)
{
    ssl->MBEDTLS_PRIVATE(in_msgtype) =  ssl->MBEDTLS_PRIVATE(in_hdr)[0];
    ssl->MBEDTLS_PRIVATE(in_msglen) = (ssl->MBEDTLS_PRIVATE(in_len)[0] << 8) | ssl->MBEDTLS_PRIVATE(in_len)[1];
}

static int tx_buffer_len(mbedtls_ssl_context *ssl, int len)
{
    (void)ssl;

    if (!len) {
        return MBEDTLS_SSL_OUT_BUFFER_LEN;
    } else {
        return len + MBEDTLS_SSL_HEADER_LEN
                   + MBEDTLS_MAX_IV_LENGTH
                   + MBEDTLS_SSL_MAC_ADD
                   + MBEDTLS_SSL_PADDING_ADD
                   + MBEDTLS_SSL_MAX_CID_EXPANSION;
    }
}

static void init_tx_buffer(mbedtls_ssl_context *ssl, unsigned char *buf)
{
    /**
     * In mbedtls, ssl->MBEDTLS_PRIVATE(out_msg) = ssl->MBEDTLS_PRIVATE(out_buf) + offset;
     */
    if (!buf) {
        ptrdiff_t out_msg_off = ssl->MBEDTLS_PRIVATE(out_msg) - ssl->MBEDTLS_PRIVATE(out_buf);

        if (!out_msg_off) {
            out_msg_off = MBEDTLS_SSL_HEADER_LEN;
        }

        ssl->MBEDTLS_PRIVATE(out_buf) = NULL;
        ssl->MBEDTLS_PRIVATE(out_ctr) = NULL;
        ssl->MBEDTLS_PRIVATE(out_hdr) = NULL;
        ssl->MBEDTLS_PRIVATE(out_len) = NULL;
        ssl->MBEDTLS_PRIVATE(out_iv)  = NULL;
        /* Stash the offset in the pointer field while the buffer is freed; it is
         * restored to a real pointer on the next allocation below. */
        ssl->MBEDTLS_PRIVATE(out_msg) = (unsigned char *)(uintptr_t)out_msg_off;
    } else {
        uintptr_t out_msg_off = (uintptr_t)ssl->MBEDTLS_PRIVATE(out_msg);

        ssl->MBEDTLS_PRIVATE(out_buf) = buf;
        ssl->MBEDTLS_PRIVATE(out_ctr) = ssl->MBEDTLS_PRIVATE(out_buf);
        ssl->MBEDTLS_PRIVATE(out_hdr) = ssl->MBEDTLS_PRIVATE(out_buf) +  8;
        ssl->MBEDTLS_PRIVATE(out_len) = ssl->MBEDTLS_PRIVATE(out_buf) + 11;
        ssl->MBEDTLS_PRIVATE(out_iv)  = ssl->MBEDTLS_PRIVATE(out_buf) + MBEDTLS_SSL_HEADER_LEN;
        ssl->MBEDTLS_PRIVATE(out_msg) = ssl->MBEDTLS_PRIVATE(out_buf) + out_msg_off;

        ESP_LOGV(TAG, "out msg offset is %u", (unsigned)out_msg_off);
    }

    ssl->MBEDTLS_PRIVATE(out_msgtype) = 0;
    ssl->MBEDTLS_PRIVATE(out_msglen) = 0;
    ssl->MBEDTLS_PRIVATE(out_left) = 0;
}

static void init_rx_buffer(mbedtls_ssl_context *ssl, unsigned char *buf)
{
    /**
     * In mbedtls, ssl->MBEDTLS_PRIVATE(in_msg) = ssl->MBEDTLS_PRIVATE(in_buf) + offset;
     */
    if (!buf) {
        ptrdiff_t in_msg_off = ssl->MBEDTLS_PRIVATE(in_msg) - ssl->MBEDTLS_PRIVATE(in_buf);

        if (!in_msg_off) {
            in_msg_off = MBEDTLS_SSL_HEADER_LEN;
        }

        ssl->MBEDTLS_PRIVATE(in_buf) = NULL;
        ssl->MBEDTLS_PRIVATE(in_ctr) = NULL;
        ssl->MBEDTLS_PRIVATE(in_hdr) = NULL;
        ssl->MBEDTLS_PRIVATE(in_len) = NULL;
        ssl->MBEDTLS_PRIVATE(in_iv)  = NULL;
        /* Stash the offset in the pointer field while the buffer is freed; it is
         * restored to a real pointer on the next allocation below. */
        ssl->MBEDTLS_PRIVATE(in_msg) = (unsigned char *)(uintptr_t)in_msg_off;
    } else {
        uintptr_t in_msg_off = (uintptr_t)ssl->MBEDTLS_PRIVATE(in_msg);

        ssl->MBEDTLS_PRIVATE(in_buf) = buf;
        ssl->MBEDTLS_PRIVATE(in_ctr) = ssl->MBEDTLS_PRIVATE(in_buf);
        ssl->MBEDTLS_PRIVATE(in_hdr) = ssl->MBEDTLS_PRIVATE(in_buf) +  8;
        ssl->MBEDTLS_PRIVATE(in_len) = ssl->MBEDTLS_PRIVATE(in_buf) + 11;
        ssl->MBEDTLS_PRIVATE(in_iv)  = ssl->MBEDTLS_PRIVATE(in_buf) + MBEDTLS_SSL_HEADER_LEN;
        ssl->MBEDTLS_PRIVATE(in_msg) = ssl->MBEDTLS_PRIVATE(in_buf) + in_msg_off;

        ESP_LOGV(TAG, "in msg offset is %u", (unsigned)in_msg_off);
    }

    ssl->MBEDTLS_PRIVATE(in_msgtype) = 0;
    ssl->MBEDTLS_PRIVATE(in_msglen) = 0;
    ssl->MBEDTLS_PRIVATE(in_left) = 0;
}

esp_err_t esp_mbedtls_dynamic_set_rx_buf_static(mbedtls_ssl_context *ssl)
{
    /* Public API: guard against being called with no RX buffer allocated
     * (e.g. before setup or after free) to avoid a NULL dereference below. */
    if (ssl == NULL || ssl->MBEDTLS_PRIVATE(in_buf) == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    unsigned char cache_buf[16];
    memcpy(cache_buf, ssl->MBEDTLS_PRIVATE(in_buf), 16);
    esp_mbedtls_reset_free_rx_buffer(ssl);

    struct esp_mbedtls_ssl_buf *esp_buf;
    int buffer_len = tx_buffer_len(ssl, MBEDTLS_SSL_IN_CONTENT_LEN);
    esp_buf = mbedtls_calloc(1, SSL_BUF_HEAD_OFFSET_SIZE + buffer_len);
    if (!esp_buf) {
        ESP_LOGE(TAG, "rx buf alloc(%d bytes) failed", SSL_BUF_HEAD_OFFSET_SIZE + buffer_len);
        return ESP_ERR_NO_MEM;
    }
    esp_mbedtls_init_ssl_buf(esp_buf, buffer_len);
    init_rx_buffer(ssl, esp_buf->buf);

    memcpy(ssl->MBEDTLS_PRIVATE(in_ctr), cache_buf, 8);
    memcpy(ssl->MBEDTLS_PRIVATE(in_iv), cache_buf + 8, 8);
    esp_mbedtls_set_buf_state(ssl->MBEDTLS_PRIVATE(in_buf), ESP_MBEDTLS_SSL_BUF_STATIC);
    return ESP_OK;

}

static int esp_mbedtls_alloc_tx_buf(mbedtls_ssl_context *ssl, int len)
{
    struct esp_mbedtls_ssl_buf *esp_buf;

    if (ssl->MBEDTLS_PRIVATE(out_buf)) {
        esp_mbedtls_free_buf(ssl->MBEDTLS_PRIVATE(out_buf));
        ssl->MBEDTLS_PRIVATE(out_buf) = NULL;
    }

    esp_buf = mbedtls_calloc(1, SSL_BUF_HEAD_OFFSET_SIZE + len);
    if (!esp_buf) {
        ESP_LOGE(TAG, "alloc(%d bytes) failed", SSL_BUF_HEAD_OFFSET_SIZE + len);
        return MBEDTLS_ERR_SSL_ALLOC_FAILED;
    }

    ESP_LOGV(TAG, "add out buffer %d bytes @ %p", len, esp_buf->buf);

    esp_mbedtls_init_ssl_buf(esp_buf, len);
    /**
     * Mark the out_msg offset from ssl->MBEDTLS_PRIVATE(out_buf).
     *
     * In mbedtls, ssl->MBEDTLS_PRIVATE(out_msg) = ssl->MBEDTLS_PRIVATE(out_buf) + offset;
     */
    ssl->MBEDTLS_PRIVATE(out_msg) = (unsigned char *)MBEDTLS_SSL_HEADER_LEN;

    init_tx_buffer(ssl, esp_buf->buf);

    return 0;
}

int esp_mbedtls_setup_tx_buffer(mbedtls_ssl_context *ssl)
{
    CHECK_OK(esp_mbedtls_alloc_tx_buf(ssl, TX_IDLE_BUFFER_SIZE));

    /* mark the out buffer has no data cached */
    esp_mbedtls_set_buf_state(ssl->MBEDTLS_PRIVATE(out_buf), ESP_MBEDTLS_SSL_BUF_NO_CACHED);

    return 0;
}

void esp_mbedtls_setup_rx_buffer(mbedtls_ssl_context *ssl)
{
    ssl->MBEDTLS_PRIVATE(in_msg) = ssl->MBEDTLS_PRIVATE(in_buf) = NULL;
    init_rx_buffer(ssl, NULL);
}

int esp_mbedtls_reset_add_tx_buffer(mbedtls_ssl_context *ssl)
{
    return esp_mbedtls_alloc_tx_buf(ssl, MBEDTLS_SSL_OUT_BUFFER_LEN);
}

int esp_mbedtls_reset_free_tx_buffer(mbedtls_ssl_context *ssl)
{
    esp_mbedtls_free_buf(ssl->MBEDTLS_PRIVATE(out_buf));
    init_tx_buffer(ssl, NULL);

    CHECK_OK(esp_mbedtls_setup_tx_buffer(ssl));

    return 0;
}

int esp_mbedtls_reset_add_rx_buffer(mbedtls_ssl_context *ssl)
{
    struct esp_mbedtls_ssl_buf *esp_buf;

    if (ssl->MBEDTLS_PRIVATE(in_buf)) {
        esp_mbedtls_free_buf(ssl->MBEDTLS_PRIVATE(in_buf));
        ssl->MBEDTLS_PRIVATE(in_buf) = NULL;
    }

    esp_buf = mbedtls_calloc(1, SSL_BUF_HEAD_OFFSET_SIZE + MBEDTLS_SSL_IN_BUFFER_LEN);
    if (!esp_buf) {
        ESP_LOGE(TAG, "alloc(%d bytes) failed", SSL_BUF_HEAD_OFFSET_SIZE + MBEDTLS_SSL_IN_BUFFER_LEN);
        return MBEDTLS_ERR_SSL_ALLOC_FAILED;
    }

    ESP_LOGV(TAG, "add in buffer %d bytes @ %p", MBEDTLS_SSL_IN_BUFFER_LEN, esp_buf->buf);

    esp_mbedtls_init_ssl_buf(esp_buf, MBEDTLS_SSL_IN_BUFFER_LEN);
    /**
     * Mark the in_msg offset from ssl->MBEDTLS_PRIVATE(in_buf).
     *
     * In mbedtls, ssl->MBEDTLS_PRIVATE(in_msg) = ssl->MBEDTLS_PRIVATE(in_buf) + offset;
     */
    ssl->MBEDTLS_PRIVATE(in_msg) = (unsigned char *)MBEDTLS_SSL_HEADER_LEN;

    init_rx_buffer(ssl, esp_buf->buf);

    return 0;
}

void esp_mbedtls_reset_free_rx_buffer(mbedtls_ssl_context *ssl)
{
    esp_mbedtls_free_buf(ssl->MBEDTLS_PRIVATE(in_buf));
    init_rx_buffer(ssl, NULL);
}

int esp_mbedtls_add_tx_buffer(mbedtls_ssl_context *ssl, size_t buffer_len)
{
    int ret = 0;
    int cached = 0;
    struct esp_mbedtls_ssl_buf *esp_buf;
    unsigned char cache_buf[CACHE_BUFFER_SIZE];

    ESP_LOGV(TAG, "--> add out");

    if (ssl->MBEDTLS_PRIVATE(out_buf)) {
        if (esp_mbedtls_get_buf_state(ssl->MBEDTLS_PRIVATE(out_buf)) == ESP_MBEDTLS_SSL_BUF_CACHED) {
            ESP_LOGV(TAG, "out buffer is not empty");
            ret = 0;
            goto exit;
        } else {
            memcpy(cache_buf, ssl->MBEDTLS_PRIVATE(out_buf), CACHE_BUFFER_SIZE);
            esp_mbedtls_free_buf(ssl->MBEDTLS_PRIVATE(out_buf));
            init_tx_buffer(ssl, NULL);
            cached = 1;
        }
    }

    buffer_len = tx_buffer_len(ssl, buffer_len);

    esp_buf = mbedtls_calloc(1, SSL_BUF_HEAD_OFFSET_SIZE + buffer_len);
    if (!esp_buf) {
        ESP_LOGE(TAG, "alloc(%zu bytes) failed", SSL_BUF_HEAD_OFFSET_SIZE + buffer_len);
        ret = MBEDTLS_ERR_SSL_ALLOC_FAILED;
        goto exit;
    }

    ESP_LOGV(TAG, "add out buffer %zu bytes @ %p", buffer_len, esp_buf->buf);

    esp_mbedtls_init_ssl_buf(esp_buf, buffer_len);
    init_tx_buffer(ssl, esp_buf->buf);

    if (cached) {
        memcpy(ssl->MBEDTLS_PRIVATE(out_ctr), cache_buf, COUNTER_SIZE);
        memcpy(ssl->MBEDTLS_PRIVATE(out_iv), cache_buf + COUNTER_SIZE, CACHE_IV_SIZE);
    }

    ESP_LOGV(TAG, "ssl->MBEDTLS_PRIVATE(out_buf)=%p ssl->MBEDTLS_PRIVATE(out_msg)=%p", ssl->MBEDTLS_PRIVATE(out_buf), ssl->MBEDTLS_PRIVATE(out_msg));

exit:
    ESP_LOGV(TAG, "<-- add out");

    return ret;
}


int esp_mbedtls_free_tx_buffer(mbedtls_ssl_context *ssl)
{
    int ret = 0;
    unsigned char buf[CACHE_BUFFER_SIZE];
    struct esp_mbedtls_ssl_buf *esp_buf;

    ESP_LOGV(TAG, "--> free out");

    if (!ssl->MBEDTLS_PRIVATE(out_buf) || (ssl->MBEDTLS_PRIVATE(out_buf) && (esp_mbedtls_get_buf_state(ssl->MBEDTLS_PRIVATE(out_buf)) == ESP_MBEDTLS_SSL_BUF_NO_CACHED))) {
        ret = 0;
        goto exit;
    }

    memcpy(buf, ssl->MBEDTLS_PRIVATE(out_ctr), COUNTER_SIZE);
    memcpy(buf + COUNTER_SIZE, ssl->MBEDTLS_PRIVATE(out_iv), CACHE_IV_SIZE);

    esp_mbedtls_free_buf(ssl->MBEDTLS_PRIVATE(out_buf));
    init_tx_buffer(ssl, NULL);

    esp_buf = mbedtls_calloc(1, SSL_BUF_HEAD_OFFSET_SIZE + TX_IDLE_BUFFER_SIZE);
    if (!esp_buf) {
        ESP_LOGE(TAG, "alloc(%d bytes) failed", SSL_BUF_HEAD_OFFSET_SIZE + TX_IDLE_BUFFER_SIZE);
        return MBEDTLS_ERR_SSL_ALLOC_FAILED;
    }

    esp_mbedtls_init_ssl_buf(esp_buf, TX_IDLE_BUFFER_SIZE);
    memcpy(esp_buf->buf, buf, CACHE_BUFFER_SIZE);
    init_tx_buffer(ssl, esp_buf->buf);
    esp_mbedtls_set_buf_state(ssl->MBEDTLS_PRIVATE(out_buf), ESP_MBEDTLS_SSL_BUF_NO_CACHED);
exit:
    ESP_LOGV(TAG, "<-- free out");

    return ret;
}

/*
 * Decide how many content bytes the RX buffer must hold for the record whose
 * 5-byte header has just been peeked into msg_head.
 *
 * The dynamic buffer is normally sized to this single record. That is unsafe
 * whenever mbedtls pulls more than the peeked record into the same buffer:
 *   - a handshake message fragmented across records is reassembled in place,
 *     so the buffer must hold the whole message;
 *   - a non-fatal alert makes mbedtls skip it and read the following record
 *     into the same buffer; and
 *   - a TLS 1.3 middlebox-compat CCS (RFC 8446 D.4) is skipped and the
 *     following, larger record is read into the same buffer.
 */
static int rx_reassembly_content_len(mbedtls_ssl_context *ssl,
                                     const unsigned char *msg_head,
                                     int *content_len)
{
    int in_msgtype = ssl->MBEDTLS_PRIVATE(in_msgtype);
    size_t in_msglen = ssl->MBEDTLS_PRIVATE(in_msglen);
    bool encrypted = ssl->MBEDTLS_PRIVATE(transform_in) != NULL;

    *content_len = (int)in_msglen;

    if (mbedtls_ssl_is_handshake_over(ssl)) {
        return 0;
    }

    if (in_msgtype == MBEDTLS_SSL_MSG_HANDSHAKE && !encrypted && in_msglen >= 4) {
        size_t hdr = mbedtls_ssl_in_hdr_len(ssl);
        int ret = mbedtls_ssl_fetch_input(ssl, hdr + 4);
        if (ret != 0) {
            return ret;
        }
        /* Handshake header (type[1] + length[3]) sits just past the record
         * header; its length field covers the whole (possibly fragmented)
         * message, independent of how it is split across records. */
        const unsigned char *hs = msg_head + hdr;
        size_t hslen = 4 + ((size_t)hs[1] << 16 | (size_t)hs[2] << 8 | hs[3]);
        if (hslen > in_msglen) {
            *content_len = hslen < MBEDTLS_SSL_IN_CONTENT_LEN
                               ? (int)hslen : MBEDTLS_SSL_IN_CONTENT_LEN;
            ESP_LOGD(TAG, "fragmented handshake message: sizing RX for %d bytes",
                     *content_len);
        }
        return 0;
    }

    /* For records that make mbedtls pull a further, unknown-size record into
     * this same buffer, the total is not knowable from the peeked header, so
     * size for the maximum record:
     *   - an encrypted handshake record (its plaintext length is not visible);
     *   - an alert: a non-fatal one makes mbedtls_ssl_handle_message_type()
     *     return MBEDTLS_ERR_SSL_NON_FATAL, so the loop reads the next record;
     *   - a TLS 1.3 middlebox-compat CCS, which is skipped and followed by a
     *     (larger) record.
     * Ordinary handshake messages are sized exactly above, so per-message
     * dynamic sizing is preserved for them. */
    if (encrypted && (in_msgtype == MBEDTLS_SSL_MSG_HANDSHAKE ||
                      in_msgtype == MBEDTLS_SSL_MSG_APPLICATION_DATA)) {
        *content_len = MBEDTLS_SSL_IN_CONTENT_LEN;
    } else if (in_msgtype == MBEDTLS_SSL_MSG_ALERT) {
        *content_len = MBEDTLS_SSL_IN_CONTENT_LEN;
    }
#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
    else if (in_msgtype == MBEDTLS_SSL_MSG_CHANGE_CIPHER_SPEC &&
             ssl->MBEDTLS_PRIVATE(tls_version) == MBEDTLS_SSL_VERSION_TLS1_3) {
        *content_len = MBEDTLS_SSL_IN_CONTENT_LEN;
    }
#endif
    return 0;
}

/* Boxes the application's BIO callbacks so the RX trampolines can bounds-check
 * against in_buf while the send path keeps the app's original p_bio. */
struct esp_ssl_bio {
    mbedtls_ssl_context *ssl;
    void *p_bio;
    mbedtls_ssl_send_t *f_send;
    mbedtls_ssl_recv_t *f_recv;
    mbedtls_ssl_recv_timeout_t *f_recv_timeout;
};

/* True if reading `len` bytes to `buf` would overrun the dynamic RX buffer.
 * Destinations outside it (stack header-peek, unallocated, DTLS) return false. */
static bool rx_read_rejected(mbedtls_ssl_context *ssl,
                             const unsigned char *buf, size_t len)
{
    unsigned char *in_buf = ssl->MBEDTLS_PRIVATE(in_buf);

#if defined(MBEDTLS_SSL_PROTO_DTLS)
    /* DTLS legitimately fetches up to the full buffer; only guard streams. */
    if (ssl->MBEDTLS_PRIVATE(conf)->MBEDTLS_PRIVATE(transport)
            != MBEDTLS_SSL_TRANSPORT_STREAM) {
        return false;
    }
#endif
    if (in_buf == NULL || buf < in_buf) {
        return false;
    }

    struct esp_mbedtls_ssl_buf *esp_buf =
        __containerof(in_buf, struct esp_mbedtls_ssl_buf, buf[0]);
    unsigned char *end = in_buf + esp_buf->len;

    if (buf >= end || (size_t)(end - buf) >= len) {
        return false;                       /* outside the buffer, or it fits */
    }

    ESP_LOGE(TAG, "RX overflow prevented: peer needs %u bytes, %u available",
             (unsigned)len, (unsigned)(end - buf));
    return true;
}

static int esp_ssl_bio_recv(void *ctx, unsigned char *buf, size_t len)
{
    struct esp_ssl_bio *bio = ctx;

    if (rx_read_rejected(bio->ssl, buf, len)) {
        return MBEDTLS_ERR_SSL_BAD_INPUT_DATA;
    }
    return bio->f_recv(bio->p_bio, buf, len);
}

static int esp_ssl_bio_recv_timeout(void *ctx, unsigned char *buf,
                                    size_t len, uint32_t timeout)
{
    struct esp_ssl_bio *bio = ctx;

    if (rx_read_rejected(bio->ssl, buf, len)) {
        return MBEDTLS_ERR_SSL_BAD_INPUT_DATA;
    }
    return bio->f_recv_timeout(bio->p_bio, buf, len, timeout);
}

static int esp_ssl_bio_send(void *ctx, const unsigned char *buf, size_t len)
{
    struct esp_ssl_bio *bio = ctx;

    return bio->f_send(bio->p_bio, buf, len);
}

/* Interpose the overflow-checking trampolines on the app's BIO callbacks.
 * Idempotent; no-op until the app has configured the callbacks. */
static void esp_mbedtls_install_bio(mbedtls_ssl_context *ssl)
{
    if (ssl->MBEDTLS_PRIVATE(f_recv) == esp_ssl_bio_recv ||
        ssl->MBEDTLS_PRIVATE(f_recv_timeout) == esp_ssl_bio_recv_timeout) {
        return;                             /* already installed */
    }
    if (ssl->MBEDTLS_PRIVATE(f_recv) == NULL &&
        ssl->MBEDTLS_PRIVATE(f_recv_timeout) == NULL) {
        return;                             /* BIO not configured yet */
    }

    struct esp_ssl_bio *bio = mbedtls_calloc(1, sizeof(*bio));
    if (bio == NULL) {
        ESP_LOGD(TAG, "BIO interpose alloc failed; overflow check disabled");
        return;                             /* degrade gracefully */
    }

    bio->ssl = ssl;
    bio->p_bio = ssl->MBEDTLS_PRIVATE(p_bio);
    bio->f_send = ssl->MBEDTLS_PRIVATE(f_send);
    bio->f_recv = ssl->MBEDTLS_PRIVATE(f_recv);
    bio->f_recv_timeout = ssl->MBEDTLS_PRIVATE(f_recv_timeout);

    ssl->MBEDTLS_PRIVATE(p_bio) = bio;
    if (bio->f_send) {
        ssl->MBEDTLS_PRIVATE(f_send) = esp_ssl_bio_send;
    }
    if (bio->f_recv) {
        ssl->MBEDTLS_PRIVATE(f_recv) = esp_ssl_bio_recv;
    }
    if (bio->f_recv_timeout) {
        ssl->MBEDTLS_PRIVATE(f_recv_timeout) = esp_ssl_bio_recv_timeout;
    }
}

void esp_mbedtls_free_bio(mbedtls_ssl_context *ssl)
{
    if (ssl->MBEDTLS_PRIVATE(f_recv) != esp_ssl_bio_recv &&
        ssl->MBEDTLS_PRIVATE(f_recv_timeout) != esp_ssl_bio_recv_timeout) {
        return;                             /* nothing installed */
    }

    struct esp_ssl_bio *bio = ssl->MBEDTLS_PRIVATE(p_bio);

    /* Restore the application's callbacks; guards against a double free. */
    ssl->MBEDTLS_PRIVATE(p_bio) = bio->p_bio;
    ssl->MBEDTLS_PRIVATE(f_send) = bio->f_send;
    ssl->MBEDTLS_PRIVATE(f_recv) = bio->f_recv;
    ssl->MBEDTLS_PRIVATE(f_recv_timeout) = bio->f_recv_timeout;
    mbedtls_free(bio);
}

int esp_mbedtls_add_rx_buffer(mbedtls_ssl_context *ssl)
{
    /*
     * If RX buffer is set to static mode, this macro will return early
     * and skip dynamic buffer allocation logic below
     */
    ESP_MBEDTLS_RETURN_IF_RX_BUF_STATIC(ssl);

    /* Interpose the overflow-checking BIO trampolines before any network read. */
    esp_mbedtls_install_bio(ssl);

    int cached = 0;
    int ret = 0;
    int buffer_len, content_len = 0;
    struct esp_mbedtls_ssl_buf *esp_buf;
    unsigned char cache_buf[16];
    unsigned char msg_head[9];
    size_t in_msglen, in_left;

    ESP_LOGV(TAG, "--> add rx");

    if (ssl->MBEDTLS_PRIVATE(in_buf)) {
        if (esp_mbedtls_get_buf_state(ssl->MBEDTLS_PRIVATE(in_buf)) == ESP_MBEDTLS_SSL_BUF_CACHED) {
            ESP_LOGV(TAG, "in buffer is not empty");
            ret = 0;
            goto exit;
        } else {
            cached = 1;
        }
    }

    ssl->MBEDTLS_PRIVATE(in_hdr) = msg_head;
    ssl->MBEDTLS_PRIVATE(in_len) = msg_head + 3;

    if ((ret = mbedtls_ssl_fetch_input(ssl, mbedtls_ssl_in_hdr_len(ssl))) != 0) {
        if (ret == MBEDTLS_ERR_SSL_TIMEOUT) {
            ESP_LOGD(TAG, "mbedtls_ssl_fetch_input reads data times out");
        } else if (ret == MBEDTLS_ERR_SSL_WANT_READ) {
            ESP_LOGD(TAG, "mbedtls_ssl_fetch_input wants to read more data");
        } else if (ret == MBEDTLS_ERR_SSL_CONN_EOF) {
            ESP_LOGD(TAG, "mbedtls_ssl_fetch_input connection EOF");
        } else {
            ESP_LOGE(TAG, "mbedtls_ssl_fetch_input error=%d", -ret);
        }

        goto exit;
    }

    esp_mbedtls_parse_record_header(ssl);

    if ((ret = rx_reassembly_content_len(ssl, msg_head, &content_len)) != 0) {
        goto exit;
    }
    buffer_len = tx_buffer_len(ssl, content_len);

    in_left = ssl->MBEDTLS_PRIVATE(in_left);
    in_msglen = ssl->MBEDTLS_PRIVATE(in_msglen);

    ESP_LOGV(TAG, "message length is %d RX buffer length should be %d left is %d",
                (int)in_msglen, (int)buffer_len, (int)ssl->MBEDTLS_PRIVATE(in_left));

    if (cached) {
        memcpy(cache_buf, ssl->MBEDTLS_PRIVATE(in_buf), 16);
        esp_mbedtls_free_buf(ssl->MBEDTLS_PRIVATE(in_buf));
        init_rx_buffer(ssl, NULL);
    }

    esp_buf = mbedtls_calloc(1, SSL_BUF_HEAD_OFFSET_SIZE + buffer_len);
    if (!esp_buf) {
        ESP_LOGE(TAG, "alloc(%d bytes) failed", SSL_BUF_HEAD_OFFSET_SIZE + buffer_len);
        ret = MBEDTLS_ERR_SSL_ALLOC_FAILED;
        goto exit;
    }

    ESP_LOGV(TAG, "add in buffer %d bytes @ %p", buffer_len, esp_buf->buf);

    esp_mbedtls_init_ssl_buf(esp_buf, buffer_len);
    init_rx_buffer(ssl, esp_buf->buf);

    if (cached) {
        memcpy(ssl->MBEDTLS_PRIVATE(in_ctr), cache_buf, 8);
        memcpy(ssl->MBEDTLS_PRIVATE(in_iv), cache_buf + 8, 8);
    }

    memcpy(ssl->MBEDTLS_PRIVATE(in_hdr), msg_head, in_left);
    ssl->MBEDTLS_PRIVATE(in_left) = in_left;
    ssl->MBEDTLS_PRIVATE(in_msglen) = 0;

exit:
    ESP_LOGV(TAG, "<-- add rx");

    return ret;
}

int esp_mbedtls_free_rx_buffer(mbedtls_ssl_context *ssl)
{
    /*
     * If RX buffer is set to static mode, this macro will return early
     * and skip dynamic buffer free logic below
     */
    ESP_MBEDTLS_RETURN_IF_RX_BUF_STATIC(ssl);

    int ret = 0;
    unsigned char buf[16];
    struct esp_mbedtls_ssl_buf *esp_buf;

    ESP_LOGV(TAG, "--> free rx");

    /**
     * When have read multi messages once, can't free the input buffer directly.
     */
    if (!ssl->MBEDTLS_PRIVATE(in_buf) || (ssl->MBEDTLS_PRIVATE(in_hslen) && (ssl->MBEDTLS_PRIVATE(in_hslen) < ssl->MBEDTLS_PRIVATE(in_msglen))) ||
        (ssl->MBEDTLS_PRIVATE(in_buf) && (esp_mbedtls_get_buf_state(ssl->MBEDTLS_PRIVATE(in_buf)) == ESP_MBEDTLS_SSL_BUF_NO_CACHED))) {
        ret = 0;
        goto exit;
    }

    /**
     * The previous processing is just skipped, so "ssl->MBEDTLS_PRIVATE(in_msglen) = 0"
     */
    if (!ssl->MBEDTLS_PRIVATE(in_msgtype)
#if defined(MBEDTLS_SSL_SRV_C)
        /**
         * The ssl server read ClientHello manually without mbedtls_ssl_read_record(), so in_msgtype is not set and is zero.
         * ClientHello has been processed and rx buffer should be freed.
         * After processing ClientHello, the ssl state has been changed to MBEDTLS_SSL_SERVER_HELLO.
         */
        && !(ssl->MBEDTLS_PRIVATE(conf)->MBEDTLS_PRIVATE(endpoint) == MBEDTLS_SSL_IS_SERVER && ssl->MBEDTLS_PRIVATE(state) == MBEDTLS_SSL_SERVER_HELLO)
#endif
    ) {
        goto exit;
    }

    memcpy(buf, ssl->MBEDTLS_PRIVATE(in_ctr), 8);
    memcpy(buf + 8, ssl->MBEDTLS_PRIVATE(in_iv), 8);

    esp_mbedtls_free_buf(ssl->MBEDTLS_PRIVATE(in_buf));
    init_rx_buffer(ssl, NULL);

    esp_buf = mbedtls_calloc(1, SSL_BUF_HEAD_OFFSET_SIZE + 16);
    if (!esp_buf) {
        ESP_LOGE(TAG, "alloc(%d bytes) failed", SSL_BUF_HEAD_OFFSET_SIZE + 16);
        ret = MBEDTLS_ERR_SSL_ALLOC_FAILED;
        goto exit;
    }

    esp_mbedtls_init_ssl_buf(esp_buf, 16);
    memcpy(esp_buf->buf, buf, 16);
    init_rx_buffer(ssl, esp_buf->buf);
    esp_mbedtls_set_buf_state(ssl->MBEDTLS_PRIVATE(in_buf), ESP_MBEDTLS_SSL_BUF_NO_CACHED);
exit:
    ESP_LOGV(TAG, "<-- free rx");

    return ret;
}

size_t esp_mbedtls_get_crt_size(mbedtls_x509_crt *cert, size_t *num)
{
    size_t n = 0;
    size_t bytes = 0;

    while (cert) {
        bytes += cert->raw.len;
        n++;

        cert = cert->next;
    }

    *num = n;

    return bytes;
}

#ifdef CONFIG_MBEDTLS_DYNAMIC_FREE_CONFIG_DATA
void esp_mbedtls_free_keycert(mbedtls_ssl_context *ssl)
{
    mbedtls_ssl_config *conf = (mbedtls_ssl_config * )mbedtls_ssl_context_get_config(ssl);
    mbedtls_ssl_key_cert *keycert = conf->MBEDTLS_PRIVATE(key_cert), *next;

    while (keycert) {
        next = keycert->next;

        if (keycert) {
            mbedtls_free(keycert);
        }

        keycert = next;
    }

    conf->MBEDTLS_PRIVATE(key_cert) = NULL;
}

void esp_mbedtls_free_keycert_key(mbedtls_ssl_context *ssl)
{
    const mbedtls_ssl_config *conf = mbedtls_ssl_context_get_config(ssl);
    mbedtls_ssl_key_cert *keycert = conf->MBEDTLS_PRIVATE(key_cert);

    while (keycert) {
        if (keycert->key) {
            mbedtls_pk_free(keycert->key);
            keycert->key = NULL;
        }
        keycert = keycert->next;
    }
}

void esp_mbedtls_free_keycert_cert(mbedtls_ssl_context *ssl)
{
    const mbedtls_ssl_config *conf = mbedtls_ssl_context_get_config(ssl);
    mbedtls_ssl_key_cert *keycert = conf->MBEDTLS_PRIVATE(key_cert);

    while (keycert) {
        if (keycert->cert) {
            mbedtls_x509_crt_free(keycert->cert);
            keycert->cert = NULL;
        }
        keycert = keycert->next;
    }
}
#endif /* CONFIG_MBEDTLS_DYNAMIC_FREE_CONFIG_DATA */

#ifdef CONFIG_MBEDTLS_DYNAMIC_FREE_CA_CERT
void esp_mbedtls_free_cacert(mbedtls_ssl_context *ssl)
{
    if (ssl->MBEDTLS_PRIVATE(conf)->MBEDTLS_PRIVATE(ca_chain)) {
        mbedtls_ssl_config *conf = (mbedtls_ssl_config * )mbedtls_ssl_context_get_config(ssl);

#if CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
        /* In case of mbedtls certificate bundle, we attach a "static const"
         * dummy cert, thus we need to avoid the write operations (memset())
         * performed by `mbedtls_x509_crt_free()`
         */
        if (!esp_crt_bundle_in_use(conf->MBEDTLS_PRIVATE(ca_chain))) {
            mbedtls_x509_crt_free(conf->MBEDTLS_PRIVATE(ca_chain));
        }
#else
        mbedtls_x509_crt_free(conf->MBEDTLS_PRIVATE(ca_chain));
#endif

        conf->MBEDTLS_PRIVATE(ca_chain) = NULL;
    }
}
#endif /* CONFIG_MBEDTLS_DYNAMIC_FREE_CA_CERT */
