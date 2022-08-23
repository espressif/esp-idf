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

#include <protocomm_security.h>
#include <protocomm_security0.h>

#include "session.pb-c.h"
#include "sec0.pb-c.h"
#include "constants.pb-c.h"

static const char* TAG = "security0";

static esp_err_t sec0_session_setup(uint32_t session_id,
                                    SessionData *req, SessionData *resp)
{
    Sec0Payload *out = (Sec0Payload *) malloc(sizeof(Sec0Payload));
    S0SessionResp *s0resp = (S0SessionResp *) malloc(sizeof(S0SessionResp));
    if (!out || !s0resp) {
        ESP_LOGE(TAG, "Error allocating response");
        free(out);
        free(s0resp);
        return ESP_ERR_NO_MEM;
    }
    sec0_payload__init(out);
    s0_session_resp__init(s0resp);
    s0resp->status = STATUS__Success;

    out->msg = SEC0_MSG_TYPE__S0_Session_Response;
    out->payload_case = SEC0_PAYLOAD__PAYLOAD_SR;
    out->sr = s0resp;

    resp->proto_case = SESSION_DATA__PROTO_SEC0;
    resp->sec0 = out;

    return ESP_OK;
}

static void sec0_session_setup_cleanup(uint32_t session_id, SessionData *resp)
{
    if (!resp) {
        return;
    }

    free(resp->sec0->sr);
    free(resp->sec0);
    return;
}

static esp_err_t sec0_req_handler(protocomm_security_handle_t handle,
                                  const void *sec_params,
                                  uint32_t session_id,
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
    if (req->sec_ver != protocomm_security0.ver) {
        ESP_LOGE(TAG, "Security version mismatch. Closing connection");
        session_data__free_unpacked(req, NULL);
        return ESP_ERR_INVALID_ARG;
    }

    session_data__init(&resp);
    ret = sec0_session_setup(session_id, req, &resp);
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

    sec0_session_setup_cleanup(session_id, &resp);
    return ESP_OK;
}

const protocomm_security_t protocomm_security0 = {
    .ver = 0,
    .security_req_handler = sec0_req_handler,
};
