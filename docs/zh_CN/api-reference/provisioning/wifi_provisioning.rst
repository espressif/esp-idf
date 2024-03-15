Wi-Fi 配网
====================

:link_to_translation:`en:[English]`

概述
----------

该组件提供控制 Wi-Fi 配网服务的 API，可以通过 SoftAP 或低功耗蓝牙建立 :doc:`protocomm` 安全会话，接收和配置 Wi-Fi 凭证。通过一组 ``wifi_prov_mgr_`` API，可以快速实现配网服务，该服务具备必要功能、代码量少且足够灵活。

.. _wifi-prov-mgr-init:

初始化
^^^^^^^^^^^^^^

调用 :cpp:func:`wifi_prov_mgr_init()` 可以配置和初始化配网管理器，因此在调用任何其他 ``wifi_prov_mgr_`` API 之前必须先调用此函数。请注意，该管理器依赖于 ESP-IDF 的其他组件，包括 NVS、TCP/IP、Event Loop 和 Wi-Fi，以及可选的 mDNS，因此在调用之前必须先初始化这些组件。调用 :cpp:func:`wifi_prov_mgr_deinit()` 可以随时反初始化管理器。

    .. code-block:: c

        wifi_prov_mgr_config_t config = {
            .scheme = wifi_prov_scheme_ble,
            .scheme_event_handler = WIFI_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BTDM
        };

        ESP_ERROR_CHECK( wifi_prov_mgr_init(config) );


以下配置结构体 :cpp:type:`wifi_prov_mgr_config_t` 里包含的部分字段可用于指定特定管理器行为:

    * :cpp:member:`wifi_prov_mgr_config_t::scheme` - 用于指定配网方案。每个方案对应一种 protocomm 支持的传输模式，因此支持三个选项：

        * ``wifi_prov_scheme_ble`` - 使用低功耗蓝牙传输和 GATT 服务器来处理配网命令。

        * ``wifi_prov_scheme_softap`` - 使用 Wi-Fi SoftAP 传输和 HTTP 服务器来处理配网命令。

        * ``wifi_prov_scheme_console`` - 使用串口传输和控制台来处理配网命令。

    * :cpp:member:`wifi_prov_mgr_config_t::scheme_event_handler` - 为方案定义的专属事件处理程序。选择适当方案后，其专属事件处理程序支持管理器自动处理特定事项。目前，该选项不适用于 SoftAP 或基于控制台的配网方案，但对于低功耗蓝牙配网方案来说非常方便。因为蓝牙需要相当多内存才能正常工作，所以配网完成后，主应用程序需要使用低功耗蓝牙或经典蓝牙时，可能需要回收配网所占的全部或部分内存。此外，未来每当配网设备重启时，都需要再次回收内存。为了便于使用 ``wifi_prov_scheme_ble`` 选项，各方案定义了专属处理程序。设备会根据所选处理程序，在反初始化配网管理器时自动释放低功耗蓝牙、经典蓝牙或蓝牙双模的内存。可用选项包括：

        * ``WIFI_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BTDM`` - 同时释放经典蓝牙和低功耗蓝牙或蓝牙双模的内存，可以在主应用程序不需要蓝牙时使用该选项。

        * ``WIFI_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BLE`` - 只释放低功耗蓝牙的内存，可以在主应用程序需要经典蓝牙时使用该选项。

        * ``WIFI_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BT`` - 仅释放经典蓝牙的内存，可以在主应用程序需要低功耗蓝牙时使用该选项，内存会在初始化管理器时立即释放。

        * ``WIFI_PROV_EVENT_HANDLER_NONE`` - 不使用任何特定方案的专属处理程序。以下情况可使用该选项：不使用低功耗蓝牙配网方案，即使用 SoftAP 或控制台方案；主应用程序需要自行回收内存；主应用程序需要同时使用低功耗蓝牙和经典蓝牙。

    * :cpp:member:`wifi_prov_mgr_config_t::app_event_handler` （不推荐）- 目前建议使用默认的事件循环处理程序捕获生成的 ``WIFI_PROV_EVENT``。关于配网服务生成事件的列表，请参阅 ``wifi_prov_cb_event_t`` 的定义。以下是配网事件示例摘录：

    .. code-block:: c

        static void event_handler(void* arg, esp_event_base_t event_base,
                                  int event_id, void* event_data)
        {
            if (event_base == WIFI_PROV_EVENT) {
                switch (event_id) {
                    case WIFI_PROV_START:
                        ESP_LOGI(TAG, "Provisioning started");
                        break;
                    case WIFI_PROV_CRED_RECV: {
                        wifi_sta_config_t *wifi_sta_cfg = (wifi_sta_config_t *)event_data;
                        ESP_LOGI(TAG, "Received Wi-Fi credentials"
                                 "\n\tSSID     : %s\n\tPassword : %s",
                                 (const char *) wifi_sta_cfg->ssid,
                                 (const char *) wifi_sta_cfg->password);
                        break;
                    }
                    case WIFI_PROV_CRED_FAIL: {
                        wifi_prov_sta_fail_reason_t *reason = (wifi_prov_sta_fail_reason_t *)event_data;
                        ESP_LOGE(TAG, "Provisioning failed!\n\tReason : %s"
                                 "\n\tPlease reset to factory and retry provisioning",
                                 (*reason == WIFI_PROV_STA_AUTH_ERROR) ?
                                 "Wi-Fi station authentication failed" : "Wi-Fi access-point not found");
                        break;
                    }
                    case WIFI_PROV_CRED_SUCCESS:
                        ESP_LOGI(TAG, "Provisioning successful");
                        break;
                    case WIFI_PROV_END:
                        /*配网完成后，反初始化管理器。*/
                        wifi_prov_mgr_deinit();
                        break;
                    default:
                        break;
                }
            }
        }

调用 :cpp:func:`wifi_prov_mgr_deinit()` 可以随时反初始化管理器。

.. _wifi-prov-check-state:

检查配网状态
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

在运行时，可以调用 :cpp:func:`wifi_prov_mgr_is_provisioned()` 检查设备是否配网完成，该函数会在内部检查 Wi-Fi 凭据是否存储在 NVS 中。

请注意，目前管理器并没有自己的 NVS 命名空间来存储 Wi-Fi 凭据，而是依赖 ``esp_wifi_`` API 来设置和获取存储在默认位置的 NVS 中的凭据。

可以采用以下任一方法重置配网状态：

    * 手动擦除 NVS 分区的配网相关部分。
    * 主应用程序必须实现某种逻辑，以在运行时调用 ``esp_wifi_`` API 来擦除凭据。
    * 主应用程序必须实现某种逻辑，以在不考虑配网状态的情况下，强制启动配网。

    .. code-block:: c

        bool provisioned = false;
        ESP_ERROR_CHECK( wifi_prov_mgr_is_provisioned(&provisioned) );


启动配网服务
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

在启动配网服务时，需要指定服务名称和相应密钥，即：

    * 使用 ``wifi_prov_scheme_softap`` 方案时，服务名称对应 Wi-Fi SoftAP 的 SSID，密钥对应密码。
    * 使用 ``wifi_prov_scheme_ble`` 方案时，服务名称对应低功耗蓝牙设备名称，无需指定密钥。

此外，由于管理器内部使用了 ``protocomm``，可以选择其提供的任一安全功能：

    * Security 1 是安全通信，该安全通信需要先握手，其中涉及 X25519 密钥交换和使用所有权证明 ``pop`` 完成身份验证，随后使用 AES-CTR 加密或解密后续消息。
    * Security 0 是纯文本通信，会直接忽略 ``pop``。

关于安全功能的更多详情，请参阅 :doc:`provisioning`。

    .. highlight:: c

    ::

        const char *service_name = "my_device";
        const char *service_key  = "password";

        wifi_prov_security_t security = WIFI_PROV_SECURITY_1;
        const char *pop = "abcd1234";

        ESP_ERROR_CHECK( wifi_prov_mgr_start_provisioning(security, pop, service_name, service_key) );


如果收到有效的 Wi-Fi AP 凭据，且设备成功连接到该 AP 并获取了 IP，配网服务会自动结束。此外，调用 :cpp:func:`wifi_prov_mgr_stop_provisioning()` 可以随时停止配网服务。

.. note::

    如果设备使用提供的凭据无法连接，则它不再接受新的凭据，但在设备重新启动前，配网服务仍然会继续运行，并向客户端传递连接失败的信息。设备重新启动后配网状态将变为已配网，因为在 NVS 中找到了凭据，但除非出现与凭据匹配的可用 AP，否则设备仍然无法使用原凭据进行连接。可以通过重置 NVS 中的凭据或强制启动配网服务来解决这个问题，详情请参阅上文 :ref:`wifi-prov-check-state`。


等待配网完成
^^^^^^^^^^^^^^^^^^^^^^

主应用程序通常会等待配网服务完成，然后反初始化管理器以释放资源，最后开始执行自己的逻辑。

有两种方法可以实现这一点，其中调用阻塞 :cpp:func:`wifi_prov_mgr_wait()` 更为简单。

    .. code-block:: c

        // 启动配网服务
        ESP_ERROR_CHECK( wifi_prov_mgr_start_provisioning(security, pop, service_name, service_key) );

        // 等待服务完成
        wifi_prov_mgr_wait();

        // 最后反初始化管理器
        wifi_prov_mgr_deinit();


另一种方法是使用默认的事件循环处理程序捕获 ``WIFI_PROV_EVENT`` 并在事件 ID 为 ``WIFI_PROV_END`` 时调用 :cpp:func:`wifi_prov_mgr_deinit()`：

    .. code-block:: c

        static void event_handler(void* arg, esp_event_base_t event_base,
                                  int event_id, void* event_data)
        {
            if (event_base == WIFI_PROV_EVENT && event_id == WIFI_PROV_END) {
                /* 配网完成后反初始化管理器 */
                wifi_prov_mgr_deinit();
            }
        }


用户端实现
^^^^^^^^^^^^^^^^^^^^^^^^

启动服务时，通过广播服务名称识别即将配网的设备。根据选择的传输方式，该服务名称为低功耗蓝牙设备的名称或 SoftAP SSID。

使用 SoftAP 传输方式时，为便于服务发现，必须在启动配网之前初始化 mDNS。在这种情况下，应使用主应用程序设置的主机名，并且在内部将服务类型设置为 ``_esp_wifi_prov``。

使用低功耗蓝牙传输方式时，应使用 :cpp:func:`wifi_prov_scheme_ble_set_service_uuid()` 设置一个自定义的 128 位 UUID。该 UUID 将包含在低功耗蓝牙广播中，并对应于提供配网端点作为 GATT 特征的主要服务。每个 GATT 特征都基于主要服务 UUID 形成，其中从第 0 个字节开始计数，第 12 和第 13 个字节为自动分配的不同字节。由于端点特征 UUID 自动分配，因此不应将其用于识别端点。客户端应用程序应通过读取每个特征的用户特征描述符 (``0x2901``) 来识别端点，该描述符包含特征的端点名称。例如，如果将服务 UUID 设置为 ``55cc035e-fb27-4f80-be02-3c60828b7451``，每个端点特征将分配到一个类似于 ``55cc____-fb27-4f80-be02-3c60828b7451`` 的 UUID，其中第 12 和第 13 个字节具有唯一值。

连接设备后，可以通过以下方式识别与配网相关的 protocomm 端点：

.. |br| raw:: html

    <br>

.. list-table:: 配网服务提供的端点
   :widths: 35 35 30
   :header-rows: 1

   * - 端点名称 |br| 即低功耗蓝牙 + GATT 服务器
     - URI 即 SoftAP |br| + HTTP 服务器 + mDNS
     - 描述
   * - prov-session
     - http://<mdns-hostname>.local/prov-session
     - 用于建立会话的安全端点
   * - prov-scan
     - http://wifi-prov.local/prov-scan
     - 用于启动 Wi-Fi 扫描和接收扫描结果的端点
   * - prov-ctrl
     - http://wifi-prov.local/prov-ctrl
     - 用于控制 Wi-Fi 配网状态的端点
   * - prov-config
     - http://<mdns-hostname>.local/prov-config
     - 用于在设备上配置 Wi-Fi 凭据的端点
   * - proto-ver
     - http://<mdns-hostname>.local/proto-ver
     - 用于获取版本信息的端点

连接后，客户端应用程序可以立即从 ``proto-ver`` 端点获取版本或属性信息。所有与此端点的通信均未加密，因此在建立安全会话前，可以检索相关必要信息，确保会话兼容。响应结果以 JSON 格式返回，格式类似于 ``prov: { ver:  v1.1, cap:  [no_pop] }, my_app: { ver:  1.345, cap:  [cloud, local_ctrl] },....``。其中 ``prov`` 标签提供了配网服务的版本 ``ver`` 和属性 ``cap``。目前仅支持 ``no_pop`` 属性，表示该服务不需要验证所有权证明。任何与应用程序相关的版本或属性将由其他标签给出，如本示例中的 ``my_app``。使用 :cpp:func:`wifi_prov_mgr_set_app_info()` 可以设置这些附加字段。

用户端应用程序需要根据所配置的安全方案实现签名握手，以建立和认证 protocomm 安全会话。当管理器配置为使用 protocomm security 0 时，则不需要实现签名握手。

关于安全握手和加密的详情，请参阅 :doc:`provisioning`。应用程序必须使用 :component:`protocomm/proto` 中的 ``.proto`` 文件。``.proto`` 文件定义了 ``prov-session`` 端点支持的 protobuf 消息结构。

建立会话后，以下 ``wifi_config`` 命令集可用于配置 Wi-Fi 凭据，这些命令会被序列化为 protobuf 消息，对应的 ``.proto`` 文件存放在 :component:`wifi_provisioning/proto` 中。

    * ``get_status`` - 用于查询 Wi-Fi 连接状态。设备响应状态为连接中、已连接或已断开。如果状态为已断开，则还会包含断开原因。

    * ``set_config`` - 用于设置 Wi-Fi 连接凭据。

    * ``apply_config`` - 用于应用先前保存的凭据，即由 ``set_config`` 设置的凭据，并启动 Wi-Fi 站点。

建立会话后，客户端还可以从设备请求 Wi-Fi 扫描结果。返回结果为 AP SSID 的列表，按信号强度降序排序。由此，客户端应用程序可以在设备配网时显示附近的 AP，并且用户可以选择其中一个 SSID 并提供密码，然后使用上述 ``wifi_config`` 命令发送密码。``wifi_scan`` 端点支持以下 protobuf 命令：

    * ``scan_start`` - 启动 Wi-Fi 扫描有多个选项，具体如下：

        * ``blocking`` （输入）- 如果参数为 true，则命令只会在扫描完成后返回。

        * ``passive`` （输入）- 如果参数为 true，则以被动模式启动扫描，扫描速度可能更慢。

        * ``group_channels`` （输入）- 该参数用于指定是否分组扫描。如果参数为 0，表示一次性扫描所有信道；如果参数为非零值，则表示分组扫描信道且参数值为每组中的信道数，每个连续组之间有 120 毫秒的延迟。分组扫描非常适用于使用 SoftAP 的传输模式，因为一次性扫描所有信道可能会导致 Wi-Fi 驱动没有足够时间发送信标，进而导致与部分站点断连。分组扫描时，管理器每扫描完一组信道，至少会等待 120 毫秒，确保驱动程序有足够时间发送信标。例如，假设共有 14 个 Wi-Fi 信道，将 ``group_channels`` 设置为 3 则将创建 5 个分组，每个分组包含 3 个信道，最后一个分组则为 14 除以 3 余下的 2 个信道。因此，扫描开始时，首先会扫描前 3 个信道，然后等待 120 毫秒，再继续扫描后 3 个信道，以此类推，直到扫描完 14 个信道。可以根据实际情况调整此参数，因为分组中信道数量过少可能会增加整体扫描时间，而信道数量过多则可能会导致连接再次断开。大多数情况下，将参数值设置为 4 即可。请注意，对于低功耗蓝牙等其他传输模式，可以放心将该参数设置为 0，从而在最短时间内完成扫描。

        * ``period_ms`` （输入）- 该扫描参数用于设置在每个信道上的等待时间。

    * ``scan_status`` - 可以返回扫描过程的状态：

        * ``scan_finished`` （输出）- 扫描完成时，该参数返回为 true。

        * ``result_count`` （输出）- 该参数返回到目前为止获取的结果总数。如果扫描仍在进行，该数字会不断更新。

    * ``scan_result`` - 用于获取扫描结果。即使扫描仍在进行，也可以调用此函数。

        * ``start_index`` （输入）- 从结果列表中获取条目的起始索引位置。

        * ``count`` （输入）- 从起始索引位置获取的条目数目。

        * ``entries`` （输出）- 返回条目的列表。每个条目包含 ``ssid``、``channel`` 和 ``rssi`` 信息。

客户端还可以使用 ``wifi_ctrl`` 端点来控制设备的配网状态。``wifi_ctrl`` 端点支持的 protobuf 命令如下：

    * ``ctrl_reset`` - 仅在配网失败时，重置设备的内部状态机并清除已配置的凭据。

    * ``ctrl_reprov`` - 仅在设备已成功配网的前提下，设备需要重新配网获取新的凭据时，重置设备的内部状态机并清除已配置的凭据。

附加端点
^^^^^^^^^^^^^^^^^^^^

如果用户想要根据自己的需求定制一些附加 protocomm 端点，可以通过两步完成。第一步是创建一个具有特定名称的端点，第二步是为该端点注册一个处理程序。关于端点处理程序的函数签名，请参阅 :doc:`protocomm`。自定义端点必须在初始化后、配网服务启动之前创建，但只能在配网服务启动后为该端点注册 protocomm 处理程序。注意在自定义端点处理程序的函数中，应使用堆来分配响应这些 protocomm 端点的内存，因为一旦传输层发送完毕，protocomm 层就会释放这部分内存。

    .. code-block:: c

        wifi_prov_mgr_init(config);
        wifi_prov_mgr_endpoint_create("custom-endpoint");
        wifi_prov_mgr_start_provisioning(security, pop, service_name, service_key);
        wifi_prov_mgr_endpoint_register("custom-endpoint", custom_ep_handler, custom_ep_data);


配网服务停止时，端点会自动取消注册。

在运行时，可以调用 :cpp:func:`wifi_prov_mgr_endpoint_unregister()` 来手动停用某个端点。该函数也可以用于停用配网服务使用的内部端点。

何时以及如何停止配网服务？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

当设备使用 ``apply_config`` 命令设置的 Wi-Fi 凭据成功连接，配网服务将默认停止，并在响应下一个 ``get_status`` 命令后自动关闭低功耗蓝牙或 softAP。如果设备没有收到 ``get_status`` 命令，配网服务将在超时 30 秒后停止。

如果设备因 SSID 或密码不正确等原因无法使用 Wi-Fi 凭据成功连接，配网服务将继续运行，并通过 ``get_status`` 命令持续响应为断连状态，并提供断连原因。此时设备不会再接受任何新的 Wi-Fi 凭据。除非强制启动配网服务或擦除 NVS 存储，这些凭据将保留。

可以调用 :cpp:func:`wifi_prov_mgr_disable_auto_stop()` 来禁用默认设置。禁用后，只有在显式调用 :cpp:func:`wifi_prov_mgr_stop_provisioning()` 之后，配网服务才会停止，且该函数会安排一个任务来停止配网服务，之后立即返回。配网服务将在一定延迟后停止，并触发 ``WIFI_PROV_END`` 事件。该延迟时间可以由 :cpp:func:`wifi_prov_mgr_disable_auto_stop()` 的参数指定。

如果需要在成功建立 Wi-Fi 连接后的某个时间再停止配网服务，应用程序可以采取定制行为。例如，如果应用程序需要设备连接到某个云服务并获取另一组凭证，继而通过自定义 protocomm 端点交换凭证，那么成功完成此操作后，可以在 protocomm 处理程序中调用 :cpp:func:`wifi_prov_mgr_stop_provisioning()` 来停止配网服务。设定适当的延迟时间可以确保 protocomm 处理程序的响应到达客户端应用程序后，才释放传输资源。

应用程序示例
---------------------

关于完整实现示例，请参阅 :example:`provisioning/wifi_prov_mgr`。

配网工具
--------------------

以下为各平台相应的配网应用程序，并附带源代码：

* Android:
    * `Play Store 上的低功耗蓝牙配网应用程序 <https://play.google.com/store/apps/details?id=com.espressif.provble>`_。
    * `Play Store 上的 SoftAP 配网应用程序 <https://play.google.com/store/apps/details?id=com.espressif.provsoftap>`_。
    * GitHub 上的源代码： `esp-idf-provisioning-android <https://github.com/espressif/esp-idf-provisioning-android>`_。
* iOS:
    * `App Store 上的低功耗蓝牙配网应用程序 <https://apps.apple.com/in/app/esp-ble-provisioning/id1473590141>`_。
    * `App Store 上的 SoftAP 配网应用程序 <https://apps.apple.com/in/app/esp-softap-provisioning/id1474040630>`_。
    * GitHub 上的源代码： `esp-idf-provisioning-ios <https://github.com/espressif/esp-idf-provisioning-ios>`_。
* Linux/MacOS/Windows: 基于 Python 的命令行工具 :idf:`tools/esp_prov`，可用于设备配网。

手机应用程序界面简洁，便于用户使用，而开发者可以使用命令行应用程序，便于调试。


API 参考
-------------

.. include-build-file:: inc/manager.inc
.. include-build-file:: inc/scheme_ble.inc
.. include-build-file:: inc/scheme_softap.inc
.. include-build-file:: inc/scheme_console.inc
.. include-build-file:: inc/wifi_config.inc
