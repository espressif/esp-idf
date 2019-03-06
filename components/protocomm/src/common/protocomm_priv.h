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

#pragma once

#include <rom/queue.h>
#include <protocomm_security.h>
#include <esp_err.h>

#define PROTOCOMM_NO_SESSION_ID UINT32_MAX

/* Bit Flags for indicating intended functionality of handler to either
 * process request or establish secure session */
#define REQ_EP      (1 << 0)    /*!< Flag indicating request  handling endpoint */
#define SEC_EP      (1 << 1)    /*!< Flag indicating security handling endpoint */
#define VER_EP      (1 << 2)    /*!< Flag indicating version  handling endpoint */

/**
 * @brief   Protocomm endpoint table entry prototype
 *
 * The structure of an entry stored in the endpoint table.
 */
typedef struct protocomm_ep {
    const char              *ep_name;       /*!< Unique endpoint name */
    protocomm_req_handler_t  req_handler;   /*!< Request handler function */

    /* Pointer to private data to be passed as a parameter to the handler
     * function at the time of call. Set to NULL if not used. */
    void            *priv_data;

    uint32_t         flag;          /*!< Flag indicating endpoint functionality */

    /* Next endpoint entry in the singly linked list for storing endpoint handlers */
    SLIST_ENTRY(protocomm_ep) next;
} protocomm_ep_t;

/**
 * @brief   Prototype structure of a Protocomm instance
 *
 * This structure corresponds to a unique instance of protocomm returned,
 * when the API protocomm_new() is called. The remaining Protocomm
 * APIs require this object as the first parameter.
 */
struct protocomm {
    /* Function Prototype of transport specific function, which is called
     * internally when protocomm_add_endpoint() is invoked. */
    int (*add_endpoint)(const char *ep_name, protocomm_req_handler_t h, void *priv_data);

    /* Function Prototype of transport specific function, which is called
     * internally when protocomm_remove_endpoint() is invoked. */
    int (*remove_endpoint)(const char *ep_name);

    /* Pointer to security layer instance to be used internally for
     * establishing secure sessions */
    const protocomm_security_t *sec;

    /* Pointer to proof of possession object */
    protocomm_security_pop_t *pop;

    /* Head of the singly linked list for storing endpoint handlers */
    SLIST_HEAD(eptable_t, protocomm_ep) endpoints;

    /* Private data to be used internally by the protocomm instance */
    void* priv;

    /* Application specific version string */
    const char* ver;
};
