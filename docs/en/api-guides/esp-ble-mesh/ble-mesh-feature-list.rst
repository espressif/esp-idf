ESP-BLE-MESH Feature List
=========================

Supported Features
------------------

Mesh Core
"""""""""

* Provisioning:
    * PB-ADV, PB-GATT and PB-Remote
    * OOB Authentication
    * Certificate-based Provisioning
    * Remote Provisioning
    * Enhanced Provisioning Authentication

* Networking
    * Relay
    * Segmentation and Reassembly
    * Key Refresh Procedure
    * IV Update Procedure
    * Friend
    * Low Power
    * Proxy Server
    * Proxy Client
    * Directed Forwarding
    * Private Beacon
    * Subnet Bridge
    * Minor Enhancements

* Multiple Client Models Run Simultaneously
    * Support multiple client models send packets to different nodes simultaneously
    * No blocking between client model and server model

* NVS Storing
    * Store provisioning and configuration information of ESP-BLE-MESH Node

Mesh Models
"""""""""""

* Foundation models
    * Configuration Server model
    * Configuration Client model
    * Health Server model
    * Health Client model
    * Remote Provisioning Server model
    * Remote Provisioning Client model
    * Directed Forwarding Configuration Server model
    * Directed Forwarding Configuration Client model
    * Bridge Configuration Server model
    * Bridge Configuration Client model
    * Mesh Private Beacon Server model
    * Mesh Private Beacon Client model
    * On-Demand Private Proxy Server model
    * On-Demand Private Proxy Client model
    * SAR Configuration Server model
    * SAR Configuration Client model
    * Solicitation PDU RPL Configuration Server model
    * Solicitation PDU RPL Configuration Client model
    * Opcodes Aggregator Server model
    * Opcodes Aggregator Client model
    * Large Composition Data Server model
    * Large Composition Data Client model

* Generic client models
    * Generic OnOff Client
    * Generic Level Client
    * Generic Default Transition Time Client
    * Generic Power OnOff Client
    * Generic Power Level Client
    * Generic Battery Client
    * Generic Location Client
    * Generic Property Client

* Sensor client models
    * Sensor Client

* Time and Scenes client models
    * Time Client
    * Scene Client
    * Scheduler Client

* Lighting client models
    * Light Lightness Client
    * Light CTL Client
    * Light HSL Client
    * Light xyL Client
    * Light LC Client

* Generic server models
    * Generic OnOff Server
    * Generic Level Server
    * Generic Default Transition Time Server
    * Generic Power OnOff Server
    * Generic Power OnOff Setup Server
    * Generic Power Level Server
    * Generic Power Level Setup Server
    * Generic Battery Server
    * Generic Location Server
    * Generic Location Setup Server
    * Generic User Property Server
    * Generic Admin Property Server
    * Generic Manufacturer Property Server
    * Generic Client Property Server

* Sensor server models
    * Sensor Server
    * Sensor Setup Server

* Time and Scenes server models
    * Time Server
    * Time Setup Server
    * Scene Server
    * Scene Setup Server
    * Scheduler Server
    * Scheduler Setup Server

* Lighting server models
    * Light Lightness Server
    * Light Lightness Setup Server
    * Light CTL Server
    * Light CTL Temperature Server
    * Light CTL Setup Server
    * Light HSL Server
    * Light HSL Hue Server
    * Light HSL Saturation Server
    * Light HSL Setup Server
    * Light xyL Server
    * Light xyL Setup Server
    * Light LC Server
    * Light LC Setup Server

Mesh Applications
"""""""""""""""""

* ESP-BLE-MESH Node
    * :example_file:`Tutorial <bluetooth/esp_ble_mesh/onoff_models/onoff_client/tutorial/BLE_Mesh_Node_OnOff_Client_Example_Walkthrough.md>`
    * :example_file:`Tutorial <bluetooth/esp_ble_mesh/onoff_models/onoff_server/tutorial/BLE_Mesh_Node_OnOff_Server_Example_Walkthrough.md>`
    * :example:`Example <bluetooth/esp_ble_mesh/onoff_models>`
* ESP-BLE-MESH Provisioner
    * :example_file:`Tutorial <bluetooth/esp_ble_mesh/provisioner/tutorial/BLE_Mesh_Provisioner_Example_Walkthrough.md>`
    * :example:`Example <bluetooth/esp_ble_mesh/provisioner>`
* ESP-BLE-MESH Fast Provisioning
    * :example_file:`Fast Provisioning Client Model Tutorial <bluetooth/esp_ble_mesh/fast_provisioning/fast_prov_client/tutorial/BLE_Mesh_Fast_Prov_Client_Example_Walkthrough.md>`
    * :example_file:`Fast Provisioning Server Model Tutorial <bluetooth/esp_ble_mesh/fast_provisioning/fast_prov_server/tutorial/BLE_Mesh_Fast_Prov_Server_Example_Walkthrough.md>`
    * :example:`Example <bluetooth/esp_ble_mesh/fast_provisioning>`
    * `Demo Video <https://dl.espressif.com/BLE/public/ESP32_BLE_Mesh_Fast_Provision.mp4>`__
* ESP-BLE-MESH and Wi-Fi Coexistence
    * :example_file:`Tutorial <bluetooth/esp_ble_mesh/wifi_coexist/tutorial/BLE_Mesh_WiFi_Coexist_Example_Walkthrough.md>`
    * :example:`Example <bluetooth/esp_ble_mesh/wifi_coexist>`
    * `Demo Video <https://dl.espressif.com/BLE/public/ESP_BLE_MESH_WIFI_Coexistence.mp4>`__


Future Release Features
-----------------------

Mesh Core
"""""""""

* Provisioner NVS Storage

Mesh Applications
"""""""""""""""""

* Fast OTA
* Friendship
