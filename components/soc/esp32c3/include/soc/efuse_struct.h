// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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
#ifndef _SOC_EFUSE_STRUCT_H_
#define _SOC_EFUSE_STRUCT_H_
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct efuse_dev_s {
    uint32_t pgm_data0;                                          /*Register 0 that stores data to be programmed.*/
    union {
        struct {
            uint32_t rd_dis:                      7;             /*Set this bit to disable reading from BlOCK4-10.*/
            uint32_t rpt4_reserved5:              1;             /*Reserved*/
            uint32_t dis_icache:                  1;             /*Set this bit to disable Icache.*/
            uint32_t dis_usb_jtag:                1;             /*Set this bit to disable function of usb switch to jtag in module of usb device.*/
            uint32_t dis_download_icache:         1;             /*Set this bit to disable Icache in download mode (boot_mode[3:0] is 0  1  2  3  6  7).*/
            uint32_t dis_usb_device:              1;             /*Set this bit to disable usb device.*/
            uint32_t dis_force_download:          1;             /*Set this bit to disable the function that forces chip into download mode.*/
            uint32_t dis_usb:                     1;             /*Set this bit to disable USB function.*/
            uint32_t dis_can:                     1;             /*Set this bit to disable CAN function.*/
            uint32_t jtag_sel_enable:             1;             /*Set this bit to enable selection between usb_to_jtag and pad_to_jtag through strapping gpio10 when both reg_dis_usb_jtag and reg_dis_pad_jtag are equal to 0.*/
            uint32_t soft_dis_jtag:               3;             /*Set these bits to disable JTAG in the soft way (odd number 1 means disable ). JTAG can be enabled in HMAC module.*/
            uint32_t dis_pad_jtag:                1;             /*Set this bit to disable JTAG in the hard way. JTAG is disabled permanently.*/
            uint32_t dis_download_manual_encrypt: 1;             /*Set this bit to disable flash encryption when in download boot modes.*/
            uint32_t usb_drefh:                   2;             /*Controls single-end input threshold vrefh  1.76 V to 2 V with step of 80 mV  stored in eFuse.*/
            uint32_t usb_drefl:                   2;             /*Controls single-end input threshold vrefl  0.8 V to 1.04 V with step of 80 mV  stored in eFuse.*/
            uint32_t usb_exchg_pins:              1;             /*Set this bit to exchange USB D+ and D- pins.*/
            uint32_t vdd_spi_as_gpio:             1;             /*Set this bit to vdd spi pin function as gpio.*/
            uint32_t btlc_gpio_enable:            2;             /*Enable btlc gpio.*/
            uint32_t powerglitch_en:              1;             /*Set this bit to enable power glitch function.*/
            uint32_t power_glitch_dsense:         2;             /*Sample delay configuration of power glitch.*/
        };
        uint32_t val;
    } pgm_data1;
    union {
        struct {
            uint32_t rpt4_reserved2:         16;                 /*Reserved (used for four backups method).*/
            uint32_t wat_delay_sel:           2;                 /*Selects RTC watchdog timeout threshold  in unit of slow clock cycle. 0: 40000. 1: 80000. 2: 160000. 3:320000.*/
            uint32_t spi_boot_crypt_cnt:      3;                 /*Set this bit to enable SPI boot encrypt/decrypt. Odd number of 1: enable. even number of 1: disable.*/
            uint32_t secure_boot_key_revoke0: 1;                 /*Set this bit to enable revoking first secure boot key.*/
            uint32_t secure_boot_key_revoke1: 1;                 /*Set this bit to enable revoking second secure boot key.*/
            uint32_t secure_boot_key_revoke2: 1;                 /*Set this bit to enable revoking third secure boot key.*/
            uint32_t key_purpose_0:           4;                 /*Purpose of Key0.*/
            uint32_t key_purpose_1:           4;                 /*Purpose of Key1.*/
        };
        uint32_t val;
    } pgm_data2;
    union {
        struct {
            uint32_t key_purpose_2:                 4;           /*Purpose of Key2.*/
            uint32_t key_purpose_3:                 4;           /*Purpose of Key3.*/
            uint32_t key_purpose_4:                 4;           /*Purpose of Key4.*/
            uint32_t key_purpose_5:                 4;           /*Purpose of Key5.*/
            uint32_t rpt4_reserved3:                4;           /*Reserved (used for four backups method).*/
            uint32_t secure_boot_en:                1;           /*Set this bit to enable secure boot.*/
            uint32_t secure_boot_aggressive_revoke: 1;           /*Set this bit to enable revoking aggressive secure boot.*/
            uint32_t rpt4_reserved0:                6;           /*Reserved (used for four backups method).*/
            uint32_t flash_tpuw:                    4;           /*Configures flash waiting time after power-up  in unit of ms. If the value is less than 15  the waiting time is the configurable value*/
        };
        uint32_t val;
    } pgm_data3;
    union {
        struct {
            uint32_t dis_download_mode:                 1;                /*Set this bit to disable download mode (boot_mode[3:0] = 0  1  2  3  6  7).*/
            uint32_t dis_direct_boot:                   1;                /*Set this bit to disable direct boot.*/
            uint32_t dis_usb_serial_jtag_rom_print:     1;                /*Set this bit to disable USB-Serial-JTAG print during rom boot*/
            uint32_t rpt4_reserved8:                    1;                /*Reserved (used for four backups method).*/
            uint32_t dis_usb_serial_jtag_download_mode: 1;                /*Set this bit to disable download mode through USB-Serial-JTAG.*/
            uint32_t enable_security_download:          1;                /*Set this bit to enable secure UART download mode.*/
            uint32_t uart_print_control:                2;                /*Set the default UARTboot message output mode. 00: Enabled. 01: Enabled when GPIO8 is low at reset. 10: Enabled when GPIO8 is high at reset. 11:disabled.*/
            uint32_t rpt4_reserved7:                    5;                /*Reserved (used for four backups method).*/
            uint32_t force_send_resume:                 1;                /*Set this bit to force ROM code to send a resume command during SPI boot.*/
            uint32_t secure_version:                    16;                /*Secure version (used by ESP-IDF anti-rollback feature).*/
            uint32_t rpt4_reserved1:                    1;                /*Reserved (used for four backups method).*/
            uint32_t err_rst_enable:                    1;                /*Use BLOCK0 to check error record registers, 0 - without check.*/
        };
        uint32_t val;
    } pgm_data4;
    union {
        struct {
            uint32_t rpt4_reserved4:24;                          /*Reserved (used for four backups method).*/
            uint32_t reserved24:     8;                          /*Reserved.*/
        };
        uint32_t val;
    } pgm_data5;
    uint32_t pgm_data6;                                          /*Register 6 that stores data to be programmed.*/
    uint32_t pgm_data7;                                          /*Register 7 that stores data to be programmed.*/
    uint32_t pgm_check_value0;                                   /*Register 0 that stores the RS code to be programmed.*/
    uint32_t pgm_check_value1;                                   /*Register 1 that stores the RS code to be programmed.*/
    uint32_t pgm_check_value2;                                   /*Register 2 that stores the RS code to be programmed.*/
    uint32_t rd_wr_dis;                                          /*BLOCK0 data register $n.*/
    union {
        struct {
            uint32_t rd_dis:                      7;             /*The value of RD_DIS.*/
            uint32_t rpt4_reserved5:              1;             /*Reserved*/
            uint32_t dis_icache:                  1;             /*The value of DIS_ICACHE.*/
            uint32_t dis_usb_jtag:                1;             /*The value of DIS_USB_JTAG.*/
            uint32_t dis_download_icache:         1;             /*The value of DIS_DOWNLOAD_ICACHE.*/
            uint32_t dis_usb_device:              1;             /*The value of DIS_USB_DEVICE.*/
            uint32_t dis_force_download:          1;             /*The value of DIS_FORCE_DOWNLOAD.*/
            uint32_t dis_usb:                     1;             /*The value of DIS_USB.*/
            uint32_t dis_can:                     1;             /*The value of DIS_CAN.*/
            uint32_t jtag_sel_enable:             1;             /*The value of JTAG_SEL_ENABLE.*/
            uint32_t soft_dis_jtag:               3;             /*The value of SOFT_DIS_JTAG.*/
            uint32_t dis_pad_jtag:                1;             /*The value of DIS_PAD_JTAG.*/
            uint32_t dis_download_manual_encrypt: 1;             /*The value of DIS_DOWNLOAD_MANUAL_ENCRYPT.*/
            uint32_t usb_drefh:                   2;             /*The value of USB_DREFH.*/
            uint32_t usb_drefl:                   2;             /*The value of USB_DREFL.*/
            uint32_t usb_exchg_pins:              1;             /*The value of USB_EXCHG_PINS.*/
            uint32_t vdd_spi_as_gpio:             1;             /*The value of VDD_SPI_AS_GPIO.*/
            uint32_t btlc_gpio_enable:            2;             /*The value of BTLC_GPIO_ENABLE.*/
            uint32_t powerglitch_en:              1;             /*The value of POWERGLITCH_EN.*/
            uint32_t power_glitch_dsense:         2;             /*The value of POWER_GLITCH_DSENSE.*/
        };
        uint32_t val;
    } rd_repeat_data0;
    union {
        struct {
            uint32_t rpt4_reserved2:         16;                 /*Reserved.*/
            uint32_t wdt_delay_sel:           2;                 /*The value of WDT_DELAY_SEL.*/
            uint32_t spi_boot_crypt_cnt:      3;                 /*The value of SPI_BOOT_CRYPT_CNT.*/
            uint32_t secure_boot_key_revoke0: 1;                 /*The value of SECURE_BOOT_KEY_REVOKE0.*/
            uint32_t secure_boot_key_revoke1: 1;                 /*The value of SECURE_BOOT_KEY_REVOKE1.*/
            uint32_t secure_boot_key_revoke2: 1;                 /*The value of SECURE_BOOT_KEY_REVOKE2.*/
            uint32_t key_purpose_0:           4;                 /*The value of KEY_PURPOSE_0.*/
            uint32_t key_purpose_1:           4;                 /*The value of KEY_PURPOSE_1.*/
        };
        uint32_t val;
    } rd_repeat_data1;
    union {
        struct {
            uint32_t key_purpose_2:                 4;           /*The value of KEY_PURPOSE_2.*/
            uint32_t key_purpose_3:                 4;           /*The value of KEY_PURPOSE_3.*/
            uint32_t key_purpose_4:                 4;           /*The value of KEY_PURPOSE_4.*/
            uint32_t key_purpose_5:                 4;           /*The value of KEY_PURPOSE_5.*/
            uint32_t rpt4_reserved3:                4;           /*Reserved.*/
            uint32_t secure_boot_en:                1;           /*The value of SECURE_BOOT_EN.*/
            uint32_t secure_boot_aggressive_revoke: 1;           /*The value of SECURE_BOOT_AGGRESSIVE_REVOKE.*/
            uint32_t rpt4_reserved0:                6;           /*Reserved.*/
            uint32_t flash_tpuw:                    4;           /*The value of FLASH_TPUW.*/
        };
        uint32_t val;
    } rd_repeat_data2;
    union {
        struct {
            uint32_t dis_download_mode:        1;                /*The value of DIS_DOWNLOAD_MODE.*/
            uint32_t dis_direct_boot:          1;                /*The value of DIS_DIRECT_BOOT.*/
            uint32_t dis_usb_serial_jtag_rom_print:1;                /*The value of DIS_USB_SERIAL_JTAG_ROM_PRINT.*/
            uint32_t rpt4_reserved8:           1;                /*Reserved.*/
            uint32_t dis_usb_serial_jtag_download_mode:    1;    /*The value of dis_usb_serial_jtag_download_mode.*/
            uint32_t enable_security_download: 1;                /*The value of ENABLE_SECURITY_DOWNLOAD.*/
            uint32_t uart_print_control:       2;                /*The value of UART_PRINT_CONTROL.*/
            uint32_t rpt4_reserved7:           5;                /*Reserved.*/
            uint32_t force_send_resume:        1;                /*The value of FORCE_SEND_RESUME.*/
            uint32_t secure_version:          16;                /*The value of SECURE_VERSION.*/
            uint32_t rpt4_reserved1:           1;                /*Reserved.*/
            uint32_t err_rst_enable:           1;                /*Use BLOCK0 to check error record registers, 0 - without check.*/
        };
        uint32_t val;
    } rd_repeat_data3;
    union {
        struct {
            uint32_t rpt4_reserved4:24;                          /*Reserved.*/
            uint32_t reserved24:     8;                          /*Reserved.*/
        };
        uint32_t val;
    } rd_repeat_data4;
    uint32_t rd_mac_spi_sys_0;                                   /*BLOCK1 data register $n.*/
    union {
        struct {
            uint32_t mac_1:         16;                          /*Stores the high 16 bits of MAC address.*/
            uint32_t spi_pad_conf_0:16;                          /*Stores the zeroth part of SPI_PAD_CONF.*/
        };
        uint32_t val;
    } rd_mac_spi_sys_1;
    uint32_t rd_mac_spi_sys_2;                                   /*BLOCK1 data register $n.*/
    union {
        struct {
            uint32_t spi_pad_conf_2:  18;                        /*Stores the second part of SPI_PAD_CONF.*/
            uint32_t sys_data_part0_0:14;                        /*Stores the fist 14 bits of the zeroth part of system data.*/
        };
        uint32_t val;
    } rd_mac_spi_sys_3;
    uint32_t rd_mac_spi_sys_4;                                   /*BLOCK1 data register $n.*/
    uint32_t rd_mac_spi_sys_5;                                   /*BLOCK1 data register $n.*/
    uint32_t rd_sys_part1_data0;                                 /*Register $n of BLOCK2 (system).*/
    uint32_t rd_sys_part1_data1;                                 /*Register $n of BLOCK2 (system).*/
    uint32_t rd_sys_part1_data2;                                 /*Register $n of BLOCK2 (system).*/
    uint32_t rd_sys_part1_data3;                                 /*Register $n of BLOCK2 (system).*/
    uint32_t rd_sys_part1_data4;                                 /*Register $n of BLOCK2 (system).*/
    uint32_t rd_sys_part1_data5;                                 /*Register $n of BLOCK2 (system).*/
    uint32_t rd_sys_part1_data6;                                 /*Register $n of BLOCK2 (system).*/
    uint32_t rd_sys_part1_data7;                                 /*Register $n of BLOCK2 (system).*/
    uint32_t rd_usr_data0;                                       /*Register $n of BLOCK3 (user).*/
    uint32_t rd_usr_data1;                                       /*Register $n of BLOCK3 (user).*/
    uint32_t rd_usr_data2;                                       /*Register $n of BLOCK3 (user).*/
    uint32_t rd_usr_data3;                                       /*Register $n of BLOCK3 (user).*/
    uint32_t rd_usr_data4;                                       /*Register $n of BLOCK3 (user).*/
    uint32_t rd_usr_data5;                                       /*Register $n of BLOCK3 (user).*/
    uint32_t rd_usr_data6;                                       /*Register $n of BLOCK3 (user).*/
    uint32_t rd_usr_data7;                                       /*Register $n of BLOCK3 (user).*/
    uint32_t rd_key0_data0;                                      /*Register $n of BLOCK4 (KEY0).*/
    uint32_t rd_key0_data1;                                      /*Register $n of BLOCK4 (KEY0).*/
    uint32_t rd_key0_data2;                                      /*Register $n of BLOCK4 (KEY0).*/
    uint32_t rd_key0_data3;                                      /*Register $n of BLOCK4 (KEY0).*/
    uint32_t rd_key0_data4;                                      /*Register $n of BLOCK4 (KEY0).*/
    uint32_t rd_key0_data5;                                      /*Register $n of BLOCK4 (KEY0).*/
    uint32_t rd_key0_data6;                                      /*Register $n of BLOCK4 (KEY0).*/
    uint32_t rd_key0_data7;                                      /*Register $n of BLOCK4 (KEY0).*/
    uint32_t rd_key1_data0;                                      /*Register $n of BLOCK5 (KEY1).*/
    uint32_t rd_key1_data1;                                      /*Register $n of BLOCK5 (KEY1).*/
    uint32_t rd_key1_data2;                                      /*Register $n of BLOCK5 (KEY1).*/
    uint32_t rd_key1_data3;                                      /*Register $n of BLOCK5 (KEY1).*/
    uint32_t rd_key1_data4;                                      /*Register $n of BLOCK5 (KEY1).*/
    uint32_t rd_key1_data5;                                      /*Register $n of BLOCK5 (KEY1).*/
    uint32_t rd_key1_data6;                                      /*Register $n of BLOCK5 (KEY1).*/
    uint32_t rd_key1_data7;                                      /*Register $n of BLOCK5 (KEY1).*/
    uint32_t rd_key2_data0;                                      /*Register $n of BLOCK6 (KEY2).*/
    uint32_t rd_key2_data1;                                      /*Register $n of BLOCK6 (KEY2).*/
    uint32_t rd_key2_data2;                                      /*Register $n of BLOCK6 (KEY2).*/
    uint32_t rd_key2_data3;                                      /*Register $n of BLOCK6 (KEY2).*/
    uint32_t rd_key2_data4;                                      /*Register $n of BLOCK6 (KEY2).*/
    uint32_t rd_key2_data5;                                      /*Register $n of BLOCK6 (KEY2).*/
    uint32_t rd_key2_data6;                                      /*Register $n of BLOCK6 (KEY2).*/
    uint32_t rd_key2_data7;                                      /*Register $n of BLOCK6 (KEY2).*/
    uint32_t rd_key3_data0;                                      /*Register $n of BLOCK7 (KEY3).*/
    uint32_t rd_key3_data1;                                      /*Register $n of BLOCK7 (KEY3).*/
    uint32_t rd_key3_data2;                                      /*Register $n of BLOCK7 (KEY3).*/
    uint32_t rd_key3_data3;                                      /*Register $n of BLOCK7 (KEY3).*/
    uint32_t rd_key3_data4;                                      /*Register $n of BLOCK7 (KEY3).*/
    uint32_t rd_key3_data5;                                      /*Register $n of BLOCK7 (KEY3).*/
    uint32_t rd_key3_data6;                                      /*Register $n of BLOCK7 (KEY3).*/
    uint32_t rd_key3_data7;                                      /*Register $n of BLOCK7 (KEY3).*/
    uint32_t rd_key4_data0;                                      /*Register $n of BLOCK8 (KEY4).*/
    uint32_t rd_key4_data1;                                      /*Register $n of BLOCK8 (KEY4).*/
    uint32_t rd_key4_data2;                                      /*Register $n of BLOCK8 (KEY4).*/
    uint32_t rd_key4_data3;                                      /*Register $n of BLOCK8 (KEY4).*/
    uint32_t rd_key4_data4;                                      /*Register $n of BLOCK8 (KEY4).*/
    uint32_t rd_key4_data5;                                      /*Register $n of BLOCK8 (KEY4).*/
    uint32_t rd_key4_data6;                                      /*Register $n of BLOCK8 (KEY4).*/
    uint32_t rd_key4_data7;                                      /*Register $n of BLOCK8 (KEY4).*/
    uint32_t rd_key5_data0;                                      /*Register $n of BLOCK9 (KEY5).*/
    uint32_t rd_key5_data1;                                      /*Register $n of BLOCK9 (KEY5).*/
    uint32_t rd_key5_data2;                                      /*Register $n of BLOCK9 (KEY5).*/
    uint32_t rd_key5_data3;                                      /*Register $n of BLOCK9 (KEY5).*/
    uint32_t rd_key5_data4;                                      /*Register $n of BLOCK9 (KEY5).*/
    uint32_t rd_key5_data5;                                      /*Register $n of BLOCK9 (KEY5).*/
    uint32_t rd_key5_data6;                                      /*Register $n of BLOCK9 (KEY5).*/
    uint32_t rd_key5_data7;                                      /*Register $n of BLOCK9 (KEY5).*/
    uint32_t rd_sys_part2_data0;                                 /*Register $n of BLOCK10 (system).*/
    uint32_t rd_sys_part2_data1;                                 /*Register $n of BLOCK9 (KEY5).*/
    uint32_t rd_sys_part2_data2;                                 /*Register $n of BLOCK10 (system).*/
    uint32_t rd_sys_part2_data3;                                 /*Register $n of BLOCK10 (system).*/
    uint32_t rd_sys_part2_data4;                                 /*Register $n of BLOCK10 (system).*/
    uint32_t rd_sys_part2_data5;                                 /*Register $n of BLOCK10 (system).*/
    uint32_t rd_sys_part2_data6;                                 /*Register $n of BLOCK10 (system).*/
    uint32_t rd_sys_part2_data7;                                 /*Register $n of BLOCK10 (system).*/
    union {
        struct {
            uint32_t rd_dis_err:                      7;         /*If any bit in RD_DIS is 1  then it indicates a programming error.*/
            uint32_t rpt4_reserved5_err:              1;         /*Reserved.*/
            uint32_t dis_icache_err:                  1;         /*If DIS_ICACHE is 1  then it indicates a programming error.*/
            uint32_t dis_usb_jtag_err:                1;         /*If DIS_USB_JTAG is 1  then it indicates a programming error.*/
            uint32_t dis_download_icache:             1;         /*If DIS_DOWNLOAD_ICACHE is 1  then it indicates a programming error.*/
            uint32_t dis_usb_device_err:              1;         /*If DIS_USB_DEVICE is 1  then it indicates a programming error.*/
            uint32_t dis_force_download_err:          1;         /*If DIS_FORCE_DOWNLOAD is 1  then it indicates a programming error.*/
            uint32_t dis_usb_err:                     1;         /*If DIS_USB is 1  then it indicates a programming error.*/
            uint32_t dis_can_err:                     1;         /*If DIS_CAN is 1  then it indicates a programming error.*/
            uint32_t jtag_sel_enable_err:             1;         /*If JTAG_SEL_ENABLE is 1  then it indicates a programming error.*/
            uint32_t soft_dis_jtag_err:               3;         /*If SOFT_DIS_JTAG is 1  then it indicates a programming error.*/
            uint32_t dis_pad_jtag_err:                1;         /*If DIS_PAD_JTAG is 1  then it indicates a programming error.*/
            uint32_t dis_download_manual_encrypt_err: 1;         /*If DIS_DOWNLOAD_MANUAL_ENCRYPT is 1  then it indicates a programming error.*/
            uint32_t usb_drefh_err:                   2;         /*If any bit in USB_DREFH is 1  then it indicates a programming error.*/
            uint32_t usb_drefl_err:                   2;         /*If any bit in USB_DREFL is 1  then it indicates a programming error.*/
            uint32_t usb_exchg_pins_err:              1;         /*If USB_EXCHG_PINS is 1  then it indicates a programming error.*/
            uint32_t vdd_spi_as_gpio_err:             1;         /*If VDD_SPI_AS_GPIO is 1  then it indicates a programming error.*/
            uint32_t btlc_gpio_enable_err:            2;         /*If any bit in BTLC_GPIO_ENABLE is 1  then it indicates a programming error.*/
            uint32_t powerglitch_en_err:              1;         /*If POWERGLITCH_EN is 1  then it indicates a programming error.*/
            uint32_t power_glitch_dsense_err:         2;         /*If any bit in POWER_GLITCH_DSENSE is 1  then it indicates a programming error.*/
        };
        uint32_t val;
    } rd_repeat_err0;
    union {
        struct {
            uint32_t rpt4_reserved2_err:         16;             /*Reserved.*/
            uint32_t wdt_delay_sel_err:           2;             /*If any bit in WDT_DELAY_SEL is 1  then it indicates a programming error.*/
            uint32_t spi_boot_crypt_cnt_err:      3;             /*If any bit in SPI_BOOT_CRYPT_CNT is 1  then it indicates a programming error.*/
            uint32_t secure_boot_key_revoke0_err: 1;             /*If SECURE_BOOT_KEY_REVOKE0 is 1  then it indicates a programming error.*/
            uint32_t secure_boot_key_revoke1_err: 1;             /*If SECURE_BOOT_KEY_REVOKE1 is 1  then it indicates a programming error.*/
            uint32_t secure_boot_key_revoke2_err: 1;             /*If SECURE_BOOT_KEY_REVOKE2 is 1  then it indicates a programming error.*/
            uint32_t key_purpose_0_err:           4;             /*If any bit in KEY_PURPOSE_0 is 1  then it indicates a programming error.*/
            uint32_t key_purpose_1_err:           4;             /*If any bit in KEY_PURPOSE_1 is 1  then it indicates a programming error.*/
        };
        uint32_t val;
    } rd_repeat_err1;
    union {
        struct {
            uint32_t key_purpose_2_err:                 4;       /*If any bit in KEY_PURPOSE_2 is 1  then it indicates a programming error.*/
            uint32_t key_purpose_3_err:                 4;       /*If any bit in KEY_PURPOSE_3 is 1  then it indicates a programming error.*/
            uint32_t key_purpose_4_err:                 4;       /*If any bit in KEY_PURPOSE_4 is 1  then it indicates a programming error.*/
            uint32_t key_purpose_5_err:                 4;       /*If any bit in KEY_PURPOSE_5 is 1  then it indicates a programming error.*/
            uint32_t rpt4_reserved3_err:                4;       /*Reserved.*/
            uint32_t secure_boot_en_err:                1;       /*If SECURE_BOOT_EN is 1  then it indicates a programming error.*/
            uint32_t secure_boot_aggressive_revoke_err: 1;       /*If SECURE_BOOT_AGGRESSIVE_REVOKE is 1  then it indicates a programming error.*/
            uint32_t rpt4_reserved0_err:                6;       /*Reserved.*/
            uint32_t flash_tpuw_err:                    4;       /*If any bit in FLASH_TPUM is 1  then it indicates a programming error.*/
        };
        uint32_t val;
    } rd_repeat_err2;
    union {
        struct {
            uint32_t dis_download_mode_err:        1;            /*If the value is not zero then it indicates a programming error on DIS_DOWNLOAD_MODE.*/
            uint32_t dis_direct_boot_err:          1;            /*If the value is not zero then it indicates a programming error on DIS_DIRECT_BOOT.*/
            uint32_t dis_usb_serial_jtag_rom_print_err:1;            /*If the value is not zero then it indicates a programming error on DIS_USB_SERIAL_JTAG_ROM_PRINT.*/
            uint32_t rpt4_reserved8_err:           1;            /*Reserved.*/
            uint32_t dis_usb_serial_jtag_download_mode_err:    1; /*If the value is not zero then it indicates a programming error on DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE.*/
            uint32_t enable_security_download_err: 1;            /*If the value is not zero then it indicates a programming error on ENABLE_SECURITY_DOWNLOAD.*/
            uint32_t uart_print_control_err:       2;            /*If the value is not zero  then it indicates a programming error on UART_PRINT_CONTROL.*/
            uint32_t rpt4_reserved7_err:           5;            /*Reserved*/
            uint32_t force_send_resume_err:        1;            /*If the value is not zero then it indicates a programming error on FORCE_SEND_RESUME.*/
            uint32_t secure_version_err:          16;            /*If the value is not zero then it indicates a programming error on SECURE_VERSION.*/
            uint32_t rpt4_reserved1_err:           1;            /*Reserved.*/
            uint32_t err_rst_enable_err:           1;            /*Use BLOCK0 to check error record registers, 0 - without check.*/
        };
        uint32_t val;
    } rd_repeat_err3;
    union {
        struct {
            uint32_t rpt4_reserved4_err:24;                      /*Reserved.*/
            uint32_t reserved24:         8;                      /*Reserved.*/
        };
        uint32_t val;
    } rd_repeat_err4;
    uint32_t reserved_190;
    uint32_t reserved_194;
    uint32_t reserved_198;
    uint32_t reserved_19c;
    uint32_t reserved_1a0;
    uint32_t reserved_1a4;
    uint32_t reserved_1a8;
    uint32_t reserved_1ac;
    uint32_t reserved_1b0;
    uint32_t reserved_1b4;
    uint32_t reserved_1b8;
    uint32_t reserved_1bc;
    union {
        struct {
            uint32_t mac_spi_8m_err_num: 3;                      /*The value of this signal means the number of error bytes.*/
            uint32_t reserved3:          1;                      /*Reserved.*/
            uint32_t sys_part1_num:      3;                      /*The value of this signal means the number of error bytes.*/
            uint32_t mac_spi_8m_fail:    1;                      /*0: Means no failure and that the data of MAC_SPI_8M is reliable 1: Means that programming MAC_SPI_8M failed and the number of error bytes is over 6.*/
            uint32_t usr_data_err_num:   3;                      /*The value of this signal means the number of error bytes.*/
            uint32_t sys_part1_fail:     1;                      /*0: Means no failure and that the data of system part1 is reliable 1: Means that programming the data of system part1 failed and the number of error bytes is over 6.*/
            uint32_t key0_err_num:       3;                      /*The value of this signal means the number of error bytes.*/
            uint32_t usr_data_fail:      1;                      /*0: Means no failure and that the data of user data is reliable 1: Means that programming user data failed and the number of error bytes is over 6.*/
            uint32_t key1_err_num:       3;                      /*The value of this signal means the number of error bytes.*/
            uint32_t key0_fail:          1;                      /*0: Means no failure and that the data of key0 is reliable 1: Means that programming key0 failed and the number of error bytes is over 6.*/
            uint32_t key2_err_num:       3;                      /*The value of this signal means the number of error bytes.*/
            uint32_t key1_fail:          1;                      /*0: Means no failure and that the data of key1 is reliable 1: Means that programming key1 failed and the number of error bytes is over 6.*/
            uint32_t key3_err_num:       3;                      /*The value of this signal means the number of error bytes.*/
            uint32_t key2_fail:          1;                      /*0: Means no failure and that the data of key2 is reliable 1: Means that programming key2 failed and the number of error bytes is over 6.*/
            uint32_t key4_err_num:       3;                      /*The value of this signal means the number of error bytes.*/
            uint32_t key3_fail:          1;                      /*0: Means no failure and that the data of key3 is reliable 1: Means that programming key3 failed and the number of error bytes is over 6.*/
        };
        uint32_t val;
    } rd_rs_err0;
    union {
        struct {
            uint32_t key5_err_num:      3;                       /*The value of this signal means the number of error bytes.*/
            uint32_t key4_fail:         1;                       /*0: Means no failure and that the data of KEY4 is reliable 1: Means that programming KEY4 failed and the number of error bytes is over 6.*/
            uint32_t sys_part2_err_num: 3;                       /*The value of this signal means the number of error bytes.*/
            uint32_t key5_fail:         1;                       /*0: Means no failure and that the data of KEY5 is reliable 1: Means that programming KEY5 failed and the number of error bytes is over 6.*/
            uint32_t reserved8:        24;                       /*Reserved.*/
        };
        uint32_t val;
    } rd_rs_err1;
    union {
        struct {
            uint32_t mem_force_pd:     1;                        /*Set this bit to force eFuse SRAM into power-saving mode.*/
            uint32_t mem_clk_force_on: 1;                        /*Set this bit and force to activate clock signal of eFuse SRAM.*/
            uint32_t mem_force_pu:     1;                        /*Set this bit to force eFuse SRAM into working mode.*/
            uint32_t reserved3:       13;                        /*Reserved.*/
            uint32_t clk_en:           1;                        /*Set this bit and force to enable clock signal of eFuse memory.*/
            uint32_t reserved17:      15;                        /*Reserved.*/
        };
        uint32_t val;
    } clk;
    union {
        struct {
            uint32_t op_code:   16;                              /*0x5A5A: Operate programming command 0x5AA5: Operate read command.*/
            uint32_t reserved16:16;                              /*Reserved.*/
        };
        uint32_t val;
    } conf;
    union {
        struct {
            uint32_t state:            4;                        /*Indicates the state of the eFuse state machine.*/
            uint32_t otp_load_sw:      1;                        /*The value of OTP_LOAD_SW.*/
            uint32_t otp_vddq_c_sync2: 1;                        /*The value of OTP_VDDQ_C_SYNC2.*/
            uint32_t otp_strobe_sw:    1;                        /*The value of OTP_STROBE_SW.*/
            uint32_t otp_csb_sw:       1;                        /*The value of OTP_CSB_SW.*/
            uint32_t otp_pgenb_sw:     1;                        /*The value of OTP_PGENB_SW.*/
            uint32_t otp_vddq_is_sw:   1;                        /*The value of OTP_VDDQ_IS_SW.*/
            uint32_t repeat_err_cnt:   8;                        /*Indicates the number of error bits during programming BLOCK0.*/
            uint32_t reserved18:      14;                        /*Reserved.*/
        };
        uint32_t val;
    } status;
    union {
        struct {
            uint32_t read_cmd:   1;                              /*Set this bit to send read command.*/
            uint32_t pgm_cmd:    1;                              /*Set this bit to send programming command.*/
            uint32_t blk_num:    4;                              /*The serial number of the block to be programmed. Value 0-10 corresponds to block number 0-10  respectively.*/
            uint32_t reserved6: 26;                              /*Reserved.*/
        };
        uint32_t val;
    } cmd;
    union {
        struct {
            uint32_t read_done:         1;                       /*The raw bit signal for read_done interrupt.*/
            uint32_t pgm_done:          1;                       /*The raw bit signal for pgm_done interrupt.*/
            uint32_t reserved2:        30;                       /*Reserved.*/
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t read_done:        1;                        /*The status signal for read_done interrupt.*/
            uint32_t pgm_done:         1;                        /*The status signal for pgm_done interrupt.*/
            uint32_t reserved2:       30;                        /*Reserved.*/
        };
        uint32_t val;
    } int_st;
    union {
        struct {
            uint32_t read_done:         1;                       /*The enable signal for read_done interrupt.*/
            uint32_t pgm_done:          1;                       /*The enable signal for pgm_done interrupt.*/
            uint32_t reserved2:        30;                       /*Reserved.*/
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t read_done:         1;                       /*The clear signal for read_done interrupt.*/
            uint32_t pgm_done:          1;                       /*The clear signal for pgm_done interrupt.*/
            uint32_t reserved2:        30;                       /*Reserved.*/
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t dac_clk_div:     8;                         /*Controls the division factor of the rising clock of the programming voltage.*/
            uint32_t dac_clk_pad_sel: 1;                         /*Don't care.*/
            uint32_t dac_num:         8;                         /*Controls the rising period of the programming voltage.*/
            uint32_t oe_clr:          1;                         /*Reduces the power supply of the programming voltage.*/
            uint32_t reserved18:     14;                         /*Reserved.*/
        };
        uint32_t val;
    } dac_conf;
    union {
        struct {
            uint32_t reserved0:    24;                           /*Configures the setup time of read operation.*/
            uint32_t read_init_num: 8;                           /*Configures the initial read time of eFuse.*/
        };
        uint32_t val;
    } rd_tim_conf;
    union {
        struct {
            uint32_t reserved0:  8;                              /*Configures the setup time of programming operation.*/
            uint32_t pwr_on_num:16;                              /*Configures the power up time for VDDQ.*/
            uint32_t reserved24: 8;                              /*Reserved.*/
        };
        uint32_t val;
    } wr_tim_conf1;
    union {
        struct {
            uint32_t pwr_off_num:16;                             /*Configures the power outage time for VDDQ.*/
            uint32_t reserved16: 16;                             /*Reserved.*/
        };
        uint32_t val;
    } wr_tim_conf2;
    uint32_t reserved_1f8;
    union {
        struct {
            uint32_t date:      28;                              /*Stores eFuse version.*/
            uint32_t reserved28: 4;                              /*Reserved.*/
        };
        uint32_t val;
    } date;
} efuse_dev_t;
extern efuse_dev_t EFUSE;
#ifdef __cplusplus
}
#endif

#endif  /* _SOC_EFUSE_STRUCT_H_ */
