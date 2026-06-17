/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
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
#include "esp_timer.h"

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
    struct sae_data sae1;
    struct sae_data sae2;
    struct wpabuf *buf1 = NULL;
    struct wpabuf *buf2 = NULL;
    struct wpabuf *confirm1 = NULL;
    struct wpabuf *confirm2 = NULL;
    const u8 addr1[ETH_ALEN] = { 0x4d, 0x3f, 0x2f, 0xff, 0xe3, 0x87 };
    const u8 addr2[ETH_ALEN] = { 0xa5, 0xd8, 0xaa, 0x95, 0x8e, 0x3c };
    char password[64];
    char ssid[33];
    size_t ssid_len;
    struct sae_pt *pt = NULL;
    int default_groups[] = { IANA_SECP256R1, 0 };
    int64_t start_us;
    int64_t prepare1_us, write1_us, form1_us, parse1_us;
    int64_t prepare2_us, write2_us, form2_us, parse2_us;
    int64_t process1_us, process2_us;

    /* Generate random SSID and password */
    generate_random_ssid(ssid, &ssid_len);
    generate_random_password(password);

    ESP_LOGI("H2E", "\nUsing random credentials:");
    ESP_LOGI("H2E", "SSID: %s (len=%zu)", ssid, ssid_len);
    ESP_LOGI("H2E", "Password: %s (len=%zu)", password, strlen(password));

    /* Step 1: Test H2E initialization */
    ESP_LOGI("H2E", "\nStep 1: Testing H2E initialization");
    memset(&sae1, 0, sizeof(sae1));
    memset(&sae2, 0, sizeof(sae2));
    ESP_LOGI("H2E", "- Initial SAE state: sta1.h2e=%d sta2.h2e=%d",
             sae1.h2e, sae2.h2e);

    TEST_ASSERT_MESSAGE(sae_set_group(&sae1, IANA_SECP256R1) == 0, "Failed to set SAE group for STA1");
    TEST_ASSERT_MESSAGE(sae_set_group(&sae2, IANA_SECP256R1) == 0, "Failed to set SAE group for STA2");
    ESP_LOGI("H2E", "- After group set: sta1.group=%d sta2.group=%d", sae1.group, sae2.group);
    TEST_ASSERT_MESSAGE(sae1.h2e == 0 && sae2.h2e == 0, "H2E should be disabled by default");

    sae1.h2e = 1;
    sae2.h2e = 1;
    ESP_LOGI("H2E", "- After H2E enable: sta1.h2e=%d sta2.h2e=%d", sae1.h2e, sae2.h2e);
    TEST_ASSERT_MESSAGE(sae1.h2e == 1 && sae2.h2e == 1, "Failed to enable H2E");

    /* Step 2: Derive PT and prepare commits for both peers */
    ESP_LOGI("H2E", "\nStep 2: Deriving PT and preparing commits");
    ESP_LOGI("H2E", "- Addr1: %02x:%02x:%02x:%02x:%02x:%02x",
             addr1[0], addr1[1], addr1[2], addr1[3], addr1[4], addr1[5]);
    ESP_LOGI("H2E", "- Addr2: %02x:%02x:%02x:%02x:%02x:%02x",
             addr2[0], addr2[1], addr2[2], addr2[3], addr2[4], addr2[5]);

    pt = sae_derive_pt(NULL, (const u8 *)ssid, ssid_len,
                       (const u8 *)password, strlen(password), NULL);
    TEST_ASSERT_MESSAGE(pt != NULL, "Failed to derive PT");
    start_us = esp_timer_get_time();
    TEST_ASSERT_MESSAGE(sae_prepare_commit_pt(&sae1, pt, addr1, addr2, NULL, NULL) == 0,
                        "Failed to prepare SAE commit for STA1");
    prepare1_us = esp_timer_get_time() - start_us;
    TEST_ASSERT_MESSAGE(sae1.h2e == 1 && sae1.tmp != NULL,
                        "STA1 H2E state not maintained after commit preparation");

    start_us = esp_timer_get_time();
    TEST_ASSERT_MESSAGE(sae_prepare_commit_pt(&sae2, pt, addr2, addr1, NULL, NULL) == 0,
                        "Failed to prepare SAE commit for STA2");
    prepare2_us = esp_timer_get_time() - start_us;
    TEST_ASSERT_MESSAGE(sae2.h2e == 1 && sae2.tmp != NULL,
                        "STA2 H2E state not maintained after commit preparation");

    /* Step 3: Form commit messages and measure timing */
    ESP_LOGI("H2E", "\nStep 3: Forming commit messages");
    buf1 = wpabuf_alloc(SAE_COMMIT_MAX_LEN);
    buf2 = wpabuf_alloc(SAE_COMMIT_MAX_LEN);
    TEST_ASSERT_MESSAGE(buf1 != NULL && buf2 != NULL, "Failed to allocate commit message buffers");

    start_us = esp_timer_get_time();
    TEST_ASSERT_MESSAGE(sae_write_commit(&sae1, buf1, NULL, NULL) == 0,
                        "Failed to write SAE commit message for STA1");
    write1_us = esp_timer_get_time() - start_us;
    form1_us = prepare1_us + write1_us;
    sae1.state = SAE_COMMITTED;

    start_us = esp_timer_get_time();
    TEST_ASSERT_MESSAGE(sae_write_commit(&sae2, buf2, NULL, NULL) == 0,
                        "Failed to write SAE commit message for STA2");
    write2_us = esp_timer_get_time() - start_us;
    form2_us = prepare2_us + write2_us;
    sae2.state = SAE_COMMITTED;

    ESP_LOGI("H2E",
             "Commit formation timing(us): sta1_prepare=%lld sta1_write=%lld sta1_total=%lld sta2_prepare=%lld sta2_write=%lld sta2_total=%lld",
             (long long) prepare1_us, (long long) write1_us, (long long) form1_us,
             (long long) prepare2_us, (long long) write2_us, (long long) form2_us);
    ESP_LOGI("H2E", "- Commit1 length: %zu bytes", wpabuf_len(buf1));
    ESP_LOGI("H2E", "- Commit2 length: %zu bytes", wpabuf_len(buf2));
    TEST_ASSERT_MESSAGE(wpabuf_len(buf1) > 0 && wpabuf_len(buf2) > 0,
                        "Commit message length is zero");

    /* Step 4: Parse commits and complete H2E handshake */
    ESP_LOGI("H2E", "\nStep 4: Parsing commits and completing handshake");
    start_us = esp_timer_get_time();
    TEST_ASSERT_MESSAGE(sae_parse_commit(&sae1, wpabuf_head(buf2), wpabuf_len(buf2),
                                         NULL, 0, default_groups, 1) == 0,
                        "Failed to parse SAE commit message for STA1");
    parse1_us = esp_timer_get_time() - start_us;
    TEST_ASSERT_MESSAGE(sae1.peer_commit_scalar != NULL &&
                        sae1.tmp != NULL &&
                        (sae1.tmp->peer_commit_element_ecc != NULL || sae1.tmp->peer_commit_element_ffc != NULL),
                        "STA1 peer commit not parsed");

    start_us = esp_timer_get_time();
    TEST_ASSERT_MESSAGE(sae_parse_commit(&sae2, wpabuf_head(buf1), wpabuf_len(buf1),
                                         NULL, 0, default_groups, 1) == 0,
                        "Failed to parse SAE commit message for STA2");
    parse2_us = esp_timer_get_time() - start_us;
    TEST_ASSERT_MESSAGE(sae2.peer_commit_scalar != NULL &&
                        sae2.tmp != NULL &&
                        (sae2.tmp->peer_commit_element_ecc != NULL || sae2.tmp->peer_commit_element_ffc != NULL),
                        "STA2 peer commit not parsed");

    ESP_LOGI("H2E",
             "Commit parse timing(us): sta1=%lld sta2=%lld avg=%lld",
             (long long) parse1_us, (long long) parse2_us,
             (long long)((parse1_us + parse2_us) / 2));

    start_us = esp_timer_get_time();
    TEST_ASSERT_MESSAGE(sae_process_commit(&sae1) == 0, "Failed to process commit for STA1");
    process1_us = esp_timer_get_time() - start_us;

    start_us = esp_timer_get_time();
    TEST_ASSERT_MESSAGE(sae_process_commit(&sae2) == 0, "Failed to process commit for STA2");
    process2_us = esp_timer_get_time() - start_us;

    ESP_LOGI("H2E",
             "Process commit timing(us): sta1=%lld sta2=%lld avg=%lld",
             (long long) process1_us, (long long) process2_us,
             (long long)((process1_us + process2_us) / 2));

    confirm1 = wpabuf_alloc(SAE_COMMIT_MAX_LEN);
    confirm2 = wpabuf_alloc(SAE_COMMIT_MAX_LEN);
    TEST_ASSERT_MESSAGE(confirm1 != NULL && confirm2 != NULL, "Failed to allocate confirm buffers");

    sae_write_confirm(&sae1, confirm1);
    sae_write_confirm(&sae2, confirm2);

    TEST_ASSERT_MESSAGE(sae_check_confirm(&sae1, wpabuf_head(confirm2), wpabuf_len(confirm2)) == 0,
                        "Failed to verify confirm for STA1");
    TEST_ASSERT_MESSAGE(sae_check_confirm(&sae2, wpabuf_head(confirm1), wpabuf_len(confirm1)) == 0,
                        "Failed to verify confirm for STA2");
    ESP_LOGI("H2E", "- Full H2E handshake completed successfully");

    if (confirm1) {
        wpabuf_free(confirm1);
    }
    if (confirm2) {
        wpabuf_free(confirm2);
    }
    if (buf1) {
        wpabuf_free(buf1);
    }
    if (buf2) {
        wpabuf_free(buf2);
    }
    if (pt) {
        sae_deinit_pt(pt);
    }
    sae_clear_data(&sae1);
    sae_clear_data(&sae2);
}

#endif /* CONFIG_WPA3_SAE */
