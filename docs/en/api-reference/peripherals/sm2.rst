SM2 Digital Signatures (SM2)
============================

:link_to_translation:`zh_CN:[中文]`

SM2 is an elliptic curve public-key algorithm defined in the Chinese national standard GB/T 32918. The {IDF_TARGET_NAME} ECC accelerator supports the SM2 recommended curve (GB/T 32918.5). The ``esp_sm2`` API uses this hardware to derive public keys, to generate signatures, and to verify signatures.

All operations run on the ECC accelerator. The API does not use a software big-number library. The point multiplication that touches the private key or the nonce runs in constant-time mode.

Every buffer in this API is 32 bytes long (:c:macro:`ESP_SM2_PARAM_LEN`) and big-endian.

Digest Input
------------

The API signs and verifies a 32-byte digest, not a message. GB/T 32918.2 defines the digest as ``e = SM3(ZA || M)``, where ``M`` is the message and ``ZA`` is the SM3 hash of the signer identity and the curve parameters. Use the ``esp_sm3`` API to compute both hashes. The caller must compute the digest before it calls this API.

Generate a Key Pair
-------------------

A private key is a random 32-byte value ``d`` in the range ``[1, n-2]``, where ``n`` is the order of the curve. Draw random candidates until the API accepts one:

.. code-block:: c

    #include "esp_sm2.h"
    #include "esp_random.h"

    uint8_t priv_key[ESP_SM2_PARAM_LEN];
    uint8_t pub_x[ESP_SM2_PARAM_LEN];
    uint8_t pub_y[ESP_SM2_PARAM_LEN];
    esp_err_t err;

    do {
        esp_fill_random(priv_key, sizeof(priv_key));
        err = esp_sm2_get_pubkey(priv_key, pub_x, pub_y);
    } while (err == ESP_ERR_INVALID_ARG);
    ESP_ERROR_CHECK(err);

The caller owns the private key. Store it in a secure location, and wipe every copy that is no longer necessary.

Sign a Digest
-------------

.. code-block:: c

    uint8_t digest[ESP_SM2_PARAM_LEN];   /* e = SM3(ZA || M), computed by the caller */
    uint8_t sig_r[ESP_SM2_PARAM_LEN];
    uint8_t sig_s[ESP_SM2_PARAM_LEN];

    ESP_ERROR_CHECK(esp_sm2_sign(digest, priv_key, sig_r, sig_s));

:cpp:func:`esp_sm2_sign` returns ``ESP_ERR_INVALID_ARG`` for a malformed input, and ``ESP_FAIL`` when it uses all of its retry attempts. Only a failing random number generator can exhaust the retries.

.. warning::

    The signature nonce comes from :cpp:func:`esp_fill_random`. Make sure that a true entropy source is active before you sign. A biased nonce lets an attacker calculate the private key from signatures. See :doc:`../system/random` for the conditions under which the random number generator produces true random numbers.

Verify a Signature
------------------

.. code-block:: c

    esp_err_t res = esp_sm2_verify(digest, pub_x, pub_y, sig_r, sig_s);
    if (res == ESP_OK) {
        /* The signature is valid. */
    }

:cpp:func:`esp_sm2_verify` validates the public key on the curve in hardware before it uses the key. It returns ``ESP_OK`` only for a valid signature. It returns ``ESP_ERR_INVALID_ARG`` for a malformed input, and ``ESP_FAIL`` for an invalid signature or an invalid public key.

.. only:: SOC_EFUSE_DIS_SM_CRYPT

    Availability
    ------------

    The ``DIS_SM_CRYPT`` eFuse permanently disables the SM2 and SM3 hardware. When this eFuse is burned, the functions in this API return ``ESP_ERR_NOT_SUPPORTED``. Invalid arguments are still reported as ``ESP_ERR_INVALID_ARG``.

Application Example
-------------------

The test application :component_file:`esp_security/test_apps/crypto_drivers/main/test_esp_sm2.c` shows the complete flow: key derivation against a known-answer vector, signing, tamper detection, and verification with generated vectors.

API Reference
-------------

.. include-build-file:: inc/esp_sm2.inc
