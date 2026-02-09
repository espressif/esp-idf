/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "esp_random.h"

#include "mbedtls/aes.h"
#include "mbedtls/cmac.h"
#include "mbedtls/md.h"
#include "mbedtls/ecp.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/ccm.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"

#include "mesh/common.h"
#include "mesh/crypto.h"
#include "mesh/trace.h"
#include "mesh/config.h"

/* ECC key storage
 *
 * MbedTLS functions expect BIG-ENDIAN format.
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
    mbedtls_aes_context ctx;
    int ret;

    mbedtls_aes_init(&ctx);

    ret = mbedtls_aes_setkey_enc(&ctx, key->key, 128);
    if (ret != 0) {
        mbedtls_aes_free(&ctx);
        return -EIO;
    }

    ret = mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_ENCRYPT, plaintext, enc_data);
    mbedtls_aes_free(&ctx);

    return ret == 0 ? 0 : -EIO;
}

int bt_mesh_ccm_encrypt(const struct bt_mesh_key *key, uint8_t nonce[13],
                        const uint8_t *plaintext, size_t len,
                        const uint8_t *aad, size_t aad_len,
                        uint8_t *enc_data, size_t mic_size)
{
    mbedtls_ccm_context ctx;
    int ret;

    mbedtls_ccm_init(&ctx);

    ret = mbedtls_ccm_setkey(&ctx, MBEDTLS_CIPHER_ID_AES, key->key, 128);
    if (ret != 0) {
        mbedtls_ccm_free(&ctx);
        return -EIO;
    }

    ret = mbedtls_ccm_encrypt_and_tag(&ctx, len, nonce, 13, aad, aad_len,
                                       plaintext, enc_data, enc_data + len, mic_size);

    mbedtls_ccm_free(&ctx);
    return ret == 0 ? 0 : -EIO;
}

int bt_mesh_ccm_decrypt(const struct bt_mesh_key *key, uint8_t nonce[13],
                        const uint8_t *enc_data, size_t len,
                        const uint8_t *aad, size_t aad_len,
                        uint8_t *plaintext, size_t mic_size)
{
    mbedtls_ccm_context ctx;
    int ret;

    mbedtls_ccm_init(&ctx);

    ret = mbedtls_ccm_setkey(&ctx, MBEDTLS_CIPHER_ID_AES, key->key, 128);
    if (ret != 0) {
        mbedtls_ccm_free(&ctx);
        return -EIO;
    }

    ret = mbedtls_ccm_auth_decrypt(&ctx, len, nonce, 13, aad, aad_len,
                                    enc_data, plaintext, enc_data + len, mic_size);

    mbedtls_ccm_free(&ctx);
    return ret == 0 ? 0 : -EIO;
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

int bt_mesh_aes_cmac_mesh_key(const struct bt_mesh_key *key,
                              struct bt_mesh_sg *sg, size_t sg_len,
                              uint8_t mac[16])
{
    return bt_mesh_aes_cmac_raw_key(key->key, sg, sg_len, mac);
}

int bt_mesh_aes_cmac_raw_key(const uint8_t key[16], struct bt_mesh_sg *sg,
                             size_t sg_len, uint8_t mac[16])
{
    mbedtls_cipher_context_t ctx;
    const mbedtls_cipher_info_t *info;
    int ret;

    info = mbedtls_cipher_info_from_type(MBEDTLS_CIPHER_AES_128_ECB);
    if (info == NULL) {
        return -EIO;
    }

    mbedtls_cipher_init(&ctx);

    ret = mbedtls_cipher_setup(&ctx, info);
    if (ret != 0) {
        mbedtls_cipher_free(&ctx);
        return -EIO;
    }

    ret = mbedtls_cipher_cmac_starts(&ctx, key, 128);
    if (ret != 0) {
        mbedtls_cipher_free(&ctx);
        return -EIO;
    }

    for (; sg_len; sg_len--, sg++) {
        ret = mbedtls_cipher_cmac_update(&ctx, sg->data, sg->len);
        if (ret != 0) {
            mbedtls_cipher_free(&ctx);
            return -EIO;
        }
    }

    ret = mbedtls_cipher_cmac_finish(&ctx, mac);
    mbedtls_cipher_free(&ctx);

    return ret == 0 ? 0 : -EIO;
}

int bt_mesh_sha256_hmac_raw_key(const uint8_t key[32], struct bt_mesh_sg *sg,
                                size_t sg_len, uint8_t mac[32])
{
    mbedtls_md_context_t ctx;
    const mbedtls_md_info_t *info;
    int ret;

    info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    if (info == NULL) {
        return -EIO;
    }

    mbedtls_md_init(&ctx);

    ret = mbedtls_md_setup(&ctx, info, 1);  /* 1 = HMAC mode */
    if (ret != 0) {
        mbedtls_md_free(&ctx);
        return -EIO;
    }

    ret = mbedtls_md_hmac_starts(&ctx, key, 32);
    if (ret != 0) {
        mbedtls_md_free(&ctx);
        return -EIO;
    }

    for (; sg_len; sg_len--, sg++) {
        ret = mbedtls_md_hmac_update(&ctx, sg->data, sg->len);
        if (ret != 0) {
            mbedtls_md_free(&ctx);
            return -EIO;
        }
    }

    ret = mbedtls_md_hmac_finish(&ctx, mac);
    mbedtls_md_free(&ctx);

    return ret == 0 ? 0 : -EIO;
}

int bt_mesh_pub_key_gen(void)
{
    mbedtls_ecp_group grp;
    mbedtls_mpi d;
    mbedtls_ecp_point Q;
    uint8_t private_key_be[PRIV_KEY_SIZE];
    uint8_t public_key_be[PUB_KEY_SIZE];
    int ret;
    int err;

    dh_pair.is_ready = false;

    do {
        err = bt_mesh_rand(dh_pair.private_key, sizeof(dh_pair.private_key));
        if (err) {
            BT_ERR("Failed to generate random private key");
            return err;
        }
        /* Ensure the private key is valid (non-zero last bytes in LE) */
    } while (dh_pair.private_key[0] == 0 &&
             dh_pair.private_key[1] == 0);

    mbedtls_ecp_group_init(&grp);
    mbedtls_mpi_init(&d);
    mbedtls_ecp_point_init(&Q);

    ret = mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256R1);
    if (ret != 0) {
        goto cleanup;
    }

    /* Read private key into MPI (MbedTLS expects big-endian) */
    ret = mbedtls_mpi_read_binary(&d, dh_pair.private_key, PRIV_KEY_SIZE);
    if (ret != 0) {
        goto cleanup;
    }

    /* Compute public key: Q = d * G */
    ret = mbedtls_ecp_mul(&grp, &Q, &d, &grp.G, NULL, NULL);
    if (ret != 0) {
        goto cleanup;
    }

    /* Export public key point in big-endian */
    ret = mbedtls_mpi_write_binary(&Q.MBEDTLS_PRIVATE(X), dh_pair.public_key, 32);
    if (ret != 0) {
        goto cleanup;
    }

    ret = mbedtls_mpi_write_binary(&Q.MBEDTLS_PRIVATE(Y), dh_pair.public_key + 32, 32);
    if (ret != 0) {
        goto cleanup;
    }

    dh_pair.is_ready = true;

cleanup:
    mbedtls_ecp_group_free(&grp);
    mbedtls_mpi_free(&d);
    mbedtls_ecp_point_free(&Q);

    return ret == 0 ? 0 : -EIO;
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
    mbedtls_ecp_group grp;
    mbedtls_mpi d;
    mbedtls_ecp_point Q;
    int ret;

    dh_pair.is_ready = false;

    memcpy(dh_pair.private_key, pri_key, PRIV_KEY_SIZE);

    /* Compute public key from private key */
    mbedtls_ecp_group_init(&grp);
    mbedtls_mpi_init(&d);
    mbedtls_ecp_point_init(&Q);

    ret = mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256R1);
    if (ret != 0) {
        goto cleanup;
    }

    ret = mbedtls_mpi_read_binary(&d, pri_key, PRIV_KEY_SIZE);
    if (ret != 0) {
        goto cleanup;
    }

    /* Compute public key: Q = d * G (no RNG needed for deterministic computation) */
    ret = mbedtls_ecp_mul(&grp, &Q, &d, &grp.G, NULL, NULL);
    if (ret != 0) {
        goto cleanup;
    }

    /* Export public key point in big-endian */
    ret = mbedtls_mpi_write_binary(&Q.MBEDTLS_PRIVATE(X), dh_pair.public_key, 32);
    if (ret != 0) {
        goto cleanup;
    }

    ret = mbedtls_mpi_write_binary(&Q.MBEDTLS_PRIVATE(Y), dh_pair.public_key + 32, 32);
    if (ret != 0) {
        goto cleanup;
    }

    BT_DBG("Pubkey:%s", bt_hex(dh_pair.public_key, PUB_KEY_SIZE));
    BT_DBG("Privkey:%s", bt_hex(dh_pair.private_key, PRIV_KEY_SIZE));
    dh_pair.is_ready = true;

cleanup:
    mbedtls_ecp_group_free(&grp);
    mbedtls_mpi_free(&d);
    mbedtls_ecp_point_free(&Q);
}

bool bt_mesh_check_public_key_raw(const uint8_t key[64])
{
    mbedtls_ecp_group grp;
    mbedtls_ecp_point Q;
    uint8_t pub_key_be[PUB_KEY_SIZE];
    int ret;

    mbedtls_ecp_group_init(&grp);
    mbedtls_ecp_point_init(&Q);

    ret = mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256R1);
    if (ret != 0) {
        goto cleanup;
    }

    ret = mbedtls_mpi_read_binary(&Q.MBEDTLS_PRIVATE(X), dh_pair.public_key, 32);
    if (ret != 0) {
        goto cleanup;
    }

    ret = mbedtls_mpi_read_binary(&Q.MBEDTLS_PRIVATE(Y), dh_pair.public_key + 32, 32);
    if (ret != 0) {
        goto cleanup;
    }

    ret = mbedtls_mpi_lset(&Q.MBEDTLS_PRIVATE(Z), 1);
    if (ret != 0) {
        goto cleanup;
    }

    ret = mbedtls_ecp_check_pubkey(&grp, &Q);

cleanup:
    mbedtls_ecp_group_free(&grp);
    mbedtls_ecp_point_free(&Q);

    return ret == 0;
}

int bt_mesh_dhkey_gen_raw(const uint8_t *pub_key, const uint8_t *priv_key,
                          uint8_t *dhkey)
{
    mbedtls_ecp_group grp;
    mbedtls_mpi d;
    mbedtls_ecp_point Q;
    mbedtls_ecp_point result;
    int ret;

    mbedtls_ecp_group_init(&grp);
    mbedtls_mpi_init(&d);
    mbedtls_ecp_point_init(&Q);
    mbedtls_ecp_point_init(&result);

    ret = mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256R1);
    if (ret != 0) {
        goto cleanup;
    }

    ret = mbedtls_mpi_read_binary(&d, priv_key ? priv_key : dh_pair.private_key, PRIV_KEY_SIZE);
    if (ret != 0) {
        goto cleanup;
    }

    /* Load public key point */
    ret = mbedtls_mpi_read_binary(&Q.MBEDTLS_PRIVATE(X), dh_pair.public_key, 32);
    if (ret != 0) {
        goto cleanup;
    }

    ret = mbedtls_mpi_read_binary(&Q.MBEDTLS_PRIVATE(Y), dh_pair.public_key + 32, 32);
    if (ret != 0) {
        goto cleanup;
    }

    ret = mbedtls_mpi_lset(&Q.MBEDTLS_PRIVATE(Z), 1);
    if (ret != 0) {
        goto cleanup;
    }

    /* Check the peer's public key */
    ret = mbedtls_ecp_check_pubkey(&grp, &Q);
    if (ret != 0) {
        goto cleanup;
    }

    /* Calculate shared secret: result = d * Q (no RNG needed for deterministic computation) */
    ret = mbedtls_ecp_mul(&grp, &result, &d, &Q, NULL, NULL);
    if (ret != 0) {
        goto cleanup;
    }

    /* Export X coordinate as DH key (big-endian) */
    ret = mbedtls_mpi_write_binary(&result.MBEDTLS_PRIVATE(X), dhkey, DH_KEY_SIZE);

cleanup:
    mbedtls_ecp_group_free(&grp);
    mbedtls_mpi_free(&d);
    mbedtls_ecp_point_free(&Q);
    mbedtls_ecp_point_free(&result);

    return ret == 0 ? 0 : -EIO;
}

int bt_mesh_key_import(enum bt_mesh_key_type type, const uint8_t in[16],
                       struct bt_mesh_key *out)
{
    (void)type;  /* Not used for non-PSA implementation */
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
    memcpy(dst, src, sizeof(struct bt_mesh_key));
}

int bt_mesh_key_destroy(const struct bt_mesh_key *key)
{
    (void)key;  /* No cleanup needed for non-PSA implementation */
    return 0;
}

int bt_mesh_key_compare(const uint8_t raw_key[16], const struct bt_mesh_key *key)
{
    return memcmp(raw_key, key->key, 16);
}

int bt_mesh_encrypt_le(const uint8_t key[16], const uint8_t plaintext[16],
                       uint8_t enc_data[16])
{
    mbedtls_aes_context ctx;
    uint8_t tmp[16];
    int ret;

    BT_DBG("key %s plaintext %s", bt_hex(key, 16), bt_hex(plaintext, 16));

    mbedtls_aes_init(&ctx);

    /* Swap key bytes (LE to BE) */
    sys_memcpy_swap(tmp, key, 16);

    ret = mbedtls_aes_setkey_enc(&ctx, tmp, 128);
    if (ret != 0) {
        mbedtls_aes_free(&ctx);
        return -EINVAL;
    }

    /* Swap plaintext bytes and encrypt */
    sys_memcpy_swap(tmp, plaintext, 16);

    ret = mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_ENCRYPT, tmp, enc_data);
    mbedtls_aes_free(&ctx);

    if (ret != 0) {
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
    mbedtls_aes_context ctx;
    int ret;

    BT_DBG("key %s plaintext %s", bt_hex(key, 16), bt_hex(plaintext, 16));

    mbedtls_aes_init(&ctx);

    ret = mbedtls_aes_setkey_enc(&ctx, key, 128);
    if (ret != 0) {
        mbedtls_aes_free(&ctx);
        return -EINVAL;
    }

    ret = mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_ENCRYPT, plaintext, enc_data);
    mbedtls_aes_free(&ctx);

    if (ret != 0) {
        return -EINVAL;
    }

    BT_DBG("enc_data %s", bt_hex(enc_data, 16));
    return 0;
}
