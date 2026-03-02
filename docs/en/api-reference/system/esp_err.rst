Error Code and Helper Functions
================================

:link_to_translation:`zh_CN:[中文]`

This section lists definitions of common ESP-IDF error codes and several helper functions related to error handling.

For general information about error codes in ESP-IDF, see :doc:`Error Handling <../../api-guides/error-handling>`.

For the full list of error codes defined in ESP-IDF, see :doc:`Error Codes Reference <../error-codes>`.

.. _registering-error-codes:

Registering Error Codes
------------------------

ESP-IDF uses a composable error code registration system that automatically collects error code definitions from all components at build time. This allows :cpp:func:`esp_err_to_name` and :cpp:func:`esp_err_to_name_r` to look up error codes defined across the entire project without requiring manual maintenance of a central registry.

How It Works
^^^^^^^^^^^^

Error codes are registered at link time using a linker section called ``.esp_err_msg_tbl``. When you define error codes in your component's header files, you need to register those headers with the build system, which will:

1. Extract the error code definitions during the build
2. Generate C code that places them into the ``.esp_err_msg_tbl`` section
3. Link them into your application automatically

The linker collects all error code entries from all components into a single array that :cpp:func:`esp_err_to_name` searches at runtime.

Registering Your Component's Error Codes
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To register error codes from your component, add the following line to your component's ``CMakeLists.txt``:

.. code-block:: cmake

    idf_define_esp_err_codes(HEADERS include/my_component.h)

Replace ``include/my_component.h`` with the path to your header file(s) that contain error code definitions. You can specify multiple headers:

.. code-block:: cmake

    idf_define_esp_err_codes(HEADERS
        include/my_api.h
        include/my_driver.h
    )

The build system will automatically process these headers and register any ``#define`` directives that match the error code pattern (typically starting with ``ESP_ERR_`` or similar).

Example
^^^^^^^

In your component's header file ``include/my_component.h``:

.. code-block:: c

    #pragma once

    #include "esp_err.h"

    #define ESP_ERR_MY_COMPONENT_BASE    0x7000
    #define ESP_ERR_MY_COMPONENT_INIT    (ESP_ERR_MY_COMPONENT_BASE + 1)  /*!< Component initialization failed */
    #define ESP_ERR_MY_COMPONENT_BUSY    (ESP_ERR_MY_COMPONENT_BASE + 2)  /*!< Component is busy */

In your component's ``CMakeLists.txt``:

.. code-block:: cmake

    idf_component_register(SRCS "my_component.c"
                          INCLUDE_DIRS "include"
                          PRIV_REQUIRES esp_common)

    # Register error codes
    idf_define_esp_err_codes(HEADERS include/my_component.h)

After building your project, calls to ``esp_err_to_name(ESP_ERR_MY_COMPONENT_INIT)`` will return the string ``"ESP_ERR_MY_COMPONENT_INIT"``.

.. note::

    Most ESP-IDF components already have their error codes registered. You only need to add ``idf_define_esp_err_codes()`` for your own custom components or when adding new error codes to existing components.

.. _esp-check-api-ref:

API Reference
-------------

.. include-build-file:: inc/esp_check.inc
.. include-build-file:: inc/esp_err.inc
