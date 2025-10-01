.. output_log

.. code-block:: none

    ...
    esptool --chip esp32s2 -p /dev/ttyUSB0 -b 460800 --before=default-reset --after=hard-reset write-flash --flash-mode dio --flash-freq 80m --flash-size 2MB 0x1000 bootloader/bootloader.bin 0x8000 partition_table/partition-table.bin 0x10000 hello_world.bin
    esptool v5.0.2
    Connected to ESP32-S2 on /dev/ttyUSB0:
    Chip type:          ESP32-S2FH4 (revision v0.0)
    Features:           Wi-Fi, Single Core, 240MHz, Embedded Flash 4MB, No Embedded PSRAM, ADC and temperature sensor calibration in BLK2 of eFuse V1
    Crystal frequency:  40MHz
    MAC:                84:f7:03:e3:3f:bc

    Stub flasher running.
    Changing baud rate to 460800...
    Changed.

    Configuring flash size...
    Flash will be erased from 0x00001000 to 0x00006fff...
    Flash will be erased from 0x00008000 to 0x00008fff...
    Flash will be erased from 0x00010000 to 0x00030fff...
    SHA digest in image updated.
    Wrote 21968 bytes (13914 compressed) at 0x00001000 in 0.7 seconds (251.0 kbit/s).
    Hash of data verified.
    Wrote 3072 bytes (103 compressed) at 0x00008000 in 0.1 seconds (238.5 kbit/s).
    Hash of data verified.
    Wrote 132096 bytes (76394 compressed) at 0x00010000 in 2.2 seconds (486.9 kbit/s).
    Hash of data verified.

    Hard resetting via RTS pin...
