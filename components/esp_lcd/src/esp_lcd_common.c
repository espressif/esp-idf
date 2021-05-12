/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "soc/rtc.h" // for querying XTAL clock
#include "soc/soc_caps.h"
#if SOC_LCDCAM_SUPPORTED
#include "esp_lcd_common.h"
#include "hal/lcd_ll.h"
#include "hal/lcd_hal.h"

typedef struct esp_lcd_platform_t {
    portMUX_TYPE spinlock; // spinlock used to protect platform level resources
    union {
        void *panels[SOC_LCD_RGB_PANELS]; // array of RGB LCD panel instances
        void *buses[SOC_LCD_I80_BUSES];   // array of i80 bus instances
    }; // LCD peripheral can only work under either RGB mode or intel 8080 mode
} esp_lcd_platform_t;

esp_lcd_platform_t s_lcd_platform = {
    .spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED,
    .buses = {} // initially the bus slots and panel slots are empty
};

int lcd_com_register_device(lcd_com_device_type_t device_type, void *device_obj)
{
    int member_id = -1;
    switch (device_type) {
    case LCD_COM_DEVICE_TYPE_I80:
        // search for a bus slot then register to platform
        for (int i = 0; (i < SOC_LCD_I80_BUSES) && (member_id == -1); i++) {
            portENTER_CRITICAL(&s_lcd_platform.spinlock);
            if (!s_lcd_platform.buses[i]) {
                s_lcd_platform.buses[i] = device_obj;
                member_id = i;
            }
            portEXIT_CRITICAL(&s_lcd_platform.spinlock);
        }
        break;
    case LCD_COM_DEVICE_TYPE_RGB:
        // search for a panel slot then register to platform
        for (int i = 0; (i < SOC_LCD_RGB_PANELS) && (member_id == -1); i++) {
            portENTER_CRITICAL(&s_lcd_platform.spinlock);
            if (!s_lcd_platform.panels[i]) {
                s_lcd_platform.panels[i] = device_obj;
                member_id = i;
            }
            portEXIT_CRITICAL(&s_lcd_platform.spinlock);
        }
        break;
    default:
        break;
    }
    return member_id;
}

void lcd_com_remove_device(lcd_com_device_type_t device_type, int member_id)
{
    switch (device_type) {
    case LCD_COM_DEVICE_TYPE_I80:
        portENTER_CRITICAL(&s_lcd_platform.spinlock);
        if (s_lcd_platform.buses[member_id]) {
            s_lcd_platform.buses[member_id] = NULL;
        }
        portEXIT_CRITICAL(&s_lcd_platform.spinlock);
        break;
    case LCD_COM_DEVICE_TYPE_RGB:
        portENTER_CRITICAL(&s_lcd_platform.spinlock);
        if (s_lcd_platform.panels[member_id]) {
            s_lcd_platform.panels[member_id] = NULL;
        }
        portEXIT_CRITICAL(&s_lcd_platform.spinlock);
        break;
    default:
        break;
    }
}

unsigned long lcd_com_select_periph_clock(lcd_hal_context_t *hal)
{
    unsigned long resolution_hz = 0;
    int clock_source = -1;
#if CONFIG_LCD_PERIPH_CLK_SRC_PLL160M
    resolution_hz = 160000000 / LCD_PERIPH_CLOCK_PRE_SCALE;
    clock_source = LCD_LL_CLOCK_SRC_PLL160M;
#elif CONFIG_LCD_PERIPH_CLK_SRC_XTAL
    resolution_hz = rtc_clk_xtal_freq_get() * 1000000 / LCD_PERIPH_CLOCK_PRE_SCALE;
    clock_source = LCD_LL_CLOCK_SRC_XTAL;
#else
#error "invalid LCD peripheral clock source"
#endif

    lcd_ll_set_group_clock_src(hal->dev, clock_source, LCD_PERIPH_CLOCK_PRE_SCALE, 1, 0);
    return resolution_hz;
}

void lcd_com_mount_dma_data(dma_descriptor_t *desc_head, const void *buffer, size_t len)
{
    size_t prepared_length = 0;
    uint8_t *data = (uint8_t *)buffer;
    dma_descriptor_t *desc = desc_head;
    while (len > DMA_DESCRIPTOR_BUFFER_MAX_SIZE) {
        desc->dw0.suc_eof = 0; // not the end of the transaction
        desc->dw0.size = DMA_DESCRIPTOR_BUFFER_MAX_SIZE;
        desc->dw0.length = DMA_DESCRIPTOR_BUFFER_MAX_SIZE;
        desc->dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
        desc->buffer = &data[prepared_length];
        desc = desc->next; // move to next descriptor
        prepared_length += DMA_DESCRIPTOR_BUFFER_MAX_SIZE;
        len -= DMA_DESCRIPTOR_BUFFER_MAX_SIZE;
    }
    if (len) {
        desc->dw0.suc_eof = 1; // end of the transaction
        desc->dw0.size = len;
        desc->dw0.length = len;
        desc->dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
        desc->buffer = &data[prepared_length];
        desc = desc->next; // move to next descriptor
        prepared_length += len;
    }
}

#endif // SOC_LCDCAM_SUPPORTED
