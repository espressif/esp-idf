from __future__ import print_function

import ttfw_idf


@ttfw_idf.idf_component_unit_test(env_tag='COMPONENT_UT_GENERIC')
def test_component_ut_esp_netif(env, extra_data):
    dut = env.get_dut('esp_netif', 'components/esp_netif/test_apps')
    dut.start_app()
    stdout = dut.expect('Tests finished', full_stdout=True)
    ttfw_idf.ComponentUTResult.parse_result(stdout)


if __name__ == '__main__':
    test_component_ut_esp_netif()
