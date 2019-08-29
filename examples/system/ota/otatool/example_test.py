from __future__ import print_function
import os
import sys
import subprocess

try:
    import IDF
    from IDF.IDFDUT import ESP32DUT
except ImportError:
    # this is a test case write with tiny-test-fw.
    # to run test cases outside tiny-test-fw,
    # we need to set environment variable `TEST_FW_PATH`,
    # then get and insert `TEST_FW_PATH` to sys path before import FW module
    test_fw_path = os.getenv('TEST_FW_PATH')
    if test_fw_path and test_fw_path not in sys.path:
        sys.path.insert(0, test_fw_path)
    import IDF


@IDF.idf_example_test(env_tag='Example_WIFI')
def test_otatool_example(env, extra_data):
    dut = env.get_dut('otatool', 'examples/system/ota/otatool', dut_class=ESP32DUT)

    # Verify factory firmware
    dut.start_app()
    dut.expect("OTA Tool Example")
    dut.expect("Example end")

    # Close connection to DUT
    dut.receive_thread.exit()
    dut.port_inst.close()

    script_path = os.path.join(os.getenv("IDF_PATH"), "examples", "system", "ota", "otatool", "otatool_example.py")
    binary_path = ""

    for flash_file in dut.app.flash_files:
        if "otatool.bin" in flash_file[1]:
            binary_path = flash_file[1]
            break

    subprocess.check_call([sys.executable, script_path, "--binary", binary_path])


if __name__ == '__main__':
    test_otatool_example()
