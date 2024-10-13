.. Translation not required: According to the USB developers, all of the ``usb_host_notes_*`` files are just internal notes for ESP-IDF developers, not for our end users. So we don't need to translate them at all.

USB Host Enumeration Driver (Enum)
==================================

Introduction
------------

The USB Host Enumeration Driver (henceforth referred to as Enum Driver) provides a software interface which abstracts away the USB device enumeration process. The Enum Driver provides a simple API to start, proceed, complete and cancel the enumeration of a particular device. Internally, the Enum Driver will handle all stages of the enumeration process such as requesting various descriptors and setting the device's configuration.

Requirements
------------

USB Specification Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Chapter 9.1.2 of the USB 2.0 specification outlines some actions, when a USB device is attached to a powered port.

The design of the Enum Driver takes into consideration following actions:

- **The hub performs the required reset processing for the port.** ``enum_config_t.enum_event_cb`` call with ``ENUM_EVENT_RESET_REQUIRED`` event used for Hub Driver notification.
- **The host assigns a unique address to the USB device.** The Enum Driver keeps the device address value and assigns a unique address to the USB device, moving the device to the Address state.
- **The host reads the device descriptor to determine what actual maximum data payload size this USB device's default pipe can use**. The Enum Driver reads the device descriptor to determine what actual maximum data payload size this USB device's default pipe can use.
- **The host reads the configuration information from the device by reading each configuration zero to n-1, where n is the number of configurations.** This requirement simplified to reading only one configuration information from the device by reading configuration with default number, which could be selected by ``enum_config_t.enum_filter_cb`` call.
- **The host assigns a configuration value to the device.** The Enum Driver assigns a configuration value to the device.

.. note::

    Typically, most USB devices only contain a single configuration. Thus, the Enum Driver defaults to selecting the configuration with `bConfigurationValue = 1`.

    If users would like to select a different configuration, the Enum Driver provides an enumeration filter callback feature (enabled via :ref:`CONFIG_USB_HOST_ENABLE_ENUM_FILTER_CALLBACK`). This callback is called during the enumeration process and allows users to decide what devices to enumerate, and which ``bConfigurationValue`` to use.

.. note::

    For more detailed information about Bus enumeration, please refer to `USB 2.0 Specification <https://www.usb.org/document-library/usb-20-specification>`_ > Chapter 9.1.2 **Bus Enumeration**.

Host Stack Requirements
^^^^^^^^^^^^^^^^^^^^^^^

In addition to the USB 2.0 specification requirements, the Enum Driver also takes into consideration the requirements set for the overall Host Stack (see :doc:`./usb_host_notes_design`):

- Enum Driver must not instantiate any tasks/threads
- Enum Driver must be event driven, providing event callbacks and an event processing function
- Enum Driver must use only API from underlying layer (USBH)

Implementation & Usage
----------------------

Host Stack Interaction
^^^^^^^^^^^^^^^^^^^^^^

The Enum Driver takes place between USB Host layer and USBH layer with a possibility to select configuration with ``enum_config_t.enum_event_cb`` callback provided on Enum Driver installation.

Events & Processing
^^^^^^^^^^^^^^^^^^^

The Enum Driver is completely event driven and all event handling is done via the ``enum_process()`` function. The ``enum_config_t.proc_req_cb`` callback provided on Enum Driver installation will be called when processing is required. Typically, ``enum_process()`` will be called from a shared USB Host stack thread/task.

The Enum Driver exposes the following event callbacks:

- ``enum_event_cb_t`` used to indicate various events regarding an enumeration process. This callback is called from the context of ``enum_process()``

The Enum Driver exposes the following events:

- ``ENUM_EVENT_STARTED`` Enumeration process has been started
- ``ENUM_EVENT_RESET_REQUIRED`` Enumeration process requires device reset
- ``ENUM_EVENT_COMPLETED`` Enumeration process has been completed
- ``ENUM_EVENT_CANCELED`` Enumeration process has been canceled (due to internal error or via ``enum_cancel()`` function call)

Device Enumeration
^^^^^^^^^^^^^^^^^^

The USB device enumeration process implemented in the Enum Driver is mostly based same process in the `Windows USB stack <https://techcommunity.microsoft.com/t5/microsoft-usb-blog/how-does-usb-stack-enumerate-a-device/ba-p/270685>`__. The Enum Driver's enumeration process involves the following steps:

#. First device descriptor request (to obtain the MPS of EP0)
#. Second device reset (to workaround some non-compliant devices)
#. Set device address
#. Second device descriptor request (to obtain and store the full device descriptor)
#. Configuration descriptor request (to obtain the full configuration descriptor of selected configuration)
#. Language ID Table request (checks to see if en-US is supported)
#. Manufacturer string descriptor request
#. Product string descriptor request
#. Serial number string descriptor request
#. Set configuration request (sets the device to target configuration number)

.. note::

    String descriptors are optional. If a device does not support string descriptors, these stages could be omitted.

Enumeration Stages
^^^^^^^^^^^^^^^^^^

The Enum Driver splits the enumeration process into multiple stages which are executed linearly. Depending on the connected device, some stages (such as fetching the string descriptors) can be skipped. When a stage completes, a call to the ``enum_config_t.proc_req_cb`` callback must be made to trigger a subsequent call of ``enum_process()``.The subsequent call of ``enum_process()`` will then select and execute the next stage of enumeration. Stage completion can trigger the ``enum_config_t.proc_req_cb`` callback in one of the following ways:

- Inside the control transfer completion callback (for stages that send a control transfer)
- Direct call to ``enum_config_t.proc_req_cb`` (for stages that don't need to wait for any event)
- Inside ``enum_proceed()`` (for stages that require some action to be carried out outside the Enum Driver)

Any control transfer made during enumeration is split into two stages, where the first stage executes the transfer and the second stage (suffixed with ``_CHECK``) will check the results of the transfers.

When requesting a variable length descriptors (e.g., configuration or string descriptors), the request is split into two control transfers. The first control transfer is fixed in length which only reads the header of the descriptor. The ``bLength`` field of the descriptor's header indicates the full length of the entire descriptor and is used to set the size of the second transfer which fetches the entire descriptor. As a result, any request for a variable length descriptor is split into four stages:

- Get short **ANY** descriptor (prefixed with ``GET_SHORT_...``)
- Check short **ANY** descriptor (prefixed with ``CHECK_SHORT_...``)
- Get full **ANY** descriptor (prefixed with ``GET_FULL_...``)
- Check full **ANY** descriptor (prefixed with ``CHECK_FULL_...``)

.. note::

    Retrieving the Device Descriptor is an exception here because the second reset is taken place after retrieving short Device Descriptor.

Cancel Enumeration
^^^^^^^^^^^^^^^^^^

In some cases (such as a device disconnection), an ongoing enumeration process may need to be cancelled. An ongoing enumeration can be cancelled (regardless of its current stage) by calling ``enum_cancel()`` which will change the enumeration process's current stage to ``ENUM_STAGE_CANCEL``.

On the next call to ``enum_process``, the Enum Driver will execute the ``ENUM_STAGE_CANCEL`` which does the following:

- releases the device's enumeration lock.
- frees all resources related to the current device.
- propagates the ``ENUM_EVENT_CANCELED`` event.
