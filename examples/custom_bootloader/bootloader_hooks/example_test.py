from __future__ import print_function

import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC', target=['esp32', 'esp32s2', 'esp32c3'])
def test_custom_bootloader_hooks_example(env, _):  # type: ignore
    # Test with default build configurations
    dut = env.get_dut('main', 'examples/custom_bootloader/bootloader_hooks')
    dut.start_app()

    # Expect to read both hooks messages
    dut.expect('This hook is called BEFORE bootloader initialization')
    dut.expect('This hook is called AFTER bootloader initialization')


if __name__ == '__main__':
    test_custom_bootloader_hooks_example()
