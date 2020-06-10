from __future__ import unicode_literals
from tiny_test_fw import Utility
import os
import ttfw_idf


def get_socket_msgs(i):
    msg = 'Socket message S{}'.format(i)
    return ['uart_select_example: {} bytes were written to socket: {}'.format(len(msg), msg),
            'uart_select_example: {} bytes were received through socket: {}'.format(len(msg), msg)]


def get_uart_msgs(i):
    msg = 'UART message U{}'.format(i)
    return ['uart_select_example: {} bytes were sent to UART1: {}'.format(len(msg), msg),
            'uart_select_example: {} bytes were received through UART1: {}'.format(len(msg), msg)]


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC')
def test_examples_select(env, extra_data):

    dut = env.get_dut('select', 'examples/system/select')
    dut.start_app()

    dut.expect('cpu_start: Starting scheduler', timeout=30)

    exp_list = []
    for i in range(1, 10):
        exp_list += get_socket_msgs(i)
        exp_list += get_uart_msgs(i)

    Utility.console_log('Expecting:{}{}'.format(os.linesep, os.linesep.join(exp_list)))
    dut.expect_all(*exp_list, timeout=60)


if __name__ == '__main__':
    test_examples_select()
