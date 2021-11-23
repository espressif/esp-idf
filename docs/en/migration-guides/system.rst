Migrate System to ESP-IDF 5.0
==================================

Inter-Processor Call
-----------------------

IPC (Inter-Processor Call) component has been moved to ``esp_system``.

Thus, any project presenting a ``CMakeLists.txt`` file with the parameters ``PRIV_REQUIRES esp_ipc`` or ``REQUIRES esp_ipc``, should be modified to simply remove these options as ``esp_system`` component is included by default.

ESP Clock
---------

The old headers ``{IDF_TARGET_NAME}/clk.h``, ``esp_clk.h`` have been removed.

Therefore, If you want to use the function with the prefix ``esp_clk`` please include ``esp_private/esp_clk.h`` instead.

Cache Error Interrupt
---------------------

The old headers ``{IDF_TARGET_NAME}/cache_err_int.h`` have been removed. Please include ``esp_private/cache_err_int.h`` instead.

Brownout
--------

The header ``brownout.h`` has been made private. ESP-IDF developers should include ``esp_private/brownout.h`` instead.

Trax
----

The header ``trax.h`` has been made private. ESP-IDF developers should include ``esp_private/trax.h`` instead.
