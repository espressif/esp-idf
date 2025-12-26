/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileCopyrightText: 2023 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "esp_random.h"

#include <tinycrypt/aes.h>
#include <tinycrypt/constants.h>
#include <tinycrypt/cmac_mode.h>
#include <tinycrypt/ccm_mode.h>
#include <tinycrypt/ecc.h>
#include <tinycrypt/ecc_dh.h>
#include <tinycrypt/hmac.h>

#include "mesh/common.h"
#include "mesh/crypto.h"
#include "mesh/trace.h"

/* Internal DH key pair storage
 *
 * TinyCrypt's uECC functions expect BIG-ENDIAN format.
 */
static struct {
    bool is_ready;
    uint8_t private_key[PRIV_KEY_SIZE];  /* Big-endian */
    uint8_t public_key[PUB_KEY_SIZE];    /* Big-endian */
} dh_pair;

int bt_mesh_crypto_init(void)
{
    memset(&dh_pair, 0, sizeof(dh_pair));
    dh_pair.is_ready = false;
    return 0;
}

int bt_mesh_encrypt(const struct bt_mesh_key *key, const uint8_t plaintext[16],
                    uint8_t enc_data[16])
{
    struct tc_aes_key_sched_struct sched;

    if (tc_aes128_set_encrypt_key(&sched, key->key) == TC_CRYPTO_FAIL) {
        return -EIO;
    }

    if (tc_aes_encrypt(enc_data, plaintext, &sched) == TC_CRYPTO_FAIL) {
        return -EIO;
    }

    return 0;
}

int bt_mesh_ccm_encrypt(const struct bt_mesh_key *key, uint8_t nonce[13],
                        const uint8_t *plaintext, size_t len,
                        const uint8_t *aad, size_t aad_len,
                        uint8_t *enc_data, size_t mic_size)
{
    struct tc_ccm_mode_struct ccm;
    struct tc_aes_key_sched_struct sched;

    if (tc_aes128_set_encrypt_key(&sched, key->key) == TC_CRYPTO_FAIL) {
        return -EIO;
    }

    if (tc_ccm_config(&ccm, &sched, nonce, 13, mic_size) == TC_CRYPTO_FAIL) {
        return -EIO;
    }

    if (tc_ccm_generation_encryption(enc_data, len + mic_size, aad, aad_len,
                                     plaintext, len, &ccm) == TC_CRYPTO_FAIL) {
        return -EIO;
    }

    return 0;
}

int bt_mesh_ccm_decrypt(const struct bt_mesh_key *key, uint8_t nonce[13],
                        const uint8_t *enc_data, size_t len,
                        const uint8_t *aad, size_t aad_len,
                        uint8_t *plaintext, size_t mic_size)
{
    struct tc_ccm_mode_struct ccm;
    struct tc_aes_key_sched_struct sched;

    if (tc_aes128_set_encrypt_key(&sched, key->key) == TC_CRYPTO_FAIL) {
        return -EIO;
    }

    if (tc_ccm_config(&ccm, &sched, nonce, 13, mic_size) == TC_CRYPTO_FAIL) {
        return -EIO;
    }

    if (tc_ccm_decryption_verification(plaintext, len, aad, aad_len,
                                       enc_data, len + mic_size, &ccm) == TC_CRYPTO_FAIL) {
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

    memcpy(mesh_key.key, key, 16);
    return bt_mesh_ccm_encrypt(&mesh_key, nonce, plaintext, len, aad, aad_len,
                               enc_data, mic_size);
}

int bt_mesh_ccm_decrypt_raw_key(const uint8_t key[16], uint8_t nonce[13],
                                const uint8_t *enc_data, size_t len,
                                const uint8_t *aad, size_t aad_len,
                                uint8_t *plaintext, size_t mic_size)
{
    struct bt_mesh_key mesh_key;

    memcpy(mesh_key.key, key, 16);
    return bt_mesh_ccm_decrypt(&mesh_key, nonce, enc_data, len, aad, aad_len,
                               plaintext, mic_size);
}

int bt_mesh_aes_cmac_raw_key(const uint8_t key[16], struct bt_mesh_sg *sg,
                             size_t sg_len, uint8_t mac[16])
{
    struct tc_aes_key_sched_struct sched;
    struct tc_cmac_struct state;

    if (tc_cmac_setup(&state, key, &sched) == TC_CRYPTO_FAIL) {
        return -EIO;
    }

    for (; sg_len; sg_len--, sg++) {
        if (tc_cmac_update(&state, sg->data, sg->len) == TC_CRYPTO_FAIL) {
            return -EIO;
        }
    }

    if (tc_cmac_final(mac, &state) == TC_CRYPTO_FAIL) {
        return -EIO;
    }

    return 0;
}

int bt_mesh_aes_cmac_mesh_key(const struct bt_mesh_key *key,
                              struct bt_mesh_sg *sg, size_t sg_len,
                              uint8_t mac[16])
{
    return bt_mesh_aes_cmac_raw_key(key->key, sg, sg_len, mac);
}

int bt_mesh_sha256_hmac_raw_key(const uint8_t key[32], struct bt_mesh_sg *sg,
                                size_t sg_len, uint8_t mac[32])
{
    struct tc_hmac_state_struct h;

    if (tc_hmac_set_key(&h, key, 32) == TC_CRYPTO_FAIL) {
        return -EIO;
    }

    if (tc_hmac_init(&h) == TC_CRYPTO_FAIL) {
        return -EIO;
    }

    for (; sg_len; sg_len--, sg++) {
        if (tc_hmac_update(&h, sg->data, sg->len) == TC_CRYPTO_FAIL) {
            return -EIO;
        }
    }

    if (tc_hmac_final(mac, 32, &h) == TC_CRYPTO_FAIL) {
        return -EIO;
    }

    return 0;
}

int bt_mesh_pub_key_gen(void)
{
    int rc;

    /* Generate a random private key */
    do {
        rc = bt_mesh_rand(dh_pair.private_key, sizeof(dh_pair.private_key));
        if (rc) {
            BT_ERR("Failed to generate random private key");
            return rc;
        }
        /* Ensure the private key is valid (first bytes in BE shall be non-zero) */
    } while (dh_pair.private_key[0] == 0 &&
             dh_pair.private_key[1] == 0);

    /* Compute the public key from the private key */
    rc = uECC_compute_public_key(dh_pair.private_key, dh_pair.public_key, uECC_secp256r1());
    if (rc != TC_CRYPTO_SUCCESS) {
        dh_pair.is_ready = false;
        BT_ERR("Failed to compute public key");
        return -EIO;
    }

    dh_pair.is_ready = true;
    return 0;
}

const uint8_t *bt_mesh_pub_key_get_raw(void)
{
    if (!dh_pair.is_ready) {
        if (bt_mesh_pub_key_gen() != 0) {
            return NULL;
        }
    }

    return dh_pair.public_key;
}

void bt_mesh_set_private_key_raw(const uint8_t pri_key[32])
{
    int rc;

    memcpy(dh_pair.private_key, pri_key, PRIV_KEY_SIZE);

    /* Compute the public key from the provided private key */
    rc = uECC_compute_public_key(dh_pair.private_key, dh_pair.public_key, uECC_secp256r1());
    if (rc != TC_CRYPTO_SUCCESS) {
        dh_pair.is_ready = false;
        BT_ERR("Failed to compute public key from private key");
        return;
    }

    BT_DBG("Pubkey:%s", bt_hex(dh_pair.public_key, PUB_KEY_SIZE));
    BT_DBG("Privkey:%s", bt_hex(dh_pair.private_key, PRIV_KEY_SIZE));
    dh_pair.is_ready = true;
}

bool bt_mesh_check_public_key_raw(const uint8_t key[64])
{
    return uECC_valid_public_key(key, uECC_secp256r1()) == 0;
}

int bt_mesh_dhkey_gen_raw(const uint8_t *pub_key, const uint8_t *priv_key,
                          uint8_t *dhkey)
{
    if (uECC_valid_public_key(pub_key, uECC_secp256r1()) != 0) {
        BT_ERR("Public key is not valid");
        return -EIO;
    }

    if (uECC_shared_secret(pub_key, priv_key ? priv_key : dh_pair.private_key,
                           dhkey, uECC_secp256r1()) != TC_CRYPTO_SUCCESS) {
        BT_ERR("DHKey generation failed");
        return -EIO;
    }

    return 0;
}

int bt_mesh_key_import(enum bt_mesh_key_type type, const uint8_t in[16],
                       struct bt_mesh_key *out)
{
    (void)type;  /* Type is not used for TinyCrypt, just store raw key */
    memcpy(out->key, in, 16);
    return 0;
}

int bt_mesh_key_export(uint8_t out[16], const struct bt_mesh_key *in)
{
    memcpy(out, in->key, 16);
    return 0;
}

void bt_mesh_key_assign(struct bt_mesh_key *dst, const struct bt_mesh_key *src)
{
    memcpy(dst->key, src->key, 16);
}

int bt_mesh_key_destroy(const struct bt_mesh_key *key)
{
    (void)key;  /* Nothing to destroy for TinyCrypt */
    return 0;
}

int bt_mesh_key_compare(const uint8_t raw_key[16], const struct bt_mesh_key *key)
{
    return memcmp(raw_key, key->key, 16);
}

int bt_mesh_encrypt_le(const uint8_t key[16], const uint8_t plaintext[16],
                       uint8_t enc_data[16])
{
    struct tc_aes_key_sched_struct sched;
    uint8_t tmp[16];

    BT_DBG("key %s plaintext %s", bt_hex(key, 16), bt_hex(plaintext, 16));

    /* Swap key bytes (LE to BE) */
    sys_memcpy_swap(tmp, key, 16);

    if (tc_aes128_set_encrypt_key(&sched, tmp) == TC_CRYPTO_FAIL) {
        return -EINVAL;
    }

    /* Swap plaintext bytes (LE to BE) */
    sys_memcpy_swap(tmp, plaintext, 16);

    if (tc_aes_encrypt(enc_data, tmp, &sched) == TC_CRYPTO_FAIL) {
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
    struct tc_aes_key_sched_struct sched;

    BT_DBG("key %s plaintext %s", bt_hex(key, 16), bt_hex(plaintext, 16));

    if (tc_aes128_set_encrypt_key(&sched, key) == TC_CRYPTO_FAIL) {
        return -EINVAL;
    }

    if (tc_aes_encrypt(enc_data, plaintext, &sched) == TC_CRYPTO_FAIL) {
        return -EINVAL;
    }

    BT_DBG("enc_data %s", bt_hex(enc_data, 16));
    return 0;
}
