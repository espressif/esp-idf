from __future__ import print_function
import os
import sys
import subprocess

import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC')
def test_examples_parttool(env, extra_data):
    dut = env.get_dut('parttool', 'examples/storage/parttool', dut_class=ttfw_idf.ESP32DUT)
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

    subprocess.check_call([sys.executable, script_path, "--binary", binary_path, "--port", dut.port])


if __name__ == '__main__':
    test_examples_parttool()
