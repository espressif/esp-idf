// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "soc/lcd_cam_reg.h"
#include "soc/lcd_cam_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LCD_LL_GET_HW(id) (((id) == 0) ? (&LCD_CAM) : NULL)

// Interrupt event, bit mask
#define LCD_LL_EVENT_VSYNC_END  (1 << 0)
#define LCD_LL_EVENT_TRANS_DONE (1 << 1)
#define LCD_LL_EVENT_MASK       (LCD_LL_EVENT_VSYNC_END | LCD_LL_EVENT_TRANS_DONE)

// Clock source ID represented in register
#define LCD_LL_CLOCK_SRC_XTAL    (1)
#define LCD_LL_CLOCK_SRC_APLL    (2)
#define LCD_LL_CLOCK_SRC_PLL160M (3)

// Maximum coefficient of clock prescaler
#define LCD_LL_CLOCK_PRESCALE_MAX (64)

static inline void lcd_ll_enable_clock(lcd_cam_dev_t *dev, bool en)
{
    dev->lcd_clock.clk_en = en;
}

static inline void lcd_ll_set_group_clock_src(lcd_cam_dev_t *dev, int src, int div_num, int div_a, int div_b)
{
    // lcd_clk = module_clock_src / (div_num + div_b / div_a)
    assert(div_num >= 2);
    dev->lcd_clock.lcd_clk_sel = src;
    dev->lcd_clock.lcd_clkm_div_num = div_num;
    dev->lcd_clock.lcd_clkm_div_a = div_a;
    dev->lcd_clock.lcd_clkm_div_b = div_b;
}

static inline void lcd_ll_set_clock_idle_level(lcd_cam_dev_t *dev, bool level)
{
    dev->lcd_clock.lcd_ck_idle_edge = level;
}

static inline void lcd_ll_set_pixel_clock_edge(lcd_cam_dev_t *dev, bool active_on_neg)
{
    dev->lcd_clock.lcd_clk_equ_sysclk = 0; // if we want to pixel_clk == lcd_clk, just make clkcnt = 0
    dev->lcd_clock.lcd_ck_out_edge = active_on_neg;
}

static inline void lcd_ll_set_pixel_clock_prescale(lcd_cam_dev_t *dev, uint32_t prescale)
{
    // Formula: pixel_clk = lcd_clk / (1 + clkcnt_n)
    dev->lcd_clock.lcd_clkcnt_n = prescale - 1;
}

static inline void lcd_ll_enable_rgb_yuv_convert(lcd_cam_dev_t *dev, bool en)
{
    dev->lcd_rgb_yuv.lcd_conv_bypass = en;
}

static inline void lcd_ll_set_phase_cycles(lcd_cam_dev_t *dev, uint32_t cmd_cycles, uint32_t dummy_cycles, uint32_t data_cycles)
{
    assert(cmd_cycles <= 2);
    dev->lcd_user.lcd_cmd = (cmd_cycles > 0);
    dev->lcd_user.lcd_dummy = (dummy_cycles > 0);
    dev->lcd_user.lcd_dout = (data_cycles > 0);
    dev->lcd_user.lcd_cmd_2_cycle_en = cmd_cycles > 1;
    dev->lcd_user.lcd_dummy_cyclelen = dummy_cycles - 1;
    dev->lcd_user.lcd_dout_cyclelen = data_cycles - 1;
}

static inline void lcd_ll_set_blank_cycles(lcd_cam_dev_t *dev, uint32_t fk_cycles, uint32_t bk_cycles)
{
    dev->lcd_misc.lcd_bk_en = (fk_cycles || bk_cycles);
    dev->lcd_misc.lcd_vfk_cyclelen = fk_cycles - 1;
    dev->lcd_misc.lcd_vbk_cyclelen = bk_cycles - 1;
}

static inline void lcd_ll_set_data_width(lcd_cam_dev_t *dev, uint32_t width)
{
    dev->lcd_user.lcd_2byte_en = (width == 16);
}

static inline uint32_t lcd_ll_get_data_width(lcd_cam_dev_t *dev)
{
    return dev->lcd_user.lcd_2byte_en ? 16 : 8;
}

static inline void lcd_ll_enable_output_always_on(lcd_cam_dev_t *dev, bool en)
{
    dev->lcd_user.lcd_always_out_en = en;
}

static inline void lcd_ll_start(lcd_cam_dev_t *dev)
{
    dev->lcd_user.lcd_update = 1; // update parameters before start transaction
    dev->lcd_user.lcd_start = 1;
}

static inline void lcd_ll_stop(lcd_cam_dev_t *dev)
{
    dev->lcd_user.lcd_start = 0;
    dev->lcd_user.lcd_update = 1; // self clear
}

static inline void lcd_ll_reset(lcd_cam_dev_t *dev)
{
    dev->lcd_user.lcd_reset = 1;
    dev->lcd_user.lcd_reset = 0;
}

static inline void lcd_ll_reverse_data_bit_order(lcd_cam_dev_t *dev, bool en)
{
    // whether to change LCD_DATA_out[N:0] to LCD_DATA_out[0:N]
    dev->lcd_user.lcd_bit_order = en;
}

static inline void lcd_ll_reverse_data_byte_order(lcd_cam_dev_t *dev, uint32_t data_width, bool en)
{
    if (data_width == 8) {
        dev->lcd_user.lcd_8bits_order = en; // valid in 8bit mode
        dev->lcd_user.lcd_byte_order = 0;
    } else if (data_width == 16) {
        dev->lcd_user.lcd_byte_order = en;  // valid in 16bit mode
        dev->lcd_user.lcd_8bits_order = 0;
    }
}

static inline void lcd_ll_fifo_reset(lcd_cam_dev_t *dev)
{
    dev->lcd_misc.lcd_afifo_reset = 1;
    dev->lcd_misc.lcd_afifo_reset = 0;
}

static inline void lcd_ll_set_dc_level(lcd_cam_dev_t *dev, bool idle_phase, bool cmd_phase, bool dummy_phase, bool data_phase)
{
    dev->lcd_misc.lcd_cd_idle_edge = idle_phase;
    dev->lcd_misc.lcd_cd_cmd_set = (cmd_phase != idle_phase);
    dev->lcd_misc.lcd_cd_dummy_set = (dummy_phase != idle_phase);
    dev->lcd_misc.lcd_cd_data_set = (data_phase != idle_phase);
}

static inline void lcd_ll_set_dc_delay_ticks(lcd_cam_dev_t *dev, uint32_t delay)
{
    dev->lcd_dly_mode.lcd_cd_mode = delay;
}

static inline void lcd_ll_set_command(lcd_cam_dev_t *dev, uint32_t data_width, uint32_t command)
{
    // if command phase has two cycles, in the first cycle, command[15:0] is sent out via lcd_data_out[15:0]
    // in the second cycle, command[31:16] is sent out via lcd_data_out[15:0]
    // no matter the LCD is in 8bit mode or 16bit mode
    // so this is a workaround especially for 8bit mode
    if (data_width == 8) {
        command = (command & 0xFF) | (command & 0xFF00) << 8;
    }
    dev->lcd_cmd_val = command;
}

static inline void lcd_ll_enable_rgb_mode(lcd_cam_dev_t *dev, bool en)
{
    dev->lcd_ctrl.lcd_rgb_mode_en = en;
}

static inline void lcd_ll_enable_auto_next_frame(lcd_cam_dev_t *dev, bool en)
{
    // in RGB mode, enabling "next frame" means LCD controller keeps sending frame data
    dev->lcd_misc.lcd_next_frame_en = en;
}

static inline void lcd_ll_set_horizontal_timing(lcd_cam_dev_t *dev, uint32_t hsw, uint32_t hbp, uint32_t active_width, uint32_t hfp)
{
    dev->lcd_ctrl2.lcd_hsync_width = hsw;
    dev->lcd_ctrl.lcd_hb_front = hbp;
    dev->lcd_ctrl1.lcd_ha_width = active_width;
    dev->lcd_ctrl1.lcd_ht_width = hsw + hbp + active_width + hfp;
}

static inline void lcd_ll_set_vertical_timing(lcd_cam_dev_t *dev, uint32_t vsw, uint32_t vbp, uint32_t active_height, uint32_t vfp)
{
    dev->lcd_ctrl2.lcd_vsync_width = vsw;
    dev->lcd_ctrl1.lcd_vb_front = vbp;
    dev->lcd_ctrl.lcd_va_height = active_height;
    dev->lcd_ctrl.lcd_vt_height = vsw + vbp + active_height + vfp;
}

static inline void lcd_ll_set_idle_level(lcd_cam_dev_t *dev, bool hsync_idle_level, bool vsync_idle_level, bool de_idle_level)
{
    dev->lcd_ctrl2.lcd_hsync_idle_pol = hsync_idle_level;
    dev->lcd_ctrl2.lcd_vsync_idle_pol = vsync_idle_level;
    dev->lcd_ctrl2.lcd_de_idle_pol = de_idle_level;
}

static inline void lcd_ll_set_delay_ticks(lcd_cam_dev_t *dev, uint32_t hsync_delay, uint32_t vsync_delay, uint32_t de_delay)
{
    dev->lcd_dly_mode.lcd_hsync_mode = hsync_delay;
    dev->lcd_dly_mode.lcd_vsync_mode = vsync_delay;
    dev->lcd_dly_mode.lcd_de_mode = de_delay;
}

static inline void lcd_ll_set_data_delay_ticks(lcd_cam_dev_t *dev, uint32_t delay)
{
    uint32_t reg_val = 0;
    for (int i = 0; i < 16; i++) {
        reg_val |= (delay & 0x03) << (2 * i);
    }
    dev->lcd_data_dout_mode.val = reg_val;
}

static inline void lcd_ll_enable_interrupt(lcd_cam_dev_t *dev, uint32_t mask, bool en)
{
    if (en) {
        dev->lc_dma_int_ena.val |= mask & 0x03;
    } else {
        dev->lc_dma_int_ena.val &= ~(mask & 0x03);
    }
}

static inline uint32_t lcd_ll_get_interrupt_status(lcd_cam_dev_t *dev)
{
    return dev->lc_dma_int_st.val & 0x03;
}

static inline void lcd_ll_clear_interrupt_status(lcd_cam_dev_t *dev, uint32_t mask)
{
    dev->lc_dma_int_clr.val = mask & 0x03;
}

static inline uint32_t lcd_ll_get_interrupt_status_reg(lcd_cam_dev_t *dev)
{
    return (uint32_t)(&dev->lc_dma_int_st);
}

#ifdef __cplusplus
}
#endif
