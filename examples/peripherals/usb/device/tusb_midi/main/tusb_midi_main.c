/*
 * SPDX-FileCopyrightText: 2019 Ha Thach (tinyusb.org)
 *
 * SPDX-License-Identifier: MIT
 *
 * SPDX-FileContributor: 2022 Espressif Systems (Shanghai) CO LTD
 */

#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "tinyusb.h"
#include "esp_timer.h"

static const char *TAG = "example";

static void midi_task_read_example(void *arg)
{
    // The MIDI interface always creates input and output port/jack descriptors
    // regardless of these being used or not. Therefore incoming traffic should be read
    // (possibly just discarded) to avoid the sender blocking in IO
    uint8_t packet[4];
    bool read = false;
    for (;;) {
        vTaskDelay(1);
        while (tud_midi_available()) {
            read = tud_midi_packet_read(packet);
            if (read) {
                ESP_LOGI(TAG, "Read - Time (ms since boot): %lld, Data: %02hhX %02hhX %02hhX %02hhX",
                         esp_timer_get_time(), packet[0], packet[1], packet[2], packet[3]);
            }
        }
    }
}

static void periodic_midi_write_example_cb(void *arg)
{
    // Example melody stored as an array of note values
    uint8_t const note_sequence[] = {
        74, 78, 81, 86, 90, 93, 98, 102, 57, 61, 66, 69, 73, 78, 81, 85, 88, 92, 97, 100, 97, 92, 88, 85, 81, 78,
        74, 69, 66, 62, 57, 62, 66, 69, 74, 78, 81, 86, 90, 93, 97, 102, 97, 93, 90, 85, 81, 78, 73, 68, 64, 61,
        56, 61, 64, 68, 74, 78, 81, 86, 90, 93, 98, 102
    };

    static uint8_t const cable_num = 0; // MIDI jack associated with USB endpoint
    static uint8_t const channel = 0; // 0 for channel 1
    static uint32_t note_pos = 0;

    // Previous positions in the note sequence.
    int previous = note_pos - 1;

    // If we currently are at position 0, set the
    // previous position to the last note in the sequence.
    if (previous < 0) {
        previous = sizeof(note_sequence) - 1;
    }

    // Send Note On for current position at full velocity (127) on channel 1.
    uint8_t note_on[3] = {0x90 | channel, note_sequence[note_pos], 127};
    tud_midi_stream_write(cable_num, note_on, 3);

    // Send Note Off for previous note.
    uint8_t note_off[3] = {0x80 | channel, note_sequence[previous], 0};
    tud_midi_stream_write(cable_num, note_off, 3);

    // Increment position
    note_pos++;

    // If we are at the end of the sequence, start over.
    if (note_pos >= sizeof(note_sequence)) {
        note_pos = 0;
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "USB initialization");

    tinyusb_config_t const tusb_cfg = {
        .device_descriptor = NULL, // If device_descriptor is NULL, tinyusb_driver_install() will use Kconfig
        .string_descriptor = NULL,
        .external_phy = false // In the most cases you need to use a `false` value
    };
    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    ESP_LOGI(TAG, "USB initialization DONE");

    // Periodically send MIDI packets
    int const tempo = 286;
    const esp_timer_create_args_t periodic_midi_args = {
        .callback = &periodic_midi_write_example_cb,
        /* name is optional, but may help identify the timer when debugging */
        .name = "periodic_midi"
    };

    ESP_LOGI(TAG, "MIDI write task init");
    esp_timer_handle_t periodic_midi_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_midi_args, &periodic_midi_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_midi_timer, tempo * 1000));

    // Read recieved MIDI packets
    ESP_LOGI(TAG, "MIDI read task init");
    xTaskCreate(midi_task_read_example, "midi_task_read_example", 2 * 1024, NULL, 5, NULL);
}
