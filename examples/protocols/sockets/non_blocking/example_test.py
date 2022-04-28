# This example code is in the Public Domain (or CC0 licensed, at your option.)

# Unless required by applicable law or agreed to in writing, this
# software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
# CONDITIONS OF ANY KIND, either express or implied.

# -*- coding: utf-8 -*-

from __future__ import print_function, unicode_literals

import re

import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC')
def test_examples_protocol_socket_non_block(env, _):
    dut = env.get_dut('non_blocking_socket', 'examples/protocols/sockets/non_blocking', dut_class=ttfw_idf.ESP32DUT)

    # start the test and expect the client to receive back it's original data
    dut.start_app()
    dut.expect(re.compile(r'nonblocking-socket-client: Received: GET / HTTP/1.1'), timeout=30)


if __name__ == '__main__':
    test_examples_protocol_socket_non_block()
