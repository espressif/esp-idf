/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <protocomm_security.h>
#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Function prototype for protocomm endpoint handler
 */
typedef esp_err_t (*protocomm_req_handler_t)(
    uint32_t        session_id, /*!< Session ID for identifying protocomm client */
    const uint8_t  *inbuf,      /*!< Pointer to user provided input data buffer */
    ssize_t         inlen,      /*!< Length o the input buffer */
    uint8_t       **outbuf,     /*!< Pointer to output buffer allocated by handler */
    ssize_t        *outlen,     /*!< Length of the allocated output buffer */
    void           *priv_data   /*!< Private data passed to the handler (NULL if not used) */
);

/**
 * @brief   This structure corresponds to a unique instance of protocomm
 *          returned when the API `protocomm_new()` is called. The remaining
 *          Protocomm APIs require this object as the first parameter.
 *
 * @note    Structure of the protocomm object is kept private
 */
typedef struct protocomm protocomm_t;

/**
 * @brief   Create a new protocomm instance
 *
 * This API will return a new dynamically allocated protocomm instance
 * with all elements of the protocomm_t structure initialized to NULL.
 *
 * @return
 *  - protocomm_t* : On success
 *  - NULL : No memory for allocating new instance
 */
protocomm_t *protocomm_new(void);

/**
 * @brief   Delete a protocomm instance
 *
 * This API will deallocate a protocomm instance that was created
 * using `protocomm_new()`.
 *
 * @param[in] pc    Pointer to the protocomm instance to be deleted
 */
void protocomm_delete(protocomm_t *pc);

/**
 * @brief   Add endpoint request handler for a protocomm instance
 *
 * This API will bind an endpoint handler function to the specified
 * endpoint name, along with any private data that needs to be pass to
 * the handler at the time of call.
 *
 * @note
 *  - An endpoint must be bound to a valid protocomm instance,
 *    created using `protocomm_new()`.
 *  - This function internally calls the registered `add_endpoint()`
 *    function of the selected transport which is a member of the
 *    protocomm_t instance structure.
 *
 * @param[in] pc        Pointer to the protocomm instance
 * @param[in] ep_name   Endpoint identifier(name) string
 * @param[in] h         Endpoint handler function
 * @param[in] priv_data Pointer to private data to be passed as a
 *                      parameter to the handler function on call.
 *                      Pass NULL if not needed.
 *
 * @return
 *  - ESP_OK : Success
 *  - ESP_FAIL : Error adding endpoint / Endpoint with this name already exists
 *  - ESP_ERR_NO_MEM : Error allocating endpoint resource
 *  - ESP_ERR_INVALID_ARG : Null instance/name/handler arguments
 */
esp_err_t protocomm_add_endpoint(protocomm_t *pc, const char *ep_name,
                                 protocomm_req_handler_t h, void *priv_data);

/**
 * @brief   Remove endpoint request handler for a protocomm instance
 *
 * This API will remove a registered endpoint handler identified by
 * an endpoint name.
 *
 * @note
 *  - This function internally calls the registered `remove_endpoint()`
 *    function which is a member of the protocomm_t instance structure.
 *
 * @param[in] pc        Pointer to the protocomm instance
 * @param[in] ep_name   Endpoint identifier(name) string
 *
 * @return
 *  - ESP_OK : Success
 *  - ESP_ERR_NOT_FOUND : Endpoint with specified name doesn't exist
 *  - ESP_ERR_INVALID_ARG : Null instance/name arguments
 */
esp_err_t protocomm_remove_endpoint(protocomm_t *pc, const char *ep_name);

/**
 * @brief   Allocates internal resources for new transport session
 *
 * @note
 *  - An endpoint must be bound to a valid protocomm instance,
 *    created using `protocomm_new()`.
 *
 * @param[in]  pc         Pointer to the protocomm instance
 * @param[in]  session_id Unique ID for a communication session
 *
 * @return
 *  - ESP_OK : Request handled successfully
 *  - ESP_ERR_NO_MEM : Error allocating internal resource
 *  - ESP_ERR_INVALID_ARG : Null instance/name arguments
 */
esp_err_t protocomm_open_session(protocomm_t *pc, uint32_t session_id);

/**
 * @brief   Frees internal resources used by a transport session
 *
 * @note
 *  - An endpoint must be bound to a valid protocomm instance,
 *    created using `protocomm_new()`.
 *
 * @param[in]  pc         Pointer to the protocomm instance
 * @param[in]  session_id Unique ID for a communication session
 *
 * @return
 *  - ESP_OK : Request handled successfully
 *  - ESP_ERR_INVALID_ARG : Null instance/name arguments
 */
esp_err_t protocomm_close_session(protocomm_t *pc, uint32_t session_id);

/**
 * @brief   Calls the registered handler of an endpoint session
 *          for processing incoming data and generating the response
 *
 * @note
 *  - An endpoint must be bound to a valid protocomm instance,
 *    created using `protocomm_new()`.
 *  - Resulting output buffer must be deallocated by the caller.
 *
 * @param[in]  pc         Pointer to the protocomm instance
 * @param[in]  ep_name    Endpoint identifier(name) string
 * @param[in]  session_id Unique ID for a communication session
 * @param[in]  inbuf      Input buffer contains input request data which is to be
 *                        processed by the registered handler
 * @param[in]  inlen      Length of the input buffer
 * @param[out] outbuf     Pointer to internally allocated output buffer,
 *                        where the resulting response data output from
 *                        the registered handler is to be stored
 * @param[out] outlen     Buffer length of the allocated output buffer
 *
 * @return
 *  - ESP_OK : Request handled successfully
 *  - ESP_FAIL : Internal error in execution of registered handler
 *  - ESP_ERR_NO_MEM : Error allocating internal resource
 *  - ESP_ERR_NOT_FOUND : Endpoint with specified name doesn't exist
 *  - ESP_ERR_INVALID_ARG : Null instance/name arguments
 */
esp_err_t protocomm_req_handle(protocomm_t *pc, const char *ep_name, uint32_t session_id,
                               const uint8_t *inbuf, ssize_t inlen,
                               uint8_t **outbuf, ssize_t *outlen);

/**
 * @brief   Add endpoint security for a protocomm instance
 *
 * This API will bind a security session establisher to the specified
 * endpoint name, along with any proof of possession that may be required
 * for authenticating a session client.
 *
 * @note
 *  - An endpoint must be bound to a valid protocomm instance,
 *    created using `protocomm_new()`.
 *  - The choice of security can be any `protocomm_security_t` instance.
 *    Choices `protocomm_security0` and `protocomm_security1` and `protocomm_security2` are readily available.
 *
 * @param[in] pc            Pointer to the protocomm instance
 * @param[in] ep_name       Endpoint identifier(name) string
 * @param[in] sec           Pointer to endpoint security instance
 * @param[in] sec_params    Pointer to security params (NULL if not needed)
 *                          The pointer should contain the security params struct
 *                          of appropriate security version.
 *                          For protocomm security version 1 and 2
 *                          sec_params should contain pointer to struct of type
 *                          protocomm_security1_params_t and protocmm_security2_params_t respectively.
 *                          The contents of this pointer must be valid till the security session
 *                          has been running and is not closed.
 * @return
 *  - ESP_OK : Success
 *  - ESP_FAIL : Error adding endpoint / Endpoint with this name already exists
 *  - ESP_ERR_INVALID_STATE : Security endpoint already set
 *  - ESP_ERR_NO_MEM : Error allocating endpoint resource
 *  - ESP_ERR_INVALID_ARG : Null instance/name/handler arguments
 */
esp_err_t protocomm_set_security(protocomm_t *pc, const char *ep_name,
                                 const protocomm_security_t *sec,
                                 const void *sec_params);
/**
 * @brief   Remove endpoint security for a protocomm instance
 *
 * This API will remove a registered security endpoint identified by
 * an endpoint name.
 *
 * @param[in] pc        Pointer to the protocomm instance
 * @param[in] ep_name   Endpoint identifier(name) string
 *
 * @return
 *  - ESP_OK : Success
 *  - ESP_ERR_NOT_FOUND : Endpoint with specified name doesn't exist
 *  - ESP_ERR_INVALID_ARG : Null instance/name arguments
 */
esp_err_t protocomm_unset_security(protocomm_t *pc, const char *ep_name);

/**
 * @brief   Set endpoint for version verification
 *
 * This API can be used for setting an application specific protocol
 * version which can be verified by clients through the endpoint.
 *
 * @note
 *  - An endpoint must be bound to a valid protocomm instance,
 *    created using `protocomm_new()`.

 * @param[in] pc        Pointer to the protocomm instance
 * @param[in] ep_name   Endpoint identifier(name) string
 * @param[in] version   Version identifier(name) string
 *
 * @return
 *  - ESP_OK : Success
 *  - ESP_FAIL : Error adding endpoint / Endpoint with this name already exists
 *  - ESP_ERR_INVALID_STATE : Version endpoint already set
 *  - ESP_ERR_NO_MEM : Error allocating endpoint resource
 *  - ESP_ERR_INVALID_ARG : Null instance/name/handler arguments
 */
esp_err_t protocomm_set_version(protocomm_t *pc, const char *ep_name,
                                const char *version);

/**
 * @brief   Remove version verification endpoint from a protocomm instance
 *
 * This API will remove a registered version endpoint identified by
 * an endpoint name.
 *
 * @param[in] pc        Pointer to the protocomm instance
 * @param[in] ep_name   Endpoint identifier(name) string
 *
 * @return
 *  - ESP_OK : Success
 *  - ESP_ERR_NOT_FOUND : Endpoint with specified name doesn't exist
 *  - ESP_ERR_INVALID_ARG : Null instance/name arguments
 */
esp_err_t protocomm_unset_version(protocomm_t *pc, const char *ep_name);

#ifdef __cplusplus
}
#endif
