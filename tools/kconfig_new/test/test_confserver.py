#!/usr/bin/env python
import os
import sys
import threading
import time
import json
import argparse
import shutil

import pexpect

sys.path.append("..")
import confserver

def create_server_thread(*args):
    t = threading.Thread()

def parse_testcases():
    with open("testcases.txt", "r") as f:
        cases = [ l for l in f.readlines() if len(l.strip()) > 0 ]
    # Each 3 lines in the file should be formatted as:
    # * Description of the test change
    # * JSON "changes" to send to the server
    # * Result JSON to expect back from the server
    if len(cases) % 3 != 0:
        print("Warning: testcases.txt has wrong number of non-empty lines (%d). Should be 3 lines per test case, always." % len(cases))

    for i in range(0, len(cases), 3):
        desc = cases[i]
        send = cases[i+1]
        expect = cases[i+2]
        if not desc.startswith("* "):
            raise RuntimeError("Unexpected description at line %d: '%s'" % (i+1, desc))
        if not send.startswith("> "):
            raise RuntimeError("Unexpected send at line %d: '%s'" % (i+2, send))
        if not expect.startswith("< "):
            raise RuntimeError("Unexpected expect at line %d: '%s'" % (i+3, expect))
        desc = desc[2:]
        send = json.loads(send[2:])
        expect = json.loads(expect[2:])
        yield (desc, send, expect)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--logfile', type=argparse.FileType('w'), help='Optional session log of the interactions with confserver.py')
    args = parser.parse_args()

    # set up temporary file to use as sdkconfig copy
    temp_sdkconfig_path = os.tmpnam()
    try:
        with open(temp_sdkconfig_path, "w") as temp_sdkconfig:
            with open("sdkconfig") as orig:
                temp_sdkconfig.write(orig.read())

        cmdline = "../confserver.py --kconfig Kconfig --config %s" % temp_sdkconfig_path
        print("Running: %s" % cmdline)
        p = pexpect.spawn(cmdline, timeout=0.5)
        p.logfile = args.logfile
        p.setecho(False)

        def expect_json():
            # run p.expect() to expect a json object back, and return it as parsed JSON
            p.expect("{.+}\r\n")
            return json.loads(p.match.group(0).strip())

        p.expect("Server running.+\r\n")
        initial = expect_json()
        print("Initial: %s" % initial)
        cases = parse_testcases()

        for (desc, send, expected) in cases:
            print(desc)
            req = { "version" : "1", "set" : send }
            req = json.dumps(req)
            print("Sending: %s" % (req))
            p.send("%s\n" % req)
            readback = expect_json()
            print("Read back: %s" % (json.dumps(readback)))
            if readback.get("version", None) != 1:
                raise RuntimeError('Expected {"version" : 1} in response')
            for expect_key in expected.keys():
                read_vals = readback[expect_key]
                exp_vals = expected[expect_key]
                if read_vals != exp_vals:
                    expect_diff = dict((k,v) for (k,v) in exp_vals.items() if not k in read_vals or v != read_vals[k])
                    raise RuntimeError("Test failed! Was expecting %s: %s" % (expect_key, json.dumps(expect_diff)))
            print("OK")

        print("Testing load/save...")
        before = os.stat(temp_sdkconfig_path).st_mtime
        p.send("%s\n" % json.dumps({ "version" : "1", "save" : temp_sdkconfig_path }))
        save_result = expect_json()
        print("Save result: %s" % (json.dumps(save_result)))
        assert len(save_result["values"]) == 0
        assert len(save_result["ranges"]) == 0
        after = os.stat(temp_sdkconfig_path).st_mtime
        assert after > before

        p.send("%s\n" % json.dumps({ "version" : "1", "load" : temp_sdkconfig_path }))
        load_result = expect_json()
        print("Load result: %s" % (json.dumps(load_result)))
        assert len(load_result["values"]) > 0  # loading same file should return all config items
        assert len(load_result["ranges"]) > 0
        print("Done. All passed.")

    finally:
        try:
            os.remove(temp_sdkconfig_path)
        except OSError:
            pass

if __name__ == "__main__":
    main()

