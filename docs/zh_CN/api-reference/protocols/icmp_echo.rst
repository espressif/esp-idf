ICMP 回显
===========

:link_to_translation:`en:[English]`

概述
----

网际控制报文协议 (ICMP) 通常用于诊断或控制目的，或响应 IP 操作中的错误。常用的网络工具 ``ping`` 的应答，即 ``Echo Reply``，就是基于类型字段值为 0 的 ICMP 数据包实现的。

在 ping 会话中，首先由源主机发出请求包 (ICMP echo request)，然后等待应答包 (ICMP echo reply)，等待具有超时限制。通过这一过程，还能测量出信息的往返用时。收到有效的应答包后，源主机会生成 IP 链路层的统计数据（如失包率、运行时间等）。

IoT 设备通常需要检查远程服务器是否可用。如果服务器离线，设备应向用户发出警告。通过创建 ping 会话，定期发送或解析 ICMP echo 数据包，就能实现这一功能。

为简化这一过程方便用户操作，ESP-IDF 提供了一些好用的 API。

创建 ping 会话
^^^^^^^^^^^^^^^^^^^^^^^^^^^

要创建 ping 会话，首先需填写 ``esp_ping_config_t``，指定目标芯片 IP 地址、间隔时间等配置。此外，还可以通过 ``esp_ping_callbacks_t`` 注册回调函数。

创建 ping 会话并注册回调函数示例：

.. code-block:: c

    static void test_on_ping_success(esp_ping_handle_t hdl, void *args)
    {
        // optionally, get callback arguments
        // const char* str = (const char*) args;
        // printf("%s\r\n", str); // "foo"
        uint8_t ttl;
        uint16_t seqno;
        uint32_t elapsed_time, recv_len;
        ip_addr_t target_addr;
        esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
        esp_ping_get_profile(hdl, ESP_PING_PROF_TTL, &ttl, sizeof(ttl));
        esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
        esp_ping_get_profile(hdl, ESP_PING_PROF_SIZE, &recv_len, sizeof(recv_len));
        esp_ping_get_profile(hdl, ESP_PING_PROF_TIMEGAP, &elapsed_time, sizeof(elapsed_time));
        printf("%d bytes from %s icmp_seq=%d ttl=%d time=%d ms\n",
               recv_len, inet_ntoa(target_addr.u_addr.ip4), seqno, ttl, elapsed_time);
    }

    static void test_on_ping_timeout(esp_ping_handle_t hdl, void *args)
    {
        uint16_t seqno;
        ip_addr_t target_addr;
        esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
        esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
        printf("From %s icmp_seq=%d timeout\n", inet_ntoa(target_addr.u_addr.ip4), seqno);
    }

    static void test_on_ping_end(esp_ping_handle_t hdl, void *args)
    {
        uint32_t transmitted;
        uint32_t received;
        uint32_t total_time_ms;

        esp_ping_get_profile(hdl, ESP_PING_PROF_REQUEST, &transmitted, sizeof(transmitted));
        esp_ping_get_profile(hdl, ESP_PING_PROF_REPLY, &received, sizeof(received));
        esp_ping_get_profile(hdl, ESP_PING_PROF_DURATION, &total_time_ms, sizeof(total_time_ms));
        printf("%d packets transmitted, %d received, time %dms\n", transmitted, received, total_time_ms);
    }

    void initialize_ping()
    {
        /* convert URL to IP address */
        ip_addr_t target_addr;
        struct addrinfo hint;
        struct addrinfo *res = NULL;
        memset(&hint, 0, sizeof(hint));
        memset(&target_addr, 0, sizeof(target_addr));
        getaddrinfo("www.espressif.com", NULL, &hint, &res);
        struct in_addr addr4 = ((struct sockaddr_in *) (res->ai_addr))->sin_addr;
        inet_addr_to_ip4addr(ip_2_ip4(&target_addr), &addr4);
        freeaddrinfo(res);

        esp_ping_config_t ping_config = ESP_PING_DEFAULT_CONFIG();
        ping_config.target_addr = target_addr;          // target IP address
        ping_config.count = ESP_PING_COUNT_INFINITE;    // ping in infinite mode, esp_ping_stop can stop it

        /* set callback functions */
        esp_ping_callbacks_t cbs;
        cbs.on_ping_success = test_on_ping_success;
        cbs.on_ping_timeout = test_on_ping_timeout;
        cbs.on_ping_end = test_on_ping_end;
        cbs.cb_args = "foo";  // arguments that will feed to all callback functions, can be NULL
        cbs.cb_args = eth_event_group;

        esp_ping_handle_t ping;
        esp_ping_new_session(&ping_config, &cbs, &ping);
    }

启动和停止 ping 会话
^^^^^^^^^^^^^^^^^^^^^^^^^^

使用 ``esp_ping_new_session`` 返回的句柄可以启动或停止 ping 会话。注意，ping 会话在创建后不会自动启动。如果 ping 会话停止后重启，ICMP 数据包的序列号会归零重新计数。


删除 ping 会话
^^^^^^^^^^^^^^^^^^^

如果不再使用 ping 会话，可用 ``esp_ping_delete_session`` 将其删除。在删除 ping 会话时，确保该会话已处于停止状态（即已调用了 ``esp_ping_stop`` ，或该会话已完成所有步骤）。


获取运行时间数据
^^^^^^^^^^^^^^^^^^

在回调函数中调用 ``esp_ping_get_profile``，可获取 ping 会话的不同运行时间数据，如上文代码示例所示。


应用示例
----------

ICMP echo 示例： :example:`protocols/icmp_echo`

API 参考
--------------

.. include-build-file:: inc/ping_sock.inc
