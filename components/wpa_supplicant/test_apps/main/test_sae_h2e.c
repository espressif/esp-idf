/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef CONFIG_WPA3_SAE

#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include "unity.h"
#include <string.h>
#include "utils/common.h"
#include "utils/includes.h"
#include "crypto/crypto.h"
#include "common/sae.h"
#include "utils/wpabuf.h"
#include "test_utils.h"
#include "test_wpa_supplicant_common.h"

/* Helper functions for random SSID and password generation */
static void generate_random_string(char *str, size_t len, bool include_special)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const char special_chars[] = "!@#$%^&*()_+-=[]{}|;:,.<>?";
    size_t charset_len = strlen(charset);
    size_t special_len = strlen(special_chars);

    srand(time(NULL));
    for (size_t i = 0; i < len - 1; i++) {
        if (include_special && (rand() % 4 == 0) && i > 0) { // 25% chance for special char
            str[i] = special_chars[rand() % special_len];
        } else {
            str[i] = charset[rand() % charset_len];
        }
    }
    str[len - 1] = '\0';
}

static void generate_random_ssid(char *ssid, size_t *ssid_len)
{
    *ssid_len = (rand() % 28) + 4; // Random length between 4 and 32
    generate_random_string(ssid, *ssid_len + 1, false); // +1 for null terminator
}

static void generate_random_password(char *password)
{
    size_t len = (rand() % 48) + 16; // Random length between 16 and 63
    generate_random_string(password, len + 1, true); // +1 for null terminator
}

TEST_CASE("Test SAE H2E complete workflow", "[wpa3_sae_h2e]")
{
    set_leak_threshold(600);
    struct sae_data sae;
    struct wpabuf *buf = NULL;
    const u8 addr1[ETH_ALEN] = { 0x4d, 0x3f, 0x2f, 0xff, 0xe3, 0x87 };
    const u8 addr2[ETH_ALEN] = { 0xa5, 0xd8, 0xaa, 0x95, 0x8e, 0x3c };
    char password[64];
    char ssid[33];
    size_t ssid_len;

    /* Generate random SSID and password */
    generate_random_ssid(ssid, &ssid_len);
    generate_random_password(password);

    ESP_LOGI("H2E", "\nUsing random credentials:");
    ESP_LOGI("H2E", "SSID: %s (len=%zu)", ssid, ssid_len);
    ESP_LOGI("H2E", "Password: %s (len=%zu)", password, strlen(password));

    /* Step 1: Test H2E initialization */
    ESP_LOGI("H2E", "\nStep 1: Testing H2E initialization");
    memset(&sae, 0, sizeof(sae));
    ESP_LOGI("H2E", "- Initial SAE state: h2e=%d", sae.h2e);

    TEST_ASSERT_MESSAGE(sae_set_group(&sae, IANA_SECP256R1) == 0, "Failed to set SAE group");
    ESP_LOGI("H2E", "- After group set: group=%d", sae.group);
    TEST_ASSERT_MESSAGE(sae.h2e == 0, "H2E should be disabled by default");

    sae.h2e = 1;
    ESP_LOGI("H2E", "- After H2E enable: h2e=%d", sae.h2e);
    TEST_ASSERT_MESSAGE(sae.h2e == 1, "Failed to enable H2E");

    /* Step 2: Test password identifier validation */
    ESP_LOGI("H2E", "\nStep 2: Testing password identifier validation");
    ESP_LOGI("H2E", "- Addr1: %02x:%02x:%02x:%02x:%02x:%02x",
             addr1[0], addr1[1], addr1[2], addr1[3], addr1[4], addr1[5]);
    ESP_LOGI("H2E", "- Addr2: %02x:%02x:%02x:%02x:%02x:%02x",
             addr2[0], addr2[1], addr2[2], addr2[3], addr2[4], addr2[5]);

    struct sae_pt *pt = sae_derive_pt(NULL, (const u8 *)ssid, ssid_len,
                                      (const u8 *)password, strlen(password), NULL);
    TEST_ASSERT_MESSAGE(pt != NULL, "Failed to derive PT");
    TEST_ASSERT_MESSAGE(sae_prepare_commit_pt(&sae, pt, addr1, addr2, NULL, NULL) == 0,
                        "Failed to prepare SAE commit with password");
    sae.state = SAE_NOTHING;
    ESP_LOGI("H2E", "- After commit prep: h2e=%d, tmp=%p", sae.h2e, (void*)sae.tmp);
    TEST_ASSERT_MESSAGE(sae.h2e == 1, "H2E state not maintained after commit preparation");
    TEST_ASSERT_MESSAGE(sae.tmp != NULL, "Temporary data not allocated");

    /* Step 3: Test commit message validation */
    ESP_LOGI("H2E", "\nStep 3: Testing commit message validation");
    buf = wpabuf_alloc(1000);
    ESP_LOGI("H2E", "- Allocated buffer size: 1000 bytes");
    TEST_ASSERT_MESSAGE(buf != NULL, "Failed to allocate commit message buffer");

    TEST_ASSERT_MESSAGE(sae_write_commit(&sae, buf, NULL, NULL) == 0,
                        "Failed to write SAE commit message");
    sae.state = SAE_COMMITTED;
    ESP_LOGI("H2E", "- Commit message length: %zu bytes", wpabuf_len(buf));
    TEST_ASSERT_MESSAGE(wpabuf_len(buf) > 0, "Commit message length is zero");

    ESP_LOGI("H2E", "- Parsing commit message...");
    /* Reset SAE context for parsing */
    sae_clear_data(&sae);
    memset(&sae, 0, sizeof(sae));

    /* Initialize SAE parameters for parsing */
    TEST_ASSERT_MESSAGE(sae_set_group(&sae, IANA_SECP256R1) == 0, "Failed to set SAE group for parsing");
    sae.h2e = 1;
    sae.state = SAE_COMMITTED;

    /* Parse the commit message */
    TEST_ASSERT_MESSAGE(sae_parse_commit(&sae, wpabuf_head(buf), wpabuf_len(buf),
                                         NULL, NULL, NULL, 1) == 0,
                        "Failed to parse SAE commit message");
    TEST_ASSERT_MESSAGE(sae.peer_commit_scalar != NULL,
                        "Peer commit scalar not parsed");
    TEST_ASSERT_MESSAGE(sae.tmp != NULL &&
                        (sae.tmp->peer_commit_element_ecc != NULL || sae.tmp->peer_commit_element_ffc != NULL),
                        "Peer commit element not parsed");
    ESP_LOGI("H2E", "- Commit message parsed successfully");

    /* Cleanup */
    if (buf) {
        wpabuf_free(buf);
        buf = NULL;
    }

    /* Free SAE PT data */
    if (pt) {
        sae_deinit_pt(pt);
        pt = NULL;
    }

    sae_clear_data(&sae);
}

#endif /* CONFIG_WPA3_SAE */
