Configuration Options
*********************

Introduction
============

ESP-IDF uses Kconfig_ system to provide a compile-time configuration mechanism. Kconfig is based around options of several types: integer, string, boolean. Kconfig files specify dependencies between options, default values of the options, the way the options are grouped together, etc.

Applications developers can use ``make menuconfig`` build target to edit components' configuration. This configuration is saved inside ``sdkconfig`` file in the project root directory. Based on ``sdkconfig``, application build targets will generate ``sdkconfig.h`` file in the build directory, and will make sdkconfig options available to component makefiles.

Using sdkconfig.defaults
========================

When updating ESP-IDF version, it is not uncommon to find that new Kconfig options are introduced. When this happens, application build targets will offer an interactive prompt to select values for the new options. New values are then written into ``sdkconfig`` file. To supress interactive prompts, applications can either define ``BATCH_BUILD`` environment variable, which will cause all prompts to be suppressed. This is the same effect as that of ``V`` or ``VERBOSE`` variables. Alternatively, ``defconfig`` build target can be used to update configuration for all new variables to the default values.

In some cases, such as when ``sdkconfig`` file is under revision control, the fact that ``sdkconfig`` file gets changed by the build system may be inconvenient. The build system offers a way to avoid this, in the form of ``sdkconfig.defaults`` file. This file is never touched by the build system, and must be created manually. It can contain all the options which matter for the given application. The format is the same as that of the ``sdkconfig`` file. Once ``sdkconfig.defaults`` is created, ``sdkconfig`` can be deleted and added to the ignore list of the revision control system (e.g. ``.gitignore`` file for git). Project build targets will automatically create ``sdkconfig`` file, populated with the settings from ``sdkconfig.defaults`` file, and the rest of the settings will be set to their default values. Note that when ``make defconfig`` is used, settings in sdkconfig will be overriden by the ones in ``sdkconfig.defaults``. For more information, see :ref:`custom-sdkconfig-defaults`.

Configuration Options Reference
===============================

Subsequent sections contain the list of available ESP-IDF options, automatically generated from Kconfig files. Note that depending on the options selected, some options listed here may not be visible by default in the interface of menuconfig.

By convention, all option names are upper case with underscores. When Kconfig generates sdkconfig and sdkconfig.h files, option names are prefixed with ``CONFIG_``. So if an option ``ENABLE_FOO`` is defined in a Kconfig file and selected in menuconfig, then sdkconfig and sdkconfig.h files will have ``CONFIG_ENABLE_FOO`` defined. In this reference, option names are also prefixed with ``CONFIG_``, same as in the source code.


.. include:: /_build/inc/kconfig.inc

.. _Kconfig: https://www.kernel.org/doc/Documentation/kbuild/kconfig-language.txt