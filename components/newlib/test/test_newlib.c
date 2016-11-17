#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include "unity.h"


TEST_CASE("test ctype functions", "[newlib]")
{
    TEST_ASSERT_TRUE( isalnum('a') && isalnum('A') && isalnum('z') && isalnum('Z') && isalnum('0') && isalnum('9') );
    TEST_ASSERT_FALSE( isalnum('(') || isalnum('-') || isalnum(' ') || isalnum('\x81') || isalnum('.') || isalnum('\\') );
    TEST_ASSERT_TRUE( isalpha('a') && isalpha('A') && isalpha('z') && isalpha('Z') );
    TEST_ASSERT_FALSE( isalpha('0') || isalpha('9') || isalpha(')') || isalpha('\t') || isalpha(' ') || isalpha('\x81') );
    TEST_ASSERT_TRUE( isspace(' ') && isspace('\t') && isspace('\n') && isspace('\r') );
    TEST_ASSERT_FALSE( isspace('0') || isspace('9') || isspace(')') || isspace('A') || isspace('*') || isspace('\x81') || isspace('a'));
}

TEST_CASE("test atoX functions", "[newlib]")
{
    TEST_ASSERT_EQUAL_INT(-2147483648, atoi("-2147483648"));
    TEST_ASSERT_EQUAL_INT(2147483647, atoi("2147483647"));
    TEST_ASSERT_EQUAL_INT(42, atoi("000000042"));
    TEST_ASSERT_EQUAL_INT(0, strtol("foo", NULL, 10));
}

TEST_CASE("test sprintf function", "[newlib]")
{
    char *res = NULL;
    asprintf(&res, "%d %011i %lu %p %x %c %.4f\n", 42, 2147483647, 2147483648UL, (void *) 0x40010000, 0x40020000, 'Q', 1.0f / 137.0f);
    TEST_ASSERT_NOT_NULL(res);
    TEST_ASSERT_EQUAL_STRING(res, "42 02147483647 2147483648 0x40010000 40020000 Q 0.0073\n");
    free(res);
}

TEST_CASE("test sscanf function", "[newlib]")
{
    const char *src = "42 02147483647 2147483648 0x40010000 40020000 Q 0.0073\n";
    int fourty_two;
    int int_max;
    unsigned long int_max_plus_one;
    void *iram_ptr;
    int irom_ptr;
    char department;
    float inv_fine_structure_constant;
    int res = sscanf(src, "%d %d %lu %p %x %c %f", &fourty_two, &int_max, &int_max_plus_one, &iram_ptr, &irom_ptr, &department, &inv_fine_structure_constant);
    TEST_ASSERT_EQUAL(7, res);
    TEST_ASSERT_EQUAL(42, fourty_two);
    TEST_ASSERT_EQUAL(2147483647, int_max);
    TEST_ASSERT_EQUAL_UINT32(2147483648UL, int_max_plus_one);
    TEST_ASSERT_EQUAL(0x40010000, iram_ptr);
    TEST_ASSERT_EQUAL(0x40020000, irom_ptr);
    TEST_ASSERT_EQUAL('Q', department);
    TEST_ASSERT_TRUE(1.0f / inv_fine_structure_constant > 136 && 1.0f / inv_fine_structure_constant < 138);
}

TEST_CASE("test time functions", "[newlib]")
{
    time_t now = 1464248488;
    setenv("TZ", "UTC-8", 1);
    struct tm *tm_utc = gmtime(&now);
    TEST_ASSERT_EQUAL( 28, tm_utc->tm_sec);
    TEST_ASSERT_EQUAL( 41, tm_utc->tm_min);
    TEST_ASSERT_EQUAL(  7, tm_utc->tm_hour);
    TEST_ASSERT_EQUAL( 26, tm_utc->tm_mday);
    TEST_ASSERT_EQUAL(  4, tm_utc->tm_mon);
    TEST_ASSERT_EQUAL(116, tm_utc->tm_year);
    TEST_ASSERT_EQUAL(  4, tm_utc->tm_wday);
    TEST_ASSERT_EQUAL(146, tm_utc->tm_yday);

    struct tm *tm_local = localtime(&now);
    TEST_ASSERT_EQUAL( 28, tm_local->tm_sec);
    TEST_ASSERT_EQUAL( 41, tm_local->tm_min);
    TEST_ASSERT_EQUAL( 15, tm_local->tm_hour);
    TEST_ASSERT_EQUAL( 26, tm_local->tm_mday);
    TEST_ASSERT_EQUAL(  4, tm_local->tm_mon);
    TEST_ASSERT_EQUAL(116, tm_local->tm_year);
    TEST_ASSERT_EQUAL(  4, tm_local->tm_wday);
    TEST_ASSERT_EQUAL(146, tm_local->tm_yday);

}


static int checkFnRom(void *fn, char *name)
{
    int fnaddr = (int)fn;
    printf("%s: 0X%x\n", name, fnaddr);
    if ((fnaddr >= 0x40000000) && (fnaddr < 0x40070000)) {
        return 1;
    } else {
        return 0;
    }
}


TEST_CASE("check if ROM is used for functions", "[newlib]")
{
    TEST_ASSERT(checkFnRom(printf, "printf"));
    TEST_ASSERT(checkFnRom(sscanf, "sscanf"));
    TEST_ASSERT(checkFnRom(atoi, "atoi"));
    TEST_ASSERT(checkFnRom(strtol, "strtol"));
}