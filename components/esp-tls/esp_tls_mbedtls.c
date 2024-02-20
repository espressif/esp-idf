/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <http_parser.h>
#include "esp_tls_mbedtls.h"
#include "esp_tls_private.h"
#include "esp_tls_error_capture_internal.h"
#include <errno.h>
#include "esp_log.h"
#include "esp_check.h"

#ifdef CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN
#include "ecdsa/ecdsa_alt.h"
#endif

#ifdef CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
#include "esp_crt_bundle.h"
#endif

#ifdef CONFIG_ESP_TLS_USE_SECURE_ELEMENT
/* cryptoauthlib includes */
#include "mbedtls/atca_mbedtls_wrap.h"
#include "tng_atca.h"
#include "cryptoauthlib.h"
static const atcacert_def_t *cert_def = NULL;
/* Prototypes for functions */
static esp_err_t esp_set_atecc608a_pki_context(esp_tls_t *tls, const void *pki);
#endif /* CONFIG_ESP_TLS_USE_SECURE_ELEMENT */

#if defined(CONFIG_ESP_TLS_USE_DS_PERIPHERAL)
#include "rsa_sign_alt.h"
static esp_err_t esp_mbedtls_init_pk_ctx_for_ds(const void *pki);
#endif /* CONFIG_ESP_TLS_USE_DS_PERIPHERAL */

static const char *TAG = "esp-tls-mbedtls";
static mbedtls_x509_crt *global_cacert = NULL;

/* This function shall return the error message when appropriate log level has been set, otherwise this function shall do nothing */
static void mbedtls_print_error_msg(int error)
{
#if (CONFIG_LOG_DEFAULT_LEVEL_DEBUG || CONFIG_LOG_DEFAULT_LEVEL_VERBOSE)
    static char error_buf[100];
    mbedtls_strerror(error, error_buf, sizeof(error_buf));
    ESP_LOGI(TAG, "(%04X): %s", error, error_buf);
#endif
}

typedef struct esp_tls_pki_t {
    mbedtls_x509_crt *public_cert;
    mbedtls_pk_context *pk_key;
    const unsigned char *publiccert_pem_buf;
    unsigned int publiccert_pem_bytes;
    const unsigned char *privkey_pem_buf;
    unsigned int privkey_pem_bytes;
    const unsigned char *privkey_password;
    unsigned int privkey_password_len;
#ifdef CONFIG_ESP_TLS_USE_DS_PERIPHERAL
    void *esp_ds_data;
#endif
} esp_tls_pki_t;

static esp_err_t set_server_config(esp_tls_cfg_server_t *cfg, esp_tls_t *tls);

esp_err_t esp_create_mbedtls_handle(const char *hostname, size_t hostlen, const void *cfg, esp_tls_t *tls, void *server_params)
{
    assert(cfg != NULL);
    assert(tls != NULL);
    int ret;
    esp_err_t esp_ret = ESP_FAIL;

#ifdef CONFIG_MBEDTLS_SSL_PROTO_TLS1_3
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "Failed to initialize PSA crypto, returned %d", (int) status);
        return esp_ret;
    }
#endif // CONFIG_MBEDTLS_SSL_PROTO_TLS1_3

    tls->server_fd.fd = tls->sockfd;
    mbedtls_ssl_init(&tls->ssl);
    mbedtls_ctr_drbg_init(&tls->ctr_drbg);
    mbedtls_ssl_config_init(&tls->conf);
    mbedtls_entropy_init(&tls->entropy);

    if ((ret = mbedtls_ctr_drbg_seed(&tls->ctr_drbg,
                                     mbedtls_entropy_func, &tls->entropy, NULL, 0)) != 0) {
        ESP_LOGE(TAG, "mbedtls_ctr_drbg_seed returned -0x%04X", -ret);
        mbedtls_print_error_msg(ret);
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_MBEDTLS, -ret);
        esp_ret = ESP_ERR_MBEDTLS_CTR_DRBG_SEED_FAILED;
        goto exit;
    }

    mbedtls_ssl_conf_rng(&tls->conf, mbedtls_ctr_drbg_random, &tls->ctr_drbg);

    if (tls->role == ESP_TLS_CLIENT) {
        esp_ret = set_client_config(hostname, hostlen, (esp_tls_cfg_t *)cfg, tls);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set client configurations, returned [0x%04X] (%s)", esp_ret, esp_err_to_name(esp_ret));
            goto exit;
        }
        const esp_tls_proto_ver_t tls_ver = ((esp_tls_cfg_t *)cfg)->tls_version;
        if (tls_ver == ESP_TLS_VER_TLS_1_3) {
#if CONFIG_MBEDTLS_SSL_PROTO_TLS1_3
            ESP_LOGD(TAG, "Setting TLS version to 0x%4x", MBEDTLS_SSL_VERSION_TLS1_3);
            mbedtls_ssl_conf_min_tls_version(&tls->conf, MBEDTLS_SSL_VERSION_TLS1_3);
            mbedtls_ssl_conf_max_tls_version(&tls->conf, MBEDTLS_SSL_VERSION_TLS1_3);
#else
            ESP_LOGW(TAG, "TLS 1.3 is not enabled in config, continuing with default TLS protocol");
#endif
        } else if (tls_ver == ESP_TLS_VER_TLS_1_2) {
            ESP_LOGD(TAG, "Setting TLS version to 0x%4x", MBEDTLS_SSL_VERSION_TLS1_2);
            mbedtls_ssl_conf_min_tls_version(&tls->conf, MBEDTLS_SSL_VERSION_TLS1_2);
            mbedtls_ssl_conf_max_tls_version(&tls->conf, MBEDTLS_SSL_VERSION_TLS1_2);
        } else if (tls_ver != ESP_TLS_VER_ANY) {
            ESP_LOGE(TAG, "Unsupported protocol version");
            esp_ret = ESP_ERR_INVALID_ARG;
            goto exit;
        }
    } else if (tls->role == ESP_TLS_SERVER) {
        if (server_params == NULL) {
            /* Server params cannot be NULL when TLS role is server */
            return ESP_ERR_INVALID_ARG;
        }
        esp_tls_server_params_t *input_server_params = server_params;
        esp_ret = input_server_params->set_server_cfg((esp_tls_cfg_server_t *) cfg, tls);
        if (esp_ret != 0) {
            ESP_LOGE(TAG, "Failed to set server configurations, returned [0x%04X] (%s)", esp_ret, esp_err_to_name(esp_ret));
            goto exit;
        }
    }

#ifdef CONFIG_MBEDTLS_DEBUG
    mbedtls_esp_enable_debug_log(&tls->conf, CONFIG_MBEDTLS_DEBUG_LEVEL);
#endif

    if ((ret = mbedtls_ssl_setup(&tls->ssl, &tls->conf)) != 0) {
        ESP_LOGE(TAG, "mbedtls_ssl_setup returned -0x%04X", -ret);
        mbedtls_print_error_msg(ret);
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_MBEDTLS, -ret);
        esp_ret = ESP_ERR_MBEDTLS_SSL_SETUP_FAILED;
        goto exit;
    }
    mbedtls_ssl_set_bio(&tls->ssl, &tls->server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

    return ESP_OK;

exit:
    esp_mbedtls_cleanup(tls);
    return esp_ret;

}

void *esp_mbedtls_get_ssl_context(esp_tls_t *tls)
{
    if (tls == NULL) {
        ESP_LOGE(TAG, "Invalid arguments");
        return NULL;
    }
    return (void*)&tls->ssl;
}

#ifdef CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS
esp_tls_client_session_t *esp_mbedtls_get_client_session(esp_tls_t *tls)
{
    if (tls == NULL) {
        ESP_LOGE(TAG, "esp_tls session context cannot be NULL");
        return NULL;
    }

    esp_tls_client_session_t *client_session = (esp_tls_client_session_t*)calloc(1, sizeof(esp_tls_client_session_t));
    if (client_session == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for client session ctx");
        return NULL;
    }

    int ret = mbedtls_ssl_get_session(&tls->ssl, &(client_session->saved_session));
    if (ret != 0) {
        ESP_LOGE(TAG, "Error in obtaining the client ssl session");
        mbedtls_print_error_msg(ret);
        free(client_session);
        return NULL;
    }

    return client_session;
}

void esp_mbedtls_free_client_session(esp_tls_client_session_t *client_session)
{
    if (client_session) {
        mbedtls_ssl_session_free(&(client_session->saved_session));
        free(client_session);
    }
}
#endif /* CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS */

int esp_mbedtls_handshake(esp_tls_t *tls, const esp_tls_cfg_t *cfg)
{
    int ret;
#ifdef CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS
    if (cfg->client_session != NULL) {
        ESP_LOGD(TAG, "Reusing the already saved client session context");
        if ((ret = mbedtls_ssl_set_session(&tls->ssl, &(cfg->client_session->saved_session))) != 0 ) {
            ESP_LOGE(TAG, " mbedtls_ssl_conf_session returned -0x%04X", -ret);
            return -1;
        }
    }
#endif
    ret = mbedtls_ssl_handshake(&tls->ssl);
    if (ret == 0) {
        tls->conn_state = ESP_TLS_DONE;

#ifdef CONFIG_ESP_TLS_USE_DS_PERIPHERAL
        esp_ds_release_ds_lock();
#endif
        return 1;
    } else {
        if (ret != ESP_TLS_ERR_SSL_WANT_READ && ret != ESP_TLS_ERR_SSL_WANT_WRITE) {
            ESP_LOGE(TAG, "mbedtls_ssl_handshake returned -0x%04X", -ret);
            mbedtls_print_error_msg(ret);
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_MBEDTLS, -ret);
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_ESP, ESP_ERR_MBEDTLS_SSL_HANDSHAKE_FAILED);
            if (cfg->crt_bundle_attach != NULL || cfg->cacert_buf != NULL || cfg->use_global_ca_store == true) {
                /* This is to check whether handshake failed due to invalid certificate*/
                esp_mbedtls_verify_certificate(tls);
            }
            tls->conn_state = ESP_TLS_FAIL;
            return -1;
        }
        /* Irrespective of blocking or non-blocking I/O, we return on getting ESP_TLS_ERR_SSL_WANT_READ
        or ESP_TLS_ERR_SSL_WANT_WRITE during handshake */
        return 0;
    }
}

ssize_t esp_mbedtls_read(esp_tls_t *tls, char *data, size_t datalen)
{

    ssize_t ret = mbedtls_ssl_read(&tls->ssl, (unsigned char *)data, datalen);
#if CONFIG_MBEDTLS_CLIENT_SSL_SESSION_TICKETS
    // If a post-handshake message is received, connection state is changed to `MBEDTLS_SSL_TLS1_3_NEW_SESSION_TICKET`
    // Call mbedtls_ssl_read() till state is `MBEDTLS_SSL_TLS1_3_NEW_SESSION_TICKET` or return code is `MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET`
    // to process session tickets in TLS 1.3 connection
    if (mbedtls_ssl_get_version_number(&tls->ssl) == MBEDTLS_SSL_VERSION_TLS1_3) {
        while (ret == MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET || tls->ssl.MBEDTLS_PRIVATE(state) == MBEDTLS_SSL_TLS1_3_NEW_SESSION_TICKET) {
            ESP_LOGD(TAG, "got session ticket in TLS 1.3 connection, retry read");
            ret = mbedtls_ssl_read(&tls->ssl, (unsigned char *)data, datalen);
        }
    }
#endif // CONFIG_MBEDTLS_CLIENT_SSL_SESSION_TICKETS

    if (ret < 0) {
        if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
            return 0;
        }
        if (ret != ESP_TLS_ERR_SSL_WANT_READ  && ret != ESP_TLS_ERR_SSL_WANT_WRITE) {
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_MBEDTLS, -ret);
            ESP_LOGE(TAG, "read error :-0x%04zX:", -ret);
            mbedtls_print_error_msg(ret);
        }
    }
    return ret;
}

ssize_t esp_mbedtls_write(esp_tls_t *tls, const char *data, size_t datalen)
{
    size_t written = 0;
    size_t write_len = datalen;
    while (written < datalen) {
        if (write_len > MBEDTLS_SSL_OUT_CONTENT_LEN) {
            write_len = MBEDTLS_SSL_OUT_CONTENT_LEN;
        }
        if (datalen > MBEDTLS_SSL_OUT_CONTENT_LEN) {
            ESP_LOGD(TAG, "Fragmenting data of excessive size :%zu, offset: %zu, size %zu", datalen, written, write_len);
        }
        ssize_t ret = mbedtls_ssl_write(&tls->ssl, (unsigned char*) data + written, write_len);
        if (ret <= 0) {
            if (ret != ESP_TLS_ERR_SSL_WANT_READ  && ret != ESP_TLS_ERR_SSL_WANT_WRITE && ret != 0) {
                ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_MBEDTLS, -ret);
                ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_ESP, ESP_ERR_MBEDTLS_SSL_WRITE_FAILED);
                ESP_LOGE(TAG, "write error :-0x%04zX:", -ret);
                mbedtls_print_error_msg(ret);
                return ret;
            } else {
                // Exiting the tls-write process as less than desired datalen are writable
                ESP_LOGD(TAG, "mbedtls_ssl_write() returned -0x%04zX, already written %zu, exitting...", -ret, written);
                mbedtls_print_error_msg(ret);
                return (written > 0) ? written : ret;
            }
        }
        written += ret;
        write_len = datalen - written;
    }
    return written;
}

void esp_mbedtls_conn_delete(esp_tls_t *tls)
{
    if (tls != NULL) {
        esp_mbedtls_cleanup(tls);
        if (tls->is_tls) {
            if (tls->server_fd.fd != -1) {
                mbedtls_net_free(&tls->server_fd);
                /* Socket is already closed by `mbedtls_net_free` and hence also change assignment of its copy to an invalid value */
                tls->sockfd = -1;
            }
        }
    }
}

void esp_mbedtls_verify_certificate(esp_tls_t *tls)
{
    int flags;

    if ((flags = mbedtls_ssl_get_verify_result(&tls->ssl)) != 0) {
        ESP_LOGI(TAG, "Failed to verify peer certificate!");
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_MBEDTLS_CERT_FLAGS, flags);
#if (CONFIG_LOG_DEFAULT_LEVEL_DEBUG || CONFIG_LOG_DEFAULT_LEVEL_VERBOSE)
        char buf[100];
        bzero(buf, sizeof(buf));
        mbedtls_x509_crt_verify_info(buf, sizeof(buf), "  ! ", flags);
        ESP_LOGD(TAG, "verification info: %s", buf);
#endif
    } else {
        ESP_LOGI(TAG, "Certificate verified.");
    }
}

ssize_t esp_mbedtls_get_bytes_avail(esp_tls_t *tls)
{
    if (!tls) {
        ESP_LOGE(TAG, "empty arg passed to esp_tls_get_bytes_avail()");
        return ESP_FAIL;
    }
    return mbedtls_ssl_get_bytes_avail(&tls->ssl);
}

void esp_mbedtls_cleanup(esp_tls_t *tls)
{
    if (!tls) {
        return;
    }
    if (tls->cacert_ptr != global_cacert) {
        mbedtls_x509_crt_free(tls->cacert_ptr);
    }
    tls->cacert_ptr = NULL;
    mbedtls_x509_crt_free(&tls->cacert);
    mbedtls_x509_crt_free(&tls->clientcert);
    mbedtls_pk_free(&tls->clientkey);
    mbedtls_entropy_free(&tls->entropy);
    mbedtls_ssl_config_free(&tls->conf);
    mbedtls_ctr_drbg_free(&tls->ctr_drbg);
    mbedtls_ssl_free(&tls->ssl);
#ifdef CONFIG_ESP_TLS_USE_SECURE_ELEMENT
    atcab_release();
#endif
#ifdef CONFIG_ESP_TLS_USE_DS_PERIPHERAL
    esp_ds_release_ds_lock();
#endif
}

static esp_err_t set_ca_cert(esp_tls_t *tls, const unsigned char *cacert, size_t cacert_len)
{
    assert(tls);
    tls->cacert_ptr = &tls->cacert;
    mbedtls_x509_crt_init(tls->cacert_ptr);
    int ret = mbedtls_x509_crt_parse(tls->cacert_ptr, cacert, cacert_len);
    if (ret < 0) {
        ESP_LOGE(TAG, "mbedtls_x509_crt_parse of CA cert returned -0x%04X", -ret);
        mbedtls_print_error_msg(ret);
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_MBEDTLS, -ret);
        return ESP_ERR_MBEDTLS_X509_CRT_PARSE_FAILED;
    }
    if (ret > 0) {
        /* This will happen if the CA chain contains one or more invalid certs, going ahead as the hadshake
         * may still succeed if the other certificates in the CA chain are enough for the authentication */
        ESP_LOGW(TAG, "mbedtls_x509_crt_parse was partly successful. No. of failed certificates: %d", ret);
    }
    mbedtls_ssl_conf_authmode(&tls->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    mbedtls_ssl_conf_ca_chain(&tls->conf, tls->cacert_ptr, NULL);
    return ESP_OK;
}

static esp_err_t set_pki_context(esp_tls_t *tls, const esp_tls_pki_t *pki)
{
    assert(tls);
    assert(pki);
    int ret;

    if (pki->publiccert_pem_buf != NULL &&
        pki->public_cert != NULL &&
        pki->pk_key != NULL) {

        mbedtls_x509_crt_init(pki->public_cert);
        mbedtls_pk_init(pki->pk_key);

        ret = mbedtls_x509_crt_parse(pki->public_cert, pki->publiccert_pem_buf, pki->publiccert_pem_bytes);
        if (ret < 0) {
            ESP_LOGE(TAG, "mbedtls_x509_crt_parse of public cert returned -0x%04X", -ret);
            mbedtls_print_error_msg(ret);
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_MBEDTLS, -ret);
            return ESP_ERR_MBEDTLS_X509_CRT_PARSE_FAILED;
        }

#ifdef CONFIG_ESP_TLS_USE_DS_PERIPHERAL
        if (pki->esp_ds_data != NULL) {
            ret = esp_mbedtls_init_pk_ctx_for_ds(pki);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Failed to initialize pk context for esp_ds");
                return ret;
            }
        } else
#endif
#ifdef CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN
        if (tls->use_ecdsa_peripheral) {
            esp_ecdsa_pk_conf_t conf = {
                .grp_id = MBEDTLS_ECP_DP_SECP256R1,
                .efuse_block = tls->ecdsa_efuse_blk,
            };
            ret = esp_ecdsa_set_pk_context(pki->pk_key, &conf);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Failed to initialize pk context for ecdsa peripheral with the key stored in efuse block %d", tls->ecdsa_efuse_blk);
                return ret;
            }
        } else
#endif
        if (pki->privkey_pem_buf != NULL) {
            ret = mbedtls_pk_parse_key(pki->pk_key, pki->privkey_pem_buf, pki->privkey_pem_bytes,
                                       pki->privkey_password, pki->privkey_password_len,
                                       mbedtls_ctr_drbg_random, &tls->ctr_drbg);
        } else {
            return ESP_ERR_INVALID_ARG;
        }

        if (ret < 0) {
            ESP_LOGE(TAG, "mbedtls_pk_parse_keyfile returned -0x%04X", -ret);
            mbedtls_print_error_msg(ret);
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_MBEDTLS, -ret);
            return ESP_ERR_MBEDTLS_PK_PARSE_KEY_FAILED;
        }

        ret = mbedtls_ssl_conf_own_cert(&tls->conf, pki->public_cert, pki->pk_key);
        if (ret < 0) {
            ESP_LOGE(TAG, "mbedtls_ssl_conf_own_cert returned -0x%04X", -ret);
            mbedtls_print_error_msg(ret);
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_MBEDTLS, -ret);
            return ESP_ERR_MBEDTLS_SSL_CONF_OWN_CERT_FAILED;
        }
    } else {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

static esp_err_t set_global_ca_store(esp_tls_t *tls)
{
    assert(tls);
    if (global_cacert == NULL) {
        ESP_LOGE(TAG, "global_cacert is NULL");
        return ESP_ERR_INVALID_STATE;
    }
    tls->cacert_ptr = global_cacert;
    mbedtls_ssl_conf_authmode(&tls->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    mbedtls_ssl_conf_ca_chain(&tls->conf, tls->cacert_ptr, NULL);
    return ESP_OK;
}

#ifdef CONFIG_ESP_TLS_SERVER_SESSION_TICKETS
int esp_mbedtls_server_session_ticket_write(void *p_ticket, const mbedtls_ssl_session *session, unsigned char *start, const unsigned char *end, size_t *tlen, uint32_t *lifetime)
{
    int ret = mbedtls_ssl_ticket_write(p_ticket, session, start, end, tlen, lifetime);
#ifndef NDEBUG
    if (ret != 0) {
        ESP_LOGE(TAG, "Writing session ticket resulted in error code -0x%04X", -ret);
        mbedtls_print_error_msg(ret);
    }
#endif
    return ret;
}

int esp_mbedtls_server_session_ticket_parse(void *p_ticket, mbedtls_ssl_session *session, unsigned char *buf, size_t len)
{
    int ret = mbedtls_ssl_ticket_parse(p_ticket, session, buf, len);
#ifndef NDEBUG
    if (ret != 0) {
        ESP_LOGD(TAG, "Parsing session ticket resulted in error code -0x%04X", -ret);
        mbedtls_print_error_msg(ret);
    }
#endif
    return ret;
}

esp_err_t esp_mbedtls_server_session_ticket_ctx_init(esp_tls_server_session_ticket_ctx_t *ctx)
{
    if (!ctx) {
        return ESP_ERR_INVALID_ARG;
    }
    mbedtls_ctr_drbg_init(&ctx->ctr_drbg);
    mbedtls_entropy_init(&ctx->entropy);
    mbedtls_ssl_ticket_init(&ctx->ticket_ctx);
    int ret;
    esp_err_t esp_ret;
    if ((ret = mbedtls_ctr_drbg_seed(&ctx->ctr_drbg,
                    mbedtls_entropy_func, &ctx->entropy, NULL, 0)) != 0) {
        ESP_LOGE(TAG, "mbedtls_ctr_drbg_seed returned -0x%04X", -ret);
        mbedtls_print_error_msg(ret);
        esp_ret = ESP_ERR_MBEDTLS_CTR_DRBG_SEED_FAILED;
        goto exit;
    }

    if((ret = mbedtls_ssl_ticket_setup(&ctx->ticket_ctx,
                    mbedtls_ctr_drbg_random, &ctx->ctr_drbg,
                    MBEDTLS_CIPHER_AES_256_GCM,
                    CONFIG_ESP_TLS_SERVER_SESSION_TICKET_TIMEOUT)) != 0) {
        ESP_LOGE(TAG, "mbedtls_ssl_ticket_setup returned -0x%04X", -ret);
        mbedtls_print_error_msg(ret);
        esp_ret = ESP_ERR_MBEDTLS_SSL_TICKET_SETUP_FAILED;
        goto exit;
    }
    return ESP_OK;
exit:
    esp_mbedtls_server_session_ticket_ctx_free(ctx);
    return esp_ret;
}

void esp_mbedtls_server_session_ticket_ctx_free(esp_tls_server_session_ticket_ctx_t *ctx)
{
    if (ctx) {
        mbedtls_ssl_ticket_free(&ctx->ticket_ctx);
        mbedtls_ctr_drbg_init(&ctx->ctr_drbg);
        mbedtls_entropy_free(&ctx->entropy);
    }
}
#endif

static esp_err_t set_server_config(esp_tls_cfg_server_t *cfg, esp_tls_t *tls)
{
    assert(cfg != NULL);
    assert(tls != NULL);
    int ret;
    esp_err_t esp_ret;
    if ((ret = mbedtls_ssl_config_defaults(&tls->conf,
                    MBEDTLS_SSL_IS_SERVER,
                    MBEDTLS_SSL_TRANSPORT_STREAM,
                    MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        ESP_LOGE(TAG, "mbedtls_ssl_config_defaults returned -0x%04X", -ret);
        mbedtls_print_error_msg(ret);
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_MBEDTLS, -ret);
        return ESP_ERR_MBEDTLS_SSL_CONFIG_DEFAULTS_FAILED;
    }

    mbedtls_ssl_conf_set_user_data_p(&tls->conf, cfg->userdata);

#ifdef CONFIG_MBEDTLS_SSL_ALPN
    if (cfg->alpn_protos) {
        mbedtls_ssl_conf_alpn_protocols(&tls->conf, cfg->alpn_protos);
    }
#endif

#if defined(CONFIG_ESP_TLS_SERVER_CERT_SELECT_HOOK)
    if (cfg->cert_select_cb != NULL) {
        ESP_LOGI(TAG, "Initializing server side cert selection cb");
        mbedtls_ssl_conf_cert_cb(&tls->conf, cfg->cert_select_cb);
    }
#endif

    if (cfg->cacert_buf != NULL) {
        esp_ret = set_ca_cert(tls, cfg->cacert_buf, cfg->cacert_bytes);
        if (esp_ret != ESP_OK) {
            return esp_ret;
        }
    } else {
#ifdef CONFIG_ESP_TLS_SERVER_MIN_AUTH_MODE_OPTIONAL
        mbedtls_ssl_conf_authmode(&tls->conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
#else
        mbedtls_ssl_conf_authmode(&tls->conf, MBEDTLS_SSL_VERIFY_NONE);
#endif
    }

    if (cfg->use_secure_element) {
#ifdef CONFIG_ESP_TLS_USE_SECURE_ELEMENT
        esp_tls_pki_t pki = {
            .public_cert = &tls->servercert,
            .pk_key = &tls->serverkey,
            .publiccert_pem_buf = cfg->servercert_buf,
            .publiccert_pem_bytes = cfg->servercert_bytes,
            .privkey_pem_buf = NULL,
            .privkey_pem_bytes = 0,
            .privkey_password = NULL,
            .privkey_password_len = 0,
        };

        ret = esp_set_atecc608a_pki_context(tls, (void*) &pki);
        if (ret != ESP_OK) {
            return ret;
        }
#else
        ESP_LOGE(TAG, "Please enable secure element support for ESP-TLS in menuconfig");
        return ESP_FAIL;
#endif /* CONFIG_ESP_TLS_USE_SECURE_ELEMENT */
    }  else if (cfg->use_ecdsa_peripheral) {
#ifdef CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN
        tls->use_ecdsa_peripheral = cfg->use_ecdsa_peripheral;
        tls->ecdsa_efuse_blk = cfg->ecdsa_key_efuse_blk;
        esp_tls_pki_t pki = {
            .public_cert = &tls->servercert,
            .pk_key = &tls->serverkey,
            .publiccert_pem_buf = cfg->servercert_buf,
            .publiccert_pem_bytes = cfg->servercert_bytes,
            .privkey_pem_buf = NULL,
            .privkey_pem_bytes = 0,
            .privkey_password = NULL,
            .privkey_password_len = 0,
        };
        esp_err_t esp_ret = set_pki_context(tls, &pki);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set client pki context");
            return esp_ret;
        }
#else
        ESP_LOGE(TAG, "Please enable the support for signing using ECDSA peripheral in menuconfig.");
        return ESP_FAIL;
#endif
    } else if (cfg->servercert_buf != NULL && cfg->serverkey_buf != NULL) {
        esp_tls_pki_t pki = {
            .public_cert = &tls->servercert,
            .pk_key = &tls->serverkey,
            .publiccert_pem_buf = cfg->servercert_buf,
            .publiccert_pem_bytes = cfg->servercert_bytes,
            .privkey_pem_buf = cfg->serverkey_buf,
            .privkey_pem_bytes = cfg->serverkey_bytes,
            .privkey_password = cfg->serverkey_password,
            .privkey_password_len = cfg->serverkey_password_len,
        };
        esp_ret = set_pki_context(tls, &pki);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set server pki context");
            return esp_ret;
        }
    } else {
#if defined(CONFIG_ESP_TLS_SERVER_CERT_SELECT_HOOK)
        if (cfg->cert_select_cb == NULL) {
            ESP_LOGE(TAG, "No cert select cb is defined");
        } else {
            /* At this point Callback MUST ALWAYS call mbedtls_ssl_set_hs_own_cert, or the handshake will abort! */
            ESP_LOGD(TAG, "Missing server cert and/or key, but cert selection cb is defined.");
            return ESP_OK;
        }
#else
        ESP_LOGE(TAG, "Missing server certificate and/or key");
#endif
        return ESP_ERR_INVALID_STATE;
    }

#ifdef CONFIG_ESP_TLS_SERVER_SESSION_TICKETS
    if (cfg->ticket_ctx) {
        ESP_LOGD(TAG, "Enabling server-side tls session ticket support");

        mbedtls_ssl_conf_session_tickets_cb( &tls->conf,
                esp_mbedtls_server_session_ticket_write,
                esp_mbedtls_server_session_ticket_parse,
                &cfg->ticket_ctx->ticket_ctx );
    }
#endif

    return ESP_OK;
}

esp_err_t set_client_config(const char *hostname, size_t hostlen, esp_tls_cfg_t *cfg, esp_tls_t *tls)
{
    assert(cfg != NULL);
    assert(tls != NULL);
    int ret;
    if (!cfg->skip_common_name) {
        char *use_host = NULL;
        if (cfg->common_name != NULL) {
            use_host = strndup(cfg->common_name, strlen(cfg->common_name));
        } else {
            use_host = strndup(hostname, hostlen);
        }

        if (use_host == NULL) {
            return ESP_ERR_NO_MEM;
        }
        /* Hostname set here should match CN in server certificate */
        if ((ret = mbedtls_ssl_set_hostname(&tls->ssl, use_host)) != 0) {
            ESP_LOGE(TAG, "mbedtls_ssl_set_hostname returned -0x%04X", -ret);
            mbedtls_print_error_msg(ret);
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_MBEDTLS, -ret);
            free(use_host);
            return ESP_ERR_MBEDTLS_SSL_SET_HOSTNAME_FAILED;
        }
        free(use_host);
    }

    if ((ret = mbedtls_ssl_config_defaults(&tls->conf,
                                           MBEDTLS_SSL_IS_CLIENT,
                                           MBEDTLS_SSL_TRANSPORT_STREAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        ESP_LOGE(TAG, "mbedtls_ssl_config_defaults returned -0x%04X", -ret);
        mbedtls_print_error_msg(ret);
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_MBEDTLS, -ret);
        return ESP_ERR_MBEDTLS_SSL_CONFIG_DEFAULTS_FAILED;
    }

#ifdef CONFIG_MBEDTLS_SSL_RENEGOTIATION
    mbedtls_ssl_conf_renegotiation(&tls->conf, MBEDTLS_SSL_RENEGOTIATION_ENABLED);
#endif

    if (cfg->alpn_protos) {
#ifdef CONFIG_MBEDTLS_SSL_ALPN
        if ((ret = mbedtls_ssl_conf_alpn_protocols(&tls->conf, cfg->alpn_protos)) != 0) {
            ESP_LOGE(TAG, "mbedtls_ssl_conf_alpn_protocols returned -0x%04X", -ret);
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_MBEDTLS, -ret);
            mbedtls_print_error_msg(ret);
            return ESP_ERR_MBEDTLS_SSL_CONF_ALPN_PROTOCOLS_FAILED;
        }
#else
        ESP_LOGE(TAG, "alpn_protos configured but not enabled in menuconfig: Please enable MBEDTLS_SSL_ALPN option");
        return ESP_ERR_INVALID_STATE;
#endif
    }

#ifdef CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS
    ESP_LOGD(TAG, "Enabling client-side tls session ticket support");
    mbedtls_ssl_conf_session_tickets(&tls->conf, MBEDTLS_SSL_SESSION_TICKETS_ENABLED);
    mbedtls_ssl_conf_renegotiation(&tls->conf, MBEDTLS_SSL_RENEGOTIATION_ENABLED);

#endif /* CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS */

    if (cfg->crt_bundle_attach != NULL) {
#ifdef CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
        ESP_LOGD(TAG, "Use certificate bundle");
        mbedtls_ssl_conf_authmode(&tls->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
        cfg->crt_bundle_attach(&tls->conf);
#else //CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
        ESP_LOGE(TAG, "use_crt_bundle configured but not enabled in menuconfig: Please enable MBEDTLS_CERTIFICATE_BUNDLE option");
        return ESP_ERR_INVALID_STATE;
#endif
    } else if (cfg->use_global_ca_store == true) {
        esp_err_t esp_ret = set_global_ca_store(tls);
        if (esp_ret != ESP_OK) {
            return esp_ret;
        }
    } else if (cfg->cacert_buf != NULL) {
        esp_err_t esp_ret = set_ca_cert(tls, cfg->cacert_buf, cfg->cacert_bytes);
        if (esp_ret != ESP_OK) {
            return esp_ret;
        }
        mbedtls_ssl_conf_ca_chain(&tls->conf, tls->cacert_ptr, NULL);
    } else if (cfg->psk_hint_key) {
#if defined(CONFIG_ESP_TLS_PSK_VERIFICATION)
        //
        // PSK encryption mode is configured only if no certificate supplied and psk pointer not null
        ESP_LOGD(TAG, "ssl psk authentication");
        ret = mbedtls_ssl_conf_psk(&tls->conf, cfg->psk_hint_key->key, cfg->psk_hint_key->key_size,
                                   (const unsigned char *)cfg->psk_hint_key->hint, strlen(cfg->psk_hint_key->hint));
        if (ret != 0) {
            ESP_LOGE(TAG, "mbedtls_ssl_conf_psk returned -0x%04X", -ret);
            mbedtls_print_error_msg(ret);
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_MBEDTLS, -ret);
            return ESP_ERR_MBEDTLS_SSL_CONF_PSK_FAILED;
        }
#else
        ESP_LOGE(TAG, "psk_hint_key configured but not enabled in menuconfig: Please enable ESP_TLS_PSK_VERIFICATION option");
        return ESP_ERR_INVALID_STATE;
#endif
#ifdef CONFIG_ESP_TLS_CLIENT_SESSION_TICKETS
    } else if (cfg->client_session != NULL) {
        ESP_LOGD(TAG, "Resuing the saved client session");
#endif
    } else {
#ifdef CONFIG_ESP_TLS_SKIP_SERVER_CERT_VERIFY
        mbedtls_ssl_conf_authmode(&tls->conf, MBEDTLS_SSL_VERIFY_NONE);
#else
        ESP_LOGE(TAG, "No server verification option set in esp_tls_cfg_t structure. Check esp_tls API reference");
        return ESP_ERR_MBEDTLS_SSL_SETUP_FAILED;
#endif
    }

    if (cfg->use_secure_element) {
#ifdef CONFIG_ESP_TLS_USE_SECURE_ELEMENT
        esp_tls_pki_t pki = {
            .public_cert = &tls->clientcert,
            .pk_key = &tls->clientkey,
            .publiccert_pem_buf = cfg->clientcert_buf,
            .publiccert_pem_bytes = cfg->clientcert_bytes,
            .privkey_pem_buf = NULL,
            .privkey_pem_bytes = 0,
            .privkey_password = NULL,
            .privkey_password_len = 0,
        };
        ret = esp_set_atecc608a_pki_context(tls, (void*) &pki);
        if (ret != ESP_OK) {
            return ret;
        }
#else
        ESP_LOGE(TAG, "Please enable secure element support for ESP-TLS in menuconfig");
        return ESP_FAIL;
#endif /* CONFIG_ESP_TLS_USE_SECURE_ELEMENT */
    } else if (cfg->ds_data != NULL) {
#ifdef CONFIG_ESP_TLS_USE_DS_PERIPHERAL
        if (cfg->clientcert_pem_buf == NULL) {
            ESP_LOGE(TAG, "Client certificate is also required with the DS parameters");
            return ESP_ERR_INVALID_STATE;
        }
        esp_ds_set_session_timeout(cfg->timeout_ms);
        /* set private key pointer to NULL since the DS peripheral with its own configuration data is used */
        esp_tls_pki_t pki = {
            .public_cert = &tls->clientcert,
            .pk_key = &tls->clientkey,
            .publiccert_pem_buf = cfg->clientcert_buf,
            .publiccert_pem_bytes = cfg->clientcert_bytes,
            .privkey_pem_buf = NULL,
            .privkey_pem_bytes = 0,
            .privkey_password = NULL,
            .privkey_password_len = 0,
            .esp_ds_data = cfg->ds_data,
        };

        esp_err_t esp_ret = set_pki_context(tls, &pki);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set client pki context for the DS peripheral, returned [0x%04X] (%s)", ret, esp_err_to_name(esp_ret));
            return esp_ret;
        }
#else
        ESP_LOGE(TAG, "Please enable the DS peripheral support for the ESP-TLS in menuconfig. (only supported for the ESP32-S2 chip)");
        return ESP_FAIL;
#endif
    } else if (cfg->use_ecdsa_peripheral) {
#ifdef CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN
        tls->use_ecdsa_peripheral = cfg->use_ecdsa_peripheral;
        tls->ecdsa_efuse_blk = cfg->ecdsa_key_efuse_blk;
        esp_tls_pki_t pki = {
            .public_cert = &tls->clientcert,
            .pk_key = &tls->clientkey,
            .publiccert_pem_buf = cfg->clientcert_buf,
            .publiccert_pem_bytes = cfg->clientcert_bytes,
            .privkey_pem_buf = NULL,
            .privkey_pem_bytes = 0,
            .privkey_password = NULL,
            .privkey_password_len = 0,
        };
        esp_err_t esp_ret = set_pki_context(tls, &pki);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set client pki context");
            return esp_ret;
        }
        static const int ecdsa_peripheral_supported_ciphersuites[] = {
            MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256,
#if CONFIG_MBEDTLS_SSL_PROTO_TLS1_3
            MBEDTLS_TLS1_3_AES_128_GCM_SHA256,
#endif
            0
        };

        ESP_LOGD(TAG, "Set the ciphersuites list");
        mbedtls_ssl_conf_ciphersuites(&tls->conf, ecdsa_peripheral_supported_ciphersuites);
#else
        ESP_LOGE(TAG, "Please enable the support for signing using ECDSA peripheral in menuconfig.");
        return ESP_FAIL;
#endif
    } else if (cfg->clientcert_pem_buf != NULL && cfg->clientkey_pem_buf != NULL) {
        esp_tls_pki_t pki = {
            .public_cert = &tls->clientcert,
            .pk_key = &tls->clientkey,
            .publiccert_pem_buf = cfg->clientcert_buf,
            .publiccert_pem_bytes = cfg->clientcert_bytes,
            .privkey_pem_buf = cfg->clientkey_buf,
            .privkey_pem_bytes = cfg->clientkey_bytes,
            .privkey_password = cfg->clientkey_password,
            .privkey_password_len = cfg->clientkey_password_len,
        };
        esp_err_t esp_ret = set_pki_context(tls, &pki);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set client pki context");
            return esp_ret;
        }
    } else if (cfg->clientcert_buf != NULL || cfg->clientkey_buf != NULL) {
        ESP_LOGE(TAG, "You have to provide both clientcert_buf and clientkey_buf for mutual authentication");
        return ESP_ERR_INVALID_STATE;
    }

    if (cfg->ciphersuites_list != NULL && cfg->ciphersuites_list[0] != 0) {
        ESP_LOGD(TAG, "Set the ciphersuites list");
        mbedtls_ssl_conf_ciphersuites(&tls->conf, cfg->ciphersuites_list);
    }
    return ESP_OK;
}

/**
 * @brief      Create TLS/SSL server session
 */
int esp_mbedtls_server_session_create(esp_tls_cfg_server_t *cfg, int sockfd, esp_tls_t *tls)
{
    if (tls == NULL || cfg == NULL) {
        return -1;
    }
    tls->role = ESP_TLS_SERVER;
    tls->sockfd = sockfd;
    esp_tls_server_params_t server_params = {};
    server_params.set_server_cfg = &set_server_config;
    esp_err_t esp_ret = esp_create_mbedtls_handle(NULL, 0, cfg, tls, &server_params);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "create_ssl_handle failed, returned [0x%04X] (%s)", esp_ret, esp_err_to_name(esp_ret));
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_ESP, esp_ret);
        tls->conn_state = ESP_TLS_FAIL;
        return -1;
    }

    tls->read = esp_mbedtls_read;
    tls->write = esp_mbedtls_write;
    int ret;
    while ((ret = mbedtls_ssl_handshake(&tls->ssl)) != 0) {
        if (ret != ESP_TLS_ERR_SSL_WANT_READ && ret != ESP_TLS_ERR_SSL_WANT_WRITE) {
            ESP_LOGE(TAG, "mbedtls_ssl_handshake returned -0x%04X", -ret);
            mbedtls_print_error_msg(ret);
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_MBEDTLS, -ret);
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_ESP, ESP_ERR_MBEDTLS_SSL_HANDSHAKE_FAILED);
            tls->conn_state = ESP_TLS_FAIL;
            return ret;
        }
    }
    return 0;
}
/**
 * @brief      Close the server side TLS/SSL connection and free any allocated resources.
 */
void esp_mbedtls_server_session_delete(esp_tls_t *tls)
{
    if (tls != NULL) {
        esp_mbedtls_cleanup(tls);
        esp_tls_internal_event_tracker_destroy(tls->error_handle);
        free(tls);
    }
};

esp_err_t esp_mbedtls_init_global_ca_store(void)
{
    if (global_cacert == NULL) {
        global_cacert = (mbedtls_x509_crt *)calloc(1, sizeof(mbedtls_x509_crt));
        if (global_cacert == NULL) {
            ESP_LOGE(TAG, "global_cacert not allocated");
            return ESP_ERR_NO_MEM;
        }
        mbedtls_x509_crt_init(global_cacert);
    }
    return ESP_OK;
}

esp_err_t esp_mbedtls_set_global_ca_store(const unsigned char *cacert_pem_buf, const unsigned int cacert_pem_bytes)
{
#ifdef CONFIG_MBEDTLS_DYNAMIC_FREE_CA_CERT
    ESP_LOGE(TAG, "Please disable dynamic freeing of ca cert in mbedtls (CONFIG_MBEDTLS_DYNAMIC_FREE_CA_CERT)\n in order to use the global ca_store");
    return ESP_FAIL;
#endif
    if (cacert_pem_buf == NULL) {
        ESP_LOGE(TAG, "cacert_pem_buf is null");
        return ESP_ERR_INVALID_ARG;
    }
    int ret;
    if (global_cacert == NULL) {
        ret = esp_mbedtls_init_global_ca_store();
        if (ret != ESP_OK) {
            return ret;
        }
    }
    ret = mbedtls_x509_crt_parse(global_cacert, cacert_pem_buf, cacert_pem_bytes);
    if (ret < 0) {
        ESP_LOGE(TAG, "mbedtls_x509_crt_parse of global CA cert returned -0x%04X", -ret);
        mbedtls_print_error_msg(ret);
        mbedtls_x509_crt_free(global_cacert);
        free(global_cacert);
        global_cacert = NULL;
        return ESP_FAIL;
    } else if (ret > 0) {
        ESP_LOGE(TAG, "mbedtls_x509_crt_parse was partly successful. No. of failed certificates: %d", ret);
        return ESP_ERR_MBEDTLS_CERT_PARTLY_OK;
    }
    return ESP_OK;
}

mbedtls_x509_crt *esp_mbedtls_get_global_ca_store(void)
{
    return global_cacert;
}

void esp_mbedtls_free_global_ca_store(void)
{
    if (global_cacert) {
        mbedtls_x509_crt_free(global_cacert);
        free(global_cacert);
        global_cacert = NULL;
    }
}

const int *esp_mbedtls_get_ciphersuites_list(void)
{
    return mbedtls_ssl_list_ciphersuites();
}

#ifdef CONFIG_ESP_TLS_USE_SECURE_ELEMENT
static esp_err_t esp_init_atecc608a(uint8_t i2c_addr)
{
    cfg_ateccx08a_i2c_default.atcai2c.address = i2c_addr;
    int ret = atcab_init(&cfg_ateccx08a_i2c_default);
    if(ret != 0) {
        ESP_LOGE(TAG, "Failed to initialize atca device, returned -0x%04X", -ret);
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t esp_set_atecc608a_pki_context(esp_tls_t *tls, const void *pki)
{
    int ret = 0;
    esp_err_t esp_ret = ESP_FAIL;
    ESP_LOGI(TAG, "Initialize the ATECC interface...");
    (void)esp_ret;
    (void)cert_def;
#if defined(CONFIG_ATECC608A_TNG) || defined(CONFIG_ATECC608A_TFLEX)
#ifdef CONFIG_ATECC608A_TNG
    esp_ret = esp_init_atecc608a(CONFIG_ATCA_I2C_ADDRESS);
    if (ret != ESP_OK) {
        return ESP_ERR_ESP_TLS_SE_FAILED;
    }
#elif CONFIG_ATECC608A_TFLEX /* CONFIG_ATECC608A_TNG */
    esp_ret = esp_init_atecc608a(CONFIG_ATCA_I2C_ADDRESS);
    if (ret != ESP_OK) {
        return ESP_ERR_ESP_TLS_SE_FAILED;
    }
#endif /* CONFIG_ATECC608A_TFLEX */
    mbedtls_x509_crt_init(&tls->clientcert);
    ret = tng_get_device_cert_def(&cert_def);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to get device cert def");
        return ESP_ERR_ESP_TLS_SE_FAILED;
    }

    /* Extract the device certificate and convert to mbedtls cert */
    ret = atca_mbedtls_cert_add(&tls->clientcert, cert_def);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to parse cert from device, return 0x%04X", ret);
        mbedtls_print_error_msg(ret);
        return ESP_ERR_ESP_TLS_SE_FAILED;
    }
#elif CONFIG_ATECC608A_TCUSTOM
    esp_ret = esp_init_atecc608a(CONFIG_ATCA_I2C_ADDRESS);
    if (ret != ESP_OK) {
        return ESP_ERR_ESP_TLS_SE_FAILED;
    }
    mbedtls_x509_crt_init(&tls->clientcert);

    esp_tls_pki_t *pki_l = (esp_tls_pki_t *) pki;
    if (pki_l->publiccert_pem_buf != NULL) {
        ret = mbedtls_x509_crt_parse(&tls->clientcert, pki_l->publiccert_pem_buf, pki_l->publiccert_pem_bytes);
        if (ret < 0) {
            ESP_LOGE(TAG, "mbedtls_x509_crt_parse of client cert returned -0x%04X", -ret);
            mbedtls_print_error_msg(ret);
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_MBEDTLS, -ret);
            return ESP_ERR_MBEDTLS_X509_CRT_PARSE_FAILED;
        }
    } else {
        ESP_LOGE(TAG, "Device certificate must be provided for TrustCustom Certs");
        return ESP_FAIL;
    }
#endif /* CONFIG_ATECC608A_TCUSTOM */
    ret = atca_mbedtls_pk_init(&tls->clientkey, 0);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to parse key from device");
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_MBEDTLS, -ret);
        mbedtls_print_error_msg(ret);
        return ESP_ERR_ESP_TLS_SE_FAILED;
    }

    ret = mbedtls_ssl_conf_own_cert(&tls->conf, &tls->clientcert, &tls->clientkey);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to configure client cert, returned -0x%04X", ret);
        mbedtls_print_error_msg(ret);
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_MBEDTLS, -ret);
        return ESP_ERR_ESP_TLS_SE_FAILED;
    }

    return ESP_OK;
}
#endif /* CONFIG_ESP_TLS_USE_SECURE_ELEMENT */

#ifdef CONFIG_ESP_TLS_USE_DS_PERIPHERAL
static esp_err_t esp_mbedtls_init_pk_ctx_for_ds(const void *pki)
{
    int ret = -1;
    /* initialize the mbedtls pk context with rsa context */
    mbedtls_rsa_context rsakey;
    mbedtls_rsa_init(&rsakey);
    if ((ret = mbedtls_pk_setup_rsa_alt(((const esp_tls_pki_t*)pki)->pk_key, &rsakey, NULL, esp_ds_rsa_sign,
                                        esp_ds_get_keylen )) != 0) {
        ESP_LOGE(TAG, "Error in mbedtls_pk_setup_rsa_alt, returned -0x%04X", -ret);
        mbedtls_print_error_msg(ret);
        ret = ESP_FAIL;
        goto exit;
    }
    ret = esp_ds_init_data_ctx(((const esp_tls_pki_t*)pki)->esp_ds_data);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize DS parameters from nvs");
        goto exit;
    }
    ESP_LOGD(TAG, "DS peripheral params initialized.");
exit:
    mbedtls_rsa_free(&rsakey);
    return ret;
}
#endif /* CONFIG_ESP_TLS_USE_DS_PERIPHERAL */
