/******************************************************************************
 *
 *  Copyright (C) 2014 Google, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#ifndef _L2CAP_CLIENT_H_
#define _L2CAP_CLIENT_H_
#if (defined(L2CAP_CLIENT_INCLUDED) && L2CAP_CLIENT_INCLUDED == TRUE)

//#include <hardware/bluetooth.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct buffer_t buffer_t;
typedef struct l2cap_client_t l2cap_client_t;

typedef struct {
    void (*connected)(l2cap_client_t *client, void *context);
    void (*disconnected)(l2cap_client_t *client, void *context);
    void (*read_ready)(l2cap_client_t *client, buffer_t *packet, void *context);
    void (*write_ready)(l2cap_client_t *client, void *context);
} l2cap_client_callbacks_t;

// Returns a new buffer with enough space for |size| bytes of L2CAP payload.
// |size| must be greater than zero. This function returns NULL if the buffer
// could not be allocated. The returned buffer must be freed with |buffer_free|
// when it is no longer needed.
buffer_t *l2cap_buffer_new(size_t size);

// Creates and returns a new L2CAP client object. |callbacks| must not be NULL and
// must specify a set of functions that should be called back when events occur
// on the L2CAP connection. |context| may be NULL and will be passed as the argument
// to all callbacks in |l2cap_client_callbacks_t|. The returned object must be freed
// with |l2cap_client_free|.
l2cap_client_t *l2cap_client_new(const l2cap_client_callbacks_t *callbacks, void *context);

// Frees the L2CAP client object allocated with |l2cap_client_new|. |client| may be NULL.
void l2cap_client_free(l2cap_client_t *client);

// Attempts to connect the |client| to a peer device specified by |remote_bdaddr|
// using the |psm| protocol specifier. This function returns true if the connect
// operation could be started and will indicate completion with either a 'connected'
// callback (success) or a 'disconnected' callback (failure).
//
// This function must not be called while a connect operation is in progress or
// while |l2cap_client_is_connected|. |client| and |remote_bdaddr| must not be NULL.
// |psm| must be greater than zero.
bool l2cap_client_connect(l2cap_client_t *client, const bt_bdaddr_t *remote_bdaddr, uint16_t psm);

// Disconnects a connected |client|. This function is asynchronous and idempotent. It
// will indicate completion with a 'disconnected' callback. |client| must not be NULL.
void l2cap_client_disconnect(l2cap_client_t *client);

// Returns true if |client| is connected and is ready to accept data written to it.
// |client| must not be NULL.
bool l2cap_client_is_connected(const l2cap_client_t *client);

// Writes data contained in |packet| to a connected |client|. This function returns
// true if the packet was successfully queued for delivery, false if the client cannot
// accept more data at this time. If this function returns false, the caller must wait
// for the 'write_ready' callback to write additional data to the client. Neither
// |client| nor |packet| may be NULL.
bool l2cap_client_write(l2cap_client_t *client, buffer_t *packet);

#endif  ///(defined(L2CAP_CLIENT_INCLUDED) && L2CAP_CLIENT_INCLUDED == TRUE)

#endif /*_L2CAP_CLIENT_H_*/
