USB Host
========

.. warning::
    The USB Host Library API is a beta version thus is subject to change.

The following document lists the API and types of the USB Host Library (that is currently under development).

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
