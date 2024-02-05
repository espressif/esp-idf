协议通信
======================

:link_to_translation:`en:[English]`

概述
----------

协议通信 (protocomm) 组件用于管理安全会话并为多种传输提供框架。应用程序还可以直接使用 protocomm 层来增加特定扩展，用于配网或非配网使用场景。

以下功能可用于配网：

    * 应用程序层面的通信安全

        * ``protocomm_security0`` （无安全功能）
        * ``protocomm_security1`` （Curve25519 密钥交换 + AES-CTR 加密/解密）
        * ``protocomm_security2`` （基于 SRP6a 的密钥交换 + AES-GCM 加密/解密）
    * 所有权验证 (Proof-of-possession)（仅 ``protocomm_security1`` 支持该功能）
    * 盐值和验证器 (Salt and Verifier)（仅 ``protocomm_security2`` 支持该功能）

在 protocomm 内部，protobuf（协议缓冲区）用于建立安全会话。用户可以自行选择（即使在不使用 Protobuf 的情况下）实现安全性，也可以在没有任何安全层的情况下使用协议。

Protocomm 为以下各种传输提供框架：

.. list::

    :SOC_BLE_SUPPORTED: - 低功耗蓝牙
    :SOC_WIFI_SUPPORTED: - Wi-Fi (SoftAP + HTTPD)
    - 控制台：使用该传输方案时，设备端会自动调用处理程序。相关代码片段，请参见下文传输示例。

请注意，对于 ``protocomm_security1`` 和 ``protocomm_security2``，客户端仍需要执行双向握手来建立会话。

.. only:: SOC_WIFI_SUPPORTED

    关于安全握手逻辑的详情，请参阅 :doc:`provisioning`。

.. _enabling-protocomm-security-version:

启用 protocomm 安全版本
-----------------------------------

关于启用/禁用相应的安全版本，请参阅 protocomm 组件的项目配置菜单。相应配置选项如下：

    * 支持 ``protocomm_security0``，该版本无安全功能：:ref:`CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_0`，该选项默认启用。
    * 支持 ``protocomm_security1``，使用 Curve25519 密钥交换和 AES-CTR 加密/解密：:ref:`CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_1`，该选项默认启用。
    * 支持 ``protocomm_security2``，使用基于 SRP6a 的密钥交换和 AES-GCM 加密/解密：:ref:`CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_2`。

.. note::

    启用多个安全版本后可以动态控制安全版本，但也会增加固件大小。

.. only:: SOC_WIFI_SUPPORTED

    使用 Security 2 的 SoftAP + HTTP 传输方案示例
    --------------------------------------------------

    示例用法请参阅 :component_file:`wifi_provisioning/src/scheme_softap.c`。

    .. highlight:: c

    ::

        /* 此为将通过 protocomm 注册的端点处理程序，会直接回显接收到的数据 */
        esp_err_t echo_req_handler (uint32_t session_id,
                                    const uint8_t *inbuf, ssize_t inlen,
                                    uint8_t **outbuf, ssize_t *outlen,
                                    void *priv_data)
        {
            /* Session ID 可以用于持久化 */
            printf("Session ID : %d", session_id);

            /* 回显接收到的数据 */
            *outlen = inlen;            /* 输出更新后的数据长度 */
            *outbuf = malloc(inlen);    /* 将在外部释放 */
            memcpy(*outbuf, inbuf, inlen);

            /* 端点创建时传递的私有数据 */
            uint32_t *priv = (uint32_t *) priv_data;
            if (priv) {
                printf("Private data : %d", *priv);
            }

            return ESP_OK;
        }

        static const char sec2_salt[] = {0xf7, 0x5f, 0xe2, 0xbe, 0xba, 0x7c, 0x81, 0xcd};
        static const char sec2_verifier[] = {0xbf, 0x86, 0xce, 0x63, 0x8a, 0xbb, 0x7e, 0x2f, 0x38, 0xa8, 0x19, 0x1b, 0x35,
            0xc9, 0xe3, 0xbe, 0xc3, 0x2b, 0x45, 0xee, 0x10, 0x74, 0x22, 0x1a, 0x95, 0xbe, 0x62, 0xf7, 0x0c, 0x65, 0x83, 0x50,
            0x08, 0xef, 0xaf, 0xa5, 0x94, 0x4b, 0xcb, 0xe1, 0xce, 0x59, 0x2a, 0xe8, 0x7b, 0x27, 0xc8, 0x72, 0x26, 0x71, 0xde,
            0xb2, 0xf2, 0x80, 0x02, 0xdd, 0x11, 0xf0, 0x38, 0x0e, 0x95, 0x25, 0x00, 0xcf, 0xb3, 0x3f, 0xf0, 0x73, 0x2a, 0x25,
            0x03, 0xe8, 0x51, 0x72, 0xef, 0x6d, 0x3e, 0x14, 0xb9, 0x2e, 0x9f, 0x2a, 0x90, 0x9e, 0x26, 0xb6, 0x3e, 0xc7, 0xe4,
            0x9f, 0xe3, 0x20, 0xce, 0x28, 0x7c, 0xbf, 0x89, 0x50, 0xc9, 0xb6, 0xec, 0xdd, 0x81, 0x18, 0xf1, 0x1a, 0xd9, 0x7a,
            0x21, 0x99, 0xf1, 0xee, 0x71, 0x2f, 0xcc, 0x93, 0x16, 0x34, 0x0c, 0x79, 0x46, 0x23, 0xe4, 0x32, 0xec, 0x2d, 0x9e,
            0x18, 0xa6, 0xb9, 0xbb, 0x0a, 0xcf, 0xc4, 0xa8, 0x32, 0xc0, 0x1c, 0x32, 0xa3, 0x97, 0x66, 0xf8, 0x30, 0xb2, 0xda,
            0xf9, 0x8d, 0xc3, 0x72, 0x72, 0x5f, 0xe5, 0xee, 0xc3, 0x5c, 0x24, 0xc8, 0xdd, 0x54, 0x49, 0xfc, 0x12, 0x91, 0x81,
            0x9c, 0xc3, 0xac, 0x64, 0x5e, 0xd6, 0x41, 0x88, 0x2f, 0x23, 0x66, 0xc8, 0xac, 0xb0, 0x35, 0x0b, 0xf6, 0x9c, 0x88,
            0x6f, 0xac, 0xe1, 0xf4, 0xca, 0xc9, 0x07, 0x04, 0x11, 0xda, 0x90, 0x42, 0xa9, 0xf1, 0x97, 0x3d, 0x94, 0x65, 0xe4,
            0xfb, 0x52, 0x22, 0x3b, 0x7a, 0x7b, 0x9e, 0xe9, 0xee, 0x1c, 0x44, 0xd0, 0x73, 0x72, 0x2a, 0xca, 0x85, 0x19, 0x4a,
            0x60, 0xce, 0x0a, 0xc8, 0x7d, 0x57, 0xa4, 0xf8, 0x77, 0x22, 0xc1, 0xa5, 0xfa, 0xfb, 0x7b, 0x91, 0x3b, 0xfe, 0x87,
            0x5f, 0xfe, 0x05, 0xd2, 0xd6, 0xd3, 0x74, 0xe5, 0x2e, 0x68, 0x79, 0x34, 0x70, 0x40, 0x12, 0xa8, 0xe1, 0xb4, 0x6c,
            0xaa, 0x46, 0x73, 0xcd, 0x8d, 0x17, 0x72, 0x67, 0x32, 0x42, 0xdc, 0x10, 0xd3, 0x71, 0x7e, 0x8b, 0x00, 0x46, 0x9b,
            0x0a, 0xe9, 0xb4, 0x0f, 0xeb, 0x70, 0x52, 0xdd, 0x0a, 0x1c, 0x7e, 0x2e, 0xb0, 0x61, 0xa6, 0xe1, 0xa3, 0x34, 0x4b,
            0x2a, 0x3c, 0xc4, 0x5d, 0x42, 0x05, 0x58, 0x25, 0xd3, 0xca, 0x96, 0x5c, 0xb9, 0x52, 0xf9, 0xe9, 0x80, 0x75, 0x3d,
            0xc8, 0x9f, 0xc7, 0xb2, 0xaa, 0x95, 0x2e, 0x76, 0xb3, 0xe1, 0x48, 0xc1, 0x0a, 0xa1, 0x0a, 0xe8, 0xaf, 0x41, 0x28,
            0xd2, 0x16, 0xe1, 0xa6, 0xd0, 0x73, 0x51, 0x73, 0x79, 0x98, 0xd9, 0xb9, 0x00, 0x50, 0xa2, 0x4d, 0x99, 0x18, 0x90,
            0x70, 0x27, 0xe7, 0x8d, 0x56, 0x45, 0x34, 0x1f, 0xb9, 0x30, 0xda, 0xec, 0x4a, 0x08, 0x27, 0x9f, 0xfa, 0x59, 0x2e,
            0x36, 0x77, 0x00, 0xe2, 0xb6, 0xeb, 0xd1, 0x56, 0x50, 0x8e};

        /* 通过 HTTP 启动 protocomm 实例的示例函数 */
        protocomm_t *start_pc()
        {
            protocomm_t *pc = protocomm_new();


            /* 配置 protocomm_httpd_start() */
            protocomm_httpd_config_t pc_config = {
                .data = {
                .config = PROTOCOMM_HTTPD_DEFAULT_CONFIG()
                }
            };

            /* 启动基于 HTTP 的 protocomm 服务器 */
            protocomm_httpd_start(pc, &pc_config);

            /* 从盐值和验证器创建 security2 参数对象。该对象必须在 protocomm 端点作用域内有效，且无需为静态对象，即可以在删除端点时动态分配和释放。*/
            const static protocomm_security2_params_t sec2_params = {
                .salt = (const uint8_t *) salt,
                .salt_len = sizeof(salt),
                .verifier = (const uint8_t *) verifier,
                .verifier_len = sizeof(verifier),
            };

            /* 在应用程序层面为通信设置安全方案。与请求处理程序类似，设置安全方案会创建一个端点，并注册 protocomm_security1 提供的处理程序。也可以使用 protocomm_security0 进行类似操作。单个 protocomm 实例中一次只能设置一种类型的安全方案。*/
            protocomm_set_security(pc, "security_endpoint", &protocomm_security2, &sec2_params);

            /* 传递给端点的私有数据必须在 protocomm 端点作用域内有效。该数据无需为静态数据，即可以在删除端点时动态分配和释放。*/
            static uint32_t priv_data = 1234;

            /* 为 protocomm 实例添加一个新端点，该端点由唯一名称标识，再注册一个处理函数，在执行函数时传递私有数据。只要端点由唯一名称标识，即可添加多个端点。*/
            protocomm_add_endpoint(pc, "echo_req_endpoint",
                                   echo_req_handler, (void *) &priv_data);
            return pc;
        }

        /* 停止 protocomm 实例的示例函数 */
        void stop_pc(protocomm_t *pc)
        {
            /* 移除由其唯一名称标识的端点 */
            protocomm_remove_endpoint(pc, "echo_req_endpoint");

            /* 移除由其名称标识的安全端点 */
            protocomm_unset_security(pc, "security_endpoint");

            /* 停止 HTTP 服务器 */
            protocomm_httpd_stop(pc);

            /* 删除（即释放）protocomm 实例 */
            protocomm_delete(pc);
        }

    使用 Security 1 的 SoftAP + HTTP 传输方案示例
    -------------------------------------------------

    示例用法请参阅 :component_file:`wifi_provisioning/src/scheme_softap.c`。

    .. highlight:: c

    ::

        /* 此为将通过 protocomm 注册的端点处理程序，会直接回显接收到的数据 */
        esp_err_t echo_req_handler (uint32_t session_id,
                                    const uint8_t *inbuf, ssize_t inlen,
                                    uint8_t **outbuf, ssize_t *outlen,
                                    void *priv_data)
        {
            /* Session ID 可以用于持久化 */
            printf("Session ID : %d", session_id);

            /* 回显接收到的数据 */
            *outlen = inlen;            /* 输出更新后的数据长度 */
            *outbuf = malloc(inlen);    /* 将在外部释放 */
            memcpy(*outbuf, inbuf, inlen);

            /* 端点创建时传递的私有数据 */
            uint32_t *priv = (uint32_t *) priv_data;
            if (priv) {
                printf("Private data : %d", *priv);
            }

            return ESP_OK;
        }

        /* 通过 HTTP 启动 protocomm 实例的示例函数 */
        protocomm_t *start_pc(const char *pop_string)
        {
            protocomm_t *pc = protocomm_new();


            /* 配置 protocomm_httpd_start() */
            protocomm_httpd_config_t pc_config = {
                .data = {
                .config = PROTOCOMM_HTTPD_DEFAULT_CONFIG()
                }
            };

            /* 启动基于 HTTP 的 protocomm 服务器 */
            protocomm_httpd_start(pc, &pc_config);

            /* 从 pop_string 创建 security1 参数对象。该对象必须在 protocomm 端点作用域内有效，且无需为静态对象，即可以在删除端点时动态分配和释放。*/
            const static protocomm_security1_params_t sec1_params = {
                .data = (const uint8_t *) strdup(pop_string),
                .len = strlen(pop_string)
            };

            /* 在应用程序层面为通信设置安全方案。与请求处理程序类似，设置安全方案会创建一个端点，并注册 protocomm_security1 提供的处理程序。也可以使用 protocomm_security0 进行类似操作。单个 protocomm 实例中一次只能设置一种类型的安全方案*/
            protocomm_set_security(pc, "security_endpoint", &protocomm_security1, &sec1_params);

            /* 传递给端点的私有数据必须在 protocomm 端点作用域内有效。该数据无需为静态数据，即可以在删除端点时动态分配和释放。*/
            static uint32_t priv_data = 1234;

            /* 为 protocomm 实例添加一个新端点，该端点由唯一名称标识，再注册一个处理函数，在执行函数时传递私有数据。只要端点由唯一名称标识，即可添加多个端点。*/
            protocomm_add_endpoint(pc, "echo_req_endpoint",
                                   echo_req_handler, (void *) &priv_data);
            return pc;
        }

        /* 停止 protocomm 实例的示例函数 */
        void stop_pc(protocomm_t *pc)
        {
            /* 移除由其唯一名称标识的端点 */
            protocomm_remove_endpoint(pc, "echo_req_endpoint");

            /* 移除由其名称标识的安全端点 */
            protocomm_unset_security(pc, "security_endpoint");

            /* 停止 HTTP 服务器 */
            protocomm_httpd_stop(pc);

            /* 删除（即释放）protocomm 实例 */
            protocomm_delete(pc);
        }

.. only:: SOC_BLE_SUPPORTED

    使用 Security 0 的低功耗蓝牙传输方案示例
    -------------------------------------------

    示例用法请参阅 :component_file:`wifi_provisioning/src/scheme_ble.c`。

    .. highlight:: c

    ::

        /* 通过低功耗蓝牙启动安全 protocomm 实例的示例函数 */
        protocomm_t *start_pc()
        {
            protocomm_t *pc = protocomm_new();

            /* 端点 UUID */
            protocomm_ble_name_uuid_t nu_lookup_table[] = {
                {"security_endpoint", 0xFF51},
                {"echo_req_endpoint", 0xFF52}
            };

            /* 配置 protocomm_ble_start() */
            protocomm_ble_config_t config = {
                .service_uuid = {
                    /* 最低有效位 <---------------------------------------
                    * ---------------------------------------> 最高有效位 */
                    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80,
                    0x00, 0x10, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00,
                },
                .nu_lookup_count = sizeof(nu_lookup_table)/sizeof(nu_lookup_table[0]),
                .nu_lookup = nu_lookup_table
            };

            /* 启动基于低功耗蓝牙的 protocomm 层 */
            protocomm_ble_start(pc, &config);

            /* protocomm_security0 方案不使用所有权证明，因此可以将其保持为 NULL */
            protocomm_set_security(pc, "security_endpoint", &protocomm_security0, NULL);
            protocomm_add_endpoint(pc, "echo_req_endpoint", echo_req_handler, NULL);
            return pc;
        }

        /* 停止 protocomm 实例的示例函数 */
        void stop_pc(protocomm_t *pc)
        {
            protocomm_remove_endpoint(pc, "echo_req_endpoint");
            protocomm_unset_security(pc, "security_endpoint");

            /* 停止低功耗蓝牙 protocomm 服务 */
            protocomm_ble_stop(pc);

            protocomm_delete(pc);
        }

API 参考
--------

.. include-build-file:: inc/protocomm.inc
.. include-build-file:: inc/protocomm_security.inc
.. include-build-file:: inc/protocomm_security0.inc
.. include-build-file:: inc/protocomm_security1.inc
.. include-build-file:: inc/protocomm_security2.inc
.. include-build-file:: inc/esp_srp.inc
.. include-build-file:: inc/protocomm_httpd.inc

.. only:: SOC_BLE_SUPPORTED

    .. include-build-file:: inc/protocomm_ble.inc
