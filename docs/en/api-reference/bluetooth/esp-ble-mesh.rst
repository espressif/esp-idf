ESP-BLE-MESH
============

With various features of ESP-BLE-MESH, users can create a managed flooding mesh network for several
scenarios, such as lighting, sensor and etc.

For an ESP32 to join and work on a ESP-BLE-MESH network, it must be provisioned firstly. By provisioning,
the ESP32, as an unprovisioned device, will join the ESP-BLE-MESH network and become a ESP-BLE-MESH node,
communicating with other nodes within or beyond the radio range.

Apart from ESP-BLE-MESH nodes, inside ESP-BLE-MESH network, there is also ESP32 that works as ESP-BLE-MESH
Provisioner, which could provision unprovisioned devices into ESP-BLE-MESH nodes and configure the nodes
with various features.

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


ESP-BLE-MESH Definitions
------------------------

This section contains only one header file, which lists the following items of ESP-BLE-MESH.

* ID of all the models and related message opcodes
* Structs of model, element and Composition Data
* Structs of used by ESP-BLE-MESH Node/Provisioner for provisioning
* Structs used to transmit/receive messages
* Event types and related event parameters

.. include:: /_build/inc/esp_ble_mesh_defs.inc


ESP-BLE-MESH Core API Reference
-------------------------------

This section contains ESP-BLE-MESH Core related APIs, which can be used to initialize ESP-BLE-MESH
stack, provision, send/publish messages, etc.

This API reference covers six components:

* `ESP-BLE-MESH Stack Initialization`_
* `Reading of Local Data Information`_
* `Low Power Operation (Updating)`_
* `Send/Publish Messages, add Local AppKey, etc.`_
* `ESP-BLE-MESH Node/Provisioner Provisioning`_
* `ESP-BLE-MESH GATT Proxy Server`_


ESP-BLE-MESH Stack Initialization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include:: /_build/inc/esp_ble_mesh_common_api.inc


Reading of Local Data Information
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include:: /_build/inc/esp_ble_mesh_local_data_operation_api.inc


Low Power Operation (Updating)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include:: /_build/inc/esp_ble_mesh_low_power_api.inc


Send/Publish Messages, add Local AppKey, etc.
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include:: /_build/inc/esp_ble_mesh_networking_api.inc


ESP-BLE-MESH Node/Provisioner Provisioning
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include:: /_build/inc/esp_ble_mesh_provisioning_api.inc


ESP-BLE-MESH GATT Proxy Server
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include:: /_build/inc/esp_ble_mesh_proxy_api.inc


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

.. include:: /_build/inc/esp_ble_mesh_config_model_api.inc


Health Client/Server Models
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include:: /_build/inc/esp_ble_mesh_generic_model_api.inc


Generic Client/Server Models
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include:: /_build/inc/esp_ble_mesh_health_model_api.inc


Sensor Client/Server Models
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include:: /_build/inc/esp_ble_mesh_lighting_model_api.inc


Time and Scenes Client/Server Models
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include:: /_build/inc/esp_ble_mesh_sensor_model_api.inc


Lighting Client/Server Models
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include:: /_build/inc/esp_ble_mesh_time_scene_model_api.inc

