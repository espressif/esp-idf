.. _component-configuration-guide:

Component Configuration Guide
=============================

This guide is intended to describe how to define configuration options for components in ESP-IDF. Following topics will be covered:

- How to define new configuration options for components.
- Basic syntax of Kconfig language.
- How to ensure backward compatibility.

How Configuration Works in ESP-IDF
----------------------------------

.. note::

    More detailed information about the configuration system in ESP-IDF can be found in the :ref:`configuration-overview`.


ESP-IDF uses unified way to configure the project, build system, ESP-IDF framework itself and external components. This configuration tool is called `Kconfig <https://docs.espressif.com/projects/esp-idf-kconfig/en/latest/kconfiglib/index.html>`_.

Configuration options are **defined** in ``Kconfig`` files. ESP-IDF contains the top-level ``Kconfig`` file in the root of the framework. Each component can have its own ``Kconfig`` file defining configuration options specific to that component, as well as relations between the options. Relations between config options can spread across multiple Kconfig files from different sources. In other words, configuration option from ``Component_A`` can depend on a configuration option from ``Component_B``, even if ``Component_B`` is e.g. maintained by another developer.

When the configuration is saved (for more information about configuration editing, see e.g. :ref:`project-configuration-guide`), the  **values** in the ``sdkconfig`` file are updated, as well as those in ``sdkconfig.h``, ``sdkconfig.cmake`` and ``sdkconfig.json``.

How to Define New Configuration Options for Your Component
----------------------------------------------------------
.. note::

    If you plan to write ``Kconfig`` configuration files for your component, but you are not familiar with ``Kconfig language`` it is recommended to refer to the `esp-idf-kconfig Documentation <https://docs.espressif.com/projects/esp-idf-kconfig/en/latest/index.html>`_, where an in-depth guide is provided.

To define new configuration options for components, you need to:

1. Create the ``Kconfig`` and/or ``Kconfig.projbuild`` in the root folder of the component.
2. Define the configuration options in the ``Kconfig`` and/or ``Kconfig.projbuild`` file. It is generally a good practice to wrap them in the ``menu-endmenu`` block. You can see a minimal example below. Complete documentation of the Kconfig language can be found in the `Kconfig Documentation <https://docs.espressif.com/projects/esp-idf-kconfig/en/latest/kconfiglib/language.html>`_.

When your component is used in a project, the ``Kconfig`` and/or ``Kconfig.projbuild`` will be automatically discovered and shown in the ``menuconfig`` tool.

.. note::

    **Difference between Kconfig and Kconfig.projbuild files**:

    - ``Kconfig``: configuration options in this file will be shown under ``Component configuration`` in the ``menuconfig`` tool.
    - ``Kconfig.projbuild``: configuration options in this file will be shown in the top menu of the ``menuconfig`` tool.

Example:

.. code-block:: kconfig

    menu "Motors configuration"

        config SUBLIGHT_DRIVE_ENABLED
            bool "Enable sublight drive"
            default n
            depends on SPACE_SHIP
            help
                This option enables sublight on our spaceship.

    endmenu

.. note::

    **Visibility and dependencies**:

    In the example above, the ``SUBLIGHT_DRIVE_ENABLED`` configuration option has a dependency on the ``SPACE_SHIP`` configuration option. This option can origin from a different component. If the ``SPACE_SHIP`` option is not set or is not defined in the current configuration (e.g. the component containing this option was  not included in the project), the dependency will not be satisfied and the ``SUBLIGHT_DRIVE_ENABLED`` option will not be shown in the ``menuconfig`` tool.

    For more information about the visibility and dependencies, please refer to the `Kconfig Documentation <https://docs.espressif.com/projects/esp-idf-kconfig/en/latest/kconfiglib/language.html>`_.


.. _configuration-options-compatibility:

How to Ensure Backward Compatibility
------------------------------------

In general, renaming a Kconfig option of a component is a breaking API change, just like renaming a function is. ESP-IDF contains a mechanism which makes it possible to maintain backward compatibility when renaming configuration options. This mechanism is based on ``sdkconfig.rename`` files which include pairs of configuration option names. File structure is described below.

When renaming configuration options of a component, create the ``sdkconfig.rename`` file in the root folder of the component. Every line in this file should contain one of the following pairs:

* ``CONFIG_OLD_NAME CONFIG_NEW_NAME`` if the new option is a direct replacement of the old option.
* ``CONFIG_OLD_NAME !CONFIG_NEW_NAME`` if the new option is a Boolean inversion of the old option.

The project configuration tool (invoked by ``idf.py menuconfig``) will automatically find it and generate the compatibility statements in ``sdkconfig`` for the user.

For more information about the ``sdkconfig.rename`` file, please refer to the :ref:`sdkconfig.rename <sdkconfig-rename-file>` section of Configuration Structure.

Detailed explanation of the backward compatibility mechanism:
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. note::

    This part of the guide is intended to explain the behavior of the backward compatibility mechanism in ESP-IDF in greater detail. It is not necessary to understand this mechanism as it it done automatically, but it is described here for the sake of completeness.

If the user has set any value for the old config option (e.g. old config name is used in ``sdkconfig`` or ``sdkconfig.defaults``) without ``sdkconfig.rename`` file provided, this value would be **silently ignored**. This behavior is the default of the Kconfig system and is not a bug. In the original project (configuration of the linux kernel) this behavior was desired and is still desired in many projects.

This behavior is suppressed in ESP-IDF by the the configuration tool (invoked by ``idf.py menuconfig``). This tool generates compatibility statements for all the renamed options in the ``sdkconfig`` file. In more detail, the following approach is used to prevent the above mentioned situation:

1. Configuration tool searches the whole ESP-IDF folder for ``sdkconfig.rename`` files. If the project target (``<chip>``) matches the last suffix of any ``sdkconfig.rename.<chip>`` file, the file will be used in the next step as well.

2. After collecting all the relevant files, the ``sdkconfig`` file (and ``sdkconfig.h/json/cmake`` files if any) is post-processed. A block of compatibility statements for all the renamed options is added during the post-process to the end of the file(s). The block starts with ``# Deprecated options for backward compatibility`` and ends with ``# End of deprecated options``.
