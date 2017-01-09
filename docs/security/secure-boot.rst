Secure Boot
===========

Secure Boot is a feature for ensuring only your code can run on the chip. Data loaded from flash is verified on each reset.

Secure Boot is separate from the `Flash Encryption` feature, and you can use secure boot without encrypting the flash contents. However we recommend using both features together for a secure environment.

Background
----------

- Most data is stored in flash. Flash access does not need to be protected from physical access in order for secure boot to function, because critical data is stored (non-software-accessible) in Efuses internal to the chip.

- Efuses are used to store the secure bootloader key (in efuse block 2), and also a single Efuse bit (ABS_DONE_0) is burned (written to 1) to permanently enable secure boot on the chip. For more details about efuse, see the (forthcoming) chapter in the Technical Reference Manual.

- To understand the secure boot process, first familiarise yourself with the standard :doc:`ESP-IDF boot process <../general-notes>`.

- Both stages of the boot process (initial software bootloader load, and subsequent partition & app loading) are verified by the secure boot process, in a "chain of trust" relationship.

Secure Boot Process Overview
----------------------------

This is a high level overview of the secure boot process. Step by step instructions are supplied under `How To Enable Secure Boot`. Further in-depth details are supplied under `Technical Details`:

1. The options to enable secure boot are provided in the ``make menuconfig`` hierarchy, under "Secure Boot Configuration".

2. Secure Boot defaults to signing images and partition table data during the build process. The "Secure boot private signing key" config item is a file path to a ECDSA public/private key pair in a PEM format file.

3. The software bootloader image is built by esp-idf with secure boot support enabled and the public key (signature verification) portion of the secure boot signing key compiled in. This software bootloader image is flashed at offset 0x1000.

4. On first boot, the software bootloader follows the following process to enable secure boot:

   - Hardware secure boot support generates a device secure bootloader key (generated via hardware RNG, then stored read/write protected in efuse), and a secure digest. The digest is derived from the key, an IV, and the bootloader image contents.
   - The secure digest is flashed at offset 0x0 in the flash.
   - Depending on Secure Boot Configuration, efuses are burned to disable JTAG and the ROM BASIC interpreter (it is strongly recommended these options are turned on.)
   - Bootloader permanently enables secure boot by burning the ABS_DONE_0 efuse. The software bootloader then becomes protected (the chip will only boot a bootloader image if the digest matches.)

5. On subsequent boots the ROM bootloader sees that the secure boot efuse is burned, reads the saved digest at 0x0 and uses hardware secure boot support to compare it with a newly calculated digest. If the digest does not match then booting will not continue. The digest and comparison are performed entirely by hardware, and the calculated digest is not readable by software. For technical details see `Hardware Secure Boot Support`.

6. When running in secure boot mode, the software bootloader uses the secure boot signing key (the public key of which is embedded in the bootloader itself, and therefore validated as part of the bootloader) to verify the signature appended to all subsequent partition tables and app images before they are booted.

Keys
----

The following keys are used by the secure boot process:

- "secure bootloader key" is a 256-bit AES key that is stored in Efuse block 2. The bootloader can generate this key itself from the internal hardware random number generator, the user does not need to supply it (it is optionally possible to supply this key, see `Re-Flashable Software Bootloader`). The Efuse holding this key is read & write protected (preventing software access) before secure boot is enabled.

- "secure boot signing key" is a standard ECDSA public/private key pair (see `Image Signing Algorithm`) in PEM format.

  - The public key from this key pair (for signature verificaton but not signature creation) is compiled into the software bootloader and used to verify the second stage of booting (partition table, app image) before booting continues. The public key can be freely distributed, it does not need to be kept secret.

  - The private key from this key pair *must be securely kept private*, as anyone who has this key can authenticate to any bootloader that is configured with secure boot and the matching public key.


How To Enable Secure Boot
-------------------------

1. Run ``make menuconfig``, navigate to "Secure Boot Configuration" and select the option "One-time Flash". (To understand the alternative "Reflashable" choice, see `Re-Flashable Software Bootloader`.)

2. Select a name for the secure boot signing key. This option will appear after secure boot is enabled. The file can be anywhere on your system. A relative path will be evaluated from the project directory. The file does not need to exist yet.

3. Set other menuconfig options (as desired). Pay particular attention to the "Bootloader Config" options, as you can only flash the bootloader once. Then exit menuconfig and save your configuration

4. The first time you run ``make``, if the signing key is not found then an error message will be printed with a command to generate a signing key via ``espsecure.py generate_signing_key``.

   **IMPORTANT** A signing key generated this way will use the best random number source available to the OS and its Python installation (`/dev/urandom` on OSX/Linux and `CryptGenRandom()` on Windows). If this random number source is weak, then the private key will be weak.

   **IMPORTANT** For production environments, we recommend generating the keypair using openssl or another industry standard encryption program. See `Generating Secure Boot Signing Key` for more details.

5. Run ``make bootloader`` to build a secure boot enabled bootloader. The output of `make` will include a prompt for a flashing command, using `esptool.py write_flash`.

6. When you're ready to flash the bootloader, run the specified command (you have to enter it yourself, this step is not performed by make) and then wait for flashing to complete. **Remember this is a one time flash, you can't change the bootloader after this!**.

7. Run `make flash` to build and flash the partition table and the just-built app image. The app image will be signed using the signing key you generated in step 4.

   *NOTE*: `make flash` doesn't flash the bootloader if secure boot is enabled.

8. Reset the ESP32 and it will boot the software bootloader you flashed. The software bootloader will enable secure boot on the chip, and then it verifies the app image signature and boots the app. You should watch the serial console output from the ESP32 to verify that secure boot is enabled and no errors have occured due to the build configuration.

*NOTE* Secure boot won't be enabled until after a valid partition table and app image have been flashed. This is to prevent accidents before the system is fully configured.

9. On subsequent boots, the secure boot hardware will verify the software bootloader has not changed (using the secure bootloader key) and then the software bootloader will verify the signed partition table and app image (using the public key portion of the secure boot signing key).

Re-Flashable Software Bootloader
--------------------------------

Configuration "Secure Boot: One-Time Flash" is the recommended configuration for production devices. In this mode, each device gets a unique key that is never stored outside the device.

However, an alternative mode "Secure Boot: Reflashable" is also available. This mode allows you to supply a 256-bit key file that is used for the secure bootloader key. As you have the key file, you can generate new bootloader images and secure boot digests for them.

In the esp-idf build process, this 256-bit key file is derived from the app signing key generated during the generate_signing_key step above. The private key's SHA-256 digest is used as the 256-bit secure bootloader key. This is a convenience so you only need to generate/protect a single private key.

*NOTE*: Although it's possible, we strongly recommend not generating one secure boot key and flashing it to every device in a production environment. The "One-Time Flash" option is recommended for production environments.

To enable a reflashable bootloader:

1. In the ``make menuconfig`` step, select "Bootloader Config" -> "Secure Boot" -> "Reflashable".

2. Follow the steps shown above to choose a signing key file, and generate the key file.

3. Run ``make bootloader``. A 256-bit key file will be created, derived from the private key that is used for signing. Two sets of flashing steps will be printed - the first set of steps includes an ``espefuse.py burn_key`` command which is used to write the bootloader key to efuse. (Flashing this key is a one-time-only process.) The second set of steps can be used to reflash the bootloader with a pre-calculated digest (generated during the build process).

4. Resume from `Step 6<Secure Boot Process Overview>` of the one-time process, to flash the bootloader and enable secure boot. Watch the console log output closely to ensure there were no errors in the secure boot configuration.

Generating Secure Boot Signing Key
----------------------------------

The build system will prompt you with a command to generate a new signing key via ``espsecure.py generate_signing_key``. This uses the python-ecdsa library, which in turn uses Python's os.urandom() as a random number source.

The strength of the signing key is proportional to (a) the random number source of the system, and (b) the correctness of the algorithm used. For production devices, we recommend generating signing keys from a system with a quality entropy source, and using the best available EC key generation utilities.

For example, to generate a signing key using the openssl command line:

```
openssl ecparam -name prime256v1 -genkey -noout -out my_secure_boot_signing_key.pem
```

Remember that the strength of the secure boot system depends on keeping the signing key private.

Remote Signing of Images
------------------------

For production builds, it can be good practice to use a remote signing server rather than have the signing key on the build machine (which is the default esp-idf secure boot configuration). The espsecure.py command line program can be used to sign app images & partition table data for secure boot, on a remote system.

To use remote signing, disable the option "Sign binaries during build". The private signing key does not need to be present on the build system. However, the public (signature verification) key is required because it is compiled into the bootloader (and can be used to verify image signatures during OTA updates.

To extract the public key from the private key::

  espsecure.py extract_public_key --keyfile PRIVATE_SIGNING_KEY PUBLIC_VERIFICATION_KEY

The path to the public signature verification key needs to be specified in the menuconfig under "Secure boot public signature verification key" in order to build the secure bootloader.

After the app image and partition table are built, the build system will print signing steps using espsecure.py::

  espsecure.py sign_data --keyfile PRIVATE_SIGNING_KEY BINARY_FILE

The above command appends the image signature to the existing binary. You can use the --output argument to place the binary with signature appended into a separate file::

  espsecure.py sign_data --keyfile PRIVATE_SIGNING_KEY --output SIGNED_BINARY_FILE BINARY_FILE

Secure Boot Best Practices
--------------------------

* Generate the signing key on a system with a quality source of entropy.
* Keep the signing key private at all times. A leak of this key will compromise the secure boot system.
* Do not allow any third party to observe any aspects of the key generation or signing process using espsecure.py. Both processes are vulnerable to timing or other side-channel attacks.
* Enable all secure boot options in the Secure Boot Configuration. These include flash encryption, disabling of JTAG, disabling BASIC ROM interpeter, and disabling the UART bootloader encrypted flash access.

Technical Details
-----------------

The following sections contain low-level descriptions of various technical functions:

Hardware Secure Boot Support
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The Secure Boot support hardware can perform three basic operations:

1. Generate a random sequence of bytes from a hardware random number generator.

2. Generate a digest from data (usually the bootloader image from flash) using a key stored in Efuse block 2. The key in Efuse can (& should) be read/write protected, which prevents software access. For full details of this algorithm see `Secure Bootloader Digest Algorithm`. The digest can only be read back by software if Efuse ABS_DONE_0 is *not* burned (ie still 0).

3. Generate a digest from data (usually the bootloader image from flash) using the same algorithm as step 2 and compare it to a pre-calculated digest supplied in a buffer (usually read from flash offset 0x0). The hardware returns a true/false comparison without making the digest available to software. This function is available even when Efuse ABS_DONE_0 is burned.

Secure Bootloader Digest Algorithm
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Starting with an "image" of binary data as input, this algorithm generates a digest as output. The digest is sometimes referred to as an "abstract" in hardware documentation.

For a Python version of this algorithm, see the `espsecure.py` tool in the components/esptool_py directory.

Items marked with (^) are to fulfill hardware restrictions, as opposed to cryptographic restrictions.

1. Prefix the image with a 128 byte randomly generated IV.
2. If the image length is not modulo 128, pad the image to a 128 byte boundary with 0xFF. (^)
3. For each 16 byte plaintext block of the input image:
   - Reverse the byte order of the plaintext input block (^)
   - Apply AES256 in ECB mode to the plaintext block.
   - Reverse the byte order of the ciphertext output block. (^)
   - Append to the overall ciphertext output.
4. Byte-swap each 4 byte word of the ciphertext (^)
5. Calculate SHA-512 of the ciphertext.

Output digest is 192 bytes of data: The 128 byte IV, followed by the 64 byte SHA-512 digest.

Image Signing Algorithm
~~~~~~~~~~~~~~~~~~~~~~~

Deterministic ECDSA as specified by `RFC6979`.

- Curve is NIST256p (openssl calls this curve "prime256v1", it is also sometimes called secp256r1).
- Hash function is SHA256.
- Key format used for storage is PEM.
  - In the bootloader, the public key (for signature verification) is flashed as 64 raw bytes.
- Image signature is 68 bytes - a 4 byte version word (currently zero), followed by a 64 bytes of signature data. These 68 bytes are appended to an app image or partition table data.

Manual Commands
~~~~~~~~~~~~~~~

Secure boot is integrated into the esp-idf build system, so `make` will automatically sign an app image if secure boot is enabled. `make bootloader` will produce a bootloader digest if menuconfig is configured for it.

However, it is possible to use the `espsecure.py` tool to make standalone signatures and digests.

To sign a binary image::

  espsecure.py sign_data --keyfile ./my_signing_key.pem --output ./image_signed.bin image-unsigned.bin

Keyfile is the PEM file containing an ECDSA private signing key.

To generate a bootloader digest::

  espsecure.py digest_secure_bootloader --keyfile ./securebootkey.bin --output ./bootloader-digest.bin build/bootloader/bootloader.bin

Keyfile is the 32 byte raw secure boot key for the device. To flash this digest onto the device::

  esptool.py write_flash 0x0 bootloader-digest.bin

.. _RFC6979: https://tools.ietf.org/html/rfc6979
.. _Flash Encryption: flash-encryption.rst
