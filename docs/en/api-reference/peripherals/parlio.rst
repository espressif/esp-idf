Parallel IO
===========

:link_to_translation:`zh_CN:[中文]`

Introduction
------------

The Parallel IO peripheral is a general purpose parallel interface that can be used to connect to external devices such as LED matrix, LCD display, Printer and Camera. The peripheral has independent TX and RX units. Each unit can have up to 8 or 16 data signals plus 1 or 2 clock signals. [1]_

The TX and RX driver of Parallel IO peripheral are designed separately, you can include ``driver/parlio_tx.h`` or ``driver/parlio_rx.h`` to use any of them.

Application Examples
--------------------

* :example:`peripherals/parlio/parlio_rx/logic_analyzer` demonstrates how to implement a logic analyzer using the Parallel IO RX peripheral, which can sample data on multiple GPIOs in parallel at a high rate, and provides options to probe either internal or external signals and output the raw data via flash or TCP stream.
* :example:`peripherals/parlio/parlio_tx/simple_rgb_led_matrix` demonstrates how to use the Parallel IO TX unit of {IDF_TARGET_NAME} to drive a RGB LED Matrix board with a HUB75 interface, using the LVGL library to display simple UI elements.


API Reference
-------------

.. include-build-file:: inc/parlio_tx.inc
.. include-build-file:: inc/parlio_rx.inc
.. include-build-file:: inc/components/esp_driver_parlio/include/driver/parlio_types.inc
.. include-build-file:: inc/components/hal/include/hal/parlio_types.inc

.. [1]
   Different ESP chip series might have different numbers of PARLIO TX/RX instances, and the maximum data bus can also be different. For more details, please refer to **{IDF_TARGET_NAME} Technical Reference Manual** > Chapter **Parallel IO (PARLIO)** [`PDF <{IDF_TARGET_TRM_EN_URL}#parlio>`__]. The driver does not forbid you from applying for more driver objects, but it returns error when all available hardware resources are used up. Please always check the return value when doing resource allocation (e.g., :cpp:func:`parlio_new_tx_unit`).
