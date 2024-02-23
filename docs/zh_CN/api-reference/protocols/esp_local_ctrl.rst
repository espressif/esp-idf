ESP 本地控制
================

:link_to_translation:`en:[English]`

概述
----

通过 ESP-IDF 的 ESP 本地控制 (**esp_local_ctrl**) 组件，可使用 HTTPS 或 BLE 协议控制 ESP 设备。通过一系列可配置的处理程序，该组件允许你对应用程序定义的读/写 **属性** 进行访问。

.. only:: SOC_BT_SUPPORTED

    通过 BLE 传输协议初始化 **esp_local_ctrl** 的过程如下：

        .. code-block:: c

            esp_local_ctrl_config_t config = {
                .transport = ESP_LOCAL_CTRL_TRANSPORT_BLE,
                .transport_config = {
                    .ble = & (protocomm_ble_config_t) {
                        .device_name  = SERVICE_NAME,
                        .service_uuid = {
                            /* LSB <---------------------------------------
                            * ---------------------------------------> MSB */
                            0x21, 0xd5, 0x3b, 0x8d, 0xbd, 0x75, 0x68, 0x8a,
                            0xb4, 0x42, 0xeb, 0x31, 0x4a, 0x1e, 0x98, 0x3d
                        }
                    }
                },
                .proto_sec = {
                    .version = PROTOCOM_SEC0,
                    .custom_handle = NULL,
                    .sec_params = NULL,
                },
                .handlers = {
                    /* User defined handler functions */
                    .get_prop_values = get_property_values,
                    .set_prop_values = set_property_values,
                    .usr_ctx         = NULL,
                    .usr_ctx_free_fn = NULL
                },
                /* Maximum number of properties that may be set */
                .max_properties = 10
            };

            /* Start esp_local_ctrl service */
            ESP_ERROR_CHECK(esp_local_ctrl_start(&config));


同样，对于 HTTP 传输：

    .. code-block:: c

        /* Set the configuration */
        httpd_ssl_config_t https_conf = HTTPD_SSL_CONFIG_DEFAULT();

        /* Load server certificate */
        extern const unsigned char servercert_start[] asm("_binary_servercert_pem_start");
        extern const unsigned char servercert_end[]   asm("_binary_servercert_pem_end");
        https_conf.servercert = servercert_start;
        https_conf.servercert_len = servercert_end - servercert_start;

        /* Load server private key */
        extern const unsigned char prvtkey_pem_start[] asm("_binary_prvtkey_pem_start");
        extern const unsigned char prvtkey_pem_end[]   asm("_binary_prvtkey_pem_end");
        https_conf.prvtkey_pem = prvtkey_pem_start;
        https_conf.prvtkey_len = prvtkey_pem_end - prvtkey_pem_start;

        esp_local_ctrl_config_t config = {
            .transport = ESP_LOCAL_CTRL_TRANSPORT_HTTPD,
            .transport_config = {
                .httpd = &https_conf
            },
            .proto_sec = {
                .version = PROTOCOM_SEC0,
                .custom_handle = NULL,
                .sec_params = NULL,
            },
            .handlers = {
                /* User defined handler functions */
                .get_prop_values = get_property_values,
                .set_prop_values = set_property_values,
                .usr_ctx         = NULL,
                .usr_ctx_free_fn = NULL
            },
            /* Maximum number of properties that may be set */
            .max_properties = 10
        };

        /* Start esp_local_ctrl service */
        ESP_ERROR_CHECK(esp_local_ctrl_start(&config));

你可以利用以下选项设置 ESP 本地控制的传输安全性：

1. ``PROTOCOM_SEC2``: 指定使用基于 SRP6a 的密钥交换和基于 AES-GCM 的端到端加密。这一选项通过增强的 PAKE 协议（即 SRP6a）提供了强大的安全保障，是最受欢迎的选项。
2. ``PROTOCOM_SEC1``: 指定使用基于 Curve25519 的密钥交换和基于 AES-CTR 的端到端加密。
3. ``PROTOCOM_SEC0``: 指定以明文（无安全性）方式交换数据。
4. ``PROTOCOM_SEC_CUSTOM``: 自定义安全需求。注意，使用这一选项时，必须提供 ``protocomm_security_t *`` 类型的 ``custom_handle``。

.. note::
    相应的安全方案需通过项目配置菜单启用。要了解详情，请参考 :doc:`Protocol Communication </api-reference/provisioning/protocomm>` 中关于启用 protocomm 安全版本的章节。

创建属性
--------

启用 **esp_local_ctrl** 后，可以为其添加属性。每个属性必须具有唯一的名称 ``name`` （字符串），还需具有类型 ``type`` （如枚举）、标记 ``flags`` （位域）和大小 ``size``。

如果希望属性值的长度可变（例如，字符串或字节流），则 ``size`` 值应保持为 0。对于有固定长度属性值的数据类型，如整型、浮点型等，将 ``size`` 字段设置为正确的值，有助于 **esp_local_ctrl** 对接收到的写入请求的参数进行内部检查。

``type`` 和 ``flags`` 字段的含义取决于具体的应用程序，它们可以是枚举、位域、甚至整型。可以使用 ``type`` 表示属性，用 ``flags`` 指定属性的特征。

例如，以下属性被用作时间戳。此处假设应用程序定义了 ``TYPE_TIMESTAMP`` 和 ``READONLY`` 来设置此处的 ``type`` 和 ``flags`` 字段：

    .. code-block:: c

        /* Create a timestamp property */
        esp_local_ctrl_prop_t timestamp = {
            .name        = "timestamp",
            .type        = TYPE_TIMESTAMP,
            .size        = sizeof(int32_t),
            .flags       = READONLY,
            .ctx         = func_get_time,
            .ctx_free_fn = NULL
        };

        /* Now register the property */
        esp_local_ctrl_add_property(&timestamp);


另外，此示例中还设置了一个 ctx 字段，指向自定义的 ``func_get_time()``，用于在属性的 get/set 处理程序中检索时间戳。

以下为 ``get_prop_values()`` 处理程序的一个示例，用于检索时间戳：

    .. code-block:: c

        static esp_err_t get_property_values(size_t props_count,
                                             const esp_local_ctrl_prop_t *props,
                                             esp_local_ctrl_prop_val_t *prop_values,
                                             void *usr_ctx)
        {
            for (uint32_t i = 0; i < props_count; i++) {
                ESP_LOGI(TAG, "Reading %s", props[i].name);
                if (props[i].type == TYPE_TIMESTAMP) {
                    /* Obtain the timer function from ctx */
                    int32_t (*func_get_time)(void) = props[i].ctx;

                    /* Use static variable for saving the value. This is essential because the value has to be valid even after this function returns. Alternative is to use dynamic allocation and set the free_fn field */
                    static int32_t ts = func_get_time();
                    prop_values[i].data = &ts;
                }
            }
            return ESP_OK;
        }


以下为 ``set_prop_values()`` 应用程序的一个示例,注意此示例是如何为只读属性限制写入操作：

    .. code-block:: c

        static esp_err_t set_property_values(size_t props_count,
                                             const esp_local_ctrl_prop_t *props,
                                             const esp_local_ctrl_prop_val_t *prop_values,
                                             void *usr_ctx)
        {
            for (uint32_t i = 0; i < props_count; i++) {
                if (props[i].flags & READONLY) {
                    ESP_LOGE(TAG, "Cannot write to read-only property %s", props[i].name);
                    return ESP_ERR_INVALID_ARG;
                } else {
                    ESP_LOGI(TAG, "Setting %s", props[i].name);

                    /* For keeping it simple, lets only log the incoming data */
                    ESP_LOG_BUFFER_HEX_LEVEL(TAG, prop_values[i].data,
                                             prop_values[i].size, ESP_LOG_INFO);
                }
            }
            return ESP_OK;
        }


完整示例请参见 :example:`protocols/esp_local_ctrl`。

客户端实现
--------------

在客户端的实现过程中，首先，通过支持的传输模式与设备建立 protocomm 会话，然后发送并接收 **esp_local_ctrl** 服务能够处理的 protobuf 信息。 **esp_local_ctrl** 服务会将这些信息转换为请求，并发起相应的处理程序 (set/get)。接着，为每个处理程序生成的响应会被再次打包到一条 protobuf 信息中，传输回客户端。

以下是 **esp_local_ctrl** 服务能够处理的各种 protobuf 信息：

1. ``get_prop_count`` : 返回服务支持的属性总数。
2. ``get_prop_values`` : 接受一个索引数组，并返回这些索引相对应的属性信息（名称、类型、标志）和属性值。
3. ``set_prop_values`` : 接受一个索引数组和一个新值数组，用于设置索引对应的属性值。

注意，在多个会话中，一个属性的索引可能相同，也可能不同。因此，客户端必须用唯一的属性名称来识别属性。每次建立新会话时，客户端都应首先调用 ``get_prop_count``，然后调用 ``get_prop_values``，为所有属性建立从索引到名称的映射。为一组属性调用 ``set_prop_values`` 时，必须先用创建的映射将名称转换为索引。如前所述，每次使用同一设备建立新会话时，客户端必须刷新该映射。

下面列出了 **esp_local_ctrl** 服务提供的各种 protocomm 端点：

.. list-table:: ESP本地控制服务提供的端点
   :widths: 10 25 50
   :header-rows: 1

   * - 端点名称 (BLE + GATT 服务器)
     - URI (HTTPS 服务器 + mDNS)
     - 描述
   * - esp_local_ctrl/version
     - `https://<mdns-hostname>.local/esp_local_ctrl/version`
     - 检索版本字符串
   * - esp_local_ctrl/control
     - `https://<mdns-hostname>.local/esp_local_ctrl/control`
     - 发送或接收控制信息


API 参考
----------

.. include-build-file:: inc/esp_local_ctrl.inc
