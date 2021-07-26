// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "esp_netif.h"
#include "lwip/sockets.h"
#include "esp32/rom/md5_hash.h"
#include "mbedtls/base64.h"

#include "esp_system.h"
#include "esp_log.h"

#include "http_utils.h"
#include "http_auth.h"

#define MD5_MAX_LEN (33)
#define HTTP_AUTH_BUF_LEN (1024)

static const char *TAG = "HTTP_AUTH";

/**
 * @brief      This function hash a formatted string with MD5 and format the result as ascii characters
 *
 * @param      md         The buffer will hold the ascii result
 * @param[in]  fmt        The format
 *
 * @return     Length of the result
 */
static int md5_printf(char *md, const char *fmt, ...)
{
    unsigned char *buf;
    unsigned char digest[MD5_MAX_LEN];
    int len, i;
    struct MD5Context md5_ctx;
    va_list ap;
    va_start(ap, fmt);
    len = vasprintf((char **)&buf, fmt, ap);
    if (buf == NULL) {
        va_end(ap);
        return ESP_FAIL;
    }

    MD5Init(&md5_ctx);
    MD5Update(&md5_ctx, buf, len);
    MD5Final(digest, &md5_ctx);

    for (i = 0; i < 16; ++i) {
        sprintf(&md[i * 2], "%02x", (unsigned int)digest[i]);
    }
    va_end(ap);

    free(buf);
    return MD5_MAX_LEN;
}

char *http_auth_digest(const char *username, const char *password, esp_http_auth_data_t *auth_data)
{
    char *ha1, *ha2 = NULL;
    char *digest = NULL;
    char *auth_str = NULL;
    char *temp_auth_str = NULL;

    if (username == NULL ||
        password == NULL ||
        auth_data->nonce == NULL ||
        auth_data->uri == NULL ||
        auth_data->realm == NULL) {
        return NULL;
    }

    ha1 = calloc(1, MD5_MAX_LEN);
    HTTP_MEM_CHECK(TAG, ha1, goto _digest_exit);

    ha2 = calloc(1, MD5_MAX_LEN);
    HTTP_MEM_CHECK(TAG, ha2, goto _digest_exit);

    digest = calloc(1, MD5_MAX_LEN);
    HTTP_MEM_CHECK(TAG, digest, goto _digest_exit);

    if (md5_printf(ha1, "%s:%s:%s", username, auth_data->realm, password) <= 0) {
        goto _digest_exit;
    }

    ESP_LOGD(TAG, "%s %s %s %s\r\n", "Digest", username, auth_data->realm, password);
    if (strcasecmp(auth_data->algorithm, "md5-sess") == 0) {
        if (md5_printf(ha1, "%s:%s:%016llx", ha1, auth_data->nonce, auth_data->cnonce) <= 0) {
            goto _digest_exit;
        }
    }
    if (md5_printf(ha2, "%s:%s", auth_data->method, auth_data->uri) <= 0) {
        goto _digest_exit;
    }

    //support qop = auth
    if (auth_data->qop && strcasecmp(auth_data->qop, "auth-int") == 0) {
        if (md5_printf(ha2, "%s:%s", ha2, "entity") <= 0) {
            goto _digest_exit;
        }
    }

    if (auth_data->qop) {
        // response=MD5(HA1:nonce:nonceCount:cnonce:qop:HA2)
        if (md5_printf(digest, "%s:%s:%08x:%016llx:%s:%s", ha1, auth_data->nonce, auth_data->nc, auth_data->cnonce, auth_data->qop, ha2) <= 0) {
            goto _digest_exit;
        }
    } else {
        // response=MD5(HA1:nonce:HA2)
        if (md5_printf(digest, "%s:%s:%s", ha1, auth_data->nonce, ha2) <= 0) {
            goto _digest_exit;
        }
    }
    asprintf(&auth_str, "Digest username=\"%s\", realm=\"%s\", nonce=\"%s\", uri=\"%s\", algorithm=\"MD5\", "
             "response=\"%s\", qop=%s, nc=%08x, cnonce=\"%016llx\"",
             username, auth_data->realm, auth_data->nonce, auth_data->uri, digest, auth_data->qop, auth_data->nc, auth_data->cnonce);
    if (auth_data->opaque) {
        asprintf(&temp_auth_str, "%s, opaque=\"%s\"", auth_str, auth_data->opaque);
        free(auth_str);
        auth_str = temp_auth_str;
    }
_digest_exit:
    free(ha1);
    free(ha2);
    free(digest);
    return auth_str;
}

char *http_auth_basic(const char *username, const char *password)
{
    int out;
    char *user_info = NULL;
    char *digest = NULL;
    size_t n = 0;
    asprintf(&user_info, "%s:%s", username, password);
    HTTP_MEM_CHECK(TAG, user_info, return NULL);
    mbedtls_base64_encode(NULL, 0, &n, (const unsigned char *)user_info, strlen(user_info));
    digest = calloc(1, 6 + n + 1);
    HTTP_MEM_CHECK(TAG, digest, goto _basic_exit);
    strcpy(digest, "Basic ");
    mbedtls_base64_encode((unsigned char *)digest + 6, n, (size_t *)&out, (const unsigned char *)user_info, strlen(user_info));
_basic_exit:
    free(user_info);
    return digest;
}
