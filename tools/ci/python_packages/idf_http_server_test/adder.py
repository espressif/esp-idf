#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import argparse
import http.client
import logging


def start_session(ip, port):
    return http.client.HTTPConnection(ip, int(port), timeout=15)


def end_session(conn):
    conn.close()


def getreq(conn, path, verbose=False):
    conn.request('GET', path)
    resp = conn.getresponse()
    data = resp.read()
    if verbose:
        logging.info(f'GET : {path}')
        logging.info(f'Status : {resp.status}')
        logging.info(f'Reason : {resp.reason}')
        logging.info(f'Data length  : {len(data)}')
        logging.info(f'Data content : {data}')
    return data


def postreq(conn, path, data, verbose=False):
    conn.request('POST', path, data)
    resp = conn.getresponse()
    data = resp.read()
    if verbose:
        logging.info(f'POST : {data}')
        logging.info(f'Status : {resp.status}')
        logging.info(f'Reason : {resp.reason}')
        logging.info(f'Data length  : {len(data)}')
        logging.info(f'Data content : {data}')
    return data


def putreq(conn, path, body, verbose=False):
    conn.request('PUT', path, body)
    resp = conn.getresponse()
    data = resp.read()
    if verbose:
        logging.info(f'PUT : {path} {body}')
        logging.info(f'Status : {resp.status}')
        logging.info(f'Reason : {resp.reason}')
        logging.info(f'Data length  : {len(data)}')
        logging.info(f'Data content : {data}')
    return data


if __name__ == '__main__':
    # Configure argument parser
    parser = argparse.ArgumentParser(description='Run HTTPd Test')
    parser.add_argument('IP', metavar='IP', type=str, help='Server IP')
    parser.add_argument('port', metavar='port', type=str, help='Server port')
    parser.add_argument('N', metavar='integer', type=int, help='Integer to sum upto')
    args = vars(parser.parse_args())

    # Get arguments
    ip = args['IP']
    port = args['port']
    N = args['N']

    # Establish HTTP connection
    logging.info('Connecting to => ' + ip + ':' + port)
    conn = start_session(ip, port)

    # Reset adder context to specified value(0)
    # -- Not needed as new connection will always
    # -- have zero value of the accumulator
    logging.info('Reset the accumulator to 0')
    putreq(conn, '/adder', str(0))

    # Sum numbers from 1 to specified value(N)
    logging.info(f'Summing numbers from 1 to {N}')
    for i in range(1, N + 1):
        postreq(conn, '/adder', str(i))

    # Fetch the result
    logging.info('Result :{}'.format(getreq(conn, '/adder')))

    # Close HTTP connection
    end_session(conn)
