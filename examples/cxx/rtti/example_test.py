from __future__ import print_function

import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC')
def test_cpp_rtti_example(env, extra_data):
    dut = env.get_dut('cpp_rtti', 'examples/cxx/rtti', dut_class=ttfw_idf.ESP32DUT)
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
    test_cpp_rtti_example()
