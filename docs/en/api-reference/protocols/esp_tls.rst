ESP-TLS
=======

Overview
--------

The ESP-TLS component provides a simplified API interface for accessing the commonly used TLS functionality. 
It supports common scenarios like CA certification validation, SNI, ALPN negotiation, non-blocking connection among others. 
All the configuration can be specified in the ``esp_tls_cfg_t`` data structure. Once done, TLS communication can be conducted using the following APIs:

    * :cpp:func:`esp_tls_conn_new`: for opening a new TLS connection.
    * :cpp:func:`esp_tls_conn_read`: for reading from the connection.
    * :cpp:func:`esp_tls_conn_write`: for writing into the connection.
    * :cpp:func:`esp_tls_conn_delete`: for freeing up the connection.

Any application layer protocol like HTTP1, HTTP2 etc can be executed on top of this layer.                       

Application Example
-------------------

Simple HTTPS example that uses ESP-TLS to establish a secure socket connection: :example:`protocols/https_request`.

Tree structure for ESP-TLS component
-------------------------------------
    .. highlight:: none

    ::

        ├── esp_tls.c
        ├── esp_tls.h
        ├── esp_tls_mbedtls.c
        ├── esp_tls_wolfssl.c
        └── private_include
            ├── esp_tls_mbedtls.h
            └── esp_tls_wolfssl.h

The ESP-TLS  component has a file :component_file:`esp-tls/esp_tls.h` which contain the public API headers for the component. Internally ESP-TLS component uses one
of the two SSL/TLS Libraries between mbedtls and wolfssl for its operation. API specific to mbedtls are present in :component_file:`esp-tls/private_include/esp_tls_mbedtls.h` and API
specific to wolfssl are present in :component_file:`esp-tls/private_include/esp_tls_wolfssl.h`.

Underlying SSL/TLS Library Options
----------------------------------
The ESP-TLS  component has an option to use mbedtls or wolfssl as their underlying SSL/TLS library. By default only mbedtls is available and is
used, wolfssl SSL/TLS library is available publicly at https://github.com/espressif/esp-wolfssl. The repository provides wolfssl component in binary format, it
also provides few examples which are useful for understanding the API. Please refer the repository README.md for
information on licensing and other options. Please see below option for using wolfssl in your project.

.. note::   `As the library options are internal to ESP-TLS, switching the libries will not change ESP-TLS specific code for a project.`

How to use wolfssl with ESP-IDF
-------------------------------
There are two ways to use wolfssl in your project

1) Directly add wolfssl as a component in your project with following three commands.::

    (First change directory (cd) to your project directory)
    mkdir components
    cd components
    git clone https://github.com/espressif/esp-wolfssl.git

2) Add wolfssl as an extra component in your project.

* Download wolfssl with::

    git clone https://github.com/espressif/esp-wolfssl.git

* Include  esp-wolfssl in ESP-IDF with setting EXTRA_COMPONENT_DIRS in CMakeLists.txt/Makefile of your project as done in `wolfssl/examples <https://github.com/espressif/esp-wolfssl/tree/master/examples>`_. For reference see Optional Project variables in :doc:`build-system.</api-guides/build-system>`

After above steps, you will have option to choose wolfssl as underlying SSL/TLS library in configuration menu of your project as follows::

    idf.py/make menuconfig -> ESP-TLS -> choose SSL/TLS Library -> mbedtls/wolfssl

Comparison between mbedtls and wolfssl
--------------------------------------
The following table shows a typical comparison between wolfssl and mbedtls when :example:`protocols/https_request` example `(which has server authentication)` was run with both
SSL/TLS libraries and with all respective configurations set to default.
`(mbedtls IN_CONTENT length and OUT_CONTENT length were set to 16384 bytes and 4096 bytes respectively)`

    +---------------------+------------+-----------+
    | Property            |  Wolfssl   |  Mbedtls  |
    +=====================+============+===========+
    | Total Heap Consumed |  ~19 Kb    |  ~37 Kb   |
    +---------------------+------------+-----------+
    | Task Stack Used     |  ~2.2 Kb   |  ~3.6 Kb  |
    +---------------------+------------+-----------+
    |     Bin size        |  ~858 Kb   |  ~736 Kb  |
    +---------------------+------------+-----------+

.. note::    `These values are subject to change with change in configuration options and version of respective libraries`.

API Reference
-------------

.. include:: /_build/inc/esp_tls.inc
