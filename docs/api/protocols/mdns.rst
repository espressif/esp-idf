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

  ::

    mdns_server_t * mdns = NULL;
    
    void start_mdns_service()
    {
        //initialize mDNS service on STA interface
        esp_err_t err = mdns_init(TCPIP_ADAPTER_IF_STA, &mdns);
        if (err) {
            printf("MDNS Init failed: %d\n", err);
            return;
        }
    
        //set hostname
        mdns_set_hostname(mdns, "my-esp32");
        //set default instance
        mdns_set_instance(mdns, "Jhon's ESP32 Thing");
    }

mDNS Services
^^^^^^^^^^^^^

mDNS can advertise information about network services that your device offers. Each service is defined by a few properties.

    * ``service``: (required) service type, prepended with underscore. Some common types can be found `here <http://www.dns-sd.org/serviceTypes.html>`_.
    * ``proto``: (required) protocol that the service runs on, prepended with underscore. Example: ``_tcp`` or ``_udp`` 
    * ``port``: (required) network port that the service runs on
    * ``instance``: friendly name for your service, like ``Jhon's ESP32 Web Server``. If not defined, ``default_instance`` will be used.
    * ``txt``: ``var=val`` array of strings, used to define properties for your service

Example method to add a few services and different properties:

  ::

    void add_mdns_services()
    {
        //add our services
        mdns_service_add(mdns, "_http", "_tcp", 80);
        mdns_service_add(mdns, "_arduino", "_tcp", 3232);
        mdns_service_add(mdns, "_myservice", "_udp", 1234);
        
        //NOTE: services must be added before their properties can be set
        //use custom instance for the web server
        mdns_service_instance_set(mdns, "_http", "_tcp", "Jhon's ESP32 Web Server");

        const char * arduTxtData[4] = {
                "board=esp32",
                "tcp_check=no",
                "ssh_upload=no",
                "auth_upload=no"
        };
        //set txt data for service (will free and replace current data)
        mdns_service_txt_set(mdns, "_arduino", "_tcp", 4, arduTxtData);
        
        //change service port
        mdns_service_port_set(mdns, "_myservice", "_udp", 4321);
    }

mDNS Query
^^^^^^^^^^

mDNS provides methods for browsing for services and resolving host's IP/IPv6 addresses.
    Results are returned as a linked list of ``mdns_result_t`` objects. If the result is from host query, 
    it will contain only ``addr`` and ``addrv6`` if found. Service queries will populate all fields 
    in a result that were found.

Example method to resolve host IPs:

  ::

    void resolve_mdns_host(const char * hostname)
    {
        printf("mDNS Host Lookup: %s.local\n", hostname);
        //run search for 1000 ms
        if (mdns_query(mdns, hostname, NULL, 1000)) {
            //results were found
            const mdns_result_t * results = mdns_result_get(mdns, 0);
            //itterate through all results
            size_t i = 1;
            while(results) {
                //print result information
                printf("  %u: IP:" IPSTR ", IPv6:" IPV6STR "\n", i++
                    IP2STR(&results->addr), IPV62STR(results->addrv6));
                //load next result. Will be NULL if this was the last one
                results = results->next;
            }
            //free the results from memory
            mdns_result_free(mdns);
        } else {
            //host was not found
            printf("  Host Not Found\n");
        }
    }

Example method to resolve local services:

  ::

    void find_mdns_service(const char * service, const char * proto)
    {
        printf("mDNS Service Lookup: %s.%s\n", service, proto);
        //run search for 1000 ms
        if (mdns_query(mdns, service, proto, 1000)) {
            //results were found
            const mdns_result_t * results = mdns_result_get(mdns, 0);
            //itterate through all results
            size_t i = 1;
            while(results) {
                //print result information
                printf("  %u: hostname:%s, instance:\"%s\", IP:" IPSTR ", IPv6:" IPV6STR ", port:%u, txt:%s\n", i++,
                    (results->host)?results->host:"NULL", (results->instance)?results->instance:"NULL",
                    IP2STR(&results->addr), IPV62STR(results->addrv6),
                    results->port, (results->txt)?results->txt:"\r");
                //load next result. Will be NULL if this was the last one
                results = results->next;
            }
            //free the results from memory
            mdns_result_free(mdns);
        } else {
            //service was not found
            printf("  Service Not Found\n");
        }
    }

Example of using the methods above:

  ::

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

Header Files
^^^^^^^^^^^^

  * :component_file:`mdns/include/mdns.h`

Macros
^^^^^^


Type Definitions
^^^^^^^^^^^^^^^^

.. doxygentypedef:: mdns_server_t
.. doxygentypedef:: mdns_result_t

Enumerations
^^^^^^^^^^^^


Structures
^^^^^^^^^^

.. doxygenstruct:: mdns_result_s
    :members:


Functions
^^^^^^^^^

.. doxygenfunction:: mdns_init
.. doxygenfunction:: mdns_free
.. doxygenfunction:: mdns_set_hostname
.. doxygenfunction:: mdns_set_instance
.. doxygenfunction:: mdns_service_add
.. doxygenfunction:: mdns_service_remove
.. doxygenfunction:: mdns_service_instance_set
.. doxygenfunction:: mdns_service_txt_set
.. doxygenfunction:: mdns_service_port_set
.. doxygenfunction:: mdns_service_remove_all
.. doxygenfunction:: mdns_query
.. doxygenfunction:: mdns_query_end
.. doxygenfunction:: mdns_result_get_count
.. doxygenfunction:: mdns_result_get
.. doxygenfunction:: mdns_result_free

