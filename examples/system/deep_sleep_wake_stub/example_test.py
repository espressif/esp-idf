from __future__ import unicode_literals

import time

import tiny_test_fw
import ttfw_idf

try:
    import typing  # noqa: F401 # pylint: disable=unused-import
except ImportError:
    pass


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC', target=['esp32', 'esp32s2', 'esp32c3', 'esp32s3'])
def test_deep_sleep_wake_stub(env, extra_data):  # type: (tiny_test_fw.Env, typing.Any) -> None
    dut = env.get_dut('deep_sleep', 'examples/system/deep_sleep_wake_stub')
    dut.start_app()

    dut.expect('Enabling timer wakeup, 10s', timeout=30)
    dut.expect('Entering deep sleep', timeout=10)

    start_sleep = time.time()
    print('Waiting for wakeup...')
    dut.expect('wake stub: going to deep sleep', timeout=15)

    sleep_time = time.time() - start_sleep
    print('Host measured sleep time at {:.2f}s'.format(sleep_time))
    assert 8 < sleep_time < 12  # note: high tolerance as measuring time on the host may have some timing skew


if __name__ == '__main__':
    test_deep_sleep_wake_stub()
