from __future__ import print_function

import re

import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC', target=['esp32', 'esp32s2', 'esp32c3'])
def test_startup_time_example(env, extra_data):
    app_name = 'startup_time'
    dut = env.get_dut(app_name, 'examples/system/startup_time')
    dut.start_app()

    res = dut.expect(re.compile(r'\((\d+)\) [^:]+: App started!'))
    time = int(res[0])

    # Allow ci-dashboard to track startup times
    print('------ startup time info ------\n'
          '[app_name] {}\n'
          '[startup_time] {}\n'
          '[config] {}\n'
          '[target] {}\n'
          '------ startup time end ------'.format(app_name,
                                                  time,
                                                  dut.app.config_name,
                                                  dut.TARGET))


if __name__ == '__main__':
    test_startup_time_example()
