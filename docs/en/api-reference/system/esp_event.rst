Event Loop Library
==================

:link_to_translation:`zh_CN:[中文]`

Overview
--------

The event loop library allows components to declare events so that other components can register handlers -- codes that executes when those events occur. This allows loosely-coupled components to attach desired behavior to state changes of other components without application involvement. This also simplifies event processing by serializing and deferring code execution to another context.

.. only:: SOC_WIFI_SUPPORTED

    One common case is, if a high-level library is using the Wi-Fi library: it may subscribe to :ref:`ESP32 Wi-Fi Programming Model <wifi-programming-model>` directly and act on those events.

.. only:: SOC_BT_SUPPORTED

    .. note::

        Various modules of the Bluetooth stack deliver events to applications via dedicated callback functions instead of via the Event Loop Library.

Using ``esp_event`` APIs
------------------------

There are two objects of concern for users of this library: events and event loops.

An event indicates an important occurrence, such as a successful Wi-Fi connection to an access point. A two-part identifier should be used when referencing events, see :ref:`declaring and defining events <esp-event-declaring-defining-events>` for details. The event loop is the bridge between events and event handlers. The event source publishes events to the event loop using the APIs provided by the event loop library, and event handlers registered to the event loop respond to specific types of events.

Using this library roughly entails the following flow:

1. The user defines a function that should run when an event is posted to a loop. This function is referred to as the event handler, and should have the same signature as :cpp:type:`esp_event_handler_t`.
2. An event loop is created using :cpp:func:`esp_event_loop_create`, which outputs a handle to the loop of type :cpp:type:`esp_event_loop_handle_t`. Event loops created using this API are referred to as user event loops. There is, however, a special type of event loop called the default event loop which is discussed in :ref:`default event loop <esp-event-default-loops>`.
3. Components register event handlers to the loop using :cpp:func:`esp_event_handler_register_with`. Handlers can be registered with multiple loops, see :ref:`notes on handler registration <esp-event-handler-registration>`.
4. Event sources post an event to the loop using :cpp:func:`esp_event_post_to`.
5. Components wanting to remove their handlers from being called can do so by unregistering from the loop using :cpp:func:`esp_event_handler_unregister_with`.
6. Event loops that are no longer needed can be deleted using :cpp:func:`esp_event_loop_delete`.

In code, the flow above may look like as follows:

.. code-block:: c

    // 1. Define the event handler
    void run_on_event(void* handler_arg, esp_event_base_t base, int32_t id, void* event_data)
    {
        // Event handler logic
    }

    void app_main()
    {
        // 2. A configuration structure of type esp_event_loop_args_t is needed to specify the properties of the loop to be created. A handle of type esp_event_loop_handle_t is obtained, which is needed by the other APIs to reference the loop to perform their operations.
        esp_event_loop_args_t loop_args = {
            .queue_size = ...,
            .task_name = ...
            .task_priority = ...,
            .task_stack_size = ...,
            .task_core_id = ...
        };

        esp_event_loop_handle_t loop_handle;

        esp_event_loop_create(&loop_args, &loop_handle);

        // 3. Register event handler defined in (1). MY_EVENT_BASE and MY_EVENT_ID specify a hypothetical event that handler run_on_event should execute when it gets posted to the loop.
        esp_event_handler_register_with(loop_handle, MY_EVENT_BASE, MY_EVENT_ID, run_on_event, ...);

        ...

        // 4. Post events to the loop. This queues the event on the event loop. At some point, the event loop executes the event handler registered to the posted event, in this case, run_on_event. To simplify the process, this example calls esp_event_post_to from app_main, but posting can be done from any other task (which is the more interesting use case).
        esp_event_post_to(loop_handle, MY_EVENT_BASE, MY_EVENT_ID, ...);

        ...

        // 5. Unregistering an unneeded handler
        esp_event_handler_unregister_with(loop_handle, MY_EVENT_BASE, MY_EVENT_ID, run_on_event);

        ...

        // 6. Deleting an unneeded event loop
        esp_event_loop_delete(loop_handle);
    }

.. _esp-event-declaring-defining-events:

Declaring and Defining Events
-----------------------------

As mentioned previously, events consist of two-part identifiers: the event base and the event ID. The event base identifies an independent group of events; the event ID identifies the event within that group. Think of the event base and event ID as a person's last name and first name, respectively. A last name identifies a family, and the first name identifies a person within that family.

The event loop library provides macros to declare and define the event base easily.

Event base declaration:

.. code-block:: c

    ESP_EVENT_DECLARE_BASE(EVENT_BASE);

Event base definition:

.. code-block:: c

    ESP_EVENT_DEFINE_BASE(EVENT_BASE);

.. note::

    In ESP-IDF, the base identifiers for system events are uppercase and are postfixed with ``_EVENT``. For example, the base for Wi-Fi events is declared and defined as ``WIFI_EVENT``, the Ethernet event base ``ETHERNET_EVENT``, and so on. The purpose is to have event bases look like constants (although they are global variables considering the definitions of macros ``ESP_EVENT_DECLARE_BASE`` and ``ESP_EVENT_DEFINE_BASE``).

For event IDs, declaring them as enumerations is recommended. Once again, for visibility, these are typically placed in public header files.

Event ID:

.. code-block:: c

    enum {
        EVENT_ID_1,
        EVENT_ID_2,
        EVENT_ID_3,
        ...
    }

.. _esp-event-default-loops:

Default Event Loop
------------------

The default event loop is a special type of loop used for system events (Wi-Fi events, for example). The handle for this loop is hidden from the user, and the creation, deletion, handler registration/deregistration, and posting of events are done through a variant of the APIs for user event loops. The table below enumerates those variants, and the user event loops equivalent.

.. list-table::
    :header-rows: 1
    :widths: 60 60
    :align: center

    * - User Event Loops
      - Default Event Loops
    * - :cpp:func:`esp_event_loop_create`
      - :cpp:func:`esp_event_loop_create_default`
    * - :cpp:func:`esp_event_loop_delete`
      - :cpp:func:`esp_event_loop_delete_default`
    * - :cpp:func:`esp_event_handler_register_with`
      - :cpp:func:`esp_event_handler_register`
    * - :cpp:func:`esp_event_handler_unregister_with`
      - :cpp:func:`esp_event_handler_unregister`
    * - :cpp:func:`esp_event_post_to`
      - :cpp:func:`esp_event_post`

If you compare the signatures for both, they are mostly similar except for the lack of loop handle specification for the default event loop APIs.

Other than the API difference and the special designation to which system events are posted, there is no difference in how default event loops and user event loops behave. It is even possible for users to post their own events to the default event loop, should the user opt to not create their own loops to save memory.

.. _esp-event-handler-registration:

Notes on Handler Registration
-----------------------------

It is possible to register a single handler to multiple events individually by using multiple calls to :cpp:func:`esp_event_handler_register_with`. For those multiple calls, the specific event base and event ID can be specified with which the handler should execute.

However, in some cases, it is desirable for a handler to execute on the following situations:

(1) all events that get posted to a loop
(2) all events of a particular base identifier

This is possible using the special event base identifier ``ESP_EVENT_ANY_BASE`` and special event ID ``ESP_EVENT_ANY_ID``. These special identifiers may be passed as the event base and event ID arguments for :cpp:func:`esp_event_handler_register_with`.

Therefore, the valid arguments to :cpp:func:`esp_event_handler_register_with` are:

1. <event base>, <event ID> - handler executes when the event with base <event base> and event ID <event ID> gets posted to the loop
2. <event base>, ESP_EVENT_ANY_ID - handler executes when any event with base <event base> gets posted to the loop
3. ESP_EVENT_ANY_BASE, ESP_EVENT_ANY_ID - handler executes when any event gets posted to the loop

As an example, suppose the following handler registrations were performed:

.. code-block:: c

    esp_event_handler_register_with(loop_handle, MY_EVENT_BASE, MY_EVENT_ID, run_on_event_1, ...);
    esp_event_handler_register_with(loop_handle, MY_EVENT_BASE, ESP_EVENT_ANY_ID, run_on_event_2, ...);
    esp_event_handler_register_with(loop_handle, ESP_EVENT_ANY_BASE, ESP_EVENT_ANY_ID, run_on_event_3, ...);

If the hypothetical event ``MY_EVENT_BASE``, ``MY_EVENT_ID`` is posted, all three handlers ``run_on_event_1``, ``run_on_event_2``, and ``run_on_event_3`` would execute.

If the hypothetical event ``MY_EVENT_BASE``, ``MY_OTHER_EVENT_ID`` is posted, only  ``run_on_event_2`` and ``run_on_event_3`` would execute.

If the hypothetical event ``MY_OTHER_EVENT_BASE``, ``MY_OTHER_EVENT_ID`` is posted, only ``run_on_event_3`` would execute.

Handler Un-Registering Itself
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In general, an event handler run by an event loop is **not allowed to do any registering/unregistering activity on that event loop**. There is one exception, though: un-registering itself is allowed for the handler. E.g., it is possible to do the following:

.. code-block:: c

    void run_on_event(void* handler_arg, esp_event_base_t base, int32_t id, void* event_data)
    {
        esp_event_loop_handle_t *loop_handle = (esp_event_loop_handle_t*) handler_arg;
        esp_event_handler_unregister_with(*loop_handle, MY_EVENT_BASE, MY_EVENT_ID, run_on_event);
    }

    void app_main(void)
    {
        esp_event_loop_handle_t loop_handle;
        esp_event_loop_create(&loop_args, &loop_handle);
        esp_event_handler_register_with(loop_handle, MY_EVENT_BASE, MY_EVENT_ID, run_on_event, &loop_handle);
        // ... post-event MY_EVENT_BASE, MY_EVENT_ID and run loop at some point
    }


Handler Registration and Handler Dispatch Order
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The general rule is that, for handlers that match a certain posted event during dispatch, those which are registered first also get executed first. The user can then control which handlers get executed first by registering them before other handlers, provided that all registrations are performed using a single task. If the user plans to take advantage of this behavior, caution must be exercised if there are multiple tasks registering handlers. While the 'first registered, first executed' behavior still holds true, the task which gets executed first also gets its handlers registered first. Handlers registered one after the other by a single task are still dispatched in the order relative to each other, but if that task gets pre-empted in between registration by another task that also registers handlers; then during dispatch those handlers also get executed in between.


Event Loop Profiling
--------------------

A configuration option :ref:`CONFIG_ESP_EVENT_LOOP_PROFILING` can be enabled in order to activate statistics collection for all event loops created. The function :cpp:func:`esp_event_dump` can be used to output the collected statistics to a file stream. More details on the information included in the dump can be found in the :cpp:func:`esp_event_dump` API Reference.

Application Example
-------------------

Examples of using the ``esp_event`` library can be found in :example:`system/esp_event`. The examples cover event declaration, loop creation, handler registration and deregistration, and event posting.

Other examples which also adopt ``esp_event`` library:

    * :example:`NMEA Parser <peripherals/uart/nmea0183_parser>`, which decodes the statements received from GPS.

API Reference
-------------

.. include-build-file:: inc/esp_event.inc
.. include-build-file:: inc/esp_event_base.inc

Related Documents
-----------------

.. toctree::
    :maxdepth: 1
