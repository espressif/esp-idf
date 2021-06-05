// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <netdb.h>
#include <esp_log.h>
#include <http_parser.h>

#include "sh2lib.h"

static const char *TAG = "sh2lib";

#define DBG_FRAME_SEND 1

/*
 * The implementation of nghttp2_send_callback type. Here we write
 * |data| with size |length| to the network and return the number of
 * bytes actually written. See the documentation of
 * nghttp2_send_callback for the details.
 */
static ssize_t callback_send_inner(struct sh2lib_handle *hd, const uint8_t *data,
                                   size_t length)
{
    int rv = esp_tls_conn_write(hd->http2_tls, data, length);
    if (rv <= 0) {
        if (rv == ESP_TLS_ERR_SSL_WANT_READ || rv == ESP_TLS_ERR_SSL_WANT_WRITE) {
            rv = NGHTTP2_ERR_WOULDBLOCK;
        } else {
            rv = NGHTTP2_ERR_CALLBACK_FAILURE;
        }
    }
    return rv;
}

static ssize_t callback_send(nghttp2_session *session, const uint8_t *data,
                             size_t length, int flags, void *user_data)
{
    int rv = 0;
    struct sh2lib_handle *hd = user_data;

    int copy_offset = 0;
    int pending_data = length;

    /* Send data in 1000 byte chunks */
    while (copy_offset != length) {
        int chunk_len = pending_data > 1000 ? 1000 : pending_data;
        int subrv = callback_send_inner(hd, data + copy_offset, chunk_len);
        if (subrv <= 0) {
            if (copy_offset == 0) {
                /* If no data is transferred, send the error code */
                rv = subrv;
            }
            break;
        }
        copy_offset += subrv;
        pending_data -= subrv;
        rv += subrv;
    }
    return rv;
}

/*
 * The implementation of nghttp2_recv_callback type. Here we read data
 * from the network and write them in |buf|. The capacity of |buf| is
 * |length| bytes. Returns the number of bytes stored in |buf|. See
 * the documentation of nghttp2_recv_callback for the details.
 */
static ssize_t callback_recv(nghttp2_session *session, uint8_t *buf,
                             size_t length, int flags, void *user_data)
{
    struct sh2lib_handle *hd = user_data;
    int rv;
    rv = esp_tls_conn_read(hd->http2_tls, (char *)buf, (int)length);
    if (rv < 0) {
        if (rv == ESP_TLS_ERR_SSL_WANT_READ || rv == ESP_TLS_ERR_SSL_WANT_WRITE) {
            rv = NGHTTP2_ERR_WOULDBLOCK;
        } else {
            rv = NGHTTP2_ERR_CALLBACK_FAILURE;
        }
    } else if (rv == 0) {
        rv = NGHTTP2_ERR_EOF;
    }
    return rv;
}

const char *sh2lib_frame_type_str(int type)
{
    switch (type) {
    case NGHTTP2_HEADERS:
        return "HEADERS";
        break;
    case NGHTTP2_RST_STREAM:
        return "RST_STREAM";
        break;
    case NGHTTP2_GOAWAY:
        return "GOAWAY";
        break;
    case NGHTTP2_DATA:
        return "DATA";
        break;
    case NGHTTP2_SETTINGS:
        return "SETTINGS";
        break;
    case NGHTTP2_PUSH_PROMISE:
        return "PUSH_PROMISE";
        break;
    case NGHTTP2_PING:
        return "PING";
        break;
    default:
        return "other";
        break;
    }
}

static int callback_on_frame_send(nghttp2_session *session,
                                  const nghttp2_frame *frame, void *user_data)
{
    ESP_LOGD(TAG, "[frame-send] frame type %s", sh2lib_frame_type_str(frame->hd.type));
    switch (frame->hd.type) {
    case NGHTTP2_HEADERS:
        if (nghttp2_session_get_stream_user_data(session, frame->hd.stream_id)) {
            ESP_LOGD(TAG, "[frame-send] C ----------------------------> S (HEADERS)");
#if DBG_FRAME_SEND
            ESP_LOGD(TAG, "[frame-send] headers nv-len = %d", frame->headers.nvlen);
            const nghttp2_nv *nva = frame->headers.nva;
            size_t i;
            for (i = 0; i < frame->headers.nvlen; ++i) {
                ESP_LOGD(TAG, "[frame-send] %s : %s", nva[i].name, nva[i].value);
            }
#endif
        }
        break;
    }
    return 0;
}

static int callback_on_frame_recv(nghttp2_session *session,
                                  const nghttp2_frame *frame, void *user_data)
{
    ESP_LOGD(TAG, "[frame-recv][sid: %d] frame type  %s", frame->hd.stream_id, sh2lib_frame_type_str(frame->hd.type));
    if (frame->hd.type != NGHTTP2_DATA) {
        return 0;
    }
    /* Subsequent processing only for data frame */
    sh2lib_frame_data_recv_cb_t data_recv_cb = nghttp2_session_get_stream_user_data(session, frame->hd.stream_id);
    if (data_recv_cb) {
        struct sh2lib_handle *h2 = user_data;
        (*data_recv_cb)(h2, NULL, 0, DATA_RECV_FRAME_COMPLETE);
    }
    return 0;
}

static int callback_on_stream_close(nghttp2_session *session, int32_t stream_id,
                                    uint32_t error_code, void *user_data)
{

    ESP_LOGD(TAG, "[stream-close][sid %d]", stream_id);
    sh2lib_frame_data_recv_cb_t data_recv_cb = nghttp2_session_get_stream_user_data(session, stream_id);
    if (data_recv_cb) {
        struct sh2lib_handle *h2 = user_data;
        (*data_recv_cb)(h2, NULL, 0, DATA_RECV_RST_STREAM);
    }
    return 0;
}

static int callback_on_data_chunk_recv(nghttp2_session *session, uint8_t flags,
                                       int32_t stream_id, const uint8_t *data,
                                       size_t len, void *user_data)
{
    sh2lib_frame_data_recv_cb_t data_recv_cb;
    ESP_LOGD(TAG, "[data-chunk][sid:%d]", stream_id);
    data_recv_cb = nghttp2_session_get_stream_user_data(session, stream_id);
    if (data_recv_cb) {
        ESP_LOGD(TAG, "[data-chunk] C <---------------------------- S (DATA chunk)"
                "%lu bytes",
                (unsigned long int)len);
        struct sh2lib_handle *h2 = user_data;
        (*data_recv_cb)(h2, (char *)data, len, 0);
        /* TODO: What to do with the return value: look for pause/abort */
    }
    return 0;
}

static int callback_on_header(nghttp2_session *session, const nghttp2_frame *frame,
                              const uint8_t *name, size_t namelen, const uint8_t *value,
                              size_t valuelen, uint8_t flags, void *user_data)
{
    ESP_LOGD(TAG, "[hdr-recv][sid:%d] %s : %s", frame->hd.stream_id, name, value);
    return 0;
}

static int do_http2_connect(struct sh2lib_handle *hd)
{
    int ret;
    nghttp2_session_callbacks *callbacks;
    nghttp2_session_callbacks_new(&callbacks);
    nghttp2_session_callbacks_set_send_callback(callbacks, callback_send);
    nghttp2_session_callbacks_set_recv_callback(callbacks, callback_recv);
    nghttp2_session_callbacks_set_on_frame_send_callback(callbacks, callback_on_frame_send);
    nghttp2_session_callbacks_set_on_frame_recv_callback(callbacks, callback_on_frame_recv);
    nghttp2_session_callbacks_set_on_stream_close_callback(callbacks, callback_on_stream_close);
    nghttp2_session_callbacks_set_on_data_chunk_recv_callback(callbacks, callback_on_data_chunk_recv);
    nghttp2_session_callbacks_set_on_header_callback(callbacks, callback_on_header);
    ret = nghttp2_session_client_new(&hd->http2_sess, callbacks, hd);
    if (ret != 0) {
        ESP_LOGE(TAG, "[sh2-connect] New http2 session failed");
        nghttp2_session_callbacks_del(callbacks);
        return -1;
    }
    nghttp2_session_callbacks_del(callbacks);

    /* Create the SETTINGS frame */
    ret = nghttp2_submit_settings(hd->http2_sess, NGHTTP2_FLAG_NONE, NULL, 0);
    if (ret != 0) {
        ESP_LOGE(TAG, "[sh2-connect] Submit settings failed");
        return -1;
    }
    return 0;
}

int sh2lib_connect(struct sh2lib_config_t *cfg, struct sh2lib_handle *hd)
{
    memset(hd, 0, sizeof(*hd));

    if (cfg == NULL) {
        ESP_LOGE(TAG, "[sh2-connect] pointer to sh2lib configurations cannot be NULL");
        goto error;
    }

    const char *proto[] = {"h2", NULL};
    esp_tls_cfg_t tls_cfg = {
        .alpn_protos = proto,
        .cacert_buf = cfg->cacert_buf,
        .cacert_bytes = cfg->cacert_bytes,
        .non_block = true,
        .timeout_ms = 10 * 1000,
    };
    if ((hd->http2_tls = esp_tls_conn_http_new(cfg->uri, &tls_cfg)) == NULL) {
        ESP_LOGE(TAG, "[sh2-connect] esp-tls connection failed");
        goto error;
    }
    struct http_parser_url u;
    http_parser_url_init(&u);
    http_parser_parse_url(cfg->uri, strlen(cfg->uri), 0, &u);
    hd->hostname = strndup(&cfg->uri[u.field_data[UF_HOST].off], u.field_data[UF_HOST].len);

    /* HTTP/2 Connection */
    if (do_http2_connect(hd) != 0) {
        ESP_LOGE(TAG, "[sh2-connect] HTTP2 Connection failed with %s", cfg->uri);
        goto error;
    }

    return 0;
error:
    sh2lib_free(hd);
    return -1;
}

void sh2lib_free(struct sh2lib_handle *hd)
{
    if (hd->http2_sess) {
        nghttp2_session_del(hd->http2_sess);
        hd->http2_sess = NULL;
    }
    if (hd->http2_tls) {
	esp_tls_conn_delete(hd->http2_tls);
        hd->http2_tls = NULL;
    }
    if (hd->hostname) {
        free(hd->hostname);
        hd->hostname = NULL;
    }
}

int sh2lib_execute(struct sh2lib_handle *hd)
{
    int ret;
    ret = nghttp2_session_send(hd->http2_sess);
    if (ret != 0) {
        ESP_LOGE(TAG, "[sh2-execute] HTTP2 session send failed %d", ret);
        return -1;
    }

    ret = nghttp2_session_recv(hd->http2_sess);
    if (ret != 0) {
        ESP_LOGE(TAG, "[sh2-execute] HTTP2 session recv failed %d", ret);
        return -1;
    }

    return 0;
}

int sh2lib_do_get_with_nv(struct sh2lib_handle *hd, const nghttp2_nv *nva, size_t nvlen, sh2lib_frame_data_recv_cb_t recv_cb)
{
    int ret = nghttp2_submit_request(hd->http2_sess, NULL, nva, nvlen, NULL, recv_cb);
    if (ret < 0) {
        ESP_LOGE(TAG, "[sh2-do-get] HEADERS call failed");
        return -1;
    }
    return ret;
}

int sh2lib_do_get(struct sh2lib_handle *hd, const char *path, sh2lib_frame_data_recv_cb_t recv_cb)
{
    const nghttp2_nv nva[] = { SH2LIB_MAKE_NV(":method", "GET"),
                               SH2LIB_MAKE_NV(":scheme", "https"),
                               SH2LIB_MAKE_NV(":authority", hd->hostname),
                               SH2LIB_MAKE_NV(":path", path),
                             };
    return sh2lib_do_get_with_nv(hd, nva, sizeof(nva) / sizeof(nva[0]), recv_cb);
}

ssize_t sh2lib_data_provider_cb(nghttp2_session *session, int32_t stream_id, uint8_t *buf,
                                size_t length, uint32_t *data_flags,
                                nghttp2_data_source *source, void *user_data)
{
    struct sh2lib_handle *h2 = user_data;
    sh2lib_putpost_data_cb_t data_cb = source->ptr;
    return (*data_cb)(h2, (char *)buf, length, data_flags);
}

int sh2lib_do_putpost_with_nv(struct sh2lib_handle *hd, const nghttp2_nv *nva, size_t nvlen,
                              sh2lib_putpost_data_cb_t send_cb,
                              sh2lib_frame_data_recv_cb_t recv_cb)
{

    nghttp2_data_provider sh2lib_data_provider;
    sh2lib_data_provider.read_callback = sh2lib_data_provider_cb;
    sh2lib_data_provider.source.ptr = send_cb;
    int ret = nghttp2_submit_request(hd->http2_sess, NULL, nva, nvlen, &sh2lib_data_provider, recv_cb);
    if (ret < 0) {
        ESP_LOGE(TAG, "[sh2-do-putpost] HEADERS call failed");
        return -1;
    }
    return ret;
}

int sh2lib_do_post(struct sh2lib_handle *hd, const char *path,
                   sh2lib_putpost_data_cb_t send_cb,
                   sh2lib_frame_data_recv_cb_t recv_cb)
{
    const nghttp2_nv nva[] = { SH2LIB_MAKE_NV(":method", "POST"),
                               SH2LIB_MAKE_NV(":scheme", "https"),
                               SH2LIB_MAKE_NV(":authority", hd->hostname),
                               SH2LIB_MAKE_NV(":path", path),
                             };
    return sh2lib_do_putpost_with_nv(hd, nva, sizeof(nva) / sizeof(nva[0]), send_cb, recv_cb);
}

int sh2lib_do_put(struct sh2lib_handle *hd, const char *path,
                  sh2lib_putpost_data_cb_t send_cb,
                  sh2lib_frame_data_recv_cb_t recv_cb)
{
    const nghttp2_nv nva[] = { SH2LIB_MAKE_NV(":method", "PUT"),
                               SH2LIB_MAKE_NV(":scheme", "https"),
                               SH2LIB_MAKE_NV(":authority", hd->hostname),
                               SH2LIB_MAKE_NV(":path", path),
                             };
    return sh2lib_do_putpost_with_nv(hd, nva, sizeof(nva) / sizeof(nva[0]), send_cb, recv_cb);
}
