# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import logging
import os
import socket
from typing import Any, List

import netifaces
import yaml

ENV_CONFIG_FILE_SEARCH = [
    os.path.join(os.environ['IDF_PATH'], 'EnvConfig.yml'),
    os.path.join(os.environ['IDF_PATH'], 'ci-test-runner-configs', os.environ.get('CI_RUNNER_DESCRIPTION', ''), 'EnvConfig.yml'),
]
ENV_CONFIG_TEMPLATE = '''
$IDF_PATH/EnvConfig.yml:
    ```yaml
    <env_name>:
        key: var
        key2: var2
    <another_env>:
        key: var
    ```
'''


def get_host_ip_by_interface(interface_name: str, ip_type: int = netifaces.AF_INET) -> str:
    if ip_type == netifaces.AF_INET:
        for _addr in netifaces.ifaddresses(interface_name)[ip_type]:
            host_ip = _addr['addr'].replace('%{}'.format(interface_name), '')
            assert isinstance(host_ip, str)
            return host_ip
    elif ip_type == netifaces.AF_INET6:
        ip6_addrs: List[str] = []
        for _addr in netifaces.ifaddresses(interface_name)[ip_type]:
            host_ip = _addr['addr'].replace('%{}'.format(interface_name), '')
            assert isinstance(host_ip, str)
            # prefer to use link local address due to example settings
            if host_ip.startswith('FE80::'):
                ip6_addrs.insert(0, host_ip)
            else:
                ip6_addrs.append(host_ip)
        if ip6_addrs:
            return ip6_addrs[0]
    return ''


def get_host_ip4_by_dest_ip(dest_ip: str = '') -> str:
    if not dest_ip:
        dest_ip = '8.8.8.8'
    s1 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s1.connect((dest_ip, 80))
    host_ip = s1.getsockname()[0]
    s1.close()
    assert isinstance(host_ip, str)
    print(f'Using host ip: {host_ip}')
    return host_ip


def get_host_ip6_by_dest_ip(dest_ip: str, interface: str) -> str:
    addr_info = socket.getaddrinfo(f'{dest_ip}%{interface}', 80, socket.AF_INET6, socket.SOCK_DGRAM)
    s1 = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM)
    s1.connect(addr_info[0][-1])
    host_ip = s1.getsockname()[0]
    s1.close()
    assert isinstance(host_ip, str)
    print(f'Using host ip: {host_ip}')
    return host_ip


def get_my_interface_by_dest_ip(dest_ip: str = '') -> str:
    my_ip = get_host_ip4_by_dest_ip(dest_ip)
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


def get_env_config_variable(env_name: str, key: str, default: Any = None) -> Any:
    """
    Get test environment related variable

    config file format: $IDF_PATH/EnvConfig.yml
        ```
        <env_name>:
            key: var
            key2: var2
        <env_name2>:
            key: var
        ```
    """
    config = dict()
    for _file in ENV_CONFIG_FILE_SEARCH:
        try:
            with open(_file, 'r') as f:
                data = yaml.load(f.read(), Loader=yaml.SafeLoader)
            config = data[env_name]
            break
        except (FileNotFoundError, KeyError):
            pass
    else:
        pass

    var = config.get(key, default)
    if var is None:
        logging.warning(f'Failed to get env variable {env_name}/{key}.')
        logging.info(f'Env config file template: {ENV_CONFIG_TEMPLATE}')
        if not os.environ.get('CI_JOB_ID'):
            # Try to get variable from stdin
            var = input(f'You can input the variable now:')
        else:
            raise ValueError(f'Env variable not found: {env_name}/{key}')
    logging.debug(f'Got env variable {env_name}/{key}: {var}')
    return var
