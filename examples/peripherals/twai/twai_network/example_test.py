# Need Python 3 string formatting functions
from __future__ import print_function

from threading import Thread

import ttfw_idf

# Define tuple of strings to expect for each DUT.
master_expect = ('TWAI Master: Driver installed', 'TWAI Master: Driver uninstalled')
slave_expect = ('TWAI Slave: Driver installed', 'TWAI Slave: Driver uninstalled')
listen_only_expect = ('TWAI Listen Only: Driver installed', 'TWAI Listen Only: Driver uninstalled')


def dut_thread_callback(**kwargs):
    # Parse keyword arguments
    dut = kwargs['dut']             # Get DUT from kwargs
    expected = kwargs['expected']
    result = kwargs['result']       # Get result[out] from kwargs. MUST be of mutable type e.g. list

    # Must reset again as flashing during start_app will reset multiple times, causing unexpected results
    dut.reset()

    for string in expected:
        dut.expect(string, 20)

    # Mark thread has run to completion without any exceptions
    result[0] = True


@ttfw_idf.idf_example_test(env_tag='Example_TWAI2', ignore=True)
def test_twai_network_example(env, extra_data):

    # Get device under test. "dut1", "dut2", and "dut3" must be properly defined in EnvConfig
    dut_master = env.get_dut('dut1', 'examples/peripherals/twai/twai_network/twai_network_master',
                             dut_class=ttfw_idf.ESP32DUT)
    dut_slave = env.get_dut('dut2', 'examples/peripherals/twai/twai_network/twai_network_slave',
                            dut_class=ttfw_idf.ESP32DUT)
    dut_listen_only = env.get_dut('dut3', 'examples/peripherals/twai/twai_network/twai_network_listen_only',
                                  dut_class=ttfw_idf.ESP32DUT)

    # Flash app onto each DUT, each DUT is reset again at the start of each thread
    dut_master.start_app()
    dut_slave.start_app()
    dut_listen_only.start_app()

    # Create dict of keyword arguments for each dut
    results = [[False], [False], [False]]
    master_kwargs = {'dut': dut_master, 'result': results[0], 'expected': master_expect}
    slave_kwargs = {'dut': dut_slave, 'result': results[1], 'expected': slave_expect}
    listen_only_kwargs = {'dut': dut_listen_only, 'result': results[2], 'expected': listen_only_expect}

    # Create thread for each dut
    dut_master_thread = Thread(target=dut_thread_callback, name='Master Thread', kwargs=master_kwargs)
    dut_slave_thread = Thread(target=dut_thread_callback, name='Slave Thread', kwargs=slave_kwargs)
    dut_listen_only_thread = Thread(target=dut_thread_callback, name='Listen Only Thread', kwargs=listen_only_kwargs)

    # Start each thread
    dut_listen_only_thread.start()
    dut_master_thread.start()
    dut_slave_thread.start()

    # Wait for threads to complete
    dut_listen_only_thread.join()
    dut_master_thread.join()
    dut_slave_thread.join()

    # check each thread ran to completion
    for result in results:
        if result[0] is not True:
            raise Exception('One or more threads did not run successfully')


if __name__ == '__main__':
    test_twai_network_example()
