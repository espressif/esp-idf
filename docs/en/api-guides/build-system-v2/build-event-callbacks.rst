.. _cmakev2-build-event-callbacks:

Using Build Event Callbacks
***************************

A component can register a callback that the build system invokes at a specific point in the build. This is the v2 way to run a custom step on the linked application, for example running a tool on the executable, without relying on internal build targets or properties.

Registering a Callback
======================

A component registers a callback in its ``project_include.cmake`` with :cmakev2:ref:`idf_component_register_build_event_callback`. The callback must be a CMake function defined in the same file. At the specified event, the build system invokes the callback and passes the relevant CMake target as the first argument.

.. code-block:: cmake
    :caption: project_include.cmake

    function(my_post_elf_hook target)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND my_tool "$<TARGET_FILE:${target}>"
            COMMENT "Running my_tool on the executable")
    endfunction()

    idf_component_register_build_event_callback(EVENT POST_ELF CALLBACK my_post_elf_hook)

Supported Events
================

``POST_ELF``
    Fired after the executable target is created and linked, but before the binary (``.bin``) image is generated. The callback receives the executable target name. Use it to act on the ELF, for example by attaching a ``POST_BUILD`` command to the executable with ``add_custom_command(TARGET ... POST_BUILD ...)``, or to add custom targets that depend on the executable.

``POST_ELF`` is currently the only supported event. Additional events may be added in the future when required.

In v1, this kind of post-link step used ``idf_build_add_post_elf_dependency``, which is not available in v2; see :ref:`cmakev2-post-elf-unavailable`.
