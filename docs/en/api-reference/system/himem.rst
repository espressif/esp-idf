The himem allocation API
========================

Overview
--------

The ESP32 can access external SPI RAM transparently, so you can use it as normal memory in your program code. However, because the address
space for external memory is limited in size, only the first 4MiB can be used as such. Access to the remaining memory is still possible,
however this needs to go through a bankswitching scheme controlled by the himem API.

Specifically, what is implemented by the himem API is a bankswitching scheme. Hardware-wise, the 4MiB region for external SPI RAM is 
mapped into the CPU address space by a MMU, which maps a configurable 32K bank/page of external SPI RAM into each of the 32K pages in the
4MiB region accessed by the CPU. For external memories that are <=4MiB, this MMU is configured to unity mapping, effectively mapping each
CPU address 1-to-1 to the external SPI RAM address.

In order to use the himem API, you have to enable it in the menuconfig using :envvar:`CONFIG_SPIRAM_BANKSWITCH_ENABLE`, as well as set the amount 
of banks reserved for this in :envvar:`CONFIG_SPIRAM_BANKSWITCH_RESERVE`. This decreases
the amount of external memory allocated by functions like ``malloc()``, but it allows you to use the himem api to map any of the remaining memory
into the reserved banks.

The himem API is more-or-less an abstraction of the bankswitching scheme: it allows you to claim one or more banks of address space
(called 'regions' in the API) as well as one or more of banks of memory to map into the ranges.

Example
-------

An example doing a simple memory test of the high memory range is available in esp-idf: :example:`system/himem`


API Reference
-------------

.. include:: /_build/inc/himem.inc
