Himem
=====

:link_to_translation:`zh_CN:[中文]`

Overview
--------

For external memories that are <= 4 MiB, the MMU is configured to use a "unity mapping", meaning that each CPU address is mapped 1-to-1 to the external SPI RAM address, thus allowing external memory to be accessed transparently. However, because the address space for external memory is limited to 4 MiB, only SPI RAM chips that are <= 4 MiB in size can be used fully transparently.

It is still possible for ESP32 to use SPI RAM chips >= 4 MiB in size. However, the memory on these chips needs to be accessed using a bank switching scheme. ESP-IDF provides the **Himem API** to control this bank switching. More specifically, the Himem API allows particular 32 K banks within 4 MiB address switch mappings at run time, thus allowing access to more than 4 MiB of external memory.

Usage
-----

In order to use the Himem API, you have to enable it in the menuconfig using :ref:`CONFIG_SPIRAM_BANKSWITCH_ENABLE`, as well as set the amount of banks reserved for this in :ref:`CONFIG_SPIRAM_BANKSWITCH_RESERVE`. This decreases the amount of external memory allocated by functions like ``malloc()``, but it allows you to use the Himem API to map any of the remaining memory into the reserved banks.

The Himem API is more-or-less an abstraction of the bank switching scheme: it allows you to claim one or more banks of address space (called 'regions' in the API) as well as one or more of banks of memory to map into the ranges.

Example
-------

An example doing a simple memory test of the high memory range is available in ESP-IDF: :example:`system/himem`


API Reference
-------------

.. include-build-file:: inc/himem.inc
