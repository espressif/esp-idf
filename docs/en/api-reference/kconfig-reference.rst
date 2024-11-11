Configuration Options Reference
===============================

.. _configuration-options-reference:

Subsequent sections contain the list of available ESP-IDF options automatically generated from Kconfig files. Note that due to dependencies between options, some options listed here may not be visible by default in ``menuconfig``.

By convention, all option names are upper-case letters with underscores. When Kconfig generates ``sdkconfig`` and ``sdkconfig.h`` files, option names are prefixed with ``CONFIG_``. So if an option ``ENABLE_FOO`` is defined in a Kconfig file and selected in ``menuconfig``, then the ``sdkconfig`` and ``sdkconfig.h`` files will have ``CONFIG_ENABLE_FOO`` defined. In the following sections, option names are also prefixed with ``CONFIG_``, same as in the source code.


.. include-build-file:: inc/kconfig.inc
