def action_extensions(base_actions, project_path=None):
    def echo(name, *args, **kwargs):
        print(name, args, kwargs)

    # Add global options
    extensions = {
        "global_options": [
            {
                "names": ["--test-0"],
                "help": "Non-deprecated option.",
                "deprecated": False
            },
            {
                "names": ["--test-1"],
                "help": "Deprecated option 1.",
                "deprecated": True
            },
            {
                "names": ["--test-2"],
                "help": "Deprecated option 2.",
                "deprecated": "Please update your parameters."
            },
            {
                "names": ["--test-3"],
                "help": "Deprecated option 3.",
                "deprecated": {
                    "custom_message": "Please update your parameters."
                }
            },
            {
                "names": ["--test-4"],
                "help": "Deprecated option 3.",
                "deprecated": {
                    "since": "v4.0",
                    "removed": "v5.0"
                }
            },
        ],
        "actions": {
            "test-0": {
                "callback":
                echo,
                "help":
                "Non-deprecated command 0",
                "options": [
                    {
                        "names": ["--test-sub-0"],
                        "help": "Non-deprecated subcommand option 0",
                        "default": None,
                    },
                    {
                        "names": ["--test-sub-1"],
                        "help": "Deprecated subcommand option 1",
                        "default": None,
                        "deprecated": True
                    },
                ],
                "arguments": [{
                    "names": ["test-arg-0"],
                }],
            },
            "test-1": {
                "callback": echo,
                "help": "Deprecated command 1",
                "deprecated": "Please use alternative command."
            },
        },
    }

    return extensions
