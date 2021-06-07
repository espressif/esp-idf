from idf_py_actions.tools import is_target_supported, ensure_build_directory, run_target
from idf_py_actions.errors import FatalError


def action_extensions(base_actions, project_path):

    SUPPORTED_TARGETS = ['esp32s2']

    def dfu_target(target_name, ctx, args, part_size):
        ensure_build_directory(args, ctx.info_name)
        run_target(target_name, args, {'ESP_DFU_PART_SIZE': part_size} if part_size else {})

    def dfu_flash_target(target_name, ctx, args):
        ensure_build_directory(args, ctx.info_name)

        try:
            run_target(target_name, args)
        except FatalError:
            # Cannot capture the error from dfu-util here so the best advise is:
            print('Please have a look at the "Device Firmware Upgrade through USB" chapter in API Guides of the '
                  'ESP-IDF documentation for solving common dfu-util issues.')
            raise

    dfu_actions = {
        'actions': {
            'dfu': {
                'callback': dfu_target,
                'short_help': 'Build the DFU binary',
                'dependencies': ['all'],
                'options': [
                    {
                        'names': ['--part-size'],
                        'help': 'Large files are split up into smaller partitions in order to avoid timeout during '
                                'erasing flash. This option allows to overwrite the default partition size of '
                                'mkdfu.py.'
                    }
                ],
            },
            'dfu-flash': {
                'callback': dfu_flash_target,
                'short_help': 'Flash the DFU binary',
                'order_dependencies': ['dfu'],
            },
        }
    }

    return dfu_actions if is_target_supported(project_path, SUPPORTED_TARGETS) else {}
