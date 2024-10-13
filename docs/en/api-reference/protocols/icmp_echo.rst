ICMP Echo
=========

:link_to_translation:`zh_CN:[中文]`

Overview
--------

ICMP (Internet Control Message Protocol) is used for diagnostic or control purposes or generated in response to errors in IP operations. The common network util ``ping`` is implemented based on the ICMP packets with the type field value of 0, also called ``Echo Reply``.

During a ping session, the source host firstly sends out an ICMP echo request packet and wait for an ICMP echo reply with specific times. In this way, it also measures the round-trip time for the messages. After receiving a valid ICMP echo reply, the source host will generate statistics about the IP link layer (e.g., packet loss, elapsed time, etc).

It is common that IoT device needs to check whether a remote server is alive or not. The device should show the warnings to users when it got offline. It can be achieved by creating a ping session and sending or parsing ICMP echo packets periodically.

To make this internal procedure much easier for users, ESP-IDF provides some out-of-box APIs.

Create a New Ping Session
^^^^^^^^^^^^^^^^^^^^^^^^^

To create a ping session, you need to fill in the ``esp_ping_config_t`` configuration structure firstly, specifying target IP address, interval times, and etc. Optionally, you can also register some callback functions with the ``esp_ping_callbacks_t`` structure.

Example method to create a new ping session and register callbacks:

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
        cbs.cb_args = "foo";  // arguments that feeds to all callback functions, can be NULL
        cbs.cb_args = eth_event_group;

        esp_ping_handle_t ping;
        esp_ping_new_session(&ping_config, &cbs, &ping);
    }

Start and Stop Ping Session
^^^^^^^^^^^^^^^^^^^^^^^^^^^

You can start and stop ping session with the handle returned by ``esp_ping_new_session``. Note that, the ping session does not start automatically after creation. If the ping session is stopped, and restart again, the sequence number in ICMP packets will recount from zero again.


Delete a Ping Session
^^^^^^^^^^^^^^^^^^^^^

If a ping session will not be used any more, you can delete it with ``esp_ping_delete_session``. Please make sure the ping session is in stop state (i.e., you have called ``esp_ping_stop`` before or the ping session has finished all the procedures) when you call this function.


Get Runtime Statistics
^^^^^^^^^^^^^^^^^^^^^^

As the example code above, you can call ``esp_ping_get_profile`` to get different runtime statistics of ping session in the callback function.


Application Example
-------------------

ICMP echo example: :example:`protocols/icmp_echo`

API Reference
-------------

.. include-build-file:: inc/ping_sock.inc
