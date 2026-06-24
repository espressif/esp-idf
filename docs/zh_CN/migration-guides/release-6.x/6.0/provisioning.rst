配网
====

:link_to_translation:`en:[English]`

重大变更
--------

本次更新从 ESP-IDF 中移除了组件 ``wifi_provisioning``，更名为 ``network_provisioning`` 并作为一个独立组件受到支持。该组件增加了对 Thread 配网的支持。可在乐鑫组件注册表中，查看更多有关 ``network_provisioning`` 的信息：

* `network_provisioning 组件 <https://components.espressif.com/component/espressif/network_provisioning>`__

在新版应用程序中， ``main`` 组件文件夹应包括组件管理器清单文件 ``idf_component.yml``，如下所示：

.. code-block:: text

  dependencies:
    espressif/network_provisioning:
      version: "^1.0.5"

对于使用 ESP-IDF v5.x 及以后版本的应用程序，需要通过添加组件管理器清单文件 ``idf_component.yml`` 拉取新版 ``network_provisioning`` 组件。同时，在编译时，应去掉已过时的 ``wifi_provisioning`` 组件。此项操作可通过项目 ``CMakeLists.txt`` 中的以下语句实现：

.. code-block:: cmake

  set(EXCLUDE_COMPONENTS wifi_provisioning)

在新版的组件中，组件接口名有所更新，绝大多数的改动是将原组件中的 ``wifi_prov_xx`` 改为 ``network_prov_xx`` 。下表展示了其余的应用程序接口在新旧组件中的变化：

.. csv-table::
   :header: "原组件接口", "现组件接口"

   "wifi_prov_mgr_is_provisioned", "network_prov_mgr_is_wifi_provisioned"
   "wifi_prov_mgr_configure_sta", "network_prov_mgr_configure_wifi_sta"
   "wifi_prov_mgr_reset_provisioning", "network_prov_mgr_reset_wifi_provisioning"
   "wifi_prov_mgr_reset_sm_state_on_failure", "network_prov_mgr_reset_wifi_sm_state_on_failure"
   "wifi_prov_mgr_reset_sm_state_for_reprovision", "network_prov_mgr_reset_wifi_sm_state_for_reprovision"

配置变更
--------

Protocomm 安全配置
~~~~~~~~~~~~~~~~~~

为了默认提高安全性，protocomm 安全配置选项的默认值已更改：

- :ref:`CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_0` 现在默认为 ``n`` （原为 ``y`` ）
- :ref:`CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_1` 现在默认为 ``n`` （原为 ``y`` ）

依赖 protocomm 安全版本 0 或 1 的项目需要在其配置中显式启用这些选项。如果您的应用程序使用 protocomm 安全版本 0（无安全）或版本 1（Curve25519 + AES-CTR），则必须在项目的 ``sdkconfig`` 中或通过 ``menuconfig`` 显式启用相应的配置选项：

- 对于安全版本 0：启用 :ref:`CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_0`
- 对于安全版本 1：启用 :ref:`CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_1`

此项变更旨在默认减少代码大小，并鼓励使用更安全的 protocomm 实现。
