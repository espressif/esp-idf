# Need Python 3 string formatting functions
from __future__ import print_function

import os
import sys

try:
    import IDF
except ImportError:
    # The test cause is dependent on the Tiny Test Framework. Ensure the
    # `TEST_FW_PATH` environment variable is set to `$IDF_PATH/tools/tiny-test-fw`
    test_fw_path = os.getenv("TEST_FW_PATH")
    if test_fw_path and test_fw_path not in sys.path:
        sys.path.insert(0, test_fw_path)
    import IDF

# CAN Self Test Example constants
STR_EXPECT = ("CAN Alert and Recovery: Driver installed", "CAN Alert and Recovery: Driver uninstalled")
EXPECT_TIMEOUT = 20


@IDF.idf_example_test(env_tag='Example_CAN1', ignore=True)
def test_can_alert_and_recovery_example(env, extra_data):
    # Get device under test, flash and start example. "dut4" must be defined in EnvConfig
    dut = env.get_dut('dut1', 'examples/peripherals/can/can_alert_and_recovery')
    dut.start_app()

    for string in STR_EXPECT:
        dut.expect(string, timeout=EXPECT_TIMEOUT)


if __name__ == '__main__':
    test_can_alert_and_recovery_example()
