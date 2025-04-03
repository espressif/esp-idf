Parallel IO RX Driver
=====================

:link_to_translation:`zh_CN:[中文]`

Introduction
------------

The Parallel IO RX unit typically supports the following scenarios:

- High-speed data acquisition, such as camera and sensor data reading
- High-speed parallel communication with other hosts as a slave device

Application Examples
---------------------

* :example:`peripherals/parlio/parlio_rx/logic_analyzer` demonstrates how to use the Parallel IO RX peripheral to implement a logic analyzer. This analyzer can sample data on multiple GPIOs at high frequency, monitor internal or external signals, and save the raw sampled data to Flash or output it through a TCP stream.

API Reference
-------------

.. include-build-file:: inc/parlio_rx.inc
