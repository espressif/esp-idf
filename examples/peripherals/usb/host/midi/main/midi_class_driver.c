/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "usb/usb_host.h"

#define ACTION_OPEN_DEV             0x01
#define ACTION_GET_DEV_INFO         0x02
#define ACTION_GET_DEV_DESC         0x04
#define ACTION_GET_CONFIG_DESC      0x08
#define ACTION_GET_STR_DESC         0x10
#define ACTION_CLAIM_INTERFACE		0x20
#define ACTION_START_READING_DATA	0x40
#define ACTION_CLOSE_DEV            0x80
#define ACTION_EXIT                 0xA0

#define USB_CLIENT_NUM_EVENT_MSG 	5
#define MIDI_MESSAGE_LENGTH 		4

typedef struct {
	uint8_t interface_nmbr;
	uint8_t alternate_setting;
	uint8_t endpoint_address;
	uint8_t max_packet_size;
} interface_config_t;

typedef struct {
    usb_host_client_handle_t client_hdl;
    uint8_t dev_addr;
    usb_device_handle_t dev_hdl;
    uint32_t actions;
    interface_config_t interface_conf;
} class_driver_t;

static const char *DRIVER_TAG = "MIDI DRIVER";

static void midi_usb_host_callback(usb_transfer_t *transfer) {
	int size = (int)transfer->actual_num_bytes;
	//one message contains 4 bytes of data
	int num_messages = size/MIDI_MESSAGE_LENGTH;

	int offset = 0;
	//print messages
	if(num_messages) {

		ESP_LOGI(DRIVER_TAG, "Received new data: \n");
		//print each message separately
		for(int i = 0; i < num_messages; i++) {
			for (int j = 0; j < MIDI_MESSAGE_LENGTH; j++) {
				printf("%d ", transfer->data_buffer[j+offset]);
			}
			printf("\n");
			offset += MIDI_MESSAGE_LENGTH;
		}
	}

	//submit new usb-message to continuously receive data
	ESP_ERROR_CHECK(usb_host_transfer_submit(transfer));
}

static void get_midi_interface_settings(const usb_config_desc_t *usb_conf, interface_config_t *interface_conf) {
	assert(usb_conf != NULL);
	assert(interface_conf != NULL);

	ESP_LOGI(DRIVER_TAG, "Getting interface config");

	int offset = 0;
	    uint16_t wTotalLength = usb_conf->wTotalLength;
	    const usb_standard_desc_t *next_desc = (const usb_standard_desc_t *)usb_conf;

	    do {
	    	if(next_desc->bDescriptorType == USB_B_DESCRIPTOR_TYPE_INTERFACE) {
	    		usb_intf_desc_t *interface_desc = (usb_intf_desc_t *)next_desc;

	    		//check if there are >0 endpoints
	    		if(interface_desc->bNumEndpoints > 0) {
	    			//use interface
	    			interface_conf->interface_nmbr = interface_desc->bInterfaceNumber;
	    			interface_conf->alternate_setting = interface_desc->bAlternateSetting;

	    			printf("Interface Number: %d, Alternate Setting: %d \n", interface_conf->interface_nmbr, interface_conf->alternate_setting);

	    		}
	    	}
	    	if(next_desc->bDescriptorType == USB_B_DESCRIPTOR_TYPE_ENDPOINT) {
	    		usb_ep_desc_t *ep_desc = (usb_ep_desc_t *)next_desc;
	    		if(USB_EP_DESC_GET_EP_DIR(ep_desc)) {
	    			//endpoint is IN
	    			interface_conf->endpoint_address = ep_desc->bEndpointAddress;
	    			interface_conf->max_packet_size = ep_desc->wMaxPacketSize;
	    			printf("endpoint address: %d , mps: %d\n", interface_conf->endpoint_address, interface_conf->max_packet_size);
	    		}
	    	}

	        next_desc = usb_parse_next_descriptor(next_desc, wTotalLength, &offset);

	    } while (next_desc != NULL);
}


static void client_event_cb(const usb_host_client_event_msg_t *event_msg, void *arg)
{
    class_driver_t *driver_obj = (class_driver_t *)arg;
    switch (event_msg->event) {
    case USB_HOST_CLIENT_EVENT_NEW_DEV:
        if (driver_obj->dev_addr == 0) {
            driver_obj->dev_addr = event_msg->new_dev.address;
            //Open the device next
            driver_obj->actions |= ACTION_OPEN_DEV;
        }
        break;
    case USB_HOST_CLIENT_EVENT_DEV_GONE:
        if (driver_obj->dev_hdl != NULL) {
            //Cancel any other actions and close the device next
            driver_obj->actions = ACTION_CLOSE_DEV;
        }
        break;
    default:
        //Should never occur
        abort();
    }
}

static void action_open_dev(class_driver_t *driver_obj)
{
    assert(driver_obj->dev_addr != 0);
    ESP_LOGI(DRIVER_TAG, "Opening device at address %d", driver_obj->dev_addr);
    ESP_ERROR_CHECK(usb_host_device_open(driver_obj->client_hdl, driver_obj->dev_addr, &driver_obj->dev_hdl));
    //Get the device's information next
    driver_obj->actions &= ~ACTION_OPEN_DEV;
    driver_obj->actions |= ACTION_GET_DEV_INFO;
}

static void action_get_info(class_driver_t *driver_obj)
{
    assert(driver_obj->dev_hdl != NULL);
    ESP_LOGI(DRIVER_TAG, "Getting device information");
    usb_device_info_t dev_info;
    ESP_ERROR_CHECK(usb_host_device_info(driver_obj->dev_hdl, &dev_info));
    ESP_LOGI(DRIVER_TAG, "\t%s speed", (dev_info.speed == USB_SPEED_LOW) ? "Low" : "Full");
    ESP_LOGI(DRIVER_TAG, "\tbConfigurationValue %d", dev_info.bConfigurationValue);
    //Todo: Print string descriptors

    //Get the device descriptor next
    driver_obj->actions &= ~ACTION_GET_DEV_INFO;
    driver_obj->actions |= ACTION_GET_DEV_DESC;
}

static void action_get_dev_desc(class_driver_t *driver_obj)
{
    assert(driver_obj->dev_hdl != NULL);
    ESP_LOGI(DRIVER_TAG, "Getting device descriptor");
    const usb_device_desc_t *dev_desc;
    ESP_ERROR_CHECK(usb_host_get_device_descriptor(driver_obj->dev_hdl, &dev_desc));
    usb_print_device_descriptor(dev_desc);
    //Get the device's config descriptor next
    driver_obj->actions &= ~ACTION_GET_DEV_DESC;
    driver_obj->actions |= ACTION_GET_CONFIG_DESC;
}

static void action_get_config_desc(class_driver_t *driver_obj)
{
    assert(driver_obj->dev_hdl != NULL);
    ESP_LOGI(DRIVER_TAG, "Getting config descriptor");
    const usb_config_desc_t *config_desc;
    ESP_ERROR_CHECK(usb_host_get_active_config_descriptor(driver_obj->dev_hdl, &config_desc));
    usb_print_config_descriptor(config_desc, NULL);

    //save interface number & alternative setting for later use
    interface_config_t interface_config = {0};
    ESP_ERROR_CHECK(usb_host_get_active_config_descriptor(driver_obj->dev_hdl, &config_desc));
    get_midi_interface_settings(config_desc, &interface_config);

    driver_obj->interface_conf = interface_config;

    //Get the device's string descriptors next
    driver_obj->actions &= ~ACTION_GET_CONFIG_DESC;
    driver_obj->actions |= ACTION_GET_STR_DESC;
}

static void action_get_str_desc(class_driver_t *driver_obj)
{
    assert(driver_obj->dev_hdl != NULL);
    usb_device_info_t dev_info;
    ESP_ERROR_CHECK(usb_host_device_info(driver_obj->dev_hdl, &dev_info));
    if (dev_info.str_desc_manufacturer) {
        ESP_LOGI(DRIVER_TAG, "Getting Manufacturer string descriptor");
        usb_print_string_descriptor(dev_info.str_desc_manufacturer);
    }
    if (dev_info.str_desc_product) {
        ESP_LOGI(DRIVER_TAG, "Getting Product string descriptor");
        usb_print_string_descriptor(dev_info.str_desc_product);
    }
    if (dev_info.str_desc_serial_num) {
        ESP_LOGI(DRIVER_TAG, "Getting Serial Number string descriptor");
        usb_print_string_descriptor(dev_info.str_desc_serial_num);
    }
    //Claim interface next
    driver_obj->actions &= ~ACTION_GET_STR_DESC;
    driver_obj->actions |= ACTION_CLAIM_INTERFACE;
}

static void action_claim_interface(class_driver_t *driver_obj) {
	assert(driver_obj->dev_hdl != NULL);
	ESP_LOGI(DRIVER_TAG, "Claiming Interface");
    ESP_ERROR_CHECK(usb_host_interface_claim(
    		driver_obj->client_hdl,
			driver_obj->dev_hdl,
			driver_obj->interface_conf.interface_nmbr,
			driver_obj->interface_conf.alternate_setting));

    driver_obj->actions &= ~ACTION_CLAIM_INTERFACE;
    driver_obj->actions |= ACTION_START_READING_DATA;
}

static void action_start_reading_data(class_driver_t *driver_obj) {
	assert(driver_obj->dev_hdl != NULL);
	ESP_LOGI(DRIVER_TAG, "Configuring usb-transfer object");

	//configure usb-transfer object
    usb_transfer_t *transfer_obj;

    usb_host_transfer_alloc(1024, 0, &transfer_obj);

    transfer_obj->num_bytes = driver_obj->interface_conf.max_packet_size;
	transfer_obj->callback = midi_usb_host_callback;
	transfer_obj->bEndpointAddress = driver_obj->interface_conf.endpoint_address;
	transfer_obj->device_handle = driver_obj->dev_hdl;
	printf("set transfer parameters\n");
    ESP_ERROR_CHECK(usb_host_transfer_submit(transfer_obj));

    //Nothing to do until the device disconnects
    driver_obj->actions &= ~ACTION_START_READING_DATA;

}

static void aciton_close_dev(class_driver_t *driver_obj)
{
	ESP_LOGI(DRIVER_TAG, "Releasing interface");
	ESP_ERROR_CHECK(usb_host_interface_release(
			driver_obj->client_hdl,
			driver_obj->dev_hdl,
			driver_obj->interface_conf.interface_nmbr));
    ESP_LOGI(DRIVER_TAG, "Closing device");
	ESP_ERROR_CHECK(usb_host_device_close(driver_obj->client_hdl, driver_obj->dev_hdl));
    driver_obj->dev_hdl = NULL;
    driver_obj->dev_addr = 0;
    //We need to exit the event handler loop
    driver_obj->actions &= ~ACTION_CLOSE_DEV;
    driver_obj->actions |= ACTION_EXIT;
}

void class_driver_task(void *arg)
{
    SemaphoreHandle_t signaling_sem = (SemaphoreHandle_t)arg;
    class_driver_t driver_obj = {0};

    //Wait until daemon task has installed USB Host Library
    xSemaphoreTake(signaling_sem, portMAX_DELAY);

    ESP_LOGI(DRIVER_TAG, "Registering Client");
    usb_host_client_config_t client_config = {
        .is_synchronous = false,    //Synchronous clients currently not supported. Set this to false
        .max_num_event_msg = USB_CLIENT_NUM_EVENT_MSG,
        .async = {
            .client_event_callback = client_event_cb,
            .callback_arg = (void *) &driver_obj,
        },
    };
    ESP_ERROR_CHECK(usb_host_client_register(&client_config, &driver_obj.client_hdl));

    //classic state machine
    while (1) {
        if (driver_obj.actions == 0) {
            usb_host_client_handle_events(driver_obj.client_hdl, portMAX_DELAY);
        } else {
            if (driver_obj.actions & ACTION_OPEN_DEV) {
                action_open_dev(&driver_obj);
            }
            if (driver_obj.actions & ACTION_GET_DEV_INFO) {
                action_get_info(&driver_obj);
            }
            if (driver_obj.actions & ACTION_GET_DEV_DESC) {
                action_get_dev_desc(&driver_obj);
            }
            if (driver_obj.actions & ACTION_GET_CONFIG_DESC) {
                action_get_config_desc(&driver_obj);
            }
            if (driver_obj.actions & ACTION_GET_STR_DESC) {
                action_get_str_desc(&driver_obj);
            }
            if(driver_obj.actions & ACTION_CLAIM_INTERFACE) {
            	action_claim_interface(&driver_obj);
            }
            if(driver_obj.actions & ACTION_START_READING_DATA) {
            	action_start_reading_data(&driver_obj);
            }
            if (driver_obj.actions & ACTION_CLOSE_DEV) {
                aciton_close_dev(&driver_obj);
            }
            if (driver_obj.actions & ACTION_EXIT) {
                driver_obj.actions = 0;
            }
        }
    }
}

