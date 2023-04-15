Host Based Security Workflows
=============================

{IDF_TARGET_CRYPT_CNT:default="SPI_BOOT_CRYPT_CNT",esp32="FLASH_CRYPT_CNT"}
{IDF_TARGET_CRYPT_CNT_MAX_VAL:default="7",esp32="127"}

Introduction
------------

It is recommended to have an uninterrupted power supply while enabling security features on ESP32 SoCs. Power failures during secure manufacturing process could cause issues that are hard to debug and, in some cases, may cause permanent boot-up failures.

This guide highlights an approach where security features are enabled with the assistance of an external host machine. Security workflows are broken down into various stages and key material is generated on the host machine; thus, allowing greater recovery chances in case of power or other failures. It also offers better timings for secure manufacturing, e.g., case of encryption of firmware on host machine vs on the device.


Goals
-----

#. Simplify the traditional workflow with stepwise instructions.
#. Design a more flexible workflow as compared to the traditional firmware based workflow.
#. Improve reliability by dividing the workflow in small operations.
#. Eliminate dependency on firmware bootloader.

Pre-requisite
-------------
* ``esptool``: Please make sure the ``esptool`` has been installed. It can be installed with help of the following command:

.. code:: bash

   pip install esptool

Scope
-----
* :ref:`enable-flash-encryption-externally`

Security Workflows
------------------

.. _enable-flash-encryption-externally:

Enable Flash Encryption Externally
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In this case all the eFuses related to flash encryption are written with help of the espefuse tool. More details about flash encryption can process can be found in the :doc:`Flash Encryption Guide </security/flash-encryption>`

1. Ensure that you have an {IDF_TARGET_NAME} device with default flash encryption eFuse settings as shown in :ref:`flash-encryption-efuse`.

  See how to check :ref:`flash-encryption-status`.

In this case the Flash on the chip must be erased and flash encryption must not be enabled.
The chip can be erased using the following command:

.. code:: bash

   esptool.py --port PORT erase_flash

2. Generate a random flash encryption key by running:

.. only:: not SOC_FLASH_ENCRYPTION_XTS_AES

    .. code-block:: bash

        espsecure.py generate_flash_encryption_key my_flash_encryption_key.bin

.. only:: SOC_FLASH_ENCRYPTION_XTS_AES_256

    If :ref:`Size of generated AES-XTS key <CONFIG_SECURE_FLASH_ENCRYPTION_KEYSIZE>` is AES-128 (256-bit key):

    .. code-block:: bash

        espsecure.py generate_flash_encryption_key my_flash_encryption_key.bin

    else if :ref:`Size of generated AES-XTS key <CONFIG_SECURE_FLASH_ENCRYPTION_KEYSIZE>` is AES-256 (512-bit key):

    .. code-block:: bash

        espsecure.py generate_flash_encryption_key --keylen 512 my_flash_encryption_key.bin


.. only:: SOC_FLASH_ENCRYPTION_XTS_AES_128 and not SOC_FLASH_ENCRYPTION_XTS_AES_256 and not SOC_EFUSE_CONSISTS_OF_ONE_KEY_BLOCK

    .. code-block:: bash

        espsecure.py generate_flash_encryption_key my_flash_encryption_key.bin

.. only:: SOC_FLASH_ENCRYPTION_XTS_AES_128 and SOC_EFUSE_CONSISTS_OF_ONE_KEY_BLOCK

      If :ref:`Size of generated AES-XTS key <CONFIG_SECURE_FLASH_ENCRYPTION_KEYSIZE>` is AES-128 (256-bit key):

      .. code-block:: bash

          espsecure.py generate_flash_encryption_key my_flash_encryption_key.bin

      else if :ref:`Size of generated AES-XTS key <CONFIG_SECURE_FLASH_ENCRYPTION_KEYSIZE>` is AES-128 key derived from 128 bits (SHA256(128 bits)):

      .. code-block:: bash

          espsecure.py generate_flash_encryption_key --keylen 128 my_flash_encryption_key.bin

3. Burn the key into your device's eFuse using the command below. This action can be done **only once**.

.. only:: not SOC_FLASH_ENCRYPTION_XTS_AES

  .. code-block:: bash

      espefuse.py --port PORT burn_key flash_encryption my_flash_encryption_key.bin

.. only:: SOC_FLASH_ENCRYPTION_XTS_AES_256

  .. code-block:: bash

      espefuse.py --port PORT burn_key BLOCK my_flash_encryption_key.bin KEYPURPOSE

  where ``BLOCK`` is a free keyblock between ``BLOCK_KEY0`` and ``BLOCK_KEY5``. And ``KEYPURPOSE`` is either ``AES_256_KEY_1``, ``XTS_AES_256_KEY_2``, ``XTS_AES_128_KEY``. See `{IDF_TARGET_NAME} Technical Reference Manual <{IDF_TARGET_TRM_EN_URL}>`_ for a description of the key purposes.

  For AES-128 (256-bit key) - ``XTS_AES_128_KEY``:

  .. code-block:: bash

      espefuse.py --port PORT burn_key BLOCK my_flash_encryption_key.bin XTS_AES_128_KEY

  For AES-256 (512-bit key) - ``XTS_AES_256_KEY_1`` and ``XTS_AES_256_KEY_2``. ``espefuse.py`` supports burning both these two key purposes together with a 512 bit key to two separate key blocks via the virtual key purpose ``XTS_AES_256_KEY``. When this is used ``espefuse.py`` will burn the first 256 bit of the key to the specified ``BLOCK`` and burn the corresponding block key purpose to ``XTS_AES_256_KEY_1``. The last 256 bit of the key will be burned to the first free key block after ``BLOCK`` and the corresponding block key purpose to ``XTS_AES_256_KEY_2``

  .. code-block:: bash

      espefuse.py --port PORT burn_key BLOCK my_flash_encryption_key.bin XTS_AES_256_KEY

  If you wish to specify exactly which two blocks are used then it is possible to divide key into two 256 bit keys, and manually burn each half with ``XTS_AES_256_KEY_1`` and ``XTS_AES_256_KEY_2`` as key purposes:

  .. code-block:: bash

    split -b 32 my_flash_encryption_key.bin my_flash_encryption_key.bin
    espefuse.py --port PORT burn_key BLOCK my_flash_encryption_key.bin.aa XTS_AES_256_KEY_1
    espefuse.py --port PORT burn_key BLOCK+1 my_flash_encryption_key.bin.ab XTS_AES_256_KEY_2


.. only:: SOC_FLASH_ENCRYPTION_XTS_AES_128 and not SOC_FLASH_ENCRYPTION_XTS_AES_256 and not SOC_EFUSE_CONSISTS_OF_ONE_KEY_BLOCK

  .. code-block:: bash

      espefuse.py --port PORT burn_key BLOCK my_flash_encryption_key.bin XTS_AES_128_KEY

  where ``BLOCK`` is a free keyblock between ``BLOCK_KEY0`` and ``BLOCK_KEY5``.

.. only:: SOC_FLASH_ENCRYPTION_XTS_AES_128 and SOC_EFUSE_CONSISTS_OF_ONE_KEY_BLOCK

  For AES-128 (256-bit key) - ``XTS_AES_128_KEY`` (the ``XTS_KEY_LENGTH_256`` eFuse will be burn to 1):

  .. code-block:: bash

      espefuse.py --port PORT burn_key BLOCK_KEY0 flash_encryption_key256.bin XTS_AES_128_KEY

  For AES-128 key derived from 128 bits (SHA256(128 bits)) - ``XTS_AES_128_KEY_DERIVED_FROM_128_EFUSE_BITS``. The FE key will be written in the lower part of eFuse BLOCK_KEY0. The upper 128 bits are not used and will remain available for reading by software. Using the special mode of the espefuse tool, shown in the ``For burning both keys together`` section below, the user can write their data to it using any espefuse commands.

  .. code-block:: bash

      espefuse.py --port PORT burn_key BLOCK_KEY0 flash_encryption_key128.bin XTS_AES_128_KEY_DERIVED_FROM_128_EFUSE_BITS

  For burning both keys together (Secure Boot and Flash Encryption):

  .. code-block:: bash

      espefuse.py --port PORT --chip esp32c2 burn_key_digest secure_boot_signing_key.pem \
                                                burn_key BLOCK_KEY0 flash_encryption_key128.bin XTS_AES_128_KEY_DERIVED_FROM_128_EFUSE_BITS


.. only:: SOC_EFUSE_BLOCK9_KEY_PURPOSE_QUIRK

    .. note::
        **WARNING**: For the {IDF_TARGET_NAME} BLOCK9 (BLOCK_KEY5) can not be used by XTS_AES keys.


4. Burn the ``{IDF_TARGET_CRYPT_CNT}`` eFuse.

If you only want to enable flash encryption in **Development** mode and want to keep the ability to disable it in future, Update the {IDF_TARGET_CRYPT_CNT} value in the below command from {IDF_TARGET_CRYPT_CNT_MAX_VAL} to 0x1. (not recommended for production)

.. code-block:: bash

    espefuse.py --port PORT --chip {IDF_TARGET_PATH_NAME} burn_efuse {IDF_TARGET_CRYPT_CNT} {IDF_TARGET_CRYPT_CNT_MAX_VAL}

.. only:: esp32

    In case of {IDF_TARGET_NAME}, you also need to burn the ``FLASH_CRYPT_CONFIG``. It can be done with help of the following command:

    .. code-block:: bash

        espefuse.py --port PORT --chip {IDF_TARGET_PATH_NAME} burn_efuse FLASH_CRYPT_CONFIG 0xF

.. note::

   At this point the flash encryption on the device has been enabled. You may test the flash encryption process as given in step 5. Please note that the security related eFuses have not been burned at this point. It is recommended that they should be burned in production use-cases as explained in step 6.

5. Encrypt and flash the binaries

The bootloader and the application binaries for the project must be built with Flash Encryption Release mode with default configurations.

Flash encryption Release mode can be set in the menuconfig as follows:

.. list::

    - :ref:`Enable flash encryption on boot <CONFIG_SECURE_FLASH_ENC_ENABLED>`
    :esp32: - :ref:`Select Release mode <CONFIG_SECURE_FLASH_ENCRYPTION_MODE>` (Note that once Release mode is selected, the ``DISABLE_DL_ENCRYPT`` and ``DISABLE_DL_DECRYPT`` eFuse bits will be burned to disable flash encryption hardware in ROM Download Mode.)
    :esp32: - :ref:`Select UART ROM download mode (Permanently disabled (recommended)) <CONFIG_SECURE_UART_ROM_DL_MODE>` (Note that this option is only available when :ref:`CONFIG_ESP32_REV_MIN` is set to 3 (ESP32 V3).) The default choice is to keep UART ROM download mode enabled, however it's recommended to permanently disable this mode to reduce the options available to an attacker.
    :not esp32: - :ref:`Select Release mode <CONFIG_SECURE_FLASH_ENCRYPTION_MODE>` (Note that once Release mode is selected, the ``EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT`` eFuse bit will be burned to disable flash encryption hardware in ROM Download Mode.)
    :not esp32: - :ref:`Select UART ROM download mode (Permanently switch to Secure mode (recommended)) <CONFIG_SECURE_UART_ROM_DL_MODE>`. This is the default option, and is recommended. It is also possible to change this configuration setting to permanently disable UART ROM download mode, if this mode is not needed.
    - :ref:`Select the appropriate bootloader log verbosity <CONFIG_BOOTLOADER_LOG_LEVEL>`
    - Save the configuration and exit.


The binaries can be encrypted on the host machine using the following command

.. only:: esp32

    .. code-block:: bash
       
       espsecure.py encrypt_flash_data --keyfile my_flash_encryption_key.bin --address 0x1000 --output bootloader-enc.bin build/bootloader/bootloader.bin

       espsecure.py encrypt_flash_data --keyfile my_flash_encryption_key.bin --address 0x8000 --output partition-table-enc.bin build/partition_table/partition-table.bin

       espsecure.py encrypt_flash_data --keyfile my_flash_encryption_key.bin --address 0x10000 --output my-app-enc.bin build/my-app.bin

.. only:: not esp32

    .. code-block:: bash

       espsecure.py encrypt_flash_data --aes_xts --keyfile my_flash_encryption_key.bin --address 0x0 --output bootloader-enc.bin build/my-app.bin

       espsecure.py encrypt_flash_data --aes_xts --keyfile my_flash_encryption_key.bin --address 0x8000 --output partition-table-enc.bin build/partition_table/partition-table.bin

       espsecure.py encrypt_flash_data --aes_xts --keyfile my_flash_encryption_key.bin --address 0x10000 --output my-app-enc.bin build/my-app.bin

The above files can then be flashed to their respective offset using ``esptool.py``. To see all of the command line options recommended for ``esptool.py``, see the output printed when ``idf.py build`` succeeds. In the above command the offsets are used for a sample firmware, the actual offset for your firmware can be obtained by checking the partition table entry or by running `idf.py partition-table`. When the application contains following partition: ``otadata``, ``nvs_encryption_keys`` they need to be encrypted as well. Please refer to :ref:`encrypted-partitions` for more details about encrypted partitions.

.. note::

   If the flashed ciphertext file is not recognized by the {IDF_TARGET_NAME} when it boots, check that the keys match and that the command line arguments match exactly, including the correct offset. It is important to provide the correct offset as the ciphertext changes when the offset changes.

   .. only:: esp32

       If your ESP32 uses non-default :ref:`FLASH_CRYPT_CONFIG value in eFuse <setting-flash-crypt-config>` then you will need to pass the ``--flash_crypt_conf`` argument to ``espsecure.py`` to set the matching value. This will not happen if the device configured flash encryption by itself, but may happen if burning eFuses manually to enable flash encryption.

The command ``espsecure.py decrypt_flash_data`` can be used with the same options (and different input/output files), to decrypt ciphertext flash contents or a previously encrypted file.

6. Burn flash encryption related security eFuses as listed below:

**A) Burn security eFuses:**

.. list::

    :esp32: - ``DISABLE_DL_ENCRYPT``: Disable the UART bootloader encryption access.
    :esp32: - ``DISABLE_DL_DECRYPT``: Disable the UART bootloader decryption access.
    :esp32: - ``DISABLE_DL_CACHE``: Disable the UART bootloader flash cache access.
    :esp32: - ``JTAG_DISABLE``: Disable the JTAG
    :SOC_EFUSE_DIS_BOOT_REMAP: - ``DIS_BOOT_REMAP``: Disable capability to Remap ROM to RAM address space
    :SOC_EFUSE_DIS_DOWNLOAD_ICACHE: - ``DIS_DOWNLOAD_ICACHE``: Disable UART cache
    :SOC_EFUSE_DIS_DOWNLOAD_DCACHE: - ``DIS_DOWNLOAD_DCACHE``: Disable UART cache.
    :SOC_EFUSE_HARD_DIS_JTAG: - ``HARD_DIS_JTAG``: Hard disable JTAG peripheral
    :SOC_EFUSE_DIS_DIRECT_BOOT:- ``DIS_DIRECT_BOOT``: Disable direct boot (legacy SPI boot mode)
    :SOC_EFUSE_DIS_LEGACY_SPI_BOOT: - ``DIS_LEGACY_SPI_BOOT``: Disable legacy SPI boot mode
    :SOC_EFUSE_DIS_USB_JTAG: - ``DIS_USB_JTAG``: Disable USB switch to JTAG
    :SOC_EFUSE_DIS_PAD_JTAG: - ``DIS_PAD_JTAG``: Disable JTAG permanently
    :not esp32: - ``DIS_DOWNLOAD_MANUAL_ENCRYPT``: Disable UART bootloader encryption access

**Burning all of the eFuses listed above is recommended for production use-case.**
The respective eFuses can be burned with help of the following command:

.. code:: bash

    espefuse.py burn_efuse --port PORT EFUSE_NAME 0x1

.. note:: Please update the EFUSE_NAME with the eFuse that you need to burn. Multiple eFuses can be burned at the same time by appending them to the above command (e.g., EFUSE_NAME VAL EFUSE_NAME2 VAL2). More documentation about `espefuse.py` can be found `here <https://docs.espressif.com/projects/esptool/en/latest/esp32/espefuse/index.html>`_

**B) Write protect security eFuses:**

After burning the respective eFuses we need to write_protect the security configurations

.. only:: esp32

    .. code:: bash

        espefuse.py --port PORT write_protect_efuse MAC

    .. note::

        The write disable bit for MAC also write disables DIS_CACHE which is required to prevent accidental burning of this bit.

    **C) Disable UART ROM DL mode**

.. only:: not esp32

    .. code:: bash

        espefuse.py --port PORT write_protect_efuse DIS_ICACHE

    .. note::

        The write protection of above eFuse also write protects multiple other eFuses, Please refer to the {IDF_TARGET_NAME} eFuse table for more details.

    **C) Enable Security Download mode:**

**WARNING:** Please burn the following bit at the very end. After this bit is burned, the espefuse tool can no longer be used to burn additional eFuses.

.. list::

    :esp32: - ``UART_DOWNLOAD_DIS`` : Disable the UART ROM Download mode.
    :not esp32: - ``ENABLE_SECURITY_DOWNLOAD``: Enable Secure ROM download mode

.. only:: esp32

    The eFuse can be burned with following command:

    .. code:: bash

        espefuse.py --port PORT burn_efuse UART_DOWNLOAD_DIS

.. only:: not esp32

    The eFuse can be burned with following command:

    .. code:: bash

        espefuse.py --port PORT burn_efuse ENABLE_SECURITY_DOWNLOAD

.. important::
    7. Delete flash encryption key on host:

    * Once the flash encryption has been enabled for the device, the key **must be deleted immediately**. This ensures that the host cannot produce encrypted binaries for the same device going forward. This step is important to reduce the vulnerability of the flash encryption key.
    * It is recommended to generate a unique flash encryption key for each device for production use-cases.

