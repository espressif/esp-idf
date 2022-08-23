Project Configuration
*********************

Introduction
============

ESP-IDF uses kconfiglib_ which is a Python-based extension to the Kconfig_ system which provides a compile-time project configuration mechanism. Kconfig is based around options of several types: integer, string, boolean. Kconfig files specify dependencies between options, default values of the options, the way the options are grouped together, etc.

For the complete list of available features please see Kconfig_ and `kconfiglib extentions`_.

.. _project-configuration-menu:

Project Configuration Menu
==========================

Application developers can open a terminal-based project configuration menu with the ``idf.py menuconfig`` build target.

After being updated, this configuration is saved inside ``sdkconfig`` file in the project root directory. Based on ``sdkconfig``, application build targets will generate ``sdkconfig.h`` file in the build directory, and will make sdkconfig options available to the project build system and source files.

Using sdkconfig.defaults
========================

In some cases, such as when ``sdkconfig`` file is under revision control, the fact that ``sdkconfig`` file gets changed by the build system may be inconvenient. The build system offers a way to avoid this, in the form of ``sdkconfig.defaults`` file. This file is never touched by the build system, and can be created manually or automatically. It can contain all the options which matter for the given application and are different from the default ones. The format is the same as that of the ``sdkconfig`` file. ``sdkconfig.defaults`` can be created manually when one remembers all the changed configurations. Otherwise, the file can be generated automatically by running the ``idf.py save-defconfig`` command.

Once ``sdkconfig.defaults`` is created, ``sdkconfig`` can be deleted and added to the ignore list of the revision control system (e.g. ``.gitignore`` file for ``git``). Project build targets will automatically create ``sdkconfig`` file, populated with the settings from ``sdkconfig.defaults`` file, and the rest of the settings will be set to their default values. Note that the build process will not override settings that are already in ``sdkconfig`` by ones from ``sdkconfig.defaults``. For more information, see :ref:`custom-sdkconfig-defaults`.

Kconfig Formatting Rules
========================

The following attributes of ``Kconfig`` files are standardized:

- Within any menu, option names should have a consistent prefix. The prefix length is currently set to at least 3
  characters.
- The indentation style is 4 characters created by spaces. All sub-items belonging to a parent item are indented by
  one level deeper. For example, ``menu`` is indented by 0 characters, the ``config`` inside of the menu by 4
  characters, the help of the ``config`` by 8 characters and the text of the ``help`` by 12 characters.
- No trailing spaces are allowed at the end of the lines.
- The maximum length of options is set to 40 characters.
- The maximum length of lines is set to 120 characters.

Format checker
--------------

``tools/check_kconfigs.py`` is provided for checking the ``Kconfig`` formatting rules. The checker checks all ``Kconfig`` and ``Kconfig.projbuild`` files in the ESP-IDF directory and generates a new file with suffix ``.new`` with some recommendations how to fix issues (if there are any). Please note that the checker cannot correct all rules and the responsibility of the developer is to check and make final corrections in order to pass the tests. For example, indentations will be corrected if there isn't some misleading previous formatting but it cannot come up with a common prefix for options inside a menu.

.. _configuration-options-compatibility:

Backward Compatibility of Kconfig Options
=========================================

The standard Kconfig_ tools ignore unknown options in ``sdkconfig``. So if a developer has custom settings for options which are renamed in newer ESP-IDF releases then the given setting for the option would be silently ignored. Therefore, several features have been adopted to avoid this:

1. ``confgen.py`` is used by the tool chain to pre-process ``sdkconfig`` files before anything else, for example
   ``menuconfig``, would read them. As the consequence, the settings for old options will be kept and not ignored.
2. ``confgen.py`` recursively finds all ``sdkconfig.rename`` files in ESP-IDF directory which contain old and new
   ``Kconfig`` option names. Old options are replaced by new ones in the ``sdkconfig`` file. Renames that should only appear for a single target can be placed in a target specific rename file: `sdkconfig.rename.TARGET`, where `TARGET` is the target name, e.g. `sdkconfig.rename.esp32s2`.
3. ``confgen.py`` post-processes ``sdkconfig`` files and generates all build
   outputs (``sdkconfig.h``, ``sdkconfig.cmake``, ``auto.conf``) by adding a list
   of compatibility statements, i.e. value of the old option is set the value of
   the new option (after modification). This is done in order to not break
   customer codes where old option might still be used.
4. :ref:`configuration-deprecated-options` are automatically generated by ``confgen.py``.

.. _configuration-options-reference:

Configuration Options Reference
===============================

Subsequent sections contain the list of available ESP-IDF options, automatically generated from Kconfig files. Note that depending on the options selected, some options listed here may not be visible by default in the interface of menuconfig.

By convention, all option names are upper case with underscores. When Kconfig generates sdkconfig and sdkconfig.h files, option names are prefixed with ``CONFIG_``. So if an option ``ENABLE_FOO`` is defined in a Kconfig file and selected in menuconfig, then sdkconfig and sdkconfig.h files will have ``CONFIG_ENABLE_FOO`` defined. In this reference, option names are also prefixed with ``CONFIG_``, same as in the source code.


.. include-build-file:: inc/kconfig.inc

.. _Kconfig: https://www.kernel.org/doc/Documentation/kbuild/kconfig-language.txt
.. _kconfiglib: https://github.com/ulfalizer/Kconfiglib
.. _kconfiglib extentions: https://pypi.org/project/kconfiglib/#kconfig-extensions
