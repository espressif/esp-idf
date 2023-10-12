SoC Capabilities
================

:link_to_translation:`zh_CN:[中文]`

This section lists the macro definitions of the {IDF_TARGET_NAME}'s SoC hardware capabilities. These macros are commonly used by conditional-compilation directives (e.g., ``#if``) in ESP-IDF to determine which hardware-dependent features are supported, thus control what portions of code are compiled.

.. warning::

    These macro definitions are currently not considered to be part of the public API, and may be changed in a breaking manner (see :doc:`../../../versions` for more details).


API Reference
-------------

.. include-build-file:: inc/soc_caps.inc
