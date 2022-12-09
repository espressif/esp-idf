# Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http:#www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import yaml

try:
    from yaml import CLoader as Loader
except ImportError:
    from yaml import Loader as Loader


class TestCase(object):
    """
    Test Case Object, mainly used with runner.
    runner can parse all test cases from a given path, set data and config for test case in prepare stage.
    TestCase instance will record these data, provide run method to let runner execute test case.

    :param test_method: test function
    :param extra_data: data passed to test function
    :param overwrite_args: kwargs that overwrite original test case configs
    """
    DEFAULT_CASE_DOC = dict()

    def __init__(self, test_method, extra_data, **overwrite_args):
        self.test_method = test_method
        self.extra_data = extra_data
        self.overwrite_args = overwrite_args

    def run(self):
        """ execute the test case """
        return self.test_method(self.extra_data, **self.overwrite_args)

    def document(self):
        """
        generate test case document.
        parse the case doc with yaml parser and update to original case attributes.

        :return: case document, dict of case attributes and values
        """
        doc_string = self.test_method.__doc__
        try:
            doc = yaml.load(doc_string, Loader=Loader)
        except (AttributeError, OSError, UnicodeDecodeError):
            doc = self.DEFAULT_CASE_DOC
        doc.update(self.test_method.env_args)
        doc.update(self.test_method.accepted_filter)
        return doc
