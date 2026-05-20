/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "sdkconfig.h"
#include "esp_heap_caps.h"
#include "esp_heap_caps_init.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_types.h"
#include "esp_mac.h"
#include "esp_random.h"
#include "esp_task.h"
#include "riscv/interrupt.h"
#include "esp_attr.h"
#include "esp_phy_init.h"
#include "esp_bt.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_pm.h"
#include "esp_ipc.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/esp_clk.h"
#include "soc/rtc.h"
#include "soc/soc_memory_layout.h"
#include "rom/ets_sys.h"
#if CONFIG_SW_COEXIST_ENABLE
#include "private/esp_coexist_internal.h"
#endif
#include "esp_private/phy.h"
#include "soc/pmu_reg.h"
// #include "soc/lp_clkrst_reg.h"
// #include "soc/lpperi_reg.h"
#include "modem/modem_syscon_reg.h"
#include "modem/modem_lpcon_reg.h"
#include "esp_private/esp_modem_clock.h"

// #include "nimble/nimble_npl.h"
// #include "nimble/nimble_port_freertos.h"

#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
#include "esp_private/sleep_modem.h"
#include "esp_private/sleep_retention.h"
#endif // CONFIG_FREERTOS_USE_TICKLESS_IDLE

#include "btdm_osal.h"
#include "btdm_endian.h"
#if CONFIG_BT_SMP_CRYPTO_STACK_MBEDTLS
#include "psa/crypto.h"
#endif
#if !CONFIG_BT_SMP_CRYPTO_STACK_MBEDTLS
#include "tinycrypt/ecc_dh.h"
#include "tinycrypt/ecc.h"
#include "tinycrypt/constants.h"
#include "tinycrypt/sha256.h"
#include "tinycrypt/hmac.h"
#endif

#if UC_BT_CTRL_BR_EDR_IS_ENABLE

#define BREDR_P256_COORD_LEN    32U
#define BREDR_P192_COORD_LEN    24U
#define BREDR_PUB_KEY_LEN_P256  (1U + BREDR_P256_COORD_LEN * 2)
#define BREDR_PUB_KEY_LEN_P192  (1U + BREDR_P192_COORD_LEN * 2)
#define BREDR_LOG_TAG       "BREDR_INIT"
#if CONFIG_BT_SMP_CRYPTO_STACK_MBEDTLS
static const char *TAG_BREDR_CRYPTO = "bredr_crypto";
#endif

/* Types definition
 ************************************************************************
 */

/* VHCI function interface */
typedef struct vhci_host_callback {
    void (*notify_host_send_available)(void);               /*!< callback used to notify that the host can send packet to controller */
    int (*notify_host_recv)(uint8_t *data, uint16_t len);   /*!< callback used to notify that the controller has a packet to send to the host*/
} vhci_host_callback_t;

typedef int (*bredr_ctrl_callback_t)(void);

/* External functions or values
 ************************************************************************
 */
extern void btdm_rf_bb_init_phase2(void); // shall be called after PHY/RF is enabled
extern const char *co_orca_get_git_version_str(void);
extern void esp_bt_modem_clock_reset(void);

/* VHCI */
extern bool API_vhci_host_check_send_available(void);
extern void API_vhci_host_send_packet(uint8_t *data, uint16_t len);
extern int API_vhci_host_register_callback(const vhci_host_callback_t *callback);

/* Initialise and De-initialise */
extern int bredr_controller_init(void *cfg);
extern void bredr_controller_deinit(void);
extern int bredr_controller_enable(void);
extern void bredr_controller_disable(void);
extern int bredr_controller_env_init(void *cfg);
extern void bredr_controller_env_deinit(void);
extern void bredr_register_setup_callback(bredr_ctrl_callback_t cbk);
extern void bredr_register_ext_dep_callback(bredr_ctrl_callback_t cbk);
extern int bredr_ctrl_feat_sync_en(void);
extern int bredr_ctrl_feat_apb_en(void);
extern int bredr_ctrl_feat_bcst_enc_en(void);
extern int bredr_ctrl_feat_pca_en(void);
extern int bredr_ctrl_feat_hold_en(void);
extern int bredr_ctrl_feat_cpb_rx_en(void);
extern int bredr_ctrl_feat_cpb_tx_en(void);
extern int bredr_ctrl_feat_dtm_en(void);
extern int bredr_ctrl_feat_test_en(void);
extern int bredr_ctrl_feat_lk_store_en(void);
extern int bredr_ctrl_feat_coex_en(void);

extern const char *co_orca_get_git_version_str(void);
/* Shutdown */
extern void bredr_controller_shutdown(void);

extern esp_bt_controller_status_t s_btdm_controller_status;

#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
extern const sleep_retention_entries_config_t *r_bredr_mac_retention_link_get(uint8_t *size);
#endif // CONFIG_FREERTOS_USE_TICKLESS_IDLE

static uint32_t esp_random_wrapper(void)
{
    return esp_random();
}
typedef struct bredr_ecc_ops {
    int (*ecc256_gen_key_pair)(uint8_t *public, uint8_t *priv);
    int (*ecc256_gen_dhkey)(const uint8_t *remote_pub_key_x, const uint8_t *remote_pub_key_y, const uint8_t *local_priv_key, uint8_t *dhkey);
    int (*ecc192_gen_key_pair)(uint8_t *public, uint8_t *priv);
    int (*ecc192_gen_dhkey)(const uint8_t *remote_pub_key_x, const uint8_t *remote_pub_key_y, const uint8_t *local_priv_key, uint8_t *dhkey);
} bredr_ecc_ops_t;

typedef struct bredr_sha_ops {
    int (*sha256)(const uint8_t *msg, uint32_t msg_len, uint8_t *digest);
    int (*hmac)(const uint8_t *key, uint32_t key_len, const uint8_t *msg, uint32_t msg_len, uint8_t *digest);
} bredr_sha_ops_t;

typedef struct bredr_crypto_ops {
    bredr_ecc_ops_t ecc_ops;
    bredr_sha_ops_t sha_ops;
} bredr_crypto_ops_t;

static int bredr_gen_keypair_ecc256(uint8_t *public_key, uint8_t *private_key);
static int bredr_gen_keypair_ecc192(uint8_t *public_key, uint8_t *private_key);
static int
bredr_gen_dhkey_ecc256(const uint8_t *peer_pub_key_x, const uint8_t *peer_pub_key_y, const uint8_t *our_priv_key, uint8_t *out_dhkey);
static int
bredr_gen_dhkey_ecc192(const uint8_t *peer_pub_key_x, const uint8_t *peer_pub_key_y, const uint8_t *our_priv_key, uint8_t *out_dhkey);
static int bredr_sha256(const uint8_t *msg, uint32_t msg_len, uint8_t *digest);
static int bredr_hmac(const uint8_t *key, uint32_t key_len, const uint8_t *msg, uint32_t msg_len, uint8_t *digest);

const bredr_ecc_ops_t g_bredr_ecc_ops = {
    .ecc256_gen_key_pair = bredr_gen_keypair_ecc256,
    .ecc256_gen_dhkey = bredr_gen_dhkey_ecc256,
    .ecc192_gen_key_pair = bredr_gen_keypair_ecc192,
    .ecc192_gen_dhkey = bredr_gen_dhkey_ecc192,
};

const bredr_sha_ops_t g_bredr_sha_ops = {
    .sha256 = bredr_sha256,
    .hmac = bredr_hmac,
};

const bredr_crypto_ops_t bredr_crypto_ops = {
    .ecc_ops = g_bredr_ecc_ops,
    .sha_ops = g_bredr_sha_ops,
};

extern int bredr_register_crypto_funcs(bredr_crypto_ops_t *);

struct bredr_bb_cca_set_params {
    uint32_t enable;
    uint32_t thresh_low;
    uint32_t power_drop_0;
    uint32_t power_high_0;
    uint32_t power_low_0;
    uint32_t thresh_high_en;
    uint32_t thresh_high;
    uint32_t power_drop1;
    uint32_t power_high_1;
    uint32_t power_low_1;
};

struct bredr_bb_ops {
    bool (*cca_fifo_empty)(void);
    bool (*cca_fifo_full)(void);
    uint32_t (*cca_fifo_count)(void);
    uint32_t (*cca_fifo_read)(void);
    void (*cca_period)(uint32_t ru_time, uint32_t cca_time);
    void (*cca_set)(struct bredr_bb_cca_set_params *param);
    void (*cca_fifo_reset)(void);
    void (*set_tx_on_delay)(uint32_t delay_us);
};

extern bool bredr_tx_cca_fifo_empty(void);
extern bool bredr_tx_cca_fifo_full(void);
extern uint32_t bredr_tx_cca_fifo_count(void);
extern uint32_t bredr_tx_cca_fifo_read(void);
extern void bredr_tx_cca_period(uint32_t ru_time, uint32_t cca_time);
extern void bredr_tx_cca_set(uint32_t enable,
                             uint32_t thresh_low,
                             uint32_t power_drop_0,
                             uint32_t power_high_0,
                             uint32_t power_low_0,
                             uint32_t thresh_high_en,
                             uint32_t thresh_high,
                             uint32_t power_drop1,
                             uint32_t power_high_1,
                             uint32_t power_low_1);
extern void bredr_tx_cca_fifo_reset(void);
extern void bredr_set_tx_on_delay(uint32_t delay_us);
static void bredr_tx_cca_set_wapper(struct bredr_bb_cca_set_params *param);

struct bredr_bb_ops s_bredr_bb_ops = {
    .cca_fifo_empty = bredr_tx_cca_fifo_empty,
    .cca_fifo_full = bredr_tx_cca_fifo_full,
    .cca_fifo_count = bredr_tx_cca_fifo_count,
    .cca_fifo_read = bredr_tx_cca_fifo_read,
    .cca_period = bredr_tx_cca_period,
    .cca_set = bredr_tx_cca_set_wapper,
    .cca_fifo_reset = bredr_tx_cca_fifo_reset,
    .set_tx_on_delay = bredr_set_tx_on_delay,
};

extern int bredr_register_bb_funcs(struct bredr_bb_ops *ops);

#if CONFIG_BT_SMP_CRYPTO_STACK_MBEDTLS
/* Same PSA ECDH pattern as components/bt/controller/esp32c6/bt.c (ble_sm_alg_*). */

static int bredr_psa_gen_keypair(bool p256, uint8_t *public_key, uint8_t *private_key)
{
    const size_t coord_len = p256 ? BREDR_P256_COORD_LEN : BREDR_P192_COORD_LEN;
    const size_t priv_len = coord_len;
    const size_t pub_len = p256 ? BREDR_PUB_KEY_LEN_P256 : BREDR_PUB_KEY_LEN_P192;

    psa_key_id_t key_id = 0;
    psa_status_t status;
    psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
    uint8_t pk[65];

    psa_set_key_type(&attr, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_bits(&attr, p256 ? 256 : 192);
    psa_set_key_algorithm(&attr, PSA_ALG_ECDH);
    psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_DERIVE | PSA_KEY_USAGE_EXPORT);

    status = psa_generate_key(&attr, &key_id);
    psa_reset_key_attributes(&attr);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG_BREDR_CRYPTO, "psa_generate_key failed: %d", (int)status);
        return -1;
    }

    size_t olen = 0;
    status = psa_export_public_key(key_id, pk, sizeof(pk), &olen);
    if (status != PSA_SUCCESS || olen != pub_len) {
        ESP_LOGE(TAG_BREDR_CRYPTO, "psa_export_public_key failed: %d olen %zu (expect %zu)",
                 (int)status, olen, pub_len);
        psa_destroy_key(key_id);
        return -1;
    }

    status = psa_export_key(key_id, private_key, priv_len, &olen);
    if (status != PSA_SUCCESS || olen != priv_len) {
        ESP_LOGE(TAG_BREDR_CRYPTO, "psa_export_key failed: %d olen %zu", (int)status, olen);
        psa_destroy_key(key_id);
        return -1;
    }

    psa_destroy_key(key_id);

    /* pk: 0x04 || X || Y (big-endian); stack expects per-coordinate endian swap like TinyCrypt path. */
    btdm_swap_buf(public_key, &pk[1], coord_len);
    btdm_swap_buf(&public_key[coord_len], &pk[1 + coord_len], coord_len);
    btdm_swap_in_place(private_key, priv_len);
    return 0;
}

static int bredr_psa_gen_dhkey(bool p256, const uint8_t *peer_pub_key_x, const uint8_t *peer_pub_key_y,
                               const uint8_t *our_priv_key, uint8_t *out_dhkey)
{
    const size_t coord_len = p256 ? BREDR_P256_COORD_LEN : BREDR_P192_COORD_LEN;
    const size_t priv_len = coord_len;
    const size_t pub_len = p256 ? BREDR_PUB_KEY_LEN_P256 : BREDR_PUB_KEY_LEN_P192;

    uint8_t priv[32];
    uint8_t pk[65];
    uint8_t dh[32];

    btdm_swap_buf(priv, our_priv_key, priv_len);
    pk[0] = 0x04;
    btdm_swap_buf(&pk[1], peer_pub_key_x, coord_len);
    btdm_swap_buf(&pk[1 + coord_len], peer_pub_key_y, coord_len);

    psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;
    psa_status_t status;

    psa_set_key_type(&attr, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_bits(&attr, p256 ? 256 : 192);
    psa_set_key_algorithm(&attr, PSA_ALG_ECDH);
    psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_DERIVE);

    status = psa_import_key(&attr, priv, priv_len, &key_id);
    psa_reset_key_attributes(&attr);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG_BREDR_CRYPTO, "psa_import_key failed: %d", (int)status);
        return -1;
    }

    size_t out_len = 0;
    status = psa_raw_key_agreement(PSA_ALG_ECDH, key_id, pk, pub_len, dh, sizeof(dh), &out_len);
    psa_destroy_key(key_id);

    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG_BREDR_CRYPTO, "psa_raw_key_agreement failed: %d", (int)status);
        return -1;
    }

    if (out_len != coord_len) {
        ESP_LOGE(TAG_BREDR_CRYPTO, "unexpected shared secret length %zu (expect %zu)", out_len, coord_len);
        return -1;
    }

    btdm_swap_buf(out_dhkey, dh, coord_len);
    return 0;
}
#else /* !CONFIG_BT_SMP_CRYPTO_STACK_MBEDTLS */
static int bredr_ecc_rand(uint8_t *dst, unsigned int size)
{
    uint8_t *buf_bytes = (uint8_t *)dst;
    while (size > 0) {
        uint32_t word = esp_random_wrapper();
        uint32_t to_copy = MIN(sizeof(word), size);
        memcpy(buf_bytes, &word, to_copy);
        buf_bytes += to_copy;
        size -= to_copy;
    }

    return 1;
}
#endif /* !CONFIG_BT_SMP_CRYPTO_STACK_MBEDTLS */

static int bredr_gen_keypair(bool p256, uint8_t *public_key, uint8_t *private_key)
{
#if CONFIG_BT_SMP_CRYPTO_STACK_MBEDTLS
    return bredr_psa_gen_keypair(p256, public_key, private_key);
#else
    uint8_t pub[64];
    size_t buf_len;

    /* always register rand function again */
    uECC_set_rng(bredr_ecc_rand);

    if (p256) {
        buf_len = 32;
        if (uECC_make_key(pub, private_key, uECC_secp256r1()) != TC_CRYPTO_SUCCESS) {
            return -1;
        }
    } else {
        buf_len = 24;
        if (uECC_make_key(pub, private_key, uECC_secp192r1()) != TC_CRYPTO_SUCCESS) {
            return -1;
        }
    }

    btdm_swap_buf(public_key, pub, buf_len);
    btdm_swap_buf(&public_key[buf_len], &pub[buf_len], buf_len);
    btdm_swap_in_place(private_key, buf_len);
#endif

    return 0;
}

static int
bredr_gen_dhkey(bool p256, const uint8_t *peer_pub_key_x, const uint8_t *peer_pub_key_y, const uint8_t *our_priv_key, uint8_t *out_dhkey)
{
#if CONFIG_BT_SMP_CRYPTO_STACK_MBEDTLS
    return bredr_psa_gen_dhkey(p256, peer_pub_key_x, peer_pub_key_y, our_priv_key, out_dhkey);
#else
    size_t buf_len = p256 ? BREDR_P256_COORD_LEN : BREDR_P192_COORD_LEN;
    uint8_t dh[32];
    uint8_t pk[64];
    uint8_t priv[32];
    int rc;

    btdm_swap_buf(pk, peer_pub_key_x, buf_len);
    btdm_swap_buf(&pk[buf_len], peer_pub_key_y, buf_len);
    btdm_swap_buf(priv, our_priv_key, buf_len);

    if (p256) {
        if (uECC_valid_public_key(pk, uECC_secp256r1()) < 0) {
            return -1;
        }

        rc = uECC_shared_secret(pk, priv, dh, uECC_secp256r1());
        if (rc == TC_CRYPTO_FAIL) {
            return -1;
        }
    } else {
        if (uECC_valid_public_key(pk, uECC_secp192r1()) < 0) {
            return -1;
        }

        rc = uECC_shared_secret(pk, priv, dh, uECC_secp192r1());
        if (rc == TC_CRYPTO_FAIL) {
            return -1;
        }
    }

    btdm_swap_buf(out_dhkey, dh, buf_len);
    return 0;
#endif /* !CONFIG_BT_SMP_CRYPTO_STACK_MBEDTLS */
}

static int bredr_gen_keypair_ecc256(uint8_t *public_key, uint8_t *private_key)
{
    return bredr_gen_keypair(true, public_key, private_key);
}

static int bredr_gen_keypair_ecc192(uint8_t *public_key, uint8_t *private_key)
{
    return bredr_gen_keypair(false, public_key, private_key);
}

static int
bredr_gen_dhkey_ecc256(const uint8_t *peer_pub_key_x, const uint8_t *peer_pub_key_y, const uint8_t *our_priv_key, uint8_t *out_dhkey)
{
    return bredr_gen_dhkey(true, peer_pub_key_x, peer_pub_key_y, our_priv_key, out_dhkey);
}

static int
bredr_gen_dhkey_ecc192(const uint8_t *peer_pub_key_x, const uint8_t *peer_pub_key_y, const uint8_t *our_priv_key, uint8_t *out_dhkey)
{
    return bredr_gen_dhkey(false, peer_pub_key_x, peer_pub_key_y, our_priv_key, out_dhkey);
}

static int bredr_sha256(const uint8_t *msg, uint32_t msg_len, uint8_t *digest)
{
#if CONFIG_BT_SMP_CRYPTO_STACK_MBEDTLS
    size_t hash_len = 0;
    psa_status_t status = psa_hash_compute(PSA_ALG_SHA_256, msg, (size_t)msg_len, digest, 32, &hash_len);

    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG_BREDR_CRYPTO, "psa_hash_compute failed: %d", (int)status);
        return -1;
    }
    if (hash_len != 32) {
        ESP_LOGE(TAG_BREDR_CRYPTO, "unexpected SHA-256 length %zu", hash_len);
        return -1;
    }
    return 0;
#else
    int rc = -1;

    do {
        int ret = TC_CRYPTO_SUCCESS;
        struct tc_sha256_state_struct state;

        ret = tc_sha256_init(&state);
        if (ret != TC_CRYPTO_SUCCESS) {
            break;
        }

        ret = tc_sha256_update(&state, msg, (size_t)msg_len);
        if (ret != TC_CRYPTO_SUCCESS) {
            break;
        }

        ret = tc_sha256_final(digest, &state);
        if (ret != TC_CRYPTO_SUCCESS) {
            break;
        }

        rc = 0;
    } while (0);

    return rc;
#endif
}

static int bredr_hmac(const uint8_t *key, uint32_t key_len, const uint8_t *msg, uint32_t msg_len, uint8_t *digest)
{
#if CONFIG_BT_SMP_CRYPTO_STACK_MBEDTLS
    psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;
    psa_status_t status;
    size_t mac_len = 0;

    if (key_len == 0) {
        return -1;
    }

    psa_set_key_type(&attr, PSA_KEY_TYPE_HMAC);
    psa_set_key_algorithm(&attr, PSA_ALG_HMAC(PSA_ALG_SHA_256));
    psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_SIGN_MESSAGE);
    psa_set_key_bits(&attr, (psa_key_bits_t)(key_len * 8));

    status = psa_import_key(&attr, key, (size_t)key_len, &key_id);
    psa_reset_key_attributes(&attr);
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG_BREDR_CRYPTO, "psa_import_key (HMAC) failed: %d", (int)status);
        return -1;
    }

    status = psa_mac_compute(key_id, PSA_ALG_HMAC(PSA_ALG_SHA_256), msg, (size_t)msg_len, digest, 32, &mac_len);
    psa_destroy_key(key_id);

    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG_BREDR_CRYPTO, "psa_mac_compute failed: %d", (int)status);
        return -1;
    }
    if (mac_len != 32) {
        ESP_LOGE(TAG_BREDR_CRYPTO, "unexpected HMAC length %zu", mac_len);
        return -1;
    }
    return 0;
#else
    int rc = -1;

    do {
        int ret = TC_CRYPTO_SUCCESS;
        struct tc_hmac_state_struct state;

        ret = tc_hmac_set_key(&state, key, (unsigned int)key_len);
        if (ret != TC_CRYPTO_SUCCESS) {
            break;
        }

        ret = tc_hmac_init(&state);
        if (ret != TC_CRYPTO_SUCCESS) {
            break;
        }

        ret = tc_hmac_update(&state, msg, (unsigned int)msg_len);
        if (ret != TC_CRYPTO_SUCCESS) {
            break;
        }

        ret = tc_hmac_final(digest, TC_SHA256_DIGEST_SIZE, &state);
        if (ret != TC_CRYPTO_SUCCESS) {
            break;
        }

        rc = 0;
    } while (0);

    return rc;
#endif
}

static void bredr_tx_cca_set_wapper(struct bredr_bb_cca_set_params *param)
{
    bredr_tx_cca_set(param->enable,
                     param->thresh_low,
                     param->power_drop_0,
                     param->power_high_0,
                     param->power_low_0,
                     param->thresh_high_en,
                     param->thresh_high,
                     param->power_drop1,
                     param->power_high_1,
                     param->power_low_1);
}

const char *bredr_controller_get_compile_version(void)
{
    return co_orca_get_git_version_str();
}

orca_mempool_t tlsf_create_with_pool(void *mem, size_t pool_bytes, size_t max_bytes);
void tlsf_destroy(orca_mempool_t tlsf);
void *tlsf_malloc(orca_mempool_t tlsf, size_t size);
void tlsf_free(orca_mempool_t tlsf, void *ptr);
size_t tlsf_block_size(void *ptr);
size_t tlsf_alloc_overhead(void);

struct orca_mempool_ops s_orca_mempool_ops = {
    .create_with_pool = tlsf_create_with_pool,
    .destroy = NULL,
    .malloc = tlsf_malloc,
    .free = tlsf_free,
};

static int bredr_ctrl_setup_callback(void)
{
    int ret = 0;

    do {
#if UC_BR_EDR_CTRL_MAX_SYNC_CONN
        ret = bredr_ctrl_feat_sync_en();
        if (ret != 0) {
            ESP_LOGE(BREDR_LOG_TAG, "bredr_ctrl_feat_sync_en failed, ret:%d", ret);
            break;
        }
#endif /* UC_BR_EDR_CTRL_MAX_SYNC_CONN */

#if UC_BR_EDR_APB_EN
        ret = bredr_ctrl_feat_apb_en();
        if (ret != 0) {
            ESP_LOGE(BREDR_LOG_TAG, "bredr_ctrl_feat_apb_en failed, ret:%d", ret);
            break;
        }
#endif /* UC_BR_EDR_APB_EN */

#if UC_BR_EDR_APB_EXT_BCST_ENC_EN
        ret = bredr_ctrl_feat_bcst_enc_en();
        if (ret != 0) {
            ESP_LOGE(BREDR_LOG_TAG, "bredr_ctrl_feat_bcst_enc_en failed, ret:%d", ret);
            break;
        }
#endif /* UC_BR_EDR_APB_EXT_BCST_ENC_EN */

#if UC_BR_EDR_APB_EXT_PCA_EN
        ret = bredr_ctrl_feat_pca_en();
        if (ret != 0) {
            ESP_LOGE(BREDR_LOG_TAG, "bredr_ctrl_feat_pca_en failed, ret:%d", ret);
            break;
        }
#endif /* UC_BR_EDR_APB_EXT_PCA_EN */

#if UC_BR_EDR_HOLD_EN
        ret = bredr_ctrl_feat_hold_en();
        if (ret != 0) {
            ESP_LOGE(BREDR_LOG_TAG, "bredr_ctrl_feat_hold_en failed, ret:%d", ret);
            break;
        }
#endif /* UC_BR_EDR_HOLD_EN */

#if UC_BR_EDR_CPB_RX_LINK_NB
        ret = bredr_ctrl_feat_cpb_rx_en();
        if (ret != 0) {
            ESP_LOGE(BREDR_LOG_TAG, "bredr_ctrl_feat_cpb_rx_en failed, ret:%d", ret);
            break;
        }
#endif /* UC_BR_EDR_CPB_RX_LINK_NB */

#if UC_BR_EDR_CPB_TX_LINK_NB
        ret = bredr_ctrl_feat_cpb_tx_en();
        if (ret != 0) {
            ESP_LOGE(BREDR_LOG_TAG, "bredr_ctrl_feat_cpb_tx_en failed, ret:%d", ret);
            break;
        }
#endif /* UC_BR_EDR_CPB_TX_LINK_NB */

#if UC_BR_EDR_TEST_MODE_EN
        ret = bredr_ctrl_feat_test_en();
        if (ret != 0) {
            ESP_LOGE(BREDR_LOG_TAG, "bredr_ctrl_feat_test_en failed, ret:%d", ret);
            break;
        }
#endif /* UC_BR_EDR_TEST_MODE_EN */

#if UC_BR_EDR_LK_STORE_EN
        ret = bredr_ctrl_feat_lk_store_en();
        if (ret != 0) {
            ESP_LOGE(BREDR_LOG_TAG, "bredr_ctrl_feat_lk_store_en failed, ret:%d", ret);
            break;
        }
#endif /* UC_BR_EDR_LK_STORE_EN */

#if UC_BR_EDR_DTM_EN
        ret = bredr_ctrl_feat_dtm_en();
        if (ret != 0) {
            ESP_LOGE(BREDR_LOG_TAG, "bredr_ctrl_feat_dtm_en failed, ret:%d", ret);
            break;
        }
#endif /* UC_BR_EDR_DTM_EN */

#if CONFIG_SW_COEXIST_ENABLE
        ret = bredr_ctrl_feat_coex_en();
        if (ret != 0) {
            ESP_LOGE(BREDR_LOG_TAG, "bredr_ctrl_feat_coex_en failed, ret:%d", ret);
            break;
        }
#endif /* CONFIG_SW_COEXIST_ENABLE */
    } while (0);

    return ret;
}

static int bredr_ctrl_ext_dep_callback(void)
{
    int ret = 0;

    do {
#if CONFIG_BT_SMP_CRYPTO_STACK_MBEDTLS
        psa_status_t psa_st = psa_crypto_init();
        if (psa_st != PSA_SUCCESS) {
            ESP_LOGE(BREDR_LOG_TAG, "psa_crypto_init failed: %d", (int)psa_st);
            ret = -1;
            break;
        }
#endif
        ret = bredr_register_crypto_funcs((bredr_crypto_ops_t *)&bredr_crypto_ops);
        if (ret != 0) {
            ESP_LOGE(BREDR_LOG_TAG, "bredr_register_crypto_funcs failed, ret:%d", ret);
            break;
        }

        ret = bredr_register_bb_funcs(&s_bredr_bb_ops);
        if (ret != 0) {
            ESP_LOGE(BREDR_LOG_TAG, "bredr_register_bb_funcs failed, ret:%d", ret);
            break;
        }
    } while (0);

    return ret;
}

int esp_bredr_controller_init(esp_bt_controller_config_t *cfg)
{
    int status;
    esp_err_t err = ESP_OK;

    ESP_LOGI(BREDR_LOG_TAG, "BT controller compile version [%s]", co_orca_get_git_version_str());

    bredr_register_setup_callback(bredr_ctrl_setup_callback);
    bredr_register_ext_dep_callback(bredr_ctrl_ext_dep_callback);

    cfg->bredr.mempool_size = 1024 * (cfg->bredr.max_acl_conn + 2);
    cfg->bredr.mempool_ops = &s_orca_mempool_ops;

    status = bredr_controller_init(&cfg->bredr);
    if (status != 0) {
        ESP_LOGE(BREDR_LOG_TAG, "bredr_controller_init failed, status:%d", status);
        err = ESP_ERR_NO_MEM;
    }

    return err;
}

int bredr_stack_init(esp_bt_controller_config_t *cfg)
{
    return esp_bredr_controller_init(cfg);
}

void bredr_stack_deinit(void)
{
    bredr_controller_deinit();
}

int bredr_stack_enable(void)
{
    return 0;
}

void bredr_stack_disable(void)
{

}

#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
esp_err_t sleep_modem_bredr_mac_modem_state_init(void)
{
    uint8_t size;
    const sleep_retention_entries_config_t *bredr_mac_modem_config = r_bredr_mac_retention_link_get(&size);
    esp_err_t err = sleep_retention_entries_create(bredr_mac_modem_config, size, REGDMA_LINK_PRI_5, SLEEP_RETENTION_MODULE_BLE_MAC);
    if (err == ESP_OK) {
        ESP_LOGI(BREDR_LOG_TAG, "Modem BREDR MAC retention initialization");
    }

    return err;
}
#endif // CONFIG_FREERTOS_USE_TICKLESS_IDLE

void
bredr_osal_elem_calc(esp_bt_controller_config_t *cfg, btdm_osal_elem_num_t *elem)
{
    elem->evt_count += 4;
    elem->evtq_count += 0;
    elem->co_count += 0;
    elem->sem_count += 1;
    elem->mutex_count += 1;
}

#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE
