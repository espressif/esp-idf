/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "usb/usb_host.h"
#include "bg96_usb.hpp"

#include "nmea.h"
#include "gpgll.h"
#include "gpgga.h"
#include "gprmc.h"
#include "gpgsa.h"
#include "gpvtg.h"
#include "gptxt.h"
#include "gpgsv.h"

#define EXAMPLE_USB_HOST_PRIORITY 20

static const char* TAG = "BG96";

static char fmt_buf[32];

/* ------------------------------- Callbacks -------------------------------- */

static void handle_rx(uint8_t *data, size_t data_len, void *user_arg)
{
    data[data_len] = '\0';
    ESP_LOGI("AT", "%s", data);
}

static void handle_gps(uint8_t* data, size_t data_len, void *user_arg)
{
    // handle nmea_data
    nmea_s *nmea_data = nmea_parse((char *)data, data_len, 0);
    if (nmea_data == NULL) {
        printf("Failed to parse the sentence!\n");
        printf("  Type: %.5s (%d)\n", data + 1, nmea_get_type((const char *)data));
    } else {
        if (nmea_data->errors != 0) {
            printf("WARN: The sentence struct contains parse errors!\n");
        }

        if (NMEA_GPGGA == nmea_data->type) {
            printf("GPGGA sentence\n");
            nmea_gpgga_s *gpgga = (nmea_gpgga_s *)nmea_data;
            printf("Number of satellites: %d\n", gpgga->n_satellites);
            printf("Altitude: %f %c\n", gpgga->altitude, gpgga->altitude_unit);
        }

        if (NMEA_GPGLL == nmea_data->type) {
            printf("GPGLL sentence\n");
            nmea_gpgll_s *pos = (nmea_gpgll_s *)nmea_data;
            printf("Longitude:\n");
            printf("  Degrees: %d\n", pos->longitude.degrees);
            printf("  Minutes: %f\n", pos->longitude.minutes);
            printf("  Cardinal: %c\n", (char)pos->longitude.cardinal);
            printf("Latitude:\n");
            printf("  Degrees: %d\n", pos->latitude.degrees);
            printf("  Minutes: %f\n", pos->latitude.minutes);
            printf("  Cardinal: %c\n", (char)pos->latitude.cardinal);
            strftime(fmt_buf, sizeof(fmt_buf), "%H:%M:%S", &pos->time);
            printf("Time: %s\n", fmt_buf);
        }

        if (NMEA_GPRMC == nmea_data->type) {
            printf("GPRMC sentence\n");
            nmea_gprmc_s *pos = (nmea_gprmc_s *)nmea_data;
            printf("Longitude:\n");
            printf("  Degrees: %d\n", pos->longitude.degrees);
            printf("  Minutes: %f\n", pos->longitude.minutes);
            printf("  Cardinal: %c\n", (char)pos->longitude.cardinal);
            printf("Latitude:\n");
            printf("  Degrees: %d\n", pos->latitude.degrees);
            printf("  Minutes: %f\n", pos->latitude.minutes);
            printf("  Cardinal: %c\n", (char)pos->latitude.cardinal);
            strftime(fmt_buf, sizeof(fmt_buf), "%d %b %T %Y", &pos->date_time);
            printf("Date & Time: %s\n", fmt_buf);
            printf("Speed, in Knots: %f\n", pos->gndspd_knots);
            printf("Track, in degrees: %f\n", pos->track_deg);
            printf("Magnetic Variation:\n");
            printf("  Degrees: %f\n", pos->magvar_deg);
            printf("  Cardinal: %c\n", (char)pos->magvar_cardinal);
            double adjusted_course = pos->track_deg;
            if (NMEA_CARDINAL_DIR_EAST == pos->magvar_cardinal) {
                adjusted_course -= pos->magvar_deg;
            } else if (NMEA_CARDINAL_DIR_WEST == pos->magvar_cardinal) {
                adjusted_course += pos->magvar_deg;
            } else {
                printf("Invalid Magnetic Variation Direction!\n");
            }

            printf("Adjusted Track (heading): %f\n", adjusted_course);
        }

        if (NMEA_GPGSA == nmea_data->type) {
            nmea_gpgsa_s *gpgsa = (nmea_gpgsa_s *)nmea_data;

            printf("GPGSA Sentence:\n");
            printf("  Mode: %c\n", gpgsa->mode);
            printf("  Fix:  %d\n", gpgsa->fixtype);
            printf("  PDOP: %.2lf\n", gpgsa->pdop);
            printf("  HDOP: %.2lf\n", gpgsa->hdop);
            printf("  VDOP: %.2lf\n", gpgsa->vdop);
        }

        if (NMEA_GPGSV == nmea_data->type) {
            nmea_gpgsv_s *gpgsv = (nmea_gpgsv_s *)nmea_data;

            printf("GPGSV Sentence:\n");
            printf("  Num: %d\n", gpgsv->sentences);
            printf("  ID:  %d\n", gpgsv->sentence_number);
            printf("  SV:  %d\n", gpgsv->satellites);
            printf("  #1:  %d %d %d %d\n", gpgsv->sat[0].prn, gpgsv->sat[0].elevation, gpgsv->sat[0].azimuth,
                   gpgsv->sat[0].snr);
            printf("  #2:  %d %d %d %d\n", gpgsv->sat[1].prn, gpgsv->sat[1].elevation, gpgsv->sat[1].azimuth,
                   gpgsv->sat[1].snr);
            printf("  #3:  %d %d %d %d\n", gpgsv->sat[2].prn, gpgsv->sat[2].elevation, gpgsv->sat[2].azimuth,
                   gpgsv->sat[2].snr);
            printf("  #4:  %d %d %d %d\n", gpgsv->sat[3].prn, gpgsv->sat[3].elevation, gpgsv->sat[3].azimuth,
                   gpgsv->sat[3].snr);
        }

        if (NMEA_GPTXT == nmea_data->type) {
            nmea_gptxt_s *gptxt = (nmea_gptxt_s *)nmea_data;

            printf("GPTXT Sentence:\n");
            printf("  ID: %d %d %d\n", gptxt->id_00, gptxt->id_01, gptxt->id_02);
            printf("  %s\n", gptxt->text);
        }

        if (NMEA_GPVTG == nmea_data->type) {
            nmea_gpvtg_s *gpvtg = (nmea_gpvtg_s *)nmea_data;

            printf("GPVTG Sentence:\n");
            printf("  Track [deg]:   %.2lf\n", gpvtg->track_deg);
            printf("  Speed [kmph]:  %.2lf\n", gpvtg->gndspd_kmph);
            printf("  Speed [knots]: %.2lf\n", gpvtg->gndspd_knots);
        }

        nmea_free(nmea_data);
    }
}

void usb_lib_task(void *arg)
{
    while (1) {
        //Start handling system events
        uint32_t event_flags;
        usb_host_lib_handle_events(portMAX_DELAY, &event_flags);
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
            printf("No more clients\n");
            ESP_ERROR_CHECK(usb_host_device_free_all());
        }
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE) {
            break;
        }
    }

    //Short delay to allow task to be cleaned up
    vTaskDelay(10);
    //Clean up USB Host
    ESP_ERROR_CHECK(usb_host_uninstall());
    vTaskDelete(NULL);
}

/* ---------------------------------- Main ---------------------------------- */
extern "C" void app_main(void)
{
    //Install USB Host driver. Should only be called once in entire application
    ESP_LOGI(TAG, "Installing USB Host");
    usb_host_config_t host_config = {
        .skip_phy_setup = false,
        .intr_flags = ESP_INTR_FLAG_LEVEL1,
    };
    ESP_ERROR_CHECK(usb_host_install(&host_config));

    // Create a task that will handle USB library events
    xTaskCreate(usb_lib_task, "usb_lib", 4096, NULL, EXAMPLE_USB_HOST_PRIORITY, NULL);

    ESP_LOGI(TAG, "Installing CDC-ACM driver");
    ESP_ERROR_CHECK(cdc_acm_host_install(NULL));

    Bg96Usb *bg96 = new Bg96Usb();
    bg96->at_start(handle_rx, NULL);

    static char text1[] = "AT\r";
    static char text2[] = "AT+GSN\r";

    ESP_LOGI(TAG, "Sending AT");
    bg96->at_write((uint8_t *)text1, strlen(text1));
    vTaskDelay(100);

    ESP_LOGI(TAG, "Enabling GNSS");
    bg96->gnss_start(handle_gps);

    vTaskDelay(100);
    ESP_LOGI(TAG, "Sending AT+GSN");
    bg96->at_write((uint8_t *)text2, strlen(text2));
}
