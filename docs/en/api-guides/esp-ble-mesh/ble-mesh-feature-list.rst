Espressif ESP-BLE-MESH Feature List
===================================

Supported Features
------------------

Mesh Core
"""""""""

* Provisioning: Node Role
    * PB-ADV and PB-GATT
    * OOB Authentication

* Provisioning: Provisioner Role
    * PB-ADV and PB-GATT
    * OOB Authentication

* Networking
    * Relay
    * Segmentation and Reassembly
    * Key Refresh Procedure
    * IV Update Procedure
    * Friend
    * Low Power
    * Proxy

* Multiple Client Models Run Simultaneously
    * Support multiple client models send packets to different nodes simultaneously
    * No blocking between client model and server model

* NVS Storing
    * Store provisioning and configuration information of ESP-BLE-MESH Node

Mesh Models
"""""""""""

* Foundation Models
    * Configuration Server Model
    * Configuration Client Model
    * Health Server Model
    * Health Client Model

* Generic Client Models
    * Generic OnOff Client
    * Generic Level Client
    * Generic Default Transition Time Client
    * Generic Power OnOff Client
    * Generic Power Level Client
    * Generic Battery Client
    * Generic Location Client
    * Generic Property Client

* Sensor Client Models
    * Sensor Client

* Time and Scenes Client Models
    * Time Client
    * Scene Client
    * Scheduler Client

* Lighting Client Models
    * Light Lightness Client
    * Light CTL Client
    * Light HSL Client
    * Light xyL Client
    * Light LC Client 

* Generic Server Models
    * Generic OnOff Server (Simple)

Mesh Applications
"""""""""""""""""

* ESP-BLE-MESH Node
    * :example:`Tutorial <bluetooth/esp_ble_mesh/ble_mesh_node/tutorial/Ble_Mesh_Node_Example_Walkthrough.md>`
    * :example:`Example <bluetooth/esp_ble_mesh/ble_mesh_node>`
* ESP-BLE-MESH Node with client model
    * :example:`Tutorial <bluetooth/esp_ble_mesh/ble_mesh_client_model/tutorial/ble_mesh_client_model.md>`
    * :example:`Example <bluetooth/esp_ble_mesh/ble_mesh_client_model>`
* ESP-BLE-MESH Provisioner
    * :example:`Tutorial <bluetooth/esp_ble_mesh/ble_mesh_provisioner/tutorial/Ble_Mesh_Provisioner_Example_Walkthrough.md>`
    * :example:`Example <bluetooth/esp_ble_mesh/ble_mesh_provisioner>`
* ESP-BLE-MESH Fast Provisioning
    * :example:`Fast Provisioning Client Model Tutorial <bluetooth/esp_ble_mesh/ble_mesh_fast_provision/ble_mesh_fast_prov_client/tutorial/ble_mesh_fast_provision_client.md>`
    * :example:`Fast Provisioning Server Model Tutorial <bluetooth/esp_ble_mesh/ble_mesh_fast_provision/ble_mesh_fast_prov_server/tutorial/ble_mesh_fast_provision_server.md>`
    * :example:`Example <bluetooth/esp_ble_mesh/ble_mesh_fast_provision>`
    * `Demo Video <http://download.espressif.com/BLE_MESH/BLE_Mesh_Demo/V0.4_Demo_Fast_Provision/ESP32_BLE_Mesh_Fast_Provision.mp4>`__
* ESP-BLE-MESH and Wi-Fi Coexistence
    * :example:`Tutorial <bluetooth/esp_ble_mesh/ble_mesh_wifi_coexist/tutorial/ble_mesh_wifi_coexist.md>`
    * :example:`Example <bluetooth/esp_ble_mesh/ble_mesh_wifi_coexist>`
    * `Demo Video <http://download.espressif.com/BLE_MESH/BLE_Mesh_Demo/V0.5_Demo_Coexistence/ESP_BLE_MESH_%26_WIFI_Coexistence.mp4>`__
* ESP-BLE-MESH Console Commands
    * :example:`Example <bluetooth/esp_ble_mesh/ble_mesh_console>`


Future Release Features
-----------------------

Mesh Core
"""""""""

* Proxy Client

Mesh Models
"""""""""""

* Generic Server Models
* Sensor Server Models
* Time and Scenes Server Models
* Lighting Server Models

Mesh Applications
"""""""""""""""""

* Fast OTA
* Friendship