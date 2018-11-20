// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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
#include <sys/param.h>
#include <esp_log.h>
#include <esp_err.h>
#include <http_parser.h>

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
    httpd_err_resp_t error;

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

    if ((r->method = parser->method) < 0) {
        ESP_LOGW(TAG, LOG_FMT("HTTP Operation not supported"));
        parser_data->error = HTTPD_501_METHOD_NOT_IMPLEMENTED;
        return ESP_FAIL;
    }

    if (sizeof(r->uri) < (length + 1)) {
        ESP_LOGW(TAG, LOG_FMT("URI length (%d) greater than supported (%d)"),
                 length, sizeof(r->uri));
        parser_data->error = HTTPD_414_URI_TOO_LONG;
        parser_data->status = PARSING_FAILED;
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
        parser_data->status = PARSING_FAILED;
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, LOG_FMT("processing url = %.*s"), length, at);

    /* Update length of URL string */
    if ((parser_data->last.length += length) > HTTPD_MAX_URI_LEN) {
        ESP_LOGW(TAG, LOG_FMT("URI length (%d) greater than supported (%d)"),
                 parser_data->last.length, HTTPD_MAX_URI_LEN);
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

    parser_data->pre_parsed = parser_data->raw_datalen
                              - (at - ra->scratch);

    if (parser_data->pre_parsed != httpd_unrecv(r, at, parser_data->pre_parsed)) {
        ESP_LOGE(TAG, LOG_FMT("data too large for un-recv = %d"),
                 parser_data->pre_parsed);
        return ESP_FAIL;
    }

    http_parser_pause(parser, 1);
    parser_data->paused = true;
    ESP_LOGD(TAG, LOG_FMT("paused"));
    return ESP_OK;
}

static size_t continue_parsing(http_parser *parser, size_t length)
{
    parser_data_t *data = (parser_data_t *) parser->data;

    /* Part of the blk may have been parsed before
     * so we must skip that */
    length = MIN(length, data->pre_parsed);
    data->pre_parsed -= length;
    ESP_LOGD(TAG, LOG_FMT("skip pre-parsed data of size = %d"), length);

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
            parser_data->status = PARSING_FAILED;
            return ESP_FAIL;
        }
    } else if (parser_data->status == PARSING_HDR_VALUE) {
        /* NULL terminate last header (key: value) pair */
        size_t offset = parser_data->last.at - ra->scratch;
        ra->scratch[offset + parser_data->last.length] = '\0';

        /* Store current values of the parser callback arguments */
        parser_data->last.at     = at;
        parser_data->last.length = 0;
        parser_data->status      = PARSING_HDR_FIELD;
    } else if (parser_data->status != PARSING_HDR_FIELD) {
        ESP_LOGE(TAG, LOG_FMT("unexpected state transition"));
        parser_data->status = PARSING_FAILED;
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, LOG_FMT("processing field = %.*s"), length, at);

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
    struct httpd_req *r        = parser_data->req;
    struct httpd_req_aux *ra   = r->aux;

    /* Check previous status */
    if (parser_data->status == PARSING_HDR_FIELD) {
        /* Store current values of the parser callback arguments */
        parser_data->last.at     = at;
        parser_data->last.length = 0;
        parser_data->status      = PARSING_HDR_VALUE;
        /* Increment header count */
        ra->req_hdrs_count++;
    } else if (parser_data->status != PARSING_HDR_VALUE) {
        ESP_LOGE(TAG, LOG_FMT("unexpected state transition"));
        parser_data->status = PARSING_FAILED;
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, LOG_FMT("processing value = %.*s"), length, at);

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
            parser_data->status = PARSING_FAILED;
            return ESP_FAIL;
        }
    } else if (parser_data->status == PARSING_HDR_VALUE) {
        /* NULL terminate last header (key: value) pair */
        size_t offset = parser_data->last.at - ra->scratch;
        ra->scratch[offset + parser_data->last.length] = '\0';

        /* Reach end of last header */
        parser_data->last.at += parser_data->last.length;
    } else {
        ESP_LOGE(TAG, LOG_FMT("unexpected state transition"));
        parser_data->status = PARSING_FAILED;
        return ESP_FAIL;
    }

    /* In absence of body/chunked encoding, http_parser sets content_len to -1 */
    r->content_len = ((int)parser->content_length != -1 ?
                      parser->content_length : 0);

    ESP_LOGD(TAG, LOG_FMT("bytes read     = %d"),  parser->nread);
    ESP_LOGD(TAG, LOG_FMT("content length = %zu"), r->content_len);

    if (parser->upgrade) {
        ESP_LOGW(TAG, LOG_FMT("upgrade from HTTP not supported"));
        parser_data->error = HTTPD_XXX_UPGRADE_NOT_SUPPORTED;
        parser_data->status = PARSING_FAILED;
        return ESP_FAIL;
    }

    parser_data->status = PARSING_BODY;
    ra->remaining_len = r->content_len;
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
        parser_data->status = PARSING_FAILED;
        return ESP_FAIL;
    }

    /* Pause parsing so that if part of another packet
     * is in queue then it doesn't get parsed, which
     * may reset the parser state and cause current
     * request packet to be lost */
    if (pause_parsing(parser, at) != ESP_OK) {
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
    const char* at             = parser_data->last.at;

    /* Check previous status */
    if (parser_data->status == PARSING_URL) {
        ESP_LOGD(TAG, LOG_FMT("no headers"));
        if (verify_url(parser) != ESP_OK) {
            parser_data->status = PARSING_FAILED;
            return ESP_FAIL;
        }
    } else if (parser_data->status != PARSING_BODY) {
        ESP_LOGE(TAG, LOG_FMT("unexpected state transition"));
        parser_data->status = PARSING_FAILED;
        return ESP_FAIL;
    }

    /* Get end of packet */
    at += strlen("\r\n\r\n");

    /* Pause parsing so that if part of another packet
     * is in queue then it doesn't get parsed, which
     * may reset the parser state and cause current
     * request packet to be lost */
    if (pause_parsing(parser, at) != ESP_OK) {
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
    size_t buf_len = MIN(length, (sizeof(raux->scratch) - offset));
    if (buf_len == 0) {
        return 0;
    }

    /* Receive data into buffer. If data is pending (from unrecv) then return
     * immediately after receiving pending data, as pending data may just complete
     * this request packet. */
    int nbytes = httpd_recv_with_opt(req, raux->scratch + offset, buf_len, true);
    if (nbytes < 0) {
        ESP_LOGD(TAG, LOG_FMT("error in httpd_recv"));
        if (nbytes == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_err(req, HTTPD_408_REQ_TIMEOUT);
        }
        return -1;
    } else if (nbytes == 0) {
        ESP_LOGD(TAG, LOG_FMT("connection closed"));
        return -1;
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
        ESP_LOGW(TAG, LOG_FMT("response uri/header too big"));
        switch (data->status) {
            case PARSING_URL:
                data->error = HTTPD_414_URI_TOO_LONG;
                break;
            case PARSING_HDR_FIELD:
            case PARSING_HDR_VALUE:
                data->error = HTTPD_431_REQ_HDR_FIELDS_TOO_LARGE;
            default:
                break;
        }
        data->status = PARSING_FAILED;
        return -1;
    }

    /* Unpause the parsing if paused */
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
        ESP_LOGW(TAG, LOG_FMT("parsing failed"));
        return -1;
    } else if (data->paused) {
        /* Keep track of parsed data to be skipped
         * during next parsing cycle */
        data->pre_parsed -= (length - nparsed);
        return 0;
    } else if (nparsed != length) {
        /* http_parser error */
        data->status = PARSING_FAILED;
        data->error  = HTTPD_400_BAD_REQUEST;
        ESP_LOGW(TAG, LOG_FMT("incomplete (%d/%d) with parser error = %d"),
                 nparsed, length, parser->http_errno);
        return -1;
    }

    /* Continue parsing this section of HTTP request packet */
    ESP_LOGD(TAG, LOG_FMT("parsed block size = %d"), offset + nparsed);
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
    http_parser   parser;
    parser_data_t parser_data;

    /* Initialize parser */
    parse_init(r, &parser, &parser_data);

    /* Set offset to start of scratch buffer */
    offset = 0;
    do {
        /* Read block into scratch buffer */
        if ((blk_len = read_block(r, offset, PARSER_BLOCK_SIZE)) < 0) {
            /* Return error to close socket */
            return ESP_FAIL;
        }

        /* This is used by the callbacks to track
         * data usage of the buffer */
        parser_data.raw_datalen = blk_len + offset;

        /* Parse data block from buffer */
        if ((offset = parse_block(&parser, offset, blk_len)) < 0) {
            /* Server/Client error. Send error code as response status */
            return httpd_resp_send_err(r, parser_data.error);
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
    memset(ra->resp_hdrs, 0, config->max_resp_headers * sizeof(struct resp_hdr));
}

static void httpd_req_cleanup(httpd_req_t *r)
{
    struct httpd_req_aux *ra = r->aux;

    /* Retrieve session info from the request into the socket database */
    if (ra->sd->ctx != r->sess_ctx) {
        /* Free previous context */
        httpd_sess_free_ctx(ra->sd->ctx, ra->sd->free_ctx);
        ra->sd->ctx = r->sess_ctx;
    }
    ra->sd->free_ctx = r->free_ctx;

    /* Clear out the request and request_aux structures */
    ra->sd = NULL;
    r->handle = NULL;
    r->aux = NULL;
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
    /* Parse request */
    esp_err_t err = httpd_parse_req(hd);
    if (err != ESP_OK) {
        httpd_req_cleanup(r);
    }
    return err;
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
         * enough to finish off the buffers fast
         */
        char dummy[32];
        int recv_len = MIN(sizeof(dummy) - 1, ra->remaining_len);
        int ret = httpd_req_recv(r, dummy, recv_len);
        if (ret <  0) {
            httpd_req_cleanup(r);
            return ESP_FAIL;
        }

        dummy[ret] = '\0';
        ESP_LOGD(TAG, LOG_FMT("purging data : %s"), dummy);
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
            hdr_ptr += strlen(hdr_ptr) + strlen("\r\n");
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
            hdr_ptr += strlen(hdr_ptr) + strlen("\r\n");
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
