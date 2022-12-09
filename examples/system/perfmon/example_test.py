from __future__ import unicode_literals
import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC')
def test_examples_perfmon(env, extra_data):

    dut = env.get_dut('perfmon', 'examples/system/perfmon')
    dut.start_app()

    dut.expect_all('example: Start',
                   'example: Start test with printing all available statistic',
                   'example: Start test with user defined statistic',
                   'example: The End',
                   timeout=60)


if __name__ == '__main__':
    test_examples_perfmon()
