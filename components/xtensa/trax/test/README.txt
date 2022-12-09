Instructions for running the test:

1. Connect an ESP32 board with JTAG adapter (WROVER-KIT, ETHERNET-KIT)
2. Set up the environment (this can be done by export.sh)
   - Add ESP32 toolchain to PATH, and check that GDB has been built with Python support
   - Add OpenOCD to PATH and set OPENOCD_SCRIPTS
   - Set IDF_PATH
3. Run `make flash test`. Check that "Test pass" is printed.
