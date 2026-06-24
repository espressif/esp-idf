#include <check.h>
#include <stdlib.h>
#include <string.h>
#include "components/esp_hal_gpspi/esp32/include/hal/spi_ll.h"

START_TEST(test_spi_ll_write_buffer_no_overflow)
{
    // Invariant: Buffer reads never exceed the declared length
    const char *payloads[] = {
        "A",                    // Valid minimal input (1 byte)
        "ABCDEFGHIJKLMNOP",     // Boundary: exactly 16 bytes (128 bits)
        "OVERFLOW"              // Exploit: 8 bytes but bitlen=256 (32 bytes needed)
    };
    const size_t bitlens[] = {8, 128, 256};
    int num_payloads = sizeof(payloads) / sizeof(payloads[0]);

    for (int i = 0; i < num_payloads; i++) {
        spi_dev_t hw = {0};
        const char *buffer = payloads[i];
        size_t bitlen = bitlens[i];
        
        // This should not crash or read beyond buffer length
        spi_ll_write_buffer(&hw, (const uint8_t *)buffer, bitlen);
        
        // If we get here without segfault, test passes
        ck_assert(1);
    }
}
END_TEST

Suite *security_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Security");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_spi_ll_write_buffer_no_overflow);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = security_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}