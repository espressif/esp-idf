from __future__ import unicode_literals

import os

import ttfw_idf
from tiny_test_fw import Env, Utility


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC')
def test_examples_eventfd(env, extra_data):
    # type: (Env, None) -> None

    dut = env.get_dut('eventfd', 'examples/system/eventfd')
    dut.start_app()

    dut.expect('cpu_start: Starting scheduler', timeout=30)

    exp_list_5seconds = [
        'eventfd_example: Select timeouted for 1 times',
        'eventfd_example: Timer triggerred for 2 times',
        'eventfd_example: Progress triggerred for 1 times',
    ]

    exp_list_10seconds = [
        'eventfd_example: Select timeouted for 2 times',
        'eventfd_example: Timer triggerred for 4 times',
        'eventfd_example: Progress triggerred for 2 times',
    ]

    Utility.console_log('Expecting:{}{}'.format(os.linesep, os.linesep.join(exp_list_5seconds)))
    dut.expect_all(*exp_list_5seconds, timeout=60)

    Utility.console_log('Expecting:{}{}'.format(os.linesep, os.linesep.join(exp_list_10seconds)))
    dut.expect_all(*exp_list_10seconds, timeout=60)


if __name__ == '__main__':
    test_examples_eventfd()
