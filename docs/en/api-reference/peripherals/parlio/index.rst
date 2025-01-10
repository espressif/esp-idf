Parallel IO
===========

:link_to_translation:`zh_CN:[中文]`

Introduction
------------

[`Parallel IO Peripheral <{IDF_TARGET_TRM_EN_URL}#parlio>`__] is a general parallel interface that can be used to connect external devices such as LED matrices, LCD displays, printers, and cameras. This peripheral has independent TX and RX units. Each unit can have up to 8 or 16 data signals and 1 or 2 clock signals.

The TX and RX drivers of the Parallel IO peripheral are independently designed and can be used by including the header files ``driver/parlio_tx.h`` or ``driver/parlio_rx.h``.

.. toctree::
    :maxdepth: 1

    parlio_tx
    parlio_rx

API Reference
-------------

.. include-build-file:: inc/components/esp_driver_parlio/include/driver/parlio_types.inc
.. include-build-file:: inc/components/hal/include/hal/parlio_types.inc
