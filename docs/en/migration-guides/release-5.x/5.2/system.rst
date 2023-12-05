System
======

:link_to_translation:`zh_CN:[中文]`

FreeRTOS
--------

Task Snapshot
^^^^^^^^^^^^^

The Task Snapshot API has been made private due to a lack of a practical way for the API to be used from user code (the scheduler must be halted before the API can be called).

.. only:: CONFIG_IDF_TARGET_ARCH_XTENSA

    Xtensa
    ------

    A number of legacy include paths for Xtensa headers have been deprecated:

    - ``#include "freertos/xtensa_api.h"`` is deprecated, please use ``#include "xtensa_api.h"`` instead.
    - ``#include "freertos/xtensa_context.h"`` is deprecated, please use ``#include "xtensa_context.h"`` instead.
    - ``#include "freertos/xtensa_timer.h"`` is deprecated, please use ``#include "xtensa_timer.h"`` instead.
