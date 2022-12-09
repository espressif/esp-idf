# Extension to generate esp_err definition as .rst
from .util import copy_if_modified, call_with_python


def setup(app):
    app.connect('idf-info', generate_err_defs)
    return {'parallel_read_safe': True, 'parallel_write_safe': True, 'version': '0.1'}


def generate_err_defs(app, project_description):
    # Generate 'esp_err_defs.inc' file with ESP_ERR_ error code definitions from inc file
    esp_err_inc_path = '{}/inc/esp_err_defs.inc'.format(app.config.build_dir)
    call_with_python('{}/tools/gen_esp_err_to_name.py --rst_output {}.in'.format(app.config.idf_path, esp_err_inc_path))
    copy_if_modified(esp_err_inc_path + '.in', esp_err_inc_path)
