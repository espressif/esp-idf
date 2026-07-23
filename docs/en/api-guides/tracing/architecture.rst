Tracing Architecture
====================

:link_to_translation:`zh_CN:[中文]`

This document explains the high-level design of the ESP-IDF tracing system.

Overview
--------

Applications use ``esp_trace`` to collect runtime information from the target and send it to a host tool for analysis. This supports use cases such as FreeRTOS task and ISR analysis with SEGGER SystemView, source code coverage with Gcov, and application-specific data collection over apptrace.

ESP-IDF provides common tracing formats and transports, and the same framework can be extended for new ones, such as a custom trace format or a transport over SPI or UDP.

The ESP-IDF tracing system follows a **Port & Adapter** design. Applications call the public ``esp_trace`` API, while the tracing core connects the selected encoder with the selected transport.

This design provides:

- A stable application-facing API.
- Independent selection of trace format and host link.
- Adapter-specific details kept outside the core tracing code.
- Startup and panic handling owned by the tracing system.

.. mermaid::

    flowchart TB
        app["Application<br/>FreeRTOS tasks, ISRs, esp_trace_write(), trace macros"]
        api["Public interface<br/>esp_trace API"]
        core["Core tracing code<br/>esp_trace component<br/>session, multi-core init, adapter coordination"]
        registry["Runtime registry<br/>maps configured names to adapters"]
        host["Host link<br/>OpenOCD over JTAG, UART, or USB Serial JTAG"]

        subgraph PORTS["Ports"]
            direction LR
            enc_port["Encoder port"]
            transport_port["Transport port"]
        end

        subgraph ADAPTERS["Adapters"]
            direction LR
            encoder["Encoder adapter<br/>external component, for example espressif/esp_sysview<br/>formats recorder events"]
            transport["Transport adapter<br/>esp_trace component<br/>apptrace over JTAG/UART, USB Serial JTAG"]
        end

        app --> api --> core
        core --- registry
        core --> enc_port
        core --> transport_port
        enc_port --> encoder
        transport_port --> transport
        encoder -.-> transport
        transport --> host

Components
----------

Core Tracing Code
^^^^^^^^^^^^^^^^^

The ``esp_trace`` component contains the public API and maintains the active trace session. It creates the encoder/transport pair during startup, coordinates multi-core initialization, and forwards API calls to the selected adapters.

Encoder Port
^^^^^^^^^^^^

The encoder port interface (:component_file:`esp_trace_port_encoder.h <esp_trace/include/esp_trace_port_encoder.h>`) defines how a trace library plugs into ``esp_trace``. An encoder receives trace writes or trace-hook events and converts them to a recorder-specific format, such as the SystemView protocol. ``esp_trace`` defines this interface but does not ship an encoder; encoders are provided by external components, for example ``espressif/esp_sysview``. See :doc:`custom-trace-library`.

Transport Port
^^^^^^^^^^^^^^

The transport port interface (:component_file:`esp_trace_port_transport.h <esp_trace/include/esp_trace_port_transport.h>`) defines how encoded trace data leaves the target. A transport writes bytes to a host-facing link and handles link-specific operations such as flushing, host-connection checks, and panic-time output. ``esp_trace`` provides built-in apptrace over JTAG/UART and USB Serial JTAG transport adapters. See :doc:`transports`.

Each trace session pairs one encoder with one transport. The encoder can pass encoded trace data to the transport selected for the session, and runtime trace writes do not allocate memory.

Initialization
--------------

``esp_trace`` initializes automatically during system startup based on the encoder and transport selected in project configuration. Applications normally do not need to call a separate initialization function before using the public tracing API. Adapter-specific initialization requirements are covered in :doc:`custom-trace-library`.

Data Flow
---------

A typical write flows from the public API to the encoder, then to the transport:

.. mermaid::

    flowchart TD
        write["esp_trace_write(handle, data, size, tmo)"]
        validate["Core validates the handle"]
        encode["Encoder writes formatted data<br/>for example SystemView protocol"]
        send["Transport sends encoded bytes<br/>JTAG, UART, or USB Serial JTAG"]
        status["Status returns to the caller"]

        write --> validate --> encode --> send --> status

Panic Handling
--------------

During a panic, interrupts are disabled and normal locking cannot be used. The core calls optional panic callbacks on the active encoder and transport. Each adapter can then flush its own buffers without taking normal locks. Panic flushing can still drop data because it must not block or use normal locking.

Registry
--------

Adapters register themselves at link time with ``ESP_TRACE_REGISTER_ENCODER()`` and ``ESP_TRACE_REGISTER_TRANSPORT()``. During initialization, the core looks up the configured encoder and transport by name. Only adapters linked into the application are available, and adding a new adapter does not require changes to the core.

Related Documentation
---------------------

- :doc:`custom-trace-library`: the adapter author contract (encoder and transport function tables, registration, locking and reentrancy rules)
- :doc:`transports`: the apptrace transport and standalone apptrace usage
- :doc:`sysview`: SEGGER SystemView usage
- :doc:`/api-reference/system/esp_trace`: ESP Trace API reference
