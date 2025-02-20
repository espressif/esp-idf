协议
====

:link_to_translation:`en:[English]`

ESP HTTP 服务器
---------------

:ref:`CONFIG_HTTPD_MAX_REQ_HDR_LEN`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:ref:`CONFIG_HTTPD_MAX_REQ_HDR_LEN` 选项定义了内部为 HTTP 请求头分配内存的上限。实际分配的内存大小取决于 HTTP 请求中接收到的请求头的大小，而不再为先前的固定值。这一方式可以根据请求头的实际大小更灵活地使用内存，提高了内存的使用效率。
