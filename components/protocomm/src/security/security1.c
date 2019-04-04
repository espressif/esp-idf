// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <esp_err.h>
#include <esp_log.h>

#include <mbedtls/aes.h>
#include <mbedtls/sha256.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/ecdh.h>
#include <mbedtls/error.h>
#include <mbedtls/ssl_internal.h>

#include <protocomm_security.h>
#include <protocomm_security1.h>

#include "session.pb-c.h"
#include "sec1.pb-c.h"
#include "constants.pb-c.h"

static const char* TAG = "security1";

#define PUBLIC_KEY_LEN  32
#define SZ_RANDOM       16

#define SESSION_STATE_CMD0  0 /* Session is not setup */
#define SESSION_STATE_CMD1  1 /* Session is not setup */
#define SESSION_STATE_DONE  2 /* Session setup successful */

typedef struct session {
    /* Session data */
    uint32_t id;
    uint8_t state;
    uint8_t device_pubkey[PUBLIC_KEY_LEN];
    uint8_t client_pubkey[PUBLIC_KEY_LEN];
    uint8_t sym_key[PUBLIC_KEY_LEN];
    uint8_t rand[SZ_RANDOM];

    /* mbedtls context data for AES */
    mbedtls_aes_context ctx_aes;
    unsigned char stb[16];
    size_t nc_off;
} session_t;

static session_t *cur_session;

static void flip_endian(uint8_t *data, size_t len)
{
    uint8_t swp_buf;
    for (int i = 0; i < len/2; i++) {
        swp_buf = data[i];
        data[i] = data[len - i - 1];
        data[len - i - 1] = swp_buf;
    }
}

static void hexdump(const char *msg, uint8_t *buf, int len)
{
    ESP_LOGD(TAG, "%s:", msg);
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, buf, len, ESP_LOG_DEBUG);
}

static esp_err_t handle_session_command1(uint32_t session_id,
                                         SessionData *req, SessionData *resp)
{
    ESP_LOGD(TAG, "Request to handle setup1_command");
    Sec1Payload *in = (Sec1Payload *) req->sec1;
    uint8_t check_buf[PUBLIC_KEY_LEN];
    int mbed_err;

    if (cur_session->state != SESSION_STATE_CMD1) {
        ESP_LOGE(TAG, "Invalid state of session %d (expected %d)", SESSION_STATE_CMD1, cur_session->state);
        return ESP_ERR_INVALID_STATE;
    }

    /* Initialize crypto context */
    mbedtls_aes_init(&cur_session->ctx_aes);
    memset(cur_session->stb, 0, sizeof(cur_session->stb));
    cur_session->nc_off = 0;

    hexdump("Client verifier", in->sc1->client_verify_data.data,
            in->sc1->client_verify_data.len);

    mbed_err = mbedtls_aes_setkey_enc(&cur_session->ctx_aes, cur_session->sym_key,
                                      sizeof(cur_session->sym_key)*8);
    if (mbed_err != 0) {
        ESP_LOGE(TAG, "Failure at mbedtls_aes_setkey_enc with error code : -0x%x", -mbed_err);
        mbedtls_aes_free(&cur_session->ctx_aes);
        return ESP_FAIL;
    }

    mbed_err = mbedtls_aes_crypt_ctr(&cur_session->ctx_aes,
                                     PUBLIC_KEY_LEN, &cur_session->nc_off,
                                     cur_session->rand, cur_session->stb,
                                     in->sc1->client_verify_data.data, check_buf);
    if (mbed_err != 0) {
        ESP_LOGE(TAG, "Failure at mbedtls_aes_crypt_ctr with error code : -0x%x", -mbed_err);
        mbedtls_aes_free(&cur_session->ctx_aes);
        return ESP_FAIL;
    }

    hexdump("Dec Client verifier", check_buf, sizeof(check_buf));

    /* constant time memcmp */
    if (mbedtls_ssl_safer_memcmp(check_buf, cur_session->device_pubkey,
                                 sizeof(cur_session->device_pubkey)) != 0) {
        ESP_LOGE(TAG, "Key mismatch. Close connection");
        mbedtls_aes_free(&cur_session->ctx_aes);
        return ESP_FAIL;
    }

    Sec1Payload *out = (Sec1Payload *) malloc(sizeof(Sec1Payload));
    SessionResp1 *out_resp = (SessionResp1 *) malloc(sizeof(SessionResp1));
    if (!out || !out_resp) {
        ESP_LOGE(TAG, "Error allocating memory for response1");
        free(out);
        free(out_resp);
        mbedtls_aes_free(&cur_session->ctx_aes);
        return ESP_ERR_NO_MEM;
    }

    sec1_payload__init(out);
    session_resp1__init(out_resp);
    out_resp->status = STATUS__Success;

    uint8_t *outbuf = (uint8_t *) malloc(PUBLIC_KEY_LEN);
    if (!outbuf) {
        ESP_LOGE(TAG, "Error allocating ciphertext buffer");
        free(out);
        free(out_resp);
        mbedtls_aes_free(&cur_session->ctx_aes);
        return ESP_ERR_NO_MEM;
    }

    mbed_err = mbedtls_aes_crypt_ctr(&cur_session->ctx_aes,
                                     PUBLIC_KEY_LEN, &cur_session->nc_off,
                                     cur_session->rand, cur_session->stb,
                                     cur_session->client_pubkey, outbuf);
    if (mbed_err != 0) {
        ESP_LOGE(TAG, "Failure at mbedtls_aes_crypt_ctr with error code : -0x%x", -mbed_err);
        free(outbuf);
        free(out);
        free(out_resp);
        mbedtls_aes_free(&cur_session->ctx_aes);
        return ESP_FAIL;
    }

    out_resp->device_verify_data.data = outbuf;
    out_resp->device_verify_data.len = PUBLIC_KEY_LEN;
    hexdump("Device verify data", outbuf, PUBLIC_KEY_LEN);

    out->msg = SEC1_MSG_TYPE__Session_Response1;
    out->payload_case = SEC1_PAYLOAD__PAYLOAD_SR1;
    out->sr1 = out_resp;

    resp->proto_case = SESSION_DATA__PROTO_SEC1;
    resp->sec1 = out;

    cur_session->state = SESSION_STATE_DONE;
    ESP_LOGD(TAG, "Secure session established successfully");
    return ESP_OK;
}

static esp_err_t handle_session_command0(uint32_t session_id,
                                         SessionData *req, SessionData *resp,
                                         const protocomm_security_pop_t *pop)
{
    ESP_LOGD(TAG, "Request to handle setup0_command");
    Sec1Payload *in = (Sec1Payload *) req->sec1;
    esp_err_t ret;
    int mbed_err;

    if (cur_session->state != SESSION_STATE_CMD0) {
        ESP_LOGE(TAG, "Invalid state of session %d (expected %d)", SESSION_STATE_CMD0, cur_session->state);
        return ESP_ERR_INVALID_STATE;
    }

    if (in->sc0->client_pubkey.len != PUBLIC_KEY_LEN) {
        ESP_LOGE(TAG, "Invalid public key length");
        return ESP_ERR_INVALID_ARG;
    }

    mbedtls_ecdh_context     *ctx_server = malloc(sizeof(mbedtls_ecdh_context));
    mbedtls_entropy_context  *entropy    = malloc(sizeof(mbedtls_entropy_context));
    mbedtls_ctr_drbg_context *ctr_drbg   = malloc(sizeof(mbedtls_ctr_drbg_context));
    if (!ctx_server || !ctx_server || !ctr_drbg) {
        ESP_LOGE(TAG, "Failed to allocate memory for mbedtls context");
        free(ctx_server);
        free(entropy);
        free(ctr_drbg);
        return ESP_ERR_NO_MEM;
    }

    mbedtls_ecdh_init(ctx_server);
    mbedtls_ctr_drbg_init(ctr_drbg);
    mbedtls_entropy_init(entropy);

    mbed_err = mbedtls_ctr_drbg_seed(ctr_drbg, mbedtls_entropy_func,
                                     entropy, NULL, 0);
    if (mbed_err != 0) {
        ESP_LOGE(TAG, "Failed at mbedtls_ctr_drbg_seed with error code : -0x%x", -mbed_err);
        ret = ESP_FAIL;
        goto exit_cmd0;
    }

    mbed_err = mbedtls_ecp_group_load(&ctx_server->grp, MBEDTLS_ECP_DP_CURVE25519);
    if (mbed_err != 0) {
        ESP_LOGE(TAG, "Failed at mbedtls_ecp_group_load with error code : -0x%x", -mbed_err);
        ret = ESP_FAIL;
        goto exit_cmd0;
    }

    mbed_err = mbedtls_ecdh_gen_public(&ctx_server->grp, &ctx_server->d, &ctx_server->Q,
                                       mbedtls_ctr_drbg_random, ctr_drbg);
    if (mbed_err != 0) {
        ESP_LOGE(TAG, "Failed at mbedtls_ecdh_gen_public with error code : -0x%x", -mbed_err);
        ret = ESP_FAIL;
        goto exit_cmd0;
    }

    mbed_err = mbedtls_mpi_write_binary(&ctx_server->Q.X,
                                        cur_session->device_pubkey,
                                        PUBLIC_KEY_LEN);
    if (mbed_err != 0) {
        ESP_LOGE(TAG, "Failed at mbedtls_mpi_write_binary with error code : -0x%x", -mbed_err);
        ret = ESP_FAIL;
        goto exit_cmd0;
    }
    flip_endian(cur_session->device_pubkey, PUBLIC_KEY_LEN);

    memcpy(cur_session->client_pubkey, in->sc0->client_pubkey.data, PUBLIC_KEY_LEN);

    uint8_t *dev_pubkey = cur_session->device_pubkey;
    uint8_t *cli_pubkey = cur_session->client_pubkey;
    hexdump("Device pubkey", dev_pubkey, PUBLIC_KEY_LEN);
    hexdump("Client pubkey", cli_pubkey, PUBLIC_KEY_LEN);

    mbed_err = mbedtls_mpi_lset(&ctx_server->Qp.Z, 1);
    if (mbed_err != 0) {
        ESP_LOGE(TAG, "Failed at mbedtls_mpi_lset with error code : -0x%x", -mbed_err);
        ret = ESP_FAIL;
        goto exit_cmd0;
    }

    flip_endian(cur_session->client_pubkey, PUBLIC_KEY_LEN);
    mbed_err = mbedtls_mpi_read_binary(&ctx_server->Qp.X, cli_pubkey, PUBLIC_KEY_LEN);
    flip_endian(cur_session->client_pubkey, PUBLIC_KEY_LEN);
    if (mbed_err != 0) {
        ESP_LOGE(TAG, "Failed at mbedtls_mpi_read_binary with error code : -0x%x", -mbed_err);
        ret = ESP_FAIL;
        goto exit_cmd0;
    }

    mbed_err = mbedtls_ecdh_compute_shared(&ctx_server->grp, &ctx_server->z, &ctx_server->Qp,
                                           &ctx_server->d, mbedtls_ctr_drbg_random, ctr_drbg);
    if (mbed_err != 0) {
        ESP_LOGE(TAG, "Failed at mbedtls_ecdh_compute_shared with error code : -0x%x", -mbed_err);
        ret = ESP_FAIL;
        goto exit_cmd0;
    }

    mbed_err = mbedtls_mpi_write_binary(&ctx_server->z, cur_session->sym_key, PUBLIC_KEY_LEN);
    if (mbed_err != 0) {
        ESP_LOGE(TAG, "Failed at mbedtls_mpi_write_binary with error code : -0x%x", -mbed_err);
        ret = ESP_FAIL;
        goto exit_cmd0;
    }
    flip_endian(cur_session->sym_key, PUBLIC_KEY_LEN);

    if (pop != NULL && pop->data != NULL && pop->len != 0) {
        ESP_LOGD(TAG, "Adding proof of possession");
        uint8_t sha_out[PUBLIC_KEY_LEN];

        mbed_err = mbedtls_sha256_ret((const unsigned char *) pop->data, pop->len, sha_out, 0);
        if (mbed_err != 0) {
            ESP_LOGE(TAG, "Failed at mbedtls_sha256_ret with error code : -0x%x", -mbed_err);
            ret = ESP_FAIL;
            goto exit_cmd0;
        }

        for (int i = 0; i < PUBLIC_KEY_LEN; i++) {
            cur_session->sym_key[i] ^= sha_out[i];
        }
    }

    hexdump("Shared key", cur_session->sym_key, PUBLIC_KEY_LEN);

    mbed_err = mbedtls_ctr_drbg_random(ctr_drbg, cur_session->rand, SZ_RANDOM);
    if (mbed_err != 0) {
        ESP_LOGE(TAG, "Failed at mbedtls_ctr_drbg_random with error code : -0x%x", -mbed_err);
        ret = ESP_FAIL;
        goto exit_cmd0;
    }

    hexdump("Device random", cur_session->rand, SZ_RANDOM);

    Sec1Payload *out = (Sec1Payload *) malloc(sizeof(Sec1Payload));
    SessionResp0 *out_resp = (SessionResp0 *) malloc(sizeof(SessionResp0));
    if (!out || !out_resp) {
        ESP_LOGE(TAG, "Error allocating memory for response0");
        ret = ESP_ERR_NO_MEM;
        free(out);
        free(out_resp);
        goto exit_cmd0;
    }

    sec1_payload__init(out);
    session_resp0__init(out_resp);

    out_resp->status = STATUS__Success;

    out_resp->device_pubkey.data = dev_pubkey;
    out_resp->device_pubkey.len = PUBLIC_KEY_LEN;

    out_resp->device_random.data = (uint8_t *) cur_session->rand;
    out_resp->device_random.len = SZ_RANDOM;

    out->msg = SEC1_MSG_TYPE__Session_Response0;
    out->payload_case = SEC1_PAYLOAD__PAYLOAD_SR0;
    out->sr0 = out_resp;

    resp->proto_case = SESSION_DATA__PROTO_SEC1;
    resp->sec1 = out;

    cur_session->state = SESSION_STATE_CMD1;

    ESP_LOGD(TAG, "Session setup phase1 done");
    ret = ESP_OK;

exit_cmd0:
    mbedtls_ecdh_free(ctx_server);
    free(ctx_server);

    mbedtls_ctr_drbg_free(ctr_drbg);
    free(ctr_drbg);

    mbedtls_entropy_free(entropy);
    free(entropy);

    return ret;
}

static esp_err_t sec1_session_setup(uint32_t session_id,
                                    SessionData *req, SessionData *resp,
                                    const protocomm_security_pop_t *pop)
{
    Sec1Payload *in = (Sec1Payload *) req->sec1;
    esp_err_t ret;

    if (!cur_session) {
        ESP_LOGE(TAG, "Invalid session context data");
        return ESP_ERR_INVALID_ARG;
    }

    if (session_id != cur_session->id) {
        ESP_LOGE(TAG, "Invalid session ID : %d (expected %d)", session_id, cur_session->id);
        return ESP_ERR_INVALID_STATE;
    }

    if (!in) {
        ESP_LOGE(TAG, "Empty session data");
        return ESP_ERR_INVALID_ARG;
    }

    switch (in->msg) {
        case SEC1_MSG_TYPE__Session_Command0:
            ret = handle_session_command0(session_id, req, resp, pop);
            break;
        case SEC1_MSG_TYPE__Session_Command1:
            ret = handle_session_command1(session_id, req, resp);
            break;
        default:
            ESP_LOGE(TAG, "Invalid security message type");
            ret = ESP_ERR_INVALID_ARG;
    }

    return ret;

}

static void sec1_session_setup_cleanup(uint32_t session_id, SessionData *resp)
{
    Sec1Payload *out = resp->sec1;

    if (!out) {
        return;
    }

    switch (out->msg) {
        case SEC1_MSG_TYPE__Session_Response0:
            {
                SessionResp0 *out_resp0 = out->sr0;
                if (out_resp0) {
                    free(out_resp0);
                }
                break;
            }
        case SEC1_MSG_TYPE__Session_Response1:
            {
                SessionResp1 *out_resp1 = out->sr1;
                if (out_resp1) {
                    free(out_resp1->device_verify_data.data);
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

static esp_err_t sec1_close_session(uint32_t session_id)
{
    if (!cur_session || cur_session->id != session_id) {
        ESP_LOGE(TAG, "Attempt to close invalid session");
        return ESP_ERR_INVALID_ARG;
    }

    if (cur_session->state == SESSION_STATE_DONE) {
        /* Free AES context data */
        mbedtls_aes_free(&cur_session->ctx_aes);
    }

    bzero(cur_session, sizeof(session_t));
    free(cur_session);
    cur_session = NULL;
    return ESP_OK;
}

static esp_err_t sec1_new_session(uint32_t session_id)
{
    if (cur_session) {
        /* Only one session is allowed at a time */
        ESP_LOGE(TAG, "Closing old session with id %u", cur_session->id);
        sec1_close_session(cur_session->id);
    }

    cur_session = (session_t *) calloc(1, sizeof(session_t));
    if (!cur_session) {
        ESP_LOGE(TAG, "Error allocating session structure");
        return ESP_ERR_NO_MEM;
    }

    cur_session->id = session_id;
    return ESP_OK;
}

static esp_err_t sec1_init()
{
    return ESP_OK;
}

static esp_err_t sec1_cleanup()
{
    if (cur_session) {
        ESP_LOGD(TAG, "Closing current session with id %u", cur_session->id);
        sec1_close_session(cur_session->id);
    }
    return ESP_OK;
}

static esp_err_t sec1_decrypt(uint32_t session_id,
                              const uint8_t *inbuf, ssize_t inlen,
                              uint8_t *outbuf, ssize_t *outlen)
{
    if (*outlen < inlen) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!cur_session || cur_session->id != session_id) {
        ESP_LOGE(TAG, "Session with ID %d not found", session_id);
        return ESP_ERR_INVALID_STATE;
    }

    if (cur_session->state != SESSION_STATE_DONE) {
        ESP_LOGE(TAG, "Secure session not established");
        return ESP_ERR_INVALID_STATE;
    }

    *outlen = inlen;
    int ret = mbedtls_aes_crypt_ctr(&cur_session->ctx_aes, inlen, &cur_session->nc_off,
                                    cur_session->rand, cur_session->stb, inbuf, outbuf);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed at mbedtls_aes_crypt_ctr with error code : %d", ret);
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t sec1_req_handler(const protocomm_security_pop_t *pop, uint32_t session_id,
                                  const uint8_t *inbuf, ssize_t inlen,
                                  uint8_t **outbuf, ssize_t *outlen,
                                  void *priv_data)
{
    SessionData *req;
    SessionData resp;
    esp_err_t ret;

    req = session_data__unpack(NULL, inlen, inbuf);
    if (!req) {
        ESP_LOGE(TAG, "Unable to unpack setup_req");
        return ESP_ERR_INVALID_ARG;
    }
    if (req->sec_ver != protocomm_security1.ver) {
        ESP_LOGE(TAG, "Security version mismatch. Closing connection");
        session_data__free_unpacked(req, NULL);
        return ESP_ERR_INVALID_ARG;
    }

    session_data__init(&resp);
    ret = sec1_session_setup(session_id, req, &resp, pop);
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

    sec1_session_setup_cleanup(session_id, &resp);
    return ESP_OK;
}

const protocomm_security_t protocomm_security1 = {
    .ver = 1,
    .init = sec1_init,
    .cleanup = sec1_cleanup,
    .new_transport_session = sec1_new_session,
    .close_transport_session = sec1_close_session,
    .security_req_handler = sec1_req_handler,
    .encrypt = sec1_decrypt, /* Encrypt == decrypt for AES-CTR */
    .decrypt = sec1_decrypt,
};
