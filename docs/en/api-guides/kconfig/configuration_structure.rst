.. _configuration-structure:

Configuration Files Structure and Relationships
===============================================

.. note::

    This article primarily focuses on the structure of the files. For more information about project configuration, please refer to the :ref:`Project Configuration Guide <project-configuration-guide>`. For the component configuration, please refer to the :ref:`Component Configuration Guide <component-configuration-guide>`.

ESP-IDF uses `Kconfig language <https://docs.espressif.com/projects/esp-idf-kconfig/en/latest/kconfiglib/language.html>`_ for configuration of the project. Configuration consists of config options (e.g. ``CONFIG_IDF_TARGET``) and their values (e.g. ``esp32``). Every config option has a prefix ``CONFIG_`` to distinguish it from e.g. environment variables.

In context of ESP-IDF, configuration consists of several files, most importantly:

- ``Kconfig`` files, defining the configuration options and their relationships, together with their default values (if any).
- ``sdkconfig`` file, containing the currently used values of configuration options.
- ``sdkconfig.defaults`` file, containing user-defined default values for the configuration options.
- ``sdkconfig.rename`` file, containing ``OLD_NAME NEW_NAME`` pairs of configuration names to ensure backward compatibility. This file is used primarily by component or ESP-IDF developers.

Configuration files can be divided into two groups: those primarily **defining** the configuration options and those containing the **values** of these options. First group includes ``Kconfig``, ``Kcofnig.projbuild`` and ``sdkconfig.rename`` files, second group includes ``sdkconfig``, ``sdkconfig.defaults``, ``sdkconfig.h`` and ``sdkconfig.cmake`` files. All the files will be described in the following sections.

For more information about the configuration system in ESP-IDF, please refer to the :ref:`Configuration Overview <configuration-overview>`.

.. _kconfig-files:

Kconfig and Kconfig.projbuild Files
-----------------------------------

The ``Kconfig.*`` files store the configuration options, together with their properties and relationships. They also may contain default values for the configuration options. Every component has its own ``Kconfig`` file, which is used to define the configuration options for that component.

The only difference between ``Kconfig`` and ``Kconfig.projbuild`` files is where the their content will appear in the configuration interface (menuconfig):

- ``Kconfig``: The content of this file will appear in the ``Component config`` window of the configuration interface.
- ``Kconfig.projbuild``: The content of this file will appear in the root window of the configuration interface.

Example of ``Kconfig`` file:

.. code-block:: kconfig

    mainmenu "Motors configuration"

        config SUBLIGHT_DRIVE_ENABLED
            bool "Enable sublight drive"
            default y
            help
                This option enables sublight on our spaceship.

For more information about the Kconfig language, please refer to the `Kconfig Documentation <https://docs.espressif.com/projects/esp-idf-kconfig/en/latest/kconfiglib/language.html>`_.

.. _sdkconfig-file:

sdkconfig and sdkconfig.old
^^^^^^^^^^^^^^^^^^^^^^^^^^^

In the ``sdkconfig`` file, **values currently assigned** to the configuration options are stored. It is generated automatically and is not to be edited, because configuration options can have relationships between one another (dependencies and reverse dependencies), that can be broken by manual editing.

Every line follows one of the following patterns:

- ``CONFIG_NAME=<value>``: Config name and its value.
- ``# CONFIG_NAME is not set``: Boolean config ``CONFIG_NAME`` is visible, but it is set to n. For non-Boolean configs, ``CONFIG_NAME=""`` will be present instead.
- Other #-style comments and empty lines.

The ``sdkconfig.old`` file is a backup of the previous configuration. It is generated every time the ``sdkconfig`` file is generated.

.. note::

    There are other sdkconfig.* files in the project, namely ``sdkconfig.h``, ``sdkconfig.cmake``, ``sdkconfig.json``. These files are the same as the ``sdkconfig`` file mentioned above, but in different formats. These files are used with their respective tools (C/C++ code, CMake). You can check :ref:`configs-in-C-Cmake` for more information.

.. _sdkconfig-rename-file:

sdkconfig.rename and sdkconfig.rename.<chip>
----------------------------------------------------

The ``sdkconfig.rename`` files are used by the build system to ensure backward compatibility. These files are created and maintained by component or ESP-IDF developers and application developer should have no need to edit them.

The structure of the ``sdkconfig.rename`` file is as follows:

* Lines starting with ``#`` and empty lines will be ignored.
* All other lines should follow one of these formats:
    * ``CONFIG_DEPRECATED_NAME CONFIG_NEW_NAME``, where ``CONFIG_DEPRECATED_NAME`` is the old config name which was renamed in a newer ESP-IDF version to ``CONFIG_NEW_NAME``.
    * ``CONFIG_DEPRECATED_NAME !CONFIG_NEW_INVERTED_NAME`` where ``CONFIG_NEW_INVERTED_NAME`` was introduced in a newer ESP-IDF version by Boolean inversion of the logic value of ``CONFIG_DEPRECATED_NAME``.

Primary use case of this file is to ensure backward compatibility when the config name is changed in the newer ESP-IDF version.

Example:

``sdkconfig.rename``:

.. code-block:: kconfig

    # new name            old name
    CONFIG_HYPEDRIVE      CONFIG_WARP_DRIVE
    CONFIG_DISABLE_WAPRT_ !CONFIG_ENABLE_WARP_DRIVE

``sdkconfig``:

.. code-block:: kconfig

    (...)
    CONFIG_HYPEDRIVE=y
    CONFIG_DISABLE_HYPEDRIVE=n
    (...)
    # Deprecated options for backward compatibility
    CONFIG_WARP_DRIVE=y
    CONFIG_ENABLE_WARP_DRIVE=y
    # End of deprecated options

.. _sdkconfig-defaults-file:

sdkconfig.defaults and sdkconfig.defaults.<chip>
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Kconfig language provides a way to set default values for configs: ``default`` option. However, input Kconfig file may be in a different project, under version control or there is another reason why it would be inconvenient to directly edit it. In this case, ``sdkconfig.defaults`` file can be used. The file structure is the same as ``sdkconfig`` file; on every line, there is a full config name (including the ``CONFIG_`` prefix) and its value. This value has precedence over the default value in the Kconfig file by ``default`` option.

It is also possible to override the default values only for specific target. In this case, you can create ``sdkconfig.defaults.<chip>`` file, where ``<chip>`` is the target name (e.g. ``esp32s2``). In this case, it is mandatory to create the ``sdkconfig.defaults`` file as well, otherwise the ``sdkconfig.defaults.<chip>`` file will be ignored. However, the ``sdkconfig.defaults`` file can be empty.

.. note::

    Values in ``sdkconfig.defaults`` are additional default values, not overrides! This means that those values will be ignored if ``sdkconfig`` file is present.

It is also possible to override the name of this file by setting an environment variable. For information on how to set the custom file name and in which order the files are processed if multiple files with default values are present, please visit :ref:`Custom Sdkconfig Defaults <custom-sdkconfig-defaults>` section of Build System documentation.

Example:

``Kconfig``:

.. code-block:: kconfig

    (...)
    config SUBLIGHT_SPEED
        int "Sublight speed"
        default 10
    (...)

``sdkconifg.defaults``:

.. code-block:: text

    CONFIG_SUBLIGHT_SPEED=42

When running e.g. ``idf.py menuconfig``, the ``SUBLIGHT_SPEED`` will be set to 42. If the value will be changed in the GUI, the value from the GUI will be used and saved into ``sdkconfig`` file.

How to generate ``sdkconfig.defaults`` file:

1. ``cd`` into your project folder.
2. Configure everything you need in ``idf.py menuconfig``.
3. Run ``idf.py save-defconfig``. This will generate ``sdkconfig.defaults`` file with all the values different from the default ones.

sdkconfig.ci
^^^^^^^^^^^^

Some of the IDF examples include a ``sdkconfig.ci`` file. This is part of the continuous integration (CI) test framework and is ignored by the normal build process.
