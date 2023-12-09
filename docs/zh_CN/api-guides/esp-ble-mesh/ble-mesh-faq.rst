ESP-BLE-MESH 常见问题手册
=========================

:link_to_translation:`en:[English]`

本文汇总了 ESP-BLE-MESH 协议栈开发的常见问题及解答，全文分为 7 个章节。

* :ref:`ble-mesh-faq-provisioner-development`
* :ref:`ble-mesh-faq-node-development`
* :ref:`ble-mesh-faq-ble-mesh-and-wi-fi-coexistence`
* :ref:`ble-mesh-faq-fast-provisioning`
* :ref:`ble-mesh-faq-log-help`
* :ref:`ble-mesh-faq-example-help`
* :ref:`ble-mesh-faq-others`

用户可以参考这些章节，快速找到问题的答案。该文档会根据各种渠道收集的反馈进行更新。


.. _ble-mesh-faq-provisioner-development:

1. Provisioner 开发
--------------------

通常而言，Provisioner 用于配网未配网设备并形成 mesh 网络。组网后，设备的角色变成节点。

1.1 未配网设备加入 ESP-BLE-MESH 网络的流程是什么？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    设备通过 Provisioner 加入 ESP-BLE-MESH 网络分为两个阶段，配网阶段和配置阶段。

    - 配网阶段：为设备分配单播地址、添加网络密钥 (NetKey) 等。通过配网，设备加入 ESP-BLE-MESH 网络，身份从未配网设备变为节点。

    - 配置阶段：为节点添加应用密钥 (AppKey), 并将应用密钥绑定到相应模型。配置期间，有些选项是可选的，比如为节点添加订阅地址、设置发布地址等。通过配置，该节点实际上可以向 Provisioner 发送消息，也可以接收来自 Provisioner 的消息。

1.2 如果 Provisioner 想要改变节点状态，其需满足什么条件？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - 需要有和节点的服务器模型相对应的客户端模型。
    - 需要和节点有相同的、可用于加密消息的网络密钥和应用密钥。
    - 需要知道节点的地址，可以是单播地址，也可以是订阅地址。

1.3 如何使用网络密钥和应用密钥？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - 网络密钥用于加密网络层的消息。具有相同网络密钥的节点视作在同一网络中，具有不同网络密钥的节点相互之间不能进行通信。
    - 应用密钥用于加密上层传输层中的消息。如果服务器模型和客户端模型绑定的应用密钥不同，则无法实现相互通信。

1.4 如何生成网络密钥或应用密钥？是否可以采用固定的网络密钥或应用密钥？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - API :cpp:func:`esp_ble_mesh_provisioner_add_local_net_key` 可以用来添加包含固定值或随机值的网络密钥。
    - API :cpp:func:`esp_ble_mesh_provisioner_add_local_app_key` 可以用来添加包含固定值或随机值的应用密钥。

1.5 Provisioner 的单播地址是不是固定的？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    :cpp:type:`esp_ble_mesh_prov_t` 中 :code:`prov_unicast_addr` 的值用于设置 Provisioner 的单播地址，只能在初始化期间设置一次，此后不能更改。

1.6 Provisioner 的地址是否可以作为节点上报状态消息的目的地址？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Provisioner 的单播地址只能在初始化期间设置一次，此后不能更改。理论而言，只要节点知道 Provisioner 的单播地址，此地址便可用作节点上报状态消息的目的地址。节点在网络配置的过程中可以知道 Provisioner 的单播地址，因为 Provisioner 往节点发送消息时，消息的源地址就是 Provisioner 的单播地址。

    订阅地址也可使用。Provisioner 订阅组地址或者虚拟地址，节点向该订阅地址发送消息。

1.7 被 Provisioner 配网到 ESP-BLE-MESH 网络中的第一个节点的单播地址是不是固定的？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    :cpp:type:`esp_ble_mesh_prov_t` 中 :code:`prov_start_address` 的值用于设置 Provisioner 配网未配网设备的起始地址，即其首先配网的节点的单播地址。单播地址只能在初始化期间设置一次，此后不能修改。

1.8 手机 App 首先配置的节点的单播地址是不是固定的？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    该 App 将确定单播地址，目前大多数单播地址是固定的。

1.9 如何知道当前 Provisioner 正在配网哪个未配网设备？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    :cpp:type:`esp_ble_mesh_prov_t` 中 :code:`prov_attention` 的值由 Provisioner 在配网过程中设置给未配网设备。该值只能在初始化期间设置一次，此后不能修改。未配网设备加入 mesh 网络后可以用特定的方式来显示自己正在配网，比如灯光闪烁，以告知 Provisioner 其正在配网。

1.10 配网过程中，认证设备共有多少种方法？提供的范例中 :example:`provided examples <bluetooth/esp_ble_mesh>` 使用了什么方法？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    共有四种设备认证方法，即 No OOB、Static OOB、Output OOB 和 Input OOB。提供的范例使用了 No OOB 的方式。

1.11 配置入网前，未配网设备的广播包可以携带哪些信息？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - Device UUID
    - OOB Info
    - URL Hash（可选的）

1.12 这些信息可以用于设备识别吗？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    是的。每个设备都有独一无二的 Device UUID, 用户可以通过 Device UUID 识别设备。

1.13 当 Provisioner 配网的节点包含多个元素时，单播地址是如何分配的？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - Provisioner 会给设备的主元素分配一个单播地址，其余元素的单播地址在此基础上递增。
    - 比如：如果一个未配网设备有三个元素，即主要元素、第二元素和第三元素。配网完成后，节点主元素的单播地址为 0x0002，节点第二元素的单播地址为 0x0003，节点第三元素的单播地址为 0x0004。

1.14 Provisioner 如何通过 Configuration Client Model 获取并且解析节点的 :ref:`构成数据 <ble-mesh-terminology-composition>` ？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - Provisioner 可以调用 :ref:`Configuration Client Model <ble-mesh-terminology-foundation-models>` API :cpp:func:`esp_ble_mesh_config_client_set_state` 设置参数，调用 :cpp:type:`esp_ble_mesh_cfg_client_get_state_t` 中的 :code:`comp_data_get` 获取节点的构成数据。
    - 用户可以参考以下代码解析 Composition Data:

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

1.15 Provisioner 如何通过获取的 Composition Data 进一步配置节点？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Provisioner 通过调用 :ref:`Configuration Client Model <ble-mesh-terminology-foundation-models>` API :cpp:func:`esp_ble_mesh_config_client_set_state` 来进行如下配置。

    - 正确设置参数 :cpp:type:`esp_ble_mesh_cfg_client_set_state_t` 中的 :code:`app_key_add`，将应用密钥添加到节点中。
    - 正确设置参数 :cpp:type:`esp_ble_mesh_cfg_client_set_state_t` 中的 :code:`model_sub_add`，将订阅地址添加到节点的模型中。
    - 正确设置参数 :cpp:type:`esp_ble_mesh_cfg_client_set_state_t` 中的 :code:`model_pub_set`，将发布地址添加到节点的模型中。

1.16 节点可以自己添加相应的配置吗？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    本法可用于特殊情况，如测试阶段。

    - 此示例展示了节点如何为自己的模型添加新的组地址。

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

    使能了节点的 NVS 存储器后，通过该方式添加的组地址以及绑定的应用密钥在设备掉电的情况下不能保存。这些配置信息只有通过 Configuration Client Model 配置时才会保存。

1.17 Provisioner 如何通过分组的方式控制节点？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    通常而言，在 ESP-BLE-MESH 网络中实现组控制有两种方法，即组地址方法和虚拟地址方法。假设有 10 个设备，即 5 个带蓝灯的设备和 5 个带红灯的设备。

    - 方案一：5 个蓝灯设备订阅一个组地址，5 个红灯设备订阅另一个组地址。Provisioner 往不同的组地址发送消息，即可实现分组控制设备。

    - 方案二：5 个蓝灯设备订阅一个虚拟地址，5 个红灯设备订阅另一个虚拟地址，Provisioner 往不同的虚拟地址发送消息，即可实现分组控制设备。

1.18 Provisioner 如何将节点添加至多个子网？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    节点配置期间，Provisioner 可以为节点添加多个网络密钥，拥有相同网络密钥的节点属于同一子网。Provisioner 可以通过不同的网络密钥与不同子网内的节点进行通信。

1.19 Provisioner 如何知道网络中的某个设备是否离线？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    节点离线通常定义为：电源故障或其他原因导致的节点无法与 mesh 网络中的其他节点正常通信的情况。

    ESP-BLE-MESH 网络中的节点间彼此不连接，它们通过广播通道进行通信。

    此示例展示了如何通过 Provisioner 检测节点是否离线。

    - 节点定期给 Provisioner 发送心跳包。如果 Provisioner 超过一定的时间未接收到心跳包，则视该节点离线。

.. note::

    心跳包的设计应该采用单包（字节数小于 11 个字节）的方式，这样收发效率会更高。

1.20 Provisioner 删除网络中的节点时，需要进行哪些操作？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    通常而言，Provisioner 从网络中移除节点主要涉及三个步骤：

    - 首先，Provisioner 将需要移除的节点添加至“黑名单”。

    - 其次，Provisioner 启动 :ref:`密钥更新程序 <ble-mesh-terminology-network-management>`。

    - 最后，节点执行节点重置程序，切换自身身份为未配网设备。

1.21 在密钥更新的过程中，Provisioner 如何更新节点的网络密钥？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - 通过正确设置参数 :cpp:type:`esp_ble_mesh_cfg_client_set_state_t` 中的 :code:`net_key_update`，使用 :ref:`Configuration Client Model <ble-mesh-terminology-foundation-models>` API :cpp:func:`esp_ble_mesh_config_client_set_state`，Provisioner 更新节点的网络密钥。

    - 通过正确设置参数 :cpp:type:`esp_ble_mesh_cfg_client_set_state_t` 中的 :code:`app_key_update`，使用 :ref:`Configuration Client Model <ble-mesh-terminology-foundation-models>` API :cpp:func:`esp_ble_mesh_config_client_set_state`，Provisioner 更新节点的应用密钥。

1.22 Provisioner 如何管理 mesh 网络中的节点？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    ESP-BLE-MESH 在示例中实现了一些基本的节点管理功能，比如 :cpp:func:`esp_ble_mesh_store_node_info`。
    ESP-BLE-MESH 还提供可用于设置节点本地名称的 API :cpp:func:`esp_ble_mesh_provisioner_set_node_name` 和可用于获取节点本地名称的 API :cpp:func:`esp_ble_mesh_provisioner_get_node_name`。

1.23 Provisioner 想要控制节点的服务器模型时需要什么？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Provisioner 在控制节点的服务器模型前，必须包括相应的客户端模型。

    Provisioner 应当添加本地的网络密钥和应用密钥。

    - Provisioner 调用 API :cpp:func:`esp_ble_mesh_provisioner_add_local_net_key` 以添加网络密钥。

    - Provisioner 调用 API :cpp:func:`esp_ble_mesh_provisioner_add_local_app_key` 以添加应用密钥。

    Provisioner 应当配置自己的客户端模型。

    - Provisioner 调用 API :cpp:func:`esp_ble_mesh_provisioner_bind_app_key_to_local_model` 以绑定应用密钥至自己的客户端模型。

1.24 Provisioner 如何控制节点的服务器模型？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    ESP-BLE-MESH 支持所有 SIG 定义的客户端模型。Provisioner 可以使用这些客户端模型控制节点的服务器模型。客户端模型分为 6 类，每类有相应的功能。

    - Configuration Client Model

        - API :cpp:func:`esp_ble_mesh_config_client_get_state` 可用于获取 Configuration Server Model 的 :cpp:type:`esp_ble_mesh_cfg_client_get_state_t` 值。
        - API :cpp:func:`esp_ble_mesh_config_client_set_state` 可用于设置 Configuration Server Model 的 :cpp:type:`esp_ble_mesh_cfg_client_set_state_t` 值。

    - Health Client Model

        - API :cpp:func:`esp_ble_mesh_health_client_get_state` 可用于获取 Health Server Model 的 :cpp:type:`esp_ble_mesh_health_client_get_state_t` 值。
        - API :cpp:func:`esp_ble_mesh_health_client_set_state` 可用于设置 Health Server Model 的 :cpp:type:`esp_ble_mesh_health_client_set_state_t` 值。

    - Generic Client Models

        - API :cpp:func:`esp_ble_mesh_generic_client_get_state` 可用于获取 Generic Server Model 的 :cpp:type:`esp_ble_mesh_generic_client_get_state_t` 值。
        - API :cpp:func:`esp_ble_mesh_generic_client_set_state` 可用于设置 Generic Server Model 的 :cpp:type:`esp_ble_mesh_generic_client_set_state_t` 值。

    - Lighting Client Models

        - API :cpp:func:`esp_ble_mesh_light_client_get_state` 可用于获取 Lighting Server Model 的 :cpp:type:`esp_ble_mesh_light_client_get_state_t` 值。
        - API :cpp:func:`esp_ble_mesh_light_client_set_state` 可用于设置 Lighting Server Model 的 :cpp:type:`esp_ble_mesh_light_client_set_state_t` 值。

    - Sensor Client Models

        - API :cpp:func:`esp_ble_mesh_sensor_client_get_state` 可用于获取 Sensor Server Model 的 :cpp:type:`esp_ble_mesh_sensor_client_get_state_t` 值。
        - API :cpp:func:`esp_ble_mesh_sensor_client_set_state` 可用于设置 Sensor Server Model 的 :cpp:type:`esp_ble_mesh_sensor_client_set_state_t` 值。

    - Time and Scenes Client Models
        - API :cpp:func:`esp_ble_mesh_time_scene_client_get_state` 可用于获取 Time and Scenes Server Model 的 :cpp:type:`esp_ble_mesh_time_scene_client_get_state_t` 值。
        - API :cpp:func:`esp_ble_mesh_time_scene_client_set_state` 可用于设置 Time and Scenes Server Model 的 :cpp:type:`esp_ble_mesh_time_scene_client_set_state_t` 值。


.. _ble-mesh-faq-node-development:

2. 节点开发
------------

2.1 节点包含什么样的模型？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - ESP-BLE-MESH 中，节点由一系列的模型组成，每个模型实现节点的某些功能。

    - 模型分为两种，客户端模型和服务器模型。客户端模型可以获取并设置服务器模型的状态。

    - 模型也可以分为 SIG 模型和自定义模型。SIG 模型的所有行为都由官方定义，而自定义模型的行为均由用户定义。

2.2 每个模型对应的消息格式是不是固定的？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - 消息由 opcode 和 payload 组成，通过 opcode 进行区分。

    - 与模型对应的消息的类型和格式都是固定的，这意味着模型之间传输的消息是固定的。

2.3 节点的模型可以使用哪些函数发送消息？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - 对于客户端模型，用户可以调用 API :cpp:func:`esp_ble_mesh_client_model_send_msg` 发送消息。

    - 对于服务器模型，用户可以调用 API :cpp:func:`esp_ble_mesh_server_model_send_msg` 发送消息。

    - 对于发布，用户可以调用 API :cpp:func:`esp_ble_mesh_model_publish` 发布消息。

2.4 如何实现消息传输不丢包？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    如果用户要实现消息传输不丢包，则需有应答的消息。等待应答的默认时间在 :ref:`CONFIG_BLE_MESH_CLIENT_MSG_TIMEOUT` 中设置。如果发送端等待应答超时，就会触发对应的超时事件。

.. note::

    API :cpp:func:`esp_ble_mesh_client_model_send_msg` 中可以设置应答的超时时间。如果参数 :code:`msg_timeout` 设为 **0**， 那么超时时间便会采用默认值（4 秒）。

2.5 如何发送无应答的消息？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    对于客户端模型，用户可以调用 API :cpp:func:`esp_ble_mesh_client_model_send_msg` with the parameter :code:`need_rsp` set to :code:`false` 发送无应答消息。

    对于服务器模型，调用 API :cpp:func:`esp_ble_mesh_server_model_send_msg` 发送的消息总是无应答的消息。

2.6 如何为模型添加订阅地址？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    通过 Configuration Client Model 添加订阅地址。

2.7 模型发送的消息和发布的消息有何不同？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    调用 API :cpp:func:`esp_ble_mesh_client_model_send_msg` 或 :cpp:func:`esp_ble_mesh_server_model_send_msg` 发送的消息会在 Network Transmit 状态规定的期限内发送。

    调用 API :cpp:func:`esp_ble_mesh_model_publish` 发布的消息将由模型发布状态决定是否发布。消息的发布一般是周期性的，或者有固定次数。发布周期和发布次数由模型发布状态控制，并且可以通过 Configuration Client Model 进行配置。

2.8 发送不分包消息时，最多可携带多少有效字节？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    不分包消息的总有效载荷长度（可由用户设置）为 11 个八位位组，因此，如果消息的 opcode 为 2 个八位位组，则该消息可以携带 9 个八位位组的有效信息。 对于 vendor 消息，由于 opcode 是 3 个八位位组，剩余的有效负载长度为 8 个八位位组。

2.9 什么时候应该使能节点的 :ref:`Relay <ble-mesh-terminology-features>` 功能？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    如果 mesh 网络中检测到的节点很稀疏，用户可以使能节点的 Relay 功能。

    如果 mesh 网络中检测到的节点很密集，用户可以选择仅使能一些节点的 Relay 功能。

    如果 mesh 网络大小未知，用户可以默认使能 Relay 功能。

2.10 什么时候应该使能节点的 :ref:`Proxy <ble-mesh-terminology-features>` 功能？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    如果未配网设备将由电话配网，则未配网设备应该使能 Proxy 功能，因为当前几乎所有电话都不支持通过广播承载层发送 ESP-BLE-MESH 数据包。并且，未配网设备成功配网成为 Proxy 节点后，其会通过 GATT 承载层和广播承载层与 mesh 网络中的其他节点通信。

2.11 如何使用代理过滤器?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    代理过滤器用于减少 Proxy Client（如手机）和 Proxy Server（如节点）之间交换的 Network PDU 的数量。另外，通过代理过滤器，Proxy Client 可以明确请求仅接收来自 Proxy Server 的某些目标地址的 mesh 消息。

2.12 Relay 节点什么时候可以中继消息？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    如果要中继消息，消息需满足以下要求。

    - 消息存在于 mesh 网络中。

    - 消息的目的地址不是节点的单播地址。

    - 消息的 TTL 值需大于 1。

2.13 如果一条消息分成几段，那么其他 Relay 节点是接收到一段消息就中继还是等接收到完整的数据包才中继？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Relay 节点收到其中一段消息时就中继，而非一直等到接收所有的消息。

2.14 使用 :ref:`Low Power <ble-mesh-terminology-features>` 功能降低功耗的原理是什么？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - 开启无线电进行收听时，设备消耗能量。使能节点的低功耗功能后，它将在大多数时间内关闭无线电功能。

    - 低功耗节点和好友节点需要合作，因此低功耗节点可以以适当或较低的频率接收消息，而无需一直收听。

    - 当低功耗节点有一些新消息时，好友节点将为其存储消息。低功耗节点可以间隔固定时间轮询好友节点，以查看是否有新的消息。

2.15 设备断电后上电，如何能继续在网络中进行通讯？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    在 `menuconfig` 中启用配置 :code:`Store BLE Mesh Node configuration persistently` 。

2.16 如何实现将节点自检的信息发送出来？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    推荐节点通过 Health Server Model 定期发布其自检结果。

2.17 节点间如何传输消息？
^^^^^^^^^^^^^^^^^^^^^^^^^^

    节点间传输信息的可能应用场景是，一旦烟雾警报检测到高浓度的烟雾，就会触发喷淋设备。 有两种实现方法。

    - 方法 1：喷淋设备订阅组地址。当烟雾警报器检测到高浓度的烟雾时，它会发布一条消息，该消息的目标地址是喷淋设备已订阅的组地址。

    - 方法 2：Provisioner 可以配置喷淋设备的单播地址为烟雾报警器的地址。当检测到高浓度的烟雾时，烟雾警报器以喷淋设备的单播地址为目标地址，将消息发送到喷淋设备。

2.18 设备通信必须要网关吗？
^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - 情况 1：节点仅在 mesh 网络内通信。这种情况下，不需要网关。ESP-BLE-MESH 网络是一个泛洪的网络，网络中的消息没有固定的路径，节点与节点之间可以随意通信.

    - 情况 2：如果用户想要远程控制网络，比如在到家之前打开某些节点，则需要网关。

2.19 何时使用 IV Update 更新程序？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    一旦节点的底层检测到发送的消息的序列号达到临界值，IV Update 更新程序便会启用。

2.20 如何启用 IV Update 更新程序？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    节点可以使用带有 Secure Network Beacon 的 IV Update 更新程序。


.. _ble-mesh-faq-ble-mesh-and-wi-fi-coexistence:

3. ESP-BLE-MESH 和 Wi-Fi 共存
-------------------------------

3.1 Wi-Fi 和 ESP-BLE-MESH 共存时，支持哪些模式？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    目前，只有 Wi-Fi STA 模式支持共存。

3.2 Wi-Fi 和 ESP-BLE-MESH 共存时，为什么 Wi-Fi 吞吐量很低？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    .. only:: esp32

        未搭载 PSRAM 的 :doc:`ESP32-DevKitC <../../hw-reference/esp32/get-started-devkitc>` 开发板，Wi-Fi 和 ESP-BLE-MESH 共存可以正常运行，但是吞吐率较低。当 Wi-Fi 和 ESP-BLE-MESH 共存时，搭载 PSRAM 的 ESP32-DevKitC 速率可以稳定在 1 Mbps 以上。

    应使能 menuconfig 中的一些配置来支持 PSRAM。

        - ``{IDF_TARGET_NAME}-specific`` > ``Support for external,SPI-connected RAM`` > ``Try to allocate memories of Wi-Fi and LWIP...``
        - ``Bluetooth`` > ``Bluedroid Enable`` > ``BT/BLE will first malloc the memory from the PSRAM``
        - ``Bluetooth`` > ``Bluedroid Enable`` > ``Use dynamic memory allocation in BT/BLE stack``
        - ``Bluetooth`` > ``Bluetooth controller`` > ``BLE full scan feature supported``
        - ``Wi-Fi`` > ``Software controls Wi-Fi/Bluetooth coexistence`` > ``Wi-Fi``


.. _ble-mesh-faq-fast-provisioning:

4. 快速配网
-----------

4.1 为什么需要快速配网？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

   通常而言，存在少量未配网设备时，用户可以逐个配置。但是如果有大量未配网设备（比如 100 个）时，逐个配置会耗费大量时间。通过快速配网，用户可以在约 50 秒内配网 100 个未配网设备。

4.2 为什么会出现 EspBleMesh App 在快速配网期间长时间等待的情况？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    快速配网期间，代理节点在配置完一个节点后会断开与 APP 的连接，待所有节点配网完成后再与 APP 重新建立连接。

4.3 为什么 APP 中显示的节点地址的数量比现有的节点地址更多？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    每完成一次快速配网后、开始新一次快速配网前，APP 会存有上次配网的数据，因此 APP 中显示的节点地址的数量比现有的节点地址更多。

4.4 在 EspBleMesh App 中输入的 **count** 值有什么用途？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    此 **count** 值提供给 App 配置的代理节点，以决定何时提前开始 Proxy 广播信息。

4.5 运行以下示例 :example:`fast_prov_server <bluetooth/esp_ble_mesh/fast_provisioning/fast_prov_server>` 的节点的 Configuration Client Model 何时开始工作？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    使能了 Temporary Provisioner 功能后，Configuration Client Model 会开始工作。

4.6 Temporary Provisioner 功能会一直处于使能的状态吗？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    节点收到打开/关闭电灯的消息后，所有节点会禁用其 Temporary Provisioner 功能并且转化为一般节点。


.. _ble-mesh-faq-log-help:

5. Log 帮助
-----------

当 ESP-BLE-MESH 协议栈底层出现错误或者警告时，可以在这儿找到这些错误和警告的含义。

5.1 ``ran out of retransmit attempts`` 代表什么？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    节点发送分段消息时，由于某些原因，接收端未收到完整的消息。节点会重传消息。当重传次数达到最大重传数时，会出现该警告，当前最大重传数为 4。

5.2 ``Duplicate found in Network Message Cache`` 代表什么？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    当节点收到一条消息时，它会把该消息与网络缓存中存储的消息进行比较。如果在缓存中找到相同的消息，这意味着之前已接受过该消息，则该消息会被丢弃。

5.3  ``Incomplete timer expired`` 代表什么？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    当节点在一定时间段（比如 10 秒）内未收到分段消息的所有段时，则 Incomplete 计时器到时，并且出现该警告。

5.4 ``No matching TX context for ack`` 代表什么？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    当节点收到一个分段 ack 且不能找到任何自己发送的与该 ack 相关的消息时，会出现该警告。

5.5  ``No free slots for new incoming segmented messages`` 代表什么？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    当节点没有空间来接收新的分段消息时，会出现该警告。用户可以通过配置 :ref:`CONFIG_BLE_MESH_RX_SEG_MSG_COUNT` 扩大空间。

5.6  ``Model not bound to AppKey 0x0000`` 代表什么？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    当节点发送带有模型的消息且该模型尚未绑定到索引为 0x000 的应用密钥时，会出现该报错。

5.7  ``Busy sending message to DST xxxx`` 代表什么？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

   该错误表示节点的客户端模型已将消息发送给目标节点，并且正在等待响应，用户无法将消息发送到单播地址相同的同一节点。接收到相应的响应或计时器到时后，可以发送另一条消息。


.. _ble-mesh-faq-example-help:

6. 示例帮助
-----------

6.1 ESP-BLE-MESH 回调函数如何分类？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - API :cpp:func:`esp_ble_mesh_register_prov_callback` 用于注册处理配网和入网相关事件的回调函数。
    - API :cpp:func:`esp_ble_mesh_register_config_client_callback` 用于注册处理 Configuration Client Model 相关事件的回调函数。
    - API :cpp:func:`esp_ble_mesh_register_config_server_callback` 用于注册处理 Configuration Server Model 相关事件的回调函数。
    - API :cpp:func:`esp_ble_mesh_register_health_client_callback` 用于注册处理 Health Client Model 相关事件的回调函数。
    - API :cpp:func:`esp_ble_mesh_register_health_server_callback` 用于注册处理 Health Server Model 相关事件的回调函数。
    - API :cpp:func:`esp_ble_mesh_register_generic_client_callback` 用于注册处理 Generic Client Models 相关事件的回调函数。
    - API :cpp:func:`esp_ble_mesh_register_light_client_callback` 用于注册处理 Lighting Client Models 相关事件的回调函数。
    - API :cpp:func:`esp_ble_mesh_register_sensor_client_callback` 用于注册处理 Sensor Client Model 相关事件的回调函数。
    - API :cpp:func:`esp_ble_mesh_register_time_scene_client_callback` 用于注册处理 Time and Scenes Client Models 相关事件的回调函数。
    - API :cpp:func:`esp_ble_mesh_register_custom_model_callback` 用于注册处理自定义模型和未实现服务器模型的相关事件的回调函数。


.. _ble-mesh-faq-others:

7. 其他
---------

7.1 如何打印数据包？
^^^^^^^^^^^^^^^^^^^^^^

    示例使用如下函数 :cpp:func:`ESP_LOG_BUFFER_HEX` 打印信息语境，而 ESP-BLE-MESH 协议栈使用 :cpp:func:`bt_hex` 打印。

7.2 重启 ESP32 应使用哪个 API？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    API :cpp:func:`esp_restart`.

7.3 如何监测任务栈的剩余空间？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    API :cpp:func:`vTaskList` 可以用于定期打印任务栈的剩余空间。

7.4 如何在不更改 menuconfig 输出级别的情况下改变 log 级别？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    无需使用 menuconfig，可以通过 API :cpp:func:`esp_log_level_set` 修改 log 的输出级别。
