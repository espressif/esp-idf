from tiny_test_fw import Utility
import ttfw_idf

try:
    from itertools import izip_longest as zip_longest
except ImportError:
    # Python 3
    from itertools import zip_longest


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC')
def test_examples_nvs_rw_value_cxx(env, extra_data):

    dut = env.get_dut('nvs_rw_value_cxx', 'examples/storage/nvs_rw_value_cxx')
    dut.start_app()

    for i, counter_state in zip_longest(range(4), ('The value is not initialized yet!', ), fillvalue='Done'):
        dut.expect_all('Opening Non-Volatile Storage (NVS) handle... Done',
                       'Reading restart counter from NVS ... {}'.format(counter_state),
                       'Restart counter = {}'.format(i) if i > 0 else '',
                       'Updating restart counter in NVS ... Done',
                       'Committing updates in NVS ... Done',
                       'Restarting in 10 seconds...',
                       timeout=20)
        Utility.console_log('loop {} has finished'.format(i))


if __name__ == '__main__':
    test_examples_nvs_rw_value_cxx()
