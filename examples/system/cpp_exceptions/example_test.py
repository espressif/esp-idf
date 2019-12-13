from __future__ import print_function

import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_WIFI')
def test_examples_system_cpp_exceptions(env, extra_data):
    dut = env.get_dut('cpp_exceptions_example', 'examples/system/cpp_exceptions')
    # start test
    dut.start_app()
    lines = ['app_main starting',
             'In constructor, arg=42',
             'In constructor, arg=0',
             'In destructor, m_arg=42',
             'Exception caught: Exception in constructor',
             'app_main done'
             ]
    for line in lines:
        dut.expect(line, timeout=2)


if __name__ == '__main__':
    test_examples_system_cpp_exceptions()
