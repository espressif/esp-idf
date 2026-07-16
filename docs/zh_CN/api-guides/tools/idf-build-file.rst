构建独立文件 - ``build-file``
*****************************

:link_to_translation:`en:[English]`

.. warning::

   此功能为实验性功能，可能在未来版本中发生变化。

``idf.py build-file`` 命令允许构建独立的 C 源文件，无需创建完整的 ESP-IDF 项目结构。适用于快速实验、小型示例和测试。

该命令会在缓存目录中自动生成容器项目，从源文件中可选的 YAML 前置配置中提取配置和依赖项，然后构建项目。

使用方法
========

.. code-block:: bash

    idf.py build-file <source_file>

该命令可以与其他 ``idf.py`` 命令组合使用，例如：

.. code-block:: bash

    idf.py build-file example.c flash monitor
    idf.py -p /dev/ttyUSB0 build-file example.c flash monitor

前置配置
========

源文件可以在块注释中包含可选的 YAML 前置配置。前置配置以 ``idf-build-file:`` 作为标记：

.. code-block:: c

    /*
     idf-build-file:
       config:
         - CONFIG_IDF_TARGET=esp32s3
         - CONFIG_SPIRAM=y
       dependencies:
         - log
         - vfs
         - "espressif/button>=4.0"
         - protocol_examples_common:
             path: ${IDF_PATH}/examples/common_components/protocol_examples_common
     */

    #include <stdio.h>
    #include "esp_log.h"

    void app_main(void)
    {
        ESP_LOGI("app", "Hello!");
    }

前置配置字段
------------

``config``
^^^^^^^^^^

``sdkconfig`` 选项列表。这些选项会写入生成的容器项目中的 ``sdkconfig.defaults``。字符串值（如 ``CONFIG_IDF_TARGET``）会自动添加引号。

.. code-block:: yaml

    config:
      - CONFIG_IDF_TARGET=esp32s3
      - CONFIG_SPIRAM=y

``compile_options``
^^^^^^^^^^^^^^^^^^^

编译源文件时使用的额外编译器选项列表，例如警告标志或预处理器定义。

.. code-block:: yaml

    compile_options:
      - -Wconversion
      - -DBUFFER_SIZE=1024

``dependencies``
^^^^^^^^^^^^^^^^

组件依赖列表。内置组件（不含 ``/`` 的纯名称）会作为 ``PRIV_REQUIRES`` 添加到生成的 CMakeLists 中。其他所有依赖项使用与 :doc:`idf_component.yml <idf-component-manager>` 相同的语法，添加到生成的 ``idf_component.yml`` 文件中。包括托管组件、基于路径的组件以及 IDF 组件管理器支持的其他形式。

.. code-block:: yaml

    dependencies:
      - log
      - vfs
      - "espressif/button>=4.0"
      - protocol_examples_common:
          path: ${IDF_PATH}/examples/common_components/protocol_examples_common

缓存
====

容器项目存储在系统临时目录中，重复构建同一文件时会复用缓存以实现快速增量构建。操作系统可能会在重启时自动清理这些文件。如需丢弃缓存的容器项目并强制完整重新构建，可运行 ``idf.py build-file --clean-cache file.c``。

示例
====

不含前置配置的最小示例：

.. code-block:: c

    #include <stdio.h>

    void app_main(void)
    {
        printf("Hello world!\n");
    }

构建并烧录：

.. code-block:: bash

    idf.py build-file hello.c flash monitor

使用 ``protocol_examples_common`` 连接 Wi-Fi 的完整示例：

.. code-block:: c

    /*
     idf-build-file:
       config:
         - CONFIG_IDF_TARGET=esp32s3
         - CONFIG_EXAMPLE_CONNECT_WIFI=y
         - CONFIG_EXAMPLE_WIFI_SSID=myssid
         - CONFIG_EXAMPLE_WIFI_PASSWORD=mypassword
       dependencies:
         - log
         - nvs_flash
         - esp_netif
         - esp_event
         - protocol_examples_common:
             path: ${IDF_PATH}/examples/common_components/protocol_examples_common
     */

    #include "esp_log.h"
    #include "esp_netif.h"
    #include "protocol_examples_common.h"
    #include "esp_event.h"
    #include "nvs_flash.h"

    static const char *TAG = "wifi_test";

    void app_main(void)
    {
        ESP_ERROR_CHECK(nvs_flash_init());
        ESP_ERROR_CHECK(esp_netif_init());
        ESP_ERROR_CHECK(esp_event_loop_create_default());
        ESP_ERROR_CHECK(example_connect());

        ESP_LOGI(TAG, "Connected to Wi-Fi!");
    }

.. code-block:: bash

    idf.py build-file wifi_test.c flash monitor
