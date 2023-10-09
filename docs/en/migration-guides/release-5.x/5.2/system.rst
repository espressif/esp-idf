System
======

:link_to_translation:`zh_CN:[中文]`

FreeRTOS
--------

Task Snapshot
^^^^^^^^^^^^^

The Task Snapshot API has been made private due to a lack of a practical way for the API to be used from user code (the scheduler must be halted before the API can be called).

As a result, the ``#include "freertos/task_snapshot.h"`` include path has been deprecated.