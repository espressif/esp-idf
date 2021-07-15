# type: ignore
from __future__ import print_function

import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC', target=['esp32', 'esp32c3'])
def test_examples_system_console_advanced(env, _):
    dut = env.get_dut('console_example', 'examples/system/console/advanced', app_config_name='history')
    print('Using binary path: {}'.format(dut.app.binary_path))
    dut.start_app()
    dut.expect('Command history enabled')
    env.close_dut(dut.name)

    dut = env.get_dut('console_example', 'examples/system/console/advanced', app_config_name='nohistory')
    print('Using binary path: {}'.format(dut.app.binary_path))
    dut.start_app()
    dut.expect('Command history disabled')


if __name__ == '__main__':
    test_examples_system_console_advanced()
