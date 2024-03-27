ESP-NETIF 自定义 I/O 驱动程序
===============================

:link_to_translation:`en:[English]`

本节概述了如何配置具有 ESP-NETIF 连接功能的新 I/O 驱动程序。

通常情况下，I/O 驱动程序须注册为 ESP-NETIF 驱动程序。因此，它依赖于 ESP-NETIF 组件，并负责提供数据路径函数、后附回调函数，并在多数情况下用于设置默认事件处理程序，根据驱动程序的生命周期转换来定义网络接口操作。


数据包 Input/Output
^^^^^^^^^^^^^^^^^^^^^^^

根据 :ref:`esp-netif structure` 章节提供的图表可以看出，须定义以下三个数据路径函数 API 以连接 ESP-NETIF：

* :cpp:func:`esp_netif_transmit()`
* :cpp:func:`esp_netif_free_rx_buffer()`
* :cpp:func:`esp_netif_receive()`

前两个函数可以传输和释放 RX 缓冲区，用作回调。它们由 ESP-NETIF（及其底层 TCP/IP 堆栈）调用，并由 I/O 驱动实现。

另一方面，接收函数由 I/O 驱动程序调用，因此驱动的代码只需在接收到新数据时调用 :cpp:func:`esp_netif_receive()` 函数。


后附回调
^^^^^^^^^^^^^^^^^^^^

网络接口初始化的最后一步是调用以下 API，将 ESP-NETIF 实例附加到 I/O 驱动程序上：

.. code:: c

    esp_err_t esp_netif_attach(esp_netif_t *esp_netif, esp_netif_iodriver_handle driver_handle);

假设 ``esp_netif_iodriver_handle`` 是指向驱动程序对象的指针，该对象是从 ``struct esp_netif_driver_base_s`` 衍生的结构体，那么 I/O 驱动结构体的第一个成员必须是此基础结构，并指向：

* 后附函数回调
* 相关的 ESP-NETIF 实例

因此，I/O 驱动程序须创建以下结构体的实例：

.. code:: c

    typedef struct my_netif_driver_s {
            esp_netif_driver_base_t base;           /*!< 保留基本结构体作为 esp-netif 驱动 */
            driver_impl             *h;             /*!< 驱动实现 */
        } my_netif_driver_t;

此实例中包含 ``my_netif_driver_t::base.post_attach`` 的真实值和实际的驱动处理程序 ``my_netif_driver_t::h``。

从初始化代码调用 :cpp:func:`esp_netif_attach()` 时，将执行 I/O 驱动程序代码的后附回调，以在 ESP-NETIF 和 I/O 驱动程序实例之间相互注册回调。通常，后附回调中也会启动驱动程序。以下为一个简单的后附回调示例：

.. code:: c

    static esp_err_t my_post_attach_start(esp_netif_t * esp_netif, void * args)
    {
        my_netif_driver_t *driver = args;
        const esp_netif_driver_ifconfig_t driver_ifconfig = {
                .driver_free_rx_buffer = my_free_rx_buf,
                .transmit = my_transmit,
                .handle = driver->driver_impl
        };
        driver->base.netif = esp_netif;
        ESP_ERROR_CHECK(esp_netif_set_driver_config(esp_netif, &driver_ifconfig));
        my_driver_start(driver->driver_impl);
        return ESP_OK;
    }


默认处理程序
^^^^^^^^^^^^^^^^

I/O 驱动程序通常还会根据 I/O 驱动程序的状态转换，为相关网络接口的生命周期行为提供默认定义，例如 *driver start* ``->`` *network start* 等。

以下是此类默认处理程序的一个示例：

.. code:: c

    esp_err_t my_driver_netif_set_default_handlers(my_netif_driver_t *driver, esp_netif_t * esp_netif)
    {
        driver_set_event_handler(driver->driver_impl, esp_netif_action_start, MY_DRV_EVENT_START, esp_netif);
        driver_set_event_handler(driver->driver_impl, esp_netif_action_stop, MY_DRV_EVENT_STOP, esp_netif);
        return ESP_OK;
    }


网络堆栈连接
------------------------

用于传输和释放 RX 缓冲区的数据路径函数（在 I/O 驱动中定义）由 ESP-NETIF 的 TCP/IP 堆栈连接层调用。

注意，ESP-IDF 为最常见的网络接口（如 Wi-Fi station 或以太网）提供了几种网络堆栈配置。这些配置定义在 :component_file:`esp_netif/include/esp_netif_defaults.h` 中，能够满足大多数网络驱动程序的需求。在少数情况下，一些专家用户可能希望自定义基于 lwIP 的接口层，这需要额外设置 lwIP 依赖。

以下参考 API 概述了这些网络堆栈和 ESP-NETIF 的交互：

.. include-build-file:: inc/esp_netif_net_stack.inc
