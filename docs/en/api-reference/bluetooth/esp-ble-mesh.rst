ESP-BLE-MESH
============

.. note::

  The current ESP-BLE-MESH v1.1 related code is a preview version, so the Mesh Protocol v1.1 related Structures, MACROs, and APIs involved in the code may be changed.

With various features of ESP-BLE-MESH, users can create a managed flooding mesh network for several scenarios, such as lighting, sensor and etc.

For an ESP32 to join and work on a ESP-BLE-MESH network, it must be provisioned firstly. By provisioning, the ESP32, as an unprovisioned device, will join the ESP-BLE-MESH network and become a ESP-BLE-MESH node, communicating with other nodes within or beyond the radio range.

Apart from ESP-BLE-MESH nodes, inside ESP-BLE-MESH network, there is also ESP32 that works as ESP-BLE-MESH provisioner, which could provision unprovisioned devices into ESP-BLE-MESH nodes and configure the nodes with various features.

For information how to start using ESP32 and ESP-BLE-MESH, please see the Section :ref:`getting-started-with-ble-mesh`. If you are interested in information on ESP-BLE-MESH architecture, including some details of software implementation, please see Section :doc:`../../api-guides/esp-ble-mesh/ble-mesh-architecture`.


Application Examples and Demos
------------------------------

Please refer to Sections :ref:`esp-ble-mesh-examples` and :ref:`esp-ble-mesh-demo-videos`.


API Reference
-------------

ESP-BLE-MESH APIs are divided into the following parts:

* `ESP-BLE-MESH Definitions`_
* `ESP-BLE-MESH Core API Reference`_
* `ESP-BLE-MESH Models API Reference`_
* `ESP-BLE-MESH (v1.1) Core API Reference`_


ESP-BLE-MESH Definitions
------------------------

This section contains only one header file, which lists the following items of ESP-BLE-MESH.

* ID of all the models and related message opcodes
* Structs of model, element and Composition Data
* Structs of used by ESP-BLE-MESH Node/Provisioner for provisioning
* Structs used to transmit/receive messages
* Event types and related event parameters

.. include-build-file:: inc/esp_ble_mesh_defs.inc


ESP-BLE-MESH Core API Reference
-------------------------------

This section contains ESP-BLE-MESH Core related APIs, which can be used to initialize ESP-BLE-MESH stack, provision, send/publish messages, etc.

This API reference covers six components:

* `ESP-BLE-MESH Stack Initialization`_
* `Reading of Local Data Information`_
* `Low Power Operation (Updating)`_
* `Send/Publish Messages, add Local AppKey, etc.`_
* `ESP-BLE-MESH Node/Provisioner Provisioning`_
* `ESP-BLE-MESH GATT Proxy Server`_


ESP-BLE-MESH Stack Initialization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_mesh_common_api.inc


Reading of Local Data Information
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_mesh_local_data_operation_api.inc


Low Power Operation (Updating)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_mesh_low_power_api.inc


Send/Publish Messages, Add Local AppKey, Etc.
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_mesh_networking_api.inc


ESP-BLE-MESH Node/Provisioner Provisioning
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_mesh_provisioning_api.inc


ESP-BLE-MESH GATT Proxy Server
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_mesh_proxy_api.inc


ESP-BLE-MESH Models API Reference
---------------------------------

This section contains ESP-BLE-MESH Model related APIs, event types, event parameters, etc.

There are six categories of models:

* `Configuration Client/Server Models`_
* `Health Client/Server Models`_
* `Generic Client/Server Models`_
* `Sensor Client/Server Models`_
* `Time and Scenes Client/Server Models`_
* `Lighting Client/Server Models`_


.. note::

    Definitions related to Server Models are being updated, and will be released soon.


Configuration Client/Server Models
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_mesh_config_model_api.inc


Health Client/Server Models
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_mesh_health_model_api.inc


Generic Client/Server Models
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_mesh_generic_model_api.inc


Sensor Client/Server Models
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_mesh_sensor_model_api.inc


Time and Scenes Client/Server Models
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_mesh_time_scene_model_api.inc


Lighting Client/Server Models
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_mesh_lighting_model_api.inc


ESP-BLE-MESH (v1.1) Core API Reference
--------------------------------------

.. note::

  This section is a preview version, so the related structures, macros, and APIs may be changed.

This section contains ESP-BLE-MESH v1.1 Core related APIs, event types, event parameters, etc.

This API reference covers 10 components:

* `Remote Provisioning`_
* `Directed Forwarding`_
* `Subnet Bridge Configuration`_
* `Mesh Private Beacon`_
* `On-Demand Private Proxy`_
* `SAR Configuration`_
* `Solicitation PDU RPL Configuration`_
* `Opcodes Aggregator`_
* `Large Composition Data`_
* `Composition and Metadata`_


Remote Provisioning
^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_mesh_rpr_model_api.inc


Directed Forwarding
^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_mesh_df_model_api.inc

Subnet Bridge Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_mesh_brc_model_api.inc

Mesh Private Beacon
^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_mesh_prb_model_api.inc

On-Demand Private Proxy
^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_mesh_odp_model_api.inc

SAR Configuration
^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_mesh_sar_model_api.inc

Solicitation PDU RPL Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_mesh_srpl_model_api.inc

Opcodes Aggregator
^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_mesh_agg_model_api.inc

Large Composition Data
^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_mesh_lcd_model_api.inc

Composition and Metadata
^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_mesh_cm_data_api.inc

