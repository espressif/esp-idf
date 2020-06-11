from __future__ import unicode_literals
import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC')
def test_examples_task_watchdog(env, extra_data):

    dut = env.get_dut('task_watchdog', 'examples/system/task_watchdog')
    dut.start_app()

    dut.expect_all('Initialize TWDT',
                   'Delay for 10 seconds', timeout=30)
    dut.expect_all('Unsubscribing and deleting tasks',
                   'Complete', timeout=20)


if __name__ == '__main__':
    test_examples_task_watchdog()
