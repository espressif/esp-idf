/*
 * SPDX-FileCopyrightText: 2018-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdbool.h>
#include "esp_crt_bundle.h"
#include "esp_log.h"
#include "esp_check.h"

#include "mbedtls/pk.h"

#define BUNDLE_HEADER_OFFSET sizeof(uint16_t) //<! size of bundle header
#define CRT_HEADER_OFFSET (2*sizeof(uint16_t)) //<! size of certificate header

#define CRT_NAME_LEN_OFFSET 0 //<! offset of certificate name length value
#define CRT_KEY_LEN_OFFSET (CRT_NAME_LEN_OFFSET + sizeof(uint16_t)) //<! offset of certificate key length value
#define CRT_NAME_OFFSET (CRT_KEY_LEN_OFFSET + sizeof(uint16_t)) //<! certificate name data starts here

static const char *TAG = "esp-x509-crt-bundle";

/* a dummy certificate so that
 * cacert_ptr passes non-NULL check during handshake */
// Making this const is a bit of a hack but saves 300+ bytes of statically allocated RAM.
// As of now, mbedtls_x509_crt_init would just zero the struct anyway, so this is actually equivalent
// to a officially initialized x509_crt; plus, as of now, this cert's *content* is not used anywhere.
static const mbedtls_x509_crt s_dummy_crt;


extern const uint8_t x509_crt_imported_bundle_bin_start[] asm("_binary_x509_crt_bundle_start");
extern const uint8_t x509_crt_imported_bundle_bin_end[]   asm("_binary_x509_crt_bundle_end");



typedef struct __attribute__((packed)) crt_bundle_info_t {
    uint16_t num_certs;
    uint8_t  data[];
} crt_bundle_info_t;

typedef struct __attribute__((packed)) crt_cert_info_t {
    uint16_t name_len;
    uint16_t key_len;
    uint8_t  data[];
} crt_cert_info_t;



typedef struct crt_bundle_t {
    const uint8_t* bundle;
    const uint32_t* offsets;
    uint16_t num_certs;
} crt_bundle_t;

static crt_bundle_t s_crt_bundle;

static uint16_t get16_le(const uint8_t* const ptr) {
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        return *((const uint16_t*)ptr);
    #else
        return (((uint16_t)ptr[1]) << 8) | ptr[0];
    #endif
}

static uint16_t esp_crt_get_certcount(const uint8_t* const bundle) {
    return get16_le(bundle);
}

static uint16_t esp_crt_get_name_len(const uint8_t* const cert) {
    return get16_le(cert + CRT_NAME_LEN_OFFSET);
}

static const uint8_t* esp_crt_get_name(const uint8_t* const cert) {
    return cert + CRT_NAME_OFFSET;
}

static uint16_t esp_crt_get_key_len(const uint8_t* const cert) {
    return get16_le(cert + CRT_KEY_LEN_OFFSET);
}

static const uint8_t* esp_crt_get_key(const uint8_t* const cert) {
    return cert + CRT_NAME_OFFSET + esp_crt_get_name_len(cert);
}

static uint16_t esp_crt_get_len(const uint8_t* const cert) {
    return CRT_HEADER_OFFSET + esp_crt_get_name_len(cert) + esp_crt_get_key_len(cert);
}

static const uint8_t* esp_crt_get_next(const uint8_t* const cert) {
    return cert + esp_crt_get_len(cert);
}

/**
 * @brief Get the certificate at the given index within a bundle.
 *
 * @param bundle pointer to the \c crt_bundle_t
 * @param index of the certificate; must be less than \c bundle->num_certs !
 * @return const uint8_t* pointer to the certificate
 */
static const uint8_t* esp_crt_get_cert(const crt_bundle_t* const bundle, const uint16_t index) {
    return bundle->bundle + bundle->offsets[index];
}


static int esp_crt_check_signature(const mbedtls_x509_crt* const child, const uint8_t* const pub_key_buf, const size_t pub_key_len)
{
    int ret = 0;
    mbedtls_pk_context pubkey;
    const mbedtls_md_info_t *md_info;
    unsigned char hash[MBEDTLS_MD_MAX_SIZE];

    mbedtls_pk_init(&pubkey);

    if ( (ret = mbedtls_pk_parse_public_key(&pubkey, pub_key_buf, pub_key_len) ) != 0) {
        ESP_LOGE(TAG, "PK parse failed with error %X", ret);
        goto cleanup;
    }


    // Fast check to avoid expensive computations when not necessary
    if (!mbedtls_pk_can_do( &pubkey, child->MBEDTLS_PRIVATE(sig_pk))) {
        ESP_LOGE(TAG, "Simple compare failed");
        ret = -1;
        goto cleanup;
    }

    md_info = mbedtls_md_info_from_type(child->MBEDTLS_PRIVATE(sig_md));
    if ( (ret = mbedtls_md( md_info, child->tbs.p, child->tbs.len, hash )) != 0 ) {
        ESP_LOGE(TAG, "Internal mbedTLS error %X", ret);
        goto cleanup;
    }

    if ( (ret = mbedtls_pk_verify_ext( child->MBEDTLS_PRIVATE(sig_pk), child->MBEDTLS_PRIVATE(sig_opts), &pubkey,
                                       child->MBEDTLS_PRIVATE(sig_md), hash, mbedtls_md_get_size( md_info ),
                                       child->MBEDTLS_PRIVATE(sig).p, child->MBEDTLS_PRIVATE(sig).len )) != 0 ) {

        ESP_LOGE(TAG, "PK verify failed with error %X", ret);
        goto cleanup;
    }
cleanup:
    mbedtls_pk_free(&pubkey);

    return ret;
}


/* This callback is called for every certificate in the chain. If the chain
 * is proper each intermediate certificate is validated through its parent
 * in the x509_crt_verify_chain() function. So this callback should
 * only verify the first untrusted link in the chain is signed by the
 * root certificate in the trusted bundle
*/
int esp_crt_verify_callback(void *buf, mbedtls_x509_crt* const crt, const int depth, uint32_t* const flags)
{
    const mbedtls_x509_crt* const child = crt;

    /* It's OK for a trusted cert to have a weak signature hash alg.
       as we already trust this certificate */
    uint32_t flags_filtered = *flags & ~(MBEDTLS_X509_BADCERT_BAD_MD);

    if (flags_filtered != MBEDTLS_X509_BADCERT_NOT_TRUSTED) {
        return 0;
    }


    if(unlikely(s_crt_bundle.bundle == NULL || s_crt_bundle.num_certs == 0)) {
        ESP_LOGE(TAG, "No certificates in bundle");
        return MBEDTLS_ERR_X509_FATAL_ERROR;
    }

    ESP_LOGD(TAG, "%d certificates in bundle", s_crt_bundle.num_certs);

    bool crt_found = false;
    int start = 0;
    int end = s_crt_bundle.num_certs - 1;
    int middle = (end - start) / 2;

    const uint8_t* cert = NULL;
    {
        const unsigned char* const child_name = child->issuer_raw.p;
        const size_t child_name_len = child->issuer_raw.len;

        /* Look for the certificate using binary search on subject name */
        while (start <= end) {
            cert = esp_crt_get_cert(&s_crt_bundle, middle);

            const size_t cert_name_len = esp_crt_get_name_len(cert);

            int cmp_res = memcmp(child_name, esp_crt_get_name(cert), child_name_len <= cert_name_len ? child_name_len : cert_name_len );

            if (cmp_res == 0) {
                cmp_res = (int)child_name_len - cert_name_len;
                if(cmp_res == 0) {
                    // Names match and have the same length
                    crt_found = true;
                    break;
                }
            }

            if (cmp_res < 0) {
                end = middle - 1;
            } else {
                start = middle + 1;
            }

            middle = (start + end) / 2;
        }
    }

    if (likely(crt_found)) {
        const int ret = esp_crt_check_signature(child, esp_crt_get_key(cert), esp_crt_get_key_len(cert));

        if (likely(ret == 0)) {
            ESP_LOGI(TAG, "Certificate validated");
            *flags = 0;
            return 0;
        }
    }

    ESP_LOGE(TAG, "Failed to verify certificate");
    return MBEDTLS_ERR_X509_FATAL_ERROR;
}

/* Initialize the bundle into an array so we can do binary search for certs,
   the bundle generated by the python utility is already presorted by subject name
 */
static esp_err_t esp_crt_bundle_init(const uint8_t* const x509_bundle, const size_t bundle_size)
{
    if (bundle_size < BUNDLE_HEADER_OFFSET + CRT_HEADER_OFFSET) {
        ESP_LOGE(TAG, "Invalid certificate bundle");
        return ESP_ERR_INVALID_ARG;
    }

    const uint16_t num_certs = esp_crt_get_certcount(x509_bundle);

    if (unlikely(num_certs > CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_MAX_CERTS)) {
        ESP_LOGE(TAG, "No. of certs in the certificate bundle = %d exceeds\n"
                      "Max allowed certificates in the certificate bundle = %d\n"
                      "Please update the menuconfig option with appropriate value", num_certs, CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_MAX_CERTS);
        return ESP_ERR_INVALID_ARG;
    }

    const uint8_t *cur_crt;
    /* This is the maximum region that is allowed to access */
    const uint8_t* const bundle_end = x509_bundle + bundle_size;
    cur_crt = x509_bundle + BUNDLE_HEADER_OFFSET;

    /* Scan through all certificates to
     * a) check for consistency of the bundle data and
     * b) find the end of the last certificate, which is the start of the offsets list
    */
    for (int i = 0; i < num_certs; i++) {
        if (unlikely(cur_crt + CRT_HEADER_OFFSET > bundle_end)) {
            ESP_LOGE(TAG, "Invalid certificate bundle");
            return ESP_ERR_INVALID_ARG;
        }

        cur_crt = esp_crt_get_next(cur_crt);
    }

    // The end of the last certificate plus the size of the offsets list must be within the bundle's limits
    if (unlikely(cur_crt > bundle_end || (cur_crt + num_certs*sizeof(uint32_t)) > bundle_end)) {
        ESP_LOGE(TAG, "Invalid certificate bundle");
        return ESP_ERR_INVALID_ARG;
    }

    /* The previous crt bundle is only updated when initialization of the
     * current crt_bundle is successful */
    s_crt_bundle.bundle = x509_bundle;
    s_crt_bundle.offsets = (const uint32_t*)(cur_crt); // offsets start right after the last certificate
    s_crt_bundle.num_certs = num_certs;

    return ESP_OK;
}

esp_err_t esp_crt_bundle_attach(void *conf)
{
    esp_err_t ret = ESP_OK;
    // If no bundle has been set by the user then use the bundle embedded in the binary
    if (s_crt_bundle.bundle == NULL) {
        ret = esp_crt_bundle_init(x509_crt_imported_bundle_bin_start, x509_crt_imported_bundle_bin_end - x509_crt_imported_bundle_bin_start);
    }

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to attach bundle");
        return ret;
    }

    if (conf) {
        /* point to a dummy certificate
         * This is only required so that the
         * cacert_ptr passes non-NULL check during handshake
         */
        mbedtls_ssl_config *ssl_conf = (mbedtls_ssl_config *)conf;
        mbedtls_ssl_conf_ca_chain(ssl_conf, (mbedtls_x509_crt*)&s_dummy_crt, NULL);
        mbedtls_ssl_conf_verify(ssl_conf, esp_crt_verify_callback, NULL);
    }

    return ret;
}

void esp_crt_bundle_detach(mbedtls_ssl_config *conf)
{
    s_crt_bundle.bundle = NULL;
    s_crt_bundle.offsets = NULL;
    s_crt_bundle.num_certs = 0;
    if (conf) {
        mbedtls_ssl_conf_verify(conf, NULL, NULL);
    }
}

esp_err_t esp_crt_bundle_set(const uint8_t *x509_bundle, size_t bundle_size)
{
    return esp_crt_bundle_init(x509_bundle, bundle_size);
}
