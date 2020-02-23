ESP-MESH 编程指南
==========================

:link_to_translation:`en:[English]`

这是 ESP-MESH 的编程指南，包括 API 参考和编码示例。本指南分为以下部分：

1. :ref:`mesh-programming-model`

2. :ref:`mesh-writing-mesh-application`

3. :ref:`mesh-self-organized-behavior`

4. :ref:`mesh-application-examples`

5. :ref:`mesh-api-reference`

有关 ESP-MESH 协议的文档，请见 :doc:`ESP-MESH API 指南 <../../api-guides/mesh>`。有关 ESP-MESH 开发框架的更多内容，请见 `ESP-MESH 开发框架 <https://github.com/espressif/esp-mdf>`_。


.. ----------------------ESP-MESH 编程模型 --------------------------

.. _mesh-programming-model:

ESP-MESH 编程模型
--------------------------

软件栈
^^^^^^^^^^^^^^

ESP-MESH 软件栈基于 Wi-Fi 驱动程序和 FreeRTOS 构建，某些情况下（如根节点）也会使用 LwIP 软件栈。下图展示了 ESP-MESH 软件栈。

.. _mesh-going-to-software-stack:

.. figure:: ../../../_static/mesh-software-stack.png
    :align: center
    :alt: ESP-MESH 软件栈
    :figclass: align-center

    ESP-MESH 软件栈

.. _mesh-events:

系统事件
^^^^^^^^^^^^^

应用程序可通过 **ESP-MESH 事件** 与 ESP-MESH 交互。由于 ESP-MESH 构建在 Wi-Fi 软件栈之上，因此也可以通过 **Wi-Fi 事件任务** 与 Wi-Fi 驱动程序进行交互。下图展示了 ESP-MESH 应用程序中各种系统事件的接口。

.. figure:: ../../../_static/mesh-events-delivery.png
    :align: center
    :alt: ESP-MESH 系统事件交付
    :figclass: align-center

    ESP-MESH 系统事件交付

:cpp:type:`mesh_event_id_t` 定义了所有可能的 ESP-MESH 事件，并且可以指示父节点和子节点的连接或断开等事件。应用程序如需使用 ESP-MESH 事件，则必须通过 :cpp:func:`esp_event_handler_register` 将 **Mesh 事件处理程序** 注册在默认事件任务中。注册完成后，ESP-MESH 事件将包含与应用程序所有相关事件相关的处理程序。

Mesh 事件的典型应用场景包括：使用 :cpp:enumerator:`MESH_EVENT_PARENT_CONNECTED` 和 :cpp:enumerator:`MESH_EVENT_CHILD_CONNECTED` 事件来指示节点何时可以分别开始传输上行和下行的数据。同样，也可以使用 :cpp:enumerator:`IP_EVENT_STA_GOT_IP` 和 :cpp:enumerator:`IP_EVENT_STA_LOST_IP` 事件来指示根节点何时可以向外部 IP 网络传输数据。

.. warning::
    在自组网模式下使用 ESP-MESH 时，用户必须确保不得调用 Wi-Fi API。原因在于：自组网模式将在内部调用 Wi-Fi API 实现连接/断开/扫描等操作。 **此时，如果外部应用程序调用 Wi-Fi API（包括来自回调函数和 Wi-Fi 事件处理程序的调用）都可能会干扰 ESP-MESH 的自组网行为**。因此，用户不应该在 :cpp:func:`esp_mesh_start` 和 :cpp:func:`esp_mesh_stop` 之间调用 Wi-Fi API。

LwIP & ESP-MESH
^^^^^^^^^^^^^^^

应用程序无需通过 LwIP 层便可直接访问 ESP-MESH 软件栈，LwIP 层仅在根节点和外部 IP 网络的数据发送与接收时会用到。但是，由于每个节点都有可能成为根节点（由于自动根节点选择机制的存在），每个节点仍必须初始化 LwIP 软件栈。

**每个节点都需要通过调用** :cpp:func:`tcpip_adapter_init` **初始化 LwIP 软件栈**。 为了防止非根节点访问 LwIP，应用程序应该在 LwIP 初始化完成后停止以下服务：

- SoftAP 接口上的 DHCP 服务器服务。
- Station 接口上的 DHCP 客户端服务。

下方代码片段展示如何为 ESP-MESH 应用程序进行 LwIP 初始化。

.. code-block:: c

    /*  tcpip 初始化 */
    tcpip_adapter_init();
    /*
    * 对于 MESH
    * 默认情况下，在 SoftAP 接口上停止 DHCP 服务器
    * 默认情况下，在 Station 接口上停止 DHCP 客户端
    */
    ESP_ERROR_CHECK(tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP));
    ESP_ERROR_CHECK(tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_STA));

.. note::

    ESP-MESH 的根节点必须与路由器连接。因此，当一个节点成为根节点时，**该节点对应的处理程序必须启动 DHCP 客户端服务并立即获取 IP 地址。** 这样做将允许其他节点开始向/从外部 IP 网络发送/接收数据包。但是，如果使用静态 IP 设置，则不需要执行此步骤。


.. ---------------------- Writing a Mesh Application --------------------------

.. _mesh-writing-mesh-application:

编写 ESP-MESH 应用程序
-------------------------------

ESP-MESH 在正常启动前必须先初始化 LwIP 和 Wi-Fi 软件栈。下方代码展示了 ESP-MESH 在开始自身初始化前必须完成的步骤。

.. code-block:: c

    tcpip_adapter_init();
    /*
    * 对于 MESH
    * 默认情况下，在 SoftAP 接口上停止 DHCP 服务器
    * 默认情况下，在 Station 接口上停止 DHCP 客户端
    */
    ESP_ERROR_CHECK(tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP));
    ESP_ERROR_CHECK(tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_STA));

    /*事件初始化*/
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /*Wi-Fi 初始化 */
    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&config));
    /*注册 IP 事件处理程序 */
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &ip_event_handler, NULL));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_FLASH));
    ESP_ERROR_CHECK(esp_wifi_start());

在完成 LwIP 和 Wi-Fi 的初始化后，需完成以下三个步骤以启动并运行 ESP-MESH。

1. :ref:`mesh-initialize-mesh`
2. :ref:`mesh-configuring-mesh`
3. :ref:`mesh-start-mesh`

.. _mesh-initialize-mesh:

初始化 Mesh 
^^^^^^^^^^^^^^^

下方代码片段展示如何初始化 ESP-MESH。

.. code-block:: c

    /*Mesh 初始化 */
    ESP_ERROR_CHECK(esp_mesh_init());
    /*注册 mesh 事件处理程序 */
    ESP_ERROR_CHECK(esp_event_handler_register(MESH_EVENT, ESP_EVENT_ANY_ID, &mesh_event_handler, NULL));

.. _mesh-configuring-mesh:

配置 ESP-MESH 网络
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. todo - Add note about unified configuration

ESP-MESH 可通过 :cpp:func:`esp_mesh_set_config` 进行配置，并使用 :cpp:type:`mesh_cfg_t` 结构体传递参数。该结构体包含以下 ESP-MESH 的配置参数：

+------------------------------+----------------------------------------------------------+
|             参数             |                           描述                           |
+------------------------------+----------------------------------------------------------+
|        Channel（信道）       | 1 到 14 信道                                             |
+------------------------------+----------------------------------------------------------+
|            Mesh ID           | ESP-MESH 网络的 ID，见 :cpp:type:`mesh_addr_t`。         |
+------------------------------+----------------------------------------------------------+
|       Router（路由器）       | 路由器配置，见 :cpp:type:`mesh_router_t`。               |
+------------------------------+----------------------------------------------------------+
|            Mesh AP           | Mesh AP 配置，见 :cpp:type:`mesh_ap_cfg_t`               |
+------------------------------+----------------------------------------------------------+
| Crypto Functions（加密函数） | Mesh IE 的加密函数，见 :cpp:type:`mesh_crypto_funcs_t`。 |
+------------------------------+----------------------------------------------------------+

下方代码片段展示如何配置 ESP-MESH。

.. code-block:: c

    /* 默认启用 MESH IE 加密 */
    mesh_cfg_t cfg = MESH_INIT_CONFIG_DEFAULT();
    /* Mesh ID */
    memcpy((uint8_t *) &cfg.mesh_id, MESH_ID, 6);
    /* 信道（需与路由器信道匹配）*/
    cfg.channel = CONFIG_MESH_CHANNEL;
    /* 路由器 */
    cfg.router.ssid_len = strlen(CONFIG_MESH_ROUTER_SSID);
    memcpy((uint8_t *) &cfg.router.ssid, CONFIG_MESH_ROUTER_SSID, cfg.router.ssid_len);
    memcpy((uint8_t *) &cfg.router.password, CONFIG_MESH_ROUTER_PASSWD,
        strlen(CONFIG_MESH_ROUTER_PASSWD));
    /* Mesh softAP */
    cfg.mesh_ap.max_connection = CONFIG_MESH_AP_CONNECTIONS;
    memcpy((uint8_t *) &cfg.mesh_ap.password, CONFIG_MESH_AP_PASSWD,
        strlen(CONFIG_MESH_AP_PASSWD));
    ESP_ERROR_CHECK(esp_mesh_set_config(&cfg));

.. _mesh-start-mesh:

启动 Mesh
^^^^^^^^^^

下方代码片段展示如何启动 ESP-MESH。

.. code-block:: c

    /* 启动 Mesh */
    ESP_ERROR_CHECK(esp_mesh_start());

启动 ESP-MESH 后，应用程序应检查 ESP-MESH 事件，以确定它是何时连接到网络的。连接后，应用程序可使用 :cpp:func:`esp_mesh_send` 和 :cpp:func:`esp_mesh_recv` 在 ESP-MESH 网络中发送、接收数据包。


.. --------------------- ESP-MESH 应用程序示例 ------------------------

.. _mesh-self-organized-behavior:

自组网
-------------------------

自组网是 ESP-MESH 的功能之一，允许节点自动扫描/选择/连接/重新连接到其他节点和路由器。此功能允许 ESP-MESH 网络具有很高的自主性，可适应变化的动态网络拓扑结构和环境。启用自组网功能后，ESP-MESH 网络中的节点能够自主完成以下操作：

- 选择或选举根节点（见 :ref:`mesh-building-a-network` 中的 **自动根节点选择**）
- 选择首选的父节点（见 :ref:`mesh-building-a-network` 中的 **父节点选择**）
- 网络断开时自动重新连接（见 :ref:`mesh-managing-a-network` 中的 **中间父节点失败**）

启用自组网功能后，ESP-MESH 软件栈将内部调用 Wi-Fi API。因此，**在启用自组网功能时，应用层不得调用 Wi-Fi API，否则会干扰 ESP-MESH 的工作。**

开关自组网
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

应用程序可以在运行时通过调用 :cpp:func:`esp_mesh_set_self_organized` 函数，启用或禁用自组网功能。该函数具有以下两个参数：

- ``bool enable`` 指定启用或禁用自组网功能。

- ``bool select_parent`` 指定在启用自组网功能时是否应选择新的父节点。根据节点类型和节点当前状态，选择新的父节点具有不同的作用。在禁用自组网功能时，此参数不使用。

禁用自组网
"""""""""""""""""""""""""""""""""""
下方代码片段展示了如何禁用自组网功能。

.. code-block:: c

    //禁用自组网
    esp_mesh_set_self_organized(false, false);

ESP-MESH 将在禁用自组网时尝试维护节点的当前 Wi-Fi 状态。

- 如果节点先前已连接到其他节点，则将保持连接。
- 如果节点先前已断开连接并且正在扫描父节点或路由器，则将停止扫描。
- 如果节点以前尝试重新连接到父节点或路由器，则将停止重新连接。

启用自组网
""""""""""""""""""""""""""""""""""

ESP-MESH 将尝试在启用自组网时保持节点的当前 Wi-Fi 状态。但是，根据节点类型以及是否选择了新的父节点，节点的 Wi-Fi 状态可能会发生变化。下表显示了启用自组网的效果。

+----------------+--------------+------------------------------------------------------------------------------------------------------------------------+
| 是否选择父节点 | 是否为根结点 |                                                          作用                                                          |
+----------------+--------------+------------------------------------------------------------------------------------------------------------------------+
|        N       |       N      | 已连接到父节点的节点将保持连接。                                                                                       |
|                |              +------------------------------------------------------------------------------------------------------------------------+
|                |              | 之前扫描父节点的节点将停止扫描。调用 :cpp:func:`esp_mesh_connect` 重新启动。                                           |
|                +--------------+------------------------------------------------------------------------------------------------------------------------+
|                |       Y      | 已连接到路由器的根节点将保持连接。                                                                                     |
|                |              +------------------------------------------------------------------------------------------------------------------------+
|                |              | 从路由器断开的根结点需调用 :cpp:func:`esp_mesh_connect` 进行重连。                                                     |
+----------------+--------------+------------------------------------------------------------------------------------------------------------------------+
|        Y       |       N      | 没有父节点的节点将自动选择首选父节点并连接。                                                                           |
|                |              +------------------------------------------------------------------------------------------------------------------------+
|                |              | 已连接到父节点的节点将断开连接，重新选择首选父节点并进行重连。                                                         |
|                +--------------+------------------------------------------------------------------------------------------------------------------------+
|                |       Y      | 根结点在连接至父节点前必须放弃“根结点”的角色。因此，根节点将断开与路由器和所有子节点的连接，选择首选父节点并进行连接。 |
+----------------+--------------+------------------------------------------------------------------------------------------------------------------------+

下方代码片段展示了如何启用自组网功能。

.. code-block:: c

    //启用自组网，并选择一个新的父节点
    esp_mesh_set_self_organized(true, true);

    ...

    //启用自组网并手动重新连接
    esp_mesh_set_self_organized(true, false);
    esp_mesh_connect();


调用 Wi-Fi API
^^^^^^^^^^^^^^^^

在有些情况下，应用程序可能希望在使用 ESP-MESH 期间调用 Wi-Fi API。例如，应用程序可能需要手动扫描邻近的接入点 (AP)。**但在应用程序调用任何 Wi-Fi API 之前，必须先禁用自组网。** 否则，ESP-MESH 软件栈可能会同时调用 Wi-Fi API，进而影响应用程序的正常调用。

应用程序不应在 :cpp:func:`esp_mesh_set_self_organized` 之间调用 Wi-Fi API。下方代码片段展示了应用程序如何在 ESP-MESH 运行期间安全地调用  :cpp:func:`esp_wifi_scan_start`。

.. code-block:: c

    //禁用自组网
    esp_mesh_set_self_organized(0, 0);

    //停止任何正在进行的扫描
    esp_wifi_scan_stop();
    //手动启动扫描运行完成时自动停止
    esp_wifi_scan_start();

    //进程扫描结果

    ...

    //如果仍为连接状态，则重新启用自组网
    esp_mesh_set_self_organized(1, 0);

    ...

    //如果不为根节点且未连接，则重新启用自组网
    esp_mesh_set_self_organized(1, 1);

    ...

    //如果为根节点且未连接，则重新启用
    esp_mesh_set_self_organized(1, 0);  //不选择新的父节点
    esp_mesh_connect();                 //手动重新连接到路由器


.. --------------------- ESP-MESH 应用程序示例 ------------------------

.. _mesh-application-examples:

应用实例
--------------------

ESP-IDF 包含以下 ESP-MESH 示例项目：

:example:`内部通信示例 <mesh/internal_communication>` 展示了如何搭建 ESP-MESH 网络，并让根节点向网络中的每个节点发送数据包。

:example:`手动连网示例 <mesh/manual_networking>` 展示了如何在禁用自组网功能的情况下使用 ESP-MESH。此示例展示了如何对节点进行编程，以手动扫描潜在父节点的列表，并根据自定义标准选择父节点。


.. -------------------------ESP-MESH API 参考 ---------------------------

.. _mesh-api-reference:

API 参考
--------------

.. include-build-file:: inc/esp_mesh.inc

