from tiny_test_fw import Utility
import random
import re
import time
import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC')
def test_examples_nvs_rw_blob(env, extra_data):

    dut = env.get_dut('nvs_rw_blob', 'examples/storage/nvs_rw_blob')
    dut.start_app()

    def expect_start_msg(index):
        dut.expect_all('Restart counter = {}'.format(index),
                       'Run time:',
                       timeout=10)

    expect_start_msg(0)
    dut.expect('Nothing saved yet!', timeout=5)

    nvs_store = []
    for i in range(1, 10):
        time.sleep(random.uniform(0.1, 2))  # in order to randomize the runtimes stored in NVS
        try:
            # Pulling GPIO0 low using DTR
            dut.port_inst.setDTR(True)
            dut.expect('Restarting...', timeout=5)  # the application waits for a second
        finally:
            dut.port_inst.setDTR(False)

        expect_start_msg(i)

        dut.expect_all(*nvs_store, timeout=10)
        Utility.console_log('Received: {}'.format(', '.join(nvs_store)))

        new_runtime = dut.expect(re.compile(r'{}: (\d+)'.format(i)), timeout=10)[0]
        nvs_store.append('{}: {}'.format(i, new_runtime))
        Utility.console_log('loop {} has finished with runtime {}'.format(i, new_runtime))


if __name__ == '__main__':
    test_examples_nvs_rw_blob()
