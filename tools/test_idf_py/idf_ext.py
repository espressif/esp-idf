# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
def action_extensions(base_actions, project_path=None):
    def echo(name, *args, **kwargs):
        print(name, args, kwargs)

    def verbose(name, ctx, args):
        print('Output from test-verbose')
        if args.verbose:
            print('Verbose mode on')

    # Add global options
    extensions = {
        'global_options': [
            {
                'names': ['--test-0'],
                'help': 'Non-deprecated option.',
                'deprecated': False
            },
            {
                'names': ['--test-1'],
                'help': 'Deprecated option 1.',
                'deprecated': True
            },
            {
                'names': ['--test-2'],
                'help': 'Deprecated option 2.',
                'deprecated': 'Please update your parameters.'
            },
            {
                'names': ['--test-3'],
                'help': 'Deprecated option 3.',
                'deprecated': {
                    'custom_message': 'Please update your parameters.'
                }
            },
            {
                'names': ['--test-4'],
                'help': 'Deprecated option 4.',
                'deprecated': {
                    'since': 'v4.0',
                    'removed': 'v5.0'
                }
            },
            {
                'names': ['--test-5'],
                'help': 'Deprecated option 5.',
                'deprecated': {
                    'since': 'v2.0',
                    'removed': 'v3.0',
                    'exit_with_error': True
                }
            },
        ],
        'actions': {
            'test-verbose': {
                'callback': verbose,
                'help': 'Command that have some verbosity',
            },
            'test-0': {
                'callback': echo,
                'help': 'Non-deprecated command 0',
                'options': [
                    {
                        'names': ['--test-sub-0'],
                        'help': 'Non-deprecated subcommand option 0',
                        'default': None,
                    },
                    {
                        'names': ['--test-sub-1'],
                        'help': 'Deprecated subcommand option 1',
                        'default': None,
                        'deprecated': True
                    },
                ],
                'arguments': [{
                    'names': ['test-arg-0'],
                }],
            },
            'test-1': {
                'callback': echo,
                'help': 'Deprecated command 1',
                'deprecated': 'Please use alternative command.'
            },
            'test-2': {
                'callback': echo,
                'help': 'Deprecated command 2',
                'deprecated': {
                    'exit_with_error': True
                }
            },
        },
    }

    return extensions
