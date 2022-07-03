# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import logging
import os
import socket

import netifaces
import yaml

ENV_CONFIG_FILE_SEARCH = [
    os.path.join(os.environ['IDF_PATH'], 'EnvConfig.yml'),
    os.path.join(os.environ['IDF_PATH'], 'ci-test-runner-configs', os.environ.get('CI_RUNNER_DESCRIPTION', ''), 'EnvConfig.yml'),
]


def get_my_ip_by_interface(interface_name: str, ip_type: int = netifaces.AF_INET) -> str:
    for i in netifaces.ifaddresses(interface_name)[ip_type]:
        interface_name = i['addr'].replace('%{}'.format(interface_name), '')
        assert isinstance(interface_name, str)
        return interface_name
    return ''


def get_my_ip4_by_getway(getway: str = '') -> str:
    getways = netifaces.gateways()
    for gw, iface_name, _ in getways[netifaces.AF_INET]:
        if gw and gw == getway:
            interface = iface_name
            break
        else:
            interface = getways['default'][netifaces.AF_INET][1]
    logging.debug('Using interface: {}.'.format(interface))
    address = netifaces.ifaddresses(interface)[netifaces.AF_INET]
    assert isinstance(address[0]['addr'], str)
    return address[0]['addr']


def get_my_ip4_by_dest_ip(dest_ip: str = '') -> str:
    if not dest_ip:
        dest_ip = '8.8.8.8'
    s1 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s1.connect((dest_ip, 80))
    my_ip = s1.getsockname()[0]
    s1.close()
    assert isinstance(my_ip, str)
    return my_ip


def get_my_interface_by_dest_ip(dest_ip: str = '') -> str:
    my_ip = get_my_ip4_by_dest_ip(dest_ip)
    interfaces = netifaces.interfaces()
    for interface in interfaces:
        try:
            addresses = netifaces.ifaddresses(interface)[netifaces.AF_INET]
        except KeyError:
            continue
        if my_ip in [a['addr'] for a in addresses]:
            assert isinstance(interface, str)
            return interface
    logging.error('Can not get interface, dest ip is {}'.format(dest_ip))
    return ''


def get_env_config(env_key: str = '', config_file: str = '') -> dict:
    if not config_file:
        for _file in ENV_CONFIG_FILE_SEARCH:
            if os.path.exists(_file):
                config_file = _file
    if not config_file:
        return dict()

    with open(config_file, 'r') as f:
        config = yaml.load(f.read(), Loader=yaml.SafeLoader)
    assert isinstance(config, dict)
    if not env_key:
        return config
    if env_key in config:
        _config = config[env_key]
        assert isinstance(_config, dict)
        return _config
    logging.warning('Can not get env config, key: {}'.format(env_key))
    return dict()
