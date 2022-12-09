* Regarding stacks in PSRAM: For tasks not calling on code in ROM in any way, directly or indirectly, the menuconfig option :ref:`CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY` will eliminate the check in xTaskCreateStatic, allowing a task's stack to be in external RAM. Using this is not advised, however.
* When used at 80 MHz clock speed, external RAM must also occupy either the HSPI or VSPI bus. Select which SPI host will be used by :ref:`CONFIG_SPIRAM_OCCUPY_SPI_HOST`.


Chip revisions
==============

There are some issues with certain revisions of ESP32 that have repercussions for use with external RAM. The issues are documented in the `ESP32 ECO`_ document. In particular, ESP-IDF handles the bugs mentioned in the following ways:


ESP32 rev v0
------------
ESP-IDF has no workaround for the bugs in this revision of silicon, and it cannot be used to map external PSRAM into ESP32's main memory map.


ESP32 rev v1
------------
The bugs in this revision of silicon cause issues if certain sequences of machine instructions operate on external memory. (`ESP32 ECO`_ 3.2). As a workaround, the GCC compiler received the flag ``-mfix-esp32-psram-cache-issue`` to filter these sequences and only output the code that can safely be executed. Enable this flag by checking :ref:`CONFIG_SPIRAM_CACHE_WORKAROUND`. 

Aside from linking to a recompiled version of Newlib with the additional flag, ESP-IDF also does the following:

- Avoids using some ROM functions
- Allocates static memory for the WiFi stack

ESP32 rev v3
------------

ESP32 revision 3 ("ECO V3") fixes the PSRAM cache issue found in rev. 1. When :ref:`CONFIG_ESP32_REV_MIN` option is set to rev. 3, compiler workarounds related to PSRAM will be disabled. For more information about ESP32 ECO V3, see `ESP32 ECO V3 User Guide`_.
