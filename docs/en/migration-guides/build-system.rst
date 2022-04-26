Migrate Build System to ESP-IDF 5.0
===================================

Migrating from make to cmake
----------------------------

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

This means that it was possible to include header files of those components without specifying them as requirements in ``idf_component_register``.

This behavior was caused by transitive dependencies of various common components.

In ESP-IDF v5.0, this behavior is fixed and these components are no longer added as public requirements by default.

Every component depending on one of the components which isn't part of common requirements has to declare this dependency explicitly. This can be done by adding ``REQUIRES <component_name>`` or ``PRIV_REQUIRES <component_name>`` in ``idf_component_register`` call inside component's ``CMakeLists.txt``. See :ref:`Component Requirements` for more information on specifying requirements.
