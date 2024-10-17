/*
 * SPDX-FileCopyrightText: 2018-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdbool.h>
#include <sys/param.h>

#include "esp_check.h"
#include "esp_crt_bundle.h"
#include "esp_log.h"

#include "mbedtls/pk.h"
#include "mbedtls/oid.h"
#include "mbedtls/asn1.h"

/*
    Format of certificate bundle:
    First, n uint32 "offset" entries, each describing the start of one certificate's data in terms of
    bytes from the beginning of the bundle. This offset list is immediately followed by the 1st...n-th
    certificate data. Hence, the first offset entry, i.e. the uint32 at the very start of the bundle,
    is equal to the size of the offset list in bytes and therefore the # of certificates in the bundle
    is [first offset]/sizeof(uint32_t)
    [offset of 1st certificate](u32)
    [offset of 2nd certificate](u32)
    ...
    [offset of n-th certificate](u32)
    [1st certificate](variable)
    ...
    [n-th certificate](variable)

    Structure of each certificate:
    [length of CN](u16)
    [length of key](u16)
    [CN](variable)
    [key](variable)

    The offset list is used for fast random access to any certificate by index.
    For verification, a certificate is looked up by its CN via binary search; for this reason,
    the offset list *must* be sorted by CN (ascending) and the first certificate must be the
    one with the least CN in the bundle, so that the first offset in the list still refers to the
    first certificate after the list (see above).

*/

#define CRT_NAME_LEN_OFFSET 0 //<! offset of certificate name length value
#define CRT_KEY_LEN_OFFSET (CRT_NAME_LEN_OFFSET + sizeof(uint16_t)) //<! offset of certificate key length value
#define CRT_NAME_OFFSET (CRT_KEY_LEN_OFFSET + sizeof(uint16_t)) //<! certificate name data starts here

#define CRT_HEADER_SIZE CRT_NAME_OFFSET //<! size of certificate header

static const char *TAG = "esp-x509-crt-bundle";

/* a dummy certificate so that
 * cacert_ptr passes non-NULL check during handshake */
static const mbedtls_x509_crt s_dummy_crt;

extern const uint8_t x509_crt_imported_bundle_bin_start[] asm("_binary_x509_crt_bundle_start");
extern const uint8_t x509_crt_imported_bundle_bin_end[]   asm("_binary_x509_crt_bundle_end");

typedef const uint8_t* bundle_t;
typedef const uint8_t* cert_t;

static bundle_t s_crt_bundle;

// Read a 16-bit value stored in little-endian format from the given address
static uint16_t get16_le(const uint8_t* ptr)
{
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    return *((const uint16_t*)ptr);
#else
    return (((uint16_t)ptr[1]) << 8) | ptr[0];
#endif
}

static uint16_t esp_crt_get_name_len(const cert_t cert)
{
    return get16_le(cert + CRT_NAME_LEN_OFFSET);
}

static const uint8_t* esp_crt_get_name(const cert_t cert)
{
    return cert + CRT_NAME_OFFSET;
}

static uint16_t esp_crt_get_key_len(const cert_t cert)
{
    return get16_le(cert + CRT_KEY_LEN_OFFSET);
}

static const uint8_t* esp_crt_get_key(const cert_t cert)
{
    return esp_crt_get_name(cert) + esp_crt_get_name_len(cert);
}

static uint16_t esp_crt_get_len(const cert_t cert)
{
    return CRT_HEADER_SIZE + esp_crt_get_name_len(cert) + esp_crt_get_key_len(cert);
}

static uint32_t esp_crt_get_cert_offset(const bundle_t bundle, const uint32_t index)
{
    return ((const uint32_t*)bundle)[index];
}

static uint32_t esp_crt_get_certcount(const bundle_t bundle)
{
    // Offset of 1st certificate == end of offset list == size of offset list == # of certs * sizeof(uint32_t)
    return esp_crt_get_cert_offset(bundle, 0) / sizeof(uint32_t);
}

/**
 * @brief Get the certificate at the given index within a bundle.
 *
 * @param bundle pointer to the \c bundle_t
 * @param index of the certificate; must be less than \c esp_crt_get_certcount(...) !
 * @return pointer to the certificate
 */
static cert_t esp_crt_get_cert(const bundle_t bundle, const uint32_t index)
{
    return bundle + esp_crt_get_cert_offset(bundle, index);
}

static int esp_crt_check_signature(const mbedtls_x509_crt* child, const uint8_t* pub_key_buf, const size_t pub_key_len)
{
    int ret = 0;
    mbedtls_pk_context pubkey;
    const mbedtls_md_info_t *md_info;

    mbedtls_pk_init(&pubkey);

    if (unlikely((ret = mbedtls_pk_parse_public_key(&pubkey, pub_key_buf, pub_key_len)) != 0)) {
        ESP_LOGE(TAG, "PK parse failed with error 0x%x", -ret);
        goto cleanup;
    }

    // Fast check to avoid expensive computations when not necessary
    if (unlikely(!mbedtls_pk_can_do(&pubkey, child->MBEDTLS_PRIVATE(sig_pk)))) {
        ESP_LOGE(TAG, "Unsuitable public key");
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
        goto cleanup;
    }

    md_info = mbedtls_md_info_from_type(child->MBEDTLS_PRIVATE(sig_md));

    if (unlikely(md_info == NULL)) {
        ESP_LOGE(TAG, "Unknown message digest");
        ret = MBEDTLS_ERR_X509_FEATURE_UNAVAILABLE;
        goto cleanup;
    }

    unsigned char hash[MBEDTLS_MD_MAX_SIZE];
    const unsigned char md_size = mbedtls_md_get_size(md_info);

    if ((ret = mbedtls_md(md_info, child->tbs.p, child->tbs.len, hash)) != 0) {
        ESP_LOGE(TAG, "MD failed with error 0x%x", -ret);
        goto cleanup;
    }

    if (unlikely((ret = mbedtls_pk_verify_ext(child->MBEDTLS_PRIVATE(sig_pk), child->MBEDTLS_PRIVATE(sig_opts), &pubkey,
                                              child->MBEDTLS_PRIVATE(sig_md), hash, md_size,
                                              child->MBEDTLS_PRIVATE(sig).p, child->MBEDTLS_PRIVATE(sig).len)) != 0)) {
        ESP_LOGE(TAG, "PK verify failed with error 0x%x", -ret);
        goto cleanup;
    }

cleanup:
    mbedtls_pk_free(&pubkey);
    return ret;
}

static cert_t esp_crt_find_cert(const unsigned char* const issuer, const size_t issuer_len)
{
    if (unlikely(issuer == NULL || issuer_len == 0)) {
        return NULL;
    }

    int start = 0;
    int end = esp_crt_get_certcount(s_crt_bundle) - 1;
    int middle = (start + end) / 2;

    cert_t cert = NULL;
    size_t cert_name_len = 0;

    /* Look for the certificate using binary search on subject name */
    while (start <= end) {
        cert = esp_crt_get_cert(s_crt_bundle, middle);
        cert_name_len = esp_crt_get_name_len(cert);

        // Issuers are in DER encoding, with lengths encoded in the content; if valid DER, differing lengths
        // are reflected in differing content.
        // Still, we won't try to memcmp beyond the given length:
        int cmp_res = memcmp(issuer, esp_crt_get_name(cert), MIN(issuer_len, cert_name_len));

        if (unlikely(cmp_res == 0)) {
            return cert;
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


    if (unlikely(s_crt_bundle == NULL)) {
        ESP_LOGE(TAG, "No certificates in bundle");
        return MBEDTLS_ERR_X509_FATAL_ERROR;
    }

    ESP_LOGD(TAG, "%" PRIu16 " certificates in bundle", (uint16_t)esp_crt_get_certcount(s_crt_bundle));

    cert_t cert = esp_crt_find_cert(child->issuer_raw.p, child->issuer_raw.len);

    if (likely(cert != NULL)) {

        const int ret = esp_crt_check_signature(child, esp_crt_get_key(cert), esp_crt_get_key_len(cert));

        if (likely(ret == 0)) {
            ESP_LOGI(TAG, "Certificate validated");
            *flags = 0;
            return 0;
        } else {
            ESP_LOGE(TAG, "Certificate matched but signature verification failed");
#if (CONFIG_LOG_DEFAULT_LEVEL_DEBUG || CONFIG_LOG_DEFAULT_LEVEL_VERBOSE)
            char *cert_name = malloc((esp_crt_get_name_len(cert) + 1) * sizeof(char));
            if (cert_name) {
                memcpy(cert_name, esp_crt_get_name(cert), esp_crt_get_name_len(cert));
                cert_name[esp_crt_get_name_len(cert)] = '\0';
                ESP_LOGE(TAG, "Certificate matched with %s but signature verification failed", cert_name);
                free(cert_name);
            }
#endif
        }

    } else {
        ESP_LOGI(TAG, "No matching trusted root certificate found");
    }

    ESP_LOGE(TAG, "Failed to verify certificate");
    return MBEDTLS_ERR_X509_CERT_VERIFY_FAILED;
}

/**
 * @brief Perform some consistency checks on the user-provided bundle data to try and make sure
 * it actually is a certificate bundle.
 *
 * @param x509_bundle pointer to the bundle data
 * @param bundle_size size of bundle data
 * @return true the given bundle data is consistent
 * @return false the given bundle data is invalid
 */
static bool esp_crt_check_bundle(const uint8_t* const x509_bundle, const size_t bundle_size)
{
    if (unlikely(x509_bundle == NULL || bundle_size <= (sizeof(uint32_t) + CRT_HEADER_SIZE))) {
        // Bundle is too small for even one offset and one certificate
        return false;
    }

    // Pointer to the first offset entry
    const uint32_t* offsets = (const uint32_t*)x509_bundle;

    if (unlikely(offsets[0] == 0 || (offsets[0] % sizeof(uint32_t)) != 0)) {
        // First offset is invalid.
        // The first certificate must start after N uint32_t offset values.
        return false;
    }

    if (unlikely(offsets[0] >= bundle_size)) {
        // First cert starts beyond end of bundle
        return false;
    }

    const uint32_t num_certs = esp_crt_get_certcount(x509_bundle);

    if (unlikely(num_certs > CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_MAX_CERTS)) {
        ESP_LOGE(TAG, "Cert bundle certificates exceed max allowed certificates");
        return false;
    }

    // Check all offsets for consistency with certificate data
    for (uint32_t i = 0; i < num_certs - 1; ++i) {
        const uint32_t off = offsets[i];
        cert_t cert = x509_bundle + off;
        // The next offset in the list must point to right after the current cert
        const uint32_t expected_next_offset = off + esp_crt_get_len(cert);

        if (unlikely(offsets[i + 1] != expected_next_offset || expected_next_offset >= bundle_size)) {
            return false;
        }
    }

    // All checks passed.
    return true;
}

/*
   the bundle generated by the python utility is already presorted by subject name
 */
static esp_err_t esp_crt_bundle_init(const uint8_t* const x509_bundle, const size_t bundle_size)
{
    if (likely(esp_crt_check_bundle(x509_bundle, bundle_size))) {
        s_crt_bundle = x509_bundle;
        return ESP_OK;
    } else {
        return ESP_ERR_INVALID_ARG;
    }
}

esp_err_t esp_crt_bundle_attach(void *conf)
{
    esp_err_t ret = ESP_OK;
    // If no bundle has been set by the user then use the bundle embedded in the binary
    if (s_crt_bundle == NULL) {
        ret = esp_crt_bundle_init(x509_crt_imported_bundle_bin_start, x509_crt_imported_bundle_bin_end - x509_crt_imported_bundle_bin_start);
    }

    if (unlikely(ret != ESP_OK)) {
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
    s_crt_bundle = NULL;
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
