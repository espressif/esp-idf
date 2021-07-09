from __future__ import unicode_literals

import re
from typing import Any

import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC', target=['esp32', 'esp32c3'])
def test_examples_timergroup(env, extra_data):  # type: (Any, Any) -> None
    dut = env.get_dut('timer_group', 'examples/peripherals/timer_group')
    dut.start_app()

    # check auto reload function
    with_auto_reload = dut.expect(re.compile(r'Timer Group (\S+) auto reload'), timeout=30)[0]
    assert with_auto_reload == 'with'
    select_groups = dut.expect(re.compile(r'Group\[(\d)\], timer\[(\d)\] alarm event'))
    timer_group_num = int(select_groups[0])
    timer_instance_num = int(select_groups[1])
    assert timer_group_num == 0 and timer_instance_num == 0
    dut.expect('EVENT TIME')
    counter_value = dut.expect(re.compile(r'Counter:\s+(0x\d+)'))[0]
    counter_value = int(counter_value, 16)
    print('counter value at auto reload event: ', counter_value)
    assert counter_value < 20

    # check timer interval
    dut.expect('Timer Group without auto reload', timeout=5)
    dut.expect('EVENT TIME')
    event_time0 = dut.expect(re.compile(r'Time\s+:\s+(\d+\.\d+)\s+s'))[0]
    print('event0={}'.format(event_time0))


if __name__ == '__main__':
    test_examples_timergroup()
