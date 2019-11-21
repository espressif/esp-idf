from __future__ import print_function
import os
import sys

try:
    import IDF
except ImportError:
    test_fw_path = os.getenv('TEST_FW_PATH')
    if test_fw_path and test_fw_path not in sys.path:
        sys.path.insert(0, test_fw_path)
    import IDF


@IDF.idf_example_test(env_tag='Example_WIFI')
def test_examples_system_console(env, extra_data):
    dut = env.get_dut('console_example', 'examples/system/console', app_config_name='history')
    print("Using binary path: {}".format(dut.app.binary_path))
    dut.start_app()
    dut.expect("Command history enabled")
    env.close_dut(dut.name)

    dut = env.get_dut('console_example', 'examples/system/console', app_config_name='nohistory')
    print("Using binary path: {}".format(dut.app.binary_path))
    dut.start_app()
    dut.expect("Command history disabled")


if __name__ == '__main__':
    test_examples_system_console()
