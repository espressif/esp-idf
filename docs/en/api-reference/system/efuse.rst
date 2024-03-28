eFuse Manager
=============

{IDF_TARGET_CODING_SCHEMES:default="Reed-Solomon", esp32="3/4 or Repeat"}


Introduction
------------

The eFuse Manager library is designed to structure access to eFuse bits and make using these easy. This library operates eFuse bits by a structure name which is assigned in eFuse table. This sections introduces some concepts used by eFuse Manager.


Hardware Description
--------------------

The {IDF_TARGET_NAME} has a number of eFuses which can store system and user parameters. Each eFuse is a one-bit field which can be programmed to 1 after which it cannot be reverted back to 0.
Some of system parameters are using these eFuse bits directly by hardware modules and have special place (for example EFUSE_BLK0).

For more details, see **{IDF_TARGET_NAME} Technical Reference Manual** > **eFuse Controller (eFuse)** [`PDF <{IDF_TARGET_TRM_EN_URL}#efuse>`__]. Some eFuse bits are available for user applications.

.. only:: esp32

    {IDF_TARGET_NAME} has 4 eFuse blocks each of the size of 256 bits (not all bits are available):

    * EFUSE_BLK0 is used entirely for system purposes;
    * EFUSE_BLK1 is used for flash encrypt key. If not using that Flash Encryption feature, they can be used for another purpose;
    * EFUSE_BLK2 is used for security boot key. If not using that Secure Boot feature, they can be used for another purpose;
    * EFUSE_BLK3 can be partially reserved for the custom MAC address, or used entirely for user application. Note that some bits are already used in ESP-IDF.

.. only:: not esp32 and not esp32c2

  .. list::

    {IDF_TARGET_NAME} has 11 eFuse blocks each of the size of 256 bits (not all bits are available):

    * EFUSE_BLK0 is used entirely for system purposes;
    * EFUSE_BLK1 is used entirely for system purposes;
    * EFUSE_BLK2 is used entirely for system purposes;
    * EFUSE_BLK3 (also named EFUSE_BLK_USER_DATA) can be used for user purposes;
    * EFUSE_BLK4 (also named EFUSE_BLK_KEY0) can be used as key (for secure_boot or flash_encryption) or for user purposes;
    * EFUSE_BLK5 (also named EFUSE_BLK_KEY1) can be used as key (for secure_boot or flash_encryption) or for user purposes;
    * EFUSE_BLK6 (also named EFUSE_BLK_KEY2) can be used as key (for secure_boot or flash_encryption) or for user purposes;
    * EFUSE_BLK7 (also named EFUSE_BLK_KEY3) can be used as key (for secure_boot or flash_encryption) or for user purposes;
    * EFUSE_BLK8 (also named EFUSE_BLK_KEY4) can be used as key (for secure_boot or flash_encryption) or for user purposes;
    :SOC_EFUSE_BLOCK9_KEY_PURPOSE_QUIRK and SOC_ECDSA_SUPPORTED: * EFUSE_BLK9 (also named EFUSE_BLK_KEY5) can be used for any purpose except for flash encryption or ECDSA (due to a HW bug);
    :SOC_EFUSE_BLOCK9_KEY_PURPOSE_QUIRK and not SOC_ECDSA_SUPPORTED: * EFUSE_BLK9 (also named EFUSE_BLK_KEY5) can be used for any purpose except for flash encryption (due to a HW bug);
    :not SOC_EFUSE_BLOCK9_KEY_PURPOSE_QUIRK: * EFUSE_BLK9 (also named EFUSE_BLK_KEY5) can be used as key (for secure_boot or flash_encryption) or for user purposes;
    * EFUSE_BLK10 (also named EFUSE_BLK_SYS_DATA_PART2) is reserved for system purposes.

.. only:: esp32c2

    {IDF_TARGET_NAME} has 4 eFuse blocks each of the size of 256 bits (not all bits are available):

    * EFUSE_BLK0 is used entirely for system purposes;
    * EFUSE_BLK1 is used entirely for system purposes;
    * EFUSE_BLK2 is used entirely for system purposes;
    * EFUSE_BLK3 (also named EFUSE_BLK_KEY0) can be used as key (for secure_boot or flash_encryption) or for user purposes;

Each block is divided into 8 32-bits registers.


eFuse Manager Component
-----------------------

The component has API functions for reading and writing fields. Access to the fields is carried out through the structures that describe the location of the eFuse bits in the blocks. The component provides the ability to form fields of any length and from any number of individual bits. The description of the fields is made in a CSV file in a table form. To generate from a tabular form (CSV file) in the C-source uses the tool ``efuse_table_gen.py``. The tool checks the CSV file for uniqueness of field names and bit intersection, in case of using a `custom` file from the user's project directory, the utility checks with the `common` CSV file.

CSV files:

* common (`esp_efuse_table.csv`) - contains eFuse fields which are used inside the ESP-IDF. C-source generation should be done manually when changing this file (run command ``idf.py efuse-common-table``). Note that changes in this file can lead to incorrect operation.
* custom - (optional and can be enabled by :ref:`CONFIG_EFUSE_CUSTOM_TABLE`) contains eFuse fields that are used by the user in their application. C-source generation should be done manually when changing this file and running ``idf.py efuse-custom-table``.


Description CSV File
--------------------

The CSV file contains a description of the eFuse fields. In the simple case, one field has one line of description.
Table header:

{IDF_TARGET_MAX_EFUSE_BLK:default = "EFUSE_BLK10", esp32 = "EFUSE_BLK3", esp32c2 = "EFUSE_BLK3"}

.. code-block:: none

    # field_name,  efuse_block(EFUSE_BLK0..{IDF_TARGET_MAX_EFUSE_BLK}), bit_start(0..255),    bit_count(1..256),        comment

Individual params in CSV file the following meanings:

field_name

    Name of field. The prefix `ESP_EFUSE_` is added to the name, and this field name is available in the code. This name is used to access the fields. The name must be unique for all fields. If the line has an empty name, then this line is combined with the previous field. This allows you to set an arbitrary order of bits in the field, and expand the field as well (see ``MAC_FACTORY`` field in the common table). The field_name supports structured format using `.` to show that the field belongs to another field (see ``WR_DIS`` and ``RD_DIS`` in the common table).

efuse_block

    Block number. It determines where the eFuse bits are placed for this field. Available EFUSE_BLK0..{IDF_TARGET_MAX_EFUSE_BLK}.

bit_start

    Start bit number (0..255). The bit_start field can be omitted. In this case, it is set to bit_start + bit_count from the previous record, if it has the same efuse_block. Otherwise (if efuse_block is different, or this is the first entry), an error will be generated.

.. only:: esp32

    bit_count

        The number of bits to use in this field (1..-). This parameter cannot be omitted. This field also may be ``MAX_BLK_LEN`` in this case, the field length has the maximum block length, taking into account the coding scheme (applicable for ``ESP_EFUSE_SECURE_BOOT_KEY`` and ``ESP_EFUSE_ENCRYPT_FLASH_KEY`` fields). The value ``MAX_BLK_LEN`` depends on :ref:`CONFIG_EFUSE_CODE_SCHEME_SELECTOR`, which will be replaced with "None" - 256, "3/4" - 192, "REPEAT" - 128.

.. only:: not esp32

    bit_count

        The number of bits to use in this field (1..-). This parameter cannot be omitted. This field also may be ``MAX_BLK_LEN`` in this case, the field length has the maximum block length.

comment

    This param is using for comment field, it also move to C-header file. The comment field can be omitted.

If a non-sequential bit order is required to describe a field, then the field description in the following lines should be continued without specifying a name, indicating that it belongs to one field. For example two fields ``MAC_FACTORY`` and ``MAC_FACTORY_CRC``:

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

This field is available in code as ``ESP_EFUSE_MAC_FACTORY`` and ``ESP_EFUSE_MAC_FACTORY_CRC``.

.. _structured-efuse-fields:

Structured eFuse Fields
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

The structured eFuse field looks like ``WR_DIS.RD_DIS`` where the dot points that this field belongs to the parent field - ``WR_DIS`` and cannot be out of the parent's range.

It is possible to use some levels of structured fields as WR_DIS.FIELD_2.B1 and B2. These fields should not be crossed each other and should be in the range of two fields: ``WR_DIS`` and ``WR_DIS.FIELD_2``.

It is possible to create aliases for fields with the same range, see ``WR_DIS.FIELD_3`` and ``WR_DIS.FIELD_3.ALIAS``.

The ESP-IDF names for structured eFuse fields should be unique. The ``efuse_table_gen`` tool generates the final names where the dot is replaced by ``_``. The names for using in ESP-IDF are ESP_EFUSE_WR_DIS, ESP_EFUSE_WR_DIS_RD_DIS, ESP_EFUSE_WR_DIS_FIELD_2_B1, etc.

The ``efuse_table_gen`` tool checks that the fields do not overlap each other and must be within the range of a field if there is a violation, then throws the following error:

.. code-block:: none

    Field at USER_DATA, EFUSE_BLK3, 0, 256  intersected with  SERIAL_NUMBER, EFUSE_BLK3, 0, 32

Solution: Describe ``SERIAL_NUMBER`` to be included in ``USER_DATA``. (``USER_DATA.SERIAL_NUMBER``).

.. code-block:: none

    Field at FIELD, EFUSE_BLK3, 0, 50  out of range  FIELD.MAJOR_NUMBER, EFUSE_BLK3, 60, 32

Solution: Change ``bit_start`` for ``FIELD.MAJOR_NUMBER`` from 60 to 0, so ``MAJOR_NUMBER`` is in the ``FIELD`` range.

``efuse_table_gen.py`` Tool
---------------------------

The tool is designed to generate C-source files from CSV file and validate fields. First of all, the check is carried out on the uniqueness of the names and overlaps of the field bits. If an additional `custom` file is used, it will be checked with the existing `common` file (esp_efuse_table.csv). In case of errors, a message will be displayed and the string that caused the error. C-source files contain structures of type `esp_efuse_desc_t`.

To generate a `common` files, use the following command ``idf.py efuse-common-table`` or:

.. code-block:: bash

    cd $IDF_PATH/components/efuse/
    ./efuse_table_gen.py --idf_target {IDF_TARGET_PATH_NAME} {IDF_TARGET_PATH_NAME}/esp_efuse_table.csv

After generation in the folder $IDF_PATH/components/efuse/`{IDF_TARGET_PATH_NAME}` create:

* `esp_efuse_table.c` file.
* In `include` folder `esp_efuse_table.c` file.

To generate a `custom` files, use the following command ``idf.py efuse-custom-table`` or:

.. code-block:: bash

    cd $IDF_PATH/components/efuse/
    ./efuse_table_gen.py --idf_target {IDF_TARGET_PATH_NAME} {IDF_TARGET_PATH_NAME}/esp_efuse_table.csv PROJECT_PATH/main/esp_efuse_custom_table.csv

After generation in the folder PROJECT_PATH/main create:

* `esp_efuse_custom_table.c` file.
* In `include` folder `esp_efuse_custom_table.c` file.

To use the generated fields, you need to include two files:

.. code-block:: c

    #include "esp_efuse.h"
    #include "esp_efuse_table.h" // or "esp_efuse_custom_table.h"


Supported Coding Scheme
-----------------------

.. only:: esp32

    eFuse have three coding schemes:

    * ``None`` (value 0).
    * ``3/4`` (value 1).
    * ``Repeat`` (value 2).

    The coding scheme affects only EFUSE_BLK1, EFUSE_BLK2 and EFUSE_BLK3 blocks. EUSE_BLK0 block always has a coding scheme ``None``.
    Coding changes the number of bits that can be written into a block, the block length is constant 256, some of these bits are used for encoding and not available for the user.

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

    In case ``3/4`` coding scheme, the writing process is divided into the coding units and we cannot use the usual mode of writing some fields. We can prepare all the data for writing and burn it in one time. You can also use this mode for ``None`` coding scheme but it is not necessary. It is important for ``3/4`` coding scheme.
    ``The batch writing mode`` blocks ``esp_efuse_read_...`` operations.

    After changing the coding scheme, run ``efuse_common_table`` and ``efuse_custom_table`` commands to check the tables of the new coding scheme.

.. only:: not esp32

    Coding schemes are used to protect against data corruption. {IDF_TARGET_NAME} supports two coding schemes:

    * ``None``. EFUSE_BLK0 is stored with four backups, meaning each bit is stored four times. This backup scheme is automatically applied by the hardware and is not visible to software. EFUSE_BLK0 can be written many times.
    * ``RS``. EFUSE_BLK1 - {IDF_TARGET_MAX_EFUSE_BLK} use Reed-Solomon coding scheme that supports up to 5 bytes of automatic error correction. Software encodes the 32-byte EFUSE_BLKx using RS (44, 32) to generate a 12-byte check code, and then burn the EFUSE_BLKx and the check code into eFuse at the same time. The eFuse Controller automatically decodes the RS encoding and applies error correction when reading back the eFuse block. Because the RS check codes are generated across the entire 256-bit eFuse block, each block can only be written to one time.

To write some fields into one block, or different blocks in one time, you need to use ``the batch writing mode``. Firstly set this mode through :cpp:func:`esp_efuse_batch_write_begin` function then write some fields as usual using the ``esp_efuse_write_...`` functions. At the end to burn them, call the :cpp:func:`esp_efuse_batch_write_commit` function. It burns prepared data to the eFuse blocks and disables the ``batch recording mode``.

.. note::

    If there is already pre-written data in the eFuse block using the ``{IDF_TARGET_CODING_SCHEMES}`` encoding scheme, then it is not possible to write anything extra (even if the required bits are empty) without breaking the previous encoding data. This encoding data will be overwritten with new encoding data and completely destroyed (however, the payload eFuses are not damaged). It can be related to: CUSTOM_MAC, SPI_PAD_CONFIG_HD, SPI_PAD_CONFIG_CS, etc. Please contact Espressif to order the required pre-burnt eFuses.

    FOR TESTING ONLY (NOT RECOMMENDED): You can ignore or suppress errors that violate encoding scheme data in order to burn the necessary bits in the eFuse block.

.. _efuse_API:

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
* :cpp:func:`esp_efuse_destroy_block` - Destroys the data in this eFuse block. There are two things to do (1) if write protection is not set, then the remaining unset bits are burned, (2) set read protection for this block if it is not locked.

For frequently used fields, special functions are made, like this :cpp:func:`esp_efuse_get_pkg_ver`.

.. only:: SOC_EFUSE_KEY_PURPOSE_FIELD or SOC_SUPPORT_SECURE_BOOT_REVOKE_KEY

    eFuse API for Keys
    ------------------

    .. only:: SOC_EFUSE_KEY_PURPOSE_FIELD

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

    .. only:: SOC_SUPPORT_SECURE_BOOT_REVOKE_KEY

        * :cpp:func:`esp_efuse_get_digest_revoke` - Returns the status of the Secure Boot public key digest revocation bit.
        * :cpp:func:`esp_efuse_set_digest_revoke` - Sets the Secure Boot public key digest revocation bit.
        * :cpp:func:`esp_efuse_get_write_protect_of_digest_revoke` - Returns a write protection of the Secure Boot public key digest revocation bit.
        * :cpp:func:`esp_efuse_set_write_protect_of_digest_revoke` - Sets a write protection of the Secure Boot public key digest revocation bit.


How to Add a New Field
----------------------

1. Find a free bits for field. Show `esp_efuse_table.csv` file or run ``idf.py show-efuse-table`` or the next command:

.. include:: inc/show-efuse-table_{IDF_TARGET_NAME}.rst

The number of bits not included in square brackets is free (some bits are reserved for Espressif). All fields are checked for overlapping.

To add fields to an existing field, use the :ref:`Structured efuse fields <structured-efuse-fields>` technique. For example, adding the fields: SERIAL_NUMBER, MODEL_NUMBER and HARDWARE REV to an existing ``USER_DATA`` field. Use ``.`` (dot) to show an attachment in a field.

.. code-block:: none

    USER_DATA.SERIAL_NUMBER,                  EFUSE_BLK3,    0,  32,
    USER_DATA.MODEL_NUMBER,                   EFUSE_BLK3,    32, 10,
    USER_DATA.HARDWARE_REV,                   EFUSE_BLK3,    42, 10,

2. Fill a line for field: field_name, efuse_block, bit_start, bit_count, comment.

3. Run a ``show_efuse_table`` command to check eFuse table. To generate source files run ``efuse_common_table`` or ``efuse_custom_table`` command.

You may get errors such as ``intersects with`` or ``out of range``. Please see how to solve them in the :ref:`Structured efuse fields <structured-efuse-fields>` article.

Bit Order
---------

The eFuses bit order is little endian (see the example below), it means that eFuse bits are read and written from LSB to MSB:

.. code-block:: none

    $ espefuse.py dump

    USER_DATA      (BLOCK3          ) [3 ] read_regs: 03020100 07060504 0B0A0908 0F0E0D0C 13121111 17161514 1B1A1918 1F1E1D1C
    BLOCK4         (BLOCK4          ) [4 ] read_regs: 03020100 07060504 0B0A0908 0F0E0D0C 13121111 17161514 1B1A1918 1F1E1D1C

    where is the register representation:

    EFUSE_RD_USR_DATA0_REG = 0x03020100
    EFUSE_RD_USR_DATA1_REG = 0x07060504
    EFUSE_RD_USR_DATA2_REG = 0x0B0A0908
    EFUSE_RD_USR_DATA3_REG = 0x0F0E0D0C
    EFUSE_RD_USR_DATA4_REG = 0x13121111
    EFUSE_RD_USR_DATA5_REG = 0x17161514
    EFUSE_RD_USR_DATA6_REG = 0x1B1A1918
    EFUSE_RD_USR_DATA7_REG = 0x1F1E1D1C

    where is the byte representation:

    byte[0] = 0x00, byte[1] = 0x01, ... byte[3] = 0x03, byte[4] = 0x04, ..., byte[31] = 0x1F

For example, csv file describes the ``USER_DATA`` field, which occupies all 256 bits (a whole block).

.. code-block:: none

    USER_DATA,          EFUSE_BLK3,    0,  256,     User data
    USER_DATA.FIELD1,   EFUSE_BLK3,    16,  16,     Field1

    ID,                 EFUSE_BLK4,    8,  3,      ID bit[0..2]
    ,                   EFUSE_BLK4,    16, 2,      ID bit[3..4]
    ,                   EFUSE_BLK4,    32, 3,      ID bit[5..7]

Thus, reading the eFuse ``USER_DATA`` block written as above gives the following results:

.. code-block:: c

    uint8_t buf[32] = { 0 };
    esp_efuse_read_field_blob(ESP_EFUSE_USER_DATA, &buf, sizeof(buf) * 8);
    // buf[0] = 0x00, buf[1] = 0x01, ... buf[31] = 0x1F

    uint32_t field1 = 0;
    size_t field1_size = ESP_EFUSE_USER_DATA[0]->bit_count; // can be used for this case because it only consists of one entry
    esp_efuse_read_field_blob(ESP_EFUSE_USER_DATA, &field1, field1_size);
    // field1 = 0x0302

    uint32_t field1_1 = 0;
    esp_efuse_read_field_blob(ESP_EFUSE_USER_DATA, &field1_1, 2); // reads only first 2 bits
    // field1 = 0x0002

    uint8_t id = 0;
    size_t id_size = esp_efuse_get_field_size(ESP_EFUSE_ID); // returns 6
    // size_t id_size = ESP_EFUSE_USER_DATA[0]->bit_count; // cannot be used because it consists of 3 entries. It returns 3 not 6.
    esp_efuse_read_field_blob(ESP_EFUSE_ID, &id, id_size);
    // id = 0x91
    // b'100 10  001
    //   [3] [2] [3]

    uint8_t id_1 = 0;
    esp_efuse_read_field_blob(ESP_EFUSE_ID, &id_1, 3);
    // id = 0x01
    // b'001

Get eFuses During Build
-----------------------

There is a way to get the state of eFuses at the build stage of the project. There are two cmake functions for this:

* ``espefuse_get_json_summary()`` - It calls the ``espefuse.py summary --format json`` command and returns a json string (it is not stored in a file).
* ``espefuse_get_efuse()`` - It finds a given eFuse name in the json string and returns its property.

The json string has the following properties:

.. code-block:: json

    {
        "MAC": {
            "bit_len": 48,
            "block": 0,
            "category": "identity",
            "description": "Factory MAC Address",
            "efuse_type": "bytes:6",
            "name": "MAC",
            "pos": 0,
            "readable": true,
            "value": "94:b9:7e:5a:6e:58 (CRC 0xe2 OK)",
            "word": 1,
            "writeable": true
        },
    }

These functions can be used from a top-level project ``CMakeLists.txt`` (:example_file:`get-started/hello_world/CMakeLists.txt`):

.. code-block:: cmake

    # ...
    project(hello_world)

    espefuse_get_json_summary(efuse_json)
    espefuse_get_efuse(ret_data ${efuse_json} "MAC" "value")
    message("MAC:" ${ret_data})

The format of the ``value`` property is the same as shown in ``espefuse.py summary``.

.. code-block:: none

    MAC:94:b9:7e:5a:6e:58 (CRC 0xe2 OK)

There is an example test :example_file:`system/efuse/CMakeLists.txt` which adds a custom target ``efuse-summary``. This allows you to run the ``idf.py efuse-summary`` command to read the required eFuses (specified in the ``efuse_names`` list) at any time, not just at project build time.

Debug eFuse & Unit Tests
------------------------

.. _virtual-efuses:

Virtual eFuses
^^^^^^^^^^^^^^

The Kconfig option :ref:`CONFIG_EFUSE_VIRTUAL` virtualizes eFuse values inside the eFuse Manager, so writes are emulated and no eFuse values are permanently changed. This can be useful for debugging app and unit tests.
During startup, the eFuses are copied to RAM. All eFuse operations (read and write) are performed with RAM instead of the real eFuse registers.

In addition to the :ref:`CONFIG_EFUSE_VIRTUAL` option there is :ref:`CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH` option that adds a feature to keep eFuses in flash memory. To use this mode the partition_table should have the `efuse` partition. partition.csv: ``"efuse_em, data, efuse,   ,   0x2000,"``.
During startup, the eFuses are copied from flash or, in case if flash is empty, from real eFuse to RAM and then update flash. This option allows keeping eFuses after reboots (possible to test secure_boot and flash_encryption features with this option).

Flash Encryption Testing
""""""""""""""""""""""""

Flash Encryption (FE) is a hardware feature that requires the physical burning of eFuses: key and FLASH_CRYPT_CNT. If FE is not actually enabled then enabling the :ref:`CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH` option just gives testing possibilities and does not encrypt anything in the flash, even though the logs say encryption happens. The :cpp:func:`bootloader_flash_write` is adapted for this purpose. But if FE is already enabled on the chip and you run an application or bootloader created with the :ref:`CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH` option then the flash encryption/decryption operations will work properly (data are encrypted as it is written into an encrypted flash partition and decrypted when they are read from an encrypted partition).

``espefuse.py``
^^^^^^^^^^^^^^^

esptool includes a useful tool for reading/writing {IDF_TARGET_NAME} eFuse bits - `espefuse.py <https://docs.espressif.com/projects/esptool/en/latest/{IDF_TARGET_PATH_NAME}/espefuse/index.html>`_.

.. include:: inc/espefuse_summary_{IDF_TARGET_NAME}.rst


.. include-build-file:: inc/esp_efuse_chip.inc
.. include-build-file:: inc/esp_efuse.inc
