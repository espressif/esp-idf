/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include "unity.h"
#include "unity_test_utils.h"

/* Note: When first exception (in system) is thrown this test produces memory leaks report (~300 bytes):
   - 8 bytes are allocated by __cxa_get_globals() to keep __cxa_eh_globals
   - 16 bytes are allocated by pthread_setspecific() which is called by __cxa_get_globals() to init TLS var for __cxa_eh_globals
   - 88 bytes are allocated by pthread_setspecific() to init internal lock
   - some more memory...
   */
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32C61 || CONFIG_IDF_TARGET_ESP32P4
#define LEAKS (128)  /* real: 72 */
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32C3
#define LEAKS (448)  /* real: 396 */
#else
#error "unknown target in CXX tests, can't set leaks threshold"
#endif

extern "C" void setUp()
{
    unity_utils_record_free_mem();
}

extern "C" void tearDown()
{
    unity_utils_evaluate_leaks_direct(LEAKS);
}

TEST_CASE("c++ exceptions work", "[cxx] [exceptions]")
{
    int thrown_value;
    try {
        throw 20;
    } catch (int e) {
        thrown_value = e;
    }
    TEST_ASSERT_EQUAL(20, thrown_value);
}

TEST_CASE("c++ bool exception", "[cxx] [exceptions]")
{
    bool thrown_value = false;
    try {
        throw true;
    } catch (bool e) {
        thrown_value = e;
    }
    TEST_ASSERT_EQUAL(true, thrown_value);
}

TEST_CASE("c++ void exception", "[cxx] [exceptions]")
{
    void* thrown_value = 0;
    try {
        throw (void*) 47;
    } catch (void* e) {
        thrown_value = e;
    }
    TEST_ASSERT_EQUAL(47, thrown_value);
}

TEST_CASE("c++ uint64_t exception", "[cxx] [exceptions]")
{
    uint64_t thrown_value = 0;
    try {
        throw (uint64_t) 47;
    } catch (uint64_t e) {
        thrown_value = e;
    }
    TEST_ASSERT_EQUAL(47, thrown_value);
}

TEST_CASE("c++ char exception", "[cxx] [exceptions]")
{
    char thrown_value = '0';
    try {
        throw '/';
    } catch (char e) {
        thrown_value = e;
    }
    TEST_ASSERT_EQUAL('/', thrown_value);
}

TEST_CASE("c++ wchar exception", "[cxx] [exceptions]")
{
    wchar_t thrown_value = 0;
    try {
        throw (wchar_t) 47;
    } catch (wchar_t e) {
        thrown_value = e;
    }
    TEST_ASSERT_EQUAL((wchar_t) 47, thrown_value);
}

TEST_CASE("c++ float exception", "[cxx] [exceptions]")
{
    float thrown_value = 0;
    try {
        throw 23.5f;
    } catch (float e) {
        thrown_value = e;
    }
    TEST_ASSERT_EQUAL(23.5, thrown_value);
}

TEST_CASE("c++ double exception", "[cxx] [exceptions]")
{
    double thrown_value = 0;
    try {
        throw 23.5d;
    } catch (double e) {
        thrown_value = e;
    }
    TEST_ASSERT_EQUAL(23.5d, thrown_value);
}

TEST_CASE("c++ const char* exception", "[cxx] [exceptions]")
{
    const char *thrown_value = 0;
    try {
        throw "Hi :)";
    } catch (const char *e) {
        thrown_value = e;
    }
    TEST_ASSERT_EQUAL_STRING("Hi :)", thrown_value);
}

struct NonExcTypeThrowee {
    int value;
public:
    NonExcTypeThrowee(int value) : value(value) { }
};

TEST_CASE("c++ any class exception", "[cxx] [exceptions]")
{
    int thrown_value = 0;
    try {
        throw NonExcTypeThrowee(47);
    } catch (NonExcTypeThrowee &e) {
        thrown_value = e.value;
    }
    TEST_ASSERT_EQUAL(47, thrown_value);
}

struct ExcTypeThrowee : public std::exception {
    int value;
public:
    ExcTypeThrowee(int value) : value(value) { }
};

TEST_CASE("c++ std::exception child", "[cxx] [exceptions]")
{
    int thrown_value = 0;
    try {
        throw ExcTypeThrowee(47);
    } catch (ExcTypeThrowee &e) {
        thrown_value = e.value;
    }
    TEST_ASSERT_EQUAL(47, thrown_value);
}

TEST_CASE("c++ exceptions emergency pool", "[cxx] [exceptions]")
{
    void **p, **pprev = NULL;
    int thrown_value = 0;
    // throw first exception to ensure that all initial allocations are made
    try {
        throw 33;
    } catch (int e) {
        thrown_value = e;
    }
    TEST_ASSERT_EQUAL(33, thrown_value);
    // consume all dynamic memory
    while ((p = (void **)malloc(sizeof(void *)))) {
        if (pprev) {
            *p = pprev;
        } else {
            *p = NULL;
        }
        pprev = p;
    }
    try {
        throw 20;
    } catch (int e) {
        thrown_value = e;
    }
#if CONFIG_COMPILER_CXX_EXCEPTIONS_EMG_POOL_SIZE > 0
    // free all memory
    while (pprev) {
        p = (void **)(*pprev);
        free(pprev);
        pprev = p;
    }
    TEST_ASSERT_EQUAL(20, thrown_value);
#else
    // if emergency pool is disabled we should never get here,
    // expect abort() due to lack of memory for new exception
    TEST_ASSERT_TRUE(0 == 1);
#endif
}

static int s_frames_unwound;

struct FrameGuard {
    ~FrameGuard()
    {
        s_frames_unwound++;
    }
};

static __attribute__((noinline)) void throw_from_depth(int depth)
{
    FrameGuard guard;
    if (depth == 0) {
        throw 42;
    }
    throw_from_depth(depth - 1);
}

TEST_CASE("c++ exception unwinds several frames", "[cxx] [exceptions]")
{
    s_frames_unwound = 0;
    int thrown_value = 0;
    try {
        throw_from_depth(5);
    } catch (int e) {
        thrown_value = e;
    }
    TEST_ASSERT_EQUAL(42, thrown_value);
    TEST_ASSERT_EQUAL(6, s_frames_unwound);
}

/* libgcc's struct dwarf_eh_bases (unwind-dw2-fde.h) */
struct dwarf_eh_bases {
    void *tbase;
    void *dbase;
    void *func;
};

extern "C" const void *_Unwind_Find_FDE(void *pc, dwarf_eh_bases *bases);
extern "C" const uint8_t __eh_frame_hdr_start[];
extern "C" const uint8_t __eh_frame[];

struct EhFrameHdr {
    uint8_t version;
    uint8_t eh_frame_ptr_enc;
    uint8_t fde_count_enc;
    uint8_t table_enc;
    int32_t eh_frame_ptr;
    uint32_t fde_count;
};

struct EhFrameHdrEntry {
    int32_t initial_location;
    int32_t fde;
};

TEST_CASE("c++ exceptions find each FDE listed in .eh_frame_hdr", "[cxx] [exceptions]")
{
    const EhFrameHdr *hdr = (const EhFrameHdr *)__eh_frame_hdr_start;
    TEST_ASSERT_EQUAL(1, hdr->version);
    TEST_ASSERT_GREATER_THAN(0, hdr->fde_count);

    /* libgcc only searches the table if it describes the .eh_frame registered at startup */
    const uint8_t *eh_frame = __eh_frame_hdr_start + offsetof(EhFrameHdr, eh_frame_ptr) + hdr->eh_frame_ptr;
    TEST_ASSERT_EQUAL_PTR(__eh_frame, eh_frame);

    /* The table is sorted by initial location, relative to the start of .eh_frame_hdr */
    const EhFrameHdrEntry *table = (const EhFrameHdrEntry *)(hdr + 1);
    uintptr_t base = (uintptr_t)hdr;
    for (uint32_t i = 0; i < hdr->fde_count; i++) {
        dwarf_eh_bases bases = {};
        uintptr_t pc = base + table[i].initial_location;
        TEST_ASSERT_EQUAL_PTR(base + table[i].fde, _Unwind_Find_FDE((void *)pc, &bases));
        TEST_ASSERT_EQUAL_PTR(pc, bases.func);
    }
}

extern "C" void app_main(void)
{
    printf("CXX EXCEPTION TEST\n");
    unity_run_menu();
}
