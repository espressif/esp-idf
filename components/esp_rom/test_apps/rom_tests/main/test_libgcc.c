/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <complex.h>
#include "unity.h"
#include "esp_rom_caps.h"

TEST_CASE("libgcc math functions", "[rom][libgcc]")
{
    extern int64_t __absvdi2(int64_t x);
    TEST_ASSERT(__absvdi2(-1L) == 1);
    extern int __absvsi2(int x);
    TEST_ASSERT(__absvsi2(-1) == 1);
    extern double __adddf3(double x, double y);
    TEST_ASSERT(__adddf3(1.0, 4.0) == 5.0);
    extern int64_t __addvdi3(int64_t x, int64_t y);
    TEST_ASSERT(__addvdi3(1L, 4L) == 5L);
    extern int __addvsi3(int x, int y);
    TEST_ASSERT(__addvsi3(1, 4) == 5);
    extern int64_t __ashldi3(int64_t x, int y);
    TEST_ASSERT(__ashldi3(1, 4) == 16);
    extern int64_t __ashrdi3(int64_t x, int y);
    TEST_ASSERT(__ashrdi3(4, 1) == 2);
    extern int64_t __bswapdi2(int64_t x);
    TEST_ASSERT(__bswapdi2(0xaabbccddeeff0011ULL) == 0x1100ffeeddccbbaaULL);
    extern int32_t __bswapsi2(int32_t x);
    TEST_ASSERT(__bswapsi2(0xaabbccdd) == 0xddccbbaa);
    extern int __clrsbdi2(int64_t x);
    TEST_ASSERT(__clrsbdi2(-1) == 63);
    extern int __clrsbsi2(int x);
    TEST_ASSERT(__clrsbsi2(-1) == 31);
    extern int __clzdi2(int64_t x);
    TEST_ASSERT(__clzdi2(1) == 63);
    extern int __clzsi2(int x);
    TEST_ASSERT(__clzsi2(1) == 31);
    extern int __cmpdi2(int64_t x, int64_t y);
    TEST_ASSERT(__cmpdi2(10, 10) == 1);
    extern int __ctzdi2(uint64_t x);
    TEST_ASSERT(__ctzdi2(0x8000000000000000ULL) == 63);
    extern int __ctzsi2(unsigned x);
    TEST_ASSERT(__ctzsi2(0x80000000U) == 31);
    extern complex double __divdc3(double a, double b, double c, double d);
    TEST_ASSERT(__divdc3(0, 1, 1, 0) == I);
    extern double __divdf3(double x, double y);
    TEST_ASSERT(__divdf3(16.0, 2.0) == 8.0);
    extern int64_t __divdi3(int64_t x, int64_t y);
    TEST_ASSERT(__divdi3(16, 2) == 8);
    extern complex float __divsc3(float a, float b, float c, float d);
    TEST_ASSERT(__divsc3(0, 1, 1, 0) == I);
    extern float __divsf3(float x, float y);
    TEST_ASSERT(__divsf3(16.0f, 2.0f) == 8.0f);
    extern int __divsi3(int x, int y);
    TEST_ASSERT(__divsi3(16, 2) == 8);
    extern int __eqdf2(double x, double y);
    TEST_ASSERT(__eqdf2(4.0, 4.0) == 0);

    extern double __extendsfdf2(float x);
    TEST_ASSERT(__extendsfdf2(4.0f) == 4.0);
    extern int __ffsdi2(uint64_t x);
    TEST_ASSERT(__ffsdi2(0x8000000000000000ULL) == 64);
    extern int __ffssi2(unsigned x);
    TEST_ASSERT(__ffssi2(0x80000000) == 32);
    extern int64_t __fixdfdi(double x);
    TEST_ASSERT(__fixdfdi(4.0) == 4LL);
    extern int __fixdfsi(double x);
    TEST_ASSERT(__fixdfsi(4.0) == 4);
    extern int64_t __fixsfdi(float x);
    TEST_ASSERT(__fixsfdi(4.0f) == 4LL);
    extern unsigned __fixunsdfsi(double x);
    TEST_ASSERT(__fixunsdfsi(16.0) == 16);
    extern uint64_t __fixunssfdi(float x);
    TEST_ASSERT(__fixunssfdi(16.0f) == 16);
    extern unsigned __fixunssfsi(float x);
    TEST_ASSERT(__fixunssfsi(16.0f) == 16);
    extern double __floatdidf(int64_t);
    TEST_ASSERT(__floatdidf(-1LL) == -1.0f);
    extern float __floatdisf(int64_t);
    TEST_ASSERT(__floatdisf(-1LL) == -1.0f);
    extern double __floatsidf(int x);
    TEST_ASSERT(__floatsidf(-1) == -1.0);

    extern double __floatundidf(uint64_t x);
    TEST_ASSERT(__floatundidf(16) == 16.0);
    extern float __floatundisf(uint64_t x);
    TEST_ASSERT(__floatundisf(16) == 16.0f);
    extern double __floatunsidf(unsigned x);
    TEST_ASSERT(__floatunsidf(16) == 16.0);
    extern int __gedf2(double x, double y);
    extern int __gtdf2(double x, double y);
    TEST_ASSERT(__gtdf2(2.0, 0.0) >= 0);
    extern int __ledf2(double x, double y);
    TEST_ASSERT(__ledf2(0.0, 2.0) <= 0);

    extern int64_t __lshrdi3(int64_t x, int y);
    TEST_ASSERT(__lshrdi3(0x8000000000000000LL, 1) == 0x4000000000000000LL);
    extern int __ltdf2(double x, double y);
    TEST_ASSERT(__ltdf2(0.0, 2.0) < 0);

    extern int64_t __moddi3(int64_t x, int64_t y);
    TEST_ASSERT(__moddi3(15, 2) == 1);
    extern int __modsi3(int x, int y);
    TEST_ASSERT(__modsi3(15, 2) == 1);
    extern complex double __muldc3(double a, double b, double c, double d);
    TEST_ASSERT(__muldc3(1.0, 0.0, 0.0, 1.0) == I);
    extern double __muldf3(double x, double y);
    TEST_ASSERT(__muldf3(2.0, 8.0) == 16.0);
    extern int64_t __muldi3(int64_t x, int64_t y);
    TEST_ASSERT(__muldi3(2, 8) == 16);
    extern complex float __mulsc3(float a, float b, float c, float d);
    TEST_ASSERT(__mulsc3(1.0f, 0.0f, 0.0f, -1.0f) == -I);

    extern int __mulsi3(int x, int y);
    TEST_ASSERT(__mulsi3(2, 8) == 16);
    extern int __mulvdi3(int64_t x, int64_t y);
    TEST_ASSERT(__mulvdi3(2, 8) == 16);
    extern int __mulvsi3(int x, int y);
    TEST_ASSERT(__mulvsi3(2, 8) == 16);
    extern int __nedf2(double x, double y);
    TEST_ASSERT(__nedf2(2.0, 2.0) == 0);
    extern double __negdf2(double x);
    TEST_ASSERT(__negdf2(1.0) == -1.0);
    extern int64_t __negdi2(int64_t x);
    TEST_ASSERT(__negdi2(-1LL) == 1);

    extern int64_t __negvdi2(int64_t x);
    TEST_ASSERT(__negvdi2(-1LL) == 1);
    extern int __negvsi2(int x);
    TEST_ASSERT(__negvsi2(-1) == 1);

    extern int __paritysi2(unsigned x);
    TEST_ASSERT(__paritysi2(0x10101010) == 0);
    extern int __popcountdi2(uint64_t);
    TEST_ASSERT(__popcountdi2(0xaaaaaaaa11111111ULL) == 24);
    extern int __popcountsi2(unsigned x);
    TEST_ASSERT(__popcountsi2(0x11111111) == 8);
    extern double __powidf2(double x, int y);
    TEST_ASSERT(__powidf2(2.0, -1) == 0.5);
    extern float __powisf2(float x, int y);
    TEST_ASSERT(__powisf2(2.0f, 2) == 4.0f);
    extern double __subdf3(double x, double y);
    TEST_ASSERT(__subdf3(2.0, 1.0) == 1.0);

    extern int64_t __subvdi3(int64_t x, int64_t y);
    TEST_ASSERT(__subvdi3(-1LL, -1LL) == 0);
    extern int __subvsi3(int x, int y);
    TEST_ASSERT(__subvsi3(-1, -1) == 0);
    extern float __truncdfsf2(double x);
    TEST_ASSERT(__truncdfsf2(4.0) == 4.0f);
    extern int __ucmpdi2(uint64_t x, uint64_t y);
    TEST_ASSERT(__ucmpdi2(0x100000000ULL, 0x100000000ULL) == 1);
    extern uint64_t __udivdi3(uint64_t x, uint64_t y);
    TEST_ASSERT(__udivdi3(15, 2) == 7);
    extern uint64_t __udivmoddi4(uint64_t x, uint64_t y, uint64_t *z);
    uint64_t z;
    TEST_ASSERT(__udivmoddi4(15, 2, &z) == 7);
    TEST_ASSERT(z == 1);
    extern unsigned __udivsi3(unsigned x, unsigned y);
    TEST_ASSERT(__udivsi3(15, 2) == 7);
    extern uint64_t __umoddi3(uint64_t x, uint64_t y);
    TEST_ASSERT(__umoddi3(15, 2) == 1);
    extern unsigned __umodsi3(unsigned x, unsigned y);
    TEST_ASSERT(__umodsi3(15, 2) == 1);
#ifndef __riscv
    extern uint64_t __umulsidi3(unsigned x, unsigned y);
    TEST_ASSERT(__umulsidi3(0x10000000, 0x10000000) == 0x100000000000000ULL);
#endif
    extern int __unorddf2(double x, double y);
    TEST_ASSERT(__unorddf2(1.0, 2.0) == 0);

#if ESP_ROM_HAS_SW_FLOAT
    extern float __addsf3(float x, float y);
    TEST_ASSERT(__addsf3(1.0f, 4.0f) == 5.0f);
    extern int __eqsf2(float x, float y);
    TEST_ASSERT(__eqsf2(4.0f, 4.0f) == 0);
    extern int __fixsfsi(float x);
    TEST_ASSERT(__fixsfsi(4.0f) == 4);
    extern float __floatsisf(int x);
    TEST_ASSERT(__floatsisf(-1) == -1.0f);
    extern float __floatunsisf(unsigned x);
    TEST_ASSERT(__floatunsisf(16) == 16.0f);
    TEST_ASSERT(__gedf2(2.0, 0.0) >= 0);
    extern int __gesf2(float x, float y);
    TEST_ASSERT(__gesf2(2.0f, 0.0f) >= 0);
    extern int __gtsf2(float x, float y);
    TEST_ASSERT(__gtsf2(2.0f, 0.0f) >= 0);
    extern int __lesf2(float x, float y);
    TEST_ASSERT(__lesf2(0.0f, 2.0f) <= 0);
    extern int __ltsf2(float x, float y);
    TEST_ASSERT(__ltsf2(0.0f, 2.0f) < 0);
    extern float __mulsf3(float a, float b);
    TEST_ASSERT(__mulsf3(2.0f, 8.0f) == 16.0f);
    extern float __negsf2(float x);
    TEST_ASSERT(__negsf2(-1.0f) == 1.0f);
    extern int __nesf2(float x, float y);
    TEST_ASSERT(__nesf2(2.0, 0.0) != 0);
    extern float __subsf3(float x, float y);
    TEST_ASSERT(__subsf3(5.0f, 4.0f) == 1.0f);
    extern int __unordsf2(float x, float y);
    TEST_ASSERT(__unordsf2(2.0f, 1.0f) == 0);
#endif //ESP_ROM_HAS_SW_FLOAT
}
