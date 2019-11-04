Performance Monitor
===================

The Performance Monitor component provides APIs to use ESP32 internal performance counters to profile functions and
applications.

Application Example
-------------------

An example which combines performance monitor is provided in ``examples/system/perfmon`` directory. 
This example initializes the performance monitor structure and execute them with printing the statistics.

High level API Reference
------------------------

Header Files
^^^^^^^^^^^^

* :component_file:`perfmon/include/perfmon.h`

API Reference
-------------

.. include:: /_build/inc/xtensa_perfmon_access.inc
.. include:: /_build/inc/xtensa_perfmon_apis.inc

