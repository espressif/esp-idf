Build Standalone Files - ``build-file``
***************************************

:link_to_translation:`zh_CN:[中文]`

.. warning::

   This feature is experimental and may change in future releases.

The ``idf.py build-file`` command allows building standalone C source files without creating a full ESP-IDF project structure. It is useful for quick experiments, small examples, and tests.

Behind the scenes, the command auto-generates a container project in a cache directory, extracts configuration and dependencies from an optional YAML frontmatter in the source file, and builds the project.

Usage
=====

.. code-block:: bash

    idf.py build-file <source_file>

The command is composable with other ``idf.py`` commands. For example:

.. code-block:: bash

    idf.py build-file example.c flash monitor
    idf.py -p /dev/ttyUSB0 build-file example.c flash monitor

Frontmatter
===========

Source files may contain an optional YAML frontmatter inside a block comment. The frontmatter starts with ``idf-build-file:`` as a marker:

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

Frontmatter Fields
------------------

``config``
^^^^^^^^^^

A list of ``sdkconfig`` options. These are written to ``sdkconfig.defaults`` in the generated container project. String values (like ``CONFIG_IDF_TARGET``) are automatically quoted.

.. code-block:: yaml

    config:
      - CONFIG_IDF_TARGET=esp32s3
      - CONFIG_SPIRAM=y

``dependencies``
^^^^^^^^^^^^^^^^

A list of component dependencies. Built-in components (plain names without ``/``) are added as ``PRIV_REQUIRES`` in the generated CMakeLists. All other dependencies use the same syntax as :doc:`idf_component.yml <idf-component-manager>` and are added to the generated ``idf_component.yml`` file. This includes managed components, path-based components, and any other forms supported by the IDF Component Manager.

.. code-block:: yaml

    dependencies:
      - log
      - vfs
      - "espressif/button>=4.0"
      - protocol_examples_common:
          path: ${IDF_PATH}/examples/common_components/protocol_examples_common

Caching
=======

Container projects are stored in the system temporary directory and reused for fast incremental builds. The OS may clean these up automatically on reboot. To discard the cached container project and force a full rebuild, run ``idf.py build-file --clean-cache file.c``.

Examples
========

A minimal example with no frontmatter:

.. code-block:: c

    #include <stdio.h>

    void app_main(void)
    {
        printf("Hello world!\n");
    }

Build and flash:

.. code-block:: bash

    idf.py build-file hello.c flash monitor

A more complete example that connects to Wi-Fi using ``protocol_examples_common``:

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
