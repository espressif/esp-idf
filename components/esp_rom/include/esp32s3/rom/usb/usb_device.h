/*
 * SPDX-FileCopyrightText: 2006 Bertrik Sikken (bertrik@sikken.nl)
 * SPDX-FileContributor: 2016 Intel Corporation
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * LPCUSB, an USB device driver for LPC microcontrollers
 */

/**
 * @file
 * @brief USB device core layer APIs and structures
 *
 * This file contains the USB device core layer APIs and structures.
 */

#pragma once

#include <stddef.h>
#include <sys/cdefs.h>
#include "usb_dc.h"
#include "esp_assert.h"

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************
 *  USB configuration
 **************************************************************************/

#define MAX_PACKET_SIZE0    64        /**< maximum packet size for EP 0 */
//Note: for FS this should be 8, 16, 32, 64 bytes. HS can go up to 512.

/*************************************************************************
 *  USB application interface
 **************************************************************************/

/** setup packet definitions */
struct usb_setup_packet {
    uint8_t bmRequestType;  /**< characteristics of the specific request */
    uint8_t bRequest;       /**< specific request */
    uint16_t wValue;        /**< request specific parameter */
    uint16_t wIndex;        /**< request specific parameter */
    uint16_t wLength;       /**< length of data transferred in data phase */
} __packed;


ESP_STATIC_ASSERT(sizeof(struct usb_setup_packet) == 8, "USB setup packet struct size error");

/**
 * Callback function signature for the device
 */
typedef void (*usb_status_callback)(enum usb_dc_status_code status_code,
                                    uint8_t *param);

/**
 * Callback function signature for the USB Endpoint status
 */
typedef void (*usb_ep_callback)(uint8_t ep,
                                enum usb_dc_ep_cb_status_code cb_status);

/**
 * Function which handles Class specific requests corresponding to an
 * interface number specified in the device descriptor table
 */
typedef int (*usb_request_handler) (struct usb_setup_packet *detup,
                                    int32_t *transfer_len, uint8_t **payload_data);

/**
 * Function for interface runtime configuration
 */
typedef void (*usb_interface_config)(uint8_t bInterfaceNumber);

/*
 * USB Endpoint Configuration
 */
struct usb_ep_cfg_data {
    /**
     * Callback function for notification of data received and
     * available to application or transmit done, NULL if callback
     * not required by application code
     */
    usb_ep_callback ep_cb;
    /**
     * The number associated with the EP in the device configuration
     * structure
     *   IN  EP = 0x80 | \<endpoint number\>
     *   OUT EP = 0x00 | \<endpoint number\>
     */
    uint8_t ep_addr;
};

/**
 * USB Interface Configuration
 */
struct usb_interface_cfg_data {
    /** Handler for USB Class specific Control (EP 0) communications */
    usb_request_handler class_handler;
    /** Handler for USB Vendor specific commands */
    usb_request_handler vendor_handler;
    /**
     * The custom request handler gets a first chance at handling
     * the request before it is handed over to the 'chapter 9' request
     * handler
     */
    usb_request_handler custom_handler;
    /**
     * This data area, allocated by the application, is used to store
     * Class specific command data and must be large enough to store the
     * largest payload associated with the largest supported Class'
     * command set. This data area may be used for USB IN or OUT
     * communications
     */
    uint8_t *payload_data;
    /**
     * This data area, allocated by the application, is used to store
     * Vendor specific payload
     */
    uint8_t *vendor_data;
};

/*
 * @brief USB device configuration
 *
 * The Application instantiates this with given parameters added
 * using the "usb_set_config" function. Once this function is called
 * changes to this structure will result in undefined behaviour. This structure
 * may only be updated after calls to usb_deconfig
 */
struct usb_cfg_data {
    /**
     * USB device description, see
     * http://www.beyondlogic.org/usbnutshell/usb5.shtml#DeviceDescriptors
     */
    const uint8_t *usb_device_description;
    /** Pointer to interface descriptor */
    const void *interface_descriptor;
    /** Function for interface runtime configuration */
    usb_interface_config interface_config;
    /** Callback to be notified on USB connection status change */
    usb_status_callback cb_usb_status;
    /** USB interface (Class) handler and storage space */
    struct usb_interface_cfg_data interface;
    /** Number of individual endpoints in the device configuration */
    uint8_t num_endpoints;
    /**
     * Pointer to an array of endpoint structs of length equal to the
     * number of EP associated with the device description,
     * not including control endpoints
     */
    struct usb_ep_cfg_data *endpoint;
};

/*
 * @brief configure USB controller
 *
 * Function to configure USB controller.
 * Configuration parameters must be valid or an error is returned
 *
 * @param[in] config Pointer to configuration structure
 *
 * @return 0 on success, negative errno code on fail
 */
int usb_set_config(struct usb_cfg_data *config);

/*
 * @brief return the USB device to it's initial state
 *
 * @return 0 on success, negative errno code on fail
 */
int usb_deconfig(void);

/*
 * @brief enable USB for host/device connection
 *
 * Function to enable USB for host/device connection.
 * Upon success, the USB module is no longer clock gated in hardware,
 * it is now capable of transmitting and receiving on the USB bus and
 * of generating interrupts.
 *
 * @return 0 on success, negative errno code on fail.
 */
int usb_enable(struct usb_cfg_data *config);

/*
 * @brief disable the USB device.
 *
 * Function to disable the USB device.
 * Upon success, the specified USB interface is clock gated in hardware,
 * it is no longer capable of generating interrupts.
 *
 * @return 0 on success, negative errno code on fail
 */
int usb_disable(void);

/*
 * @brief Check if a write to an in ep would block until there is enough space
 * in the fifo
 *
 * @param[in]  ep        Endpoint address corresponding to the one listed in the
 *                       device configuration table
 *
 * @return 0 if free to write, 1 if a write would block, negative errno code on fail
 */
int usb_write_would_block(uint8_t ep);

/*
 * @brief write data to the specified endpoint
 *
 * Function to write data to the specified endpoint. The supplied
 * usb_ep_callback will be called when transmission is done.
 *
 * @param[in]  ep        Endpoint address corresponding to the one listed in the
 *                       device configuration table
 * @param[in]  data      Pointer to data to write
 * @param[in]  data_len  Length of data requested to write. This may be zero for
 *                       a zero length status packet.
 * @param[out] bytes_ret Bytes written to the EP FIFO. This value may be NULL if
 *                       the application expects all bytes to be written
 *
 * @return 0 on success, negative errno code on fail
 */
int usb_write(uint8_t ep, const uint8_t *data, uint32_t data_len,
              uint32_t *bytes_ret);

/*
 * @brief read data from the specified endpoint
 *
 * This function is called by the Endpoint handler function, after an
 * OUT interrupt has been received for that EP. The application must
 * only call this function through the supplied usb_ep_callback function.
 *
 * @param[in]  ep           Endpoint address corresponding to the one listed in
 *                          the device configuration table
 * @param[in]  data         Pointer to data buffer to write to
 * @param[in]  max_data_len Max length of data to read
 * @param[out] ret_bytes    Number of bytes read. If data is NULL and
 *                          max_data_len is 0 the number of bytes available
 *                          for read is returned.
 *
 * @return  0 on success, negative errno code on fail
 */
int usb_read(uint8_t ep, uint8_t *data, uint32_t max_data_len,
             uint32_t *ret_bytes);

/*
 * @brief set STALL condition on the specified endpoint
 *
 * This function is called by USB device class handler code to set stall
 * conditionin on endpoint.
 *
 * @param[in]  ep           Endpoint address corresponding to the one listed in
 *                          the device configuration table
 *
 * @return  0 on success, negative errno code on fail
 */
int usb_ep_set_stall(uint8_t ep);


/*
 * @brief clears STALL condition on the specified endpoint
 *
 * This function is called by USB device class handler code to clear stall
 * conditionin on endpoint.
 *
 * @param[in]  ep           Endpoint address corresponding to the one listed in
 *                          the device configuration table
 *
 * @return  0 on success, negative errno code on fail
 */
int usb_ep_clear_stall(uint8_t ep);

/**
 * @brief read data from the specified endpoint
 *
 * This is similar to usb_ep_read, the difference being that, it doesn't
 * clear the endpoint NAKs so that the consumer is not bogged down by further
 * upcalls till he is done with the processing of the data. The caller should
 * reactivate ep by invoking usb_ep_read_continue() do so.
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
int usb_ep_read_wait(uint8_t ep, uint8_t *data, uint32_t max_data_len,
                     uint32_t *read_bytes);


/**
 * @brief Continue reading data from the endpoint
 *
 * Clear the endpoint NAK and enable the endpoint to accept more data
 * from the host. Usually called after usb_ep_read_wait() when the consumer
 * is fine to accept more data. Thus these calls together acts as flow control
 * mechanism.
 *
 * @param[in]  ep           Endpoint address corresponding to the one
 *                          listed in the device configuration table
 *
 * @return 0 on success, negative errno code on fail.
 */
int usb_ep_read_continue(uint8_t ep);

/**
 * Callback function signature for transfer completion.
 */
typedef void (*usb_transfer_callback)(uint8_t ep, int tsize, void *priv);

/* USB transfer flags */
#define USB_TRANS_READ       BIT(0)   /** Read transfer flag */
#define USB_TRANS_WRITE      BIT(1)   /** Write transfer flag */
#define USB_TRANS_NO_ZLP     BIT(2)   /** No zero-length packet flag */

/**
 * @brief Transfer management endpoint callback
 *
 * If a USB class driver wants to use high-level transfer functions, driver
 * needs to register this callback as usb endpoint callback.
 */
void usb_transfer_ep_callback(uint8_t ep, enum usb_dc_ep_cb_status_code);

/**
 * @brief Start a transfer
 *
 * Start a usb transfer to/from the data buffer. This function is asynchronous
 * and can be executed in IRQ context. The provided callback will be called
 * on transfer completion (or error) in thread context.
 *
 * @param[in]  ep           Endpoint address corresponding to the one
 *                          listed in the device configuration table
 * @param[in]  data         Pointer to data buffer to write-to/read-from
 * @param[in]  dlen         Size of data buffer
 * @param[in]  flags        Transfer flags (USB_TRANS_READ, USB_TRANS_WRITE...)
 * @param[in]  cb           Function called on transfer completion/failure
 * @param[in]  priv         Data passed back to the transfer completion callback
 *
 * @return 0 on success, negative errno code on fail.
 */
int usb_transfer(uint8_t ep, uint8_t *data, size_t dlen, unsigned int flags,
                 usb_transfer_callback cb, void *priv);

/**
 * @brief Start a transfer and block-wait for completion
 *
 * Synchronous version of usb_transfer, wait for transfer completion before
 * returning.
 *
 * @param[in]  ep           Endpoint address corresponding to the one
 *                          listed in the device configuration table
 * @param[in]  data         Pointer to data buffer to write-to/read-from
 * @param[in]  dlen         Size of data buffer
 * @param[in]  flags        Transfer flags

 *
 * @return number of bytes transferred on success, negative errno code on fail.
 */
int usb_transfer_sync(uint8_t ep, uint8_t *data, size_t dlen, unsigned int flags);

/**
 * @brief Cancel any ongoing transfer on the specified endpoint
 *
 * @param[in]  ep           Endpoint address corresponding to the one
 *                          listed in the device configuration table
 *
 * @return 0 on success, negative errno code on fail.
 */
void usb_cancel_transfer(uint8_t ep);

/**
 * @brief Provide IDF with an interface to clear the static variable usb_dev
 *
 *
 */
void usb_dev_deinit(void);

void usb_dev_resume(int configuration);
int usb_dev_get_configuration(void);


#ifdef __cplusplus
}
#endif
