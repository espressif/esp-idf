Protocol Communication
======================

:link_to_translation:`zh_CN:[中文]`

Overview
--------

The Protocol Communication (protocomm) component manages secure sessions and provides the framework for multiple transports. The application can also use the protocomm layer directly to have application-specific extensions for the provisioning or non-provisioning use cases.

Following features are available for provisioning:

    * Communication security at the application level

        * ``protocomm_security0`` (no security)
        * ``protocomm_security1`` (Curve25519 key exchange + AES-CTR encryption/decryption)
        * ``protocomm_security2`` (SRP6a-based key exchange + AES-GCM encryption/decryption)
    * Proof-of-possession (support with protocomm_security1 only)
    * Salt and Verifier (support with protocomm_security2 only)

Protocomm internally uses protobuf (protocol buffers) for secure session establishment. Users can choose to implement their own security (even without using protobuf). Protocomm can also be used without any security layer.

Protocomm provides the framework for various transports:

.. list::

    :SOC_BLE_SUPPORTED: - Bluetooth LE
    :SOC_WIFI_SUPPORTED: - Wi-Fi (SoftAP + HTTPD)
    - Console, in which case the handler invocation is automatically taken care of on the device side. See Transport Examples below for code snippets.

Note that for protocomm_security1 and protocomm_security2, the client still needs to establish sessions by performing the two-way handshake.

.. only:: SOC_WIFI_SUPPORTED

    See :doc:`provisioning` for more details about the secure handshake logic.

.. _enabling-protocomm-security-version:

Enabling Protocomm Security Version
-----------------------------------

The protocomm component provides a project configuration menu to enable/disable support of respective security versions. The respective configuration options are as follows:

    * Support ``protocomm_security0``, with no security: :ref:`CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_0`, this option is enabled by default.
    * Support ``protocomm_security1`` with Curve25519 key exchange + AES-CTR encryption/decryption: :ref:`CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_1`, this option is enabled by default.
    * Support ``protocomm_security2`` with SRP6a-based key exchange + AES-GCM encryption/decryption: :ref:`CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_2`.

.. note::

    Enabling multiple security versions at once offers the ability to control them dynamically but also increases the firmware size.

.. only:: SOC_WIFI_SUPPORTED

    SoftAP + HTTP Transport Example with Security 2
    -----------------------------------------------

    For sample usage, see :component_file:`wifi_provisioning/src/scheme_softap.c`.

    .. highlight:: c

    ::

        /* The endpoint handler to be registered with protocomm. This simply echoes back the received data. */
        esp_err_t echo_req_handler (uint32_t session_id,
                                    const uint8_t *inbuf, ssize_t inlen,
                                    uint8_t **outbuf, ssize_t *outlen,
                                    void *priv_data)
        {
            /* Session ID may be used for persistence. */
            printf("Session ID : %d", session_id);

            /* Echo back the received data. */
            *outlen = inlen;            /* Output the data length updated. */
            *outbuf = malloc(inlen);    /* This is to be deallocated outside. */
            memcpy(*outbuf, inbuf, inlen);

            /* Private data that was passed at the time of endpoint creation. */
            uint32_t *priv = (uint32_t *) priv_data;
            if (priv) {
                printf("Private data : %d", *priv);
            }

            return ESP_OK;
        }

        static const char sec2_salt[] = {0xf7, 0x5f, 0xe2, 0xbe, 0xba, 0x7c, 0x81, 0xcd};
        static const char sec2_verifier[] = {0xbf, 0x86, 0xce, 0x63, 0x8a, 0xbb, 0x7e, 0x2f, 0x38, 0xa8, 0x19, 0x1b, 0x35,
            0xc9, 0xe3, 0xbe, 0xc3, 0x2b, 0x45, 0xee, 0x10, 0x74, 0x22, 0x1a, 0x95, 0xbe, 0x62, 0xf7, 0x0c, 0x65, 0x83, 0x50,
            0x08, 0xef, 0xaf, 0xa5, 0x94, 0x4b, 0xcb, 0xe1, 0xce, 0x59, 0x2a, 0xe8, 0x7b, 0x27, 0xc8, 0x72, 0x26, 0x71, 0xde,
            0xb2, 0xf2, 0x80, 0x02, 0xdd, 0x11, 0xf0, 0x38, 0x0e, 0x95, 0x25, 0x00, 0xcf, 0xb3, 0x3f, 0xf0, 0x73, 0x2a, 0x25,
            0x03, 0xe8, 0x51, 0x72, 0xef, 0x6d, 0x3e, 0x14, 0xb9, 0x2e, 0x9f, 0x2a, 0x90, 0x9e, 0x26, 0xb6, 0x3e, 0xc7, 0xe4,
            0x9f, 0xe3, 0x20, 0xce, 0x28, 0x7c, 0xbf, 0x89, 0x50, 0xc9, 0xb6, 0xec, 0xdd, 0x81, 0x18, 0xf1, 0x1a, 0xd9, 0x7a,
            0x21, 0x99, 0xf1, 0xee, 0x71, 0x2f, 0xcc, 0x93, 0x16, 0x34, 0x0c, 0x79, 0x46, 0x23, 0xe4, 0x32, 0xec, 0x2d, 0x9e,
            0x18, 0xa6, 0xb9, 0xbb, 0x0a, 0xcf, 0xc4, 0xa8, 0x32, 0xc0, 0x1c, 0x32, 0xa3, 0x97, 0x66, 0xf8, 0x30, 0xb2, 0xda,
            0xf9, 0x8d, 0xc3, 0x72, 0x72, 0x5f, 0xe5, 0xee, 0xc3, 0x5c, 0x24, 0xc8, 0xdd, 0x54, 0x49, 0xfc, 0x12, 0x91, 0x81,
            0x9c, 0xc3, 0xac, 0x64, 0x5e, 0xd6, 0x41, 0x88, 0x2f, 0x23, 0x66, 0xc8, 0xac, 0xb0, 0x35, 0x0b, 0xf6, 0x9c, 0x88,
            0x6f, 0xac, 0xe1, 0xf4, 0xca, 0xc9, 0x07, 0x04, 0x11, 0xda, 0x90, 0x42, 0xa9, 0xf1, 0x97, 0x3d, 0x94, 0x65, 0xe4,
            0xfb, 0x52, 0x22, 0x3b, 0x7a, 0x7b, 0x9e, 0xe9, 0xee, 0x1c, 0x44, 0xd0, 0x73, 0x72, 0x2a, 0xca, 0x85, 0x19, 0x4a,
            0x60, 0xce, 0x0a, 0xc8, 0x7d, 0x57, 0xa4, 0xf8, 0x77, 0x22, 0xc1, 0xa5, 0xfa, 0xfb, 0x7b, 0x91, 0x3b, 0xfe, 0x87,
            0x5f, 0xfe, 0x05, 0xd2, 0xd6, 0xd3, 0x74, 0xe5, 0x2e, 0x68, 0x79, 0x34, 0x70, 0x40, 0x12, 0xa8, 0xe1, 0xb4, 0x6c,
            0xaa, 0x46, 0x73, 0xcd, 0x8d, 0x17, 0x72, 0x67, 0x32, 0x42, 0xdc, 0x10, 0xd3, 0x71, 0x7e, 0x8b, 0x00, 0x46, 0x9b,
            0x0a, 0xe9, 0xb4, 0x0f, 0xeb, 0x70, 0x52, 0xdd, 0x0a, 0x1c, 0x7e, 0x2e, 0xb0, 0x61, 0xa6, 0xe1, 0xa3, 0x34, 0x4b,
            0x2a, 0x3c, 0xc4, 0x5d, 0x42, 0x05, 0x58, 0x25, 0xd3, 0xca, 0x96, 0x5c, 0xb9, 0x52, 0xf9, 0xe9, 0x80, 0x75, 0x3d,
            0xc8, 0x9f, 0xc7, 0xb2, 0xaa, 0x95, 0x2e, 0x76, 0xb3, 0xe1, 0x48, 0xc1, 0x0a, 0xa1, 0x0a, 0xe8, 0xaf, 0x41, 0x28,
            0xd2, 0x16, 0xe1, 0xa6, 0xd0, 0x73, 0x51, 0x73, 0x79, 0x98, 0xd9, 0xb9, 0x00, 0x50, 0xa2, 0x4d, 0x99, 0x18, 0x90,
            0x70, 0x27, 0xe7, 0x8d, 0x56, 0x45, 0x34, 0x1f, 0xb9, 0x30, 0xda, 0xec, 0x4a, 0x08, 0x27, 0x9f, 0xfa, 0x59, 0x2e,
            0x36, 0x77, 0x00, 0xe2, 0xb6, 0xeb, 0xd1, 0x56, 0x50, 0x8e};

        /* The example function for launching a protocomm instance over HTTP. */
        protocomm_t *start_pc()
        {
            protocomm_t *pc = protocomm_new();


            /* Config for protocomm_httpd_start(). */
            protocomm_httpd_config_t pc_config = {
                .data = {
                .config = PROTOCOMM_HTTPD_DEFAULT_CONFIG()
                }
            };

            /* Start the protocomm server on top of HTTP. */
            protocomm_httpd_start(pc, &pc_config);

            /* Create Security2 params object from salt and verifier. It must be valid throughout the scope of protocomm endpoint. This does not need to be static, i.e., could be dynamically allocated and freed at the time of endpoint removal. */
            const static protocomm_security2_params_t sec2_params = {
                .salt = (const uint8_t *) salt,
                .salt_len = sizeof(salt),
                .verifier = (const uint8_t *) verifier,
                .verifier_len = sizeof(verifier),
            };

            /* Set security for communication at the application level. Just like for request handlers, setting security creates an endpoint and registers the handler provided by protocomm_security1. One can similarly use protocomm_security0. Only one type of security can be set for a protocomm instance at a time. */
            protocomm_set_security(pc, "security_endpoint", &protocomm_security2, &sec2_params);

            /* Private data passed to the endpoint must be valid throughout the scope of protocomm endpoint. This need not be static, i.e., could be dynamically allocated and freed at the time of endpoint removal. */
            static uint32_t priv_data = 1234;

            /* Add a new endpoint for the protocomm instance, identified by a unique name, and register a handler function along with the private data to be passed at the time of handler execution. Multiple endpoints can be added as long as they are identified by unique names. */
            protocomm_add_endpoint(pc, "echo_req_endpoint",
                                   echo_req_handler, (void *) &priv_data);
            return pc;
        }

        /* The example function for stopping a protocomm instance. */
        void stop_pc(protocomm_t *pc)
        {
            /* Remove the endpoint identified by its unique name. */
            protocomm_remove_endpoint(pc, "echo_req_endpoint");

            /* Remove the security endpoint identified by its name. */
            protocomm_unset_security(pc, "security_endpoint");

            /* Stop the HTTP server. */
            protocomm_httpd_stop(pc);

            /* Delete, namely deallocate the protocomm instance. */
            protocomm_delete(pc);
        }

    SoftAP + HTTP Transport Example with Security 1
    -----------------------------------------------

    For sample usage, see :component_file:`wifi_provisioning/src/scheme_softap.c`.

    .. highlight:: c

    ::

        /* The endpoint handler to be registered with protocomm. This simply echoes back the received data. */
        esp_err_t echo_req_handler (uint32_t session_id,
                                    const uint8_t *inbuf, ssize_t inlen,
                                    uint8_t **outbuf, ssize_t *outlen,
                                    void *priv_data)
        {
            /* Session ID may be used for persistence. */
            printf("Session ID : %d", session_id);

            /* Echo back the received data. */
            *outlen = inlen;            /* Output the data length updated. */
            *outbuf = malloc(inlen);    /* This is to be deallocated outside. */
            memcpy(*outbuf, inbuf, inlen);

            /* Private data that was passed at the time of endpoint creation. */
            uint32_t *priv = (uint32_t *) priv_data;
            if (priv) {
                printf("Private data : %d", *priv);
            }

            return ESP_OK;
        }

        /* The example function for launching a protocomm instance over HTTP. */
        protocomm_t *start_pc(const char *pop_string)
        {
            protocomm_t *pc = protocomm_new();


            /* Config for protocomm_httpd_start(). */
            protocomm_httpd_config_t pc_config = {
                .data = {
                .config = PROTOCOMM_HTTPD_DEFAULT_CONFIG()
                }
            };

            /* Start the protocomm server on top of HTTP. */
            protocomm_httpd_start(pc, &pc_config);

            /* Create security1 params object from pop_string. It must be valid throughout the scope of protocomm endpoint. This need not be static, i.e., could be dynamically allocated and freed at the time of endpoint removal. */
            const static protocomm_security1_params_t sec1_params = {
                .data = (const uint8_t *) strdup(pop_string),
                .len = strlen(pop_string)
            };

            /* Set security for communication at the application level. Just like for request handlers, setting security creates an endpoint and registers the handler provided by protocomm_security1. One can similarly use protocomm_security0. Only one type of security can be set for a protocomm instance at a time. */
            protocomm_set_security(pc, "security_endpoint", &protocomm_security1, &sec1_params);

            /* Private data passed to the endpoint must be valid throughout the scope of protocomm endpoint. This need not be static, i.e., could be dynamically allocated and freed at the time of endpoint removal. */
            static uint32_t priv_data = 1234;

            /* Add a new endpoint for the protocomm instance identified by a unique name, and register a handler function along with the private data to be passed at the time of handler execution. Multiple endpoints can be added as long as they are identified by unique names. */
            protocomm_add_endpoint(pc, "echo_req_endpoint",
                                   echo_req_handler, (void *) &priv_data);
            return pc;
        }

        /* The example function for stopping a protocomm instance. */
        void stop_pc(protocomm_t *pc)
        {
            /* Remove the endpoint identified by its unique name. */
            protocomm_remove_endpoint(pc, "echo_req_endpoint");

            /* Remove the security endpoint identified by its name. */
            protocomm_unset_security(pc, "security_endpoint");

            /* Stop the HTTP server. */
            protocomm_httpd_stop(pc);

            /* Delete, namely deallocate the protocomm instance. */
            protocomm_delete(pc);
        }

.. only:: SOC_BLE_SUPPORTED

    Bluetooth LE Transport Example with Security 0
    ----------------------------------------------

    For sample usage, see :component_file:`wifi_provisioning/src/scheme_ble.c`.

    .. highlight:: c

    ::

        /* The example function for launching a secure protocomm instance over Bluetooth LE. */
        protocomm_t *start_pc()
        {
            protocomm_t *pc = protocomm_new();

            /* Endpoint UUIDs */
            protocomm_ble_name_uuid_t nu_lookup_table[] = {
                {"security_endpoint", 0xFF51},
                {"echo_req_endpoint", 0xFF52}
            };

            /* Config for protocomm_ble_start(). */
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

            /* Start protocomm layer on top of Bluetooth LE. */
            protocomm_ble_start(pc, &config);

            /* For protocomm_security0, Proof of Possession is not used, and can be kept NULL. */
            protocomm_set_security(pc, "security_endpoint", &protocomm_security0, NULL);
            protocomm_add_endpoint(pc, "echo_req_endpoint", echo_req_handler, NULL);
            return pc;
        }

        /* The example function for stopping a protocomm instance. */
        void stop_pc(protocomm_t *pc)
        {
            protocomm_remove_endpoint(pc, "echo_req_endpoint");
            protocomm_unset_security(pc, "security_endpoint");

            /* Stop the Bluetooth LE protocomm service. */
            protocomm_ble_stop(pc);

            protocomm_delete(pc);
        }

API Reference
-------------

.. include-build-file:: inc/protocomm.inc
.. include-build-file:: inc/protocomm_security.inc
.. include-build-file:: inc/protocomm_security0.inc
.. include-build-file:: inc/protocomm_security1.inc
.. include-build-file:: inc/protocomm_security2.inc
.. include-build-file:: inc/esp_srp.inc
.. include-build-file:: inc/protocomm_httpd.inc

.. only:: SOC_BLE_SUPPORTED

    .. include-build-file:: inc/protocomm_ble.inc
