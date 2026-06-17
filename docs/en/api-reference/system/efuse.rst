eFuse Manager
=============

:link_to_translation:`zh_CN:[中文]`

{IDF_TARGET_CODING_SCHEMES:default="Reed-Solomon", esp32="3/4 or Repeat"}


Introduction
------------

eFuse (Electronic Fuses) are microscopic one-time programmable fuses that can be "burned" (i.e., programmed) to store data into the {IDF_TARGET_NAME}. eFuse bits are organized into different data fields, and these data fields could be used for system parameters (i.e., data parameters used by ESP-IDF of {IDF_TARGET_NAME}) or user defined parameters.

The eFuse Manager component is a collection of tools and APIs that assist with defining, burning, accessing eFuses parameters. The notable tools and APIs include:

* A table format used to define eFuse data fields in CSV file.
* ``efuse_table_gen.py`` tool to generate C structure representation of eFuse data fields specified by the CSV file.
* Collection of C API to read/write eFuse data fields.

eFuse Manager vs ``idf.py``
---------------------------

``idf.py`` provides a subset of the functionality of the eFuse Manager via the ``idf.py efuse-<subcommand>`` commands. In this documentation, mostly ``idf.py`` based commands will be used, although you can still see some ``espefuse`` based commands for advanced or rare cases. To see all available commands, run ``idf.py --help`` and search for those prefixed with ``efuse-``.

Hardware Description
--------------------

The {IDF_TARGET_NAME} has a number of eFuses which can store system and user parameters. Each eFuse is a one-bit field which can be programmed to 1 after which it cannot be reverted back to 0. The eFuse bits are grouped into blocks of 256 bits, where each block is further divided into 8 32-bit registers. Some blocks are reserved for system parameters while the remaining blocks can be used for user parameters.

For more details, see *{IDF_TARGET_NAME} Technical Reference Manual* > *eFuse Controller (eFuse)* [`PDF <{IDF_TARGET_TRM_EN_URL}#efuse>`__].

.. only:: esp32

    {IDF_TARGET_NAME} has 4 eFuse blocks each containing 256 bits (not all bits can be used for user parameters):

    * EFUSE_BLK0 is used entirely for system purposes
    * EFUSE_BLK1 is used for Flash Encryption keys. If the Flash Encryption feature is not used, this block can be used for user parameters.
    * EFUSE_BLK2 is used for the Secure Boot key. If the Secure Boot feature is not used, this block can be used for user parameters.
    * EFUSE_BLK3 can be partially reserved to store a custom MAC address, or can be used entirely for user parameters. Note that some bits are already used in ESP-IDF.

.. only:: not esp32 and not esp32c2

    {IDF_TARGET_NAME} has 11 eFuse blocks each containing 256 bits (not all bits can be used for user parameters):

    .. list::

        * EFUSE_BLK0 is used entirely for system parameters
        * EFUSE_BLK1 is used entirely for system parameters
        * EFUSE_BLK2 is used entirely for system parameters
        * EFUSE_BLK3 (also named EFUSE_BLK_USER_DATA) can be used for user parameters
        * EFUSE_BLK4 to EFUSE_BLK8 (also named EFUSE_BLK_KEY0 to EFUSE_BLK_KEY4) can be used to store keys for Secure Boot or Flash Encryption. If both features are unused, these blocks can be used for user parameters.
        :SOC_EFUSE_BLOCK9_KEY_PURPOSE_QUIRK and SOC_ECDSA_SUPPORTED: * EFUSE_BLK9 (also named EFUSE_BLK_KEY5) can be used for any purpose except for Flash Encryption or ECDSA (due to a HW errata);
        :SOC_EFUSE_BLOCK9_KEY_PURPOSE_QUIRK and not SOC_ECDSA_SUPPORTED: * EFUSE_BLK9 (also named EFUSE_BLK_KEY5) can be used for any purpose except for Flash Encryption (due to a HW errata);
        :not SOC_EFUSE_BLOCK9_KEY_PURPOSE_QUIRK: * EFUSE_BLK9 (also named EFUSE_BLK_KEY5) can be used to store keys for Secure Boot or Flash Encryption. If both features are unused, these blocks can be used for user parameters.
        * EFUSE_BLK10 (also named EFUSE_BLK_SYS_DATA_PART2) is reserved for system parameters.

.. only:: esp32c2

    {IDF_TARGET_NAME} has 4 eFuse blocks each containing 256 bits (not all bits can be used for user parameters):

    * EFUSE_BLK0 is used entirely for system parameters
    * EFUSE_BLK1 is used entirely for system parameters
    * EFUSE_BLK2 is used entirely for system parameters
    * EFUSE_BLK3 (also named EFUSE_BLK_KEY0) can be used to store keys for Secure Boot or Flash Encryption. If both features are unused, these blocks can be used for user parameters.

Defining eFuse Fields
---------------------

eFuse fields are defined as a table of records in a CSV file according to a specific format. This record format provides the ability to form eFuse fields of any length and from any number of individual bits.

Moreover, the record format allows structured definition of eFuse fields consisting of sub-fields, meaning that a parent eFuse field may consist of multiple child eFuse fields occupying the same eFuse bits.

Record Format
^^^^^^^^^^^^^

In simple cases, each record occupies a single row in the table. Each record contains the following values (i.e., columns):

{IDF_TARGET_MAX_EFUSE_BLK:default = "EFUSE_BLK10", esp32 = "EFUSE_BLK3", esp32c2 = "EFUSE_BLK3", esp32s31 = "EFUSE_BLK9"}

.. code-block:: none

    # field_name, efuse_block(EFUSE_BLK0..{IDF_TARGET_MAX_EFUSE_BLK}), bit_start(0..255), bit_count(1..256), comment

- ``field_name``

    - Name of the eFuse field.
    - The prefix ``ESP_EFUSE_`` is automatically added to the name, and this name will be used when referring to the field in C code.
    - ``field_name`` unique across all eFuse fields.
    - If this value is left empty, then this record is combined with the previous record. This allows you define an eFuse field with arbitrary bit ordering (see ``MAC_FACTORY`` field in the common table).
    - Using ``.`` will define a child eFuse field. See :ref:`structured-efuse-fields` for more details.

- ``efuse_block``

    - The eFuse field's block number. E.g., EFUSE_BLK0 to {IDF_TARGET_MAX_EFUSE_BLK}.
    - This determines which block the eFuse field is placed.

- ``bit_start``

    - Bit offset (0 to 255) of the eFuse within the block.
    - ``bit_start`` is optional and can be omitted.

        - In this case, it is set to ``bit_start + bit_count`` from the previous record, given that the previous record is in the same eFuse block.
        - If the previous record is in a different eFuse block, an error will be generated.

- ``bit_count``

    - The size of the eFuse field in bits (1 to N).
    - ``bit_count`` cannot be omitted.
    - If set to ``MAX_BLK_LEN`` the eFuse field's size will be the maximum allowable eFuse field size in the block.

        .. only:: esp32

            - ``MAX_BLK_LEN`` takes into account the coding scheme of eFuse.
            - Depending on the coding scheme selected via :ref:`CONFIG_EFUSE_CODE_SCHEME_SELECTOR`, ``MAX_BLK_LEN`` could be 256 ("None"), 192 ("3/4"), or 128 ("REPEAT").

- ``comment``

    - Comment describing the eFuse field.
    - The comment is copied verbatim into the C header file.

If an eFuse field requires non-sequential bit ordering, then the eFuse field will span multiple records (i.e., multiple rows). The first record's ``field_name`` should specify the eFuse field's name, and the following records should leave ``field_name`` blank to indicate that they belong to the same eFuse field.

The following example demonstrates the records to specify the non-sequential eFuse field ``MAC_FACTORY`` followed by a regular eFuse field ``MAC_FACTORY_CRC``:

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

This eFuse fields will be made available in C code as ``ESP_EFUSE_MAC_FACTORY`` and ``ESP_EFUSE_MAC_FACTORY_CRC``.

.. _structured-efuse-fields:

Structured eFuse Fields
-----------------------

Typically, an eFuse field represents a particular parameter. However, in some cases where an eFuse field consists of multiple sub-fields, it may be useful to have isolated access to those sub-fields. For example, if an eFuse field contained a floating point parameter, it may be useful to be access the sign, exponent, and mantissa fields of the floating as separate eFuse fields.

Therefore, it is possible for records to define eFuse fields in a structured manner using the ``.`` operator in ``field_name``. For example, ``XX.YY.ZZ`` defines a eFuse field ``ZZ`` that is a child of eFuse field ``YY`` which in turn is a child field of eFuse field ``XX``.

The following records demonstrate the definition of eFuse fields in a structured manner:

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

Some things to note regarding the example above:

* The ``WR_DIS`` record defines the parent eFuse field. All the other records are child fields of ``WR_DIS`` due to their ``WR_DIS.`` prefix.
* The child fields must utilize the same bits as their parent field. Take note of ``bit_start`` and ``bit_count`` of the child and parent fields:

    * The bits of the child fields are always in the range of their parent field. For example, ``WR_DIS.RD_DIS`` and ``WR_DIS.RD_DIS`` occupy the first and second bit of ``WR_DIS``.
    * Child fields cannot use overlapping bits (except for when aliasing).

* It is possible to create aliases as a child field. For example, ``WR_DIS.FIELD_3.ALIAS`` is a child field and alias of ``WR_DIS.FIELD_3`` as they both occupy the same bits.

All eFuse Fields are eventually converted to C structures via the ``efuse_table_gen.py`` tool. The C structure for each eFuse field will derive their identifier from the ``field_name`` of the eFuse field's record, where all ``.`` are replaced with ``_``. For example, the C symbols for ``WR_DIS.RD_DIS`` and ``WR_DIS.FIELD_2.B1`` will be ``ESP_EFUSE_WR_DIS_RD_DIS`` and ``ESP_EFUSE_WR_DIS_FIELD_2_B1`` respectively.

The ``efuse_table_gen.py`` tool also checks that the fields do not overlap each other and must be within the range of a field. If there is a violation, then the following error is generated:

.. code-block:: none

    Field at USER_DATA, EFUSE_BLK3, 0, 256 intersected with SERIAL_NUMBER, EFUSE_BLK3, 0, 32

In this case, the error can be resolved by making ``SERIAL_NUMBER`` a child field of ``USER_DATA`` via ``USER_DATA.SERIAL_NUMBER``.

.. code-block:: none

    Field at FIELD, EFUSE_BLK3, 0, 50 out of range FIELD.MAJOR_NUMBER, EFUSE_BLK3, 60, 32

In this case, the error can be resolved by changing ``bit_start`` for ``FIELD.MAJOR_NUMBER`` from ``60`` to ``0`` so that ``MAJOR_NUMBER`` overlaps with ``FIELD``.

``efuse_table_gen.py`` Tool
---------------------------

The ``efuse_table_gen.py`` tool is designed to generate C source files containing C structures (of type :cpp:type:`esp_efuse_desc_t`) representing the eFuse fields defined in CSV files. Moreover, the tool also runs some checks on the provided CSV files before generation to ensure that:

- the names of the eFuse fields are unique
- the eFuse fields do not use overlapping bits

As mentioned previously, eFuse fields can be used to hold either system parameters or user parameters. Given that system parameter eFuse fields are inherently required by ESP-IDF and {IDF_TARGET_NAME}, those eFuse fields are defined in a **common** CSV file (``esp_efuse_table.csv``) and distributed as part of ESP-IDF. For user parameter eFuse fields, users should define those fields in a **custom** CSV file (e.g., ``esp_efuse_custom_table.csv``).

To generate C source files using the **common** CSV file, use the ``idf.py efuse-common-table`` or the following:

.. code-block:: bash

    cd $IDF_PATH/components/efuse/
    ./efuse_table_gen.py --idf_target {IDF_TARGET_PATH_NAME} {IDF_TARGET_PATH_NAME}/esp_efuse_table.csv

The following C source/header files will be generated by the tool in ``$IDF_PATH/components/efuse/{IDF_TARGET_PATH_NAME}``:

* ``esp_efuse_table.c`` file containing the C structures of the system parameter eFuse fields
* ``esp_efuse_table.h`` file in the ``include`` folder. This header can be included by the application to use those C structures.

To generate C source files using a **custom** CSV file, use the command ``idf.py efuse-custom-table`` or the following:

.. code-block:: bash

    cd $IDF_PATH/components/efuse/
    ./efuse_table_gen.py --idf_target {IDF_TARGET_PATH_NAME} {IDF_TARGET_PATH_NAME}/esp_efuse_table.csv PROJECT_PATH/main/esp_efuse_custom_table.csv

The following C source/header files will be generated by the tool in ``PROJECT_PATH/main``:

* ``esp_efuse_custom_table.c`` file containing the C structures of the user parameter eFuse fields
* ``esp_efuse_custom_table.h`` file in the ``include`` folder. This header can be included by the application to use those C structures.

To use the generated fields, you need to include two files:

.. code-block:: c

    #include "esp_efuse.h"
    #include "esp_efuse_table.h" // or "esp_efuse_custom_table.h"


Supported Coding Schemes
------------------------

Various coding schemes are supported by eFuses which can protect eFuses against data corruption by detecting and/or correcting for errors.

.. only:: esp32

    {IDF_TARGET_NAME} supports the following eFuse coding schemes:

    * ``None`` (value 0), meaning no coding scheme is applied.
    * ``3/4`` (value 1).
    * ``Repeat`` (value 2). It is not entirely supported by IDF, not recommended for use.

    The coding schemes will encode each eFuse block individually. Furthermore, only EFUSE_BLK1, EFUSE_BLK2 and EFUSE_BLK3 will be encoded, meaning EUSE_BLK0 always uses the ``None`` coding scheme.

    Coding schemes require some bits within an eFuse block to be used as overhead. Thus, by applying a coding scheme, only a subset of the 256 bits within an eFuse block will be usable as eFuse fields.

    * ``None``: 256 usable bits
    * ``3/4``: 192 usable bits
    * ``Repeat``: 128 usable bits

    When using a coding scheme, the length of the payload that can be written is limited. For more details, Please Refer to *{IDF_TARGET_NAME} Technical Reference Manual* > *Chapter 20 eFuse Controller* [`PDF <{IDF_TARGET_TRM_EN_URL}#efuse>`__] > *Section 20.3.1.3 System Parameter coding_scheme*.

    You can find out the coding scheme of your chip:

    * run the ``idf.py efuse-summary`` command.
    * from ``esptool`` utility logs (during flashing).
    * calling the function :cpp:func:`esp_efuse_get_coding_scheme` in the application for the EFUSE_BLK3 block.

    The eFuse fields specified in the CSV files must always comply with the eFuse coding scheme used by the chip. The :ref:`CONFIG_EFUSE_CODE_SCHEME_SELECTOR` option selects which coding scheme is used by the CSV files. When generating source files, if the records in the CSV files do not adhere to the coding scheme, an error message will be displayed. In that case, you must adjust the ``bit_start`` and ``bit_count`` of the records to comply with the limitations of the selected coding scheme.

    .. note::

        After changing the coding scheme, run ``efuse_common_table`` and ``efuse_custom_table`` commands to check the tables of the new coding scheme.

    If your program was compiled with ``None`` encoding but ``3/4`` is used by the chip, then the ``ESP_ERR_CODING`` error may occur when calling the eFuse API (The field is outside the block boundaries). If the field matches the new block boundaries, then the API will work without errors.

    ``None`` Coding Scheme
    ^^^^^^^^^^^^^^^^^^^^^^

    The ``None`` coding scheme indicates that no coding scheme is applied, thus all 256 bits of each eFuse block are usable. However, there will be no protection against the corruption of eFuse bits.

    ``3/4`` Coding Scheme
    ^^^^^^^^^^^^^^^^^^^^^

    The ``3/4`` coding scheme imposes restrictions on writing bits belonging to one coding unit. The whole block with a length of 256 bits is divided into 4 coding units. In each coding unit there are 6 bytes of useful data and 2 service bytes. These 2 service bytes contain the checksum of the previous 6 data bytes.

    Due to the calculation of the checksum for each coding unit, the writing process must be divided into the coding units. As such, the normal method (used by the ``None`` coding scheme) of burning eFuse bits separately over multiple write operations will no longer work. The data for the eFuse fields of a particular coding unit and the unit's associated checksum must be burned in one go. This is known as Batch Writing Mode.

    As a result of Batch Writing Mode, a particular coding unit can only be written once (i.e., repeated writing to the same coding unit is prohibited). Thus, any coding unit that is written at run time can only contain one eFuse field. However, if the eFuse fields of a coding unit was specified in advance (via CSV records) or written to via :cpp:func:`esp_efuse_write_block`, then a coding unit can still contain multiple eFuse fields.

    ``Repeat`` Coding Scheme
    ^^^^^^^^^^^^^^^^^^^^^^^^

    The ``Repeat`` coding scheme simply repeats each eFuse bit, thus does not impose the same Batch Writing Mode restrictions as the ``3/4`` coding scheme. However, this comes at the cost of a larger overhead, leaving only 128 usable bits per eFuse block.

.. only:: not esp32

    {IDF_TARGET_NAME} does not support selection of coding schemes. The following coding schemes are automatically applied to various eFuse blocks:

    * ``None``: Applied to EFUSE_BLK0
    * ``RS``: Applied to EFUSE_BLK1 - {IDF_TARGET_MAX_EFUSE_BLK}

    ``None`` Coding Scheme
    ^^^^^^^^^^^^^^^^^^^^^^

    The ``None`` coding scheme is automatically applied to EFUSE_BLK0. This scheme does not involve any encoding, but simply maintains four backups of EFUSE_BLK0 in hardware, meaning each bit is stored four times. As a result, EFUSE_BLK0 can be written many times.

    This scheme is automatically applied by the hardware and is not visible to software.

    ``RS`` Coding Scheme
    ^^^^^^^^^^^^^^^^^^^^^^

    The ``RS`` coding scheme uses Reed-Solomon encoding and is automatically applied to EFUSE_BLK1 to {IDF_TARGET_MAX_EFUSE_BLK}. The coding scheme supports up to 6 bytes of automatic error correction.

    Software encodes the 32-byte EFUSE_BLKx using ``RS(44, 32)`` to generate a 12-byte check-symbols, and then burn the EFUSE_BLKx and the check-symbols into eFuse at the same time.

    The eFuse Controller automatically decodes the ``RS`` encoding and applies error correction when reading back the eFuse block. Because the ``RS`` check-symbols are generated across the entire 256-bit eFuse block, each block can only be written to one time. As a result of the check-symbols, Batch Writing Mode must be used.

Batch Writing Mode
^^^^^^^^^^^^^^^^^^

When writing to eFuse fields at run time, it may be necessary to use the Batch Writing Mode depending on the coding scheme used for eFuse block. Batch writing mode can be used as follows:

#. Enable batch writing mode by calling :cpp:func:`esp_efuse_batch_write_begin`
#. Write to the eFuse fields as usual using various ``esp_efuse_write_...`` functions.
#. Once all writes are complete, call :cpp:func:`esp_efuse_batch_write_commit` which burns prepared data to the eFuse blocks.

.. warning::

    If there is already pre-written data in the eFuse block using the ``{IDF_TARGET_CODING_SCHEMES}`` encoding scheme, then it is not possible to write anything extra (even if the required bits are empty) without breaking the previous data's checksums/check-symbols.

    The checksums/check-symbols will be overwritten with new checksums/check-symbols and be completely destroyed (however, the payload eFuses are not damaged).

    If you happen to find pre-written data in CUSTOM_MAC, SPI_PAD_CONFIG_HD, SPI_PAD_CONFIG_CS, etc., please contact Espressif to obtain the required pre-burnt eFuses.

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
* :cpp:func:`esp_efuse_read_block` - reads a key from an eFuse block starting at the offset with required size.
* :cpp:func:`esp_efuse_write_block` - writes a key to an eFuse block starting at the offset with required size.
* :cpp:func:`esp_efuse_batch_write_begin` - set the batch mode of writing fields.
* :cpp:func:`esp_efuse_batch_write_commit` - writes all prepared data for batch writing mode and reset the batch writing mode.
* :cpp:func:`esp_efuse_batch_write_cancel` - reset the batch writing mode and prepared data.
* :cpp:func:`esp_efuse_get_key_dis_read` - Returns a read protection for the key block.
* :cpp:func:`esp_efuse_set_key_dis_read` - Sets a read protection for the key block.
* :cpp:func:`esp_efuse_get_key_dis_write` - Returns a write protection for the key block.
* :cpp:func:`esp_efuse_set_key_dis_write` - Sets a write protection for the key block.
* :cpp:func:`esp_efuse_get_key_purpose` - Returns the current purpose set for an eFuse key block.
* :cpp:func:`esp_efuse_write_key` - Programs a block of key data to an eFuse block.
* :cpp:func:`esp_efuse_write_keys` - Programs keys to unused eFuse blocks.
* :cpp:func:`esp_efuse_find_purpose` - Finds a key block with the particular purpose set.
* :cpp:func:`esp_efuse_get_keypurpose_dis_write` - Returns a write protection of the key purpose field for an eFuse key block (for esp32 always true).
* :cpp:func:`esp_efuse_key_block_unused` - Returns true if the key block is unused, false otherwise.
* :cpp:func:`esp_efuse_destroy_block` - Destroys the data in this eFuse block. There are two things to do: (1) if write protection is not set, then the remaining unset bits are burned, (2) set read protection for this block if it is not locked.
* :cpp:func:`esp_efuse_token_dump` - Generates a compact, single-line eFuse token (``EFSR``, ``EFSW``, or ``EFSRW``) that can be copied from device logs and decoded on the host with ``espefuse --token ...`` (useful when direct eFuse access is not available).
* :cpp:func:`esp_efuse_token_burn` - Applies an ``EFSW`` token on the device by burning the staged eFuse writes encoded in the token (other token types are rejected).

For frequently used fields, special functions are made, like this :cpp:func:`esp_efuse_get_pkg_ver`.

.. only:: SOC_EFUSE_KEY_PURPOSE_FIELD or SOC_SUPPORT_SECURE_BOOT_REVOKE_KEY

    eFuse API for Keys
    ------------------

    .. only:: SOC_EFUSE_KEY_PURPOSE_FIELD

        EFUSE_BLK_KEY0 - EFUSE_BLK_KEY5 are intended to keep up to 6 keys with a length of 256-bits. Each key has an ``ESP_EFUSE_KEY_PURPOSE_x`` field which defines the purpose of these keys. The purpose field is described in :cpp:type:`esp_efuse_purpose_t`.

        The purposes like ``ESP_EFUSE_KEY_PURPOSE_XTS_AES_...`` are used for flash encryption.

        The purposes like ``ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST...`` are used for secure boot.

        There are some eFuse APIs useful to work with states of keys:

        * :cpp:func:`esp_efuse_get_purpose_field` - Returns a pointer to a key purpose for an eFuse key block.
        * :cpp:func:`esp_efuse_get_key` - Returns a pointer to a key block.
        * :cpp:func:`esp_efuse_set_key_purpose` - Sets a key purpose for an eFuse key block.
        * :cpp:func:`esp_efuse_set_keypurpose_dis_write` - Sets a write protection of the key purpose field for an eFuse key block.
        * :cpp:func:`esp_efuse_find_unused_key_block` - Search for an unused key block and return the first one found.
        * :cpp:func:`esp_efuse_count_unused_key_blocks` - Returns the number of unused eFuse key blocks in the range EFUSE_BLK_KEY0 to EFUSE_BLK_KEY_MAX

    .. only:: SOC_SUPPORT_SECURE_BOOT_REVOKE_KEY

        * :cpp:func:`esp_efuse_get_digest_revoke` - Returns the status of the Secure Boot public key digest revocation bit.
        * :cpp:func:`esp_efuse_set_digest_revoke` - Sets the Secure Boot public key digest revocation bit.
        * :cpp:func:`esp_efuse_get_write_protect_of_digest_revoke` - Returns a write protection of the Secure Boot public key digest revocation bit.
        * :cpp:func:`esp_efuse_set_write_protect_of_digest_revoke` - Sets a write protection of the Secure Boot public key digest revocation bit.


How to Add a New Field
----------------------

1. Find free bits for field. Refer to the ``esp_efuse_table.csv`` file, running ``idf.py show-efuse-table``, or running the following command:

.. include:: inc/show-efuse-table_{IDF_TARGET_NAME}.rst

The number of bits not included in square brackets are free (some bits are reserved by Espressif). All fields are checked for overlapping bits.

To add child fields to an existing field, :ref:`structured-efuse-fields` can be used. The following example demonstrates adding of the the fields ``SERIAL_NUMBER``, ``MODEL_NUMBER`` and ``HARDWARE_REV`` to an existing ``USER_DATA`` field by using the ``.`` operator:

.. code-block:: none

    USER_DATA.SERIAL_NUMBER,                  EFUSE_BLK3,    0,  32,
    USER_DATA.MODEL_NUMBER,                   EFUSE_BLK3,    32, 10,
    USER_DATA.HARDWARE_REV,                   EFUSE_BLK3,    42, 10,

In general, to add new eFuse Fields:

#. Add a record for each eFuse field in CSV file.
#. Run the ``show_efuse_table`` command to check eFuse table.
#.  To generate source files run the ``efuse_common_table`` or ``efuse_custom_table`` commands.

You may get errors such as ``intersects with`` or ``out of range``. Please see how to solve them in the :ref:`structured-efuse-fields` article.

Bit Order
---------

The eFuses bit order is little endian (see the example below), meaning that eFuse bits are read and written from LSB to MSB:

.. code-block:: none

    $ idf.py efuse-dump

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

For example, CSV file describes the ``USER_DATA`` field, which occupies all 256 bits (a whole block).

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
    // size_t id_size = ESP_EFUSE_USER_DATA[0]->bit_count; // cannot be used because it consists of 3 entries. It returns 3 not 6
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

There is a way to get the state of eFuses at the build stage of the project. There are two CMake functions for this:

* ``espefuse_get_json_summary()`` - It calls the ``espefuse summary --format json`` command and returns a JSON string (it is not stored in a file).
* ``espefuse_get_efuse()`` - It finds a given eFuse name in the JSON string and returns its property.

The JSON string has the following properties:

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

These functions can be used from a top-level project ``CMakeLists.txt`` (:example_file:`system/efuse/CMakeLists.txt`):

.. code-block:: cmake

    # ...
    project(hello_world)

    espefuse_get_json_summary(efuse_json)
    espefuse_get_efuse(ret_data ${efuse_json} "MAC" "value")
    message("MAC:" ${ret_data})

The format of the ``value`` property is the same as shown in ``espefuse summary`` or ``idf.py efuse-summary``.

.. code-block:: none

    MAC:94:b9:7e:5a:6e:58 (CRC 0xe2 OK)

There is an example test :example_file:`system/efuse/CMakeLists.txt` which adds a custom target ``efuse-filter``. This allows you to run the ``idf.py efuse-filter`` command to read the required eFuses (specified in the ``efuse_names`` list) at any time, not just during the project build.

Debug eFuse & Unit Tests
------------------------

.. _virtual-efuses:

Virtual eFuses
^^^^^^^^^^^^^^

The Kconfig option :ref:`CONFIG_EFUSE_VIRTUAL` virtualizes eFuse values inside the eFuse Manager, so writes are emulated and no eFuse values are permanently changed. This can be useful for debugging and unit testing.

During startup, the eFuses are copied to RAM. All eFuse operations (read and write) are performed with RAM instead of the real eFuse registers.

In addition to the :ref:`CONFIG_EFUSE_VIRTUAL` option, there is the :ref:`CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH` option that adds a feature to keep eFuses in flash memory. To use this mode, the partition_table should have include an ``efuse`` partition in ``partition.csv``:

.. code-block:: none

    efuse_em, data, efuse,   ,   0x2000,

During startup, the eFuses are copied from flash, or in case where flash is empty, copied from real eFuse to RAM and then write flash. This option allows keeping eFuses after reboots, making it possible to test Secure Boot and Flash Encryption features.

Flash Encryption Testing
""""""""""""""""""""""""

Flash encryption is a hardware feature that requires the physical burning of eFuses ``key`` and ``FLASH_CRYPT_CNT``. If flash encryption is not actually enabled, then enabling the :ref:`CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH` option just provides testing possibilities and does not encrypt anything in the flash, even though the logs indicates that encryption happens.

The :cpp:func:`bootloader_flash_write` is adapted for this purpose. But if flash encryption is already enabled on the chip when the application is run, or if the bootloader is created with the :ref:`CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH` option, then the flash encryption/decryption operations will work properly. This means that data are encrypted as it is written into an encrypted flash partition and decrypted when they are read from an encrypted partition.

``espefuse``
^^^^^^^^^^^^

esptool includes a useful tool for reading/writing {IDF_TARGET_NAME} eFuse bits - `espefuse <https://docs.espressif.com/projects/esptool/en/latest/{IDF_TARGET_PATH_NAME}/espefuse/index.html>`_.

Part of the functionality of this tool is also provided directly by ``idf.py`` commands. For example, the ``idf.py efuse-summary`` command is equivalent to ``espefuse summary``.

.. include:: inc/espefuse_summary_{IDF_TARGET_NAME}.rst

To get a dump for all eFuse registers.

.. include:: inc/espefuse_summary_{IDF_TARGET_NAME}_dump.rst

Deferred WR_DIS Burning
-----------------------

``WR_DIS`` (Write Disable) is a special eFuse field that implements permanent write-protection. Each bit in ``WR_DIS`` disables further programming of one (or more) associated eFuse fields. Once a ``WR_DIS`` bit is burned, its associated fields can no longer be modified.

When burning staged data in BLOCK0, the ``WR_DIS`` bits are burned separately after all other BLOCK0 data to ensure the burn function can recover from coding errors via its retry mechanism. This approach guarantees that write-protection is applied only after other BLOCK0 data is successfully burned.

Token Dump
----------

The *token dump* feature provides a compact, single-line representation of an eFuse state that can be copied from device logs and decoded later on the host. This is designed for cases where reading eFuses directly with host tools is not possible or not convenient (for example, UART download is disabled, secure download is enabled, secure boot/flash encryption is deployed, or the device is remote).

.. code-block:: none

    EFSR:esp32c3:004:AAGAAAEAAAAAAAAEAAAAAAAAAAAAAAAA:AAAAAAAAAAAAAAAAAAAQAAAAAAAAAAAA:AgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA::::::::::epNVBg

A token can represent:

* the currently programmed eFuses (read snapshot, `EFSR`),
* the staged (not yet burned) write set (write snapshot, `EFSW`) — in batch write mode you can dump the pending writes **before** calling :cpp:func:`esp_efuse_batch_write_commit`,
* both the programmed eFuses and the staged writes in a single token (combined snapshot, `EFSRW`).

Tokens include a CRC32 checksum to detect truncation and accidental modifications.

Typical Use Cases
^^^^^^^^^^^^^^^^^

* **Production/field diagnostics:** export the eFuse state from a locked-down device and decode it offline.
* **Post-provisioning verification:** confirm security configuration and key purposes after manufacturing steps.
* **Coding error investigation:** capture and share coding-error register snapshots alongside block data.
* **Audit and traceability:** store a token as a provisioning artifact for later review.
* **Staged write transfer:** generate ``EFSW`` in firmware (or on the host) and apply it later using a controlled workflow.

Supported Workflows
^^^^^^^^^^^^^^^^^^^

* On the device:

    * :cpp:func:`esp_efuse_token_dump()` — always supports ``EFSR`` tokens; ``EFSW`` and ``EFSRW`` require :ref:`CONFIG_EFUSE_ENABLE_STAGED_TOKEN_API` because they expose staged values that are not yet burned and not yet read-protected.
    * :cpp:func:`esp_efuse_token_burn()` — accepts an ``EFSW`` token and applies staged writes on the device.

* On the host:

  * ``espefuse --token EFS... summary`` — decodes tokens and shows the eFuse summary without connecting to a device.
  * ``espefuse dump --format EFSR`` — generates an ``EFSR`` token from a connected chip for sharing/backup.
  * ``espefuse burn-efuse ... --show-token`` — generates an ``EFSW`` token representing staged writes.

Security Note
^^^^^^^^^^^^^

.. important::

    Tokens are not encrypted. Treat tokens as sensitive data:

    * Tokens can include **unique identifiers** (for example MAC/UUID-like fields) and **security-relevant configuration bits** (secure boot, flash encryption, JTAG/UART disablement, key purposes).
    * ``EFSW`` tokens represent **staged writes**. They may include **write-only key data in plaintext** because the staged view shows values that have not yet been burned and are not yet protected by the eFuse read-protection bits. This can disclose provisioning intent and secret material before it is irreversibly locked down.
    * ``EFSRW`` tokens include the same staged portion and therefore carry the same plaintext key-exposure risk.
    * Even when certain eFuses are read-protected on the target, the token may still carry **operationally sensitive values**.

    If firmware exposes a console command, remote endpoint, or other runtime API that prints eFuse tokens, the firmware must authenticate and authorize that request before generating the token.

Recommendations:

* Share tokens only with trusted parties and via trusted channels (avoid public issue trackers).
* Store tokens as you would store other manufacturing/provisioning artifacts (restricted access, limited retention).
* Prefer ``EFSR`` tokens for diagnostics and auditing; use ``EFSW`` tokens only when you explicitly need to transfer staged write state.

Token format
^^^^^^^^^^^^

A token is a colon-separated sequence:

.. code-block:: none

    <token_name>:<chip>:<ver>:<b64_block0>:...:<b64_blockN>:<b64_cerr>:<b64_crc32>

Fields:

* ``token_name`` — one of ``EFSR``, ``EFSW``, or ``EFSRW``.
* ``chip`` — chip name (lowercase, without dashes), for example ``esp32c3``.
* ``ver`` — chip revision as three decimal digits (leading zeros), for example ``004``. Constructed from major and minor wafer version fields using the formula ``ver = major * 100 + minor``, where the major version occupies the first digit and the minor version occupies the last two digits.
* ``b64_block0 ... b64_blockN`` — Base64URL-encoded per-block data. Each block is a concatenation of 32-bit words in little-endian byte order. The number of blocks is not explicitly encoded in the format. It is derived from the chip type. The number of blocks can be determined by counting the colon separators (``:``) in the token. Empty blocks are represented as consecutive colons (``::``).
* ``b64_cerr`` — optional Base64URL-encoded coding-error registers snapshot. It may be empty if there are no errors.
* ``b64_crc32`` — Base64URL-encoded CRC32 over whole token ``"<token_name>:<chip>:<ver>:<b64_block0>:...:<b64_blockN>:<b64_cerr>:"``. CRC32 is stored little-endian and encoded as unpadded Base64URL.

A token can be decoded and interpreted correctly only when it is processed using the same target it was created for. ESP-IDF APIs and ``espefuse`` validate and rely on the following fields: chip name, chip revision, and block layout, as well as CRC32 integrity. If any of these do not match the target chip, decoding errors or missing fields may occur.

Base64URL uses the same alphabet as Base64 but replaces ``+`` with ``-`` and ``/`` with ``_``, and omits padding (``=``).

Generating Token On-Device
^^^^^^^^^^^^^^^^^^^^^^^^^^

Use :cpp:func:`esp_efuse_token_dump()` to create a token in a buffer or print it to the log:

.. code-block:: c

    char token[1024]; /* size depends on target and eFuses */
    esp_efuse_token_type_t token_type = ESP_EFUSE_TOKEN_FROM_READ;
    ESP_ERROR_CHECK(esp_efuse_token_dump(token_type, token, sizeof(token)));
    ESP_LOGI(TAG, "IDF_MONITOR_EXECUTE_ESPEFUSE_SUMMARY %s", token);

Token type values:

* ``ESP_EFUSE_TOKEN_FROM_READ`` — token of programmed eFuses (starts with ``EFSR``).
* ``ESP_EFUSE_TOKEN_FROM_STAGED`` — token of staged writes (starts with ``EFSW``). It can expose keys in plaintext because it shows values that are not yet burned and not yet read-protected. Requires :ref:`CONFIG_EFUSE_ENABLE_STAGED_TOKEN_API`. Only this type can be burned back.
* ``ESP_EFUSE_TOKEN_FROM_READ_STAGED`` — combined token (starts with ``EFSRW``). It includes the same staged portion and the same plaintext key-exposure risk. Requires :ref:`CONFIG_EFUSE_ENABLE_STAGED_TOKEN_API`.

If ``buf == NULL``, the token is printed to the console (INFO level) without colors, tag, or timestamp.

Burn Token On-Device
^^^^^^^^^^^^^^^^^^^^

Use :cpp:func:`esp_efuse_token_burn()` to apply an ``EFSW`` token on the device by burning the staged eFuse writes encoded in the token. The function validates the token integrity (CRC32) and checks compatibility using the chip name and revision. The token is rejected if the **major** wafer version does not match the target chip. To bypass the version check, use the ignore argument. Example of burning a token:

.. code-block:: c

    esp_efuse_batch_write_begin();
    esp_efuse_token_burn(token, false);  // set true to ignore major version mismatch
    esp_efuse_batch_write_commit();

You can skip the major-version check only when you know the token was generated for the same eFuse layout (for example, the same target with only a minor wafer revision difference); in that case the token version may differ only in the last two digits, while the first digit (major version) must normally match.

ESP-IDF Monitor Integration
^^^^^^^^^^^^^^^^^^^^^^^^^^^

When running ``idf.py monitor``, the host can automatically decode eFuse tokens printed by the target and display the result inline if the log line starts with one of the following markers:

* ``IDF_MONITOR_EXECUTE_ESPEFUSE_SUMMARY`` → ``espefuse --token {ARGS} summary --active``
* ``IDF_MONITOR_EXECUTE_ESPEFUSE_DUMP`` → ``espefuse --token <TOKEN> dump``

``{ARGS}`` must include an eFuse token (``EFSR/EFSW/EFSRW``) and may include ``--extend-efuse-table <csv>`` to load custom eFuse definitions.

Example: The following shows executing the summary command with ``--active`` to display only non-zero eFuse fields, which reduces output size. The ``--extend-efuse-table`` option loads a custom eFuse table definition:

.. code-block:: text

   I (441) example: IDF_MONITOR_EXECUTE_ESPEFUSE_SUMMARY EFSR:esp32c3:100:AAAAAAAAAAAAAAAAAAAAAAAAAIAAAAAA:zIH3-VVgAAAAAAAAAAAAS8kmEVKwQgYB:ZSd8yloMSAJssOWmfZQw8lFbphuTZH574QcV3ggAAAA:AAAAAAAAAAEayAcAAAAAAAAAAAAAAAAAAAAAAAAAAAA:::::::::ydrNkQ --extend-efuse-table main/esp_efuse_custom_table.csv

    --- Executing monitor command: espefuse --token EFSR:esp32c3:100:AAAAAAAAAAAAAAAAAAAAAAAAAIAAAAAA:zIH3-VVgAAAAAAAAAAAAS8kmEVKwQgYB:ZSd8yloMSAJssOWmfZQw8lFbphuTZH574QcV3ggAAAA:AAAAAAAAAAEayAcAAAAAAAAAAAAAAAAAAAAAAAAAAAA:::::::::ydrNkQ --extend-efuse-table main/esp_efuse_custom_table.csv summary --active
    espefuse v5.1.0

    === Run "summary" command ===
    EFUSE_NAME (Block) Description  = [Meaningful Value] [Readable/Writeable] (Hex Value)
    ----------------------------------------------------------------------------------------
    Calibration fuses:
    K_RTC_LDO (BLOCK1)                                 BLOCK1 K_RTC_LDO                                   = 77 R/W (0b1001101)
    K_DIG_LDO (BLOCK1)                                 BLOCK1 K_DIG_LDO                                   = 68 R/W (0b1000100)
    V_RTC_DBIAS20 (BLOCK1)                             BLOCK1 voltage of rtc dbias20                      = 144 R/W (0x90)
    V_DIG_DBIAS20 (BLOCK1)                             BLOCK1 voltage of digital dbias20                  = 130 R/W (0x82)
    DIG_DBIAS_HVT (BLOCK1)                             BLOCK1 digital dbias when hvt                      = 21 R/W (0b10101)
    THRES_HVT (BLOCK1)                                 BLOCK1 pvt threshold when hvt                      = 400 R/W (0b0110010000)
    TEMP_CALIB (BLOCK2)                                Temperature calibration data                       = -10.600000000000001 R/W (0b101101010)
    OCODE (BLOCK2)                                     ADC OCode                                          = 101 R/W (0x65)
    ADC1_INIT_CODE_ATTEN0 (BLOCK2)                     ADC1 init code at atten0                           = 442 R/W (0b0110111010)
    ADC1_INIT_CODE_ATTEN1 (BLOCK2)                     ADC1 init code at atten1                           = 588 R/W (0b1001001100)
    ADC1_INIT_CODE_ATTEN2 (BLOCK2)                     ADC1 init code at atten2                           = 612 R/W (0b1001100100)
    ADC1_INIT_CODE_ATTEN3 (BLOCK2)                     ADC1 init code at atten3                           = 735 R/W (0b1011011111)
    ADC1_CAL_VOL_ATTEN0 (BLOCK2)                       ADC1 calibration voltage at atten0                 = 535 R/W (0b1000010111)
    ADC1_CAL_VOL_ATTEN1 (BLOCK2)                       ADC1 calibration voltage at atten1                 = 31 R/W (0b0000011111)
    ADC1_CAL_VOL_ATTEN2 (BLOCK2)                       ADC1 calibration voltage at atten2                 = 533 R/W (0b1000010101)
    ADC1_CAL_VOL_ATTEN3 (BLOCK2)                       ADC1 calibration voltage at atten3                 = 567 R/W (0b1000110111)

    Config fuses:
    ERR_RST_ENABLE (BLOCK0)                            Use BLOCK0 to check error record registers         = with check R/W (0b1)
    BLOCK_USR_DATA (BLOCK3)                            User data
    = 00 00 00 00 00 00 00 01 1a c8 07 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W

    Flash fuses:
    FLASH_CAP (BLOCK1)                                 Flash capacity                                     = 4M R/W (0b001)
    FLASH_TEMP (BLOCK1)                                Flash temperature                                  = 105C R/W (0b01)
    FLASH_VENDOR (BLOCK1)                              Flash vendor                                       = XMC R/W (0b001)

    Identity fuses:
    BLK_VERSION_MINOR (BLOCK1)                         BLK_VERSION_MINOR                                  = 3 R/W (0b011)
    WAFER_VERSION_MAJOR (BLOCK1)                       WAFER_VERSION_MAJOR                                = 1 R/W (0b01)
    OPTIONAL_UNIQUE_ID (BLOCK2)                        Optional unique 128-bit ID
    = 65 27 7c ca 5a 0c 48 02 6c b0 e5 a6 7d 94 30 f2 R/W
    BLK_VERSION_MAJOR (BLOCK2)                         BLK_VERSION_MAJOR of BLOCK2                        = With calibration R/W (0b01)

    Mac fuses:
    MAC (BLOCK1)                                       MAC address
    = 60:55:f9:f7:81:cc (OK) R/W

    User fuses:
    MODULE_VERSION (BLOCK3)                            Module version (56-63)                             = 1 R/W (0x01)
    DEVICE_ROLE (BLOCK3)                               Device role (64-66)                                = 2 R/W (0b010)
    SETTING_1 (BLOCK3)                                 Setting 1 (67-72)                                  = 3 R/W (0b000011)
    SETTING_2 (BLOCK3)                                 Setting 2 (73-77)                                  = 4 R/W (0b00100)
    CUSTOM_SECURE_VERSION (BLOCK3)                     Custom secure version (78-93)                      = 31 R/W (0x001f)
    ...

Example (dump):

.. code-block:: text

    I (441) example: IDF_MONITOR_EXECUTE_ESPEFUSE_DUMP EFSR:esp32c3:100:AAAAAAAAAAAAAAAAAAAAAAAAAIAAAAAA:zIH3-VVgAAAAAAAAAAAAS8kmEVKwQgYB:ZSd8yloMSAJssOWmfZQw8lFbphuTZH574QcV3ggAAAA:AAAAAAAAAAEayAcAAAAAAAAAAAAAAAAAAAAAAAAAAAA:::::::::ydrNkQ

    --- Executing monitor command: espefuse --token EFSR:esp32c3:100:AAAAAAAAAAAAAAAAAAAAAAAAAIAAAAAA:zIH3-VVgAAAAAAAAAAAAS8kmEVKwQgYB:ZSd8yloMSAJssOWmfZQw8lFbphuTZH574QcV3ggAAAA:AAAAAAAAAAEayAcAAAAAAAAAAAAAAAAAAAAAAAAAAAA:::::::::ydrNkQ dump
    espefuse v5.1.0

    === Run "dump" command ===
    BLOCK0          (                ) [0 ] dump: 00000000 00000000 00000000 00000000 80000000 00000000
    MAC_SPI_8M_0    (BLOCK1          ) [1 ] dump: f9f781cc 00006055 00000000 4b000000 521126c9 010642b0
    BLOCK_SYS_DATA  (BLOCK2          ) [2 ] dump: ca7c2765 02480c5a a6e5b06c f230947d 1ba65b51 7b7e6493 de1507e1 00000008
    BLOCK_USR_DATA  (BLOCK3          ) [3 ] dump: 00000000 01000000 0007c81a 00000000 00000000 00000000 00000000 00000000
    BLOCK_KEY0      (BLOCK4          ) [4 ] dump: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
    BLOCK_KEY1      (BLOCK5          ) [5 ] dump: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
    BLOCK_KEY2      (BLOCK6          ) [6 ] dump: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
    BLOCK_KEY3      (BLOCK7          ) [7 ] dump: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
    BLOCK_KEY4      (BLOCK8          ) [8 ] dump: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
    BLOCK_KEY5      (BLOCK9          ) [9 ] dump: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
    BLOCK_SYS_DATA2 (BLOCK10         ) [10] dump: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000

Application Examples
--------------------

- :example:`system/efuse` demonstrates how to use the eFuse API on {IDF_TARGET_NAME}, showing read and write operations with fields from the common and custom eFuse tables, and explaining the use of virtual eFuses for debugging purposes.

API Reference
-------------

.. include-build-file:: inc/esp_efuse_chip.inc
.. include-build-file:: inc/esp_efuse.inc
