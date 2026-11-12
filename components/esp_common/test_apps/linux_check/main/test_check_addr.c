/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_check.h"
#include "esp_err.h"
#include "esp_log_write.h"
#include "unity.h"

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#else
#include <link.h>
#include <unistd.h>
#endif

static char s_log[1024];
static size_t s_log_len;

static int capture_vprintf(const char *fmt, va_list args)
{
    if (s_log_len + 1 >= sizeof(s_log)) {
        return 0;
    }
    int wrote = vsnprintf(s_log + s_log_len, sizeof(s_log) - s_log_len, fmt, args);
    if (wrote > 0) {
        size_t take = (size_t)wrote;
        if (s_log_len + take >= sizeof(s_log)) {
            take = sizeof(s_log) - s_log_len - 1;
        }
        s_log_len += take;
        s_log[s_log_len] = '\0';
    }
    return wrote;
}

static void log_reset(void)
{
    s_log_len = 0;
    s_log[0] = '\0';
}

#if !defined(__APPLE__)
static int bias_cb(struct dl_phdr_info *info, size_t size, void *data)
{
    (void)size;
    /* The main executable is visited first. dlpi_addr is its load bias. */
    *(uintptr_t *)data = (uintptr_t)info->dlpi_addr;
    return 1;
}
#endif

/* The logged address is a runtime pointer. addr2line wants the address recorded
 * in the binary, so subtract the load bias (ASLR slide). */
static uintptr_t file_addr(uintptr_t runtime)
{
#if defined(__APPLE__)
    return runtime - (uintptr_t)_dyld_get_image_vmaddr_slide(0);
#else
    uintptr_t bias = 0;
    dl_iterate_phdr(bias_cb, &bias);
    return runtime - bias;
#endif
}
static bool exe_path(char *out, size_t out_len)
{
#if defined(__APPLE__)
    uint32_t size = (uint32_t)out_len;
    return _NSGetExecutablePath(out, &size) == 0;
#else
    ssize_t n = readlink("/proc/self/exe", out, out_len - 1);
    if (n < 0) {
        return false;
    }
    out[n] = '\0';
    return true;
#endif
}

/* First ":<digits>" in a symbolizer line. "file:line" and "file:line:column" both yield the line. */
static int line_in_text(const char *text)
{
    for (const char *p = text; *p != '\0'; p++) {
        if (*p != ':') {
            continue;
        }
        char *end = NULL;
        long n = strtol(p + 1, &end, 10);
        if (end != p + 1 && n > 0) {
            return (int)n;
        }
    }
    return -1;
}

/* GNU addr2line prints "file:line". llvm-symbolizer prints a function name and then
 * "file:line:column". A missing tool still lets popen() succeed, so an empty read
 * means "try the next one". */
static bool line_from_tool(const char *tool, const char *exe, uintptr_t addr, int *line_out)
{
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "%s -f -e \"%s\" 0x%" PRIxPTR " 2>/dev/null", tool, exe, addr);
    FILE *pipe = popen(cmd, "r");
    if (pipe == NULL) {
        return false;
    }

    int line = -1;
    char buf[512];
    while (fgets(buf, sizeof(buf), pipe) != NULL && line < 0) {
        line = line_in_text(buf);
    }
    int status = pclose(pipe);
    if (line < 0 || status != 0) {
        return false;
    }
    *line_out = line;
    return true;
}

#if defined(__APPLE__)
/* Apple links a debug map, not the line table, so addr2line cannot see it. dsymutil
 * collects the line table; atos then resolves the address. */
static bool line_from_atos(const char *exe, uintptr_t addr, int *line_out)
{
    char dsym[640];
    snprintf(dsym, sizeof(dsym), "%s.dSYM", exe);
    char cmd[1400];
    snprintf(cmd, sizeof(cmd), "xcrun dsymutil \"%s\" -o \"%s\" >/dev/null 2>&1", exe, dsym);
    if (system(cmd) != 0) {
        return false;
    }

    const char *base = strrchr(exe, '/');
    base = (base != NULL) ? base + 1 : exe;
    char dwarf[768];
    snprintf(dwarf, sizeof(dwarf), "%s/Contents/Resources/DWARF/%s", dsym, base);
    snprintf(cmd, sizeof(cmd), "xcrun atos -o \"%s\" 0x%" PRIxPTR " 2>/dev/null", dwarf, addr);

    FILE *pipe = popen(cmd, "r");
    if (pipe == NULL) {
        return false;
    }
    int line = -1;
    char buf[512];
    while (fgets(buf, sizeof(buf), pipe) != NULL && line < 0) {
        line = line_in_text(buf);
    }
    int status = pclose(pipe);
    if (line < 0 || status != 0) {
        return false;
    }
    *line_out = line;
    return true;
}
#endif

static int line_of_addr(uintptr_t addr)
{
    char exe[512];
    TEST_ASSERT_TRUE_MESSAGE(exe_path(exe, sizeof(exe)), "could not find this executable");

    uintptr_t where = file_addr(addr);
    const char *tools[] = {"addr2line", "llvm-addr2line", "llvm-symbolizer", NULL};
    for (int i = 0; tools[i] != NULL; i++) {
        int line = 0;
        if (line_from_tool(tools[i], exe, where, &line)) {
            return line;
        }
    }
#if defined(__APPLE__)
    int line = 0;
    if (line_from_atos(exe, where, &line)) {
        return line;
    }
#endif
    TEST_FAIL_MESSAGE("could not resolve the check address to a source line");
    return 0;
}

static uintptr_t logged_addr(const char *log)
{
    const char *at = strstr(log, " at 0x");
    TEST_ASSERT_NOT_NULL_MESSAGE(at, log);
    uintptr_t hex = 0;
    int n = 0;
    TEST_ASSERT_EQUAL_INT(1, sscanf(at, " at 0x%" SCNxPTR "%n", &hex, &n));
    /* %08" PRIxPTR is at least eight digits. On a 32-bit target that is what
     * idf.py monitor decodes; a 64-bit host pointer can be wider. */
    TEST_ASSERT_GREATER_OR_EQUAL_INT(8, n - (int)strlen(" at 0x"));
    return hex;
}

static int g_line_state;
static int g_line_arg;
static int g_line_false;

/* noinline: -Oz may otherwise inline these into the test, and addr2line then
 * reports the call site instead of the check.
 */
static esp_err_t __attribute__((noinline)) fail_two(int which)
{
    if (which == 0) {
        g_line_state = __LINE__; ESP_RETURN_ON_ERROR(ESP_ERR_INVALID_STATE, "chk", "hidden state");
    }
    g_line_arg = __LINE__; ESP_RETURN_ON_ERROR(ESP_ERR_INVALID_ARG, "chk", "hidden arg");
    return ESP_OK;
}

static bool __attribute__((noinline)) fail_false(void)
{
    g_line_false = __LINE__; ESP_RETURN_ON_FALSE(false, false, "chk", "hidden false");
    return true;
}

/* Address only must name this check, not the previous line and not the reporter.
 * A failed *_ON_ERROR check also prints its error code; a failed *_ON_FALSE check does not.
 * The logged address is the return address minus 2. On this host that falls inside the
 * call instruction. It does not reproduce a 2-byte RISC-V c.jal, which is the call that
 * used to resolve to the previous line. */
TEST_CASE("address-only checks log their own line", "[linux_check]")
{
    vprintf_like_t prev = esp_log_set_vprintf(capture_vprintf);

    log_reset();
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, fail_two(0));
    TEST_ASSERT_NULL(strstr(s_log, "hidden"));
    TEST_ASSERT_NOT_NULL(strstr(s_log, "esp_err_t 0x103"));
    uintptr_t state_addr = logged_addr(s_log);
    TEST_ASSERT_EQUAL_INT(g_line_state, line_of_addr(state_addr));

    log_reset();
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, fail_two(1));
    TEST_ASSERT_NULL(strstr(s_log, "hidden"));
    TEST_ASSERT_NOT_NULL(strstr(s_log, "esp_err_t 0x102"));
    uintptr_t arg_addr = logged_addr(s_log);
    TEST_ASSERT_NOT_EQUAL(state_addr, arg_addr);
    TEST_ASSERT_EQUAL_INT(g_line_arg, line_of_addr(arg_addr));

    log_reset();
    TEST_ASSERT_FALSE(fail_false());
    TEST_ASSERT_NULL(strstr(s_log, "hidden"));
    TEST_ASSERT_NULL(strstr(s_log, "esp_err_t"));
    TEST_ASSERT_EQUAL_INT(g_line_false, line_of_addr(logged_addr(s_log)));

    esp_log_set_vprintf(prev);
}

void app_main(void)
{
    unity_run_menu();
}
