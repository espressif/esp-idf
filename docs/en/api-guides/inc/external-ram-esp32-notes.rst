* Regarding stacks in PSRAM: For tasks that do not call ROM code in any way (directly or indirectly), the :ref:`CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY` option will eliminate the check in :cpp:func:`xTaskCreateStatic`, allowing a task's stack to be in external RAM. However, using this is **not advised**.
* When used at 80 MHz clock speed, external RAM must also occupy either the HSPI or VSPI bus. Select which SPI host will be used by :ref:`CONFIG_SPIRAM_OCCUPY_SPI_HOST`.


Chip Revisions
==============

There are some issues with certain revisions of ESP32 that have repercussions for use with external RAM. The issues are documented in the `ESP32 Series SoC Errata`_ document. In particular, ESP-IDF handles the bugs mentioned in the following ways:


ESP32 Rev v0.0
--------------
ESP-IDF has no workaround for the bugs in this revision of silicon, and it cannot be used to map external PSRAM into ESP32's main memory map.


ESP32 Rev v1.0
--------------
The bugs in this revision of silicon cause issues if certain sequences of machine instructions operate on external memory. (`ESP32 Series SoC Errata`_ 3.2). As a workaround, the ``-mfix-esp32-psram-cache-issue`` flag has been added to the ESP32 GCC compiler such that these sequences are filtered out. As a result, the compiler only outputs code that can safely be executed. The :ref:`CONFIG_SPIRAM_CACHE_WORKAROUND` option can be used to enable this workaround.

Aside from linking to a recompiled version of Newlib with the additional flag, ESP-IDF also does the following:

- Avoids using some ROM functions
- Allocates static memory for the Wi-Fi stack

ESP32 Rev v3.0
--------------

ESP32 rev v3.0 fixes the PSRAM cache issue found in rev v1.0. When :ref:`CONFIG_ESP32_REV_MIN` option is set to ``rev v3.0``, compiler workarounds related to PSRAM will be disabled. For more information about ESP32 v3.0, see `ESP32 Chip Revision v3.0 User Guide`_.
