/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "soc/soc_caps.h"
#if SOC_LCDCAM_SUPPORTED
#include "hal/lcd_hal.h"
#include "hal/dma_types.h"
#else
#error "lcd peripheral is not supported on this chip"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_LCDCAM_SUPPORTED

#define LCD_PERIPH_CLOCK_PRE_SCALE (2) // This is the minimum divider that can be applied to LCD peripheral

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

/**
 * @brief Select clock source and return peripheral clock resolution (in Hz)
 *
 * @note The clock source selection is injected by the Kconfig system,
 *       dynamic switching peripheral clock source is not supported in driver.
 *
 * @param hal HAL object
 * @return Peripheral clock resolution, in Hz
 */
unsigned long lcd_com_select_periph_clock(lcd_hal_context_t *hal);

/**
 * @brief Mount data to DMA descriptors
 *
 * @param desc_head Point to the head of DMA descriptor chain
 * @param buffer Data buffer
 * @param len Size of the data buffer, in bytes
 */
void lcd_com_mount_dma_data(dma_descriptor_t *desc_head, const void *buffer, size_t len);

#endif // SOC_LCDCAM_SUPPORTED

#ifdef __cplusplus
}
#endif
