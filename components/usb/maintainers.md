# USB Host Stack Maintainers Notes

This document is intended future maintainers of the ESP-IDF USB Host stack.

Note: The underlying DWC_OTG controller will sometimes be referred to as the Host Controller in this document.

The host driver is currently split into the following layers, ordered from the lowest (furthest away from the user) to the highest layer (closest to the user).

* USB Host lower layer in `usbh_ll.h`
* USB HAL in `usbh_hal.h` and `usbh_hal.c`
* Host Controller Driver in `hcd.c` and `hcd.h`

# USB Host Lower Layer

* `usbh_ll.h` abstracts away the basic register operation of the **DWC** OTG controller
* The LL provides register operations of the DWC OTG controller operating under Host Mode using scatter/gather internal DMA.
* For non-host mode specific register operations (i.e. global registers), the functions are prefixed with `usb_ll_...()`
* For host mode specific register operations, the functions are prefixed with `usbh_ll_...()`

# USB Host HAL

The HAL layer abstracts the DWC_OTG operating in Host Mode using Internal Scatter/Gather DMA. The HAL presents an abstraction of a single Host Port and multiple channels.

## HAL Host Port

- Models a single USB port where a single device can be attached
  - Actions: Port can be powered ON/OFF, reset, suspended
  - Events: When an interrupt occurs, call `usbh_hal_decode_intr()` to decoded the port interrupt.
  - Port can detect various events such as connection, disconnection, enabled (i.e., connected device successfully reset), overcurrent etc.

## HAL Channels

- Channels are essentially the controllers abstraction of USB pipes. At any one point in time, a channel can be configured to map to a particular endpoint on a particular connected device (i.e., a particular device address).
- Channels have to be allocated and freed. It's possible to change a channel's endpoint characteristics (i.e., EP number, device address, direction, transfer type etc) so long as the channel is halted (i.e., not actively executing transfer descriptors).
  - Use `usbh_hal_chan_alloc()` to allocate a channel
  - Once allocated, use `usbh_hal_chan_set_ep_char()` to set the Endpoint characteristics of the channel (i.e., the information of the endpoint that the channel is communicating with). There are also some `usbh_hal_chan_set...()` functions to change a particular characteristic.
  - Once the channel is no longer needed, call `usbh_hal_chan_free()` to free the channel
- Channels use a list of Queue Transfer Descriptors (QTDs) to executed USB transfers.
  - A transfer descriptor list must be filled using `usbh_hal_xfer_desc_fill()`
  - Once filled, a channel can be activated using `usbh_hal_chan_activate()`
  - Once the channel is done (i.e., a descriptor with the `USBH_HAL_XFER_DESC_FLAG_HOC` is executed), a `USBH_HAL_CHAN_EVENT_CPLT` event is generated. The channel is now halted
  - Call `usbh_hal_xfer_desc_parse()` to parse the results of the descriptor list
  - If you need to halt the channel early (such as aborting a transfer), call `usbh_hal_chan_request_halt()`
- In case of a channel error event:
  - Call `usbh_hal_chan_get_error()` to get the specific channel error that occurred
  - You must call `usbh_hal_chan_clear_error()` after an error to clear the error and allow the channel to continue to be used.

# Host Controller Driver (HCD)

The HCD (Host Controller Driver) abstracts the DWC_OTG as N number of ports and an arbitrary number of pipes that can be routed through one of the ports to a device. However note that **the underlying hardware controller only has a single port, so technically only one port can ever be enabled**.

- In other words, the HCD essentially implements a root hub (not fully behavioral compliant) that contains a single port.
- Pipes are "an association between an endpoint on a device and software on the host". URBs (USB Request Block) represent a USB transfer that can be enqueued into a pipe for transmission, and dequeued from a pipe when completed.

The HCD currently has the following limitations:

- HCD **does not** "present the root hub and its behavior according to the hub class definition". We currently don't have a hub driver yet, so the port commands in the driver do not fully represent an interface of a USB hub as described in 10.4 of the USB2.0 spec.
- No more than 8 pipes can be allocated at any one time due to underlying Host Controllers 8 channel limit. In the future, we could make particular pipes share a single Host Controller channel.
- The HCD currently only supports Control and Bulk transfer types.
- If you are connecting to a device with a large MPS requirements (e.g., Isochronous transfers), you may need to call `hcd_port_set_fifo_bias()` to adjust the size of the internal FIFO

## HCD Port

- An HCD port can be as a simplified version of a port on the Root Hub of the host controller. However, the complexity of parsing Hub Requests is discarded in favor of port commands (`hcd_port_cmd_t`) as the current USB Host Stack does not support hubs yet.
- A port must first initialized before it can be used. A port is identified by its handled of type `hcd_port_handle_t`
- The port can be manipulated using commands such as:
  - Powering the port ON/OFF
  - Issuing reset/resume signals
- The various host port events are represented in the `hcd_port_event_t` enumeration
- When a fatal error (such as a sudden disconnection or a port over current), the port will be put into the HCD_PORT_STATE_RECOVERY state. The port can be deinitialized from there, or recovered using `hcd_port_recover()`. All the pipes routed through the port will be made invalid.
- The FIFO bias of a port can be set using `hcd_port_set_fifo_bias()`. Biasing the FIFO will affect the permissible MPS sizes of pipes. For example, if the connected device has an IN endpoint with large MPS (e.g., 512 bytes), the FIFO should be biased as `HCD_PORT_FIFO_BIAS_RX`.

## HCD Pipes

- URBs can be enqueued into a pipe. Pipes use a linked list internally, so there is in-theory no limit to the number of URBs that can be enqueued.
- URBs need to be dequeued once they are completed.
- URBs need to have the transfer information (such as data buffer, transfer length in bytes) filled before they should be enqueued.
- URBs will be owned by the HCD until they are dequeued. Thus, users should not attempt to modify an URB object (and the URB's data buffer) until the URB is dequeued.
- The URB is defined in `usb_private.h` instead of `hcd.h` so that the same structure can shared throughout the entire Host stack. Each layer simply needs to pass the pointer of the URB to the next layer thus minimizing the amount of copying required.

## HCD SW Arch

The software arch for the HCD has the following properties and intentions:

- Some static functions need to be called in critical sections whilst others need not. Therefore, static functions prefixed by an underscore ((e.g., `_some_static_func()`) will need to be called in critical sections.
- Some static functions may be blocking. Those functions will have a note in their descriptions.
- The HCD communicates events entirely through callbacks and polling/handling functions. The client can choose what type of data structure they want to use in the callbacks to keep track of port and pipe events. Callbacks don't even need to be used, and the HCD should be able to operate entirely on a polling basis.
- The port and each pipe have to be treated as completely separate entities (wither their own handles and events). This allows client to group these entities however it sees fit. For example, the client can:
  - Let a hub driver manage the port, and manipulate the port via its port handle
  - Group pipes into interfaces and expose the interface as a single entity
- The HCD will not internally allocate any tasks. It is up to the client to decide how distribute workload (e.g., a single while loop polling a port and all its pipes vs or each pipe having its own task).
- The HCD uses an interrupt process things that require low latency such as processing pipe transfer requests. Where possible, processing is deferred to the `hcd_port_handle_event()` to reduce ISR workload.

### Events

In order to communicate events to the client of the HCD, the HCD does not attempt to allocate any queues to store events that have occurred. Callbacks are used in order to achieve maximum flexibility. Within these callbacks, the client of the HCD is free too use whatever OS primitives they want to store/forward these events.

There are two types callbacks that the HCD offers:

- Port callback will run whenever an event a port occurs. `hcd_port_handle_event()` should be called after a port event occurs.
- A pipe callback on each pipe, that will run when a event occurs on a pipe. 

The client of the HCD can also forego callbacks entirely and simply poll for port and pipe events using the `hcd_port_handle_event()` and `hcd_pipe_get_event()` respectively.

- Some of the port and pipe commands may need to block for a certain condition before the command is executed. For example when suspending the port, the port command must block until all pipes to finish their current transfers. The blocking and unblocking is handled by an internal event mechanism.

### Thread Safety

The HCD API is thread safe however the following limitations should be noted:

- It is the client's responsibility to ensure that `hcd_install()` is called before any other HCD function is called
- Likewise, it is the client's responsibility to ensure that events and pipes are cleared before calling `hcd_port_deinit()`.
- `hcd_port_command()` is thread safe, but only one port command can be executed at any one time. Therefore HCD internally used a mutex to protect against concurrent commands.
- If multiple threads attempt to execute a command on the sample one, all but one of those threads will return with an invalid state error.
- HCD functions should not be called from critical sections and interrupts, as some functions (e.g., `hcd_port_command()` and `hcd_pipe_command()`) may be blocking.