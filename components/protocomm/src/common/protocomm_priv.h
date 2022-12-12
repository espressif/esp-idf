/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <sys/queue.h>
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

    /* Pointer to security layer to be used internally for
     * establishing secure sessions */
    const protocomm_security_t *sec;

    /* Handle to the security layer instance */
    protocomm_security_handle_t sec_inst;

    /* Pointer to security params */
    void *sec_params;

    /* Head of the singly linked list for storing endpoint handlers */
    SLIST_HEAD(eptable_t, protocomm_ep) endpoints;

    /* Private data to be used internally by the protocomm instance */
    void* priv;

    /* Application specific version string */
    const char* ver;
};
