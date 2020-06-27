HMAC
====

The HMAC (Hash-based Message Authentication Code) module provides hardware acceleration for SHA256-HMAC generation using a key burned into an eFuse block.
HMACs work with pre-shared secret keys and provide authenticity and integrity to a message.

Look into the `{IDF_TARGET_NAME} Technical Reference Manual <{IDF_TARGET_TRM_EN_URL}>`_ (PDF) for more detailed information about the application workflow and the HMAC calculation process.

Generalized Application Scheme
------------------------------
Let there be two parties, A and B. They want to verify the authenticity and integrity of messages sent between each other.
Before they can start sending messages, they need to exchange the secret key via a secure channel.
To verify A's messages, B can do the following:

- A calculates the HMAC of the message it wants to send.
- A sends the message and the HMAC to B.
- B calculates HMAC of the received message itself.
- B checks wether the received and calculated HMACs match.
  If they do match, the message is authentic.

However, the HMAC itself isn't bound to this use case.
It can also be used for challenge-response protocols supporting HMAC or as a key input for further security modules (see below), etc.

HMAC on the {IDF_TARGET_NAME}
-----------------------------
On the {IDF_TARGET_NAME}, the HMAC module works with a secret key burnt into the eFuses.
This eFuse key can be made completely inaccessible for any resources outside the cryptographic modules, thus avoiding key leakage.

Furthermore, the {IDF_TARGET_NAME} has three different application scenarios for its HMAC module:

#. HMAC is generated for software use
#. HMAC is used as a key for the Digital Signature (DS) module
#. HMAC is used for enabling the soft-disabled JTAG interface

The first mode is also called *Upstream* mode, while the last two modes are also called *Downstream* modes.

eFuse Keys for HMAC
^^^^^^^^^^^^^^^^^^^
Six physical eFuse blocks can be used as keys for the HMAC module: block 4 up to block 9.
The enum :cpp:enum:`hmac_key_id_t` in the API maps them to `HMAC_KEY0 ... HMAC_KEY5`.
Each key has a corresponding eFuse parameter *key purpose* determining for which of the three HMAC application scenarios (see below) the key may be used:

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

To calculate an HMAC, the software has to provide the ID of the key block containing the secret key as well as the *key purpose* (see chapter *eFuse Controller* in the `{IDF_TARGET_NAME} Technical Reference Manual <{IDF_TARGET_TRM_EN_URL}>`_).
Before the HMAC key calculation, the HMAC module looks up the purpose of the provided key block. 
The calculation only proceeds if the provided key purpose matches the purpose stored in the eFuses of the key block provided by the ID.

HMAC Generation for Software
^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Key Purpose value: 8

In this case, the HMAC is given out to the software (e.g. to authenticate a message).

The API to calculate the HMAC is :cpp:func:`esp_hmac_calculate`.
Only the message, message length and the eFuse key block ID have to be provided to that function.
The rest, like setting the key purpose, is done automatically.

HMAC for Digital Signature
^^^^^^^^^^^^^^^^^^^^^^^^^^
Key Purpose values: 7, 5

The HMAC can be used as a key derivation function to decrypt private key parameters which are used by the Digital Signature module.
A standard message is used by the hardware in that case.
The user only needs to provide the eFuse key block and purpose on the HMAC side (additional parameters are required for the Digital Signature component in that case).
Neither the key nor the actual HMAC are ever exposed to outside the HMAC module and DS component.
The calculation of the HMAC and its hand-over to the DS component happen internally.

For more details, check the chapter *Digital Signature* in the `{IDF_TARGET_NAME} Technical Reference Manual <{IDF_TARGET_TRM_EN_URL}>`_.

HMAC for Enabling JTAG
^^^^^^^^^^^^^^^^^^^^^^
Key Purpose values: 6, 5

The third application is using the HMAC as a key to enable JTAG if it was soft-disabled before.
This functionality is currently not implemented.

Application Outline
-------------------

Following code is an outline of how to set an eFuse key and then use it to calculate an HMAC for software usage.
We use `ets_efuse_write_key` to set physical key block 4 in the eFuse for the HMAC module together with its purpose.
`ETS_EFUSE_KEY_PURPOSE_HMAC_UP` (8) means that this key can only be used for HMAC generation for software usage:

.. code-block:: c

    #include "esp32s2/rom/efuse.h"

    const uint8_t key_data[32] = { ... };

    int ets_status = ets_efuse_write_key(ETS_EFUSE_BLOCK_KEY4,
                        ETS_EFUSE_KEY_PURPOSE_HMAC_UP,
                        key_data, sizeof(key_data));

    if (ets_status == ESP_OK) {
        // written key
    } else {
        // writing key failed, maybe written already
    }

Now we can use the saved key to calculate an HMAC for software usage.

.. code-block:: c

    #include "esp_hmac.h"

    uint8_t hmac[32];

    const char *message = "Hello, HMAC!";
    const size_t msg_len = 12;

    esp_err_t result = esp_hmac_calculate(HMAC_KEY4, message, msg_len, hmac);

    if (result == ESP_OK) {
        // HMAC written to hmac now
    } else {
        // failure calculating HMAC
    }

API Reference
-------------

.. include-build-file:: inc/esp_hmac.inc
