.. _xip_from_psram:

XIP from PSRAM Feature
----------------------

If :ref:`CONFIG_SPIRAM_FETCH_INSTRUCTIONS` is enabled, the flash ``.text`` sections (used for instructions) will be placed in PSRAM.

If :ref:`CONFIG_SPIRAM_RODATA` is enabled, the flash ``.rodata`` sections (used for read only data) will be placed in PSRAM.

The corresponding virtual memory range will be re-mapped to PSRAM.

If both of the above options are enabled, the Cache won't be disabled during an SPI1 Flash operation. You don't need to make sure ISRs, ISR callbacks and involved data are placed in internal RAM.
