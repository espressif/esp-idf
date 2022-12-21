/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include <esp_err.h>
#include <esp_log.h>
#include <sys/queue.h>

#include <protocomm.h>
#include <protocomm_security.h>

#include "protocomm_priv.h"

static const char *TAG = "protocomm";

protocomm_t *protocomm_new(void)
{
    protocomm_t *pc;

    pc = (protocomm_t *) calloc(1, sizeof(protocomm_t));
    if (!pc) {
       ESP_LOGE(TAG, "Error allocating protocomm");
       return NULL;
    }
    SLIST_INIT(&pc->endpoints);

    return pc;
}

void protocomm_delete(protocomm_t *pc)
{
    if (pc == NULL) {
        return;
    }

    protocomm_ep_t *it, *tmp;
    /* Remove endpoints first */
    SLIST_FOREACH_SAFE(it, &pc->endpoints, next, tmp) {
        free(it);
    }

    /* Free memory allocated to version string */
    if (pc->ver) {
        free((void *)pc->ver);
    }

    /* Free memory allocated to security */
    if (pc->sec && pc->sec->cleanup) {
        pc->sec->cleanup(pc->sec_inst);
    }
    if (pc->sec_params) {
        free(pc->sec_params);
    }
    free(pc);
}

static protocomm_ep_t *search_endpoint(protocomm_t *pc, const char *ep_name)
{
    protocomm_ep_t *it;
    SLIST_FOREACH(it, &pc->endpoints, next) {
        if (strcmp(it->ep_name, ep_name) == 0) {
            return it;
        }
    }
    return NULL;
}

static esp_err_t protocomm_add_endpoint_internal(protocomm_t *pc, const char *ep_name,
                                                 protocomm_req_handler_t h, void *priv_data,
                                                 uint32_t flag)
{
    if ((pc == NULL) || (ep_name == NULL) || (h == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }

    protocomm_ep_t *ep;
    esp_err_t ret;

    ep = search_endpoint(pc, ep_name);
    if (ep) {
        ESP_LOGE(TAG, "Endpoint with this name already exists");
        return ESP_FAIL;
    }

    if (pc->add_endpoint) {
        ret = pc->add_endpoint(ep_name, h, priv_data);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Error adding endpoint");
            return ret;
        }
    }

    ep = (protocomm_ep_t *) calloc(1, sizeof(protocomm_ep_t));
    if (!ep) {
        ESP_LOGE(TAG, "Error allocating endpoint resource");
        return ESP_ERR_NO_MEM;
    }

    /* Initialize ep handler */
    ep->ep_name = ep_name;
    ep->req_handler = h;
    ep->priv_data = priv_data;
    ep->flag = flag;

    /* Add endpoint to the head of the singly linked list */
    SLIST_INSERT_HEAD(&pc->endpoints, ep, next);

    return ESP_OK;
}

esp_err_t protocomm_add_endpoint(protocomm_t *pc, const char *ep_name,
                                 protocomm_req_handler_t h, void *priv_data)
{
    return protocomm_add_endpoint_internal(pc, ep_name, h, priv_data, REQ_EP);
}

esp_err_t protocomm_remove_endpoint(protocomm_t *pc, const char *ep_name)
{
    if ((pc == NULL) || (ep_name == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }

    if (pc->remove_endpoint) {
        pc->remove_endpoint(ep_name);
    }

    protocomm_ep_t *it, *tmp;
    SLIST_FOREACH_SAFE(it, &pc->endpoints, next, tmp) {
        if (strcmp(ep_name, it->ep_name) == 0) {
            SLIST_REMOVE(&pc->endpoints, it, protocomm_ep, next);
            free(it);
            return ESP_OK;
        }
    }
    return ESP_ERR_NOT_FOUND;
}

esp_err_t protocomm_open_session(protocomm_t *pc, uint32_t session_id)
{
    if (!pc) {
        return ESP_ERR_INVALID_ARG;
    }

    if (pc->sec && pc->sec->new_transport_session) {
        esp_err_t ret = pc->sec->new_transport_session(pc->sec_inst, session_id);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to launch new session with ID: %" PRId32, session_id);
            return ret;
        }
    }
    return ESP_OK;
}

esp_err_t protocomm_close_session(protocomm_t *pc, uint32_t session_id)
{
    if (!pc) {
        return ESP_ERR_INVALID_ARG;
    }

    if (pc->sec && pc->sec->close_transport_session) {
        esp_err_t ret = pc->sec->close_transport_session(pc->sec_inst, session_id);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Error while closing session with ID: %" PRId32, session_id);
            return ret;
        }
    }
    return ESP_OK;
}

esp_err_t protocomm_req_handle(protocomm_t *pc, const char *ep_name, uint32_t session_id,
                               const uint8_t *inbuf, ssize_t inlen,
                               uint8_t **outbuf, ssize_t *outlen)
{
    if (!pc || !ep_name || !outbuf || !outlen) {
        ESP_LOGE(TAG, "Invalid params %p %p", pc, ep_name);
        return ESP_ERR_INVALID_ARG;
    }

    *outbuf = NULL;
    *outlen = 0;

    protocomm_ep_t *ep = search_endpoint(pc, ep_name);
    if (!ep) {
        ESP_LOGE(TAG, "No registered endpoint for %s", ep_name);
        return ESP_ERR_NOT_FOUND;
    }

    esp_err_t ret = ESP_FAIL;
    if (ep->flag & SEC_EP) {
        /* Call the registered endpoint handler for establishing secure session */
        ret = ep->req_handler(session_id, inbuf, inlen, outbuf, outlen, ep->priv_data);
        ESP_LOGD(TAG, "SEC_EP Req handler returned %d", ret);
    } else if (ep->flag & REQ_EP) {
        if (pc->sec && pc->sec->decrypt) {
            /* Decrypt the data first */
            ssize_t dec_inbuf_len = 0;
            uint8_t *dec_inbuf = NULL;
            ret = pc->sec->decrypt(pc->sec_inst, session_id, inbuf, inlen, &dec_inbuf, &dec_inbuf_len);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Decryption of response failed for endpoint %s", ep_name);
                free(dec_inbuf);
                return ret;
            }

            /* Invoke the request handler */
            uint8_t *plaintext_resp = NULL;
            ssize_t plaintext_resp_len = 0;
            ret = ep->req_handler(session_id,
                                  dec_inbuf, dec_inbuf_len,
                                  &plaintext_resp, &plaintext_resp_len,
                                  ep->priv_data);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Request handler for %s failed", ep_name);
                free(plaintext_resp);
                free(dec_inbuf);
                return ret;
            }
            /* We don't need decrypted data anymore */
            free(dec_inbuf);

            uint8_t *enc_resp = NULL;
            ssize_t enc_resp_len = 0;
            ret = pc->sec->encrypt(pc->sec_inst, session_id, plaintext_resp, plaintext_resp_len,
                                   &enc_resp, &enc_resp_len);

            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Encryption of response failed for endpoint %s", ep_name);
                free(enc_resp);
                free(plaintext_resp);
                return ret;
            }

            /* We no more need plaintext response */
            free(plaintext_resp);

            /* Set outbuf and outlen appropriately */
            *outbuf = enc_resp;
            *outlen = enc_resp_len;
        } else {
            /* No encryption */
            ret = ep->req_handler(session_id,
                                  inbuf, inlen,
                                  outbuf, outlen,
                                  ep->priv_data);
            ESP_LOGD(TAG, "No encrypt ret %d", ret);
        }
    } else if (ep->flag & VER_EP) {
        ret = ep->req_handler(session_id, inbuf, inlen, outbuf, outlen, ep->priv_data);
        ESP_LOGD(TAG, "VER_EP Req handler returned %d", ret);
    }
    return ret;
}

static int protocomm_common_security_handler(uint32_t session_id,
                                             const uint8_t *inbuf, ssize_t inlen,
                                             uint8_t **outbuf, ssize_t *outlen,
                                             void *priv_data)
{
    protocomm_t *pc = (protocomm_t *) priv_data;

    if (pc->sec && pc->sec->security_req_handler) {
        return pc->sec->security_req_handler(pc->sec_inst,
                                             pc->sec_params, session_id,
                                             inbuf, inlen,
                                             outbuf, outlen,
                                             priv_data);
    }

    return ESP_OK;
}

esp_err_t protocomm_set_security(protocomm_t *pc, const char *ep_name,
                                 const protocomm_security_t *sec,
                                 const void *sec_params)
{
    if ((pc == NULL) || (ep_name == NULL) || (sec == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }

    if (pc->sec) {
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t ret = protocomm_add_endpoint_internal(pc, ep_name,
                                                    protocomm_common_security_handler,
                                                    (void *) pc, SEC_EP);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error adding security endpoint");
        return ret;
    }

    if (sec->init) {
        ret = sec->init(&pc->sec_inst);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Error initializing security");
            protocomm_remove_endpoint(pc, ep_name);
            return ret;
        }
    }
    pc->sec = sec;

    /* sec params is not needed and thus checked in case of security 0 */
    if (pc->sec->ver == 1) {
        if (sec_params) {
            pc->sec_params = calloc(1, sizeof(protocomm_security1_params_t));
            if (pc->sec_params == NULL) {
                ESP_LOGE(TAG, "Error allocating memory for security1 params");
                ret = ESP_ERR_NO_MEM;
                goto cleanup;
            }
            memcpy((void *)pc->sec_params, sec_params, sizeof(protocomm_security1_params_t));
        }
    } else if (pc->sec->ver == 2) {
        if (sec_params) {
            pc->sec_params = calloc(1, sizeof(protocomm_security2_params_t));
            if (pc->sec_params == NULL) {
                ESP_LOGE(TAG, "Error allocating memory for security2 params");
                ret = ESP_ERR_NO_MEM;
                goto cleanup;
            }
            memcpy((void *)pc->sec_params, sec_params, sizeof(protocomm_security2_params_t));
        } else {
            ESP_LOGE(TAG, "Security params cannot be null");
            ret = ESP_ERR_INVALID_ARG;
            goto cleanup;
        }
    }
    return ESP_OK;

cleanup:
    if (pc->sec && pc->sec->cleanup) {
            pc->sec->cleanup(pc->sec_inst);
            pc->sec_inst = NULL;
            pc->sec = NULL;
        }
    protocomm_remove_endpoint(pc, ep_name);
    return ret;
}

esp_err_t protocomm_unset_security(protocomm_t *pc, const char *ep_name)
{
    if ((pc == NULL) || (ep_name == NULL)) {
        return ESP_FAIL;
    }

    if (pc->sec && pc->sec->cleanup) {
        pc->sec->cleanup(pc->sec_inst);
        pc->sec_inst = NULL;
        pc->sec = NULL;
    }

    if (pc->sec_params) {
        free(pc->sec_params);
        pc->sec_params = NULL;
    }

    return protocomm_remove_endpoint(pc, ep_name);
}

static int protocomm_version_handler(uint32_t session_id,
                                     const uint8_t *inbuf, ssize_t inlen,
                                     uint8_t **outbuf, ssize_t *outlen,
                                     void *priv_data)
{
    protocomm_t *pc = (protocomm_t *) priv_data;
    if (!pc->ver) {
        *outlen = 0;
        *outbuf = NULL;
        return ESP_OK;
    }

    /* Output is a non null terminated string with length specified */
    *outlen = strlen(pc->ver);
    *outbuf = malloc(*outlen);
    if (*outbuf == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for version response");
        return ESP_ERR_NO_MEM;
    }

    memcpy(*outbuf, pc->ver, *outlen);
    return ESP_OK;
}

esp_err_t protocomm_set_version(protocomm_t *pc, const char *ep_name, const char *version)
{
    if ((pc == NULL) || (ep_name == NULL) || (version == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }

    if (pc->ver) {
        return ESP_ERR_INVALID_STATE;
    }

    pc->ver = strdup(version);
    if (pc->ver == NULL) {
        ESP_LOGE(TAG, "Error allocating version string");
        return ESP_ERR_NO_MEM;
    }

    esp_err_t ret = protocomm_add_endpoint_internal(pc, ep_name,
                                                    protocomm_version_handler,
                                                    (void *) pc, VER_EP);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error adding version endpoint");
        return ret;
    }
    return ESP_OK;
}

esp_err_t protocomm_unset_version(protocomm_t *pc, const char *ep_name)
{
    if ((pc == NULL) || (ep_name == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }

    if (pc->ver) {
        free((char *)pc->ver);
        pc->ver = NULL;
    }

    return protocomm_remove_endpoint(pc, ep_name);
}
