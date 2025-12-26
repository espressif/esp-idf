/*
 * SPDX-FileCopyrightText: 2023 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "esp_random.h"
#include "psa/crypto.h"

#include "mesh/common.h"
#include "mesh/crypto.h"
#include "mesh/trace.h"
#include "mesh/config.h"

/* PSA Key ID range for BLE Mesh */
#define BT_MESH_PSA_KEY_ID_MIN      0x0001A000
#define BT_MESH_PSA_KEY_ID_RANGE_SIZE \
    (2 * CONFIG_BLE_MESH_SUBNET_COUNT + 2 * CONFIG_BLE_MESH_APP_KEY_COUNT + 2)

/* Internal DH key pair storage
 *
 * PSA functions expect BIG-ENDIAN format.
 */
static struct {
    bool is_ready;
    psa_key_id_t priv_key_id;
    uint8_t public_key[PUB_KEY_SIZE + 1];  /* Big-endian, PSA format: 0x04 + X + Y */
} dh_pair;

/* Bitmap for tracking allocated key IDs */
static uint32_t pst_keys[(BT_MESH_PSA_KEY_ID_RANGE_SIZE + 31) / 32];

static psa_key_id_t keyid_alloc(void)
{
    for (int i = 0; i < BT_MESH_PSA_KEY_ID_RANGE_SIZE; i++) {
        int word = i / 32;
        int bit = i % 32;
        if (!(pst_keys[word] & (1U << bit))) {
            pst_keys[word] |= (1U << bit);
            return BT_MESH_PSA_KEY_ID_MIN + i;
        }
    }
    return PSA_KEY_ID_NULL;
}

static int keyid_free(psa_key_id_t key_id)
{
    if (key_id >= BT_MESH_PSA_KEY_ID_MIN &&
        key_id < BT_MESH_PSA_KEY_ID_MIN + BT_MESH_PSA_KEY_ID_RANGE_SIZE) {
        int idx = key_id - BT_MESH_PSA_KEY_ID_MIN;
        int word = idx / 32;
        int bit = idx % 32;
        pst_keys[word] &= ~(1U << bit);
        return 0;
    }
    return -EIO;
}

static void keyid_assign(psa_key_id_t key_id)
{
    if (key_id >= BT_MESH_PSA_KEY_ID_MIN &&
        key_id < BT_MESH_PSA_KEY_ID_MIN + BT_MESH_PSA_KEY_ID_RANGE_SIZE) {
        int idx = key_id - BT_MESH_PSA_KEY_ID_MIN;
        int word = idx / 32;
        int bit = idx % 32;
        pst_keys[word] |= (1U << bit);
    }
}

int bt_mesh_crypto_init(void)
{
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        BT_ERR("PSA crypto init failed: %d", status);
        return -EIO;
    }

    dh_pair.is_ready = false;
    dh_pair.priv_key_id = PSA_KEY_ID_NULL;
    memset(pst_keys, 0, sizeof(pst_keys));

    return 0;
}

int bt_mesh_encrypt(const struct bt_mesh_key *key, const uint8_t plaintext[16],
                    uint8_t enc_data[16])
{
    size_t output_len;
    psa_status_t status;

    status = psa_cipher_encrypt(key->key, PSA_ALG_ECB_NO_PADDING,
                                plaintext, 16, enc_data, 16, &output_len);

    if (status != PSA_SUCCESS || output_len != 16) {
        BT_ERR("PSA cipher encrypt failed: %d", status);
        return -EIO;
    }

    return 0;
}

int bt_mesh_ccm_encrypt(const struct bt_mesh_key *key, uint8_t nonce[13],
                        const uint8_t *plaintext, size_t len,
                        const uint8_t *aad, size_t aad_len,
                        uint8_t *enc_data, size_t mic_size)
{
    size_t output_len;
    psa_status_t status;
    psa_algorithm_t alg = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, mic_size);

    status = psa_aead_encrypt(key->key, alg, nonce, 13, aad, aad_len,
                              plaintext, len, enc_data, len + mic_size, &output_len);

    if (status != PSA_SUCCESS || output_len != len + mic_size) {
        BT_ERR("PSA AEAD encrypt failed: %d", status);
        return -EIO;
    }

    return 0;
}

int bt_mesh_ccm_decrypt(const struct bt_mesh_key *key, uint8_t nonce[13],
                        const uint8_t *enc_data, size_t len,
                        const uint8_t *aad, size_t aad_len,
                        uint8_t *plaintext, size_t mic_size)
{
    size_t output_len;
    psa_status_t status;
    psa_algorithm_t alg = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, mic_size);

    status = psa_aead_decrypt(key->key, alg, nonce, 13, aad, aad_len,
                              enc_data, len + mic_size, plaintext, len, &output_len);

    if (status != PSA_SUCCESS || output_len != len) {
        BT_ERR("PSA AEAD decrypt failed: %d", status);
        return -EIO;
    }

    return 0;
}

int bt_mesh_ccm_encrypt_raw_key(const uint8_t key[16], uint8_t nonce[13],
                                const uint8_t *plaintext, size_t len,
                                const uint8_t *aad, size_t aad_len,
                                uint8_t *enc_data, size_t mic_size)
{
    struct bt_mesh_key mesh_key;
    int err;

    err = bt_mesh_key_import(BT_MESH_KEY_TYPE_CCM, key, &mesh_key);
    if (err) {
        return err;
    }

    err = bt_mesh_ccm_encrypt(&mesh_key, nonce, plaintext, len, aad, aad_len,
                              enc_data, mic_size);

    psa_destroy_key(mesh_key.key);
    return err;
}

int bt_mesh_ccm_decrypt_raw_key(const uint8_t key[16], uint8_t nonce[13],
                                const uint8_t *enc_data, size_t len,
                                const uint8_t *aad, size_t aad_len,
                                uint8_t *plaintext, size_t mic_size)
{
    struct bt_mesh_key mesh_key;
    int err;

    err = bt_mesh_key_import(BT_MESH_KEY_TYPE_CCM, key, &mesh_key);
    if (err) {
        return err;
    }

    err = bt_mesh_ccm_decrypt(&mesh_key, nonce, enc_data, len, aad, aad_len,
                              plaintext, mic_size);

    psa_destroy_key(mesh_key.key);
    return err;
}

int bt_mesh_aes_cmac_mesh_key(const struct bt_mesh_key *key,
                              struct bt_mesh_sg *sg, size_t sg_len,
                              uint8_t mac[16])
{
    psa_mac_operation_t operation = PSA_MAC_OPERATION_INIT;
    psa_status_t status;
    size_t mac_len;

    status = psa_mac_sign_setup(&operation, key->key, PSA_ALG_CMAC);
    if (status != PSA_SUCCESS) {
        BT_ERR("PSA MAC setup failed: %d", status);
        return -EIO;
    }

    for (; sg_len; sg_len--, sg++) {
        status = psa_mac_update(&operation, sg->data, sg->len);
        if (status != PSA_SUCCESS) {
            psa_mac_abort(&operation);
            BT_ERR("PSA MAC update failed: %d", status);
            return -EIO;
        }
    }

    status = psa_mac_sign_finish(&operation, mac, 16, &mac_len);
    if (status != PSA_SUCCESS || mac_len != 16) {
        BT_ERR("PSA MAC finish failed: %d", status);
        return -EIO;
    }

    return 0;
}

int bt_mesh_aes_cmac_raw_key(const uint8_t key[16], struct bt_mesh_sg *sg,
                             size_t sg_len, uint8_t mac[16])
{
    struct bt_mesh_key key_id;
    int err;

    err = bt_mesh_key_import(BT_MESH_KEY_TYPE_CMAC, key, &key_id);
    if (err) {
        return err;
    }

    err = bt_mesh_aes_cmac_mesh_key(&key_id, sg, sg_len, mac);

    psa_destroy_key(key_id.key);

    return err;
}

int bt_mesh_sha256_hmac_raw_key(const uint8_t key[32], struct bt_mesh_sg *sg,
                                size_t sg_len, uint8_t mac[32])
{
    psa_mac_operation_t operation = PSA_MAC_OPERATION_INIT;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id;
    psa_status_t status;
    size_t mac_len;
    int err = 0;

    /* Import HMAC key */
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_MESSAGE);
    psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_VOLATILE);
    psa_set_key_algorithm(&attributes, PSA_ALG_HMAC(PSA_ALG_SHA_256));
    psa_set_key_type(&attributes, PSA_KEY_TYPE_HMAC);
    psa_set_key_bits(&attributes, 256);

    status = psa_import_key(&attributes, key, 32, &key_id);
    if (status != PSA_SUCCESS) {
        BT_ERR("PSA import HMAC key failed: %d", status);
        return -EIO;
    }
    psa_reset_key_attributes(&attributes);

    status = psa_mac_sign_setup(&operation, key_id, PSA_ALG_HMAC(PSA_ALG_SHA_256));
    if (status != PSA_SUCCESS) {
        BT_ERR("PSA HMAC setup failed: %d", status);
        err = -EIO;
        goto end;
    }

    for (; sg_len; sg_len--, sg++) {
        status = psa_mac_update(&operation, sg->data, sg->len);
        if (status != PSA_SUCCESS) {
            psa_mac_abort(&operation);
            BT_ERR("PSA HMAC update failed: %d", status);
            err = -EIO;
            goto end;
        }
    }

    status = psa_mac_sign_finish(&operation, mac, 32, &mac_len);
    if (status != PSA_SUCCESS || mac_len != 32) {
        BT_ERR("PSA HMAC finish failed: %d", status);
        err = -EIO;
    }

end:
    psa_destroy_key(key_id);
    return err;
}

int bt_mesh_pub_key_gen(void)
{
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_status_t status;
    uint8_t private_key[PRIV_KEY_SIZE];
    size_t key_len;
    int err;

    /* Destroy any existing key */
    if (dh_pair.priv_key_id != PSA_KEY_ID_NULL) {
        psa_destroy_key(dh_pair.priv_key_id);
        dh_pair.priv_key_id = PSA_KEY_ID_NULL;
    }
    dh_pair.is_ready = false;

    /* Generate a random private key (in little-endian format for storage) */
    do {
        err = bt_mesh_rand(private_key, sizeof(private_key));
        if (err) {
            BT_ERR("Failed to generate random private key");
            return err;
        }
        /* Ensure the private key is valid (non-zero first bytes in BE) */
    } while (private_key[0] == 0 && private_key[1] == 0);

    /* Configure key attributes for ECDH with P-256 */
    psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_DERIVE);
    psa_set_key_lifetime(&key_attributes, PSA_KEY_LIFETIME_VOLATILE);
    psa_set_key_algorithm(&key_attributes, PSA_ALG_ECDH);
    psa_set_key_type(&key_attributes, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_bits(&key_attributes, 256);

    /* Import the private key */
    status = psa_import_key(&key_attributes, private_key, sizeof(private_key),
                            &dh_pair.priv_key_id);
    if (status != PSA_SUCCESS) {
        BT_ERR("PSA import private key failed: %d", status);
        psa_reset_key_attributes(&key_attributes);
        return -EIO;
    }

    /* Export public key (PSA computes it from the private key) */
    status = psa_export_public_key(dh_pair.priv_key_id, dh_pair.public_key,
                                   sizeof(dh_pair.public_key), &key_len);
    if (status != PSA_SUCCESS || key_len != PUB_KEY_SIZE + 1) {
        BT_ERR("PSA export public key failed: %d", status);
        psa_destroy_key(dh_pair.priv_key_id);
        dh_pair.priv_key_id = PSA_KEY_ID_NULL;
        psa_reset_key_attributes(&key_attributes);
        return -EIO;
    }

    dh_pair.is_ready = true;
    psa_reset_key_attributes(&key_attributes);

    return 0;
}

const uint8_t *bt_mesh_pub_key_get_raw(void)
{
    if (!dh_pair.is_ready) {
        if (bt_mesh_pub_key_gen() != 0) {
            return NULL;
        }
    }

    return &dh_pair.public_key[1];
}

void bt_mesh_set_private_key_raw(const uint8_t pri_key[32])
{
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_status_t status;
    size_t key_len;

    /* Destroy any existing key */
    if (dh_pair.priv_key_id != PSA_KEY_ID_NULL) {
        psa_destroy_key(dh_pair.priv_key_id);
        dh_pair.priv_key_id = PSA_KEY_ID_NULL;
    }
    dh_pair.is_ready = false;

    /* Import the provided private key */
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DERIVE);
    psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_VOLATILE);
    psa_set_key_algorithm(&attributes, PSA_ALG_ECDH);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_bits(&attributes, 256);

    status = psa_import_key(&attributes, pri_key, PRIV_KEY_SIZE, &dh_pair.priv_key_id);
    if (status != PSA_SUCCESS) {
        BT_ERR("PSA import private key failed: %d", status);
        psa_reset_key_attributes(&attributes);
        return;
    }

    /* Export public key (PSA computes it from the private key) */
    status = psa_export_public_key(dh_pair.priv_key_id, dh_pair.public_key,
                                   sizeof(dh_pair.public_key), &key_len);
    if (status != PSA_SUCCESS || key_len != PUB_KEY_SIZE + 1) {
        BT_ERR("PSA export public key failed: %d", status);
        psa_destroy_key(dh_pair.priv_key_id);
        dh_pair.priv_key_id = PSA_KEY_ID_NULL;
        psa_reset_key_attributes(&attributes);
        return;
    }

    BT_DBG("Pubkey:%s", bt_hex(&dh_pair.public_key[1], PUB_KEY_SIZE));
    BT_DBG("Privkey:%s", bt_hex(pri_key, PRIV_KEY_SIZE));
    dh_pair.is_ready = true;
    psa_reset_key_attributes(&attributes);
}

bool bt_mesh_check_public_key_raw(const uint8_t key[64])
{
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id;
    psa_status_t status;
    uint8_t pub_be[PUB_KEY_SIZE + 1];

    /* PSA requires 0x04 prefix for uncompressed point */
    pub_be[0] = 0x04;
    /* Convert from little-endian to big-endian */
    memcpy(&pub_be[1], key, 32);
    memcpy(&pub_be[33], key + 32, 32);

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_VERIFY_HASH);
    psa_set_key_algorithm(&attributes, PSA_ALG_ECDSA(PSA_ALG_SHA_256));
    psa_set_key_type(&attributes, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_bits(&attributes, 256);

    status = psa_import_key(&attributes, pub_be, sizeof(pub_be), &key_id);
    psa_reset_key_attributes(&attributes);

    if (status != PSA_SUCCESS) {
        return false;
    }

    psa_destroy_key(key_id);
    return true;
}

int bt_mesh_dhkey_gen_raw(const uint8_t *pub_key, const uint8_t *priv_key,
                          uint8_t *dhkey)
{
    psa_key_id_t priv_key_id = PSA_KEY_ID_NULL;
    uint8_t public_key_be[PUB_KEY_SIZE + 1];
    psa_status_t status;
    size_t dh_key_len;
    int err = 0;

    if (priv_key) {
        psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

        /* Import custom private key */
        psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DERIVE);
        psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_VOLATILE);
        psa_set_key_algorithm(&attributes, PSA_ALG_ECDH);
        psa_set_key_type(&attributes, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
        psa_set_key_bits(&attributes, 256);

        status = psa_import_key(&attributes, priv_key, PRIV_KEY_SIZE, &priv_key_id);
        psa_reset_key_attributes(&attributes);

        if (status != PSA_SUCCESS) {
            BT_ERR("PSA import private key failed: %d", status);
            return -EIO;
        }
    } else {
        priv_key_id = dh_pair.priv_key_id;
    }

    /* Prepare public key with 0x04 prefix in big-endian format */
    public_key_be[0] = 0x04;
    /* Convert from little-endian to big-endian */
    memcpy(public_key_be + 1, pub_key, 32);
    memcpy(public_key_be + 33, pub_key + 32, 32);

    /* Calculate shared secret */
    status = psa_raw_key_agreement(PSA_ALG_ECDH, priv_key_id, public_key_be,
                                   PUB_KEY_SIZE + 1, dhkey, DH_KEY_SIZE, &dh_key_len);

    if (status != PSA_SUCCESS || dh_key_len != DH_KEY_SIZE) {
        BT_ERR("PSA ECDH failed: %d", status);
        err = -EIO;
    }

    if (priv_key && priv_key_id != PSA_KEY_ID_NULL) {
        psa_destroy_key(priv_key_id);
    }

    return err;
}

int bt_mesh_key_import(enum bt_mesh_key_type type, const uint8_t in[16],
                       struct bt_mesh_key *out)
{
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_status_t status;
    psa_key_id_t key_id = PSA_KEY_ID_NULL;

    switch (type) {
    case BT_MESH_KEY_TYPE_ECB:
        psa_set_key_lifetime(&key_attributes, PSA_KEY_LIFETIME_VOLATILE);
        psa_set_key_usage_flags(&key_attributes,
            PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
        psa_set_key_algorithm(&key_attributes, PSA_ALG_ECB_NO_PADDING);
        break;
    case BT_MESH_KEY_TYPE_CCM:
        psa_set_key_lifetime(&key_attributes, PSA_KEY_LIFETIME_VOLATILE);
        psa_set_key_usage_flags(&key_attributes,
            PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
        psa_set_key_algorithm(&key_attributes,
            PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(PSA_ALG_CCM, 4));
        break;
    case BT_MESH_KEY_TYPE_CMAC:
        psa_set_key_lifetime(&key_attributes, PSA_KEY_LIFETIME_VOLATILE);
        psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_SIGN_MESSAGE);
        psa_set_key_algorithm(&key_attributes, PSA_ALG_CMAC);
        break;
    case BT_MESH_KEY_TYPE_NET:
#if CONFIG_BT_SETTINGS
        key_id = keyid_alloc();
        if (key_id == PSA_KEY_ID_NULL) {
            return -ENOMEM;
        }
        psa_set_key_lifetime(&key_attributes, PSA_KEY_LIFETIME_PERSISTENT);
        psa_set_key_id(&key_attributes, key_id);
#else
        psa_set_key_lifetime(&key_attributes, PSA_KEY_LIFETIME_VOLATILE);
#endif
        psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_EXPORT);
        break;
    case BT_MESH_KEY_TYPE_APP:
    case BT_MESH_KEY_TYPE_DEV:
#if CONFIG_BT_SETTINGS
        key_id = keyid_alloc();
        if (key_id == PSA_KEY_ID_NULL) {
            return -ENOMEM;
        }
        psa_set_key_lifetime(&key_attributes, PSA_KEY_LIFETIME_PERSISTENT);
        psa_set_key_id(&key_attributes, key_id);
#else
        psa_set_key_lifetime(&key_attributes, PSA_KEY_LIFETIME_VOLATILE);
#endif
        psa_set_key_usage_flags(&key_attributes,
            PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT | PSA_KEY_USAGE_EXPORT);
        psa_set_key_algorithm(&key_attributes,
            PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(PSA_ALG_CCM, 4));
        break;
    default:
        return -EINVAL;
    }

    psa_set_key_type(&key_attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&key_attributes, 128);

    status = psa_import_key(&key_attributes, in, 16, &out->key);
    if (status == PSA_ERROR_ALREADY_EXISTS) {
        BT_WARN("Key 0x%04x already exists, destroying and reimporting", key_id);
        psa_destroy_key(key_id);
        status = psa_import_key(&key_attributes, in, 16, &out->key);
    }

    psa_reset_key_attributes(&key_attributes);

    if (status != PSA_SUCCESS) {
        BT_ERR("PSA import key failed: %d", status);
        if (key_id != PSA_KEY_ID_NULL) {
            keyid_free(key_id);
        }
        return -EIO;
    }

    return 0;
}

int bt_mesh_key_export(uint8_t out[16], const struct bt_mesh_key *in)
{
    size_t data_length;
    psa_status_t status;

    status = psa_export_key(in->key, out, 16, &data_length);
    if (status != PSA_SUCCESS || data_length != 16) {
        BT_ERR("PSA export key failed: %d", status);
        return -EIO;
    }

    return 0;
}

void bt_mesh_key_assign(struct bt_mesh_key *dst, const struct bt_mesh_key *src)
{
    memcpy(dst, src, sizeof(struct bt_mesh_key));
#if CONFIG_BT_SETTINGS
    keyid_assign(dst->key);
#endif
}

int bt_mesh_key_destroy(const struct bt_mesh_key *key)
{
    psa_status_t status;

    status = psa_destroy_key(key->key);
    if (status != PSA_SUCCESS) {
        BT_ERR("PSA destroy key failed: %d", status);
        return -EIO;
    }

#if CONFIG_BT_SETTINGS
    return keyid_free(key->key);
#else
    return 0;
#endif
}

int bt_mesh_key_compare(const uint8_t raw_key[16], const struct bt_mesh_key *key)
{
    uint8_t out[16];
    int err;

    err = bt_mesh_key_export(out, key);
    if (err) {
        return err;
    }

    return memcmp(out, raw_key, 16);
}

int bt_mesh_encrypt_le(const uint8_t key[16], const uint8_t plaintext[16],
                       uint8_t enc_data[16])
{
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id;
    psa_status_t status;
    uint8_t tmp[16];
    size_t output_len;

    BT_DBG("key %s plaintext %s", bt_hex(key, 16), bt_hex(plaintext, 16));

    /* Swap key bytes (LE to BE) */
    sys_memcpy_swap(tmp, key, 16);

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_ECB_NO_PADDING);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 128);

    status = psa_import_key(&attributes, tmp, 16, &key_id);
    psa_reset_key_attributes(&attributes);

    if (status != PSA_SUCCESS) {
        BT_ERR("PSA import key failed: %d", status);
        return -EINVAL;
    }

    /* Swap plaintext bytes and encrypt */
    sys_memcpy_swap(tmp, plaintext, 16);

    status = psa_cipher_encrypt(key_id, PSA_ALG_ECB_NO_PADDING,
                                tmp, 16, enc_data, 16, &output_len);

    psa_destroy_key(key_id);

    if (status != PSA_SUCCESS || output_len != 16) {
        BT_ERR("PSA encrypt failed: %d", status);
        return -EINVAL;
    }

    /* Swap result bytes (BE to LE) */
    sys_mem_swap(enc_data, 16);

    BT_DBG("enc_data %s", bt_hex(enc_data, 16));
    return 0;
}

int bt_mesh_encrypt_be(const uint8_t key[16], const uint8_t plaintext[16],
                       uint8_t enc_data[16])
{
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id;
    psa_status_t status;
    size_t output_len;

    BT_DBG("key %s plaintext %s", bt_hex(key, 16), bt_hex(plaintext, 16));

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_ECB_NO_PADDING);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 128);

    status = psa_import_key(&attributes, key, 16, &key_id);
    psa_reset_key_attributes(&attributes);

    if (status != PSA_SUCCESS) {
        BT_ERR("PSA import key failed: %d", status);
        return -EINVAL;
    }

    status = psa_cipher_encrypt(key_id, PSA_ALG_ECB_NO_PADDING,
                                plaintext, 16, enc_data, 16, &output_len);

    psa_destroy_key(key_id);

    if (status != PSA_SUCCESS || output_len != 16) {
        BT_ERR("PSA encrypt failed: %d", status);
        return -EINVAL;
    }

    BT_DBG("enc_data %s", bt_hex(enc_data, 16));
    return 0;
}

#if 0
int bt_mesh_rand(void *buf, size_t len)
{
    psa_status_t status;

    if (buf == NULL || len == 0) {
        return -EINVAL;
    }

    status = psa_generate_random(buf, len);
    return status == PSA_SUCCESS ? 0 : -EIO;
}
#endif
