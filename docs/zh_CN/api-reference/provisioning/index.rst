配网 API
****************

:link_to_translation:`en:[English]`

本文介绍了将设备配入 `Wi-Fi <../network/esp_wifi.rst>`_ 或 `Thread <../network/esp_openthread.rst>`_ 网络中的方法。

.. toctree::
    :maxdepth: 1

    protocomm
    provisioning

本部分的 API 示例代码存放在 idf-extra-compoenent 仓库的 `network_provisioning <https://github.com/espressif/idf-extra-components/tree/master/network_provisioning/examples>`_ 目录下。

.. only:: SOC_WIFI_SUPPORTED

    .. toctree::
        :maxdepth: 1

        ../network/esp_smartconfig

    本部分的 API 示例代码存放在 :example:`wifi/smart_config` 目录下。

    .. toctree::
        :maxdepth: 1

        ../network/esp_dpp

    本部分的 API 示例代码存放在 :example:`wifi/wifi_easy_connect/dpp-enrollee` 目录下。
