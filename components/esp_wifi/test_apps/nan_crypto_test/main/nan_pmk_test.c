/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/*
 * NAN ND-PMK Derivation Test Vectors
 *
 * Test vectors from Wi-Fi Aware Specification v4.0
 * Appendix M.1 - Example test vectors for pass-phrase to ND-PMK conversion
 */

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "utils/common.h"    /* u8 typedef used by sha256.h */
#include "crypto/sha256.h"   /* mbedTLS-backed pbkdf2_sha256 from wpa_supplicant */

static const char *TAG = "nan_pmk_test";

typedef struct {
    const char *passphrase;
    uint8_t cipher_id;
    uint8_t service_id[6];
    uint8_t nmi[6];
    uint8_t expected_pmk[32];
    bool use_pmk;
    uint8_t input_pmk[32];
    const char *description;
} test_vector_t;

/* Test Vector 1 */
static const test_vector_t test_vector_1 = {
    .passphrase = "NAN",
    .cipher_id = 0x01,
    .service_id = {0x2b, 0x9c, 0x45, 0x0f, 0x66, 0x71},
    .nmi = {0x02, 0x90, 0x4c, 0x12, 0xd0, 0x01},
    .expected_pmk = {
        0xee, 0x35, 0x85, 0x06, 0x30, 0x56, 0xd1, 0x64,
        0xd1, 0x54, 0x54, 0xad, 0x39, 0x01, 0x0d, 0x4e,
        0x26, 0x40, 0xb0, 0xd8, 0x2f, 0xb2, 0x4a, 0x2d,
        0x68, 0x99, 0x86, 0x2d, 0x27, 0x3c, 0x68, 0xbf
    },
    .description = "Test Vector 1: Passphrase 'NAN'"
};

/* Test Vector 2 */
static const test_vector_t test_vector_2 = {
    .passphrase = "NAN2",
    .cipher_id = 0x01,
    .service_id = {0x2b, 0x9c, 0x45, 0x0f, 0x66, 0x71},
    .nmi = {0x02, 0x90, 0x4c, 0x12, 0xd0, 0x01},
    .expected_pmk = {
        0x87, 0x53, 0x4f, 0xa7, 0x74, 0xb1, 0x73, 0x2d,
        0xb0, 0x42, 0x66, 0xc4, 0x2c, 0x5d, 0x08, 0xd0,
        0x9e, 0x58, 0x63, 0xd1, 0xda, 0x11, 0xce, 0x25,
        0x76, 0xa8, 0xf1, 0x55, 0xfe, 0x26, 0xcd, 0x2a
    },
    .description = "Test Vector 2: Passphrase 'NAN2'"
};

/* Test Vector 3 */
static const test_vector_t test_vector_3 = {
    .passphrase = "NAN-Testvector-Phrase",
    .cipher_id = 0x01,
    .service_id = {0x2b, 0x9c, 0x45, 0x0f, 0x66, 0x71},
    .nmi = {0x02, 0x90, 0x4c, 0x12, 0xd0, 0x01},
    .expected_pmk = {
        0x4d, 0xc8, 0x6c, 0xcd, 0xa8, 0x04, 0xf4, 0xe2,
        0xe1, 0x39, 0xfc, 0xa5, 0xdd, 0xd2, 0x1b, 0xa5,
        0xc0, 0xb1, 0xb6, 0xed, 0x31, 0xff, 0xd7, 0x00,
        0x5e, 0x2d, 0x56, 0xf1, 0xe7, 0xbf, 0x51, 0x87
    },
    .description = "Test Vector 3: Passphrase 'NAN-Testvector-Phrase'"
};

/* Test Vector 4: Direct PMK */
static const test_vector_t test_vector_4 = {
    .use_pmk = true,
    .input_pmk = {
        0xee, 0x35, 0x85, 0x06, 0x30, 0x56, 0xd1, 0x64,
        0xd1, 0x54, 0x54, 0xad, 0x39, 0x01, 0x0d, 0x4e,
        0x26, 0x40, 0xb0, 0xd8, 0x2f, 0xb2, 0x4a, 0x2d,
        0x68, 0x99, 0x86, 0x2d, 0x27, 0x3c, 0x68, 0xbf
    },
    .expected_pmk = {
        0xee, 0x35, 0x85, 0x06, 0x30, 0x56, 0xd1, 0x64,
        0xd1, 0x54, 0x54, 0xad, 0x39, 0x01, 0x0d, 0x4e,
        0x26, 0x40, 0xb0, 0xd8, 0x2f, 0xb2, 0x4a, 0x2d,
        0x68, 0x99, 0x86, 0x2d, 0x27, 0x3c, 0x68, 0xbf
    },
    .description = "Test Vector 4: Direct PMK (using PMK from Vector 1)"
};

static void print_hex(const char *label, const uint8_t *data, size_t len)
{
    printf("%s: ", label);
    for (size_t i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
}

static bool run_test_vector(const test_vector_t *tv)
{
    uint8_t salt[14];
    uint8_t pmk[32];

    ESP_LOGI(TAG, "\n=== %s ===", tv->description);

    if (tv->use_pmk) {
        ESP_LOGI(TAG, "(Direct PMK Mode)");
        memcpy(pmk, tv->input_pmk, 32);
    } else {
        /* Construct Salt = Salt_Version || Cipher_Suite_ID || Service_ID || Publisher_NMI */
        salt[0] = 0x00;  /* Salt Version */
        salt[1] = tv->cipher_id;
        memcpy(&salt[2], tv->service_id, 6);
        memcpy(&salt[8], tv->nmi, 6);

        print_hex("Passphrase", (const uint8_t *)tv->passphrase, strlen(tv->passphrase));
        print_hex("Salt", salt, sizeof(salt));

        /* Derive PMK using PBKDF2-SHA256 */
        if (pbkdf2_sha256(tv->passphrase, salt, sizeof(salt), 4096, pmk, 32) != 0) {
            ESP_LOGE(TAG, "PBKDF2-SHA256 derivation failed!");
            return false;
        }
    }

    print_hex("Derived PMK  ", pmk, 32);
    print_hex("Expected PMK ", tv->expected_pmk, 32);

    /* Compare with expected PMK */
    if (memcmp(pmk, tv->expected_pmk, 32) == 0) {
        ESP_LOGI(TAG, "✓ TEST PASSED");
        return true;
    } else {
        ESP_LOGE(TAG, "✗ TEST FAILED - PMK mismatch!");
        return false;
    }
}

void nan_pmk_run_test_vectors(void)
{
    int passed = 0;
    int total = 4;

    ESP_LOGI(TAG, "\n");
    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "  NAN ND-PMK Derivation Test Vectors");
    ESP_LOGI(TAG, "  Wi-Fi Aware Spec v4.0 - Appendix M.1");
    ESP_LOGI(TAG, "========================================");

    if (run_test_vector(&test_vector_1)) {
        passed++;
    }
    if (run_test_vector(&test_vector_2)) {
        passed++;
    }
    if (run_test_vector(&test_vector_3)) {
        passed++;
    }
    if (run_test_vector(&test_vector_4)) {
        passed++;
    }

    ESP_LOGI(TAG, "\n");
    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "  Test Results: %d/%d PASSED", passed, total);
    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "\n");
}
