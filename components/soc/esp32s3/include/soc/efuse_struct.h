// Copyright 2017-2020 Espressif Systems (Shanghai) PTE LTD
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


#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct efuse_dev_s {
    uint32_t pgm_data0;
    uint32_t pgm_data1;
    uint32_t pgm_data2;
    uint32_t pgm_data3;
    uint32_t pgm_data4;
    uint32_t pgm_data5;
    uint32_t pgm_data6;
    uint32_t pgm_data7;
    uint32_t pgm_check_value0;
    uint32_t pgm_check_value1;
    uint32_t pgm_check_value2;
    uint32_t rd_wr_dis;
    union {
        struct {
            uint32_t reg_rd_dis                    :    7;  /*Set this bit to disable reading from BlOCK4-10.*/
            uint32_t reg_rpt4_reserved5            :    1;  /*Reserved*/
            uint32_t reg_dis_icache                :    1;  /*Set this bit to disable Icache.*/
            uint32_t reg_dis_dcache                :    1;  /*Set this bit to disable Dcache.*/
            uint32_t reg_dis_download_icache       :    1;  /*Set this bit to disable Icache in download mode (boot_mode[3:0] is 0, 1, 2, 3, 6, 7).*/
            uint32_t reg_dis_download_dcache       :    1;  /*Set this bit to disable Dcache in download mode ( boot_mode[3:0] is 0, 1, 2, 3, 6, 7).*/
            uint32_t reg_dis_force_download        :    1;  /*Set this bit to disable the function that forces chip into download mode.*/
            uint32_t reg_dis_usb                   :    1;  /*Set this bit to disable USB function.*/
            uint32_t reg_dis_can                   :    1;  /*Set this bit to disable CAN function.*/
            uint32_t reg_dis_app_cpu               :    1;  /*Disable app cpu.*/
            uint32_t reg_soft_dis_jtag             :    3;  /*Set these bits to disable JTAG in the soft way (odd number 1 means disable ). JTAG can be enabled in HMAC module.*/
            uint32_t reg_dis_pad_jtag              :    1;  /*Set this bit to disable JTAG in the hard way. JTAG is disabled permanently.*/
            uint32_t reg_dis_download_manual_encrypt:    1;  /*Set this bit to disable flash encryption when in download boot modes.*/
            uint32_t reg_usb_drefh                 :    2;  /*Controls single-end input threshold vrefh, 1.76 V to 2 V with step of 80 mV, stored in eFuse.*/
            uint32_t reg_usb_drefl                 :    2;  /*Controls single-end input threshold vrefl, 0.8 V to 1.04 V with step of 80 mV, stored in eFuse.*/
            uint32_t reg_usb_exchg_pins            :    1;  /*Set this bit to exchange USB D+ and D- pins.*/
            uint32_t reg_ext_phy_enable            :    1;  /*Set this bit to enable external PHY.*/
            uint32_t reg_btlc_gpio_enable          :    2;  /*Enable btlc gpio.*/
            uint32_t reg_vdd_spi_modecurlim        :    1;  /*SPI regulator switches current limit mode.*/
            uint32_t reg_vdd_spi_drefh             :    2;  /*SPI regulator high voltage reference.*/
        };
        uint32_t val;
    } rd_repeat_data0;
    union {
        struct {
            uint32_t reg_vdd_spi_drefm             :    2;  /*SPI regulator medium voltage reference.*/
            uint32_t reg_vdd_spi_drefl             :    2;  /*SPI regulator low voltage reference.*/
            uint32_t reg_vdd_spi_xpd               :    1;  /*SPI regulator power up signal.*/
            uint32_t reg_vdd_spi_tieh              :    1;  /*SPI regulator output is short connected to VDD3P3_RTC_IO.*/
            uint32_t reg_vdd_spi_force             :    1;  /*Set this bit and force to use the configuration of eFuse to configure VDD_SPI.*/
            uint32_t reg_vdd_spi_en_init           :    1;  /*Set SPI regulator to 0 to configure init[1:0]=0.*/
            uint32_t reg_vdd_spi_encurlim          :    1;  /*Set SPI regulator to 1 to enable output current limit.*/
            uint32_t reg_vdd_spi_dcurlim           :    3;  /*Tunes the current limit threshold of SPI regulator when tieh=0, about 800 mA/(8+d).*/
            uint32_t reg_vdd_spi_init              :    2;  /*Adds resistor from LDO output to ground. 0: no resistance 1: 6 K 2: 4 K 3: 2 K.*/
            uint32_t reg_vdd_spi_dcap              :    2;  /*Prevents SPI regulator from overshoot.*/
            uint32_t reg_wdt_delay_sel             :    2;  /*Selects RTC watchdog timeout threshold, in unit of slow clock cycle. 0: 40000. 1: 80000. 2: 160000. 3:320000.*/
            uint32_t reg_spi_boot_crypt_cnt        :    3;  /*Set this bit to enable SPI boot encrypt/decrypt. Odd number of 1: enable. even number of 1: disable.*/
            uint32_t reg_secure_boot_key_revoke0   :    1;  /*Set this bit to enable revoking first secure boot key.*/
            uint32_t reg_secure_boot_key_revoke1   :    1;  /*Set this bit to enable revoking second secure boot key.*/
            uint32_t reg_secure_boot_key_revoke2   :    1;  /*Set this bit to enable revoking third secure boot key.*/
            uint32_t reg_key_purpose_0             :    4;  /*Purpose of Key0.*/
            uint32_t reg_key_purpose_1             :    4;  /*Purpose of Key1.*/
        };
        uint32_t val;
    } rd_repeat_data1;
    union {
        struct {
            uint32_t reg_key_purpose_2             :    4;  /*Purpose of Key2.*/
            uint32_t reg_key_purpose_3             :    4;  /*Purpose of Key3.*/
            uint32_t reg_key_purpose_4             :    4;  /*Purpose of Key4.*/
            uint32_t reg_key_purpose_5             :    4;  /*Purpose of Key5.*/
            uint32_t reg_rpt4_reserved0            :    4;  /*Reserved (used for four backups method).*/
            uint32_t reg_secure_boot_en            :    1;  /*Set this bit to enable secure boot.*/
            uint32_t reg_secure_boot_aggressive_revoke:    1;  /*Set this bit to enable revoking aggressive secure boot.*/
            uint32_t reg_dis_usb_jtag              :    1;  /*Set this bit to disable function of usb switch to jtag in module of usb device.*/
            uint32_t reg_dis_usb_device            :    1;  /*Set this bit to disable usb device.*/
            uint32_t reg_strap_jtag_sel            :    1;  /*Set this bit to enable selection between usb_to_jtag and pad_to_jtag through strapping gpio10 when both reg_dis_usb_jtag and reg_dis_pad_jtag are equal to 0.*/
            uint32_t reg_usb_phy_sel               :    1;  /*This bit is used to switch internal PHY and external PHY for USB OTG and USB Device. 0: internal PHY is assigned to USB Device while external PHY is assigned to USB OTG. 1: internal PHY is assigned to USB OTG while external PHY is assigned to USB Device.*/
            uint32_t reg_power_glitch_dsense       :    2;  /*Sample delay configuration of power glitch.*/
            uint32_t reg_flash_tpuw                :    4;  /*Configures flash waiting time after power-up, in unit of ms. If the value is less than 15, the waiting time is the configurable value; Otherwise, the waiting time is twice the configurable value.*/
        };
        uint32_t val;
    } rd_repeat_data2;
    union {
        struct {
            uint32_t reg_dis_download_mode         :    1;  /*Set this bit to disable download mode (boot_mode[3:0] = 0, 1, 2, 3, 6, 7).*/
            uint32_t reg_dis_direct_boot           :    1;  /*Set this bit to disable direct boot..*/
            uint32_t dis_usb_serial_jtag_rom_print :    1;  /*Set this bit to disable USB-Serial-JTAG print during rom boot*/
            uint32_t reg_flash_ecc_mode            :    1;  /*Set ECC mode in ROM, 0: ROM would Enable Flash ECC 16to18 byte mode. 1:ROM would use 16to17 byte mode.*/
            uint32_t reg_dis_usb_serial_jtag_download_mode: 1;  /*Set this bit to disable download through USB-Serial-JTAG.*/
            uint32_t reg_enable_security_download  :    1;  /*Set this bit to enable secure UART download mode.*/
            uint32_t reg_uart_print_control        :    2;  /*Set the default UARTboot message output mode. 00: Enabled. 01: Enabled when GPIO8 is low at reset. 10: Enabled when GPIO8 is high at reset. 11:disabled.*/
            uint32_t reg_pin_power_selection       :    1;  /*GPIO33-GPIO37 power supply selection in ROM code. 0: VDD3P3_CPU. 1: VDD_SPI.*/
            uint32_t reg_flash_type                :    1;  /*Set the maximum lines of SPI flash. 0: four lines. 1: eight lines.*/
            uint32_t reg_flash_page_size           :    2;  /*Set Flash page size.*/
            uint32_t reg_flash_ecc_en              :    1;  /*Set 1 to enable ECC for flash boot.*/
            uint32_t reg_force_send_resume         :    1;  /*Set this bit to force ROM code to send a resume command during SPI boot.*/
            uint32_t reg_secure_version            :    16;  /*Secure version (used by ESP-IDF anti-rollback feature).*/
            uint32_t reg_rpt4_reserved1            :    1;  /*Reserved (used for four backups method).*/
            uint32_t reg_dis_usb_otg_download_mode :    1;  /*Set this bit to disable download through USB-OTG*/
        };
        uint32_t val;
    } rd_repeat_data3;
    union {
        struct {
            uint32_t reg_rpt4_reserved2            :    24;  /*Reserved (used for four backups method).*/
            uint32_t reserved24                    :    8;  /*Reserved.*/
        };
        uint32_t val;
    } rd_repeat_data4;
    uint32_t rd_mac_spi_sys_0;
    union {
        struct {
            uint32_t reg_mac_1                     :    16;  /*Stores the high 16 bits of MAC address.*/
            uint32_t reg_spi_pad_conf_0            :    16;  /*Stores the zeroth part of SPI_PAD_CONF.*/
        };
        uint32_t val;
    } rd_mac_spi_sys_1;
    uint32_t rd_mac_spi_sys_2;
    union {
        struct {
            uint32_t reg_spi_pad_conf_2            :    18;  /*Stores the second part of SPI_PAD_CONF.*/
            uint32_t reg_sys_data_part0_0          :    14;  /*Stores the fist 14 bits of the zeroth part of system data.*/
        };
        uint32_t val;
    } rd_mac_spi_sys_3;
    uint32_t rd_mac_spi_sys_4;
    uint32_t rd_mac_spi_sys_5;
    uint32_t rd_sys_part1_data0;
    uint32_t rd_sys_part1_data1;
    uint32_t rd_sys_part1_data2;
    uint32_t rd_sys_part1_data3;
    uint32_t rd_sys_part1_data4;
    uint32_t rd_sys_part1_data5;
    uint32_t rd_sys_part1_data6;
    uint32_t rd_sys_part1_data7;
    uint32_t rd_usr_data0;
    uint32_t rd_usr_data1;
    uint32_t rd_usr_data2;
    uint32_t rd_usr_data3;
    uint32_t rd_usr_data4;
    uint32_t rd_usr_data5;
    uint32_t rd_usr_data6;
    uint32_t rd_usr_data7;
    uint32_t rd_key0_data0;
    uint32_t rd_key0_data1;
    uint32_t rd_key0_data2;
    uint32_t rd_key0_data3;
    uint32_t rd_key0_data4;
    uint32_t rd_key0_data5;
    uint32_t rd_key0_data6;
    uint32_t rd_key0_data7;
    uint32_t rd_key1_data0;
    uint32_t rd_key1_data1;
    uint32_t rd_key1_data2;
    uint32_t rd_key1_data3;
    uint32_t rd_key1_data4;
    uint32_t rd_key1_data5;
    uint32_t rd_key1_data6;
    uint32_t rd_key1_data7;
    uint32_t rd_key2_data0;
    uint32_t rd_key2_data1;
    uint32_t rd_key2_data2;
    uint32_t rd_key2_data3;
    uint32_t rd_key2_data4;
    uint32_t rd_key2_data5;
    uint32_t rd_key2_data6;
    uint32_t rd_key2_data7;
    uint32_t rd_key3_data0;
    uint32_t rd_key3_data1;
    uint32_t rd_key3_data2;
    uint32_t rd_key3_data3;
    uint32_t rd_key3_data4;
    uint32_t rd_key3_data5;
    uint32_t rd_key3_data6;
    uint32_t rd_key3_data7;
    uint32_t rd_key4_data0;
    uint32_t rd_key4_data1;
    uint32_t rd_key4_data2;
    uint32_t rd_key4_data3;
    uint32_t rd_key4_data4;
    uint32_t rd_key4_data5;
    uint32_t rd_key4_data6;
    uint32_t rd_key4_data7;
    uint32_t rd_key5_data0;
    uint32_t rd_key5_data1;
    uint32_t rd_key5_data2;
    uint32_t rd_key5_data3;
    uint32_t rd_key5_data4;
    uint32_t rd_key5_data5;
    uint32_t rd_key5_data6;
    uint32_t rd_key5_data7;
    uint32_t rd_sys_part2_data0;
    uint32_t rd_sys_part2_data1;
    uint32_t rd_sys_part2_data2;
    uint32_t rd_sys_part2_data3;
    uint32_t rd_sys_part2_data4;
    uint32_t rd_sys_part2_data5;
    uint32_t rd_sys_part2_data6;
    uint32_t rd_sys_part2_data7;
    union {
        struct {
            uint32_t reg_rd_dis_err                :    7;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_rpt4_reserved5_err        :    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_dis_icache_err            :    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_dis_dcache_err            :    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_dis_download_icache_err   :    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_dis_download_dcache_err   :    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_dis_force_download_err    :    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_dis_usb_err               :    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_dis_can_err               :    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_dis_app_cpu_err           :    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_soft_dis_jtag_err         :    3;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_dis_pad_jtag_err          :    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_dis_download_manual_encrypt_err:    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_usb_drefh_err             :    2;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_usb_drefl_err             :    2;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_usb_exchg_pins_err        :    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_ext_phy_enable_err        :    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_btlc_gpio_enable_err      :    2;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_vdd_spi_modecurlim_err    :    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_vdd_spi_drefh_err         :    2;  /*If any bits in this filed are 1, then it indicates a programming error.*/
        };
        uint32_t val;
    } rd_repeat_err0;
    union {
        struct {
            uint32_t reg_vdd_spi_drefm_err         :    2;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_vdd_spi_drefl_err         :    2;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_vdd_spi_xpd_err           :    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_vdd_spi_tieh_err          :    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_vdd_spi_force_err         :    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_vdd_spi_en_init_err       :    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_vdd_spi_encurlim_err      :    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_vdd_spi_dcurlim_err       :    3;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_vdd_spi_init_err          :    2;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_vdd_spi_dcap_err          :    2;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_wdt_delay_sel_err         :    2;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_spi_boot_crypt_cnt_err    :    3;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_secure_boot_key_revoke0_err:    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_secure_boot_key_revoke1_err:    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_secure_boot_key_revoke2_err:    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_key_purpose_0_err         :    4;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_key_purpose_1_err         :    4;  /*If any bits in this filed are 1, then it indicates a programming error.*/
        };
        uint32_t val;
    } rd_repeat_err1;
    union {
        struct {
            uint32_t reg_key_purpose_2_err         :    4;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_key_purpose_3_err         :    4;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_key_purpose_4_err         :    4;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_key_purpose_5_err         :    4;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_rpt4_reserved0_err        :    4;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_secure_boot_en_err        :    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_secure_boot_aggressive_revoke_err:    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_dis_usb_jtag_err          :    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_dis_usb_device_err        :    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_strap_jtag_sel_err        :    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_usb_phy_sel_err           :    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_power_glitch_dsense_err   :    2;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_flash_tpuw_err            :    4;  /*If any bits in this filed are 1, then it indicates a programming error.*/
        };
        uint32_t val;
    } rd_repeat_err2;
    union {
        struct {
            uint32_t reg_dis_download_mode_err     :    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_dis_direct_boot_err       :    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_dis_usb_serial_jtag_rom_print_err:1;/*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_flash_ecc_mode_err        :    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_dis_usb_serial_jtag_download_mode_err :    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_enable_security_download_err:    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_uart_print_control_err    :    2;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_pin_power_selection_err   :    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_flash_type_err            :    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_flash_page_size_err       :    2;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_flash_ecc_en_err          :    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_force_send_resume_err     :    1;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_secure_version_err        :    16; /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reg_rpt4_reserved1_err        :    1;  /*Reserved.*/
            uint32_t reg_dis_usb_otg_download_mode_err: 1;  /*Set this bit to disable download through USB-OTG*/
        };
        uint32_t val;
    } rd_repeat_err3;
    union {
        struct {
            uint32_t reg_rpt4_reserved2_err        :    24;  /*If any bits in this filed are 1, then it indicates a programming error.*/
            uint32_t reserved24                    :    8;  /*Reserved.*/
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
            uint32_t rd_mac_spi_8m_err_num: 3;     /*The value of this signal means the number of error bytes.*/
            uint32_t rd_mac_spi_8m_fail:    1;     /*0: Means no failure and that the data of MAC_SPI_8M is reliable 1: Means that programming user data failed and the number of error bytes is over 6.*/
            uint32_t rd_sys_part1_num:      3;     /*The value of this signal means the number of error bytes.*/
            uint32_t rd_sys_part1_fail:     1;     /*0: Means no failure and that the data of system part1 is reliable 1: Means that programming user data failed and the number of error bytes is over 6.*/
            uint32_t rd_usr_data_err_num:   3;     /*The value of this signal means the number of error bytes.*/
            uint32_t rd_usr_data_fail:      1;     /*0: Means no failure and that the user data is reliable 1: Means that programming user data failed and the number of error bytes is over 6.*/
            uint32_t rd_key0_err_num:       3;     /*The value of this signal means the number of error bytes.*/
            uint32_t rd_key0_fail:          1;     /*0: Means no failure and that the data of key$n is reliable 1: Means that programming key$n failed and the number of error bytes is over 6.*/
            uint32_t rd_key1_err_num:       3;     /*The value of this signal means the number of error bytes.*/
            uint32_t rd_key1_fail:          1;     /*0: Means no failure and that the data of key$n is reliable 1: Means that programming key$n failed and the number of error bytes is over 6.*/
            uint32_t rd_key2_err_num:       3;     /*The value of this signal means the number of error bytes.*/
            uint32_t rd_key2_fail:          1;     /*0: Means no failure and that the data of key$n is reliable 1: Means that programming key$n failed and the number of error bytes is over 6.*/
            uint32_t rd_key3_err_num:       3;     /*The value of this signal means the number of error bytes.*/
            uint32_t rd_key3_fail:          1;     /*0: Means no failure and that the data of key$n is reliable 1: Means that programming key$n failed and the number of error bytes is over 6.*/
            uint32_t rd_key4_err_num:       3;     /*The value of this signal means the number of error bytes.*/
            uint32_t rd_key4_fail:          1;     /*0: Means no failure and that the data of key$n is reliable 1: Means that programming key$n failed and the number of error bytes is over 6.*/
        };
        uint32_t val;
    } rd_rs_err0;
    union {
        struct {
            uint32_t rd_key5_err_num:   3;         /*The value of this signal means the number of error bytes.*/
            uint32_t rd_key5_fail:      1;         /*0: Means no failure and that the data of KEY5 is reliable 1: Means that programming user data failed and the number of error bytes is over 6.*/
            uint32_t rd_sys_part2_num:  3;         /*The value of this signal means the number of error bytes.*/
            uint32_t rd_sys_part2_fail: 1;         /*0: Means no failure and that the data of system part2 is reliable 1: Means that programming user data failed and the number of error bytes is over 6.*/
            uint32_t reserved8:        24;         /*Reserved.*/
        };
        uint32_t val;
    } rd_rs_err1;
    union {
        struct {
            uint32_t mem_force_pd:     1;          /*Set this bit to force eFuse SRAM into power-saving mode.*/
            uint32_t mem_clk_force_on: 1;          /*Set this bit and force to activate clock signal of eFuse SRAM.*/
            uint32_t mem_force_pu:     1;          /*Set this bit to force eFuse SRAM into working mode.*/
            uint32_t reserved3:       13;          /*Reserved.*/
            uint32_t clk_en:           1;          /*Set this bit and force to enable clock signal of eFuse memory.*/
            uint32_t reserved17:      15;          /*Reserved.*/
        };
        uint32_t val;
    } clk;
    union {
        struct {
            uint32_t op_code:   16;                /*0x5A5A: Operate programming command 0x5AA5: Operate read command.*/
            uint32_t reserved16: 16;               /*Reserved.*/
        };
        uint32_t val;
    } conf;
    union {
        struct {
            uint32_t state:            4;          /*Indicates the state of the eFuse state machine.*/
            uint32_t otp_load_sw:      1;          /*The value of OTP_LOAD_SW.*/
            uint32_t otp_vddq_c_sync2: 1;          /*The value of OTP_VDDQ_C_SYNC2.*/
            uint32_t otp_strobe_sw:    1;          /*The value of OTP_STROBE_SW.*/
            uint32_t otp_csb_sw:       1;          /*The value of OTP_CSB_SW.*/
            uint32_t otp_pgenb_sw:     1;          /*The value of OTP_PGENB_SW.*/
            uint32_t otp_vddq_is_sw:   1;          /*The value of OTP_VDDQ_IS_SW.*/
            uint32_t repeat_err_cnt:   8;          /*Indicates the number of error bits during programming BLOCK0.*/
            uint32_t reserved18:      14;          /*Reserved.*/
        };
        uint32_t val;
    } status;
    union {
        struct {
            uint32_t read_cmd:   1;                /*Set this bit to send read command.*/
            uint32_t pgm_cmd:    1;                /*Set this bit to send programming command.*/
            uint32_t blk_num:    4;                /*The serial number of the block to be programmed. Value 0-10 corresponds to block number 0-10  respectively.*/
            uint32_t reserved6: 26;                /*Reserved.*/
        };
        uint32_t val;
    } cmd;
    union {
        struct {
            uint32_t read_done:         1;         /*The raw bit signal for read_done interrupt.*/
            uint32_t pgm_done:          1;         /*The raw bit signal for pgm_done interrupt.*/
            uint32_t reserved2:        30;         /*Reserved.*/
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t read_done:        1;          /*The status signal for read_done interrupt.*/
            uint32_t pgm_done:         1;          /*The status signal for pgm_done interrupt.*/
            uint32_t reserved2:       30;          /*Reserved.*/
        };
        uint32_t val;
    } int_st;
    union {
        struct {
            uint32_t read_done:         1;         /*The enable signal for read_done interrupt.*/
            uint32_t pgm_done:          1;         /*The enable signal for pgm_done interrupt.*/
            uint32_t reserved2:        30;         /*Reserved.*/
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t read_done:         1;         /*The clear signal for read_done interrupt.*/
            uint32_t pgm_done:          1;         /*The clear signal for pgm_done interrupt.*/
            uint32_t reserved2:        30;         /*Reserved.*/
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t dac_clk_div:     8;           /*Controls the division factor of the rising clock of the programming voltage.*/
            uint32_t dac_clk_pad_sel: 1;           /*Don't care.*/
            uint32_t dac_num:         8;           /*Controls the rising period of the programming voltage.*/
            uint32_t oe_clr:          1;           /*Reduces the power supply of the programming voltage.*/
            uint32_t reserved18:     14;           /*Reserved.*/
        };
        uint32_t val;
    } dac_conf;
    union {
        struct {
            uint32_t reserved0                     :    24;  /*Reserved. (Default read timing parameter)*/
            uint32_t reg_read_init_num             :    8;  /*Configures the initial read time of eFuse.*/
        };
        uint32_t val;
    } rd_tim_conf;
    uint32_t wr_tim_conf0;
    union {
        struct {
            uint32_t tsup_a:     8;                /*Configures the setup time of programming operation.*/
            uint32_t pwr_on_num: 16;               /*Configures the power up time for VDDQ.*/
            uint32_t reserved24: 8;                /*Reserved.*/
        };
        uint32_t val;
    } wr_tim_conf1;
    union {
        struct {
            uint32_t pwr_off_num: 16;              /*Configures the power outage time for VDDQ.*/
            uint32_t reserved16: 16;               /*Reserved.*/
        };
        uint32_t val;
    } wr_tim_conf2;
    union {
        struct {
            uint32_t date:      28;                /*Stores eFuse version.*/
            uint32_t reserved28: 4;                /*Reserved.*/
        };
        uint32_t val;
    } date;
} efuse_dev_t;

extern efuse_dev_t EFUSE;

#ifdef __cplusplus
}
#endif



#endif /*_SOC_EFUSE_STRUCT_H_ */
