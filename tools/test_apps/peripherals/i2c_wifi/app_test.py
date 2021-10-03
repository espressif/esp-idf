#!/usr/bin/env python
import tiny_test_fw
import ttfw_idf


@ttfw_idf.idf_custom_test(env_tag='Example_GENERIC')
def test_startup(env, _):  # type: (tiny_test_fw.Env.Env, None) -> None
    # Only test for master usage.
    dut = env.get_dut('i2c_wifi', 'tools/test_apps/peripherals/i2c_wifi')
    dut.start_app()

    dut.expect('I2C-WIFI test success')

    env.close_dut(dut.name)


if __name__ == '__main__':
    test_startup()
