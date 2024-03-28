Project Configuration
*********************
:link_to_translation:`zh_CN:[中文]`

Introduction
============

The esp-idf-kconfig_ package that ESP-IDF uses is based on kconfiglib_, which is a Python extension to the Kconfig_ system. Kconfig provides a compile-time project configuration mechanism and offers configuration options of several types (e.g., integers, strings, and Booleans). Kconfig files specify dependencies between options, default values of options, the way options are grouped together, etc.

For the full list of available features, please see Kconfig_ and `kconfiglib extensions`_.

.. _project-configuration-menu:

Project Configuration Menu
==========================

Application developers can open a terminal-based project configuration menu with the ``idf.py menuconfig`` build target.

After being updated, this configuration is saved in the ``sdkconfig`` file under the project root directory. Based on ``sdkconfig``, application build targets will generate the ``sdkconfig.h`` file under the build directory, and will make the ``sdkconfig`` options available to the project build system and source files.

Using ``sdkconfig.defaults``
============================

In some cases, for example, when the ``sdkconfig`` file is under revision control, it may be inconvenient for the build system to change the ``sdkconfig`` file. The build system offers a solution to prevent it from happening, which is to create the ``sdkconfig.defaults`` file. This file is never touched by the build system, and can be created manually or automatically. It contains all the options which matter to the given application and are different from the default ones. The format is the same as that of the ``sdkconfig`` file. ``sdkconfig.defaults`` can be created manually when one remembers all the changed configuration, or it can be generated automatically by running the ``idf.py save-defconfig`` command.

Once ``sdkconfig.defaults`` is created, ``sdkconfig`` can be deleted or added to the ignore list of the revision control system (e.g., the ``.gitignore`` file for ``git``). Project build targets will automatically create the ``sdkconfig`` file, populate it with the settings from the ``sdkconfig.defaults`` file, and configure the rest of the settings to their default values. Note that during the build process, settings from ``sdkconfig.defaults`` will not override those already in ``sdkconfig``. For more information, see :ref:`custom-sdkconfig-defaults`.

Kconfig Format Rules
====================

Format rules for Kconfig files are as follows:

- Option names in any menus should have consistent prefixes. The prefix currently should have at least 3 characters.
- The unit of indentation should be 4 spaces. All sub-items belonging to a parent item are indented by one level deeper. For example, ``menu`` is indented by 0 spaces, ``config``  ``menu`` by 4 spaces, ``help`` in ``config`` by 8 spaces, and the text under ``help`` by 12 spaces.
- No trailing spaces are allowed at the end of the lines.
- The maximum length of options is 50 characters.
- The maximum length of lines is 120 characters.

.. note::

    The ``help`` section of each config in the menu is treated as reStructuredText to generate the reference documentation for each option.

Format Checker
--------------

``kconfcheck`` tool in esp-idf-kconfig_ package is provided for checking Kconfig files against the above format rules. The checker checks all Kconfig and ``Kconfig.projbuild`` files given as arguments, and generates a new file with suffix ``.new`` with some suggestions about how to fix issues (if there are any). Please note that the checker cannot correct all format issues and the responsibility of the developer is to final check and make corrections in order to pass the tests. For example, indentations will be corrected if there is not any misleading formatting, but it cannot come up with a common prefix for options inside a menu.

The ``esp-idf-kconfig`` package is available in ESP-IDF environments, where the checker tool can be invoked by running command ``python -m kconfcheck <path_to_kconfig_file>``.

For more information, please refer to `esp-idf-kconfig package documentation <https://github.com/espressif/esp-idf-kconfig/blob/master/docs/DOCUMENTATION.md>`__.

.. _configuration-options-compatibility:

Backward Compatibility of Kconfig Options
=========================================

The standard Kconfig_ tools ignore unknown options in ``sdkconfig``. So if a developer has custom settings for options which are renamed in newer ESP-IDF releases, then the given setting for the option would be silently ignored. Therefore, several features have been adopted to avoid this:

1. ``kconfgen`` is used by the tool chain to pre-process ``sdkconfig`` files before anything else. For example, ``menuconfig`` would read them, so the settings for old options is kept and not ignored.
2. ``kconfgen`` recursively finds all ``sdkconfig.rename`` files in ESP-IDF directory which contain old and new ``Kconfig`` option names. Old options are replaced by new ones in the ``sdkconfig`` file. Renames that should only appear for a single target can be placed in a target-specific rename file ``sdkconfig.rename.TARGET``, where ``TARGET`` is the target name, e.g., ``sdkconfig.rename.esp32s2``.
3. ``kconfgen`` post-processes ``sdkconfig`` files and generates all build outputs (``sdkconfig.h``, ``sdkconfig.cmake``, and ``auto.conf``) by adding a list of compatibility statements, i.e., the values of old options are set for new options after modification. If users still use old options in their code, this will prevent it from breaking.
4. :ref:`configuration-deprecated-options` are automatically generated by ``kconfgen``.

.. _configuration-options-reference:

Configuration Options Reference
===============================

Subsequent sections contain the list of available ESP-IDF options automatically generated from Kconfig files. Note that due to dependencies between options, some options listed here may not be visible by default in ``menuconfig``.

By convention, all option names are upper-case letters with underscores. When Kconfig generates ``sdkconfig`` and ``sdkconfig.h`` files, option names are prefixed with ``CONFIG_``. So if an option ``ENABLE_FOO`` is defined in a Kconfig file and selected in ``menuconfig``, then the ``sdkconfig`` and ``sdkconfig.h`` files will have ``CONFIG_ENABLE_FOO`` defined. In the following sections, option names are also prefixed with ``CONFIG_``, same as in the source code.


.. include-build-file:: inc/kconfig.inc

.. _Kconfig: https://www.kernel.org/doc/Documentation/kbuild/kconfig-language.txt
.. _esp-idf-kconfig: https://pypi.org/project/esp-idf-kconfig/
.. _kconfiglib: https://github.com/ulfalizer/Kconfiglib
.. _kconfiglib extensions: https://pypi.org/project/kconfiglib/#kconfig-extensions
