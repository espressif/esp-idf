#!/usr/bin/env python
#
# Long-running server process uses stdin & stdout to communicate JSON
# with a caller
#
from __future__ import print_function
import argparse
import json
import kconfiglib
import os
import sys
import confgen
from confgen import FatalError, __version__

def main():
    parser = argparse.ArgumentParser(description='confserver.py v%s - Config Generation Tool' % __version__, prog=os.path.basename(sys.argv[0]))

    parser.add_argument('--config',
                       help='Project configuration settings',
                       required=True)

    parser.add_argument('--kconfig',
                        help='KConfig file with config item definitions',
                        required=True)

    parser.add_argument('--env', action='append', default=[],
                        help='Environment to set when evaluating the config file', metavar='NAME=VAL')

    args = parser.parse_args()

    try:
       args.env = [ (name,value) for (name,value) in ( e.split("=",1) for e in args.env) ]
    except ValueError:
       print("--env arguments must each contain =. To unset an environment variable, use 'ENV='")
       sys.exit(1)

    for name, value in args.env:
        os.environ[name] = value

    print("Server running, waiting for requests on stdin...", file=sys.stderr)
    run_server(args.kconfig, args.config)


def run_server(kconfig, sdkconfig):
    config = kconfiglib.Kconfig(kconfig)
    config.load_config(sdkconfig)

    config_dict = confgen.get_json_values(config)
    ranges_dict = get_ranges(config)
    json.dump({"version": 1, "values" : config_dict, "ranges" : ranges_dict}, sys.stdout)
    print("\n")

    while True:
        line = sys.stdin.readline()
        if not line:
            break
        req = json.loads(line)
        before = confgen.get_json_values(config)
        before_ranges = get_ranges(config)

        if "load" in req:  # if we're loading a different sdkconfig, response should have all items in it
            before = {}
            before_ranges = {}

            # if no new filename is supplied, use existing sdkconfig path, otherwise update the path
            if req["load"] is None:
                req["load"] = sdkconfig
            else:
                sdkconfig = req["load"]

        if "save" in req:
            if req["save"] is None:
                req["save"] = sdkconfig
            else:
                sdkconfig = req["save"]

        error = handle_request(config, req)

        after = confgen.get_json_values(config)
        after_ranges = get_ranges(config)

        values_diff = diff(before, after)
        ranges_diff = diff(before_ranges, after_ranges)
        response = {"version" : 1, "values" : values_diff, "ranges" : ranges_diff}
        if error:
            for e in error:
                print("Error: %s" % e, file=sys.stderr)
            response["error"] = error
        json.dump(response, sys.stdout)
        print("\n")


def handle_request(config, req):
    if not "version" in req:
        return [ "All requests must have a 'version'" ]
    if int(req["version"]) != 1:
        return [ "Only version 1 requests supported" ]

    error = []

    if "load" in req:
        print("Loading config from %s..." % req["load"], file=sys.stderr)
        try:
            config.load_config(req["load"])
        except Exception as e:
            error += [ "Failed to load from %s: %s" % (req["load"], e) ]

    if "set" in req:
        handle_set(config, error, req["set"])

    if "save" in req:
        try:
            print("Saving config to %s..." % req["save"], file=sys.stderr)
            confgen.write_config(config, req["save"])
        except Exception as e:
            error += [ "Failed to save to %s: %s" % (req["save"], e) ]

    return error

def handle_set(config, error, to_set):
    missing = [ k for k in to_set if not k in config.syms ]
    if missing:
        error.append("The following config symbol(s) were not found: %s" % (", ".join(missing)))
    # replace name keys with the full config symbol for each key:
    to_set = dict((config.syms[k],v) for (k,v) in to_set.items() if not k in missing)

    # Work through the list of values to set, noting that
    # some may not be immediately applicable (maybe they depend
    # on another value which is being set). Therefore, defer
    # knowing if any value is unsettable until then end

    while len(to_set):
        set_pass = [ (k,v) for (k,v) in to_set.items() if k.visibility ]
        if not set_pass:
            break  # no visible keys left
        for (sym,val) in set_pass:
            if sym.type in (kconfiglib.BOOL, kconfiglib.TRISTATE):
                if val == True:
                    sym.set_value(2)
                elif val == False:
                    sym.set_value(0)
                else:
                    error.append("Boolean symbol %s only accepts true/false values" % sym.name)
            else:
                sym.set_value(str(val))
            print("Set %s" % sym.name)
            del to_set[sym]

    if len(to_set):
        error.append("The following config symbol(s) were not visible so were not updated: %s" % (", ".join(s.name for s in to_set)))



def diff(before, after):
    """
    Return a dictionary with the difference between 'before' and 'after' (either with the new value if changed,
    or None as the value if a key in 'before' is missing in 'after'
    """
    diff = dict((k,v) for (k,v) in after.items() if before.get(k, None) != v)
    hidden = dict((k,None) for k in before if k not in after)
    diff.update(hidden)
    return diff


def get_ranges(config):
    ranges_dict = {}
    def handle_node(node):
        sym = node.item
        if not isinstance(sym, kconfiglib.Symbol):
            return
        active_range = sym.active_range
        if active_range[0] is not None:
            ranges_dict[sym.name] = active_range

    config.walk_menu(handle_node)
    return ranges_dict


if __name__ == '__main__':
    try:
        main()
    except FatalError as e:
        print("A fatal error occurred: %s" % e, file=sys.stderr)
        sys.exit(2)

