from __future__ import print_function
import os
import sys
import subprocess

try:
    import IDF
    from IDF.IDFDUT import ESP32DUT
except ImportError:
    test_fw_path = os.getenv('TEST_FW_PATH')
    if test_fw_path and test_fw_path not in sys.path:
        sys.path.insert(0, test_fw_path)
    import IDF


@IDF.idf_example_test(env_tag='Example_WIFI')
def test_examples_parttool(env, extra_data):
    dut = env.get_dut('parttool', 'examples/storage/parttool', dut_class=ESP32DUT)
    dut.start_app(False)

    # Verify factory firmware
    dut.expect("Partitions Tool Example")
    dut.expect("Example end")

    # Close connection to DUT
    dut.receive_thread.exit()
    dut.port_inst.close()

    # Run the example python script
    script_path = os.path.join(os.getenv("IDF_PATH"), "examples", "storage", "parttool", "parttool_example.py")

    binary_path = ""
    for flash_file in dut.app.flash_files:
        if "parttool.bin" in flash_file[1]:
            binary_path = flash_file[1]
            break

    subprocess.check_call([sys.executable, script_path, "--binary", binary_path])


if __name__ == '__main__':
    test_examples_parttool()
