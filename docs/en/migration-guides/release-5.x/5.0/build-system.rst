Build System
============

Migrating from GNU Make Build System
------------------------------------

ESP-IDF v5.0 no longer supports GNU make-based projects. Please follow the :ref:`build system <migrating_from_make>` guide for migration.

Update Fragment File Grammar
----------------------------

The former grammar, supported in ESP-IDF v3.x, was dropped in ESP-IDF v5.0. Here are a few notes on how to migrate properly:

1. Indentation is now enforced: improperly indented fragment files generate a runtime parse exception. Although the former version did not enforce this, the previous documentation and examples demonstrated properly indented grammar.
2. Migrate the old condition entry to the ``if...elif...else`` structure for conditionals. You can refer to the :ref:`ldgen-conditional-placements` for detailed grammar.
3. Mapping fragments now requires a name like other fragment types.


Specify Component Requirements Explicitly
-----------------------------------------

In previous versions of ESP-IDF, some components were always added as public requirements (dependencies) to every component in the build, in addition to the :ref:`common component requirements <component-common-requirements>`:

* ``driver``
* ``efuse``
* ``esp_timer``
* ``lwip``
* ``vfs``
* ``esp_wifi``
* ``esp_event``
* ``esp_netif``
* ``esp_eth``
* ``esp_phy``

This means that it was possible to include header files of those components without specifying them as requirements in ``idf_component_register``. This behavior was caused by transitive dependencies of various common components.

In ESP-IDF v5.0, this behavior is fixed and these components are no longer added as public requirements by default.

Every component depending on one of the components which isn't part of common requirements has to declare this dependency explicitly. This can be done by adding ``REQUIRES <component_name>`` or ``PRIV_REQUIRES <component_name>`` in ``idf_component_register`` call inside component's ``CMakeLists.txt``. See :ref:`Component Requirements <component-requirements>` for more information on specifying requirements.

Setting ``COMPONENT_DIRS`` and ``EXTRA_COMPONENT_DIRS`` Variables
-----------------------------------------------------------------

.. highlight:: cmake

ESP-IDF v5.0 includes a number of improvements to support building projects with space characters in their paths. To make that possible, there are some changes related to setting ``COMPONENT_DIRS`` and ``EXTRA_COMPONENT_DIRS`` variables in project CMakeLists.txt files.

Adding non-existent directories to ``COMPONENT_DIRS`` or ``EXTRA_COMPONENT_DIRS`` is no longer supported and will result in an error.

Using string concatenation to define ``COMPONENT_DIRS`` or ``EXTRA_COMPONENT_DIRS`` variables is now deprecated. These variables should be defined as CMake lists, instead. For example, use::

    set(EXTRA_COMPONENT_DIRS path1 path2)
    list(APPEND EXTRA_COMPONENT_DIRS path3)

instead of::

    set(EXTRA_COMPONENT_DIRS "path1 path2")
    set(EXTRA_COMPONENT_DIRS "${EXTRA_COMPONENT_DIRS} path3")

Defining these variables as CMake lists is compatible with previous ESP-IDF versions.

Update Usage of ``target_link_libraries`` with ``project_elf``
--------------------------------------------------------------

ESP-IDF v5.0 fixes CMake variable propagation issues for components. This issue caused compiler flags and definitions that were supposed to apply to one component to be applied to every component in the project.

As a side effect of this, user projects from ESP-IDF v5.0 onwards must use ``target_link_libraries`` with ``project_elf`` explicitly and custom CMake projects must specify ``PRIVATE``, ``PUBLIC``, or ``INTERFACE`` arguments. This is a breaking change and is not backward compatible with previous ESP-IDF versions.

For example::

    target_link_libraries(${project_elf} PRIVATE "-Wl,--wrap=esp_panic_handler")

instead of::

    target_link_libraries(${project_elf} "-Wl,--wrap=esp_panic_handler")

Update CMake Version
-----------------------

In ESP-IDF v5.0 minimal CMake version was increased to 3.16 and versions lower than 3.16 are not supported anymore.  Run ``tools/idf_tools.py install cmake`` to install a suitable version if your OS version doesn't have one.

This affects ESP-IDF users who use system-provided CMake and custom CMake.

Reorder the Applying of the Target-Specific Config Files
-----------------------------------------------------------------------------------------------

.. highlight:: none

ESP-IDF v5.0 reorders the applying order of target-specific config files and other files listed in SDKCONFIG_DEFAULTS. Now, target-specific files will be applied right after the file brings it in, before all latter files in ``SDKCONFIG_DEFAULTS``.

For example::

    If ``SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig_devkit1"``, and there is a file ``sdkconfig.defaults.esp32`` in the same folder, then the files will be applied in the following order: (1) sdkconfig.defaults (2) sdkconfig.defaults.esp32 (3) sdkconfig_devkit1.

If you have a key with different values in the target-specific files of the former item (e.g., ``sdkconfig.defaults.esp32`` above) and the latter item (e.g., ``sdkconfig_devkit1`` above), please note the latter will override the target-specific file of the former.

If you do want to have some target-specific config values, please put it into the target-specific file of the latter item (e.g., ``sdkconfig_devkit1.esp32``).
