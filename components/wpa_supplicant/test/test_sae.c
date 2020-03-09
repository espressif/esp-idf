// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifdef CONFIG_WPA3_SAE

#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include "unity.h"
#include <string.h>
#include "crypto/crypto.h"
#include "../src/common/sae.h"
#include "utils/wpabuf.h"

typedef struct crypto_bignum crypto_bignum;


static struct wpabuf *wpabuf_alloc2(size_t len)
{
    struct wpabuf *buf = (struct wpabuf *)os_zalloc(sizeof(struct wpabuf) + len);
    if (buf == NULL)
        return NULL;
    buf->size = len;
    return buf;
}

/**
 *  * wpabuf_free - Free a wpabuf
 *   * @buf: wpabuf buffer
 *    */
void wpabuf_free2(struct wpabuf *buf)
{
    if (buf == NULL)
        return;
    os_free(buf->ext_data);
    os_free(buf);
}




TEST_CASE("Test SAE functionality with ECC group", "[wpa3_sae]")
{
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

        TEST_ASSERT(sae_prepare_commit(addr1, addr2, pwd, strlen((const char *)pwd), NULL, &sae) == 0);

        buf = wpabuf_alloc2(SAE_COMMIT_MAX_LEN);

        TEST_ASSERT( buf != NULL);

        sae_write_commit(&sae, buf, NULL, NULL);// No anti-clogging token

        /* Parsing commit created by self will be detected as reflection attack*/
        TEST_ASSERT(sae_parse_commit(&sae,
                    wpabuf_mhead(buf), buf->used, NULL, 0, default_groups) == SAE_SILENTLY_DISCARD);

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
        TEST_ASSERT(sae_prepare_commit(addr1, addr2, pwd, strlen((const char *)pwd), NULL, &sae1) == 0);

        /* STA2 prepares for commit*/
        TEST_ASSERT(sae_prepare_commit(addr2, addr1, pwd, strlen((const char *)pwd), NULL, &sae2) == 0);

        /* STA1 creates commit msg buffer*/
        buf1 = wpabuf_alloc2(SAE_COMMIT_MAX_LEN);
        TEST_ASSERT( buf1 != NULL);
        sae_write_commit(&sae1, buf1, NULL, NULL);// No anti-clogging token
	ESP_LOG_BUFFER_HEXDUMP("SAE: Commit1", wpabuf_mhead_u8(buf1), wpabuf_len(buf1), ESP_LOG_INFO);


        /* STA2 creates commit msg buffer*/
        buf2 = wpabuf_alloc2(SAE_COMMIT_MAX_LEN);
        TEST_ASSERT( buf2 != NULL);
        sae_write_commit(&sae2, buf2, NULL, NULL);// No anti-clogging token
        ESP_LOG_BUFFER_HEXDUMP("SAE: Commit2", wpabuf_mhead_u8(buf2), wpabuf_len(buf2), ESP_LOG_INFO);

        sae1.state = SAE_COMMITTED;
        sae2.state = SAE_COMMITTED;

        /* STA1 parses STA2 commit*/
        TEST_ASSERT(sae_parse_commit(&sae1,
                    wpabuf_mhead(buf2), buf2->used, NULL, 0, default_groups) == 0);

        /* STA2 parses STA1 commit*/
        TEST_ASSERT(sae_parse_commit(&sae2,
                    wpabuf_mhead(buf1), buf1->used, NULL, 0, default_groups) == 0);

        /* STA1 processes commit*/
        TEST_ASSERT(sae_process_commit(&sae1) == 0);

        /* STA2 processes commit*/
        TEST_ASSERT(sae_process_commit(&sae2) == 0);

        /* STA1 creates confirm msg buffer*/
        buf3 = wpabuf_alloc2(SAE_COMMIT_MAX_LEN);
        TEST_ASSERT( buf3 != NULL);
        sae_write_confirm(&sae1, buf3);
        ESP_LOG_BUFFER_HEXDUMP("SAE: Confirm1", wpabuf_mhead_u8(buf3), wpabuf_len(buf3), ESP_LOG_INFO);

        /* STA2 creates confirm msg buffer*/
        buf4 = wpabuf_alloc2(SAE_COMMIT_MAX_LEN);
        TEST_ASSERT( buf3 != NULL);
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
        TEST_ASSERT(sae_prepare_commit(addr1, addr2, pwd1, strlen((const char *)pwd1), NULL, &sae1) == 0);

        /* STA2 prepares for commit*/
        TEST_ASSERT(sae_prepare_commit(addr2, addr1, pwd2, strlen((const char *)pwd2), NULL, &sae2) == 0);

        /* STA1 creates commit msg buffer*/
        buf1 = wpabuf_alloc2(SAE_COMMIT_MAX_LEN);
        TEST_ASSERT( buf1 != NULL);
        sae_write_commit(&sae1, buf1, NULL, NULL);// No anti-clogging token

        /* STA2 creates commit msg buffer*/
        buf2 = wpabuf_alloc2(SAE_COMMIT_MAX_LEN);
        TEST_ASSERT( buf2 != NULL);
        sae_write_commit(&sae2, buf2, NULL, NULL);// No anti-clogging token

        sae1.state = SAE_COMMITTED;
        sae2.state = SAE_COMMITTED;

        /* STA1 parses STA2 commit*/
        TEST_ASSERT(sae_parse_commit(&sae1,
                    wpabuf_mhead(buf2), buf2->used, NULL, 0, default_groups) == 0);

        /* STA2 parses STA1 commit*/
        TEST_ASSERT(sae_parse_commit(&sae2,
                    wpabuf_mhead(buf1), buf1->used, NULL, 0, default_groups) == 0);

        /* STA1 processes commit*/
        TEST_ASSERT(sae_process_commit(&sae1) == 0);

        /* STA2 processes commit*/
        TEST_ASSERT(sae_process_commit(&sae2) == 0);

        /* STA1 creates confirm msg buffer*/
        buf3 = wpabuf_alloc2(SAE_COMMIT_MAX_LEN);
        TEST_ASSERT( buf3 != NULL);
        sae_write_confirm(&sae1, buf3);

        /* STA2 creates confirm msg buffer*/
        buf4 = wpabuf_alloc2(SAE_COMMIT_MAX_LEN);
        TEST_ASSERT( buf3 != NULL);
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
