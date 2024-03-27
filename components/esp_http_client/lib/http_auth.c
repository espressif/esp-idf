/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "sys/socket.h"
#include "esp_rom_md5.h"
#include "esp_tls_crypto.h"
#include "mbedtls/sha256.h"

#include "esp_log.h"
#include "esp_check.h"

#include "http_utils.h"
#include "http_auth.h"

#define MD5_MAX_LEN (33)
#define SHA256_LEN (32)
#define SHA256_HEX_LEN (65)
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
    md5_context_t md5_ctx;
    va_list ap;
    va_start(ap, fmt);
    len = vasprintf((char **)&buf, fmt, ap);
    if (buf == NULL) {
        va_end(ap);
        return ESP_FAIL;
    }

    esp_rom_md5_init(&md5_ctx);
    esp_rom_md5_update(&md5_ctx, buf, len);
    esp_rom_md5_final(digest, &md5_ctx);

    for (i = 0; i < 16; ++i) {
        sprintf(&md[i * 2], "%02x", (unsigned int)digest[i]);
    }
    va_end(ap);

    free(buf);
    return MD5_MAX_LEN;
}

/**
 * @brief      This function hash a formatted string with SHA256 and format the result as ascii characters
 *
 * @param      sha          The buffer will hold the ascii result
 * @param[in]  fmt          The format
 *
 * @return     Length of the result
 */
static int sha256_sprintf(char *sha, const char *fmt, ...)
{

    unsigned char *buf;
    unsigned char digest[SHA256_LEN];
    int len, i;
    va_list ap;
    va_start(ap, fmt);
    len = vasprintf((char **)&buf, fmt, ap);
    if (buf == NULL) {
        va_end(ap);
        return ESP_FAIL;
    }

    int ret = 0;
    mbedtls_sha256_context sha256;
    mbedtls_sha256_init(&sha256);
    if (mbedtls_sha256_starts(&sha256, 0) != 0) {
        goto exit;
    }
    if (mbedtls_sha256_update(&sha256, buf, len) != 0) {
        goto exit;
    }
    if (mbedtls_sha256_finish(&sha256, digest) != 0) {
        goto exit;
    }

    for (i = 0; i < 32; ++i) {
        sprintf(&sha[i * 2], "%02x", (unsigned int)digest[i]);
    }
    sha[SHA256_HEX_LEN - 1] = '\0';
    ret = SHA256_HEX_LEN;

exit:
    free(buf);
    mbedtls_sha256_free(&sha256);
    va_end(ap);
    return ret;
}

char *http_auth_digest(const char *username, const char *password, esp_http_auth_data_t *auth_data)
{
    char *ha1, *ha2 = NULL;
    char *digest = NULL;
    char *auth_str = NULL;
    char *temp_auth_str = NULL;
    esp_err_t ret = ESP_OK;

    if (username == NULL ||
            password == NULL ||
            auth_data->nonce == NULL ||
            auth_data->uri == NULL ||
            auth_data->realm == NULL) {
        return NULL;
    }

    int digest_size = MD5_MAX_LEN;
    int (*digest_func)(char *digest, const char *fmt, ...) = md5_printf;
    if (!memcmp(auth_data->algorithm, "SHA256", strlen("SHA256")) ||
            !memcmp(auth_data->algorithm, "SHA-256", strlen("SHA-256"))) {
        digest_size = SHA256_HEX_LEN;
        digest_func = sha256_sprintf;
    }

    ha1 = calloc(1, digest_size);
    ESP_GOTO_ON_FALSE(ha1, ESP_FAIL, _digest_exit, TAG, "Memory exhausted");

    ha2 = calloc(1, digest_size);
    ESP_GOTO_ON_FALSE(ha2, ESP_FAIL, _digest_exit, TAG, "Memory exhausted");

    digest = calloc(1, digest_size);
    ESP_GOTO_ON_FALSE(digest, ESP_FAIL, _digest_exit, TAG, "Memory exhausted");

    if (digest_func(ha1, "%s:%s:%s", username, auth_data->realm, password) <= 0) {
        goto _digest_exit;
    }

    ESP_LOGD(TAG, "%s %s %s %s", "Digest", username, auth_data->realm, password);
    if ((strcasecmp(auth_data->algorithm, "md5-sess") == 0) ||
            (strcasecmp(auth_data->algorithm, "SHA256") == 0) ||
            (strcasecmp(auth_data->algorithm, "md5-sess") == 0)) {
        if (digest_func(ha1, "%s:%s:%016llx", ha1, auth_data->nonce, auth_data->cnonce) <= 0) {
            goto _digest_exit;
        }
    }
    if (digest_func(ha2, "%s:%s", auth_data->method, auth_data->uri) <= 0) {
        goto _digest_exit;
    }

    //support qop = auth
    if (auth_data->qop && strcasecmp(auth_data->qop, "auth-int") == 0) {
        if (digest_func(ha2, "%s:%s", ha2, "entity") <= 0) {
            goto _digest_exit;
        }
    }

    if (auth_data->qop) {
        // response=digest_func(HA1:nonce:nonceCount:cnonce:qop:HA2)
        if (digest_func(digest, "%s:%s:%08x:%016llx:%s:%s", ha1, auth_data->nonce, auth_data->nc, auth_data->cnonce, auth_data->qop, ha2) <= 0) {
            goto _digest_exit;
        }
    } else {
        /* Although as per RFC-2617, "qop" directive is optional in order to maintain backward compatibality, it is recommended
           to use it if the server indicated that qop is supported. This enhancement was introduced to protect against attacks
           like chosen-plaintext attack. */
        ESP_LOGW(TAG, "\"qop\" directive not found. This may lead to attacks like chosen-plaintext attack");
        // response=digest_func(HA1:nonce:HA2)
        if (digest_func(digest, "%s:%s:%s", ha1, auth_data->nonce, ha2) <= 0) {
            goto _digest_exit;
        }
    }
    int rc = asprintf(&auth_str, "Digest username=\"%s\", realm=\"%s\", nonce=\"%s\", uri=\"%s\", algorithm=%s, "
                      "response=\"%s\"", username, auth_data->realm, auth_data->nonce, auth_data->uri, auth_data->algorithm, digest);
    if (rc < 0) {
        ESP_LOGE(TAG, "asprintf() returned: %d", rc);
        ret = ESP_FAIL;
        goto _digest_exit;
    }

    if (auth_data->qop) {
        rc = asprintf(&temp_auth_str, ", qop=%s, nc=%08x, cnonce=\"%016"PRIx64"\"", auth_data->qop, auth_data->nc, auth_data->cnonce);
        if (rc < 0) {
            ESP_LOGE(TAG, "asprintf() returned: %d", rc);
            ret = ESP_FAIL;
            goto _digest_exit;
        }
        auth_str = http_utils_append_string(&auth_str, temp_auth_str, strlen(temp_auth_str));
        if (!auth_str) {
            ret = ESP_FAIL;
            goto _digest_exit;
        }
        free(temp_auth_str);
        auth_data->nc ++;
    }
    if (auth_data->opaque) {
        rc = asprintf(&temp_auth_str, "%s, opaque=\"%s\"", auth_str, auth_data->opaque);
        // Free the previous memory allocated for `auth_str`
        free(auth_str);
        if (rc < 0) {
            ESP_LOGE(TAG, "asprintf() returned: %d", rc);
            ret = ESP_FAIL;
            goto _digest_exit;
        }
        auth_str = temp_auth_str;
    }
_digest_exit:
    free(ha1);
    free(ha2);
    free(digest);
    return (ret == ESP_OK) ? auth_str : NULL;
}

char *http_auth_basic(const char *username, const char *password)
{
    size_t out;
    char *user_info = NULL;
    char *digest = NULL;
    esp_err_t ret = ESP_OK;
    size_t n = 0;
    if (asprintf(&user_info, "%s:%s", username, password) < 0) {
        return NULL;
    }
    ESP_RETURN_ON_FALSE(user_info, NULL, TAG, "Memory exhausted");
    esp_crypto_base64_encode(NULL, 0, &n, (const unsigned char *)user_info, strlen(user_info));
    digest = calloc(1, 6 + n + 1);
    ESP_GOTO_ON_FALSE(digest, ESP_FAIL, _basic_exit, TAG, "Memory exhausted");
    strcpy(digest, "Basic ");
    esp_crypto_base64_encode((unsigned char *)digest + 6, n, &out, (const unsigned char *)user_info, strlen(user_info));
_basic_exit:
    free(user_info);
    return (ret == ESP_OK) ? digest : NULL;
}
