/*
 * SPDX-FileCopyrightText: 2018-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <errno.h>
#include <esp_log.h>
#include <esp_err.h>

#include <esp_http_server.h>
#include "esp_httpd_priv.h"
#include <netinet/tcp.h>
#include "ctrl_sock.h"

static const char *TAG = "httpd_txrx";

esp_err_t httpd_sess_set_send_override(httpd_handle_t hd, int sockfd, httpd_send_func_t send_func)
{
    struct sock_db *sess = httpd_sess_get(hd, sockfd);
    if (!sess) {
        return ESP_ERR_INVALID_ARG;
    }
    sess->send_fn = send_func;
    return ESP_OK;
}

esp_err_t httpd_sess_set_recv_override(httpd_handle_t hd, int sockfd, httpd_recv_func_t recv_func)
{
    struct sock_db *sess = httpd_sess_get(hd, sockfd);
    if (!sess) {
        return ESP_ERR_INVALID_ARG;
    }
    sess->recv_fn = recv_func;
    return ESP_OK;
}

esp_err_t httpd_sess_set_pending_override(httpd_handle_t hd, int sockfd, httpd_pending_func_t pending_func)
{
    struct sock_db *sess = httpd_sess_get(hd, sockfd);
    if (!sess) {
        return ESP_ERR_INVALID_ARG;
    }
    sess->pending_fn = pending_func;
    return ESP_OK;
}

int httpd_send(httpd_req_t *r, const char *buf, size_t buf_len)
{
    if (r == NULL || buf == NULL) {
        return HTTPD_SOCK_ERR_INVALID;
    }

    if (!httpd_valid_req(r)) {
        return HTTPD_SOCK_ERR_INVALID;
    }

    struct httpd_req_aux *ra = r->aux;
    int ret = ra->sd->send_fn(ra->sd->handle, ra->sd->fd, buf, buf_len, 0);
    if (ret < 0) {
        ESP_LOGD(TAG, LOG_FMT("error in send_fn"));
        return ret;
    }
    return ret;
}

static esp_err_t httpd_send_all(httpd_req_t *r, const char *buf, size_t buf_len)
{
    struct httpd_req_aux *ra = r->aux;
    int ret;

    while (buf_len > 0) {
        ret = ra->sd->send_fn(ra->sd->handle, ra->sd->fd, buf, buf_len, 0);
        if (ret < 0) {
            ESP_LOGD(TAG, LOG_FMT("error in send_fn"));
            return ESP_FAIL;
        }
        ESP_LOGD(TAG, LOG_FMT("sent = %d"), ret);
        buf     += ret;
        buf_len -= ret;
    }
    return ESP_OK;
}

static size_t httpd_recv_pending(httpd_req_t *r, char *buf, size_t buf_len)
{
    struct httpd_req_aux *ra = r->aux;
    size_t offset = sizeof(ra->sd->pending_data) - ra->sd->pending_len;

    /* buf_len must not be greater than remaining_len */
    buf_len = MIN(ra->sd->pending_len, buf_len);
    memcpy(buf, ra->sd->pending_data + offset, buf_len);

    ra->sd->pending_len -= buf_len;
    return buf_len;
}

int httpd_recv_with_opt(httpd_req_t *r, char *buf, size_t buf_len, httpd_recv_opt_t opt)
{
    ESP_LOGD(TAG, LOG_FMT("requested length = %"NEWLIB_NANO_COMPAT_FORMAT), NEWLIB_NANO_COMPAT_CAST(buf_len));

    size_t pending_len = 0;
    struct httpd_req_aux *ra = r->aux;

    /* First fetch pending data from local buffer */
    if (ra->sd->pending_len > 0) {
        ESP_LOGD(TAG, LOG_FMT("pending length = %"NEWLIB_NANO_COMPAT_FORMAT), NEWLIB_NANO_COMPAT_CAST(ra->sd->pending_len));
        pending_len = httpd_recv_pending(r, buf, buf_len);
        buf     += pending_len;
        buf_len -= pending_len;

        /* If buffer filled then no need to recv.
         * If asked to halt after receiving pending data then
         * return with received length */
        if (!buf_len || opt == HTTPD_RECV_OPT_HALT_AFTER_PENDING) {
            return pending_len;
        }
    }

    /* Receive data of remaining length */
    size_t recv_len = pending_len;
    do {
        int ret = ra->sd->recv_fn(ra->sd->handle, ra->sd->fd, buf, buf_len, 0);
        if (ret <= 0) {
            ESP_LOGD(TAG, LOG_FMT("error in recv_fn"));
            if ((ret == HTTPD_SOCK_ERR_TIMEOUT) && (pending_len != 0)) {
                /* If recv() timeout occurred, but pending data is
                * present, return length of pending data.
                * This behavior is similar to that of socket recv()
                * function, which, in case has only partially read the
                * requested length, due to timeout, returns with read
                * length, rather than error */
                return pending_len;
            }
            return ret;
        }

        recv_len += ret;
        buf      += ret;
        buf_len  -= ret;
    } while (buf_len > 0 && opt == HTTPD_RECV_OPT_BLOCKING);

    ESP_LOGD(TAG, LOG_FMT("received length = %"NEWLIB_NANO_COMPAT_FORMAT), NEWLIB_NANO_COMPAT_CAST(recv_len));
    return recv_len;
}

int httpd_recv(httpd_req_t *r, char *buf, size_t buf_len)
{
    return httpd_recv_with_opt(r, buf, buf_len, HTTPD_RECV_OPT_NONE);
}

size_t httpd_unrecv(struct httpd_req *r, const char *buf, size_t buf_len)
{
    struct httpd_req_aux *ra = r->aux;
    /* Truncate if external buf_len is greater than pending_data buffer size */
    ra->sd->pending_len = MIN(sizeof(ra->sd->pending_data), buf_len);

    /* Copy data into internal pending_data buffer with the exact offset
     * such that it is right aligned inside the buffer */
    size_t offset = sizeof(ra->sd->pending_data) - ra->sd->pending_len;
    memcpy(ra->sd->pending_data + offset, buf, ra->sd->pending_len);
    ESP_LOGD(TAG, LOG_FMT("length = %"NEWLIB_NANO_COMPAT_FORMAT), NEWLIB_NANO_COMPAT_CAST(ra->sd->pending_len));
    return ra->sd->pending_len;
}

/**
 * This API appends an additional header field-value pair in the HTTP response.
 * But the header isn't sent out until any of the send APIs is executed.
 */
esp_err_t httpd_resp_set_hdr(httpd_req_t *r, const char *field, const char *value)
{
    if (r == NULL || field == NULL || value == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!httpd_valid_req(r)) {
        return ESP_ERR_HTTPD_INVALID_REQ;
    }

    struct httpd_req_aux *ra = r->aux;
    struct httpd_data *hd = (struct httpd_data *) r->handle;

    /* Number of additional headers is limited */
    if (ra->resp_hdrs_count >= hd->config.max_resp_headers) {
        return ESP_ERR_HTTPD_RESP_HDR;
    }

    /* Assign header field-value pair */
    ra->resp_hdrs[ra->resp_hdrs_count].field = field;
    ra->resp_hdrs[ra->resp_hdrs_count].value = value;
    ra->resp_hdrs_count++;

    ESP_LOGD(TAG, LOG_FMT("new header = %s: %s"), field, value);
    return ESP_OK;
}

/**
 * This API sets the status of the HTTP response to the value specified.
 * But the status isn't sent out until any of the send APIs is executed.
 */
esp_err_t httpd_resp_set_status(httpd_req_t *r, const char *status)
{
    if (r == NULL || status == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!httpd_valid_req(r)) {
        return ESP_ERR_HTTPD_INVALID_REQ;
    }

    struct httpd_req_aux *ra = r->aux;
    ra->status = (char *)status;
    return ESP_OK;
}

/**
 * This API sets the method/type of the HTTP response to the value specified.
 * But the method isn't sent out until any of the send APIs is executed.
 */
esp_err_t httpd_resp_set_type(httpd_req_t *r, const char *type)
{
    if (r == NULL || type == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!httpd_valid_req(r)) {
        return ESP_ERR_HTTPD_INVALID_REQ;
    }

    struct httpd_req_aux *ra = r->aux;
    ra->content_type = (char *)type;
    return ESP_OK;
}

esp_err_t httpd_resp_send(httpd_req_t *r, const char *buf, ssize_t buf_len)
{
    if (r == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!httpd_valid_req(r)) {
        return ESP_ERR_HTTPD_INVALID_REQ;
    }

    struct httpd_req_aux *ra = r->aux;
    const char *httpd_hdr_str = "HTTP/1.1 %s\r\nContent-Type: %s\r\nContent-Length: %d\r\n";
    const char *colon_separator = ": ";
    const char *cr_lf_seperator = "\r\n";

    if (buf_len == HTTPD_RESP_USE_STRLEN) {
        buf_len = strlen(buf);
    }

    /* Request headers are no longer available */
    ra->req_hdrs_count = 0;

    /* Calculate the size of the headers. +1 for the null terminator */
    size_t required_size = snprintf(NULL, 0, httpd_hdr_str, ra->status, ra->content_type, buf_len) + 1;
    if (required_size > ra->max_req_hdr_len) {
        return ESP_ERR_HTTPD_RESP_HDR;
    }
    char *res_buf = malloc(required_size); /* Temporary buffer to store the headers */
    if (res_buf == NULL) {
        ESP_LOGE(TAG, "Unable to allocate httpd send buffer");
        return ESP_ERR_HTTPD_ALLOC_MEM;
    }

    esp_err_t ret = snprintf(res_buf, required_size, httpd_hdr_str, ra->status, ra->content_type, buf_len);
    if (ret < 0 || ret >= required_size) {
        free(res_buf);
        return ESP_ERR_HTTPD_RESP_HDR;
    }
    ESP_LOGD(TAG, "httpd send buffer size = %d", strlen(res_buf));
    ret = httpd_send_all(r, res_buf, strlen(res_buf));
    free(res_buf);
    if (ret != ESP_OK) {
        return ESP_ERR_HTTPD_RESP_SEND;
    }

    /* Sending additional headers based on set_header */
    for (unsigned i = 0; i < ra->resp_hdrs_count; i++) {
        /* Send header field */
        if (httpd_send_all(r, ra->resp_hdrs[i].field, strlen(ra->resp_hdrs[i].field)) != ESP_OK) {
            return ESP_ERR_HTTPD_RESP_SEND;
        }
        /* Send ': ' */
        if (httpd_send_all(r, colon_separator, strlen(colon_separator)) != ESP_OK) {
            return ESP_ERR_HTTPD_RESP_SEND;
        }
        /* Send header value */
        if (httpd_send_all(r, ra->resp_hdrs[i].value, strlen(ra->resp_hdrs[i].value)) != ESP_OK) {
            return ESP_ERR_HTTPD_RESP_SEND;
        }
        /* Send CR + LF */
        if (httpd_send_all(r, cr_lf_seperator, strlen(cr_lf_seperator)) != ESP_OK) {
            return ESP_ERR_HTTPD_RESP_SEND;
        }
    }

    /* End header section */
    if (httpd_send_all(r, cr_lf_seperator, strlen(cr_lf_seperator)) != ESP_OK) {
        return ESP_ERR_HTTPD_RESP_SEND;
    }
    struct httpd_data *hd = (struct httpd_data *) r->handle;
    hd->http_server_state = HTTP_SERVER_EVENT_HEADERS_SENT;
    esp_http_server_dispatch_event(HTTP_SERVER_EVENT_HEADERS_SENT, &(ra->sd->fd), sizeof(int));

    /* Sending content */
    if (buf && buf_len) {
        if (httpd_send_all(r, buf, buf_len) != ESP_OK) {
            return ESP_ERR_HTTPD_RESP_SEND;
        }
    }
    esp_http_server_event_data evt_data = {
        .fd = ra->sd->fd,
        .data_len = buf_len,
    };
    hd->http_server_state = HTTP_SERVER_EVENT_SENT_DATA;
    esp_http_server_dispatch_event(HTTP_SERVER_EVENT_SENT_DATA, &evt_data, sizeof(esp_http_server_event_data));
    return ESP_OK;
}

esp_err_t httpd_resp_send_chunk(httpd_req_t *r, const char *buf, ssize_t buf_len)
{
    if (r == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!httpd_valid_req(r)) {
        return ESP_ERR_HTTPD_INVALID_REQ;
    }

    if (buf_len == HTTPD_RESP_USE_STRLEN) {
        buf_len = strlen(buf);
    }

    struct httpd_req_aux *ra = r->aux;
    struct httpd_data *hd = (struct httpd_data *) r->handle;
    const char *httpd_chunked_hdr_str = "HTTP/1.1 %s\r\nContent-Type: %s\r\nTransfer-Encoding: chunked\r\n";
    const char *colon_separator = ": ";
    const char *cr_lf_seperator = "\r\n";

    /* Request headers are no longer available */
    ra->req_hdrs_count = 0;

    if (!ra->first_chunk_sent) {
        /* Calculate the size of the headers. +1 for the null terminator */
        size_t required_size = snprintf(NULL, 0, httpd_chunked_hdr_str, ra->status, ra->content_type) + 1;
        if (required_size > ra->max_req_hdr_len) {
            return ESP_ERR_HTTPD_RESP_HDR;
        }
        char *res_buf = malloc(required_size); /* Temporary buffer to store the headers */
        if (res_buf == NULL) {
            ESP_LOGE(TAG, "Unable to allocate httpd send chunk buffer");
            return ESP_ERR_HTTPD_ALLOC_MEM;
        }
        esp_err_t ret = snprintf(res_buf, required_size, httpd_chunked_hdr_str, ra->status, ra->content_type);
        if (ret < 0 || ret >= required_size) {
            free(res_buf);
            return ESP_ERR_HTTPD_RESP_HDR;
        }
        ESP_LOGD(TAG, "httpd send chunk buffer size = %d", strlen(res_buf));
        /* Size of essential headers is limited by scratch buffer size */
        ret = httpd_send_all(r, res_buf, strlen(res_buf));
        free(res_buf);
        if (ret != ESP_OK) {
            return ESP_ERR_HTTPD_RESP_SEND;
        }

        /* Sending additional headers based on set_header */
        for (unsigned i = 0; i < ra->resp_hdrs_count; i++) {
            /* Send header field */
            if (httpd_send_all(r, ra->resp_hdrs[i].field, strlen(ra->resp_hdrs[i].field)) != ESP_OK) {
                return ESP_ERR_HTTPD_RESP_SEND;
            }
            /* Send ': ' */
            if (httpd_send_all(r, colon_separator, strlen(colon_separator)) != ESP_OK) {
                return ESP_ERR_HTTPD_RESP_SEND;
            }
            /* Send header value */
            if (httpd_send_all(r, ra->resp_hdrs[i].value, strlen(ra->resp_hdrs[i].value)) != ESP_OK) {
                return ESP_ERR_HTTPD_RESP_SEND;
            }
            /* Send CR + LF */
            if (httpd_send_all(r, cr_lf_seperator, strlen(cr_lf_seperator)) != ESP_OK) {
                return ESP_ERR_HTTPD_RESP_SEND;
            }
        }

        /* End header section */
        if (httpd_send_all(r, cr_lf_seperator, strlen(cr_lf_seperator)) != ESP_OK) {
            return ESP_ERR_HTTPD_RESP_SEND;
        }
        ra->first_chunk_sent = true;
    }

    /* Sending chunked content */
    char len_str[10];
    snprintf(len_str, sizeof(len_str), "%lx\r\n", (long)buf_len);
    if (httpd_send_all(r, len_str, strlen(len_str)) != ESP_OK) {
        return ESP_ERR_HTTPD_RESP_SEND;
    }

    if (buf) {
        if (httpd_send_all(r, buf, (size_t) buf_len) != ESP_OK) {
            return ESP_ERR_HTTPD_RESP_SEND;
        }
    }

    /* Indicate end of chunk */
    if (httpd_send_all(r, "\r\n", strlen("\r\n")) != ESP_OK) {
        return ESP_ERR_HTTPD_RESP_SEND;
    }
    esp_http_server_event_data evt_data = {
        .fd = ra->sd->fd,
        .data_len = buf_len,
    };
    hd->http_server_state = HTTP_SERVER_EVENT_SENT_DATA;
    esp_http_server_dispatch_event(HTTP_SERVER_EVENT_SENT_DATA, &evt_data, sizeof(esp_http_server_event_data));

    return ESP_OK;
}

esp_err_t httpd_resp_send_err(httpd_req_t *req, httpd_err_code_t error, const char *usr_msg)
{
    esp_err_t ret;
    const char *msg;
    const char *status;

    switch (error) {
    case HTTPD_501_METHOD_NOT_IMPLEMENTED:
        status = "501 Method Not Implemented";
        msg    = "Server does not support this method";
        break;
    case HTTPD_505_VERSION_NOT_SUPPORTED:
        status = "505 Version Not Supported";
        msg    = "HTTP version not supported by server";
        break;
    case HTTPD_400_BAD_REQUEST:
        status = "400 Bad Request";
        msg    = "Bad request syntax";
        break;
    case HTTPD_401_UNAUTHORIZED:
        status = "401 Unauthorized";
        msg    = "No permission -- see authorization schemes";
        break;
    case HTTPD_403_FORBIDDEN:
        status = "403 Forbidden";
        msg    = "Request forbidden -- authorization will not help";
        break;
    case HTTPD_404_NOT_FOUND:
        status = "404 Not Found";
        msg    = "Nothing matches the given URI";
        break;
    case HTTPD_405_METHOD_NOT_ALLOWED:
        status = "405 Method Not Allowed";
        msg    = "Specified method is invalid for this resource";
        break;
    case HTTPD_408_REQ_TIMEOUT:
        status = "408 Request Timeout";
        msg    = "Server closed this connection";
        break;
    case HTTPD_414_URI_TOO_LONG:
        status = "414 URI Too Long";
        msg    = "URI is too long";
        break;
    case HTTPD_411_LENGTH_REQUIRED:
        status = "411 Length Required";
        msg    = "Client must specify Content-Length";
        break;
    case HTTPD_413_CONTENT_TOO_LARGE:
        status = "413 Content Too Large";
        msg    = "Content is too large";
        break;
    case HTTPD_431_REQ_HDR_FIELDS_TOO_LARGE:
        status = "431 Request Header Fields Too Large";
        msg    = "Header fields are too long";
        break;
    case HTTPD_500_INTERNAL_SERVER_ERROR:
    default:
        status = "500 Internal Server Error";
        msg    = "Server has encountered an unexpected error";
    }

    /* If user has provided custom message, override default message */
    msg = usr_msg ? usr_msg : msg;
    ESP_LOGW(TAG, LOG_FMT("%s - %s"), status, msg);

    /* Set error code in HTTP response */
    httpd_resp_set_status(req, status);
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);

#ifdef CONFIG_HTTPD_ERR_RESP_NO_DELAY
    /* Use TCP_NODELAY option to force socket to send data in buffer
     * This ensures that the error message is sent before the socket
     * is closed */
    struct httpd_req_aux *ra = req->aux;
    int nodelay = 1;
    if (setsockopt(ra->sd->fd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay)) < 0) {
        /* If failed to turn on TCP_NODELAY, throw warning and continue */
        ESP_LOGW(TAG, LOG_FMT("error calling setsockopt : %d"), errno);
        nodelay = 0;
    }
#endif

    /* Send HTTP error message */
    ret = httpd_resp_send(req, msg, HTTPD_RESP_USE_STRLEN);

#ifdef CONFIG_HTTPD_ERR_RESP_NO_DELAY
    /* If TCP_NODELAY was set successfully above, time to disable it */
    if (nodelay == 1) {
        nodelay = 0;
        if (setsockopt(ra->sd->fd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay)) < 0) {
            /* If failed to turn off TCP_NODELAY, throw error and
             * return failure to signal for socket closure */
            ESP_LOGE(TAG, LOG_FMT("error calling setsockopt : %d"), errno);
            return ESP_ERR_INVALID_STATE;
        }
    }
#endif
    esp_http_server_dispatch_event(HTTP_SERVER_EVENT_ERROR, &error, sizeof(httpd_err_code_t));

    return ret;
}

esp_err_t httpd_resp_send_custom_err(httpd_req_t *req, const char *status, const char *msg)
{
    ESP_LOGW(TAG, LOG_FMT("%s - %s"), status, msg);

    /* Set error code in HTTP response */
    httpd_resp_set_status(req, status);
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);

#ifdef CONFIG_HTTPD_ERR_RESP_NO_DELAY
    /* Use TCP_NODELAY option to force socket to send data in buffer
     * This ensures that the error message is sent before the socket
     * is closed */
    struct httpd_req_aux *ra = req->aux;
    int nodelay = 1;
    if (setsockopt(ra->sd->fd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay)) < 0) {
        /* If failed to turn on TCP_NODELAY, throw warning and continue */
        ESP_LOGW(TAG, LOG_FMT("error calling setsockopt : %d"), errno);
        nodelay = 0;
    }
#endif

    /* Send HTTP error message */
    esp_err_t ret = httpd_resp_send(req, msg, HTTPD_RESP_USE_STRLEN);

#ifdef CONFIG_HTTPD_ERR_RESP_NO_DELAY
    /* If TCP_NODELAY was set successfully above, time to disable it */
    if (nodelay == 1) {
        nodelay = 0;
        if (setsockopt(ra->sd->fd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay)) < 0) {
            /* If failed to turn off TCP_NODELAY, throw error and
             * return failure to signal for socket closure */
            ESP_LOGE(TAG, LOG_FMT("error calling setsockopt : %d"), errno);
            return ESP_ERR_INVALID_STATE;
        }
    }
#endif
    return ret;
}

esp_err_t httpd_register_err_handler(httpd_handle_t handle,
                                     httpd_err_code_t error,
                                     httpd_err_handler_func_t err_handler_fn)
{
    if (handle == NULL || error >= HTTPD_ERR_CODE_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    struct httpd_data *hd = (struct httpd_data *) handle;
    hd->err_handler_fns[error] = err_handler_fn;
    return ESP_OK;
}

esp_err_t httpd_req_handle_err(httpd_req_t *req, httpd_err_code_t error)
{
    struct httpd_data *hd = (struct httpd_data *) req->handle;
    esp_err_t ret;

    /* Invoke custom error handler if configured */
    if (hd->err_handler_fns[error]) {
        ret = hd->err_handler_fns[error](req, error);

        /* If error code is 500, force return failure
         * irrespective of the handler's return value */
        ret = (error == HTTPD_500_INTERNAL_SERVER_ERROR ? ESP_FAIL : ret);
    } else {
        /* If no handler is registered for this error default
         * behavior is to send the HTTP error response and
         * return failure for closure of underlying socket */
        httpd_resp_send_err(req, error, NULL);
        ret = ESP_FAIL;
    }
    return ret;
}

int httpd_req_recv(httpd_req_t *r, char *buf, size_t buf_len)
{
    if (r == NULL || buf == NULL) {
        return HTTPD_SOCK_ERR_INVALID;
    }

    if (!httpd_valid_req(r)) {
        ESP_LOGW(TAG, LOG_FMT("invalid request"));
        return HTTPD_SOCK_ERR_INVALID;
    }

    struct httpd_req_aux *ra = r->aux;
    ESP_LOGD(TAG, LOG_FMT("remaining length = %"NEWLIB_NANO_COMPAT_FORMAT), NEWLIB_NANO_COMPAT_CAST(ra->remaining_len));

    if (buf_len > ra->remaining_len) {
        buf_len = ra->remaining_len;
    }
    if (buf_len == 0) {
        return buf_len;
    }

    int ret = httpd_recv(r, buf, buf_len);
    if (ret < 0) {
        ESP_LOGD(TAG, LOG_FMT("error in httpd_recv"));
        return ret;
    }
    ra->remaining_len -= ret;
    ESP_LOGD(TAG, LOG_FMT("received length = %d"), ret);
    struct httpd_data *hd = (struct httpd_data *) r->handle;
    hd->http_server_state = HTTP_SERVER_EVENT_ON_DATA;
    esp_http_server_event_data evt_data = {
        .fd = ra->sd->fd,
        .data_len = ret,
    };
    esp_http_server_dispatch_event(HTTP_SERVER_EVENT_ON_DATA, &evt_data, sizeof(esp_http_server_event_data));
    return ret;
}

esp_err_t httpd_req_async_handler_begin(httpd_req_t *r, httpd_req_t **out)
{
    if (r == NULL || out == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    // alloc async req
    httpd_req_t *async = malloc(sizeof(httpd_req_t));
    if (async == NULL) {
        return ESP_ERR_NO_MEM;
    }
    memcpy(async, r, sizeof(httpd_req_t));

    // alloc async aux
    async->aux = malloc(sizeof(struct httpd_req_aux));
    if (async->aux == NULL) {
        free(async);
        return ESP_ERR_NO_MEM;
    }
    memcpy(async->aux, r->aux, sizeof(struct httpd_req_aux));

    // Copy response header block
    struct httpd_data *hd = (struct httpd_data *) r->handle;
    struct httpd_req_aux *async_aux = (struct httpd_req_aux *) async->aux;
    struct httpd_req_aux *r_aux = (struct httpd_req_aux *) r->aux;

    if (r_aux->scratch) {
        async_aux->scratch = malloc(r_aux->scratch_cur_size);
        if (async_aux->scratch == NULL) {
            free(async_aux);
            free(async);
            return ESP_ERR_NO_MEM;
        }
        memcpy(async_aux->scratch, r_aux->scratch, r_aux->scratch_cur_size);
    } else {
        async_aux->scratch = NULL;
    }

    async_aux->resp_hdrs = calloc(hd->config.max_resp_headers, sizeof(struct resp_hdr));
    if (async_aux->resp_hdrs == NULL) {
        free(async_aux->scratch);
        free(async_aux);
        free(async);
        return ESP_ERR_NO_MEM;
    }
    memcpy(async_aux->resp_hdrs, r_aux->resp_hdrs, hd->config.max_resp_headers * sizeof(struct resp_hdr));

    // Prevent the main thread from reading the rest of the request after the handler returns.
    r_aux->remaining_len = 0;

    // mark socket as "in use"
    r_aux->sd->for_async_req = true;

    *out = async;

    return ESP_OK;
}

esp_err_t httpd_req_async_handler_complete(httpd_req_t *r)
{
    if (r == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    // Get server handle and control socket info before freeing the request
    struct httpd_data *hd = (struct httpd_data *) r->handle;
    int msg_fd = hd->msg_fd;
    int port = hd->config.ctrl_port;

    struct httpd_req_aux *ra = r->aux;
    ra->sd->for_async_req = false;
    free(ra->scratch);
    ra->scratch = NULL;
    ra->scratch_cur_size = 0;
    ra->scratch_size_limit = 0;
    free(ra->resp_hdrs);
    free(r->aux);
    free(r);

    // Send a dummy control message(httpd_ctrl_data) to unblock the main HTTP server task from the select() call.
    // Since the current connection FD was marked as inactive for async requests, the main task
    // will now re-add this FD to its select() descriptor list. This ensures that subsequent requests
    // on the same FD are processed correctly
    struct httpd_ctrl_data msg = {.hc_msg = HTTPD_CTRL_MAX};
    int ret = cs_send_to_ctrl_sock(msg_fd, port, &msg, sizeof(msg));
    if (ret < 0) {
        ESP_LOGW(TAG, LOG_FMT("failed to send socket notification"));
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, LOG_FMT("socket notification sent"));
    return ESP_OK;
}

int httpd_req_to_sockfd(httpd_req_t *r)
{
    if (r == NULL) {
        return -1;
    }

    if (!httpd_valid_req(r)) {
        ESP_LOGW(TAG, LOG_FMT("invalid request"));
        return -1;
    }

    struct httpd_req_aux *ra = r->aux;
    return ra->sd->fd;
}

static int httpd_sock_err(const char *ctx, int sockfd)
{
    int errval;
    ESP_LOGW(TAG, LOG_FMT("error in %s : %d"), ctx, errno);

    switch (errno) {
    case EAGAIN:
    case EINTR:
        errval = HTTPD_SOCK_ERR_TIMEOUT;
        break;
    case EINVAL:
    case EBADF:
    case EFAULT:
    case ENOTSOCK:
        errval = HTTPD_SOCK_ERR_INVALID;
        break;
    default:
        errval = HTTPD_SOCK_ERR_FAIL;
    }
    return errval;
}

int httpd_default_send(httpd_handle_t hd, int sockfd, const char *buf, size_t buf_len, int flags)
{
    (void)hd;
    if (buf == NULL) {
        return HTTPD_SOCK_ERR_INVALID;
    }

    int ret = send(sockfd, buf, buf_len, flags);
    if (ret < 0) {
        return httpd_sock_err("send", sockfd);
    }
    return ret;
}

int httpd_default_recv(httpd_handle_t hd, int sockfd, char *buf, size_t buf_len, int flags)
{
    (void)hd;
    if (buf == NULL) {
        return HTTPD_SOCK_ERR_INVALID;
    }

    int ret = recv(sockfd, buf, buf_len, flags);
    if (ret < 0) {
        return httpd_sock_err("recv", sockfd);
    }
    return ret;
}

int httpd_socket_send(httpd_handle_t hd, int sockfd, const char *buf, size_t buf_len, int flags)
{
    struct sock_db *sess = httpd_sess_get(hd, sockfd);
    if (!sess) {
        return HTTPD_SOCK_ERR_INVALID;
    }
    if (!sess->send_fn) {
        return HTTPD_SOCK_ERR_INVALID;
    }
    return sess->send_fn(hd, sockfd, buf, buf_len, flags);
}

int httpd_socket_recv(httpd_handle_t hd, int sockfd, char *buf, size_t buf_len, int flags)
{
    struct sock_db *sess = httpd_sess_get(hd, sockfd);
    if (!sess) {
        return HTTPD_SOCK_ERR_INVALID;
    }
    if (!sess->recv_fn) {
        return HTTPD_SOCK_ERR_INVALID;
    }
    return sess->recv_fn(hd, sockfd, buf, buf_len, flags);
}
