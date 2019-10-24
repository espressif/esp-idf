# idf.py extensions
Python modules (subdirectories and files) in this directory named `[your_extension]_ext` will be loaded as idf.py extensions.
If you want to provide extra extensions just provide `;` separated list of directories with extensions in  `IDF_EXTRA_ACTIONS_PATH`. Extensions will be loaded in alphanumeric order.
Command line arguments parsing and extension mechanism is implemented on top of [Click](https://click.palletsprojects.com/en/5.x/) (versions >=5.0 are supported).

They should define a function `action_extensions(base_actions, project_path)` where:

- base_actions - dictionary with actions that are already available for idf.py
- project_path - working dir, may be defaulted to `os.getcwd()`

This function have to return a dict with 3 possible keys:

```python
{
    # Additional options that will be available from id
    "global_options": [{
        "names": ["--option-name"],
        "help": "Help for option --option-name.",
    }],
    # List of functions that will have access to full app context, and can mangle with arguments
    "global_action_callbacks": [global_callback],
    # Additional subcommands for idf.py
    "actions": {
        "subcommand_name": {
            "callback": subcommand_callback,
            "help": "Help for subcommand.",
        },
    },
}
```

Where function `global_callback(ctx, global_args, tasks)` accepts 3 arguments:

- ctx - [Click context](https://click.palletsprojects.com/en/5.x/api/#context)
- global_args - dictionary of all available global arguments
- tasks - list of Task objects

And `subcommand_callback(subcommand_name, ctx, args)` accepts 3 arguments:

- subcommand_name - name of subcommand
- ctx - [Click context](https://click.palletsprojects.com/en/5.x/api/#context)
- args - list of command's arguments
