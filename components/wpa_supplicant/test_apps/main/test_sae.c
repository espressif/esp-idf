/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
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

typedef struct crypto_bignum crypto_bignum;

static struct wpabuf *wpabuf_alloc2(size_t len)
{
    struct wpabuf *buf = (struct wpabuf *)os_zalloc(sizeof(struct wpabuf) + len);
    if (buf == NULL) {
        return NULL;
    }
    buf->size = len;
    buf->buf = (u8 *)(buf + 1);
    return buf;
}

/**
 *  * wpabuf_free - Free a wpabuf
 *   * @buf: wpabuf buffer
 *    */
void wpabuf_free2(struct wpabuf *buf)
{
    if (buf == NULL) {
        return;
    }
    os_free(buf);
}

TEST_CASE("Test SAE functionality with ECC group", "[wpa3_sae]")
{
    set_leak_threshold(400);
    ESP_LOGI("SAE Test", "### Beginning SAE init and deinit ###");
    {
        /* Test init and deinit*/
        struct sae_data sae;
        memset(&sae, 0, sizeof(sae));
        TEST_ASSERT(sae_set_group(&sae, IANA_SECP256R1) == 0);
        sae_clear_temp_data(&sae);
        sae_clear_data(&sae);

    }
    ESP_LOGI("SAE Test", "=========== Complete ============");

    ESP_LOGI("SAE Test", "### Verification of Hunt and Peck, Hash to Element crypto operation using defined vectors ###");
    {
        struct sae_data sae;
        /* IEEE Std 802.11-2020, Annex J.10 */
        const u8 addr1[ETH_ALEN] = { 0x4d, 0x3f, 0x2f, 0xff, 0xe3, 0x87 };
        const u8 addr2[ETH_ALEN] = { 0xa5, 0xd8, 0xaa, 0x95, 0x8e, 0x3c };
        const char *ssid = "byteme";
        const char *pw = "mekmitasdigoat";
        const char *pwid = "psk4internet";
        const u8 local_rand[] = {
            0x99, 0x24, 0x65, 0xfd, 0x3d, 0xaa, 0x3c, 0x60,
            0xaa, 0x65, 0x65, 0xb7, 0xf6, 0x2a, 0x2a, 0x7f,
            0x2e, 0x12, 0xdd, 0x12, 0xf1, 0x98, 0xfa, 0xf4,
            0xfb, 0xed, 0x89, 0xd7, 0xff, 0x1a, 0xce, 0x94
        };
        const u8 local_mask[] = {
            0x95, 0x07, 0xa9, 0x0f, 0x77, 0x7a, 0x04, 0x4d,
            0x6a, 0x08, 0x30, 0xb9, 0x1e, 0xa3, 0xd5, 0xdd,
            0x70, 0xbe, 0xce, 0x44, 0xe1, 0xac, 0xff, 0xb8,
            0x69, 0x83, 0xb5, 0xe1, 0xbf, 0x9f, 0xb3, 0x22
        };
        const u8 local_commit[] = {
            0x13, 0x00, 0x2e, 0x2c, 0x0f, 0x0d, 0xb5, 0x24,
            0x40, 0xad, 0x14, 0x6d, 0x96, 0x71, 0x14, 0xce,
            0x00, 0x5c, 0xe1, 0xea, 0xb0, 0xaa, 0x2c, 0x2e,
            0x5c, 0x28, 0x71, 0xb7, 0x74, 0xf6, 0xc2, 0x57,
            0x5c, 0x65, 0xd5, 0xad, 0x9e, 0x00, 0x82, 0x97,
            0x07, 0xaa, 0x36, 0xba, 0x8b, 0x85, 0x97, 0x38,
            0xfc, 0x96, 0x1d, 0x08, 0x24, 0x35, 0x05, 0xf4,
            0x7c, 0x03, 0x53, 0x76, 0xd7, 0xac, 0x4b, 0xc8,
            0xd7, 0xb9, 0x50, 0x83, 0xbf, 0x43, 0x82, 0x7d,
            0x0f, 0xc3, 0x1e, 0xd7, 0x78, 0xdd, 0x36, 0x71,
            0xfd, 0x21, 0xa4, 0x6d, 0x10, 0x91, 0xd6, 0x4b,
            0x6f, 0x9a, 0x1e, 0x12, 0x72, 0x62, 0x13, 0x25,
            0xdb, 0xe1
        };
        const u8 peer_commit[] = {
            0x13, 0x00, 0x59, 0x1b, 0x96, 0xf3, 0x39, 0x7f,
            0xb9, 0x45, 0x10, 0x08, 0x48, 0xe7, 0xb5, 0x50,
            0x54, 0x3b, 0x67, 0x20, 0xd8, 0x83, 0x37, 0xee,
            0x93, 0xfc, 0x49, 0xfd, 0x6d, 0xf7, 0xe0, 0x8b,
            0x52, 0x23, 0xe7, 0x1b, 0x9b, 0xb0, 0x48, 0xd3,
            0x87, 0x3f, 0x20, 0x55, 0x69, 0x53, 0xa9, 0x6c,
            0x91, 0x53, 0x6f, 0xd8, 0xee, 0x6c, 0xa9, 0xb4,
            0xa6, 0x8a, 0x14, 0x8b, 0x05, 0x6a, 0x90, 0x9b,
            0xe0, 0x3e, 0x83, 0xae, 0x20, 0x8f, 0x60, 0xf8,
            0xef, 0x55, 0x37, 0x85, 0x80, 0x74, 0xdb, 0x06,
            0x68, 0x70, 0x32, 0x39, 0x98, 0x62, 0x99, 0x9b,
            0x51, 0x1e, 0x0a, 0x15, 0x52, 0xa5, 0xfe, 0xa3,
            0x17, 0xc2
        };
        const u8 kck[] = {
            0x1e, 0x73, 0x3f, 0x6d, 0x9b, 0xd5, 0x32, 0x56,
            0x28, 0x73, 0x04, 0x33, 0x88, 0x31, 0xb0, 0x9a,
            0x39, 0x40, 0x6d, 0x12, 0x10, 0x17, 0x07, 0x3a,
            0x5c, 0x30, 0xdb, 0x36, 0xf3, 0x6c, 0xb8, 0x1a
        };
        const u8 pmk[] = {
            0x4e, 0x4d, 0xfa, 0xb1, 0xa2, 0xdd, 0x8a, 0xc1,
            0xa9, 0x17, 0x90, 0xf9, 0x53, 0xfa, 0xaa, 0x45,
            0x2a, 0xe5, 0xc6, 0x87, 0x3a, 0xb7, 0x5b, 0x63,
            0x60, 0x5b, 0xa6, 0x63, 0xf8, 0xa7, 0xfe, 0x59
        };
        const u8 pmkid[] = {
            0x87, 0x47, 0xa6, 0x00, 0xee, 0xa3, 0xf9, 0xf2,
            0x24, 0x75, 0xdf, 0x58, 0xca, 0x1e, 0x54, 0x98
        };
        struct wpabuf *buf = NULL;
        struct crypto_bignum *mask = NULL;
        const u8 pwe_19_x[32] = {
            0xc9, 0x30, 0x49, 0xb9, 0xe6, 0x40, 0x00, 0xf8,
            0x48, 0x20, 0x16, 0x49, 0xe9, 0x99, 0xf2, 0xb5,
            0xc2, 0x2d, 0xea, 0x69, 0xb5, 0x63, 0x2c, 0x9d,
            0xf4, 0xd6, 0x33, 0xb8, 0xaa, 0x1f, 0x6c, 0x1e
        };
        const u8 pwe_19_y[32] = {
            0x73, 0x63, 0x4e, 0x94, 0xb5, 0x3d, 0x82, 0xe7,
            0x38, 0x3a, 0x8d, 0x25, 0x81, 0x99, 0xd9, 0xdc,
            0x1a, 0x5e, 0xe8, 0x26, 0x9d, 0x06, 0x03, 0x82,
            0xcc, 0xbf, 0x33, 0xe6, 0x14, 0xff, 0x59, 0xa0
        };

        int pt_groups[] = { 19 };
        struct sae_pt *pt_info, *pt;
        const u8 addr1b[ETH_ALEN] = { 0x00, 0x09, 0x5b, 0x66, 0xec, 0x1e };
        const u8 addr2b[ETH_ALEN] = { 0x00, 0x0b, 0x6b, 0xd9, 0x02, 0x46 };

        os_memset(&sae, 0, sizeof(sae));
        buf = wpabuf_alloc2(1000);
        TEST_ASSERT(buf != NULL);
        TEST_ASSERT(sae_set_group(&sae, 19) == 0);
        TEST_ASSERT(sae_prepare_commit(addr1, addr2, (const u8 *) pw, os_strlen(pw), &sae) == 0);

        /* Override local values based on SAE test vector */
        crypto_bignum_deinit(sae.tmp->sae_rand, 1);
        sae.tmp->sae_rand = crypto_bignum_init_set(local_rand, sizeof(local_rand));
        mask = crypto_bignum_init_set(local_mask, sizeof(local_mask));

        TEST_ASSERT(sae.tmp->sae_rand != NULL);
        TEST_ASSERT(mask != NULL);

        TEST_ASSERT(crypto_bignum_add(sae.tmp->sae_rand, mask, sae.tmp->own_commit_scalar) == 0);
        TEST_ASSERT(crypto_bignum_mod(sae.tmp->own_commit_scalar, sae.tmp->order, sae.tmp->own_commit_scalar) == 0);
        TEST_ASSERT(crypto_ec_point_mul(sae.tmp->ec, sae.tmp->pwe_ecc, mask, sae.tmp->own_commit_element_ecc) == 0);
        TEST_ASSERT(crypto_ec_point_invert(sae.tmp->ec, sae.tmp->own_commit_element_ecc) == 0);

        TEST_ASSERT(sae_write_commit(&sae, buf, NULL, NULL) == 0);

        /* Check that output matches the test vector */
        TEST_ASSERT(wpabuf_len(buf) == sizeof(local_commit));
        ESP_LOG_BUFFER_HEXDUMP("SAE: Derived SAE Commit    ", wpabuf_mhead_u8(buf), wpabuf_len(buf), ESP_LOG_INFO);
        ESP_LOG_BUFFER_HEXDUMP("SAE: Predefined SAE Commit ", local_commit, sizeof(local_commit), ESP_LOG_INFO);
        TEST_ASSERT(os_memcmp(wpabuf_head(buf), local_commit, sizeof(local_commit)) == 0);

        TEST_ASSERT(sae_parse_commit(&sae, peer_commit, sizeof(peer_commit), NULL, NULL, NULL, 0) == 0);
        TEST_ASSERT(sae_process_commit(&sae) == 0);

        ESP_LOGI("SAE TEST", "### Compare derived KCK,PMK,PMKID with predefined vectors ###");
        ESP_LOG_BUFFER_HEXDUMP("SAE: Derived KCK    ",  sae.tmp->kck, SAE_KCK_LEN, ESP_LOG_INFO);
        ESP_LOG_BUFFER_HEXDUMP("SAE: Predefined KCK ", kck, SAE_KCK_LEN, ESP_LOG_INFO);
        TEST_ASSERT(os_memcmp(kck, sae.tmp->kck, SAE_KCK_LEN) == 0);

        ESP_LOG_BUFFER_HEXDUMP("SAE: Derived PMK    ",  sae.pmk, SAE_PMK_LEN, ESP_LOG_INFO);
        ESP_LOG_BUFFER_HEXDUMP("SAE: Predefined PMK ", pmk, SAE_PMK_LEN, ESP_LOG_INFO);
        TEST_ASSERT(os_memcmp(pmk, sae.pmk, SAE_PMK_LEN) == 0);

        ESP_LOG_BUFFER_HEXDUMP("SAE: Derived PMKID    ", sae.pmkid, SAE_PMKID_LEN, ESP_LOG_INFO);
        ESP_LOG_BUFFER_HEXDUMP("SAE: Predefined PMKID ", pmkid, SAE_PMKID_LEN, ESP_LOG_INFO);
        TEST_ASSERT(os_memcmp(pmkid, sae.pmkid, SAE_PMKID_LEN) == 0);

        pt_info = sae_derive_pt(pt_groups,
                                (const u8 *) ssid, os_strlen(ssid),
                                (const u8 *) pw, os_strlen(pw), pwid);

        TEST_ASSERT(pt_info != NULL);

        struct crypto_ec_point *pwe;
        u8 bin[SAE_MAX_ECC_PRIME_LEN * 2];
        size_t prime_len = sizeof(pwe_19_x);

        pt = pt_info;
        pwe = sae_derive_pwe_from_pt_ecc(pt, addr1b, addr2b);

        TEST_ASSERT(pwe != NULL);
        TEST_ASSERT(crypto_ec_point_to_bin(pt->ec, pwe, bin, bin + prime_len) == 0);
        ESP_LOGI("SAE TEST", "### Compare derived SAE: PT.x and SAE: PT.y with predefined vectors ###");

        ESP_LOG_BUFFER_HEXDUMP("SAE: Derived SAE: PT.x    ", bin, prime_len, ESP_LOG_INFO);
        ESP_LOG_BUFFER_HEXDUMP("SAE: Predefined SAE: PT.x ", pwe_19_x, prime_len, ESP_LOG_INFO);
        TEST_ASSERT(os_memcmp(pwe_19_x, bin, prime_len) == 0);

        ESP_LOG_BUFFER_HEXDUMP("SAE: Derived SAE: PT.y    ", bin + prime_len, prime_len, ESP_LOG_INFO);
        ESP_LOG_BUFFER_HEXDUMP("SAE: Predefined SAE: PT.y ", pwe_19_y, prime_len, ESP_LOG_INFO);
        TEST_ASSERT(os_memcmp(pwe_19_y, bin + prime_len, prime_len) == 0);

        crypto_ec_point_deinit(pwe, 1);
        sae_deinit_pt(pt_info);
        sae_clear_data(&sae);
        wpabuf_free(buf);
        crypto_bignum_deinit(mask, 1);
    }
    ESP_LOGI("SAE Test", "=========== Complete ============");

    ESP_LOGI("SAE Test", "### Beginning SAE commit msg formation and parsing ###");
    {
        /* Test SAE commit msg formation and parsing*/
        struct sae_data sae;
        u8 addr1[ETH_ALEN] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0x11};
        u8 addr2[ETH_ALEN] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
        u8 pwd[] = "ESP32-WPA3";
        struct wpabuf *buf;
        int default_groups[] = { IANA_SECP256R1, 0 };

        memset(&sae, 0, sizeof(sae));

        TEST_ASSERT(sae_set_group(&sae, IANA_SECP256R1) == 0);

        TEST_ASSERT(sae_prepare_commit(addr1, addr2, pwd, strlen((const char *)pwd), &sae) == 0);

        buf = wpabuf_alloc2(SAE_COMMIT_MAX_LEN);

        TEST_ASSERT(buf != NULL);

        sae_write_commit(&sae, buf, NULL, NULL);// No anti-clogging token

        /* Parsing commit created by self will be detected as reflection attack*/
        TEST_ASSERT(sae_parse_commit(&sae,
                                     wpabuf_mhead(buf), buf->used, NULL, 0, default_groups, 0) == SAE_SILENTLY_DISCARD);

        wpabuf_free2(buf);
        sae_clear_temp_data(&sae);
        sae_clear_data(&sae);

    }
    ESP_LOGI("SAE Test", "=========== Complete ============");

    ESP_LOGI("SAE Test", "### Beginning SAE handshake ###");
    {
        /* SAE handshake*/
        struct sae_data sae1; // STA1 data
        struct sae_data sae2; // STA2 data
        u8 addr1[ETH_ALEN] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0x11};
        u8 addr2[ETH_ALEN] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
        u8 pwd[] = "ESP32-WPA3";

        memset(&sae1, 0, sizeof(sae1));
        memset(&sae2, 0, sizeof(sae2));

        struct wpabuf *buf1, *buf2, *buf3, *buf4;
        int default_groups[] = { IANA_SECP256R1, 0 };

        TEST_ASSERT(sae_set_group(&sae1, IANA_SECP256R1) == 0);
        TEST_ASSERT(sae_set_group(&sae2, IANA_SECP256R1) == 0);

        /* STA1 prepares for commit*/
        TEST_ASSERT(sae_prepare_commit(addr1, addr2, pwd, strlen((const char *)pwd), &sae1) == 0);

        /* STA2 prepares for commit*/
        TEST_ASSERT(sae_prepare_commit(addr2, addr1, pwd, strlen((const char *)pwd), &sae2) == 0);

        /* STA1 creates commit msg buffer*/
        buf1 = wpabuf_alloc2(SAE_COMMIT_MAX_LEN);
        TEST_ASSERT(buf1 != NULL);
        sae_write_commit(&sae1, buf1, NULL, NULL);// No anti-clogging token
        ESP_LOG_BUFFER_HEXDUMP("SAE: Commit1", wpabuf_mhead_u8(buf1), wpabuf_len(buf1), ESP_LOG_INFO);

        /* STA2 creates commit msg buffer*/
        buf2 = wpabuf_alloc2(SAE_COMMIT_MAX_LEN);
        TEST_ASSERT(buf2 != NULL);
        sae_write_commit(&sae2, buf2, NULL, NULL);// No anti-clogging token
        ESP_LOG_BUFFER_HEXDUMP("SAE: Commit2", wpabuf_mhead_u8(buf2), wpabuf_len(buf2), ESP_LOG_INFO);

        sae1.state = SAE_COMMITTED;
        sae2.state = SAE_COMMITTED;

        /* STA1 parses STA2 commit*/
        TEST_ASSERT(sae_parse_commit(&sae1,
                                     wpabuf_mhead(buf2), buf2->used, NULL, 0, default_groups, 0) == 0);

        /* STA2 parses STA1 commit*/
        TEST_ASSERT(sae_parse_commit(&sae2,
                                     wpabuf_mhead(buf1), buf1->used, NULL, 0, default_groups, 0) == 0);

        /* STA1 processes commit*/
        TEST_ASSERT(sae_process_commit(&sae1) == 0);

        /* STA2 processes commit*/
        TEST_ASSERT(sae_process_commit(&sae2) == 0);

        /* STA1 creates confirm msg buffer*/
        buf3 = wpabuf_alloc2(SAE_COMMIT_MAX_LEN);
        TEST_ASSERT(buf3 != NULL);
        sae_write_confirm(&sae1, buf3);
        ESP_LOG_BUFFER_HEXDUMP("SAE: Confirm1", wpabuf_mhead_u8(buf3), wpabuf_len(buf3), ESP_LOG_INFO);

        /* STA2 creates confirm msg buffer*/
        buf4 = wpabuf_alloc2(SAE_COMMIT_MAX_LEN);
        TEST_ASSERT(buf3 != NULL);
        sae_write_confirm(&sae2, buf4);
        ESP_LOG_BUFFER_HEXDUMP("SAE: Confirm2", wpabuf_mhead_u8(buf4), wpabuf_len(buf4), ESP_LOG_INFO);

        /* STA1 checks confirm from STA2*/
        TEST_ASSERT(sae_check_confirm(&sae1, wpabuf_mhead(buf4), buf4->used) == 0);

        /* STA2 checks confirm from STA1*/
        TEST_ASSERT(sae_check_confirm(&sae2, wpabuf_mhead(buf3), buf3->used) == 0);

        ESP_LOG_BUFFER_HEXDUMP("SAE: PMK1", sae1.pmk, SAE_PMK_LEN, ESP_LOG_INFO);
        ESP_LOG_BUFFER_HEXDUMP("SAE: PMK2", sae2.pmk, SAE_PMK_LEN, ESP_LOG_INFO);

        wpabuf_free2(buf1);
        wpabuf_free2(buf2);
        wpabuf_free2(buf3);
        wpabuf_free2(buf4);
        sae_clear_temp_data(&sae1);
        sae_clear_temp_data(&sae2);
        sae_clear_data(&sae1);
        sae_clear_data(&sae2);

    }
    ESP_LOGI("SAE Test", "=========== Complete ============");

    ESP_LOGI("SAE Test", "### SAE handshake negative testcase. ###");
    {
        /* SAE handshake failure when different passwords are used.*/
        struct sae_data sae1; // STA1 data
        struct sae_data sae2; // STA2 data
        u8 addr1[ETH_ALEN] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0x11};
        u8 addr2[ETH_ALEN] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
        u8 pwd1[] = "abcd1234";
        u8 pwd2[] = "wxyz5678";

        memset(&sae1, 0, sizeof(sae1));
        memset(&sae2, 0, sizeof(sae2));

        struct wpabuf *buf1, *buf2, *buf3, *buf4;
        int default_groups[] = { IANA_SECP256R1, 0 };

        TEST_ASSERT(sae_set_group(&sae1, IANA_SECP256R1) == 0);
        TEST_ASSERT(sae_set_group(&sae2, IANA_SECP256R1) == 0);

        /* STA1 prepares for commit*/
        TEST_ASSERT(sae_prepare_commit(addr1, addr2, pwd1, strlen((const char *)pwd1), &sae1) == 0);

        /* STA2 prepares for commit*/
        TEST_ASSERT(sae_prepare_commit(addr2, addr1, pwd2, strlen((const char *)pwd2), &sae2) == 0);

        /* STA1 creates commit msg buffer*/
        buf1 = wpabuf_alloc2(SAE_COMMIT_MAX_LEN);
        TEST_ASSERT(buf1 != NULL);
        sae_write_commit(&sae1, buf1, NULL, NULL);// No anti-clogging token

        /* STA2 creates commit msg buffer*/
        buf2 = wpabuf_alloc2(SAE_COMMIT_MAX_LEN);
        TEST_ASSERT(buf2 != NULL);
        sae_write_commit(&sae2, buf2, NULL, NULL);// No anti-clogging token

        sae1.state = SAE_COMMITTED;
        sae2.state = SAE_COMMITTED;

        /* STA1 parses STA2 commit*/
        TEST_ASSERT(sae_parse_commit(&sae1,
                                     wpabuf_mhead(buf2), buf2->used, NULL, 0, default_groups, 0) == 0);

        /* STA2 parses STA1 commit*/
        TEST_ASSERT(sae_parse_commit(&sae2,
                                     wpabuf_mhead(buf1), buf1->used, NULL, 0, default_groups, 0) == 0);

        /* STA1 processes commit*/
        TEST_ASSERT(sae_process_commit(&sae1) == 0);

        /* STA2 processes commit*/
        TEST_ASSERT(sae_process_commit(&sae2) == 0);

        /* STA1 creates confirm msg buffer*/
        buf3 = wpabuf_alloc2(SAE_COMMIT_MAX_LEN);
        TEST_ASSERT(buf3 != NULL);
        sae_write_confirm(&sae1, buf3);

        /* STA2 creates confirm msg buffer*/
        buf4 = wpabuf_alloc2(SAE_COMMIT_MAX_LEN);
        TEST_ASSERT(buf3 != NULL);
        sae_write_confirm(&sae2, buf4);

        /* STA1 checks confirm from STA2 and the check fails*/
        TEST_ASSERT(sae_check_confirm(&sae1, wpabuf_mhead(buf4), buf4->used) != 0);

        /* STA2 checks confirm from STA1 and the check fails*/
        TEST_ASSERT(sae_check_confirm(&sae2, wpabuf_mhead(buf3), buf3->used) != 0);

        wpabuf_free2(buf1);
        wpabuf_free2(buf2);
        wpabuf_free2(buf3);
        wpabuf_free2(buf4);
        sae_clear_temp_data(&sae1);
        sae_clear_temp_data(&sae2);
        sae_clear_data(&sae1);
        sae_clear_data(&sae2);

    }
    ESP_LOGI("SAE Test", "=========== Complete ============");

}

#endif /* CONFIG_WPA3_SAE */
