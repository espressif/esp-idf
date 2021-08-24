from __future__ import unicode_literals

import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC', target=['esp32'])
def test_examples_ipc_isr(env, _):  # type: (ttfw_idf.TinyFW.Env, None) -> None

    dut = env.get_dut('ipc_isr', 'examples/system/ipc/ipc_isr')
    dut.start_app()

    dut.expect_all('example: Start',
                   'example: PS_INTLEVEL = 0x5',
                   'example: PS_EXCM = 0x0',
                   'example: PS_UM = 0x1',
                   'example: in[0] = 0x1',
                   'example: in[1] = 0x2',
                   'example: in[2] = 0x3',
                   'example: out[0] = (in[0] | in[1] | in[2]) = 0x3',
                   'example: out[1] = (in[0] & in[1] & in[2]) = 0x6',
                   'example: out[2] = in[2] = 0x3',
                   'example: out[3] = PS of other cpu = 0x25',
                   'example: End',
                   timeout=10)


if __name__ == '__main__':
    test_examples_ipc_isr()
