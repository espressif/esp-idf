from __future__ import unicode_literals

import re
import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC', target=['esp32s2'])
def test_examples_ulp_riscv(env, extra_data):
    dut = env.get_dut('ulp_riscv', 'examples/system/ulp_riscv')
    dut.start_app()

    dut.expect_all('Not a ULP-RISC-V wakeup, initializing it!',
                   'Entering in deep sleep',
                   timeout=30)

    # Run two times to make sure device sleep
    # and wake up properly
    for i in range(0, 2):
        # Pulling GPIO0 low using DTR
        dut.port_inst.setDTR(True)

        dut.expect('ULP-RISC-V woke up the main CPU!', timeout=5)

        # We pulled GPIO0 low previously
        dut.expect(re.compile(r'ULP-RISC-V read changes in GPIO_0 current is: Low'), timeout=5)

        # Go back to sleep
        dut.expect('Entering in deep sleep', timeout=5)


if __name__ == '__main__':
    test_examples_ulp_riscv()
