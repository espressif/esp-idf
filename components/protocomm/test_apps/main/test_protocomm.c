/*
 * SPDX-FileCopyrightText: 2018-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_system.h>
#include <sys/random.h>
#include <unistd.h>
#include <unity.h>

/* TODO: Currently MBEDTLS_ECDH_LEGACY_CONTEXT is enabled by default
 * when MBEDTLS_ECP_RESTARTABLE is enabled.
 * This is a temporary workaround to allow that.
 *
 * The legacy option is soon going to be removed in future mbedtls
 * versions and this workaround will be removed once the appropriate
 * solution is available.
 */
#ifdef CONFIG_MBEDTLS_ECDH_LEGACY_CONTEXT
#define ACCESS_ECDH(S, var) S.MBEDTLS_PRIVATE(var)
#else
#define ACCESS_ECDH(S, var) S.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh).MBEDTLS_PRIVATE(var)
#endif

#include <mbedtls/aes.h>
#include <mbedtls/sha256.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/ecdh.h>
#include <mbedtls/error.h>

#include <protocomm.h>
#include <protocomm_security.h>
#include <protocomm_security0.h>
#include <protocomm_security1.h>
#include "test_utils.h"

#include "session.pb-c.h"

#ifdef CONFIG_HEAP_TRACING
    #include <esp_heap_trace.h>
    #define NUM_RECORDS 100
    static heap_trace_record_t trace_record[NUM_RECORDS]; // This buffer must be in internal RAM
#endif

#define PUBLIC_KEY_LEN  32
#define SZ_RANDOM       16

typedef struct {
    uint32_t id;
    uint8_t  sec_ver;
    uint8_t  weak;
    const protocomm_security1_params_t *pop;
    uint8_t device_pubkey[PUBLIC_KEY_LEN];
    uint8_t client_pubkey[PUBLIC_KEY_LEN];
    uint8_t sym_key[PUBLIC_KEY_LEN];
    uint8_t rand[SZ_RANDOM];

    /* mbedtls context data for Curve25519 */
    mbedtls_ecdh_context ctx_client;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;

    /* mbedtls context data for AES */
    mbedtls_aes_context ctx_aes;
    unsigned char stb[16];
    size_t nc_off;
} session_t;

static const char *TAG = "protocomm_test";

static protocomm_t *test_pc = NULL;
static const protocomm_security_t *test_sec = NULL;
protocomm_security_handle_t sec_inst = NULL;
static uint32_t test_priv_data = 1234;

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
    ESP_LOGI(TAG, "%s:", msg);
    ESP_LOG_BUFFER_HEX(TAG, buf, len);
}

static esp_err_t prepare_command0(session_t *session, SessionData *req)
{
    Sec1Payload *in = (Sec1Payload *) malloc(sizeof(Sec1Payload));
    if (in == NULL) {
        ESP_LOGE(TAG, "Error allocating memory for request");
        return ESP_ERR_NO_MEM;
    }

    SessionCmd0 *in_req = (SessionCmd0 *) malloc(sizeof(SessionCmd0));
    if (in_req == NULL) {
        ESP_LOGE(TAG, "Error allocating memory for request");
        free(in);
        return ESP_ERR_NO_MEM;
    }

    sec1_payload__init(in);
    session_cmd0__init(in_req);

    in_req->client_pubkey.data = session->client_pubkey;
    in_req->client_pubkey.len = PUBLIC_KEY_LEN;

    in->msg = SEC1_MSG_TYPE__Session_Command0;
    in->payload_case = SEC1_PAYLOAD__PAYLOAD_SC0;
    in->sc0 = in_req;

    req->proto_case = SESSION_DATA__PROTO_SEC1;
    req->sec_ver = protocomm_security1.ver;
    req->sec1 = in;

    return ESP_OK;
}

static void cleanup_command0(SessionData *req)
{
    free(req->sec1->sc0);
    free(req->sec1);
}

static esp_err_t verify_response0(session_t *session, SessionData *resp)
{
    if ((resp->proto_case != SESSION_DATA__PROTO_SEC1) ||
        (resp->sec1->msg  != SEC1_MSG_TYPE__Session_Response0)) {
        ESP_LOGE(TAG, "Invalid response type");
        return ESP_ERR_INVALID_ARG;
    }

    int ret;
    Sec1Payload *in = (Sec1Payload *) resp->sec1;

    if (in->sr0->device_pubkey.len != PUBLIC_KEY_LEN) {
        ESP_LOGE(TAG, "Device public key length as not as expected");
        return ESP_FAIL;
    }

    if (in->sr0->device_random.len != SZ_RANDOM) {
        ESP_LOGE(TAG, "Device random data length is not as expected");
        return ESP_FAIL;
    }

    uint8_t *cli_pubkey = session->client_pubkey;
    uint8_t *dev_pubkey = session->device_pubkey;
    memcpy(session->device_pubkey, in->sr0->device_pubkey.data, in->sr0->device_pubkey.len);

    hexdump("Device pubkey", dev_pubkey, PUBLIC_KEY_LEN);
    hexdump("Client pubkey", cli_pubkey, PUBLIC_KEY_LEN);

    ret = mbedtls_mpi_lset(ACCESS_ECDH(&session->ctx_client, Qp).MBEDTLS_PRIVATE(Z), 1);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed at mbedtls_mpi_lset with error code : %d", ret);
        return ESP_FAIL;
    }

    flip_endian(session->device_pubkey, PUBLIC_KEY_LEN);
    ret = mbedtls_mpi_read_binary(ACCESS_ECDH(&session->ctx_client, Qp).MBEDTLS_PRIVATE(X), dev_pubkey, PUBLIC_KEY_LEN);
    flip_endian(session->device_pubkey, PUBLIC_KEY_LEN);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed at mbedtls_mpi_read_binary with error code : %d", ret);
        return ESP_FAIL;
    }

    ret = mbedtls_ecdh_compute_shared(ACCESS_ECDH(&session->ctx_client, grp),
                                      ACCESS_ECDH(&session->ctx_client, z),
                                      ACCESS_ECDH(&session->ctx_client, Qp),
                                      ACCESS_ECDH(&session->ctx_client, d),
                                      mbedtls_ctr_drbg_random,
                                      &session->ctr_drbg);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed at mbedtls_ecdh_compute_shared with error code : %d", ret);
        return ESP_FAIL;
    }

    ret = mbedtls_mpi_write_binary(ACCESS_ECDH(&session->ctx_client, z), session->sym_key, PUBLIC_KEY_LEN);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed at mbedtls_mpi_write_binary with error code : %d", ret);
        return ESP_FAIL;
    }
    flip_endian(session->sym_key, PUBLIC_KEY_LEN);

    const protocomm_security1_params_t *pop = session->pop;
    if (pop != NULL && pop->data != NULL && pop->len != 0) {
        ESP_LOGD(TAG, "Adding proof of possession");
        uint8_t sha_out[PUBLIC_KEY_LEN];

        ret = mbedtls_sha256((const unsigned char *) pop->data, pop->len, sha_out, 0);
        if (ret != 0) {
            ESP_LOGE(TAG, "Failed at mbedtls_sha256_ret with error code : %d", ret);
            return ESP_FAIL;
        }

        for (int i = 0; i < PUBLIC_KEY_LEN; i++) {
            session->sym_key[i] ^= sha_out[i];
        }
    }

    hexdump("Shared key", session->sym_key, PUBLIC_KEY_LEN);

    memcpy(session->rand, in->sr0->device_random.data, in->sr0->device_random.len);
    hexdump("Dev random", session->rand, sizeof(session->rand));
    return ESP_OK;
}

static esp_err_t prepare_command1(session_t *session, SessionData *req)
{
    int ret;
    uint8_t *outbuf = (uint8_t *) malloc(PUBLIC_KEY_LEN);
    if (!outbuf) {
        ESP_LOGE(TAG, "Error allocating ciphertext buffer");
        return ESP_ERR_NO_MEM;
    }

    /* Initialise crypto context */
    mbedtls_aes_init(&session->ctx_aes);
    memset(session->stb, 0, sizeof(session->stb));
    session->nc_off = 0;

    ret = mbedtls_aes_setkey_enc(&session->ctx_aes, session->sym_key,
                                 sizeof(session->sym_key)*8);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed at mbedtls_aes_setkey_enc with error code : %d", ret);
        free(outbuf);
        return ESP_FAIL;
    }

    ret = mbedtls_aes_crypt_ctr(&session->ctx_aes, PUBLIC_KEY_LEN,
                                &session->nc_off, session->rand,
                                session->stb, session->device_pubkey, outbuf);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed at mbedtls_aes_crypt_ctr with error code : %d", ret);
        free(outbuf);
        return ESP_FAIL;
    }

    Sec1Payload *out = (Sec1Payload *) malloc(sizeof(Sec1Payload));
    if (!out) {
        ESP_LOGE(TAG, "Error allocating out buffer");
        free(outbuf);
        return ESP_ERR_NO_MEM;
    }
    sec1_payload__init(out);

    SessionCmd1 *out_req = (SessionCmd1 *) malloc(sizeof(SessionCmd1));
    if (!out_req) {
        ESP_LOGE(TAG, "Error allocating out_req buffer");
        free(outbuf);
        free(out);
        return ESP_ERR_NO_MEM;
    }
    session_cmd1__init(out_req);

    out_req->client_verify_data.data = outbuf;
    out_req->client_verify_data.len = PUBLIC_KEY_LEN;
    hexdump("Client verify data", outbuf, PUBLIC_KEY_LEN);

    out->msg = SEC1_MSG_TYPE__Session_Command1;
    out->payload_case = SEC1_PAYLOAD__PAYLOAD_SC1;
    out->sc1 = out_req;

    req->proto_case = SESSION_DATA__PROTO_SEC1;
    req->sec_ver = protocomm_security1.ver;
    req->sec1 = out;

    return ESP_OK;
}

static void cleanup_command1(SessionData *req)
{
    free(req->sec1->sc1->client_verify_data.data);
    free(req->sec1->sc1);
    free(req->sec1);
}

static esp_err_t verify_response1(session_t *session, SessionData *resp)
{
    uint8_t *cli_pubkey = session->client_pubkey;
    uint8_t *dev_pubkey = session->device_pubkey;

    hexdump("Device pubkey", dev_pubkey, PUBLIC_KEY_LEN);
    hexdump("Client pubkey", cli_pubkey, PUBLIC_KEY_LEN);

    if ((resp->proto_case != SESSION_DATA__PROTO_SEC1) ||
        (resp->sec1->msg  != SEC1_MSG_TYPE__Session_Response1)) {
        ESP_LOGE(TAG, "Invalid response type");
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t check_buf[PUBLIC_KEY_LEN];
    Sec1Payload *in = (Sec1Payload *) resp->sec1;

    int ret = mbedtls_aes_crypt_ctr(&session->ctx_aes, PUBLIC_KEY_LEN,
                                    &session->nc_off, session->rand, session->stb,
                                    in->sr1->device_verify_data.data, check_buf);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed at mbedtls_aes_crypt_ctr with error code : %d", ret);
        return ESP_FAIL;
    }
    hexdump("Dec Device verifier", check_buf, sizeof(check_buf));

    if (memcmp(check_buf, session->client_pubkey, sizeof(session->client_pubkey)) != 0) {
        ESP_LOGE(TAG, "Key mismatch. Close connection");
        return ESP_FAIL;
    }

    return ESP_OK;
}

static esp_err_t test_new_session(session_t *session)
{
    if (session->sec_ver == 0) {
        return ESP_OK;
    }

    if (!test_sec) {
        return ESP_ERR_INVALID_STATE;
    }

    if (test_sec->init && (test_sec->init(&sec_inst) != ESP_OK)) {
        return ESP_ERR_NO_MEM;
    }

    uint32_t session_id = session->id;
    if (test_sec->new_transport_session &&
        (test_sec->new_transport_session(sec_inst, session_id) != ESP_OK)) {
        ESP_LOGE(TAG, "Failed to launch new transport session");
        return ESP_FAIL;
    }

    if (protocomm_open_session(test_pc, session_id) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open new protocomm session");
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t test_delete_session(session_t *session)
{
    if (!test_sec) {
        return ESP_ERR_INVALID_STATE;
    }

    if (test_sec->cleanup && (test_sec->cleanup(sec_inst) != ESP_OK)) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t test_sec_endpoint(session_t *session)
{
    if (session->sec_ver == 0) {
        return ESP_OK;
    }

    uint32_t session_id = session->id;

    int ret = ESP_FAIL;
    SessionData req;
    SessionData *resp;
    ssize_t  inlen = 0;
    uint8_t *inbuf = NULL;
    ssize_t  outlen = 0;
    uint8_t *outbuf = NULL;

    mbedtls_ecdh_init(&session->ctx_client);
    mbedtls_ecdh_setup(&session->ctx_client, MBEDTLS_ECP_DP_CURVE25519);
    mbedtls_ctr_drbg_init(&session->ctr_drbg);

    mbedtls_entropy_init(&session->entropy);
    ret = mbedtls_ctr_drbg_seed(&session->ctr_drbg, mbedtls_entropy_func,
                                &session->entropy, NULL, 0);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed at mbedtls_ctr_drbg_seed with error code : %d", ret);
        goto abort_test_sec_endpoint;
    }

    ret = mbedtls_ecp_group_load(ACCESS_ECDH(&session->ctx_client, grp), MBEDTLS_ECP_DP_CURVE25519);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed at mbedtls_ecp_group_load with error code : %d", ret);
        goto abort_test_sec_endpoint;
    }

    ret = mbedtls_ecdh_gen_public(ACCESS_ECDH(&session->ctx_client, grp),
                                  ACCESS_ECDH(&session->ctx_client, d),
                                  ACCESS_ECDH(&session->ctx_client, Q),
                                  mbedtls_ctr_drbg_random,
                                  &session->ctr_drbg);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed at mbedtls_ecdh_gen_public with error code : %d", ret);
        goto abort_test_sec_endpoint;
    }

    if (session->weak) {
        /* Read zero client public key */
        ret = mbedtls_mpi_read_binary(ACCESS_ECDH(&session->ctx_client, Q).MBEDTLS_PRIVATE(X),
                                      session->client_pubkey,
                                      PUBLIC_KEY_LEN);
        if (ret != 0) {
            ESP_LOGE(TAG, "Failed at mbedtls_mpi_read_binary with error code : %d", ret);
            goto abort_test_sec_endpoint;
        }
    }
    ret = mbedtls_mpi_write_binary(ACCESS_ECDH(&session->ctx_client, Q).MBEDTLS_PRIVATE(X),
                                   session->client_pubkey,
                                   PUBLIC_KEY_LEN);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed at mbedtls_mpi_write_binary with error code : %d", ret);
        goto abort_test_sec_endpoint;
    }
    flip_endian(session->client_pubkey, PUBLIC_KEY_LEN);

    /*********** Transaction0 = SessionCmd0 + SessionResp0 ****************/
    session_data__init(&req);
    if (prepare_command0(session, &req) != ESP_OK) {
        ESP_LOGE(TAG, "Failed in prepare_command0");
        goto abort_test_sec_endpoint;
    }

    inlen = session_data__get_packed_size(&req);
    inbuf = (uint8_t *) malloc(inlen);
    if (!inbuf) {
        ESP_LOGE(TAG, "Failed to allocate inbuf");
        goto abort_test_sec_endpoint;
    }

    session_data__pack(&req, inbuf);
    cleanup_command0(&req);

    outlen = 0;
    outbuf = NULL;
    ret = protocomm_req_handle(test_pc, "test-sec", session_id,
                               inbuf, inlen, &outbuf, &outlen);

    free(inbuf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "test-sec handler failed");
        free(outbuf);
        goto abort_test_sec_endpoint;
    }

    resp = session_data__unpack(NULL, outlen, outbuf);
    free(outbuf);
    if (!resp) {
        ESP_LOGE(TAG, "Unable to unpack SessionResp0");
        goto abort_test_sec_endpoint;
    }

    if (verify_response0(session, resp) != ESP_OK) {
        ESP_LOGE(TAG, "Invalid response 0");
        session_data__free_unpacked(resp, NULL);
        goto abort_test_sec_endpoint;
    }

    session_data__free_unpacked(resp, NULL);

    /*********** Transaction1 = SessionCmd1 + SessionResp1 ****************/
    session_data__init(&req);
    if (prepare_command1(session, &req) != ESP_OK) {
        ESP_LOGE(TAG, "Failed in prepare_command1");
        goto abort_test_sec_endpoint;
    }

    inlen = session_data__get_packed_size(&req);
    inbuf = (uint8_t *) malloc(inlen);
    if (!inbuf) {
        ESP_LOGE(TAG, "Failed to allocate inbuf");
        goto abort_test_sec_endpoint;
    }

    session_data__pack(&req, inbuf);
    cleanup_command1(&req);

    outlen = 0;
    outbuf = NULL;
    ret = protocomm_req_handle(test_pc, "test-sec", session_id,
                               inbuf, inlen, &outbuf, &outlen);

    free(inbuf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "test-sec handler failed");
        free(outbuf);
        goto abort_test_sec_endpoint;
    }

    resp = session_data__unpack(NULL, outlen, outbuf);
    free(outbuf);
    if (!resp) {
        ESP_LOGE(TAG, "Unable to unpack SessionResp0");
        goto abort_test_sec_endpoint;
    }

    if (verify_response1(session, resp) != ESP_OK) {
        ESP_LOGE(TAG, "Invalid response 1");
        session_data__free_unpacked(resp, NULL);
        goto abort_test_sec_endpoint;
    }

    session_data__free_unpacked(resp, NULL);
    mbedtls_ecdh_free(&session->ctx_client);
    mbedtls_ctr_drbg_free(&session->ctr_drbg);
    mbedtls_entropy_free(&session->entropy);

    return ESP_OK;

abort_test_sec_endpoint:

    mbedtls_ecdh_free(&session->ctx_client);
    mbedtls_ctr_drbg_free(&session->ctr_drbg);
    mbedtls_entropy_free(&session->entropy);
    return ESP_FAIL;
}

#define TEST_VER_STR "<some version string>"

static esp_err_t test_ver_endpoint(session_t *session)
{
    ssize_t  ver_data_len = 0;
    uint8_t *ver_data = NULL;

    esp_err_t ret = protocomm_req_handle(test_pc, "test-ver", session->id,
                                         NULL, 0, &ver_data, &ver_data_len);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "test-ver handler failed");
        return ESP_FAIL;
    }

    if (ver_data_len != strlen(TEST_VER_STR) || memcmp(TEST_VER_STR, ver_data, ver_data_len)) {
        ESP_LOGE(TAG, "incorrect response data from test-ver");
        free(ver_data);
        return ESP_FAIL;
    }
    free(ver_data);
    return ESP_OK;
}

static esp_err_t test_req_endpoint(session_t *session)
{
    uint32_t session_id = session->id;

    uint8_t rand_test_data[512], enc_test_data[512];
    getrandom(rand_test_data, sizeof(rand_test_data), 0);

    if (session->sec_ver == 0) {
        memcpy(enc_test_data, rand_test_data, sizeof(rand_test_data));
    }
    else if (session->sec_ver == 1) {
#if !CONFIG_MBEDTLS_HARDWARE_AES
        // Check if the AES key is correctly set before calling the software encryption
        // API. Without this check, the code will crash, resulting in a test case failure.
        // For hardware AES, portability layer takes care of this.
        if (session->ctx_aes.MBEDTLS_PRIVATE(nr) > 0) {
#endif

            mbedtls_aes_crypt_ctr(&session->ctx_aes, sizeof(rand_test_data), &session->nc_off,
                    session->rand, session->stb, rand_test_data, enc_test_data);
#if !CONFIG_MBEDTLS_HARDWARE_AES
        }
#endif
    }

    ssize_t  verify_data_len = 0;
    uint8_t *enc_verify_data = NULL;

    esp_err_t ret = protocomm_req_handle(test_pc, "test-ep", session_id,
                                         enc_test_data, sizeof(enc_test_data),
                                         &enc_verify_data, &verify_data_len);

    if (ret != ESP_OK || !verify_data_len) {
        ESP_LOGE(TAG, "test-ep handler failed");
        return ESP_FAIL;
    }

    uint8_t *verify_data = malloc(verify_data_len);
    if (!verify_data) {
        ESP_LOGE(TAG, "error allocating memory for decrypted data");
        free(enc_verify_data);
        return ESP_FAIL;
    }

    if (session->sec_ver == 0) {
        memcpy(verify_data, enc_verify_data, verify_data_len);
    }
    else if (session->sec_ver == 1) {
        mbedtls_aes_crypt_ctr(&session->ctx_aes, verify_data_len, &session->nc_off,
                              session->rand, session->stb, enc_verify_data, verify_data);
    }
    free(enc_verify_data);

    hexdump("Sent data", rand_test_data, sizeof(rand_test_data));
    hexdump("Recv data", verify_data,    verify_data_len);

    ESP_LOGI(TAG, "verify data len   : %d", verify_data_len);
    ESP_LOGI(TAG, "expected data len : %d", sizeof(rand_test_data));

    if (verify_data_len != sizeof(rand_test_data)) {
        ESP_LOGE(TAG, "incorrect response length from test-ep");
        free(verify_data);
        return ESP_FAIL;
    }
    if (memcmp(rand_test_data, verify_data, verify_data_len)) {
        ESP_LOGE(TAG, "incorrect response data from test-ep");
        free(verify_data);
        return ESP_FAIL;
    }
    free(verify_data);
    return ESP_OK;
}

esp_err_t test_req_handler (uint32_t session_id,
                            const uint8_t *inbuf, ssize_t inlen,
                            uint8_t **outbuf, ssize_t *outlen,
                            void *priv_data)
{
    *outbuf = malloc(inlen);
    if (*outbuf) {
        *outlen = inlen;
        memcpy(*outbuf, inbuf, inlen);
    } else {
        ESP_LOGE(TAG, "Error allocating response outbuf");
        *outbuf = NULL;
        *outlen = 0;
    }

    uint32_t *priv = (uint32_t *) priv_data;
    if ((&test_priv_data != priv) || (test_priv_data != *priv)) {
        ESP_LOGE(TAG, "Handler private data doesn't match");
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t start_test_service(uint8_t sec_ver, const protocomm_security1_params_t *pop)
{
    test_pc = protocomm_new();
    if (test_pc == NULL) {
        ESP_LOGE(TAG, "Failed to create new protocomm instance");
        return ESP_FAIL;
    }

    if (sec_ver == 0) {
        if (protocomm_set_security(test_pc, "test-sec", &protocomm_security0, NULL) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set Security0");
            return ESP_FAIL;
        }
        test_sec = &protocomm_security0;
    } else if (sec_ver == 1) {
        if (protocomm_set_security(test_pc, "test-sec", &protocomm_security1, pop) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set Security1");
            return ESP_FAIL;
        }
        test_sec = &protocomm_security1;
    }

    if (protocomm_set_version(test_pc, "test-ver", TEST_VER_STR) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set version");
        return ESP_FAIL;
    }

    if (protocomm_add_endpoint(test_pc, "test-ep",
                               test_req_handler,
                               (void *) &test_priv_data) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set test-ep endpoint handler");
        return ESP_FAIL;
    }
    return ESP_OK;
}

static void stop_test_service(void)
{
    test_sec = NULL;
    protocomm_delete(test_pc);
    test_pc = NULL;
}

static esp_err_t test_security1_no_encryption (void)
{
    ESP_LOGI(TAG, "Starting Security 1 no encryption test");

    const char *pop_data = "test pop";
    protocomm_security1_params_t pop = {
        .data = (const uint8_t *)pop_data,
        .len  = strlen(pop_data)
    };

    session_t *session = calloc(1, sizeof(session_t));
    if (session == NULL) {
        ESP_LOGE(TAG, "Error allocating session");
        return ESP_ERR_NO_MEM;
    }

    session->id        = 1;
    session->sec_ver   = 1;
    session->pop       = &pop;

    // Start protocomm service
    if (start_test_service(1, &pop) != ESP_OK) {
        ESP_LOGE(TAG, "Error starting test");
        free(session);
        return ESP_ERR_INVALID_STATE;
    }

    // Intialise protocomm session with zero public keys
    if (test_new_session(session) != ESP_OK) {
        ESP_LOGE(TAG, "Error creating new session");
        stop_test_service();
        free(session);
        return ESP_FAIL;
    }

    // Perform 25519 security handshake to set public keys
    if (test_sec_endpoint(session) != ESP_OK) {
        ESP_LOGE(TAG, "Error testing security endpoint");
        test_delete_session(session);
        stop_test_service();
        free(session);
        return ESP_FAIL;
    }

    // Force endpoint with un-encrypted data
    session->sec_ver = 0;

    // Send unencrypted request data to echo endpoint.
    // Response would be encrypted causing echoed back
    // data to not match that which was sent, hence failing.
    if (test_req_endpoint(session) == ESP_OK) {
        ESP_LOGE(TAG, "Error testing request endpoint");
        session->sec_ver = 1;
        test_delete_session(session);
        stop_test_service();
        free(session);
        return ESP_FAIL;
    }

    session->sec_ver = 1;
    test_delete_session(session);
    stop_test_service();
    free(session);
    ESP_LOGI(TAG, "Protocomm test successful");
    return ESP_OK;
}

static esp_err_t test_security1_session_overflow (void)
{
    ESP_LOGI(TAG, "Starting Security 1 session overflow test");

    const char *pop_data = "test pop";
    protocomm_security1_params_t pop = {
        .data = (const uint8_t *)pop_data,
        .len  = strlen(pop_data)
    };

    session_t *session1 = calloc(1, sizeof(session_t));
    if (session1 == NULL) {
        ESP_LOGE(TAG, "Error allocating session");
        return ESP_ERR_NO_MEM;
    }

    session1->id        = 2;
    session1->sec_ver   = 1;
    session1->pop       = &pop;

    session_t *session2 = calloc(1, sizeof(session_t));
    if (session2 == NULL) {
        ESP_LOGE(TAG, "Error allocating session");
        free(session1);
        return ESP_ERR_NO_MEM;
    }

    session2->id         = 3;
    session2->sec_ver    = 1;
    session2->pop        = NULL;

    // Start protocomm service
    if (start_test_service(1, &pop) != ESP_OK) {
        ESP_LOGE(TAG, "Error starting test");
        free(session1);
        free(session2);
        return ESP_FAIL;
    }

    // Intialise protocomm session with zero public keys
    if (test_new_session(session1) != ESP_OK) {
        ESP_LOGE(TAG, "Error creating new session");
        stop_test_service();
        free(session1);
        free(session2);
        return ESP_FAIL;
    }

    // Perform 25519 security handshake to set public keys
    if (test_sec_endpoint(session1) != ESP_OK) {
        ESP_LOGE(TAG, "Error testing security endpoint");
        test_delete_session(session1);
        stop_test_service();
        free(session1);
        free(session2);
        return ESP_FAIL;
    }

    // Try to perform security handshake again with different
    // session ID without registering new session, hence failing
    if (test_sec_endpoint(session2) == ESP_OK) {
        ESP_LOGE(TAG, "Error testing security endpoint");
        test_delete_session(session1);
        stop_test_service();
        free(session1);
        free(session2);
        return ESP_FAIL;
    }

    test_delete_session(session1);
    stop_test_service();
    free(session1);
    free(session2);

    ESP_LOGI(TAG, "Protocomm test successful");
    return ESP_OK;
}

static esp_err_t test_security1_wrong_pop (void)
{
    ESP_LOGI(TAG, "Starting Security 1 wrong auth test");

    const char *pop_data = "test pop";
    protocomm_security1_params_t pop = {
        .data = (const uint8_t *)pop_data,
        .len  = strlen(pop_data)
    };

    session_t *session = calloc(1, sizeof(session_t));
    if (session == NULL) {
        ESP_LOGE(TAG, "Error allocating session");
        return ESP_ERR_NO_MEM;
    }

    session->id        = 4;
    session->sec_ver   = 1;
    session->pop       = &pop;

    // Start protocomm service
    if (start_test_service(1, &pop) != ESP_OK) {
        ESP_LOGE(TAG, "Error starting test");
        free(session);
        return ESP_FAIL;
    }

    // Intialise protocomm session with zero public keys
    if (test_new_session(session) != ESP_OK) {
        ESP_LOGE(TAG, "Error creating new session");
        stop_test_service();
        free(session);
        return ESP_FAIL;
    }

    const char *wrong_pop_data = "wrong pop";
    protocomm_security1_params_t wrong_pop = {
        .data = (const uint8_t *)wrong_pop_data,
        .len  = strlen(wrong_pop_data)
    };

    // Force wrong pop during authentication
    session->pop = &wrong_pop;

    // Perform 25519 security handshake with
    // wrong pop, hence failing
    if (test_sec_endpoint(session) == ESP_OK) {
        ESP_LOGE(TAG, "Error testing security endpoint");
        test_delete_session(session);
        stop_test_service();
        free(session);
        return ESP_FAIL;
    }

    test_delete_session(session);
    stop_test_service();
    free(session);

    ESP_LOGI(TAG, "Protocomm test successful");
    return ESP_OK;
}

static esp_err_t test_security1_insecure_client (void)
{
    ESP_LOGI(TAG, "Starting Security 1 insecure client test");

    const char *pop_data = "test pop";
    protocomm_security1_params_t pop = {
        .data = (const uint8_t *)pop_data,
        .len  = strlen(pop_data)
    };

    session_t *session = calloc(1, sizeof(session_t));
    if (session == NULL) {
        ESP_LOGE(TAG, "Error allocating session");
        return ESP_ERR_NO_MEM;
    }

    session->id      = 5;
    session->sec_ver = 1;
    session->pop     = &pop;

    // Start protocomm service
    if (start_test_service(1, &pop) != ESP_OK) {
        ESP_LOGE(TAG, "Error starting test");
        free(session);
        return ESP_FAIL;
    }

    // Perform 25519 security handshake without
    // initialising session, hence failing
    if (test_sec_endpoint(session) == ESP_OK) {
        ESP_LOGE(TAG, "Error testing security endpoint");
        stop_test_service();
        free(session);
        return ESP_FAIL;
    }

    // Communicating with request endpoint without
    // initialising session, hence failing
    if (test_req_endpoint(session) == ESP_OK) {
        ESP_LOGE(TAG, "Error testing request endpoint");
        stop_test_service();
        free(session);
        return ESP_FAIL;
    }

    stop_test_service();
    free(session);

    ESP_LOGI(TAG, "Protocomm test successful");
    return ESP_OK;
}

static esp_err_t test_security1_weak_session (void)
{
    ESP_LOGI(TAG, "Starting Security 1 weak session test");

    const char *pop_data = "test pop";
    protocomm_security1_params_t pop = {
        .data = (const uint8_t *)pop_data,
        .len  = strlen(pop_data)
    };

    session_t *session = calloc(1, sizeof(session_t));
    if (session == NULL) {
        ESP_LOGE(TAG, "Error allocating session");
        return ESP_ERR_NO_MEM;
    }

    session->id        = 6;
    session->sec_ver   = 1;
    session->pop       = &pop;
    session->weak      = 1;

    // Start protocomm service
    if (start_test_service(1, &pop) != ESP_OK) {
        ESP_LOGE(TAG, "Error starting test");
        free(session);
        return ESP_FAIL;
    }

    // Intialise protocomm session with zero public keys
    if (test_new_session(session) != ESP_OK) {
        ESP_LOGE(TAG, "Error creating new session");
        stop_test_service();
        free(session);
        return ESP_FAIL;
    }

    // Perform 25519 security handshake with weak (zero)
    // client public key, hence failing
    if (test_sec_endpoint(session) == ESP_OK) {
        ESP_LOGE(TAG, "Error testing security endpoint");
        test_delete_session(session);
        stop_test_service();
        free(session);
        return ESP_FAIL;
    }

    // Sending request data to echo endpoint encrypted with zero
    // public keys on both client and server side should fail
    if (test_req_endpoint(session) == ESP_OK) {
        ESP_LOGE(TAG, "Error testing request endpoint");
        test_delete_session(session);
        stop_test_service();
        free(session);
        return ESP_FAIL;
    }

    test_delete_session(session);
    stop_test_service();
    free(session);

    ESP_LOGI(TAG, "Protocomm test successful");
    return ESP_OK;
}

static esp_err_t test_protocomm (session_t *session)
{
    ESP_LOGI(TAG, "Starting Protocomm test");

    // Start protocomm service
    if (start_test_service(session->sec_ver, session->pop) != ESP_OK) {
        ESP_LOGE(TAG, "Error starting test");
        return ESP_FAIL;
    }

    // Check version endpoint
    if (test_ver_endpoint(session) != ESP_OK) {
        ESP_LOGE(TAG, "Error testing version endpoint");
        stop_test_service();
        return ESP_FAIL;
    }

    // Intialise protocomm session with zero public keys
    if (test_new_session(session) != ESP_OK) {
        ESP_LOGE(TAG, "Error creating new session");
        stop_test_service();
        return ESP_FAIL;
    }

    // Perform 25519 security handshake to set public keys
    if (test_sec_endpoint(session) != ESP_OK) {
        ESP_LOGE(TAG, "Error testing security endpoint");
        test_delete_session(session);
        stop_test_service();
        return ESP_FAIL;
    }

    // Send request data to echo endpoint encrypted with
    // the set public keys on both client and server side
    if (test_req_endpoint(session) != ESP_OK) {
        ESP_LOGE(TAG, "Error testing request endpoint");
        test_delete_session(session);
        stop_test_service();
        return ESP_FAIL;
    }

    // Stop protocomm service
    test_delete_session(session);
    stop_test_service();
    ESP_LOGI(TAG, "Protocomm test successful");
    return ESP_OK;
}

static esp_err_t test_security1 (void)
{
    ESP_LOGI(TAG, "Starting Sec1 test");

    const char *pop_data = "test pop";
    protocomm_security1_params_t pop = {
        .data = (const uint8_t *)pop_data,
        .len  = strlen(pop_data)
    };

    session_t *session = calloc(1, sizeof(session_t));
    if (session == NULL) {
        ESP_LOGE(TAG, "Error allocating session");
        return ESP_ERR_NO_MEM;
    }

    session->id        = 7;
    session->sec_ver   = 1;
    session->pop       = &pop;

    if (test_protocomm (session) != ESP_OK) {
        ESP_LOGE(TAG, "Sec1 test failed");
        free(session);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Sec1 test successful");
    free(session);
    return ESP_OK;
}

static esp_err_t test_security0 (void)
{
    ESP_LOGI(TAG, "Starting Sec0 test");

    session_t *session = calloc(1, sizeof(session_t));
    if (session == NULL) {
        ESP_LOGE(TAG, "Error allocating session");
        return ESP_ERR_NO_MEM;
    }

    session->id        = 8;
    session->sec_ver   = 0;
    session->pop       = NULL;

    if (test_protocomm (session) != ESP_OK) {
        ESP_LOGE(TAG, "Sec0 test failed");
        free(session);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Sec0 test successful");
    free(session);
    return ESP_OK;
}

TEST_CASE("leak test", "[PROTOCOMM]")
{
#ifdef CONFIG_HEAP_TRACING
    heap_trace_init_standalone(trace_record, NUM_RECORDS);
    heap_trace_start(HEAP_TRACE_LEAKS);
#endif

    /* Run basic tests for the first time to allow for internal long
     * time allocations to happen (not related to protocomm) */
    test_security0();
    test_security1();
    usleep(1000);

#ifdef CONFIG_HEAP_TRACING
    heap_trace_stop();
    heap_trace_dump();
#endif

    /* Run all tests passively. Any leaks due
     * to protocomm should show  up now */
    unsigned pre_start_mem = esp_get_free_heap_size();

    test_security0();
    test_security1();
    test_security1_no_encryption();
    test_security1_session_overflow();
    test_security1_wrong_pop();
    test_security1_insecure_client();
    test_security1_weak_session();

    usleep(1000);

    unsigned post_stop_mem = esp_get_free_heap_size();

    if (pre_start_mem != post_stop_mem) {
        ESP_LOGE(TAG, "Mismatch in free heap size : %d bytes", post_stop_mem - pre_start_mem);
    }

    TEST_ASSERT(pre_start_mem == post_stop_mem);
}

TEST_CASE("security 0 basic test", "[PROTOCOMM]")
{
    TEST_ASSERT(test_security0() == ESP_OK);
}

TEST_CASE("security 1 basic test", "[PROTOCOMM]")
{
    TEST_ASSERT(test_security1() == ESP_OK);
}

TEST_CASE("security 1 no encryption test", "[PROTOCOMM]")
{
    TEST_ASSERT(test_security1_no_encryption() == ESP_OK);
}

TEST_CASE("security 1 session overflow test", "[PROTOCOMM]")
{
    TEST_ASSERT(test_security1_session_overflow() == ESP_OK);
}

TEST_CASE("security 1 wrong pop test", "[PROTOCOMM]")
{
    TEST_ASSERT(test_security1_wrong_pop() == ESP_OK);
}

TEST_CASE("security 1 insecure client test", "[PROTOCOMM]")
{
    TEST_ASSERT(test_security1_insecure_client() == ESP_OK);
}

TEST_CASE("security 1 weak session test", "[PROTOCOMM]")
{
    TEST_ASSERT(test_security1_weak_session() == ESP_OK);
}

void app_main(void)
{
    unity_run_menu();
}
