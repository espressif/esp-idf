Digital Signature
=================

The Digital Signature (DS) module provides hardware acceleration of signing messages based on RSA.
It uses pre-encrypted parameters to calculate a signature.
The parameters are encrypted using HMAC as a key-derivation function.
In turn, the HMAC uses eFuses as input key.
The whole process happens in hardware so that neither the decryption key for the RSA parameters nor the input key for the HMAC key derivation function can be seen by the software while calculating the signature.

For more detailed information on the hardware involved in signature calculation and the registers used, see *{IDF_TARGET_NAME} Technical Reference Manual* > *Digital Signature (DS)* [`PDF <{IDF_TARGET_TRM_EN_URL}#digsig>`__].


Private Key Parameters
----------------------
The private key parameters for the RSA signature are stored in flash.
To prevent unauthorized access, they are AES-encrypted.
The HMAC module is used as a key-derivation function to calculate the AES encryption key for the private key parameters.
In turn, the HMAC module uses a key from the eFuses key block which can be read-protected to prevent unauthorized access as well.

Upon signature calculation invocation, the software only specifies which eFuse key to use, the corresponding eFuse key purpose, the location of the encrypted RSA parameters and the message.

Key Generation
--------------
Both the HMAC key and the RSA private key have to be created and stored before the DS peripheral can be used.
This needs to be done in software on the {IDF_TARGET_NAME} or alternatively on a host.
For this context, the IDF provides :cpp:func:`esp_efuse_write_block` to set the HMAC key and :cpp:func:`esp_hmac_calculate` to encrypt the private RSA key parameters.

You can find instructions on how to calculate and assemble the private key parameters in *{IDF_TARGET_NAME} Technical Reference Manual* > *Digital Signature (DS)* [`PDF <{IDF_TARGET_TRM_EN_URL}#digsig>`__].

Signature Calculation with IDF
------------------------------

For more detailed information on the workflow and the registers used, see *{IDF_TARGET_NAME} Technical Reference Manual* > *Digital Signature (DS)* [`PDF <{IDF_TARGET_TRM_EN_URL}#digsig>`__].

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

Configure the DS peripheral for a TLS connection
------------------------------------------------

The DS peripheral on {IDF_TARGET_NAME} chip must be configured before it can be used for a TLS connection.
The configuration involves the following steps -

1) Randomly generate a 256 bit value called the `Initialization Vector` (IV).
2) Randomly generate a 256 bit value called  the `HMAC_KEY`.
3) Calculate the encrypted private key paramters from the client private key (RSA) and the parameters generated in the above steps.
4) Then burn the 256 bit `HMAC_KEY` on the efuse, which can only be read by the DS peripheral.

For more details, see *{IDF_TARGET_NAME} Technical Reference Manual* > *Digital Signature (DS)* [`PDF <{IDF_TARGET_TRM_EN_URL}#digsig>`__].

To configure the DS peripheral for development purposes, you can use the python script :example_file:`configure_ds.py<protocols/mqtt/ssl_ds/configure_ds.py>`.
More details about the `configure_ds.py` script can be found at :example_file:`mqtt example README <protocols/mqtt/ssl_ds/README.md>`.

The encrypted private key parameters obtained after the DS peripheral configuration are then to be kept in flash. Furthermore, they are to be passed to the DS peripheral which makes use of those parameters for the Digital Signature operation.
:doc:`Non Volatile Storage<../storage/nvs_flash>` can be used to store the encrypted private key parameters in flash.
The script :example_file:`configure_ds.py<protocols/mqtt/ssl_ds/configure_ds.py>` creates an NVS partition for the encrypted private key parameters. Then the script flashes this partition onto the {IDF_TARGET_NAME}.
The application then needs to read the DS data from NVS, which can be done with the function `esp_read_ds_data_from_nvs` in file :example_file:`ssl_mutual_auth/main/app_main.c <protocols/mqtt/ssl_mutual_auth/main/app_main.c>`

The process of initializing the DS peripheral and then performing the Digital Signature operation is done internally with help of `ESP-TLS`. Please refer to `Digital Signature with ESP-TLS` in :doc:`ESP-TLS <../protocols/esp_tls>` for more details.
As mentioned in the `ESP-TLS` documentation, the application only needs to provide the encrypted private key parameters to the esp_tls context (as `ds_data`), which internally performs
all necessary operations for initializing the DS peripheral and then performing the DS operation.

Example for SSL Mutual Authentication using DS
----------------------------------------------
The example :example:`ssl_ds<protocols/mqtt/ssl_ds>` shows how to use the DS peripheral for mutual authentication. The example uses `mqtt_client` (Implemented through `ESP-MQTT`)
to connect to broker test.mosquitto.org using ssl transport with mutual authentication. The ssl part is internally performed with `ESP-TLS`.
See :example_file:`example README<protocols/mqtt/ssl_ds/README.md>` for more details.

API Reference
-------------

.. include-build-file:: inc/esp_ds.inc
