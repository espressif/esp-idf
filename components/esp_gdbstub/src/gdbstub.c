/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_gdbstub.h"
#include "esp_gdbstub_common.h"
#include "esp_gdbstub_memory_regions.h"
#include "sdkconfig.h"
#include <sys/param.h>

#include "soc/soc_caps.h"
#include "soc/uart_reg.h"
#include "soc/periph_defs.h"
#include "esp_attr.h"
#include "esp_cpu.h"
#include "esp_log.h"
#include "esp_intr_alloc.h"
#include "hal/wdt_hal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifdef CONFIG_ESP_GDBSTUB_SUPPORT_TASKS
static inline int gdb_tid_to_task_index(int tid);
static inline int task_index_to_gdb_tid(int tid);
static void init_task_info(void);
static void find_paniced_task_index(void);
static void set_active_task(size_t index);
static int handle_task_commands(unsigned char *cmd, int len);
static void esp_gdbstub_send_str_as_hex(const char *str);
#endif

#ifdef CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
static void handle_qSupported_command(const unsigned char *cmd, int len);
#endif // CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME

#if (CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME || CONFIG_ESP_GDBSTUB_SUPPORT_TASKS)
static bool command_name_matches(const char *pattern, const unsigned char *ucmd, int len);
#endif // (CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME || CONFIG_ESP_GDBSTUB_SUPPORT_TASKS)

static void send_reason(void);
static char gdb_packet(char *dest_buff, char *src_buff, int len);

esp_gdbstub_scratch_t s_scratch;
esp_gdbstub_gdb_regfile_t *gdb_local_regfile = &s_scratch.regfile;

/**
 * @brief panic handler
*/
void esp_gdbstub_panic_handler(void *in_frame)
{
    esp_gdbstub_frame_t *frame = (esp_gdbstub_frame_t *)in_frame;
#ifndef CONFIG_ESP_GDBSTUB_SUPPORT_TASKS
    esp_gdbstub_frame_to_regfile(frame, &s_scratch.regfile);
#else
    if (s_scratch.state == GDBSTUB_STARTED) {
        /* We have re-entered GDB Stub. Try disabling task support. */
        s_scratch.state = GDBSTUB_TASK_SUPPORT_DISABLED;
        /* Flush any pending GDB packet (this creates a garbage value) */
        esp_gdbstub_send_end();
    } else if (s_scratch.state == GDBSTUB_NOT_STARTED) {
        s_scratch.state = GDBSTUB_STARTED;
        /* Save the panicked frame and get the list of tasks */
        memcpy(&s_scratch.paniced_frame, frame, sizeof(*frame));
        init_task_info();
        find_paniced_task_index();
        /* Current task is the panicked task */
        if (s_scratch.paniced_task_index == GDBSTUB_CUR_TASK_INDEX_UNKNOWN) {
            set_active_task(0);
        } else {
            set_active_task(s_scratch.paniced_task_index);
        }
    }
#endif /* CONFIG_ESP_GDBSTUB_SUPPORT_TASKS */

    s_scratch.signal = esp_gdbstub_get_signal(frame);
    send_reason();
    while (true) {
        unsigned char *cmd;
        size_t size;
        int res = esp_gdbstub_read_command(&cmd, &size);
        if (res > 0) {
            /* character received instead of a command */
            continue;
        }
        if (res == GDBSTUB_ST_ERR) {
            esp_gdbstub_send_str_packet("E01");
            continue;
        }
        res = esp_gdbstub_handle_command(cmd, size);
        if (res == GDBSTUB_ST_ERR) {
            esp_gdbstub_send_str_packet(NULL);
        }
    }
}

/**
 * Set interrupt reason to GDB
*/
static void send_reason(void)
{
    esp_gdbstub_send_start();
    esp_gdbstub_send_char('T');
    esp_gdbstub_send_hex(s_scratch.signal, 8);
    esp_gdbstub_send_end();
}

/**
 * Swap bytes in word
*/
static uint32_t gdbstub_hton(uint32_t i)
{
    return __builtin_bswap32(i);
}

static wdt_hal_context_t rtc_wdt_ctx = RWDT_HAL_CONTEXT_DEFAULT();
static bool rtc_wdt_ctx_enabled = false;
static wdt_hal_context_t wdt0_context = {.inst = WDT_MWDT0, .mwdt_dev = &TIMERG0};
static bool wdt0_context_enabled = false;
#if SOC_TIMER_GROUPS >= 2
static wdt_hal_context_t wdt1_context = {.inst = WDT_MWDT1, .mwdt_dev = &TIMERG1};
static bool wdt1_context_enabled = false;
#endif // SOC_TIMER_GROUPS

/**
 * Disable all enabled WDTs
 */
static inline void disable_all_wdts(void)
{
    wdt0_context_enabled = wdt_hal_is_enabled(&wdt0_context);
    #if SOC_TIMER_GROUPS >= 2
    wdt1_context_enabled = wdt_hal_is_enabled(&wdt1_context);
    #endif
    rtc_wdt_ctx_enabled = wdt_hal_is_enabled(&rtc_wdt_ctx);

    /*Task WDT is the Main Watchdog Timer of Timer Group 0 */
    if (true == wdt0_context_enabled) {
        wdt_hal_write_protect_disable(&wdt0_context);
        wdt_hal_disable(&wdt0_context);
        wdt_hal_feed(&wdt0_context);
        wdt_hal_write_protect_enable(&wdt0_context);
    }

    #if SOC_TIMER_GROUPS >= 2
    /* Interrupt WDT is the Main Watchdog Timer of Timer Group 1 */
    if (true == wdt1_context_enabled) {
        wdt_hal_write_protect_disable(&wdt1_context);
        wdt_hal_disable(&wdt1_context);
        wdt_hal_feed(&wdt1_context);
        wdt_hal_write_protect_enable(&wdt1_context);
    }
    #endif // SOC_TIMER_GROUPS >= 2

    if (true == rtc_wdt_ctx_enabled) {
        wdt_hal_write_protect_disable(&rtc_wdt_ctx);
        wdt_hal_disable(&rtc_wdt_ctx);
        wdt_hal_feed(&rtc_wdt_ctx);
        wdt_hal_write_protect_enable(&rtc_wdt_ctx);
    }
}

/**
 * Enable all enabled WDTs
 */
static inline void enable_all_wdts(void)
{
    /* Task WDT is the Main Watchdog Timer of Timer Group 0 */
    if (false == wdt0_context_enabled) {
        wdt_hal_write_protect_disable(&wdt0_context);
        wdt_hal_enable(&wdt0_context);
        wdt_hal_write_protect_enable(&wdt0_context);
    }
    #if SOC_TIMER_GROUPS >= 2
    /* Interrupt WDT is the Main Watchdog Timer of Timer Group 1 */
    if (false == wdt1_context_enabled) {
        wdt_hal_write_protect_disable(&wdt1_context);
        wdt_hal_enable(&wdt1_context);
        wdt_hal_write_protect_enable(&wdt1_context);
    }
    #endif // SOC_TIMER_GROUPS >= 2

    if (false == rtc_wdt_ctx_enabled) {
        wdt_hal_write_protect_disable(&rtc_wdt_ctx);
        wdt_hal_enable(&rtc_wdt_ctx);
        wdt_hal_write_protect_enable(&rtc_wdt_ctx);
    }
}

int getActiveTaskNum(void);
int __swrite(struct _reent *, void *, const char *, int);
int gdbstub__swrite(struct _reent *data1, void *data2, const char *buff, int len);

volatile esp_gdbstub_frame_t *temp_regs_frame;

#ifdef CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
static int bp_count = 0;
static int wp_count = 0;
static uint32_t bp_list[SOC_CPU_BREAKPOINTS_NUM] = {0};
static uint32_t wp_list[SOC_CPU_WATCHPOINTS_NUM] = {0};
static uint32_t wp_size[SOC_CPU_WATCHPOINTS_NUM] = {0};
static esp_cpu_watchpoint_trigger_t wp_access[SOC_CPU_WATCHPOINTS_NUM] = {0};

static volatile bool step_in_progress = false;
static bool not_send_reason = false;
static bool process_gdb_kill = false;
static bool gdb_debug_int = false;

/**
 * @brief Handle UART interrupt
 *
 * Handle UART interrupt for gdbstub. The function disable WDT.
 * If Ctrl+C combination detected (0x03), then application will start to process incoming GDB messages.
 * The gdbstub will stay in this interrupt until continue command will be received ($c#63).
 *
 * @param curr_regs - actual registers frame
 *
 */
void gdbstub_handle_uart_int(esp_gdbstub_frame_t *regs_frame)
{
    temp_regs_frame = regs_frame;
    not_send_reason = step_in_progress;
    if (step_in_progress == true) {
        esp_gdbstub_send_str_packet("S05");
        step_in_progress = false;
    }
//    process_gdb_kill = false;
    bp_count = 0;
    wp_count = 0;
    /* Disable all enabled WDT on enter */
    disable_all_wdts();

    int doDebug = esp_gdbstub_getfifo();
    s_scratch.signal = esp_gdbstub_get_signal(regs_frame);

    if (doDebug) {
        process_gdb_kill = false;
        /* To enable console output in GDB, we replace the default stdout->_write function */
        stdout->_write = gdbstub__swrite;
        stderr->_write = gdbstub__swrite;
        /* Stall other core until GDB exit */
        esp_gdbstub_stall_other_cpus_start();
#ifdef CONFIG_ESP_GDBSTUB_SUPPORT_TASKS
        init_task_info();
#endif/* CONFIG_ESP_GDBSTUB_SUPPORT_TASKS */
        esp_gdbstub_frame_to_regfile(regs_frame, gdb_local_regfile);
        send_reason();
        while (true) {
            unsigned char *cmd;
            size_t size;

            int res = esp_gdbstub_read_command(&cmd, &size);
            if (res == '-') {
                send_reason();
                continue;
            }
            if (res > 0) {
                /* character received instead of a command */
                continue;
            }
            if (res == -2) {
                esp_gdbstub_send_str_packet("E01");
                continue;
            }
            res = esp_gdbstub_handle_command(cmd, size);
            if (res == -2) {
                esp_gdbstub_send_str_packet(NULL);
            }
            if (res == GDBSTUB_ST_CONT) {
                break;
            }
        }
        {
            /* Resume other core */
            if (step_in_progress == false) {
                esp_gdbstub_stall_other_cpus_end();
            }
        }
    }
    esp_gdbstub_getfifo();
    /* This is workaround for problem with 'next' command. */
    if (process_gdb_kill == false) {
        esp_gdbstub_send_str("OK");
    } else {
        /* We flush all data before exit from GDB.*/
        esp_gdbstub_flush();
    }
}

void gdbstub_handle_debug_int(esp_gdbstub_frame_t *regs_frame)
{
    bp_count = 0;
    wp_count = 0;
    temp_regs_frame = regs_frame;
    gdb_debug_int = true;
    not_send_reason = step_in_progress;
    if (step_in_progress == true) {
        esp_gdbstub_clear_step();
        esp_gdbstub_send_str_packet("S05");
        step_in_progress = false;
    }

    int doDebug = esp_gdbstub_getfifo();
    s_scratch.signal = 5; /* esp_gdbstub_get_db_signal(regs_frame); */

    doDebug = 1;
    if (doDebug) {
#if CONFIG_ESP_GDBSTUB_SUPPORT_TASKS
        s_scratch.current_task_index = getActiveTaskNum();
#endif
        process_gdb_kill = false;
        /* Stall other core until GDB exit */
        esp_gdbstub_stall_other_cpus_start();
#ifdef CONFIG_ESP_GDBSTUB_SUPPORT_TASKS
        init_task_info();
#endif/* CONFIG_ESP_GDBSTUB_SUPPORT_TASKS */
        esp_gdbstub_frame_to_regfile(regs_frame, gdb_local_regfile);
        send_reason();
        while (true) {
            unsigned char *cmd;
            size_t size;

            int res = esp_gdbstub_read_command(&cmd, &size);
            if (res == '-') {
                send_reason();
                continue;
            }
            if (res > 0) {
                /* character received instead of a command */
                continue;
            }
            if (res == -2) {
                esp_gdbstub_send_str_packet("E01");
                continue;
            }
            res = esp_gdbstub_handle_command(cmd, size);
            if (res == -2) {
                esp_gdbstub_send_str_packet(NULL);
            }
            if (res == GDBSTUB_ST_CONT) {
                break;
            }
        }
        {
            /* Resume other core */
            if (step_in_progress == false) {
                esp_gdbstub_stall_other_cpus_end();
            }
        }
    }
    esp_gdbstub_getfifo();
    if (process_gdb_kill == true) {
        /* We flush all data before exit from GDB.*/
        esp_gdbstub_flush();
    }
    gdb_debug_int = false;
}

/** @brief Init gdbstub
 * Init uart interrupt for gdbstub
 * */
void esp_gdbstub_init(void)
{
    esp_intr_alloc(ETS_UART0_INTR_SOURCE, 0, esp_gdbstub_int, NULL, NULL);
    esp_gdbstub_init_dports();
}
#endif /* CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME */

#ifdef CONFIG_ESP_GDBSTUB_SUPPORT_TASKS

/** Send string as a het to uart */
static void esp_gdbstub_send_str_as_hex(const char *str)
{
    while (*str) {
        esp_gdbstub_send_hex(*str, 8);
        str++;
    }
}
#endif

/** Send all registers to gdb */
static void handle_g_command(const unsigned char *cmd, int len)
{
    uint32_t *p = (uint32_t *) &s_scratch.regfile;
    esp_gdbstub_send_start();
    for (int i = 0; i < sizeof(s_scratch.regfile) / sizeof(*p); ++i) {
        esp_gdbstub_send_hex(gdbstub_hton(*p++), 32);
    }
    esp_gdbstub_send_end();
}

/** Receive register values from gdb */
static void handle_G_command(const unsigned char *cmd, int len)
{
    uint32_t *p = (uint32_t *) &s_scratch.regfile;
    for (int i = 0; i < sizeof(s_scratch.regfile) / sizeof(*p); ++i) {
        *p++ = gdbstub_hton(esp_gdbstub_gethex(&cmd, 32));
    }
    esp_gdbstub_send_str_packet("OK");
}

static int esp_gdbstub_readmem(intptr_t addr)
{
    if (!is_valid_memory_region(addr)) {
        /* see esp_cpu_configure_region_protection */
        return -1;
    }
    uint32_t val_aligned = *(uint32_t *)(addr & (~3));
    uint32_t shift = (addr & 3) * 8;
    return (val_aligned >> shift) & 0xff;
}

static int esp_gdbstub_writemem(unsigned int addr, unsigned char data)
{
    if (!is_valid_memory_region(addr)) {
        /* see esp_cpu_configure_region_protection */
        return -1;
    }

    unsigned *addr_aligned = (unsigned *)(addr & (~3));
    const uint32_t bit_offset = (addr & 0x3) * 8;
    const uint32_t mask = ~(0xff << bit_offset);
    *addr_aligned = (*addr_aligned & mask) | (data << bit_offset);

#if CONFIG_IDF_TARGET_ARCH_XTENSA
    asm volatile("ISYNC\nISYNC\n");
#endif // CONFIG_IDF_TARGET_ARCH_XTENSA

    return 0;
}

/** Read memory to gdb */
static void handle_m_command(const unsigned char *cmd, int len)
{
    intptr_t addr = (intptr_t) esp_gdbstub_gethex(&cmd, -1);
    cmd++;
    uint32_t size = esp_gdbstub_gethex(&cmd, -1);

    if (esp_gdbstub_readmem(addr) < 0 || esp_gdbstub_readmem(addr + size - 1) < 0) {
        esp_gdbstub_send_str_packet("E01");
        return;
    }

    esp_gdbstub_send_start();
    for (int i = 0; i < size; ++i) {
        int b = esp_gdbstub_readmem(addr++);
        esp_gdbstub_send_hex(b, 8);
    }
    esp_gdbstub_send_end();
}

/** Write memory from gdb */
static void handle_M_command(const unsigned char *cmd, int len)
{
    intptr_t addr = (intptr_t) esp_gdbstub_gethex(&cmd, -1);
    cmd++; /* skip '.' */
    uint32_t size = esp_gdbstub_gethex(&cmd, -1);
    cmd++; /* skip ':' */

    if (esp_gdbstub_readmem(addr) < 0 || esp_gdbstub_readmem(addr + size - 1) < 0) {
        esp_gdbstub_send_str_packet("E01");
        return;
    }
    for (int k = 0; k < size; k++) {
        esp_gdbstub_writemem(addr, esp_gdbstub_gethex(&cmd, 8));
        addr++;
    }
    esp_gdbstub_send_start();
    esp_gdbstub_send_str_packet("OK");
    esp_gdbstub_send_end();
}

#ifdef CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
void update_breakpoints(void)
{
#if CONFIG_IDF_TARGET_ARCH_XTENSA
    for (size_t i = 0; i < SOC_CPU_BREAKPOINTS_NUM; i++) {
        if (bp_list[i] != 0) {
            esp_cpu_set_breakpoint(i, (const void *)bp_list[i]);
        } else {
            esp_cpu_clear_breakpoint(i);
        }
    }
    for (size_t i = 0; i < SOC_CPU_WATCHPOINTS_NUM; i++) {
        if (wp_list[i] != 0) {
            esp_cpu_set_watchpoint(i, (void *)wp_list[i], wp_size[i], wp_access[i]);
        } else {
            esp_cpu_clear_watchpoint(i);
        }
    }
#else  // CONFIG_IDF_TARGET_ARCH_XTENSA
#if (SOC_CPU_BREAKPOINTS_NUM != SOC_CPU_WATCHPOINTS_NUM)
#error "riscv have a common number of BP and WP"
#endif
    /*
     * On riscv we have no separated registers for setting BP and WP as we have for xtensa.
     * Instead we have common registers which could be configured as BP or WP.
     */
    size_t i = 0;
    for (size_t b = 0; b < SOC_CPU_BREAKPOINTS_NUM; b++) {
        if (bp_list[b] != 0) {
            esp_cpu_set_breakpoint(i, (const void *)bp_list[b]);
            i++;
        }
    }
    for (size_t w = 0; w < SOC_CPU_WATCHPOINTS_NUM && i < SOC_CPU_WATCHPOINTS_NUM; w++) {
        if (wp_list[w] != 0) {
            esp_cpu_set_watchpoint(i, (void *)wp_list[w], wp_size[w], wp_access[w]);
            i++;
        }
    }
    for (; i < SOC_CPU_BREAKPOINTS_NUM; i++) {
        esp_cpu_clear_breakpoint(i);
    }
#endif // CONFIG_IDF_TARGET_ARCH_XTENSA
}

/** Write breakpoint */
static void handle_Z0_command(const unsigned char *cmd, int len)
{
    cmd++; /* skip 'Z' */
    cmd++; /* skip '0' */
    uint32_t addr = esp_gdbstub_gethex(&cmd, -1);
    if (bp_count >= SOC_CPU_BREAKPOINTS_NUM) {
        esp_gdbstub_send_str_packet("E02");
        return;
    }
    bool add_bp = true;
    /* Check if bp already exist */
    for (size_t i = 0; i < SOC_CPU_BREAKPOINTS_NUM; i++) {
        if (bp_list[i] == addr) {
            add_bp = false;
            break;
        }
    }
    if (true == add_bp) {
        for (size_t i = 0; i < SOC_CPU_BREAKPOINTS_NUM; i++) {
            if (bp_list[i] == 0) {
                bp_list[i] = (uint32_t)addr;
                bp_count++;
                break;
            }
        }
    }
    update_breakpoints();
    esp_gdbstub_trigger_cpu();
    esp_gdbstub_send_str_packet("OK");
}

/** Clear breakpoint */
static void handle_z0_command(const unsigned char *cmd, int len)
{
    cmd++; /* skip 'z' */
    cmd++; /* skip '0' */
    uint32_t addr = esp_gdbstub_gethex(&cmd, -1);
    for (size_t i = 0; i < SOC_CPU_BREAKPOINTS_NUM; i++) {
        if (bp_list[i] == addr) {
            bp_list[i] = 0;
            bp_count--;
        }
    }
    update_breakpoints();
    esp_gdbstub_trigger_cpu();
    esp_gdbstub_send_str_packet("OK");
}
/** Write watchpoints write*/
static void handle_Z2_command(const unsigned char *cmd, int len)
{
    cmd++; /* skip 'Z' */
    cmd++; /* skip '2' */
    uint32_t addr = esp_gdbstub_gethex(&cmd, -1);
    cmd++;
    uint32_t size = esp_gdbstub_gethex(&cmd, -1);

    if (wp_count >= SOC_CPU_WATCHPOINTS_NUM) {
        esp_gdbstub_send_str_packet("E02");
        return;
    }
    wp_access[wp_count] = ESP_CPU_WATCHPOINT_STORE;
    wp_size[wp_count] = size;
    wp_list[wp_count++] = (uint32_t)addr;
    update_breakpoints();
    esp_gdbstub_trigger_cpu();
    esp_gdbstub_send_str_packet("OK");
}
/** Write watchpoints read*/
static void handle_Z3_command(const unsigned char *cmd, int len)
{
    cmd++; /* skip 'Z' */
    cmd++; /* skip '3' */
    uint32_t addr = esp_gdbstub_gethex(&cmd, -1);
    cmd++;
    uint32_t size = esp_gdbstub_gethex(&cmd, -1);
    if (wp_count >= SOC_CPU_WATCHPOINTS_NUM) {
        esp_gdbstub_send_str_packet("E02");
        return;
    }
    wp_access[wp_count] = ESP_CPU_WATCHPOINT_LOAD;
    wp_size[wp_count] = size;
    wp_list[wp_count++] = (uint32_t)addr;
    update_breakpoints();
    esp_gdbstub_trigger_cpu();
    esp_gdbstub_send_str_packet("OK");
}
/** Write watchpoints access*/
static void handle_Z4_command(const unsigned char *cmd, int len)
{
    cmd++; /* skip 'Z' */
    cmd++; /* skip '4' */
    uint32_t addr = esp_gdbstub_gethex(&cmd, -1);
    cmd++;
    uint32_t size = esp_gdbstub_gethex(&cmd, -1);
    if (wp_count >= SOC_CPU_WATCHPOINTS_NUM) {
        esp_gdbstub_send_str_packet("E02");
        return;
    }
    wp_access[wp_count] = ESP_CPU_WATCHPOINT_ACCESS;
    wp_size[wp_count] = size;
    wp_list[wp_count++] = (uint32_t)addr;
    update_breakpoints();
    esp_gdbstub_trigger_cpu();
    esp_gdbstub_send_str_packet("OK");
}
/** Clear watchpoint */
static void handle_zx_command(const unsigned char *cmd, int len)
{
    cmd++; /* skip 'z' */
    cmd++; /* skip 'x' */
    uint32_t addr = esp_gdbstub_gethex(&cmd, -1);
    for (size_t i = 0; i < SOC_CPU_WATCHPOINTS_NUM; i++) {
        if (wp_list[i] == addr) {
            wp_access[i] = 0;
            wp_list[i] = 0;
        }
    }
    update_breakpoints();
    esp_gdbstub_trigger_cpu();
    esp_gdbstub_send_str_packet("OK");
}

/** Step ... */
static void handle_S_command(const unsigned char *cmd, int len)
{
    esp_gdbstub_send_str_packet("S05");
}

/** Step ... */
static void handle_s_command(const unsigned char *cmd, int len)
{
    step_in_progress = true;
    esp_gdbstub_do_step((esp_gdbstub_frame_t *)temp_regs_frame);
}

/** Step ... */
static void handle_C_command(const unsigned char *cmd, int len)
{
    esp_gdbstub_send_str_packet("OK");
}


/* Set Register ... */
static void handle_P_command(const unsigned char *cmd, int len)
{
    uint32_t reg_index = 0;
    if (cmd[1] == '=') {
        reg_index = esp_gdbstub_gethex(&cmd, 4);
        cmd++;
    } else if (cmd[2] == '=') {
        reg_index = esp_gdbstub_gethex(&cmd, 8);
        cmd++;
        cmd++;
    } else {
        esp_gdbstub_send_str_packet("E02");
        return;
    }
    uint32_t addr = esp_gdbstub_gethex(&cmd, -1);
    /* The address comes with inverted byte order.*/
    uint8_t *addr_ptr = (uint8_t *)&addr;
    uint32_t p_address = 0;
    uint8_t *p_addr_ptr = (uint8_t *)&p_address;
    p_addr_ptr[3] = addr_ptr[0];
    p_addr_ptr[2] = addr_ptr[1];
    p_addr_ptr[1] = addr_ptr[2];
    p_addr_ptr[0] = addr_ptr[3];

    esp_gdbstub_set_register((esp_gdbstub_frame_t *)temp_regs_frame, reg_index, p_address);
    /* Convert current register file to GDB*/
    esp_gdbstub_frame_to_regfile((esp_gdbstub_frame_t *)temp_regs_frame, gdb_local_regfile);
    /* Sen OK response*/
    esp_gdbstub_send_str_packet("OK");
}

/** qSupported requests the communication with GUI
 */
static void handle_qSupported_command(const unsigned char *cmd, int len)
{
    esp_gdbstub_send_start();
    esp_gdbstub_send_str("qSupported:multiprocess+;swbreak-;hwbreak+;qRelocInsn+;fork-events+;vfork-events+;exec-events+;vContSupported+;QThreadEvents+;no-resumed+");
    esp_gdbstub_send_end();
}

#endif // CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME

/** Handle a command received from gdb */
int esp_gdbstub_handle_command(unsigned char *cmd, int len)
{
    unsigned char *data = cmd + 1;
    if (cmd[0] == 'g') {
        handle_g_command(data, len - 1);
    } else if (cmd[0] == 'G') {
        /* receive content for all registers from gdb */
        handle_G_command(data, len - 1);
    } else if (cmd[0] == 'm') {
        /* read memory to gdb */
        handle_m_command(data, len - 1);
    } else if (cmd[0] == 'M') {
        /* write to memory from GDB */
        handle_M_command(data, len - 1);
    } else if (cmd[0] == '?') {
        /* Reply with stop reason */
        send_reason();
#ifdef CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
    } else if (cmd[0] == 'Z') {
        if (cmd[1] == '0') {
            /* Write breakpoint */
            handle_Z0_command(data, len - 1);
        }
        if (cmd[1] == '2') {
            /* Write breakpoint */
            handle_Z2_command(data, len - 1);
        }
        if (cmd[1] == '3') {
            /* Write breakpoint */
            handle_Z3_command(data, len - 1);
        }
        if (cmd[1] == '4') {
            /* Write breakpoint */
            handle_Z4_command(data, len - 1);
        }
    } else if (cmd[0] == 'z') {
        /* Clear  breakpoint*/
        if (cmd[1] == '0') {
            handle_z0_command(data, len - 1);
        }
        /* Clear  breakpoint*/
        if ((cmd[1] == '2') || (cmd[1] == '3') || (cmd[1] == '4')) {
            handle_zx_command(data, len - 1);
        }
    } else if (cmd[0] == 'S') {
        /* Step */
        handle_S_command(data, len - 1);
    } else if (cmd[0] == 'k') {
        /* Kill GDB and continue without */
        /* By exit from GDB we have to replcae stdout->_write back */
        stdout->_write = __swrite;
        stderr->_write = __swrite;
        process_gdb_kill = true;
        return GDBSTUB_ST_CONT;
    } else if (cmd[0] == 's') {
        /* Step and continue*/
        handle_s_command(data, len - 1);
        return GDBSTUB_ST_CONT;
    } else if (cmd[0] == 'C') {
        /* Just continue*/
        handle_C_command(data, len - 1);
        size_t size;
        esp_gdbstub_read_command(&cmd, &size);
        return GDBSTUB_ST_CONT;
    } else if (cmd[0] == 'P') {
        handle_P_command(data, len - 1);
    } else if (cmd[0] == 'c') { //continue execution
        return GDBSTUB_ST_CONT;
    } else if (command_name_matches("qSupported", cmd, 10)) {
        handle_qSupported_command(cmd, len);
#endif // CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
#if CONFIG_ESP_GDBSTUB_SUPPORT_TASKS
    } else if (s_scratch.state != GDBSTUB_TASK_SUPPORT_DISABLED) {
        return handle_task_commands(cmd, len);
#endif // CONFIG_ESP_GDBSTUB_SUPPORT_TASKS
    } else {
        /* Unrecognized command */
        return GDBSTUB_ST_ERR;
    }
    return GDBSTUB_ST_OK;
}
/**
 * Replace standard __swrite function for GDB
 */
/* const int buff_len = 16; */
/* static char log_buffer[16*2 + 7]; */
int gdbstub__swrite(struct _reent *data1, void *data2, const char *buff, int len)
{
    const int buff_len = 16;
    char log_buffer[buff_len * 2 + 7];
    char s_chsum = 'O';
    char *sent_data = (char *)buff;
    size_t remaining = len;
    size_t send_pos = 0;
    while (remaining > 0) {
        size_t will_send = MIN(remaining, buff_len);
        remaining -= will_send;
        /* prepare and set 'will_send' number of bytes */
        if (will_send > 0) {
            log_buffer[0] = '$';
            log_buffer[1] = 'O';
            s_chsum = 'O';
            s_chsum += gdb_packet(&log_buffer[2], &sent_data[send_pos], will_send);
            sprintf(&log_buffer[will_send * 2 + 2], "#%2.2x\n", s_chsum);
            __swrite(data1, data2, log_buffer, will_send * 2 + 6);
            send_pos += buff_len;
        }
    }
    return len;
}


/** @brief Convert to ASCI
 * Function convert byte value to two ASCI carecters
 */
void gdb_get_asci_char(unsigned char data, char *buff)
{
    const char *hex_chars = "0123456789abcdef";
    buff[0] = hex_chars[(data >> 4) & 0x0f];
    buff[1] = hex_chars[(data) & 0x0f];
}


/* Everything below is related to the support for listing FreeRTOS tasks as threads in GDB */

/** @brief Prepare GDB packet
 * Function build GDB asci packet and return checksum
 *
 * Return checksum
 */
char gdb_packet(char *dest_buff, char *src_buff, int len)
{
    char s_chsum = 0;
    for (size_t i = 0; i < len; i++) {
        gdb_get_asci_char(src_buff[i], &dest_buff[i * 2 + 0]);
    }
    for (size_t i = 0; i < len * 2; i++) {
        s_chsum += dest_buff[i];
    }
    return s_chsum;
}

#if (CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME || CONFIG_ESP_GDBSTUB_SUPPORT_TASKS)
static bool command_name_matches(const char *pattern, const unsigned char *ucmd, int len)
{
    const char *cmd = (const char *) ucmd;
    const char *end = cmd + len;
    for (; *pattern && cmd < end; ++cmd, ++pattern) {
        if (*pattern == '?') {
            continue;
        }
        if (*pattern != *cmd) {
            return false;
        }
    }
    return *pattern == 0 && (cmd == end || *cmd == ',');
}
#endif // (CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME || CONFIG_ESP_GDBSTUB_SUPPORT_TASKS)

#ifdef CONFIG_ESP_GDBSTUB_SUPPORT_TASKS

/* Some terminology related to task/thread indices:
 *
 * "task index" — Index used here in gdbstub.c to enumberate all the tasks.
 *                Range is from 0 to <number of tasks> - 1.
 *                The order is defined by uxTaskGetSnapshotAll.
 *
 * "GDB TID" — Thread ID used by GDB internally and in the remote protocol.
 *             IDs of real threads can be any positive values other than 0.
 *             For example, OpenOCD uses the TCB pointer (cast to a 32-bit int) as GDB TID.
 *             Values 0 and -1, when used in place of TID in the remote protocol
 *             have special meanings:
 *             -1: indicates all threads, may be used in 'Hc' command
 *              0: indicates an arbitrary process or thread (GDB client doesn't care, server decides)
 *
 * Since GDB TIDs are arbitrary, except that 0 is reserved, we set them using a simple rule:
 *    GDB TID = task index + 1.
 * The two functions below perform the conversions between the kinds of IDs.
 */
static inline int gdb_tid_to_task_index(int tid)
{
    return tid - 1;
}

static inline int task_index_to_gdb_tid(int tid)
{
    return tid + 1;
}

static void init_task_info(void)
{
    unsigned tcb_size;
    s_scratch.task_count = uxTaskGetSnapshotAll(s_scratch.tasks, GDBSTUB_TASKS_NUM, &tcb_size);
}

static bool get_task_handle(size_t index, TaskHandle_t *handle)
{
    if (index >= s_scratch.task_count) {
        return false;
    }
    *handle = (TaskHandle_t) s_scratch.tasks[index].pxTCB;
    return true;
}

static eTaskState get_task_state(size_t index)
{
    eTaskState result = eReady;
    TaskHandle_t handle = NULL;
    get_task_handle(index, &handle);
#if CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
    if (gdb_debug_int == false) {
        result = eTaskGetState(handle);
    }
#endif
    return result;
}

static int get_task_cpu_id(size_t index)
{
    TaskHandle_t handle;
    if (!get_task_handle(index, &handle)) {
        return -1;
    }
    BaseType_t core_id = xTaskGetCoreID(handle);
    return (int)core_id;
}

/** Get the index of the task running on the current CPU, and save the result */
static void find_paniced_task_index(void)
{
    TaskHandle_t cur_handle = (TaskHandle_t)xTaskGetCurrentTaskHandleForCore(xPortGetCoreID());
    TaskHandle_t handle;
    for (int i = 0; i < s_scratch.task_count; i++) {
        if (get_task_handle(i, &handle) && cur_handle == handle) {
            s_scratch.paniced_task_index = i;
            return;
        }
    }
    s_scratch.paniced_task_index = GDBSTUB_CUR_TASK_INDEX_UNKNOWN;
}

/** Select the current task and update the regfile */
static void set_active_task(size_t index)
{
    if (index == s_scratch.paniced_task_index) {
        /* Have to get the registers from exception frame */
        esp_gdbstub_frame_to_regfile(&s_scratch.paniced_frame, &s_scratch.regfile);
    } else {
        /* Get the registers from TCB.
         * FIXME: for the task currently running on the other CPU, extracting the registers from TCB
         * isn't valid. Need to use some IPC mechanism to obtain the registers of the other CPU.
         */
        TaskHandle_t handle = NULL;
        get_task_handle(index, &handle);
        if (handle != NULL) {
            esp_gdbstub_tcb_to_regfile(handle, &s_scratch.regfile);
        }
    }
    s_scratch.current_task_index = index;
}

/** H command sets the "current task" for the purpose of further commands */
static void handle_H_command(const unsigned char *cmd, int len)
{
    const char *ret = "OK";
    if (cmd[1] == 'g') {
        cmd += 2;
        int requested_tid = esp_gdbstub_gethex(&cmd, -1);
        int requested_task_index = gdb_tid_to_task_index(requested_tid);
        if (requested_tid == 0) {
            /* 0 means "arbitrary process or thread", keep the current thread */
        } else if (requested_task_index >= s_scratch.task_count ||
                   requested_tid == -1) { /* -1 means "all threads", which doesn't make sense for "Hg" */
            ret = "E00";
        } else {
            set_active_task(requested_task_index);
        }
        esp_gdbstub_send_str_packet(ret);
    } else if (cmd[1] == 'c') {
        /* Select the task for "continue" and "step" operations. No-op in post-mortem mode. */
        /* Note that the argument may be -1 to indicate "all threads" or 0 to indicate "any thread" */
        esp_gdbstub_send_str_packet(ret);
    } else {
        esp_gdbstub_send_str_packet(NULL);
    }
}

/** qC returns the current thread ID */
static void handle_qC_command(const unsigned char *cmd, int len)
{
    esp_gdbstub_send_start();
    esp_gdbstub_send_str("QC");
    esp_gdbstub_send_hex(task_index_to_gdb_tid(s_scratch.current_task_index), 32);
    esp_gdbstub_send_end();
}

int getActiveTaskNum(void)
{
    for (size_t i = 0; i < s_scratch.task_count; i++) {
        {
            eTaskState state = get_task_state(task_index_to_gdb_tid(i));
            if (state == eRunning) {
                return i;
            }
        }
    }
    return s_scratch.task_count;
}

/** T command checks if the task is alive.
 *  Since GDB isn't going to ask about the tasks which haven't been listed by q*ThreadInfo,
 *  and the state of tasks can not change (no stepping allowed), simply return "OK" here.
 */
static void handle_T_command(const unsigned char *cmd, int len)
{
    esp_gdbstub_send_str_packet("OK");
}

/** Called by qfThreadInfo and qsThreadInfo handlers */
static void send_single_thread_info(int task_index)
{
    esp_gdbstub_send_start();
    esp_gdbstub_send_str("m");
    esp_gdbstub_send_hex(task_index_to_gdb_tid(task_index), 32);
    esp_gdbstub_send_end();
}

/** qfThreadInfo requests the start of the thread list, qsThreadInfo (below) is repeated to
 *  get the subsequent threads.
 */
static void handle_qfThreadInfo_command(const unsigned char *cmd, int len)
{
    assert(s_scratch.task_count > 0);  /* There should be at least one task */
    send_single_thread_info(0);
    s_scratch.thread_info_index = 1;
}

static void handle_qsThreadInfo_command(const unsigned char *cmd, int len)
{
    int task_index = s_scratch.thread_info_index;
    if (task_index == s_scratch.task_count) {
        /* No more tasks */
        esp_gdbstub_send_str_packet("l");
        return;
    }
    send_single_thread_info(s_scratch.thread_info_index);
    s_scratch.thread_info_index++;
}

/** qThreadExtraInfo requests the thread name */
static void handle_qThreadExtraInfo_command(const unsigned char *cmd, int len)
{
    cmd += sizeof("qThreadExtraInfo,") - 1;
    int task_index = gdb_tid_to_task_index(esp_gdbstub_gethex(&cmd, -1));
    TaskHandle_t handle;
    if (!get_task_handle(task_index, &handle)) {
        esp_gdbstub_send_str_packet("E01");
        return;
    }
    esp_gdbstub_send_start();
    esp_gdbstub_send_str_as_hex("Name: ");
    esp_gdbstub_send_str_as_hex((const char *)pcTaskGetName(handle));
    esp_gdbstub_send_hex(' ', 8);

    eTaskState state = get_task_state(task_index);
    /* esp_gdbstub_send_str_as_hex("State: Suspended"); */
    switch (state) {
    case eRunning:
        esp_gdbstub_send_str_as_hex("State: Running ");
        esp_gdbstub_send_str_as_hex("@CPU - ");
        esp_gdbstub_send_hex(get_task_cpu_id(task_index) + '0', 8);
        break;
    case eReady:
        esp_gdbstub_send_str_as_hex("State: Ready");
        break;
    case eBlocked:
        esp_gdbstub_send_str_as_hex("State: Blocked");
        break;
    case eSuspended:
        esp_gdbstub_send_str_as_hex("State: Suspended");
        break;
    case eDeleted:
        esp_gdbstub_send_str_as_hex("State: Deleted");
        break;
    default:
        esp_gdbstub_send_str_as_hex("State: Invalid");
        break;
    }

    esp_gdbstub_send_end();
}

/** Handle all the thread-related commands */
static int handle_task_commands(unsigned char *cmd, int len)
{
    if (cmd[0] == 'H') {
        /* Continue with task */
        handle_H_command(cmd, len);
    } else if (cmd[0] == 'T') {
        /* Task alive check */
        handle_T_command(cmd, len);
    } else if (cmd[0] == 'q') {
        if (command_name_matches("qfThreadInfo", cmd, len)) {
            handle_qfThreadInfo_command(cmd, len);
        } else if (command_name_matches("qsThreadInfo", cmd, len)) {
            handle_qsThreadInfo_command(cmd, len);
        } else if (command_name_matches("qC", cmd, len)) {
            handle_qC_command(cmd, len);
        } else if (command_name_matches("qThreadExtraInfo", cmd, len)) {
            handle_qThreadExtraInfo_command(cmd, len);
        } else {
            /* Unrecognized command */
            return GDBSTUB_ST_ERR;
        }
    } else if (strncmp((char *)cmd, "vCont;c", 7) == 0 || cmd[0] == 'c') { /*continue execution */
        return GDBSTUB_ST_CONT;
    } else {
        /* Unrecognized command */
        return GDBSTUB_ST_ERR;
    }
    return GDBSTUB_ST_OK;
}

#endif /* CONFIG_ESP_GDBSTUB_SUPPORT_TASKS */
