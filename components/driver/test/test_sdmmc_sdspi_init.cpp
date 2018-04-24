#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"


/**
 * Check that C-style designated initializers are valid in C++ file.
 */
static void test_initializers() __attribute__((unused));

static void test_initializers()
{
    sdmmc_host_t sdmmc_host = SDMMC_HOST_DEFAULT();
    (void) sdmmc_host;
    sdmmc_slot_config_t sdmmc_slot = SDMMC_SLOT_CONFIG_DEFAULT();
    (void) sdmmc_slot;
    sdmmc_host_t sdspi_host = SDSPI_HOST_DEFAULT();
    (void) sdspi_host;
    sdspi_slot_config_t sdspi_slot = SDSPI_SLOT_CONFIG_DEFAULT();
    (void) sdspi_slot;
}
