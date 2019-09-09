from __future__ import print_function
import os
import sys

EXPECT_TIMEOUT = 20

try:
    import IDF
    from IDF.IDFDUT import ESP32DUT
except ImportError:
    test_fw_path = os.getenv("TEST_FW_PATH")
    if test_fw_path and test_fw_path not in sys.path:
        sys.path.insert(0, test_fw_path)
    import IDF


@IDF.idf_example_test(env_tag='Example_I2C_CCS811_SENSOR')
def test_i2ctools_example(env, extra_data):
    # Get device under test, flash and start example. "i2ctool" must be defined in EnvConfig
    dut = env.get_dut('i2ctools', 'examples/peripherals/i2c/i2c_tools', dut_class=ESP32DUT)
    dut.start_app()
    dut.expect("esp32>", timeout=EXPECT_TIMEOUT)
    # Get i2c address
    dut.write("i2cdetect")
    dut.expect("5b", timeout=EXPECT_TIMEOUT)
    # Get chip ID
    dut.write("i2cget -c 0x5b -r 0x20 -l 1")
    dut.expect("0x81", timeout=EXPECT_TIMEOUT)
    # Reset sensor
    dut.write("i2cset -c 0x5b -r 0xFF 0x11 0xE5 0x72 0x8A")
    dut.expect("OK", timeout=EXPECT_TIMEOUT)
    # Get status
    dut.write("i2cget -c 0x5b -r 0x00 -l 1")
    dut.expect_any("0x10", timeout=EXPECT_TIMEOUT)
    # Change work mode
    dut.write("i2cset -c 0x5b -r 0xF4")
    dut.expect("OK", timeout=EXPECT_TIMEOUT)
    dut.write("i2cset -c 0x5b -r 0x01 0x10")
    dut.expect("OK", timeout=EXPECT_TIMEOUT)
    # Get new status
    dut.write("i2cget -c 0x5b -r 0x00 -l 1")
    dut.expect_any("0x98", "0x90", timeout=EXPECT_TIMEOUT)


if __name__ == '__main__':
    test_i2ctools_example()
