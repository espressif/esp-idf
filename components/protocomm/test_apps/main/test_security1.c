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
#include <protocomm_security1.h>
#include "session.pb-c.h"
#include "sec1.pb-c.h"
#include "sec2.pb-c.h"

#if CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_1

static const char *TAG = "test_security1";

/* Must match PUBLIC_KEY_LEN in security1.c */
#define SEC1_PUBLIC_KEY_LEN  32

static esp_err_t test_cmd0_missing_sc0_payload(void)
{
    esp_err_t ret;

    protocomm_t *pc = protocomm_new();
    if (!pc) {
        return ESP_ERR_NO_MEM;
    }

    ret = protocomm_set_security(pc, "test-sec", &protocomm_security1, NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "protocomm_set_security failed (0x%x)", ret);
        protocomm_delete(pc);
        return ret;
    }

    const uint32_t session_id = 200;
    ret = protocomm_open_session(pc, session_id);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "protocomm_open_session failed (0x%x)", ret);
        protocomm_delete(pc);
        return ret;
    }

    /* msg=Command0 but payload_case left as NOT_SET and sc0 left as NULL */
    Sec1Payload payload = SEC1_PAYLOAD__INIT;
    payload.msg = SEC1_MSG_TYPE__Session_Command0;
    /* payload.payload_case deliberately left as SEC1_PAYLOAD__PAYLOAD__NOT_SET */
    /* payload.sc0 deliberately left as NULL                                     */

    SessionData req = SESSION_DATA__INIT;
    req.sec_ver    = (SecSchemeVersion)protocomm_security1.ver;
    req.proto_case = SESSION_DATA__PROTO_SEC1;
    req.sec1       = &payload;

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
        ESP_LOGE(TAG, "Missing sc0 payload was accepted — expected rejection");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Missing sc0 payload correctly rejected (ret=0x%x)", ret);
    return ESP_OK;
}

TEST_CASE("security1 cmd0 missing sc0 payload NULL dereference", "[PROTOCOMM_SEC1]")
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

    ret = protocomm_set_security(pc, "test-sec", &protocomm_security1, NULL);
    if (ret != ESP_OK) {
        protocomm_delete(pc);
        return ret;
    }

    const uint32_t session_id = 201;
    ret = protocomm_open_session(pc, session_id);
    if (ret != ESP_OK) {
        protocomm_delete(pc);
        return ret;
    }

    /* msg=Command0 but sc1 (not sc0) populated in the oneof */
    uint8_t verify_data[SEC1_PUBLIC_KEY_LEN];
    memset(verify_data, 0x55, sizeof(verify_data));

    SessionCmd1 cmd1 = SESSION_CMD1__INIT;
    cmd1.client_verify_data.data = verify_data;
    cmd1.client_verify_data.len  = sizeof(verify_data);

    Sec1Payload payload = SEC1_PAYLOAD__INIT;
    payload.msg          = SEC1_MSG_TYPE__Session_Command0; /* CMD0 msg */
    payload.payload_case = SEC1_PAYLOAD__PAYLOAD_SC1;       /* SC1 payload — mismatch */
    payload.sc1          = &cmd1;

    SessionData req = SESSION_DATA__INIT;
    req.sec_ver    = (SecSchemeVersion)protocomm_security1.ver;
    req.proto_case = SESSION_DATA__PROTO_SEC1;
    req.sec1       = &payload;

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

TEST_CASE("security1 cmd0 msg with sc1 payload oneof type confusion", "[PROTOCOMM_SEC1]")
{
    TEST_ASSERT_EQUAL(ESP_OK, test_cmd0_msg_with_sc1_payload());
}

static esp_err_t test_cmd1_missing_sc1_payload(void)
{
    esp_err_t ret;

    protocomm_t *pc = protocomm_new();
    if (!pc) {
        return ESP_ERR_NO_MEM;
    }

    ret = protocomm_set_security(pc, "test-sec", &protocomm_security1, NULL);
    if (ret != ESP_OK) {
        protocomm_delete(pc);
        return ret;
    }

    const uint32_t session_id = 202;
    ret = protocomm_open_session(pc, session_id);
    if (ret != ESP_OK) {
        protocomm_delete(pc);
        return ret;
    }

    /* msg=Command1 but payload_case left as NOT_SET and sc1 left as NULL */
    Sec1Payload payload = SEC1_PAYLOAD__INIT;
    payload.msg = SEC1_MSG_TYPE__Session_Command1;
    /* payload.payload_case deliberately left as SEC1_PAYLOAD__PAYLOAD__NOT_SET */
    /* payload.sc1 deliberately left as NULL                                     */

    SessionData req = SESSION_DATA__INIT;
    req.sec_ver    = (SecSchemeVersion)protocomm_security1.ver;
    req.proto_case = SESSION_DATA__PROTO_SEC1;
    req.sec1       = &payload;

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
        ESP_LOGE(TAG, "Missing sc1 payload was accepted — expected rejection");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Missing sc1 payload correctly rejected (ret=0x%x)", ret);
    return ESP_OK;
}

TEST_CASE("security1 cmd1 missing sc1 payload NULL dereference", "[PROTOCOMM_SEC1]")
{
    TEST_ASSERT_EQUAL(ESP_OK, test_cmd1_missing_sc1_payload());
}

static esp_err_t test_cmd1_msg_with_sc0_payload(void)
{
    esp_err_t ret;

    protocomm_t *pc = protocomm_new();
    if (!pc) {
        return ESP_ERR_NO_MEM;
    }

    ret = protocomm_set_security(pc, "test-sec", &protocomm_security1, NULL);
    if (ret != ESP_OK) {
        protocomm_delete(pc);
        return ret;
    }

    const uint32_t session_id = 203;
    ret = protocomm_open_session(pc, session_id);
    if (ret != ESP_OK) {
        protocomm_delete(pc);
        return ret;
    }

    /* msg=Command1 but sc0 (not sc1) populated in the oneof */
    uint8_t pubkey[SEC1_PUBLIC_KEY_LEN];
    memset(pubkey, 0xAB, sizeof(pubkey));

    SessionCmd0 cmd0 = SESSION_CMD0__INIT;
    cmd0.client_pubkey.data = pubkey;
    cmd0.client_pubkey.len  = sizeof(pubkey);

    Sec1Payload payload = SEC1_PAYLOAD__INIT;
    payload.msg          = SEC1_MSG_TYPE__Session_Command1; /* CMD1 msg */
    payload.payload_case = SEC1_PAYLOAD__PAYLOAD_SC0;       /* SC0 payload — mismatch */
    payload.sc0          = &cmd0;

    SessionData req = SESSION_DATA__INIT;
    req.sec_ver    = (SecSchemeVersion)protocomm_security1.ver;
    req.proto_case = SESSION_DATA__PROTO_SEC1;
    req.sec1       = &payload;

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

TEST_CASE("security1 cmd1 msg with sc0 payload oneof type confusion", "[PROTOCOMM_SEC1]")
{
    TEST_ASSERT_EQUAL(ESP_OK, test_cmd1_msg_with_sc0_payload());
}

static esp_err_t test_sec2_payload_with_sec1_version(void)
{
    esp_err_t ret;

    protocomm_t *pc = protocomm_new();
    if (!pc) {
        return ESP_ERR_NO_MEM;
    }

    ret = protocomm_set_security(pc, "test-sec", &protocomm_security1, NULL);
    if (ret != ESP_OK) {
        protocomm_delete(pc);
        return ret;
    }

    const uint32_t session_id = 204;
    ret = protocomm_open_session(pc, session_id);
    if (ret != ESP_OK) {
        protocomm_delete(pc);
        return ret;
    }

    /* SessionData with sec_ver=1 but proto_case=SEC2, carrying a Sec2 CMD0.
     * Sec2's S2SessionCmd0 (56 bytes) is larger than Sec1's SessionCmd0
     * (40 bytes), so sec1 code reading the union as Sec1Payload causes
     * type confusion. */
    uint8_t pubkey[384];
    memset(pubkey, 0xAB, sizeof(pubkey));
    uint8_t uname[] = "wifiprov";

    S2SessionCmd0 s2cmd0 = S2_SESSION_CMD0__INIT;
    s2cmd0.client_pubkey.data   = pubkey;
    s2cmd0.client_pubkey.len    = sizeof(pubkey);
    s2cmd0.client_username.data = uname;
    s2cmd0.client_username.len  = sizeof(uname) - 1;

    Sec2Payload sec2_payload = SEC2_PAYLOAD__INIT;
    sec2_payload.msg          = SEC2_MSG_TYPE__S2Session_Command0;
    sec2_payload.payload_case = SEC2_PAYLOAD__PAYLOAD_SC0;
    sec2_payload.sc0          = &s2cmd0;

    SessionData req = SESSION_DATA__INIT;
    req.sec_ver    = SEC_SCHEME_VERSION__SecScheme1; /* sec_ver=1 — passes ver check */
    req.proto_case = SESSION_DATA__PROTO_SEC2;       /* but SEC2 proto — mismatch   */
    req.sec2       = &sec2_payload;

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
        ESP_LOGE(TAG, "sec2 payload with sec_ver=1 was accepted — expected rejection");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "sec2 payload with sec_ver=1 correctly rejected (ret=0x%x)", ret);
    return ESP_OK;
}

TEST_CASE("security1 sec2 payload with sec_ver=1 cross-scheme type confusion", "[PROTOCOMM_SEC1]")
{
    TEST_ASSERT_EQUAL(ESP_OK, test_sec2_payload_with_sec1_version());
}

#endif /* CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_1 */
