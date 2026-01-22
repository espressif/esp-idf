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

.. _how-values-are-assigned-to-configuration-options:

How Values are Assigned to Configuration Options
------------------------------------------------

Configuration process automatically decides which value a given configuration option should have and saves it to the ``sdkconfig`` file (if the ``sdkconfig`` file does not exist, it is created, otherwise it is updated). This process is run during the ``idf.py build``, ``idf.py reconfigure``, ``idf.py menuconfig`` or ``idf.py set-target`` commands. The ``idf.py set-target`` command deletes previous ``sdkconfig`` (if any) and will always create a new one.

After ``sdkconfig`` file is created/updated, it contains **all currently visible configuration options** together with values currently assigned to them. It is important to mention that once the ``sdkconfig`` file is created, all the configuration options that are contained in it are **locked**, meaning their values are not changed automatically, and can only be changed manually via `idf.py menuconfig`. Read further for in-depth explanation or skip to the :ref:`workarounds-and-best-practices` section.

There are three sources a configuration option can get its value from, where later sources may override the previous ones:

1. ``Kconfig`` file. This is the source of **default values** for configuration options. Every configuration option should have its default value(s) defined as a part of its definition. Default values can be **conditioned**. This means that given default value will be used only if its condition holds. For more information how to define default values, please refer to the `default option description in Kconfig Documentation <https://docs.espressif.com/projects/esp-idf-kconfig/en/release-v2/kconfiglib/language.html#the-default-option>`_.
2. ``sdkconfig.defaults`` file. If it exists, configuration options that have a value assigned in this file will be assigned this value, ignoring the default value (the one obtained from the ``Kconfig`` file). Configuration options **not set** in this file **are still free to change** based on the conditions of their default values!
3. ``sdkconfig`` file. If it exists, configuration options that have a value assigned in this file will be assigned this value, ignoring the one obtained from the ``Kconfig`` or ``sdkconfig.defaults`` file (or caused by default value change during ``sdkconfig.defaults`` file processing).

As it was mentioned, ``sdkconfig`` file is created as a result of the configuration process and it contains **all currently visible configuration options** together with values currently assigned to them. This has a very important consequence: once ``sdkconfig`` file is created, conditional default values from ``Kconfig`` files **won't be reevaluated**, even if given configuration option was not set by the user. See an example:

.. code-block:: kconfig

    config FEATURE_X
        bool "Enable feature X"
        default n

    config MODE
        string "Mode"
        default "fast" if FEATURE_X
        default "safe"

Assume ``sdkconfig`` does not exist yet (or was deleted) and you run ``idf.py menuconfig``:

1. According to the ``Kconfig`` file, ``FEATURE_X`` is ``n`` by default, so ``MODE`` gets the conditional default ``"safe"``.
2. During the configuration process, ``sdkconfig`` file is created and now contains ``CONFIG_MODE="safe"`` explicitly.
3. Menuconfig is entered.

Because ``sdkconfig`` file was created before entering menuconfig, if you enable ``FEATURE_X``, the ``MODE``'s conditional ``default "fast" if FEATURE_X`` statement won't be applied because ``CONFIG_MODE="safe"`` is already set in ``sdkconfig`` and overrides any other value set in previous steps.

This is a known behavior of the Kconfig system and is not a bug, although it may seem somewhat unintuitive at first glance.

.. _workarounds-and-best-practices:

Workarounds and Best Practices
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

How to work around this behavior depends on the exact situation:

1. **Values need to be set only in specific situations**: Instruct the user to set all the necessary values in menuconfig and do not rely on conditional defaults from ``Kconfig`` files. This solution requires users to do some manual work and if users delete their ``sdkconfig`` file, they would need to set the values again.

2. **Values need to be set often**: You can instruct users to create a ``sdkconfig.defaults`` in their project root folder and set these values there. This file is described in the :ref:`sdkconfig.defaults <sdkconfig-defaults-file>` section of Configuration Structure. It is important to mention that **components themselves cannot provide these files** in a way that would be automatically picked up by the build system. Instead, users need to be instructed to create this file inside their project and set the values there. This solution requires users to do some manual work, but it is more convenient than the previous one. After the ``sdkconfig.defaults`` file is created, it is necessary to **delete the ``sdkconfig`` file** and run ``idf.py menuconfig`` again to pick up the new values.

Thanks to the way how configuration system works, having necessary values set in ``sdkconfig.defaults`` file will also ensure that conditional default values from ``Kconfig`` files **are reevaluated** for configuration options that were not set there.

See the following example:

.. code-block:: kconfig

    config FEATURE_X
        bool "Enable feature X"
        default n

    config MODE
        string "Mode"
        default "fast" if FEATURE_X
        default "safe"

``sdkconfig.defaults`` (placed in the project root folder):

.. code-block:: text

    CONFIG_FEATURE_X=y

Assume ``sdkconfig`` does not exist yet (or was deleted) and you run ``idf.py menuconfig``:

1. Default values from ``Kconfig`` file are used. ``FEATURE_X`` is ``n`` by default, so ``MODE`` gets the conditional default ``"safe"``.
2. ``sdkconfig.defaults`` file is processed. ``FEATURE_X`` is explicitly set to ``y`` there. However, ``MODE``'s default value **will be reevaluated** (``MODE`` is not explicitly set in ``sdkconfig.defaults`` file and ``sdkconfig`` file does not exist yet). Because ``FEATURE_X`` is now set to ``y`` and the conditional default ``default "fast" if FEATURE_X`` applies, ``MODE`` will get the conditional default ``"fast"``.
3. ``sdkconfig`` file is created and contains ``CONFIG_MODE="fast"`` explicitly. This will once again **lock the value** of ``MODE`` configuration option, this time to ``"fast"``.

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
