#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import argparse
import errno
import http.client
import logging


def verbose_print(verbosity, *args):
    if (verbosity):
        logging.info(''.join(str(elems) for elems in args))


def test_val(text, expected, received):
    if expected != received:
        logging.info(' Fail!')
        logging.info('  [reason] {} :'.format(text))
        logging.info('        expected: {}'.format(expected))
        logging.info('        received: {}'.format(received))
        return False
    return True


def test_get_handler(ip, port, verbosity=False):
    verbose_print(verbosity, '========  GET HANDLER TEST =============')
    # Establish HTTP connection
    verbose_print(verbosity, 'Connecting to => ' + ip + ':' + port)
    sess = http.client.HTTPConnection(ip + ':' + port, timeout=15)

    uri = '/hello?query1=value1&query2=value2&query3=value3'
    # GET hello response
    test_headers = {'Test-Header-1':'Test-Value-1', 'Test-Header-2':'Test-Value-2'}
    verbose_print(verbosity, 'Sending GET to URI : ', uri)
    verbose_print(verbosity, 'Sending additional headers : ')
    for k, v in test_headers.items():
        verbose_print(verbosity, '\t', k, ': ', v)
    sess.request('GET', url=uri, headers=test_headers)
    resp = sess.getresponse()
    resp_hdrs = resp.getheaders()
    resp_data = resp.read().decode()
    # Close HTTP connection
    sess.close()

    if not (
        test_val('Status code mismatch', 200, resp.status) and
        test_val('Response mismatch', 'Custom-Value-1', resp.getheader('Custom-Header-1')) and
        test_val('Response mismatch', 'Custom-Value-2', resp.getheader('Custom-Header-2')) and
        test_val('Response mismatch', 'Hello World!', resp_data)
    ):
        return False

    verbose_print(verbosity, 'vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv')
    verbose_print(verbosity, 'Server response to GET /hello')
    verbose_print(verbosity, 'Response Headers : ')
    for k, v in resp_hdrs:
        verbose_print(verbosity, '\t', k, ': ', v)
    verbose_print(verbosity, 'Response Data : ' + resp_data)
    verbose_print(verbosity, '========================================\n')
    return True


def test_post_handler(ip, port, msg, verbosity=False):
    verbose_print(verbosity, '========  POST HANDLER TEST ============')
    # Establish HTTP connection
    verbose_print(verbosity, 'Connecting to => ' + ip + ':' + port)
    sess = http.client.HTTPConnection(ip + ':' + port, timeout=15)

    # POST message to /echo and get back response
    sess.request('POST', url='/echo', body=msg)
    resp = sess.getresponse()
    resp_data = resp.read().decode()
    verbose_print(verbosity, 'Server response to POST /echo (' + msg + ')')
    verbose_print(verbosity, 'vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv')
    verbose_print(verbosity, resp_data)
    verbose_print(verbosity, '========================================\n')

    # Close HTTP connection
    sess.close()
    return test_val('Response mismatch', msg, resp_data)


def test_put_handler(ip, port, verbosity=False):
    verbose_print(verbosity, '========  PUT HANDLER TEST =============')
    # Establish HTTP connection
    verbose_print(verbosity, 'Connecting to => ' + ip + ':' + port)
    sess = http.client.HTTPConnection(ip + ':' + port, timeout=15)

    # PUT message to /ctrl to disable /hello and /echo URI handlers
    # and set 404 error handler to custom http_404_error_handler()
    verbose_print(verbosity, 'Disabling /hello and /echo handlers')
    sess.request('PUT', url='/ctrl', body='0')
    resp = sess.getresponse()
    resp.read()

    try:
        # Send HTTP request to /hello URI
        sess.request('GET', url='/hello')
        resp = sess.getresponse()
        resp_data = resp.read().decode()

        # 404 Error must be returned from server as URI /hello is no longer available.
        # But the custom error handler http_404_error_handler() will not close the
        # session if the requested URI is /hello
        if not test_val('Status code mismatch', 404, resp.status):
            raise AssertionError

        # Compare error response string with expectation
        verbose_print(verbosity, 'Response on GET /hello : ' + resp_data)
        if not test_val('Response mismatch', '/hello URI is not available', resp_data):
            raise AssertionError

        # Using same session for sending an HTTP request to /echo, as it is expected
        # that the custom error handler http_404_error_handler() would not have closed
        # the session
        sess.request('POST', url='/echo', body='Some content')
        resp = sess.getresponse()
        resp_data = resp.read().decode()

        # 404 Error must be returned from server as URI /hello is no longer available.
        # The custom error handler http_404_error_handler() will close the session
        # this time as the requested URI is /echo
        if not test_val('Status code mismatch', 404, resp.status):
            raise AssertionError

        # Compare error response string with expectation
        verbose_print(verbosity, 'Response on POST /echo : ' + resp_data)
        if not test_val('Response mismatch', '/echo URI is not available', resp_data):
            raise AssertionError

        try:
            # Using same session should fail as by now the session would have closed
            sess.request('POST', url='/hello', body='Some content')
            resp = sess.getresponse()
            resp.read().decode()

            # If control reaches this point then the socket was not closed.
            # This is not expected
            verbose_print(verbosity, 'Socket not closed by server')
            raise AssertionError

        except http.client.HTTPException:
            # Catch socket error as we tried to communicate with an already closed socket
            pass
        except IOError as err:
            if err.errno == errno.EPIPE:
                # Sometimes Broken Pipe error is returned
                # when sending data to a closed socket
                pass

    except http.client.HTTPException:
        verbose_print(verbosity, 'Socket closed by server')
        return False

    except AssertionError:
        return False

    finally:
        # Close HTTP connection
        sess.close()

        verbose_print(verbosity, 'Enabling /hello handler')
        # Create new connection
        sess = http.client.HTTPConnection(ip + ':' + port, timeout=15)
        # PUT message to /ctrl to enable /hello URI handler
        # and restore 404 error handler to default
        sess.request('PUT', url='/ctrl', body='1')
        resp = sess.getresponse()
        resp.read()
        # Close HTTP connection
        sess.close()

    # Create new connection
    sess = http.client.HTTPConnection(ip + ':' + port, timeout=15)

    try:
        # Sending HTTP request to /hello should work now
        sess.request('GET', url='/hello')
        resp = sess.getresponse()
        resp_data = resp.read().decode()

        if not test_val('Status code mismatch', 200, resp.status):
            raise AssertionError

        verbose_print(verbosity, 'Response on GET /hello : ' + resp_data)
        if not test_val('Response mismatch', 'Hello World!', resp_data):
            raise AssertionError

        # 404 Error handler should have been restored to default
        sess.request('GET', url='/invalid')
        resp = sess.getresponse()
        resp_data = resp.read().decode()

        if not test_val('Status code mismatch', 404, resp.status):
            raise AssertionError

        verbose_print(verbosity, 'Response on GET /invalid : ' + resp_data)
        if not test_val('Response mismatch', 'Nothing matches the given URI', resp_data):
            raise AssertionError

    except http.client.HTTPException:
        verbose_print(verbosity, 'Socket closed by server')
        return False

    except AssertionError:
        return False

    finally:
        # Close HTTP connection
        sess.close()

    return True


def test_custom_uri_query(ip, port, query, verbosity=False):
    verbose_print(verbosity, '========  GET HANDLER TEST =============')
    # Establish HTTP connection
    verbose_print(verbosity, 'Connecting to => ' + ip + ':' + port)
    sess = http.client.HTTPConnection(ip + ':' + port, timeout=15)

    uri = '/hello?' + query
    # GET hello response
    verbose_print(verbosity, 'Sending GET to URI : ', uri)
    sess.request('GET', url=uri, headers={})
    resp = sess.getresponse()
    resp_data = resp.read().decode()

    verbose_print(verbosity, 'vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv')
    verbose_print(verbosity, 'Server response to GET /hello')
    verbose_print(verbosity, 'Response Data : ' + resp_data)
    verbose_print(verbosity, '========================================\n')

    # Close HTTP connection
    sess.close()
    return 'Hello World!' == resp_data


if __name__ == '__main__':
    # Configure argument parser
    parser = argparse.ArgumentParser(description='Run HTTPd Test')
    parser.add_argument('IP',   metavar='IP',      type=str, help='Server IP')
    parser.add_argument('port', metavar='port',    type=str, help='Server port')
    parser.add_argument('msg',  metavar='message', type=str, help='Message to be sent to server')
    args = vars(parser.parse_args())

    # Get arguments
    ip   = args['IP']
    port = args['port']
    msg  = args['msg']

    if not (
        test_get_handler(ip, port, True) and
        test_put_handler(ip, port, True) and
        test_post_handler(ip, port, msg, True)
    ):
        logging.info('Failed!')
