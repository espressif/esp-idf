Secure Boot V2
==============

.. important::

    The references in this document are related to Secure Boot V2, the preferred scheme from ESP32-ECO3 onwards and in ESP32-S2.

    .. only:: esp32

        Refer also to :doc:`Secure Boot <secure-boot-v1>` for ESP32.

    Secure Boot V2 uses RSA based app and bootloader verification. This document can also be referred for signing apps with the RSA scheme without signing the bootloader.

Background
----------

Secure Boot protects a device from running unsigned code (verification at time of load). A new RSA based secure boot
verification scheme (Secure Boot V2) has been introduced for ESP32-S2 and ESP32 ECO3 onwards.

- The software bootloader’s RSA-PSS signature is verified by the Mask ROM and it is executed post successful verification.
- The verified software bootloader verifies the RSA-PSS signature of the application image before it is executed.

Advantages
----------

- The RSA public key is stored on the device. The corresponding RSA private key is kept secret on a server and is never accessed by the device.

  .. only:: esp32

    - Only one public key can be generated and stored in ESP32 ECO3 during manufacturing.

  .. only:: esp32s2

    - Up to three public keys can be generated and stored in the chip during manufacturing.

    - {IDF_TARGET_NAME} provides the facility to permanently revoke individual public keys. This can be configured conservatively or aggressively.

    - Conservatively - The old key is revoked after the bootloader and application have successfully migrated to a new key. Aggressively - The key is revoked as soon as verification with this key fails.

- Same image format & signature verification is applied for applications & software bootloader.

-  No secrets are stored on the device. Therefore immune to passive side-channel attacks (timing or power analysis, etc.)


Secure Boot V2 Process
----------------------

This is an overview of the Secure Boot V2 Process, Step by step instructions are supplied under :ref:`secure-boot-v2-howto`.

1. Secure Boot V2 verifies the signature blocks appended to the bootloader and application binaries. The signature block contains the image binary signed by a RSA-3072 private key and its corresponding public key. More details on the :ref:`signature-block-format`.

2. On startup, ROM code checks the Secure Boot V2  bit in eFuse.

3. If secure boot is enabled, ROM checks the SHA-256 of the public key in the signature block in the eFuse.

4. The ROM code validates the public key embedded in the software bootloader's signature block by matching the SHA-256 of its public key to the SHA-256 in eFuse as per the earlier step. Boot process will be aborted if a valid hash of the public key isn’t found in the eFuse.

5. The ROM code verifies the signature of the bootloader with the pre-validated public key with the RSA-PSS Scheme. In depth information on :ref:`verify_signature-block`.

6. Software bootloader, reads the app partition and performs similar verification on the application. The application is verified on every boot up and OTA update. If selected OTA app partition fails verification, bootloader will fall back and look for another correctly signed partition.

.. _signature-block-format:

Signature Block Format
----------------------

The bootloader and application images are padded to the next 4096 byte boundary, thus the signature has a flash sector of its own. The signature is calculated over all bytes in the image including the padding bytes.

Each signature block contains the following:

* **Offset 0 (1 byte):** Magic byte (0xe7)

* **Offset 1 (1 byte):** Version number byte (currently 0x02), 0x01 is for Secure Boot V1.

* **Offset 2 (2 bytes):** Padding bytes, Reserved. Should be zero.

* **Offset 4 (32 bytes):** SHA-256 hash of only the image content, not including the signature block.

* **Offset 36 (384 bytes):** RSA Public Modulus used for signature verification. (value ‘n’ in RFC8017).

* **Offset 420 (4 bytes):** RSA Public Exponent used for signature verification (value ‘e’ in RFC8017).

* **Offset 424 (384 bytes):** Precalculated R, derived from ‘n’.

* **Offset 808 (4 bytes):** Precalculated M’, derived from ‘n’

* **Offset 812 (384 bytes):** RSA-PSS Signature result (section 8.1.1 of RFC8017) of image content, computed using following PSS parameters: SHA256 hash, MFG1 function, 0 length salt, default trailer field (0xBC).

* **Offset 1196:** CRC32 of the preceding 1095 bytes.

* **Offset 1200 (16 bytes):** Zero padding to length 1216 bytes.

.. note::
  R and M' are used for hardware-assisted Montgomery Multiplication.

The remainder of the signature sector is erased flash (0xFF) which allows writing other signature blocks after previous signature block.

.. _verify_signature-block:

Verifying the signature Block
-----------------------------

A signature block is “valid” if the first byte is 0xe7 and a valid CRC32 is stored at offset 1196.

  .. only:: esp32

    Only one signature block can be appended to the bootloader or application image in ESP32 ECO3.

  .. only:: esp32s2

    Upto 3 signature blocks can be appended to the bootloader or application image in {IDF_TARGET_NAME}.

An image is “verified” if the public key stored in any signature block is valid for this device, and if the stored signature is valid for the image data read from flash.

1. The magic byte, signature block CRC is validated.

2. Public key digests are generated per signature block and compared with the digests from eFuse. If none of the digests match, the verification process is aborted.

3. The application image digest is generated and matched with the image digest in the signature blocks. The verification process is aborted is the digests don't match.

4. The public key is used to verify the signature of the bootloader image, using RSA-PSS (section 8.1.2 of RFC8017) with the image digest calculated in step (3) for comparison.

- The application signing scheme is set to RSA for Secure Boot V2 and to ECDSA for Secure Boot V1.

.. important::
  It is recommended to use Secure Boot V2 on the chip versions supporting them.

Bootloader Size
---------------

Enabling Secure boot and/or flash encryption will increase the size of bootloader, which might require updating partition table offset. See :ref:`secure-boot-bootloader-size`.

.. _efuse-usage:

eFuse usage
-----------

.. only:: esp32

    ESP32-ECO3:

    - ABS_DONE_1 - Enables secure boot protection on boot.

    - BLK2 - Stores the SHA-256 digest of the public key. SHA-256 hash of public key modulus, exponent, precalculated R & M’ values (represented as 776 bytes – offsets 36 to 812 - as per the :ref:`signature-block-format`) is written to an eFuse key block.

.. only:: esp32s2

    - SECURE_BOOT_EN - Enables secure boot protection on boot.

    - KEY_PURPOSE_X - Set the purpose of the key block on {IDF_TARGET_NAME} by programming SECURE_BOOT_DIGESTX (X = 0, 1, 2) into KEY_PURPOSE_X (X = 0, 1, 2, 3, 4, 5). Example: If KEY_PURPOSE_2 is set to SECURE_BOOT_DIGEST1, then BLOCK_KEY2 will have the Secure Boot V2 public key digest.

    - BLOCK_KEYX - The block contains the data corresponding to its purpose programmed in KEY_PURPOSE_X. Stores the SHA-256 digest of the public key. SHA-256 hash of public key modulus, exponent, precalculated R & M’ values (represented as 776 bytes – offsets 36 to 812 - as per the :ref:`signature-block-format`) is written to an eFuse key block.

    - KEY_REVOKEX - The revocation bits corresponding to each of the 3 key block. Ex. Setting KEY_REVOKE2 revokes the key block whose key purpose is SECURE_BOOT_DIGEST2.

    - SECURE_BOOT_AGGRESSIVE_REVOKE - Enables aggressive revocation of keys. The key is revoked as soon as verification with this key fails.

.. _secure-boot-v2-howto:

How To Enable Secure Boot V2
----------------------------

1. Open the :ref:`project-configuration-menu`, in "Security features" set "Enable hardware Secure Boot in bootloader" to enable Secure Boot.

.. only:: esp32

    2. For ESP32, Secure Boot V2 is available only ESP32 ECO3 onwards. To view the "Secure Boot V2" option the chip revision should be changed to revision 3 (ESP32- ECO3). To change the chip revision, set "Minimum Supported ESP32 Revision" to Rev 3 in "Component Config" -> "ESP32- Specific".

    3. Specify the path to secure boot signing key, relative to the project directory.

.. only:: esp32s2

    2. The "Secure Boot V2" option will be selected and the "App Signing Scheme" would be set to RSA by default.

    3. Specify the path to secure boot signing key, relative to the project directory.

4. Set other menuconfig options (as desired). Pay particular attention to the "Bootloader Config" options, as you can only flash the bootloader once. Then exit menuconfig and save your configuration.

5. The first time you run ``make`` or ``idf.py build``, if the signing key is not found then an error message will be printed with a command to generate a signing key via ``espsecure.py generate_signing_key``.

.. important::
   A signing key generated this way will use the best random number source available to the OS and its Python installation (`/dev/urandom` on OSX/Linux and `CryptGenRandom()` on Windows). If this random number source is weak, then the private key will be weak.

.. important::
   For production environments, we recommend generating the keypair using openssl or another industry standard encryption program. See :ref:`secure-boot-v2-generate-key` for more details.

6. Run ``idf.py bootloader`` to build a secure boot enabled bootloader. The build output will include a prompt for a flashing command, using ``esptool.py write_flash``.

7. When you're ready to flash the bootloader, run the specified command (you have to enter it yourself, this step is not performed by the build system) and then wait for flashing to complete.

8. Run ``idf.py flash`` to build and flash the partition table and the just-built app image. The app image will be signed using the signing key you generated in step 4.

.. note:: ``idf.py flash`` doesn't flash the bootloader if secure boot is enabled.

9. Reset the {IDF_TARGET_NAME} and it will boot the software bootloader you flashed. The software bootloader will enable secure boot on the chip, and then it verifies the app image signature and boots the app. You should watch the serial console output from the {IDF_TARGET_NAME} to verify that secure boot is enabled and no errors have occurred due to the build configuration.

.. note:: Secure boot won't be enabled until after a valid partition table and app image have been flashed. This is to prevent accidents before the system is fully configured.

.. note:: If the {IDF_TARGET_NAME} is reset or powered down during the first boot, it will start the process again on the next boot.

10. On subsequent boots, the secure boot hardware will verify the software bootloader has not changed and the software bootloader will verify the signed app image (using the validated public key portion of its appended signature block).

Restrictions after Secure Boot is enabled
-----------------------------------------

- Any updated bootloader or app will need to be signed with a key matching the digest already stored in efuse.

- After Secure Boot is enabled, no further efuses can be read protected. (If :doc:`/security/flash-encryption` is enabled then the bootloader will ensure that any flash encryption key generated on first boot will already be read protected.) If :ref:`CONFIG_SECURE_BOOT_INSECURE` is enabled then this behaviour can be disabled, but this is not recommended.

.. _secure-boot-v2-generate-key:

Generating Secure Boot Signing Key
----------------------------------

The build system will prompt you with a command to generate a new signing key via ``espsecure.py generate_signing_key``. The --version 2 parameter will generate the RSA 3072 private key for Secure Boot V2.

The strength of the signing key is proportional to (a) the random number source of the system, and (b) the correctness of the algorithm used. For production devices, we recommend generating signing keys from a system with a quality entropy source, and using the best available RSA key generation utilities.

For example, to generate a signing key using the openssl command line:

```
openssl genrsa -out my_secure_boot_signing_key.pem 3072
```

Remember that the strength of the secure boot system depends on keeping the signing key private.

.. _remote-sign-v2-image:

Remote Signing of Images
------------------------

For production builds, it can be good practice to use a remote signing server rather than have the signing key on the build machine (which is the default esp-idf secure boot configuration). The espsecure.py command line program can be used to sign app images & partition table data for secure boot, on a remote system.

To use remote signing, disable the option "Sign binaries during build". The private signing key does not need to be present on the build system.

After the app image and partition table are built, the build system will print signing steps using espsecure.py::

  espsecure.py sign_data --version 2 --keyfile PRIVATE_SIGNING_KEY BINARY_FILE

The above command appends the image signature to the existing binary. You can use the `--output` argument to write the signed binary to a separate file::

  espsecure.py sign_data --version 2 --keyfile PRIVATE_SIGNING_KEY --output SIGNED_BINARY_FILE BINARY_FILE

Secure Boot Best Practices
--------------------------

* Generate the signing key on a system with a quality source of entropy.
* Keep the signing key private at all times. A leak of this key will compromise the secure boot system.
* Do not allow any third party to observe any aspects of the key generation or signing process using espsecure.py. Both processes are vulnerable to timing or other side-channel attacks.
* Enable all secure boot options in the Secure Boot Configuration. These include flash encryption, disabling of JTAG, disabling BASIC ROM interpeter, and disabling the UART bootloader encrypted flash access.
* Use secure boot in combination with :doc:`flash encryption<flash-encryption>` to prevent local readout of the flash contents.

.. only:: esp32s2

    Key Management
    --------------

    * Between 1 and 3 RSA-3072 public keypairs (Keys #0, #1, #2) should be computed independently and stored separately.
    * The KEY_DIGEST efuses should be write protected after being programmed.
    * The unused KEY_DIGEST slots must have their corresponding KEY_REVOKE efuse burned to permanently disable them. This must happen before the device leaves the factory.
    * The eFuses can either be written by the software bootloader during during first boot after enabling "Secure Boot V2" from menuconfig or can be done using `espefuse.py` which communicates with the serial bootloader program in ROM.
    * The KEY_DIGESTs should be numbered sequentially beginning at key digest #0. (ie if key digest #1 is used, key digest #0 should be used. If key digest #2 is used, key digest #0 & #1 must be used.)
    * The software bootloader (non OTA upgradeable) is signed using at least one, possibly all three, private keys and flashed in the factory.
    * Apps should only be signed with a single private key (the others being stored securely elsewhere), however they may be signed with multiple private keys if some are being revoked (see Key Revocation, below).

    Multiple Keys
    -------------

    * The bootloader should be signed with all the private key(s) that are needed for the life of the device, before it is flashed.
    * The build system can sign with at most one private key, user has to run manual commands to append more signatures if necessary.
    * You can use the append functionality of ``espsecure.py``, this command would also printed at the end of the Secure Boot V2 enabled bootloader compilation.
        espsecure.py sign_data -k secure_boot_signing_key2.pem -v 2 --append_signatures -o signed_bootloader.bin build/bootloader/bootloader.bin
    * While signing with multiple private keys, it is recommended that the private keys be signed independently, if possible on different servers and stored separately.
    * You can check the signatures attached to a binary using -
        espsecure.py signature_info_v2 datafile.bin

    Key Revocation
    --------------

    * Keys are processed in a linear order. (key #0, key #1, key #2).
    * Applications should be signed with only one key at a time, to minimise the exposure of unused private keys.
    * The bootloader can be signed with multiple keys from the factory.

    Assuming a trusted private key (N-1) has been compromised, to update to new keypair (N).

    1. Server sends an OTA update with an application signed with the new private key (#N).
    2. The new OTA update is written to an unused OTA app partition.
    3. The new application's signature block is validated. The public keys are checked against the digests programmed in the eFuse & the application is verified using the verified public key.
    4. The active partition is set to the new OTA application's partition.
    5. Device resets, loads the bootloader (verified with key #N-1) which then boots new app (verified with key #N).
    6. The new app verifies bootloader with key #N (as a final check) and then runs code to revoke key #N-1 (sets KEY_REVOKE efuse bit).
    7. The API `esp_ota_revoke_secure_boot_public_key()` can be used to revoke the key #N-1.

    * A similiar approach can also be used to physically reflash with a new key. For physical reflashing, the bootloader content can also be changed at the same time.

.. _secure-boot-v2-technical-details:

Technical Details
-----------------

The following sections contain low-level reference descriptions of various secure boot elements:

Manual Commands
~~~~~~~~~~~~~~~

Secure boot is integrated into the esp-idf build system, so ``make`` or ``idf.py build`` will sign an app image and ``idf.py bootloader`` will produce a signed bootloader if secure signed binaries on build is enabled.

However, it is possible to use the ``espsecure.py`` tool to make standalone signatures and digests.

To sign a binary image::

  espsecure.py sign_data --version 2 --keyfile ./my_signing_key.pem --output ./image_signed.bin image-unsigned.bin

Keyfile is the PEM file containing an RSA-3072 private signing key.

.. _secure-boot-v2-and-flash-encr:

Secure Boot & Flash Encryption
------------------------------

If secure boot is used without :doc:`Flash Encryption <flash-encryption>`, it is possible to launch "time-of-check to time-of-use" attack, where flash contents are swapped after the image is verified and running. Therefore, it is recommended to use both the features together.

Advanced Features
-----------------

JTAG Debugging
~~~~~~~~~~~~~~

By default, when Secure Boot is enabled then JTAG debugging is disabled via eFuse. The bootloader does this on first boot, at the same time it enables Secure Boot.

See :ref:`jtag-debugging-security-features` for more information about using JTAG Debugging with either Secure Boot or signed app verification enabled.
