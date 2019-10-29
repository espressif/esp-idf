from __future__ import print_function
import os
import sys

try:
    import IDF
    from IDF.IDFDUT import ESP32DUT
except ImportError:
    # this is a test case write with tiny-test-fw.
    # to run test cases outside tiny-test-fw,
    # we need to set environment variable `TEST_FW_PATH`,
    # then get and insert `TEST_FW_PATH` to sys path before import FW module
    test_fw_path = os.getenv('TEST_FW_PATH')
    if test_fw_path and test_fw_path not in sys.path:
        sys.path.insert(0, test_fw_path)
    import IDF

STATS_TASK_ITERS = 3
STATS_TASK_EXPECT = "Real time stats obtained"


@IDF.idf_example_test(env_tag='Example_WIFI')
def test_real_time_stats_example(env, extra_data):
    dut = env.get_dut('real_time_stats', 'examples/system/freertos/real_time_stats', dut_class=ESP32DUT)
    dut.start_app()

    for iteration in range(0, STATS_TASK_ITERS):
        dut.expect(STATS_TASK_EXPECT)


if __name__ == '__main__':
    test_real_time_stats_example()
