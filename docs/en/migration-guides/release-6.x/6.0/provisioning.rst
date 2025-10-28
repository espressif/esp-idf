Provisioning
============

:link_to_translation:`zh_CN:[中文]`

Breaking Changes
----------------

The ESP-IDF component ``wifi_provisioning`` has been removed from ESP-IDF and is supported as a separate component. It has been renamed to ``network_provisioning`` and Thread network provisioning support has been added to the new component. Additional information for the ``network_provisioning`` component can be found in ESP Component Registry:

* `network_provisioning component <https://components.espressif.com/component/espressif/network_provisioning>`__

The ``main`` component folder of the new application shall include the component manager manifest file ``idf_component.yml`` as in the example below:

.. code-block:: text

  dependencies:
    espressif/network_provisioning:
      version: "^1.1.0"

For applications targeting v5.x releases of ESP-IDF that need to use new ``network_provisioning`` component, adding the component manager manifest file ``idf_component.yml`` will be sufficient to pull in the new component. However, users should also exclude the legacy ``wifi_provisioning`` component from the build. This can be achieved using the statement below in the project's ``CMakeLists.txt``:

.. code-block:: cmake

  set(EXCLUDE_COMPONENTS wifi_provisioning)

The API names have been updated after migrating to the new component. Most changes involve renaming API names from ``wifi_prov_xx`` to ``network_prov_xx``. The table below summarizes the remaining differences between the old APIs and their new counterparts.

.. csv-table::
   :header: "Previous API", "Current API"

   "wifi_prov_mgr_is_provisioned", "network_prov_mgr_is_wifi_provisioned"
   "wifi_prov_mgr_configure_sta", "network_prov_mgr_configure_wifi_sta"
   "wifi_prov_mgr_reset_provisioning", "network_prov_mgr_reset_wifi_provisioning"
   "wifi_prov_mgr_reset_sm_state_on_failure", "network_prov_mgr_reset_wifi_sm_state_on_failure"
   "wifi_prov_mgr_reset_sm_state_for_reprovision", "network_prov_mgr_reset_wifi_sm_state_for_reprovision"

Configuration Changes
---------------------

Protocomm Security Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The default values for protocomm security configuration options have been changed to improve security by default:

- :ref:`CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_0` now defaults to ``n`` (previously ``y``)
- :ref:`CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_1` now defaults to ``n`` (previously ``y``)

Projects that rely on protocomm security versions 0 or 1 will need to explicitly enable these options in their configuration. If your application uses protocomm security version 0 (no security) or version 1 (Curve25519 + AES-CTR), you must explicitly enable the corresponding configuration option in your project's ``sdkconfig`` or through ``menuconfig``:

- For security version 0: Enable :ref:`CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_0`
- For security version 1: Enable :ref:`CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_1`

This change was made to reduce code size by default and encourage the use of more secure protocomm implementations.
