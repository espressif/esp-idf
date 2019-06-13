#!/usr/bin/env python
from __future__ import print_function
import os
import json
import argparse
import tempfile

import pexpect

# Each protocol version to be tested needs a 'testcases_vX.txt' file
PROTOCOL_VERSIONS = [1, 2]


def parse_testcases(version):
    with open("testcases_v%d.txt" % version, "r") as f:
        cases = [l for l in f.readlines() if len(l.strip()) > 0]
    # Each 3 lines in the file should be formatted as:
    # * Description of the test change
    # * JSON "changes" to send to the server
    # * Result JSON to expect back from the server
    if len(cases) % 3 != 0:
        print("Warning: testcases.txt has wrong number of non-empty lines (%d). Should be 3 lines per test case, always." % len(cases))

    for i in range(0, len(cases), 3):
        desc = cases[i]
        send = cases[i + 1]
        expect = cases[i + 2]
        if not desc.startswith("* "):
            raise RuntimeError("Unexpected description at line %d: '%s'" % (i + 1, desc))
        if not send.startswith("> "):
            raise RuntimeError("Unexpected send at line %d: '%s'" % (i + 2, send))
        if not expect.startswith("< "):
            raise RuntimeError("Unexpected expect at line %d: '%s'" % (i + 3, expect))
        desc = desc[2:]
        send = json.loads(send[2:])
        expect = json.loads(expect[2:])
        yield (desc, send, expect)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--logfile', type=argparse.FileType('w'), help='Optional session log of the interactions with confserver.py')
    args = parser.parse_args()

    try:
        # set up temporary file to use as sdkconfig copy
        with tempfile.NamedTemporaryFile(mode="w", delete=False) as temp_sdkconfig:
            temp_sdkconfig_path = os.path.join(tempfile.gettempdir(), temp_sdkconfig.name)
            with open("sdkconfig") as orig:
                temp_sdkconfig.write(orig.read())

        cmdline = "../confserver.py --kconfig Kconfig --config %s" % temp_sdkconfig_path
        print("Running: %s" % cmdline)
        p = pexpect.spawn(cmdline, timeout=30, logfile=args.logfile, echo=False, use_poll=True, maxread=1)

        p.expect("Server running.+\r\n")
        initial = expect_json(p)
        print("Initial: %s" % initial)

        for version in PROTOCOL_VERSIONS:
            test_protocol_version(p, version)

        test_load_save(p, temp_sdkconfig_path)

        test_invalid_json(p)

        print("Done. All passed.")

    finally:
        try:
            os.remove(temp_sdkconfig_path)
        except OSError:
            pass


def expect_json(p):
    # run p.expect() to expect a json object back, and return it as parsed JSON
    p.expect("{.+}\r\n")
    result = p.match.group(0).strip().decode()
    print("Read raw data from server: %s" % result)
    return json.loads(result)


def send_request(p, req):
    req = json.dumps(req)
    print("Sending: %s" % (req))
    p.send("%s\n" % req)
    readback = expect_json(p)
    print("Read back: %s" % (json.dumps(readback)))
    return readback


def test_protocol_version(p, version):
    print("*****")
    print("Testing version %d..." % version)

    # reload the config from the sdkconfig file
    req = {"version": version, "load": None}
    readback = send_request(p, req)
    print("Reset response: %s" % (json.dumps(readback)))

    # run through each test case
    cases = parse_testcases(version)
    for (desc, send, expected) in cases:
        print(desc)
        req = {"version": version, "set": send}
        readback = send_request(p, req)
        if readback.get("version", None) != version:
            raise RuntimeError('Expected {"version" : %d} in response' % version)
        for expect_key in expected.keys():
            read_vals = readback[expect_key]
            exp_vals = expected[expect_key]
            if read_vals != exp_vals:
                expect_diff = dict((k,v) for (k,v) in exp_vals.items() if k not in read_vals or v != read_vals[k])
                raise RuntimeError("Test failed! Was expecting %s: %s" % (expect_key, json.dumps(expect_diff)))
        print("OK")
    print("Version %d OK" % version)


def test_load_save(p, temp_sdkconfig_path):
    print("Testing load/save...")
    before = os.stat(temp_sdkconfig_path).st_mtime
    save_result = send_request(p, {"version": 2, "save": temp_sdkconfig_path})
    print("Save result: %s" % (json.dumps(save_result)))
    assert "error" not in save_result
    assert len(save_result["values"]) == 0  # nothing changes when we save
    assert len(save_result["ranges"]) == 0
    after = os.stat(temp_sdkconfig_path).st_mtime
    assert after > before  # something got written to disk

    # Do a V2 load
    load_result = send_request(p, {"version": 2, "load": temp_sdkconfig_path})
    print("V2 Load result: %s" % (json.dumps(load_result)))
    assert "error" not in load_result
    assert len(load_result["values"]) == 0  # in V2, loading same file should return no config items
    assert len(load_result["ranges"]) == 0

    # Do a V1 load
    load_result = send_request(p, {"version": 1, "load": temp_sdkconfig_path})
    print("V1 Load result: %s" % (json.dumps(load_result)))
    assert "error" not in load_result
    assert len(load_result["values"]) > 0  # in V1, loading same file should return all config items
    assert len(load_result["ranges"]) > 0


def test_invalid_json(p):
    print("Testing invalid JSON formatting...")

    bad_escaping = r'{ "version" : 2, "load" : "c:\some\path\not\escaped\as\json" }'
    p.send("%s\n" % bad_escaping)
    readback = expect_json(p)
    print(readback)
    assert "json" in readback["error"][0].lower()

    not_really_json = 'Hello world!!'
    p.send("%s\n" % not_really_json)
    readback = expect_json(p)
    print(readback)
    assert "json" in readback["error"][0].lower()


if __name__ == "__main__":
    main()
