Migrate System to ESP-IDF 5.0
==================================

Inter-Processor Call
-----------------------

IPC (Inter-Processor Call) component has been moved to ``esp_system``.

Thus, any project presenting a ``CMakeLists.txt`` file with the parameters ``PRIV_REQUIRES esp_ipc`` or ``REQUIRES esp_ipc``, should be modified to simply remove these options as ``esp_system`` component is included by default.

