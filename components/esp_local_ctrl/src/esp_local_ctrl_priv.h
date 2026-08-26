// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <esp_err.h>
#include <protocomm.h>
#include <esp_local_ctrl.h>

/**
 * @brief   `esp_local_ctrl` transport specific structure
 *
 * Every supported transport layer should have the following functions
 * implemented for starting, stopping and configuring a protocomm service
 */
struct esp_local_ctrl_transport {
    /**
     * Handler for starting a protocomm service as per specified configuration
     */
    esp_err_t (*start_service) (protocomm_t *pc,
                                const esp_local_ctrl_transport_config_t *config);

    /**
     * Handler for stopping a protocomm service
     */
    void (*stop_service) (protocomm_t *pc);

    /**
     * Handler for creating a copy of the transport specific configuration
     */
    esp_err_t (*copy_config) (esp_local_ctrl_transport_config_t *dest_config,
                              const esp_local_ctrl_transport_config_t *src_config);

    /**
     * Handler for allocating resources corresponding to a protocomm endpoint.
     * Usually when adding a new endpoint `protocomm_endpoint_add()` API is used,
     * but the transport layer may need to perform resource allocation for
     * each endpoint, prior to starting the protocomm instance. This handler
     * is useful in that case, as it is called before `start_service()`.
     */
    esp_err_t (*declare_ep) (esp_local_ctrl_transport_config_t *config,
                             const char *ep_name, uint16_t ep_uuid);

    /**
     * Handler for freeing a transport specific configuration
     */
    void (*free_config) (esp_local_ctrl_transport_config_t *config);
};

/**
 * @brief   Protocomm handler for `esp_local_ctrl`
 *
 * This is the handler which is responsible for processing incoming requests
 * over a protocomm channel, then invokes one of the following functions
 * depending upon the request type:
 * - `esp_local_ctrl_get_prop_count()`
 * - `esp_local_ctrl_get_prop_values()`
 * -` esp_local_ctrl_set_prop_values()`
 * The output of the above functions are used to form the response messages
 * corresponding to request types. The formed response messages are packed and
 * sent back via the protocomm channel.
 *
 * @param[in]  session_id   A number to identify an ongoing session between
 *                          device and client
 * @param[in]  inbuf        Buffer which holds serialized / packed request data
 * @param[in]  inlen        Length of input buffer
 * @param[out] outbuf       Buffer which holds serialized / packed response data
 * @param[out] outlen       Length of output buffer
 * @param[in]  priv_data    Private data associated with `esp_local_ctrl` endpoint
 *
 * @return
 *  - ESP_OK      : Success
 *  - ESP_FAIL    : Failure
 */
esp_err_t esp_local_ctrl_data_handler(uint32_t session_id, const uint8_t *inbuf, ssize_t inlen,
                                      uint8_t **outbuf, ssize_t *outlen, void *priv_data);

/**
 * @brief   Use this for obtaining total number of properties registered
 *          with `esp_local_ctrl` service
 *
 * @param[out] count   Pointer to variable where the result is to be stored
 *
 * @return
 *  - ESP_OK      : Success
 *  - ESP_FAIL    : Failure
 */
esp_err_t esp_local_ctrl_get_prop_count(size_t *count);

/**
 * @brief   Get descriptions and values of multiple properties at the same time.
 *          The properties are requested by indices. This internally calls the
 *          `get_prop_values` handler specified in the `esp_local_ctrl_handlers_t`
 *          structure. Since `get_prop_values` accepts property structure, the
 *          indices are first converted to the corresponding `esp_local_ctrl_prop_t`
 *          internally.
 *
 * @param[in]  total_indices   The number of elements in the `indices` array argument
 * @param[in]  indices         An array of indices, that specify which properties to get
 * @param[out] props           A pre-allocated array of empty property structures, elements of
 *                             which are to be populated with names, types and flags of those
 *                             properties which correspond to the provided indices
 * @param[out] values          A pre-allocated array of empty value structures, elements of
 *                             which are to be populated with values and sizes of those
 *                             properties which correspond to the provided indices
 *
 * @return
 *  - ESP_OK      : Success
 *  - ESP_FAIL    : Failure
 */
esp_err_t esp_local_ctrl_get_prop_values(size_t total_indices, uint32_t *indices,
                                         esp_local_ctrl_prop_t *props,
                                         esp_local_ctrl_prop_val_t *values);

/**
 * @brief   Set values of multiple properties at the same time. The properties to
 *          set are specified by indices. This internally calls the `set_prop_values`
 *          handler specified in the `esp_local_ctrl_handlers_t` structure. Since
 *          `set_prop_values` accepts property structures, the indices are first
 *          converted to the corresponding `esp_local_ctrl_prop_t` internally.
 *
 * @param[in] total_indices   The number of elements in the `indices` array argument
 * @param[in] indices         An array of indices, that specify which properties to set
 * @param[in] values          A array of values. Every value should have the correct
 *                            size, if it is for setting a fixed size property, else
 *                            error will be generated and none of the properties will
 *                            be set to any of the given values
 *
 * @return
 *  - ESP_OK      : Success
 *  - ESP_FAIL    : Failure
 */
esp_err_t esp_local_ctrl_set_prop_values(size_t total_indices, uint32_t *indices,
                                         const esp_local_ctrl_prop_val_t *values);

#ifdef __cplusplus
}
#endif
