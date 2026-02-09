Provisioning API
****************

:link_to_translation:`zh_CN:[中文]`

This document describes methods to connect a device to `Wi-Fi <../network/esp_wifi.rst>`_ or `Thread <../network/esp_openthread.rst>`_ network.

.. toctree::
    :maxdepth: 1

    protocomm
    provisioning

Code examples for above API are provided in the `network_provisioning <https://github.com/espressif/idf-extra-components/tree/master/network_provisioning/examples>`_ component.

.. only:: SOC_WIFI_SUPPORTED

    .. toctree::
        :maxdepth: 1

        ../network/esp_smartconfig

    Code example for above API is provided in :example:`wifi/smart_config`.

    .. toctree::
        :maxdepth: 1

        ../network/esp_dpp

    Code example for above API is provided in :example:`wifi/wifi_easy_connect/dpp-enrollee`.
