错误代码和辅助函数
================================

:link_to_translation:`en:[English]`

本节列出了 ESP-IDF 中常见错误代码的定义，以及部分与错误处理相关的辅助函数。

有关 ESP-IDF 中错误代码的基本信息，请参阅 :doc:`错误处理 <../../api-guides/error-handling>`。

有关 ESP-IDF 定义的错误代码的完整列表，请参阅 :doc:`错误代码参考 <../error-codes>`。

.. _registering-error-codes:

注册错误码
----------

ESP-IDF 使用可组合的错误码注册系统，在构建时自动从所有组件中收集错误码定义。这使得 :cpp:func:`esp_err_to_name` 和 :cpp:func:`esp_err_to_name_r` 能够查找整个项目中定义的错误码，而无需手动维护中央注册表。

工作原理
^^^^^^^^

错误码在链接时通过名为 ``.esp_err_msg_tbl`` 的链接器段进行注册。在组件的头文件中定义错误码后，需要将这些头文件注册到构建系统中，构建系统将会：

1. 在构建过程中提取错误码定义
2. 生成 C 代码，将错误码放入 ``.esp_err_msg_tbl`` 段
3. 自动将生成的 C 代码链接到应用程序中

链接器会将所有组件中的错误码条目收集到一个数组中，供 :cpp:func:`esp_err_to_name` 在运行时查找。

注册组件的错误码
^^^^^^^^^^^^^^^^

要注册组件中的错误码，请在组件的 ``CMakeLists.txt`` 中添加以下内容：

.. code-block:: cmake

    idf_define_esp_err_codes(HEADERS include/my_component.h)

将 ``include/my_component.h`` 替换为包含错误码定义的头文件路径。可以指定多个头文件：

.. code-block:: cmake

    idf_define_esp_err_codes(HEADERS
        include/my_api.h
        include/my_driver.h
    )

构建系统将自动处理这些头文件，并注册所有符合错误码命名规则的 ``#define`` 指令（通常以 ``ESP_ERR_`` 或类似前缀开头）。

示例
^^^^

在组件的头文件 ``include/my_component.h`` 中：

.. code-block:: c

    #pragma once

    #include "esp_err.h"

    #define ESP_ERR_MY_COMPONENT_BASE    0x7000
    #define ESP_ERR_MY_COMPONENT_INIT    (ESP_ERR_MY_COMPONENT_BASE + 1)  /*!< Component initialization failed */
    #define ESP_ERR_MY_COMPONENT_BUSY    (ESP_ERR_MY_COMPONENT_BASE + 2)  /*!< Component is busy */

在组件的 ``CMakeLists.txt`` 中：

.. code-block:: cmake

    idf_component_register(SRCS "my_component.c"
                          INCLUDE_DIRS "include"
                          PRIV_REQUIRES esp_common)

    # Register error codes
    idf_define_esp_err_codes(HEADERS include/my_component.h)

构建项目后，调用 ``esp_err_to_name(ESP_ERR_MY_COMPONENT_INIT)`` 将返回字符串 ``"ESP_ERR_MY_COMPONENT_INIT"``。

.. note::

    大多数 ESP-IDF 组件的错误码已经完成注册，只有在为自定义组件注册错误码，或向现有组件添加新错误码时，才需要添加 ``idf_define_esp_err_codes()``。

.. _esp-check-api-ref:

API 参考
-------------

.. include-build-file:: inc/esp_check.inc
.. include-build-file:: inc/esp_err.inc
