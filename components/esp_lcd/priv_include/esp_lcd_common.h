/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stddef.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "hal/dma_types.h"
#include "esp_intr_alloc.h"
#include "esp_heap_caps.h"
#if SOC_LCDCAM_SUPPORTED
#include "hal/lcd_hal.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

// size of the internal buffer to transform the data into a proper format (e.g. data endian)
#define LCD_I80_IO_FORMAT_BUF_SIZE  32

#define LCD_I80_INTR_ALLOC_FLAGS     ESP_INTR_FLAG_INTRDISABLED
#define LCD_I80_MEM_ALLOC_CAPS       MALLOC_CAP_DEFAULT

#define LCD_PERIPH_CLOCK_PRE_SCALE (2) // This is the minimum divider that can be applied to LCD peripheral

#if SOC_PERIPH_CLK_CTRL_SHARED
#define LCD_CLOCK_SRC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define LCD_CLOCK_SRC_ATOMIC()
#endif

#define LCD_DMA_DESCRIPTOR_BUFFER_MAX_SIZE 4095

#if SOC_LCDCAM_SUPPORTED

typedef enum {
    LCD_COM_DEVICE_TYPE_I80,
    LCD_COM_DEVICE_TYPE_RGB
} lcd_com_device_type_t;

/**
 * @brief Register a LCD device to platform
 *
 * @param device_type Device type, refer to lcd_com_device_type_t
 * @param device_obj Device object
 * @return >=0: member_id, <0: no free lcd bus/panel slots
 */
int lcd_com_register_device(lcd_com_device_type_t device_type, void *device_obj);

/**
 * @brief Remove a device from platform
 *
 * @param device_type Device type, refer to lcd_com_device_type_t
 * @param member_id member ID
 */
void lcd_com_remove_device(lcd_com_device_type_t device_type, int member_id);
#endif // SOC_LCDCAM_SUPPORTED

/**
 * @brief Reverse the bytes in the buffer
 *
 * @note  LCD is big-endian, e.g. to send command 0x1234, byte 0x12 should appear on the bus first
 *        However, the low level peripheral (like i80, i2s) will send 0x34 first.
 *        This helper function is used to reverse the bytes order
 *
 * @param buf buffer address
 * @param start start index of the buffer
 * @param end end index of the buffer
 */
static inline void lcd_com_reverse_buffer_bytes(uint8_t *buf, int start, int end)
{
    uint8_t temp = 0;
    while (start < end) {
        temp = buf[start];
        buf[start] = buf[end];
        buf[end] = temp;
        start++;
        end--;
    }
}

#ifdef __cplusplus
}
#endif
