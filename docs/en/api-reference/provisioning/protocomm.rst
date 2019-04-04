Protocol Communication
======================

Overview
--------
Protocol Communication (protocomm) component manages secure sessions and provides framework for multiple transports. The application can also use protocomm layer directly to have application specific extensions for the provisioning (or non-provisioning) use cases.

Following features are available for provisioning :
    * Communication security at application level -
        * protocomm_security0 (no security)
        * protocomm_security1 (curve25519 key exchange + AES-CTR encryption)
    * Proof-of-possession (support with protocomm_security1 only)

Protocomm internally uses protobuf (protocol buffers) for secure session establishment. Though users can implement their own security (even without using protobuf). One can even use protocomm without any security layer.

Protocomm provides framework for various transports - WiFi (SoftAP+HTTPD), BLE, console - in which case the handler invocation is automatically taken care of on the device side (see Transport Examples below for code snippets).

Note that the client still needs to establish session (only for protocomm_security1) by performing the two way handshake. See :doc:`provisioning` for more details about the secure handshake logic.

Transport Example (SoftAP + HTTP) with Security 1
-------------------------------------------------
For complete example see :example:`provisioning/softap_prov`

    .. highlight:: c

    ::

        /* Endpoint handler to be registered with protocomm.
         * This simply echoes back the received data. */
        esp_err_t echo_req_handler (uint32_t session_id,
                                    const uint8_t *inbuf, ssize_t inlen,
                                    uint8_t **outbuf, ssize_t *outlen,
                                    void *priv_data)
        {
            /* Session ID may be used for persistence */
            printf("Session ID : %d", session_id);

            /* Echo back the received data */
            *outlen = inlen;            /* Output data length updated */
            *outbuf = malloc(inlen);    /* This will be deallocated outside */
            memcpy(*outbuf, inbuf, inlen);

            /* Private data that was passed at the time of endpoint creation */
            uint32_t *priv = (uint32_t *) priv_data;
            if (priv) {
                printf("Private data : %d", *priv);
            }

            return ESP_OK;
        }

        /* Example function for launching a protocomm instance over HTTP */
        protocomm_t *start_pc(const char *pop_string)
        {
            protocomm_t *pc = protocomm_new();


            /* Config for protocomm_httpd_start() */
            protocomm_httpd_config_t pc_config = {
                .data = {
                .config = PROTOCOMM_HTTPD_DEFAULT_CONFIG()
                }
            };

            /* Start protocomm server on top of HTTP */
            protocomm_httpd_start(pc, &pc_config);

            /* Create Proof of Possession object from pop_string. It must be valid
             * throughout the scope of protocomm endpoint. This need not be static,
             * ie. could be dynamically allocated and freed at the time of endpoint
             * removal */
            const static protocomm_security_pop_t pop_obj = {
                .data = (const uint8_t *) strdup(pop_string),
                .len = strlen(pop_string)
            };

            /* Set security for communication at application level. Just like for
             * request handlers, setting security creates an endpoint and registers
             * the handler provided by protocomm_security1. One can similarly use
             * protocomm_security0. Only one type of security can be set for a
             * protocomm instance at a time. */
            protocomm_set_security(pc, "security_endpoint", &protocomm_security1, &pop_obj);

            /* Private data passed to the endpoint must be valid throughout the scope
             * of protocomm endpoint. This need not be static, ie. could be dynamically
             * allocated and freed at the time of endpoint removal */
            static uint32_t priv_data = 1234;

            /* Add a new endpoint for the protocomm instance, identified by a unique name
             * and register a handler function along with private data to be passed at the
             * time of handler execution. Multiple endpoints can be added as long as they
             * are identified by unique names */
            protocomm_add_endpoint(pc, "echo_req_endpoint",
                                   echo_req_handler, (void *) &priv_data);
            return pc;
        }

        /* Example function for stopping a protocomm instance */
        void stop_pc(protocomm_t *pc)
        {
            /* Remove endpoint identified by it's unique name */
            protocomm_remove_endpoint(pc, "echo_req_endpoint");

            /* Remove security endpoint identified by it's name */
            protocomm_unset_security(pc, "security_endpoint");

            /* Stop HTTP server */
            protocomm_httpd_stop(pc);

            /* Delete (deallocate) the protocomm instance */
            protocomm_delete(pc);
        }

Transport Example (BLE) with Security 0
---------------------------------------
For complete example see :example:`provisioning/ble_prov`

    .. highlight:: c

    ::

        /* Example function for launching a secure protocomm instance over BLE */
        protocomm_t *start_pc()
        {
            protocomm_t *pc = protocomm_new();

            /* Endpoint UUIDs */
            protocomm_ble_name_uuid_t nu_lookup_table[] = {
                {"security_endpoint", 0xFF51},
                {"echo_req_endpoint", 0xFF52}
            };

            /* Config for protocomm_ble_start() */
            protocomm_ble_config_t config = {
                .service_uuid = {
                    /* LSB <---------------------------------------
                    * ---------------------------------------> MSB */
                    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80,
                    0x00, 0x10, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00,
                },
                .nu_lookup_count = sizeof(nu_lookup_table)/sizeof(nu_lookup_table[0]),
                .nu_lookup = nu_lookup_table
            };

            /* Start protocomm layer on top of BLE */
            protocomm_ble_start(pc, &config);

            /* For protocomm_security0, Proof of Possession is not used, and can be kept NULL */
            protocomm_set_security(pc, "security_endpoint", &protocomm_security0, NULL);
            protocomm_add_endpoint(pc, "echo_req_endpoint", echo_req_handler, NULL);
            return pc;
        }

        /* Example function for stopping a protocomm instance */
        void stop_pc(protocomm_t *pc)
        {
            protocomm_remove_endpoint(pc, "echo_req_endpoint");
            protocomm_unset_security(pc, "security_endpoint");

            /* Stop BLE protocomm service */
            protocomm_ble_stop(pc);

            protocomm_delete(pc);
        }

API Reference
-------------

.. include:: /_build/inc/protocomm.inc
.. include:: /_build/inc/protocomm_security.inc
.. include:: /_build/inc/protocomm_security0.inc
.. include:: /_build/inc/protocomm_security1.inc
.. include:: /_build/inc/protocomm_httpd.inc
.. include:: /_build/inc/protocomm_ble.inc
