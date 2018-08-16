#!/usr/bin/env python
#
# Copyright 2018 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import httplib
import argparse

def start_session (ip, port):
    return httplib.HTTPConnection(ip, int(port))

def end_session (conn):
    conn.close()

def getreq (conn, path, verbose = False):
    conn.request("GET", path)
    resp = conn.getresponse()
    data = resp.read()
    if verbose:
        print "GET : ", path
        print "Status : ", resp.status
        print "Reason : ", resp.reason
        print "Data length  : ", len(data)
        print "Data content : ", data
    return data

def postreq (conn, path, data, verbose = False):
    conn.request("POST", path, data)
    resp = conn.getresponse()
    data = resp.read()
    if verbose:
        print "POST : ", data
        print "Status : ", resp.status
        print "Reason : ", resp.reason
        print "Data length  : ", len(data)
        print "Data content : ", data
    return data

def putreq (conn, path, body, verbose = False):
    conn.request("PUT", path, body)
    resp = conn.getresponse()
    data = resp.read()
    if verbose:
        print "PUT : ", path, body
        print "Status : ", resp.status
        print "Reason : ", resp.reason
        print "Data length  : ", len(data)
        print "Data content : ", data
    return data

if __name__ == '__main__':
    # Configure argument parser
    parser = argparse.ArgumentParser(description='Run HTTPd Test')
    parser.add_argument('IP'  , metavar='IP'  ,    type=str, help='Server IP')
    parser.add_argument('port', metavar='port',    type=str, help='Server port')
    parser.add_argument('N'   , metavar='integer', type=int, help='Integer to sum upto')
    args = vars(parser.parse_args())

    # Get arguments
    ip   = args['IP']
    port = args['port']
    N    = args['N']

    # Establish HTTP connection
    print "Connecting to => " + ip + ":" + port
    conn = start_session (ip, port)

    # Reset adder context to specified value(0)
    # -- Not needed as new connection will always
    # -- have zero value of the accumulator
    print "Reset the accumulator to 0"
    putreq (conn, "/adder", str(0))

    # Sum numbers from 1 to specified value(N)
    print "Summing numbers from 1 to " + str(N)
    for i in xrange(1, N+1):
        postreq (conn, "/adder", str(i))

    # Fetch the result
    print "Result :", getreq  (conn, "/adder")

    # Close HTTP connection
    end_session (conn)
