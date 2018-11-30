from __future__ import print_function
import re
import os
import sys

# this is a test case write with tiny-test-fw.
# to run test cases outside tiny-test-fw,
# we need to set environment variable `TEST_FW_PATH`,
# then get and insert `TEST_FW_PATH` to sys path before import FW module
test_fw_path = os.getenv('TEST_FW_PATH')
if test_fw_path and test_fw_path not in sys.path:
    sys.path.insert(0, test_fw_path)

import TinyFW
import IDF

TASK_ITERATION_LIMIT = 10

TASK_ITERATION_POSTING = "posting TASK_EVENTS:TASK_ITERATION_EVENT to {}, iteration {} out of " + str(TASK_ITERATION_LIMIT)
TASK_ITERATION_HANDLING = "handling TASK_EVENTS:TASK_ITERATION_EVENT from {}, iteration {}"

@IDF.idf_example_test(env_tag='Example_WIFI')
def test_user_event_loops_example(env, extra_data):
    dut = env.get_dut('user_event_loops', 'examples/system/event/user_event_loops')

    dut.start_app()

    dut.expect("setting up")
    dut.expect("starting event source")
    dut.expect("starting application task")
    print("Finished setup")

    for iteration in range(1, TASK_ITERATION_LIMIT + 1):
        loop = None

        if (iteration % 2 == 0):
            loop = "loop_with_task"
        else:
            loop = "loop_without_task"

        dut.expect(TASK_ITERATION_POSTING.format(loop, iteration))
        print("Posted iteration {} to {}".format(iteration, loop))
        dut.expect(TASK_ITERATION_HANDLING.format(loop, iteration))
        print("Handled iteration {} from {}".format(iteration, loop))

    dut.expect("deleting task event source")
    print("Deleted task event source")

if __name__ == '__main__':
    test_user_event_loops_example()
