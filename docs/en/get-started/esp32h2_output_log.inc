.. output_log

.. code-block:: none

    ...
    esptool --chip esp32h2 -p /dev/ttyUSB0 -b 460800 --before=default-reset --after=hard-reset write-flash --flash-mode dio --flash-freq 48m --flash-size 2MB 0x0 bootloader/bootloader.bin 0x8000 partition_table/partition-table.bin 0x10000 hello_world.bin
    esptool v5.0.2
    Connected to ESP32-H2 on /dev/ttyUSB0:
    Chip type:          ESP32-H2 (revision v0.2)
    Features:           BT 5 (LE), IEEE802.15.4, Single Core, 96MHz
    Crystal frequency:  32MHz
    MAC:                60:55:f9:ff:fe:f7:3a:56
    BASE MAC:           60:55:f9:f7:3a:56
    MAC_EXT:            ff:fe

    Stub flasher running.
    Changing baud rate to 460800...
    Changed.

    Configuring flash size...
    Flash will be erased from 0x00000000 to 0x00005fff...
    Flash will be erased from 0x00008000 to 0x00008fff...
    Flash will be erased from 0x00010000 to 0x00032fff...
    SHA digest in image updated.
    Wrote 22192 bytes (13780 compressed) at 0x00000000 in 0.7 seconds (252.5 kbit/s).
    Hash of data verified.
    Wrote 3072 bytes (103 compressed) at 0x00008000 in 0.1 seconds (248.5 kbit/s).
    Hash of data verified.
    Wrote 141920 bytes (76653 compressed) at 0x00010000 in 2.5 seconds (458.5 kbit/s).
    Hash of data verified.

    Hard resetting via RTS pin...
