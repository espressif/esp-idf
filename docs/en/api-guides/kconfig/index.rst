.. _configuration-overview:

Project Configuration
=====================

In smaller projects (e.g. command line utility like ``grep`` command), it is often enough to configure it e.g. via command line parameters. When the projects gets larger, dedicated configuration files (C/C++ header files, YAML, JSON etc.) are often used with parameters altering the code behavior. However, as the number of configuration options grows, it becomes more and more difficult to manage them. Furthermore, different parts of the project (think of e.g. IDE plugins) can have different ways of configuring.

ESP-IDF project unified the way to configure the project, build system, ESP-IDF framework itself and external components. This configuration tool is called Kconfig.

Kconfig tool uses distributed structure of configuration files of the same name (``Kconfig``). These files contain the **definition** of configuration options (e.g. name, type) and may contain their **default values**. Every component has its own ``Kconfig`` file. Their discovery is done automatically by the configuration tool.

**Values currently assigned** to the configuration options are then stored in one :ref:`sdkconfig file <sdkconfig-file>` in the project's root folder. This file is specific for the given project and is changed every time user makes a change in any of the configuration option's value. For the purpose of editing the configuration, the :ref:`menuconfig tool <project-configuration-menu>` is used. The values of the configuration options are also propagated to the CMake scripts and C code via ``sdkconfig.h`` and ``sdkconfig.cmake`` files.

For the purpose of setting **user-defined default values** for configuration options, the :ref:`sdkconfig.defaults file <sdkconfig-defaults-file>` can be used. This approach is also preferred in the situation when the project is under version control. This file is used to replace default values set by ``Kconfig`` files when the project is configured the first time and no ``sdkconfig`` is present, but those values can still be changed by the user via configuration tools (e.g. ``idf.py menuconfig``).

.. toctree::
    :maxdepth: 2

    ESP-IDF Project Configuration Guide <project-configuration-guide>
    Configuration Files and Structure <configuration_structure>
    Defining configuration options of components <component-configuration-guide>


.. note::

    For the detailed information about Kconfig language or ``esp-idf-kconfig``, which is the package of tools for the ESP-IDF configuration, please refer to the to the lower-level and more implementation-focused `esp-idf-kconfig documentation <https://docs.espressif.com/projects/esp-idf-kconfig/en/latest/>`_.

.. note::
    For the complete list of configuration options, please refer to the :ref:`Configuration Options Reference <configuration-options-reference>` and in the top left corner, choose your combination of a chip and ESP-IDF version.
