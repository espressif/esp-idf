:orphan:

Secure Boot v2
==============

:link_to_translation:`zh_CN:[中文]`

{IDF_TARGET_SBV2_SCHEME:default="RSA-PSS", esp32c2="ECDSA", esp32c6="RSA-PSS or ECDSA", esp32h2="RSA-PSS or ECDSA", esp32p4="RSA-PSS or ECDSA", esp32c5="RSA-PSS or ECDSA"}

{IDF_TARGET_SBV2_KEY:default="RSA-3072", esp32c2="ECDSA-256 or ECDSA-192", esp32c6="RSA-3072, ECDSA-256, or ECDSA-192", esp32h2="RSA-3072, ECDSA-256, or ECDSA-192", esp32p4="RSA-3072, ECDSA-256, or ECDSA-192", esp32c5="ECDSA-256, or ECDSA-192"}

{IDF_TARGET_SECURE_BOOT_OPTION_TEXT:default="", esp32c6="RSA is recommended because of faster verification time. You can choose between RSA and ECDSA scheme from the menu.", esp32h2="RSA is recommended because of faster verification time. You can choose between RSA and ECDSA scheme from the menu.", esp32p4="RSA is recommended because of faster verification time. You can choose between RSA and ECDSA scheme from the menu."}

{IDF_TARGET_ECO_VERSION:default="", esp32="(v3.0 onwards)", esp32c3="(v0.3 onwards)"}

{IDF_TARGET_RSA_TIME:default="", esp32c6="about 2.7 ms", esp32h2="about 4.5 ms", esp32p4="about 2.4 ms"}

{IDF_TARGET_ECDSA_TIME:default="", esp32c6="about 21.5 ms", esp32h2="about 36 ms", esp32p4="about 10.3 ms"}

{IDF_TARGET_CPU_FREQ:default="", esp32c6="160 MHz", esp32h2="96 MHz", esp32p4="360 MHz"}

{IDF_TARGET_SBV2_DEFAULT_SCHEME:default="RSA", esp32c2="ECDSA (v2), esp32c5="ECDSA (v2)"}

{IDF_TARGET_EFUSE_WR_DIS_RD_DIS:default="ESP_EFUSE_WR_DIS_RD_DIS", esp32="ESP_EFUSE_WR_DIS_EFUSE_RD_DISABLE"}

.. important::

    This document is about Secure Boot v2, supported on {IDF_TARGET_NAME} {IDF_TARGET_ECO_VERSION}.

    .. only:: esp32

        For ESP32 before chip revision v3.0, refer to :doc:`secure-boot-v1`. It is recommended to use Secure Boot v2 if you have a chip revision that supports it. Secure Boot v2 is safer and more flexible than Secure Boot V1.

    Secure Boot v2 uses {IDF_TARGET_SBV2_SCHEME} based app and bootloader :ref:`second-stage-bootloader` verification. This document can also be used as a reference for signing apps using the {IDF_TARGET_SBV2_SCHEME} scheme without signing the bootloader.

.. only:: esp32

    ``Secure Boot v2`` and ``RSA App Signing Scheme`` options are available for ESP32 from chip revision v3.0 onwards. To use these options in menuconfig, set :ref:`CONFIG_ESP32_REV_MIN` greater than or equal to `v3.0`.

.. only:: esp32c3

    ``Secure Boot v2`` is available for ESP32-C3 from chip revision v0.3 (ECO3) onwards. To use these options in menuconfig, set :ref:`CONFIG_ESP32C3_REV_MIN` greater than or equal to `v0.3 (ECO3)`.


.. note::

    In this guide, most used commands are in the form of ``idf.py secure-<command>``, which is a wrapper around corresponding ``espsecure.py <command>``. The ``idf.py`` based commands provides more user-friendly experience, although may lack some of the advanced functionality of their ``espsecure.py`` based counterparts.

Background
----------

Secure Boot protects a device from running any unauthorized (i.e., unsigned) code by checking that each piece of software that is being booted is signed. On an {IDF_TARGET_NAME}, these pieces of software include the second stage bootloader and each application binary. Note that the first stage bootloader does not require signing as it is ROM code and thus cannot be changed.

.. only:: esp32 or (SOC_SECURE_BOOT_V2_RSA and not SOC_SECURE_BOOT_V2_ECC)

    An RSA-based Secure Boot verification scheme, i.e., Secure Boot v2, is implemented on {IDF_TARGET_NAME} {IDF_TARGET_ECO_VERSION}.

.. only:: SOC_SECURE_BOOT_V2_ECC and not SOC_SECURE_BOOT_V2_RSA

    An ECC-based Secure Boot verification scheme i.e., Secure Boot v2, has been introduced on {IDF_TARGET_NAME}.

.. only:: SOC_SECURE_BOOT_V2_RSA and SOC_SECURE_BOOT_V2_ECC

    {IDF_TARGET_NAME} has provision to choose a {IDF_TARGET_SBV2_SCHEME} based Secure Boot verification scheme.

The Secure Boot process on {IDF_TARGET_NAME} involves the following steps:

1. The first stage bootloader (i.e. ROM boot), which is residing in ROM, loads the second stage bootloader, and the second stage bootloader's {IDF_TARGET_SBV2_SCHEME} signature is verified. Only if the verification is successful, the second stage bootloader is executed.

2. When the second stage bootloader loads a particular application image, the application's {IDF_TARGET_SBV2_SCHEME} signature is verified. If the verification is successful, the application image is executed.


Advantages
----------

- The {IDF_TARGET_SBV2_SCHEME}'s public key is stored on the device. The corresponding {IDF_TARGET_SBV2_SCHEME} private key is kept at a secret place and is never accessed by the device.

.. only:: esp32 or esp32c2

    - Only one public key can be generated and stored in the chip during manufacturing.

.. only:: SOC_EFUSE_REVOKE_BOOT_KEY_DIGESTS

    - Up to three public keys can be generated and stored in the chip during manufacturing.

    - {IDF_TARGET_NAME} provides the facility to permanently revoke individual public keys. This can be configured conservatively or aggressively.

      - Conservatively: The old key is revoked after the bootloader and application have successfully migrated to a new key.
      - Aggressively: The key is revoked as soon as verification with this key fails.

- The same image format and signature verification method is applied for applications and the software bootloader.

- No secrets are stored on the device. Therefore, it is immune to passive side-channel attacks, e.g., timing or power analysis.


Secure Boot v2 Process
----------------------

This is an overview of the Secure Boot v2 Process. Instructions on how to enable Secure Boot are supplied in section :ref:`secure-boot-v2-howto`.

Secure Boot v2 verifies the bootloader image and application binary images using a dedicated *signature block*. Each image has a separately generated signature block which is appended to the end of the image.

.. only:: esp32

  Only one signature block can be appended to the bootloader or application image in ESP32 chip revision v3.0.

.. only:: esp32c2

  Only one signature block can be appended to the bootloader or application image in {IDF_TARGET_NAME}

.. only:: SOC_EFUSE_REVOKE_BOOT_KEY_DIGESTS

  Up to three signature blocks can be appended to the bootloader or application image in {IDF_TARGET_NAME}.

Each signature block contains a signature of the preceding image as well as the corresponding {IDF_TARGET_SBV2_KEY} public key. For more details about the format, refer to :ref:`signature-block-format`. A digest of the {IDF_TARGET_SBV2_KEY} public key is stored in the eFuse.

The application image is not only verified on every boot but also on each over the air (OTA) update. If the currently selected OTA app image cannot be verified, the bootloader will fall back and look for another correctly signed application image.

The Secure Boot v2 process follows these steps:

1. On startup, the ROM code checks the Secure Boot v2 bit in the eFuse. If Secure Boot is disabled, a normal boot will be executed; if Secure Boot is enabled, the boot will proceed according to the following steps.

2. The ROM code verifies the bootloader's signature block, see :ref:`verify_signature-block`. If this fails, the boot process will be aborted.

3. The ROM code verifies the bootloader image using the raw image data, its corresponding signature block(s), and the eFuse, see :ref:`verify_image`. If this fails, the boot process will be aborted.

4. The ROM code executes the bootloader.

5. The bootloader verifies the application image's signature block, see :ref:`verify_signature-block`. If this fails, the boot process will be aborted.

6. The bootloader verifies the application image using the raw image data, its corresponding signature blocks, and the eFuse, see :ref:`verify_image`. If this fails, the boot process will be aborted. If the verification fails but another application image is found, the bootloader will then try to verify that other image using steps 5 to 7. This repeats until a valid image is found or no other images are found.

7. The bootloader executes the verified application image.


.. _signature-block-format:

Signature Block Format
----------------------

The signature block starts on a 4 KB aligned boundary and has a flash sector of its own. The signature is calculated over all bytes in the image including the padding bytes, see :ref:`secure_padding`.

.. only:: SOC_SECURE_BOOT_V2_RSA and SOC_SECURE_BOOT_V2_ECC

    .. note::

        {IDF_TARGET_NAME} has a provision to choose between the RSA scheme and the ECDSA scheme. Only one scheme can be used per device.

        ECDSA provides similar security strength, compared to RSA, with shorter key lengths. Current estimates are that ECDSA with curve P-256 has an approximate equivalent strength to RSA with 3072-bit keys. However, ECDSA signature verification takes considerably more amount of time as compared to RSA signature verification.

        RSA is recommended for use cases where fast boot-up time is required whereas ECDSA is recommended for use cases where shorter key length is required.

        .. only:: not esp32p4 or not esp32c5

            .. list-table:: Comparison between signature verification time
                :widths: 10 10 20
                :header-rows: 1

                * - **Verification scheme**
                  - **Time**
                  - **CPU Frequency**
                * - RSA-3072
                  - {IDF_TARGET_RSA_TIME}
                  - {IDF_TARGET_CPU_FREQ}
                * - ECDSA-P256
                  - {IDF_TARGET_ECDSA_TIME}
                  - {IDF_TARGET_CPU_FREQ}

          The above table compares the time taken to verify a signature in a particular scheme. It does not indicate the boot-up time.

The content of each signature block is shown in the following table:

.. only:: esp32 or SOC_SECURE_BOOT_V2_RSA

    .. list-table:: Content of a RSA Signature Block
        :widths: 10 10 40
        :header-rows: 1

        * - **Offset**
          - **Size (bytes)**
          - **Description**
        * - 0
          - 1
          - Magic byte.
        * - 1
          - 1
          - Version number byte, currently 0x02, and 0x01 is for Secure Boot V1.
        * - 2
          - 2
          - Padding bytes. Reserved, should be zero.
        * - 4
          - 32
          - SHA-256 hash of only the image content, not including the signature block.
        * - 36
          - 384
          - RSA Public Modulus used for signature verification, value 'n' in RFC8017.
        * - 420
          - 4
          - RSA Public Exponent used for signature verification, value 'e' in RFC8017.
        * - 424
          - 384
          - Pre-calculated R, derived from 'n'.
        * - 808
          - 4
          - Pre-calculated M', derived from 'n'.
        * - 812
          - 384
          - RSA-PSS Signature result (section 8.1.1 of RFC8017) of image content, computed using the following PSS parameters: SHA256 hash, MGF1 function, salt length 32 bytes, default trailer field 0xBC.
        * - 1196
          - 4
          - CRC32 of the preceding 1196 bytes.
        * - 1200
          - 16
          - Zero padding to length 1216 bytes.


    .. note::

      R and M' are used for hardware-assisted Montgomery Multiplication.

.. only:: SOC_SECURE_BOOT_V2_ECC

    .. list-table:: Content of an ECDSA Signature Block
        :widths: 10 10 40
        :header-rows: 1

        * - **Offset**
          - **Size (bytes)**
          - **Description**
        * - 0
          - 1
          - Magic byte.
        * - 1
          - 1
          - Version number byte, currently 0x03.
        * - 2
          - 2
          - Padding bytes. Reserved, should be zero.
        * - 4
          - 32
          - SHA-256 hash of only the image content, not including the signature block.
        * - 36
          - 1
          - Curve ID. 1 for NIST192p curve. 2 for NIST256p curve.
        * - 37
          - 64
          - ECDSA Public key: 32-byte X coordinate followed by 32-byte Y coordinate.
        * - 101
          - 64
          - ECDSA Signature result (section 5.3.2 of RFC6090) of the image content: 32-byte R component followed by-32 byte S component.
        * - 165
          - 1031
          - Reserved.
        * - 1196
          - 4
          - CRC32 of the preceding 1196 bytes.
        * - 1200
          - 16
          - Zero padding to length 1216 bytes.

The remainder of the signature sector is erased flash (0xFF) which allows writing other signature blocks after the previous signature block.


.. _secure_padding:

Secure Padding
--------------

In the Secure Boot v2 scheme, the application image is padded to the flash MMU page size boundary to ensure that only verified contents are mapped in the internal address space, which is known as secure padding. The signature of the image is calculated after padding and then the signature block (4 KB) gets appended to the image.

.. list::

    - Default flash MMU page size is 64 KB
    :SOC_MMU_PAGE_SIZE_CONFIGURABLE: - {IDF_TARGET_NAME} supports configurable flash MMU page size, and ``CONFIG_MMU_PAGE_SIZE`` gets set based on the :ref:`CONFIG_ESPTOOLPY_FLASHSIZE`
    - Secure padding is applied through the option ``--secure-pad-v2`` in the ``elf2image`` conversion using ``esptool.py``

The following table explains the Secure Boot v2 signed image with secure padding and signature block appended:

.. list-table:: Contents of a signed application
        :widths: 20 20 20
        :header-rows: 1

        * - **Offset**
          - **Size (KB)**
          - **Description**
        * - 0
          - 580
          - Unsigned application size, as an example
        * - 580
          - 60
          - Secure padding, aligned to the next 64 KB boundary
        * - 640
          - 4
          - Signature block

.. note::

    Please note that the application image always starts on the next flash MMU page size boundary, default 64 KB, and hence the space left over after the signature block shown above can be utilized to store any other data partitions, e.g., ``nvs``.


.. _verify_signature-block:

Verifying a Signature Block
---------------------------

A signature block is valid if the first byte is ``0xe7`` and a valid CRC32 is stored at offset 1196. Otherwise, it is invalid.


.. _verify_image:

Verifying an Image
------------------

An image is verified if the public key stored in any signature block is valid for this device, and if the signature stored in that signature block matches with the signature calculated for the image data read from flash.

1. Compare the SHA-256 hash digest of the public key embedded in the bootloader's signature block with the digest(s) saved in the eFuses. If the public key's hash does not match any of the hashes from the eFuses, the verification fails.

2. Generate the application image digest and match it with the image digest in the signature block. If the digests do not match, the verification fails.

.. only:: esp32 or (SOC_SECURE_BOOT_V2_RSA and not SOC_SECURE_BOOT_V2_ECC)

    3. Use the public key to verify the signature of the bootloader image, using RSA-PSS (section 8.1.2 of RFC8017) with the image digest calculated in step (2) for comparison.

.. only:: SOC_SECURE_BOOT_V2_ECC and not SOC_SECURE_BOOT_V2_RSA

    3. Use the public key to verify the signature of the bootloader image, using ECDSA signature verification (section 5.3.3 of RFC6090) with the image digest calculated in step (2) for comparison.

.. only:: SOC_SECURE_BOOT_V2_ECC and SOC_SECURE_BOOT_V2_RSA

    3. Use the public key to verify the signature of the bootloader image, using either RSA-PSS (section 8.1.2 of RFC8017) or ECDSA signature verification (section 5.3.3 of RFC6090) with the image digest calculated in step (2) for comparison.


Bootloader Size
---------------

Enabling Secure Boot and/or flash encryption will increase the size of the bootloader, which might require updating the partition table offset. See :ref:`bootloader-size`.

When :ref:`CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES` is disabled, the bootloader will use the ``--pad-to-size`` option in ``elf2image`` command of ``esptool`` for sector padding, with a size of 4 KB per sector.


.. _efuse-usage:

eFuse Usage
-----------

.. only:: esp32

    ESP32 chip revision v3.0:

    - ABS_DONE_1 - Enables Secure Boot protection on boot.

    - BLK2 - Stores the SHA-256 digest of the public key. SHA-256 hash of public key modulus, exponent, pre-calculated R & M' values is written to an eFuse key block. This digest is represented as 776 bytes, with offsets of 36 to 812, as per the :ref:`signature-block-format`. The write-protection bit must be set, but the read-protection bit must not.

.. only:: not esp32

    - SECURE_BOOT_EN - Enables Secure Boot protection on boot.

.. only:: SOC_EFUSE_KEY_PURPOSE_FIELD

    - KEY_PURPOSE_X - Set the purpose of the key block on {IDF_TARGET_NAME} by programming SECURE_BOOT_DIGESTX (X = 0, 1, 2) into KEY_PURPOSE_X (X = 0, 1, 2, 3, 4, 5). Example: If KEY_PURPOSE_2 is set to SECURE_BOOT_DIGEST1, then BLOCK_KEY2 will have the Secure Boot v2 public key digest. The write-protection bit must be set, and this field does not have a read-protection bit.

    - BLOCK_KEYX - The block contains the data corresponding to its purpose programmed in KEY_PURPOSE_X. Stores the SHA-256 digest of the public key is written to an eFuse key block. This digest is represented as 776 bytes, with offsets of 36 to 812, as per the :ref:`signature-block-format`. The write-protection bit must be set, but the read-protection bit must not.

    - KEY_REVOKEX - The revocation bits corresponding to each of the 3 key blocks. E.g., setting KEY_REVOKE2 revokes the key block whose key purpose is SECURE_BOOT_DIGEST2.

    - SECURE_BOOT_AGGRESSIVE_REVOKE - Enables aggressive revocation of keys. The key is revoked as soon as verification with this key fails.

    To ensure no trusted keys can be added later by an attacker, each unused key digest slot should be revoked with KEY_REVOKEX. It will be checked during app startup in :cpp:func:`esp_secure_boot_init_checks` and fixed unless :ref:`CONFIG_SECURE_BOOT_ALLOW_UNUSED_DIGEST_SLOTS` is enabled.

The key(s) must be readable in order to give software access to it. If the key(s) is read-protected then the software reads the key(s) as all zeros and the signature verification process will fail, and the boot process will be aborted.


.. _secure-boot-v2-howto:

How To Enable Secure Boot v2
----------------------------

1. Open the :ref:`project-configuration-menu`, in ``Security features`` set ``Enable hardware Secure Boot in bootloader`` to enable Secure Boot.

.. only:: esp32

    2. For ESP32, Secure Boot v2 is available only ESP32 chip revision v3.0 onwards. To view the ``Secure Boot v2`` option, the chip revision should be changed to ESP32 chip revision v3.0. To change the chip revision, set ``Minimum Supported ESP32 Revision`` to v3.0 in ``Component Config`` > ``ESP32- Specific``.

    3. Specify the path to the Secure Boot signing key, relative to the project directory.

    4. Select the desired UART ROM download mode in ``UART ROM download mode``. By default the UART ROM download mode has been kept enabled in order to prevent permanently disabling it in the development phase, this option is a potentially insecure option. It is recommended to disable the UART download mode for better security.

.. only:: SOC_SECURE_BOOT_V2_RSA or SOC_SECURE_BOOT_V2_ECC

    2. The ``Secure Boot v2`` option will be selected and the ``App Signing Scheme`` will be set to {IDF_TARGET_SBV2_DEFAULT_SCHEME} by default. {IDF_TARGET_SECURE_BOOT_OPTION_TEXT}

    3. Specify the path to the Secure Boot signing key, relative to the project directory.

    4. Select the desired UART ROM download mode in ``UART ROM download mode``. By default, it is set to ``Permanently switch to Secure mode`` which is generally recommended. For production devices, the most secure option is to set it to ``Permanently disabled``.

5. Set other menuconfig options as desired. Then exit menuconfig and save your configuration.

6. The first time you run ``idf.py build``, if the signing key is not found then an error message will be printed with a command to generate a signing key via ``idf.py secure-generate-signing-key``.

.. important::

   A signing key generated this way will use the best random number source available to the OS and its Python installation, which is `/dev/urandom` on OSX/Linux and `CryptGenRandom()` on Windows. If this random number source is weak, then the private key will be weak.

.. important::

   For production environments, we recommend generating the key pair using OpenSSL or another industry-standard encryption program. See :ref:`secure-boot-v2-generate-key` for more details.

7. Run ``idf.py bootloader`` to build a Secure Boot-enabled bootloader. The build output will include a prompt for a flashing command, using ``esptool.py write_flash``.

8. When you are ready to flash the bootloader, run the specified command and then wait for flashing to complete. You have to enter it yourself, this step is not performed by the build system.

9. Run ``idf.py flash`` to build and flash the partition table and the just-built app image. The app image will be signed using the signing key you generated in step 6.

.. note::

  ``idf.py flash`` does not flash the bootloader if Secure Boot is enabled.

10. Reset the {IDF_TARGET_NAME} and it will boot the software bootloader you flashed. The software bootloader will enable Secure Boot on the chip, and then it verifies the app image signature and boots the app. You should watch the serial console output from the {IDF_TARGET_NAME} to verify that Secure Boot is enabled and no errors have occurred due to the build configuration.

.. note::

  Secure Boot will not be enabled until after a valid partition table and app image have been flashed. This is to prevent accidents before the system is fully configured.

.. note::

  If the {IDF_TARGET_NAME} is reset or powered down during the first boot, it will start the process again on the next boot.

11. On subsequent boots, the Secure Boot hardware will verify the software bootloader has not changed and the software bootloader will verify the signed app image using the validated public key portion of its appended signature block.


Restrictions After Secure Boot Is Enabled
-----------------------------------------

- Any updated bootloader or app will need to be signed with a key matching the digest already stored in eFuse.

- Please note that enabling Secure Boot or flash encryption disables the USB-OTG USB stack in the ROM, disallowing updates via the serial emulation or Device Firmware Update (DFU) on that port.

Burning read-protected keys
~~~~~~~~~~~~~~~~~~~~~~~~~~~

After Secure Boot is enabled, no further eFuses can be read-protected, because this might allow an attacker to read-protect the efuse block holding the public key digest, causing an immediate denial of service and possibly allowing an additional fault injection attack to bypass the signature protection.

If :doc:`/security/flash-encryption` is enabled by the 2nd stage bootloader, it ensures that the flash encryption key generated on the first boot shall already be read-protected.

In case you need to read-protect a key after Secure Boot has been enabled on the device, for example,

.. list::
  :SOC_FLASH_ENC_SUPPORTED:* the flash encryption key

  :SOC_HMAC_SUPPORTED:* HMAC keys

  :SOC_ECDSA_SUPPORTED:* ECDSA keys

  :SOC_KEY_MANAGER_SUPPORTED:* Key Manager keys

  you need to enable the config :ref:`CONFIG_SECURE_BOOT_V2_ALLOW_EFUSE_RD_DIS` at the same time when you enable Secure Boot to prevent disabling the ability to read-protect further efuses.

It is highly recommended that all such keys must been burned before enabling secure boot.

In case you need to enable :ref:`CONFIG_SECURE_BOOT_V2_ALLOW_EFUSE_RD_DIS`, make sure that you burn the efuse {IDF_TARGET_EFUSE_WR_DIS_RD_DIS}, using ``esp_efuse_write_field_bit()`` API from ``esp_efuse.h``, once all the read-protected efuse keys have been programmed.


.. _secure-boot-v2-generate-key:

Generating Secure Boot Signing Key
----------------------------------

The build system will prompt you with a command to generate a new signing key via ``idf.py secure-generate-signing-key``.

.. only:: esp32 or SOC_SECURE_BOOT_V2_RSA

   The ``--version 2`` parameter will generate the RSA 3072 private key for Secure Boot v2. Additionally ``--scheme rsa3072`` can be passed as well to generate RSA 3072 private key.

.. only:: SOC_SECURE_BOOT_V2_ECC

   Select the ECDSA scheme by passing ``--version 2 --scheme ecdsa256`` or ``--version 2 --scheme ecdsa192`` to generate corresponding ECDSA private key.

The strength of the signing key is proportional to (a) the random number source of the system, and (b) the correctness of the algorithm used. For production devices, we recommend generating signing keys from a system with a quality entropy source and using the best available {IDF_TARGET_SBV2_SCHEME} key generation utilities.

For example, to generate a signing key using the OpenSSL command line:

.. only:: esp32 or SOC_SECURE_BOOT_V2_RSA

    For RSA 3072

    .. code-block::

      openssl genrsa -out my_secure_boot_signing_key.pem 3072

.. only:: SOC_SECURE_BOOT_V2_ECC

    For the ECC NIST192p curve

    .. code-block::

      openssl ecparam -name prime192v1 -genkey -noout -out my_secure_boot_signing_key.pem

    For the ECC NIST256p curve

    .. code-block::

      openssl ecparam -name prime256v1 -genkey -noout -out my_secure_boot_signing_key.pem

Remember that the strength of the Secure Boot system depends on keeping the signing key private.


.. _remote-sign-v2-image:

Remote Signing of Images
------------------------

Signing Using ``idf.py``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For production builds, it can be good practice to use a remote signing server rather than have the signing key on the build machine (which is the default ESP-IDF Secure Boot configuration). The ``espsecure.py`` command line program can be used to sign app images and partition table data for Secure Boot, on a remote system.

To use remote signing, disable the option :ref:`CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES` and build the firmware. The private signing key does not need to be present on the build system.

After the app image and partition table are built, the build system will print signing steps using ``idf.py``:

.. code-block::

  idf.py secure-sign-data BINARY_FILE --keyfile PRIVATE_SIGNING_KEY

The above command appends the image signature to the existing binary. You can use the `--output` argument to write the signed binary to a separate file:

.. code-block::

  idf.py secure-sign-data --keyfile PRIVATE_SIGNING_KEY --output SIGNED_BINARY_FILE BINARY_FILE


Signing Using Pre-calculated Signatures
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you have valid pre-calculated signatures generated for an image and their corresponding public keys, you can use these signatures to generate a signature sector and append it to the image. Note that the pre-calculated signature should be calculated over all bytes in the image including the secure-padding bytes.

In such cases, the firmware image should be built by disabling the option :ref:`CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES`. This image will be secure-padded and to generate a signed binary use the following command:

.. code-block::

  idf.py secure-sign-data --pub-key PUBLIC_SIGNING_KEY --signature SIGNATURE_FILE --output SIGNED_BINARY_FILE BINARY_FILE

The above command verifies the signature, generates a signature block (refer to :ref:`signature-block-format`), and appends it to the binary file.


Signing Using an External Hardware Security Module (HSM)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For security reasons, you might also use an external Hardware Security Module (HSM) to store your private signing key, which cannot be accessed directly but has an interface to generate the signature of a binary file and its corresponding public key.

In such cases, disable the option :ref:`CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES` and build the firmware. This secure-padded image then can be used to supply the external HSM for generating a signature. Refer to `Signing using an External HSM <https://docs.espressif.com/projects/esptool/en/latest/{IDF_TARGET_PATH_NAME}/espsecure/index.html#remote-signing-using-an-external-hsm>`_ to generate a signed image.

.. only:: SOC_EFUSE_REVOKE_BOOT_KEY_DIGESTS

    .. note::

      For all the above three remote signing workflows, the signed binary is written to the filename provided to the ``--output`` argument, and the option ``--append_signatures`` allows us to append multiple signatures (up to 3) to the image.

.. only:: not SOC_EFUSE_REVOKE_BOOT_KEY_DIGESTS

    .. note::

      For all the above three remote signing workflows, the signed binary is written to the filename provided to the ``--output`` argument.


Secure Boot Best Practices
--------------------------

* Generate the signing key on a system with a quality source of entropy.
* Keep the signing key private at all times. A leak of this key will compromise the Secure Boot system.
* Do not allow any third party to observe any aspects of the key generation or signing process using ``idf.py secure-`` commands. Both processes are vulnerable to timing or other side-channel attacks.
* Enable all Secure Boot options in the Secure Boot Configuration. These include flash encryption, disabling of JTAG, disabling BASIC ROM interpreter, and disabling the UART bootloader encrypted flash access.
* Use Secure Boot in combination with :doc:`flash-encryption` to prevent local readout of the flash contents.

.. only:: SOC_EFUSE_REVOKE_BOOT_KEY_DIGESTS

    Key Management
    --------------

    * Between 1 and 3 {IDF_TARGET_SBV2_KEY} public key pairs (Keys #0, #1, #2) should be computed independently and stored separately.
    * The KEY_DIGEST eFuses should be write-protected after being programmed.
    * The unused KEY_DIGEST slots must have their corresponding KEY_REVOKE eFuse burned to permanently disable them. This must happen before the device leaves the factory.
    * The eFuses can either be written by the software bootloader during first boot after enabling ``Secure Boot v2`` from menuconfig or can be done using ``espefuse.py`` which communicates with the serial bootloader program in ROM.
    * The KEY_DIGESTs should be numbered sequentially beginning at key digest #0. If key digest #1 is used, key digest #0 should be used. If key digest #2 is used, key digest #0 & #1 must be used.
    * The software bootloader is non-OTA upgradeable, and is signed using at least one, possibly all three, private keys and flashed in the factory.
    * Apps should only be signed with a single private key, with the others being stored securely elsewhere. However, they may be signed with multiple private keys if some are being revoked, see :ref:`secure-boot-v2-key-revocation` below.


    Multiple Keys
    -------------

    * The bootloader should be signed with all the private key(s) that are needed for the life of the device, before it is flashed.
    * The build system can sign with at most one private key, user has to run manual commands to append more signatures if necessary.
    * You can use the append functionality of ``idf.py secure-sign-data``, this command would also printed at the end of the Secure Boot V2 enabled bootloader compilation.

    .. code-block::

        idf.py secure-sign-data -k secure_boot_signing_key2.pem --append_signatures -o signed_bootloader.bin build/bootloader/bootloader.bin

    * While signing with multiple private keys, it is recommended that the private keys be signed independently, if possible on different servers and stored separately.
    * You can check the signatures attached to a binary using:

    .. code-block::

        espsecure.py signature_info_v2 datafile.bin

    .. _secure-boot-v2-key-revocation:

    Key Revocation
    --------------

    * Keys are processed in a linear order, i.e., key #0, key #1, key #2.
    * Applications should be signed with only one key at a time, to minimize the exposure of unused private keys.
    * The bootloader can be signed with multiple keys from the factory.

    .. note::

        Note that enabling the config :ref:`CONFIG_SECURE_BOOT_ALLOW_UNUSED_DIGEST_SLOTS` only makes sure that the **app** does not revoke the unused digest slots.
        But if you plan to enable secure boot during the fist boot up, the bootloader will intentionally revoke the unused digest slots while enabling secure boot, even if the above config is enabled. Because keeping the unused key slots unrevoked would be a security hazard.
        In case for any development workflow if you need to avoid this revocation, you should :ref:`enable-secure-boot-v2-externally`, rather than enabling it during the boot up, so that the bootloader would not need to enable secure boot, and thus you could avoid its revocation strategy.

    Conservative Approach
    ~~~~~~~~~~~~~~~~~~~~~~

    Assuming a trusted private key (N-1) has been compromised, to update to new key pair (N).

    1. The server sends an OTA update with an application signed with the new private key (#N).
    2. The new OTA update is written to an unused OTA app partition.
    3. The new application's signature block is validated. The public keys are checked against the digests programmed in the eFuse and the application is verified using the verified public key.
    4. The active partition is set to the new OTA application's partition.
    5. The device resets and loads the bootloader that is verified with key #N-1, which then boots the new app verified with key #N.
    6. The new app verifies the bootloader with key #N as a final check, and then runs code to revoke key #N-1, i.e., sets KEY_REVOKE eFuse bit.
    7. The API `esp_ota_revoke_secure_boot_public_key()` can be used to revoke the key #N-1.

    * A similar approach can also be used to physically re-flash with a new key. For physical re-flashing, the bootloader content can also be changed at the same time.


    .. _secure-boot-v2-aggressive-key-revocation:

    Aggressive Approach
    ~~~~~~~~~~~~~~~~~~~

    ROM code has an additional feature of revoking a public key digest if the signature verification fails.

    To enable this feature, you need to burn ``SECURE_BOOT_AGGRESSIVE_REVOKE`` eFuse or enable :ref:`CONFIG_SECURE_BOOT_ENABLE_AGGRESSIVE_KEY_REVOKE`.

    Key revocation is not applicable unless Secure Boot is successfully enabled. Also, a key is not revoked in case of an invalid signature block or invalid image digest, it is only revoked in case the signature verification fails, i.e., revoke key only if failure in step 3 of :ref:`verify_image`.

    Once a key is revoked, it can never be used for verifying the signature of an image. This feature provides strong resistance against physical attacks on the device. However, this could also brick the device permanently if all the keys are revoked because of signature verification failure.


.. _secure-boot-v2-technical-details:

Technical Details
-----------------

The following sections contain low-level reference descriptions of various Secure Boot elements:


Manual Commands
~~~~~~~~~~~~~~~

Secure Boot is integrated into the ESP-IDF build system, so ``idf.py build`` will sign an app image, and ``idf.py bootloader`` will produce a signed bootloader if :ref:`CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES` is enabled.

However, it is possible to use the ``idf.py`` tool to make standalone signatures and digests.

To sign a binary image:

.. code-block::

  idf.py secure-sign-data --keyfile ./my_signing_key.pem --output ./image_signed.bin image-unsigned.bin

Keyfile is the PEM file containing an {IDF_TARGET_SBV2_KEY} private signing key.


.. _secure-boot-v2-and-flash-encr:

Secure Boot & Flash Encryption
------------------------------

If Secure Boot is used without :doc:`flash-encryption`, it is possible to launch a ``time-of-check to time-of-use`` attack, where flash contents are swapped after the image is verified and running. Therefore, it is recommended to use both features together.

.. only:: esp32c2

    .. important::

       {IDF_TARGET_NAME} has only one eFuse key block, which is used for both keys: Secure Boot and Flash Encryption. The eFuse key block can only be burned once. Therefore these keys should be burned together at the same time. Please note that ``Secure Boot`` and ``Flash Encryption`` can not be enabled separately as subsequent writes to the eFuse key block shall return an error.


.. _signed-app-verify-v2:

Signed App Verification Without Hardware Secure Boot
----------------------------------------------------

The Secure Boot v2 signature of apps can be verified during an OTA update without the need to enable the hardware Secure Boot option. This approach utilizes the same app signature scheme as Secure Boot v2. However, unlike hardware Secure Boot, Software secure boot does not provide protection against an attacker with write access to flash memory, who could potentially bypass the signature verification.

This may be desirable in cases where the delay of Secure Boot verification on startup is unacceptable, and/or where the threat model does not include physical access or attackers writing to the bootloader or app partitions in flash.

In this mode, the public key that is present in the signature block of the currently running app will be used to verify the signature of a newly updated app. The signature on the running app is not verified during the update process, it is assumed to be valid. In this way, the system creates a chain of trust from the running app to the newly updated app.

For this reason, it is essential that the initial app flashed to the device is also signed. Upon startup, the application checks for signatures. If no valid signatures are found, the app will abort and no updates can be applied. This is done in order to prevent a situation where no further updates are possible and the device shall be bricked. The app should have only one valid signature block in the first position. Note again that, unlike hardware Secure Boot v2, the signature of the running app is not verified on boot. The system only verifies a signature block in the first position and ignores any other appended signatures.

.. only:: not esp32

    Although multiple trusted keys are supported when using hardware Secure Boot, only the first public key in the signature block is used to verify updates if signature checking without Secure Boot is configured. If multiple trusted public keys are required, it is necessary to enable the full Secure Boot feature instead.

.. note::

    In general, it is recommended to use full hardware Secure Boot unless certain that this option is sufficient for application security needs.


.. _signed-app-verify-v2-howto:

How To Enable Signed App Verification
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

1. Open :ref:`project-configuration-menu` > ``Security features``.

.. only:: esp32

    2. Ensure ``App Signing Scheme`` is ``RSA``. For the ESP32 chip revision v3.0 chip, select :ref:`CONFIG_ESP32_REV_MIN` to ``v3.0`` to get the ``RSA`` option available.

.. only:: SOC_SECURE_BOOT_V2_RSA and not SOC_SECURE_BOOT_V2_ECC

    2. Ensure ``App Signing Scheme`` is ``RSA``.

.. only:: SOC_SECURE_BOOT_V2_ECC and not SOC_SECURE_BOOT_V2_RSA

    2. Ensure ``App Signing Scheme`` is ``ECDSA (v2)``.

.. only:: SOC_SECURE_BOOT_V2_RSA and SOC_SECURE_BOOT_V2_ECC

    2. Choose ``App Signing Scheme``. Either ``RSA`` or ``ECDSA (v2)``.


3. Enable :ref:`CONFIG_SECURE_SIGNED_APPS_NO_SECURE_BOOT`.

4. By default, ``Sign binaries during build`` will be enabled by selecting the ``Require signed app images`` option, which will sign binary files as a part of the build process. The file named in ``Secure Boot private signing key`` will be used to sign the image.

5. If you disable the ``Sign binaries during build`` option then all app binaries must be manually signed by following instructions in :ref:`remote-sign-v2-image`.

.. warning::

    It is very important that all apps flashed have been signed, either during the build or after the build.


Advanced Features
-----------------

JTAG Debugging
~~~~~~~~~~~~~~

By default, when Secure Boot is enabled, JTAG debugging is disabled via eFuse. The bootloader does this on the first boot, at the same time it enables Secure Boot.

See :ref:`jtag-debugging-security-features` for more information about using JTAG Debugging with either Secure Boot or signed app verification enabled.
