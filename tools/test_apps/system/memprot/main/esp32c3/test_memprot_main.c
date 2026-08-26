/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "hal/memprot_types.h"
#include "soc/memprot_defs.h"
#include "esp_private/esp_memprot_internal.h"
#include "esp_memprot.h"
#include "esp_rom_sys.h"

/**
 * ESP32C3 MEMORY PROTECTION MODULE TEST
 * =====================================
 *
 * In order to safely test all the memprot features, this test application uses memprot default settings
 * plus proprietary testing buffers:
 *      - s_iram_test_buffer (.iram_end_test, 1kB) - all IRAM/DRAM low region operations, test-only section
 *      - s_dram_test_buffer (.dram0.data, 1kB) - all IRAM/DRAM high region operations, standard section
 *      - s_rtc_text_test_buffer (.rtc_text_end_test, 1kB) - all RTCFAST low region operations, test-only section
 *      - s_rtc_data_test_buffer (.rtc.data, 1kB) - all RTCFAST high region operations, standard section
 * Testing addresses are set to the middle of the testing buffers:
 *      - test_ptr_low = (s_iram_test_buffer | s_rtc_text_test_buffer) + 0x200
 *      - test_ptr_high = (s_dram_test_buffer | s_rtc_data_test_buffer) + 0x200
 * Each operation is tested at both low & high region addresses.
 * Each test result checked against expected status of PMS violation interrupt status and
 * against expected value stored in the memory tested (where applicable)
 *
 * Testing scheme is depicted below:
 *
 *                            DRam0/DMA                                     IRam0
 *                              -----------------------------------------------
 *                              |   IRam0_PMS_0 = IRam0_PMS_1 = IRam0_PMS_2   |
 *                              |                 DRam0_PMS_0                 |
 *                              |                                             |
 *                              |                                             |
 *                              | - - - - - - - s_iram_test_buffer - - - - - -|   IRam0_line1_Split_addr
 * DRam0_DMA_line0_Split_addr   |               -- test_ptr_low --            |             =
 *               =              ===============================================   IRam0_line0_Split_addr
 * DRam0_DMA_line1_Split_addr   |                                             |             =
 *                              | - - - - - - - s_dram_test_buffer - - - - - --|   IRam0_DRam0_Split_addr (main I/D)
 *                              |              -- test_ptr_high --            |
 *                              | - - - - - - - - - - - - - - - - - - - - - - |
 *                              |                                             |
 *                              |   DRam0_PMS_1 = DRam0_PMS_2 = DRam0_PMS_3   |
 *                              |                 IRam0_PMS_3                 |
 *                              |                                             |
 *                              |                     ...                     |
 *                              |                                             |
 *                              ===============================================   SOC_RTC_IRAM_LOW (0x50000000)
 *                              |               -- test_ptr_low --            |
 *                              | - - - - - - s_rtc_text_test_buffer - - - - -|   RtcFast_Split_addr (_rtc_text_end)
 *                              |               -- .rtc.dummy --              |      (UNUSED - PADDING)
 *           8 kB               | - - - - - - - - - - - - - - - - - - - - - - |   [_rtc_dummy_end = _rtc_force_fast_start]
 *                              |             -- .rtc.force_fast --           |      (NOT USED IN THIS TEST)
 *                              | - - - - - - s_rtc_data_test_buffer - - - - -|   [_rtc_force_fast_end = _rtc_data_start]
 *                              |              -- test_ptr_high --            |
 *                              | - - - - - - - - - - - - - - - - - - - - - - |
 *                              ===============================================   SOC_RTC_IRAM_HIGH (0x50001FFF)
 *                              |                                             |
 *                              -----------------------------------------------
 */


/* Binary code for the following asm [int func(int x) { return x+x; }]
    slli a0,a0,0x1
    ret
 */
static uint8_t s_fnc_buff[] = { 0x06, 0x05, 0x82, 0x80 };
typedef int (*fnc_ptr)(int);

#define SRAM_TEST_BUFFER_SIZE      0x400
#define SRAM_TEST_OFFSET           0x200

static uint8_t __attribute__((section(".iram_end_test"))) s_iram_test_buffer[SRAM_TEST_BUFFER_SIZE] = {0};
static uint8_t __attribute__((section(".rtc_text_end_test"))) s_rtc_text_test_buffer[SRAM_TEST_BUFFER_SIZE] = {0};
static uint8_t RTC_DATA_ATTR s_rtc_data_test_buffer[SRAM_TEST_BUFFER_SIZE] = {0};
static uint8_t s_dram_test_buffer[SRAM_TEST_BUFFER_SIZE] = {0};
extern volatile bool g_override_illegal_instruction;

static void *test_mprot_addr_low(esp_mprot_mem_t mem_type)
{
    switch (mem_type) {
    case MEMPROT_TYPE_IRAM0_SRAM:
        return (void *)((uint32_t)s_iram_test_buffer + SRAM_TEST_OFFSET);
    case MEMPROT_TYPE_DRAM0_SRAM:
        return (void *)MAP_IRAM_TO_DRAM((uint32_t)s_iram_test_buffer + SRAM_TEST_OFFSET);
    case MEMPROT_TYPE_IRAM0_RTCFAST:
        return (void *)((uint32_t)s_rtc_text_test_buffer + SRAM_TEST_OFFSET);
    default:
        abort();
    }
}

static void *test_mprot_addr_high(esp_mprot_mem_t mem_type)
{
    switch (mem_type) {
    case MEMPROT_TYPE_IRAM0_SRAM:
        return (void *)MAP_DRAM_TO_IRAM((uint32_t)s_dram_test_buffer + SRAM_TEST_OFFSET);
    case MEMPROT_TYPE_DRAM0_SRAM:
        return (void *)((uint32_t)s_dram_test_buffer + SRAM_TEST_OFFSET);
    case MEMPROT_TYPE_IRAM0_RTCFAST:
        return (void *)((uint32_t)s_rtc_data_test_buffer + SRAM_TEST_OFFSET);
    default:
        abort();
    }
}

static void __attribute__((unused)) test_mprot_dump_status_register(esp_mprot_mem_t mem_type)
{
    esp_rom_printf("FAULT [");

    void *addr;
    esp_err_t err = esp_mprot_get_violate_addr(mem_type, &addr, DEFAULT_CPU_NUM);
    if (err == ESP_OK) {
        esp_rom_printf("fault addr: 0x%08X,", (uint32_t)addr);
    } else {
        esp_rom_printf("fault addr: N/A (%s),", esp_err_to_name(err));
    }

    esp_mprot_pms_world_t world;
    err = esp_mprot_get_violate_world(mem_type, &world, DEFAULT_CPU_NUM);
    if (err == ESP_OK) {
        esp_rom_printf(" world: %s,", esp_mprot_pms_world_to_str(world));
    } else {
        esp_rom_printf(" world: N/A (%s),", esp_err_to_name(err));
    }

    uint32_t oper;
    err = esp_mprot_get_violate_operation(mem_type, &oper, DEFAULT_CPU_NUM);
    if (err == ESP_OK) {
        esp_rom_printf(" operation: %s", esp_mprot_oper_type_to_str(oper));
    } else {
        esp_rom_printf(" operation: N/A (%s)", esp_err_to_name(err));
    }

    // DRAM/DMA fault: check byte-enables
    if (mem_type == MEMPROT_TYPE_DRAM0_SRAM) {
        uint32_t byteen;
        err = esp_mprot_get_violate_byte_enables(mem_type, &byteen, DEFAULT_CPU_NUM);
        if (err == ESP_OK) {
            esp_rom_printf(", byte en: 0x%08X", byteen);
        } else {
            esp_rom_printf(", byte en: N/A (%s)", esp_err_to_name(err));
        }
    }

    esp_rom_printf( "]\n" );
}

static void test_mprot_check_test_result(esp_mprot_mem_t mem_type, bool expected_status)
{
    esp_memp_intr_source_t memp_intr;
    esp_err_t err = esp_mprot_get_active_intr(&memp_intr);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_get_active_intr() failed (%s) - test_mprot_check_test_result\n", esp_err_to_name(err));
        return;
    }

    bool intr_on = memp_intr.mem_type == mem_type && memp_intr.core > -1;
    bool test_result = expected_status ? !intr_on : intr_on;

    if (test_result) {
        esp_rom_printf("OK\n");
    } else {
        test_mprot_dump_status_register(mem_type);
    }
}

static void test_mprot_clear_all_interrupts(void)
{
    esp_err_t err = esp_mprot_monitor_clear_intr(MEMPROT_TYPE_IRAM0_SRAM, DEFAULT_CPU_NUM);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_monitor_clear_intr(MEMPROT_TYPE_IRAM0_SRAM) failed (%s) - test_mprot_clear_all_interrupts\n", esp_err_to_name(err));
    }
    err = esp_mprot_monitor_clear_intr(MEMPROT_TYPE_DRAM0_SRAM, DEFAULT_CPU_NUM);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_monitor_clear_intr(MEMPROT_TYPE_DRAM0_SRAM) failed (%s) - test_mprot_clear_all_interrupts\n", esp_err_to_name(err));
    }
    err = esp_mprot_monitor_clear_intr(MEMPROT_TYPE_IRAM0_RTCFAST, DEFAULT_CPU_NUM);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_monitor_clear_intr(MEMPROT_TYPE_IRAM0_RTCFAST) failed (%s) - test_mprot_clear_all_interrupts\n", esp_err_to_name(err));
    }
}

static void test_mprot_get_permissions(bool low, esp_mprot_mem_t mem_type, bool *read, bool *write, bool *exec )
{
    esp_mprot_pms_area_t area;

    switch (mem_type) {
    case MEMPROT_TYPE_IRAM0_SRAM:
        area = low ? MEMPROT_PMS_AREA_IRAM0_2 : MEMPROT_PMS_AREA_IRAM0_3;
        break;
    case MEMPROT_TYPE_DRAM0_SRAM:
        area = low ? MEMPROT_PMS_AREA_DRAM0_0 : MEMPROT_PMS_AREA_DRAM0_1;
        break;
    case MEMPROT_TYPE_IRAM0_RTCFAST:
        area = low ? MEMPROT_PMS_AREA_IRAM0_RTCFAST_LO : MEMPROT_PMS_AREA_IRAM0_RTCFAST_HI;
        break;
    default:
        abort();
    }

    uint32_t flags;
    esp_err_t err = esp_mprot_get_pms_area(area, &flags, DEFAULT_CPU_NUM);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_get_pms_area() failed (%s) - test_mprot_get_permissions\n", esp_err_to_name(err));
        return;
    }

    if (read) {
        *read = flags & MEMPROT_OP_READ;
    }
    if (write) {
        *write = flags & MEMPROT_OP_WRITE;
    }
    if (exec) {
        *exec = flags & MEMPROT_OP_EXEC;
    }
}

static void test_mprot_set_permissions(bool low, esp_mprot_mem_t mem_type, bool read, bool write, bool *exec)
{
    esp_err_t err;
    uint32_t flags = 0;
    if (read) {
        flags |= MEMPROT_OP_READ;
    }
    if (write) {
        flags |= MEMPROT_OP_WRITE;
    }
    if (exec && *exec) {
        flags |= MEMPROT_OP_EXEC;
    }

    switch (mem_type) {
    case MEMPROT_TYPE_IRAM0_SRAM: {
        if (low) {
            if ((err = esp_mprot_set_pms_area(MEMPROT_PMS_AREA_IRAM0_0, flags, DEFAULT_CPU_NUM)) != ESP_OK) {
                break;
            }
            if ((err = esp_mprot_set_pms_area(MEMPROT_PMS_AREA_IRAM0_1, flags, DEFAULT_CPU_NUM)) != ESP_OK) {
                break;
            }
            if ((err = esp_mprot_set_pms_area(MEMPROT_PMS_AREA_IRAM0_2, flags, DEFAULT_CPU_NUM)) != ESP_OK) {
                break;
            }
        } else {
            if ((err = esp_mprot_set_pms_area(MEMPROT_PMS_AREA_IRAM0_3, flags, DEFAULT_CPU_NUM)) != ESP_OK) {
                break;
            }
        }
    }
    break;
    case MEMPROT_TYPE_DRAM0_SRAM: {
        if (low) {
            if ((err = esp_mprot_set_pms_area(MEMPROT_PMS_AREA_DRAM0_0, flags, DEFAULT_CPU_NUM)) != ESP_OK) {
                break;
            }
        } else {
            if ((err = esp_mprot_set_pms_area(MEMPROT_PMS_AREA_DRAM0_1, flags, DEFAULT_CPU_NUM)) != ESP_OK) {
                break;
            }
            if ((err = esp_mprot_set_pms_area(MEMPROT_PMS_AREA_DRAM0_2, flags, DEFAULT_CPU_NUM)) != ESP_OK) {
                break;
            }
            if ((err = esp_mprot_set_pms_area(MEMPROT_PMS_AREA_DRAM0_3, flags, DEFAULT_CPU_NUM)) != ESP_OK) {
                break;
            }
        }
    }
    break;
    case MEMPROT_TYPE_IRAM0_RTCFAST: {
        if (low) {
            if ((err = esp_mprot_set_pms_area(MEMPROT_PMS_AREA_IRAM0_RTCFAST_LO, flags, DEFAULT_CPU_NUM)) != ESP_OK) {
                break;
            }
        } else {
            if ((err = esp_mprot_set_pms_area(MEMPROT_PMS_AREA_IRAM0_RTCFAST_HI, flags, DEFAULT_CPU_NUM)) != ESP_OK) {
                break;
            }
        }
    }
    break;
    default:
        abort();
    }

    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_set_pms_area() failed (%s) - test_mprot_set_permissions\n", esp_err_to_name(err));
    }
}

static void test_mprot_read(esp_mprot_mem_t mem_type)
{
    test_mprot_clear_all_interrupts();

    //get current permission settings
    bool write_perm_low, write_perm_high, read_perm_low, read_perm_high, exec_perm_low, exec_perm_high;
    bool is_exec_mem = mem_type & MEMPROT_TYPE_IRAM0_ANY;
    test_mprot_get_permissions(true, mem_type, &read_perm_low, &write_perm_low, is_exec_mem ? &exec_perm_low : NULL);
    test_mprot_get_permissions(false, mem_type, &read_perm_high, &write_perm_high, is_exec_mem ? &exec_perm_high : NULL);

    //get testing pointers for low & high regions
    volatile uint32_t *ptr_low = test_mprot_addr_low(mem_type);
    volatile uint32_t *ptr_high = test_mprot_addr_high(mem_type);
    const uint32_t test_val = 100;

    //temporarily allow WRITE for setting the test values
    esp_err_t err = esp_mprot_set_monitor_en(mem_type, false, DEFAULT_CPU_NUM);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_set_monitor_en() failed (%s) - test_mprot_read\n", esp_err_to_name(err));
        return;
    }

    test_mprot_set_permissions(true, mem_type, read_perm_low, true, is_exec_mem ? &exec_perm_low : NULL);
    test_mprot_set_permissions(false, mem_type, read_perm_high, true, is_exec_mem ? &exec_perm_high : NULL);

    //save testing values
    *ptr_low = test_val;
    *ptr_high = test_val + 1;

    //restore current PMS settings
    test_mprot_set_permissions(true, mem_type, read_perm_low, write_perm_low, is_exec_mem ? &exec_perm_low : NULL);
    test_mprot_set_permissions(false, mem_type, read_perm_high, write_perm_high, is_exec_mem ? &exec_perm_high : NULL);

    //reenable monitoring
    err = esp_mprot_set_monitor_en(mem_type, true, DEFAULT_CPU_NUM);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_set_monitor_en() failed (%s) - test_mprot_read\n", esp_err_to_name(err));
        return;
    }

    //perform READ test in low region
    esp_rom_printf("%s read low: ", esp_mprot_mem_type_to_str(mem_type));
    err = esp_mprot_monitor_clear_intr(mem_type, DEFAULT_CPU_NUM);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_monitor_clear_intr() failed (%s) - test_mprot_read\n", esp_err_to_name(err));
        return;
    }

    volatile uint32_t val = *ptr_low;

    if (read_perm_low && val != test_val) {
        esp_rom_printf( "UNEXPECTED VALUE 0x%08X -", val );
        test_mprot_dump_status_register(mem_type);
    } else {
        test_mprot_check_test_result(mem_type, read_perm_low);
    }

    //perform READ in high region
    esp_rom_printf("%s read high: ", esp_mprot_mem_type_to_str(mem_type));
    err = esp_mprot_monitor_clear_intr(mem_type, DEFAULT_CPU_NUM);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_monitor_clear_intr() failed (%s) - test_mprot_read\n", esp_err_to_name(err));
        return;
    }

    val = *ptr_high;

    if (read_perm_high && val != (test_val + 1)) {
        esp_rom_printf( "UNEXPECTED VALUE 0x%08X -", val);
        test_mprot_dump_status_register(mem_type);
    } else {
        test_mprot_check_test_result(mem_type, read_perm_high);
    }
}

static void test_mprot_write(esp_mprot_mem_t mem_type)
{
    test_mprot_clear_all_interrupts();

    //get current READ & WRITE permission settings
    bool write_perm_low, write_perm_high, read_perm_low, read_perm_high, exec_perm_low, exec_perm_high;
    bool is_exec_mem = mem_type & MEMPROT_TYPE_IRAM0_ANY;
    test_mprot_get_permissions(true, mem_type, &read_perm_low, &write_perm_low, is_exec_mem ? &exec_perm_low : NULL);
    test_mprot_get_permissions(false, mem_type, &read_perm_high, &write_perm_high, is_exec_mem ? &exec_perm_high : NULL);

    //ensure READ enabled
    esp_err_t err = esp_mprot_set_monitor_en(mem_type, false, DEFAULT_CPU_NUM);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_set_monitor_en() failed (%s) - test_mprot_write\n", esp_err_to_name(err));
        return;
    }

    test_mprot_set_permissions(true, mem_type, true, write_perm_low, is_exec_mem ? &exec_perm_low : NULL);
    test_mprot_set_permissions(false, mem_type, true, write_perm_high, is_exec_mem ? &exec_perm_high : NULL);

    err = esp_mprot_set_monitor_en(mem_type, true, DEFAULT_CPU_NUM);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_set_monitor_en() failed (%s) - test_mprot_write\n", esp_err_to_name(err));
        return;
    }

    volatile uint32_t *ptr_low = test_mprot_addr_low(mem_type);
    volatile uint32_t *ptr_high = test_mprot_addr_high(mem_type);
    const uint32_t test_val = 10;

    //perform WRITE in low region
    esp_rom_printf("%s write low: ", esp_mprot_mem_type_to_str(mem_type));
    err = esp_mprot_monitor_clear_intr(mem_type, DEFAULT_CPU_NUM);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_monitor_clear_intr() failed (%s) - test_mprot_write\n", esp_err_to_name(err));
        return;
    }

    volatile uint32_t val = 0;
    *ptr_low = test_val;
    val = *ptr_low;

    if (val != test_val && write_perm_low) {
        esp_rom_printf( "UNEXPECTED VALUE 0x%08X -", val);
        test_mprot_dump_status_register(mem_type);
    } else {
        test_mprot_check_test_result(mem_type, write_perm_low);
    }

    //perform WRITE in high region
    esp_rom_printf("%s write high: ", esp_mprot_mem_type_to_str(mem_type));
    err = esp_mprot_monitor_clear_intr(mem_type, DEFAULT_CPU_NUM);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_monitor_clear_intr() failed (%s) - test_mprot_write\n", esp_err_to_name(err));
        return;
    }

    val = 0;
    *ptr_high = test_val + 1;
    val = *ptr_high;

    if (val != (test_val + 1) && write_perm_high) {
        esp_rom_printf( "UNEXPECTED VALUE 0x%08X -", val);
        test_mprot_dump_status_register(mem_type);
    } else {
        test_mprot_check_test_result(mem_type, write_perm_high);
    }

    //restore original permissions
    err = esp_mprot_set_monitor_en(mem_type, false, DEFAULT_CPU_NUM);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_set_monitor_en() failed (%s) - test_mprot_write\n", esp_err_to_name(err));
        return;
    }

    test_mprot_set_permissions(true, mem_type, read_perm_low, write_perm_low, is_exec_mem ? &exec_perm_low : NULL);
    test_mprot_set_permissions(false, mem_type, read_perm_high, write_perm_high, is_exec_mem ? &exec_perm_high : NULL);

    err = esp_mprot_set_monitor_en(mem_type, true, DEFAULT_CPU_NUM);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_set_monitor_en() failed (%s) - test_mprot_write\n", esp_err_to_name(err));
        return;
    }
}

static void test_mprot_exec(esp_mprot_mem_t mem_type)
{
    if (!(mem_type & MEMPROT_TYPE_IRAM0_ANY)) {
        esp_rom_printf("Error: EXEC test available only for IRAM access.\n" );
        return;
    }

    test_mprot_clear_all_interrupts();

    bool write_perm_low, write_perm_high, read_perm_low, read_perm_high, exec_perm_low, exec_perm_high;

    //temporarily enable READ/WRITE
    test_mprot_get_permissions(true, mem_type, &read_perm_low, &write_perm_low, &exec_perm_low);
    test_mprot_get_permissions(false, mem_type, &read_perm_high, &write_perm_high, &exec_perm_high);
    esp_err_t err = esp_mprot_set_monitor_en(mem_type, false, DEFAULT_CPU_NUM);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_set_monitor_en() failed (%s) - test_mprot_exec\n", esp_err_to_name(err));
        return;
    }
    test_mprot_set_permissions(true, mem_type, true, true, &exec_perm_low);
    test_mprot_set_permissions(false, mem_type, true, true, &exec_perm_high);
    err = esp_mprot_set_monitor_en(mem_type, true, DEFAULT_CPU_NUM);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_set_monitor_en() failed (%s) - test_mprot_exec\n", esp_err_to_name(err));
        return;
    }

    //get testing pointers for low & high regions, zero 8B slot
    void *fnc_ptr_low = test_mprot_addr_low(mem_type);
    void *fnc_ptr_high = test_mprot_addr_high(mem_type);
    memset(fnc_ptr_low, 0, 8);
    memset(fnc_ptr_high, 0, 8);

    //inject the code to both low & high segments
    memcpy((void *)fnc_ptr_low, (const void *)s_fnc_buff, sizeof(s_fnc_buff));
    memcpy((void *)fnc_ptr_high, (const void *)s_fnc_buff, sizeof(s_fnc_buff));

    uint32_t res = 0;

    //LOW REGION: clear the intr flag & try to execute the code injected
    esp_rom_printf("%s exec low: ", esp_mprot_mem_type_to_str(mem_type));
    err = esp_mprot_monitor_clear_intr(mem_type, DEFAULT_CPU_NUM);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_monitor_clear_intr() failed (%s) - test_mprot_exec\n", esp_err_to_name(err));
        return;
    }

    fnc_ptr fnc = (fnc_ptr)fnc_ptr_low;

    g_override_illegal_instruction = true;
    res = fnc(5);
    g_override_illegal_instruction = false;

    //check results
    bool fnc_call_ok = res == 10;
    if (fnc_call_ok) {
        test_mprot_check_test_result(mem_type, exec_perm_low);
    } else {
        if (!exec_perm_low) {
            test_mprot_check_test_result(mem_type, false);
        } else {
            esp_rom_printf(" FAULT [injected code not executed]\n");
        }
    }

    //HIGH REGION: clear the intr-on flag & try to execute the code injected
    esp_rom_printf("%s exec high: ", esp_mprot_mem_type_to_str(mem_type));
    err = esp_mprot_monitor_clear_intr(mem_type, DEFAULT_CPU_NUM);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_monitor_clear_intr() failed (%s) - test_mprot_exec\n", esp_err_to_name(err));
        return;
    }

    fnc = (fnc_ptr)fnc_ptr_high;

    g_override_illegal_instruction = true;
    res = fnc(6);
    g_override_illegal_instruction = false;

    fnc_call_ok = res == 12;
    if (fnc_call_ok) {
        test_mprot_check_test_result(mem_type, exec_perm_high);
    } else {
        if (!exec_perm_high) {
            test_mprot_check_test_result(mem_type, false);
        } else {
            esp_rom_printf(" FAULT [injected code not executed]\n");
        }
    }

    //restore original permissions
    err = esp_mprot_set_monitor_en(mem_type, false, DEFAULT_CPU_NUM);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_set_monitor_en() failed (%s) - test_mprot_exec\n", esp_err_to_name(err));
        return;
    }
    test_mprot_set_permissions(true, mem_type, read_perm_low, write_perm_low, &exec_perm_low);
    test_mprot_set_permissions(false, mem_type, read_perm_high, write_perm_high, &exec_perm_high);
    err = esp_mprot_set_monitor_en(mem_type, true, DEFAULT_CPU_NUM);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_set_monitor_en() failed (%s) - test_mprot_exec\n", esp_err_to_name(err));
        return;
    }
}

/* ********************************************************************************************
 * main test runner
 */
void app_main(void)
{
    esp_memp_config_t memp_cfg = ESP_MEMPROT_DEFAULT_CONFIG();
    memp_cfg.invoke_panic_handler = false;
    memp_cfg.lock_feature = false;

    esp_err_t err = esp_mprot_set_prot(&memp_cfg);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_set_prot() failed (%s) - app_main\n", esp_err_to_name(err));
        return;
    }

    test_mprot_read(MEMPROT_TYPE_IRAM0_SRAM);
    test_mprot_write(MEMPROT_TYPE_IRAM0_SRAM);
    test_mprot_exec(MEMPROT_TYPE_IRAM0_SRAM);

    test_mprot_read(MEMPROT_TYPE_DRAM0_SRAM);
    test_mprot_write(MEMPROT_TYPE_DRAM0_SRAM);

    test_mprot_read(MEMPROT_TYPE_IRAM0_RTCFAST);
    test_mprot_write(MEMPROT_TYPE_IRAM0_RTCFAST);
    test_mprot_exec(MEMPROT_TYPE_IRAM0_RTCFAST);
}
