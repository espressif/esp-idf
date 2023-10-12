Performance Monitor
===================

:link_to_translation:`zh_CN:[中文]`

The Performance Monitor component provides APIs to use {IDF_TARGET_NAME} internal performance counters to profile functions and applications.

Application Example
-------------------

An example which combines performance monitor is provided in ``examples/system/perfmon`` directory. This example initializes the performance monitor structure and execute them with printing the statistics.

High-Level API Reference
------------------------

Header Files
^^^^^^^^^^^^

* :component_file:`perfmon/include/perfmon.h`

API Reference
-------------

.. include-build-file:: inc/xtensa_perfmon_access.inc
.. include-build-file:: inc/xtensa_perfmon_apis.inc
