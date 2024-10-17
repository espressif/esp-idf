/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "hal/memprot_types.h"
#include "soc/memprot_defs.h"
#include "soc/sensitive_reg.h"
#include "esp_private/esp_memprot_internal.h"
#include "esp_memprot.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

/**
 * ESP32S3 MEMORY PROTECTION MODULE TEST
 * =====================================
 *
 * In order to safely test all the memory protection features in guarded regions, this test application uses the default Memprot settings,
 * and deploys the following buffers and testing sections:
 *      - s_iram_test_buffer (.iram_end_test, 1kB) - all IRAM/DRAM low region operations, test-only section, code
 *      - s_dram_test_buffer (.dram0.data, 1kB) - all IRAM/DRAM high region operations, standard section, data
 *      - s_rtc_text_test_buffer (.rtc_text_end_test, 1kB) - all RTCFAST low region operations, test-only section, code
 *      - s_rtc_data_test_buffer (.rtc.force_fast, 1kB) - all RTCFAST high region operations, standard section, data
 * The testing addresses are used for direct verification of R/W/X permissions setup, as given by the Memprot default setup
 *      for each memory type guarded
 * The addresses are set to the "middle" of the testing buffers:
 *      - test_ptr_low = (s_iram_test_buffer | s_rtc_text_test_buffer) + 0x200
 *      - test_ptr_high = (s_dram_test_buffer | s_rtc_data_test_buffer) + 0x200
 * Each operation is tested at both low & high regions, though each section is considered a standalone test
 * Each test result is checked against expected status of the PMS violation interrupt and possibly
 *      against expected value possibly stored at the concerned address
 *
 * NOTE: RTCFAST split-line is always set to _rtc_text_end address, unlike D/IRAM memory (configurable split-lines). Always check
 *       the corresponding MAP file for real addresses, offsets and alignments, or/and to verify given section existence
 *       (see the GNU LD manual for memory sections generation rules)
 *
 * The testing memory disposition is depicted below:
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
 *                              | - - - - - - - s_dram_test_buffer - - - - - -|   IRam0_DRam0_Split_addr (main I/D)
 *                              |              -- test_ptr_high --            |
 *                              | - - - - - - - - - - - - - - - - - - - - - - |
 *                              |                                             |
 *                              |   DRam0_PMS_1 = DRam0_PMS_2 = DRam0_PMS_3   |
 *                              |                 IRam0_PMS_3                 |
 *                              |                                             |
 *                              |                     ...                     |
 *                              |                                             |
 *                              ===============================================   SOC_RTC_IRAM_LOW (0x600FE000)
 *                              |               -- test_ptr_low --            |
 *                              |                   .rtc.text                 |   RTC FAST code (s_rtc_text_test_buffer, own rtc_text_end_test section)
 *                              |                                             |
 *           8 kB               | - - - - - - - - - - - - - - - - - - - - - - |   RtcFast_Split_addr (_rtc_text_end, fixed)
 *                              |               -- test_ptr_high --           |   [_rtc_text_end = _rtc_force_fast_start]
 *                              |                 .rtc.force_fast             |
 *                              |                                             |   RTC FAST data (s_rtc_data_test_buffer)
 *                              | - - - - - - - - - - - - - - - - - - - - - - |
 *                              |                     ...                     |
 *                              ===============================================
 */

/* !!!IMPORTANT!!!
 * a0 needs to be saved/restored manually (not clobbered) to avoid return address corruption
 * caused by ASM block handling
 */
#define CODE_EXEC(code_buf, param, res) \
    asm volatile ( \
        "mov a3, a0\n\t" \
        "movi a2," #param "\n\t" \
        "callx0 %1\n\t" \
        "mov %0,a2\n\t" \
        "mov a0, a3\n\t" \
        : "=r"(res) \
        : "r"(code_buf) \
        : "a2", "a3" );

/* Binary code for the following asm:
 *
    .type _testfunc,@function
    .global _testfunc
    .align 4

 _testfunc:
    slli a2, a2, 1
    ret.n
 */

static uint8_t s_fnc_buff[] = {0xf0, 0x22, 0x11, 0x0d, 0xf0, 0x00, 0x00, 0x00};
typedef int (*fnc_ptr)(int);

//testing buffers
#define MEMPROT_TEST_BUFFER_SIZE        0x400
#define MEMPROT_TEST_OFFSET             0x200

static uint8_t __attribute__((section(".iram_end_test"))) s_iram_test_buffer[MEMPROT_TEST_BUFFER_SIZE] = {0};
static uint8_t __attribute__((section(".rtc_text_end_test"))) s_rtc_code_test_buffer[MEMPROT_TEST_BUFFER_SIZE] = {0};
static uint8_t RTC_FAST_ATTR s_rtc_data_test_buffer[MEMPROT_TEST_BUFFER_SIZE] = {0};
static uint8_t s_dram_test_buffer[MEMPROT_TEST_BUFFER_SIZE] = {0};

//auxiliary defines
#define LOW_REGION                      true
#define HIGH_REGION                     false
#define READ_ENA                        true
#define READ_DIS                        false
#define WRITE_ENA                       true
#define WRITE_DIS                       false
#define EXEC_ENA                        true
#define EXEC_DIS                        false

volatile bool g_override_illegal_instruction;

static void *test_mprot_addr_low(esp_mprot_mem_t mem_type)
{
    switch (mem_type) {
    case MEMPROT_TYPE_IRAM0_SRAM:
        return (void *)((uint32_t)s_iram_test_buffer + MEMPROT_TEST_OFFSET);
    case MEMPROT_TYPE_DRAM0_SRAM:
        return (void *)MAP_IRAM_TO_DRAM((uint32_t)s_iram_test_buffer + MEMPROT_TEST_OFFSET);
    case MEMPROT_TYPE_IRAM0_RTCFAST:
        return (void *)((uint32_t)s_rtc_code_test_buffer + MEMPROT_TEST_OFFSET);
    default:
        abort();
    }
}

static void *test_mprot_addr_high(esp_mprot_mem_t mem_type)
{
    switch (mem_type) {
    case MEMPROT_TYPE_IRAM0_SRAM:
        return (void *)MAP_DRAM_TO_IRAM((uint32_t)s_dram_test_buffer + MEMPROT_TEST_OFFSET);
    case MEMPROT_TYPE_DRAM0_SRAM:
        return (void *)((uint32_t)s_dram_test_buffer + MEMPROT_TEST_OFFSET);
    case MEMPROT_TYPE_IRAM0_RTCFAST:
        return (void *)((uint32_t)s_rtc_data_test_buffer + MEMPROT_TEST_OFFSET);
    default:
        abort();
    }
}

static void __attribute__((unused)) test_mprot_dump_status_register(esp_mprot_mem_t mem_type, const int core)
{
    esp_rom_printf("FAULT [");

    esp_rom_printf("core 0 dram0: 0x%08X, core 1 dram0: 0x%08X, ", REG_READ(SENSITIVE_CORE_0_DRAM0_PMS_MONITOR_2_REG), REG_READ(SENSITIVE_CORE_1_DRAM0_PMS_MONITOR_2_REG));
    esp_rom_printf("core 0 iram0: 0x%08X, core 1 iram0: 0x%08X, ", REG_READ(SENSITIVE_CORE_0_IRAM0_PMS_MONITOR_2_REG), REG_READ(SENSITIVE_CORE_1_IRAM0_PMS_MONITOR_2_REG));

    void *addr;
    esp_err_t err = esp_mprot_get_violate_addr(mem_type, &addr, core);
    if (err == ESP_OK) {
        esp_rom_printf("fault addr: 0x%08X,", (uint32_t)addr);
    } else {
        esp_rom_printf("fault addr: N/A (%s),", esp_err_to_name(err));
    }

    esp_mprot_pms_world_t world;
    err = esp_mprot_get_violate_world(mem_type, &world, core);
    if (err == ESP_OK) {
        esp_rom_printf(" world: %s,", esp_mprot_pms_world_to_str(world));
    } else {
        esp_rom_printf(" world: N/A (%s),", esp_err_to_name(err));
    }

    uint32_t oper;
    err = esp_mprot_get_violate_operation(mem_type, &oper, core);
    if (err == ESP_OK) {
        esp_rom_printf(" operation: %s", esp_mprot_oper_type_to_str(oper));
    } else {
        esp_rom_printf(" operation: N/A (%s)", esp_err_to_name(err));
    }

    // DRAM/DMA fault: check byte-enables
    if (mem_type == MEMPROT_TYPE_DRAM0_SRAM) {
        uint32_t byteen;
        err = esp_mprot_get_violate_byte_enables(mem_type, &byteen, core);
        if (err == ESP_OK) {
            esp_rom_printf(", byte en: 0x%08X", byteen);
        } else {
            esp_rom_printf(", byte en: N/A (%s)", esp_err_to_name(err));
        }
    }

    esp_rom_printf("]\n");
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
        test_mprot_dump_status_register(mem_type, memp_intr.core);
    }
}

static void test_mprot_get_permissions(bool low, esp_mprot_mem_t mem_type, bool *read, bool *write, bool *exec, const int core)
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
    esp_err_t err = esp_mprot_get_pms_area(area, &flags, core);
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

static void test_mprot_set_permissions(bool low, esp_mprot_mem_t mem_type, bool read, bool write, bool *exec, const int core)
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
            if ((err = esp_mprot_set_pms_area(MEMPROT_PMS_AREA_IRAM0_0, flags, core)) != ESP_OK) {
                break;
            }
            if ((err = esp_mprot_set_pms_area(MEMPROT_PMS_AREA_IRAM0_1, flags, core)) != ESP_OK) {
                break;
            }
            if ((err = esp_mprot_set_pms_area(MEMPROT_PMS_AREA_IRAM0_2, flags, core)) != ESP_OK) {
                break;
            }
        } else {
            if ((err = esp_mprot_set_pms_area(MEMPROT_PMS_AREA_IRAM0_3, flags, core)) != ESP_OK) {
                break;
            }
        }
    }
    break;
    case MEMPROT_TYPE_DRAM0_SRAM: {
        if (low) {
            if ((err = esp_mprot_set_pms_area(MEMPROT_PMS_AREA_DRAM0_0, flags, core)) != ESP_OK) {
                break;
            }
        } else {
            if ((err = esp_mprot_set_pms_area(MEMPROT_PMS_AREA_DRAM0_1, flags, core)) != ESP_OK) {
                break;
            }
            if ((err = esp_mprot_set_pms_area(MEMPROT_PMS_AREA_DRAM0_2, flags, core)) != ESP_OK) {
                break;
            }
            if ((err = esp_mprot_set_pms_area(MEMPROT_PMS_AREA_DRAM0_3, flags, core)) != ESP_OK) {
                break;
            }
        }
    }
    break;
    case MEMPROT_TYPE_IRAM0_RTCFAST: {
        if (low) {
            if ((err = esp_mprot_set_pms_area(MEMPROT_PMS_AREA_IRAM0_RTCFAST_LO, flags, core)) != ESP_OK) {
                break;
            }
        } else {
            if ((err = esp_mprot_set_pms_area(MEMPROT_PMS_AREA_IRAM0_RTCFAST_HI, flags, core)) != ESP_OK) {
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

static void test_mprot_read(esp_mprot_mem_t mem_type, const int core)
{
    esp_err_t err = esp_mprot_monitor_clear_intr(mem_type, core);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_monitor_clear_intr() failed on core %d (%s) - test_mprot_read\n", core, esp_err_to_name(err));
        return;
    }

    //get current permission settings
    bool write_perm_low, write_perm_high, read_perm_low, read_perm_high, exec_perm_low, exec_perm_high;
    bool is_exec_mem = mem_type & MEMPROT_TYPE_IRAM0_ANY;
    test_mprot_get_permissions(true, mem_type, &read_perm_low, &write_perm_low, is_exec_mem ? &exec_perm_low : NULL, core);
    test_mprot_get_permissions(false, mem_type, &read_perm_high, &write_perm_high, is_exec_mem ? &exec_perm_high : NULL, core);

    //get testing pointers for low & high regions
    uint32_t *ptr_low = test_mprot_addr_low(mem_type);
    uint32_t *ptr_high = test_mprot_addr_high(mem_type);
    const uint32_t test_val = 100;

    //temporarily allow WRITE for setting the test values
    err = esp_mprot_set_monitor_en(mem_type, false, core);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_set_monitor_en() failed (%s) - test_mprot_read\n", esp_err_to_name(err));
        return;
    }

    test_mprot_set_permissions(LOW_REGION, mem_type, read_perm_low, WRITE_ENA, is_exec_mem ? &exec_perm_low : NULL, core);
    test_mprot_set_permissions(HIGH_REGION, mem_type, read_perm_high, WRITE_ENA, is_exec_mem ? &exec_perm_high : NULL, core);

    //store testing values to appropriate memory
    *ptr_low = test_val;
    *ptr_high = test_val + 1;

    //restore current PMS settings
    test_mprot_set_permissions(LOW_REGION, mem_type, read_perm_low, write_perm_low, is_exec_mem ? &exec_perm_low : NULL, core);
    test_mprot_set_permissions(HIGH_REGION, mem_type, read_perm_high, write_perm_high, is_exec_mem ? &exec_perm_high : NULL, core);

    //re-enable monitoring
    err = esp_mprot_set_monitor_en(mem_type, true, core);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_set_monitor_en() failed (%s) - test_mprot_read\n", esp_err_to_name(err));
        return;
    }

    //perform READ test in low region
    esp_rom_printf("%s (core %d) read low: ", esp_mprot_mem_type_to_str(mem_type), core);
    err = esp_mprot_monitor_clear_intr(mem_type, core);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_monitor_clear_intr() failed (%s) - test_mprot_read\n", esp_err_to_name(err));
        return;
    }

    volatile uint32_t val = *ptr_low;

    if (read_perm_low && val != test_val) {
        esp_rom_printf("UNEXPECTED VALUE 0x%08X -", val);
        test_mprot_dump_status_register(mem_type, core);
    } else {
        test_mprot_check_test_result(mem_type, read_perm_low);
    }

    //perform READ in high region
    esp_rom_printf("%s (core %d) read high: ", esp_mprot_mem_type_to_str(mem_type), core);
    err = esp_mprot_monitor_clear_intr(mem_type, core);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_monitor_clear_intr() failed on core %d (%s) - test_mprot_read\n", core, esp_err_to_name(err));
        return;
    }

    val = *ptr_high;

    if (read_perm_high && val != (test_val + 1)) {
        esp_rom_printf("UNEXPECTED VALUE 0x%08X -", val);
        test_mprot_dump_status_register(mem_type, core);
    } else {
        test_mprot_check_test_result(mem_type, read_perm_high);
    }

    esp_mprot_monitor_clear_intr(mem_type, core);
}

static void test_mprot_write(esp_mprot_mem_t mem_type, const int core)
{
    esp_err_t err = esp_mprot_monitor_clear_intr(mem_type, core);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_monitor_clear_intr() failed on core %d (%s) - test_mprot_write\n", core, esp_err_to_name(err));
        return;
    }

    //get current READ & WRITE permission settings
    bool write_perm_low, write_perm_high, read_perm_low, read_perm_high, exec_perm_low, exec_perm_high;
    bool is_exec_mem = mem_type & MEMPROT_TYPE_IRAM0_ANY;
    test_mprot_get_permissions(LOW_REGION, mem_type, &read_perm_low, &write_perm_low, is_exec_mem ? &exec_perm_low : NULL, core);
    test_mprot_get_permissions(HIGH_REGION, mem_type, &read_perm_high, &write_perm_high, is_exec_mem ? &exec_perm_high : NULL, core);

    //ensure READ enabled
    err = esp_mprot_set_monitor_en(mem_type, false, core);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_set_monitor_en() failed (%s) - test_mprot_write\n", esp_err_to_name(err));
        return;
    }

    test_mprot_set_permissions(LOW_REGION, mem_type, READ_ENA, write_perm_low, is_exec_mem ? &exec_perm_low : NULL, core);
    test_mprot_set_permissions(HIGH_REGION, mem_type, READ_ENA, write_perm_high, is_exec_mem ? &exec_perm_high : NULL, core);

    err = esp_mprot_set_monitor_en(mem_type, true, core);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_set_monitor_en() failed (%s) - test_mprot_write\n", esp_err_to_name(err));
        return;
    }

    volatile uint32_t *ptr_low = test_mprot_addr_low(mem_type);
    volatile uint32_t *ptr_high = test_mprot_addr_high(mem_type);
    const uint32_t test_val = 10;

    //perform WRITE in low region
    esp_rom_printf("%s (core %d) write low: ", esp_mprot_mem_type_to_str(mem_type), core);
    err = esp_mprot_monitor_clear_intr(mem_type, core);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_monitor_clear_intr() failed on core %d (%s) - test_mprot_write\n", core, esp_err_to_name(err));
        return;
    }

    volatile uint32_t val = 0;
    *ptr_low = test_val;
    val = *ptr_low;

    if (write_perm_low && val != test_val) {
        esp_rom_printf("UNEXPECTED VALUE 0x%08X -", val);
        test_mprot_dump_status_register(mem_type, core);
    } else {
        test_mprot_check_test_result(mem_type, write_perm_low);
    }

    //perform WRITE in high region
    esp_rom_printf("%s (core %d) write high: ", esp_mprot_mem_type_to_str(mem_type), core);
    err = esp_mprot_monitor_clear_intr(mem_type, core);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_monitor_clear_intr() failed on core %d (%s) - test_mprot_write\n", core, esp_err_to_name(err));
        return;
    }

    val = 0;
    *ptr_high = test_val + 1;
    val = *ptr_high;

    if (val != (test_val + 1) && write_perm_high) {
        esp_rom_printf("UNEXPECTED VALUE 0x%08X -", val);
        test_mprot_dump_status_register(mem_type, core);
    } else {
        test_mprot_check_test_result(mem_type, write_perm_high);
    }

    //restore original permissions
    err = esp_mprot_set_monitor_en(mem_type, false, core);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_set_monitor_en() failed (%s) - test_mprot_write\n", esp_err_to_name(err));
        return;
    }

    test_mprot_set_permissions(LOW_REGION, mem_type, read_perm_low, write_perm_low, is_exec_mem ? &exec_perm_low : NULL, core);
    test_mprot_set_permissions(HIGH_REGION, mem_type, read_perm_high, write_perm_high, is_exec_mem ? &exec_perm_high : NULL, core);

    err = esp_mprot_set_monitor_en(mem_type, true, core);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_set_monitor_en() failed (%s) - test_mprot_write\n", esp_err_to_name(err));
        return;
    }

    esp_mprot_monitor_clear_intr(mem_type, core);
}

static void test_mprot_exec(esp_mprot_mem_t mem_type, const int core)
{
    if (!(mem_type & MEMPROT_TYPE_IRAM0_ANY)) {
        esp_rom_printf("Error: EXEC test available only for IRAM access.\n");
        return;
    }

    esp_err_t err = esp_mprot_monitor_clear_intr(mem_type, core);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_monitor_clear_intr() failed on core %d (%s) - test_mprot_exec\n", core, esp_err_to_name(err));
        return;
    }

    err = esp_mprot_set_monitor_en(mem_type, false, core);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_set_monitor_en() failed (%s) - test_mprot_exec\n", esp_err_to_name(err));
        return;
    }

    //get require mem_type permissions
    bool write_perm_low, write_perm_high, read_perm_low, read_perm_high, exec_perm_low, exec_perm_high;
    test_mprot_get_permissions(LOW_REGION, mem_type, &read_perm_low, &write_perm_low, &exec_perm_low, core);
    test_mprot_get_permissions(HIGH_REGION, mem_type, &read_perm_high, &write_perm_high, &exec_perm_high, core);

    void *fnc_ptr_low = NULL;
    void *fnc_ptr_high = NULL;

    if (mem_type == MEMPROT_TYPE_IRAM0_SRAM) {
        //temporarily enable WRITE for DBUS
        test_mprot_set_permissions(LOW_REGION, MEMPROT_TYPE_DRAM0_SRAM, READ_ENA, WRITE_ENA, NULL, core);

        //get testing pointers for low & high regions using DBUS FOR CODE INJECTION!
        fnc_ptr_low = test_mprot_addr_low(MEMPROT_TYPE_DRAM0_SRAM);
        fnc_ptr_high = test_mprot_addr_high(MEMPROT_TYPE_DRAM0_SRAM);

        //inject the code to both low & high segments
        memcpy(fnc_ptr_low, (const void *) s_fnc_buff, sizeof(s_fnc_buff));
        memcpy(fnc_ptr_high, (const void *) s_fnc_buff, sizeof(s_fnc_buff));

        fnc_ptr_low = (void *) MAP_DRAM_TO_IRAM(fnc_ptr_low);
        fnc_ptr_high = (void *) MAP_DRAM_TO_IRAM(fnc_ptr_high);

        //re-enable DBUS protection
        test_mprot_set_permissions(LOW_REGION, MEMPROT_TYPE_DRAM0_SRAM, READ_ENA, WRITE_DIS, NULL, core);
    } else if (mem_type == MEMPROT_TYPE_IRAM0_RTCFAST) {
        //enable WRITE for low region
        test_mprot_set_permissions(LOW_REGION, MEMPROT_TYPE_IRAM0_RTCFAST, read_perm_low, WRITE_ENA, &exec_perm_low, core);

        fnc_ptr_low = test_mprot_addr_low(MEMPROT_TYPE_IRAM0_RTCFAST);
        fnc_ptr_high = test_mprot_addr_high(MEMPROT_TYPE_IRAM0_RTCFAST);

        //inject the code to both low & high segments
        memcpy(fnc_ptr_low, (const void *) s_fnc_buff, sizeof(s_fnc_buff));
        memcpy(fnc_ptr_high, (const void *) s_fnc_buff, sizeof(s_fnc_buff));

        //re-enable original protection
        test_mprot_set_permissions(LOW_REGION, MEMPROT_TYPE_IRAM0_RTCFAST, read_perm_low, write_perm_low, &exec_perm_low, core);
    } else {
        assert(0);
    }

    uint32_t res = 0;

    //LOW REGION: clear the intr flag & try to execute the code injected
    esp_rom_printf("%s (core %d) exec low: ", esp_mprot_mem_type_to_str(mem_type), core);
    err = esp_mprot_monitor_clear_intr(mem_type, core);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_monitor_clear_intr() failed (%s) - test_mprot_exec\n", esp_err_to_name(err));
        return;
    }

    err = esp_mprot_set_monitor_en(mem_type, true, core);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_set_monitor_en() failed (%s) - test_mprot_exec\n", esp_err_to_name(err));
        return;
    }

    fnc_ptr fnc = (fnc_ptr)fnc_ptr_low;

    g_override_illegal_instruction = true;
    CODE_EXEC(fnc, 5, res)
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
    esp_rom_printf("%s (core %d) exec high: ", esp_mprot_mem_type_to_str(mem_type), core);
    err = esp_mprot_monitor_clear_intr(mem_type, core);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_monitor_clear_intr() failed (%s) - test_mprot_exec\n", esp_err_to_name(err));
        return;
    }

    fnc = (fnc_ptr)fnc_ptr_high;

    g_override_illegal_instruction = true;
    CODE_EXEC(fnc, 6, res)
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

    esp_mprot_monitor_clear_intr(mem_type, core);
}

// testing per-CPU tasks
esp_memp_config_t memp_cfg = {
    .invoke_panic_handler = false,
    .lock_feature = false,
    .split_addr = NULL,
    .mem_type_mask = MEMPROT_TYPE_ALL,
#if configNUM_CORES > 1
    .target_cpu_count = 2,
    .target_cpu = {PRO_CPU_NUM, APP_CPU_NUM}
#else
    .target_cpu_count = 1,
    .target_cpu = {PRO_CPU_NUM}
#endif
};

typedef struct {
    int core;
    SemaphoreHandle_t sem;
} test_ctx_t;

static void task_on_CPU(void *arg)
{
    test_ctx_t *ctx = (test_ctx_t *)arg;

    if (memp_cfg.mem_type_mask & MEMPROT_TYPE_IRAM0_SRAM) {
        test_mprot_read(MEMPROT_TYPE_IRAM0_SRAM, ctx->core);
        test_mprot_write(MEMPROT_TYPE_IRAM0_SRAM, ctx->core);
        test_mprot_exec(MEMPROT_TYPE_IRAM0_SRAM, ctx->core);
    }

    if (memp_cfg.mem_type_mask & MEMPROT_TYPE_DRAM0_SRAM) {
        test_mprot_read(MEMPROT_TYPE_DRAM0_SRAM, ctx->core);
        test_mprot_write(MEMPROT_TYPE_DRAM0_SRAM, ctx->core);
    }

    if (memp_cfg.mem_type_mask & MEMPROT_TYPE_IRAM0_RTCFAST) {
        test_mprot_read(MEMPROT_TYPE_IRAM0_RTCFAST, ctx->core);
        test_mprot_write(MEMPROT_TYPE_IRAM0_RTCFAST, ctx->core);
        test_mprot_exec(MEMPROT_TYPE_IRAM0_RTCFAST, ctx->core);
    }

    xSemaphoreGive(ctx->sem);

    vTaskDelete(NULL);
}

/* ********************************************************************************************
 * main test runner
 */
void app_main(void)
{
    esp_err_t err = esp_mprot_set_prot(&memp_cfg);
    if (err != ESP_OK) {
        esp_rom_printf("Error: esp_mprot_set_prot() failed (%s) - app_main\n", esp_err_to_name(err));
        return;
    }

    //test each core separate
    test_ctx_t ctx = {
        .sem = xSemaphoreCreateBinary(),
        .core = PRO_CPU_NUM
    };

    xTaskCreatePinnedToCore(task_on_CPU, "task_PRO_CPU", 4096, &ctx, 3, NULL, PRO_CPU_NUM);
    xSemaphoreTake(ctx.sem, portMAX_DELAY);

    //multicore setup
    if (configNUM_CORES > 1) {
        ctx.core = APP_CPU_NUM;
        xTaskCreatePinnedToCore(task_on_CPU, "task_APP_CPU", 4096, &ctx, 3, NULL, APP_CPU_NUM);
        xSemaphoreTake(ctx.sem, portMAX_DELAY);
    }
}
