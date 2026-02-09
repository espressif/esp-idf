Hash-Based Message Authentication Code (HMAC)
=============================================

:link_to_translation:`zh_CN:[中文]`

Hash-based Message Authentication Code (HMAC) is a secure authentication technique that verifies the authenticity and integrity of a message with a pre-shared key. This module provides hardware acceleration for SHA256-HMAC generation using a key burned into an eFuse block.

For more detailed information on the application workflow and the HMAC calculation process, see **{IDF_TARGET_NAME} Technical Reference Manual** > **HMAC Accelerator (HMAC)** [`PDF <{IDF_TARGET_TRM_EN_URL}#hmac>`__].

Generalized Application Scheme
------------------------------

Let there be two parties, A and B. They want to verify the authenticity and integrity of messages sent between each other. Before they can start sending messages, they need to exchange the secret key via a secure channel.

To verify A's messages, B can do the following:

- A calculates the HMAC of the message it wants to send.
- A sends the message and the HMAC to B.
- B calculates the HMAC of the received message itself.
- B checks whether the received and calculated HMACs match.

If they do match, the message is authentic.

However, the HMAC itself is not bound to this use case. It can also be used for challenge-response protocols supporting HMAC or as a key input for further security modules (see below), etc.

HMAC on {IDF_TARGET_NAME}
-----------------------------

On {IDF_TARGET_NAME}, the HMAC module works with a secret key burnt into the eFuses. This eFuse key can be made completely inaccessible for any resources outside the cryptographic modules, thus avoiding key leakage.

Furthermore, {IDF_TARGET_NAME} has three different application scenarios for its HMAC module:

#. HMAC is generated for software use
#. HMAC is used as a key for the Digital Signature (DS) module
#. HMAC is used for enabling the soft-disabled JTAG interface

The first mode is called **Upstream** mode, while the last two modes are called **Downstream** modes.

eFuse Keys for HMAC
^^^^^^^^^^^^^^^^^^^

Six physical eFuse blocks can be used as keys for the HMAC module: block 4 ~ block 9. The enum :cpp:enum:`hmac_key_id_t` in the API maps them to ``HMAC_KEY0`` ~ ``HMAC_KEY5``.

Each key has a corresponding eFuse parameter **key purpose** determining for which of the three HMAC application scenarios (see below) the key may be used:

.. list-table::
   :widths: 15 70
   :header-rows: 1

   * - Key Purpose
     - Application Scenario
   * - 8
     - HMAC generated for software use
   * - 7
     - HMAC used as a key for the Digital Signature (DS) module
   * - 6
     - HMAC used for enabling the soft-disabled JTAG interface
   * - 5
     - HMAC both as a key for the DS module and for enabling JTAG

This is to prevent the usage of a key for a different function than originally intended.

To calculate an HMAC, the software has to provide the ID of the key block containing the secret key as well as the **key purpose** (see **{IDF_TARGET_NAME} Technical Reference Manual** > **eFuse Controller (eFuse)** [`PDF <{IDF_TARGET_TRM_EN_URL}#efuse>`__]).

Before the HMAC key calculation, the HMAC module looks up the purpose of the provided key block. The calculation only proceeds if the purpose of the provided key block matches the purpose stored in the eFuses of the key block provided by the ID.

HMAC Generation for Software
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Key purpose value: 8

In this case, the HMAC is given out to the software, e.g., to authenticate a message.

The API to calculate the HMAC is :cpp:func:`psa_mac_compute`, which takes an opaque PSA key referencing an eFuse key block that contains the secret and has its purpose set to Upstream mode.

HMAC for Digital Signature
^^^^^^^^^^^^^^^^^^^^^^^^^^

Key purpose values: 7, 5

The HMAC can be used as a key derivation function to decrypt private key parameters which are used by the Digital Signature module. A standard message is used by the hardware in that case. You only need to provide the eFuse key block and purpose on the HMAC side, additional parameters are required for the Digital Signature component in that case.

Neither the key nor the actual HMAC is ever exposed outside the HMAC module and DS component. The calculation of the HMAC and its handover to the DS component happen internally.

For more details, see **{IDF_TARGET_NAME} Technical Reference Manual** > **Digital Signature (DS)** [`PDF <{IDF_TARGET_TRM_EN_URL}#digsig>`__].

.. _hmac_for_enabling_jtag:

HMAC for Enabling JTAG
^^^^^^^^^^^^^^^^^^^^^^

Key purpose values: 6, 5

The third application is using the HMAC as a key to enable JTAG if it was soft-disabled before.

Following is the procedure to re-enable the JTAG:

**Stage 1: Setup**

1. Generate a 256-bit HMAC secret key to use for JTAG re-enable.
2. Write the key to an eFuse block with key purpose HMAC_DOWN_ALL (5) or HMAC_DOWN_JTAG (6). This can be done using the ``esp_efuse_write_key()`` function in the firmware or using ``idf.py efuse-burn-key`` from the host.
3. Configure the eFuse key block to be read-protected using the ``esp_efuse_set_read_protect()``, so that software cannot read back the value.
4. Burn the ``soft JTAG disable`` bit/bits on {IDF_TARGET_NAME}. This will permanently disable JTAG unless the correct key value is provided by the software.

.. only:: esp32s2

    .. note::

      The API **esp_efuse_write_field_bit(ESP_EFUSE_SOFT_DIS_JTAG)** can be used to burn ``soft JTAG disable`` bit on {IDF_TARGET_NAME}.

.. only:: not esp32s2

    .. note::

      The API **esp_efuse_write_field_cnt(ESP_EFUSE_SOFT_DIS_JTAG, ESP_EFUSE_SOFT_DIS_JTAG[0]->bit_count)** can be used to burn ``soft JTAG disable`` bits on {IDF_TARGET_NAME}.

.. only:: esp32s2 or esp32s3

    .. note::

      If ``HARD_DIS_JTAG`` eFuse is set, then ``SOFT_DIS_JTAG`` functionality does not work because JTAG is permanently disabled.

.. only:: not esp32s2 and not esp32s3

    .. note::

      If ``DIS_PAD_JTAG`` eFuse is set, then ``SOFT_DIS_JTAG`` functionality does not work because JTAG is permanently disabled.

JTAG enables

1. The key to re-enable JTAG is the output of the HMAC-SHA256 function using the secret key in eFuse and 32 ``0x00`` bytes as the message.
2. Pass this key value when calling the :cpp:func:`esp_hmac_jtag_enable` function from the firmware.
3. To re-disable JTAG in the firmware, reset the system or call :cpp:func:`esp_hmac_jtag_disable`.

For a complete workflow of soft-disabling and re-enabling JTAG, refer to :example:`security/hmac_soft_jtag`. This example demonstrates how to use HMAC to re-enable a soft-disabled JTAG interface, covering steps like generating an HMAC key, burning it to eFuse, and creating token data from the key.

For more details, see **{IDF_TARGET_NAME} Technical Reference Manual** > **HMAC Accelerator (HMAC)** [`PDF <{IDF_TARGET_TRM_EN_URL}#hmac>`__].


Application Outline
-------------------

The following code is an outline of how to set an eFuse key and then use it to calculate an HMAC for software usage.

We use ``esp_efuse_write_key`` to set physical key block 4 in the eFuse for the HMAC module together with its purpose. ``ESP_EFUSE_KEY_PURPOSE_HMAC_UP`` (8) means that this key can only be used for HMAC generation for software usage:

.. code-block:: c

    #include "esp_efuse.h"

    const uint8_t key_data[32] = { ... };

    esp_err_t status = esp_efuse_write_key(EFUSE_BLK_KEY4,
                        ESP_EFUSE_KEY_PURPOSE_HMAC_UP,
                        key_data, sizeof(key_data));

    if (status == ESP_OK) {
        // written key
    } else {
        // writing key failed, maybe written already
    }

Now we can calculate an HMAC for software usage with the saved key through the PSA Crypto API.

.. code-block:: c

    #include "psa/crypto.h"
    #include "psa_crypto_driver_esp_hmac_opaque.h"

    uint8_t hmac[32];
    size_t hmac_length = 0;

    const char *message = "Hello, HMAC!";
    const size_t msg_len = 12;

    // Setup key attributes for ESP-HMAC opaque driver
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_MESSAGE);
    psa_set_key_algorithm(&attributes, PSA_ALG_HMAC(PSA_ALG_SHA_256));
    psa_set_key_type(&attributes, PSA_KEY_TYPE_HMAC);
    psa_set_key_bits(&attributes, 256);
    psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_ESP_HMAC_VOLATILE);

    // Create opaque key reference
    esp_hmac_opaque_key_t opaque_key = {
        .use_km_key = false,
        .efuse_block = EFUSE_BLK_KEY4,
    };

    // Import the opaque key
    psa_key_id_t key_id = 0;
    psa_status_t status = psa_import_key(&attributes, (uint8_t *)&opaque_key,
                                         sizeof(opaque_key), &key_id);
    if (status != PSA_SUCCESS) {
        // Failed to import key
        psa_reset_key_attributes(&attributes);
        return;
    }

    // Compute HMAC
    status = psa_mac_compute(key_id, PSA_ALG_HMAC(PSA_ALG_SHA_256),
                             (uint8_t *)message, msg_len,
                             hmac, sizeof(hmac), &hmac_length);

    // Clean up
    psa_destroy_key(key_id);
    psa_reset_key_attributes(&attributes);

    if (status == PSA_SUCCESS) {
        // HMAC written to hmac now
    } else {
        // failure calculating HMAC
    }

API Reference
-------------

.. include-build-file:: inc/psa_crypto_driver_esp_hmac_opaque_contexts.inc
