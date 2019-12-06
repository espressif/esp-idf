eFuse Manager
=============


Introduction
------------

The eFuse Manager library is designed to structure access to eFuse bits and make using these easy. This library operates eFuse bits by a structure name wich assigned in eFuse table. This sections introduces some concepts used by eFuse Manager.


Hardware description
--------------------

The ESP32 has a number of eFuses which can store system and user parameters. Each eFuse is a one-bit field which can be programmed to 1 after which it cannot be reverted back to 0. 
Some of system parameters are using these eFuse bits directly by hardware modules and have special place (for example EFUSE_BLK0). For more details see `ESP32 Technical Reference Manual <https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf>`_ in part 20 eFuse controller. Some eFuse bits are available for user applications. 

ESP32 has 4 eFuse blocks each of the size of 256 bits (not all bits are available):

* EFUSE_BLK0 is used entirely for system purposes;
* EFUSE_BLK1 is used for flash encrypt key. If not using that Flash Encryption feature, they can be used for another purpose;
* EFUSE_BLK2 is used for security boot key. If not using that Secure Boot feature, they can be used for another purpose;
* EFUSE_BLK3 can be partially reserved for the custom MAC address, or used entirely for user application. Note that some bits are already used in IDF.

Each block is divided into 8 32-bits registers.


eFuse Manager component
-----------------------

The component has API functions for reading and writing fields. Access to the fields is carried out through the structures that describe the location of the eFuse bits in the blocks. The component provides the ability to form fields of any length and from any number of individual bits. The description of the fields is made in a CSV file in a table form. To generate from a tabular form (CSV file) in the C-source uses the tool `efuse_table_gen.py`. The tool checks the CSV file for uniqueness of field names and bit intersection, in case of using a `custom` file from the user's project directory, the utility will check with the `common` CSV file.

CSV files:

* common (`esp_efuse_table.csv`) - contains eFuse fields which are used inside the IDF. C-source generation should be done manually when changing this file (run command ``idf.py efuse_common_table``). Note that changes in this file can lead to incorrect operation.
* custom - (optional and can be enabled by :envvar:`CONFIG_EFUSE_CUSTOM_TABLE`) contains eFuse fields that are used by the user in their application. C-source generation should be done manually when changing this file and running ``idf.py efuse_custom_table``.


Description CSV file
--------------------

The CSV file contains a description of the eFuse fields. In the simple case, one field has one line of description.
Table header:

::

	# field_name,  efuse_block(EFUSE_BLK0..EFUSE_BLK3), bit_start(0..255),	bit_count(1..256),		comment


Individual params in CSV file the following meanings:

field_name
    Name of field. The prefix `ESP_EFUSE_` will be added to the name, and this field name will be available in the code. This name will be used to access the fields. The name must be unique for all fields. If the line has an empty name, then this line is combined with the previous field. This allows you to set an arbitrary order of bits in the field, and expand the field as well (see ``MAC_FACTORY`` field in the common table).

efuse_block
    Block number. It determines where the eFuse bits will be placed for this field. Available EFUSE_BLK0..EFUSE_BLK3.

bit_start
    Start bit number (0..255). The bit_start field can be omitted. In this case, it will be set to bit_start + bit_count from the previous record, if it has the same efuse_block. Otherwise (if efuse_block is different, or this is the first entry), an error will be generated.

bit_count
    The number of bits to use in this field (1..-). This parameter can not be omitted. This field also may be ``MAX_BLK_LEN`` in this case, the field length will have the maximum block length, taking into account the coding scheme (applicable for ``ESP_EFUSE_SECURE_BOOT_KEY`` and ``ESP_EFUSE_ENCRYPT_FLASH_KEY`` fields). The value ``MAX_BLK_LEN`` depends on :envvar:`CONFIG_EFUSE_MAX_BLK_LEN`, will be replaced with "None" - 256, "3/4" - 192, "REPEAT" - 128.

comment
    This param is using for comment field, it also move to C-header file. The comment field can be omitted.
    
If a non-sequential bit order is required to describe a field, then the field description in the following lines should be continued without specifying a name, this will indicate that it belongs to one field. For example two fields MAC_FACTORY and MAC_FACTORY_CRC:

::

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

efuse_table_gen.py tool
-----------------------

The tool is designed to generate C-source files from CSV file and validate fields. First of all, the check is carried out on the uniqueness of the names and overlaps of the field bits. If an additional `custom` file is used, it will be checked with the existing `common` file (esp_efuse_table.csv). In case of errors, a message will be displayed and the string that caused the error. C-source files contain structures of type `esp_efuse_desc_t`.

To generate a `common` files, use the following command ``idf.py efuse_common_table`` or:

::
	
	cd $IDF_PATH/components/efuse/
	./efuse_table_gen.py esp32/esp_efuse_table.csv

After generation in the folder `esp32` create:

* `esp_efuse_table.c` file.
* In `include` folder `esp_efuse_table.c` file.

To generate a `custom` files, use the following command ``idf.py efuse_custom_table`` or:

::

	cd $IDF_PATH/components/efuse/
	./efuse_table_gen.py esp32/esp_efuse_table.csv PROJECT_PATH/main/esp_efuse_custom_table.csv

After generation in the folder PROJECT_PATH/main create:

* `esp_efuse_custom_table.c` file.
* In `include` folder `esp_efuse_custom_table.c` file.

To use the generated fields, you need to include two files:

::

	#include "esp_efuse.h"
	#include "esp_efuse_table.h" or "esp_efuse_custom_table.h"

Support coding scheme
---------------------

eFuse have three coding schemes:

* ``None`` (value 0).
* ``3/4`` (value 1).
* ``Repeat`` (value 2).

The coding scheme affects only EFUSE_BLK1, EFUSE_BLK2 and EFUSE_BLK3 blocks. EUSE_BLK0 block always has a coding scheme ``None``. 
Coding changes the number of bits that can be written into a block, the block length is constant 256, some of these bits are used for encoding and are not used.

When using a coding scheme, the length of the payload that can be written is limited (for more details ``20.3.1.3 System Parameter coding_scheme``):

* None 256 bits.
* 3/4 192 bits.
* Repeat 128 bits.

You can find out the coding scheme of your chip:

* run a ``espefuse.py -p COM4 summary`` command.
* from ``esptool`` utility logs (during flashing). 
* calling the function in the code :cpp:func:`esp_efuse_get_coding_scheme` for the EFUSE_BLK3 block.

eFuse tables must always comply with the coding scheme in the chip. There is an :envvar:`EFUSE_CODE_SCHEME_SELECTOR` option to select the coding type for tables in a Kconfig. When generating source files, if your tables do not follow the coding scheme, an error message will be displayed. Adjust the length or offset fields.
If your program was compiled with ``None`` encoding and ``3/4`` is used in the chip, then the ``ESP_ERR_CODING`` error may occur when calling the eFuse API (the field is outside the block boundaries). If the field matches the new block boundaries, then the API will work without errors.

Also, 3/4 coding scheme imposes restrictions on writing bits belonging to one coding unit. The whole block with a length of 256 bits is divided into 4 coding units, and in each coding unit there are 6 bytes of useful data and 2 service bytes. These 2 service bytes contain the checksum of the previous 6 data bytes.

It turns out that only one field can be written into one coding unit. Repeated rewriting in one coding unit is prohibited. But if the record was made in advance or through a :cpp:func:`esp_efuse_write_block` function, then reading the fields belonging to one coding unit is possible.

In case ``3/4`` coding scheme, the writing process is divided into the coding units and we can not use the usual mode of writing some fields. We can prepare all the data for writing and burn it in one time. You can also use this mode for ``None`` coding scheme but it is not necessary. It is important for ``3/4`` coding scheme.
To write some fields in one time need to use ``the batch writing mode``. Firstly set this mode through :cpp:func:`esp_efuse_batch_write_begin` function then write some fields as usual use the ``esp_efuse_write_...`` functions. At the end to burn they, need to call the :cpp:func:`esp_efuse_batch_write_commit` function. It burns prepared data to the efuse blocks and disable the ``batch recording mode``.
``The batch writing mode`` blocks ``esp_efuse_read_...`` operations.

After changing the coding scheme, run ``efuse_common_table`` and ``efuse_custom_table`` commands to check the tables of the new coding scheme.

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

For frequently used fields, special functions are made, like this :cpp:func:`esp_efuse_get_chip_ver`, :cpp:func:`esp_efuse_get_pkg_ver`.


How add a new field
-------------------

1. Find a free bits for field. Show `esp_efuse_table.csv` file or run ``idf.py show_efuse_table`` or the next command:

::

	$ ./efuse_table_gen.py esp32/esp_efuse_table.csv --info
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
	
	Parsing eFuse CSV input file $IDF_PATH/components/efuse/esp32/esp_efuse_table.csv ...
	Verifying eFuse table...


The number of bits not included in square brackets is free (bits in EFUSE_BLK0 are reserved for Espressif). All fields are checked for overlapping.

2. Fill a line for field: field_name, efuse_block, bit_start, bit_count, comment.

3. Run a ``show_efuse_table`` command to check eFuse table. To generate source files run ``efuse_common_table`` or ``efuse_custom_table`` command.

Debug eFuse & Unit tests
------------------------

Virtual eFuses
^^^^^^^^^^^^^^

The Kconfig option :envvar:`CONFIG_EFUSE_VIRTUAL` will virtualize eFuse values inside the eFuse Manager, so writes are emulated and no eFuse values are permanently changed. This can be useful for debugging app and unit tests.

espefuse.py
^^^^^^^^^^^

esptool includes a useful tool for reading/writing ESP32 eFuse bits - `espefuse.py <https://github.com/espressif/esptool/wiki/espefuse>`_.

::

	espefuse.py -p COM4 summary
	
	espefuse.py v2.3.1
	Connecting........_
	Security fuses:
	FLASH_CRYPT_CNT        Flash encryption mode counter                     = 0 R/W (0x0)
	FLASH_CRYPT_CONFIG     Flash encryption config (key tweak bits)          = 0 R/W (0x0)
	CONSOLE_DEBUG_DISABLE  Disable ROM BASIC interpreter fallback            = 1 R/W (0x1)
	ABS_DONE_0             secure boot enabled for bootloader                = 0 R/W (0x0)
	ABS_DONE_1             secure boot abstract 1 locked                     = 0 R/W (0x0)
	JTAG_DISABLE           Disable JTAG                                      = 0 R/W (0x0)
	DISABLE_DL_ENCRYPT     Disable flash encryption in UART bootloader       = 0 R/W (0x0)
	DISABLE_DL_DECRYPT     Disable flash decryption in UART bootloader       = 0 R/W (0x0)
	DISABLE_DL_CACHE       Disable flash cache in UART bootloader            = 0 R/W (0x0)
	BLK1                   Flash encryption key
	  = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W
	BLK2                   Secure boot key
	  = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W
	BLK3                   Variable Block 3
	  = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 fa 87 02 91 00 00 00 00 00 00 00 00 00 00 00 00 R/W
	
	Efuse fuses:
	WR_DIS                 Efuse write disable mask                          = 0 R/W (0x0)
	RD_DIS                 Efuse read disablemask                            = 0 R/W (0x0)
	CODING_SCHEME          Efuse variable block length scheme                = 1 R/W (0x1) (3/4)
	KEY_STATUS             Usage of efuse block 3 (reserved)                 = 0 R/W (0x0)
	
	Config fuses:
	XPD_SDIO_FORCE         Ignore MTDI pin (GPIO12) for VDD_SDIO on reset    = 0 R/W (0x0)
	XPD_SDIO_REG           If XPD_SDIO_FORCE, enable VDD_SDIO reg on reset   = 0 R/W (0x0)
	XPD_SDIO_TIEH          If XPD_SDIO_FORCE & XPD_SDIO_REG, 1=3.3V 0=1.8V   = 0 R/W (0x0)
	SPI_PAD_CONFIG_CLK     Override SD_CLK pad (GPIO6/SPICLK)                = 0 R/W (0x0)
	SPI_PAD_CONFIG_Q       Override SD_DATA_0 pad (GPIO7/SPIQ)               = 0 R/W (0x0)
	SPI_PAD_CONFIG_D       Override SD_DATA_1 pad (GPIO8/SPID)               = 0 R/W (0x0)
	SPI_PAD_CONFIG_HD      Override SD_DATA_2 pad (GPIO9/SPIHD)              = 0 R/W (0x0)
	SPI_PAD_CONFIG_CS0     Override SD_CMD pad (GPIO11/SPICS0)               = 0 R/W (0x0)
	DISABLE_SDIO_HOST      Disable SDIO host                                 = 0 R/W (0x0)
	
	Identity fuses:
	MAC                    MAC Address
	  = 84:0d:8e:18:8e:44 (CRC ad OK) R/W
	CHIP_VER_REV1          Silicon Revision 1                                = 1 R/W (0x1)
	CHIP_VERSION           Reserved for future chip versions                 = 2 R/W (0x2)
	CHIP_PACKAGE           Chip package identifier                           = 0 R/W (0x0)
	
	Calibration fuses:
	BLK3_PART_RESERVE      BLOCK3 partially served for ADC calibration data  = 1 R/W (0x1)
	ADC_VREF               Voltage reference calibration                     = 1114 R/W (0x2)
	ADC1_TP_LOW            ADC1 150mV reading                                = 346 R/W (0x11)
	ADC1_TP_HIGH           ADC1 850mV reading                                = 3285 R/W (0x5)
	ADC2_TP_LOW            ADC2 150mV reading                                = 449 R/W (0x7)
	ADC2_TP_HIGH           ADC2 850mV reading                                = 3362 R/W (0x1f5)
	
	Flash voltage (VDD_SDIO) determined by GPIO12 on reset (High for 1.8V, Low/NC for 3.3V).

To get a dump for all eFuse registers.

::
	
	espefuse.py -p COM4 dump

	$ espefuse.py -p COM4 dump
	espefuse.py v2.3.1
	Connecting........__
	EFUSE block 0:
	00000000 c403bb68 0082240a 00000000 00000035 00000000 00000000
	EFUSE block 1:
	00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
	EFUSE block 2:
	00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
	EFUSE block 3:
	00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000


.. include:: /_build/inc/esp_efuse.inc
