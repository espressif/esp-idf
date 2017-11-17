// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
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
#ifndef __ESP_EXAMPLE_SH2_LIB_H_
#define __ESP_EXAMPLE_SH2_LIB_H_

#include <openssl/ssl.h>
#include <nghttp2/nghttp2.h>

/*
 * This is a thin API wrapper over nghttp2 that offers simplified APIs for usage
 * in the application. The intention of this wrapper is to act as a stepping
 * stone to quickly get started with using the HTTP/2 client. Since the focus is
 * on simplicity, not all the features of HTTP/2 are supported through this
 * wrapper. Once you are fairly comfortable with nghttp2, feel free to directly
 * use nghttp2 APIs or make changes to sh2lib.c for realising your objectives.
 *
 * TODO:
 * - Allowing to query response code, content-type etc in the receive callback
 * - A simple function for per-stream header callback
 */
/**
 * @brief Handle for working with sh2lib APIs
 */
struct sh2lib_handle {
    /* Ideally, CTX is per-program, so we could potentially take it out of this
     * per-connection structure
     */
    SSL_CTX         *ssl_ctx;      /*!< Pointer to the SSL context */
    SSL             *ssl;          /*!< Pointer to the SSL handle */
    nghttp2_session *http2_sess;   /*!< Pointer to the HTTP2 session handle */
    int             sockfd;        /*!< Socket file descriptor */
};

/** Flag indicating receive stream is reset */
#define DATA_RECV_RST_STREAM      1
/** Flag indicating frame is completely received  */
#define DATA_RECV_FRAME_COMPLETE  2

/**
 * @brief Function Prototype for data receive callback
 *
 * This function gets called whenever data is received on any stream. The
 * function is also called for indicating events like frame receive complete, or
 * end of stream. The function may get called multiple times as long as data is
 * received on the stream.
 *
 * @param[in] handle     Pointer to the sh2lib handle.
 * @param[in] data       Pointer to a buffer that contains the data received.
 * @param[in] len        The length of valid data stored at the 'data' pointer.
 * @param[in] flags      Flags indicating whether the stream is reset (DATA_RECV_RST_STREAM) or
 *                       this particularly frame is completely received
 *                       DATA_RECV_FRAME_COMPLETE).
 *
 * @return The function should return 0
 */
typedef int (*sh2lib_frame_data_recv_cb_t)(struct sh2lib_handle *handle, const char *data, size_t len, int flags);

/**
 * @brief Function Prototype for callback to send data in PUT/POST
 *
 * This function gets called whenever nghttp2 wishes to send data, like for
 * PUT/POST requests to the server. The function keeps getting called until this
 * function sets the flag NGHTTP2_DATA_FLAG_EOF to indicate end of data.
 *
 * @param[in] handle       Pointer to the sh2lib handle.
 * @param[out] data        Pointer to a buffer that should contain the data to send.
 * @param[in] len          The maximum length of data that can be sent out by this function.
 * @param[out] data_flags  Pointer to the data flags. The NGHTTP2_DATA_FLAG_EOF
 *                         should be set in the data flags to indicate end of new data.
 *
 * @return The function should return the number of valid bytes stored in the
 * data pointer
 */
typedef int (*sh2lib_putpost_data_cb_t)(struct sh2lib_handle *handle, char *data, size_t len, uint32_t *data_flags);

/**
 * @brief Connect to a URI using HTTP/2
 *
 * This API opens an HTTP/2 connection with the provided URI. If successful, the
 * hd pointer is populated with a valid handle for subsequent communication.
 *
 * Only 'https' URIs are supported.
 *
 * @param[out] hd      Pointer to a variable of the type 'struct sh2lib_handle'.
 * @param[in]  uri      Pointer to the URI that should be connected to.
 *
 * @return
 *             - ESP_OK if the connection was successful
 *             - ESP_FAIL if the connection fails
 */
int sh2lib_connect(struct sh2lib_handle *hd, const char *uri);

/**
 * @brief Free a sh2lib handle
 *
 * This API frees-up an sh2lib handle, thus closing any open connections that
 * may be associated with this handle, and freeing up any resources.
 *
 * @param[in] hd      Pointer to a variable of the type 'struct sh2lib_handle'.
 *
 */
void sh2lib_free(struct sh2lib_handle *hd);

/**
 * @brief Setup an HTTP GET request stream
 *
 * This API sets up an HTTP GET request to be sent out to the server. A new
 * stream is created for handling the request. Once the request is setup, the
 * API sh2lib_execute() must be called to actually perform the socket I/O with
 * the server.
 *
 * @param[in] hd        Pointer to a variable of the type 'struct sh2lib_handle'.
 * @param[in] path      Pointer to the string that contains the resource to
 *                      perform the HTTP GET operation on (for example, /users).
 * @param[in] recv_cb   The callback function that should be called for
 *                      processing the request's response
 *
 * @return
 *             - ESP_OK if request setup is successful
 *             - ESP_FAIL if the request setup fails
 */
int sh2lib_do_get(struct sh2lib_handle *hd, const char *path, sh2lib_frame_data_recv_cb_t recv_cb);

/**
 * @brief Setup an HTTP POST request stream
 *
 * This API sets up an HTTP POST request to be sent out to the server. A new
 * stream is created for handling the request. Once the request is setup, the
 * API sh2lib_execute() must be called to actually perform the socket I/O with
 * the server.
 *
 * @param[in] hd        Pointer to a variable of the type 'struct sh2lib_handle'.
 * @param[in] path      Pointer to the string that contains the resource to
 *                      perform the HTTP POST operation on (for example, /users).
 * @param[in] send_cb   The callback function that should be called for
 *                      sending data as part of this request.
 * @param[in] recv_cb   The callback function that should be called for
 *                      processing the request's response
 *
 * @return
 *             - ESP_OK if request setup is successful
 *             - ESP_FAIL if the request setup fails
 */
int sh2lib_do_post(struct sh2lib_handle *hd, const char *path,
                   sh2lib_putpost_data_cb_t send_cb,
                   sh2lib_frame_data_recv_cb_t recv_cb);

/**
 * @brief Setup an HTTP PUT request stream
 *
 * This API sets up an HTTP PUT request to be sent out to the server. A new
 * stream is created for handling the request. Once the request is setup, the
 * API sh2lib_execute() must be called to actually perform the socket I/O with
 * the server.
 *
 * @param[in] hd        Pointer to a variable of the type 'struct sh2lib_handle'.
 * @param[in] path      Pointer to the string that contains the resource to
 *                      perform the HTTP PUT operation on (for example, /users).
 * @param[in] send_cb   The callback function that should be called for
 *                      sending data as part of this request.
 * @param[in] recv_cb   The callback function that should be called for
 *                      processing the request's response
 *
 * @return
 *             - ESP_OK if request setup is successful
 *             - ESP_FAIL if the request setup fails
 */
int sh2lib_do_put(struct sh2lib_handle *hd, const char *path,
                  sh2lib_putpost_data_cb_t send_cb,
                  sh2lib_frame_data_recv_cb_t recv_cb);

/**
 * @brief Execute send/receive on an HTTP/2 connection
 *
 * While the API sh2lib_do_get(), sh2lib_do_post() setup the requests to be
 * initiated with the server, this API performs the actual data send/receive
 * operations on the HTTP/2 connection. The callback functions are accordingly
 * called during the processing of these requests.
 *
 * @param[in] hd      Pointer to a variable of the type 'struct sh2lib_handle'
 *
 * @return
 *             - ESP_OK if the connection was successful
 *             - ESP_FAIL if the connection fails
 */
int sh2lib_execute(struct sh2lib_handle *hd);

#define SH2LIB_MAKE_NV(NAME, VALUE)                                    \
  {                                                                    \
    (uint8_t *)NAME, (uint8_t *)VALUE, strlen(NAME), strlen(VALUE),    \
        NGHTTP2_NV_FLAG_NONE                                           \
  }

/**
 * @brief Setup an HTTP GET request stream with custom name-value pairs
 *
 * For a simpler version of the API, please refer to sh2lib_do_get().
 *
 * This API sets up an HTTP GET request to be sent out to the server. A new
 * stream is created for handling the request. Once the request is setup, the
 * API sh2lib_execute() must be called to actually perform the socket I/O with
 * the server.
 *
 * Please note that the following name value pairs MUST be a part of the request
 *     -  name:value
 *     -  ":method":"GET"
 *     -  ":scheme":"https"
 *     -  ":path":<the-path-for-the-GET-operation>  (for example, /users)
 *
 * @param[in] hd        Pointer to a variable of the type 'struct sh2lib_handle'.
 * @param[in] nva       An array of name-value pairs that should be part of the request.
 * @param[in] nvlen     The number of elements in the array pointed to by 'nva'.
 * @param[in] recv_cb   The callback function that should be called for
 *                      processing the request's response
 *
 * @return
 *             - ESP_OK if request setup is successful
 *             - ESP_FAIL if the request setup fails
 */
int sh2lib_do_get_with_nv(struct sh2lib_handle *hd, const nghttp2_nv *nva, size_t nvlen, sh2lib_frame_data_recv_cb_t recv_cb);

/**
 * @brief Setup an HTTP PUT/POST request stream with custom name-value pairs
 *
 * For a simpler version of the API, please refer to sh2lib_do_put() or
 * sh2lib_do_post().
 *
 * This API sets up an HTTP PUT/POST request to be sent out to the server. A new
 * stream is created for handling the request. Once the request is setup, the
 * API sh2lib_execute() must be called to actually perform the socket I/O with
 * the server.
 *
 * Please note that the following name value pairs MUST be a part of the request
 *     -  name:value
 *     -  ":method":"PUT" (or POST)
 *     -  ":scheme":"https"
 *     -  ":path":<the-path-for-the-PUT-operation>  (for example, /users)
 *
 * @param[in] hd        Pointer to a variable of the type 'struct sh2lib_handle'.
 * @param[in] nva       An array of name-value pairs that should be part of the request.
 * @param[in] nvlen     The number of elements in the array pointed to by 'nva'.
 * @param[in] send_cb   The callback function that should be called for
 *                      sending data as part of this request.
 * @param[in] recv_cb   The callback function that should be called for
 *                      processing the request's response
 *
 * @return
 *             - ESP_OK if request setup is successful
 *             - ESP_FAIL if the request setup fails
 */
int sh2lib_do_putpost_with_nv(struct sh2lib_handle *hd, const nghttp2_nv *nva, size_t nvlen,
                              sh2lib_putpost_data_cb_t send_cb,
                              sh2lib_frame_data_recv_cb_t recv_cb);

#endif /* ! __ESP_EXAMPLE_SH2_LIB_H_ */
