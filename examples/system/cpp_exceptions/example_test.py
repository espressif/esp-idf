from __future__ import print_function
import os
import sys

try:
    import IDF
    from IDF.IDFDUT import ESP32DUT
except ImportError:
    test_fw_path = os.getenv('TEST_FW_PATH')
    if test_fw_path and test_fw_path not in sys.path:
        sys.path.insert(0, test_fw_path)
    import IDF


@IDF.idf_example_test(env_tag='Example_WIFI')
def test_examples_system_cpp_exceptions(env, extra_data):
    dut = env.get_dut('cpp_exceptions_example', 'examples/system/cpp_exceptions', dut_class=ESP32DUT)
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
