USB Host Driver (USBH)
======================

Introduction
------------

The USB Host Driver (henceforth referred to as USBH) provides a USB Host software interface which abstracts USB devices. The USBH interface provides APIs to...

- manage the device pool (i.e., adding and removing devices)
- address and configure a device (i.e., setting device and configuration descriptors)
- submit transfers to a particular endpoint of a device

Requirements
------------

USB Specification Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Chapter 10 of the USB 2.0 specification outlines some requirements of the USBH (referred to as USBD in the specification). The design of the USBH takes into consideration these requirements from the specification.

- Default pipe of a device is owned by the USBH
- All other pipes are owned and managed by clients of the USBH
- USBH interface must provide the following services

    - Configuration and command mechanism
    - Transfer services via both command and pipe mechanisms
    - Event notification
    - Status reporting and error recovery

Host Stack Requirements
^^^^^^^^^^^^^^^^^^^^^^^

In addition to the USB 2.0 specification requirements, the USBH also takes into consideration the requirements set for the overall Host Stack (see :doc:`./usb_host_notes_design`):

- USBH must not instantiate any tasks/threads
- USBH must be event driven, providing event callbacks and an event processing function

Implementation & Usage
----------------------

Events & Processing
^^^^^^^^^^^^^^^^^^^

The USBH is completely event driven and all event handling is done via then ``usbh_process()`` function. The ``usbh_config_t.proc_req_cb`` callback provided on USBH installation will be called when processing is required. Typically, ``usbh_process()`` will be called from a dedicated thread/task.

The USBH exposes the following event callbacks:

- ``usbh_event_cb_t`` used to indicate various events regarding a particular device and control transfers to EP0. This callback is called from the context of ``usbh_process()``
- ``usbh_ep_cb_t`` used to indicate events for all other endpoints. This callback is not called from the ``usbh_process()`` context (currently called from an HCD interrupt context).

Device Pool
^^^^^^^^^^^

The USBH keeps track of all currently connected devices by internally maintaining a device pool (simply a linked list) where each device is represented by a device object.

The USB 2.0 specification "assumes a specialized client of the USBD, called a hub driver, that acts as a clearinghouse for the addition and removal of devices from a particular hub". As a result, the USBH is completely reliant on an external client(s) (typically a Hub Driver) to inform the USBH of device addition and removal. The USBH provides the following APIs for device addition and removal:

- ``usbh_devs_add()`` which will allocate a new device object and add it to the device pool. The newly added device will be unenumerated, meaning the device object will...

    - be assigned to address 0
    - have no device and configuration descriptor

- ``usbh_devs_remove()`` which will indicate to the USBH that a device has been removed (such as due to a disconnection or a port error).

    - If the device is not currently opened (i.e., used by one or more clients), the USBH will free the underlying device object immediately.
    - If the device is currently opened, a ``USBH_EVENT_DEV_GONE`` event will be propagated and the device will be flagged for removal. The last client to close the device will free the device object.
    - When a device object is freed, a ``USBH_EVENT_DEV_FREE`` event will be propagated. This event is used to indicate that the device's upstream port can be recycled.

Device Enumeration
^^^^^^^^^^^^^^^^^^

Newly added devices will need to be enumerated. The USBH provides various ``usbh_dev_set_...()`` functions to enumerate the device, such as assigning the device's address and setting device/configuration/string descriptors. Given that USBH devices can be shared by multiple clients, attempting to enumerate a device while another client has opened the device can cause issues.

Thus, before calling any ``usbh_dev_set_...()`` enumeration function, a device must be locked for enumeration by calling ``usbh_dev_enum_lock()``. This prevents the device from being opened by any other client but the enumerating client.

After enumeration is complete, the enumerating client can call ``usbh_devs_trigger_new_dev_event()`` to propagate a ``USBH_EVENT_NEW_DEV`` event.

Device Usage
^^^^^^^^^^^^

Clients that want to use a device must open the device by calling ``usbh_devs_open()`` and providing the device's address. The device's address can either be obtained from a ``USBH_EVENT_NEW_DEV`` event or by calling ``usbh_devs_addr_list_fill()``.

Opening a device will do the following:

- Return a ``usb_device_handle_t`` device handle which can be used to refer to the device in various USBH functions
- Increment the device's internal ``open_count`` which indicates how many clients have opened the device. As long as ``open_count > 0``, the underlying device object will not be freed, thus guaranteeing that the device handle refers to a valid device object.

Once a client no longer needs to use a device, the client should call ``usbh_devs_close()`` thus invalidating the device handle.

.. note::

    Most device related APIs accept ``usb_device_handle_t`` as an argument, which means that the calling client must have previously opened the device to obtain the device handle beforehand. This design choice is intentional in order to enforce an "open before use" pattern.

    However, a limited set of APIs (e.g., ``usbh_devs_remove()``)  refer to devices using a Unique Identifier (``uid``) which is assigned on device addition (see ``usbh_devs_add()``). The use of ``uid`` in these functions allows their callers to refer to a device **without needing to open it** due to the lack of a ``usb_device_handle_t``.

    As a result, it is possible that a caller of a ``uid`` function may refer to a device that has already been freed. Thus, callers should account for a fact that these functions may return :c:macro:`ESP_ERR_NOT_FOUND`.

Endpoints & Transfers
^^^^^^^^^^^^^^^^^^^^^

USBH supports transfer to default (i.e., EP0) and non-default endpoints.

For non-default endpoints:

- A client must first allocate the endpoint by calling ``usbh_ep_alloc()`` which assigns a ``usbh_ep_cb_t`` callback and returns a ``usbh_ep_handle_t`` endpoint handle so that the endpoint can be referred to.
- A client can then enqueue a ``urb_t`` transfer to the endpoint by calling ``usbh_ep_enqueue_urb()``.
- The ``usbh_ep_cb_t`` callback is called to indicate transfer completion
- The client must then dequeue the transfer using ``usbh_ep_dequeue_urb()``

Default endpoints are owned and managed by the USBH, thus API for control transfers are different:

- EP0 is always allocated for each device, thus clients do no need to allocate EP0 or provide an endpoint callback.
- Clients call should call ``usbh_dev_submit_ctrl_urb()`` to submit a control transfer to a device's EP0.
- A ``USBH_EVENT_CTRL_XFER`` event will be propagated when the transfer is complete
- Control transfers do not need to be dequeued
