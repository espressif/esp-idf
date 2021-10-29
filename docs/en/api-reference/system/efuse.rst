eFuse Manager
=============


Introduction
------------

The eFuse Manager library is designed to structure access to eFuse bits and make using these easy. This library operates eFuse bits by a structure name which is assigned in eFuse table. This sections introduces some concepts used by eFuse Manager.


Hardware description
--------------------

The {IDF_TARGET_NAME} has a number of eFuses which can store system and user parameters. Each eFuse is a one-bit field which can be programmed to 1 after which it cannot be reverted back to 0.
Some of system parameters are using these eFuse bits directly by hardware modules and have special place (for example EFUSE_BLK0).

For more details, see *{IDF_TARGET_NAME} Technical Reference Manual* > *eFuse Controller (eFuse)* [`PDF <{IDF_TARGET_TRM_EN_URL}#efuse>`__]. Some eFuse bits are available for user applications.

.. only:: esp32

    {IDF_TARGET_NAME} has 4 eFuse blocks each of the size of 256 bits (not all bits are available):

    * EFUSE_BLK0 is used entirely for system purposes;
    * EFUSE_BLK1 is used for flash encrypt key. If not using that Flash Encryption feature, they can be used for another purpose;
    * EFUSE_BLK2 is used for security boot key. If not using that Secure Boot feature, they can be used for another purpose;
    * EFUSE_BLK3 can be partially reserved for the custom MAC address, or used entirely for user application. Note that some bits are already used in IDF.

.. only:: not esp32

    {IDF_TARGET_NAME} has 11 eFuse blocks each of the size of 256 bits (not all bits are available):

    * EFUSE_BLK0 is used entirely for system purposes;
    * EFUSE_BLK1 is used entirely for system purposes;
    * EFUSE_BLK2 is used entirely for system purposes;
    * EFUSE_BLK3 or EFUSE_BLK_USER_DATA can be used for user purposes;
    * EFUSE_BLK4 or EFUSE_BLK_KEY0 can be used as key (for secure_boot or flash_encryption) or for user purposes;
    * EFUSE_BLK5 or EFUSE_BLK_KEY1 can be used as key (for secure_boot or flash_encryption) or for user purposes;
    * EFUSE_BLK6 or EFUSE_BLK_KEY2 can be used as key (for secure_boot or flash_encryption) or for user purposes;
    * EFUSE_BLK7 or EFUSE_BLK_KEY3 can be used as key (for secure_boot or flash_encryption) or for user purposes;
    * EFUSE_BLK8 or EFUSE_BLK_KEY4 can be used as key (for secure_boot or flash_encryption) or for user purposes;
    * EFUSE_BLK9 or EFUSE_BLK_KEY5 can be used as key (for secure_boot or flash_encryption) or for user purposes;
    * EFUSE_BLK10 or EFUSE_BLK_SYS_DATA_PART2 is reseved for system purposes.


Each block is divided into 8 32-bits registers.


eFuse Manager component
-----------------------

The component has API functions for reading and writing fields. Access to the fields is carried out through the structures that describe the location of the eFuse bits in the blocks. The component provides the ability to form fields of any length and from any number of individual bits. The description of the fields is made in a CSV file in a table form. To generate from a tabular form (CSV file) in the C-source uses the tool `efuse_table_gen.py`. The tool checks the CSV file for uniqueness of field names and bit intersection, in case of using a `custom` file from the user's project directory, the utility will check with the `common` CSV file.

CSV files:

* common (`esp_efuse_table.csv`) - contains eFuse fields which are used inside the IDF. C-source generation should be done manually when changing this file (run command ``idf.py efuse-common-table``). Note that changes in this file can lead to incorrect operation.
* custom - (optional and can be enabled by :ref:`CONFIG_EFUSE_CUSTOM_TABLE`) contains eFuse fields that are used by the user in their application. C-source generation should be done manually when changing this file and running ``idf.py efuse-custom-table``.


Description CSV file
--------------------

The CSV file contains a description of the eFuse fields. In the simple case, one field has one line of description.
Table header:

{IDF_TARGET_MAX_EFUSE_BLK:default = "EFUSE_BLK10", esp32 = "EFUSE_BLK3"}

.. code-block:: none

    # field_name,  efuse_block(EFUSE_BLK0..{IDF_TARGET_MAX_EFUSE_BLK}), bit_start(0..255),    bit_count(1..256),        comment

Individual params in CSV file the following meanings:

field_name
    Name of field. The prefix `ESP_EFUSE_` will be added to the name, and this field name will be available in the code. This name will be used to access the fields. The name must be unique for all fields. If the line has an empty name, then this line is combined with the previous field. This allows you to set an arbitrary order of bits in the field, and expand the field as well (see ``MAC_FACTORY`` field in the common table). The field_name supports structured format using `.` to show that the field belongs to another field (see ``WR_DIS`` and ``RD_DIS`` in the common table).

efuse_block
    Block number. It determines where the eFuse bits will be placed for this field. Available EFUSE_BLK0..{IDF_TARGET_MAX_EFUSE_BLK}.

bit_start
    Start bit number (0..255). The bit_start field can be omitted. In this case, it will be set to bit_start + bit_count from the previous record, if it has the same efuse_block. Otherwise (if efuse_block is different, or this is the first entry), an error will be generated.

.. only:: esp32

    bit_count
        The number of bits to use in this field (1..-). This parameter can not be omitted. This field also may be ``MAX_BLK_LEN`` in this case, the field length will have the maximum block length, taking into account the coding scheme (applicable for ``ESP_EFUSE_SECURE_BOOT_KEY`` and ``ESP_EFUSE_ENCRYPT_FLASH_KEY`` fields). The value ``MAX_BLK_LEN`` depends on :ref:`CONFIG_EFUSE_CODE_SCHEME_SELECTOR`, will be replaced with "None" - 256, "3/4" - 192, "REPEAT" - 128.

.. only:: not esp32

    bit_count
        The number of bits to use in this field (1..-). This parameter can not be omitted. This field also may be ``MAX_BLK_LEN`` in this case, the field length will have the maximum block length.

comment
    This param is using for comment field, it also move to C-header file. The comment field can be omitted.

If a non-sequential bit order is required to describe a field, then the field description in the following lines should be continued without specifying a name, this will indicate that it belongs to one field. For example two fields MAC_FACTORY and MAC_FACTORY_CRC:

.. code-block:: none

    # Factory MAC address #
    #######################
    MAC_FACTORY,            EFUSE_BLK0,    72,    8,    Factory MAC addr [0]
    ,                       EFUSE_BLK0,    64,    8,    Factory MAC addr [1]
    ,                       EFUSE_BLK0,    56,    8,    Factory MAC addr [2]
    ,                       EFUSE_BLK0,    48,    8,    Factory MAC addr [3]
    ,                       EFUSE_BLK0,    40,    8,    Factory MAC addr [4]
    ,                       EFUSE_BLK0,    32,    8,    Factory MAC addr [5]
    MAC_FACTORY_CRC,        EFUSE_BLK0,    80,    8,    CRC8 for factory MAC address

This field will available in code as ESP_EFUSE_MAC_FACTORY and ESP_EFUSE_MAC_FACTORY_CRC.

Structured efuse fields
-----------------------

.. code-block:: none

    WR_DIS,                           EFUSE_BLK0,   0,    32,     Write protection
    WR_DIS.RD_DIS,                    EFUSE_BLK0,   0,    1,      Write protection for RD_DIS
    WR_DIS.FIELD_1,                   EFUSE_BLK0,   1,    1,      Write protection for FIELD_1
    WR_DIS.FIELD_2,                   EFUSE_BLK0,   2,    4,      Write protection for FIELD_2 (includes B1 and B2)
    WR_DIS.FIELD_2.B1,                EFUSE_BLK0,   2,    2,      Write protection for FIELD_2.B1
    WR_DIS.FIELD_2.B2,                EFUSE_BLK0,   4,    2,      Write protection for FIELD_2.B2
    WR_DIS.FIELD_3,                   EFUSE_BLK0,   5,    1,      Write protection for FIELD_3
    WR_DIS.FIELD_3.ALIAS,             EFUSE_BLK0,   5,    1,      Write protection for FIELD_3 (just a alias for WR_DIS.FIELD_3)
    WR_DIS.FIELD_4,                   EFUSE_BLK0,   7,    1,      Write protection for FIELD_4

The structured eFuse field looks like ``WR_DIS.RD_DIS`` where the dot points that this field belongs to the parent field - ``WR_DIS`` and can not be out of the parent's range.

It is possible to use some levels of structured fields as WR_DIS.FIELD_2.B1 and B2. These fields should not be crossed each other and should be in the range of two fields: ``WR_DIS`` and ``WR_DIS.FIELD_2``.

It is possible to create aliases for fields with the same range, see ``WR_DIS.FIELD_3`` and ``WR_DIS.FIELD_3.ALIAS``.

The IDF names for structured efuse fields should be unique. The ``efuse_table_gen`` tool will generate the final names where the dot will be replaced by ``_``. The names for using in IDF are ESP_EFUSE_WR_DIS, ESP_EFUSE_WR_DIS_RD_DIS, ESP_EFUSE_WR_DIS_FIELD_2_B1, etc.

efuse_table_gen.py tool
-----------------------

The tool is designed to generate C-source files from CSV file and validate fields. First of all, the check is carried out on the uniqueness of the names and overlaps of the field bits. If an additional `custom` file is used, it will be checked with the existing `common` file (esp_efuse_table.csv). In case of errors, a message will be displayed and the string that caused the error. C-source files contain structures of type `esp_efuse_desc_t`.

To generate a `common` files, use the following command ``idf.py efuse-common-table`` or:

.. code-block:: bash

    cd $IDF_PATH/components/efuse/
    ./efuse_table_gen.py {IDF_TARGET_PATH_NAME}/esp_efuse_table.csv

After generation in the folder $IDF_PATH/components/efuse/`{IDF_TARGET_PATH_NAME}` create:

* `esp_efuse_table.c` file.
* In `include` folder `esp_efuse_table.c` file.

To generate a `custom` files, use the following command ``idf.py efuse-custom-table`` or:

.. code-block:: bash

    cd $IDF_PATH/components/efuse/
    ./efuse_table_gen.py {IDF_TARGET_PATH_NAME}/esp_efuse_table.csv PROJECT_PATH/main/esp_efuse_custom_table.csv

After generation in the folder PROJECT_PATH/main create:

* `esp_efuse_custom_table.c` file.
* In `include` folder `esp_efuse_custom_table.c` file.

To use the generated fields, you need to include two files:

.. code-block:: c

    #include "esp_efuse.h"
    #include "esp_efuse_table.h" or "esp_efuse_custom_table.h"


Supported coding scheme
-----------------------

.. only:: esp32

    eFuse have three coding schemes:

    * ``None`` (value 0).
    * ``3/4`` (value 1).
    * ``Repeat`` (value 2).

    The coding scheme affects only EFUSE_BLK1, EFUSE_BLK2 and EFUSE_BLK3 blocks. EUSE_BLK0 block always has a coding scheme ``None``.
    Coding changes the number of bits that can be written into a block, the block length is constant 256, some of these bits are used for encoding and not avaliable for the user.

    When using a coding scheme, the length of the payload that can be written is limited (for more details ``20.3.1.3 System Parameter coding_scheme``):

    * None 256 bits.
    * 3/4 192 bits.
    * Repeat 128 bits.

    You can find out the coding scheme of your chip:

    * run a ``espefuse.py -p PORT summary`` command.
    * from ``esptool`` utility logs (during flashing).
    * calling the function in the code :cpp:func:`esp_efuse_get_coding_scheme` for the EFUSE_BLK3 block.

    eFuse tables must always comply with the coding scheme in the chip. There is an :ref:`CONFIG_EFUSE_CODE_SCHEME_SELECTOR` option to select the coding type for tables in a Kconfig. When generating source files, if your tables do not follow the coding scheme, an error message will be displayed. Adjust the length or offset fields.
    If your program was compiled with ``None`` encoding and ``3/4`` is used in the chip, then the ``ESP_ERR_CODING`` error may occur when calling the eFuse API (the field is outside the block boundaries). If the field matches the new block boundaries, then the API will work without errors.

    Also, 3/4 coding scheme imposes restrictions on writing bits belonging to one coding unit. The whole block with a length of 256 bits is divided into 4 coding units, and in each coding unit there are 6 bytes of useful data and 2 service bytes. These 2 service bytes contain the checksum of the previous 6 data bytes.

    It turns out that only one field can be written into one coding unit. Repeated rewriting in one coding unit is prohibited. But if the record was made in advance or through a :cpp:func:`esp_efuse_write_block` function, then reading the fields belonging to one coding unit is possible.

    In case ``3/4`` coding scheme, the writing process is divided into the coding units and we can not use the usual mode of writing some fields. We can prepare all the data for writing and burn it in one time. You can also use this mode for ``None`` coding scheme but it is not necessary. It is important for ``3/4`` coding scheme.
    ``The batch writing mode`` blocks ``esp_efuse_read_...`` operations.

    After changing the coding scheme, run ``efuse_common_table`` and ``efuse_custom_table`` commands to check the tables of the new coding scheme.

.. only:: not esp32

    Coding schemes are used to protect against data corruption. {IDF_TARGET_NAME} supports two coding schemes:

    * ``None``. EFUSE_BLK0 is stored with four backups, meaning each bit is stored four times. This backup scheme is automatically applied by the hardware and is not visible to software. EFUSE_BLK0 can be written many times.
    * ``RS``. EFUSE_BLK1 - EFUSE_BLK10 use Reed-Solomon coding scheme that supports up to 5 bytes of automatic error correction. Software will encode the 32-byte EFUSE_BLKx using RS (44, 32) to generate a 12-byte check code, and then burn the EFUSE_BLKx and the check code into eFuse at the same time. The eFuse Controller automatically decodes the RS encoding and applies error correction when reading back the eFuse block. Because the RS check codes are generated across the entire 256-bit eFuse block, each block can only be written to one time.

To write some fields into one block, or different blocks in one time, you need to use ``the batch writing mode``. Firstly set this mode through :cpp:func:`esp_efuse_batch_write_begin` function then write some fields as usual using the ``esp_efuse_write_...`` functions. At the end to burn them, call the :cpp:func:`esp_efuse_batch_write_commit` function. It burns prepared data to the eFuse blocks and disables the ``batch recording mode``.

eFuse API
---------

Access to the fields is via a pointer to the description structure. API functions have some basic operation:

* :cpp:func:`esp_efuse_read_field_blob` - returns an array of read eFuse bits.
* :cpp:func:`esp_efuse_read_field_cnt` - returns the number of bits programmed as "1".
* :cpp:func:`esp_efuse_write_field_blob` - writes an array.
* :cpp:func:`esp_efuse_write_field_cnt` - writes a required count of bits as "1".
* :cpp:func:`esp_efuse_get_field_size` - returns the number of bits by the field name.
* :cpp:func:`esp_efuse_read_reg` - returns value of eFuse register.
* :cpp:func:`esp_efuse_write_reg` - writes value to eFuse register.
* :cpp:func:`esp_efuse_get_coding_scheme` - returns eFuse coding scheme for blocks.
* :cpp:func:`esp_efuse_read_block` - reads key to eFuse block starting at the offset and the required size.
* :cpp:func:`esp_efuse_write_block` - writes key to eFuse block starting at the offset and the required size.
* :cpp:func:`esp_efuse_batch_write_begin` - set the batch mode of writing fields.
* :cpp:func:`esp_efuse_batch_write_commit` - writes all prepared data for batch writing mode and reset the batch writing mode.
* :cpp:func:`esp_efuse_batch_write_cancel` - reset the batch writing mode and prepared data.
* :cpp:func:`esp_efuse_get_key_dis_read` - Returns a read protection for the key block.
* :cpp:func:`esp_efuse_set_key_dis_read` - Sets a read protection for the key block.
* :cpp:func:`esp_efuse_get_key_dis_write` - Returns a write protection for the key block.
* :cpp:func:`esp_efuse_set_key_dis_write` - Sets a write protection for the key block.
* :cpp:func:`esp_efuse_get_key_purpose` - Returns the current purpose set for an eFuse key block.
* :cpp:func:`esp_efuse_write_key` - Programs a block of key data to an eFuse block
* :cpp:func:`esp_efuse_write_keys` - Programs keys to unused eFuse blocks
* :cpp:func:`esp_efuse_find_purpose` - Finds a key block with the particular purpose set.
* :cpp:func:`esp_efuse_get_keypurpose_dis_write` - Returns a write protection of the key purpose field for an eFuse key block (for esp32 always true).
* :cpp:func:`esp_efuse_key_block_unused` - Returns true if the key block is unused, false otherwise.

For frequently used fields, special functions are made, like this :cpp:func:`esp_efuse_get_chip_ver`, :cpp:func:`esp_efuse_get_pkg_ver`.

.. only:: not esp32

    eFuse API for keys
    ------------------

    EFUSE_BLK_KEY0 - EFUSE_BLK_KEY5 are intended to keep up to 6 keys with a length of 256-bits. Each key has an ``ESP_EFUSE_KEY_PURPOSE_x`` field which defines the purpose of these keys. The purpose field is described in :cpp:type:`esp_efuse_purpose_t`.

    The purposes like ``ESP_EFUSE_KEY_PURPOSE_XTS_AES_...`` are used for flash encryption.

    The purposes like ``ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST...`` are used for secure boot.

    There are some eFuse APIs useful to work with states of keys.

    * :cpp:func:`esp_efuse_get_purpose_field` - Returns a pointer to a key purpose for an eFuse key block.
    * :cpp:func:`esp_efuse_get_key` - Returns a pointer to a key block.
    * :cpp:func:`esp_efuse_set_key_purpose` - Sets a key purpose for an eFuse key block.
    * :cpp:func:`esp_efuse_set_keypurpose_dis_write` - Sets a write protection of the key purpose field for an eFuse key block.
    * :cpp:func:`esp_efuse_find_unused_key_block` - Search for an unused key block and return the first one found.
    * :cpp:func:`esp_efuse_count_unused_key_blocks` - Returns the number of unused eFuse key blocks in the range EFUSE_BLK_KEY0..EFUSE_BLK_KEY_MAX
    * :cpp:func:`esp_efuse_get_digest_revoke` - Returns the status of the Secure Boot public key digest revocation bit.
    * :cpp:func:`esp_efuse_set_digest_revoke` - Sets the Secure Boot public key digest revocation bit.
    * :cpp:func:`esp_efuse_get_write_protect_of_digest_revoke` - Returns a write protection of the Secure Boot public key digest revocation bit.
    * :cpp:func:`esp_efuse_set_write_protect_of_digest_revoke` - Sets a write protection of the Secure Boot public key digest revocation bit.


How to add a new field
----------------------

1. Find a free bits for field. Show `esp_efuse_table.csv` file or run ``idf.py show-efuse-table`` or the next command:

.. code-block:: none

    $ ./efuse_table_gen.py {IDF_TARGET_PATH_NAME}/esp_efuse_table.csv --info
    eFuse coding scheme: NONE
    #       field_name                      efuse_block     bit_start       bit_count
    1       WR_DIS_FLASH_CRYPT_CNT          EFUSE_BLK0         2               1
    2       WR_DIS_BLK1                     EFUSE_BLK0         7               1
    3       WR_DIS_BLK2                     EFUSE_BLK0         8               1
    4       WR_DIS_BLK3                     EFUSE_BLK0         9               1
    5       RD_DIS_BLK1                     EFUSE_BLK0         16              1
    6       RD_DIS_BLK2                     EFUSE_BLK0         17              1
    7       RD_DIS_BLK3                     EFUSE_BLK0         18              1
    8       FLASH_CRYPT_CNT                 EFUSE_BLK0         20              7
    9       MAC_FACTORY                     EFUSE_BLK0         32              8
    10      MAC_FACTORY                     EFUSE_BLK0         40              8
    11      MAC_FACTORY                     EFUSE_BLK0         48              8
    12      MAC_FACTORY                     EFUSE_BLK0         56              8
    13      MAC_FACTORY                     EFUSE_BLK0         64              8
    14      MAC_FACTORY                     EFUSE_BLK0         72              8
    15      MAC_FACTORY_CRC                 EFUSE_BLK0         80              8
    16      CHIP_VER_DIS_APP_CPU            EFUSE_BLK0         96              1
    17      CHIP_VER_DIS_BT                 EFUSE_BLK0         97              1
    18      CHIP_VER_PKG                    EFUSE_BLK0        105              3
    19      CHIP_CPU_FREQ_LOW               EFUSE_BLK0        108              1
    20      CHIP_CPU_FREQ_RATED             EFUSE_BLK0        109              1
    21      CHIP_VER_REV1                   EFUSE_BLK0        111              1
    22      ADC_VREF_AND_SDIO_DREF          EFUSE_BLK0        136              6
    23      XPD_SDIO_REG                    EFUSE_BLK0        142              1
    24      SDIO_TIEH                       EFUSE_BLK0        143              1
    25      SDIO_FORCE                      EFUSE_BLK0        144              1
    26      ENCRYPT_CONFIG                  EFUSE_BLK0        188              4
    27      CONSOLE_DEBUG_DISABLE           EFUSE_BLK0        194              1
    28      ABS_DONE_0                      EFUSE_BLK0        196              1
    29      DISABLE_JTAG                    EFUSE_BLK0        198              1
    30      DISABLE_DL_ENCRYPT              EFUSE_BLK0        199              1
    31      DISABLE_DL_DECRYPT              EFUSE_BLK0        200              1
    32      DISABLE_DL_CACHE                EFUSE_BLK0        201              1
    33      ENCRYPT_FLASH_KEY               EFUSE_BLK1         0              256
    34      SECURE_BOOT_KEY                 EFUSE_BLK2         0              256
    35      MAC_CUSTOM_CRC                  EFUSE_BLK3         0               8
    36      MAC_CUSTOM                      EFUSE_BLK3         8               48
    37      ADC1_TP_LOW                     EFUSE_BLK3         96              7
    38      ADC1_TP_HIGH                    EFUSE_BLK3        103              9
    39      ADC2_TP_LOW                     EFUSE_BLK3        112              7
    40      ADC2_TP_HIGH                    EFUSE_BLK3        119              9
    41      SECURE_VERSION                  EFUSE_BLK3        128              32
    42      MAC_CUSTOM_VER                  EFUSE_BLK3        184              8

    Used bits in eFuse table:
    EFUSE_BLK0
    [2 2] [7 9] [16 18] [20 27] [32 87] [96 97] [105 109] [111 111] [136 144] [188 191] [194 194] [196 196] [198 201]

    EFUSE_BLK1
    [0 255]

    EFUSE_BLK2
    [0 255]

    EFUSE_BLK3
    [0 55] [96 159] [184 191]

    Note: Not printed ranges are free for using. (bits in EFUSE_BLK0 are reserved for Espressif)

    Parsing eFuse CSV input file $IDF_PATH/components/efuse/{IDF_TARGET_PATH_NAME}/esp_efuse_table.csv ...
    Verifying eFuse table...


The number of bits not included in square brackets is free (bits in EFUSE_BLK0 are reserved for Espressif). All fields are checked for overlapping.

2. Fill a line for field: field_name, efuse_block, bit_start, bit_count, comment.

3. Run a ``show_efuse_table`` command to check eFuse table. To generate source files run ``efuse_common_table`` or ``efuse_custom_table`` command.

Debug eFuse & Unit tests
------------------------

Virtual eFuses
^^^^^^^^^^^^^^

The Kconfig option :ref:`CONFIG_EFUSE_VIRTUAL` will virtualize eFuse values inside the eFuse Manager, so writes are emulated and no eFuse values are permanently changed. This can be useful for debugging app and unit tests.
During startup, the eFuses are copied to RAM. All eFuse operations (read and write) are performed with RAM instead of the real eFuse registers.

In addition to the :ref:`CONFIG_EFUSE_VIRTUAL` option there is :ref:`CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH` option that adds a feature to keep eFuses in flash memory. To use this mode the partition_table should have the `efuse` partition. partition.csv: ``"efuse_em, data, efuse,   ,   0x2000,"``.
During startup, the eFuses are copied from flash or, in case if flash is empty, from real eFuse to RAM and then update flash. This option allows keeping eFuses after reboots (possible to test secure_boot and flash_encryption features with this option).

espefuse.py
^^^^^^^^^^^

esptool includes a useful tool for reading/writing {IDF_TARGET_NAME} eFuse bits - `espefuse.py <https://github.com/espressif/esptool/wiki/espefuse>`_.

   .. include:: inc/espefuse_summary_{IDF_TARGET_NAME}.rst


.. include-build-file:: inc/esp_efuse.inc
