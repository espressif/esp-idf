#
# SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

import kconfiglib
from pyparsing import (Combine, Group, Literal, Optional, Word, alphanums, hexnums, infixNotation, nums, oneOf,
                       opAssoc, printables, quotedString, removeQuotes)


class SDKConfig:
    """
    Evaluates conditional expressions based on the build's sdkconfig and Kconfig files.
    This also defines the grammar of conditional expressions.
    """

    # A configuration entry is in the form CONFIG=VALUE. Definitions of components of that grammar
    IDENTIFIER = Word(alphanums.upper() + '_')

    HEX = Combine('0x' + Word(hexnums)).setParseAction(lambda t: int(t[0], 16))
    DECIMAL = Combine(Optional(Literal('+') | Literal('-')) + Word(nums)).setParseAction(lambda t: int(t[0]))
    LITERAL = Word(printables.replace(':', ''))
    QUOTED_LITERAL = quotedString.setParseAction(removeQuotes)

    VALUE = HEX | DECIMAL | LITERAL | QUOTED_LITERAL

    # Operators supported by the expression evaluation
    OPERATOR = oneOf(['=', '!=', '>', '<', '<=', '>='])

    def __init__(self, kconfig_file, sdkconfig_file):
        self.config = kconfiglib.Kconfig(kconfig_file)
        self.config.load_config(sdkconfig_file)

    def evaluate_expression(self, expression):
        result = self.config.eval_string(expression)

        if result == 0:  # n
            return False
        elif result == 2:  # y
            return True
        else:  # m
            raise Exception('unsupported config expression result')

    @staticmethod
    def get_expression_grammar():
        identifier = SDKConfig.IDENTIFIER.setResultsName('identifier')
        operator = SDKConfig.OPERATOR.setResultsName('operator')
        value = SDKConfig.VALUE.setResultsName('value')

        test_binary = identifier + operator + value
        test_single = identifier

        test = test_binary | test_single

        condition = Group(Optional('(').suppress() + test + Optional(')').suppress())

        grammar = infixNotation(condition, [
            ('!', 1, opAssoc.RIGHT),
            ('&&', 2, opAssoc.LEFT),
            ('||', 2, opAssoc.LEFT)])

        return grammar
