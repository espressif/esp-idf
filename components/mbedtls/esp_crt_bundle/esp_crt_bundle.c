/*
 * SPDX-FileCopyrightText: 2018-2025 Espressif Systems (Shanghai) CO LTD
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

#include "sdkconfig.h"

#include "psa/crypto.h"
#include "mbedtls/psa_util.h"

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
#if !defined(CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_CROSS_SIGNED_VERIFY)
static const mbedtls_x509_crt s_dummy_crt;
#endif // CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_CROSS_SIGNED_VERIFY

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
    psa_key_id_t key_id = 0;
    psa_status_t status;
    psa_key_attributes_t key_attr = PSA_KEY_ATTRIBUTES_INIT;
    bool key_imported = false;

    mbedtls_pk_init(&pubkey);

    if (unlikely((ret = mbedtls_pk_parse_public_key(&pubkey, pub_key_buf, pub_key_len)) != 0)) {
        ESP_LOGE(TAG, "PK parse failed with error 0x%x", -ret);
        goto cleanup;
    }

    // Get the message digest info for the hash algorithm used in the certificate
    // We need to know this BEFORE importing the key so we can set the correct algorithm
    md_info = mbedtls_md_info_from_type(child->MBEDTLS_PRIVATE(sig_md));
    if (unlikely(md_info == NULL)) {
        ESP_LOGE(TAG, "Unknown message digest type: %d", child->MBEDTLS_PRIVATE(sig_md));
        ret = MBEDTLS_ERR_X509_FEATURE_UNAVAILABLE;
        goto cleanup;
    }

    // Map mbedTLS MD type to PSA hash algorithm
    psa_algorithm_t psa_hash_alg;
    switch (child->MBEDTLS_PRIVATE(sig_md)) {
        case MBEDTLS_MD_SHA256:
            psa_hash_alg = PSA_ALG_SHA_256;
            break;
        case MBEDTLS_MD_SHA384:
            psa_hash_alg = PSA_ALG_SHA_384;
            break;
        case MBEDTLS_MD_SHA512:
            psa_hash_alg = PSA_ALG_SHA_512;
            break;
        case MBEDTLS_MD_SHA1:
            psa_hash_alg = PSA_ALG_SHA_1;
            break;
        default:
            ESP_LOGE(TAG, "Unsupported hash algorithm: %d", child->MBEDTLS_PRIVATE(sig_md));
            ret = MBEDTLS_ERR_X509_FEATURE_UNAVAILABLE;
            goto cleanup;
    }

    // Get the appropriate key attributes for signature verification
    ret = mbedtls_pk_get_psa_attributes(&pubkey, PSA_KEY_USAGE_VERIFY_HASH, &key_attr);
    if (unlikely(ret != 0)) {
        ESP_LOGE(TAG, "Failed to get PSA key attributes with error 0x%x", -ret);
        goto cleanup;
    }

    // Determine the PSA algorithm based on the key type and hash type
    // We need to set this BEFORE importing the key
    psa_algorithm_t psa_alg;
    psa_key_type_t key_type = psa_get_key_type(&key_attr);

    ESP_LOGD(TAG, "Key type: 0x%x, Hash alg: 0x%x",
             (unsigned int)key_type, (unsigned int)psa_hash_alg);

    if (PSA_KEY_TYPE_IS_RSA(key_type)) {
        // For RSA keys, use PKCS#1 v1.5 with the specific hash algorithm
        psa_alg = PSA_ALG_RSA_PKCS1V15_SIGN(psa_hash_alg);
        ESP_LOGD(TAG, "Using RSA PKCS1V15 SIGN algorithm with hash");
    } else if (PSA_KEY_TYPE_IS_ECC(key_type)) {
        // For ECC keys, use ECDSA_ANY which works with psa_verify_hash
        // and doesn't constrain the hash length
        psa_alg = PSA_ALG_ECDSA_ANY;
        ESP_LOGD(TAG, "Using ECDSA_ANY algorithm (no hash constraint)");
    } else {
        ESP_LOGE(TAG, "Unsupported key type: 0x%x", (unsigned int)key_type);
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
        goto cleanup;
    }

    // Override the algorithm in key attributes with the specific hash algorithm
    // This is required because PSA_ALG_ANY_HASH wildcard doesn't work for verification
    psa_set_key_algorithm(&key_attr, psa_alg);

    // Import the public key into PSA
    ret = mbedtls_pk_import_into_psa(&pubkey, &key_attr, &key_id);
    if (unlikely(ret != 0)) {
        ESP_LOGE(TAG, "Failed to import key into PSA with error 0x%x", -ret);
        goto cleanup;
    }
    key_imported = true;

    unsigned char hash[MBEDTLS_MD_MAX_SIZE];
    const unsigned char md_size = mbedtls_md_get_size(md_info);

    size_t hash_len = 0;
    status = psa_hash_compute(psa_hash_alg, child->tbs.p, child->tbs.len, hash, sizeof(hash), &hash_len);

    unsigned char *sig_ptr = child->MBEDTLS_PRIVATE(sig).p;
    size_t sig_len = child->MBEDTLS_PRIVATE(sig).len;
    unsigned char raw_sig[MBEDTLS_ECDSA_MAX_LEN];

    if (PSA_KEY_TYPE_IS_ECC(key_type)) {
        // Convert DER-encoded ECDSA signature to raw (r||s) format for PSA
        // Get the key size in bits from PSA attributes
        size_t key_bits = psa_get_key_bits(&key_attr);
        ret = mbedtls_ecdsa_der_to_raw(key_bits,
                                       child->MBEDTLS_PRIVATE(sig).p,
                                       child->MBEDTLS_PRIVATE(sig).len,
                                       raw_sig, sizeof(raw_sig), &sig_len);
        if (ret != 0) {
            ESP_LOGE(TAG, "Failed to convert ECDSA signature to raw format: 0x%x (returned %d)", -ret, ret);
            ret = MBEDTLS_ERR_X509_INVALID_SIGNATURE;
            goto cleanup;
        }
        sig_ptr = raw_sig;
        ESP_LOGD(TAG, "Converted DER signature (len=%zu) to raw format (len=%zu) for %zu-bit key",
                 child->MBEDTLS_PRIVATE(sig).len, sig_len, key_bits);
    }

    // Verify the signature using PSA with the correct algorithm
    ESP_LOGD(TAG, "Verifying signature: alg=0x%08x, hash_len=%d, sig_len=%zu",
             (unsigned int)psa_alg, md_size, sig_len);
    status = psa_verify_hash(key_id, psa_alg, hash, md_size, sig_ptr, sig_len);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "PSA signature verification failed with error 0x%x (decimal: %d)",
                 (unsigned int)status, (int)status);
        ret = MBEDTLS_ERR_X509_INVALID_SIGNATURE;
        goto cleanup;
    }

    ret = 0;
    ESP_LOGD(TAG, "Certificate signature verified successfully");

cleanup:
    if (key_imported) {
        psa_destroy_key(key_id);
    }
    psa_reset_key_attributes(&key_attr);
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

#if defined(CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_CROSS_SIGNED_VERIFY)
static int esp_crt_copy_asn1(const mbedtls_asn1_named_data *src, mbedtls_asn1_named_data *dst)
{
    if (src == NULL || dst == NULL) {
        return -1;
    }

    dst->oid.tag = src->oid.tag;
    dst->oid.len = src->oid.len;
    dst->oid.p = calloc(1, src->oid.len);
    if (dst->oid.p == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for OID");
        return -1;
    }
    memcpy(dst->oid.p, src->oid.p, src->oid.len);
    dst->val.tag = src->val.tag;
    dst->val.len = src->val.len;
    dst->val.p = calloc(1, src->val.len);
    if (dst->val.p == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for value");
        free(dst->oid.p);
        return -1;
    }
    memcpy(dst->val.p, src->val.p, src->val.len);
    return 0;
}

static int esp_crt_ca_cb_callback(void *ctx, mbedtls_x509_crt const *child, mbedtls_x509_crt **candidate_cas)
{
    if (unlikely(s_crt_bundle == NULL)) {
        ESP_LOGE(TAG, "No certificates in bundle");
        return MBEDTLS_ERR_X509_FATAL_ERROR;
    }

    ESP_LOGD(TAG, "%" PRIu16 " certificates in bundle", (uint16_t)esp_crt_get_certcount(s_crt_bundle));

    cert_t cert = esp_crt_find_cert(child->issuer_raw.p, child->issuer_raw.len);

    if (likely(cert == NULL)) {
        *candidate_cas = NULL;
        return 0;
    }
    // If we found a matching certificate, we need to allocate a new
    // mbedtls_x509_crt structure and copy the certificate data into it.
    mbedtls_x509_crt *new_cert = calloc(1, sizeof(mbedtls_x509_crt));
    if (unlikely(new_cert == NULL)) {
        ESP_LOGE(TAG, "Failed to allocate memory for new certificate");
        return MBEDTLS_ERR_X509_ALLOC_FAILED;
    }
    mbedtls_x509_crt_init(new_cert);

    new_cert->MBEDTLS_PRIVATE(ca_istrue) = true;
    new_cert->version = 3;

    const uint8_t *cert_name = esp_crt_get_name(cert);
    uint16_t cert_name_len = esp_crt_get_name_len(cert);
    new_cert->subject_raw.tag = MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE;
    new_cert->subject_raw.len = cert_name_len;
    new_cert->subject_raw.p = calloc(1, cert_name_len);
    if (new_cert->subject_raw.p == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for subject");
        mbedtls_x509_crt_free(new_cert);
        free(new_cert);
        return MBEDTLS_ERR_X509_ALLOC_FAILED;
    }
    memcpy(new_cert->subject_raw.p, cert_name, cert_name_len);

    const uint8_t *cert_key = esp_crt_get_key(cert);
    uint16_t cert_key_len = esp_crt_get_key_len(cert);
    // Set the public key in the new certificate
    mbedtls_pk_init(&new_cert->pk);
    // Use mbedtls_pk_parse_public_key() instead of deprecated mbedtls_pk_parse_subpubkey()
    int ret = mbedtls_pk_parse_public_key(&new_cert->pk, cert_key, cert_key_len);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to parse public key from certificate: %d", ret);
        mbedtls_x509_crt_free(new_cert);
        free(new_cert);
        return ret;
    }

    // Loop through the child->issuer and copy the values to the new certificate
    const mbedtls_asn1_named_data *child_issuer = &child->issuer;
    mbedtls_asn1_named_data *parent_subject = &new_cert->subject;
    while (child_issuer != NULL) {
        if (esp_crt_copy_asn1(child_issuer, parent_subject) != 0) {
            ESP_LOGE(TAG, "Failed to copy ASN.1 data");
            mbedtls_x509_crt_free(new_cert);
            free(new_cert);
            return MBEDTLS_ERR_X509_ALLOC_FAILED;
        }
        child_issuer = child_issuer->next;
        if (child_issuer == NULL) {
            break;
        }

        if (parent_subject->next == NULL) {
            parent_subject->next = calloc(1, sizeof(mbedtls_asn1_named_data));
            if (parent_subject->next == NULL) {
                ESP_LOGE(TAG, "Failed to allocate memory for next issuer");
                mbedtls_x509_crt_free(new_cert);
                free(new_cert);
                return MBEDTLS_ERR_X509_ALLOC_FAILED;
            }
            parent_subject = parent_subject->next;
        }
    }

    // Set the parsed certificate as the candidate CA
    *candidate_cas = new_cert;
    return 0;
}
#endif /* CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_CROSS_SIGNED_VERIFY */

esp_err_t esp_crt_bundle_attach(void *conf)
{
    esp_err_t ret = ESP_OK;
    // If no bundle has been set by the user then use the bundle embedded in the binary
    if (s_crt_bundle == NULL) {
        ret = esp_crt_bundle_init(x509_crt_imported_bundle_bin_start, (uintptr_t)x509_crt_imported_bundle_bin_end - (uintptr_t)x509_crt_imported_bundle_bin_start);
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
        mbedtls_ssl_conf_verify(ssl_conf, esp_crt_verify_callback, NULL);
#if defined(CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_CROSS_SIGNED_VERIFY)
        mbedtls_ssl_conf_ca_cb(ssl_conf, esp_crt_ca_cb_callback, NULL);
#else
        mbedtls_ssl_conf_ca_chain(ssl_conf, (mbedtls_x509_crt*)&s_dummy_crt, NULL);
#endif /* CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_CROSS_SIGNED_VERIFY */
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
#if defined(CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_CROSS_SIGNED_VERIFY)
    return false; // Cross-signed verification does not use the dummy certificate
#else
    return ((ca_chain == &s_dummy_crt) ? true : false);
#endif // CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_CROSS_SIGNED_VERIFY
}
