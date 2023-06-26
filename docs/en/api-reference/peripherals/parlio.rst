Parallel IO
===========

Introduction
------------

The Parallel IO peripheral is a general purpose parallel interface that can be used to connect to external devices such as LED matrix, LCD display, Printer and Camera. The peripheral has independent TX and RX units. Each unit can have up to 8 or 16 data signals plus 1 or 2 clock signals. [1]_

.. warning::
	At the moment, the Parallel IO driver only supports TX mode. The RX feature is still working in progress.


Application Examples
--------------------

* Simple REG LED Matrix with HUB75 interface: :example:`peripherals/parlio/simple_rgb_led_matrix`.


API Reference
-------------

.. include-build-file:: inc/parlio_tx.inc
.. include-build-file:: inc/components/driver/parlio/include/driver/parlio_types.inc
.. include-build-file:: inc/components/hal/include/hal/parlio_types.inc

.. [1]
   Different ESP chip series might have different numbers of PARLIO TX/RX instances, and the maximum data bus can also be different. For more details, please refer to *{IDF_TARGET_NAME} Technical Reference Manual* > Chapter *Parallel IO (PARLIO)* [`PDF <{IDF_TARGET_TRM_EN_URL}#parlio>`__]. The driver will not forbid you from applying for more driver objects, but it will return error when all available hardware resources are used up. Please always check the return value when doing resource allocation (e.g. :cpp:func:`parlio_new_tx_unit`).
