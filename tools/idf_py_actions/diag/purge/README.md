# Purge format description for idf.py diag

Once diagnostic information is collected, the purge file is utilized to remove
any sensitive data from the gathered files. By default, the purge file located
at `tools/idf_py_actions/diag/purge/purge.yml` is used unless it is specified
with the --purge argument, in which case the default file is not used.

## Overview of Purge Structure

It is a straightforward YAML file that includes a list of regular expressions
and the corresponding strings that should replace any matches.

    - regex: regular expression to look for
      repl: substitute string for match

The `regex.sub` function from Python is used internally.
