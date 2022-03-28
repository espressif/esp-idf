Migrate Build System to ESP-IDF 5.0
===================================

Migrating from make to cmake
----------------------------

Please follow the :ref:`build system <migrating_from_make>` guide for migrating make-based projects no longer supported in ESP-IDF v5.0.

Update fragment file grammar
----------------------------

Please follow the :ref:`migrate linker script fragment files grammar<ldgen-migrate-lf-grammar>` chapter for migrating v3.x grammar to the new one.

Component dependencies that shall be explicit
---------------------------------------------

In previous versions of ESP-IDF, target components (``components/esp32*``) had a dependency on ``driver`` and ``efuse`` components. Since target components were part of common requirements (:ref:`more info about common requirements <component-common-requirements>`), all components in the project implicitly had a dependency on ``driver`` and ``efuse``. Now that the dependency of target components on these components has been removed, every component depending on ``driver`` or ``efuse`` has to declare this dependency explicitly. This can be done by adding ``REQUIRES <component_name>`` or ``PRIV_REQUIRES <component_name>`` in ``idf_component_register`` call inside component's ``CMakeLists.txt``. See :ref:`Component Requirements` for more information on specifying requirements.
