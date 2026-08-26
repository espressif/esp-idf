# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os


def action_extensions(base_actions, project_path=os.getcwd()):
    def test_callback(ut_apply_config_name, ctx, args):
        print('!!! From test_subcommand')

    def test_global_callback(ctx, global_args, tasks):
        print('!!! From test global callback: %s' % global_args.test_extension_option)

    return {
        'global_options': [{
            'names': ['--test-extension-option'],
            'help': 'Help for option --test-extension-option',
            'default': 'test',
        }],
        'global_action_callbacks': [test_global_callback],
        'actions': {
            'test_subcommand': {
                'callback': test_callback,
                'help': 'Help for test subcommand.',
            },
            'hidden_one': {
                'callback': test_callback,
                'hidden': True
            }
        }
    }
