# fatfs component target tests

This directory contains tests for `fatfs` component which are run on chip targets.

See also [test_fatfs_host](../test_fatfs_host) directory for the tests which run on a Linux host.

Fatfs tests can be executed with different `diskio` backends: `diskio_sdmmc` (SD cards over SD or SPI interface), `diskio_spiflash` (wear levelling in internal flash) and `diskio_rawflash` (read-only, no wear levelling, internal flash). There is one test app here for each of these backends:

- [sdcard](sdcard/) — runs fatfs tests with an SD card over SDMMC or SDSPI interface
- [flash_wl](flash_wl/) - runs fatfs test in a wear_levelling partition in SPI flash
- [flash_ro](flash_ro/) - runs fatfs test in a read-only (no wear levelling) partition in SPI flash

These test apps define:
- test functions
- setup/teardown routines
- build/test configurations
- pytest test runners

The actual test cases (many of which are common between the test apps) are defined in the [test_fatfs_common component](test_fatfs_common/).
