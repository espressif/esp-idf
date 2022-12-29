Optional features for flash
===========================

Some features are not supported on all ESP chips and Flash chips. You can check the list below for more information.

-  `Auto Suspend & Resume <#auto-suspend-&-resume>`__

-  `Flash unique ID <#flash-unique-id>`__

-  `High performance mode <#high-performance-mode>`__

-  `OPI flash support <#opi-flash-support>`__

-  `32-bit Address Flash Chips <#32-bit-address-flash-chips>`__

.. note::

    - The features listed above needs to be supported by both esp chips and flash chips.

    - If you are using an official Espressif modules/SiP. Some of the modules/SiPs always support the feature, in this case you can see these features listed in the datasheet. Otherwise please contact `Espressif's business team <https://www.espressif.com/en/contact-us/sales-questions>`_ to know if we can supply such products for you.

    - If you are making your own modules with your own bought flash chips, and you need features listed above. Please contact your vendor if they support the those features, and make sure that the chips can be supplied continuously.

.. attention::

    This document only shows that IDF code has supported the features of those flash chips. It's not a list of stable flash chips certified by Espressif. If you build your own hardware from flash chips with your own brought flash chips (even with flash listed in this page), you need to validate the reliability of flash chips yourself.

Auto Suspend & Resume
---------------------

.. only:: esp32c3

    You can refer to :ref:`auto-suspend` for more information about this feature. The support list is as follows.

ESP Chips List:

1. ESP32C3

Flash Chips List:

1. XM25QxxC series.

Flash unique ID
---------------

Unique ID is not flash id, which means flash has 64-Bit unique ID for each device. The instruction to read the unique ID (4Bh) accesses a factory-set read-only 64-bit number that is unique to each flash device. This ID number helps you to recognize each single device. Not all flash vendors support this feature. If you try to read the unique ID on a chip which does not have this feature, the behavior is not determined. The support list is as follows.

ESP Chips Lists:

ALL

Flash Chips List:

1. ISSI
2. GD
3. TH
4. FM
5. Winbond
6. XMC
7. BOYA

High performance mode
---------------------

.. note::

    This section is provided for Dual mode (DOUT/DIO) and Quad mode (QIO/QOUT) flash chips. Octal flash used on ESP-chips support High performance mode by default so far, you can refer to the octal flash support list below.

High performance mode (HPM) means that the SPI1 and flash chip works under high frequency. Usually, when the operating frequency of the flash is greater than 80MHz, it is considered that the flash works under HPM. As far as we acknowledged, flash chips have more than two different coping strategies when flash work under HPM. For some flash chips, HPM is controlled by high performance flag (HPF) in status register and for some flash chips, HPM is controlled by dummy cycle bit.

For following conditions， IDF start code deals with HPM internally.

ESP Chips List:

1. ESP32S3

Flash Chips (name & ID) List:

1. GD25Q64C (ID: 0xC84017)
2. GD25Q32C (ID: 0xC84016)

.. attention::

    It is hard to create several strategies to cover all situations, so all flash chips using HPM need to be supported explicitly. Therefore, if you try to use a flash not listed as supported under high performance mode, it might cause some error. So, when you try to use the flash chip beyond supported list, please test properly.

OPI flash support
-----------------

OPI flash means that the flash chip supports octal peripheral interface, which has octal I/O pins. Different octal flash has different configurations and different commands. Hence, it is necessary to carefully check the support list.

.. only:: esp32s3

    .. note::

       To know how to configure menuconfig for a board with different Flash and PSRAM, please refer to the :ref:`SPI Flash and External SPI RAM Configuration <flash-psram-configuration>`

ESP Chips List:

1. ESP32S3

Flash Chips List:

1. MX25UM25645G

32-bit Address Flash Chips
--------------------------

Most NOR flash chips used by Espressif chips use 24-bits address, which can cover 16 MBytes memory. However, for larger memory (usually equal to or larger than 16 MBytes), flash uses a 32-bits address to address larger memory. Regretfully, 32-bits address chips have vendor-specific commands, so we need to support the chips one by one.

ESP Chips List:

ALL ESP Chips support this.

Flash Chips List:

1. W25Q256
2. GD25Q256
