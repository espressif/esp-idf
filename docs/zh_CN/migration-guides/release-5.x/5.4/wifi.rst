Wi-Fi
=====

:link_to_translation:`en:[English]`


Wi-Fi 扫描和连接
------------------------

    以下类型已被更改：

    - :component_file:`esp_wifi/include/esp_wifi_he_types.h`

        - :cpp:struct:`esp_wifi_htc_omc_t` 中:

            - ``uph_id``， ``ul_pw_headroom``， ``min_tx_pw_flag`` 字段被弃用

    - :component_file:`esp_wifi/include/esp_wifi_types_generic.h`

        - :cpp:struct:`wifi_ap_record_t` 中:

            - ``bandwidth`` 的类型从 ``uint8_t`` 更改为 ``wifi_bandwidth_t``

重大更新
~~~~~~~~

WPA3 企业版 Wi‑Fi 认证模式更改
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

在 ESP‑IDF v5.4 之前，支持 WPA3‑Enterprise‑Only 和 WPA3‑Enterprise‑Transition 的接入点（AP）的认证模式（``wifi_auth_mode_t``）均被识别为 ``WIFI_AUTH_WPA2_ENTERPRISE``.

从 v5.4 开始，该行为已更新：

- 支持 **WPA3‑Enterprise‑Only** 模式的AP现在被检测时，其 ``wifi_auth_mode_t`` 设置为 ``WIFI_AUTH_WPA3_ENTERPRISE``.
- 支持 **WPA3‑Enterprise‑Transition** 模式的AP被检测时，其 ``wifi_auth_mode_t`` 设置为 ``WIFI_AUTH_WPA2_WPA3_ENTERPRISE``.

.. note::

    **对应用程序的影响**

    如果您的应用程序严格检查 ``WIFI_AUTH_WPA2_ENTERPRISE`` 或 ``WIFI_AUTH_ENTERPRISE`` 来识别企业级 AP，则无法识别以 WPA3‑Enterprise‑Only 或 WPA3‑Enterprise‑Transition 模式运行的 AP.


    **建议操作**

    为确保与这些模式兼容，请更新您的应用程序关于认证模式的检查逻辑，将新的 ``WIFI_AUTH_WPA3_ENTERPRISE`` 和 ``WIFI_AUTH_WPA2_WPA3_ENTERPRISE`` 模式包含进来.

    这可能涉及更新条件判断语句或认证流程，以涵盖 ``WIFI_AUTH_WPA3_ENTERPRISE`` 和 ``WIFI_AUTH_WPA2_WPA3_ENTERPRISE`` 的情况.
