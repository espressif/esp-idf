ESP-BLE-MESH FAQ
================

This document provides a summary of frequently asked questions about developing with ESP-BLE-MESH, and is divided into seven sections:

* :ref:`ble-mesh-faq-provisioner-development`
* :ref:`ble-mesh-faq-node-development`
* :ref:`ble-mesh-faq-ble-mesh-and-wi-fi-coexistence`
* :ref:`ble-mesh-faq-fast-provisioning`
* :ref:`ble-mesh-faq-log-help`
* :ref:`ble-mesh-faq-example-help`
* :ref:`ble-mesh-faq-others`

Users could refer to the sections for quick answer to their questions. This document will be updated based on the feedback collected via various channels.


.. _ble-mesh-faq-provisioner-development:

1. Provisioner Development
--------------------------

Generally, a Provisioner is used to provision unprovisioned devices and form a mesh network. And after provisioning, roles of the unprovisioned devices will be changed to those of a node.

1.1 What is the flow for an unprovisioned device to join ESP-BLE-MESH network?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    There are two phases for a device to join ESP-BLE-MESH network via a Provisioner, namely, provisioning and configuration.

    - The phase of provisioning is to assign unicast address, add NetKey and etc. to a device. By provisioning, the device joins the ESP-BLE-MESH network and its role is changed from an unprovisioned device to a node.

    - The phase of configuration is to add AppKeys to the node and bind AppKeys to corresponding models. And some items are optional during configuration, including adding subscription addresses to the node, set publication information, etc. By configuration, the node can actually transmit messages to a Provisioner and receive messages from it.

1.2 If a Provisioner wants to change states of a node, what requirements should be met for a Provisioner?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - Client model that corresponds to server model of the node is required.
    - NetKey and AppKey used to encrypt messages shall be owned by both the node and the Provisioner.
    - The address owned by the node shall be known, which could be its unicast address or subscription address.

1.3 How can NetKey and AppKey be used?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - NetKey is used for encryption of messages in Network Layer. Nodes with the same NetKey are assumed to be in the same subnet while those with different NetKeys cannot communicate with each other.
    - AppKey is used for encryption of messages in Upper Transport Layer. If client model and server model are bound to different AppKeys, the communication cannot be achieved.

1.4 How to generate a NetKey or AppKey for Provisioner? Can we use a fixed NetKey or AppKey?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - The API :cpp:func:`esp_ble_mesh_provisioner_add_local_net_key` can be used to add a NetKey with a fixed or random value.
    - The API :cpp:func:`esp_ble_mesh_provisioner_add_local_app_key` can be used to add an AppKey with a fixed or random value.

1.5 Is the unicast address of Provisioner fixed?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    The value of :code:`prov_unicast_addr` in :cpp:type:`esp_ble_mesh_prov_t` is used to set the unicast address of Provisioner, it can be set only once during initialization and can't be changed afterwards.

1.6 Can the address of Provisioner serve as destination address of the node-reporting-status message？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    The unicast address of Provisioner can be set only once during initialization and can't be changed afterwards. In theory, it can serve as the destination address of the node-reporting-status message, provided that the unicast address of the Provisioner is known by nodes. Nodes can know the unicast address of Provisioner during configuration since Provisioner sends messages to them with its unicast address used as the source address.
    Subscription address can also be used. Provisioner subscribes to a group address or virtual address, and nodes send messages to the subscription address.

1.7 Is the unicast address of the node that is firstly provisioned by Provisioner to ESP-BLE-MESH network fixed？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    The value of :code:`prov_start_address` in :cpp:type:`esp_ble_mesh_prov_t` is used to set the starting address when the Provisioner provisions unprovisioned devices, i.e. the unicast address of the node it firstly provisioned. It can be set only once during initialization and can't be changed afterwards.

1.8 Is the unicast address of the node that mobile App firstly provisioned fixed?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    The App will decide the unicast address, and currently most of them are fixed.

1.9 How to know which unprovisioned device is the Provisioner that is provisioning currently?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    The value of :code:`prov_attention` in :cpp:type:`esp_ble_mesh_prov_t` is used by Provisioner set to unprovisioned device during provisioning. It can be set only once during initialization and can't be changed afterwards. When the unprovisioned device is joining the mesh network, it can display in a specific way like flashing light to notify Provisioner that it is being provisioned.

1.10 How many ways to authenticate the devices during provisioning? Which way was used in the :example:`provided examples <bluetooth/esp_ble_mesh>`?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    There are four authentication methods, i.e. No OOB, Static OOB, Output OOB and Input OOB. In the provided examples, No OOB is used.

1.11 What information can be carried by the advertising packets of the unprovisioned device before provisioning into the network?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - Device UUID
    - OOB Info
    - URL Hash (optional)

1.12 Can such information be used for device identification?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    For example, each unprovisioned device contains a unique Device UUID, which can be used for device identification.

1.13 How is the unicast address assigned when the node provisioned by Provisioner contains multiple elements?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - Provisioner will assign an unicast address for the primary element of the node, and unicast address of the remaining elements are incremented one by one.
    - For example: If an unprovisioned device has three elements, i.e. the primary element, the second element and the third element. After provisioning, the primary element address of the node is 0x0002 while the second element address is 0x0003, and the third element address is 0x0004.

1.14 How can Provisioner get and parse the :ref:`Composition Data <ble-mesh-terminology-composition>` of nodes through Configuration Client Model?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - Provisioner can get the Composition Data of nodes using the :ref:`Configuration Client Model <ble-mesh-terminology-foundation-models>` API :cpp:func:`esp_ble_mesh_config_client_set_state` with :code:`comp_data_get` in the parameter :cpp:type:`esp_ble_mesh_cfg_client_get_state_t` set properly.
    - Users can refer to the following code to parse the Composition Data:

    .. code:: c

        #include <stdio.h>
        #include <string.h>
        #include <stdint.h>

        //test date: 0C001A0001000800030000010501000000800100001003103F002A00
        //0C00 1A00 0100 0800 0300 0001 05 01 0000 0080 0100 0010 0310 3F002A00

        // CID is 0x000C
        // PID is 0x001A
        // VID is 0x0001
        // CRPL is 0x0008
        // Features is 0x0003 – Relay and Friend features.
        // Loc is “front” – 0x0100
        // NumS is 5
        // NumV is 1
        // The Bluetooth SIG Models supported are: 0x0000, 0x8000, 0x0001, 0x1000, 0x1003
        // The Vendor Models supported are: Company Identifier 0x003F and Model Identifier 0x002A

        typedef struct {
            int16_t cid;
            int16_t pid;
            int16_t vid;
            int16_t crpl;
            int16_t features;
            int16_t all_models;
            uint8_t sig_models;
            uint8_t vnd_models;
        } esp_ble_mesh_composition_head;

        typedef struct {
            uint16_t model_id;
            uint16_t vendor_id;
        } tsModel;

        typedef struct {
            // reserve space for up to 20 SIG models
            uint16_t SIG_models[20];
            uint8_t numSIGModels;

            // reserve space for up to 4 vendor models
            tsModel Vendor_models[4];
            uint8_t numVendorModels;
        } esp_ble_mesh_composition_decode;

        int decode_comp_data(esp_ble_mesh_composition_head *head, esp_ble_mesh_composition_decode *data, uint8_t *mystr, int size)
        {
            int pos_sig_base;
            int pos_vnd_base;
            int i;

            memcpy(head, mystr, sizeof(*head));

            if(size < sizeof(*head) + head->sig_models * 2 + head->vnd_models * 4) {
                return -1;
            }

            pos_sig_base = sizeof(*head) - 1;

            for(i = 1; i < head->sig_models * 2; i = i + 2) {
                data->SIG_models[i/2] = mystr[i + pos_sig_base] | (mystr[i + pos_sig_base + 1] << 8);
                printf("%d: %4.4x\n", i/2, data->SIG_models[i/2]);
            }

            pos_vnd_base = head->sig_models * 2 + pos_sig_base;

            for(i = 1; i < head->vnd_models * 2; i = i + 2) {
                data->Vendor_models[i/2].model_id = mystr[i + pos_vnd_base] | (mystr[i + pos_vnd_base + 1] << 8);
                printf("%d: %4.4x\n", i/2, data->Vendor_models[i/2].model_id);

                data->Vendor_models[i/2].vendor_id = mystr[i + pos_vnd_base + 2] | (mystr[i + pos_vnd_base + 3] << 8);
                printf("%d: %4.4x\n", i/2, data->Vendor_models[i/2].vendor_id);
            }

            return 0;
        }

        void app_main(void)
        {
            esp_ble_mesh_composition_head head = {0};
            esp_ble_mesh_composition_decode data = {0};
            uint8_t mystr[] = { 0x0C, 0x00, 0x1A, 0x00,
                                0x01, 0x00, 0x08, 0x00,
                                0x03, 0x00, 0x00, 0x01,
                                0x05, 0x01, 0x00, 0x00,
                                0x00, 0x80, 0x01, 0x00,
                                0x00, 0x10, 0x03, 0x10,
                                0x3F, 0x00, 0x2A, 0x00};
            int ret;

            ret = decode_comp_data(&head, &data, mystr, sizeof(mystr));
            if (ret == -1) {
                printf("decode_comp_data error");
            }
        }

1.15 How can Provisioner further configure nodes through obtained Composition Data?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Provisioner do the following configuration by calling the :ref:`Configuration Client Model <ble-mesh-terminology-foundation-models>` API :cpp:func:`esp_ble_mesh_config_client_set_state`.

    - Add AppKey to nodes with :code:`app_key_add` in the parameter :cpp:type:`esp_ble_mesh_cfg_client_set_state_t` set properly.
    - Add subscription address to the models of nodes with :code:`model_sub_add` in the parameter :cpp:type:`esp_ble_mesh_cfg_client_set_state_t` set properly.
    - Set publication information to the models of nodes with :code:`model_pub_set` in the parameter :cpp:type:`esp_ble_mesh_cfg_client_set_state_t` set properly.

1.16 Can nodes add corresponding configurations for themselves?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    This method can be used in special cases like testing period.

    - Here is an example to show nodes add new group addresses for their models.

    .. code:: c

        esp_err_t example_add_fast_prov_group_address(uint16_t model_id, uint16_t group_addr)
        {
            const esp_ble_mesh_comp_t *comp = NULL;
            esp_ble_mesh_elem_t *element = NULL;
            esp_ble_mesh_model_t *model = NULL;
            int i, j;

            if (!ESP_BLE_MESH_ADDR_IS_GROUP(group_addr)) {
                return ESP_ERR_INVALID_ARG;
            }

            comp = esp_ble_mesh_get_composition_data();
            if (!comp) {
                return ESP_FAIL;
            }

            for (i = 0; i < comp->element_count; i++) {
                element = &comp->elements[i];
                model = esp_ble_mesh_find_sig_model(element, model_id);
                if (!model) {
                    continue;
                }
                for (j = 0; j < ARRAY_SIZE(model->groups); j++) {
                    if (model->groups[j] == group_addr) {
                        break;
                    }
                }
                if (j != ARRAY_SIZE(model->groups)) {
                    ESP_LOGW(TAG, "%s: Group address already exists, element index: %d", __func__, i);
                    continue;
                }
                for (j = 0; j < ARRAY_SIZE(model->groups); j++) {
                    if (model->groups[j] == ESP_BLE_MESH_ADDR_UNASSIGNED) {
                        model->groups[j] = group_addr;
                        break;
                    }
                }
                if (j == ARRAY_SIZE(model->groups)) {
                    ESP_LOGE(TAG, "%s: Model is full of group addresses, element index: %d", __func__, i);
                }
            }

            return ESP_OK;
        }

.. note::

    When the NVS storage of the node is enabled, group address added and AppKey bound by this method will not be saved in the NVS when the device is powered off currently. These configuration information can only be saved if they are configured by Configuration Client Model.

1.17 How does Provisioner control nodes by grouping?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Generally there are two approaches to implement group control in ESP-BLE-MESH network, group address approach and virtual address approach. And supposing there are 10 devices, i.e., five devices with blue lights and five devices with red lights.

    - Method 1: 5 blue lights can subscribe to a group address, 5 red lights subscribe to another one. By sending messages to different group addresses, Provisioner can realize group control.

    - Method 2: 5 blue lights can subscribe to a virtual address, 5 red lights subscribe to another one. By sending messages to different virtual addresses, Provisioner can realize group control.

1.18 How does Provisioner add nodes to multiple subnets?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Provisioner can add multiple NetKeys to nodes during configuration, and nodes sharing the same NetKey belong to the same subnet. Provisioner can communicate with nodes on different subnets by using different NetKeys.

1.19 How does Provisioner know if a node in the mesh network is offline?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Node offline is usually defined as: the condition that the node cannot be properly communicated with other nodes in the mesh network due to power failure or some other reasons.

    There is no connection between nodes and nodes in the ESP-BLE-MESH network. They communicate with each other through advertising channels.

    An example is given here to show how to detect a node is offline by Provisioner.

    - The node can periodically send heartbeat messages to Provisioner. And if Provisioner failed to receive heartbeat messages in a certain period, the node is considered to be offline.

.. note::

    The heartbeat message should be designed into a single package (less than 11 bytes), so the transmission and reception of it can be more efficient.

1.20 What operations should be performed when Provisioner removes nodes from the network?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Usually when Provisioner tries to remove node from the mesh network, the procedure includes three main steps:

    - Firstly, Provisioner adds the node that need to be removed to the "blacklist".

    - Secondly, Provisioner performs the :ref:`Key Refresh procedure <ble-mesh-terminology-network-management>`.

    - Lastly, the node performs node reset procedure, and switches itself to an unprovisioned device.

1.21 In the Key Refresh procedure, how does Provisioner update the Netkey owned by nodes?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - Provisioner updates the NetKey of nodes using the :ref:`Configuration Client Model <ble-mesh-terminology-foundation-models>` API :cpp:func:`esp_ble_mesh_config_client_set_state` with :code:`net_key_update` in the parameter :cpp:type:`esp_ble_mesh_cfg_client_set_state_t` set properly.

    - Provisioner updates the AppKey of nodes using the :ref:`Configuration Client Model <ble-mesh-terminology-foundation-models>` API :cpp:func:`esp_ble_mesh_config_client_set_state` with :code:`app_key_update` in the parameter :cpp:type:`esp_ble_mesh_cfg_client_set_state_t` set properly.

1.22 How does Provisioner manage nodes in the mesh network?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    ESP-BLE-MESH implements several functions related to basic node management in the example, such as :cpp:func:`esp_ble_mesh_store_node_info`. And ESP-BLE-MESH also provides the API :cpp:func:`esp_ble_mesh_provisioner_set_node_name` which can be used to set the node's local name and the API :cpp:func:`esp_ble_mesh_provisioner_get_node_name` which can be used to get the node's local name.

1.23 What does Provisioner need when trying to control the server model of nodes?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Provisioner must include corresponding client model before controlling the server model of nodes.

    Provisioner shall add its local NetKey and AppKey.

    - Provisioner add NetKey by calling the API :cpp:func:`esp_ble_mesh_provisioner_add_local_net_key`.

    - Provisioner add AppKey by calling the API :cpp:func:`esp_ble_mesh_provisioner_add_local_app_key`.

    Provisioner shall configure its own client model.

    - Provisioner bind AppKey to its own client model by calling the API :cpp:func:`esp_ble_mesh_provisioner_bind_app_key_to_local_model`.

1.24 How does Provisoner control the server model of nodes?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    ESP-BLE-MESH supports all SIG-defined client models. Provisioner can use these client models to control the server models of nodes. And the client models are divided into 6 categories with each category has the corresponding functions.

    - Configuration Client Model

        - The API :cpp:func:`esp_ble_mesh_config_client_get_state` can be used to get the :cpp:type:`esp_ble_mesh_cfg_client_get_state_t` values of Configuration Server Model.
        - The API :cpp:func:`esp_ble_mesh_config_client_set_state` can be used to set the :cpp:type:`esp_ble_mesh_cfg_client_set_state_t` values of Configuration Server Model.

    - Health Client Model

        - The API :cpp:func:`esp_ble_mesh_health_client_get_state` can be used to get the :cpp:type:`esp_ble_mesh_health_client_get_state_t` values of Health Server Model.
        - The API :cpp:func:`esp_ble_mesh_health_client_set_state` can be used to set the :cpp:type:`esp_ble_mesh_health_client_set_state_t` values of Health Server Model.

    - Generic Client Models

        - The API :cpp:func:`esp_ble_mesh_generic_client_get_state` can be used to get the :cpp:type:`esp_ble_mesh_generic_client_get_state_t` values of Generic Server Models.
        - The API :cpp:func:`esp_ble_mesh_generic_client_set_state` can be used to set the :cpp:type:`esp_ble_mesh_generic_client_set_state_t` values of Generic Server Models.

    - Lighting Client Models

        - The API :cpp:func:`esp_ble_mesh_light_client_get_state` can be used to get the :cpp:type:`esp_ble_mesh_light_client_get_state_t` values of Lighting Server Models.
        - The API :cpp:func:`esp_ble_mesh_light_client_set_state` can be used to set the :cpp:type:`esp_ble_mesh_light_client_set_state_t` values of Lighting Server Models.

    - Sensor Client Models

        - The API :cpp:func:`esp_ble_mesh_sensor_client_get_state` can be used to get the :cpp:type:`esp_ble_mesh_sensor_client_get_state_t` values of Sensor Server Model.
        - The API :cpp:func:`esp_ble_mesh_sensor_client_set_state` can be used to set the :cpp:type:`esp_ble_mesh_sensor_client_set_state_t` values of Sensor Server Model.

    - Time and Scenes Client Models
        - The API :cpp:func:`esp_ble_mesh_time_scene_client_get_state` can be used to get the :cpp:type:`esp_ble_mesh_time_scene_client_get_state_t` values of Time and Scenes Server Models.
        - The API :cpp:func:`esp_ble_mesh_time_scene_client_set_state` can be used to set the :cpp:type:`esp_ble_mesh_time_scene_client_set_state_t` values of Time and Scenes Server Models.


.. _ble-mesh-faq-node-development:

2. Node Development
-------------------

2.1 What kind of models are included by nodes?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - In ESP-BLE-MESH, nodes are all composed of a series of models with each model implements some functions of the node.

    - Model has two types, client model and server model. Client model can get and set the states of server model.

    - Model can also be divided into SIG model and vendor model. All behaviors of SIG models are officially defined while behaviors of vendor models are defined by users.

2.2 Is the format of messages corresponding to each model fixed?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - Messages, which consist of opcode and payload, are divided by opcode.

    - The type and the format of the messages corresponding to models are both fixed, which means the messages transmitted between models are fixed.

2.3 Which functions can be used to send messages with the models of nodes?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - For client models, users can use the API :cpp:func:`esp_ble_mesh_client_model_send_msg` to send messages.

    - For server models, users can use the API :cpp:func:`esp_ble_mesh_server_model_send_msg` to send messages.

    - For publication, users call the API :cpp:func:`esp_ble_mesh_model_publish` to publish messages.

2.4 How to achieve the transmission of messages without packet loss?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Acknowledegd message is needed if users want to transmit messages without packet loss. The default time to wait for corresponding response is set in :ref:`CONFIG_BLE_MESH_CLIENT_MSG_TIMEOUT`. If the sender waits for the response until the timer expires, the corresponding timeout event would be triggered.

.. note::

    Response timeout can be set in the API :cpp:func:`esp_ble_mesh_client_model_send_msg`. The default value (4 seconds) would be applied if the parameter :code:`msg_timeout` is set to **0**.

2.5 How to send unacknowledged messages?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    For client models, users can use the API :cpp:func:`esp_ble_mesh_client_model_send_msg` with the parameter :code:`need_rsp` set to :code:`false` to send unacknowledged messages.

    For server models, the messages sent by using the API :cpp:func:`esp_ble_mesh_server_model_send_msg` are always unacknowledged messages.

2.6 How to add subscription address to models?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Subscription address can be added through Configuration Client Model.

2.7 What is the difference between messages sent and published by models?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Messages sent by calling the API :cpp:func:`esp_ble_mesh_client_model_send_msg` or :cpp:func:`esp_ble_mesh_server_model_send_msg` will be sent in the duration determined by the Network Transmit state.

    Messages published by calling the API :cpp:func:`esp_ble_mesh_model_publish` will be published determined by the Model Publication state. And the publication of messages is generally periodic or with a fixed number of counts. The publication period and publication count are controlled by the Model Publication state, and can be configured through Configuration Client Model.

2.8 How many bytes can be carried when sending unsegmented messages?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    The total payload length (which can be set by users) of unsegmented message is 11 octets, so if the opcode of the message is 2 octets, then the message can carry 9-octets of valid information. For vendor messages, due to the 3-octets opcode, the remaining payload length is 8 octets.

2.9 When should the :ref:`Relay <ble-mesh-terminology-features>` feature of nodes be enabled?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Users can enable the Relay feature of all nodes when nodes detected in the mesh network are sparse.

    For dense mesh network, users can choose to just enable the Relay feature of several nodes.

    And users can enable the Relay feature by default if the mesh network size is unknown.

2.10 When should the :ref:`Proxy <ble-mesh-terminology-features>` feature of node be enabled?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    If the unprovisioned device is expected to be provisioned by a phone, then it should enable the Proxy feature since almost all the phones do not support sending ESP-BLE-MESH packets through advertising bearer currently. And after the unprovisioned device is provisioned successfully and becoming a Proxy node, it will communicate with the phone using GATT bearer and using advertising bearer to communicate with other nodes in the mesh network.

2.11 How to use the Proxy filter?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    The Proxy filter is used to reduce the number of Network PDUs exchanged between a Proxy Client (e.g. the phone) and a Proxy Server (e.g. the node). And with the Proxy filter, Proxy Client can explicitly request to receive only mesh messages with certain destination addresses from Proxy Server.

2.12 When a message can be relayed by a Relay node?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    If a message need to be relayed, the following conditions should be met.

    - The message is in the mesh network.

    - The message is not sent to the unicast address of the node.

    - The value of TTL in the message is greater than 1.

2.13 If a message is segmented into several segments, should the other Relay nodes just relay when one of these segments is received or wait until the message is received completely?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Relay nodes will forward segments when one of them are received rather than keeping waiting until all the segments are received.

2.14 What is the principle of reducing power consumption using :ref:`Low Power <ble-mesh-terminology-features>` feature?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - When the radio is turned on for listening, the device is consuming energy. When low power feature of the node is enabled, it will turn off its radio in the most of the time.

    - And cooperation is needed between low power node and friend node, thus low power node can receive messages at an appropriate or lower frequency without the need to keep listening.

    - When there are some new messages for low power node, its friend node will store the messages for it. And low power node can poll friend nodes to see if there are new messages at a fixed interval.

2.15 How to continue the communication on the network after powering-down and powering-up again?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Enable the configuration :code:`Store ESP-BLE-MESH Node configuration persistently` in `menuconfig`.

2.16 How to send out the self-test results of nodes?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    It is recommended that nodes can publish its self-test results periodically through Health Server Model.

2.17 How to transmit information between nodes?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    One possible application scenario for transmitting information between nodes is that spray nodes would be triggered once smoke alarm detected high smoke concentration. There are two approaches in implementation.

    - Approach 1 is that spray node subscribes to a group address. When smoke alarm detects high smoke concentration, it will publish a message whose destination address is the group address which has been subscribed by spray node.

    - Approach 2 is that Provisioner can configure the unicast address of spray node to the smoke alarm. When high smoke concentration is detected, smoke alarm can use send messages to the spray node with the spray node's unicast address as the destination address.

2.18 Is gateway a must for nodes communication?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - Situation 1: nodes only communicate within the mesh network. In this situation, no gateway is need. ESP-BLE-MESH network is a flooded network, messages in the network have no fixed paths, and nodes can communicate with each other freely.

    - Situation 2: if users want to control the nodes remotely, for example turn on some nodes before getting home, then a gateway is needed.

2.19 When will the IV Update procedure be performed?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    IV Update procedure would be performed once sequence number of messages sent detected by the bottom layer of node reached a critical value.

2.20 How to perform IV Update procedure?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Nodes can perform IV Update procedure with Secure Network Beacon.


.. _ble-mesh-faq-ble-mesh-and-wi-fi-coexistence:

3. ESP-BLE-MESH and Wi-Fi Coexistence
-------------------------------------

3.1 Which modes does Wi-Fi support when it coexists with ESP-BLE-MESH?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Currently only Wi-Fi station mode supports the coexistence.

3.2 Why is the Wi-Fi throughput so low when Wi-Fi and ESP-BLE-MESH coexist?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    The `ESP32-DevKitC <../../hw-reference/get-started-devkitc>`_ board without PSRAM can run properly but the throughput of it is low since it has no PSRAM. When Bluetooth and Wi-Fi coexist, the throughput of ESP32-DevKitC with PSRAM can be stabilized to more than 1Mbps.

    And some configurations in menuconfig shall be enabled to support PSRAM.

        - :code:`ESP32-specific --> Support for external,SPI-connected RAM --> Try to allocate memories of Wi-Fi and LWIP...`
        - :code:`Bluetooth --> Bluedroid Enable --> BT/BLE will first malloc the memory from the PSRAM`
        - :code:`Bluetooth --> Bluedroid Enable --> Use dynamic memory allocation in BT/BLE stack.`
        - :code:`Bluetooth --> Bluetooth controller --> BLE full scan feature supported.`
        - :code:`Wi-Fi --> Software controls Wi-Fi/Bluetooth coexistence --> Wi-Fi`


.. _ble-mesh-faq-fast-provisioning:

4. Fast Provisioning
--------------------

4.1 Why is fast provisioning needed?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Normally when they are several unprovisioned devices, users can provision them one by one. But when it comes to a large number of unprovisioned devices (e.g. 100), provisioning them one by one will take huge amount of time. With fast provisioning, users can provision 100 unprovisioned devices in about 50 seconds.

4.2 Why EspBleMesh App would wait for a long time or keep waiting during fast provisioning?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    After the App provisioned one Proxy node, it will disconnect from the App during fast provisioning, and reconnect with the App when all the nodes are provisioned.

4.3 Why is the number of node addresses displayed in the App is more than that of existing node addresses?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Each time after a fast provisioning process, and before starting a new one, the node addresses in the App should be cleared, otherwise the number of the node address will be incorrect.

4.4 What is the usage of the **count** value which was input in EspBleMesh App?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    The **count** value is provided to the Proxy node which is provisioned by the App so as to determine when to start Proxy advertising in advance.

4.5 When will Configuration Client Model of the node running :example:`ble_mesh_fast_prov_server <bluetooth/esp_ble_mesh/ble_mesh_fast_provision/ble_mesh_fast_prov_server>` example start to work?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Configuration Client Model will start to work after the Temporary Provisioner functionality is enabled.

4.6 Will the Temporary Provisioner functionality be enabled all the time?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    After the nodes receive messages used to turn on/off lights, all the nodes will disable its Temporary Provisioner functionality and become nodes.


.. _ble-mesh-faq-log-help:

5. Log Help
-----------

You can find meaning of errors or warnings when they appear at the bottom of ESP-BLE-MESH stack.

5.1 What is the meaning of warning :code:`ran out of retransmit attempts`?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    When the node transmits a segmented message, and due to some reasons, the receiver doesn't receive the complete message. Then the node will retransmit the message. When the retransmission count reaches the maximum number, which is 4 currently, then this warning will appear.

5.2 What is the meaning of warning :code:`Duplicate found in Network Message Cache`?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    When the node receives a message, it will compare the message with the ones stored in the network cache. If the same has been found in the cache, which means it has been received before, then the message will be dropped.

5.3 What is the meaning of warning :code:`Incomplete timer expired`?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    When the node doesn't receive all the segments of a segmented message during a certain period (e.g. 10 seconds), then the Incomplete timer will expire and this warning will appear.

5.4 What is the meaning of warning :code:`No matching TX context for ack`?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    When the node receives a segment ack and it doesn't find any self-send segmented message related with this ack, then this warning will appear.

5.5 What is the meaning of warning :code:`No free slots for new incoming segmented messages`?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    When the node has no space for receiving new segmented message, this warning will appear. Users can make the space larger through the configuration :ref:`CONFIG_BLE_MESH_RX_SEG_MSG_COUNT`.

5.6 What is the meaning of error :code:`Model not bound to Appkey 0x0000`？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    When the node sends messages with a model and the model has not been bound to the AppKey with AppKey Index 0x000, then this error will appear.

5.7 What is the meaning of error :code:`Busy sending message to DST xxxx`?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

   This error means client model of the node has transmitted a message to the target node and now is waiting for a response, users can not send messages to the same node with the same unicast address. After the corresponding response is received or timer is expired, then another message can be sent.


.. _ble-mesh-faq-example-help:

6. Example Help
---------------

6.1 How are the ESP-BLE-MESH callback functions classified?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - The API :cpp:func:`esp_ble_mesh_register_prov_callback` is used to register callback function used to handle provisioning and networking related events.
    - The API :cpp:func:`esp_ble_mesh_register_config_client_callback` is used to register callback function used to handle Configuration Client Model related events.
    - The API :cpp:func:`esp_ble_mesh_register_config_server_callback` is used to register callback function used to handle Configuration Server Model related events.
    - The API :cpp:func:`esp_ble_mesh_register_health_client_callback` is used to register callback function used to handle Health Client Model related events.
    - The API :cpp:func:`esp_ble_mesh_register_health_server_callback` is used to register callback function used to handle Health Server Model related events.
    - The API :cpp:func:`esp_ble_mesh_register_generic_client_callback` is used to register callback function used to handle Generic Client Models related events.
    - The API :cpp:func:`esp_ble_mesh_register_light_client_callback` is used to register callback function used to handle Lighting Client Models related events.
    - The API :cpp:func:`esp_ble_mesh_register_sensor_client_callback` is used to register callback function used to handle Sensor Client Model related events.
    - The API :cpp:func:`esp_ble_mesh_register_time_scene_client_callback` is used to register callback function used to handle Time and Scenes Client Models related events.
    - The API :cpp:func:`esp_ble_mesh_register_custom_model_callback` is used to register callback function used to handle vendor model and unrealized server models related events.


.. _ble-mesh-faq-others:

7. Others
---------

7.1 How to print the message context?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    The examples use :cpp:func:`ESP_LOG_BUFFER_HEX` to print the message context while the ESP-BLE-MESH protocol stack uses :cpp:func:`bt_hex`.

7.2 Which API can be used to restart ESP32?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    The API :cpp:func:`esp_restart`.

7.3 How to monitor the remaining space of the stack of a task?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    The API :cpp:func:`vTaskList` can be used to print the remaining space of the task stack periodically.

7.4 How to change the level of log without changing the menuconfig output level?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    The API :cpp:func:`esp_log_level_set` can be used to change the log output level rather than using menuconfig to change it.
