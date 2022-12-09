FreeRTOS
========

Overview
--------

This section contains documentation of FreeRTOS types, functions, and macros. It is automatically generated from FreeRTOS header files.

.. note::
    ESP-IDF FreeRTOS is based on the Xtensa port of FreeRTOS v8.2.0, however some functions of FreeRTOS v9.0.0 have been backported. See the :ref:`backported-features` for more information.

For more information about FreeRTOS features specific to ESP-IDF, see :doc:`ESP-IDF FreeRTOS SMP Changes<../../api-guides/freertos-smp>`
and :doc:`ESP-IDF FreeRTOS Additions<freertos_additions>`.


Task API
--------

.. include-build-file:: inc/task.inc

Queue API
---------

.. include-build-file:: inc/queue.inc

Semaphore API
-------------

.. include-build-file:: inc/semphr.inc

Timer API
---------

.. include-build-file:: inc/timers.inc


Event Group API
---------------

.. include-build-file:: inc/event_groups.inc


