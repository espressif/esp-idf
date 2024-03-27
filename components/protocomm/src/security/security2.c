/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_check.h>
#include <inttypes.h>

#include <mbedtls/gcm.h>
#include <mbedtls/error.h>
#include <esp_random.h>

#include <protocomm_security.h>
#include <protocomm_security2.h>

#include "session.pb-c.h"
#include "sec2.pb-c.h"
#include "constants.pb-c.h"

#include "esp_srp.h"

static const char *TAG = "security2";

ESP_EVENT_DEFINE_BASE(PROTOCOMM_SECURITY_SESSION_EVENT);

#define SALT_LEN                    (16)
#define PUBLIC_KEY_LEN              (384)
#define CLIENT_PROOF_LEN            (64)
#define AES_GCM_KEY_LEN             (256)
#define AES_GCM_IV_SIZE             (16)
#define AES_GCM_TAG_LEN             (16)

#define SESSION_STATE_CMD0  0 /* Session is not setup: Initial State*/
#define SESSION_STATE_CMD1  1 /* Session is not setup: Cmd0 done */
#define SESSION_STATE_DONE  2 /* Session setup successful */

typedef struct session {
    /* Session data */
    uint32_t id;
    uint8_t state;
    /* Currently fixing the salt length to 16, we may keep it flexible */
    char *username;
    uint16_t username_len;
    char *salt;
    uint16_t salt_len;
    char *verifier;
    uint16_t verifier_len;
    char *session_key;
    uint16_t session_key_len;
    uint8_t iv[AES_GCM_IV_SIZE];
    /* mbedtls context data for AES-GCM */
    mbedtls_gcm_context ctx_gcm;
    esp_srp_handle_t *srp_hd;
} session_t;

static void hexdump(const char *msg, char *buf, int len)
{
    ESP_LOGD(TAG, "%s ->", msg);
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, buf, len, ESP_LOG_DEBUG);
}

static esp_err_t sec2_new_session(protocomm_security_handle_t handle, uint32_t session_id);

static esp_err_t handle_session_command0(session_t *cur_session,
        uint32_t session_id,
        SessionData *req, SessionData *resp,
        const protocomm_security2_params_t *sv)
{
    ESP_LOGD(TAG, "Request to handle setup0_command");
    Sec2Payload *in = (Sec2Payload *) req->sec2;

    if (cur_session->state != SESSION_STATE_CMD0) {
        ESP_LOGW(TAG, "Invalid state of session %d (expected %d). Restarting session.",
                 SESSION_STATE_CMD0, cur_session->state);
        sec2_new_session(cur_session, session_id);
    }

    if (in->sc0->client_pubkey.len != PUBLIC_KEY_LEN) {
        ESP_LOGE(TAG, "Invalid public key length");
        if (esp_event_post(PROTOCOMM_SECURITY_SESSION_EVENT, PROTOCOMM_SECURITY_SESSION_INVALID_SECURITY_PARAMS, NULL, 0, portMAX_DELAY) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to post secure session invalid security params event");
        }
        return ESP_ERR_INVALID_ARG;
    }

    if (in->sc0->client_username.len <= 0) {
        ESP_LOGE(TAG, "Invalid username");
        if (esp_event_post(PROTOCOMM_SECURITY_SESSION_EVENT, PROTOCOMM_SECURITY_SESSION_INVALID_SECURITY_PARAMS, NULL, 0, portMAX_DELAY) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to post secure session invalid security params event");
        }
        return ESP_ERR_INVALID_ARG;
    }

    if (sv == NULL) {
        ESP_LOGE(TAG, "Invalid security params");
        return ESP_ERR_INVALID_ARG;
    }


    ESP_LOGD(TAG, "Username: %.*s", in->sc0->client_username.len, in->sc0->client_username.data);


    hexdump("Client Public Key", (char *) in->sc0->client_pubkey.data, PUBLIC_KEY_LEN);

    /* Initialize mu srp context */
    cur_session->srp_hd = esp_srp_init(ESP_NG_3072);
    if (cur_session->srp_hd == NULL) {
        ESP_LOGE(TAG, "Failed to initialise security context!");
        return ESP_FAIL;
    }

    char *device_pubkey = NULL;
    int device_pubkey_len = 0;

    cur_session->salt = (char *)sv->salt;
    cur_session->salt_len = sv->salt_len;
    cur_session->verifier = (char *)sv->verifier;
    cur_session->verifier_len = sv->verifier_len;
    ESP_LOGI(TAG, "Using salt and verifier to generate public key...");

    if (sv->salt != NULL && sv->salt_len != 0 && sv->verifier != NULL && sv->verifier_len != 0) {
        if (esp_srp_set_salt_verifier(cur_session->srp_hd, cur_session->salt,
                cur_session->salt_len, cur_session->verifier, cur_session->verifier_len) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set salt and verifier!");
            esp_srp_free(cur_session->srp_hd);
            return ESP_FAIL;
        }
        if (esp_srp_srv_pubkey_from_salt_verifier(cur_session->srp_hd, &device_pubkey, &device_pubkey_len) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to device public key!");
            esp_srp_free(cur_session->srp_hd);
            return ESP_FAIL;
        }
    }

    hexdump("Device Public Key", device_pubkey, device_pubkey_len);
    if (esp_srp_get_session_key(cur_session->srp_hd, (char *) in->sc0->client_pubkey.data, PUBLIC_KEY_LEN,
                                &cur_session->session_key, &cur_session->session_key_len) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to generate device session key!");
        esp_srp_free(cur_session->srp_hd);
        return ESP_FAIL;
    }
    hexdump("Session Key", cur_session->session_key, cur_session->session_key_len);

    Sec2Payload *out = (Sec2Payload *) malloc(sizeof(Sec2Payload));
    S2SessionResp0 *out_resp = (S2SessionResp0 *) malloc(sizeof(S2SessionResp0));
    if (!out || !out_resp) {
        ESP_LOGE(TAG, "Error allocating memory for response0");
        esp_srp_free(cur_session->srp_hd);
        free(out);
        free(out_resp);
        return ESP_ERR_NO_MEM;
    }

    sec2_payload__init(out);
    s2_session_resp0__init(out_resp);

    out_resp->status = STATUS__Success;

    out_resp->device_pubkey.data = (uint8_t *)device_pubkey;
    out_resp->device_pubkey.len = device_pubkey_len;

    out_resp->device_salt.data = (uint8_t *)cur_session->salt;
    out_resp->device_salt.len = cur_session->salt_len;

    out->msg = SEC2_MSG_TYPE__S2Session_Response0;
    out->payload_case = SEC2_PAYLOAD__PAYLOAD_SR0;
    out->sr0 = out_resp;

    resp->sec_ver = SEC_SCHEME_VERSION__SecScheme2;
    resp->proto_case = SESSION_DATA__PROTO_SEC2;
    resp->sec2 = out;

    cur_session->username_len = in->sc0->client_username.len;
    cur_session->username = malloc(cur_session->username_len);
    if (!cur_session->username) {
        ESP_LOGE(TAG, "Failed to allocate memory!");
        esp_srp_free(cur_session->srp_hd);
        return ESP_ERR_NO_MEM;
    }
    memcpy(cur_session->username, in->sc0->client_username.data, in->sc0->client_username.len);

    cur_session->state = SESSION_STATE_CMD1;

    ESP_LOGD(TAG, "Session setup phase1 done");
    return ESP_OK;
}

static esp_err_t handle_session_command1(session_t *cur_session,
        uint32_t session_id,
        SessionData *req, SessionData *resp)
{
    ESP_LOGD(TAG, "Request to handle setup1_command");
    Sec2Payload *in = (Sec2Payload *) req->sec2;
    int mbed_err = -0x0001;

    if (cur_session->state != SESSION_STATE_CMD1) {
        ESP_LOGE(TAG, "Invalid state of session %d (expected %d)", SESSION_STATE_CMD1, cur_session->state);
        return ESP_ERR_INVALID_STATE;
    }

    ESP_RETURN_ON_FALSE(in->sc1->client_proof.len == CLIENT_PROOF_LEN, ESP_FAIL, TAG, "The client proof length does not match");

    hexdump("Client proof", (char * ) in->sc1->client_proof.data, in->sc1->client_proof.len);

    char *device_proof = calloc(CLIENT_PROOF_LEN, sizeof(char));
    if (!device_proof) {
        ESP_LOGE(TAG, "Failed to allocate memory!");
        return ESP_ERR_NO_MEM;
    }

    if (esp_srp_exchange_proofs(cur_session->srp_hd, cur_session->username, cur_session->username_len, (char * ) in->sc1->client_proof.data, device_proof) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to authenticate client proof!");
        free(device_proof);
        if (esp_event_post(PROTOCOMM_SECURITY_SESSION_EVENT, PROTOCOMM_SECURITY_SESSION_CREDENTIALS_MISMATCH, NULL, 0, portMAX_DELAY) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to post credential mismatch event");
        }
        return ESP_FAIL;
    }
    hexdump("Device proof", device_proof, CLIENT_PROOF_LEN);

    /* Initialize crypto context */
    mbedtls_gcm_init(&cur_session->ctx_gcm);

    /* Considering the protocomm component is only used after RF ( Wifi/Bluetooth ) is enabled.
     * Hence, we can be sure that the RNG generates true random numbers */
    esp_fill_random(&cur_session->iv, AES_GCM_IV_SIZE);

    hexdump("Initialization vector", (char *)cur_session->iv, AES_GCM_IV_SIZE);

    mbed_err = mbedtls_gcm_setkey(&cur_session->ctx_gcm, MBEDTLS_CIPHER_ID_AES, (unsigned char *)cur_session->session_key, AES_GCM_KEY_LEN);
    if (mbed_err != 0) {
        ESP_LOGE(TAG, "Failure at mbedtls_gcm_setkey_enc with error code : -0x%x", -mbed_err);
        free(device_proof);
        mbedtls_gcm_free(&cur_session->ctx_gcm);
        return ESP_FAIL;
    }

    Sec2Payload *out = (Sec2Payload *) malloc(sizeof(Sec2Payload));
    S2SessionResp1 *out_resp = (S2SessionResp1 *) malloc(sizeof(S2SessionResp1));
    if (!out || !out_resp) {
        ESP_LOGE(TAG, "Error allocating memory for response1");
        free(device_proof);
        free(out);
        free(out_resp);
        mbedtls_gcm_free(&cur_session->ctx_gcm);
        return ESP_ERR_NO_MEM;
    }

    sec2_payload__init(out);
    s2_session_resp1__init(out_resp);
    out_resp->status = STATUS__Success;

    out_resp->device_proof.data = (uint8_t *)device_proof;
    out_resp->device_proof.len = CLIENT_PROOF_LEN;

    out_resp->device_nonce.data = cur_session->iv;
    out_resp->device_nonce.len = AES_GCM_IV_SIZE;

    out->msg = SEC2_MSG_TYPE__S2Session_Response1;
    out->payload_case = SEC2_PAYLOAD__PAYLOAD_SR1;
    out->sr1 = out_resp;

    resp->sec_ver = SEC_SCHEME_VERSION__SecScheme2;
    resp->proto_case = SESSION_DATA__PROTO_SEC2;
    resp->sec2 = out;

    cur_session->state = SESSION_STATE_DONE;
    if (esp_event_post(PROTOCOMM_SECURITY_SESSION_EVENT, PROTOCOMM_SECURITY_SESSION_SETUP_OK, NULL, 0, portMAX_DELAY) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to post secure session setup success event");
    }
    ESP_LOGD(TAG, "Secure session established successfully");
    return ESP_OK;
}

static esp_err_t sec2_session_setup(session_t *cur_session,
                                    uint32_t session_id,
                                    SessionData *req, SessionData *resp,
                                    const protocomm_security2_params_t *sv)
{
    Sec2Payload *in = (Sec2Payload *) req->sec2;
    esp_err_t ret;

    if (!in) {
        ESP_LOGE(TAG, "Empty session data");
        return ESP_ERR_INVALID_ARG;
    }

    switch (in->msg) {
    case SEC2_MSG_TYPE__S2Session_Command0:
        ret = handle_session_command0(cur_session, session_id, req, resp, sv);
        break;
    case SEC2_MSG_TYPE__S2Session_Command1:
        ret = handle_session_command1(cur_session, session_id, req, resp);
        break;
    default:
        ESP_LOGE(TAG, "Invalid security message type");
        ret = ESP_ERR_INVALID_ARG;
    }

    return ret;

}

static void sec2_session_setup_cleanup(session_t *cur_session, uint32_t session_id, SessionData *resp)
{
    Sec2Payload *out = resp->sec2;

    if (!out) {
        return;
    }

    switch (out->msg) {
    case SEC2_MSG_TYPE__S2Session_Response0: {
        S2SessionResp0 *out_resp0 = out->sr0;
        if (out_resp0) {
            free(out_resp0);
        }
        break;
    }
    case SEC2_MSG_TYPE__S2Session_Response1: {
        S2SessionResp1 *out_resp1 = out->sr1;
        if (out_resp1) {
            free(out_resp1->device_proof.data);
            free(out_resp1);
        }
        break;
    }
    default:
        break;
    }
    free(out);

    return;
}

static esp_err_t sec2_close_session(protocomm_security_handle_t handle, uint32_t session_id)
{
    session_t *cur_session = (session_t *) handle;
    if (!cur_session) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!cur_session || cur_session->id != session_id) {
        ESP_LOGE(TAG, "Attempt to close invalid session");
        return ESP_ERR_INVALID_STATE;
    }

    if (cur_session->state == SESSION_STATE_DONE) {
        /* Free GCM context data */
        mbedtls_gcm_free(&cur_session->ctx_gcm);
    }

    free(cur_session->username);

    if (cur_session->srp_hd) {
        esp_srp_free(cur_session->srp_hd);
    }

    memset(cur_session, 0, sizeof(session_t));
    cur_session->id = -1;
    return ESP_OK;
}

static esp_err_t sec2_new_session(protocomm_security_handle_t handle, uint32_t session_id)
{
    session_t *cur_session = (session_t *) handle;
    if (!cur_session) {
        return ESP_ERR_INVALID_ARG;
    }

    if (cur_session->id != -1) {
        /* Only one session is allowed at a time */
        ESP_LOGE(TAG, "Closing old session with id %" PRIu32, cur_session->id);
        sec2_close_session(cur_session, session_id);
    }

    cur_session->id = session_id;
    return ESP_OK;
}

static esp_err_t sec2_init(protocomm_security_handle_t *handle)
{
    if (!handle) {
        return ESP_ERR_INVALID_ARG;
    }
    session_t *cur_session = (session_t *) calloc(1, sizeof(session_t));
    if (!cur_session) {
        ESP_LOGE(TAG, "Error allocating new session");
        return ESP_ERR_NO_MEM;
    }
    cur_session->id = -1;
    *handle = (protocomm_security_handle_t) cur_session;
    return ESP_OK;
}

static esp_err_t sec2_cleanup(protocomm_security_handle_t handle)
{
    session_t *cur_session = (session_t *) handle;
    if (cur_session) {
        sec2_close_session(handle, cur_session->id);
    }
    free(handle);
    return ESP_OK;
}

static esp_err_t sec2_encrypt(protocomm_security_handle_t handle,
                              uint32_t session_id,
                              const uint8_t *inbuf, ssize_t inlen,
                              uint8_t **outbuf, ssize_t *outlen)
{
    session_t *cur_session = (session_t *) handle;
    if (!cur_session) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!cur_session || cur_session->id != session_id) {
        ESP_LOGE(TAG, "Session with ID %" PRId32 "not found", session_id);
        return ESP_ERR_INVALID_STATE;
    }

    if (cur_session->state != SESSION_STATE_DONE) {
        ESP_LOGE(TAG, "Secure session not established");
        return ESP_ERR_INVALID_STATE;
    }

    *outlen = inlen + AES_GCM_TAG_LEN;
    *outbuf = (uint8_t *) malloc(*outlen);
    if (!*outbuf) {
        ESP_LOGE(TAG, "Failed to allocate encrypt buf len %d", *outlen);
        return ESP_ERR_NO_MEM;
    }
    uint8_t gcm_tag[AES_GCM_TAG_LEN];

    int ret = mbedtls_gcm_crypt_and_tag(&cur_session->ctx_gcm, MBEDTLS_GCM_ENCRYPT, inlen, cur_session->iv,
                                        AES_GCM_IV_SIZE, NULL, 0, inbuf,
                                        *outbuf, AES_GCM_TAG_LEN, gcm_tag);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed at mbedtls_gcm_crypt_and_tag with error code : %d", ret);
        return ESP_FAIL;
    }
    memcpy(*outbuf + inlen, gcm_tag, AES_GCM_TAG_LEN);

    return ESP_OK;
}

static esp_err_t sec2_decrypt(protocomm_security_handle_t handle,
                              uint32_t session_id,
                              const uint8_t *inbuf, ssize_t inlen,
                              uint8_t **outbuf, ssize_t *outlen)
{
    session_t *cur_session = (session_t *) handle;
    if (!cur_session) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!cur_session || cur_session->id != session_id) {
        ESP_LOGE(TAG, "Session with ID %" PRId32 "not found", session_id);
        return ESP_ERR_INVALID_STATE;
    }

    if (cur_session->state != SESSION_STATE_DONE) {
        ESP_LOGE(TAG, "Secure session not established");
        return ESP_ERR_INVALID_STATE;
    }

    *outlen = inlen - AES_GCM_TAG_LEN;
    *outbuf = (uint8_t *) malloc(*outlen);
    if (!*outbuf) {
        ESP_LOGE(TAG, "Failed to allocate decrypt buf len %d", *outlen);
        return ESP_ERR_NO_MEM;
    }

    int ret = mbedtls_gcm_auth_decrypt(&cur_session->ctx_gcm, inlen - AES_GCM_TAG_LEN, cur_session->iv,
                                       AES_GCM_IV_SIZE, NULL, 0, inbuf + (inlen - AES_GCM_TAG_LEN), AES_GCM_TAG_LEN, inbuf, *outbuf);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed at mbedtls_gcm_auth_decrypt : %d", ret);
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t sec2_req_handler(protocomm_security_handle_t handle,
                                  const void *sec_params,
                                  uint32_t session_id,
                                  const uint8_t *inbuf, ssize_t inlen,
                                  uint8_t **outbuf, ssize_t *outlen,
                                  void *priv_data)
{
    session_t *cur_session = (session_t *) handle;
    if (!cur_session) {
        ESP_LOGE(TAG, "Invalid session context data");
        return ESP_ERR_INVALID_ARG;
    }

    if (session_id != cur_session->id) {
        ESP_LOGE(TAG, "Invalid session ID:%" PRId32 "(expected %" PRId32 ")", session_id, cur_session->id);
        return ESP_ERR_INVALID_STATE;
    }

    SessionData *req;
    SessionData resp;
    esp_err_t ret;

    req = session_data__unpack(NULL, inlen, inbuf);
    if (!req) {
        ESP_LOGE(TAG, "Unable to unpack setup_req");
        return ESP_ERR_INVALID_ARG;
    }
    if (req->sec_ver != protocomm_security2.ver) {
        ESP_LOGE(TAG, "Security version mismatch. Closing connection");
        session_data__free_unpacked(req, NULL);
        return ESP_ERR_INVALID_ARG;
    }

    session_data__init(&resp);
    ret = sec2_session_setup(cur_session, session_id, req, &resp, (protocomm_security2_params_t *) sec_params);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Session setup error %d", ret);
        session_data__free_unpacked(req, NULL);
        return ESP_FAIL;
    }

    resp.sec_ver = req->sec_ver;
    session_data__free_unpacked(req, NULL);

    *outlen = session_data__get_packed_size(&resp);
    *outbuf = (uint8_t *) malloc(*outlen);
    if (!*outbuf) {
        ESP_LOGE(TAG, "System out of memory");
        return ESP_ERR_NO_MEM;
    }
    session_data__pack(&resp, *outbuf);

    sec2_session_setup_cleanup(cur_session, session_id, &resp);
    return ESP_OK;
}

const protocomm_security_t protocomm_security2 = {
    .ver = 2,
    .init = sec2_init,
    .cleanup = sec2_cleanup,
    .new_transport_session = sec2_new_session,
    .close_transport_session = sec2_close_session,
    .security_req_handler = sec2_req_handler,
    .encrypt = sec2_encrypt,
    .decrypt = sec2_decrypt,
};
