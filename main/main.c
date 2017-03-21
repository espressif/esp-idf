/*
 * ESP32 "Factory" WLAN Config + Factory Setup app
 *
 * Copyright (C) 2017 Andy Green <andy@warmcat.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation:
 *  version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA  02110-1301  USA
 */
#include <libwebsockets.h>
#include <nvs_flash.h>

void (*lws_cb_scan_done)(void *);
void *lws_cb_scan_done_arg;

/* protocol for scan updates over ws and saving wlan setup */
#include "protocol_esp32_lws_scan.c"
/* protocol for OTA update using POST / https / browser upload */
#include "protocol_esp32_lws_ota.c"

static TimerHandle_t flash_timer, id_timer;
static int flashes, id_flashes;

static const struct lws_protocols protocols_ap[] = {
	{
		"http-only",
		lws_callback_http_dummy,
		0,	/* per_session_data_size */
		0, 0, NULL, 900
	},
	LWS_PLUGIN_PROTOCOL_ESPLWS_SCAN,
	LWS_PLUGIN_PROTOCOL_ESPLWS_OTA,

	{ NULL, NULL, 0, 0, 0, NULL, 0 } /* terminator */
};

static const struct lws_protocol_vhost_options pvo_headers = {
	NULL,
	NULL,
	"Keep-Alive:",
	"timeout=15, max=20",
};

static const struct lws_protocol_vhost_options ap_pvo2 = {
	NULL,
	NULL,
	"esplws-ota",
	""
};

static const struct lws_protocol_vhost_options ap_pvo = {
	&ap_pvo2,
	NULL,
	"esplws-scan",
	""
};

static const struct lws_http_mount mount_ota_post = {
	.mountpoint		= "/otaform",
	.origin			= "esplws-ota",
	.origin_protocol	= LWSMPRO_CALLBACK,
	.mountpoint_len		= 8,
};

static const struct lws_http_mount mount_ap_post = {
	.mount_next		= &mount_ota_post,
	.mountpoint		= "/factory",
	.origin			= "esplws-scan",
	.origin_protocol	= LWSMPRO_CALLBACK,
	.mountpoint_len		= 8,
};

static struct lws_http_mount mount_ap = {
	.mount_next		= &mount_ap_post,
	.mountpoint		= "/",
        .origin			= "/ap",
        .def			= "index.html",
        .origin_protocol	= LWSMPRO_FILE,
        .mountpoint_len		= 1,
	.cache_max_age		= 300,
	.cache_reusable		= 1,
};

esp_err_t event_handler(void *ctx, system_event_t *event)
{
	switch(event->event_id) {
	case SYSTEM_EVENT_SCAN_DONE:
		if (lws_cb_scan_done)
			lws_cb_scan_done(lws_cb_scan_done_arg);
		break;

	default:
		return lws_esp32_event_passthru(ctx, event);
	}

	return ESP_OK;
}

#define GPIO_ID 23

/* we flash the LED slowly to show we are in setup / factory mode */

static void flash_timer_cb(TimerHandle_t t)
{
	flashes++;

	if (flashes & 1)
		gpio_output_set(0, 1 << GPIO_ID, 1 << GPIO_ID, 0);
	else
		gpio_output_set(1 << GPIO_ID, 0, 1 << GPIO_ID, 0);
}

static void id_timer_cb(TimerHandle_t t)
{
	id_flashes++;

	if (id_flashes == 101) {
		xTimerStop(id_timer, 0);
		return;
	}

	if (id_flashes & 1)
		gpio_output_set(0, 1 << GPIO_ID, 1 << GPIO_ID, 0);
	else
		gpio_output_set(1 << GPIO_ID, 0, 1 << GPIO_ID, 0);
}



/*
 * This is called when the user asks to "Identify physical device"
 * he is configuring, by pressing the Identify button on the AP
 * setup page for the device.
 *
 * It should do something device-specific that
 * makes it easy to identify which physical device is being
 * addressed, like flash an LED on the device on a timer for a
 * few seconds.
 */
void
lws_esp32_identify_physical_device(void)
{
	lwsl_notice("%s\n", __func__);

	gpio_pad_select_gpio(GPIO_ID);
	gpio_pad_unhold(GPIO_ID);
	gpio_output_set(0, 1 << GPIO_ID, 1 << GPIO_ID, 0);

	xTimerStart(id_timer, 0);
	id_flashes = 0;
}

void app_main(void)
{
	static struct lws_context_creation_info info;
	struct lws_context *context;

	flash_timer = xTimerCreate("x", pdMS_TO_TICKS(500), 1, NULL,
                          (TimerCallbackFunction_t)flash_timer_cb);
	id_timer = xTimerCreate("x", pdMS_TO_TICKS(50), 1, NULL,
                          (TimerCallbackFunction_t)id_timer_cb);
	xTimerStart(flash_timer, 0);

	lws_esp32_set_creation_defaults(&info);

	info.vhost_name = "ap";
	info.protocols = protocols_ap;
	info.mounts = &mount_ap;
	info.pvo = &ap_pvo;
	info.headers = &pvo_headers;

	nvs_flash_init();
	lws_esp32_wlan_config();

	ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL));

	lws_esp32_wlan_start_ap();
	context = lws_esp32_init(&info);

	if (info.port == 80) {
		lwsl_notice("setting mount default to factory\n");
		mount_ap.def = "factory.html";
	}

	while (!lws_service(context, 50))
		vTaskDelay(1);
}
