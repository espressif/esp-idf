.. include:: ../../../components/log/README.rst

Application Example
-------------------

Log library is commonly used by most of esp-idf components and examples. For demonstration of log functionality check `examples <https://github.com/espressif/esp-idf/tree/master/examples>`_ folder of `espressif/esp-idf <https://github.com/espressif/esp-idf>`_ repository, that among others, contains the following examples:
 
* `system/ota <https://github.com/espressif/esp-idf/tree/master/examples/system/ota>`_ 
* `storage/sd_card <https://github.com/espressif/esp-idf/tree/master/examples/storage/sd_card>`_ 
* `protocols/https_request <https://github.com/espressif/esp-idf/tree/master/examples/protocols/https_request>`_ 

API Reference
-------------

Header Files
^^^^^^^^^^^^

  * `log/include/esp_log.h <https://github.com/espressif/esp-idf/blob/master/components/log/include/esp_log.h>`_

Macros
^^^^^^

.. doxygendefine:: LOG_COLOR_E
.. doxygendefine:: LOG_COLOR_W
.. doxygendefine:: LOG_COLOR_I
.. doxygendefine:: LOG_COLOR_D
.. doxygendefine:: LOG_COLOR_V
.. doxygendefine:: LOG_RESET_COLOR
.. doxygendefine:: LOG_FORMAT
.. doxygendefine:: LOG_LOCAL_LEVEL
.. doxygendefine:: ESP_EARLY_LOGE
.. doxygendefine:: ESP_EARLY_LOGW
.. doxygendefine:: ESP_EARLY_LOGI
.. doxygendefine:: ESP_EARLY_LOGD
.. doxygendefine:: ESP_EARLY_LOGV
.. doxygendefine:: ESP_LOGE
.. doxygendefine:: ESP_LOGW
.. doxygendefine:: ESP_LOGI
.. doxygendefine:: ESP_LOGD
.. doxygendefine:: ESP_LOGV

Type Definitions
^^^^^^^^^^^^^^^^

.. doxygentypedef:: vprintf_like_t

Enumerations
^^^^^^^^^^^^

.. doxygenenum:: esp_log_level_t

Functions
^^^^^^^^^

.. doxygenfunction:: esp_log_level_set
.. doxygenfunction:: esp_log_set_vprintf
.. doxygenfunction:: esp_log_timestamp
.. doxygenfunction:: esp_log_write










