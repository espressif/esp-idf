Wi-Fi
======

:link_to_translation:`zh_CN:[中文]`


Wi-Fi Scan and Connect
------------------------

    The following types have been modified:

    - :component_file:`esp_wifi/include/esp_wifi_he_types.h`

        - :cpp:struct:`esp_wifi_htc_omc_t`:

            - ``uph_id``, ``ul_pw_headroom``, ``min_tx_pw_flag`` are deprecated.

    - :component_file:`esp_wifi/include/esp_wifi_types_generic.h`

        - :cpp:struct:`wifi_ap_record_t`:

            - The type of ``bandwidth`` has been changed from ``uint8_t`` to ``wifi_bandwidth_t``

Breaking Changes
~~~~~~~~~~~~~~~~

Wi-Fi Authentication Mode Changes for WPA3-Enterprise
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In ESP-IDF versions prior to v5.4, the authentication mode (``wifi_auth_mode_t``) for Access Points (APs) supporting WPA3-Enterprise-Only and WPA3-Enterprise-Transition was identified as ``WIFI_AUTH_WPA2_ENTERPRISE``.

Starting from v5.4, this behavior has been updated:

- APs supporting **WPA3-Enterprise-Only** mode are now detected with ``wifi_auth_mode_t`` set to ``WIFI_AUTH_WPA3_ENTERPRISE``.
- APs supporting **WPA3-Enterprise-Transition** mode are detected with ``wifi_auth_mode_t`` set to ``WIFI_AUTH_WPA2_WPA3_ENTERPRISE``.

.. note::

    **Impact on Applications**

    If your application strictly checks for ``WIFI_AUTH_WPA2_ENTERPRISE`` or ``WIFI_AUTH_ENTERPRISE`` to identify enterprise APs, it will not recognize APs operating in WPA3-Enterprise-Only or WPA3-Enterprise-Transition modes.

    **Recommended Action**

    To ensure compatibility with these modes, update your application's authentication mode checks to include the new ``WIFI_AUTH_WPA3_ENTERPRISE`` and ``WIFI_AUTH_WPA2_WPA3_ENTERPRISE`` modes.

    This may involve updating conditional statements or authentication procedures to account for ``WIFI_AUTH_WPA3_ENTERPRISE`` and ``WIFI_AUTH_WPA2_WPA3_ENTERPRISE``.
