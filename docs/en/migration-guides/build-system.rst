Migrate Build System to ESP-IDF 5.0
===================================

Migrating from GNU Make build system
------------------------------------

Please follow the :ref:`build system <migrating_from_make>` guide for migrating make-based projects no longer supported in ESP-IDF v5.0.

Update fragment file grammar
----------------------------

Please follow the :ref:`migrate linker script fragment files grammar<ldgen-migrate-lf-grammar>` chapter for migrating v3.x grammar to the new one.

Specify component requirements explicitly
-----------------------------------------

In previous versions of ESP-IDF, some components were always added as public requirements (dependencies) to every component in the build, in addition to the :ref:`common requirements<component-common-requirements>`:

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

This means that it was possible to include header files of those components without specifying them as requirements in ``idf_component_register``.

This behavior was caused by transitive dependencies of various common components.

In ESP-IDF v5.0, this behavior is fixed and these components are no longer added as public requirements by default.

Every component depending on one of the components which isn't part of common requirements has to declare this dependency explicitly. This can be done by adding ``REQUIRES <component_name>`` or ``PRIV_REQUIRES <component_name>`` in ``idf_component_register`` call inside component's ``CMakeLists.txt``. See :ref:`Component Requirements` for more information on specifying requirements.

Setting ``COMPONENT_DIRS`` and ``EXTRA_COMPONENT_DIRS`` variables
-----------------------------------------------------------------

.. highlight:: cmake

ESP-IDF 5.0 includes a number of improvements to support building projects with space characters in their paths. To make that possible, there are some changes related to setting ``COMPONENT_DIRS`` and ``EXTRA_COMPONENT_DIRS`` variables in project CMakeLists.txt files.

Adding non-existent directories to ``COMPONENT_DIRS`` or ``EXTRA_COMPONENT_DIRS`` is no longer supported and will result in an error.

Using string concatenation to define ``COMPONENT_DIRS`` or ``EXTRA_COMPONENT_DIRS`` variables is now deprecated. These variables should be defined as CMake lists, instead. For example, use::

    set(EXTRA_COMPONENT_DIRS path1 path2)
    list(APPEND EXTRA_COMPONENT_DIRS path3)

instead of::

    set(EXTRA_COMPONENT_DIRS "path1 path2")
    set(EXTRA_COMPONENT_DIRS "${EXTRA_COMPONENT_DIRS} path3")

Defining these variables as CMake lists is compatible with previous IDF versions.

Update usage of target_link_libraries with project_elf
------------------------------------------------------

ESP-IDF v5.0 fixes issues with CMake variable propagation for components, which caused among others compiler flags and defines applied to one component to be applied to every component in the project.

As a side effect of this ESP-IDF user projects from ESP-IDF v5.0 onwards using ``target_link_libraries`` with ``project_elf`` explicitly and custom CMake projects must specify ``PRIVATE``, ``PUBLIC`` or ``INTERFACE`` arguments. This is a breaking change and is not backwards compatible with previous ESP-IDF versions.

For example::

    target_link_libraries(${project_elf} PRIVATE "-Wl,--wrap=esp_panic_handler")

instead of::

    target_link_libraries(${project_elf} "-Wl,--wrap=esp_panic_handler")
