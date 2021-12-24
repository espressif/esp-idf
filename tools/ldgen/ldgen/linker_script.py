#
# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

import collections
import os

from pyparsing import ParseException, Suppress, White

from .fragments import Fragment
from .generation import GenerationException


class LinkerScript:
    """
    Process a linker script template, which contains markers with grammar:

    [<target>]

    The <target> is where output commands (see output_commands.py) are placed.
    """

    Marker = collections.namedtuple('Marker', 'target indent rules')

    def __init__(self, template_file):
        self.members = []
        self.file = os.path.realpath(template_file.name)

        self._generate_members(template_file)

    def _generate_members(self, template_file):
        lines = template_file.readlines()

        target = Fragment.IDENTIFIER
        reference = Suppress('mapping') + Suppress('[') + target + Suppress(']')
        pattern = White(' \t') + reference

        # Find the markers in the template file line by line. If line does not match marker grammar,
        # set it as a literal to be copied as is to the output file.
        for line in lines:
            try:
                parsed = pattern.parse_string(line)
            except ParseException:
                # Does not match marker syntax
                self.members.append(line)
            else:
                indent, target = parsed
                marker = LinkerScript.Marker(target, indent, [])
                self.members.append(marker)

    def fill(self, mapping_rules):
        for member in self.members:
            target = None
            try:
                target = member.target
                rules = member.rules

                rules.extend(mapping_rules[target])
            except KeyError:
                message = GenerationException.UNDEFINED_REFERENCE + " to target '" + target + "'."
                raise GenerationException(message)
            except AttributeError:
                pass

    def write(self, output_file):
        # Add information that this is a generated file.
        output_file.write('/* Automatically generated file; DO NOT EDIT */\n')
        output_file.write('/* Espressif IoT Development Framework Linker Script */\n')
        output_file.write('/* Generated from: %s */\n' % self.file)
        output_file.write('\n')

        # Do the text replacement
        for member in self.members:
            try:
                indent = member.indent
                rules = member.rules

                for rule in rules:
                    generated_line = ''.join([indent, str(rule), '\n'])
                    output_file.write(generated_line)
            except AttributeError:
                output_file.write(member)
