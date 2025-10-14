Protocols
=========

:link_to_translation:`zh_CN:[中文]`

ESP HTTP SERVER
---------------

:ref:`CONFIG_HTTPD_MAX_REQ_HDR_LEN`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The :ref:`CONFIG_HTTPD_MAX_REQ_HDR_LEN` option now defines the maximum limit for the memory that can be allocated internally for the HTTP request header. The actual memory allocated for the header depends on the size of the header received in the HTTP request, rather than being fixed to this value as before. This provides more flexible memory usage based on the actual header size.
