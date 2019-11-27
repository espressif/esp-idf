from __future__ import print_function

import ttfw_idf

EXPECT_TIMEOUT = 20


@ttfw_idf.idf_example_test(env_tag='Example_RMT_IR_PROTOCOLS')
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
