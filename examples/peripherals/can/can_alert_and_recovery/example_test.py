# Need Python 3 string formatting functions
from __future__ import print_function

import ttfw_idf

# CAN Self Test Example constants
STR_EXPECT = ("CAN Alert and Recovery: Driver installed", "CAN Alert and Recovery: Driver uninstalled")
EXPECT_TIMEOUT = 20


@ttfw_idf.idf_example_test(env_tag='Example_CAN1')
def test_can_alert_and_recovery_example(env, extra_data):
    dut = env.get_dut('dut1', 'examples/peripherals/can/can_alert_and_recovery')
    dut.start_app()

    for string in STR_EXPECT:
        dut.expect(string, timeout=EXPECT_TIMEOUT)


if __name__ == '__main__':
    test_can_alert_and_recovery_example()
