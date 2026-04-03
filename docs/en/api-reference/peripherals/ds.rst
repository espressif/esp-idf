RSA Digital Signature Peripheral (RSA_DS)
==========================================

:link_to_translation:`zh_CN:[中文]`

The RSA Digital Signature Peripheral (RSA_DS) provides hardware acceleration of signing messages based on RSA. It uses pre-encrypted parameters to calculate a signature. The parameters are encrypted using HMAC as a key-derivation function. In turn, the HMAC uses eFuses as the input key.

.. only:: SOC_KEY_MANAGER_SUPPORTED

    On {IDF_TARGET_NAME}, the RSA Digital Signature Peripheral (RSA_DS) can also use a key stored in the Key Manager instead of an eFuse key block. The AES encryption key can be directly deployed in the Key Manager with the type :cpp:enumerator:`ESP_KEY_MGR_DS_KEY`. Refer to :ref:`key-manager` for more details.

The whole process happens in hardware so that neither the decryption key for the RSA parameters nor the input key for the HMAC key derivation function can be seen by the software while calculating the signature.

For more detailed information on the hardware involved in the signature calculation and the registers used, see **{IDF_TARGET_NAME} Technical Reference Manual** > **RSA Digital Signature Peripheral (RSA_DS)** [`PDF <{IDF_TARGET_TRM_EN_URL}#digsig>`__].


Private Key Parameters
----------------------

The private key parameters for the RSA signature are stored in flash. To prevent unauthorized access, they are AES-encrypted. The HMAC module is used as a key-derivation function to calculate the AES encryption key for the private key parameters. In turn, the HMAC module uses a key from the eFuses key block which can be read-protected to prevent unauthorized access as well.

Upon signature calculation invocation, the software only specifies which eFuse key to use, the corresponding eFuse key purpose, the location of the encrypted RSA parameters, and the message.

Key Generation
--------------

Both the HMAC key and the RSA private key have to be created and stored before the RSA_DS peripheral can be used. This needs to be done in software on the {IDF_TARGET_NAME} or alternatively on a host. For this context, ESP-IDF provides :cpp:func:`esp_efuse_write_block` to set the HMAC key and :cpp:func:`esp_hmac_calculate` to encrypt the private RSA key parameters.

You can find instructions on how to calculate and assemble the private key parameters in **{IDF_TARGET_NAME} Technical Reference Manual** > **RSA Digital Signature Peripheral (RSA_DS)** [`PDF <{IDF_TARGET_TRM_EN_URL}#digsig>`__].

Signature Calculation with ESP-IDF
----------------------------------

For more detailed information on the workflow and the registers used, see **{IDF_TARGET_NAME} Technical Reference Manual** > **RSA Digital Signature Peripheral (RSA_DS)** [`PDF <{IDF_TARGET_TRM_EN_URL}#digsig>`__].

Three parameters need to be prepared to calculate the digital signature:

#. The eFuse key block ID which is used as the key for the HMAC
#. The location of the encrypted private key parameters
#. The message to be signed

**Low-level API (plain RSA)**

Since the signature calculation takes some time, there are two possible API versions to use in ESP-IDF. The first one is :cpp:func:`esp_ds_sign` and simply blocks until the calculation is finished. If software needs to do something else during the calculation, :cpp:func:`esp_ds_start_sign` can be called, followed by periodic calls to :cpp:func:`esp_ds_is_busy` to check when the calculation has finished. Once the calculation has finished, :cpp:func:`esp_ds_finish_sign` can be called to get the resulting signature.

The APIs :cpp:func:`esp_ds_sign` and :cpp:func:`esp_ds_start_sign` calculate a plain RSA signature with the help of the RSA_DS peripheral. This signature must be converted to an appropriate format (e.g., PKCS#1 v1.5 or PSS) for use in TLS or other protocols.

.. note::

    This is only the basic RSA_DS building block; the message length is fixed. To create signatures of arbitrary messages, the input is normally a hash of the actual message, padded up to the required length.

**PSA Crypto driver**

The RSA_DS peripheral is also exposed via the **PSA Crypto RSA_DS driver**, so you can use standard PSA APIs for signing (PKCS#1 v1.5 or PSS) and RSA decryption (PKCS#1 v1.5 or OAEP). Enable ``CONFIG_MBEDTLS_HARDWARE_RSA_DS_PERIPHERAL`` in ``Component config`` > ``mbedTLS``. For using the RSA_DS peripheral with ESP-TLS (e.g. TLS client authentication), see :ref:`digital-signature-with-esp-tls` in the ESP-TLS documentation.

.. _configure-the-ds-peripheral:

Configure the RSA_DS Peripheral for a TLS Connection
----------------------------------------------------

The RSA_DS peripheral on {IDF_TARGET_NAME} chip must be configured before it can be used for a TLS connection. The configuration involves the following steps:

1) Randomly generate a 256-bit value called the ``Initialization Vector`` (IV).
2) Randomly generate a 256-bit value called the ``HMAC_KEY``.
3) Calculate the encrypted private key parameters from the client private key (RSA) and the parameters generated in the above steps.
4) Then burn the 256-bit ``HMAC_KEY`` on the eFuse, which can only be read by the RSA_DS peripheral.

For more details, see **{IDF_TARGET_NAME} Technical Reference Manual** > **RSA Digital Signature Peripheral (RSA_DS)** [`PDF <{IDF_TARGET_TRM_EN_URL}#digsig>`__].

To configure the RSA_DS peripheral for development purposes, you can use the `esp-secure-cert-tool <https://pypi.org/project/esp-secure-cert-tool>`_.

The encrypted private key parameters obtained after the RSA_DS peripheral configuration should be stored in flash. The application needs to read the RSA_DS data from flash (e.g., through the APIs provided by the `esp_secure_cert_mgr <https://github.com/espressif/esp_secure_cert_mgr>`_ component; see the `component/README <https://github.com/espressif/esp_secure_cert_mgr#readme>`_ for more details). For using the RSA_DS peripheral with ESP-TLS, see :ref:`digital-signature-with-esp-tls`.

Using RSA_DS with PSA Crypto
-----------------------------

To use the RSA_DS peripheral for signing or decryption in application code (outside of ESP-TLS), enable ``CONFIG_MBEDTLS_HARDWARE_RSA_DS_PERIPHERAL``. Populate an ``esp_ds_data_ctx_t`` with the encrypted key data (:cpp:type:`esp_ds_data_t`), the eFuse key block ID, and the RSA key length in bits. Ensure the ``rsa_length`` field of :cpp:type:`esp_ds_data_t` is set when the key is created (e.g. via :cpp:func:`esp_ds_encrypt_params` or the RSA_DS provisioning tool). Then wrap the context in an ``esp_rsa_ds_opaque_key_t``, import it as a PSA opaque key using ``PSA_KEY_LIFETIME_ESP_RSA_DS_VOLATILE``, and call ``psa_sign_hash()`` or ``psa_asymmetric_decrypt()``:

.. code-block:: c

    #include "psa/crypto.h"
    #include "psa_crypto_driver_esp_rsa_ds.h"

    // ds_ctx points to esp_ds_data_ctx_t (e.g. from secure cert or NVS)
    esp_ds_data_ctx_t *ds_ctx = ...;
    esp_rsa_ds_opaque_key_t rsa_ds_opaque_key = {
        .ds_data_ctx = ds_ctx,
    };

    psa_key_attributes_t attrs = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_type(&attrs, PSA_KEY_TYPE_RSA_KEY_PAIR);
    psa_set_key_bits(&attrs, ds_ctx->rsa_length_bits);
    psa_set_key_usage_flags(&attrs, PSA_KEY_USAGE_SIGN_HASH);
    psa_set_key_algorithm(&attrs, PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256));
    psa_set_key_lifetime(&attrs, PSA_KEY_LIFETIME_ESP_RSA_DS_VOLATILE);

    psa_key_id_t key_id;
    psa_status_t status = psa_import_key(&attrs,
                                         (const uint8_t *)&rsa_ds_opaque_key,
                                         sizeof(rsa_ds_opaque_key),
                                         &key_id);
    psa_reset_key_attributes(&attrs);
    if (status != PSA_SUCCESS) {
        // handle error
    }

    // Sign a hash (e.g. SHA-256 of your message)
    uint8_t hash[32] = { ... };
    uint8_t signature[256];
    size_t sig_len;
    status = psa_sign_hash(key_id, PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256),
                           hash, sizeof(hash), signature, sizeof(signature), &sig_len);

    psa_destroy_key(key_id);

Example for SSL Mutual Authentication Using RSA_DS
---------------------------------------------------

The SSL mutual authentication example that previously lived under ``examples/protocols/mqtt/ssl_ds`` is now shipped with the standalone `espressif/mqtt <https://components.espressif.com/components/espressif/mqtt>`__ component. Follow the component documentation to fetch the SSL RSA_DS example and build it together with ESP-MQTT. The example continues to use ``mqtt_client`` (implemented by ESP-MQTT) to connect to ``test.mosquitto.org`` over mutual-authenticated TLS, with the TLS portion handled by ESP-TLS.

.. only:: SOC_KEY_MANAGER_SUPPORTED

    In case both the :cpp:member:`esp_ds_data_ctx_t::efuse_key_id` and :cpp:member:`esp_rsa_ds_opaque_key_t::key_recovery_info` are set, the ESP-DS PSA driver prefers using the Key Manager-based DS key over the eFuse-based DS key.

API Reference
-------------

.. include-build-file:: inc/psa_crypto_driver_esp_rsa_ds_contexts.inc
