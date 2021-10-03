from __future__ import unicode_literals

import re

import tiny_test_fw
import ttfw_idf
from tiny_test_fw import DUT


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC', target=['esp32s2'])
def test_examples_ulp_riscv(env, extra_data):  # type: (tiny_test_fw.Env.Env, None) -> None # pylint: disable=unused-argument
    dut = env.get_dut('ulp_riscv', 'examples/system/ulp_riscv/gpio')
    dut.start_app()

    dut.expect_all('Not a ULP-RISC-V wakeup, initializing it!',
                   'Entering in deep sleep',
                   timeout=30)

    # Run two times to make sure device sleep
    # and wake up properly
    for i in range(0, 2):
        # Set GPIO0 using DTR
        dut.port_inst.setDTR(i % 2 == 0)

        dut.expect('ULP-RISC-V woke up the main CPU!', timeout=5)

        # Check GPIO state
        state = 'Low' if i % 2 == 0 else 'High'
        dut.expect(re.compile(r'ULP-RISC-V read changes in GPIO_0 current is: %s' % state), timeout=5)

        # Go back to sleep
        dut.expect('Entering in deep sleep', timeout=5)

        try:
            # We expect a timeout here, otherwise it means that
            # the main CPU woke up unexpectedly!
            dut.expect('ULP-RISC-V woke up the main CPU!', timeout=20)
            raise Exception('Main CPU woke up unexpectedly!')
        except DUT.ExpectTimeout:
            pass


if __name__ == '__main__':
    test_examples_ulp_riscv()
