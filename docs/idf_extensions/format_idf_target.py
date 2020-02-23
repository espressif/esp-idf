import re

TARGET_NAMES = {'esp32': 'ESP32', 'esp32s2': 'ESP32-S2'}
TOOLCHAIN_NAMES = {'esp32': 'esp', 'esp32s2': 'esp32s2'}
CONFIG_PREFIX = {'esp32': 'ESP32', 'esp32s2': 'ESP32S2'}

TRM_EN_URL = {'esp32': 'https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf',
              'esp32s2': 'https://www.espressif.com/sites/default/files/documentation/esp32-s2_technical_reference_manual_en.pdf'}

TRM_CN_URL = {'esp32': 'https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_cn.pdf',
              'esp32s2': 'https://www.espressif.com/sites/default/files/documentation/esp32-s2_technical_reference_manual_cn.pdf'}


class StringSubstituter:
    """ Allows for string substitution of target related strings
        before any markup is parsed

        Supports the following replacements (examples shown is for target=esp32s2):
        {IDF_TARGET_NAME}, replaced with the current target name, e.g. ESP32-S2 Beta
        {IDF_TARGET_PATH_NAME}, replaced with the path name, e.g. esp32s2
        {IDF_TARGET_TOOLCHAIN_NAME}, replaced with the toolchain name, e.g. esp32s2
        {IDF_TARGET_CFG_PREFIX}, replaced with the prefix used for config parameters, e.g. ESP32S2
        {IDF_TARGET_TRM_EN_URL}, replaced with the url to the English technical reference manual
        {IDF_TARGET_TRM_CH_URL}, replaced with the url to the Chinese technical reference manual

        Also supports defines of local (single rst file) with the format:
        {IDF_TARGET_TX_PIN:default="IO3",esp32="IO4",esp32s2="IO5"}

        This will define a replacement of the tag {IDF_TARGET_TX_PIN} in the current rst-file, see e.g. uart.rst for example

    """
    RE_PATTERN = re.compile(r'^\s*{IDF_TARGET_(\w+?):(.+?)}', re.MULTILINE)

    def __init__(self):
        self.substitute_strings = {}
        self.local_sub_strings = {}

    def add_pair(self, tag, replace_value):
        self.substitute_strings[tag] = replace_value

    def init_sub_strings(self, app, config):
        self.target_name = config.idf_target

        self.add_pair("{IDF_TARGET_NAME}", TARGET_NAMES[config.idf_target])
        self.add_pair("{IDF_TARGET_PATH_NAME}", config.idf_target)
        self.add_pair("{IDF_TARGET_TOOLCHAIN_NAME}", TOOLCHAIN_NAMES[config.idf_target])
        self.add_pair("{IDF_TARGET_CFG_PREFIX}", CONFIG_PREFIX[config.idf_target])
        self.add_pair("{IDF_TARGET_TRM_EN_URL}", TRM_EN_URL[config.idf_target])
        self.add_pair("{IDF_TARGET_TRM_CN_URL}", TRM_CN_URL[config.idf_target])

    def add_local_subs(self, matches):

        for sub_def in matches:
            if len(sub_def) != 2:
                raise ValueError("IDF_TARGET_X substitution define invalid, val={}".format(sub_def))

            tag = "{" + "IDF_TARGET_{}".format(sub_def[0]) + "}"

            match_default = re.match(r'^\s*default(\s*)=(\s*)\"(.*?)\"', sub_def[1])

            if match_default is None:
                # There should always be a default value
                raise ValueError("No default value in IDF_TARGET_X substitution define, val={}".format(sub_def))

            match_target = re.match(r'^.*{}(\s*)=(\s*)\"(.*?)\"'.format(self.target_name), sub_def[1])

            if match_target is None:
                sub_value = match_default.groups()[2]
            else:
                sub_value = match_target.groups()[2]

            self.local_sub_strings[tag] = sub_value

    def substitute(self, app, docname, source):
        # Add any new local tags that matches the reg.ex.
        sub_defs = re.findall(self.RE_PATTERN, source[0])

        if len(sub_defs) != 0:
            self.add_local_subs(sub_defs)

        # Remove the tag defines
        source[0] = re.sub(self.RE_PATTERN,'', source[0])

        for key in self.local_sub_strings:
            source[0] = source[0].replace(key, self.local_sub_strings[key])

        self.local_sub_strings = {}

        for key in self.substitute_strings:
            source[0] = source[0].replace(key, self.substitute_strings[key])


def setup(app):
    sub = StringSubstituter()

    # Config values not available when setup is called
    app.connect('config-inited', sub.init_sub_strings)
    app.connect('source-read', sub.substitute)

    return {'parallel_read_safe': True, 'parallel_write_safe': True, 'version': '0.1'}
