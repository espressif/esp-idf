mDNS Service
============

Overview
--------

mDNS is a multicast UDP service that is used to provide local network service and host discovery.

mDNS is installed by default on most operating systems or is available as separate package. On ``Mac OS`` it is installed by default and is called ``Bonjour``. Apple releases an installer for ``Windows`` that can be found `on Apple's support page <https://support.apple.com/downloads/bonjour%2520for%2520windows>`_. On ``Linux``, mDNS is provided by `avahi <https://github.com/lathiat/avahi>`_ and is usually installed by default.

mDNS Properties
^^^^^^^^^^^^^^^

    * ``hostname``: the hostname that the device will respond to. If not set, the ``hostname`` will be read from the interface. Example: ``my-esp32`` will resolve to ``my-esp32.local``
    * ``default_instance``: friendly name for your device, like ``Jhon's ESP32 Thing``. If not set, ``hostname`` will be used.

Example method to start mDNS for the STA interface and set ``hostname`` and ``default_instance``:

.. highlight:: c

::

    void start_mdns_service()
    {
        //initialize mDNS service
        esp_err_t err = mdns_init();
        if (err) {
            printf("MDNS Init failed: %d\n", err);
            return;
        }
    
        //set hostname
        mdns_hostname_set("my-esp32");
        //set default instance
        mdns_instance_name_set("Jhon's ESP32 Thing");
    }

mDNS Services
^^^^^^^^^^^^^

mDNS can advertise information about network services that your device offers. Each service is defined by a few properties.

    * ``instance_name``: friendly name for your service, like ``Jhon's ESP32 Web Server``. If not defined, ``default_instance`` will be used.
    * ``service_type``: (required) service type, prepended with underscore. Some common types can be found `here <http://www.dns-sd.org/serviceTypes.html>`_.
    * ``proto``: (required) protocol that the service runs on, prepended with underscore. Example: ``_tcp`` or ``_udp`` 
    * ``port``: (required) network port that the service runs on
    * ``txt``: ``{var, val}`` array of strings, used to define properties for your service

Example method to add a few services and different properties::

    void add_mdns_services()
    {
        //add our services
        mdns_service_add(NULL, "_http", "_tcp", 80, NULL, 0);
        mdns_service_add(NULL, "_arduino", "_tcp", 3232, NULL, 0);
        mdns_service_add(NULL, "_myservice", "_udp", 1234, NULL, 0);
        
        //NOTE: services must be added before their properties can be set
        //use custom instance for the web server
        mdns_service_instance_name_set("_http", "_tcp", "Jhon's ESP32 Web Server");

        mdns_txt_item_t serviceTxtData[3] = {
            {"board","esp32"},
            {"u","user"},
            {"p","password"}
        };
        //set txt data for service (will free and replace current data)
        mdns_service_txt_set("_http", "_tcp", serviceTxtData, 3);
        
        //change service port
        mdns_service_port_set("_myservice", "_udp", 4321);
    }

mDNS Query
^^^^^^^^^^

mDNS provides methods for browsing for services and resolving host's IP/IPv6 addresses.

Results for services are returned as a linked list of ``mdns_result_t`` objects.

Example method to resolve host IPs::

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

Example method to resolve local services::

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
                if(a->addr.type == MDNS_IP_PROTOCOL_V6){
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

Example of using the methods above::

    void my_app_some_method(){
        //search for esp32-mdns.local
        resolve_mdns_host("esp32-mdns");
        
        //search for HTTP servers
        find_mdns_service("_http", "_tcp");
        //or file servers
        find_mdns_service("_smb", "_tcp"); //windows sharing
        find_mdns_service("_afpovertcp", "_tcp"); //apple sharing
        find_mdns_service("_nfs", "_tcp"); //NFS server
        find_mdns_service("_ftp", "_tcp"); //FTP server
        //or networked printer
        find_mdns_service("_printer", "_tcp");
        find_mdns_service("_ipp", "_tcp");
    }

Application Example
-------------------

mDNS server/scanner example: :example:`protocols/mdns`.

API Reference
-------------

.. include:: /_build/inc/mdns.inc


