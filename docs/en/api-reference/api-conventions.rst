API Conventions
===============
:link_to_translation:`zh_CN:[中文]`

.. highlight:: c

This document describes conventions and assumptions common to ESP-IDF Application Programming Interfaces (APIs).

ESP-IDF provides several kinds of programming interfaces:

* C functions, structures, enums, type definitions, and preprocessor macros declared in public header files of ESP-IDF components. Various pages in the API Reference section of the programming guide contain descriptions of these functions, structures, and types.
* Build system functions, predefined variables, and options. These are documented in the :ref:`ESP-IDF CMake Build System API <cmake_buildsystem_api>`.
* :doc:`Kconfig <kconfig>` options can be used in code and in the build system (``CMakeLists.txt``) files.
* :doc:`Host tools <../api-guides/tools/index>` and their command line parameters are also part of the ESP-IDF interfaces.

ESP-IDF is made up of multiple components where these components either contain code specifically written for ESP chips, or contain a third-party library (i.e., a third-party component). In some cases, third-party components contain an "ESP-IDF specific" wrapper in order to provide an interface that is either simpler or better integrated with the rest of ESP-IDF's features. In other cases, third-party components present the original API of the underlying library directly.

The following sections explain some of the aspects of ESP-IDF APIs and their usage.

Error Handling
--------------

Most ESP-IDF APIs return error codes defined with the :cpp:type:`esp_err_t` type. See :doc:`Error Handling <../api-guides/error-handling>` section for more information about error handling approaches. :doc:`Error Codes Reference <error-codes>` contains the list of error codes returned by ESP-IDF components.

.. _api_reference_config_structures:

Configuration Structures
------------------------

.. important:: Correct initialization of configuration structures is an important part of making the application compatible with future versions of ESP-IDF.

Most initialization, configuration, and installation functions in ESP-IDF (typically named ``..._init()``, ``..._config()``, and ``..._install()``) take a configuration structure pointer as an argument. For example::

    const esp_timer_create_args_t my_timer_args = {
        .callback = &my_timer_callback,
        .arg = callback_arg,
        .name = "my_timer"
    };
    esp_timer_handle_t my_timer;
    esp_err_t err = esp_timer_create(&my_timer_args, &my_timer);

These functions never store the pointer to the configuration structure, so it is safe to allocate the structure on the stack.

The application must initialize all fields of the structure. The following is incorrect::

    esp_timer_create_args_t my_timer_args;
    my_timer_args.callback = &my_timer_callback;
    /* Incorrect! Fields .arg and .name are not initialized */
    esp_timer_create(&my_timer_args, &my_timer);

Most ESP-IDF examples use C99 `designated initializers`_ for structure initialization since they provide a concise way of setting a subset of fields, and zero-initializing the remaining fields::

    const esp_timer_create_args_t my_timer_args = {
        .callback = &my_timer_callback,
        /* Correct, fields .arg and .name are zero-initialized */
    };

The C++ language supports designated initializer syntax, too, but the initializers must be in the order of declaration. When using ESP-IDF APIs in C++ code, you may consider using the following pattern::

    /* Correct, fields .dispatch_method, .name and .skip_unhandled_events are zero-initialized */
    const esp_timer_create_args_t my_timer_args = {
        .callback = &my_timer_callback,
        .arg = &my_arg,
    };

    ///* Incorrect, .arg is declared after .callback in esp_timer_create_args_t */
    //const esp_timer_create_args_t my_timer_args = {
    //    .arg = &my_arg,
    //    .callback = &my_timer_callback,
    //};

For more information on designated initializers, see :ref:`cplusplus_designated_initializers`. Note that C++ language versions older than C++20, which are not the default in the current version of ESP-IDF, do not support designated initializers. If you have to compile code with an older C++ standard than C++20, you may use GCC extensions to produce the following pattern::

    esp_timer_create_args_t my_timer_args = {};
    /* All the fields are zero-initialized */
    my_timer_args.callback = &my_timer_callback;

Default Initializers
^^^^^^^^^^^^^^^^^^^^

For some configuration structures, ESP-IDF provides macros for setting default values of fields::

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    /* HTTPD_DEFAULT_CONFIG expands to a designated initializer. Now all fields are set to the default values, and any field can still be modified: */
    config.server_port = 8081;
    httpd_handle_t server;
    esp_err_t err = httpd_start(&server, &config);

It is recommended to use default initializer macros whenever they are provided for a particular configuration structure.

.. _api_reference_private_apis:

Private APIs
------------

Certain header files in ESP-IDF contain APIs intended to be used only in ESP-IDF source code rather than by the applications. Such header files often contain ``private`` or ``esp_private`` in their name or path. Certain components, such as :doc:`hal <../api-guides/hardware-abstraction>` only contain private APIs.

Private APIs may be removed or changed in an incompatible way between minor or patch releases.

.. _api_reference_example_components:

Components in Example Projects
------------------------------

ESP-IDF examples contain a variety of projects demonstrating the usage of ESP-IDF APIs. In order to reduce code duplication in the examples, a few common helpers are defined inside components that are used by multiple examples. This includes components located in :example:`common_components` directory, as well as some of the components located in the examples themselves. These components are not considered to be part of the ESP-IDF API.

It is not recommended to reference these components directly in custom projects (via ``EXTRA_COMPONENT_DIRS`` build system variable), as they may change significantly between ESP-IDF versions. When starting a new project based on an ESP-IDF example, copy both the project and the common components it depends on out of ESP-IDF, and treat the common components as part of the project. Note that the common components are written with examples in mind, and might not include all the error handling required for production applications. Before using, take time to read the code and understand if it is applicable to your use case.

API Stability
-------------

ESP-IDF uses `Semantic Versioning <https://semver.org/>`_ as explained in the :ref:`Versioning Scheme <versioning-scheme>`.

Minor and bugfix releases of ESP-IDF guarantee compatibility with previous releases. The sections below explain different aspects and limitations to compatibility.

Source-level Compatibility
^^^^^^^^^^^^^^^^^^^^^^^^^^

ESP-IDF guarantees source-level compatibility of C functions, structures, enums, type definitions, and preprocessor macros declared in public header files of ESP-IDF components. Source-level compatibility implies that the application source code can be recompiled with the newer version of ESP-IDF without changes.

The following changes are allowed between minor versions and do not break source-level compatibility:

* Deprecating functions (using the ``deprecated`` attribute) and header files (using a preprocessor ``#warning``). Deprecations are listed in ESP-IDF release notes. It is recommended to update the source code to use the newer functions or files that replace the deprecated ones, however, this is not mandatory. Deprecated functions and files can be removed from major versions of ESP-IDF.
* Renaming components, moving source and header files between components — provided that the build system ensures that correct files are still found.
* Renaming Kconfig options. Kconfig system's :ref:`backward compatibility <configuration-options-compatibility>` ensures that the original Kconfig option names can still be used by the application in ``sdkconfig`` file, CMake files, and source code.

Lack of Binary Compatibility
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ESP-IDF does not guarantee binary compatibility between releases. This means that if a precompiled library is built with one ESP-IDF version, it is not guaranteed to work the same way with the next minor or bugfix release. The following are the possible changes that keep source-level compatibility but not binary compatibility:

* Changing numerical values for C enum members.
* Adding new structure members or changing the order of members. See :ref:`api_reference_config_structures` for tips that help ensure compatibility.
* Replacing an ``extern`` function with a ``static inline`` one with the same signature, or vice versa.
* Replacing a function-like macro with a compatible C function.

Other Exceptions from Compatibility
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

While we try to make upgrading to a new ESP-IDF version easy, there are parts of ESP-IDF that may change between minor versions in an incompatible way. We appreciate issuing reports about any unintended breaking changes that do not fall into the categories below.

* :ref:`api_reference_private_apis`.
* :ref:`api_reference_example_components`.
* Features clearly marked as "beta", "preview", or "experimental".
* Changes made to mitigate security issues or to replace insecure default behaviors with secure ones.
* Features that were never functional. For example, if it was never possible to use a certain function or an enumeration value, it may get renamed (as part of fixing it) or removed. This includes software features that depend on non-functional chip hardware features.
* Unexpected or undefined behavior that is not documented explicitly may be fixed/changed, such as due to missing validation of argument ranges.
* Location of :doc:`Kconfig <kconfig>` options in menuconfig.
* Location and names of example projects.

.. _designated initializers: https://en.cppreference.com/w/c/language/struct_initialization
