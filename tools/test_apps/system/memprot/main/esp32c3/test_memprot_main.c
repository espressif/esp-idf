/* MEMPROT (PMS) testing code */
#include <stdio.h>
#include "sdkconfig.h"
#include "esp32c3/memprot.h"
#include "esp_rom_sys.h"
#include <string.h>

/**
 * ESP32C3 MEMORY PROTECTION MODULE TEST
 * =====================================
 *
 * In order to safely test all the memprot features, this test application uses memprot default settings
 * plus proprietary testing buffers:
 *      - iram_test_buffer (.iram_end_test, 1kB) - all low region operations
 *      - dram_test_buffer (.dram0.data, 1kB) - all high region operations
 * Testing addresses are set to the middle of the testing buffers:
 *      - test_ptr_low = iram_test_buffer + 0x200
 *      - test_ptr_high = dram_test_buffer + 0x200
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
 *                              | - - - - - - - iram_test_buffer - - - - - - -|   IRam0_line1_Split_addr
 * DRam0_DMA_line0_Split_addr   |               -- test_ptr_low --            |             =
 *               =              ===============================================   IRam0_line0_Split_addr
 * DRam0_DMA_line1_Split_addr   |                                             |             =
 *                              | - - - - - - - dram_test_buffer - - - - - - -|   IRam0_DRam0_Split_addr (main I/D)
 *                              |              -- test_ptr_high --            |
 *                              | - - - - - - - - - - - - - - - - - - - - - - |
 *                              |                                             |
 *                              |   DRam0_PMS_1 = DRam0_PMS_2 = DRam0_PMS_3   |
 *                              |                 IRam0_PMS_3                 |
 *                              -----------------------------------------------
 *
 * For more details on PMS memprot settings see 'esp_memprot_set_prot_int' function in esp32c3/memprot.h
 */


/* Binary code for the following asm [int func(int x) { return x+x; }]
    slli a0,a0,0x1
    ret
 */
static uint8_t fnc_buff[] = { 0x06, 0x05, 0x82, 0x80 };
typedef int (*fnc_ptr)(int);

#define SRAM_TEST_BUFFER_SIZE      0x400
#define SRAM_TEST_OFFSET           0x200

static uint8_t __attribute__((section(".iram_end_test"))) iram_test_buffer[SRAM_TEST_BUFFER_SIZE] = {0};
static uint8_t dram_test_buffer[SRAM_TEST_BUFFER_SIZE] = {0};
extern volatile bool g_override_illegal_instruction;


static void *test_memprot_addr_low(mem_type_prot_t mem_type)
{
    switch ( mem_type ) {
        case MEMPROT_IRAM0_SRAM:
            return (void*)((uint32_t)iram_test_buffer + SRAM_TEST_OFFSET);
        case MEMPROT_DRAM0_SRAM:
            return (void*)MAP_IRAM_TO_DRAM((uint32_t)iram_test_buffer + SRAM_TEST_OFFSET);
        default:
            abort();
    }
}

static void *test_memprot_addr_high(mem_type_prot_t mem_type)
{
    switch ( mem_type ) {
        case MEMPROT_IRAM0_SRAM:
            return (void*)MAP_DRAM_TO_IRAM((uint32_t)dram_test_buffer + SRAM_TEST_OFFSET);
        case MEMPROT_DRAM0_SRAM:
            return (void*)((uint32_t)dram_test_buffer + SRAM_TEST_OFFSET);
        default:
            abort();
    }
}

static void __attribute__((unused)) dump_status_register(mem_type_prot_t mem_type)
{
    char operation = 0;

    // IRAM fault: check instruction-fetch flag
    if ( mem_type == MEMPROT_IRAM0_SRAM ) {
        if ( esp_memprot_get_violate_loadstore(mem_type) ) {
            operation = 'X';
        }
    }

    // W/R - common
    if ( operation == 0 ) {
        operation = esp_memprot_get_violate_wr(mem_type) == MEMPROT_PMS_OP_WRITE ? 'W' : 'R';
    }

    esp_rom_printf(
        " FAULT [ world: %u, fault addr: 0x%08X, operation: %c",
        esp_memprot_get_violate_world(mem_type),
        esp_memprot_get_violate_addr(mem_type),
        operation
    );

    // DRAM/DMA fault: check byte-enables
    if ( mem_type == MEMPROT_DRAM0_SRAM ) {
        esp_rom_printf( ", byte en: 0x%08X", esp_memprot_get_violate_byte_en(mem_type) );
    }

    esp_rom_printf( " ]\n" );
}

static void check_test_result(mem_type_prot_t mem_type, bool expected_status)
{
    bool test_result =
            expected_status ?
            !esp_memprot_get_violate_intr_on(mem_type) :
            esp_memprot_get_violate_intr_on(mem_type);

    if ( test_result ) {
        esp_rom_printf("OK\n");
    } else {
        dump_status_register(mem_type);
    }
}

static void test_memprot_get_permissions(bool low, mem_type_prot_t mem_type, bool *read, bool *write, bool *exec )
{
    bool _r, _w, _x;
    pms_area_t area = MEMPROT_PMS_AREA_NONE;

    switch ( mem_type ) {
    case MEMPROT_IRAM0_SRAM:
        area = low ? MEMPROT_IRAM0_PMS_AREA_2 : MEMPROT_IRAM0_PMS_AREA_3;
        esp_memprot_iram_get_pms_area(area, &_r, &_w, &_x);
        break;
    case MEMPROT_DRAM0_SRAM:
        area = low ? MEMPROT_DRAM0_PMS_AREA_0 : MEMPROT_DRAM0_PMS_AREA_1;
        esp_memprot_dram_get_pms_area(area, &_r, &_w);
        break;
    default:
        abort();
    }

    if ( read ) {
        *read = _r;
    }
    if ( write ) {
        *write = _w;
    }
    if ( exec ) {
        *exec = _x;
    }
}

static void test_memprot_set_permissions(bool low, mem_type_prot_t mem_type, bool read, bool write, bool *exec)
{
    switch ( mem_type ) {
    case MEMPROT_IRAM0_SRAM: {
        bool _ex;
        if (!exec) {
            test_memprot_get_permissions( low, mem_type, NULL, NULL, &_ex);
            exec = &_ex;
        }
        if (low) {
            esp_memprot_iram_set_pms_area(MEMPROT_IRAM0_PMS_AREA_0, read, write, *exec);
            esp_memprot_iram_set_pms_area(MEMPROT_IRAM0_PMS_AREA_1, read, write, *exec);
            esp_memprot_iram_set_pms_area(MEMPROT_IRAM0_PMS_AREA_2, read, write, *exec);
        } else {
            esp_memprot_iram_set_pms_area(MEMPROT_IRAM0_PMS_AREA_3, read, write, *exec);
        }
    }
    break;
    case MEMPROT_DRAM0_SRAM: {
        if (low) {
            esp_memprot_dram_set_pms_area( MEMPROT_DRAM0_PMS_AREA_0, read, write );
        } else {
            esp_memprot_dram_set_pms_area(MEMPROT_DRAM0_PMS_AREA_1, read, write);
            esp_memprot_dram_set_pms_area(MEMPROT_DRAM0_PMS_AREA_2, read, write);
            esp_memprot_dram_set_pms_area(MEMPROT_DRAM0_PMS_AREA_3, read, write);
        }
    }
    break;
    default:
        abort();
    }
}

static void test_memprot_read(mem_type_prot_t mem_type)
{
    //get current READ & WRITE permission settings
    bool write_perm_low, write_perm_high, read_perm_low, read_perm_high, exec_perm_low, exec_perm_high;
    test_memprot_get_permissions(true, mem_type, &read_perm_low, &write_perm_low, mem_type == MEMPROT_IRAM0_SRAM ? &exec_perm_low : NULL);
    test_memprot_get_permissions(false, mem_type, &read_perm_high, &write_perm_high, mem_type == MEMPROT_IRAM0_SRAM ? &exec_perm_high : NULL);

    //get testing pointers for low & high regions
    volatile uint32_t *ptr_low = test_memprot_addr_low(mem_type);
    volatile uint32_t *ptr_high = test_memprot_addr_high(mem_type);
    const uint32_t test_val = 100;

    //temporarily allow WRITE for setting the test values
    esp_memprot_set_monitor_en(mem_type, false);
    test_memprot_set_permissions(true, mem_type, read_perm_low, true, mem_type == MEMPROT_IRAM0_SRAM ? &exec_perm_low : NULL);
    test_memprot_set_permissions(false, mem_type, read_perm_high, true, mem_type == MEMPROT_IRAM0_SRAM ? &exec_perm_high : NULL);

    *ptr_low = test_val;
    *ptr_high = test_val + 1;

    test_memprot_set_permissions(true, mem_type, read_perm_low, write_perm_low, mem_type == MEMPROT_IRAM0_SRAM ? &exec_perm_low : NULL);
    test_memprot_set_permissions(false, mem_type, read_perm_high, write_perm_high, mem_type == MEMPROT_IRAM0_SRAM ? &exec_perm_high : NULL);
    esp_memprot_set_monitor_en(mem_type, true);

    //perform READ test in low region
    esp_rom_printf("%s read low: ", esp_memprot_mem_type_to_str(mem_type));
    esp_memprot_monitor_clear_intr(mem_type);

    volatile uint32_t val = *ptr_low;

    if ( read_perm_low && val != test_val ) {
        esp_rom_printf( "UNEXPECTED VALUE 0x%08X -", val );
        dump_status_register(mem_type);
    } else {
        check_test_result(mem_type, read_perm_low);
    }

    //perform READ in high region
    esp_rom_printf("%s read high: ", esp_memprot_mem_type_to_str(mem_type));
    esp_memprot_monitor_clear_intr(mem_type);

    val = *ptr_high;

    if ( read_perm_high && val != (test_val + 1) ) {
        esp_rom_printf( "UNEXPECTED VALUE 0x%08X -", val);
        dump_status_register(mem_type);
    } else {
        check_test_result(mem_type, read_perm_high);
    }
}

static void test_memprot_write(mem_type_prot_t mem_type)
{
    //get current READ & WRITE permission settings
    bool write_perm_low, write_perm_high, read_perm_low, read_perm_high;
    test_memprot_get_permissions(true, mem_type, &read_perm_low, &write_perm_low, NULL);
    test_memprot_get_permissions(false, mem_type, &read_perm_high, &write_perm_high, NULL);

    //ensure READ enabled
    esp_memprot_set_monitor_en(mem_type, false);
    test_memprot_set_permissions(true, mem_type, true, write_perm_low, NULL);
    test_memprot_set_permissions(false, mem_type, true, write_perm_high, NULL);
    esp_memprot_set_monitor_en(mem_type, true);

    //get testing pointers for low & high regions
    volatile uint32_t *ptr_low = test_memprot_addr_low(mem_type);
    volatile uint32_t *ptr_high = test_memprot_addr_high(mem_type);
    const uint32_t test_val = 10;

    //perform WRITE in low region
    esp_rom_printf("%s write low: ", esp_memprot_mem_type_to_str(mem_type));
    esp_memprot_monitor_clear_intr(mem_type);

    volatile uint32_t val = 0;
    *ptr_low = test_val;
    val = *ptr_low;

    if ( val != test_val && write_perm_low ) {
        esp_rom_printf( "UNEXPECTED VALUE 0x%08X -", val);
        dump_status_register(mem_type);
    } else {
        check_test_result(mem_type, write_perm_low);
    }

    //perform WRITE in high region
    esp_rom_printf("%s write high: ", esp_memprot_mem_type_to_str(mem_type));
    esp_memprot_monitor_clear_intr(mem_type);

    val = 0;
    *ptr_high = test_val + 1;
    val = *ptr_high;

    if ( val != (test_val + 1) && write_perm_high ) {
        esp_rom_printf( "UNEXPECTED VALUE 0x%08X -", val);
        dump_status_register(mem_type);
    } else {
        check_test_result(mem_type, write_perm_high);
    }

    //restore original permissions
    esp_memprot_set_monitor_en(mem_type, false);
    test_memprot_set_permissions(true, mem_type, true, read_perm_low, NULL);
    test_memprot_set_permissions(false, mem_type, true, read_perm_high, NULL);
    esp_memprot_set_monitor_en(mem_type, true);
}

static void test_memprot_exec(mem_type_prot_t mem_type)
{
    if ( mem_type != MEMPROT_IRAM0_SRAM ) {
        esp_rom_printf("Error: EXEC test available only for IRAM access.\n" );
        return;
    }

    bool write_perm_low, write_perm_high, read_perm_low, read_perm_high, exec_perm_low, exec_perm_high;

    //temporarily enable READ/WRITE
    test_memprot_get_permissions(true, mem_type, &read_perm_low, &write_perm_low, &exec_perm_low);
    test_memprot_get_permissions(false, mem_type, &read_perm_high, &write_perm_high, &exec_perm_high);
    esp_memprot_set_monitor_en(mem_type, false);
    test_memprot_set_permissions(true, mem_type, true, true, &exec_perm_low);
    test_memprot_set_permissions(false, mem_type, true, true, &exec_perm_high);
    esp_memprot_set_monitor_en(mem_type, true);

    //get testing pointers for low & high regions, zero 8B slot
    void *fnc_ptr_low = test_memprot_addr_low(mem_type);
    void *fnc_ptr_high = test_memprot_addr_high(mem_type);
    memset( fnc_ptr_low, 0, 8);
    memset( fnc_ptr_high, 0, 8);

    //inject the code to both low & high segments
    memcpy( (void *)fnc_ptr_low, (const void *)fnc_buff, sizeof(fnc_buff) );
    memcpy( (void *)fnc_ptr_high, (const void *)fnc_buff, sizeof(fnc_buff) );

    uint32_t res = 0;

    //LOW REGION: clear the intr flag & try to execute the code injected
    esp_rom_printf("%s exec low: ", esp_memprot_mem_type_to_str(mem_type));
    esp_memprot_monitor_clear_intr(mem_type);

    fnc_ptr fnc = (fnc_ptr)fnc_ptr_low;

    g_override_illegal_instruction = true;
    res = fnc( 5 );
    g_override_illegal_instruction = false;

    //check results
    bool fnc_call_ok = res == 10;
    if ( fnc_call_ok ) {
        check_test_result(mem_type, exec_perm_low);
    } else {
        if ( !exec_perm_low ) {
            check_test_result(mem_type, false);
        } else {
            esp_rom_printf(" FAULT [injected code not executed]\n");
        }
    }

    //HIGH REGION: clear the intr-on flag & try to execute the code injected
    esp_rom_printf("%s exec high: ", esp_memprot_mem_type_to_str(mem_type));
    esp_memprot_monitor_clear_intr(mem_type);

    fnc = (fnc_ptr)fnc_ptr_high;

    g_override_illegal_instruction = true;
    res = fnc( 6 );
    g_override_illegal_instruction = false;

    fnc_call_ok = res == 12;
    if ( fnc_call_ok ) {
        check_test_result(mem_type, exec_perm_high);
    } else {
        if ( !exec_perm_high ) {
            check_test_result(mem_type, false);
        } else {
            esp_rom_printf(" FAULT [injected code not executed]\n");
        }
    }

    //restore original permissions
    esp_memprot_set_monitor_en(mem_type, false);
    test_memprot_set_permissions(true, mem_type, read_perm_low, write_perm_low, &exec_perm_low);
    test_memprot_set_permissions(false, mem_type, read_perm_high, write_perm_high, &exec_perm_high);
    esp_memprot_set_monitor_en(mem_type, true);
}


/* ********************************************************************************************
 * main test runner
 */
void app_main(void)
{
    esp_memprot_set_prot_int(false, false, NULL, NULL);

    test_memprot_read(MEMPROT_IRAM0_SRAM);
    test_memprot_write(MEMPROT_IRAM0_SRAM);
    test_memprot_exec(MEMPROT_IRAM0_SRAM);

    test_memprot_read(MEMPROT_DRAM0_SRAM);
    test_memprot_write(MEMPROT_DRAM0_SRAM);
}
