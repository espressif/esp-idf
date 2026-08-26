/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <esp_err.h>
#include <esp_log.h>
#include <unity.h>
#include <protocomm.h>
#include <protocomm_security.h>
#include <protocomm_security2.h>
#include "session.pb-c.h"
#include "sec1.pb-c.h"
#include "sec2.pb-c.h"

#if CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_2

static const char *TAG = "test_security2";

/*
 * Pre-computed salt and verifier for username="wifiprov", password="abcd1234".
 * Identical to the vectors used in test_srp.c.
 */
static const char sec2_salt[] = {
    0x03, 0x6e, 0xe0, 0xc7, 0xbc, 0xb9, 0xed, 0xa8,
    0x4c, 0x9e, 0xac, 0x97, 0xd9, 0x3d, 0xec, 0xf4
};

static const char sec2_verifier[] = {
    0x7c, 0x7c, 0x85, 0x47, 0x65, 0x08, 0x94, 0x6d, 0xd6, 0x36, 0xaf, 0x37, 0xd7, 0xe8, 0x91, 0x43,
    0x78, 0xcf, 0xfd, 0x61, 0x6c, 0x59, 0xd2, 0xf8, 0x39, 0x08, 0x12, 0x72, 0x38, 0xde, 0x9e, 0x24,
    0xa4, 0x70, 0x26, 0x1c, 0xdf, 0xa9, 0x03, 0xc2, 0xb2, 0x70, 0xe7, 0xb1, 0x32, 0x24, 0xda, 0x11,
    0x1d, 0x97, 0x18, 0xdc, 0x60, 0x72, 0x08, 0xcc, 0x9a, 0xc9, 0x0c, 0x48, 0x27, 0xe2, 0xae, 0x89,
    0xaa, 0x16, 0x25, 0xb8, 0x04, 0xd2, 0x1a, 0x9b, 0x3a, 0x8f, 0x37, 0xf6, 0xe4, 0x3a, 0x71, 0x2e,
    0xe1, 0x27, 0x86, 0x6e, 0xad, 0xce, 0x28, 0xff, 0x54, 0x46, 0x60, 0x1f, 0xb9, 0x96, 0x87, 0xdc,
    0x57, 0x40, 0xa7, 0xd4, 0x6c, 0xc9, 0x77, 0x54, 0xdc, 0x16, 0x82, 0xf0, 0xed, 0x35, 0x6a, 0xc4,
    0x70, 0xad, 0x3d, 0x90, 0xb5, 0x81, 0x94, 0x70, 0xd7, 0xbc, 0x65, 0xb2, 0xd5, 0x18, 0xe0, 0x2e,
    0xc3, 0xa5, 0xf9, 0x68, 0xdd, 0x64, 0x7b, 0xb8, 0xb7, 0x3c, 0x9c, 0xfc, 0x00, 0xd8, 0x71, 0x7e,
    0xb7, 0x9a, 0x7c, 0xb1, 0xb7, 0xc2, 0xc3, 0x18, 0x34, 0x29, 0x32, 0x43, 0x3e, 0x00, 0x99, 0xe9,
    0x82, 0x94, 0xe3, 0xd8, 0x2a, 0xb0, 0x96, 0x29, 0xb7, 0xdf, 0x0e, 0x5f, 0x08, 0x33, 0x40, 0x76,
    0x52, 0x91, 0x32, 0x00, 0x9f, 0x97, 0x2c, 0x89, 0x6c, 0x39, 0x1e, 0xc8, 0x28, 0x05, 0x44, 0x17,
    0x3f, 0x68, 0x02, 0x8a, 0x9f, 0x44, 0x61, 0xd1, 0xf5, 0xa1, 0x7e, 0x5a, 0x70, 0xd2, 0xc7, 0x23,
    0x81, 0xcb, 0x38, 0x68, 0xe4, 0x2c, 0x20, 0xbc, 0x40, 0x57, 0x76, 0x17, 0xbd, 0x08, 0xb8, 0x96,
    0xbc, 0x26, 0xeb, 0x32, 0x46, 0x69, 0x35, 0x05, 0x8c, 0x15, 0x70, 0xd9, 0x1b, 0xe9, 0xbe, 0xcc,
    0xa9, 0x38, 0xa6, 0x67, 0xf0, 0xad, 0x50, 0x13, 0x19, 0x72, 0x64, 0xbf, 0x52, 0xc2, 0x34, 0xe2,
    0x1b, 0x11, 0x79, 0x74, 0x72, 0xbd, 0x34, 0x5b, 0xb1, 0xe2, 0xfd, 0x66, 0x73, 0xfe, 0x71, 0x64,
    0x74, 0xd0, 0x4e, 0xbc, 0x51, 0x24, 0x19, 0x40, 0x87, 0x0e, 0x92, 0x40, 0xe6, 0x21, 0xe7, 0x2d,
    0x4e, 0x37, 0x76, 0x2f, 0x2e, 0xe2, 0x68, 0xc7, 0x89, 0xe8, 0x32, 0x13, 0x42, 0x06, 0x84, 0x84,
    0x53, 0x4a, 0xb3, 0x0c, 0x1b, 0x4c, 0x8d, 0x1c, 0x51, 0x97, 0x19, 0xab, 0xae, 0x77, 0xff, 0xdb,
    0xec, 0xf0, 0x10, 0x95, 0x34, 0x33, 0x6b, 0xcb, 0x3e, 0x84, 0x0f, 0xb9, 0xd8, 0x5f, 0xb8, 0xa0,
    0xb8, 0x55, 0x53, 0x3e, 0x70, 0xf7, 0x18, 0xf5, 0xce, 0x7b, 0x4e, 0xbf, 0x27, 0xce, 0xce, 0xa8,
    0xb3, 0xbe, 0x40, 0xc5, 0xc5, 0x32, 0x29, 0x3e, 0x71, 0x64, 0x9e, 0xde, 0x8c, 0xf6, 0x75, 0xa1,
    0xe6, 0xf6, 0x53, 0xc8, 0x31, 0xa8, 0x78, 0xde, 0x50, 0x40, 0xf7, 0x62, 0xde, 0x36, 0xb2, 0xba
};

/* Must match PUBLIC_KEY_LEN in security2.c */
#define SEC2_PUBLIC_KEY_LEN  384

static esp_err_t test_cmd0_oversized_username(void)
{
    esp_err_t ret;

    protocomm_t *pc = protocomm_new();
    if (!pc) {
        return ESP_ERR_NO_MEM;
    }

    protocomm_security2_params_t sv = {
        .salt         = sec2_salt,
        .salt_len     = sizeof(sec2_salt),
        .verifier     = sec2_verifier,
        .verifier_len = sizeof(sec2_verifier),
    };

    ret = protocomm_set_security(pc, "test-sec", &protocomm_security2, &sv);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "protocomm_set_security failed (0x%x)", ret);
        protocomm_delete(pc);
        return ret;
    }

    const uint32_t session_id = 100;
    ret = protocomm_open_session(pc, session_id);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "protocomm_open_session failed (0x%x)", ret);
        protocomm_delete(pc);
        return ret;
    }

    const size_t username_len = (size_t)UINT16_MAX + 2;
    uint8_t *username_buf = calloc(1, username_len);
    if (!username_buf) {
        ESP_LOGE(TAG, "Cannot allocate %zu-byte username buffer — skipping test", username_len);
        protocomm_delete(pc);
        return ESP_ERR_NO_MEM;
    }
    memset(username_buf, 'A', username_len);

    uint8_t client_pubkey[SEC2_PUBLIC_KEY_LEN];
    memset(client_pubkey, 0xAB, sizeof(client_pubkey));

    S2SessionCmd0 cmd0 = S2_SESSION_CMD0__INIT;
    cmd0.client_username.data = username_buf;
    cmd0.client_username.len  = username_len;
    cmd0.client_pubkey.data   = client_pubkey;
    cmd0.client_pubkey.len    = SEC2_PUBLIC_KEY_LEN;

    Sec2Payload payload = SEC2_PAYLOAD__INIT;
    payload.msg          = SEC2_MSG_TYPE__S2Session_Command0;
    payload.payload_case = SEC2_PAYLOAD__PAYLOAD_SC0;
    payload.sc0          = &cmd0;

    SessionData req = SESSION_DATA__INIT;
    req.sec_ver    = (SecSchemeVersion)protocomm_security2.ver;
    req.proto_case = SESSION_DATA__PROTO_SEC2;
    req.sec2       = &payload;

    size_t packed_len = session_data__get_packed_size(&req);
    uint8_t *packed = malloc(packed_len);
    if (!packed) {
        ESP_LOGE(TAG, "Cannot allocate %zu-byte packed buffer — skipping test", packed_len);
        free(username_buf);
        protocomm_delete(pc);
        return ESP_ERR_NO_MEM;
    }
    session_data__pack(&req, packed);
    free(username_buf);

    uint8_t *outbuf = NULL;
    ssize_t  outlen = 0;
    ret = protocomm_req_handle(pc, "test-sec", session_id,
                               packed, (ssize_t)packed_len,
                               &outbuf, &outlen);
    free(packed);
    free(outbuf);
    protocomm_delete(pc);

    if (ret == ESP_OK) {
        ESP_LOGE(TAG, "Oversized username was accepted — expected rejection");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Oversized username correctly rejected (ret=0x%x)", ret);
    return ESP_OK;
}

TEST_CASE("security2 cmd0 oversized username uint16_t truncation heap overflow", "[PROTOCOMM_SEC2]")
{
    TEST_ASSERT_EQUAL(ESP_OK, test_cmd0_oversized_username());
}

static esp_err_t test_cmd0_missing_sc0_payload(void)
{
    esp_err_t ret;

    protocomm_t *pc = protocomm_new();
    if (!pc) {
        return ESP_ERR_NO_MEM;
    }

    protocomm_security2_params_t sv = {
        .salt         = sec2_salt,
        .salt_len     = sizeof(sec2_salt),
        .verifier     = sec2_verifier,
        .verifier_len = sizeof(sec2_verifier),
    };

    ret = protocomm_set_security(pc, "test-sec", &protocomm_security2, &sv);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "protocomm_set_security failed (0x%x)", ret);
        protocomm_delete(pc);
        return ret;
    }

    const uint32_t session_id = 101;
    ret = protocomm_open_session(pc, session_id);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "protocomm_open_session failed (0x%x)", ret);
        protocomm_delete(pc);
        return ret;
    }

    Sec2Payload payload = SEC2_PAYLOAD__INIT;
    payload.msg = SEC2_MSG_TYPE__S2Session_Command0;
    /* payload.payload_case deliberately left as SEC2_PAYLOAD__PAYLOAD__NOT_SET */
    /* payload.sc0 deliberately left as NULL                                     */

    SessionData req = SESSION_DATA__INIT;
    req.sec_ver    = (SecSchemeVersion)protocomm_security2.ver;
    req.proto_case = SESSION_DATA__PROTO_SEC2;
    req.sec2       = &payload;

    size_t packed_len = session_data__get_packed_size(&req);
    uint8_t *packed = malloc(packed_len);
    if (!packed) {
        ESP_LOGE(TAG, "Cannot allocate packed buffer");
        protocomm_delete(pc);
        return ESP_ERR_NO_MEM;
    }
    session_data__pack(&req, packed);

    uint8_t *outbuf = NULL;
    ssize_t  outlen = 0;
    ret = protocomm_req_handle(pc, "test-sec", session_id,
                               packed, (ssize_t)packed_len,
                               &outbuf, &outlen);
    free(packed);
    free(outbuf);
    protocomm_delete(pc);

    if (ret == ESP_OK) {
        ESP_LOGE(TAG, "Missing sc0 payload was accepted — expected rejection");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Missing sc0 payload correctly rejected (ret=0x%x)", ret);
    return ESP_OK;
}

TEST_CASE("security2 cmd0 missing sc0 payload NULL dereference", "[PROTOCOMM_SEC2]")
{
    TEST_ASSERT_EQUAL(ESP_OK, test_cmd0_missing_sc0_payload());
}

static esp_err_t test_cmd0_msg_with_sc1_payload(void)
{
    esp_err_t ret;

    protocomm_t *pc = protocomm_new();
    if (!pc) {
        return ESP_ERR_NO_MEM;
    }

    protocomm_security2_params_t sv = {
        .salt         = sec2_salt,
        .salt_len     = sizeof(sec2_salt),
        .verifier     = sec2_verifier,
        .verifier_len = sizeof(sec2_verifier),
    };

    ret = protocomm_set_security(pc, "test-sec", &protocomm_security2, &sv);
    if (ret != ESP_OK) {
        protocomm_delete(pc);
        return ret;
    }

    const uint32_t session_id = 102;
    ret = protocomm_open_session(pc, session_id);
    if (ret != ESP_OK) {
        protocomm_delete(pc);
        return ret;
    }

    /* Build a Sec2Payload with msg=CMD0 but sc1 (not sc0) in the oneof. */
    uint8_t proof[64];
    memset(proof, 0x33, sizeof(proof));

    S2SessionCmd1 cmd1 = S2_SESSION_CMD1__INIT;
    cmd1.client_proof.data = proof;
    cmd1.client_proof.len  = sizeof(proof);

    Sec2Payload payload = SEC2_PAYLOAD__INIT;
    payload.msg          = SEC2_MSG_TYPE__S2Session_Command0; /* CMD0 msg */
    payload.payload_case = SEC2_PAYLOAD__PAYLOAD_SC1;         /* SC1 payload — mismatch */
    payload.sc1          = &cmd1;

    SessionData req = SESSION_DATA__INIT;
    req.sec_ver    = (SecSchemeVersion)protocomm_security2.ver;
    req.proto_case = SESSION_DATA__PROTO_SEC2;
    req.sec2       = &payload;

    size_t packed_len = session_data__get_packed_size(&req);
    uint8_t *packed = malloc(packed_len);
    if (!packed) {
        protocomm_delete(pc);
        return ESP_ERR_NO_MEM;
    }
    session_data__pack(&req, packed);

    uint8_t *outbuf = NULL;
    ssize_t  outlen = 0;
    ret = protocomm_req_handle(pc, "test-sec", session_id,
                               packed, (ssize_t)packed_len,
                               &outbuf, &outlen);
    free(packed);
    free(outbuf);
    protocomm_delete(pc);

    if (ret == ESP_OK) {
        ESP_LOGE(TAG, "msg=CMD0/sc1 payload mismatch was accepted — expected rejection");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "msg=CMD0/sc1 payload mismatch correctly rejected (ret=0x%x)", ret);
    return ESP_OK;
}

TEST_CASE("security2 cmd0 msg with sc1 payload oneof type confusion heap overflow", "[PROTOCOMM_SEC2]")
{
    TEST_ASSERT_EQUAL(ESP_OK, test_cmd0_msg_with_sc1_payload());
}

static esp_err_t test_cmd1_msg_with_sc0_payload(void)
{
    esp_err_t ret;

    protocomm_t *pc = protocomm_new();
    if (!pc) {
        return ESP_ERR_NO_MEM;
    }

    protocomm_security2_params_t sv = {
        .salt         = sec2_salt,
        .salt_len     = sizeof(sec2_salt),
        .verifier     = sec2_verifier,
        .verifier_len = sizeof(sec2_verifier),
    };

    ret = protocomm_set_security(pc, "test-sec", &protocomm_security2, &sv);
    if (ret != ESP_OK) {
        protocomm_delete(pc);
        return ret;
    }

    const uint32_t session_id = 103;
    ret = protocomm_open_session(pc, session_id);
    if (ret != ESP_OK) {
        protocomm_delete(pc);
        return ret;
    }

    /* Build a Sec2Payload with msg=CMD1 but sc0 (not sc1) in the oneof. */
    uint8_t pubkey[SEC2_PUBLIC_KEY_LEN];
    memset(pubkey, 0xAB, sizeof(pubkey));
    uint8_t uname[] = "alice";

    S2SessionCmd0 cmd0 = S2_SESSION_CMD0__INIT;
    cmd0.client_username.data = uname;
    cmd0.client_username.len  = sizeof(uname) - 1;
    cmd0.client_pubkey.data   = pubkey;
    cmd0.client_pubkey.len    = sizeof(pubkey);

    Sec2Payload payload = SEC2_PAYLOAD__INIT;
    payload.msg          = SEC2_MSG_TYPE__S2Session_Command1; /* CMD1 msg */
    payload.payload_case = SEC2_PAYLOAD__PAYLOAD_SC0;         /* SC0 payload — mismatch */
    payload.sc0          = &cmd0;

    SessionData req = SESSION_DATA__INIT;
    req.sec_ver    = (SecSchemeVersion)protocomm_security2.ver;
    req.proto_case = SESSION_DATA__PROTO_SEC2;
    req.sec2       = &payload;

    size_t packed_len = session_data__get_packed_size(&req);
    uint8_t *packed = malloc(packed_len);
    if (!packed) {
        protocomm_delete(pc);
        return ESP_ERR_NO_MEM;
    }
    session_data__pack(&req, packed);

    uint8_t *outbuf = NULL;
    ssize_t  outlen = 0;
    ret = protocomm_req_handle(pc, "test-sec", session_id,
                               packed, (ssize_t)packed_len,
                               &outbuf, &outlen);
    free(packed);
    free(outbuf);
    protocomm_delete(pc);

    if (ret == ESP_OK) {
        ESP_LOGE(TAG, "msg=CMD1/sc0 payload mismatch was accepted — expected rejection");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "msg=CMD1/sc0 payload mismatch correctly rejected (ret=0x%x)", ret);
    return ESP_OK;
}

TEST_CASE("security2 cmd1 msg with sc0 payload oneof type confusion", "[PROTOCOMM_SEC2]")
{
    TEST_ASSERT_EQUAL(ESP_OK, test_cmd1_msg_with_sc0_payload());
}

static esp_err_t test_sec1_payload_with_sec2_version(void)
{
    esp_err_t ret;

    protocomm_t *pc = protocomm_new();
    if (!pc) {
        return ESP_ERR_NO_MEM;
    }

    protocomm_security2_params_t sv = {
        .salt         = sec2_salt,
        .salt_len     = sizeof(sec2_salt),
        .verifier     = sec2_verifier,
        .verifier_len = sizeof(sec2_verifier),
    };

    ret = protocomm_set_security(pc, "test-sec", &protocomm_security2, &sv);
    if (ret != ESP_OK) {
        protocomm_delete(pc);
        return ret;
    }

    const uint32_t session_id = 104;
    ret = protocomm_open_session(pc, session_id);
    if (ret != ESP_OK) {
        protocomm_delete(pc);
        return ret;
    }

    uint8_t pubkey[32];
    memset(pubkey, 0xAB, sizeof(pubkey));

    SessionCmd0 sec1_cmd0 = SESSION_CMD0__INIT;
    sec1_cmd0.client_pubkey.data = pubkey;
    sec1_cmd0.client_pubkey.len  = sizeof(pubkey);

    Sec1Payload sec1_payload = SEC1_PAYLOAD__INIT;
    sec1_payload.msg          = SEC1_MSG_TYPE__Session_Command0;
    sec1_payload.payload_case = SEC1_PAYLOAD__PAYLOAD_SC0;
    sec1_payload.sc0          = &sec1_cmd0;

    SessionData req = SESSION_DATA__INIT;
    req.sec_ver    = SEC_SCHEME_VERSION__SecScheme2; /* sec_ver=2 — passes ver check */
    req.proto_case = SESSION_DATA__PROTO_SEC1;       /* but SEC1 proto — mismatch   */
    req.sec1       = &sec1_payload;

    size_t packed_len = session_data__get_packed_size(&req);
    uint8_t *packed = malloc(packed_len);
    if (!packed) {
        protocomm_delete(pc);
        return ESP_ERR_NO_MEM;
    }
    session_data__pack(&req, packed);

    uint8_t *outbuf = NULL;
    ssize_t  outlen = 0;
    ret = protocomm_req_handle(pc, "test-sec", session_id,
                               packed, (ssize_t)packed_len,
                               &outbuf, &outlen);
    free(packed);
    free(outbuf);
    protocomm_delete(pc);

    if (ret == ESP_OK) {
        ESP_LOGE(TAG, "sec1 payload with sec_ver=2 was accepted — expected rejection");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "sec1 payload with sec_ver=2 correctly rejected (ret=0x%x)", ret);
    return ESP_OK;
}

TEST_CASE("security2 sec1 payload with sec_ver=2 cross-scheme type confusion heap overflow", "[PROTOCOMM_SEC2]")
{
    TEST_ASSERT_EQUAL(ESP_OK, test_sec1_payload_with_sec2_version());
}

#endif /* CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_2 */
