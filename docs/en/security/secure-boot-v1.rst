Secure Boot
===========

.. important::

    All references in this document are related to Secure Boot V1 (The AES based Secure Boot Scheme). ESP32 Revision 3 onwards, the preferred secure boot scheme is :doc:`Secure Boot V2 <secure-boot-v2>`.  
    Please refer to Secure Boot V2 document for ESP32 Revision 3 or ESP32-S2.

Secure Boot is a feature for ensuring only your code can run on the chip. Data loaded from flash is verified on each reset.

Secure Boot is separate from the :doc:`Flash Encryption <flash-encryption>` feature, and you can use secure boot without encrypting the flash contents. However, for a secure environment both should be used simultaneously. See :ref:`secure-boot-and-flash-encr` for more details.

.. important::

    Enabling secure boot limits your options for further updates of your {IDF_TARGET_NAME}. Make sure to read this document throughly and understand the implications of enabling secure boot.

Background
----------

- Most data is stored in flash. Flash access does not need to be protected from physical access in order for secure boot to function, because critical data is stored (non-software-accessible) in Efuses internal to the chip.

- Efuses are used to store the secure bootloader key (in efuse BLOCK2), and also a single Efuse bit (ABS_DONE_0) is burned (written to 1) to permanently enable secure boot on the chip.  For more details on eFuses, see *{IDF_TARGET_NAME} Technical Reference Manual* > *eFuse Controller (eFuse)* [`PDF <{IDF_TARGET_TRM_EN_URL}#efuse>`__].

- To understand the secure boot process, first familiarise yourself with the standard :doc:`ESP-IDF boot process <../api-guides/general-notes>`.

- Both stages of the boot process (initial software bootloader load, and subsequent partition & app loading) are verified by the secure boot process, in a "chain of trust" relationship.

Secure Boot Process Overview
----------------------------

This is a high level overview of the secure boot process. Step by step instructions are supplied under :ref:`secure-boot-howto`. Further in-depth details are supplied under :ref:`secure-boot-technical-details`:

1. The options to enable secure boot are provided in the :ref:`project-configuration-menu`, under "Secure Boot Configuration".

2. Secure Boot defaults to signing images and partition table data during the build process. The "Secure boot private signing key" config item is a file path to a ECDSA public/private key pair in a PEM format file.

3. The software bootloader image is built by esp-idf with secure boot support enabled and the public key (signature verification) portion of the secure boot signing key compiled in. This software bootloader image is flashed at offset 0x1000.

4. On first boot, the software bootloader follows the following process to enable secure boot:

   - Hardware secure boot support generates a device secure bootloader key (generated via hardware RNG, then stored read/write protected in efuse), and a secure digest. The digest is derived from the key, an IV, and the bootloader image contents.
   - The secure digest is flashed at offset 0x0 in the flash.
   - Depending on Secure Boot Configuration, efuses are burned to disable JTAG and the ROM BASIC interpreter (it is strongly recommended these options are turned on.)
   - Bootloader permanently enables secure boot by burning the ABS_DONE_0 efuse. The software bootloader then becomes protected (the chip will only boot a bootloader image if the digest matches.)

5. On subsequent boots the ROM bootloader sees that the secure boot efuse is burned, reads the saved digest at 0x0 and uses hardware secure boot support to compare it with a newly calculated digest. If the digest does not match then booting will not continue. The digest and comparison are performed entirely by hardware, and the calculated digest is not readable by software. For technical details see :ref:`secure-boot-hardware-support`.

6. When running in secure boot mode, the software bootloader uses the secure boot signing key (the public key of which is embedded in the bootloader itself, and therefore validated as part of the bootloader) to verify the signature appended to all subsequent partition tables and app images before they are booted.

Keys
----

The following keys are used by the secure boot process:

- "secure bootloader key" is a 256-bit AES key that is stored in Efuse block 2. The bootloader can generate this key itself from the internal hardware random number generator, the user does not need to supply it (it is optionally possible to supply this key, see :ref:`secure-boot-reflashable`). The Efuse holding this key is read & write protected (preventing software access) before secure boot is enabled.

  - By default, the Efuse Block 2 Coding Scheme is "None" and a 256 bit key is stored in this block. On some {IDF_TARGET_NAME}s, the Coding Scheme is set to 3/4 Encoding (CODING_SCHEME efuse has value 1) and a 192 bit key must be stored in this block.

  For more details, see *{IDF_TARGET_NAME} Technical Reference Manual* > *eFuse Controller (eFuse)* > *System Parameter coding_scheme* [`PDF <{IDF_TARGET_TRM_EN_URL}#efuse>`__].

  The algorithm operates on a 256 bit key in all cases, 192 bit keys are extended by repeating some bits (:ref:`details<secure-bootloader-digest-algorithm>`).

- "secure boot signing key" is a standard ECDSA public/private key pair (see :ref:`secure-boot-image-signing-algorithm`) in PEM format.

  - The public key from this key pair (for signature verification but not signature creation) is compiled into the software bootloader and used to verify the second stage of booting (partition table, app image) before booting continues. The public key can be freely distributed, it does not need to be kept secret.

  - The private key from this key pair *must be securely kept private*, as anyone who has this key can authenticate to any bootloader that is configured with secure boot and the matching public key.

Bootloader Size
---------------

Enabling Secure boot and/or flash encryption will increase the size of bootloader, which might require updating partition table offset. See :ref:`secure-boot-bootloader-size`.

.. _secure-boot-howto:

How To Enable Secure Boot
-------------------------

1. Open the :ref:`project-configuration-menu`, navigate to "Secure Boot Configuration" and select the option "One-time Flash". (To understand the alternative "Reflashable" choice, see :ref:`secure-boot-reflashable`.)

2. Select a name for the secure boot signing key. This option will appear after secure boot is enabled. The file can be anywhere on your system. A relative path will be evaluated from the project directory. The file does not need to exist yet.

3. Set other menuconfig options (as desired). Pay particular attention to the "Bootloader Config" options, as you can only flash the bootloader once. Then exit menuconfig and save your configuration

4. The first time you run ``make``, if the signing key is not found then an error message will be printed with a command to generate a signing key via ``espsecure.py generate_signing_key``.

.. important::
   A signing key generated this way will use the best random number source available to the OS and its Python installation (`/dev/urandom` on OSX/Linux and `CryptGenRandom()` on Windows). If this random number source is weak, then the private key will be weak.

.. important::
   For production environments, we recommend generating the keypair using openssl or another industry standard encryption program. See :ref:`secure-boot-generate-key` for more details.

5. Run ``idf.py bootloader`` to build a secure boot enabled bootloader. The build output will include a prompt for a flashing command, using ``esptool.py write_flash``.

.. _secure-boot-resume-normal-flashing:

6. When you're ready to flash the bootloader, run the specified command (you have to enter it yourself, this step is not performed by make) and then wait for flashing to complete. **Remember this is a one time flash, you can't change the bootloader after this!**.

7. Run ``idf.py flash`` to build and flash the partition table and the just-built app image. The app image will be signed using the signing key you generated in step 4.

.. note:: ``idf.py flash`` doesn't flash the bootloader if secure boot is enabled.

8. Reset the {IDF_TARGET_NAME} and it will boot the software bootloader you flashed. The software bootloader will enable secure boot on the chip, and then it verifies the app image signature and boots the app. You should watch the serial console output from the {IDF_TARGET_NAME} to verify that secure boot is enabled and no errors have occurred due to the build configuration.

.. note:: Secure boot won't be enabled until after a valid partition table and app image have been flashed. This is to prevent accidents before the system is fully configured.

.. note:: If the {IDF_TARGET_NAME} is reset or powered down during the first boot, it will start the process again on the next boot.

9. On subsequent boots, the secure boot hardware will verify the software bootloader has not changed (using the secure bootloader key) and then the software bootloader will verify the signed partition table and app image (using the public key portion of the secure boot signing key).

.. _secure-boot-reflashable:

Re-Flashable Software Bootloader
--------------------------------

Configuration "Secure Boot: One-Time Flash" is the recommended configuration for production devices. In this mode, each device gets a unique key that is never stored outside the device.

However, an alternative mode :ref:`Secure Boot: Reflashable <CONFIG_SECURE_BOOTLOADER_MODE>` is also available. This mode allows you to supply a binary key file that is used for the secure bootloader key. As you have the key file, you can generate new bootloader images and secure boot digests for them.

In the esp-idf build process, this 256-bit key file is derived from the ECDSA app signing key generated by the user (see the :ref:`secure-boot-generate-key` step below). This private key's SHA-256 digest is used as the secure bootloader key in efuse (as-is for Coding Scheme None, or truncate to 192 bytes for 3/4 Encoding). This is a convenience so you only need to generate/protect a single private key.

.. note:: Although it's possible, we strongly recommend not generating one secure boot key and flashing it to every device in a production environment. The "One-Time Flash" option is recommended for production environments.

To enable a reflashable bootloader:

1. In the :ref:`project-configuration-menu`, select "Bootloader Config" -> :ref:`CONFIG_SECURE_BOOT` -> CONFIG_SECURE_BOOT_V1_ENABLED  ->  :ref:`CONFIG_SECURE_BOOTLOADER_MODE` -> Reflashable.

2. If necessary, set the :ref:`CONFIG_SECURE_BOOTLOADER_KEY_ENCODING` based on the coding scheme used by the device. The coding scheme is shown in the ``Features`` line when ``esptool.py`` connects to the chip, or in the ``espefuse.py summary`` output.

2. Follow the steps shown above to choose a signing key file, and generate the key file.

3. Run ``idf.py bootloader``. A binary key file will be created, derived from the private key that is used for signing. Two sets of flashing steps will be printed - the first set of steps includes an ``espefuse.py burn_key secure_boot_v1 path_to/secure-bootloader-key-xxx.bin`` command which is used to write the bootloader key to efuse. (Flashing this key is a one-time-only process.) The second set of steps can be used to reflash the bootloader with a pre-calculated digest (generated during the build process).

4. Resume from :ref:`Step 6 of the one-time flashing process <secure-boot-resume-normal-flashing>`, to flash the bootloader and enable secure boot. Watch the console log output closely to ensure there were no errors in the secure boot configuration.

.. _secure-boot-generate-key:

Generating Secure Boot Signing Key
----------------------------------

The build system will prompt you with a command to generate a new signing key via ``espsecure.py generate_signing_key``. This uses the python-ecdsa library, which in turn uses Python's os.urandom() as a random number source.

The strength of the signing key is proportional to (a) the random number source of the system, and (b) the correctness of the algorithm used. For production devices, we recommend generating signing keys from a system with a quality entropy source, and using the best available EC key generation utilities.

For example, to generate a signing key using the openssl command line:

```
openssl ecparam -name prime256v1 -genkey -noout -out my_secure_boot_signing_key.pem
```

Remember that the strength of the secure boot system depends on keeping the signing key private.

.. _remote-sign-image:

Remote Signing of Images
------------------------

For production builds, it can be good practice to use a remote signing server rather than have the signing key on the build machine (which is the default esp-idf secure boot configuration). The espsecure.py command line program can be used to sign app images & partition table data for secure boot, on a remote system.

To use remote signing, disable the option "Sign binaries during build". The private signing key does not need to be present on the build system. However, the public (signature verification) key is required because it is compiled into the bootloader (and can be used to verify image signatures during OTA updates.

To extract the public key from the private key::

  espsecure.py extract_public_key --keyfile PRIVATE_SIGNING_KEY PUBLIC_VERIFICATION_KEY

The path to the public signature verification key needs to be specified in the menuconfig under "Secure boot public signature verification key" in order to build the secure bootloader.

After the app image and partition table are built, the build system will print signing steps using espsecure.py::

  espsecure.py sign_data --keyfile PRIVATE_SIGNING_KEY BINARY_FILE

The above command appends the image signature to the existing binary. You can use the `--output` argument to write the signed binary to a separate file::

  espsecure.py sign_data --keyfile PRIVATE_SIGNING_KEY --output SIGNED_BINARY_FILE BINARY_FILE

Secure Boot Best Practices
--------------------------

* Generate the signing key on a system with a quality source of entropy.
* Keep the signing key private at all times. A leak of this key will compromise the secure boot system.
* Do not allow any third party to observe any aspects of the key generation or signing process using espsecure.py. Both processes are vulnerable to timing or other side-channel attacks.
* Enable all secure boot options in the Secure Boot Configuration. These include flash encryption, disabling of JTAG, disabling BASIC ROM interpeter, and disabling the UART bootloader encrypted flash access.
* Use secure boot in combination with :doc:`flash encryption<flash-encryption>` to prevent local readout of the flash contents.

.. _secure-boot-technical-details:

Technical Details
-----------------

The following sections contain low-level reference descriptions of various secure boot elements:

.. _secure-boot-hardware-support:

Secure Boot Hardware Support
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The first stage of secure boot verification (checking the software bootloader) is done via hardware. The {IDF_TARGET_NAME}'s Secure Boot support hardware can perform three basic operations:

1. Generate a random sequence of bytes from a hardware random number generator.

2. Generate a digest from data (usually the bootloader image from flash) using a key stored in Efuse block 2. The key in Efuse can (& should) be read/write protected, which prevents software access. For full details of this algorithm see `Secure Bootloader Digest Algorithm`_. The digest can only be read back by software if Efuse ABS_DONE_0 is *not* burned (ie still 0).

3. Generate a digest from data (usually the bootloader image from flash) using the same algorithm as step 2 and compare it to a pre-calculated digest supplied in a buffer (usually read from flash offset 0x0). The hardware returns a true/false comparison without making the digest available to software. This function is available even when Efuse ABS_DONE_0 is burned.

.. _secure-bootloader-digest-algorithm:

Secure Bootloader Digest Algorithm
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Starting with an "image" of binary data as input, this algorithm generates a digest as output. The digest is sometimes referred to as an "abstract" in hardware documentation.

For a Python version of this algorithm, see the ``espsecure.py`` tool in the components/esptool_py directory (specifically, the ``digest_secure_bootloader`` command).

Items marked with (^) are to fulfill hardware restrictions, as opposed to cryptographic restrictions.

1. Read the AES key from efuse block 2, in reversed byte order. If Coding Scheme is set to 3/4 Encoding, extend the 192 bit key to 256 bits using the same algorithm described in :ref:`flash-encryption-algorithm`.
2. Prefix the image with a 128 byte randomly generated IV.
3. If the image length is not modulo 128, pad the image to a 128 byte boundary with 0xFF. (^)
4. For each 16 byte plaintext block of the input image:
   - Reverse the byte order of the plaintext input block (^)
   - Apply AES256 in ECB mode to the plaintext block.
   - Reverse the byte order of the ciphertext output block. (^)
   - Append to the overall ciphertext output.
5. Byte-swap each 4 byte word of the ciphertext (^)
6. Calculate SHA-512 of the ciphertext.

Output digest is 192 bytes of data: The 128 byte IV, followed by the 64 byte SHA-512 digest.

.. _secure-boot-image-signing-algorithm:

Image Signing Algorithm
~~~~~~~~~~~~~~~~~~~~~~~

Deterministic ECDSA as specified by `RFC 6979 <https://tools.ietf.org/html/rfc6979>`_.

- Curve is NIST256p (openssl calls this curve "prime256v1", it is also sometimes called secp256r1).
- Hash function is SHA256.
- Key format used for storage is PEM.

  - In the bootloader, the public key (for signature verification) is flashed as 64 raw bytes.

- Image signature is 68 bytes - a 4 byte version word (currently zero), followed by a 64 bytes of signature data. These 68 bytes are appended to an app image or partition table data.

Manual Commands
~~~~~~~~~~~~~~~

Secure boot is integrated into the esp-idf build system, so ``make`` will automatically sign an app image if secure boot is enabled. ``idf.py bootloader`` will produce a bootloader digest if menuconfig is configured for it.

However, it is possible to use the ``espsecure.py`` tool to make standalone signatures and digests.

To sign a binary image::

  espsecure.py sign_data --keyfile ./my_signing_key.pem --output ./image_signed.bin image-unsigned.bin

Keyfile is the PEM file containing an ECDSA private signing key.

To generate a bootloader digest::

  espsecure.py digest_secure_bootloader --keyfile ./securebootkey.bin --output ./bootloader-digest.bin build/bootloader/bootloader.bin

Keyfile is the 32 byte raw secure boot key for the device.

The output of the ``espsecure.py digest_secure_bootloader`` command is a single file which contains both the digest and the bootloader appended to it. To flash the combined digest plus bootloader to the device::

  esptool.py write_flash 0x0 bootloader-digest.bin

.. _secure-boot-and-flash-encr:

Secure Boot & Flash Encryption
------------------------------

If secure boot is used without :doc:`Flash Encryption <flash-encryption>`, it is possible to launch "time-of-check to time-of-use" attack, where flash contents are swapped after the image is verified and running. Therefore, it is recommended to use both the features together.

.. _signed-app-verify:

Signed App Verification Without Hardware Secure Boot
----------------------------------------------------

The integrity of apps can be checked even without enabling the hardware secure boot option. This option uses the same app signature scheme as hardware secure
boot, but unlike hardware secure boot it does not prevent the bootloader from being physically updated. This means that the device can be secured
against remote network access, but not physical access. Compared to using hardware Secure Boot this option is much simpler to implement. See :ref:`signed-app-verify-howto` for step by step instructions.

An app can be verified on update and, optionally, be verified on boot.

- Verification on update: When enabled, the signature is automatically checked whenever the esp_ota_ops.h APIs are used for OTA updates. If hardware secure boot is enabled, this option is always enabled and cannot be disabled. If hardware secure boot is not enabled, this option still adds significant security against network-based attackers by preventing spoofing of OTA updates.

- Verification on boot: When enabled, the bootloader will be compiled with code to verify that an app is signed before booting it. If hardware secure boot is enabled, this option is always enabled and cannot be disabled. If hardware secure boot is not enabled, this option doesn't add significant security by itself so most users will want to leave it disabled.

.. _signed-app-verify-howto:

How To Enable Signed App Verification
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

1. Open :ref:`project-configuration-menu` -> Security features -> Enable :ref:`CONFIG_SECURE_SIGNED_APPS_NO_SECURE_BOOT`

2. "Bootloader verifies app signatures" can be enabled, which verifies app on boot.

3. By default, "Sign binaries during build" will be enabled on selecting "Require signed app images" option, which will sign binary files as a part of build process. The file named in "Secure boot private signing key" will be used to sign the image.

4. If you disable "Sign binaries during build" option then you'll have to enter path of a public key file used to verify signed images in "Secure boot public signature verification key".
   In this case, private signing key should be generated by following instructions in :ref:`secure-boot-generate-key`; public verification key and signed image should be generated by following instructions in :ref:`remote-sign-image`.

Advanced Features
-----------------

JTAG Debugging
~~~~~~~~~~~~~~

By default, when Secure Boot is enabled then JTAG debugging is disabled via eFuse. The bootloader does this on first boot, at the same time it enables Secure Boot.

See :ref:`jtag-debugging-security-features` for more information about using JTAG Debugging with either Secure Boot or signed app verification enabled.
