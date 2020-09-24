# kconfig_new

kconfig_new is the kconfig support used by the CMake-based build system.

It depends on the [kconfiglib](https://github.com/ulfalizer/Kconfiglib) package.

## confserver.py

confserver.py is a small Python program intended to support IDEs and other clients who want to allow editing sdkconfig, without needing to reproduce all of the kconfig logic in a particular program.

After launching confserver.py (which can be done via `idf.py confserver` command or `confserver` build target in ninja/make), the confserver communicates via JSON sent/received on stdout. Out-of-band errors are logged via stderr.

### Configuration Structure

During cmake run, the CMake-based build system produces a number of metadata files including `build/config/kconfig_menus.json`, which is a JSON representation of all the menu items in the project configuration and their structure.

This format is currently undocumented, however running CMake with an IDF project will give an indication of the format. The format is expected to be stable.

### Initial Process

After initializing, the server will print "Server running, waiting for requests on stdin..." on stderr.

Then it will print a JSON dictionary on stdout, representing the initial state of sdkconfig:

```
{
  "version": 2,
  "ranges": {
               "TEST_CONDITIONAL_RANGES": [0, 10] },
  "visible": { "TEST_CONDITIONAL_RANGES": true,
                "CHOICE_A": true,
                "test-config-submenu": true },
  "values": { "TEST_CONDITIONAL_RANGES": 1,
              "CHOICE_A": true },
}
```

(Note: actual output is not pretty-printed and will print on a single line. Order of dictionary keys is undefined.)

* "version" key is the protocol version in use.
* "ranges" holds is a dictionary for any config symbol which has a valid integer range. The array value has two values for min/max.
* "visible" holds a dictionary showing initial visibility status of config symbols (identified by the config symbol name) and menus (which don't represent a symbol but are represented as an id 'slug'). Both these names (symbol name and menu slug) correspond to the 'id' key in kconfig_menus.json.
* "values" holds a dictionary showing initial values of all config symbols. Invisible symbols are not included here.

### Interaction

Interaction consists of the client sending JSON dictionary "requests" to the server one at a time. The server will respond to each request with a JSON dictionary response. Interaction is done when the client closes stdout (at this point the server will exit).

Requests look like:

```
{ "version": 2,
  "set": { "TEST_CHILD_STR": "New value",
           "TEST_BOOL": true }
}
```

Note: Requests don't need to be pretty-printed, they just need to be valid JSON.

The `version` key *must* be present in the request and must match a protocol version supported by confserver.

The `set` key is optional. If present, its value must be a dictionary of new values to set on kconfig symbols.

Additional optional keys:

* `load`: If this key is set, sdkconfig file will be reloaded from filesystem before any values are set applied. The value of this key can be a filename, in which case configuration will be loaded from this file. If the value of this key is `null`, configuration will be loaded from the last used file. The response to a "load" command is always the full set of config values and ranges, the same as when the server is initially started.

* `save`: If this key is set, sdkconfig file will be saved after any values are set. Similar to `load`, the value of this key can be a filename to save to a particular file, or `null` to reuse the last used file.

After a request is processed, a response is printed to stdout similar to this:

```
{ "version": 2,
  "ranges": {},
  "visible": { "test-config-submenu": false},
  "values": { "SUBMENU_TRIGGER": false }
}
```

* `version` is the protocol version used by the server.
* `ranges` contains any changed ranges, where the new range of the config symbol has changed (due to some other configuration change or because a new sdkconfig has been loaded).
* `visible` contains any visibility changes, where the visible config symbols have changed.
* `values` contains any value changes, where a config symbol value has changed. This may be due to an explicit change (ie the client `set` this value), or a change caused by some other change in the config system. Note that a change which is set by the client may not be reflected exactly the same in the response, due to restrictions on allowed values which are enforced by the config server. Invalid changes are ignored by the config server.

If setting a value also changes the possible range of values that an item can have, this is also represented with a dictionary `ranges` that contains key/value pairs of config items to their new ranges:

```
{ "version": 2,
  "values": {"OTHER_NAME": true },
  "visible": { },
  "ranges" : { "HAS_RANGE" : [ 3, 4 ] } }
```

Note: The configuration server does not automatically load any changes which are applied externally to the `sdkconfig` file. Send a "load" command or restart the server if the file is externally edited.

Note: The configuration server does not re-run CMake to regenerate other build files or metadata files after `sdkconfig` is updated. This will happen automatically the next time ``CMake`` or ``idf.py`` is run.

### KConfig Item Types

* `string` types are represented as JSON strings.
* `bool` and `tristate` types are represented as JSON Booleans, the third `tristate` state is not supported.
* `int` types are represented as JSON integers
* `hex` types are also represented as JSON integers, clients should read the separate metadata file to know if the UI representation is `int` or `hex`. It is possible to set a `hex` item by sending the server a JSON string of hex digits (no prefix) as the value, but the server always sends `hex` values as JSON integers.

### Error Responses

In some cases, a request may lead to an error message. In this case, the error message is printed to stderr but an array of errors is also returned in the `error` key of the response:

```
{ "version": 777,
  "error": [ "Unsupported request version 777. Server supports versions 1-2" ]
}
```

These error messages are intended to be human readable, not machine parseable.

### Protocol Version Changes

* V2: Added the `visible` key to the response. Invisible items are no longer represented as having value null.
* V2: `load` now sends changes compared to values before the load, not the whole list of config items.
