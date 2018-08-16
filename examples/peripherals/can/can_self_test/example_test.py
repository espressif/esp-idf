#Need Python 3 string formatting functions
from __future__ import print_function

import re
import os
import sys
# The test cause is dependent on the Tiny Test Framework. Ensure the
# `TEST_FW_PATH` environment variable is set to `$IDF_PATH/tools/tiny-test-fw`
test_fw_path = os.getenv("TEST_FW_PATH")
if test_fw_path and test_fw_path not in sys.path:
    sys.path.insert(0, test_fw_path)
import TinyFW
import IDF

# CAN Self Test Example constants
STR_EXPECT = ("CAN Self Test: Driver installed", "CAN Self Test: Driver uninstalled")
EXPECT_TIMEOUT = 20

@IDF.idf_example_test(env_tag='Example_CAN')
def test_can_self_test_example(env, extra_data):
    #Get device under test, flash and start example. "dut4" must be defined in EnvConfig
    dut = env.get_dut('dut4', 'examples/peripherals/can/can_self_test')
    dut.start_app()

    for string in STR_EXPECT:
        dut.expect(string, timeout = EXPECT_TIMEOUT)

if __name__ == '__main__':
    test_can_self_test_example()
