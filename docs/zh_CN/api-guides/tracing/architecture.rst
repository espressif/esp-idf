跟踪架构
========

:link_to_translation:`en:[English]`

本文档介绍 ESP-IDF 跟踪系统的高层设计。

概述
----

应用程序使用 ``esp_trace`` 从目标设备收集运行时信息，并将其发送到主机工具进行分析。这支持多种场景，例如使用 SEGGER SystemView 分析 FreeRTOS 任务和中断、使用 Gcov 获取源代码覆盖率，以及通过 apptrace 收集应用程序自定义数据。

ESP-IDF 提供通用的跟踪格式和传输方式，并且可以在同一框架下扩展新的跟踪格式和传输方式，例如自定义跟踪格式、SPI 传输或 UDP 传输。

ESP-IDF 跟踪系统采用 **Port & Adapter（端口与适配器）** 设计。应用程序调用公共的 ``esp_trace`` API，跟踪核心则将所选编码器与所选传输连接起来。

该设计具有以下优势：

- 提供稳定的面向应用程序的 API。
- 可独立选择跟踪格式和主机链路。
- 将适配器相关实现细节与核心跟踪代码隔离。
- 由跟踪系统统一负责启动和 Panic 处理。

.. mermaid::

    flowchart TB
        app["应用程序<br/>FreeRTOS 任务、ISR、esp_trace_write()、跟踪宏"]
        api["公共接口<br/>esp_trace API"]
        core["核心跟踪代码<br/>esp_trace 组件<br/>会话、多核初始化、适配器协调"]
        registry["运行时注册表<br/>将配置名称映射到适配器"]
        host["主机链路<br/>通过 JTAG 的 OpenOCD、UART 或 USB Serial JTAG"]

        subgraph PORTS["端口"]
            direction LR
            enc_port["编码器端口"]
            transport_port["传输端口"]
        end

        subgraph ADAPTERS["适配器"]
            direction LR
            encoder["编码器适配器<br/>外部组件，例如 espressif/esp_sysview<br/>格式化记录器事件"]
            transport["传输适配器<br/>esp_trace 组件<br/>JTAG 或 UART 上的 apptrace、USB Serial JTAG"]
        end

        app --> api --> core
        core --- registry
        core --> enc_port
        core --> transport_port
        enc_port --> encoder
        transport_port --> transport
        encoder -.-> transport
        transport --> host

组件
----

核心跟踪代码
^^^^^^^^^^^^

``esp_trace`` 组件包含公共 API，并维护活动跟踪会话。它在启动期间创建编码器/传输配对，协调多核初始化，并将 API 调用转发给所选适配器。

编码器端口
^^^^^^^^^^

编码器端口接口（:component_file:`esp_trace_port_encoder.h <esp_trace/include/esp_trace_port_encoder.h>`）定义跟踪库如何接入 ``esp_trace``。编码器接收跟踪写入或跟踪钩子事件，并将其转换为记录器特定格式，例如 SystemView 协议。``esp_trace`` 定义该接口，但自身不提供编码器；编码器由外部组件提供，例如 ``espressif/esp_sysview``。参见 :doc:`custom-trace-library`。

传输端口
^^^^^^^^

传输端口接口（:component_file:`esp_trace_port_transport.h <esp_trace/include/esp_trace_port_transport.h>`）定义编码后的跟踪数据如何离开目标设备。传输负责将字节写入面向主机的链路，并处理链路相关操作，例如刷新、主机连接检查和 Panic 时输出。``esp_trace`` 提供内置的 apptrace（JTAG 或 UART）和 USB Serial JTAG 传输适配器。参见 :doc:`transports`。

每个跟踪会话都将一个编码器与一个传输配对。编码器可以将编码后的跟踪数据交给当前会话选择的传输，且跟踪写入期间不进行动态分配。

初始化
------

``esp_trace`` 会根据项目配置中选择的编码器和传输，在系统启动期间自动初始化。应用程序在使用公共跟踪 API 前通常不需要调用单独的初始化函数。适配器相关的初始化要求参见 :doc:`custom-trace-library`。

数据流
------

一次典型写入会从公共 API 流向编码器，再流向传输：

.. mermaid::

    flowchart TD
        write["esp_trace_write(handle, data, size, tmo)"]
        validate["核心校验句柄"]
        encode["编码器写入格式化后的数据<br/>例如 SystemView 协议"]
        send["传输发送编码后的字节<br/>JTAG、UART 或 USB Serial JTAG"]
        status["状态返回给调用方"]

        write --> validate --> encode --> send --> status

Panic 处理
----------

发生 Panic 时，中断已禁用，常规加锁机制不可用。核心会调用活动编码器和传输的可选 Panic 回调。每个适配器随后可以在不使用常规锁的情况下刷新自己的缓冲区。由于 Panic 刷新不得阻塞或使用常规加锁机制，因此仍可能丢弃部分数据。

注册表
------

适配器在链接时通过 ``ESP_TRACE_REGISTER_ENCODER()`` 和 ``ESP_TRACE_REGISTER_TRANSPORT()`` 自行注册。初始化期间，核心按名称查找所配置的编码器和传输。只有实际链接进应用程序的适配器才可用，且添加新适配器无需改动核心。

相关文档
--------

- :doc:`custom-trace-library`：适配器作者契约（编码器与传输函数表、注册、加锁与重入规则）
- :doc:`transports`：apptrace 传输及独立的 apptrace 用法
- :doc:`sysview`：SEGGER SystemView 用法
- :doc:`/api-reference/system/esp_trace`：ESP Trace API 参考
