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

EXPECT_TIMEOUT = 20


@IDF.idf_example_test(env_tag='Example_RMT_IR_PROTOCOLS')
def test_examples_rmt_ir_protocols(env, extra_data):
    dut = env.get_dut('ir_protocols_example', 'examples/peripherals/rmt/ir_protocols', app_config_name='nec')
    print("Using binary path: {}".format(dut.app.binary_path))
    dut.start_app()
    dut.expect("example: Send command 0x20 to address 0x10", timeout=EXPECT_TIMEOUT)
    dut.expect("Scan Code  --- addr: 0x0010 cmd: 0x0020", timeout=EXPECT_TIMEOUT)
    dut.expect("Scan Code (repeat) --- addr: 0x0010 cmd: 0x0020", timeout=EXPECT_TIMEOUT)
    env.close_dut(dut.name)

    dut = env.get_dut('ir_protocols_example', 'examples/peripherals/rmt/ir_protocols', app_config_name='rc5')
    print("Using binary path: {}".format(dut.app.binary_path))
    dut.start_app()
    dut.expect("example: Send command 0x20 to address 0x10", timeout=EXPECT_TIMEOUT)
    dut.expect("Scan Code  --- addr: 0x0010 cmd: 0x0020", timeout=EXPECT_TIMEOUT)
    dut.expect("Scan Code (repeat) --- addr: 0x0010 cmd: 0x0020", timeout=EXPECT_TIMEOUT)
    env.close_dut(dut.name)


if __name__ == '__main__':
    test_examples_rmt_ir_protocols()
