Digital Signature
=================

The Digital Signature (DS) module provides hardware acceleration of signing messages based on RSA.
It uses pre-encrypted parameters to calculate a signature.
The parameters are encrypted using HMAC as a key-derivation function.
In turn, the HMAC uses eFuses as input key.
The whole process happens in hardware so that neither the decryption key for the RSA parameters nor the input key for the HMAC key derivation function can be seen by the software while calculating the signature.

Look into the `{IDF_TARGET_NAME} Technical Reference Manual <{IDF_TARGET_TRM_EN_URL}>`_ (PDF) for more detailed information about the involved hardware during the signature calculation process and the used registers.

Private Key Parameters
----------------------
The private key parameters for the RSA signature are stored in flash.
To prevent unauthorized access, they are AES-encrypted.
The HMAC module is used as a key-derivation function to calculate the AES encryption key for the private key parameters.
In turn, the HMAC module uses a key from the eFuses key block which can be read-protected to prevent unauthorized access as well.

Upon signature calculation invocation, the software only specifies which eFuse key to use, the corresponding eFuse key purpose, the location of the encrypted RSA parameters and the message.

Key Generation
--------------
Both the HMAC key and the RSA private key have to be created and stored before the DS module can be used.
This needs to be done in software on the {IDF_TARGET_NAME} or alternatively on a host.
For this context, the IDF provides :cpp:func:`esp_efuse_write_block` to set the HMAC key and :cpp:func:`esp_hmac_calculate` to encrypt the private RSA key parameters.

Instructions on how to calculate and assemble the private key parameters are described in the `{IDF_TARGET_NAME} Technical Reference Manual <{IDF_TARGET_TRM_EN_URL}>`_.

Signature Calculation with IDF
------------------------------
*For thorough information about involved registers and the workflow, please have a look at the* `{IDF_TARGET_NAME} Technical Reference Manual <{IDF_TARGET_TRM_EN_URL}>`_.

Three parameters need to be prepared to calculate the digital signature:

#. the eFuse key block ID which is used as key for the HMAC,
#. the location of the encrypted private key parameters,
#. and the message to be signed.

Since the signature calculation takes some time, there are two possible API versions to use in IDF.
The first one is :cpp:func:`esp_ds_sign` and simply blocks until the calculation is finished.
If software needs to do something else during the calculation, :cpp:func:`esp_ds_start_sign` can be called, followed by periodic calls to :cpp:func:`esp_ds_is_busy` to check when the calculation has finished.
Once the calculation has finished, :cpp:func:`esp_ds_finish_sign` can be called to get the resulting signature.

.. note::
    Note that this is only the basic DS building block, the message length is fixed.
    To create signatures of arbitrary messages, the input is normally a hash of the actual message, padded up to the required length.
    An API to do this is planned in the future.

API Reference
-------------

.. include-build-file:: inc/esp_ds.inc
