#!/usr/bin/env python

from __future__ import division, print_function, unicode_literals

import re
from typing import Any

import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC', target=['esp32', 'esp32s2', 'esp32c3'])
def test_examples_pthread(env, _):  # type: (Any, Any) -> None
    app_name = 'pthread'
    dut = env.get_dut(app_name, 'examples/system/pthread')
    dut.start_app()

    # Note: this test doesn't really confirm anything, except that threads are created
    # and stdout is not being corrupted by multiple threads printing ot it.
    dut.expect(re.compile(r'Created thread 0x[\da-f]+'))
    dut.expect(re.compile(r'Created larger stack thread 0x[\da-f]+'))
    dut.expect(r'Threads have exited')
    dut.expect(re.compile(r'Created thread 0x[\da-f]+ with new default config'))
    dut.expect('Thread has exited')


if __name__ == '__main__':
    test_examples_pthread()
