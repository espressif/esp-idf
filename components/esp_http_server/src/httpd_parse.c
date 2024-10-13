/*
 * SPDX-FileCopyrightText: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <esp_log.h>
#include <esp_err.h>
#include <http_parser.h>
#include <inttypes.h>

#include <esp_http_server.h>
#include "esp_httpd_priv.h"
#include "osal.h"

static const char *TAG = "httpd_parse";

typedef struct {
    /* Parser settings for http_parser_execute() */
    http_parser_settings settings;

    /* Request being parsed */
    struct httpd_req *req;

    /* Status of the parser describes the part of the
     * HTTP request packet being processed at any moment.
     */
    enum {
        PARSING_IDLE = 0,
        PARSING_URL,
        PARSING_HDR_FIELD,
        PARSING_HDR_VALUE,
        PARSING_BODY,
        PARSING_COMPLETE,
        PARSING_FAILED
    } status;

    /* Response error code in case of PARSING_FAILED */
    httpd_err_code_t error;

    /* For storing last callback parameters */
    struct {
        const char *at;
        size_t      length;
    } last;

    /* State variables */
    bool   paused;          /*!< Parser is paused */
    size_t pre_parsed;      /*!< Length of data to be skipped while parsing */
    size_t raw_datalen;     /*!< Full length of the raw data in scratch buffer */
} parser_data_t;

static esp_err_t verify_url (http_parser *parser)
{
    parser_data_t *parser_data  = (parser_data_t *) parser->data;
    struct httpd_req *r         = parser_data->req;
    struct httpd_req_aux *ra    = r->aux;
    struct http_parser_url *res = &ra->url_parse_res;

    /* Get previous values of the parser callback arguments */
    const char *at = parser_data->last.at;
    size_t  length = parser_data->last.length;

    r->method = parser->method;
    if (r->method < 0) {
        ESP_LOGW(TAG, LOG_FMT("HTTP method not supported (%d)"), r->method);
        parser_data->error = HTTPD_501_METHOD_NOT_IMPLEMENTED;
        return ESP_FAIL;
    }

    if (sizeof(r->uri) < (length + 1)) {
        ESP_LOGW(TAG, LOG_FMT("URI length (%"NEWLIB_NANO_COMPAT_FORMAT") greater than supported (%"NEWLIB_NANO_COMPAT_FORMAT")"),
                 NEWLIB_NANO_COMPAT_CAST(length), NEWLIB_NANO_COMPAT_CAST(sizeof(r->uri)));
        parser_data->error = HTTPD_414_URI_TOO_LONG;
        return ESP_FAIL;
    }

    /* Keep URI with terminating null character. Note URI string pointed
     * by 'at' is not NULL terminated, therefore use length provided by
     * parser while copying the URI to buffer */
    strlcpy((char *)r->uri, at, (length + 1));
    ESP_LOGD(TAG, LOG_FMT("received URI = %s"), r->uri);

    /* Make sure version is HTTP/1.1 */
    if ((parser->http_major != 1) && (parser->http_minor != 1)) {
        ESP_LOGW(TAG, LOG_FMT("unsupported HTTP version = %d.%d"),
                 parser->http_major, parser->http_minor);
        parser_data->error = HTTPD_505_VERSION_NOT_SUPPORTED;
        return ESP_FAIL;
    }

    /* Parse URL and keep result for later */
    http_parser_url_init(res);
    if (http_parser_parse_url(r->uri, strlen(r->uri),
                              r->method == HTTP_CONNECT, res)) {
        ESP_LOGW(TAG, LOG_FMT("http_parser_parse_url failed with errno = %d"),
                              parser->http_errno);
        parser_data->error = HTTPD_400_BAD_REQUEST;
        return ESP_FAIL;
    }
    return ESP_OK;
}

/* http_parser callback on finding url in HTTP request
 * Will be invoked ATLEAST once every packet
 */
static esp_err_t cb_url(http_parser *parser,
                        const char *at, size_t length)
{
    parser_data_t *parser_data = (parser_data_t *) parser->data;

    if (parser_data->status == PARSING_IDLE) {
        ESP_LOGD(TAG, LOG_FMT("message begin"));

        /* Store current values of the parser callback arguments */
        parser_data->last.at     = at;
        parser_data->last.length = 0;
        parser_data->status      = PARSING_URL;
    } else if (parser_data->status != PARSING_URL) {
        ESP_LOGE(TAG, LOG_FMT("unexpected state transition"));
        parser_data->error = HTTPD_500_INTERNAL_SERVER_ERROR;
        parser_data->status = PARSING_FAILED;
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, LOG_FMT("processing url = %.*s"), (int)length, at);

    /* Update length of URL string */
    if ((parser_data->last.length += length) > HTTPD_MAX_URI_LEN) {
        ESP_LOGW(TAG, LOG_FMT("URI length (%"NEWLIB_NANO_COMPAT_FORMAT") greater than supported (%d)"),
                 NEWLIB_NANO_COMPAT_CAST(parser_data->last.length), HTTPD_MAX_URI_LEN);
        parser_data->error = HTTPD_414_URI_TOO_LONG;
        parser_data->status = PARSING_FAILED;
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t pause_parsing(http_parser *parser, const char* at)
{
    parser_data_t *parser_data = (parser_data_t *) parser->data;
    struct httpd_req *r        = parser_data->req;
    struct httpd_req_aux *ra   = r->aux;

    /* The length of data that was not parsed due to interruption
     * and hence needs to be read again later for parsing */
    ssize_t unparsed = parser_data->raw_datalen - (at - ra->scratch);
    if (unparsed < 0) {
        ESP_LOGE(TAG, LOG_FMT("parsing beyond valid data = %d"), (int)(-unparsed));
        return ESP_ERR_INVALID_STATE;
    }

    /* Push back the un-parsed data into pending buffer for
     * receiving again with httpd_recv_with_opt() later when
     * read_block() executes */
    if (unparsed && (unparsed != httpd_unrecv(r, at, unparsed))) {
        ESP_LOGE(TAG, LOG_FMT("data too large for un-recv = %d"), (int)unparsed);
        return ESP_FAIL;
    }

    /* Signal http_parser to pause execution and save the maximum
     * possible length, of the yet un-parsed data, that may get
     * parsed before http_parser_execute() returns. This pre_parsed
     * length will be updated then to reflect the actual length
     * that got parsed, and must be skipped when parsing resumes */
    parser_data->pre_parsed = unparsed;
    http_parser_pause(parser, 1);
    parser_data->paused = true;
    ESP_LOGD(TAG, LOG_FMT("paused"));
    return ESP_OK;
}

static size_t continue_parsing(http_parser *parser, size_t length)
{
    parser_data_t *data = (parser_data_t *) parser->data;

    /* Part of the received data may have been parsed earlier
     * so we must skip that before parsing resumes */
    length = MIN(length, data->pre_parsed);
    data->pre_parsed -= length;
    ESP_LOGD(TAG, LOG_FMT("skip pre-parsed data of size = %"NEWLIB_NANO_COMPAT_FORMAT), NEWLIB_NANO_COMPAT_CAST(length));

    http_parser_pause(parser, 0);
    data->paused = false;
    ESP_LOGD(TAG, LOG_FMT("un-paused"));
    return length;
}

/* http_parser callback on header field in HTTP request
 * May be invoked ATLEAST once every header field
 */
static esp_err_t cb_header_field(http_parser *parser, const char *at, size_t length)
{
    parser_data_t *parser_data = (parser_data_t *) parser->data;
    struct httpd_req *r        = parser_data->req;
    struct httpd_req_aux *ra   = r->aux;

    /* Check previous status */
    if (parser_data->status == PARSING_URL) {
        if (verify_url(parser) != ESP_OK) {
            /* verify_url would already have set the
             * error field of parser data, so only setting
             * status to failed */
            parser_data->status = PARSING_FAILED;
            return ESP_FAIL;
        }

        ESP_LOGD(TAG, LOG_FMT("headers begin"));
        /* Last at is set to start of scratch where headers
         * will be received next */
        parser_data->last.at     = ra->scratch;
        parser_data->last.length = 0;
        parser_data->status      = PARSING_HDR_FIELD;

        /* Stop parsing for now and give control to process */
        if (pause_parsing(parser, at) != ESP_OK) {
            parser_data->error = HTTPD_500_INTERNAL_SERVER_ERROR;
            parser_data->status = PARSING_FAILED;
            return ESP_FAIL;
        }
    } else if (parser_data->status == PARSING_HDR_VALUE) {
        /* Overwrite terminator (CRLFs) following last header
         * (key: value) pair with null characters */
        char *term_start = (char *)parser_data->last.at + parser_data->last.length;
        memset(term_start, '\0', at - term_start);

        /* Store current values of the parser callback arguments */
        parser_data->last.at     = at;
        parser_data->last.length = 0;
        parser_data->status      = PARSING_HDR_FIELD;

        /* Increment header count */
        ra->req_hdrs_count++;
    } else if (parser_data->status != PARSING_HDR_FIELD) {
        ESP_LOGE(TAG, LOG_FMT("unexpected state transition"));
        parser_data->error = HTTPD_500_INTERNAL_SERVER_ERROR;
        parser_data->status = PARSING_FAILED;
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, LOG_FMT("processing field = %.*s"), (int)length, at);

    /* Update length of header string */
    parser_data->last.length += length;
    return ESP_OK;
}

/* http_parser callback on header value in HTTP request.
 * May be invoked ATLEAST once every header value
 */
static esp_err_t cb_header_value(http_parser *parser, const char *at, size_t length)
{
    parser_data_t *parser_data = (parser_data_t *) parser->data;

    /* Check previous status */
    if (parser_data->status == PARSING_HDR_FIELD) {
        /* Store current values of the parser callback arguments */
        parser_data->last.at     = at;
        parser_data->last.length = 0;
        parser_data->status      = PARSING_HDR_VALUE;

        if (length == 0) {
            /* As per behavior of http_parser, when length > 0,
             * `at` points to the start of CRLF. But, in the
             * case when header value is empty (zero length),
             * then `at` points to the position right after
             * the CRLF. Since for our purpose we need `last.at`
             * to point to exactly where the CRLF starts, it
             * needs to be adjusted by the right offset */
            char *at_adj = (char *)parser_data->last.at;
            /* Find the end of header field string */
            while (*(--at_adj) != ':');
            /* Now skip leading spaces' */
            while (*(++at_adj) == ' ');
            /* Now we are at the right position */
            parser_data->last.at = at_adj;
        }
    } else if (parser_data->status != PARSING_HDR_VALUE) {
        ESP_LOGE(TAG, LOG_FMT("unexpected state transition"));
        parser_data->error = HTTPD_500_INTERNAL_SERVER_ERROR;
        parser_data->status = PARSING_FAILED;
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, LOG_FMT("processing value = %.*s"), (int)length, at);

    /* Update length of header string */
    parser_data->last.length += length;
    return ESP_OK;
}

/* http_parser callback on completing headers in HTTP request.
 * Will be invoked ONLY once every packet
 */
static esp_err_t cb_headers_complete(http_parser *parser)
{
    parser_data_t *parser_data = (parser_data_t *) parser->data;
    struct httpd_req *r        = parser_data->req;
    struct httpd_req_aux *ra   = r->aux;

    /* Check previous status */
    if (parser_data->status == PARSING_URL) {
        ESP_LOGD(TAG, LOG_FMT("no headers"));
        if (verify_url(parser) != ESP_OK) {
            /* verify_url would already have set the
             * error field of parser data, so only setting
             * status to failed */
            parser_data->status = PARSING_FAILED;
            return ESP_FAIL;
        }
    } else if (parser_data->status == PARSING_HDR_VALUE) {
        /* Locate end of last header */
        char *at = (char *)parser_data->last.at + parser_data->last.length;

        /* Check if there is data left to parse. This value should
         * at least be equal to the number of line terminators, i.e. 2 */
        ssize_t remaining_length = parser_data->raw_datalen - (at - ra->scratch);
        if (remaining_length < 2) {
            ESP_LOGE(TAG, LOG_FMT("invalid length of data remaining to be parsed"));
            parser_data->error = HTTPD_500_INTERNAL_SERVER_ERROR;
            parser_data->status = PARSING_FAILED;
            return ESP_FAIL;
        }

        /* Locate end of headers section by skipping the remaining
         * two line terminators. No assumption is made here about the
         * termination sequence used apart from the necessity that it
         * must end with an LF, because:
         *      1) some clients may send non standard LFs instead of
         *         CRLFs for indicating termination.
         *      2) it is the responsibility of http_parser to check
         *         that the termination is either CRLF or LF and
         *         not any other sequence */
        unsigned short remaining_terminators = 2;
        while (remaining_length-- && remaining_terminators) {
            if (*at == '\n') {
                remaining_terminators--;
            }
            /* Overwrite termination characters with null */
            *(at++) = '\0';
        }
        if (remaining_terminators) {
            ESP_LOGE(TAG, LOG_FMT("incomplete termination of headers"));
            parser_data->error = HTTPD_400_BAD_REQUEST;
            parser_data->status = PARSING_FAILED;
            return ESP_FAIL;
        }

        /* Place the parser ptr right after the end of headers section */
        parser_data->last.at = at;

        /* Increment header count */
        ra->req_hdrs_count++;
    } else {
        ESP_LOGE(TAG, LOG_FMT("unexpected state transition"));
        parser_data->error = HTTPD_500_INTERNAL_SERVER_ERROR;
        parser_data->status = PARSING_FAILED;
        return ESP_FAIL;
    }

    /* In absence of body/chunked encoding, http_parser sets content_len to -1 */
    r->content_len = ((int)parser->content_length != -1 ?
                      parser->content_length : 0);

    ESP_LOGD(TAG, LOG_FMT("bytes read     = %" PRId32 ""),  parser->nread);
    ESP_LOGD(TAG, LOG_FMT("content length = %"NEWLIB_NANO_COMPAT_FORMAT), NEWLIB_NANO_COMPAT_CAST(r->content_len));

    /* Handle upgrade requests - only WebSocket is supported for now */
    if (parser->upgrade) {
#ifdef CONFIG_HTTPD_WS_SUPPORT
        ESP_LOGD(TAG, LOG_FMT("Got an upgrade request"));

        /* If there's no "Upgrade" header field, then it's not WebSocket. */
        char ws_upgrade_hdr_val[] = "websocket";
        if (httpd_req_get_hdr_value_str(r, "Upgrade", ws_upgrade_hdr_val, sizeof(ws_upgrade_hdr_val)) != ESP_OK) {
            ESP_LOGW(TAG, LOG_FMT("Upgrade header does not match the length of \"websocket\""));
            parser_data->error = HTTPD_400_BAD_REQUEST;
            parser_data->status = PARSING_FAILED;
            return ESP_FAIL;
        }

        /* If "Upgrade" field's key is not "websocket", then we should also forget about it. */
        if (strcasecmp("websocket", ws_upgrade_hdr_val) != 0) {
            ESP_LOGW(TAG, LOG_FMT("Upgrade header found but it's %s"), ws_upgrade_hdr_val);
            parser_data->error = HTTPD_400_BAD_REQUEST;
            parser_data->status = PARSING_FAILED;
            return ESP_FAIL;
        }

        /* Now set handshake flag to true */
        ra->ws_handshake_detect = true;
#else
        ESP_LOGD(TAG, LOG_FMT("WS functions has been disabled, Upgrade request is not supported."));
        parser_data->error = HTTPD_400_BAD_REQUEST;
        parser_data->status = PARSING_FAILED;
        return ESP_FAIL;
#endif
    }

    parser_data->status = PARSING_BODY;
    ra->remaining_len = r->content_len;
    esp_http_server_dispatch_event(HTTP_SERVER_EVENT_ON_HEADER, &(ra->sd->fd), sizeof(int));
    return ESP_OK;
}

/* Last http_parser callback if body present in HTTP request.
 * Will be invoked ONLY once every packet
 */
static esp_err_t cb_on_body(http_parser *parser, const char *at, size_t length)
{
    parser_data_t *parser_data = (parser_data_t *) parser->data;

    /* Check previous status */
    if (parser_data->status != PARSING_BODY) {
        ESP_LOGE(TAG, LOG_FMT("unexpected state transition"));
        parser_data->error = HTTPD_500_INTERNAL_SERVER_ERROR;
        parser_data->status = PARSING_FAILED;
        return ESP_FAIL;
    }

    /* Pause parsing so that if part of another packet
     * is in queue then it doesn't get parsed, which
     * may reset the parser state and cause current
     * request packet to be lost */
    if (pause_parsing(parser, at) != ESP_OK) {
        parser_data->error = HTTPD_500_INTERNAL_SERVER_ERROR;
        parser_data->status = PARSING_FAILED;
        return ESP_FAIL;
    }

    parser_data->last.at     = 0;
    parser_data->last.length = 0;
    parser_data->status      = PARSING_COMPLETE;
    ESP_LOGD(TAG, LOG_FMT("body begins"));
    return ESP_OK;
}

/* Last http_parser callback if body absent in HTTP request.
 * Will be invoked ONLY once every packet
 */
static esp_err_t cb_no_body(http_parser *parser)
{
    parser_data_t *parser_data = (parser_data_t *) parser->data;

    /* Check previous status */
    if (parser_data->status == PARSING_URL) {
        ESP_LOGD(TAG, LOG_FMT("no headers"));
        if (verify_url(parser) != ESP_OK) {
            /* verify_url would already have set the
             * error field of parser data, so only setting
             * status to failed */
            parser_data->status = PARSING_FAILED;
            return ESP_FAIL;
        }
    } else if (parser_data->status != PARSING_BODY) {
        ESP_LOGE(TAG, LOG_FMT("unexpected state transition"));
        parser_data->error = HTTPD_500_INTERNAL_SERVER_ERROR;
        parser_data->status = PARSING_FAILED;
        return ESP_FAIL;
    }

    /* Pause parsing so that if part of another packet
     * is in queue then it doesn't get parsed, which
     * may reset the parser state and cause current
     * request packet to be lost */
    if (pause_parsing(parser, parser_data->last.at) != ESP_OK) {
        parser_data->error = HTTPD_500_INTERNAL_SERVER_ERROR;
        parser_data->status = PARSING_FAILED;
        return ESP_FAIL;
    }

    parser_data->last.at     = 0;
    parser_data->last.length = 0;
    parser_data->status      = PARSING_COMPLETE;
    ESP_LOGD(TAG, LOG_FMT("message complete"));
    return ESP_OK;
}

static int read_block(httpd_req_t *req, size_t offset, size_t length)
{
    struct httpd_req_aux *raux  = req->aux;

    /* Limits the read to scratch buffer size */
    ssize_t buf_len = MIN(length, (sizeof(raux->scratch) - offset));
    if (buf_len <= 0) {
        return 0;
    }

    /* Receive data into buffer. If data is pending (from unrecv) then return
     * immediately after receiving pending data, as pending data may just complete
     * this request packet. */
    int nbytes = httpd_recv_with_opt(req, raux->scratch + offset, buf_len, true);
    if (nbytes < 0) {
        ESP_LOGD(TAG, LOG_FMT("error in httpd_recv"));
        /* If timeout occurred allow the
         * situation to be handled */
        if (nbytes == HTTPD_SOCK_ERR_TIMEOUT) {
            /* Invoke error handler which may return ESP_OK
             * to signal for retrying call to recv(), else it may
             * return ESP_FAIL to signal for closure of socket */
            return (httpd_req_handle_err(req, HTTPD_408_REQ_TIMEOUT) == ESP_OK) ?
                    HTTPD_SOCK_ERR_TIMEOUT : HTTPD_SOCK_ERR_FAIL;
        }
        /* Some socket error occurred. Return failure
         * to force closure of underlying socket.
         * Error message is not sent as socket may not
         * be valid anymore */
        return HTTPD_SOCK_ERR_FAIL;
    } else if (nbytes == 0) {
        ESP_LOGD(TAG, LOG_FMT("connection closed"));
        /* Connection closed by client so no
         * need to send error response */
        return HTTPD_SOCK_ERR_FAIL;
    }

    ESP_LOGD(TAG, LOG_FMT("received HTTP request block size = %d"), nbytes);
    return nbytes;
}

static int parse_block(http_parser *parser, size_t offset, size_t length)
{
    parser_data_t        *data  = (parser_data_t *)(parser->data);
    httpd_req_t          *req   = data->req;
    struct httpd_req_aux *raux  = req->aux;
    size_t nparsed = 0;

    if (!length) {
        /* Parsing is still happening but nothing to
         * parse means no more space left on buffer,
         * therefore it can be inferred that the
         * request URI/header must be too long */
        ESP_LOGW(TAG, LOG_FMT("request URI/header too long"));
        switch (data->status) {
            case PARSING_URL:
                data->error = HTTPD_414_URI_TOO_LONG;
                break;
            case PARSING_HDR_FIELD:
            case PARSING_HDR_VALUE:
                data->error = HTTPD_431_REQ_HDR_FIELDS_TOO_LARGE;
                break;
            default:
                ESP_LOGE(TAG, LOG_FMT("unexpected state"));
                data->error = HTTPD_500_INTERNAL_SERVER_ERROR;
                break;
        }
        data->status = PARSING_FAILED;
        return -1;
    }

    /* Un-pause the parsing if paused */
    if (data->paused) {
        nparsed = continue_parsing(parser, length);
        length -= nparsed;
        offset += nparsed;
        if (!length) {
            return nparsed;
        }
    }

    /* Execute http_parser */
    nparsed = http_parser_execute(parser, &data->settings,
                                  raux->scratch + offset, length);

    /* Check state */
    if (data->status == PARSING_FAILED) {
        /* It is expected that the error field of
         * parser data should have been set by now */
        ESP_LOGW(TAG, LOG_FMT("parsing failed"));
        return -1;
    } else if (data->paused) {
        /* Update the value of pre_parsed which was set when
         * pause_parsing() was called. (length - nparsed) is
         * the length of the data that will need to be parsed
         * again later and hence must be deducted from the
         * pre_parsed length */
        data->pre_parsed -= (length - nparsed);
        return 0;
    } else if (nparsed != length) {
        /* http_parser error */
        data->error  = HTTPD_400_BAD_REQUEST;
        data->status = PARSING_FAILED;
        ESP_LOGW(TAG, LOG_FMT("incomplete (%"NEWLIB_NANO_COMPAT_FORMAT"/%"NEWLIB_NANO_COMPAT_FORMAT") with parser error = %d"),
                 NEWLIB_NANO_COMPAT_CAST(nparsed), NEWLIB_NANO_COMPAT_CAST(length), parser->http_errno);
        return -1;
    }

    /* Return with the total length of the request packet
     * that has been parsed till now */
    ESP_LOGD(TAG, LOG_FMT("parsed block size = %"NEWLIB_NANO_COMPAT_FORMAT), NEWLIB_NANO_COMPAT_CAST((offset + nparsed)));
    return offset + nparsed;
}

static void parse_init(httpd_req_t *r, http_parser *parser, parser_data_t *data)
{
    /* Initialize parser data */
    memset(data, 0, sizeof(parser_data_t));
    data->req = r;

    /* Initialize parser */
    http_parser_init(parser, HTTP_REQUEST);
    parser->data = (void *)data;

    /* Initialize parser settings */
    http_parser_settings_init(&data->settings);

    /* Set parser callbacks */
    data->settings.on_url              = cb_url;
    data->settings.on_header_field     = cb_header_field;
    data->settings.on_header_value     = cb_header_value;
    data->settings.on_headers_complete = cb_headers_complete;
    data->settings.on_body             = cb_on_body;
    data->settings.on_message_complete = cb_no_body;
}

/* Function that receives TCP data and runs parser on it
 */
static esp_err_t httpd_parse_req(struct httpd_data *hd)
{
    httpd_req_t *r = &hd->hd_req;
    int blk_len,  offset;
    http_parser   parser = {};
    parser_data_t parser_data = {};

    /* Initialize parser */
    parse_init(r, &parser, &parser_data);

    /* Set offset to start of scratch buffer */
    offset = 0;
    do {
        /* Read block into scratch buffer */
        if ((blk_len = read_block(r, offset, PARSER_BLOCK_SIZE)) < 0) {
            if (blk_len == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry read in case of non-fatal timeout error.
                 * read_block() ensures that the timeout error is
                 * handled properly so that this doesn't get stuck
                 * in an infinite loop */
                continue;
            }
            /* If not HTTPD_SOCK_ERR_TIMEOUT, returned error must
             * be HTTPD_SOCK_ERR_FAIL which means we need to return
             * failure and thereby close the underlying socket */
            return ESP_FAIL;
        }

        /* This is used by the callbacks to track
         * data usage of the buffer */
        parser_data.raw_datalen = blk_len + offset;

        /* Parse data block from buffer */
        if ((offset = parse_block(&parser, offset, blk_len)) < 0) {
            /* HTTP error occurred.
             * Send error code as response status and
             * invoke error handler */
            return httpd_req_handle_err(r, parser_data.error);
        }
    } while (parser_data.status != PARSING_COMPLETE);

    ESP_LOGD(TAG, LOG_FMT("parsing complete"));
    return httpd_uri(hd);
}

static void init_req(httpd_req_t *r, httpd_config_t *config)
{
    r->handle = 0;
    r->method = 0;
    memset((char*)r->uri, 0, sizeof(r->uri));
    r->content_len = 0;
    r->aux = 0;
    r->user_ctx = 0;
    r->sess_ctx = 0;
    r->free_ctx = 0;
    r->ignore_sess_ctx_changes = 0;
}

static void init_req_aux(struct httpd_req_aux *ra, httpd_config_t *config)
{
    ra->sd = 0;
    memset(ra->scratch, 0, sizeof(ra->scratch));
    ra->remaining_len = 0;
    ra->status = 0;
    ra->content_type = 0;
    ra->first_chunk_sent = 0;
    ra->req_hdrs_count = 0;
    ra->resp_hdrs_count = 0;
#if CONFIG_HTTPD_WS_SUPPORT
    ra->ws_handshake_detect = false;
#endif
    memset(ra->resp_hdrs, 0, config->max_resp_headers * sizeof(struct resp_hdr));
}

static void httpd_req_cleanup(httpd_req_t *r)
{
    struct httpd_req_aux *ra = r->aux;

    /* Check if the context has changed and needs to be cleared */
    if ((r->ignore_sess_ctx_changes == false) && (ra->sd->ctx != r->sess_ctx)) {
        httpd_sess_free_ctx(&ra->sd->ctx, ra->sd->free_ctx);
    }

#if CONFIG_HTTPD_WS_SUPPORT
    /* Close the socket when a WebSocket Close request is received */
    if (ra->sd->ws_close) {
        ESP_LOGD(TAG, LOG_FMT("Try closing WS connection at FD: %d"), ra->sd->fd);
        httpd_sess_trigger_close(r->handle, ra->sd->fd);
    }
#endif

    /* Retrieve session info from the request into the socket database. */
    ra->sd->ctx = r->sess_ctx;
    ra->sd->free_ctx = r->free_ctx;
    ra->sd->ignore_sess_ctx_changes = r->ignore_sess_ctx_changes;

    /* Clear out the request and request_aux structures */
    ra->sd = NULL;
    r->handle = NULL;
    r->aux = NULL;
    r->user_ctx = NULL;
}

/* Function that processes incoming TCP data and
 * updates the http request data httpd_req_t
 */
esp_err_t httpd_req_new(struct httpd_data *hd, struct sock_db *sd)
{
    httpd_req_t *r = &hd->hd_req;
    init_req(r, &hd->config);
    init_req_aux(&hd->hd_req_aux, &hd->config);
    r->handle = hd;
    r->aux = &hd->hd_req_aux;

    /* Associate the request to the socket */
    struct httpd_req_aux *ra = r->aux;
    ra->sd = sd;

    /* Set defaults */
    ra->status = (char *)HTTPD_200;
    ra->content_type = (char *)HTTPD_TYPE_TEXT;
    ra->first_chunk_sent = false;

    /* Copy session info to the request */
    r->sess_ctx = sd->ctx;
    r->free_ctx = sd->free_ctx;
    r->ignore_sess_ctx_changes = sd->ignore_sess_ctx_changes;

    esp_err_t ret;

#ifdef CONFIG_HTTPD_WS_SUPPORT
    /* Copy user_ctx to the request */
    r->user_ctx = sd->ws_user_ctx;
    /* Handle WebSocket */
    ESP_LOGD(TAG, LOG_FMT("New request, has WS? %s, sd->ws_handler valid? %s, sd->ws_close? %s"),
             sd->ws_handshake_done ? "Yes" : "No",
             sd->ws_handler != NULL ? "Yes" : "No",
             sd->ws_close ? "Yes" : "No");
    if (sd->ws_handshake_done && sd->ws_handler != NULL) {
        if (sd->ws_close == true) {
            /* WS was marked as close state, do not deal with this socket */
            ESP_LOGD(TAG, LOG_FMT("WS was marked close"));
            return ESP_OK;
        }

        ret = httpd_ws_get_frame_type(r);
        ESP_LOGD(TAG, LOG_FMT("New WS request from existing socket, ws_type=%d"), ra->ws_type);

        if (ra->ws_type == HTTPD_WS_TYPE_CLOSE) {
            /*  Only mark ws_close to true if it's a CLOSE frame */
            sd->ws_close = true;
        } else if (ra->ws_type == HTTPD_WS_TYPE_PONG) {
            /* Pass the PONG frames to the handler as well, as user app might send PINGs */
            ESP_LOGD(TAG, LOG_FMT("Received PONG frame"));
        }

        /* Call handler if it's a non-control frame (or if handler requests control frames, as well) */
        if (ret == ESP_OK &&
            (ra->ws_type < HTTPD_WS_TYPE_CLOSE || sd->ws_control_frames)) {
            ret = sd->ws_handler(r);
        }

        if (ret != ESP_OK) {
            httpd_req_cleanup(r);
        }
        return ret;
    }
#endif

    /* Parse request */
    ret = httpd_parse_req(hd);
    if (ret != ESP_OK) {
        httpd_req_cleanup(r);
    }
    return ret;
}

/* Function that resets the http request data
 */
esp_err_t httpd_req_delete(struct httpd_data *hd)
{
    httpd_req_t *r = &hd->hd_req;
    struct httpd_req_aux *ra = r->aux;

    /* Finish off reading any pending/leftover data */
    while (ra->remaining_len) {
        /* Any length small enough not to overload the stack, but large
         * enough to finish off the buffers fast */
        char dummy[CONFIG_HTTPD_PURGE_BUF_LEN];
        int recv_len = MIN(sizeof(dummy), ra->remaining_len);
        recv_len = httpd_req_recv(r, dummy, recv_len);
        if (recv_len <= 0) {
            httpd_req_cleanup(r);
            return ESP_FAIL;
        }

        ESP_LOGD(TAG, LOG_FMT("purging data size : %d bytes"), recv_len);

#ifdef CONFIG_HTTPD_LOG_PURGE_DATA
        /* Enabling this will log discarded binary HTTP content data at
         * Debug level. For large content data this may not be desirable
         * as it will clutter the log */
        ESP_LOGD(TAG, "================= PURGED DATA =================");
        ESP_LOG_BUFFER_HEX_LEVEL(TAG, dummy, recv_len, ESP_LOG_DEBUG);
        ESP_LOGD(TAG, "===============================================");
#endif
    }

    httpd_req_cleanup(r);
    return ESP_OK;
}

/* Validates the request to prevent users from calling APIs, that are to
 * be called only inside URI handler, outside the handler context
 */
bool httpd_validate_req_ptr(httpd_req_t *r)
{
    if (r) {
        struct httpd_data *hd = (struct httpd_data *) r->handle;
        if (hd) {
            /* Check if this function is running in the context of
             * the correct httpd server thread */
            if (httpd_os_thread_handle() == hd->hd_td.handle) {
                return true;
            }
        }
    }
    return false;
}

/* Helper function to get a URL query tag from a query string of the type param1=val1&param2=val2 */
esp_err_t httpd_query_key_value(const char *qry_str, const char *key, char *val, size_t val_size)
{
    if (qry_str == NULL || key == NULL || val == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    const char   *qry_ptr = qry_str;
    const size_t  buf_len = val_size;

    while (strlen(qry_ptr)) {
        /* Search for the '=' character. Else, it would mean
         * that the parameter is invalid */
        const char *val_ptr = strchr(qry_ptr, '=');
        if (!val_ptr) {
            break;
        }
        size_t offset = val_ptr - qry_ptr;

        /* If the key, does not match, continue searching.
         * Compare lengths first as key from url is not
         * null terminated (has '=' in the end) */
        if ((offset != strlen(key)) ||
            (strncasecmp(qry_ptr, key, offset))) {
            /* Get the name=val string. Multiple name=value pairs
             * are separated by '&' */
            qry_ptr = strchr(val_ptr, '&');
            if (!qry_ptr) {
                break;
            }
            qry_ptr++;
            continue;
        }

        /* Locate start of next query */
        qry_ptr = strchr(++val_ptr, '&');
        /* Or this could be the last query, in which
         * case get to the end of query string */
        if (!qry_ptr) {
            qry_ptr = val_ptr + strlen(val_ptr);
        }

        /* Update value length, including one byte for null */
        val_size = qry_ptr - val_ptr + 1;

        /* Copy value to the caller's buffer. */
        strlcpy(val, val_ptr, MIN(val_size, buf_len));

        /* If buffer length is smaller than needed, return truncation error */
        if (buf_len < val_size) {
            return ESP_ERR_HTTPD_RESULT_TRUNC;
        }
        return ESP_OK;
    }
    ESP_LOGD(TAG, LOG_FMT("key %s not found"), key);
    return ESP_ERR_NOT_FOUND;
}

size_t httpd_req_get_url_query_len(httpd_req_t *r)
{
    if (r == NULL) {
        return 0;
    }

    if (!httpd_valid_req(r)) {
        return 0;
    }

    struct httpd_req_aux   *ra  = r->aux;
    struct http_parser_url *res = &ra->url_parse_res;

    /* Check if query field is present in the URL */
    if (res->field_set & (1 << UF_QUERY)) {
        return res->field_data[UF_QUERY].len;
    }
    return 0;
}

esp_err_t httpd_req_get_url_query_str(httpd_req_t *r, char *buf, size_t buf_len)
{
    if (r == NULL || buf == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!httpd_valid_req(r)) {
        return ESP_ERR_HTTPD_INVALID_REQ;
    }

    struct httpd_req_aux   *ra  = r->aux;
    struct http_parser_url *res = &ra->url_parse_res;

    /* Check if query field is present in the URL */
    if (res->field_set & (1 << UF_QUERY)) {
        const char *qry = r->uri + res->field_data[UF_QUERY].off;

        /* Minimum required buffer len for keeping
         * null terminated query string */
        size_t min_buf_len = res->field_data[UF_QUERY].len + 1;

        strlcpy(buf, qry, MIN(buf_len, min_buf_len));
        if (buf_len < min_buf_len) {
            return ESP_ERR_HTTPD_RESULT_TRUNC;
        }
        return ESP_OK;
    }
    return ESP_ERR_NOT_FOUND;
}

/* Get the length of the value string of a header request field */
size_t httpd_req_get_hdr_value_len(httpd_req_t *r, const char *field)
{
    if (r == NULL || field == NULL) {
        return 0;
    }

    if (!httpd_valid_req(r)) {
        return 0;
    }

    struct httpd_req_aux *ra = r->aux;
    const char   *hdr_ptr = ra->scratch;         /*!< Request headers are kept in scratch buffer */
    unsigned      count   = ra->req_hdrs_count;  /*!< Count set during parsing  */

    while (count--) {
        /* Search for the ':' character. Else, it would mean
         * that the field is invalid
         */
        const char *val_ptr = strchr(hdr_ptr, ':');
        if (!val_ptr) {
            break;
        }

        /* If the field, does not match, continue searching.
         * Compare lengths first as field from header is not
         * null terminated (has ':' in the end).
         */
        if ((val_ptr - hdr_ptr != strlen(field)) ||
            (strncasecmp(hdr_ptr, field, strlen(field)))) {
            if (count) {
                /* Jump to end of header field-value string */
                hdr_ptr = 1 + strchr(hdr_ptr, '\0');

                /* Skip all null characters (with which the line
                 * terminators had been overwritten) */
                while (*hdr_ptr == '\0') {
                    hdr_ptr++;
                }
            }
            continue;
        }

        /* Skip ':' */
        val_ptr++;

        /* Skip preceding space */
        while ((*val_ptr != '\0') && (*val_ptr == ' ')) {
            val_ptr++;
        }
        return strlen(val_ptr);
    }
    return 0;
}

/* Get the value of a field from the request headers */
esp_err_t httpd_req_get_hdr_value_str(httpd_req_t *r, const char *field, char *val, size_t val_size)
{
    if (r == NULL || field == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!httpd_valid_req(r)) {
        return ESP_ERR_HTTPD_INVALID_REQ;
    }

    struct httpd_req_aux *ra = r->aux;
    const char   *hdr_ptr = ra->scratch;         /*!< Request headers are kept in scratch buffer */
    unsigned     count    = ra->req_hdrs_count;  /*!< Count set during parsing  */
    const size_t buf_len  = val_size;

    while (count--) {
        /* Search for the ':' character. Else, it would mean
         * that the field is invalid
         */
        const char *val_ptr = strchr(hdr_ptr, ':');
        if (!val_ptr) {
            break;
        }

        /* If the field, does not match, continue searching.
         * Compare lengths first as field from header is not
         * null terminated (has ':' in the end).
         */
        if ((val_ptr - hdr_ptr != strlen(field)) ||
            (strncasecmp(hdr_ptr, field, strlen(field)))) {
            if (count) {
                /* Jump to end of header field-value string */
                hdr_ptr = 1 + strchr(hdr_ptr, '\0');

                /* Skip all null characters (with which the line
                 * terminators had been overwritten) */
                while (*hdr_ptr == '\0') {
                    hdr_ptr++;
                }
            }
            continue;
        }

        /* Skip ':' */
        val_ptr++;

        /* Skip preceding space */
        while ((*val_ptr != '\0') && (*val_ptr == ' ')) {
            val_ptr++;
        }

        /* Get the NULL terminated value and copy it to the caller's buffer. */
        strlcpy(val, val_ptr, buf_len);

        /* Update value length, including one byte for null */
        val_size = strlen(val_ptr) + 1;

        /* If buffer length is smaller than needed, return truncation error */
        if (buf_len < val_size) {
            return ESP_ERR_HTTPD_RESULT_TRUNC;
        }
        return ESP_OK;
    }
    return ESP_ERR_NOT_FOUND;
}

/* Helper function to get a cookie value from a cookie string of the type "cookie1=val1; cookie2=val2" */
esp_err_t static httpd_cookie_key_value(const char *cookie_str, const char *key, char *val, size_t *val_size)
{
    if (cookie_str == NULL || key == NULL || val == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    const char *cookie_ptr = cookie_str;
    const size_t buf_len = *val_size;
    size_t _val_size = *val_size;

    while (strlen(cookie_ptr)) {
        /* Search for the '=' character. Else, it would mean
         * that the parameter is invalid */
        const char *val_ptr = strchr(cookie_ptr, '=');
        if (!val_ptr) {
            break;
        }
        size_t offset = val_ptr - cookie_ptr;

        /* If the key, does not match, continue searching.
         * Compare lengths first as key from cookie string is not
         * null terminated (has '=' in the end) */
        if ((offset != strlen(key)) || (strncasecmp(cookie_ptr, key, offset) != 0)) {
            /* Get the name=val string. Multiple name=value pairs
             * are separated by '; ' */
            cookie_ptr = strchr(val_ptr, ' ');
            if (!cookie_ptr) {
                break;
            }
            cookie_ptr++;
            continue;
        }

        /* Locate start of next query */
        cookie_ptr = strchr(++val_ptr, ';');
        /* Or this could be the last query, in which
         * case get to the end of query string */
        if (!cookie_ptr) {
            cookie_ptr = val_ptr + strlen(val_ptr);
        }

        /* Update value length, including one byte for null */
        _val_size = cookie_ptr - val_ptr + 1;

        /* Copy value to the caller's buffer. */
        strlcpy(val, val_ptr, MIN(_val_size, buf_len));

        /* If buffer length is smaller than needed, return truncation error */
        if (buf_len < _val_size) {
            *val_size = _val_size;
            return ESP_ERR_HTTPD_RESULT_TRUNC;
        }
        /* Save amount of bytes copied to caller's buffer */
        *val_size = MIN(_val_size, buf_len);
        return ESP_OK;
    }
    ESP_LOGD(TAG, LOG_FMT("cookie %s not found"), key);
    return ESP_ERR_NOT_FOUND;
}

/* Get the value of a cookie from the request headers */
esp_err_t httpd_req_get_cookie_val(httpd_req_t *req, const char *cookie_name, char *val, size_t *val_size)
{
    esp_err_t ret;
    size_t hdr_len_cookie = httpd_req_get_hdr_value_len(req, "Cookie");
    char *cookie_str = NULL;

    if (hdr_len_cookie <= 0) {
        return ESP_ERR_NOT_FOUND;
    }
    cookie_str = malloc(hdr_len_cookie + 1);
    if (cookie_str == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for cookie string");
        return ESP_ERR_NO_MEM;
    }

    if (httpd_req_get_hdr_value_str(req, "Cookie", cookie_str, hdr_len_cookie + 1) != ESP_OK) {
        ESP_LOGW(TAG, "Cookie not found in header uri:[%s]", req->uri);
        free(cookie_str);
        return ESP_ERR_NOT_FOUND;
    }

    ret = httpd_cookie_key_value(cookie_str, cookie_name, val, val_size);
    free(cookie_str);
    return ret;

}
