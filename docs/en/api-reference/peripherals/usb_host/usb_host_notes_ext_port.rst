USB Host External Port Driver (Ext Port)
========================================

Introduction
------------

The External Port Driver (henceforth referred to as Ext Port Driver) isolates the handling process for downstream facing ports, which are provided by the Ext Hub Driver.

.. note::

    For more detailed information, please refer to `USB 2.0 Specification <https://www.usb.org/document-library/usb-20-specification>`_ > Chapter 11.5 **Downstream Facing Ports**.

Requirements
------------

Host Stack Requirements
^^^^^^^^^^^^^^^^^^^^^^^

The Ext Port Driver takes into consideration the requirements set for the overall Host Stack (see :doc:`./usb_host_notes_design`):

- The Ext Port Driver must not instantiate any tasks/threads
- The Ext Port Driver must be event driven, providing event callbacks and an event processing function
- The Ext Port Driver must use only API from underlying layer (The Ext Hub Driver)

Implementation & Usage
----------------------

Host Stack Interaction
^^^^^^^^^^^^^^^^^^^^^^

The Ext Port Driver is a part of The Ext Hub Driver, so the interaction and hierarchical place in USB Host Stack is the same as for the Ext Hub Driver. The Ext Hub and the Ext Port Drivers were split into two Drivers to achieve the goal of logic distinguishing between external Hubs and Downstream Facing Ports handling.

Ports handling
^^^^^^^^^^^^^^^

The Ext Port Driver can be installed via ``ext_port_install()`` call and uninstalled via ``ext_port_uninstall()`` call.

After installation, the Ext Port Driver API could be requested via ``ext_port_get_driver()`` call.

The Ext Port Driver API
-----------------------

The Ext Port Driver provides an API, which could be split into three groups: object control, device control and general.

The Ext Port Driver: Object Control
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - Create object
    - Delete object

The Ext Port Driver: Port Control
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - Reset
    - Disable
    - Recycle
    - Activate
    - Get Speed
    - Get status
    - Set status
    - Gone

The Ext Port Driver: General Purpose
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    - Request processing

Events & Processing
-------------------

The Ext Port Driver is completely event driven and all event handling is done via the ``ext_port_process()`` function. The ``ext_port_driver_config_t.proc_req_cb`` callback provided on the Ext Port Driver installation will be called when processing is required. Typically, ``ext_port_process()`` will be called from the Hub Driver ``hub_process()`` processing function.

The Ext Port Driver exposes the following events via ``ext_port_driver_config_t.event_cb``:

- ``EXT_PORT_CONNECTED`` Downstream facing port has a device connection event
- ``EXT_PORT_RESET_COMPLETED`` Downstream facing port has a device and completed the port reset
- ``EXT_PORT_DISCONNECTED`` Downstream facing port has a device disconnection event

The Ext Port Driver ports processing is based on the Hub class-specific request ``Get Port Status``.

After successful completion of the class-specific request ``Get Port Status`` and setting the new port status, the Ext Port Driver continues the port handling while it is required by ports' state and status.

.. note::

    For more detailed information, please refer to `USB 2.0 Specification <https://www.usb.org/document-library/usb-20-specification>`_ > Chapter 11.24.2.7 **Get Port Status**
