# This example code is in the Public Domain (or CC0 licensed, at your option.)

# Unless required by applicable law or agreed to in writing, this
# software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
# CONDITIONS OF ANY KIND, either express or implied.

# -*- coding: utf-8 -*-

from __future__ import print_function, unicode_literals

import re

import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC')
def test_examples_asio_chat(env, _):  # type: (ttfw_idf.TinyFW.Env, None) -> None
    msg = 'asio-chat: received hi'
    dut = env.get_dut('asio_chat', 'examples/protocols/asio/asio_chat')
    # start the test and expect the client to receive back it's original data
    dut.start_app()
    dut.expect(re.compile(r'{}'.format('Waiting for input')), timeout=30)
    dut.write(msg)
    dut.write('exit')
    dut.expect(re.compile(r'{}'.format(msg)), timeout=30)


if __name__ == '__main__':
    test_examples_asio_chat()
