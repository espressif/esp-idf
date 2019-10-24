def action_extensions(base_actions, project_path):
    def some_callback(ut_apply_config_name, ctx, args):
        print("!!! From some subcommand")

    def some_global_callback(ctx, global_args, tasks):
        print("!!! From some global callback: %s" % global_args.some_extension_option)

    return {
        "global_options": [{
            "names": ["--some-extension-option"],
            "help": "Help for option --some-extension-option",
            "default": "test",
        }],
        "global_action_callbacks": [some_global_callback],
        "actions": {
            "extra_subcommand": {
                "callback": some_callback,
                "help": "Help for some subcommand.",
            },
        },
    }
