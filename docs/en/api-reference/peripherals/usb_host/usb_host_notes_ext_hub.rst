USB Host External Hub Driver (Ext Hub)
======================================

Introduction
------------

The External Hub Driver (henceforth referred to as Ext Hub Driver)

Requirements
------------

USB Specification Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Chapter 11 of the USB 2.0 specification outlines some aspects, when a USB Hub device is attached to a powered port.

The design of the Ext Driver takes into consideration the following:

- **Connectivity behavior**
- **Power management**
- **Device connect/disconnect detection**
- **Bus fault detection and recovery**
- **High-, full-, and low-speed device support**

.. note::

    For more detailed information, please refer to `USB 2.0 Specification <https://www.usb.org/document-library/usb-20-specification>`_ > Chapter 11.1 **Overview**.

Host Stack Requirements
^^^^^^^^^^^^^^^^^^^^^^^

In addition to the USB 2.0 specification requirements, the Ext Hub Driver also takes into consideration the requirements set for the overall Host Stack (see :doc:`./usb_host_notes_design`):

- Ext Hub Driver must not instantiate any tasks/threads
- Ext Hub Driver must be event driven, providing event callbacks and an event processing function
- Ext Hub Driver must use only API from underlying layer (USBH)

Implementation & Usage
----------------------

Host Stack Interaction
^^^^^^^^^^^^^^^^^^^^^^

The Ext Hub Driver takes place between USB Host layer and USBH layer, next to the Hub Driver. The Hub Driver and the Ext Hub Driver were split into two Drivers to achieve the goal of logic distinguishing between root Hub and external Hub.

Device handling
^^^^^^^^^^^^^^^

The Ext Hub Driver can be installed via ``ext_hub_install()`` call and uninstalled via ``ext_hub_uninstall()`` call. After installation the Ext Hub provides the following APIs for external Hub addition and removal:

- ``ext_hub_new_dev()`` which will verify the device class (`HUB_CLASSCODE (09H)`) and, if the device has the Hub class, the Ext Hub Driver:

    - allocates a new device object
    - adds it to the external device pool
    - starts the process of Hub configuration (retrieving Hub Descriptor, Device status and Hub status)

- ``ext_hub_dev_gone()`` which will verify the device in the Ext Hub Driver list and start the process of external Hub device removing.

Events & Processing
^^^^^^^^^^^^^^^^^^^

The Ext Hub Driver is completely event driven and all event handling is done via the ``ext_hub_process()`` function. The ``ext_hub_config_t.proc_req_cb`` callback provided on the Ext Hub Driver installation will be called when processing is required. Typically, ``ext_hub_process()`` will be called from the Hub Driver ``hub_process()`` processing function.

The Ext Hub Driver does not expose any event callback.

