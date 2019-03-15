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

# Min/Max supported protocol versions
MIN_PROTOCOL_VERSION = 1
MAX_PROTOCOL_VERSION = 2


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

    parser.add_argument('--version', help='Set protocol version to use on initial status',
                        type=int, default=MAX_PROTOCOL_VERSION)

    args = parser.parse_args()

    if args.version < MIN_PROTOCOL_VERSION:
        print("Version %d is older than minimum supported protocol version %d. Client is much older than ESP-IDF version?" %
              (args.version, MIN_PROTOCOL_VERSION))

    if args.version > MAX_PROTOCOL_VERSION:
        print("Version %d is newer than maximum supported protocol version %d. Client is newer than ESP-IDF version?" %
              (args.version, MAX_PROTOCOL_VERSION))

    try:
        args.env = [(name,value) for (name,value) in (e.split("=",1) for e in args.env)]
    except ValueError:
        print("--env arguments must each contain =. To unset an environment variable, use 'ENV='")
        sys.exit(1)

    for name, value in args.env:
        os.environ[name] = value

    run_server(args.kconfig, args.config)


def run_server(kconfig, sdkconfig, default_version=MAX_PROTOCOL_VERSION):
    config = kconfiglib.Kconfig(kconfig)
    config.load_config(sdkconfig)

    print("Server running, waiting for requests on stdin...", file=sys.stderr)

    config_dict = confgen.get_json_values(config)
    ranges_dict = get_ranges(config)
    visible_dict = get_visible(config)

    if default_version == 1:
        # V1: no 'visibility' key, send value None for any invisible item
        values_dict = dict((k, v if visible_dict[k] else False) for (k,v) in config_dict.items())
        json.dump({"version": 1, "values": values_dict, "ranges": ranges_dict}, sys.stdout)
    else:
        # V2 onwards: separate visibility from version
        json.dump({"version": default_version, "values": config_dict, "ranges": ranges_dict, "visible": visible_dict}, sys.stdout)
    print("\n")

    while True:
        line = sys.stdin.readline()
        if not line:
            break
        try:
            req = json.loads(line)
        except ValueError as e:  # json module throws JSONDecodeError (sublcass of ValueError) on Py3 but ValueError on Py2
            response = {"version": default_version, "error": ["JSON formatting error: %s" % e]}
            json.dump(response, sys.stdout)
            print("\n")
            continue
        before = confgen.get_json_values(config)
        before_ranges = get_ranges(config)
        before_visible = get_visible(config)

        if "load" in req:  # load a new sdkconfig

            if req.get("version", default_version) == 1:
                # for V1 protocol, send all items when loading new sdkconfig.
                # (V2+ will only send changes, same as when setting an item)
                before = {}
                before_ranges = {}
                before_visible = {}

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
        after_visible = get_visible(config)

        values_diff = diff(before, after)
        ranges_diff = diff(before_ranges, after_ranges)
        visible_diff = diff(before_visible, after_visible)
        if req["version"] == 1:
            # V1 response, invisible items have value None
            for k in (k for (k,v) in visible_diff.items() if not v):
                values_diff[k] = None
            response = {"version": 1, "values": values_diff, "ranges": ranges_diff}
        else:
            # V2+ response, separate visibility values
            response = {"version": req["version"], "values": values_diff, "ranges": ranges_diff, "visible": visible_diff}
        if error:
            for e in error:
                print("Error: %s" % e, file=sys.stderr)
            response["error"] = error
        json.dump(response, sys.stdout)
        print("\n")


def handle_request(config, req):
    if "version" not in req:
        return ["All requests must have a 'version'"]

    if req["version"] < MIN_PROTOCOL_VERSION or req["version"] > MAX_PROTOCOL_VERSION:
        return ["Unsupported request version %d. Server supports versions %d-%d" % (
            req["version"],
            MIN_PROTOCOL_VERSION,
            MAX_PROTOCOL_VERSION)]

    error = []

    if "load" in req:
        print("Loading config from %s..." % req["load"], file=sys.stderr)
        try:
            config.load_config(req["load"])
        except Exception as e:
            error += ["Failed to load from %s: %s" % (req["load"], e)]

    if "set" in req:
        handle_set(config, error, req["set"])

    if "save" in req:
        try:
            print("Saving config to %s..." % req["save"], file=sys.stderr)
            confgen.write_config(config, req["save"])
        except Exception as e:
            error += ["Failed to save to %s: %s" % (req["save"], e)]

    return error


def handle_set(config, error, to_set):
    missing = [k for k in to_set if k not in config.syms]
    if missing:
        error.append("The following config symbol(s) were not found: %s" % (", ".join(missing)))
    # replace name keys with the full config symbol for each key:
    to_set = dict((config.syms[k],v) for (k,v) in to_set.items() if k not in missing)

    # Work through the list of values to set, noting that
    # some may not be immediately applicable (maybe they depend
    # on another value which is being set). Therefore, defer
    # knowing if any value is unsettable until then end

    while len(to_set):
        set_pass = [(k,v) for (k,v) in to_set.items() if k.visibility]
        if not set_pass:
            break  # no visible keys left
        for (sym,val) in set_pass:
            if sym.type in (kconfiglib.BOOL, kconfiglib.TRISTATE):
                if val is True:
                    sym.set_value(2)
                elif val is False:
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
    Return a dictionary with the difference between 'before' and 'after',
    for items which are present in 'after' dictionary
    """
    diff = dict((k,v) for (k,v) in after.items() if before.get(k, None) != v)
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


def get_visible(config):
    """
    Return a dict mapping node IDs (config names or menu node IDs) to True/False for their visibility
    """
    result = {}
    menus = []

    # when walking the menu the first time, only
    # record whether the config symbols are visible
    # and make a list of menu nodes (that are not symbols)
    def handle_node(node):
        sym = node.item
        try:
            visible = (sym.visibility != 0)
            result[node] = visible
        except AttributeError:
            menus.append(node)
    config.walk_menu(handle_node)

    # now, figure out visibility for each menu. A menu is visible if any of its children are visible
    for m in reversed(menus):  # reverse to start at leaf nodes
        result[m] = any(v for (n,v) in result.items() if n.parent == m)

    # return a dict mapping the node ID to its visibility.
    result = dict((confgen.get_menu_node_id(n),v) for (n,v) in result.items())

    return result


if __name__ == '__main__':
    try:
        main()
    except FatalError as e:
        print("A fatal error occurred: %s" % e, file=sys.stderr)
        sys.exit(2)
