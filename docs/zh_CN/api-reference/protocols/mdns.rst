mDNS 服务
=========
:link_to_translation:`en:[English]`

概述
----

mDNS 是一种组播 UDP 服务，用来提供本地网络服务和主机发现。

绝大多数的操作系统默认都会安装 mDNS 服务，或者提供单独的安装包。``Mac OS`` 默认会安装名为 ``Bonjour`` 的服务（该服务基于 mDNS），此外 Apple 还发布了适用于 Windows 系统的安装程序，可以在 `官方支持 <https://support.apple.com/downloads/bonjour%2520for%2520windows>`_ 找到。在 ``Linux`` 上，mDNS 服务由 `avahi <https://github.com/lathiat/avahi>`_ 提供，通常也会被默认安装。

mDNS 属性
^^^^^^^^^

    * ``hostname``：设备会去响应的主机名，如果没有设置，会根据设备的网络接口名定义 ``hostname`` 。例如，``my-esp32`` 会被解析为 ``my-esp32.local``。
    * ``default_instance``：默认实例名（即易记的设备名），例如 ``Jhon's ESP32 Thing``。如果没有设置，将会使用 ``hostname``。

以下为 STA 接口启动 mDNS 服务并设置 ``hostname`` 和 ``default_instance`` 的示例方法：

.. highlight:: c

::

    void start_mdns_service()
    {
        // 初始化 mDNS 服务
        esp_err_t err = mdns_init();
        if (err) {
            printf("MDNS Init failed: %d\n", err);
            return;
        }
    
        // 设置 hostname
        mdns_hostname_set("my-esp32");
        // 设置默认实例
        mdns_instance_name_set("Jhon's ESP32 Thing");
    }

mDNS 服务
^^^^^^^^^

mDNS 可以广播设备能够提供的网络服务的相关信息，每个服务会由以下属性构成。

    * ``instance_name``：实例名（即易记的服务名），例如 ``Jhon's ESP32 Web Server``。如果没有定义，会使用 ``default_instance``。
    * ``service_type``：（必需）服务类型，以下划线为前缀，`这里 <http://www.dns-sd.org/serviceTypes.html>`_ 列出了常见的类型。
    * ``proto``：（必需）服务运行所依赖的协议，以下划线为前缀，例如 ``_tcp`` 或者 ``_udp``。
    * ``port``：（必需）服务运行所用的端口号。
    * ``txt``：形如 ``{var, val}`` 的字符串数组，用于定义服务的属性。

添加一些服务和不同属性的示例方法::

    void add_mdns_services()
    {
        // 添加服务
        mdns_service_add(NULL, "_http", "_tcp", 80, NULL, 0);
        mdns_service_add(NULL, "_arduino", "_tcp", 3232, NULL, 0);
        mdns_service_add(NULL, "_myservice", "_udp", 1234, NULL, 0);
        
        // 注意：必须先添加服务，然后才能设置其属性
        // web 服务器使用自定义的实例名
        mdns_service_instance_name_set("_http", "_tcp", "Jhon's ESP32 Web Server");

        mdns_txt_item_t serviceTxtData[3] = {
            {"board","esp32"},
            {"u","user"},
            {"p","password"}
        };
        // 设置服务的文本数据（会释放并替换当前数据）
        mdns_service_txt_set("_http", "_tcp", serviceTxtData, 3);
        
        // 修改服务端口号
        mdns_service_port_set("_myservice", "_udp", 4321);
    }

mDNS 查询
^^^^^^^^^

mDNS 提供查询服务和解析主机 IP/IPv6 地址的方法。

服务查询的结果会作为 ``mdns_result_t`` 类型对象的链表返回。

解析主机 IP 地址的示例方法::

    void resolve_mdns_host(const char * host_name)
    {
        printf("Query A: %s.local", host_name);

        struct ip4_addr addr;
        addr.addr = 0;

        esp_err_t err = mdns_query_a(host_name, 2000,  &addr);
        if(err){
            if(err == ESP_ERR_NOT_FOUND){
                printf("Host was not found!");
                return;
            }
            printf("Query Failed");
            return;
        }

        printf(IPSTR, IP2STR(&addr));
    }

解析本地服务的示例方法::

    static const char * if_str[] = {"STA", "AP", "ETH", "MAX"};
    static const char * ip_protocol_str[] = {"V4", "V6", "MAX"};

    void mdns_print_results(mdns_result_t * results){
        mdns_result_t * r = results;
        mdns_ip_addr_t * a = NULL;
        int i = 1, t;
        while(r){
            printf("%d: Interface: %s, Type: %s\n", i++, if_str[r->tcpip_if], ip_protocol_str[r->ip_protocol]);
            if(r->instance_name){
                printf("  PTR : %s\n", r->instance_name);
            }
            if(r->hostname){
                printf("  SRV : %s.local:%u\n", r->hostname, r->port);
            }
            if(r->txt_count){
                printf("  TXT : [%u] ", r->txt_count);
                for(t=0; t<r->txt_count; t++){
                    printf("%s=%s; ", r->txt[t].key, r->txt[t].value);
                }
                printf("\n");
            }
            a = r->addr;
            while(a){
                if(a->addr.type == IPADDR_TYPE_V6){
                    printf("  AAAA: " IPV6STR "\n", IPV62STR(a->addr.u_addr.ip6));
                } else {
                    printf("  A   : " IPSTR "\n", IP2STR(&(a->addr.u_addr.ip4)));
                }
                a = a->next;
            }
            r = r->next;
        }

    }

    void find_mdns_service(const char * service_name, const char * proto)
    {
        ESP_LOGI(TAG, "Query PTR: %s.%s.local", service_name, proto);

        mdns_result_t * results = NULL;
        esp_err_t err = mdns_query_ptr(service_name, proto, 3000, 20,  &results);
        if(err){
            ESP_LOGE(TAG, "Query Failed");
            return;
        }
        if(!results){
            ESP_LOGW(TAG, "No results found!");
            return;
        }

        mdns_print_results(results);
        mdns_query_results_free(results);
    }

使用上述方法的示例::

    void my_app_some_method(){
        // 搜索 esp32-mdns.local
        resolve_mdns_host("esp32-mdns");
        
        // 搜索 HTTP 服务器
        find_mdns_service("_http", "_tcp");
        // 或者搜索文件服务器
        find_mdns_service("_smb", "_tcp"); // Windows 系统的共享服务
        find_mdns_service("_afpovertcp", "_tcp"); // Apple AFP 文件共享服务
        find_mdns_service("_nfs", "_tcp"); // NFS 服务器
        find_mdns_service("_ftp", "_tcp"); // FTP 服务器
        // 或者网络打印机
        find_mdns_service("_printer", "_tcp");
        find_mdns_service("_ipp", "_tcp");
    }

应用示例
--------

有关 mDNS 服务器和查询器的应用示例请参考 :example:`protocols/mdns`。

API 参考
--------

.. include:: /_build/inc/mdns.inc


