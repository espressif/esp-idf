#!/usr/bin/env python
import os
import sys
import unittest

import kconfiglib

try:
    import gen_kconfig_doc
except ImportError:
    sys.path.insert(0, os.path.normpath(os.path.join(os.path.dirname(os.path.realpath(__file__)), '../..')))
    import gen_kconfig_doc


class ConfigTargetVisibilityTestCase(unittest.TestCase):
    def setUp(self):
        self.target = os.environ['IDF_TARGET']
        self.config = kconfiglib.Kconfig('Kconfig')
        self.v = gen_kconfig_doc.ConfigTargetVisibility(self.config, self.target)

    def _get_config(self, name):
        sym = self.config.syms.get(name)
        if sym and len(sym.nodes) > 0:
            return sym.nodes[0]
        choice = self.config.named_choices.get(name)
        if choice:
            return choice.nodes[0]
        raise RuntimeError('Unimplemented {}'.format(name))

    def visible(self, config_name):
        self.assertTrue(self.v.visible(self._get_config(config_name)))

    def invisible(self, config_name):
        self.assertFalse(self.v.visible(self._get_config(config_name)))


class ConfigTargetVisibilityChipA(ConfigTargetVisibilityTestCase):
    @classmethod
    def setUpClass(cls):
        os.environ['IDF_TARGET'] = 'chipa'

    def test_config_visibility(self):
        self.invisible('IDF_TARGET')
        self.invisible('IDF_TARGET_CHIPA')
        self.visible('ALWAYS_VISIBLE')
        self.visible('ALWAYS_VISIBLE_CHOICE')
        self.visible('CONFIG_FOR_CHIPA')
        self.invisible('CONFIG_FOR_CHIPB')
        self.visible('CHOICE_FOR_CHIPA')
        self.invisible('CHOICE_FOR_CHIPB')
        self.visible('EXT_CONFIG1_FOR_CHIPA_MENU')
        self.visible('EXT_CONFIG2_FOR_CHIPA_MENU')
        self.visible('EXT_CONFIG3_FOR_CHIPA')
        self.invisible('EXT_CONFIG1_FOR_CHIPB_MENU')
        self.invisible('EXT_CONFIG2_FOR_CHIPB_MENU')
        self.invisible('EXT_CONFIG3_FOR_CHIPB')
        self.visible('EXT_CONFIG4')
        self.visible('DEEP_DEPENDENT_CONFIG')
        self.visible('DEEP_DEPENDENT_CONFIG_INV')
        self.visible('DEEP_DEPENDENT_CHOICE')
        self.invisible('INVISIBLE1')
        self.visible('VISIBLE1')
        self.visible('CONFIG_FOR_CHIPA_DEPENDS_VAR1')
        self.visible('CONFIG_FOR_CHIPA_DEPENDS_VAR2')
        self.visible('CONFIG_FOR_CHIPA_DEPENDS_VAR3')
        self.visible('CONFIG_DEPENDS_ENV_VAR1')
        self.visible('CONFIG_DEPENDS_ENV_VAR2')
        self.visible('CHIPA_VERSION')
        self.invisible('CHIPA_REV_MIN')
        self.visible('CHIPA_FEATURE_FROM_V1')
        self.visible('CHIPA_FEATURE_FROM_V3')


class ConfigTargetVisibilityChipB(ConfigTargetVisibilityTestCase):
    @classmethod
    def setUpClass(cls):
        os.environ['IDF_TARGET'] = 'chipb'

    def test_config_visibility(self):
        self.invisible('IDF_TARGET')
        self.invisible('IDF_TARGET_CHIPA')
        self.visible('ALWAYS_VISIBLE')
        self.visible('ALWAYS_VISIBLE_CHOICE')
        self.invisible('CONFIG_FOR_CHIPA')
        self.visible('CONFIG_FOR_CHIPB')
        self.invisible('CHOICE_FOR_CHIPA')
        self.visible('CHOICE_FOR_CHIPB')
        self.invisible('EXT_CONFIG1_FOR_CHIPA_MENU')
        self.invisible('EXT_CONFIG2_FOR_CHIPA_MENU')
        self.invisible('EXT_CONFIG3_FOR_CHIPA')
        self.visible('EXT_CONFIG1_FOR_CHIPB_MENU')
        self.visible('EXT_CONFIG2_FOR_CHIPB_MENU')
        self.visible('EXT_CONFIG3_FOR_CHIPB')
        self.visible('EXT_CONFIG4')
        self.invisible('DEEP_DEPENDENT_CONFIG')
        self.visible('DEEP_DEPENDENT_CONFIG_INV')
        self.invisible('DEEP_DEPENDENT_CHOICE')
        self.invisible('INVISIBLE1')
        self.visible('VISIBLE1')
        self.invisible('CONFIG_FOR_CHIPA_DEPENDS_VAR1')
        self.invisible('CONFIG_FOR_CHIPA_DEPENDS_VAR2')
        self.invisible('CONFIG_FOR_CHIPA_DEPENDS_VAR3')
        self.visible('CONFIG_DEPENDS_ENV_VAR1')
        self.visible('CONFIG_DEPENDS_ENV_VAR2')
        self.invisible('CHIPA_VERSION')
        self.invisible('CHIPA_REV_MIN')
        self.invisible('CHIPA_FEATURE_FROM_V1')
        self.invisible('CHIPA_FEATURE_FROM_V3')


if __name__ == '__main__':
    unittest.main()
