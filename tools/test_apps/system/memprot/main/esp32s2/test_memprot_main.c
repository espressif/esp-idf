/* MEMPROT IramDram testing code */
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp32s2/memprot.h"
#include "soc/soc.h"

/*
 * ESP32S2 MEMORY PROTECTION MODULE TEST
 * =====================================
 *
 * In order to safely test all the mem_prot features configuration, this test uses a combination of
 * proprietary settings and ESP-IDF defaults.
 * Each operation is tested at both low region and high region testing address.
 * Complete testing scheme is depicted below:
 *
 * ********************************************************************************************
 *
 *           IRAM0                 SRAM (320kB)                 DRAM0
 *                          ===========================
 *                          |                         |
 *                          |-------------------------|
 *                          | iram_test_buffer (1kB)  |
 *                      <---------- test addr low ------->  iram_test_buffer + 0x200
 *                          |                         |
 *      _iram_text_end  <======== real splt.addr ========>  _data_start (real splt.addr == test splt.addr)
 *                          |                         |
 *                      <---|-------------------------|-->
 *                          | dram_test_buffer (1kB)  |
 *                      <--------- test addr high ------->  dram_test_buffer + 0x200
 *                          |           ...           |
 *                          |-------------------------|
 *                          |                         |
 *                          ===========================
 *
 *                                 RTC_FAST (8kB)
 *                          ===========================
 *       _rtc_text_end  <======== real splt.addr ========>  _rtc_dummy_end
 *                          |   rtcfast_dummy_buffer  |
 *                          |          (2kB)          |
 *                      <---------- test addr low ------->  test_buffer - 0x200
 *                      <-------- test splt.addr -------->  test_buffer = rtcfast_dummy_buffer / 2
 *                      <---------- test addr low ------->  test_buffer + 0x200
 *                          |-------------------------|
 *                          |                         |
 *                          ===========================
 *
 * ********************************************************************************************
 *
 *       PERIBUS_1               RTC_SLOW (8/768kB)          PERIBUS_2_0   PERIBUS_2_1
 *                          ===========================
 *                          |                         |
 *       0x3F421000     <======== real splt.addr ========>   0x50000000    0x60021000
 *                          |   rtcslow_dummy_buffer  |
 *                          |          (2kB)          |
 *                      <---------- test addr low ------->  test_buffer - 0x200
 *                      <-------- test splt.addr -------->  test_buffer = rtcslow_dummy_buffer / 2
 *                      <---------- test addr low ------->  test_buffer + 0x200
 *                          |-------------------------|
 *                          |                         |
 *                          ===========================
 *
 * ********************************************************************************************
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
static uint8_t fnc_call0_buff[] = {0xf0, 0x22, 0x11, 0x0d, 0xf0, 0x00, 0x00, 0x00};

volatile bool g_override_illegal_instruction = false;

#define SRAM_TEST_BUFFER_SIZE      0x400
#define SRAM_TEST_OFFSET           0x200

static uint8_t __attribute__((section(".iram_end_test"))) iram_test_buffer[SRAM_TEST_BUFFER_SIZE] = {0};
static uint8_t dram_test_buffer[SRAM_TEST_BUFFER_SIZE] = {0};
static uint8_t RTC_FAST_ATTR rtcfast_dummy_buffer[2 * SRAM_TEST_BUFFER_SIZE] = {0};
static uint8_t RTC_SLOW_ATTR rtcslow_dummy_buffer[2 * SRAM_TEST_BUFFER_SIZE] = {0};


/* ********************************************************************************************
 * testing regions and splitting address scheme
 *
 */

static uint32_t *test_memprot_dram0_rtcfast_get_min_split_addr(void)
{
    return (uint32_t *)(rtcfast_dummy_buffer + sizeof(rtcfast_dummy_buffer) / 2);
}

static uint32_t *test_memprot_iram0_rtcfast_get_min_split_addr(void)
{
    return (uint32_t *)
           ((uint32_t)test_memprot_dram0_rtcfast_get_min_split_addr()
            + esp_memprot_get_low_limit(MEMPROT_IRAM0_RTCFAST)
            - esp_memprot_get_low_limit(MEMPROT_DRAM0_RTCFAST));
}

static uint32_t *test_memprot_peri2_rtcslow_0_get_min_split_addr(void)
{
    return (uint32_t *)(rtcslow_dummy_buffer + sizeof(rtcslow_dummy_buffer) / 2);
}

static uint32_t *test_memprot_peri1_rtcslow_get_min_split_addr(void)
{
    return (uint32_t *)
           ((uint32_t)test_memprot_peri2_rtcslow_0_get_min_split_addr()
            - (esp_memprot_get_low_limit(MEMPROT_PERI2_RTCSLOW_0)
               - esp_memprot_get_low_limit(MEMPROT_PERI1_RTCSLOW)));
}

static uint32_t *test_memprot_peri2_rtcslow_1_get_min_split_addr(void)
{
    return (uint32_t *)
           ((uint32_t)test_memprot_peri2_rtcslow_0_get_min_split_addr()
            + esp_memprot_get_low_limit(MEMPROT_PERI2_RTCSLOW_1)
            - esp_memprot_get_low_limit(MEMPROT_PERI2_RTCSLOW_0));
}

static uint32_t *test_memprot_addr_low(mem_type_prot_t mem_type)
{
    switch ( mem_type ) {
    case MEMPROT_IRAM0_SRAM:
        return (uint32_t *)((uint32_t)iram_test_buffer + SRAM_TEST_OFFSET);
    case MEMPROT_DRAM0_SRAM:
        return (uint32_t *)MAP_IRAM_TO_DRAM((uint32_t)iram_test_buffer + SRAM_TEST_OFFSET);
    case MEMPROT_IRAM0_RTCFAST:
        return (uint32_t *)((uint32_t)test_memprot_iram0_rtcfast_get_min_split_addr() - SRAM_TEST_OFFSET);
    case MEMPROT_DRAM0_RTCFAST:
        return (uint32_t *)((uint32_t)test_memprot_dram0_rtcfast_get_min_split_addr() - SRAM_TEST_OFFSET);
    case MEMPROT_PERI1_RTCSLOW:
        return (uint32_t *)((uint32_t)test_memprot_peri1_rtcslow_get_min_split_addr() - SRAM_TEST_OFFSET);
    case MEMPROT_PERI2_RTCSLOW_0:
        return (uint32_t *)((uint32_t)test_memprot_peri2_rtcslow_0_get_min_split_addr() - SRAM_TEST_OFFSET);
    case MEMPROT_PERI2_RTCSLOW_1:
        return (uint32_t *)((uint32_t)test_memprot_peri2_rtcslow_1_get_min_split_addr() - SRAM_TEST_OFFSET);
    default:
        abort();
    }
}

static uint32_t *test_memprot_addr_high(mem_type_prot_t mem_type)
{
    switch ( mem_type ) {
    case MEMPROT_IRAM0_SRAM:
        return (uint32_t *)MAP_DRAM_TO_IRAM((uint32_t)dram_test_buffer + SRAM_TEST_OFFSET);
    case MEMPROT_DRAM0_SRAM:
        return (uint32_t *)((uint32_t)dram_test_buffer + SRAM_TEST_OFFSET);
    case MEMPROT_IRAM0_RTCFAST:
        return (uint32_t *)((uint32_t)test_memprot_iram0_rtcfast_get_min_split_addr() + SRAM_TEST_OFFSET);
    case MEMPROT_DRAM0_RTCFAST:
        return (uint32_t *)((uint32_t)test_memprot_dram0_rtcfast_get_min_split_addr() + SRAM_TEST_OFFSET);
    case MEMPROT_PERI1_RTCSLOW:
        return (uint32_t *)((uint32_t)test_memprot_peri1_rtcslow_get_min_split_addr() + SRAM_TEST_OFFSET);
    case MEMPROT_PERI2_RTCSLOW_0:
        return (uint32_t *)((uint32_t)test_memprot_peri2_rtcslow_0_get_min_split_addr() + SRAM_TEST_OFFSET);
    case MEMPROT_PERI2_RTCSLOW_1:
        return (uint32_t *)((uint32_t)test_memprot_peri2_rtcslow_1_get_min_split_addr() + SRAM_TEST_OFFSET);
    default:
        abort();
    }
}


static uint32_t *test_memprot_get_split_addr(mem_type_prot_t mem_type)
{
    switch (mem_type) {
    case MEMPROT_IRAM0_SRAM:
        return esp_memprot_get_split_addr(MEMPROT_IRAM0_SRAM);
    case MEMPROT_DRAM0_SRAM:
        return esp_memprot_get_split_addr(MEMPROT_DRAM0_SRAM);
    case MEMPROT_IRAM0_RTCFAST:
        return test_memprot_iram0_rtcfast_get_min_split_addr();
    case MEMPROT_DRAM0_RTCFAST:
        return test_memprot_dram0_rtcfast_get_min_split_addr();
    case MEMPROT_PERI1_RTCSLOW:
        return test_memprot_peri1_rtcslow_get_min_split_addr();
    case MEMPROT_PERI2_RTCSLOW_0:
        return test_memprot_peri2_rtcslow_0_get_min_split_addr();
    case MEMPROT_PERI2_RTCSLOW_1:
        return test_memprot_peri2_rtcslow_1_get_min_split_addr();
    default:
        abort();
    }
}


/*
 * testing setup of the memory-protection module
 */
static void test_memprot_set_prot(uint32_t *mem_type_mask, bool use_panic_handler)
{
    //any IRAM0/DRAM0 enable/disable call applies to all memory modules connected
    uint32_t required_mem_prot = mem_type_mask == NULL ? (uint32_t)MEMPROT_ALL : *mem_type_mask;
    bool use_iram0 = required_mem_prot & MEMPROT_IRAM0_SRAM || required_mem_prot & MEMPROT_IRAM0_RTCFAST;
    bool use_dram0 = required_mem_prot & MEMPROT_DRAM0_SRAM || required_mem_prot & MEMPROT_DRAM0_RTCFAST;
    bool use_peri1 = required_mem_prot & MEMPROT_PERI1_RTCSLOW;
    bool use_peri2 = required_mem_prot & MEMPROT_PERI2_RTCSLOW_0 || required_mem_prot & MEMPROT_PERI2_RTCSLOW_1;

    //disable protection
    if (use_iram0) {
        esp_memprot_intr_ena(MEMPROT_IRAM0_SRAM, false);
    }
    if (use_dram0) {
        esp_memprot_intr_ena(MEMPROT_DRAM0_SRAM, false);
    }
    if (use_peri1) {
        esp_memprot_intr_ena(MEMPROT_PERI1_RTCSLOW, false);
    }
    if (use_peri2) {
        esp_memprot_intr_ena(MEMPROT_PERI2_RTCSLOW_0, false);
    }

    if ( use_panic_handler ) {
        if (use_iram0) {
            esp_memprot_intr_init(MEMPROT_IRAM0_SRAM);
        }
        if (use_dram0) {
            esp_memprot_intr_init(MEMPROT_DRAM0_SRAM);
        }
        if (use_peri1) {
            esp_memprot_intr_init(MEMPROT_PERI1_RTCSLOW);
        }
        if (use_peri2) {
            esp_memprot_intr_init(MEMPROT_PERI2_RTCSLOW_0);
        }
    }

    //set permissions
    if (required_mem_prot & MEMPROT_IRAM0_SRAM) {
        esp_memprot_set_prot_iram(MEMPROT_IRAM0_SRAM, DEF_SPLIT_LINE, WR_LOW_DIS, RD_LOW_ENA, EX_LOW_ENA, WR_HIGH_DIS, RD_HIGH_DIS, EX_HIGH_DIS);
    }
    if (required_mem_prot & MEMPROT_IRAM0_RTCFAST) {
        esp_memprot_set_prot_iram(MEMPROT_IRAM0_RTCFAST, test_memprot_iram0_rtcfast_get_min_split_addr(), WR_LOW_DIS, RD_LOW_ENA, EX_LOW_ENA, WR_HIGH_DIS, RD_HIGH_DIS, EX_HIGH_DIS);
    }
    if (required_mem_prot & MEMPROT_DRAM0_SRAM) {
        esp_memprot_set_prot_dram(MEMPROT_DRAM0_SRAM, DEF_SPLIT_LINE, WR_LOW_DIS, RD_LOW_ENA, WR_HIGH_ENA, RD_HIGH_ENA);
    }
    if (required_mem_prot & MEMPROT_DRAM0_RTCFAST) {
        esp_memprot_set_prot_dram(MEMPROT_DRAM0_RTCFAST, test_memprot_dram0_rtcfast_get_min_split_addr(), WR_LOW_DIS, RD_LOW_ENA, WR_HIGH_ENA, RD_HIGH_ENA);
    }
    if (required_mem_prot & MEMPROT_PERI1_RTCSLOW) {
        esp_memprot_set_prot_peri1(MEMPROT_PERI1_RTCSLOW, test_memprot_peri1_rtcslow_get_min_split_addr(), WR_LOW_DIS, RD_LOW_DIS, RD_HIGH_DIS, WR_HIGH_DIS);
    }
    if (required_mem_prot & MEMPROT_PERI2_RTCSLOW_0) {
        esp_memprot_set_prot_peri2(MEMPROT_PERI2_RTCSLOW_0, test_memprot_peri2_rtcslow_0_get_min_split_addr(), WR_LOW_ENA, RD_LOW_ENA, EX_LOW_DIS, WR_HIGH_ENA, RD_HIGH_ENA, EX_HIGH_DIS);
    }
    if (required_mem_prot & MEMPROT_PERI2_RTCSLOW_1) {
        esp_memprot_set_prot_peri2(MEMPROT_PERI2_RTCSLOW_1, test_memprot_peri2_rtcslow_1_get_min_split_addr(), WR_LOW_DIS, RD_LOW_DIS, EX_LOW_DIS, WR_HIGH_DIS, RD_HIGH_DIS, EX_HIGH_DIS);
    }

    //reenable protection (bus based)
    if (use_iram0) {
        esp_memprot_intr_ena(MEMPROT_IRAM0_SRAM, true);
    }
    if (use_dram0) {
        esp_memprot_intr_ena(MEMPROT_DRAM0_SRAM, true);
    }
    if (use_peri1) {
        esp_memprot_intr_ena(MEMPROT_PERI1_RTCSLOW, true);
    }
    if (use_peri2) {
        esp_memprot_intr_ena(MEMPROT_PERI2_RTCSLOW_0, true);
    }
}


/* ********************************************************************************************
 * auxiliary functions
 */
static void __attribute__((unused)) dump_fnc_buff(uint32_t *buff)
{
    esp_rom_printf( "0x%08X: 0x%08X-0x%08X\n", (uint32_t)buff, buff[0], buff[1] );
}

static void __attribute__((unused)) dump_bus_permissions(mem_type_prot_t mem_type)
{
    bool write_perm_low, write_perm_high, read_perm_low, read_perm_high, exec_perm_low, exec_perm_high;
    esp_memprot_get_permissions(mem_type, &write_perm_low, &write_perm_high, &read_perm_low, &read_perm_high, &exec_perm_low, &exec_perm_high);

    esp_rom_printf("%s permissions: LW=%u LR=%u LX=%u HW=%u HR=%u HX=%u\n", esp_memprot_type_to_str(mem_type),
                   write_perm_low, read_perm_low, exec_perm_low, write_perm_high, read_perm_high, exec_perm_high);
}

static void __attribute__((unused)) dump_status_register(mem_type_prot_t mem_type)
{
    uint32_t *faulting_address, op_type, op_subtype;
    esp_err_t res = esp_memprot_get_fault_status(mem_type, &faulting_address, &op_type, &op_subtype);
    if ( res == ESP_OK ) {
        uint32_t fault_reg;
        res = esp_memprot_get_fault_reg(mem_type, &fault_reg);
        esp_rom_printf(
            " FAULT [split addr: 0x%08X, fault addr: 0x%08X, fault status: ",
            (uint32_t) test_memprot_get_split_addr(mem_type),
            (uint32_t) faulting_address
        );
        if ( res == ESP_OK ) {
            esp_rom_printf("0x%08X]\n", fault_reg );
        } else {
            esp_rom_printf("<failed, err: 0x%08X>]\n", res );
        }
    } else {
        esp_rom_printf(" FAULT [failed to get fault details, error 0x%08X]\n", res);
    }
}


/* ********************************************************************************************
 * testing functions
 */
static void check_test_result(mem_type_prot_t mem_type, bool expected_status)
{
    uint32_t fault;
    esp_err_t res = esp_memprot_get_fault_reg(mem_type, &fault);
    if ( res == ESP_OK ) {
        bool test_result = expected_status ? fault == 0 : fault != 0;
        if (test_result) {
            esp_rom_printf("OK\n");
        } else {
            dump_status_register(mem_type);
        }
    } else {
        esp_rom_printf(" FAULT [failed to get test results, error 0x%08X]\n", res);
    }
}

static void test_memprot_read(mem_type_prot_t mem_type)
{
    //get current READ & WRITE permission settings
    bool write_perm_low, write_perm_high, read_perm_low, read_perm_high;
    esp_memprot_get_perm_write(mem_type, &write_perm_low, &write_perm_high);
    esp_memprot_get_perm_read(mem_type, &read_perm_low, &read_perm_high);

    volatile uint32_t *ptr_low = test_memprot_addr_low(mem_type);
    volatile uint32_t *ptr_high = test_memprot_addr_high(mem_type);

    //temporarily allow WRITE for setting the test values
    esp_memprot_set_write_perm(mem_type, true, true);

    const uint32_t test_val = 100;
    *ptr_low = test_val;
    *ptr_high = test_val + 1;

    esp_memprot_set_write_perm(mem_type, write_perm_low, write_perm_high);

    //perform READ in low region
    esp_rom_printf("%s read low: ", esp_memprot_type_to_str(mem_type));
    esp_memprot_clear_intr(mem_type);
    volatile uint32_t val = *ptr_low;
    if ( val != 0 && val != test_val ) {
        esp_rom_printf( "UNEXPECTED VALUE 0x%08X -", val );
        dump_status_register(mem_type);
    } else {
        check_test_result(mem_type, val == test_val);
    }

    //perform read in high region
    esp_rom_printf("%s read high: ", esp_memprot_type_to_str(mem_type));
    esp_memprot_clear_intr(mem_type);
    val = *ptr_high;
    if ( val != 0 && val != (test_val + 1) ) {
        esp_rom_printf( "UNEXPECTED VALUE 0x%08X -", val);
        dump_status_register(mem_type);
    } else {
        check_test_result(mem_type, val == (test_val + 1));
    }
}

static void test_memprot_write(mem_type_prot_t mem_type)
{
    //get current READ & WRITE permission settings
    bool write_perm_low, write_perm_high, read_perm_low, read_perm_high;
    esp_memprot_get_perm_write(mem_type, &write_perm_low, &write_perm_high);
    esp_memprot_get_perm_read(mem_type, &read_perm_low, &read_perm_high);

    //temporarily allow READ operation
    esp_memprot_set_read_perm(mem_type, true, true);

    volatile uint32_t *ptr_low = test_memprot_addr_low(mem_type);
    volatile uint32_t *ptr_high = test_memprot_addr_high(mem_type);

    //perform WRITE in low region
    const uint32_t test_val = 10;
    esp_rom_printf("%s write low: ", esp_memprot_type_to_str(mem_type));
    esp_memprot_clear_intr(mem_type);

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
    esp_rom_printf("%s write high: ", esp_memprot_type_to_str(mem_type));
    esp_memprot_clear_intr(mem_type);
    val = 0;
    *ptr_high = test_val + 1;
    val = *ptr_high;

    if ( val != (test_val + 1) && write_perm_high ) {
        esp_rom_printf( "UNEXPECTED VALUE 0x%08X -", val);
        dump_status_register(mem_type);
    } else {
        check_test_result(mem_type, write_perm_high);
    }

    esp_memprot_set_read_perm(mem_type, read_perm_low, read_perm_high);
}

static void test_memprot_exec(mem_type_prot_t mem_type)
{
    //store current write permissions
    bool write_perm_low, write_perm_high;
    esp_memprot_get_perm_write(mem_type, &write_perm_low, &write_perm_high);

    //get current EXEC permission settings
    bool exec_perm_low, exec_perm_high;
    esp_memprot_get_perm_exec(mem_type, &exec_perm_low, &exec_perm_high);

    volatile uint32_t *fnc_ptr_low = test_memprot_addr_low(mem_type);
    volatile uint32_t *fnc_ptr_high = test_memprot_addr_high(mem_type);

    //enable WRITE permission for both segments
    esp_memprot_set_write_perm(mem_type, true, true);

    //inject the code to both low & high segments
    memcpy( (void *)fnc_ptr_low, fnc_call0_buff, sizeof(fnc_call0_buff) );
    memcpy( (void *)fnc_ptr_high, fnc_call0_buff, sizeof(fnc_call0_buff) );

    //restore original WRITE perms
    esp_memprot_set_write_perm(mem_type, write_perm_low, write_perm_high);

    uint32_t res = 0;

    //LOW REGION: clear the intr flag & try to execute the code injected
    esp_memprot_clear_intr(mem_type);
    esp_rom_printf("%s exec low: ", esp_memprot_type_to_str(mem_type));

    g_override_illegal_instruction = true;
    CODE_EXEC(fnc_ptr_low, 5, res);
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

    //HIGH REGION: clear the intr flag & try to execute the code injected
    esp_memprot_clear_intr(mem_type);
    esp_rom_printf("%s exec high: ", esp_memprot_type_to_str(mem_type));

    g_override_illegal_instruction = true;
    CODE_EXEC(fnc_ptr_high, 6, res);
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
}

/* ********************************************************************************************
 * main test runner
 */
void app_main(void)
{
    test_memprot_set_prot(NULL, false);

    test_memprot_read(MEMPROT_IRAM0_SRAM);
    test_memprot_write(MEMPROT_IRAM0_SRAM);
    test_memprot_exec(MEMPROT_IRAM0_SRAM);

    test_memprot_read(MEMPROT_IRAM0_RTCFAST);
    test_memprot_write(MEMPROT_IRAM0_RTCFAST);
    test_memprot_exec(MEMPROT_IRAM0_RTCFAST);

    test_memprot_read(MEMPROT_DRAM0_SRAM);
    test_memprot_write(MEMPROT_DRAM0_SRAM);

    test_memprot_read(MEMPROT_DRAM0_RTCFAST);
    test_memprot_write(MEMPROT_DRAM0_RTCFAST);

    test_memprot_read(MEMPROT_PERI1_RTCSLOW);
    test_memprot_write(MEMPROT_PERI1_RTCSLOW);

    test_memprot_read(MEMPROT_PERI2_RTCSLOW_0);
    test_memprot_write(MEMPROT_PERI2_RTCSLOW_0);
    test_memprot_exec(MEMPROT_PERI2_RTCSLOW_0);

    test_memprot_read(MEMPROT_PERI2_RTCSLOW_1);
    test_memprot_write(MEMPROT_PERI2_RTCSLOW_1);
    test_memprot_exec(MEMPROT_PERI2_RTCSLOW_1);
}
