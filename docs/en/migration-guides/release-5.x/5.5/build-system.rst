Build System
============

:link_to_translation:`zh_CN:[中文]`

Examples Built with MINIMAL_BUILD Property Enabled
--------------------------------------------------

Most of the examples in ESP-IDF are now being built with the ``MINIMAL_BUILD`` property enabled in their project ``CMakeLists.txt`` by using ``idf_build_set_property(MINIMAL_BUILD ON)``. This reduces the build time by including only the ``main`` component and its transitive dependencies.

As a side effect, only these components appear in menuconfig, as noted in the ``Components config`` menu. With ``MINIMAL_BUILD`` enabled, to make other components visible and compiled, add them as dependencies of the ``main`` component dependency or as its transitive dependencies. For more information, please see :ref:`Including Components in the Build <including-components-in-the-build>`.
