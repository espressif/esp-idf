.. output_log


.. code-block:: none

    ...
    esptool --chip esp32p4 -p /dev/ttyUSB0 -b 460800 --before=default-reset --after=hard-reset write-flash --flash-mode dio --flash-freq 80m --flash-size 2MB 0x2000 bootloader/bootloader.bin 0x8000 partition_table/partition-table.bin 0x10000 hello_world.bin
    esptool v5.0.2
    Connected to ESP32-P4 on /dev/ttyUSB0:
    Chip type:          ESP32-P4 (revision v1.0)
    Features:           Dual Core + LP Core, 400MHz
    Crystal frequency:  40MHz
    MAC:                60:55:f9:fc:00:a2

    Stub flasher running.
    Changing baud rate to 460800...
    Changed.

    Configuring flash size...
    Flash will be erased from 0x00002000 to 0x00007fff...
    Flash will be erased from 0x00008000 to 0x00008fff...
    Flash will be erased from 0x00010000 to 0x00038fff...
    SHA digest in image updated.
    Wrote 22896 bytes (13985 compressed) at 0x00002000 in 0.4 seconds (477.0 kbit/s).
    Hash of data verified.
    Wrote 3072 bytes (103 compressed) at 0x00008000 in 0.1 seconds (416.6 kbit/s).
    Hash of data verified.
    Wrote 164496 bytes (87363 compressed) at 0x00010000 in 1.4 seconds (931.3 kbit/s).
    Hash of data verified.

    Hard resetting via RTS pin...
