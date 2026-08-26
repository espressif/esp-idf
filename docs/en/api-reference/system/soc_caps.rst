SoC Capability Macros
=====================

:link_to_translation:`zh_CN:[中文]`

Different models of ESP chips integrate various hardware modules. Even the same type of module may have subtle differences across different chips. ESP-IDF provides a small "database" to describe the differences between chips (please note, only differences are described, not commonalities). The contents of this "database" are defined as macros in the **soc/soc_caps.h** file, referred to as **SoC capability macros**. Users can utilize these macros in their code with conditional compilation directives (such as ``#if``) to control which code is actually compiled.

.. note::

    Please note that the contents of **soc/soc_caps.h** are currently unstable and may undergo significant changes in the future.

Using SoC Capability Macros
---------------------------

We recommend accessing SoC capability macros indirectly through the following macro functions:

.. list-table::
    :widths: 30 60 80
    :header-rows: 1

    * - Macro Function
      - Description
      - Example
    * - :c:macro:`SOC_IS`
      - Determines the chip model
      - ``#if SOC_IS(ESP32)`` checks if the chip is ESP32
    * - :c:macro:`SOC_HAS`
      - Checks if the chip has a specific hardware module or feature
      - ``#if SOC_HAS(DAC)`` checks if the chip has a DAC module


API Reference
-------------

.. include-build-file:: inc/soc_caps.inc
.. include-build-file:: inc/soc_caps_eval.inc
