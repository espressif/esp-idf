.. _xip_from_psram:

XIP from PSRAM Feature
----------------------

If :ref:`CONFIG_SPIRAM_XIP_FROM_PSRAM` is enabled, the flash ``.text`` sections (used for instructions) and the flash ``.rodata`` sections (used for read only data) will be placed in PSRAM.

The corresponding virtual memory range will be mapped to PSRAM.

If both of the above options are enabled, the Cache won't be disabled during an SPI1 Flash operation. You don't need to make sure ISRs, ISR callbacks and involved data are placed in internal RAM.
