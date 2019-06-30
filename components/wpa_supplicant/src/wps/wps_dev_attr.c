/*
 * Wi-Fi Protected Setup - device attributes
 * Copyright (c) 2008, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */
#include "utils/includes.h"
#include "utils/common.h"

#include "wps/wps_i.h"
#include "wps/wps_dev_attr.h"

int wps_build_manufacturer(struct wps_device_data *dev, struct wpabuf *msg)
{
	size_t len;
	wpa_printf(MSG_DEBUG,  "WPS:  * Manufacturer");
	wpabuf_put_be16(msg, ATTR_MANUFACTURER);
	len = dev->manufacturer ? os_strlen(dev->manufacturer) : 0;
#ifndef CONFIG_WPS_STRICT
	if (len == 0) {
		/*
		 * Some deployed WPS implementations fail to parse zero-length
		 * attributes. As a workaround, send a space character if the
		 * device attribute string is empty.
		 */
		wpabuf_put_be16(msg, 1);
		wpabuf_put_u8(msg, ' ');
		return 0;
	}
#endif /* CONFIG_WPS_STRICT */
	wpabuf_put_be16(msg, len);
	wpabuf_put_data(msg, dev->manufacturer, len);
	return 0;
}


int wps_build_model_name(struct wps_device_data *dev, struct wpabuf *msg)
{
	size_t len;
	wpa_printf(MSG_DEBUG,  "WPS:  * Model Name");
	wpabuf_put_be16(msg, ATTR_MODEL_NAME);
	len = dev->model_name ? os_strlen(dev->model_name) : 0;
#ifndef CONFIG_WPS_STRICT
	if (len == 0) {
		/*
		 * Some deployed WPS implementations fail to parse zero-length
		 * attributes. As a workaround, send a space character if the
		 * device attribute string is empty.
		 */
		wpabuf_put_be16(msg, 1);
		wpabuf_put_u8(msg, ' ');
		return 0;
	}
#endif /* CONFIG_WPS_STRICT */
	wpabuf_put_be16(msg, len);
	wpabuf_put_data(msg, dev->model_name, len);
	return 0;
}


int wps_build_model_number(struct wps_device_data *dev, struct wpabuf *msg)
{
	size_t len;
	wpa_printf(MSG_DEBUG,  "WPS:  * Model Number");
	wpabuf_put_be16(msg, ATTR_MODEL_NUMBER);
	len = dev->model_number ? os_strlen(dev->model_number) : 0;
#ifndef CONFIG_WPS_STRICT
	if (len == 0) {
		/*
		 * Some deployed WPS implementations fail to parse zero-length
		 * attributes. As a workaround, send a space character if the
		 * device attribute string is empty.
		 */
		wpabuf_put_be16(msg, 1);
		wpabuf_put_u8(msg, ' ');
		return 0;
	}
#endif /* CONFIG_WPS_STRICT */
	wpabuf_put_be16(msg, len);
	wpabuf_put_data(msg, dev->model_number, len);
	return 0;
}


static int wps_build_serial_number(struct wps_device_data *dev,
				   struct wpabuf *msg)
{
	size_t len;
	wpa_printf(MSG_DEBUG,  "WPS:  * Serial Number");
	wpabuf_put_be16(msg, ATTR_SERIAL_NUMBER);
	len = dev->serial_number ? os_strlen(dev->serial_number) : 0;
#ifndef CONFIG_WPS_STRICT
	if (len == 0) {
		/*
		 * Some deployed WPS implementations fail to parse zero-length
		 * attributes. As a workaround, send a space character if the
		 * device attribute string is empty.
		 */
		wpabuf_put_be16(msg, 1);
		wpabuf_put_u8(msg, ' ');
		return 0;
	}
#endif /* CONFIG_WPS_STRICT */
	wpabuf_put_be16(msg, len);
	wpabuf_put_data(msg, dev->serial_number, len);
	return 0;
}


int wps_build_primary_dev_type(struct wps_device_data *dev, struct wpabuf *msg)
{
	wpa_printf(MSG_DEBUG,  "WPS:  * Primary Device Type");
	wpabuf_put_be16(msg, ATTR_PRIMARY_DEV_TYPE);
	wpabuf_put_be16(msg, WPS_DEV_TYPE_LEN);
	wpabuf_put_data(msg, dev->pri_dev_type, WPS_DEV_TYPE_LEN);
	return 0;
}


int wps_build_secondary_dev_type(struct wps_device_data *dev,
				  struct wpabuf *msg)
{
	if (!dev->num_sec_dev_types)
		return 0;

	wpa_printf(MSG_DEBUG,  "WPS:  * Secondary Device Type");
	wpabuf_put_be16(msg, ATTR_SECONDARY_DEV_TYPE_LIST);
	wpabuf_put_be16(msg, WPS_DEV_TYPE_LEN * dev->num_sec_dev_types);
	wpabuf_put_data(msg, dev->sec_dev_type,
			WPS_DEV_TYPE_LEN * dev->num_sec_dev_types);

	return 0;
}


int wps_build_req_dev_type(struct wps_device_data *dev, struct wpabuf *msg,
			   unsigned int num_req_dev_types,
			   const u8 *req_dev_types)
{
	unsigned int i;

	for (i = 0; i < num_req_dev_types; i++) {
		wpa_hexdump(MSG_DEBUG, "WPS: * Requested Device Type",
			    req_dev_types + i * WPS_DEV_TYPE_LEN,
			    WPS_DEV_TYPE_LEN);
		wpabuf_put_be16(msg, ATTR_REQUESTED_DEV_TYPE);
		wpabuf_put_be16(msg, WPS_DEV_TYPE_LEN);
		wpabuf_put_data(msg, req_dev_types + i * WPS_DEV_TYPE_LEN,
				WPS_DEV_TYPE_LEN);
	}

	return 0;
}


int wps_build_dev_name(struct wps_device_data *dev, struct wpabuf *msg)
{
	size_t len;
	wpa_printf(MSG_DEBUG,  "WPS:  * Device Name");
	wpabuf_put_be16(msg, ATTR_DEV_NAME);
	len = dev->device_name ? os_strlen(dev->device_name) : 0;
#ifndef CONFIG_WPS_STRICT
	if (len == 0) {
		/*
		 * Some deployed WPS implementations fail to parse zero-length
		 * attributes. As a workaround, send a space character if the
		 * device attribute string is empty.
		 */
		wpabuf_put_be16(msg, 1);
		wpabuf_put_u8(msg, ' ');
		return 0;
	}
#endif /* CONFIG_WPS_STRICT */
	wpabuf_put_be16(msg, len);
	wpabuf_put_data(msg, dev->device_name, len);
	return 0;
}


int wps_build_device_attrs(struct wps_device_data *dev, struct wpabuf *msg)
{
	if (wps_build_manufacturer(dev, msg) ||
	    wps_build_model_name(dev, msg) ||
	    wps_build_model_number(dev, msg) ||
	    wps_build_serial_number(dev, msg) ||
	    wps_build_primary_dev_type(dev, msg) ||
	    wps_build_dev_name(dev, msg))
		return -1;
	return 0;
}


int wps_build_os_version(struct wps_device_data *dev, struct wpabuf *msg)
{
	wpa_printf(MSG_DEBUG,  "WPS:  * OS Version");
	wpabuf_put_be16(msg, ATTR_OS_VERSION);
	wpabuf_put_be16(msg, 4);
	wpabuf_put_be32(msg, 0x80000000 | dev->os_version);
	return 0;
}


int wps_build_vendor_ext_m1(struct wps_device_data *dev, struct wpabuf *msg)
{
	if (dev->vendor_ext_m1 != NULL) {
		wpa_hexdump(MSG_DEBUG, "WPS:  * Vendor Extension M1",
			    wpabuf_head_u8(dev->vendor_ext_m1),
			    wpabuf_len(dev->vendor_ext_m1));
		wpabuf_put_be16(msg, ATTR_VENDOR_EXT);
		wpabuf_put_be16(msg, wpabuf_len(dev->vendor_ext_m1));
		wpabuf_put_buf(msg, dev->vendor_ext_m1);
	}
	return 0;
}


int wps_build_rf_bands(struct wps_device_data *dev, struct wpabuf *msg)
{
	wpa_printf(MSG_DEBUG,  "WPS:  * RF Bands (%x)", dev->rf_bands);
	wpabuf_put_be16(msg, ATTR_RF_BANDS);
	wpabuf_put_be16(msg, 1);
	wpabuf_put_u8(msg, dev->rf_bands);
	return 0;
}


int wps_build_vendor_ext(struct wps_device_data *dev, struct wpabuf *msg)
{
	int i;

	for (i = 0; i < MAX_WPS_VENDOR_EXTENSIONS; i++) {
		if (dev->vendor_ext[i] == NULL)
			continue;
		wpa_hexdump(MSG_DEBUG, "WPS:  * Vendor Extension",
			    wpabuf_head_u8(dev->vendor_ext[i]),
			    wpabuf_len(dev->vendor_ext[i]));
		wpabuf_put_be16(msg, ATTR_VENDOR_EXT);
		wpabuf_put_be16(msg, wpabuf_len(dev->vendor_ext[i]));
		wpabuf_put_buf(msg, dev->vendor_ext[i]);
	}

	return 0;
}


static int wps_process_manufacturer(struct wps_device_data *dev, const u8 *str,
				    size_t str_len)
{
	if (str == NULL) {
		wpa_printf(MSG_DEBUG,  "WPS: No Manufacturer received");
		return -1;
	}

	wpa_hexdump_ascii(MSG_DEBUG, "WPS: Manufacturer", str, str_len);

	os_free(dev->manufacturer);
	dev->manufacturer = (char *)os_malloc(str_len + 1);
	if (dev->manufacturer == NULL)
		return -1;
	os_memcpy(dev->manufacturer, str, str_len);
	dev->manufacturer[str_len] = '\0';

	return 0;
}


static int wps_process_model_name(struct wps_device_data *dev, const u8 *str,
				  size_t str_len)
{
	if (str == NULL) {
		wpa_printf(MSG_DEBUG,  "WPS: No Model Name received");
		return -1;
	}

	wpa_hexdump_ascii(MSG_DEBUG, "WPS: Model Name", str, str_len);

	os_free(dev->model_name);
	dev->model_name = (char *)os_malloc(str_len + 1);
	if (dev->model_name == NULL)
		return -1;
	os_memcpy(dev->model_name, str, str_len);
	dev->model_name[str_len] = '\0';

	return 0;
}


static int wps_process_model_number(struct wps_device_data *dev, const u8 *str,
				    size_t str_len)
{
	if (str == NULL) {
		wpa_printf(MSG_DEBUG,  "WPS: No Model Number received");
		return -1;
	}

	wpa_hexdump_ascii(MSG_DEBUG, "WPS: Model Number", str, str_len);

	os_free(dev->model_number);
	dev->model_number = (char *)os_malloc(str_len + 1);
	if (dev->model_number == NULL)
		return -1;
	os_memcpy(dev->model_number, str, str_len);
	dev->model_number[str_len] = '\0';

	return 0;
}


static int wps_process_serial_number(struct wps_device_data *dev,
				     const u8 *str, size_t str_len)
{
	if (str == NULL) {
		wpa_printf(MSG_DEBUG,  "WPS: No Serial Number received");
		return -1;
	}

	wpa_hexdump_ascii(MSG_DEBUG, "WPS: Serial Number", str, str_len);

	os_free(dev->serial_number);
	dev->serial_number = (char *)os_malloc(str_len + 1);
	if (dev->serial_number == NULL)
		return -1;
	os_memcpy(dev->serial_number, str, str_len);
	dev->serial_number[str_len] = '\0';

	return 0;
}


static int wps_process_dev_name(struct wps_device_data *dev, const u8 *str,
				size_t str_len)
{
	if (str == NULL) {
		wpa_printf(MSG_DEBUG,  "WPS: No Device Name received");
		return -1;
	}

	wpa_hexdump_ascii(MSG_DEBUG, "WPS: Device Name", str, str_len);

	os_free(dev->device_name);
	dev->device_name = (char *)os_malloc(str_len + 1);
	if (dev->device_name == NULL)
		return -1;
	os_memcpy(dev->device_name, str, str_len);
	dev->device_name[str_len] = '\0';

	return 0;
}


static int wps_process_primary_dev_type(struct wps_device_data *dev,
					const u8 *dev_type)
{
	if (dev_type == NULL) {
		wpa_printf(MSG_DEBUG,  "WPS: No Primary Device Type received");
		return -1;
	}

	os_memcpy(dev->pri_dev_type, dev_type, WPS_DEV_TYPE_LEN);

	return 0;
}


int wps_process_device_attrs(struct wps_device_data *dev,
			     struct wps_parse_attr *attr)
{
	if (wps_process_manufacturer(dev, attr->manufacturer,
				     attr->manufacturer_len) ||
	    wps_process_model_name(dev, attr->model_name,
				   attr->model_name_len) ||
	    wps_process_model_number(dev, attr->model_number,
				     attr->model_number_len) ||
	    wps_process_serial_number(dev, attr->serial_number,
				      attr->serial_number_len) ||
	    wps_process_primary_dev_type(dev, attr->primary_dev_type) ||
	    wps_process_dev_name(dev, attr->dev_name, attr->dev_name_len))
		return -1;
	return 0;
}


int wps_process_os_version(struct wps_device_data *dev, const u8 *ver)
{
	if (ver == NULL) {
		wpa_printf(MSG_DEBUG,  "WPS: No OS Version received");
		return -1;
	}

	dev->os_version = WPA_GET_BE32(ver);
	wpa_printf(MSG_DEBUG,  "WPS: OS Version %08x", dev->os_version);

	return 0;
}


int wps_process_rf_bands(struct wps_device_data *dev, const u8 *bands)
{
	if (bands == NULL) {
		wpa_printf(MSG_DEBUG,  "WPS: No RF Bands received");
		return -1;
	}

	dev->rf_bands = *bands;
	wpa_printf(MSG_DEBUG,  "WPS: Enrollee RF Bands 0x%x", dev->rf_bands);

	return 0;
}


void wps_device_data_dup(struct wps_device_data *dst,
			 const struct wps_device_data *src)
{
	if (src->device_name)
		dst->device_name = os_strdup(src->device_name);
	if (src->manufacturer)
		dst->manufacturer = os_strdup(src->manufacturer);
	if (src->model_name)
		dst->model_name = os_strdup(src->model_name);
	if (src->model_number)
		dst->model_number = os_strdup(src->model_number);
	if (src->serial_number)
		dst->serial_number = os_strdup(src->serial_number);
	os_memcpy(dst->pri_dev_type, src->pri_dev_type, WPS_DEV_TYPE_LEN);
	dst->os_version = src->os_version;
	dst->rf_bands = src->rf_bands;
}


void wps_device_data_free(struct wps_device_data *dev)
{
	os_free(dev->device_name);
	dev->device_name = NULL;
	os_free(dev->manufacturer);
	dev->manufacturer = NULL;
	os_free(dev->model_name);
	dev->model_name = NULL;
	os_free(dev->model_number);
	dev->model_number = NULL;
	os_free(dev->serial_number);
	dev->serial_number = NULL;
}
