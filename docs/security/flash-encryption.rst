Flash Encryption
================

Flash Encryption is a feature for encrypting the contents of the ESP32's attached SPI flash. When flash encryption is enabled, physical readout of the SPI flash is not sufficient to recover most flash contents.

Flash Encryption is separate from the `Secure Boot` feature, and you can use flash encryption without enabling secure boot. However we recommend using both features together for a secure environment.

**IMPORTANT: Enabling flash encryption limits your options for further updates of your ESP32. Make sure to read this document (including `Limitations of Flash Encryption` and understand the implications of enabling flash encryption.**

Background
----------

- The contents of the flash are encrypted using AES with a 256 bit key. The flash encryption key is stored in efuse internal to the chip, and is (by default) protected from software access.

- Flash access is transparent via the flash cache mapping feature of ESP32 - any flash regions which are mapped to the address space will be transparently decrypted when read.

- Encryption is applied by flashing the ESP32 with plaintext data, and (if encryption is enabled) the bootloader encrypts the data in place on first boot.

- Not all of the flash is encrypted. The following kinds of flash data are encrypted:
  - Bootloader
  - Secure boot bootloader digest (if secure boot is enabled)
  - Partition Table
  - All "app" type partitions
  - Any partition marked with the "encrypt" flag in the partition table

	It may be desirable for some data partitions to remain unencrypted for ease of access, or to use flash-friendly update algorithms that are ineffective if the data is encrypted. "NVS" partitions for non-volatile storage cannot be encrypted.

- The flash encryption key is stored in efuse key block 1, internal to the ESP32 chip. By default, this key is read- and write-protected so software cannot access it or change it.

- The `flash encryption algorithm` is AES-256, where the key is "tweaked" with the offset address of each 32 byte block of flash. This means every 32 byte block (two consecutive 16 byte AES blocks) is encrypted with a unique key derived from the flash encryption key.

- Although software running on the chip can transparently decrypt flash contents, by default it is made possible for the UART bootloader to decrypt (or encrypt) data when flash encryption is enabled.

Flash Encryption Initialisation
-------------------------------

This is the default (and recommended) flash encryption initialisation process. It is possible to customise this process for development or other purposes, see `Flash Encryption Advanced Features` for details.

**IMPORTANT: Once flash encryption is enabled on first boot, the hardware allows a maximum of 3 subsequent flash updates via physical re-flashing. If secure boot is enabled, no physical re-flashes are possible. OTA updates can be used to update flash content without counting towards this limit. When enabling flash encryption in development, use a `precalculated flash encryption key` to allow physically re-flashing an unlimited number of times with pre-encrypted data.**

- The bootloader must be compiled with flash encryption support enabled. In ``make menuconfig``, navigate to "Security Features" and select "Yes" for "Enable flash nencryption on boot".

- If enabling Secure Boot at the same time, you can simultaneously select those options now. See the `Secure Boot` documentation for details.

- Build and flash the bootloader, partition table and factory app image as normal. These partitions are initially written to the flash unencrypted.

- On first boot, the bootloader sees ``FLASH_CRYPT_CNT`` efuse is set to 0 so it generates a flash encryption key using the hardware random number generator. This key is stored in efuse. The key is read and write protected against further software access.

- All of the encrypted partitions are then encrypted in-place by the bootloader. Encrypting in-place can take some time (up to a minute for large partitions.)

**IMPORTANT: Do not interrupt power to the ESP32 while the first boot encryption pass is running. If power is interrupted, the flash contents will be corrupted and require flashing with unencrypted data again. This re-flash will not count towards the flashing limit, as ``FLASH_CRYPT_CNT`` is only updated after this process finishes.**

- Once flashing is complete. efuses are blown (by default) to disable encrypted flash access while the UART bootloader is running.

- If not already write-protected, the ``FLASH_CRYPT_CONFIG`` efuse is also burned to the maximum value (``0xF``) to maximise the number of key bits which are tweaked in the flash algorithm. See `Setting FLASH_CRYPT_CONFIG` for details of this efuse.

- Finally, the ``FLASH_CRYPT_CNT`` efuse is burned with the initial value 1. It is this efuse which activates the transparent flash encryption layer, and limits the number of subsequent reflashes. See the `Updating Encrypted Flash` section for details about ``FLASH_CRYPT_CNT``.

- The bootloader resets itself to reboot from the newly encrypted flash.


Encrypted Flash Access
----------------------

Reading Encrypted Flash
^^^^^^^^^^^^^^^^^^^^^^^

Whenever the ``FLASH_CRYPT_CNT`` efuse is set to a value with an odd number of bits set, all flash content which is accessed via the MMU's flash cache is transparently decrypted. This includes:

- Executable application code in flash (IROM).
- All read-only data stored in flash (DROM).
- Any data accessed via ``esp_spi_flash_mmap``.
- The software bootloader image when it is read by the ROM bootloader.

**IMPORTANT: The MMU flash cache unconditionally decrypts all data. Data which is stored unencrypted in the flash will be "transparently decrypted" via the flash cache and appear to software like random garbage.**

To read data without using a flash cache MMU mapping, we recommend using the partition read function ``esp_partition_read``. When using this function, data will only be decrypted when it is read from an encrypted partition. Other partitions will be read unencrypted. In this way, software can access encrypted and non-encrypted flash in the same way.

Data which is read via other SPI read APIs are not decrypted:

- Data read via ``esp_spi_flash_read`` is not decrypted
- Data read via ROM function ``SPIRead`` is not decrypted (this function is not supported in esp-idf apps).
- Data stored using the Non-Volatile Storage (NVS) API is always stored decrypted.


Writing Encrypted Flash
^^^^^^^^^^^^^^^^^^^^^^^

Where possible, we recommend using the partition write function ``esp_partition_write``. When using this function, data will only be encrypted when writing to encrypted partitions. Data will be written to other partitions unencrypted. In this way, software can access encrypted and non-encrypted flash in the same way.

The ``esp_spi_flash_write`` function will write data when the write_encrypted parameter is set to true. Otherwise, data will be written unencrypted.

The ROM function ``SPI_Encrypt_Write`` will write encrypted data to flash, the ROM function ``SPIWrite`` will write unencrypted to flash. (these function are not supported in esp-idf apps).

The minimum write size for unencrypted data is 4 bytes (and the alignment is 4 bytes). Because data is encrypted in blocks, the minimum write size for encrypted data is 32 bytes (and the alignment is 32 bytes.)

Updating Encrypted Flash
------------------------

OTA Updates
^^^^^^^^^^^

OTA updates to encrypted partitions will automatically write encrypted, as long as the ``esp_partition_write`` function is used.

Serial Flashing
^^^^^^^^^^^^^^^

Provided secure boot is not used, the ``FLASH_CRYPT_CNT`` registers allow the flash to be updated with new plaintext data via serial flashing (or other physical methods), up to 3 additional times. ``FLASH_CRYPT_CNT`` efuse is an 8-bit value, and the flash encryption enables or disables based on the number of bits which are set to "1":

- Even number (0-6) bits are set: Transparent reading of encrypted flash is disabled, any encrypted data cannot be decrypted. If the bootloader was built with "Enable flash encryption on boot" then it will see this situation and immediately re-encrypt the flash wherever it finds unencrypted data. Once done, it sets another bit in the efuse to '1' meaning an odd number of bits are now set.

- Odd number (1-7) bits are set: Transparent reading of encrypted flash is enabled.

- All 8 bits are set (valuye 0: Transparent reading of encrypted flash is disabled, any encrypted data is inaccessible. Bootloader will normally detect this condition and halt. To avoid use of this state to load unauthorised code, secure boot must be used or ``FLASH_CRYPT_CNT`` must be write-protected.

The espefuse.py tool can be used to manually change the number of bits set in ``FLASH_CRYPT_CNT``, via serial bootloader.

Limited Updates
^^^^^^^^^^^^^^^

Only 4 physical flash updates (writing plaintext data which is then encrypted) are possible:

1. On first plaintext boot, bit count has brand new value 0 and bootloader changes to 1 (0x01) following encryption.
2. On next plaintext flash update, bit count is manually updated to 2 (0x03) and bootloader changes to 4 (0x07) following encryption.
3. Then bit count is manually updated to 4 (0x0F) and the bootloader changes efuse bit count to 5 (0x1F).
4. Finally bootloader is manually updated to 6 (0x3F) and bootloader changes efuse bit count to 7 (0x7F).

Cautions With Re-Flashing
^^^^^^^^^^^^^^^^^^^^^^^^^^

- When reflashing via serial, reflash every partition that was previously written with plaintext (including bootloader). It is possible to skip app partitions which are not the "currently selected" OTA partition (these will not be re-encrypted unless a plaintext app image is found there.) However any partition marked with the "encrypt" flag will be unconditionally re-encrypted, meaning that any already encrypted data will be encrypted twice and corrupted.

- If secure boot is enabled, you can't reflash via serial at all unless you used chosen the "Reflashable" option for Secure Boot, pre-generated a key and burned it to the ESP32. In this case you can re-flash a plaintext secure boot digest and bootloader image at offset 0 (see `Secure Boot` documentation.) In production secure boot configuration, the secure boot digest is stored encrypted - so if ``FLASH_CRYPT_CNT`` is set to an even value then the ROM bootloader will read the encrypted digest as-is and therefore will fail to verify any bootloader image as valid.

Re-Flashing Procedure
^^^^^^^^^^^^^^^^^^^^^

The steps to update a device with plaintext via UART bootloader, when flash encryption is enabled are:

- Build the application as usual.

- Burn the ``FLASH_CRYPT_CNT`` efuse by running the command ``espefuse.py burn_efuse FLASH_CRYPT_CNT``. espefuse.py will automatically increment the bit count by 1.

- Flash the device with plaintext data as usual (``make flash`` or ``esptool.py`` commands.) Flash all previously encrypted partitions, including the bootloader. If secure boot is enabled, it must be enabled in "Reflashable" mode and a pre-generated key burned to the ESP32 - flash the bootloader-reflash-digest.bin file at offset 0x0.

- Reset the device and it will re-encrypt plaintext partitions, burn the ``FLASH_CRYPT_CNT`` flag to re-enable encryption.


Disabling Updates
^^^^^^^^^^^^^^^^^

To prevent further plaintext updates via physical access, use espefuse.py to write protect the ``FLASH_CRYPT_CNT`` efuse after flash encryption has been enabled (ie after first boot is complete)::

    espefuse.py write_protect_efuse FLASH_CRYPT_CNT

This prevents any further modifications to disable or re-enable flash encryption.

Limitations of Flash Encryption
-------------------------------

Flash Encryption prevents plaintext readout of the encrypted flash, to protect firmware against unauthorised readout and modification. It is important to understand the limitations of the flash encryption system:

- Flash encryption is only as strong as the key. For this reason, we recommend keys are generated on the device during first boot (default behaviour). If generating keys off-device to burn with ``esp_efuse.py burn_key``, ensure they are generated from a quality random number source, kept secure, and never shared between devices.

- Not all data is stored encrypted. If storing data on flash, check if the method you are using (library, API, etc.) supports flash encryption.

- Flash encryption does not prevent an attacker from understanding the high-level layout of the flash. This is because the same AES key is used for every two 16 byte AES blocks. When both adjacent 16 byte blocks contain identical content (such as empty or padding areas), these blocks will encrypt to produce matching pairs of encrypted blocks. This may allow an attacker to make high-level comparisons between encrypted devices (ie to tell if two devices are probably running the same firmware version).

- For the same reason, an attacker can always guess when two adjacent 16 byte blocks (32 byte aligned) contain identical content. Keep this in mind if storing sensitive data on the flash, design your flash storage so this doesn't happen (using a counter byte or some other non-identical value every 16 bytes is sufficient).

- Flash encryption alone may not prevent an attacker from modifying the firmware of the device. Always use flash encryption in combination with Secure Boot.


Flash Encryption Advanced Features
----------------------------------

Encrypted Partition Flag
^^^^^^^^^^^^^^^^^^^^^^^^

In the `partition table` description CSV files, there is a field for flags.

Usually left blank, if you write "encrypted" in this field then the partition will be marked as encrypted in the partition table, and data written here will be treated as encrypted (same as an app partition)::

   # Name,   Type, SubType, Offset,  Size, Flags
   nvs,      data, nvs,     0x9000,  0x6000
   phy_init, data, phy,     0xf000,  0x1000
   factory,  app,  factory, 0x10000, 1M
   secret_data, 0x40, 0x01, 0x20000, 256K, encrypted

- None of the default partition formats have any encrypted data partitions.

- It is not necessary to mark "app" partitions as encrypted, they are always treated as encrypted.

- The "encrypted" flag does nothing if flash encryption is not enabled.

- It is possible to mark the optional ``phy`` partition with ``phy_init`` data as encrypted, if you wish to protect this data from physical access readout or modification.

- It is not possible to mark the ``nvs`` partition as encrypted.

Precalculated Flash Encryption Key
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

It is possible to pre-generate a flash encryption key on the host computer and burn it into the ESP32 efuse. This allows data to be per-encrypted on the host and flashed to the ESP32 without needing a plaintext flash update.

This is useful for development, because it removes the 4 flash limit and allows reflashing with secure boot enabled.

**IMPORTANT** This method is intended to assist with development only, not for production devices. If pre-generating flash encryption for production, ensure the keys are generated from a high quality random number source and do not share the same flash encryption key across multiple devices.

Obtaining Flash Encryption Key
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Flash encryption keys are 32 bytes of random data. You can generate a random key with espsecure.py::

  espsecure.py generate_flash_encryption_key my_flash_encryption_key.bin

(The randomness of this data is only as good as the OS and it's Python installation's random data source.)

Alternatively, if you're using `secure boot` and have a secure boot signing key then you can generate a deterministic SHA-256 digest of the secure boot private key to use::

  espsecure.py digest_private-key --keyfile secure_boot_signing_key.pem my_flash_encryption_key.bin

The same key is used as the secure boot digest key if you enabled "Reflashable" mode for secure boot.

This means you can always re-calculate the flash encryption key from the secure boot private signing key. This method is **not at all suitable** for production devices.

Burning Flash Encryption Key
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Once you have generated a flash encryption key, you need to burn it to efuse on the device. This **must be done before first boot**, otherwise the ESP32 will generate a random key that software can't access.

To burn a key to the device (possible one time only)::

  espefuse.py burn_key flash_encryption my_flash_encryption_key.bin

First Flash
~~~~~~~~~~~

For the first flash, follow the same steps as for default `Flash Encryption Initialisation` and flash a plaintext image. The bootloader will enable flash encryption using the pre-burned key and encrypt all partitions.

Reflashing
~~~~~~~~~~

To reflash an encrypted image requires an additional manual update step, to encrypt the data you wish to flash.

Suppose that this is the normal flashing non-encrypted flashing step::

  esptool.py --port /dev/ttyUSB0 --baud 115200 write_flash -z 0x10000 build/my-app.bin

The data needs to be pre-encrypted with knowledge of the address (0x10000) and the binary file name::

  espsecure.py encrypt_flash_data --keyfile my_flash_encryption_key.bin --address 0x10000 -o build/my-app-encrypted.bin build/my-app.bin

This step will encrypt ``my-app.bin`` using the supplied key, and produce an encrypted file ``my-app-encrypted.bin``. Be sure that the address argument matches the address where you plan to flash the binary.

Then, flash the encrypted binary with esptool.py::

    esptool.py --port /dev/ttyUSB0 --baud 115200 write_flash -z 0x10000 build/my-app-encrypted.bin

Enabling UART Bootloader Encryption/Decryption
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

By default, on first boot the flash encryption process will burn efuses ``DISABLE_DL_ENCRYPT``, ``DISABLE_DL_DECRYPT`` and ``DISABLE_DL_CACHE``.

- ``DISABLE_DL_ENCRYPT`` disables the flash encryption operations when running in UART bootloader boot mode.
- ``DISABLE_DL_DECRYPT`` disables transparent flash decryption when running in UART bootloader mode, even if ``FLASH_CRYPT_CNT`` is set to enable it in normal operation.
- ``DISABLE_DL_CACHE`` disables the entire MMU flash cache when running in UART bootloader mode.

It is possible to burn only some of these efuses, and write-protect the rest (with unset value 0) before the first boot, in order to preserve them::

  espefuse.py burn_efuse DISABLE_DL_DECRYPT
  espefuse.py write_protect_efuse DISABLE_DL_ENCRYPT

(Note that all 3 of these efuses are disabled via one write protect bit, so write protecting one will write protect all of them.)

Write protecting these efuses when they are unset (0) is not currently useful, as ``esptool.py`` does not support flash encryption functions.

However, note that write protecting ``DISABLE_DL_DECRYPT`` when it is unset (0) effectively makes flash encryption useless, as an attacker with physical access can use UART bootloader mode to read out the flash.

Technical Details
-----------------

Flash Encryption Algorithm
^^^^^^^^^^^^^^^^^^^^^^^^^^

- AES-256 operates on 16 byte blocks of data. The flash encryption engine encrypts and decrypts data in 32 byte blocks, two AES blocks in series.

- AES algorithm is used inverted in flash encryption, so the flash encryption "encrypt" operation is AES decrypt and the "decrypt" operation is AES encrypt. This is for performance reasons and does not alter the effectiveness of the algorithm.

- The main flash encryption key is stored in efuse (BLK2) and by default is protected from further writes or software readout.

- Each 32 byte block is encrypted with a unique key which is derived from this main flash encryption key XORed with the offset of this block in the flash (a "key tweak").

- The specific tweak depends on the setting of ``FLASH_CRYPT_CONFIG`` efuse. This is a 4 bit efuse, where each bit enables XORing of a particular range of the key bits:

  - Bit 1, bits 0-66 of the key are XORed.
  - Bit 2, bits 67-131 of the key are XORed.
  - Bit 3, bits 132-194 of the key are XORed.
  - Bit 4, bits 195-256 of the key are XORed.

  It is recommended that ``FLASH_CRYPT_CONFIG`` is always left to set the default value `0xF`, so that all key bits are XORed with the block offset. See `Setting FLASH_CRYPT_CONFIG` for details.

- The high 19 bits of the block offset (bit 5 to bit 23) are XORed with the main flash encryption key. This range is chosen for two reasons: the maximum flash size is 16MB (24 bits), and each block is 32 bytes so the least significant 5 bits are always zero.

- There is a particular mapping from each of the 19 block offset bits to the 256 bits of the flash encryption key, to determine which bit is XORed with which. See the variable _FLASH_ENCRYPTION_TWEAK_PATTERN in espsecure.py for a list of these.

- For the full algorithm implemented in Python, see `_flash_encryption_operation()` in the espsecure.py source code.

Setting FLASH_CRYPT_CONFIG
^^^^^^^^^^^^^^^^^^^^^^^^^^

The ``FLASH_CRYPT_CONFIG`` efuse determines the number of bits in the flash encryption key which are "tweaked" with the block offset. See `Flash Encryption Algorithm` for details.

First boot of the bootloader always sets this value to the maximum `0xF`.

It is possible to write these efuse manually, and write protect it before first boot in order to select different tweak values. This is not recommended.

It is strongly recommended to never write protect ``FLASH_CRYPT_CONFIG`` when it the value is zero. If this efuse is set to zero, no bits in the flash encryption key are tweaked and the flash encryption algorithm is equivalent to AES ECB mode.

.. _Secure Boot: secure-boot.rst
.. _partition table: ../partition-tables.rst
