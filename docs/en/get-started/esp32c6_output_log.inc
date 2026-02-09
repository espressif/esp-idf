.. output_log

.. code-block:: none

    ...
    esptool --chip esp32c6 -p /dev/ttyUSB0 -b 460800 --before=default-reset --after=hard-reset write-flash --flash-mode dio --flash-freq 80m --flash-size 2MB 0x0 bootloader/bootloader.bin 0x8000 partition_table/partition-table.bin 0x10000 hello_world.bin
    esptool v5.0.2
    Connected to ESP32-C6 on /dev/ttyUSB0:
    Chip type:          ESP32-C6 (QFN40) (revision v0.0)
    Features:           Wi-Fi 6, BT 5 (LE), IEEE802.15.4, Single Core + LP Core, 160MHz
    Crystal frequency:  40MHz
    MAC:                40:4c:ca:ff:fe:50:bf:18
    BASE MAC:           40:4c:ca:50:bf:18
    MAC_EXT:            ff:fe

    Stub flasher running.
    Changing baud rate to 460800...
    Changed.

    Configuring flash size...
    Flash will be erased from 0x00000000 to 0x00005fff...
    Flash will be erased from 0x00008000 to 0x00008fff...
    Flash will be erased from 0x00010000 to 0x0002efff...
    SHA digest in image updated.
    Wrote 22512 bytes (13994 compressed) at 0x00000000 in 0.3 seconds (662.3 kbit/s).
    Hash of data verified.
    Wrote 3072 bytes (103 compressed) at 0x00008000 in 0.0 seconds (630.2 kbit/s).
    Hash of data verified.
    Wrote 122976 bytes (63916 compressed) at 0x00010000 in 0.9 seconds (1087.1 kbit/s).
    Hash of data verified.

    Hard resetting via RTS pin...
