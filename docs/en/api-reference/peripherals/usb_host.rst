USB Host
========

The document provides information regarding the USB Host Library. This document is split into the following sections:

.. contents:: Sections
  :depth: 2


.. ---------------------------------------------------- Overview -------------------------------------------------------

Overview
--------

The USB Host Library (hereinafter referred to as the Host Library) is the lowest public facing API layer of the ESP-IDF USB Host Stack. In most cases, applications that require USB Host functionality will not need to interface with the Host Library directly. Instead, most applications will use the API provided by a host class driver that is implemented on top of the Host Library.

However, users may want to use the Host Library directly for some of (but not limited to) the following reasons:

- The user needs to implement a custom host class driver such as a vendor specific class driver
- The user has a requirement for a lower level of abstraction due to resource/latency requirements

Features & Limitations
^^^^^^^^^^^^^^^^^^^^^^

The Host Library has the following features:

- Supports Full Speed (FS) and Low Speed (LS) Devices
- Supports all four transfer types (Control, Bulk, Interrupt, and Isochronous)
- Allows multiple class drivers to run simultaneously (i.e., multiple clients of the Host Library)
- A single device can be used by multiple clients simultaneously (e.g., composite devices)
- The Host Library itself (and the underlying Host Stack) does not internally instantiate any OS tasks. The number of tasks are entirely controlled by how the Host Library interface is used. However, a general rule of thumb regarding the number of tasks is ``(the number of host class drivers running + 1)``.

Currently, the Host Library (and the underlying Host Stack) has the following limitations:

- Only supports a single device, but the Host Library's API is designed for multiple device support.
- Only supports Asynchronous transfers
- Transfer timeouts are not supported yet


.. -------------------------------------------------- Architecture -----------------------------------------------------

Architecture
------------

.. figure:: ../../../_static/usb_host_lib_entities.png
    :align: center
    :alt: Diagram of the Key Entities of USB Host Functionality
    :figclass: align-center

    Diagram of the key entities involved in USB Host functionality

The diagram above shows the key entities that are involved when implementing USB Host functionality. These entities are:

- The **Host Library**
- **Clients** of the Host Library
- **Devices**
- Host Library **Daemon Task**

Host Library
^^^^^^^^^^^^

The Host Library is the a lowest public facing layer of the USB Host Stack. Any other IDF component (such as a class driver or a user component) that needs to communicate with a connected USB device can only do so using the Host Library API either directly or indirectly.

The Host Library's API is split into two sub-sets, namely the **Library API** and **Client API**.

- The Client API handles the communication between a client of the Host Library and one or more USB devices. The Client API should only be called by registered clients of the Host Library.
- The Library API handles all of the Host Library processing that is not specific to a single client (e.g., device enumeration). Usually, the library API is called by a Host Library Daemon Task.

Clients
^^^^^^^

A client of the Host Library is a software component (such as a host class driver or user component) that uses the Host Library to communicate with a USB device. Generally each client has a one-to-one relation with a task, meaning that for a particular client, all of its Client API calls should be done from the context of the same task.

By organizing the software components that use the Host Library's into clients, the Host Library can delegate the handling of all client events (i.e., the events specific to that client) to the client's task. In other words, each client task is responsible for all the required processing and event handling associated with the USB communication that the client initiates.

Daemon Task
^^^^^^^^^^^

Although the Host Library delegates the handling of client events to the clients themselves, there are still Library events (i.e., events that are not specific to a client) that need to be handled. Library event handling can include things such as:

- Handling USB device connection, enumeration, and disconnection
- Rerouting control transfers to/from clients
- Forwarding events to clients

Therefore, in addition to the client tasks, the Host Library also requires a task (usually the Host Library Daemon Task) to handle all of the library events.

Devices
^^^^^^^

The Host Library hides the details of device handling (such as connection, memory allocation, and enumeration) from the clients. The clients are provided only with a list of already connected and enumerated devices to choose from. During enumeration, each device is configured to use configuration 1.

It is possible for a two or more clients to simultaneously communicate with the same device as long as they are not communicating to the same interface. However, multiple clients can simultaneously communicate with the same device's default endpoint (EP0), which will result in their control transfers being serialized.

For a client to communicate with a device, the client must:

#. Open the device using the device's address. This lets the Host Library know that the client is using that device.
#. Claim the interface(s) that will be used for communication. This prevents other clients from claiming the same interface(s).
#. Send transfers to the endpoints in the claimed interface. The client's task is responsible for handling its own processing and events.


.. ------------------------------------------------------ Usage --------------------------------------------------------

Usage
-----

The Host Library (and the underlying Host Stack) will not create any tasks. All tasks (i.e., the client tasks and the Daemon Task) will need to be created by the class drivers or the user. Instead, the Host Library provides two event handler functions that will handle all of the required Host Library processing, thus these functions should be called repeatedly from the client tasks and the Daemon Task. Therefore, the implementation of client tasks and the Daemon Task will be the largely centered around the invocation of these event handler functions.

Host Library & Daemon Task
^^^^^^^^^^^^^^^^^^^^^^^^^^

Basic Usage
"""""""""""

The Host Library API provides :cpp:func:`usb_host_lib_handle_events` to handle library events. This function should be called repeatedly, typically from the daemon task. Some notable features regarding :cpp:func:`usb_host_lib_handle_events` are:

- The function can block until a library event needs handling
- Event flags are returned on each invocation. These event flags are useful for knowing when the Host Library can be uninstalled.

A bare-bones Daemon Task would resemble something like the following code snippet:

.. code-block:: c

    #include "usb/usb_host.h"

    void daemon_task(void *arg)
    {
        ...
        bool exit = false;
        while (!exit) {
            uint32_t event_flags;
            usb_host_lib_handle_events(portMAX_DELAY, &event_flags);
            if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
                ...
            }
            if (event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE) {
                ...
            }
            ...
        }
        ...
    }

.. note::
    See the :example:`peripherals/usb/host/usb_host_lib` example for a full implementation of the Daemon Task

Lifecycle
"""""""""

.. figure:: ../../../_static/usb_host_lib_lifecycle.png
    :align: center
    :alt: Graph of Typical USB Host Library Lifecycle
    :figclass: align-center

    Graph of Typical USB Host Library Lifecycle

The graph above illustrates the typical lifecycle of the Host Library with multiple clients and devices. Specifically, the example involves...

- two registered clients (Client 1 and Client 2)
- two connected devices (Device 1 and Device 2), where Client 1 communicates with Device 1 and Client 2 communicates with Device 2.

With reference the graph above, the typical lifecycle involves the following key stages.

1. The Host Library is installed by calling :cpp:func:`usb_host_install`.
    - Installation must be done before any other Host Library API is called.
    - Where :cpp:func:`usb_host_install` is called (e.g., from the Daemon Task or another task) will depend on the synchronization logic between the Daemon Task, client tasks, and the rest of the system.
2. Once the Host Library is installed, the clients can be registered by calling :cpp:func:`usb_host_client_register`.
    - This is typically called from the client task (where the client task waits for a signal from the Daemon Task).
    - This can be called elsewhere if necessary as long it is called after :cpp:func:`usb_host_install`.
3. Device 1 connects and is then enumerated. 
    - Each registered client (in this case Client 1 and Client 2) are notified of the new device by way of the :cpp:enumerator:`USB_HOST_CLIENT_EVENT_NEW_DEV` event.
    - Client 1 opens Device 1 and begins communication with it.
4. Similarly Device 2 connects and is enumerated.
    - Client 1 and 2 are notified of a new device (via a :cpp:enumerator:`USB_HOST_CLIENT_EVENT_NEW_DEV` event).
    - Client 2 opens Device 2 and begins communication with it.
5. Device 1 suddenly disconnects.
    - Client 1 is notified by way of :cpp:enumerator:`USB_HOST_CLIENT_EVENT_DEV_GONE` and begins its cleanup.
    - Client 2 is not notified as it has not opened Device 1.
6. Client 1 completes its clean up and deregisters by calling :cpp:func:`usb_host_client_deregister`.
    - This is typically called from the client task before the task exits.
    - This can be called elsewhere if necessary as long as Client 1 has already completed its clean up.
7. Client 2 completes its communication with Device 2. Client 2 then closes Device 2 and deregisters itself.
    - The Daemon Task is notified of the deregistration of all clients by way the :c:macro:`USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS` event flag as Client 2 is the last client to deregister.
    - Device 2 is still allocated (i.e., not freed) as it is still connected albeit not currently opened by any client.
8. The Daemon Task decides to cleanup as there are no more clients.
    - The Daemon Task must free Device 2 first by calling :cpp:func:`usb_host_device_free_all`.
    - If :cpp:func:`usb_host_device_free_all` was able to free all devices, the function will return `ESP_OK` indicating that all devices have been freed.
    - If :cpp:func:`usb_host_device_free_all` was unable to free all devices (e.g., because the device is still opened by a client), the function will return `ESP_ERR_NOT_FINISHED`.
    - The Daemon Task must wait for :cpp:func:`usb_host_lib_handle_events` to return the :c:macro:`USB_HOST_LIB_EVENT_FLAGS_ALL_FREE` event flag in order to know when all devices have been freed.
9. Once the Daemon Task has verified that all clients have deregistered and all devices have been freed, it can now uninstall the Host Library by calling :cpp:func:`usb_host_uninstall`.

Clients & Class Driver
^^^^^^^^^^^^^^^^^^^^^^

Basic Usage
"""""""""""

The Host Library API provides :cpp:func:`usb_host_client_handle_events` to handle a particular client's events. This function should be called repeatedly, typically from the client's task. Some notable features regarding :cpp:func:`usb_host_client_handle_events` are:

- The function can block until a client event needs handling
- The function's primary purpose is to call the various event handling callbacks when a client event occurs.

The following callbacks are called from within :cpp:func:`usb_host_client_handle_events` thus allowing the client task to be notified of events.

- The client event callback of type :cpp:type:`usb_host_client_event_cb_t` which delivers client event messages to the client. Client event messages indicate events such as the addition or removal of a device.
- The USB transfer completion callback of type :cpp:type:`usb_transfer_cb_t` which indicates that a particular USB transfer previously submitted by the client has completed.

.. note::
    Given that the callbacks are called from within :cpp:func:`usb_host_client_handle_events`, users should avoid blocking from within the callbacks as this will result in :cpp:func:`usb_host_client_handle_events` being blocked as well, thus preventing other pending client events from being handled.

The following code snippet demonstrates a bare-bones host class driver and its client task. The code snippet contains:

- A simple client task function ``client_task`` that calls :cpp:func:`usb_host_client_handle_events` in a loop.
- Implementations of a client event callback and transfer completion callbacks.
- Implementation of a simple state machine for the class driver. The class driver simply opens a device, sends an OUT transfer to EP1, then closes the device.

.. code-block:: c

    #include <string.h>
    #include "usb/usb_host.h"

    #define CLASS_DRIVER_ACTION_OPEN_DEV    0x01
    #define CLASS_DRIVER_ACTION_TRANSFER    0x02
    #define CLASS_DRIVER_ACTION_CLOSE_DEV   0x03

    struct class_driver_control {
        uint32_t actions;
        uint8_t dev_addr;
        usb_host_client_handle_t client_hdl;
        usb_device_handle_t dev_hdl;
    };

    static void client_event_cb(const usb_host_client_event_msg_t *event_msg, void *arg)
    {
        //This is function is called from within usb_host_client_handle_events(). Don't block and try to keep it short
        struct class_driver_control *class_driver_obj = (struct class_driver_control *)arg;
        switch (event_msg->event) {
            case USB_HOST_CLIENT_EVENT_NEW_DEV:
                class_driver_obj->actions |= CLASS_DRIVER_ACTION_OPEN_DEV;
                class_driver_obj->dev_addr = event_msg->new_dev.address; //Store the address of the new device
                break;
            case USB_HOST_CLIENT_EVENT_DEV_GONE:
                class_driver_obj->actions |= CLASS_DRIVER_ACTION_CLOSE_DEV;
                break;
            default:
                break;
        }
    }

    static void transfer_cb(usb_transfer_t *transfer)
    {
        //This is function is called from within usb_host_client_handle_events(). Don't block and try to keep it short
        struct class_driver_control *class_driver_obj = (struct class_driver_control *)transfer->context;
        printf("Transfer status %d, actual number of bytes transferred %d\n", transfer->status, transfer->actual_num_bytes);
        class_driver_obj->actions |= CLASS_DRIVER_ACTION_CLOSE_DEV;
    }

    void client_task(void *arg)
    {
        ... //Wait until Host Library is installed
        //Initialize class driver objects
        struct class_driver_control class_driver_obj = {0};
        //Register the client
        usb_host_client_config_t client_config = {
            .is_synchronous = false,
            .max_num_event_msg = 5,
            .async = {
                .client_event_callback = client_event_cb,
                .callback_arg = &class_driver_obj,
            }
        };
        usb_host_client_register(&client_config, &class_driver_obj.client_hdl);
        //Allocate a USB transfer
        usb_transfer_t *transfer;
        usb_host_transfer_alloc(1024, 0, &transfer);

        //Event handling loop
        bool exit = false;
        while (!exit) {
            //Call the client event handler function
            usb_host_client_handle_events(class_driver_obj.client_hdl, portMAX_DELAY);
            //Execute pending class driver actions
            if (class_driver_obj.actions & CLASS_DRIVER_ACTION_OPEN_DEV) {
                //Open the device and claim interface 1
                usb_host_device_open(class_driver_obj.client_hdl, class_driver_obj.dev_addr, &class_driver_obj.dev_hdl);
                usb_host_interface_claim(class_driver_obj.client_hdl, class_driver_obj.dev_hdl, 1, 0);
            }
            if (class_driver_obj.actions & CLASS_DRIVER_ACTION_TRANSFER) {
                //Send an OUT transfer to EP1
                memset(transfer->data_buffer, 0xAA, 1024);
                transfer->num_bytes = 1024;
                transfer->device_handle = class_driver_obj.dev_hdl;
                transfer->bEndpointAddress = 0x01;
                transfer->callback = transfer_cb;
                transfer->context = (void *)&class_driver_obj;
                usb_host_transfer_submit(transfer);
            }
            if (class_driver_obj.actions & CLASS_DRIVER_ACTION_CLOSE_DEV) {
                //Release the interface and close the device
                usb_host_interface_release(class_driver_obj.client_hdl, class_driver_obj.dev_hdl, 1);
                usb_host_device_close(class_driver_obj.client_hdl, class_driver_obj.dev_hdl);
                exit = true;
            }
            ... //Handle any other actions required by the class driver
        }

        //Cleanup class driver
        usb_host_transfer_free(transfer);
        usb_host_client_deregister(class_driver_obj.client_hdl);
        ... //Delete the task and any other signal Daemon Task if required
    }

.. note::
    An actual host class driver will likely supported many more features, thus will have a much more complex state machine. A host class driver will likely also need to:

    - Be able to open multiple devices
    - Parse an opened device's descriptors to identify if the device is of the target class
    - Communicate with multiple endpoints of an interface in a particular order
    - Claim multiple interfaces of a device
    - Handle various errors

Lifecycle
"""""""""

The typical life cycle of a client task and class driver will go through the following stages:

#. Wait for some signal regarding the Host Library being installed.
#. Register the client via :cpp:func:`usb_host_client_register` and allocate any other class driver resources (e.g., allocating transfers using :cpp:func:`usb_host_transfer_alloc`).
#. For each new device that the class driver needs to communicate with:

    a. Check if the device is already connected via :cpp:func:`usb_host_device_addr_list_fill`.
    b. If the device is not already connected, wait for a :cpp:enumerator:`USB_HOST_CLIENT_EVENT_NEW_DEV` event from the client event callback.
    c. Open the device via :cpp:func:`usb_host_device_open`.
    d. Parse the device and configuration descriptors via :cpp:func:`usb_host_get_device_descriptor` and :cpp:func:`usb_host_get_active_config_descriptor` respectively.
    e. Claim the necessary interfaces of the device via :cpp:func:`usb_host_interface_claim`.

#. Submit transfers to the device via :cpp:func:`usb_host_transfer_submit` or :cpp:func:`usb_host_transfer_submit_control`.
#. Once an opened device is no longer needed by the class driver, or has disconnected (as indicated by a :cpp:enumerator:`USB_HOST_CLIENT_EVENT_DEV_GONE` event):

    a. Stop any previously submitted transfers to the device's endpoints by calling :cpp:func:`usb_host_endpoint_halt` and :cpp:func:`usb_host_endpoint_flush` on those endpoints.
    b. Release all previously claimed interfaces via :cpp:func:`usb_host_interface_release`.
    c. Close the device via :cpp:func:`usb_host_device_close`.

#. Deregister the client via :cpp:func:`usb_host_client_deregister` and free any other class driver resources.
#. Delete the client task. Signal the Daemon Task if necessary.


.. ---------------------------------------------------- Examples -------------------------------------------------------

Examples
--------

Host Library Examples
^^^^^^^^^^^^^^^^^^^^^

The :example:`peripherals/usb/host/usb_host_lib` demonstrates basic usage of the USB Host Library's API to implement a pseudo class driver.

Class Driver Examples
^^^^^^^^^^^^^^^^^^^^^

The USB Host Stack provides a number examples that implement host class drivers using the Host Library's API.

CDC-ACM
"""""""

* A host class driver for the Communication Device Class (Abstract Control Model) is deployed to `IDF component registry <https://components.espressif.com/component/espressif/usb_host_cdc_acm>`__.
* The :example:`peripherals/usb/host/cdc/cdc_acm_host` example uses the CDC-ACM host driver component to communicate with CDC-ACM devices
* The :example:`peripherals/usb/host/cdc/cdc_acm_bg96` example uses the CDC-ACM host driver component to communicate with non-compliant CDC-ACM devices (i.e., vendor-specific classes that support a subset of CDC-ACM features) such as the Quectel BG96 modem.
* The :example:`peripherals/usb/host/cdc/cdc_acm_vcp` example shows how can you extend the CDC-ACM host driver to interface Virtual COM Port devices.

MSC
"""

* A host class driver for the Mass Storage Class (Bulk-Only Transport) is deployed to `IDF component registry <https://components.espressif.com/component/espressif/usb_host_msc>`__. You can find its example in :example:`peripherals/usb/host/msc`.


.. -------------------------------------------------- API Reference ----------------------------------------------------

API Reference
-------------

The API of the USB Host Library is separated into the following header files. However, it is sufficient for applications to only ``#include "usb/usb_host.h"`` and all of USB Host Library headers will also be included.

- :component_file:`usb/include/usb/usb_host.h` contains the functions and types of the USB Host Library
- :component_file:`usb/include/usb/usb_helpers.h` contains various helper functions that are related to the USB protocol such as descriptor parsing.
- :component_file:`usb/include/usb/usb_types_stack.h` contains types that are are used across multiple layers of the USB Host stack.
- :component_file:`usb/include/usb/usb_types_ch9.h` contains types and macros related to Chapter 9 of the USB2.0 specification (i.e., descriptors and standard requests).


.. include-build-file:: inc/usb_host.inc

.. include-build-file:: inc/usb_helpers.inc

.. include-build-file:: inc/usb_types_stack.inc

.. include-build-file:: inc/usb_types_ch9.inc
