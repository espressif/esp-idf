from __future__ import print_function
import os
import sys

try:
    import IDF
except ImportError:
    # this is a test case write with tiny-test-fw.
    # to run test cases outside tiny-test-fw,
    # we need to set environment variable `TEST_FW_PATH`,
    # then get and insert `TEST_FW_PATH` to sys path before import FW module
    test_fw_path = os.getenv('TEST_FW_PATH')
    if test_fw_path and test_fw_path not in sys.path:
        sys.path.insert(0, test_fw_path)
    import IDF


@IDF.idf_example_test(env_tag='Example_WIFI')
def test_real_time_stats_example(env, extra_data):
    dut = env.get_dut('cpp_rtti', 'examples/system/cpp_rtti')
    dut.start_app()

    dut.expect('Type name of std::cout is: std::ostream')
    dut.expect('Type name of std::cin is: std::istream')
    dut.expect('Type of app_main is: void ()')
    dut.expect('Type name of a lambda function is: app_main::{lambda(int, int)#1}')

    dut.expect('dynamic_cast<DerivedA*>(obj)=0')
    dut.expect('dynamic_cast<DerivedB*>(obj)=0x')
    dut.expect('dynamic_cast<DerivedB*>(obj)=0')
    dut.expect('dynamic_cast<DerivedA*>(obj)=0x')

    dut.expect('Example finished.')


if __name__ == '__main__':
    test_real_time_stats_example()
