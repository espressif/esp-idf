Managing Component Backward Compatibility
*****************************************

A component that must build under both v1 and v2 can use the :cmakev2:ref:`IDF_BUILD_V2` variable to run different CMake code under each build system. ``IDF_BUILD_V2`` is set when the component is evaluated under v2. You can use it to adjust small parts of an existing v1 component, or to include a completely separate ``CMakeLists.txt`` for v2. For the specific differences between v1 and v2 that may require this, see :doc:`breaking-changes`.

Below is a component, ``my_component``, whose ``CMakeLists.txt`` is adjusted to include and evaluate a separate v2 file, ``CMakeLists_v2.txt``, when it is evaluated under v2.

.. note::

    Most v1 components should work without modification under v2. This is only a simple, illustrative example of how ``IDF_BUILD_V2`` can be used to evaluate different ``CMakeLists.txt`` files for v1 and v2.

Adjusted ``CMakeLists.txt``:

.. code-block:: cmake

    if(IDF_BUILD_V2)
        # Include component CMake code for v2 and return.
        include(CMakeLists_v2.txt)
        return()
    endif()

    # Here follows the original component CMake code for v1.
    idf_component_register(SRCS "my_component.c"
                           PRIV_REQUIRES spi_flash
                           INCLUDE_DIRS "")

The ``CMakeLists_v2.txt`` for v2:

.. code-block:: cmake

    idf_component_include(spi_flash)

    add_library(${COMPONENT_TARGET} STATIC
        "my_component.c"
    )

    target_include_directories(${COMPONENT_TARGET} PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}"
    )

    target_link_libraries(${COMPONENT_TARGET} PRIVATE
        idf::spi_flash
    )

For more on writing a native v2 component, see :ref:`cmakev2-component-native`.
