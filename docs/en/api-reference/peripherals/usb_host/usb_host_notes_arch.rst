USB Host Maintainers Notes (Architecture)
=========================================

The Host Stack is roughly split into multiple layers of abstraction, with each layer representing different USB concepts and a different level of USB Host operation. For example, a higher layer may present an abstraction of devices and application data transfers, whereas a lower layer may present an abstraction of endpoints and USB transfers.

Layer Descriptions
------------------

The layers of the Host Stack are described in the following table. The layers are ordered from lowest layer (i.e, furthest away from the user) to highest layer (i.e., closest to the user).

.. list-table:: Host Stack Layers
    :widths: 20 10 70
    :header-rows: 1

    * - Layer
      - Files
      - Description
    * - Host Controller (DWC_OTG)
      - N/A
      - This layer represents the USB Controller Hardware of the {IDF_TARGET_NAME}. The API presented by this layer is the register interface of the controller.
    * - LL
      - ``usbh_ll.h``
      - The LL (Low Level) layer abstracts the basic register access of the USB controller according to ESP-IDF's :doc:`Hardware Abstraction API Guidelines <../../../api-guides/hardware-abstraction>`. In other words, this layer provides APIs to access the controller's registers and format/parse the controller's DMA descriptors.
    * - HAL
      - ``usbh_hal.h``, ``usbh_hal.c``
      - The HAL (Hardware Abstraction Layer) abstracts the operating steps of the USB controller into functions according to ESP-IDF's :doc:`Hardware Abstraction API Guidelines <../../../api-guides/hardware-abstraction>`. This layer also abstracts the controller's host port and host channels, and provides APIs to operate the them.
    * - HCD
      - ``hcd.h``, ``hcd.c``
      - The HCD (Host Controller Driver) acts as hardware agnostic API for all USB controllers (i.e., an API that can theoretically be used with any USB controller implementation). This layer also abstracts the root port (i.e., root hub) and USB pipes.
    * - USBH and Hub Driver
      - ``usbh.h``, ``usbh.c``
      - The USBH (USB Host Driver) layer is equivalent to the USBD layer described in chapter 10 of the USB2.0 specification. The USBH presents an abstraction of USB devices, internally manages a list of connected devices (i.e., device pool), and also arbitrates device sharing between clients (i.e., tracks which endpoints are in use and also presents a shared endpoint 0).
    * - Hub Driver
      - ``hub.h``, ``hub.c``
      - The Hub Driver layer acts as a special client of the USBH that is responsible for handling device attachment/detachment, and notifying the USBH of such events. For device attachment, the Hub Driver also handles the enumeration process as well.
    * - USB Host Library
      - ``usb_host.h``, ``usb_host.c``
      - The USB Host Library layer is the lowest public API layer of the Host Stack and presents the concept of USB Host Clients. The abstraction of clients allows for multiple class drivers to coexist simultaneously (where each class roughly maps to a single client) and also acts as a mechanism for division of labor (where each client is responsible for its own processing and event handling).
    * - Host Class Drivers
      - See the `ESP-IDF Extra Components repository <https://github.com/espressif/idf-extra-components>`_ or the USB Host examples in ESP-IDF (via :example:`peripherals/usb/host`).
      - The Host Class Drivers implement the host side of a particular device class (e.g., CDC, MSC, HID). The exposed API is specific to each class driver.

Layer Dependencies
------------------

The Host Stack roughly follows a top to bottom hierarchy with inter-layer dependencies. Given layers A (highest), B, and C (lowest), the Host Stack has the following inter-layer dependency rules:

- a particular layer can use the API of any layer directly below (Layer A using layer B is allowed)
- a particular layer can use the API of any layer indirectly below (Layer A using layer C is allowed) i.e., skipping layers.
- a particular layer must not use the API of any layer above (Layer C using layer A/B is forbidden)

.. note::

  Layer skipping is permitted in order to circumvent the need to repeat the same abstraction across multiple layers. For example, the abstraction of pipes are presented at the HCD layer but are used by multiple layers above.

.. todo::

  Add diagram of API dependencies between layers
