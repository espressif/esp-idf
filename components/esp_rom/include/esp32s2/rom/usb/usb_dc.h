/*
 * SPDX-FileCopyrightText: 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief USB device controller APIs
 *
 * This file contains the USB device controller APIs. All device controller
 * drivers should implement the APIs described in this file.
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * USB endpoint direction and number.
 */

#define USB_EP_DIR_MASK 0x80
#define USB_EP_DIR_IN   0x80
#define USB_EP_DIR_OUT  0x00

/**
 * USB Driver Status Codes
 */
enum usb_dc_status_code {
    USB_DC_ERROR,        /* USB error reported by the controller */
    USB_DC_RESET,        /* USB reset */
    /* USB connection established, hardware enumeration is completed */
    USB_DC_CONNECTED,
    USB_DC_CONFIGURED,   /* USB configuration done */
    USB_DC_DISCONNECTED, /* USB connection lost */
    USB_DC_SUSPEND,      /* USB connection suspended by the HOST */
    USB_DC_RESUME,       /* USB connection resumed by the HOST */
    USB_DC_INTERFACE,    /* USB interface selected */
    USB_DC_SET_HALT,     /* Set Feature ENDPOINT_HALT received */
    USB_DC_CLEAR_HALT,   /* Clear Feature ENDPOINT_HALT received */
    USB_DC_UNKNOWN       /* Initial USB connection status */
};

/**
 * USB Endpoint Callback Status Codes
 */
enum usb_dc_ep_cb_status_code {
    USB_DC_EP_SETUP,    /* SETUP received */
    /* Out transaction on this EP, data is available for read */
    USB_DC_EP_DATA_OUT,
    USB_DC_EP_DATA_IN,  /* In transaction done on this EP */
};

/**
 * USB Endpoint type
 */
enum usb_dc_ep_type {
    USB_DC_EP_CONTROL = 0,  /* Control type endpoint */
    USB_DC_EP_ISOCHRONOUS,  /* Isochronous type endpoint */
    USB_DC_EP_BULK,         /* Bulk type endpoint */
    USB_DC_EP_INTERRUPT     /* Interrupt type endpoint  */
};

/**
 * USB Endpoint Configuration.
 */
struct usb_dc_ep_cfg_data {
    /** The number associated with the EP in the device
     *  configuration structure
     *       IN  EP = 0x80 | \<endpoint number\>
     *       OUT EP = 0x00 | \<endpoint number\>
     */
    uint8_t ep_addr;
    uint16_t ep_mps;             /** Endpoint max packet size */
    enum usb_dc_ep_type ep_type; /** Endpoint type */
};

/**
 * Callback function signature for the USB Endpoint status
 */
typedef void (*usb_dc_ep_callback)(uint8_t ep,
                                   enum usb_dc_ep_cb_status_code cb_status);

/**
 * Callback function signature for the device
 */
typedef void (*usb_dc_status_callback)(enum usb_dc_status_code cb_status,
                                       uint8_t *param);

/**
 * @brief attach USB for device connection
 *
 * Function to attach USB for device connection. Upon success, the USB PLL
 * is enabled, and the USB device is now capable of transmitting and receiving
 * on the USB bus and of generating interrupts.
 *
 * @return 0 on success, negative errno code on fail.
 */
int usb_dc_attach(void);

/**
 * @brief detach the USB device
 *
 * Function to detach the USB device. Upon success, the USB hardware PLL
 * is powered down and USB communication is disabled.
 *
 * @return 0 on success, negative errno code on fail.
 */
int usb_dc_detach(void);

/**
 * @brief reset the USB device
 *
 * This function returns the USB device and firmware back to it's initial state.
 * N.B. the USB PLL is handled by the usb_detach function
 *
 * @return 0 on success, negative errno code on fail.
 */
int usb_dc_reset(void);

/**
 * @brief set USB device address
 *
 * @param[in] addr device address
 *
 * @return 0 on success, negative errno code on fail.
 */
int usb_dc_set_address(const uint8_t addr);

/**
 * @brief set USB device controller status callback
 *
 * Function to set USB device controller status callback. The registered
 * callback is used to report changes in the status of the device controller.
 *
 * @param[in] cb callback function
 *
 * @return 0 on success, negative errno code on fail.
 */
int usb_dc_set_status_callback(const usb_dc_status_callback cb);

/**
 * @brief check endpoint capabilities
 *
 * Function to check capabilities of an endpoint. usb_dc_ep_cfg_data structure
 * provides the endpoint configuration parameters: endpoint address,
 * endpoint maximum packet size and endpoint type.
 * The driver should check endpoint capabilities and return 0 if the
 * endpoint configuration is possible.
 *
 * @param[in] cfg Endpoint config
 *
 * @return 0 on success, negative errno code on fail.
 */
int usb_dc_ep_check_cap(const struct usb_dc_ep_cfg_data *const cfg);

/**
 * @brief configure endpoint
 *
 * Function to configure an endpoint. usb_dc_ep_cfg_data structure provides
 * the endpoint configuration parameters: endpoint address, endpoint maximum
 * packet size and endpoint type.
 *
 * @param[in] cfg Endpoint config
 *
 * @return 0 on success, negative errno code on fail.
 */
int usb_dc_ep_configure(const struct usb_dc_ep_cfg_data *const cfg);

/**
 * @brief set stall condition for the selected endpoint
 *
 * @param[in] ep Endpoint address corresponding to the one
 *               listed in the device configuration table
 *
 * @return 0 on success, negative errno code on fail.
 */
int usb_dc_ep_set_stall(const uint8_t ep);

/**
 * @brief clear stall condition for the selected endpoint
 *
 * @param[in] ep Endpoint address corresponding to the one
 *               listed in the device configuration table
 *
 * @return 0 on success, negative errno code on fail.
 */
int usb_dc_ep_clear_stall(const uint8_t ep);

/**
 * @brief check if selected endpoint is stalled
 *
 * @param[in]  ep       Endpoint address corresponding to the one
 *                      listed in the device configuration table
 * @param[out] stalled  Endpoint stall status
 *
 * @return 0 on success, negative errno code on fail.
 */
int usb_dc_ep_is_stalled(const uint8_t ep, uint8_t *const stalled);

/**
 * @brief halt the selected endpoint
 *
 * @param[in] ep Endpoint address corresponding to the one
 *               listed in the device configuration table
 *
 * @return 0 on success, negative errno code on fail.
 */
int usb_dc_ep_halt(const uint8_t ep);

/**
 * @brief enable the selected endpoint
 *
 * Function to enable the selected endpoint. Upon success interrupts are
 * enabled for the corresponding endpoint and the endpoint is ready for
 * transmitting/receiving data.
 *
 * @param[in] ep Endpoint address corresponding to the one
 *               listed in the device configuration table
 *
 * @return 0 on success, negative errno code on fail.
 */
int usb_dc_ep_enable(const uint8_t ep);

/**
 * @brief disable the selected endpoint
 *
 * Function to disable the selected endpoint. Upon success interrupts are
 * disabled for the corresponding endpoint and the endpoint is no longer able
 * for transmitting/receiving data.
 *
 * @param[in] ep Endpoint address corresponding to the one
 *               listed in the device configuration table
 *
 * @return 0 on success, negative errno code on fail.
 */
int usb_dc_ep_disable(const uint8_t ep);

/**
 * @brief flush the selected endpoint
 *
 * @param[in] ep Endpoint address corresponding to the one
 *               listed in the device configuration table
 *
 * @return 0 on success, negative errno code on fail.
 */
int usb_dc_ep_flush(const uint8_t ep);

/**
 * @brief write data to the specified endpoint
 *
 * This function is called to write data to the specified endpoint. The supplied
 * usb_ep_callback function will be called when data is transmitted out.
 *
 * @param[in]  ep        Endpoint address corresponding to the one
 *                       listed in the device configuration table
 * @param[in]  data      pointer to data to write
 * @param[in]  data_len  length of data requested to write. This may
 *                       be zero for a zero length status packet.
 * @param[out] ret_bytes bytes scheduled for transmission. This value
 *                       may be NULL if the application expects all
 *                       bytes to be written
 *
 * @return 0 on success, negative errno code on fail.
 */
int usb_dc_ep_write(const uint8_t ep, const uint8_t *const data,
                    const uint32_t data_len, uint32_t *const ret_bytes);



/**
 * @brief Indicate if the write to an IN endpoint (using usb_dc_ep_write) would block
 * to wait until the endpoint has enoug space
 *
 * @param[in]  ep        Endpoint address corresponding to the one
 *                       listed in the device configuration table
 *
 * @return 0 when writable, 0 when not, negative errno code on fail.
 */
int usb_dc_ep_write_would_block(const uint8_t ep);


/**
 * @brief read data from the specified endpoint
 *
 * This function is called by the Endpoint handler function, after an OUT
 * interrupt has been received for that EP. The application must only call this
 * function through the supplied usb_ep_callback function. This function clears
 * the ENDPOINT NAK, if all data in the endpoint FIFO has been read,
 * so as to accept more data from host.
 *
 * @param[in]  ep           Endpoint address corresponding to the one
 *                          listed in the device configuration table
 * @param[in]  data         pointer to data buffer to write to
 * @param[in]  max_data_len max length of data to read
 * @param[out] read_bytes   Number of bytes read. If data is NULL and
 *                          max_data_len is 0 the number of bytes
 *                          available for read should be returned.
 *
 * @return 0 on success, negative errno code on fail.
 */
int usb_dc_ep_read(const uint8_t ep, uint8_t *const data,
                   const uint32_t max_data_len, uint32_t *const read_bytes);

/**
 * @brief set callback function for the specified endpoint
 *
 * Function to set callback function for notification of data received and
 * available to application or transmit done on the selected endpoint,
 * NULL if callback not required by application code.
 *
 * @param[in] ep Endpoint address corresponding to the one
 *               listed in the device configuration table
 * @param[in] cb callback function
 *
 * @return 0 on success, negative errno code on fail.
 */
int usb_dc_ep_set_callback(const uint8_t ep, const usb_dc_ep_callback cb);

/**
 * @brief read data from the specified endpoint
 *
 * This is similar to usb_dc_ep_read, the difference being that, it doesn't
 * clear the endpoint NAKs so that the consumer is not bogged down by further
 * upcalls till he is done with the processing of the data. The caller should
 * reactivate ep by invoking usb_dc_ep_read_continue() do so.
 *
 * @param[in]  ep           Endpoint address corresponding to the one
 *                          listed in the device configuration table
 * @param[in]  data         pointer to data buffer to write to
 * @param[in]  max_data_len max length of data to read
 * @param[out] read_bytes   Number of bytes read. If data is NULL and
 *                          max_data_len is 0 the number of bytes
 *                          available for read should be returned.
 *
 * @return 0 on success, negative errno code on fail.
 */
int usb_dc_ep_read_wait(uint8_t ep, uint8_t *data, uint32_t max_data_len,
                        uint32_t *read_bytes);


/**
 * @brief Continue reading data from the endpoint
 *
 * Clear the endpoint NAK and enable the endpoint to accept more data
 * from the host. Usually called after usb_dc_ep_read_wait() when the consumer
 * is fine to accept more data. Thus these calls together acts as flow control
 * mechanism.
 *
 * @param[in]  ep           Endpoint address corresponding to the one
 *                          listed in the device configuration table
 *
 * @return 0 on success, negative errno code on fail.
 */
int usb_dc_ep_read_continue(uint8_t ep);

/**
 * @brief Get endpoint max packet size
 *
 * @param[in]  ep           Endpoint address corresponding to the one
 *                          listed in the device configuration table
 *
 * @return enpoint max packet size (mps)
 */
int usb_dc_ep_mps(uint8_t ep);


/**
 * @brief Poll for interrupts that need to be handled
 *
 * When the USB interrupt is not hooked up to an actual CPU interrupt, you
 * can call this periodically to handle the USB events that need handling.
 */
void usb_dc_check_poll_for_interrupts(void);


/*
 * @brief Prepare for USB persist
 *
 * This takes the USB peripheral offline in such a way that it seems 'just busy' to the
 * host. This way, the chip can reboot (e.g. into bootloader mode) and pick up the USB
 * configuration again, without the conenction to the host being interrupted.
 *
 * @note Actual persistence is depending on USBDC_PERSIST_ENA being set in flags, as this
 * is also used to e.g. reboot into DFU mode.
 *
 * @note Please reboot soon after calling this.
 */
int usb_dc_prepare_persist(void);

/*
 * @brief USB interrupt handler
 *
 * This can be hooked up by the OS to the USB peripheral interrupt.
 */
void usb_dw_isr_handler(void);

/**
 * @brief Provide IDF with an interface to clear the static variable usb_dw_ctrl
 *
 *
 */
void usb_dw_ctrl_deinit(void);



#ifdef __cplusplus
}
#endif
