#
# Copyright 2021 Espressif Systems (Shanghai) CO LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

from entity import Entity


class InputSectionDesc():

    def __init__(self, entity, sections, exclusions=None):
        assert(entity.specificity != Entity.Specificity.SYMBOL)

        self.entity = entity
        self.sections = set(sections)

        self.exclusions = set()

        if exclusions:
            assert(not [e for e in exclusions if e.specificity == Entity.Specificity.SYMBOL or
                        e.specificity == Entity.Specificity.NONE])
            self.exclusions = set(exclusions)
        else:
            self.exclusions = set()

    def __str__(self):
        if self.sections:
            exclusion_strings = []

            for exc in sorted(self.exclusions):
                if exc.specificity == Entity.Specificity.ARCHIVE:
                    exc_string = '*%s' % (exc.archive)
                else:
                    exc_string = '*%s:%s.*' % (exc.archive, exc.obj)

                exclusion_strings.append(exc_string)

            section_strings = []

            if exclusion_strings:
                exclusion_string = 'EXCLUDE_FILE(%s)' % ' '.join(exclusion_strings)

                for section in sorted(self.sections):
                    section_strings.append('%s %s' % (exclusion_string, section))
            else:
                for section in sorted(self.sections):
                    section_strings.append(section)

            sections_string = '(%s)' % ' '.join(section_strings)
        else:
            sections_string = '( )'

        command = None

        if self.entity.specificity == Entity.Specificity.NONE:
            command = '*%s' % (sections_string)
        elif self.entity.specificity == Entity.Specificity.ARCHIVE:
            command = '*%s:%s' % (self.entity.archive, sections_string)
        else:
            command = '*%s:%s.*%s' % (self.entity.archive, self.entity.obj, sections_string)

        return command

    def __eq__(self, other):
        return (self.entity == other.entity and
                self.sections == other.sections and
                self.exclusions == other.exclusions)
