Bluetooth\ :sup:`®` API
*****************************

:link_to_translation:`zh_CN:[中文]`

This section provides the API reference for Bluetooth components supported in ESP-IDF. ESP-IDF supports two host stacks: **Bluedroid** and **NimBLE**.

- **Bluedroid** (the default stack): Supports both Bluetooth Classic and Bluetooth LE. Recommended for applications that require both technologies.
- **NimBLE**: A lightweight stack for Bluetooth LE only. Ideal for resource-constrained applications due to smaller code size and memory usage.

Use the navigation links below to explore API documentation and application examples.

----

**Controller Interface API**

The low-level interface between the Bluetooth host stack and the controller.

.. toctree::
    :maxdepth: 1

    controller_vhci


**Bluedroid Stack API**

The default host stack in ESP-IDF, supporting both Bluetooth Classic and Bluetooth LE.

.. toctree::
    :maxdepth: 1

    bt_common
    :SOC_BT_CLASSIC_SUPPORTED: classic_bt
    bt_le

For architecture and feature overviews, refer to the following documents in API Guides:

.. only:: SOC_BT_CLASSIC_SUPPORTED

    :doc:`../../api-guides/bt-architecture/index`, :doc:`../../api-guides/classic-bt/index`, :doc:`../../api-guides/ble/index`

.. only:: not SOC_BT_CLASSIC_SUPPORTED

    :doc:`../../api-guides/bt-architecture/index`, :doc:`../../api-guides/ble/index`

**NimBLE Stack API**

A lightweight host stack for Bluetooth LE.

.. toctree::
    :maxdepth: 1

    nimble/index

For additional details and API reference from the upstream documentation, refer to `Apache Mynewt NimBLE User Guide <https://mynewt.apache.org/latest/network/index.html>`_.

.. only:: SOC_BLE_MESH_SUPPORTED

    **ESP-BLE-MESH API**

    Implements Bluetooth LE Mesh networking.

    .. toctree::
        :maxdepth: 1

        esp-ble-mesh

----

Examples and Tutorials
---------------------------

Explore examples and tutorials in the ESP-IDF examples directory:

- **Bluedroid**: :example:`bluetooth/bluedroid`
- **NimBLE**: :example:`bluetooth/nimble`

Step-by-step tutorials for developing with the Bluedroid stack:

* :example_file:`GATT Client Example Walkthrough <bluetooth/bluedroid/ble/gatt_client/tutorial/Gatt_Client_Example_Walkthrough.md>`
* :example_file:`GATT Server Service Table Example Walkthrough <bluetooth/bluedroid/ble/gatt_server_service_table/tutorial/Gatt_Server_Service_Table_Example_Walkthrough.md>`
* :example_file:`GATT Server Example Walkthrough <bluetooth/bluedroid/ble/gatt_server/tutorial/Gatt_Server_Example_Walkthrough.md>`
* :example_file:`GATT Security Client Example Walkthrough <bluetooth/bluedroid/ble/gatt_security_client/tutorial/Gatt_Security_Client_Example_Walkthrough.md>`
* :example_file:`GATT Security Server Example Walkthrough <bluetooth/bluedroid/ble/gatt_security_server/tutorial/Gatt_Security_Server_Example_Walkthrough.md>`
* :example_file:`GATT Client Multi-connection Example Walkthrough <bluetooth/bluedroid/ble/gattc_multi_connect/tutorial/Gatt_Client_Multi_Connection_Example_Walkthrough.md>`

Step-by-step tutorials for developing with the NimBLE stack:

* :example_file:`Bluetooth LE Central Example Walkthrough <bluetooth/nimble/blecent/tutorial/blecent_walkthrough.md>`
* :example_file:`Bluetooth LE Heart Rate Example Walkthrough <bluetooth/nimble/blehr/tutorial/blehr_walkthrough.md>`
* :example_file:`Bluetooth LE Peripheral Example Walkthrough <bluetooth/nimble/bleprph/tutorial/bleprph_walkthrough.md>`
