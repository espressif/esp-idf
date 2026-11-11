/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/* PSA FFDH cannot take a short exponent: import sets the group from the
 * buffer length, and generate draws X across the whole prime. This example
 * uses the private bignum API shipped with this IDF tree. The macro must
 * precede every header, because psa/crypto.h includes that header first. */
#define MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_task_wdt.h"
#include "nvs_flash.h"
#include "esp_random.h"
#include "esp_heap_caps.h"
#if CONFIG_BT_CONTROLLER_ENABLED || !CONFIG_BT_NIMBLE_ENABLED
#include "esp_bt.h"
#endif

#include "esp_blufi_api.h"
#include "blufi_example.h"

#include "psa/crypto.h"
#include "esp_crc.h"
#include "esp_timer.h"
#include "soc/soc_caps.h"
#if !SOC_MPI_SUPPORTED
#include "mbedtls/bignum.h"
#include "bootloader_random.h"
#include "esp_task.h"
#endif

/*
   The SEC_TYPE_xxx is for self-defined packet data type in the procedure of "BLUFI negotiate key"
   If user use other negotiation procedure to exchange(or generate) key, should redefine the type by yourself.
 */
#define SEC_TYPE_DH_PARAM_LEN   0x00
#define SEC_TYPE_DH_PARAM_DATA  0x01
#define SEC_TYPE_DH_P           0x02
#define SEC_TYPE_DH_G           0x03
#define SEC_TYPE_DH_PUBLIC      0x04

#define BLUFI_IV_PREFIX_LEN     9
#define BLUFI_IV_SIZE           16
#define BLUFI_HASH_SIZE         32   /* SHA-256 output size */
#define BLUFI_KEY_SIZE          32
#define BLUFI_ENC_DOMAIN_STR "blufi_enc"
#define BLUFI_DEC_DOMAIN_STR "blufi_dec"

#if !SOC_MPI_SUPPORTED
#define BLUFI_DH_PREGEN_STACK_SIZE   6144
/* Above app_main so the task runs, and takes its ~4 kB block, before
 * initialise_wifi() continues. */
#define BLUFI_DH_PREGEN_PRIO        (ESP_TASK_MAIN_PRIO + 1)
#define BLUFI_DH_PREGEN_WAIT_MS      30000
#define BLUFI_DH_HEAVY_CRYPTO_WDT_MS 30000
/* ffdhe3072 is a 128-bit group. RFC 7919's floor for the private exponent is
 * twice that strength. A full-size exponent is what makes software modexp
 * take ~12s on ESP32-C2; 256 bits keeps the same prime and public-key size.
 * Set to 1 to run a two-party agreement check at boot. Each modexp logs
 * its own time. */
#define BLUFI_DH_PRIV_LEN            32
#define BLUFI_DH_SHORT_EXP_SELF_TEST 0
#endif

struct blufi_security {
#define DH_PARAM_LEN_MAX        1024
#define DH_SELF_PUB_KEY_LEN     384  /* Support 3072-bit DH key (3072/8 = 384 bytes) */
    uint8_t  self_public_key[DH_SELF_PUB_KEY_LEN];
#define SHARE_KEY_LEN           384  /* Support 3072-bit DH key (3072/8 = 384 bytes) */
    uint8_t  share_key[SHARE_KEY_LEN];
    size_t   share_len;
#define PSK_LEN                 32
    uint8_t  psk[PSK_LEN];
    uint8_t  *dh_param;
    int      dh_param_len;
    psa_key_id_t aes_key;
    psa_cipher_operation_t enc_operation;  /* Persistent cipher operation for encryption */
    psa_cipher_operation_t dec_operation;  /* Persistent cipher operation for decryption */
};
static struct blufi_security *blufi_sec;

extern void btc_blufi_report_error(esp_blufi_error_state_t state);

/**
 * @brief Clean up DH parameter memory
 */
static void blufi_cleanup_dh_param(void)
{
    if (blufi_sec && blufi_sec->dh_param) {
        free(blufi_sec->dh_param);
        blufi_sec->dh_param = NULL;
    }
}

/**
 * @brief Clean up AES key and cipher operations
 *
 * @param abort_enc Whether to abort encryption operation
 * @param abort_dec Whether to abort decryption operation
 */
static void blufi_cleanup_aes_session(bool abort_enc, bool abort_dec)
{
    if (!blufi_sec) {
        return;
    }

    if (abort_enc) {
        psa_cipher_abort(&blufi_sec->enc_operation);
    }
    if (abort_dec) {
        psa_cipher_abort(&blufi_sec->dec_operation);
    }
    if (blufi_sec->aes_key != 0) {
        psa_destroy_key(blufi_sec->aes_key);
        blufi_sec->aes_key = 0;
    }
}

/**
 * @brief Clean up entire DH negotiation session (AES key, cipher ops, and DH params)
 *
 * @param abort_enc Whether to abort encryption operation
 * @param abort_dec Whether to abort decryption operation
 */
static void blufi_cleanup_negotiation(bool abort_enc, bool abort_dec)
{
    blufi_cleanup_aes_session(abort_enc, abort_dec);
    blufi_cleanup_dh_param();
}

#if !SOC_MPI_SUPPORTED
/*
 * DH keypair pre-generation: overlap G^X mod P with BLE advertising so only
 * key agreement remains on the critical path when the phone sends its public
 * key. The exponent is 256 bits. The modulus stays RFC 7919 ffdhe3072, and
 * the public value is still 384 bytes, so the phone does not change.
 *
 * The worker outlives every connection. Creating it per keypair would ask for
 * its stack after Wi-Fi and the BLE host are up, which is where xTaskCreate()
 * fails.
 */
#define BLUFI_PREGEN_DONE_BIT   BIT0

static const uint8_t s_ffdhe3072_p[] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xAD, 0xF8, 0x54, 0x58, 0xA2, 0xBB, 0x4A, 0x9A,
    0xAF, 0xDC, 0x56, 0x20, 0x27, 0x3D, 0x3C, 0xF1,
    0xD8, 0xB9, 0xC5, 0x83, 0xCE, 0x2D, 0x36, 0x95,
    0xA9, 0xE1, 0x36, 0x41, 0x14, 0x64, 0x33, 0xFB,
    0xCC, 0x93, 0x9D, 0xCE, 0x24, 0x9B, 0x3E, 0xF9,
    0x7D, 0x2F, 0xE3, 0x63, 0x63, 0x0C, 0x75, 0xD8,
    0xF6, 0x81, 0xB2, 0x02, 0xAE, 0xC4, 0x61, 0x7A,
    0xD3, 0xDF, 0x1E, 0xD5, 0xD5, 0xFD, 0x65, 0x61,
    0x24, 0x33, 0xF5, 0x1F, 0x5F, 0x06, 0x6E, 0xD0,
    0x85, 0x63, 0x65, 0x55, 0x3D, 0xED, 0x1A, 0xF3,
    0xB5, 0x57, 0x13, 0x5E, 0x7F, 0x57, 0xC9, 0x35,
    0x98, 0x4F, 0x0C, 0x70, 0xE0, 0xE6, 0x8B, 0x77,
    0xE2, 0xA6, 0x89, 0xDA, 0xF3, 0xEF, 0xE8, 0x72,
    0x1D, 0xF1, 0x58, 0xA1, 0x36, 0xAD, 0xE7, 0x35,
    0x30, 0xAC, 0xCA, 0x4F, 0x48, 0x3A, 0x79, 0x7A,
    0xBC, 0x0A, 0xB1, 0x82, 0xB3, 0x24, 0xFB, 0x61,
    0xD1, 0x08, 0xA9, 0x4B, 0xB2, 0xC8, 0xE3, 0xFB,
    0xB9, 0x6A, 0xDA, 0xB7, 0x60, 0xD7, 0xF4, 0x68,
    0x1D, 0x4F, 0x42, 0xA3, 0xDE, 0x39, 0x4D, 0xF4,
    0xAE, 0x56, 0xED, 0xE7, 0x63, 0x72, 0xBB, 0x19,
    0x0B, 0x07, 0xA7, 0xC8, 0xEE, 0x0A, 0x6D, 0x70,
    0x9E, 0x02, 0xFC, 0xE1, 0xCD, 0xF7, 0xE2, 0xEC,
    0xC0, 0x34, 0x04, 0xCD, 0x28, 0x34, 0x2F, 0x61,
    0x91, 0x72, 0xFE, 0x9C, 0xE9, 0x85, 0x83, 0xFF,
    0x8E, 0x4F, 0x12, 0x32, 0xEE, 0xF2, 0x81, 0x83,
    0xC3, 0xFE, 0x3B, 0x1B, 0x4C, 0x6F, 0xAD, 0x73,
    0x3B, 0xB5, 0xFC, 0xBC, 0x2E, 0xC2, 0x20, 0x05,
    0xC5, 0x8E, 0xF1, 0x83, 0x7D, 0x16, 0x83, 0xB2,
    0xC6, 0xF3, 0x4A, 0x26, 0xC1, 0xB2, 0xEF, 0xFA,
    0x88, 0x6B, 0x42, 0x38, 0x61, 0x1F, 0xCF, 0xDC,
    0xDE, 0x35, 0x5B, 0x3B, 0x65, 0x19, 0x03, 0x5B,
    0xBC, 0x34, 0xF4, 0xDE, 0xF9, 0x9C, 0x02, 0x38,
    0x61, 0xB4, 0x6F, 0xC9, 0xD6, 0xE6, 0xC9, 0x07,
    0x7A, 0xD9, 0x1D, 0x26, 0x91, 0xF7, 0xF7, 0xEE,
    0x59, 0x8C, 0xB0, 0xFA, 0xC1, 0x86, 0xD9, 0x1C,
    0xAE, 0xFE, 0x13, 0x09, 0x85, 0x13, 0x92, 0x70,
    0xB4, 0x13, 0x0C, 0x93, 0xBC, 0x43, 0x79, 0x44,
    0xF4, 0xFD, 0x44, 0x52, 0xE2, 0xD7, 0x4D, 0xD3,
    0x64, 0xF2, 0xE2, 0x1E, 0x71, 0xF5, 0x4B, 0xFF,
    0x5C, 0xAE, 0x82, 0xAB, 0x9C, 0x9D, 0xF6, 0x9E,
    0xE8, 0x6D, 0x2B, 0xC5, 0x22, 0x36, 0x3A, 0x0D,
    0xAB, 0xC5, 0x21, 0x97, 0x9B, 0x0D, 0xEA, 0xDA,
    0x1D, 0xBF, 0x9A, 0x42, 0xD5, 0xC4, 0x48, 0x4E,
    0x0A, 0xBC, 0xD0, 0x6B, 0xFA, 0x53, 0xDD, 0xEF,
    0x3C, 0x1B, 0x20, 0xEE, 0x3F, 0xD5, 0x9D, 0x7C,
    0x25, 0xE4, 0x1D, 0x2B, 0x66, 0xC6, 0x2E, 0x37,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
_Static_assert(sizeof(s_ffdhe3072_p) == 384, "ffdhe3072 prime must be 384 bytes");
static const uint8_t s_ffdhe3072_g[] = { 0x02 };

/* R^2 mod P. exp_mod builds this while its ~4 KB window buffer is live.
 * On ESP32-C2 that pair does not fit the second time. Compute once, reuse. */
static mbedtls_mpi s_dh_rr;
static bool s_dh_rr_inited;

static uint8_t s_pregen_private[BLUFI_DH_PRIV_LEN];
static bool s_pregen_has_private;
static uint8_t s_pregen_public_key[DH_SELF_PUB_KEY_LEN];
static size_t s_pregen_public_key_len;
static bool s_pregen_ok;
static bool s_pregen_busy;
static EventGroupHandle_t s_pregen_evt;
static SemaphoreHandle_t s_pregen_req;
static TaskHandle_t s_pregen_task_hdl;
static void (*s_pregen_complete_cb)(void);
/* True only for the boot draw, before esp_wifi_start(). */
static bool s_entropy_before_rf;

static void blufi_wdt_set_timeout(uint32_t timeout_ms)
{
    esp_task_wdt_config_t cfg = {
        .timeout_ms = timeout_ms,
        .idle_core_mask = (1 << portNUM_PROCESSORS) - 1,
        .trigger_panic = true,
    };
    esp_task_wdt_reconfigure(&cfg);
}

/* out_len is the fixed-width result (384). The exponent buffer must be the
 * raw 32-byte value, not zero-padded out to the prime size: the constant-time
 * modexp walks every limb it is given. */
static int blufi_dh_modexp(const uint8_t *base, size_t base_len,
                           const uint8_t *exp, size_t exp_len,
                           uint8_t *out, size_t out_len, bool check_range)
{
    mbedtls_mpi a, e, n, x, pm1;
    int ret = -1;
    int64_t t0;

    mbedtls_mpi_init(&a);
    mbedtls_mpi_init(&e);
    mbedtls_mpi_init(&n);
    mbedtls_mpi_init(&x);
    mbedtls_mpi_init(&pm1);

    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&n, s_ffdhe3072_p, sizeof(s_ffdhe3072_p)));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&a, base, base_len));

    /* RFC 7919 §5.1: 1 < Y < P-1. ffdhe3072 is a safe prime, so 1 and P-1
     * are the only low-order elements. Check this before loading the secret. */
    if (check_range) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_sub_int(&pm1, &n, 1));
        if (mbedtls_mpi_cmp_int(&a, 1) <= 0 || mbedtls_mpi_cmp_mpi(&a, &pm1) >= 0) {
            ret = MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
            goto cleanup;
        }
    }

    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&e, exp, exp_len));

    if (!s_dh_rr_inited) {
        mbedtls_mpi_init(&s_dh_rr);
        s_dh_rr_inited = true;
    }

    t0 = esp_timer_get_time();
    MBEDTLS_MPI_CHK(mbedtls_mpi_exp_mod(&x, &a, &e, &n, &s_dh_rr));
    BLUFI_INFO("DH modexp exp_bits=%zu %u ms",
               mbedtls_mpi_bitlen(&e),
               (unsigned)((esp_timer_get_time() - t0) / 1000));
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&x, out, out_len));
    ret = 0;

cleanup:
    if (ret != 0) {
        BLUFI_ERROR("DH modexp failed %d (free %u, largest %u)",
                    ret,
                    (unsigned)esp_get_free_heap_size(),
                    (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL));
    }
    mbedtls_mpi_free(&a);
    mbedtls_mpi_free(&e);
    mbedtls_mpi_free(&n);
    mbedtls_mpi_free(&x);
    mbedtls_mpi_free(&pm1);
    return ret;
}

/* before_rf: Wi-Fi/BT are not up, so the RNG is only pseudo-random unless
 * the SAR ADC entropy source is enabled. Never enable it once the radio is
 * on; that source and the RF subsystem share the ADC. */
static void blufi_dh_random_priv(uint8_t priv[BLUFI_DH_PRIV_LEN], bool before_rf)
{
    if (before_rf) {
        bootloader_random_enable();
    }
    esp_fill_random(priv, BLUFI_DH_PRIV_LEN);
    if (before_rf) {
        bootloader_random_disable();
    }
    priv[0] |= 0x80;
}

static int blufi_dh_make_public(const uint8_t priv[BLUFI_DH_PRIV_LEN],
                                uint8_t pub[DH_SELF_PUB_KEY_LEN])
{
    return blufi_dh_modexp(s_ffdhe3072_g, sizeof(s_ffdhe3072_g),
                           priv, BLUFI_DH_PRIV_LEN,
                           pub, DH_SELF_PUB_KEY_LEN, false);
}

static int blufi_dh_agree(const uint8_t priv[BLUFI_DH_PRIV_LEN],
                          const uint8_t *peer, size_t peer_len,
                          uint8_t share[SHARE_KEY_LEN])
{
    if (peer_len != DH_SELF_PUB_KEY_LEN) {
        return MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
    }
    return blufi_dh_modexp(peer, peer_len, priv, BLUFI_DH_PRIV_LEN,
                           share, SHARE_KEY_LEN, true);
}

static int blufi_dh_generate(uint8_t priv[BLUFI_DH_PRIV_LEN],
                             uint8_t pub[DH_SELF_PUB_KEY_LEN],
                             bool before_rf)
{
    blufi_dh_random_priv(priv, before_rf);
    if (blufi_dh_make_public(priv, pub) != 0) {
        mbedtls_platform_zeroize(priv, BLUFI_DH_PRIV_LEN);
        return -1;
    }
    return 0;
}

#if BLUFI_DH_SHORT_EXP_SELF_TEST
static void blufi_dh_short_exp_self_test(void)
{
    uint8_t priv_a[BLUFI_DH_PRIV_LEN];
    uint8_t priv_b[BLUFI_DH_PRIV_LEN];
    uint8_t *buf = calloc(4, DH_SELF_PUB_KEY_LEN);
    uint8_t *pub_a;
    uint8_t *pub_b;
    uint8_t *sec_a;
    uint8_t *sec_b;
    int ret = -1;

    if (buf == NULL) {
        BLUFI_ERROR("DH short-exp self-test alloc failed");
        return;
    }
    pub_a = buf;
    pub_b = buf + DH_SELF_PUB_KEY_LEN;
    sec_a = buf + 2 * DH_SELF_PUB_KEY_LEN;
    sec_b = buf + 3 * DH_SELF_PUB_KEY_LEN;

    BLUFI_INFO("DH short-exp self-test start");
    ret = blufi_dh_generate(priv_a, pub_a, s_entropy_before_rf);
    if (ret == 0) {
        ret = blufi_dh_generate(priv_b, pub_b, s_entropy_before_rf);
    }
    if (ret == 0) {
        ret = blufi_dh_agree(priv_a, pub_b, DH_SELF_PUB_KEY_LEN, sec_a);
    }
    if (ret == 0) {
        ret = blufi_dh_agree(priv_b, pub_a, DH_SELF_PUB_KEY_LEN, sec_b);
    }
    if (ret == 0 && memcmp(sec_a, sec_b, SHARE_KEY_LEN) != 0) {
        ret = -1;
    }

    mbedtls_platform_zeroize(priv_a, sizeof(priv_a));
    mbedtls_platform_zeroize(priv_b, sizeof(priv_b));
    mbedtls_platform_zeroize(buf, 4 * DH_SELF_PUB_KEY_LEN);
    free(buf);
    BLUFI_INFO("DH short-exp self-test %s", ret == 0 ? "OK" : "FAILED");
}
#endif

static void blufi_dh_pregen_generate(void)
{
    mbedtls_platform_zeroize(s_pregen_private, sizeof(s_pregen_private));
    s_pregen_has_private = false;
    s_pregen_public_key_len = 0;

    if (blufi_dh_generate(s_pregen_private, s_pregen_public_key, s_entropy_before_rf) == 0) {
        s_pregen_public_key_len = DH_SELF_PUB_KEY_LEN;
        s_pregen_has_private = true;
        s_pregen_ok = true;
    } else {
        s_pregen_ok = false;
    }

    BLUFI_INFO("DH keypair pre-generation %s", s_pregen_ok ? "done" : "FAILED");
    BLUFI_INFO("DH pregen stack free %u",
               (unsigned)uxTaskGetStackHighWaterMark(NULL));
}

static void blufi_dh_pregen_task(void *arg)
{
    (void)arg;

    while (true) {
        xSemaphoreTake(s_pregen_req, portMAX_DELAY);

#if BLUFI_DH_SHORT_EXP_SELF_TEST
        static bool self_test_done;
        if (!self_test_done) {
            self_test_done = true;
            blufi_dh_short_exp_self_test();
        }
#endif

        blufi_wdt_set_timeout(BLUFI_DH_HEAVY_CRYPTO_WDT_MS);
        blufi_dh_pregen_generate();
        blufi_wdt_set_timeout(CONFIG_ESP_TASK_WDT_TIMEOUT_S * 1000);

        s_pregen_busy = false;
        xEventGroupSetBits(s_pregen_evt, BLUFI_PREGEN_DONE_BIT);

        if (s_pregen_complete_cb) {
            void (*cb)(void) = s_pregen_complete_cb;
            s_pregen_complete_cb = NULL;
            cb();
        }
    }
}

static bool blufi_dh_pregen_worker_ensure(void)
{
    if (s_pregen_task_hdl != NULL) {
        return true;
    }

    if (s_pregen_evt == NULL) {
        s_pregen_evt = xEventGroupCreate();
    }
    if (s_pregen_req == NULL) {
        s_pregen_req = xSemaphoreCreateBinary();
    }
    if (s_pregen_evt == NULL || s_pregen_req == NULL) {
        BLUFI_ERROR("Failed to create pre-gen sync objects");
        return false;
    }

    if (xTaskCreate(blufi_dh_pregen_task, "blufi_pregen",
                    BLUFI_DH_PREGEN_STACK_SIZE, NULL,
                    BLUFI_DH_PREGEN_PRIO, &s_pregen_task_hdl) != pdPASS) {
        BLUFI_ERROR("Failed to create pre-gen task");
        s_pregen_task_hdl = NULL;
        return false;
    }

    return true;
}

static void blufi_dh_pregen_start_impl(void (*done_cb)(void), bool before_rf)
{
    if (!blufi_dh_pregen_worker_ensure()) {
        /* Advertise anyway; negotiation falls back to inline key generation. */
        if (done_cb) {
            done_cb();
        }
        return;
    }

    /* A keypair is already banked, or one is on its way. */
    if (s_pregen_busy || (s_pregen_ok && s_pregen_has_private)) {
        if (done_cb) {
            done_cb();
        }
        return;
    }

    /* Set only when a draw is actually queued. A later no-op call must not
     * change the flag the running task is about to read. */
    s_entropy_before_rf = before_rf;
    s_pregen_public_key_len = 0;
    s_pregen_has_private = false;
    s_pregen_ok = false;
    s_pregen_complete_cb = done_cb;
    s_pregen_busy = true;
    xEventGroupClearBits(s_pregen_evt, BLUFI_PREGEN_DONE_BIT);
    xSemaphoreGive(s_pregen_req);
}

void blufi_dh_pregen_start(void)
{
    /* app_main only, before esp_wifi_start(). */
    blufi_dh_pregen_start_impl(NULL, true);
}

void blufi_dh_pregen_start_with_cb(void (*done_cb)(void))
{
    /* Wi-Fi or BT is already up, so the RF entropy source is on. */
    blufi_dh_pregen_start_impl(done_cb, false);
}

void blufi_dh_pregen_wait(void)
{
    if (s_pregen_evt == NULL) {
        return;
    }
    xEventGroupWaitBits(s_pregen_evt, BLUFI_PREGEN_DONE_BIT,
                        pdFALSE, pdTRUE, portMAX_DELAY);
}

/**
 * @brief Copy the banked private exponent to the caller and wipe the copy
 *
 * @return true when a usable keypair was banked
 */
static bool blufi_dh_pregen_take(uint8_t priv[BLUFI_DH_PRIV_LEN],
                                 uint8_t *pub, size_t *pub_len)
{
    if (!s_pregen_ok || !s_pregen_has_private) {
        return false;
    }

    memcpy(priv, s_pregen_private, BLUFI_DH_PRIV_LEN);
    mbedtls_platform_zeroize(s_pregen_private, sizeof(s_pregen_private));
    s_pregen_has_private = false;
    if (pub != NULL && pub_len != NULL) {
        memcpy(pub, s_pregen_public_key, s_pregen_public_key_len);
        *pub_len = s_pregen_public_key_len;
    }
    s_pregen_ok = false;
    xEventGroupClearBits(s_pregen_evt, BLUFI_PREGEN_DONE_BIT);

    return true;
}
#endif /* !SOC_MPI_SUPPORTED */

void blufi_dh_negotiate_data_handler(uint8_t *data, int len, uint8_t **output_data, int *output_len, bool *need_free)
{
    if (data == NULL || len < 3) {
        BLUFI_ERROR("BLUFI Invalid data format");
        btc_blufi_report_error(ESP_BLUFI_DATA_FORMAT_ERROR);
        return;
    }

    /* Validate output parameters */
    if (output_data == NULL || output_len == NULL || need_free == NULL) {
        BLUFI_ERROR("BLUFI Invalid output parameters");
        btc_blufi_report_error(ESP_BLUFI_DATA_FORMAT_ERROR);
        return;
    }

    uint8_t type = data[0];

    if (blufi_sec == NULL) {
        BLUFI_ERROR("BLUFI Security is not initialized");
        btc_blufi_report_error(ESP_BLUFI_INIT_SECURITY_ERROR);
        return;
    }

    switch (type) {
    case SEC_TYPE_DH_PARAM_LEN:
        blufi_sec->dh_param_len = ((data[1]<<8)|data[2]);
        // Security fix: Limit DH param length to prevent DoS via large memory allocation
        if (blufi_sec->dh_param_len == 0 || blufi_sec->dh_param_len > DH_PARAM_LEN_MAX) {
            BLUFI_ERROR("%s, invalid dh param len %d\n", __func__, blufi_sec->dh_param_len);
            blufi_sec->dh_param_len = 0;
            btc_blufi_report_error(ESP_BLUFI_DH_PARAM_ERROR);
            return;
        }

        /* Allow renegotiation - clean up previous state */
        blufi_cleanup_dh_param();
        blufi_cleanup_aes_session(true, true);

        blufi_sec->dh_param = (uint8_t *)malloc(blufi_sec->dh_param_len);
        if (blufi_sec->dh_param == NULL) {
            blufi_sec->dh_param_len = 0;  /* Reset length to avoid using unallocated memory */
            btc_blufi_report_error(ESP_BLUFI_DH_MALLOC_ERROR);
            BLUFI_ERROR("%s, malloc failed\n", __func__);
            return;
        }
        break;
    case SEC_TYPE_DH_PARAM_DATA:{
        if (blufi_sec->dh_param == NULL) {
            BLUFI_ERROR("%s, blufi_sec->dh_param == NULL\n", __func__);
            btc_blufi_report_error(ESP_BLUFI_DH_PARAM_ERROR);
            return;
        }

        if (len < (blufi_sec->dh_param_len + 1)) {
            BLUFI_ERROR("%s, invalid dh param len\n", __func__);
            btc_blufi_report_error(ESP_BLUFI_DH_PARAM_ERROR);
            return;
        }

        uint8_t *param = blufi_sec->dh_param;
        memcpy(blufi_sec->dh_param, &data[1], blufi_sec->dh_param_len);

        /* Parse P length */
        if (blufi_sec->dh_param_len < 2) {
            BLUFI_ERROR("DH param too short for P length");
            btc_blufi_report_error(ESP_BLUFI_DH_PARAM_ERROR);
            return;
        }
        size_t p_len = (param[0] << 8) | param[1];
        size_t p_offset = 2 + p_len;
        if (p_offset > (size_t)blufi_sec->dh_param_len) {
            BLUFI_ERROR("P length %d exceeds dh_param bounds", p_len);
            btc_blufi_report_error(ESP_BLUFI_DH_PARAM_ERROR);
            return;
        }
        param += p_offset;

        /* Parse G length */
        if ((param - blufi_sec->dh_param) + 2 > (size_t)blufi_sec->dh_param_len) {
            BLUFI_ERROR("DH param too short for G length");
            btc_blufi_report_error(ESP_BLUFI_DH_PARAM_ERROR);
            return;
        }
        size_t g_len = (param[0] << 8) | param[1];
        size_t g_offset = (param - blufi_sec->dh_param) + 2 + g_len;
        if (g_offset > (size_t)blufi_sec->dh_param_len) {
            BLUFI_ERROR("G length %d exceeds dh_param bounds", g_len);
            btc_blufi_report_error(ESP_BLUFI_DH_PARAM_ERROR);
            return;
        }
        param += 2 + g_len;

        /* Parse public key length */
        if ((param - blufi_sec->dh_param) + 2 > (size_t)blufi_sec->dh_param_len) {
            BLUFI_ERROR("DH param too short for public key length");
            btc_blufi_report_error(ESP_BLUFI_DH_PARAM_ERROR);
            return;
        }
        size_t pub_len = (param[0] << 8) | param[1];
        param += 2;

        size_t pub_offset = (param - blufi_sec->dh_param) + pub_len;
        if (pub_offset > (size_t)blufi_sec->dh_param_len) {
            BLUFI_ERROR("Public key length %d exceeds dh_param bounds", pub_len);
            btc_blufi_report_error(ESP_BLUFI_DH_PARAM_ERROR);
            return;
        }

        /* Determine key bits based on public key length (RFC7919 key sizes)
         * Note: Only 3072 bit keys are supported due to buffer size constraints
         * (DH_SELF_PUB_KEY_LEN = 384 bytes = 3072 bits) */
        size_t key_bits = pub_len * 8;
        if (key_bits != 3072) {
            BLUFI_ERROR("Unsupported DH key length: %d bytes (only 3072 bit keys supported)", pub_len);
            btc_blufi_report_error(ESP_BLUFI_DH_PARAM_ERROR);
            return;
        }

        /* Validate public key length fits in output buffer */
        if (pub_len > DH_SELF_PUB_KEY_LEN) {
            BLUFI_ERROR("Public key length %d exceeds output buffer size %d", pub_len, DH_SELF_PUB_KEY_LEN);
            btc_blufi_report_error(ESP_BLUFI_DH_PARAM_ERROR);
            return;
        }

        size_t public_key_len = 0;
        psa_status_t status;

#if !SOC_MPI_SUPPORTED
        /* No bignum accelerator: agree with the 256-bit exponent. The
         * pre-generated keypair hides G^X; Y^X is what the phone waits on.
         * Free the DH parameter buffer first. Inline generation and agreement
         * each need one ~4.2 kB contiguous internal block. */
        uint8_t priv[BLUFI_DH_PRIV_LEN];
        bool have_priv = false;
        int dh_ret;

        if (s_pregen_evt != NULL &&
            (xEventGroupWaitBits(s_pregen_evt, BLUFI_PREGEN_DONE_BIT, pdFALSE, pdTRUE,
                                 pdMS_TO_TICKS(BLUFI_DH_PREGEN_WAIT_MS)) & BLUFI_PREGEN_DONE_BIT)) {
            if (blufi_dh_pregen_take(priv, blufi_sec->self_public_key, &public_key_len)) {
                have_priv = true;
                BLUFI_INFO("Using pre-generated DH keypair");
            }
        }

        uint8_t peer_pub[DH_SELF_PUB_KEY_LEN];
        memcpy(peer_pub, param, pub_len);
        blufi_cleanup_dh_param();

        if (!have_priv) {
            BLUFI_INFO("Pre-gen unavailable, generating DH keypair inline");
            blufi_wdt_set_timeout(BLUFI_DH_HEAVY_CRYPTO_WDT_MS);
            dh_ret = blufi_dh_generate(priv, blufi_sec->self_public_key, false);
            blufi_wdt_set_timeout(CONFIG_ESP_TASK_WDT_TIMEOUT_S * 1000);
            if (dh_ret != 0) {
                BLUFI_ERROR("%s DH keygen failed %d\n", __func__, dh_ret);
                mbedtls_platform_zeroize(priv, sizeof(priv));
                btc_blufi_report_error(ESP_BLUFI_DH_MALLOC_ERROR);
                return;
            }
            public_key_len = DH_SELF_PUB_KEY_LEN;
        }

        blufi_wdt_set_timeout(BLUFI_DH_HEAVY_CRYPTO_WDT_MS);
        dh_ret = blufi_dh_agree(priv, peer_pub, pub_len, blufi_sec->share_key);
        mbedtls_platform_zeroize(priv, sizeof(priv));
        mbedtls_platform_zeroize(peer_pub, sizeof(peer_pub));
        blufi_wdt_set_timeout(CONFIG_ESP_TASK_WDT_TIMEOUT_S * 1000);
        if (dh_ret == 0) {
            blufi_sec->share_len = SHARE_KEY_LEN;
            status = PSA_SUCCESS;
        } else {
            status = PSA_ERROR_GENERIC_ERROR;
        }
#else /* SOC_MPI_SUPPORTED */
        psa_algorithm_t alg = PSA_ALG_FFDH;
        psa_key_id_t private_key = 0;
        {
            psa_key_attributes_t keygen_attr = psa_key_attributes_init();
            psa_set_key_type(&keygen_attr, PSA_KEY_TYPE_DH_KEY_PAIR(PSA_DH_FAMILY_RFC7919));
            psa_set_key_bits(&keygen_attr, key_bits);
            psa_set_key_algorithm(&keygen_attr, alg);
            psa_set_key_usage_flags(&keygen_attr, PSA_KEY_USAGE_DERIVE);

            status = psa_generate_key(&keygen_attr, &private_key);
            if (status != PSA_SUCCESS) {
                BLUFI_ERROR("%s psa_generate_key failed %d\n", __func__, status);
                btc_blufi_report_error(ESP_BLUFI_DH_MALLOC_ERROR);
                blufi_cleanup_dh_param();
                return;
            }
            psa_reset_key_attributes(&keygen_attr);

            status = psa_export_public_key(private_key, blufi_sec->self_public_key,
                                           DH_SELF_PUB_KEY_LEN, &public_key_len);
            if (status != PSA_SUCCESS) {
                BLUFI_ERROR("%s psa_export_public_key failed %d\n", __func__, status);
                psa_destroy_key(private_key);
                btc_blufi_report_error(ESP_BLUFI_DH_MALLOC_ERROR);
                blufi_cleanup_dh_param();
                return;
            }
        }

        status = psa_raw_key_agreement(alg, private_key, param, pub_len,
                                       blufi_sec->share_key, SHARE_KEY_LEN,
                                       &blufi_sec->share_len);
        psa_destroy_key(private_key);
        blufi_cleanup_dh_param();
#endif /* !SOC_MPI_SUPPORTED */

        if (status != PSA_SUCCESS) {
#if !SOC_MPI_SUPPORTED
            BLUFI_ERROR("%s DH agree failed %d\n", __func__, status);
#else
            BLUFI_ERROR("%s psa_raw_key_agreement failed %d\n", __func__, status);
#endif
            btc_blufi_report_error(ESP_BLUFI_DH_PARAM_ERROR);
            return;
        }

        /* Validate share key length fits in buffer */
        if (blufi_sec->share_len > SHARE_KEY_LEN) {
            BLUFI_ERROR("Share key length %d exceeds buffer size %d", blufi_sec->share_len, SHARE_KEY_LEN);
            blufi_cleanup_dh_param();
            btc_blufi_report_error(ESP_BLUFI_DH_PARAM_ERROR);
            return;
        }

        size_t hash_length = 0;
        status = psa_hash_compute(PSA_ALG_SHA_256, blufi_sec->share_key, blufi_sec->share_len, blufi_sec->psk, PSK_LEN, &hash_length);
        if (status != PSA_SUCCESS) {
            BLUFI_ERROR("%s psa_hash_compute failed %d\n", __func__, status);
            btc_blufi_report_error(ESP_BLUFI_CALC_SHA_256_ERROR);
            blufi_cleanup_dh_param();
            return;
        }

        /* Destroy previous AES key and cipher operations if they exist */
        blufi_cleanup_aes_session(true, true);

        /* Import AES key for CTR mode */
        psa_key_attributes_t attributes = psa_key_attributes_init();
        psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
        psa_set_key_bits(&attributes, PSK_LEN * 8);
        psa_set_key_algorithm(&attributes, PSA_ALG_CTR);
        psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
        status = psa_import_key(&attributes, blufi_sec->psk, PSK_LEN, &blufi_sec->aes_key);
        if (status != PSA_SUCCESS) {
            BLUFI_ERROR("%s psa_import_key failed %d\n", __func__, status);
            btc_blufi_report_error(ESP_BLUFI_DH_MALLOC_ERROR);
            blufi_cleanup_dh_param();
            return;
        }
        psa_reset_key_attributes(&attributes);

        /* Derive domain-separated IVs for encryption and decryption */
        uint8_t enc_material[BLUFI_IV_PREFIX_LEN + SHARE_KEY_LEN];
        memcpy(enc_material, BLUFI_ENC_DOMAIN_STR, BLUFI_IV_PREFIX_LEN);
        memcpy(enc_material + BLUFI_IV_PREFIX_LEN, blufi_sec->share_key, blufi_sec->share_len);

        uint8_t enc_hash[BLUFI_HASH_SIZE];
        status = psa_hash_compute(PSA_ALG_SHA_256, enc_material, BLUFI_IV_PREFIX_LEN + blufi_sec->share_len,
                                 enc_hash, BLUFI_HASH_SIZE, &hash_length);
        if (status != PSA_SUCCESS) {
            BLUFI_ERROR("%s encryption IV derivation failed %d\n", __func__, status);
            btc_blufi_report_error(ESP_BLUFI_DH_MALLOC_ERROR);
            blufi_cleanup_negotiation(false, false);
            return;
        }
        mbedtls_platform_zeroize(enc_material, sizeof(enc_material));

        uint8_t dec_material[BLUFI_IV_PREFIX_LEN + SHARE_KEY_LEN];
        memcpy(dec_material, BLUFI_DEC_DOMAIN_STR, BLUFI_IV_PREFIX_LEN);
        memcpy(dec_material + BLUFI_IV_PREFIX_LEN, blufi_sec->share_key, blufi_sec->share_len);

        uint8_t dec_hash[BLUFI_HASH_SIZE];
        status = psa_hash_compute(PSA_ALG_SHA_256, dec_material, BLUFI_IV_PREFIX_LEN + blufi_sec->share_len,
                                 dec_hash, BLUFI_HASH_SIZE, &hash_length);
        if (status != PSA_SUCCESS) {
            BLUFI_ERROR("%s decryption IV derivation failed %d\n", __func__, status);
            btc_blufi_report_error(ESP_BLUFI_DH_MALLOC_ERROR);
            blufi_cleanup_negotiation(false, false);
            return;
        }
        mbedtls_platform_zeroize(dec_material, sizeof(dec_material));

        /* Use first 16 bytes (128 bits) as IV for CTR mode */
        uint8_t iv_enc[BLUFI_IV_SIZE], iv_dec[BLUFI_IV_SIZE];
        memcpy(iv_enc, enc_hash, BLUFI_IV_SIZE);
        memcpy(iv_dec, dec_hash, BLUFI_IV_SIZE);
        mbedtls_platform_zeroize(enc_hash, sizeof(enc_hash));
        mbedtls_platform_zeroize(dec_hash, sizeof(dec_hash));

        /* Setup persistent cipher operations */
        blufi_sec->enc_operation = psa_cipher_operation_init();
        status = psa_cipher_encrypt_setup(&blufi_sec->enc_operation, blufi_sec->aes_key, PSA_ALG_CTR);
        if (status != PSA_SUCCESS) {
            BLUFI_ERROR("%s encryption setup failed %d\n", __func__, status);
            btc_blufi_report_error(ESP_BLUFI_DH_MALLOC_ERROR);
            blufi_cleanup_negotiation(false, false);
            return;
        }

        status = psa_cipher_set_iv(&blufi_sec->enc_operation, iv_enc, BLUFI_IV_SIZE);
        if (status != PSA_SUCCESS) {
            BLUFI_ERROR("%s encryption IV setup failed %d\n", __func__, status);
            btc_blufi_report_error(ESP_BLUFI_DH_MALLOC_ERROR);
            blufi_cleanup_negotiation(true, false);
            return;
        }
        mbedtls_platform_zeroize(iv_enc, sizeof(iv_enc));

        blufi_sec->dec_operation = psa_cipher_operation_init();
        /* Note: CTR mode uses encrypt_setup for both encryption and decryption */
        status = psa_cipher_encrypt_setup(&blufi_sec->dec_operation, blufi_sec->aes_key, PSA_ALG_CTR);
        if (status != PSA_SUCCESS) {
            BLUFI_ERROR("%s decryption setup failed %d\n", __func__, status);
            btc_blufi_report_error(ESP_BLUFI_DH_MALLOC_ERROR);
            blufi_cleanup_negotiation(true, false);
            return;
        }

        status = psa_cipher_set_iv(&blufi_sec->dec_operation, iv_dec, BLUFI_IV_SIZE);
        if (status != PSA_SUCCESS) {
            BLUFI_ERROR("%s decryption IV setup failed %d\n", __func__, status);
            btc_blufi_report_error(ESP_BLUFI_DH_MALLOC_ERROR);
            blufi_cleanup_negotiation(true, true);
            return;
        }
        mbedtls_platform_zeroize(iv_dec, sizeof(iv_dec));

        /* alloc output data */
        *output_data = &blufi_sec->self_public_key[0];
        *output_len = public_key_len;
        *need_free = false;
    }
        break;
    case SEC_TYPE_DH_P:
        break;
    case SEC_TYPE_DH_G:
        break;
    case SEC_TYPE_DH_PUBLIC:
        break;
    default:
        /* Reject unknown packet types */
        BLUFI_ERROR("Unknown packet type: 0x%02x", type);
        btc_blufi_report_error(ESP_BLUFI_DATA_FORMAT_ERROR);
        break;
    }
}

int blufi_aes_encrypt(uint8_t iv8, uint8_t *crypt_data, int crypt_len)
{
    (void)iv8;  /* iv8 parameter is no longer used with persistent CTR operation */

    if (!blufi_sec || !crypt_data || crypt_len < 0) {
        return -1;
    }

    if (crypt_len == 0) {
        return 0;
    }

    if (blufi_sec->aes_key == 0) {
        BLUFI_ERROR("AES key is not initialized");
        return -1;
    }

    /* Use persistent cipher operation - CTR counter is automatically managed */
    size_t output_len = 0;
    size_t output_buf_size = PSA_CIPHER_ENCRYPT_OUTPUT_SIZE(PSA_KEY_TYPE_AES, PSA_ALG_CTR, crypt_len);
    uint8_t *output_buf = (uint8_t *)malloc(output_buf_size);
    if (output_buf == NULL) {
        BLUFI_ERROR("Failed to allocate output buffer");
        return -1;
    }

    psa_status_t status = psa_cipher_update(&blufi_sec->enc_operation, crypt_data, crypt_len,
                                           output_buf, output_buf_size, &output_len);
    if (status != PSA_SUCCESS) {
        BLUFI_ERROR("Encryption failed with status=%d", status);
        free(output_buf);
        return -1;
    }

    if (output_len > (size_t)crypt_len) {
        BLUFI_ERROR("Output length %d exceeds input buffer size %d", output_len, crypt_len);
        free(output_buf);
        return -1;
    }

    memcpy(crypt_data, output_buf, output_len);
    free(output_buf);

    return output_len;
}

int blufi_aes_decrypt(uint8_t iv8, uint8_t *crypt_data, int crypt_len)
{
    (void)iv8;  /* iv8 parameter is no longer used with persistent CTR operation */

    if (!blufi_sec || !crypt_data || crypt_len < 0) {
        return -1;
    }

    if (crypt_len == 0) {
        return 0;
    }

    if (blufi_sec->aes_key == 0) {
        BLUFI_ERROR("AES key is not initialized");
        return -1;
    }

    /* Use persistent cipher operation - CTR counter is automatically managed */
    size_t output_len = 0;
    size_t output_buf_size = PSA_CIPHER_DECRYPT_OUTPUT_SIZE(PSA_KEY_TYPE_AES, PSA_ALG_CTR, crypt_len);
    uint8_t *output_buf = (uint8_t *)malloc(output_buf_size);
    if (output_buf == NULL) {
        BLUFI_ERROR("Failed to allocate output buffer");
        return -1;
    }

    psa_status_t status = psa_cipher_update(&blufi_sec->dec_operation, crypt_data, crypt_len,
                                           output_buf, output_buf_size, &output_len);
    if (status != PSA_SUCCESS) {
        BLUFI_ERROR("Decryption failed with status=%d", status);
        free(output_buf);
        return -1;
    }

    if (output_len > (size_t)crypt_len) {
        BLUFI_ERROR("Output length %d exceeds input buffer size %d", output_len, crypt_len);
        free(output_buf);
        return -1;
    }

    memcpy(crypt_data, output_buf, output_len);
    free(output_buf);

    return output_len;
}

uint16_t blufi_crc_checksum(uint8_t iv8, uint8_t *data, int len)
{
    /* This iv8 ignore, not used */
    return esp_crc16_be(0, data, len);
}

esp_err_t blufi_security_init(void)
{
    blufi_sec = (struct blufi_security *)malloc(sizeof(struct blufi_security));
    if (blufi_sec == NULL) {
        return ESP_FAIL;
    }

    memset(blufi_sec, 0x0, sizeof(struct blufi_security));
    blufi_sec->enc_operation = psa_cipher_operation_init();
    blufi_sec->dec_operation = psa_cipher_operation_init();

#if !SOC_MPI_SUPPORTED
    /* Connect is after Wi-Fi/BT init. A no-op when boot pregen is still live. */
    blufi_dh_pregen_start_with_cb(NULL);
#endif

    return 0;
}

void blufi_security_deinit(void)
{
    if (blufi_sec == NULL) {
        return;
    }

    /* The pre-generation worker and any keypair it has banked deliberately
     * survive the disconnect: the next connection reuses them, and its task
     * stack could not be reallocated once Wi-Fi and the BLE host are up. */

    /* Clean up all resources */
    blufi_cleanup_negotiation(true, true);

    memset(blufi_sec, 0x0, sizeof(struct blufi_security));

    free(blufi_sec);
    blufi_sec = NULL;
}
