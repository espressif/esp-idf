/*
 * SPDX-FileCopyrightText: 2018-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdbool.h>
#include "esp_crt_bundle.h"
#include "esp_log.h"

#define BUNDLE_HEADER_OFFSET 2
#define CRT_HEADER_OFFSET 4

static const char *TAG = "esp-x509-crt-bundle";

/* a dummy certificate so that
 * cacert_ptr passes non-NULL check during handshake */
static const mbedtls_x509_crt s_dummy_crt;

extern const uint8_t x509_crt_imported_bundle_bin_start[] asm("_binary_x509_crt_bundle_start");
extern const uint8_t x509_crt_imported_bundle_bin_end[]   asm("_binary_x509_crt_bundle_end");


typedef struct crt_bundle_t {
    const uint8_t **crts;
    uint16_t num_certs;
    size_t x509_crt_bundle_len;
} crt_bundle_t;

static crt_bundle_t s_crt_bundle;

static int esp_crt_check_signature(mbedtls_x509_crt *child, const uint8_t *pub_key_buf, size_t pub_key_len);


static int esp_crt_check_signature(mbedtls_x509_crt *child, const uint8_t *pub_key_buf, size_t pub_key_len)
{
    int ret = 0;
    mbedtls_x509_crt parent;
    const mbedtls_md_info_t *md_info;
    unsigned char hash[MBEDTLS_MD_MAX_SIZE];

    mbedtls_x509_crt_init(&parent);

    if ( (ret = mbedtls_pk_parse_public_key(&parent.pk, pub_key_buf, pub_key_len) ) != 0) {
        ESP_LOGE(TAG, "PK parse failed with error %X", ret);
        goto cleanup;
    }


    // Fast check to avoid expensive computations when not necessary
    if (!mbedtls_pk_can_do(&parent.pk, child->MBEDTLS_PRIVATE(sig_pk))) {
        ESP_LOGE(TAG, "Simple compare failed");
        ret = -1;
        goto cleanup;
    }

    md_info = mbedtls_md_info_from_type(child->MBEDTLS_PRIVATE(sig_md));
    if ( (ret = mbedtls_md( md_info, child->tbs.p, child->tbs.len, hash )) != 0 ) {
        ESP_LOGE(TAG, "Internal mbedTLS error %X", ret);
        goto cleanup;
    }

    if ( (ret = mbedtls_pk_verify_ext( child->MBEDTLS_PRIVATE(sig_pk), child->MBEDTLS_PRIVATE(sig_opts), &parent.pk,
                                       child->MBEDTLS_PRIVATE(sig_md), hash, mbedtls_md_get_size( md_info ),
                                       child->MBEDTLS_PRIVATE(sig).p, child->MBEDTLS_PRIVATE(sig).len )) != 0 ) {

        ESP_LOGE(TAG, "PK verify failed with error %X", ret);
        goto cleanup;
    }
cleanup:
    mbedtls_x509_crt_free(&parent);

    return ret;
}


static size_t esp_crt_name_len(const uint8_t *crt)
{
    return crt[0] << 8 | crt[1];
}

static size_t esp_crt_key_len(const uint8_t *crt)
{
    return crt[2] << 8 | crt[3];
}

static const uint8_t *esp_crt_name(const uint8_t *crt)
{
    return crt + CRT_HEADER_OFFSET;
}

static const uint8_t *esp_crt_key(const uint8_t *crt)
{
    return crt + CRT_HEADER_OFFSET + esp_crt_name_len(crt);
}

/* Binary search on the DER subject name; returns the bundle entry or NULL */
static const uint8_t *esp_crt_find_cert(const uint8_t *name, size_t len)
{
    if (name == NULL || len == 0) {
        return NULL;
    }

    int start = 0;
    int end = s_crt_bundle.num_certs - 1;
    int middle = (start + end) / 2;

    while (start <= end) {
        const uint8_t *crt = s_crt_bundle.crts[middle];
        size_t crt_name_len = esp_crt_name_len(crt);

        /* Both names are DER, so their lengths are encoded in the first bytes
           and a differing length shows up as differing content. */
        int cmp_res = memcmp(name, esp_crt_name(crt), len < crt_name_len ? len : crt_name_len);
        if (cmp_res == 0) {
            return crt;
        } else if (cmp_res < 0) {
            end = middle - 1;
        } else {
            start = middle + 1;
        }
        middle = (start + end) / 2;
    }

    return NULL;
}

/* This callback is called for every certificate in the chain. If the chain
 * is proper each intermediate certificate is validated through its parent
 * in the x509_crt_verify_chain() function. So this callback only has to
 * decide about the first untrusted link in the chain: either it is a
 * cross-signed copy of a root in the trusted bundle, or it must be signed
 * by a root in the trusted bundle.
*/
int esp_crt_verify_callback(void *buf, mbedtls_x509_crt *crt, int depth, uint32_t *flags)
{
    mbedtls_x509_crt *child = crt;

    /* It's OK for a trusted cert to have a weak signature hash alg.
       as we already trust this certificate */
    uint32_t flags_filtered = *flags & ~(MBEDTLS_X509_BADCERT_BAD_MD);

    if (flags_filtered != MBEDTLS_X509_BADCERT_NOT_TRUSTED) {
        return 0;
    }


    if (s_crt_bundle.crts == NULL) {
        ESP_LOGE(TAG, "No certificates in bundle");
        return MBEDTLS_ERR_X509_FATAL_ERROR;
    }

    ESP_LOGD(TAG, "%d certificates in bundle", s_crt_bundle.num_certs);

    /* A certificate mbedtls could not chain to a trusted CA may be a
       cross-signed copy of a root the bundle already trusts. Its subject and
       public key then match a bundle entry, and mbedtls has already verified
       the chain below it against that key, so the cross-signer need not be
       in the bundle. Limit: mbedtls has no CA list here and cannot stop at a
       trusted certificate, so if the server also sends the cross-signer's
       root, the path extends to that untrusted root and verification fails. */
    const uint8_t *self = esp_crt_find_cert(child->subject_raw.p, child->subject_raw.len);
    if (self != NULL && child->pk_raw.len == esp_crt_key_len(self) &&
            memcmp(child->pk_raw.p, esp_crt_key(self), child->pk_raw.len) == 0) {
        ESP_LOGI(TAG, "Certificate validated (cross-signed copy of a trusted root)");
        *flags = 0;
        return 0;
    }

    /* Otherwise the certificate must be signed by a root in the bundle */
    int ret = MBEDTLS_ERR_X509_FATAL_ERROR;
    const uint8_t *issuer = esp_crt_find_cert(child->issuer_raw.p, child->issuer_raw.len);
    if (issuer != NULL) {
        ret = esp_crt_check_signature(child, esp_crt_key(issuer), esp_crt_key_len(issuer));
    }

    if (ret == 0) {
        ESP_LOGI(TAG, "Certificate validated");
        *flags = 0;
        return 0;
    }

    ESP_LOGE(TAG, "Failed to verify certificate");
    return MBEDTLS_ERR_X509_FATAL_ERROR;
}


/* Initialize the bundle into an array so we can do binary search for certs,
   the bundle generated by the python utility is already presorted by subject name
 */
static esp_err_t esp_crt_bundle_init(const uint8_t *x509_bundle, size_t bundle_size)
{
    if (bundle_size < BUNDLE_HEADER_OFFSET + CRT_HEADER_OFFSET) {
        ESP_LOGE(TAG, "Invalid certificate bundle");
        return ESP_ERR_INVALID_ARG;
    }

    uint16_t num_certs = (x509_bundle[0] << 8) | x509_bundle[1];
    if (num_certs > CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_MAX_CERTS) {
        ESP_LOGE(TAG, "No. of certs in the certificate bundle = %d exceeds\n"
                      "Max allowed certificates in the certificate bundle = %d\n"
                      "Please update the menuconfig option with appropriate value", num_certs, CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_MAX_CERTS);
        return ESP_ERR_INVALID_ARG;
    }

    const uint8_t **crts = calloc(num_certs, sizeof(x509_bundle));
    if (crts == NULL) {
        ESP_LOGE(TAG, "Unable to allocate memory for bundle");
        return ESP_ERR_NO_MEM;
    }

    const uint8_t *cur_crt;
    /* This is the maximum region that is allowed to access */
    const uint8_t *bundle_end = x509_bundle + bundle_size;
    cur_crt = x509_bundle + BUNDLE_HEADER_OFFSET;

    for (int i = 0; i < num_certs; i++) {
        crts[i] = cur_crt;
        if (cur_crt + CRT_HEADER_OFFSET > bundle_end) {
            ESP_LOGE(TAG, "Invalid certificate bundle");
            free(crts);
            return ESP_ERR_INVALID_ARG;
        }
        size_t name_len = cur_crt[0] << 8 | cur_crt[1];
        size_t key_len = cur_crt[2] << 8 | cur_crt[3];
        cur_crt = cur_crt + CRT_HEADER_OFFSET + name_len + key_len;
    }

    if (cur_crt > bundle_end) {
        ESP_LOGE(TAG, "Invalid certificate bundle");
        free(crts);
        return ESP_ERR_INVALID_ARG;
    }

    /* The previous crt bundle is only updated when initialization of the
     * current crt_bundle is successful */
    /* Free previous crt_bundle */
    free(s_crt_bundle.crts);
    s_crt_bundle.num_certs = num_certs;
    s_crt_bundle.crts = crts;
    return ESP_OK;
}

esp_err_t esp_crt_bundle_attach(void *conf)
{
    esp_err_t ret = ESP_OK;
    // If no bundle has been set by the user then use the bundle embedded in the binary
    if (s_crt_bundle.crts == NULL) {
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
    free(s_crt_bundle.crts);
    s_crt_bundle.crts = NULL;
    if (conf) {
        mbedtls_ssl_conf_verify(conf, NULL, NULL);
    }
}

esp_err_t esp_crt_bundle_set(const uint8_t *x509_bundle, size_t bundle_size)
{
    return esp_crt_bundle_init(x509_bundle, bundle_size);
}

bool esp_crt_bundle_in_use(const mbedtls_x509_crt* ca_chain)
{
    return ((ca_chain == &s_dummy_crt) ? true : false);
}
