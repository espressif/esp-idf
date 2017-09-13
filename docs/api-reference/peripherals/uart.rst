UART
====

Overview
--------

`Instructions`_

.. _Instructions: ../template.html

Application Example
-------------------

Configure uart settings and install uart driver to read/write using UART0 and UART1 interfaces: :example:`peripherals/uart`.

API Reference
-------------

.. include:: /_build/inc/uart.inc

GPIO Lookup Macros
^^^^^^^^^^^^^^^^^^
Some useful macros can be used to specified the **direct** GPIO (UART module connected to pads through direct IO mux without the GPIO mux) number of a UART channel, or vice versa. 
The pin name can be omitted if specify the channel of a GPIO num.
e.g.

1. ``UART_NUM_2_TXD_DIRECT_GPIO_NUM`` is the GPIO number of UART channel 2 TXD pin (17);
2. ``UART_GPIO19_DIRECT_CHANNEL`` is the UART channel number of GPIO 19 (channel 0);
3. ``UART_CTS_GPIO19_DIRECT_CHANNEL`` is the UART channel number of GPIO 19, and GPIO 19 must be a CTS pin (channel 0).


.. include:: /_build/inc/uart_channel.inc


