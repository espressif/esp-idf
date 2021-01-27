from idf_py_actions.tools import ensure_build_directory, run_target


def action_extensions(base_actions, project_path):
    def uf2_target(target_name, ctx, args):
        ensure_build_directory(args, ctx.info_name)
        run_target(target_name, args)

    uf2_actions = {
        'actions': {
            'uf2': {
                'callback': uf2_target,
                'short_help': 'Generate the UF2 binary with all the binaries included',
                'dependencies': ['all'],
            },
            'uf2-app': {
                'callback': uf2_target,
                'short_help': 'Generate an UF2 binary for the application only',
                'dependencies': ['all'],
            },
        }
    }

    return uf2_actions
