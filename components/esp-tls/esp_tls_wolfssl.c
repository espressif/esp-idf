/*
 * SPDX-FileCopyrightText: 2019-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_tls_wolfssl.h"

#if defined(CONFIG_ESP_TLS_USING_MBEDTLS) || (CONFIG_ESP_TLS_USING_MBEDTLS != 0)
    #error  "mbedTLS enabled for wolfSSL file"
#endif

#ifdef CONFIG_ESP_TLS_USING_WOLFSSL

#include <wolfssl/wolfcrypt/settings.h>
#ifndef WOLFSSL_ESPIDF
    #warning "WOLFSSL_ESPIDF not defined! Check build system."
#endif
#include <wolfssl/openssl/x509.h>
#include <wolfssl/ssl.h>
#include <wolfssl/version.h>
#ifdef LIBWOLFSSL_VERSION_HEX
    #if LIBWOLFSSL_VERSION_HEX < 0x05007002
        #warning "wolfSSL 5.7.2 or newer is recommended."
        #warning "Managed Components recommended. See: https://components.espressif.com/components/wolfssl/wolfssl"
    #endif
#else
    #warning "Unknown wolfSSL version. Check build system."
#endif

#include <wolfssl/wolfcrypt/port/Espressif/esp_crt_bundle.h>

#include <http_parser.h>
#include "esp_tls_wolfssl.h"
#include "esp_tls_error_capture_internal.h"
#include <errno.h>
#include "esp_log.h"

#if defined(CONFIG_WOLFSSL_CERTIFICATE_BUNDLE) && \
    defined(CONFIG_WOLFSSL_CERTIFICATE_BUNDLE_DEFAULT_NONE) && \
    (CONFIG_WOLFSSL_CERTIFICATE_BUNDLE_DEFAULT_NONE == 0)

/* Only define the extern cert bundle data when using a bundle
 * other than the "none" selection.
 *
 * NOTICE: These wolfSSL certs are loaded from
 * [WOLFSSL_ROOT]/wolfcrypt/src/port/Espressif/esp_crt_bundle
 *   and *NOT*
 * [ESP-IDF root]/components/mbedtls/esp_crt_bundle */

extern const uint8_t x509_crt_imported_bundle_wolfssl_bin_start[]
                     asm("_binary_x509_crt_bundle_wolfssl_start");

extern const uint8_t x509_crt_imported_bundle_wolfssl_bin_end[]
                     asm("_binary_x509_crt_bundle_wolfssl_end");
#endif

/* Bundle debug may come from user_settings.h and/or sdkconfig.h */
#if defined(CONFIG_WOLFSSL_DEBUG_CERT_BUNDLE) || \
    defined(       WOLFSSL_DEBUG_CERT_BUNDLE)
    /* Only display certificate bundle debugging messages when enabled: */
    #define ESP_LOGCBI ESP_LOGI
    #define ESP_LOGCBW ESP_LOGW
#else
    /* Only display certificate bundle messages for most verbosee setting.
     * Note that the delays will likely cause TLS connection failures. */
    #define ESP_LOGCBI ESP_LOGV
    #define ESP_LOGCBW ESP_LOGV
    #define ESP_LOGCBV ESP_LOGV
#endif
#if defined(WOLFSSL_EXAMPLE_VERBOSITY)
    #define ESP_LOGXI ESP_LOGI
    #define ESP_LOGXW ESP_LOGW
    #define ESP_LOGXV ESP_LOGW
#else
    #define ESP_LOGXI ESP_LOGV
    #define ESP_LOGXI ESP_LOGV
    #define ESP_LOGXI ESP_LOGV
#endif

/* WOLFSSL_NO_CONF_COMPATIBILITY is not defined;
 * assumes compatibility with existing Espressif "conf" parameters. */

static const char *TAG = "esp-tls-wolfssl";

static unsigned char *global_cacert = NULL;
static unsigned int global_cacert_pem_bytes = 0;

inline void esp_wolfssl_net_init(esp_tls_t *tls)
{
    ESP_LOGV(TAG, "esp_wolfssl_net_init");
    if ((tls == NULL) || (tls->priv_ssl == NULL)){
        /* nothing to do */
    }
    else {
        if (wolfSSL_set_fd((WOLFSSL*)&tls->priv_ssl, -1) != SSL_SUCCESS) {
            ESP_LOGE(TAG, "Failed to initialize socket file descriptor");
        }
    }
}


/* Prototypes for the static functions */
static esp_err_t set_client_config(const char *hostname, size_t hostlen, esp_tls_cfg_t *cfg, esp_tls_t *tls);
static int ShowCiphers(WOLFSSL* ssl);

#if defined(CONFIG_ESP_TLS_PSK_VERIFICATION)
#include "freertos/semphr.h"
static SemaphoreHandle_t tls_conn_lock;
static inline unsigned int esp_wolfssl_psk_client_cb(WOLFSSL* ssl, const char* hint, char* identity,
        unsigned int id_max_len, unsigned char* key,unsigned int key_max_len);
static esp_err_t esp_wolfssl_set_cipher_list(WOLFSSL_CTX *ctx);
#ifdef WOLFSSL_TLS13
#define PSK_MAX_ID_LEN 128
#else
#define PSK_MAX_ID_LEN 64
#endif
#define PSK_MAX_KEY_LEN 64

static char psk_id_str[PSK_MAX_ID_LEN];
static uint8_t psk_key_array[PSK_MAX_KEY_LEN];
static uint8_t psk_key_max_len = 0;
#endif /* CONFIG_ESP_TLS_PSK_VERIFICATION */

#ifdef CONFIG_ESP_TLS_SERVER
static esp_err_t set_server_config(esp_tls_cfg_server_t *cfg, esp_tls_t *tls);
#endif /* CONFIG_ESP_TLS_SERVER */


/* This function shall return the error message when appropriate log level has been set otherwise this function shall do nothing */
static void wolfssl_print_error_msg(int error)
{
#if defined(DEBUG_WOLFSSL) || (CONFIG_LOG_DEFAULT_LEVEL_DEBUG || CONFIG_LOG_DEFAULT_LEVEL_VERBOSE)
    static char error_buf[100];
#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
    ESP_LOGE(TAG, "(%d) : %s", error, ERR_error_string(error, error_buf));
#else
    ESP_LOGE(TAG, "Error: (%d)", error);
#endif
#endif
}

typedef enum x509_file_type {
    FILE_TYPE_CA_CERT = 0, /* CA certificate to authenticate entity at other end */
    FILE_TYPE_SELF_CERT, /* Self certificate of the entity */
    FILE_TYPE_SELF_KEY, /* Private key in the self cert-key pair */
} x509_file_type_t;

/* Error type conversion utility so that esp-tls read/write API to return negative number on error */
static inline ssize_t esp_tls_convert_wolfssl_err_to_ssize(int wolfssl_error)
{
    switch (wolfssl_error) {
        case WOLFSSL_ERROR_WANT_READ:
            return ESP_TLS_ERR_SSL_WANT_READ;
        case WOLFSSL_ERROR_WANT_WRITE:
            return ESP_TLS_ERR_SSL_WANT_WRITE;
        default:
            // Make sure we return a negative number
            return wolfssl_error>0 ? -wolfssl_error: wolfssl_error;
    }
}

/* Checks whether the certificate provided is in pem format or not */
static esp_err_t esp_load_wolfssl_verify_buffer(esp_tls_t *tls, const unsigned char *cert_buf, unsigned int cert_len, x509_file_type_t type, int *err_ret)
{
    int wolf_fileformat = WOLFSSL_FILETYPE_DEFAULT;
    if (type == FILE_TYPE_SELF_KEY) {
        if (cert_buf[cert_len - 1] == '\0' && strstr( (const char *) cert_buf, "-----BEGIN " )) {
            wolf_fileformat = WOLFSSL_FILETYPE_PEM;
        } else {
            wolf_fileformat = WOLFSSL_FILETYPE_ASN1;
        }
        if ((*err_ret = wolfSSL_CTX_use_PrivateKey_buffer( (WOLFSSL_CTX *)tls->priv_ctx, cert_buf, cert_len, wolf_fileformat)) == WOLFSSL_SUCCESS) {
            return ESP_OK;
        }
        return ESP_FAIL;
    } else {
        if (cert_buf[cert_len - 1] == '\0' && strstr( (const char *) cert_buf, "-----BEGIN CERTIFICATE-----" )) {
            wolf_fileformat = WOLFSSL_FILETYPE_PEM;
        } else {
            wolf_fileformat = WOLFSSL_FILETYPE_ASN1;
        }
        if (type == FILE_TYPE_SELF_CERT) {
            if ((*err_ret = wolfSSL_CTX_use_certificate_chain_buffer_format( (WOLFSSL_CTX *)tls->priv_ctx, cert_buf, cert_len, wolf_fileformat)) == WOLFSSL_SUCCESS) {
                return ESP_OK;
            }
            return ESP_FAIL;
        } else if (type == FILE_TYPE_CA_CERT) {
            if ((*err_ret = wolfSSL_CTX_load_verify_buffer( (WOLFSSL_CTX *)tls->priv_ctx, cert_buf, cert_len, wolf_fileformat)) == WOLFSSL_SUCCESS) {
                return ESP_OK;
            }
            return ESP_FAIL;
        } else {
            /* Wrong file type provided */
            return ESP_FAIL;
        }
    }
}

void *esp_wolfssl_get_ssl_context(esp_tls_t *tls)
{
    if (tls == NULL) {
        ESP_LOGE(TAG, "Invalid arguments");
        return NULL;
    }
    return (void*)tls->priv_ssl;
}

static int _is_wolfssl_init = 0;

/* ESP-IDF v5.3 introduced a new parameter: server_params */
#if defined(ESP_IDF_VERSION) && (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 3, 0))
    esp_err_t esp_create_wolfssl_handle(const char *hostname, size_t hostlen, const void *cfg, esp_tls_t *tls, void *server_params)
#else
    esp_err_t esp_create_wolfssl_handle(const char *hostname, size_t hostlen, const void *cfg, esp_tls_t *tls)
#endif
{
#ifdef CONFIG_ESP_DEBUG_WOLFSSL
    wolfSSL_Debugging_ON();
#endif

    assert(cfg != NULL);
    assert(tls != NULL);

    esp_err_t esp_ret = ESP_FAIL;
    int ret = WOLFSSL_SUCCESS;


    if (_is_wolfssl_init == 0) {
        ESP_LOGXI(TAG, "wolfSSL_Init");
        ret = wolfSSL_Init();
      //  _is_wolfssl_init = 1;
    }
    else {
        ESP_LOGXI(TAG, "skipping wolfSSL_Init");
    }

    if (ret != WOLFSSL_SUCCESS) {
        ESP_LOGE(TAG, "Init wolfSSL failed: 0x%04X", ret);
        wolfssl_print_error_msg(ret);
        int err = wolfSSL_get_error( (WOLFSSL *)tls->priv_ssl, ret);
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_WOLFSSL, err);
        goto exit;
    }

    if (tls->role == ESP_TLS_CLIENT) {
        ESP_LOGXI(TAG, "Set set_client_config");
        esp_ret = set_client_config(hostname, hostlen, (esp_tls_cfg_t *)cfg, tls);
        ESP_LOGXI(TAG, "Set set_client_config done!");
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set client configurations, [0x%04X] (%s)", esp_ret, esp_err_to_name(esp_ret));
            goto exit;
        }
    } else if (tls->role == ESP_TLS_SERVER) {
#ifdef CONFIG_ESP_TLS_SERVER
        esp_ret = set_server_config((esp_tls_cfg_server_t *) cfg, tls);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set server configurations, [0x%04X] (%s)", esp_ret, esp_err_to_name(esp_ret));
            goto exit;
        }
#else
        ESP_LOGE(TAG, "ESP_TLS_SERVER Not enabled in menuconfig");
        goto exit;
#endif
    }
    else {
        ESP_LOGE(TAG, "tls->role is not valid");
        goto exit;
    }

    return ESP_OK;
exit:
    ESP_LOGE(TAG, "Error exit");
    esp_wolfssl_cleanup(tls);
    return esp_ret;
}

int my_verify_callback(int preverify_ok, WOLFSSL_X509_STORE_CTX* store) {
    // You can add custom verification logic here
    // For now, just return the result passed to the callback
    ESP_LOGW(TAG, "my_verify_callback");
    return preverify_ok;
}

#ifndef WOLFSSL_NO_CONF_COMPATIBILITY
/* Some functions such as those used in esp-tls Certificate Bundles
 * expect a `conf` property of the tls. Keep them in sync with the
 * actual values (tls->priv_ctx) and (tls->priv_ssl):
 *
 * example:
 *     tls->sync((void*)&tls);
 *
 * In all cases, it is preferred to use the non-conf-prefixed values.
 *
 * Once all instances of compatibility have been updated, this feature
 * will likely be permanently removed. */
static void sync_tls_conf_values(struct esp_tls *tls) {
    if (tls) {
        tls->conf.priv_ctx = tls->priv_ctx;
        tls->conf.priv_ssl = tls->priv_ssl;
    }
}
#endif

static esp_err_t set_client_config(const char *hostname, size_t hostlen, esp_tls_cfg_t *cfg, esp_tls_t *tls)
{
    int ret = WOLFSSL_FAILURE;

    assert(cfg != NULL);
    assert(tls != NULL);

#if defined(CONFIG_WOLFSSL_ALLOW_TLS13) && defined(CONFIG_WOLFSSL_ALLOW_TLS12)
    ESP_LOGXI(TAG, "Set Client Config for ANY TLS version");
    tls->priv_ctx = (void *)wolfSSL_CTX_new(wolfSSLv23_client_method());
#elif defined(CONFIG_WOLFSSL_ALLOW_TLS13) & defined(WOLFSSL_TLS13)
    ESP_LOGXI(TAG, "Set Client Config for TLS1.3 Only");
    tls->priv_ctx = (void *)wolfSSL_CTX_new(wolfTLSv1_3_client_method());
#elif defined(CONFIG_WOLFSSL_ALLOW_TLS13)
    #error "CONFIG_WOLFSSL_ALLOW_TLS13 configured without WOLFSSL_TLS13?"
#elif defined(CONFIG_WOLFSSL_ALLOW_TLS12)
    ESP_LOGXI(TAG, "Set Client Config for TLS1.2 Only");
    tls->priv_ctx = (void *)wolfSSL_CTX_new(wolfTLSv1_2_client_method());
#else
    ESP_LOGW(TAG, "No TLS enabled!");
    #warning "No TLS enabled!"
#endif

#if defined(CONFIG_WOLFSSL_ALLOW_TLS13) || defined(CONFIG_WOLFSSL_ALLOW_TLS12)
    if (tls == NULL) {
        ESP_LOGE(TAG, "Failed to create wolfSSL TLS Client Method");
        return ESP_ERR_INVALID_STATE;
    }
    else {
    #ifndef WOLFSSL_NO_CONF_COMPATIBILITY
        tls->conf.priv_ctx = tls->priv_ctx;
        tls->conf.priv_ssl = tls->priv_ssl;
        tls->sync = sync_tls_conf_values;
    #endif
    }

#if defined(CONFIG_WOLFSSL_CERTIFICATE_BUNDLE) && \
    defined(CONFIG_WOLFSSL_CERTIFICATE_BUNDLE_DEFAULT_NONE) && \
    (CONFIG_WOLFSSL_CERTIFICATE_BUNDLE_DEFAULT_NONE == 0)
    esp_crt_bundle_init(x509_crt_imported_bundle_wolfssl_bin_start,
                        x509_crt_imported_bundle_wolfssl_bin_end - x509_crt_imported_bundle_wolfssl_bin_start,
                        tls);
    ESP_LOGXI(TAG, "No Certificate Bundle Selected.");
#endif /* Some certificate bundle other than "none" */
#endif /* CONFIG_WOLFSSL_ALLOW_[TLS12 or TLS13] */

    if (!tls->priv_ctx) {
        ESP_LOGE(TAG, "Set wolfSSL ctx failed");
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_WOLFSSL, ret);
        return ESP_ERR_WOLFSSL_CTX_SETUP_FAILED;
    }

    /* Optionally call crt_bundle_attach */
    if (cfg->crt_bundle_attach != NULL) {
#ifdef CONFIG_WOLFSSL_CERTIFICATE_BUNDLE
        ESP_LOGD(TAG, "Use certificate bundle");
        wolfSSL_CTX_set_verify( (WOLFSSL_CTX *)tls->priv_ctx, WOLFSSL_VERIFY_PEER, my_verify_callback);
        cfg->crt_bundle_attach(&tls->conf); /* callback is also set here */
    #ifndef WOLFSSL_NO_CONF_COMPATIBILITY
        tls->conf.priv_ctx = tls->priv_ctx;
        tls->conf.priv_ssl = tls->priv_ssl;
    #endif

#else /* CONFIG_WOLFSSL_CERTIFICATE_BUNDLE */
        ESP_LOGE(TAG, "use_crt_bundle configured but not enabled in menuconfig:"
                      "Please enable CONFIG_WOLFSSL_CERTIFICATE_BUNDLE option");
        return ESP_ERR_INVALID_STATE;
#endif
        /* cfg->crt_bundle_attach != NULL */
    } else if (cfg->use_global_ca_store == true) {
        WOLFSSL_MSG("Using Global CA Store in esp_tls_wolfssl");
        if ((esp_load_wolfssl_verify_buffer(tls, global_cacert, global_cacert_pem_bytes, FILE_TYPE_CA_CERT, &ret)) != ESP_OK) {
            int err = wolfSSL_get_error( (WOLFSSL *)tls->priv_ssl, ret);
            ESP_LOGE(TAG, "Error in loading certificate verify buffer, returned %d, error code: %d", ret, err);
            wolfssl_print_error_msg(err);
            return ESP_ERR_WOLFSSL_CERT_VERIFY_SETUP_FAILED;
        } /* esp_load_wolfssl_verify_buffer */
        wolfSSL_CTX_set_verify( (WOLFSSL_CTX *)tls->priv_ctx, WOLFSSL_VERIFY_PEER, NULL);
    } else if (cfg->cacert_buf != NULL) {
        WOLFSSL_MSG("set_client_config found cert_buf");
        if ((esp_load_wolfssl_verify_buffer(tls, cfg->cacert_buf, cfg->cacert_bytes, FILE_TYPE_CA_CERT, &ret)) != ESP_OK) {
            int err = wolfSSL_get_error( (WOLFSSL *)tls->priv_ssl, ret);
            ESP_LOGE(TAG, "Error in loading certificate verify buffer, returned %d, error code: %d", ret, err);
            wolfssl_print_error_msg(err);
            return ESP_ERR_WOLFSSL_CERT_VERIFY_SETUP_FAILED;
        }
        wolfSSL_CTX_set_verify( (WOLFSSL_CTX *)tls->priv_ctx, WOLFSSL_VERIFY_PEER, NULL);
        /* end  (cfg->cacert_buf != NULL) */
    }
#if defined(CONFIG_ESP_TLS_PSK_VERIFICATION)
    else if (cfg->psk_hint_key) {
        /*** PSK encryption mode is configured only if no certificate supplied and psk pointer not null ***/
        if(cfg->psk_hint_key->key == NULL || cfg->psk_hint_key->hint == NULL || cfg->psk_hint_key->key_size <= 0) {
            ESP_LOGE(TAG, "Please provide appropriate key, keysize and hint to use PSK");
            return ESP_FAIL;
        }
        /* mutex is given back when call back function executes or in case of failure (at cleanup) */
        if ((xSemaphoreTake(tls_conn_lock, 1000/portTICK_PERIOD_MS) != pdTRUE)) {
            ESP_LOGE(TAG, "tls_conn_lock could not be obtained in specified time");
            return -1;
        }
        ESP_LOGXI(TAG, "setting psk configurations");
        if((cfg->psk_hint_key->key_size > PSK_MAX_KEY_LEN) || (strlen(cfg->psk_hint_key->hint) > PSK_MAX_ID_LEN)) {
            ESP_LOGE(TAG, "psk key length should be <= %d and identity hint length should be <= %d", PSK_MAX_KEY_LEN, PSK_MAX_ID_LEN);
            return ESP_ERR_INVALID_ARG;
        }
        psk_key_max_len = cfg->psk_hint_key->key_size;
        memset(psk_key_array, 0, sizeof(psk_key_array));
        memset(psk_id_str, 0, sizeof(psk_id_str));
        memcpy(psk_key_array, cfg->psk_hint_key->key, psk_key_max_len);
        memcpy(psk_id_str, cfg->psk_hint_key->hint, strlen(cfg->psk_hint_key->hint));
        wolfSSL_CTX_set_psk_client_callback( (WOLFSSL_CTX *)tls->priv_ctx, esp_wolfssl_psk_client_cb);
        if(esp_wolfssl_set_cipher_list( (WOLFSSL_CTX *)tls->priv_ctx) != ESP_OK) {
            ESP_LOGE(TAG, "error in setting cipher-list");
            return ESP_FAIL;
#else
     else if (1) {
        ESP_LOGE(TAG, "psk_hint_key configured but not enabled in menuconfig: Please enable ESP_TLS_PSK_VERIFICATION option");
        return ESP_ERR_INVALID_STATE;
#endif /* end (cfg->psk_hint_key) */
    } else {
        /* Not using Global CA Store, the cfg->cacert_buf is NULL, no PSK Hint Key */
#ifdef CONFIG_ESP_TLS_SKIP_SERVER_CERT_VERIFY
        wolfSSL_CTX_set_verify( (WOLFSSL_CTX *)tls->priv_ctx, WOLFSSL_VERIFY_NONE, NULL); /* */
#else
        wolfSSL_CTX_set_verify( (WOLFSSL_CTX *)tls->priv_ctx, WOLFSSL_VERIFY_PEER, NULL);
        ESP_LOGW(TAG, "No server verification option set in esp_tls_cfg_t structure. Check esp_tls API reference");
        return ESP_ERR_WOLFSSL_SSL_SETUP_FAILED; /* TODO: really? */
#endif
    }

    if (cfg->clientcert_buf != NULL && cfg->clientkey_buf != NULL) {
        if ((esp_load_wolfssl_verify_buffer(tls,cfg->clientcert_buf, cfg->clientcert_bytes, FILE_TYPE_SELF_CERT, &ret)) != ESP_OK) {
            int err = wolfSSL_get_error( (WOLFSSL *)tls->priv_ssl, ret);
            ESP_LOGE(TAG, "Error in loading certificate verify buffer, returned %d, error code: %d", ret, err);
            wolfssl_print_error_msg(err);
            return ESP_ERR_WOLFSSL_CERT_VERIFY_SETUP_FAILED;
        }
        if ((esp_load_wolfssl_verify_buffer(tls,cfg->clientkey_buf, cfg->clientkey_bytes, FILE_TYPE_SELF_KEY, &ret)) != ESP_OK) {
            int err = wolfSSL_get_error( (WOLFSSL *)tls->priv_ssl, ret);
            ESP_LOGE(TAG, "Error in loading private key verify buffer, returned %d, error code: %d", ret, err);
            wolfssl_print_error_msg(err);
            return ESP_ERR_WOLFSSL_CERT_VERIFY_SETUP_FAILED;
        }
    } else if (cfg->clientcert_buf != NULL || cfg->clientkey_buf != NULL) {
        ESP_LOGE(TAG, "You have to provide both clientcert_buf and clientkey_buf for mutual authentication");
        return ESP_FAIL;
    }

    tls->priv_ssl =(void *)wolfSSL_new( (WOLFSSL_CTX *)tls->priv_ctx);
    tls->conf.priv_ssl = tls->priv_ssl;
    if (!tls->priv_ssl) {
        ESP_LOGE(TAG, "Create wolfSSL failed");
        int err = wolfSSL_get_error( (WOLFSSL *)tls->priv_ssl, ret);
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_WOLFSSL, err);
        return ESP_ERR_WOLFSSL_SSL_SETUP_FAILED;
    }

    /* Reminder: name check occurs after wolfSSL priv_ctx and priv_ssl init. */
    if (!cfg->skip_common_name) {
        char *use_host = NULL;
        if (cfg->common_name != NULL) {
            use_host = strdup(cfg->common_name);
        } else {
            use_host = strndup(hostname, hostlen);
        }
        if (use_host == NULL) {
            return ESP_ERR_NO_MEM;
        }
        else {
            ESP_LOGXI(TAG, "Using host for wolfSSL Check Domain: %s", use_host);
        }
        /* Hostname set here should match CN in server certificate */
        if ((ret = (wolfSSL_check_domain_name( (WOLFSSL *)tls->priv_ssl, use_host))) != WOLFSSL_SUCCESS) {
            int err = wolfSSL_get_error( (WOLFSSL *)tls->priv_ssl, ret);
            ESP_LOGE(TAG, "wolfSSL_check_domain_name returned %d, error code: %d", ret, err);
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_WOLFSSL, err);
            free(use_host);
            return ESP_ERR_WOLFSSL_SSL_SET_HOSTNAME_FAILED;
        }
        /* Mimic the semantics of mbedtls_ssl_set_hostname() */
#ifdef HAVE_SNI
        if ((ret = wolfSSL_CTX_UseSNI(tls->priv_ctx, WOLFSSL_SNI_HOST_NAME, use_host, strlen(use_host))) != WOLFSSL_SUCCESS) {
            ESP_LOGE(TAG, "wolfSSL_CTX_UseSNI failed, returned %d", ret);
            return ESP_ERR_WOLFSSL_SSL_SET_HOSTNAME_FAILED;
        }
#endif
        free(use_host);
    }

    if (cfg->alpn_protos) {
#if defined(CONFIG_WOLFSSL_HAVE_ALPN) && defined(HAVE_ALPN)
        char **alpn_list = (char **)cfg->alpn_protos;
        for (; *alpn_list != NULL; alpn_list ++) {
            ESP_LOGD(TAG, "alpn protocol is %s", *alpn_list);
            if ((ret = wolfSSL_UseALPN( (WOLFSSL *)tls->priv_ssl, *alpn_list, strlen(*alpn_list), WOLFSSL_ALPN_FAILED_ON_MISMATCH)) != WOLFSSL_SUCCESS) {
                int err = wolfSSL_get_error( (WOLFSSL *)tls->priv_ssl, ret);
                ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_WOLFSSL, err);
                ESP_LOGE(TAG, "wolfSSL UseALPN failed, returned %d, error code: %d", ret, err);
                wolfssl_print_error_msg(err);
                return ESP_ERR_WOLFSSL_SSL_CONF_ALPN_PROTOCOLS_FAILED;
            }
        }
#else
    ESP_LOGE(TAG, "CONFIG_WOLFSSL_HAVE_ALPN not enabled in menuconfig");
    return ESP_FAIL;
#endif /* CONFIG_WOLFSSL_HAVE_ALPN */
    }

#ifdef CONFIG_WOLFSSL_HAVE_OCSP
    int ocsp_options = 0;
#ifdef ESP_TLS_OCSP_CHECKALL
    ocsp_options |= WOLFSSL_OCSP_CHECKALL;
#endif
    /* enable OCSP certificate status check for this TLS context */
    if ((ret = wolfSSL_CTX_EnableOCSP((WOLFSSL_CTX *)tls->priv_ctx, ocsp_options)) != WOLFSSL_SUCCESS) {
        ESP_LOGE(TAG, "wolfSSL_CTX_EnableOCSP failed, returned %d", ret);
        return ESP_ERR_WOLFSSL_CTX_SETUP_FAILED;
    }
    /* enable OCSP stapling for this TLS context */
    if ((ret = wolfSSL_CTX_EnableOCSPStapling((WOLFSSL_CTX *)tls->priv_ctx )) != WOLFSSL_SUCCESS) {
        ESP_LOGE(TAG, "wolfSSL_CTX_EnableOCSPStapling failed, returned %d", ret);
        return ESP_ERR_WOLFSSL_CTX_SETUP_FAILED;
    }
    /* set option to use OCSP v1 stapling with nounce extension */
    if ((ret = wolfSSL_UseOCSPStapling((WOLFSSL *)tls->priv_ssl, WOLFSSL_CSR_OCSP, WOLFSSL_CSR_OCSP_USE_NONCE)) != WOLFSSL_SUCCESS) {
        ESP_LOGE(TAG, "wolfSSL_UseOCSPStapling failed, returned %d", ret);
        return ESP_ERR_WOLFSSL_SSL_SETUP_FAILED;
    }
#endif /* CONFIG_WOLFSSL_HAVE_OCSP */

    ret = wolfSSL_set_fd((WOLFSSL *)tls->priv_ssl, tls->sockfd);
    if (ret == WOLFSSL_SUCCESS) {
        ESP_LOGXI(TAG, "Attach wolfSSL to the socket success!");
    }
    else {
        ESP_LOGE(TAG, "ERROR: failed wolfSSL_set_fd. Error: %d\n", ret);
        return ESP_FAIL;
    }
    return ESP_OK;
}

#ifdef CONFIG_ESP_TLS_SERVER
static esp_err_t set_server_config(esp_tls_cfg_server_t *cfg, esp_tls_t *tls)
{
    int ret = WOLFSSL_FAILURE;

#if defined(CONFIG_WOLFSSL_ALLOW_TLS13) && defined(CONFIG_WOLFSSL_ALLOW_TLS12)
    WOLFSSL_MSG("Set Server Config for any TLS version");
    tls->priv_ctx = (void *)wolfSSL_CTX_new(wolfSSLv23_server_method());
#elif defined(CONFIG_WOLFSSL_ALLOW_TLS13)
    WOLFSSL_MSG("Set Server Config for TLS1.3 Only");
    tls->priv_ctx = (void *)wolfSSL_CTX_new(wolfTLSv1_3_server_method());
#elif defined(CONFIG_WOLFSSL_ALLOW_TLS12)
    WOLFSSL_MSG("Set Server Config for TLS1.2 Only");
    tls->priv_ctx = (void *)wolfSSL_CTX_new(wolfTLSv1_2_server_method());
#else
    #warning "No TLS enabled!"
#endif /* TLS Version */

#ifndef WOLFSSL_NO_CONF_COMPATIBILITY
    tls->sync((void*)&tls);
#endif
    if (!tls->priv_ctx) {
        ESP_LOGE(TAG, "Set wolfSSL ctx failed");
        return ESP_ERR_WOLFSSL_CTX_SETUP_FAILED;
    }

    if (cfg->cacert_buf != NULL) {
        if ((esp_load_wolfssl_verify_buffer(tls,cfg->cacert_buf, cfg->cacert_bytes, FILE_TYPE_CA_CERT, &ret)) != ESP_OK) {
            int err = wolfSSL_get_error( (WOLFSSL *)tls->priv_ssl, ret);
            ESP_LOGE(TAG, "Error in loading certificate verify buffer, returned %d, error code: %d", ret, err);
            wolfssl_print_error_msg(err);
            return ESP_ERR_WOLFSSL_CERT_VERIFY_SETUP_FAILED;
        }
        wolfSSL_CTX_set_verify( (WOLFSSL_CTX *)tls->priv_ctx, WOLFSSL_VERIFY_PEER | WOLFSSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
        ESP_LOGD(TAG," Verify Client for Mutual Auth");
    } else {
        ESP_LOGD(TAG," Not verifying Client ");
        wolfSSL_CTX_set_verify( (WOLFSSL_CTX *)tls->priv_ctx, WOLFSSL_VERIFY_NONE, NULL);
    }

    if (cfg->servercert_buf != NULL && cfg->serverkey_buf != NULL) {
        if ((esp_load_wolfssl_verify_buffer(tls,cfg->servercert_buf, cfg->servercert_bytes, FILE_TYPE_SELF_CERT, &ret)) != ESP_OK) {
            int err = wolfSSL_get_error( (WOLFSSL *)tls->priv_ssl, ret);
            ESP_LOGE(TAG, "Error in loading certificate verify buffer, returned %d, error code: %d", ret, err);
            wolfssl_print_error_msg(err);
            return ESP_ERR_WOLFSSL_CERT_VERIFY_SETUP_FAILED;
        }
        if ((esp_load_wolfssl_verify_buffer(tls,cfg->serverkey_buf, cfg->serverkey_bytes, FILE_TYPE_SELF_KEY, &ret)) != ESP_OK) {
            int err = wolfSSL_get_error( (WOLFSSL *)tls->priv_ssl, ret);
            ESP_LOGE(TAG, "Error in loading private key verify buffer, returned %d, error code: %d", ret, err);
            wolfssl_print_error_msg(err);
            return ESP_ERR_WOLFSSL_CERT_VERIFY_SETUP_FAILED;
        }
    } else {
        ESP_LOGE(TAG, "You have to provide both servercert_buf and serverkey_buf for https_server");
        return ESP_FAIL;
    }

    tls->priv_ssl =(void *)wolfSSL_new( (WOLFSSL_CTX *)tls->priv_ctx);
    if (!tls->priv_ssl) {
        ESP_LOGE(TAG, "Create wolfSSL failed");
        return ESP_ERR_WOLFSSL_SSL_SETUP_FAILED;
    }

    wolfSSL_set_fd((WOLFSSL *)tls->priv_ssl, tls->sockfd);
    return ESP_OK;
}
#endif
int ShowCiphers(WOLFSSL* ssl)
{
    #define CLIENT_TLS_MAX_CIPHER_LENGTH 4096
    char ciphers[CLIENT_TLS_MAX_CIPHER_LENGTH];
    const char* cipher_used;
    int ret = 0;

    if (ssl == NULL) {
        ESP_LOGXI(TAG, "WOLFSSL* ssl is NULL, so no cipher in use");
        ret = wolfSSL_get_ciphers(ciphers, (int)sizeof(ciphers));
        if (ret == WOLFSSL_SUCCESS) {
            for (int i = 0; i < CLIENT_TLS_MAX_CIPHER_LENGTH; i++) {
                if (ciphers[i] == ':') {
                    ciphers[i] = '\n';
                }
            }
            ESP_LOGXI(TAG, "Available Ciphers:\n%s\n", ciphers);
        }
        else {
            ESP_LOGE(TAG, "Failed to call wolfSSL_get_ciphers. Error %d", ret);
        }
    }
    else {
        cipher_used = wolfSSL_get_cipher_name(ssl);
        ESP_LOGXI(TAG, "WOLFSSL* ssl using %s", cipher_used);
    }

    return ret;
}
int esp_wolfssl_handshake(esp_tls_t *tls, const esp_tls_cfg_t *cfg)
{
    int ret;

    #ifdef DEBUG_WOLFSSL
        wolfSSL_Debugging_ON();
    #endif
    ESP_LOGCBI(TAG, "Begin esp-tls esp_wolfssl_handshake");
    ret = wolfSSL_connect( (WOLFSSL *)tls->priv_ssl);
#ifndef WOLFSSL_NO_CONF_COMPATIBILITY
    tls->conf.priv_ssl = tls->priv_ssl;
#endif
    if (ret == WOLFSSL_SUCCESS) {
        tls->conn_state = ESP_TLS_DONE;
        ShowCiphers((WOLFSSL *)tls->priv_ssl);
        return 1;
    } else {
        int err = wolfSSL_get_error( (WOLFSSL *)tls->priv_ssl, ret);
        if (err != WOLFSSL_ERROR_WANT_READ && err != WOLFSSL_ERROR_WANT_WRITE) {
            ESP_LOGE(TAG, "wolfSSL_connect returned %d, error code: %d", ret, err);
            wolfssl_print_error_msg(err);
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_WOLFSSL, err);
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_ESP, ESP_ERR_WOLFSSL_SSL_HANDSHAKE_FAILED);
            if (cfg->crt_bundle_attach != NULL || cfg->cacert_buf != NULL || cfg->use_global_ca_store == true) {
                /* This is to check whether handshake failed due to invalid certificate */
                esp_wolfssl_verify_certificate(tls);
            }
            tls->conn_state = ESP_TLS_FAIL;
            return -1;
        }
        /* Irrespective of blocking or non-blocking I/O, we return on getting wolfSSL_want_read
        or wolfSSL_want_write during handshake */
        return 0;
    }
}

ssize_t esp_wolfssl_read(esp_tls_t *tls, char *data, size_t datalen)
{
    ssize_t ret = wolfSSL_read( (WOLFSSL *)tls->priv_ssl, (unsigned char *)data, datalen);
    if (ret < 0) {
        int err = wolfSSL_get_error( (WOLFSSL *)tls->priv_ssl, ret);
        /* peer sent close notify */
        if (err == WOLFSSL_ERROR_ZERO_RETURN) {
            return 0;
        }

        if (ret != WOLFSSL_ERROR_WANT_READ && ret != WOLFSSL_ERROR_WANT_WRITE) {
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_WOLFSSL, -ret);
            ESP_LOGE(TAG, "read error :%d:", ret);
            wolfssl_print_error_msg(ret);
        }
        return esp_tls_convert_wolfssl_err_to_ssize(ret);
    }
    return ret;
}

ssize_t esp_wolfssl_write(esp_tls_t *tls, const char *data, size_t datalen)
{
    ssize_t ret = wolfSSL_write( (WOLFSSL *)tls->priv_ssl, (unsigned char *) data, datalen);
    if (ret <= 0) {
        int err = wolfSSL_get_error( (WOLFSSL *)tls->priv_ssl, ret);
        if (err != WOLFSSL_ERROR_WANT_READ  && err != WOLFSSL_ERROR_WANT_WRITE) {
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_WOLFSSL, -err);
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_ESP, ESP_ERR_WOLFSSL_SSL_WRITE_FAILED);
            ESP_LOGE(TAG, "write error :%d:", err);
            wolfssl_print_error_msg(err);
        }
        return esp_tls_convert_wolfssl_err_to_ssize(ret);
    }
    return ret;
}

void esp_wolfssl_verify_certificate(esp_tls_t *tls)
{
    int flags;
    if ((flags = wolfSSL_get_verify_result( (WOLFSSL *)tls->priv_ssl)) != WOLFSSL_SUCCESS) {
        ESP_LOGE(TAG, "Failed to verify peer certificate , returned %d", flags);
#ifdef WOLFSSL_ALT_CERT_CHAINS
        ESP_LOGW(TAG, "WOLFSSL_ALT_CERT_CHAINS is defined");
#else
        /* wolfSSL is considerably more strict with certificates by default. */
    #if defined(CONFIG_WOLFSSL_CERTIFICATE_BUNDLE) && CONFIG_WOLFSSL_CERTIFICATE_BUNDLE
        ESP_LOGW(TAG, "Consider changing the certificates loaded and/or defining WOLFSSL_ALT_CERT_CHAINS to relax certificate check. (1)");
    #else
        ESP_LOGW(TAG, "Consider enabling CONFIG_WOLFSSL_CERTIFICATE_BUNDLE.");
    #endif
#endif
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_WOLFSSL_CERT_FLAGS, flags);
    } else {
        ESP_LOGXI(TAG, "Certificate verified.");
    }
}

ssize_t esp_wolfssl_get_bytes_avail(esp_tls_t *tls)
{
    if (!tls) {
        ESP_LOGE(TAG, "empty arg passed to esp_tls_get_bytes_avail()");
        return ESP_FAIL;
    }
    return wolfSSL_pending( (WOLFSSL *)tls->priv_ssl);
}

void esp_wolfssl_conn_delete(esp_tls_t *tls)
{
    ESP_LOGCBW(TAG, "esp_wolfssl_conn_delete");
    if (tls != NULL) {
        esp_wolfssl_cleanup(tls);
    }
}

void esp_wolfssl_cleanup(esp_tls_t *tls)
{
    ESP_LOGCBW(TAG, "esp_wolfssl_cleanup");
    if (!tls) {
        return;
    }
#ifdef CONFIG_ESP_TLS_PSK_VERIFICATION
    xSemaphoreGive(tls_conn_lock);
#endif /* CONFIG_ESP_TLS_PSK_VERIFICATION */

#ifndef WOLFSSL_NO_CONF_COMPATIBILITY
	if ((tls->priv_ssl != tls->conf.priv_ssl) && (tls->conf.priv_ssl != NULL)) {
    	/* priv_ssl may have been already cleanup up when connection closed. */
    	ESP_LOGE(TAG, "tls->priv_ssl != tls->conf.priv_ssl inconsistency!");
	}
	if (tls->priv_ctx != tls->conf.priv_ctx) {
    	ESP_LOGE(TAG, "tls->priv_ctx != tls->conf.priv_ctx inconsistency!");
	}
#endif
    wolfSSL_shutdown( (WOLFSSL *)tls->priv_ssl);
    wolfSSL_free( (WOLFSSL *)tls->priv_ssl);
    tls->priv_ssl = NULL;
    wolfSSL_CTX_free( (WOLFSSL_CTX *)tls->priv_ctx);
    tls->priv_ctx = NULL;

#ifndef WOLFSSL_NO_CONF_COMPATIBILITY
    /* The same addresses, but for clarity: */
    tls->conf.priv_ssl = NULL;
    tls->conf.priv_ctx = NULL;
#endif
#ifdef CONFIG_WOLFSSL_CERTIFICATE_BUNDLE
    wolfSSL_bundle_cleanup();
#endif
    wolfSSL_Cleanup();
}

/**
 * @brief       Create TLS/SSL server session
 */
int esp_wolfssl_server_session_create(esp_tls_cfg_server_t *cfg, int sockfd, esp_tls_t *tls)
{
    if (tls == NULL || cfg == NULL) {
        return -1;
    }
    tls->role = ESP_TLS_SERVER;
    tls->sockfd = sockfd;
    esp_tls_server_params_t server_params = {};
    // server_params.set_server_cfg = &set_server_config;
    // TODO

#if defined(ESP_IDF_VERSION) && (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 3, 0))
    esp_err_t esp_ret = esp_create_wolfssl_handle(NULL, 0, cfg, tls, &server_params);
#else
    esp_err_t esp_ret = esp_create_wolfssl_handle(NULL, 0, cfg, tls);
#endif
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "create_ssl_handle failed, [0x%04X] (%s)", esp_ret, esp_err_to_name(esp_ret));
        ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_ESP, esp_ret);
        tls->conn_state = ESP_TLS_FAIL;
        return -1;
    }
    tls->read = esp_wolfssl_read;
    tls->write = esp_wolfssl_write;
    int ret;
    while ((ret = wolfSSL_accept((WOLFSSL *)tls->priv_ssl)) != WOLFSSL_SUCCESS) {
        int err = wolfSSL_get_error((WOLFSSL *)tls->priv_ssl, ret);
        if (err != WOLFSSL_ERROR_WANT_READ && ret != WOLFSSL_ERROR_WANT_WRITE) {
            ESP_INT_EVENT_TRACKER_CAPTURE(tls->error_handle, ESP_TLS_ERR_TYPE_WOLFSSL, err);
            ESP_LOGE(TAG, "wolfSSL_accept returned %d, error code: %d", ret, err);
            wolfssl_print_error_msg(err);
            tls->conn_state = ESP_TLS_FAIL;
            return -1;
        }
    }
    return 0;
}

/**
 * @brief       Close the server side TLS/SSL connection and free any allocated resources.
 */
int esp_wolfssl_server_session_delete(esp_tls_t *tls)
{
    if (tls != NULL) {
        esp_wolfssl_cleanup(tls);
        esp_tls_internal_event_tracker_destroy(tls->error_handle);
        free(tls);
    }
    return ESP_OK;
}

esp_err_t esp_wolfssl_init_global_ca_store(void)
{
    /* This function is just to provide consistency between function calls of esp_tls.h and wolfssl */
    ESP_LOGW(TAG, "NOT implemented: esp_wolfssl_init_global_ca_store");
    return ESP_OK;
}

esp_err_t esp_wolfssl_set_global_ca_store(const unsigned char *cacert_pem_buf, const unsigned int cacert_pem_bytes)
{
    ESP_LOGXI(TAG, "Enter esp_wolfssl_set_global_ca_store");
    if (cacert_pem_buf == NULL) {
        ESP_LOGE(TAG, "cacert_pem_buf is null");
        return ESP_ERR_INVALID_ARG;
    }
    if (global_cacert != NULL) {
        esp_wolfssl_free_global_ca_store();
    }

    global_cacert = (unsigned char *)strndup((const char *)cacert_pem_buf, cacert_pem_bytes);
    if (!global_cacert) {
        return ESP_FAIL;
    }

    global_cacert_pem_bytes  = cacert_pem_bytes;

    return ESP_OK;
}

void esp_wolfssl_free_global_ca_store(void)
{
    if (global_cacert) {
        free(global_cacert);
        global_cacert = NULL;
        global_cacert_pem_bytes = 0;
    }
}

#if defined(CONFIG_ESP_TLS_PSK_VERIFICATION)
static esp_err_t esp_wolfssl_set_cipher_list(WOLFSSL_CTX *ctx)
{
    const char *defaultCipherList;
    int ret;
#if defined(HAVE_AESGCM) && !defined(NO_DH)
#ifdef WOLFSSL_TLS13
    defaultCipherList = "DHE-PSK-AES128-GCM-SHA256:"
                                    "TLS13-AES128-GCM-SHA256";
#else
    defaultCipherList = "DHE-PSK-AES128-GCM-SHA256";
#endif
#elif defined(HAVE_NULL_CIPHER)
    defaultCipherList = "PSK-NULL-SHA256";
#else
    defaultCipherList = "PSK-AES128-CBC-SHA256";
#endif
    ESP_LOGD(TAG, "cipher list is %s", defaultCipherList);
    if ((ret = wolfSSL_CTX_set_cipher_list(ctx,defaultCipherList)) != WOLFSSL_SUCCESS) {
        wolfSSL_CTX_free(ctx);
        int err = wolfSSL_get_error( (WOLFSSL *)tls->priv_ssl, ret);
        ESP_LOGE(TAG, "can't set cipher list, returned %d, error code: %d", ret, err);
        wolfssl_print_error_msg(err);
        return ESP_FAIL;
    }
    return ESP_OK;
}

/* initialize the mutex before app_main() when using PSK */
static void __attribute__((constructor))
espt_tls_wolfssl_init_conn_lock (void)
{
    if ((tls_conn_lock = xSemaphoreCreateMutex()) == NULL) {
        ESP_EARLY_LOGE(TAG, "mutex for tls psk connection could not be created");
    }
}

/* Some callback functions required by PSK */
static inline unsigned int esp_wolfssl_psk_client_cb(WOLFSSL* ssl, const char* hint,
        char* identity, unsigned int id_max_len, unsigned char* key,
        unsigned int key_max_len)
{
    (void)key_max_len;

    /* see internal.h MAX_PSK_ID_LEN for PSK identity limit */
    memcpy(identity, psk_id_str, id_max_len);
    for(int count = 0; count < psk_key_max_len; count ++) {
         key[count] = psk_key_array[count];
    }
    xSemaphoreGive(tls_conn_lock);
    return psk_key_max_len;
    /* return length of key in octets or 0 or for error */
}
#endif /* CONFIG_ESP_TLS_PSK_VERIFICATION */

#endif /* CONFIG_ESP_TLS_USING_WOLFSSL */
