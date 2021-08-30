// Copyright 2017-2021 Espressif Systems (Shanghai) PTE LTD
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
#ifndef _SOC_INTERRUPT_CORE0_STRUCT_H_
#define _SOC_INTERRUPT_CORE0_STRUCT_H_
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct interrupt_core0_dev_s {
    union {
        struct {
            uint32_t core0_mac_intr_map: 5;
            uint32_t reserved5:         27;
        };
        uint32_t val;
    } core0_mac_intr_map;
    union {
        struct {
            uint32_t core0_mac_nmi_map: 5;
            uint32_t reserved5:        27;
        };
        uint32_t val;
    } core0_mac_nmi_map;
    union {
        struct {
            uint32_t core0_pwr_intr_map: 5;
            uint32_t reserved5:         27;
        };
        uint32_t val;
    } core0_pwr_intr_map;
    union {
        struct {
            uint32_t core0_bb_int_map: 5;
            uint32_t reserved5:       27;
        };
        uint32_t val;
    } core0_bb_int_map;
    union {
        struct {
            uint32_t core0_bt_mac_int_map: 5;
            uint32_t reserved5:           27;
        };
        uint32_t val;
    } core0_bt_mac_int_map;
    union {
        struct {
            uint32_t core0_bt_bb_int_map: 5;
            uint32_t reserved5:          27;
        };
        uint32_t val;
    } core0_bt_bb_int_map;
    union {
        struct {
            uint32_t core0_bt_bb_nmi_map: 5;
            uint32_t reserved5:          27;
        };
        uint32_t val;
    } core0_bt_bb_nmi_map;
    union {
        struct {
            uint32_t core0_rwbt_irq_map: 5;
            uint32_t reserved5:         27;
        };
        uint32_t val;
    } core0_rwbt_irq_map;
    union {
        struct {
            uint32_t core0_rwble_irq_map: 5;
            uint32_t reserved5:          27;
        };
        uint32_t val;
    } core0_rwble_irq_map;
    union {
        struct {
            uint32_t core0_rwbt_nmi_map: 5;
            uint32_t reserved5:         27;
        };
        uint32_t val;
    } core0_rwbt_nmi_map;
    union {
        struct {
            uint32_t core0_rwble_nmi_map: 5;
            uint32_t reserved5:          27;
        };
        uint32_t val;
    } core0_rwble_nmi_map;
    union {
        struct {
            uint32_t core0_i2c_mst_int_map: 5;
            uint32_t reserved5:            27;
        };
        uint32_t val;
    } core0_i2c_mst_int_map;
    union {
        struct {
            uint32_t core0_slc0_intr_map: 5;
            uint32_t reserved5:          27;
        };
        uint32_t val;
    } core0_slc0_intr_map;
    union {
        struct {
            uint32_t core0_slc1_intr_map: 5;
            uint32_t reserved5:          27;
        };
        uint32_t val;
    } core0_slc1_intr_map;
    union {
        struct {
            uint32_t core0_uhci0_intr_map: 5;
            uint32_t reserved5:           27;
        };
        uint32_t val;
    } core0_uhci0_intr_map;
    union {
        struct {
            uint32_t core0_uhci1_intr_map: 5;
            uint32_t reserved5:           27;
        };
        uint32_t val;
    } core0_uhci1_intr_map;
    union {
        struct {
            uint32_t core0_gpio_interrupt_pro_map: 5;
            uint32_t reserved5:                   27;
        };
        uint32_t val;
    } core0_gpio_interrupt_pro_map;
    union {
        struct {
            uint32_t core0_gpio_interrupt_pro_nmi_map: 5;
            uint32_t reserved5:                       27;
        };
        uint32_t val;
    } core0_gpio_interrupt_pro_nmi_map;
    union {
        struct {
            uint32_t core0_gpio_interrupt_app_map: 5;
            uint32_t reserved5:                   27;
        };
        uint32_t val;
    } core0_gpio_interrupt_app_map;
    union {
        struct {
            uint32_t core0_gpio_interrupt_app_nmi_map: 5;
            uint32_t reserved5:                       27;
        };
        uint32_t val;
    } core0_gpio_interrupt_app_nmi_map;
    union {
        struct {
            uint32_t core0_spi_intr_1_map: 5;
            uint32_t reserved5:           27;
        };
        uint32_t val;
    } core0_spi_intr_1_map;
    union {
        struct {
            uint32_t core0_spi_intr_2_map: 5;
            uint32_t reserved5:           27;
        };
        uint32_t val;
    } core0_spi_intr_2_map;
    union {
        struct {
            uint32_t core0_spi_intr_3_map: 5;
            uint32_t reserved5:           27;
        };
        uint32_t val;
    } core0_spi_intr_3_map;
    union {
        struct {
            uint32_t core0_spi_intr_4_map: 5;
            uint32_t reserved5:           27;
        };
        uint32_t val;
    } core0_spi_intr_4_map;
    union {
        struct {
            uint32_t core0_lcd_cam_int_map: 5;
            uint32_t reserved5:            27;
        };
        uint32_t val;
    } core0_lcd_cam_int_map;
    union {
        struct {
            uint32_t core0_i2s0_int_map: 5;
            uint32_t reserved5:         27;
        };
        uint32_t val;
    } core0_i2s0_int_map;
    union {
        struct {
            uint32_t core0_i2s1_int_map: 5;
            uint32_t reserved5:         27;
        };
        uint32_t val;
    } core0_i2s1_int_map;
    union {
        struct {
            uint32_t core0_uart_intr_map: 5;
            uint32_t reserved5:          27;
        };
        uint32_t val;
    } core0_uart_intr_map;
    union {
        struct {
            uint32_t core0_uart1_intr_map: 5;
            uint32_t reserved5:           27;
        };
        uint32_t val;
    } core0_uart1_intr_map;
    union {
        struct {
            uint32_t core0_uart2_intr_map: 5;
            uint32_t reserved5:           27;
        };
        uint32_t val;
    } core0_uart2_intr_map;
    union {
        struct {
            uint32_t core0_sdio_host_interrupt_map: 5;
            uint32_t reserved5:                    27;
        };
        uint32_t val;
    } core0_sdio_host_interrupt_map;
    union {
        struct {
            uint32_t core0_pwm0_intr_map: 5;
            uint32_t reserved5:          27;
        };
        uint32_t val;
    } core0_pwm0_intr_map;
    union {
        struct {
            uint32_t core0_pwm1_intr_map: 5;
            uint32_t reserved5:          27;
        };
        uint32_t val;
    } core0_pwm1_intr_map;
    union {
        struct {
            uint32_t core0_pwm2_intr_map: 5;
            uint32_t reserved5:          27;
        };
        uint32_t val;
    } core0_pwm2_intr_map;
    union {
        struct {
            uint32_t core0_pwm3_intr_map: 5;
            uint32_t reserved5:          27;
        };
        uint32_t val;
    } core0_pwm3_intr_map;
    union {
        struct {
            uint32_t core0_ledc_int_map: 5;
            uint32_t reserved5:         27;
        };
        uint32_t val;
    } core0_ledc_int_map;
    union {
        struct {
            uint32_t core0_efuse_int_map: 5;
            uint32_t reserved5:          27;
        };
        uint32_t val;
    } core0_efuse_int_map;
    union {
        struct {
            uint32_t core0_can_int_map: 5;
            uint32_t reserved5:        27;
        };
        uint32_t val;
    } core0_can_int_map;
    union {
        struct {
            uint32_t core0_usb_intr_map: 5;
            uint32_t reserved5:         27;
        };
        uint32_t val;
    } core0_usb_intr_map;
    union {
        struct {
            uint32_t core0_rtc_core_intr_map: 5;
            uint32_t reserved5:              27;
        };
        uint32_t val;
    } core0_rtc_core_intr_map;
    union {
        struct {
            uint32_t core0_rmt_intr_map: 5;
            uint32_t reserved5:         27;
        };
        uint32_t val;
    } core0_rmt_intr_map;
    union {
        struct {
            uint32_t core0_pcnt_intr_map: 5;
            uint32_t reserved5:          27;
        };
        uint32_t val;
    } core0_pcnt_intr_map;
    union {
        struct {
            uint32_t core0_i2c_ext0_intr_map: 5;
            uint32_t reserved5:              27;
        };
        uint32_t val;
    } core0_i2c_ext0_intr_map;
    union {
        struct {
            uint32_t core0_i2c_ext1_intr_map: 5;
            uint32_t reserved5:              27;
        };
        uint32_t val;
    } core0_i2c_ext1_intr_map;
    union {
        struct {
            uint32_t core0_spi2_dma_int_map: 5;
            uint32_t reserved5:             27;
        };
        uint32_t val;
    } core0_spi2_dma_int_map;
    union {
        struct {
            uint32_t core0_spi3_dma_int_map: 5;
            uint32_t reserved5:             27;
        };
        uint32_t val;
    } core0_spi3_dma_int_map;
    union {
        struct {
            uint32_t core0_spi4_dma_int_map: 5;
            uint32_t reserved5:             27;
        };
        uint32_t val;
    } core0_spi4_dma_int_map;
    union {
        struct {
            uint32_t core0_wdg_int_map: 5;
            uint32_t reserved5:        27;
        };
        uint32_t val;
    } core0_wdg_int_map;
    union {
        struct {
            uint32_t core0_timer_int1_map: 5;
            uint32_t reserved5:           27;
        };
        uint32_t val;
    } core0_timer_int1_map;
    union {
        struct {
            uint32_t core0_timer_int2_map: 5;
            uint32_t reserved5:           27;
        };
        uint32_t val;
    } core0_timer_int2_map;
    union {
        struct {
            uint32_t core0_tg_t0_int_map: 5;
            uint32_t reserved5:          27;
        };
        uint32_t val;
    } core0_tg_t0_int_map;
    union {
        struct {
            uint32_t core0_tg_t1_int_map: 5;
            uint32_t reserved5:          27;
        };
        uint32_t val;
    } core0_tg_t1_int_map;
    union {
        struct {
            uint32_t core0_tg_wdt_int_map: 5;
            uint32_t reserved5:           27;
        };
        uint32_t val;
    } core0_tg_wdt_int_map;
    union {
        struct {
            uint32_t core0_tg1_t0_int_map: 5;
            uint32_t reserved5:           27;
        };
        uint32_t val;
    } core0_tg1_t0_int_map;
    union {
        struct {
            uint32_t core0_tg1_t1_int_map: 5;
            uint32_t reserved5:           27;
        };
        uint32_t val;
    } core0_tg1_t1_int_map;
    union {
        struct {
            uint32_t core0_tg1_wdt_int_map: 5;
            uint32_t reserved5:            27;
        };
        uint32_t val;
    } core0_tg1_wdt_int_map;
    union {
        struct {
            uint32_t core0_cache_ia_int_map: 5;
            uint32_t reserved5:             27;
        };
        uint32_t val;
    } core0_cache_ia_int_map;
    union {
        struct {
            uint32_t core0_systimer_target0_int_map: 5;
            uint32_t reserved5:                     27;
        };
        uint32_t val;
    } core0_systimer_target0_int_map;
    union {
        struct {
            uint32_t core0_systimer_target1_int_map: 5;
            uint32_t reserved5:                     27;
        };
        uint32_t val;
    } core0_systimer_target1_int_map;
    union {
        struct {
            uint32_t core0_systimer_target2_int_map: 5;
            uint32_t reserved5:                     27;
        };
        uint32_t val;
    } core0_systimer_target2_int_map;
    union {
        struct {
            uint32_t core0_spi_mem_reject_intr_map: 5;
            uint32_t reserved5:                    27;
        };
        uint32_t val;
    } core0_spi_mem_reject_intr_map;
    union {
        struct {
            uint32_t core0_dcache_preload_int_map: 5;
            uint32_t reserved5:                   27;
        };
        uint32_t val;
    } core0_dcache_preload_int_map;
    union {
        struct {
            uint32_t core0_icache_preload_int_map: 5;
            uint32_t reserved5:                   27;
        };
        uint32_t val;
    } core0_icache_preload_int_map;
    union {
        struct {
            uint32_t core0_dcache_sync_int_map: 5;
            uint32_t reserved5:                27;
        };
        uint32_t val;
    } core0_dcache_sync_int_map;
    union {
        struct {
            uint32_t core0_icache_sync_int_map: 5;
            uint32_t reserved5:                27;
        };
        uint32_t val;
    } core0_icache_sync_int_map;
    union {
        struct {
            uint32_t core0_apb_adc_int_map: 5;
            uint32_t reserved5:            27;
        };
        uint32_t val;
    } core0_apb_adc_int_map;
    union {
        struct {
            uint32_t core0_dma_in_ch0_int_map: 5;
            uint32_t reserved5:               27;
        };
        uint32_t val;
    } core0_dma_in_ch0_int_map;
    union {
        struct {
            uint32_t core0_dma_in_ch1_int_map: 5;
            uint32_t reserved5:               27;
        };
        uint32_t val;
    } core0_dma_in_ch1_int_map;
    union {
        struct {
            uint32_t core0_dma_in_ch2_int_map: 5;
            uint32_t reserved5:               27;
        };
        uint32_t val;
    } core0_dma_in_ch2_int_map;
    union {
        struct {
            uint32_t core0_dma_in_ch3_int_map: 5;
            uint32_t reserved5:               27;
        };
        uint32_t val;
    } core0_dma_in_ch3_int_map;
    union {
        struct {
            uint32_t core0_dma_in_ch4_int_map: 5;
            uint32_t reserved5:               27;
        };
        uint32_t val;
    } core0_dma_in_ch4_int_map;
    union {
        struct {
            uint32_t core0_dma_out_ch0_int_map: 5;
            uint32_t reserved5:                27;
        };
        uint32_t val;
    } core0_dma_out_ch0_int_map;
    union {
        struct {
            uint32_t core0_dma_out_ch1_int_map: 5;
            uint32_t reserved5:                27;
        };
        uint32_t val;
    } core0_dma_out_ch1_int_map;
    union {
        struct {
            uint32_t core0_dma_out_ch2_int_map: 5;
            uint32_t reserved5:                27;
        };
        uint32_t val;
    } core0_dma_out_ch2_int_map;
    union {
        struct {
            uint32_t core0_dma_out_ch3_int_map: 5;
            uint32_t reserved5:                27;
        };
        uint32_t val;
    } core0_dma_out_ch3_int_map;
    union {
        struct {
            uint32_t core0_dma_out_ch4_int_map: 5;
            uint32_t reserved5:                27;
        };
        uint32_t val;
    } core0_dma_out_ch4_int_map;
    union {
        struct {
            uint32_t core0_rsa_int_map: 5;
            uint32_t reserved5:        27;
        };
        uint32_t val;
    } core0_rsa_int_map;
    union {
        struct {
            uint32_t core0_aes_int_map: 5;
            uint32_t reserved5:        27;
        };
        uint32_t val;
    } core0_aes_int_map;
    union {
        struct {
            uint32_t core0_sha_int_map: 5;
            uint32_t reserved5:        27;
        };
        uint32_t val;
    } core0_sha_int_map;
    union {
        struct {
            uint32_t core0_cpu_intr_from_cpu_0_map: 5;
            uint32_t reserved5:                    27;
        };
        uint32_t val;
    } core0_cpu_intr_from_cpu_0_map;
    union {
        struct {
            uint32_t core0_cpu_intr_from_cpu_1_map: 5;
            uint32_t reserved5:                    27;
        };
        uint32_t val;
    } core0_cpu_intr_from_cpu_1_map;
    union {
        struct {
            uint32_t core0_cpu_intr_from_cpu_2_map: 5;
            uint32_t reserved5:                    27;
        };
        uint32_t val;
    } core0_cpu_intr_from_cpu_2_map;
    union {
        struct {
            uint32_t core0_cpu_intr_from_cpu_3_map: 5;
            uint32_t reserved5:                    27;
        };
        uint32_t val;
    } core0_cpu_intr_from_cpu_3_map;
    union {
        struct {
            uint32_t core0_assist_debug_intr_map: 5;
            uint32_t reserved5:                  27;
        };
        uint32_t val;
    } core0_assist_debug_intr_map;
    union {
        struct {
            uint32_t core0_dma_apbperi_pms_monitor_violate_intr_map: 5;
            uint32_t reserved5:                                     27;
        };
        uint32_t val;
    } core0_dma_apbperi_pms_monitor_violate_intr_map;
    union {
        struct {
            uint32_t core0_core_0_iram0_pms_monitor_violate_intr_map: 5;
            uint32_t reserved5:                                      27;
        };
        uint32_t val;
    } core0_core_0_iram0_pms_monitor_violate_intr_map;
    union {
        struct {
            uint32_t core0_core_0_dram0_pms_monitor_violate_intr_map: 5;
            uint32_t reserved5:                                      27;
        };
        uint32_t val;
    } core0_core_0_dram0_pms_monitor_violate_intr_map;
    union {
        struct {
            uint32_t core0_core_0_pif_pms_monitor_violate_intr_map: 5;
            uint32_t reserved5:                                    27;
        };
        uint32_t val;
    } core0_core_0_pif_pms_monitor_violate_intr_map;
    union {
        struct {
            uint32_t core0_core_0_pif_pms_monitor_violate_size_intr_map: 5;
            uint32_t reserved5:                                         27;
        };
        uint32_t val;
    } core0_core_0_pif_pms_monitor_violate_size_intr_map;
    union {
        struct {
            uint32_t core0_core_1_iram0_pms_monitor_violate_intr_map: 5;
            uint32_t reserved5:                                      27;
        };
        uint32_t val;
    } core0_core_1_iram0_pms_monitor_violate_intr_map;
    union {
        struct {
            uint32_t core0_core_1_dram0_pms_monitor_violate_intr_map: 5;
            uint32_t reserved5:                                      27;
        };
        uint32_t val;
    } core0_core_1_dram0_pms_monitor_violate_intr_map;
    union {
        struct {
            uint32_t core0_core_1_pif_pms_monitor_violate_intr_map: 5;
            uint32_t reserved5:                                    27;
        };
        uint32_t val;
    } core0_core_1_pif_pms_monitor_violate_intr_map;
    union {
        struct {
            uint32_t core0_core_1_pif_pms_monitor_violate_size_intr_map: 5;
            uint32_t reserved5:                                         27;
        };
        uint32_t val;
    } core0_core_1_pif_pms_monitor_violate_size_intr_map;
    union {
        struct {
            uint32_t core0_backup_pms_violate_intr_map: 5;
            uint32_t reserved5:                        27;
        };
        uint32_t val;
    } core0_backup_pms_violate_intr_map;
    union {
        struct {
            uint32_t core0_cache_core0_acs_int_map: 5;
            uint32_t reserved5:                    27;
        };
        uint32_t val;
    } core0_cache_core0_acs_int_map;
    union {
        struct {
            uint32_t core0_cache_core1_acs_int_map: 5;
            uint32_t reserved5:                    27;
        };
        uint32_t val;
    } core0_cache_core1_acs_int_map;
    union {
        struct {
            uint32_t core0_usb_device_int_map: 5;
            uint32_t reserved5:               27;
        };
        uint32_t val;
    } core0_usb_device_int_map;
    union {
        struct {
            uint32_t core0_peri_backup_int_map: 5;
            uint32_t reserved5:                27;
        };
        uint32_t val;
    } core0_peri_backup_int_map;
    union {
        struct {
            uint32_t core0_dma_extmem_reject_int_map: 5;
            uint32_t reserved5:                      27;
        };
        uint32_t val;
    } core0_dma_extmem_reject_int_map;
    uint32_t core0_intr_status_0;                                             /**/
    uint32_t core0_intr_status_1;                                             /**/
    uint32_t core0_intr_status_2;                                             /**/
    uint32_t core0_intr_status_3;                                             /**/
    union {
        struct {
            uint32_t core0_clk_en: 1;
            uint32_t reserved1:   31;
        };
        uint32_t val;
    } core0_clock_gate;
    uint32_t reserved_1a0;
    uint32_t reserved_1a4;
    uint32_t reserved_1a8;
    uint32_t reserved_1ac;
    uint32_t reserved_1b0;
    uint32_t reserved_1b4;
    uint32_t reserved_1b8;
    uint32_t reserved_1bc;
    uint32_t reserved_1c0;
    uint32_t reserved_1c4;
    uint32_t reserved_1c8;
    uint32_t reserved_1cc;
    uint32_t reserved_1d0;
    uint32_t reserved_1d4;
    uint32_t reserved_1d8;
    uint32_t reserved_1dc;
    uint32_t reserved_1e0;
    uint32_t reserved_1e4;
    uint32_t reserved_1e8;
    uint32_t reserved_1ec;
    uint32_t reserved_1f0;
    uint32_t reserved_1f4;
    uint32_t reserved_1f8;
    uint32_t reserved_1fc;
    uint32_t reserved_200;
    uint32_t reserved_204;
    uint32_t reserved_208;
    uint32_t reserved_20c;
    uint32_t reserved_210;
    uint32_t reserved_214;
    uint32_t reserved_218;
    uint32_t reserved_21c;
    uint32_t reserved_220;
    uint32_t reserved_224;
    uint32_t reserved_228;
    uint32_t reserved_22c;
    uint32_t reserved_230;
    uint32_t reserved_234;
    uint32_t reserved_238;
    uint32_t reserved_23c;
    uint32_t reserved_240;
    uint32_t reserved_244;
    uint32_t reserved_248;
    uint32_t reserved_24c;
    uint32_t reserved_250;
    uint32_t reserved_254;
    uint32_t reserved_258;
    uint32_t reserved_25c;
    uint32_t reserved_260;
    uint32_t reserved_264;
    uint32_t reserved_268;
    uint32_t reserved_26c;
    uint32_t reserved_270;
    uint32_t reserved_274;
    uint32_t reserved_278;
    uint32_t reserved_27c;
    uint32_t reserved_280;
    uint32_t reserved_284;
    uint32_t reserved_288;
    uint32_t reserved_28c;
    uint32_t reserved_290;
    uint32_t reserved_294;
    uint32_t reserved_298;
    uint32_t reserved_29c;
    uint32_t reserved_2a0;
    uint32_t reserved_2a4;
    uint32_t reserved_2a8;
    uint32_t reserved_2ac;
    uint32_t reserved_2b0;
    uint32_t reserved_2b4;
    uint32_t reserved_2b8;
    uint32_t reserved_2bc;
    uint32_t reserved_2c0;
    uint32_t reserved_2c4;
    uint32_t reserved_2c8;
    uint32_t reserved_2cc;
    uint32_t reserved_2d0;
    uint32_t reserved_2d4;
    uint32_t reserved_2d8;
    uint32_t reserved_2dc;
    uint32_t reserved_2e0;
    uint32_t reserved_2e4;
    uint32_t reserved_2e8;
    uint32_t reserved_2ec;
    uint32_t reserved_2f0;
    uint32_t reserved_2f4;
    uint32_t reserved_2f8;
    uint32_t reserved_2fc;
    uint32_t reserved_300;
    uint32_t reserved_304;
    uint32_t reserved_308;
    uint32_t reserved_30c;
    uint32_t reserved_310;
    uint32_t reserved_314;
    uint32_t reserved_318;
    uint32_t reserved_31c;
    uint32_t reserved_320;
    uint32_t reserved_324;
    uint32_t reserved_328;
    uint32_t reserved_32c;
    uint32_t reserved_330;
    uint32_t reserved_334;
    uint32_t reserved_338;
    uint32_t reserved_33c;
    uint32_t reserved_340;
    uint32_t reserved_344;
    uint32_t reserved_348;
    uint32_t reserved_34c;
    uint32_t reserved_350;
    uint32_t reserved_354;
    uint32_t reserved_358;
    uint32_t reserved_35c;
    uint32_t reserved_360;
    uint32_t reserved_364;
    uint32_t reserved_368;
    uint32_t reserved_36c;
    uint32_t reserved_370;
    uint32_t reserved_374;
    uint32_t reserved_378;
    uint32_t reserved_37c;
    uint32_t reserved_380;
    uint32_t reserved_384;
    uint32_t reserved_388;
    uint32_t reserved_38c;
    uint32_t reserved_390;
    uint32_t reserved_394;
    uint32_t reserved_398;
    uint32_t reserved_39c;
    uint32_t reserved_3a0;
    uint32_t reserved_3a4;
    uint32_t reserved_3a8;
    uint32_t reserved_3ac;
    uint32_t reserved_3b0;
    uint32_t reserved_3b4;
    uint32_t reserved_3b8;
    uint32_t reserved_3bc;
    uint32_t reserved_3c0;
    uint32_t reserved_3c4;
    uint32_t reserved_3c8;
    uint32_t reserved_3cc;
    uint32_t reserved_3d0;
    uint32_t reserved_3d4;
    uint32_t reserved_3d8;
    uint32_t reserved_3dc;
    uint32_t reserved_3e0;
    uint32_t reserved_3e4;
    uint32_t reserved_3e8;
    uint32_t reserved_3ec;
    uint32_t reserved_3f0;
    uint32_t reserved_3f4;
    uint32_t reserved_3f8;
    uint32_t reserved_3fc;
    uint32_t reserved_400;
    uint32_t reserved_404;
    uint32_t reserved_408;
    uint32_t reserved_40c;
    uint32_t reserved_410;
    uint32_t reserved_414;
    uint32_t reserved_418;
    uint32_t reserved_41c;
    uint32_t reserved_420;
    uint32_t reserved_424;
    uint32_t reserved_428;
    uint32_t reserved_42c;
    uint32_t reserved_430;
    uint32_t reserved_434;
    uint32_t reserved_438;
    uint32_t reserved_43c;
    uint32_t reserved_440;
    uint32_t reserved_444;
    uint32_t reserved_448;
    uint32_t reserved_44c;
    uint32_t reserved_450;
    uint32_t reserved_454;
    uint32_t reserved_458;
    uint32_t reserved_45c;
    uint32_t reserved_460;
    uint32_t reserved_464;
    uint32_t reserved_468;
    uint32_t reserved_46c;
    uint32_t reserved_470;
    uint32_t reserved_474;
    uint32_t reserved_478;
    uint32_t reserved_47c;
    uint32_t reserved_480;
    uint32_t reserved_484;
    uint32_t reserved_488;
    uint32_t reserved_48c;
    uint32_t reserved_490;
    uint32_t reserved_494;
    uint32_t reserved_498;
    uint32_t reserved_49c;
    uint32_t reserved_4a0;
    uint32_t reserved_4a4;
    uint32_t reserved_4a8;
    uint32_t reserved_4ac;
    uint32_t reserved_4b0;
    uint32_t reserved_4b4;
    uint32_t reserved_4b8;
    uint32_t reserved_4bc;
    uint32_t reserved_4c0;
    uint32_t reserved_4c4;
    uint32_t reserved_4c8;
    uint32_t reserved_4cc;
    uint32_t reserved_4d0;
    uint32_t reserved_4d4;
    uint32_t reserved_4d8;
    uint32_t reserved_4dc;
    uint32_t reserved_4e0;
    uint32_t reserved_4e4;
    uint32_t reserved_4e8;
    uint32_t reserved_4ec;
    uint32_t reserved_4f0;
    uint32_t reserved_4f4;
    uint32_t reserved_4f8;
    uint32_t reserved_4fc;
    uint32_t reserved_500;
    uint32_t reserved_504;
    uint32_t reserved_508;
    uint32_t reserved_50c;
    uint32_t reserved_510;
    uint32_t reserved_514;
    uint32_t reserved_518;
    uint32_t reserved_51c;
    uint32_t reserved_520;
    uint32_t reserved_524;
    uint32_t reserved_528;
    uint32_t reserved_52c;
    uint32_t reserved_530;
    uint32_t reserved_534;
    uint32_t reserved_538;
    uint32_t reserved_53c;
    uint32_t reserved_540;
    uint32_t reserved_544;
    uint32_t reserved_548;
    uint32_t reserved_54c;
    uint32_t reserved_550;
    uint32_t reserved_554;
    uint32_t reserved_558;
    uint32_t reserved_55c;
    uint32_t reserved_560;
    uint32_t reserved_564;
    uint32_t reserved_568;
    uint32_t reserved_56c;
    uint32_t reserved_570;
    uint32_t reserved_574;
    uint32_t reserved_578;
    uint32_t reserved_57c;
    uint32_t reserved_580;
    uint32_t reserved_584;
    uint32_t reserved_588;
    uint32_t reserved_58c;
    uint32_t reserved_590;
    uint32_t reserved_594;
    uint32_t reserved_598;
    uint32_t reserved_59c;
    uint32_t reserved_5a0;
    uint32_t reserved_5a4;
    uint32_t reserved_5a8;
    uint32_t reserved_5ac;
    uint32_t reserved_5b0;
    uint32_t reserved_5b4;
    uint32_t reserved_5b8;
    uint32_t reserved_5bc;
    uint32_t reserved_5c0;
    uint32_t reserved_5c4;
    uint32_t reserved_5c8;
    uint32_t reserved_5cc;
    uint32_t reserved_5d0;
    uint32_t reserved_5d4;
    uint32_t reserved_5d8;
    uint32_t reserved_5dc;
    uint32_t reserved_5e0;
    uint32_t reserved_5e4;
    uint32_t reserved_5e8;
    uint32_t reserved_5ec;
    uint32_t reserved_5f0;
    uint32_t reserved_5f4;
    uint32_t reserved_5f8;
    uint32_t reserved_5fc;
    uint32_t reserved_600;
    uint32_t reserved_604;
    uint32_t reserved_608;
    uint32_t reserved_60c;
    uint32_t reserved_610;
    uint32_t reserved_614;
    uint32_t reserved_618;
    uint32_t reserved_61c;
    uint32_t reserved_620;
    uint32_t reserved_624;
    uint32_t reserved_628;
    uint32_t reserved_62c;
    uint32_t reserved_630;
    uint32_t reserved_634;
    uint32_t reserved_638;
    uint32_t reserved_63c;
    uint32_t reserved_640;
    uint32_t reserved_644;
    uint32_t reserved_648;
    uint32_t reserved_64c;
    uint32_t reserved_650;
    uint32_t reserved_654;
    uint32_t reserved_658;
    uint32_t reserved_65c;
    uint32_t reserved_660;
    uint32_t reserved_664;
    uint32_t reserved_668;
    uint32_t reserved_66c;
    uint32_t reserved_670;
    uint32_t reserved_674;
    uint32_t reserved_678;
    uint32_t reserved_67c;
    uint32_t reserved_680;
    uint32_t reserved_684;
    uint32_t reserved_688;
    uint32_t reserved_68c;
    uint32_t reserved_690;
    uint32_t reserved_694;
    uint32_t reserved_698;
    uint32_t reserved_69c;
    uint32_t reserved_6a0;
    uint32_t reserved_6a4;
    uint32_t reserved_6a8;
    uint32_t reserved_6ac;
    uint32_t reserved_6b0;
    uint32_t reserved_6b4;
    uint32_t reserved_6b8;
    uint32_t reserved_6bc;
    uint32_t reserved_6c0;
    uint32_t reserved_6c4;
    uint32_t reserved_6c8;
    uint32_t reserved_6cc;
    uint32_t reserved_6d0;
    uint32_t reserved_6d4;
    uint32_t reserved_6d8;
    uint32_t reserved_6dc;
    uint32_t reserved_6e0;
    uint32_t reserved_6e4;
    uint32_t reserved_6e8;
    uint32_t reserved_6ec;
    uint32_t reserved_6f0;
    uint32_t reserved_6f4;
    uint32_t reserved_6f8;
    uint32_t reserved_6fc;
    uint32_t reserved_700;
    uint32_t reserved_704;
    uint32_t reserved_708;
    uint32_t reserved_70c;
    uint32_t reserved_710;
    uint32_t reserved_714;
    uint32_t reserved_718;
    uint32_t reserved_71c;
    uint32_t reserved_720;
    uint32_t reserved_724;
    uint32_t reserved_728;
    uint32_t reserved_72c;
    uint32_t reserved_730;
    uint32_t reserved_734;
    uint32_t reserved_738;
    uint32_t reserved_73c;
    uint32_t reserved_740;
    uint32_t reserved_744;
    uint32_t reserved_748;
    uint32_t reserved_74c;
    uint32_t reserved_750;
    uint32_t reserved_754;
    uint32_t reserved_758;
    uint32_t reserved_75c;
    uint32_t reserved_760;
    uint32_t reserved_764;
    uint32_t reserved_768;
    uint32_t reserved_76c;
    uint32_t reserved_770;
    uint32_t reserved_774;
    uint32_t reserved_778;
    uint32_t reserved_77c;
    uint32_t reserved_780;
    uint32_t reserved_784;
    uint32_t reserved_788;
    uint32_t reserved_78c;
    uint32_t reserved_790;
    uint32_t reserved_794;
    uint32_t reserved_798;
    uint32_t reserved_79c;
    uint32_t reserved_7a0;
    uint32_t reserved_7a4;
    uint32_t reserved_7a8;
    uint32_t reserved_7ac;
    uint32_t reserved_7b0;
    uint32_t reserved_7b4;
    uint32_t reserved_7b8;
    uint32_t reserved_7bc;
    uint32_t reserved_7c0;
    uint32_t reserved_7c4;
    uint32_t reserved_7c8;
    uint32_t reserved_7cc;
    uint32_t reserved_7d0;
    uint32_t reserved_7d4;
    uint32_t reserved_7d8;
    uint32_t reserved_7dc;
    uint32_t reserved_7e0;
    uint32_t reserved_7e4;
    uint32_t reserved_7e8;
    uint32_t reserved_7ec;
    uint32_t reserved_7f0;
    uint32_t reserved_7f4;
    uint32_t reserved_7f8;
    union {
        struct {
            uint32_t core0_interrupt_date:28;
            uint32_t reserved28:           4;
        };
        uint32_t val;
    } core0_interrupt_date;
} interrupt_core0_dev_t;
extern interrupt_core0_dev_t INTERRUPT_CORE0;
#ifdef __cplusplus
}
#endif

#endif  /* _SOC_INTERRUPT_CORE0_STRUCT_H_ */
