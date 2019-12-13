from __future__ import print_function

import ttfw_idf

STATS_TASK_ITERS = 3
STATS_TASK_EXPECT = "Real time stats obtained"


@ttfw_idf.idf_example_test(env_tag='Example_WIFI')
def test_real_time_stats_example(env, extra_data):
    dut = env.get_dut('real_time_stats', 'examples/system/freertos/real_time_stats')
    dut.start_app()

    for iteration in range(0, STATS_TASK_ITERS):
        dut.expect(STATS_TASK_EXPECT)


if __name__ == '__main__':
    test_real_time_stats_example()
