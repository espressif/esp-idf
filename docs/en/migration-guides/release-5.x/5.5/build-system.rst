Build System
============

:link_to_translation:`zh_CN:[中文]`

Examples built with the MINIMAL_BUILD build property enabled
------------------------------------------------------------

Most of the examples in ESP-IDF are now being built with the ``MINIMAL_BUILD`` property enabled in their project ``CMakeLists.txt`` by using ``idf_build_set_property(MINIMAL_BUILD ON)``. This change has reduced the build time for the examples because only the ``main`` component and its transitive dependencies are included in the build. As a side effect, only the configuration for these components will be available in menuconfig, which is also noted in the ``Components config`` menu. With ``MINIMAL_BUILD`` enabled, a component needs to be added to the build as a ``main`` component dependency or as its transitive dependency to become visible in menuconfig and be compiled. For more information, please see :ref:`Including Components in the Build <including-components-in-the-build>`
