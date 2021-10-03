from __future__ import print_function

import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC', target=['esp32', 'esp32s2', 'esp32c3'])
def test_custom_bootloader_impl_example(env, _):  # type: ignore
    # Test with default build configurations
    dut = env.get_dut('main', 'examples/custom_bootloader/bootloader_override')
    dut.start_app()

    # Expect to read a message from the custom bootloader
    # This message is defined in the Kconfig file, retrieve it while deleting
    # leading and trailing quotes (")
    welcome_message = dut.app.get_sdkconfig()['CONFIG_EXAMPLE_BOOTLOADER_WELCOME_MESSAGE'].strip("\"")
    dut.expect(welcome_message)

    # Expect to read a message from the user application
    dut.expect('Application started!')


if __name__ == '__main__':
    test_custom_bootloader_impl_example()
