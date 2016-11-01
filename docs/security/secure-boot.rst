Secure Boot
===========

Secure Boot is a feature for ensuring only your code can run on the chip. Data loaded from flash is verified on each reset.

Secure Boot is separate from the Encrypted Flash feature, and you can use secure boot without encrypting the flash contents. However for maximum protection we recommend using both features together.

Background
----------

- Most data is stored in flash. Flash access does not need to protected for secure boot to function, because critical data is stored in Efuses internal to the chip.

- Efuses are used to store the secure bootloader key (in efuse block 2), and also a single Efuse (ABS_DONE_0) is burned (written to 1) to permanently enable secure boot on the chip. For more details about efuse, see the (forthcoming) chapter in the Technical Reference Manual.

- To understand the secure boot process, first familiarise yourself with the standard `esp-idf boot process`.

Secure Boot Process Overview
----------------------------

This is a high level overview of the secure boot process. Step by step instructions are supplied under `How To Enable Secure Boot`. Further in-depth details are supplied under `Technical Details`:

1. The options to enable secure boot are provided in the ``make menuconfig`` hierarchy, under "Bootloader Config".

2. Bootloader Config includes the path to a ECDSA public key. This "image public key" is compiled into the software bootloader.

2. The software bootloader image is built by esp-idf with the public key embedded, and with a secure boot flag set in its header. This software bootloader image is flashed at offset 0x1000.

3. On first boot, the software bootloader tests the secure boot flag. If it is set, the following process is followed to enable secure boot:
  - Hardware secure boot support generates a device secure bootloader key (stored read/write protected in efuse), and a secure digest. The digest is derived from the key, an IV, and the bootloader image contents.
  - The secure digest is flashed at offset 0x0 in the flash.
  - Bootloader permanently enables secure boot by burning the ABS_DONE_0 efuse. The software bootloader then becomes protected (the chip will only boot this bootloader image.)
  - Bootloader also disables JTAG via efuse.

4. On subsequent boots the ROM bootloader sees that the secure boot efuse is burned, reads the saved digest at 0x0 and uses hardware secure boot support to compare it with a newly calculated digest. If the digest does not match then booting will not continue.

5. When running in secure boot mode, the software bootloader uses the image public key (embedded in the bootloader itself) to verify all subsequent partition tables and app images before they are booted.

Keys
----

The following keys are used by the secure boot process:

- "secure bootloader key" is a 256-bit AES key that is stored in Efuse block 2. The bootloader can generate this key itself from hardware, the user does not need to supply it. The Efuse holding this key must be read & write protected (preventing software access) before secure boot is enabled.

- "image public key" is a ECDSA public key (curve TBD) in format TBD. This public key is flashed into the software bootloader and used to verify the remaining parts of the flash (partition table, app image) before booting continues. The public key can be freely distributed, it does not need to be kept secret.

- "image private key" is a ECDSA private key, a matching pair with "image public key". This private key is used to sign partition tables and app images for the secure boot process. **This private key must be securely kept private** as anyone who has this key can authenticate to the secure boot process. It is acceptable to use the same private key across multiple production devices.

How To Enable Secure Boot
-------------------------

1. Run ``make menuconfig``, navigate to "Bootloader Config" -> "Secure Boot" and select the option "One-time Flash". (For the alternative "Reflashable" choice, see `Re-Flashable Software Bootloader`.)

2. Select names for public & private image key files "Image Public Key File" and "Image Private Key File". These options will appear after secure boot is enabled. The files can be anywhere on your system. Relative paths are evaluated from the project directory. The files don't need to exist yet.

3. Set other config options (as desired). Pay particular attention to the "Bootloader Config" options, as you can only flash the bootloader once. Then exit menuconfig and save your configuration

4. Run ``make generate_image_keypair`` to generate an image public key and a matching image private key.

   **IMPORTANT** The key pair is generated using the best random number source available via the OS and its Python installation (`/dev/urandom` on OSX/Linux and `CryptGenRandom()` on Windows). If this random number source is weak, then the private key will be weak.

5. Run ``make bootloader`` to build a secure boot enabled bootloader. The output of `make` will include a prompt for a flashing command, using `esptool.py write_flash`.

6. When you're ready to flash the bootloader, run the specified command (you have to enter it yourself, this step is not automated) and then wait for flashing to complete. **Remember this is a one time flash, you can't change the bootloader after this!**.

7. Run `make flash` to build and flash the partition table and the just-built app image. The app image will be signed using the private key you generated in step 4.

   *NOTE*: `make flash` doesn't flash the bootloader if secure boot is enabled.

8. Reset the ESP32 and it will boot the software bootloader you flashed. The software bootloader will enable secure boot on the chip, and then it verifies the app image signature and boots the app. You should watch the serial console output from the ESP32 to verify that secure boot is enabled and no errors have occured due to the build configuration.

**IMPORTANT** Secure boot won't ever be enabled until after a valid partition table and app image have been flashed. This is to prevent accidents before the system is fully configured.

9. On subsequent boots, the secure boot hardware will verify the software bootloader (using the secure bootloader key) and then the software bootloader will verify the partition table and app image (using the image public key).

Re-Flashable Software Bootloader
--------------------------------

The "Secure Boot: One-Time Flash" is the recommended software bootloader configuration for production devices. In this mode, each device gets a unique key that is never stored outside the device.

However, an alternative mode "Secure Boot: Reflashable" is also available. This mode allows you to supply a 256-bit key file that is used for the secure bootloader key. As you have the key file, you can generate new bootloader images and secure boot digests for them.

*NOTE*: Although it's possible, we strongly recommend not generating one secure boot key and flashing it to every device in a production environment.

1. In the ``make menuconfig`` step, select "Bootloader Config" -> "Secure Boot" -> "Reflashable".

2. Select a name for the "Secure bootloader key file". The file can be anywhere on your system, and does not have to exist yet. A path is evaluated relative to the project directory. The file doesn't have to exist yet.

3. The first time you run ``make bootloader``, the system will prompt you with a ``espsecure.py generate_key`` command that can be used to generate the secure bootloader key.

   **IMPORTANT** The new key is generated using the best random number source available via the OS and its Python installation (`/dev/urandom` on OSX/Linux and `CryptGenRandom()` on Windows). If this random number source is weak, then the secure bootloader key will be weak.

4. Run ``make bootloader`` again. Two sets of flashing steps will be printed - the first set of steps includes an ``espefuse.py burn_key`` command which is used to write the secure bootloader key to efuse. (Flashing this key is a one-time-only process.) The second set of steps can be used to reflash the bootloader with a pre-generated digest (generated during the build process, using the secure bootloader key file).

5. Resume from `Step 6<Secure Boot Process Overview>` of the one-time process, to flash the bootloader and enable secure boot. Watch the console log output closely to ensure there were no errors in the secure boot configuration.


Technical Details
-----------------

The following sections contain low-level descriptions of various technical functions:

Hardware Secure Boot Support
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The Secure Boot support hardware can perform three basic operations:

1. Generate a random sequence of bytes from a hardware random number generator.

2. Generate a digest from data (usually the bootloader image from flash) using a key stored in Efuse block 2. The key in Efuse can (& should) be read/write protected, which prevents software access. For full details of this algorithm see `Secure Bootloader Digest Algorithm`. The digest can only be read from hardware if Efuse ABS_DONE_0 is *not* burned (ie still 0), to prevent new digests from being calculated on the device after secure boot is enabled.

3. Verify a digest from data (usually the bootloader image from flash), and compare it to a pre-existing digest (usually read from flash offset 0x0). The hardware returns a true/false comparison without making the digest available to software.

Secure Bootloader Digest Algorithm
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Starting with an "image" of binary data as input, this algorithm generates a digest as output.

For a Python version of this algorithm, see the `espsecure.py` tool in the components/esptool_py directory.

Items marked with (^) are to fulfill hardware restrictions, as opposed to cryptographic restrictions.

1. Prefix the image with a 128 byte randomly generated IV.
2. If the image is not modulo 128, pad the image to a 128 byte boundary with 0xFF. (^)
3. For each 16 byte block of the input image:
   - Reverse the byte order of the block (^)
   - Use the AES256 algorithm in ECB mode to encrypt the block.
   - Reverse the byte order of the 16 bytes of ciphertext output. (^)
   - Append to the overall ciphertext output.
4. Byte-swap each 4 byte word of the ciphertext (^)
5. Calculate SHA-512 of the ciphertext.

Output digest is 192 bytes of data: The 128 byte IV, followed by the 64 byte SHA-512 digest.

Image Signing Algorithm
~~~~~~~~~~~~~~~~~~~~~~~

Deterministic ECDSA as specified by `RFC6979`.

Curve is TBD.
Key format is TBD.
Output format is TBD.


.. _esp-idf boot process: ../boot-process.rst
.. _RFC6979: https://tools.ietf.org/html/rfc6979
