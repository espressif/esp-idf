#!/usr/bin/env python
# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import copy
import json
import math
import re
import struct
import sys
from typing import Any
from typing import cast
from typing import Dict
from typing import List
from typing import Tuple
from typing import Type
from typing import TypedDict

# Increase this if you change the on-disk binary output format of the BitScrambler so it's
# not compatible with previous versions
BITSCRAMBLER_BINARY_VER = 1
# If we have multiple BitScrambler versions, this'll indicate what hardware we're expecting
# to run on.
BITSCRAMBLER_HW_REV = 0


class Element(TypedDict, total=False):
    more_in_instruction: bool
    is_label: bool
    is_meta: bool
    text: str
    line: int
    column: int


class InputFlags(TypedDict, total=False):
    rel_addr: int
    ctrsel: int
    lutsel: int


class Input(TypedDict, total=False):
    text: str
    ele: Element
    input: int
    muxsel: int
    flags: InputFlags


class Opcode(TypedDict, total=False):
    ele: Element
    op: int
    c: int
    end_val: int
    ctr_val: int
    tgt: int
    h: int
    l: int
    ctr_add: int
    ctl_cond_src: Input


class Inst(TypedDict, total=False):
    op: Opcode
    mux: Dict[int, Input]
    write: int
    read: int


class Chipcfg(TypedDict, total=False):
    chipname: str
    extra_instruction_groups: List[str]
    support_all: bool


# Parser.
# A bsasm file consists of labels, instruction bundles, meta-instructions
# and comments. Comments start at a # and run to a newline and will be
# ignored. Labels are the first word in a line followed by a colon.
# Meta-instructions start with 'cfg' and end at the end of the line;
# they cannot contain commas. Instruction bundles consist of multiple
# sub-instructions separated by a comma. An instruction bundle ends
# when there is a newline and the last non-whitespace character was
# not a comma. An element is defined as either a meta-instruction or a
# sub-instruction.
# This routine uses a state machine to keep track of what it has parsed.
# It handles comments separately as they essentially can be ignored by
# the rest of the state machine; that only needs to see the newline at
# the end of the sentence.
# The output of this routine is an array with element descriptions:
# line -> line the element starts on
# column -> column the element starts on
# text -> text of the element; no whitespace at the start or end and
#         all whitespace between words changed to one single space
# more_in_instruction -> false if this element is a meta-instruction,
#         label or at the end of an instruction bundle.
# is_label -> true if element is a label
# Note that this parser can't see the difference between a
# meta-instruction and an instruction in an instruction bundle: errors
# like a bundle containing a meta-instruction needs to be detected
# elsewhere.

# (Note that this handrolled parser might not be the best option wrt ease
# to understand and maintain, and that has been brought up by Espressif
# colleagues. I've spent some time trying to implement it as a PyParsing
# syntax, but it's very hard to get it to agree with the
# newline-is-sometimes-an-end-of-statement-and-sometimes-not. Antlr might
# be a better choice for that. However, rewriting this likely is more
# work than the ease of fixing bugs in the current parser warrants. If
# that assumption turns out to be very wrong and you're the unlucky
# soul tasked with fixing up this code, feel free to create an issue to
# rewrite this and assign it to me - Jeroen)


def bsasm_parse(src: str) -> List[Element]:
    # Small hack: we trigger processing things on a newline. If a file is read without
    # a newline at the end of the last instruction, we'd erroneously ignore the last element.
    # Easiest way to fix it is to make sure the src always ends in a newline.
    src = src + '\n'
    # Define the various states
    ST_WH_PRE = 0  # Whitespace before an instruction.
    ST_ELEMENT = 1  # Inside a subinstruction or meta-instruction
    ST_WH_IN_EL = 2  # Whitespace, but we're unsure if this is at the end of the element
    ST_AFTER_COMMA = 3  # Encountered a comma, plus possibly whitespace
    state = ST_WH_PRE
    # We keep track of row/col for error reporting
    line = 0
    column = 0
    elements: List[Element] = []
    curr_element: Element = {}
    in_comment = False  # True if we're anywhere between a # and a newline.
    for ch in src:
        # We use these as flags later in the code to start or finish an element.
        start_element = False
        finish_element = False

        if in_comment:
            # If we're in a comment, go back to no-comment mode at the end of the line.
            # We'll need to parse the newline, so this is not part of the big
            # if statement below.
            if ch == '\n':
                in_comment = False

        # Big statemachine handler depending on ch.
        if in_comment:
            # Ignore any character in comment
            pass
        elif ch == '#':
            # Start of a comment.
            in_comment = True
        elif ch in [' ', '\t']:
            # Whitespace. This can be before an element (ignored) or inside an element (might need
            # to insert space if element continues after this)
            if state == ST_ELEMENT:
                state = ST_WH_IN_EL
        elif ch == '\n' or ch == '\r':
            # Newline. If not after a comma, this finishes the element. If after a comma,
            # this can be ignored as whitespace-before-the-next-element.
            if state == ST_ELEMENT or state == ST_WH_IN_EL:
                finish_element = True
                state = ST_WH_PRE
            elif state == ST_AFTER_COMMA:
                state = ST_WH_PRE
        elif ch == ',':
            # A comma. If this is at the end of an element, this finishes the element and
            # prepares for more elements in the instruction.
            if state == ST_ELEMENT or state == ST_WH_IN_EL:
                curr_element['more_in_instruction'] = True
                finish_element = True
                state = ST_AFTER_COMMA
            elif state == ST_AFTER_COMMA:
                raise RuntimeError(
                    f'Line {line} column {column}: Empty subinstruction found'
                )
            elif state == ST_WH_PRE:
                raise RuntimeError(f'Line {line} column {column}: Stray comma found')
        elif ch == ':':
            # This indicates the current element is a label; a colon is not used anywhere else.
            if state == ST_ELEMENT:
                # Check if label is before any instruction
                if len(elements) == 0 or not elements[-1]['more_in_instruction']:
                    # Looks okay.
                    curr_element['is_label'] = True
                    finish_element = True
                    state = ST_WH_PRE
                else:
                    raise RuntimeError(
                        f'Line {line} column {column}: Stray semicolon found'
                    )
            else:
                raise RuntimeError(f'Line {line} column {column}: Stray semicolon found')
        else:
            # Any other characters.
            if state == ST_ELEMENT:
                curr_element['text'] += ch
            elif state == ST_WH_PRE or state == ST_AFTER_COMMA:
                start_element = True
                state = ST_ELEMENT
            elif state == ST_WH_IN_EL:
                curr_element['text'] += ' ' + ch
                state = ST_ELEMENT

        # Handle starting and finishing of elements
        if start_element:
            if 'line' in curr_element:
                raise RuntimeError(
                    f'Line {line} column {column}: Internal error: Element started twice!'
                )
            curr_element['line'] = line
            curr_element['column'] = column
            curr_element['text'] = ch
            curr_element['more_in_instruction'] = False
            curr_element['is_label'] = False
        if finish_element:
            if 'line' not in curr_element:
                raise RuntimeError(
                    f'Line {line} column {column}: Internal error: Element finished while none started'
                )
            elements.append(curr_element)
            curr_element = {}

        # Handle line and column counts
        if ch == '\n':
            line = line + 1
            column = 0
        else:
            column = column + 1
    return elements


# Specific syntax error exception. Reports details about the element[s] to make debugging
# assembly sources easier.

class bsasm_syntax_error(Exception):
    def __new__(cls: Type['bsasm_syntax_error'], *args: str, **kwargs: str) -> 'bsasm_syntax_error':  # noqa: F821
        return cast(bsasm_syntax_error, super().__new__(cls))

    def __init__(self, *args: Any) -> None:  # noqa: F821
        if len(args) == 2:
            ele = args[0]
            message = args[1]
            self.msg = 'Line {} column {}: "{}": {}'.format(ele['line'], ele['column'], ele['text'], message)
        else:
            ele1 = args[0]
            ele2 = args[1]
            message = args[1]
            self.msg = 'Line {} col {}: "{}" and line {} col {}: "{}": {}'.format(ele1['line'],
                                                                                  ele1['column'],
                                                                                  ele1['text'],
                                                                                  ele2['line'],
                                                                                  ele2['column'],
                                                                                  ele2['text'],
                                                                                  message)

    def __str__(self) -> str:  # noqa: F821
        return self.msg


# Definition of possible meta 'cfg' commands
class Meta_inst_def(TypedDict, total=False):
    op: str
    default: int
    enum: Dict[str, int]
    min: int
    max: int


meta_inst_defs: List[Meta_inst_def] = [
    # RX_FETCH_MODE: 0 - on startup fill M0/M1, 1 - don't
    {'op': 'prefetch', 'default': 1, 'enum': {'true': 1, 'false': 0, '1': 1, '0': 0}},
    # Amount of bytes read from input or written to output (depending on eof_on)
    # after EOF on input before we send an EOF to the output.
    {'op': 'trailing_bytes', 'default': 0, 'min': 0, 'max': 8192},
    # Source where 'trailing' counts the bytes after input EOF before generating an output EOF
    {'op': 'eof_on', 'default': 1, 'enum': {'upstream': 1, 'downstream': 0}},
    # Width, in bits, of the LUT memory
    {'op': 'lut_width_bits', 'default': 32, 'enum': {'8': 8, '16': 16, '32': 32}},
]


# Check if element is a meta element
def is_meta(ele: Element) -> bool:
    if ele['text'].lower()[0:4] == 'cfg ':
        return True
    if ele['text'].lower()[0:4] == 'lut ':
        return True
    return False


# Parse a config meta-instruction: check if the values are within range and convert from enums to values
def parse_meta_cfg(ele: Element) -> Tuple[str, int]:
    words = ele['text'].lower().split(' ')
    meta_key = ''
    if len(words) != 3:
        raise bsasm_syntax_error(ele, f'too many arguments to cfg statement')
    for meta_inst_def in meta_inst_defs:
        if meta_inst_def['op'] == words[1]:
            if 'enum' in meta_inst_def:
                if words[2] in meta_inst_def['enum']:
                    meta_key = words[1]
                    meta_value = meta_inst_def['enum'][words[2]]
                else:
                    raise bsasm_syntax_error(
                        ele, f'{words[2]} is not an allowed value for {words[1]}'
                    )
            else:
                v = parse_val(ele, words[2], meta_inst_def['min'], meta_inst_def['max'])
                meta_key = words[1]
                meta_value = v
    if meta_key == '':
        raise bsasm_syntax_error(
            ele, f'{words[1]} is not a recognized meta-instruction'
        )
    return (meta_key, meta_value)


# Check the number of arguments an element has vs what it should have
def check_arg_ct(ele: Element, words: list, ct: int) -> None:
    if len(words) != ct:
        raise bsasm_syntax_error(ele, 'invalid number of arguments')


# Parses a textual range like '1..10' into an actual Python range
def parse_output_range(ele: Element, text: str) -> range:
    b = re.findall(r'^([0-9]+)(?:\.\.([0-9]+))?$', text)
    if not b:
        raise bsasm_syntax_error(ele, f'{text} not a valid integer or range)')
    start = int(b[0][0])
    if b[0][1] != '':
        end = int(b[0][1])
    else:
        end = int(b[0][0])
    if start < 0 or end < 0 or start > 31 or end > 31:
        raise bsasm_syntax_error(ele, f"'{text}' is not a valid integer or range)")
    if start <= end:
        return range(start, end + 1)
    else:
        return range(start, end - 1, -1)


# Resolve an input to a mux selection number and CTL_LUT_SEL/CTL_SRC_SEL/rel_addr
# settings, if those need those to be in a specific state.
def parse_input(ele: Element, text: str, meta: Dict[str, int]) -> Input:
    # Note that strings in the input def arrays need to be lower case.
    inputs = (
        # REG_MEM0
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '10', '11', '12', '13', '14', '15',
        '16', '17', '18', '19', '20', '21', '22', '23', '24', '25', '26', '27', '28', '29', '30', '31',
        # This region is multiplexed using SRC_SEL and LUT_SEL
        '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '',
        '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '',
        # AUX
        'bl<=o0', 'bl>o0', 'bl=o0', 'bl<=o8', 'bl>o8', 'bl=o8', 'bl<=o16', 'bl>o16',
        'bl=o16', 'bl<=o24', 'bl>o24', 'bl=o24', 'bh<=o0', 'bh>o0', 'bh=o0', 'bh<=o8',
        'bh>o8', 'bh=o8', 'bh<=o16', 'bh>o16', 'bh=o16', 'bh<=o24', 'bh>o24', 'bh=o24',
        'b<=o0', 'b>o0', 'b=o0', 'b<=o16', 'b>o16', 'b=o16', 'l', 'h',
        # Reg_last
        'o0', 'o1', 'o2', 'o3', 'o4', 'o5', 'o6', 'o7', 'o8', 'o9', 'o10', 'o11', 'o12', 'o13', 'o14', 'o15',
        'o16', 'o17', 'o18', 'o19', 'o20', 'o21', 'o22', 'o23', 'o24', 'o25', 'o26', 'o27', 'o28', 'o29', 'o30', 'o31',
    )
    inputs_ctrsel_set = (
        # Counter reg
        'a0', 'a1', 'a2', 'a3', 'a4', 'a5', 'a6', 'a7', 'a8', 'a9', 'a10', 'a11', 'a12', 'a13', 'a14', 'a15',
        'b0', 'b1', 'b2', 'b3', 'b4', 'b5', 'b6', 'b7', 'b8', 'b9', 'b10', 'b11', 'b12', 'b13', 'b14', 'b15',
    )
    inputs_ctrsel_clr = (
        # REG_MEM1
        '32', '33', '34', '35', '36', '37', '38', '39', '40', '41', '42', '43', '44', '45', '46', '47',
        '48', '49', '50', '51', '52', '53', '54', '55', '56', '57', '58', '59', '60', '61', '62', '63',
    )
    # Note that the index of this depends on the selected LUT width.
    input_lut_bits = (
        'l0', 'l1', 'l2', 'l3', 'l4', 'l5', 'l6', 'l7', 'l8', 'l9', 'l10', 'l11', 'l12', 'l13', 'l14', 'l15',
        'l16', 'l17', 'l18', 'l19', 'l20', 'l21', 'l22', 'l23', 'l24', 'l25', 'l26', 'l27', 'l28', 'l29', 'l30', 'l31',
    )

    # Note where in the counter reg / mem1 reg region the LUT starts, if enabled
    lut_starts = {8: 24, 16: 16, 32: 0}
    lut_start = lut_starts[meta['lut_width_bits']]

    ret: Input = {'text': text, 'ele': ele, 'flags': {}}
    # Handle relative addressing
    rel_addr = False
    if text[-2:] == '+a':
        rel_addr = True
        text = text[:-2]  # chop off the '+a'

    # Find in what list the input is, and process accordingly.
    if text in inputs:
        # These inputs are always accessible regardless of ctrsel/lutsel
        ret['input'] = inputs.index(text)
    elif text in inputs_ctrsel_set:
        # These inputs need ctrsel to be 1
        i = inputs_ctrsel_set.index(text)
        ret['input'] = i + 32
        ret['flags']['ctrsel'] = 1
        if i >= lut_start:
            ret['flags']['lutsel'] = 0
    elif text in inputs_ctrsel_clr:
        # These inputs need ctrsel to be 0.
        i = inputs_ctrsel_clr.index(text)
        ret['input'] = i + 32
        ret['flags']['ctrsel'] = 0
        if i >= lut_start:
            # These overlap the LUT. lutsel cannot be 1 when these are addressed.
            ret['flags']['lutsel'] = 0
    elif text in input_lut_bits:
        # These inputs need lutsel to be 1.
        i = input_lut_bits.index(text)
        if i > meta['lut_width_bits']:
            raise bsasm_syntax_error(
                ele,
                f"'LUT input {text} referenced, but LUT is configured to be only {meta['lut_width_bits']} bits wide.",
            )
        ret['input'] = i + 32 + lut_start
        ret['flags']['lutsel'] = 1
    else:
        raise bsasm_syntax_error(ele, f"'Input {text} is not valid.")

    if ret['input'] >= 64 and rel_addr:
        raise bsasm_syntax_error(
            ele, f"'LUT input {text} cannot be relatively addressed."
        )

    if ret['input'] < 64:
        ret['flags']['rel_addr'] = rel_addr

    return ret


# Raises an error if two inputs can't be used at the same time
def check_input_compatible(in1: Input, in2: Input) -> None:
    if 'flags' not in in1:
        return
    if 'flags' not in in2:
        return
    in1f = in1['flags']
    in2f = in2['flags']
    err = ''
    if 'rel_addr' in in1f and 'rel_addr' in in2f and in1f['rel_addr'] != in2f['rel_addr']:
        err = 'Cannot have these inputs as both relative and not-relative in the same instruction'
    if 'ctrsel' in in1f and 'ctrsel' in in2f and in1f['ctrsel'] != in2f['ctrsel']:
        err = 'Cannot have both counters/LUT as well as reg_mem1 inputs in the same instruction'
    if 'lutsel' in in1f and 'lutsel' in in2f and in1f['lutsel'] != in2f['lutsel']:
        err = 'With the selected LUT width, the LUT input overlaps the selected counter input'
    if err != '':
        raise bsasm_syntax_error(in1['ele'], in2['ele'], err)


# returns the range as a list of mux selections, plus any ctrsel/lutsel/rel_addr bits needed
# Returns a dictionary with the selected input in 'muxsel' plus a 'flags' dictionary. If a
# rel_addr/lutsel/ctrsel key is in the 'flags' field, that bit must be set or cleared in
# the instruction; if it's not set, the value of that bit doesn't matter for that input.
def parse_input_range(ele: Element, text: str, meta: Dict[str, int]) -> List[Input]:
    # Validate the range and split into start and optionally end fields
    b = re.findall(r'^([a-z0-9><=+]+)(?:\.\.([a-z0-9<>=+]+))?$', text)
    if not b:
        raise bsasm_syntax_error(
            ele, f'{text} not a valid input selection or range of input selections)'
        )
    start = parse_input(ele, b[0][0], meta)
    if b[0][1] != '':
        end = parse_input(ele, b[0][1], meta)
    else:
        end = start

    # Note: this function cannot parse 'weird' ranges, like 31..L1. That's why we limit the ranges
    # to not cross the 32-bit boundary, except when referring to reg_mem0/reg_mem1. This will
    # generally match what the user tries to do.
    if (
        start['input'] < 32
        and 32 <= end['input'] < 64
        and ('flags' not in end or 'ctrsel' not in end['flags'] or end['flags']['ctrsel'] == 0)
        and ('flags' not in end or 'lutsel' not in end['flags'] or end['flags']['lutsel'] == 0)
    ):
        pass
    elif (
        end['input'] < 32
        and start['input'] >= 32
        and start['input'] < 64
        and ('flags' not in start or 'ctrsel' not in start['flags'] or start['flags']['ctrsel'] == 0)
        and ('flags' not in start or 'lutsel' not in start['flags'] or start['flags']['lutsel'] == 0)
    ):
        # in case a 'backwards' range like [35..5] is passed
        pass
    elif math.floor(start['input'] / 32) != math.floor(end['input'] / 32):
        errtxt = f'{text} is not a valid range of input selections. '
        if 'flags' in start and 'lutsel' in start['flags'] and start['flags']['lutsel'] == 1 \
                and ('flags' not in end or 'lutsel' not in end['flags']) and end['input'] < 32:
            errtxt += 'Did you forget an L at the end of the range? (e.g. L0..31 instead of L0..L31)'
        else:
            errtxt += 'Try splitting up the range.'
        raise bsasm_syntax_error(ele, errtxt)

    # The start and end *should* guaranteed to be compatible by now. Check anyway
    # to catch any errors. If this triggers an exception, we have a bug...
    check_input_compatible(start, end)

    flags: InputFlags = {}
    if 'rel_addr' in start['flags']:
        flags['rel_addr'] = start['flags']['rel_addr']
    if 'rel_addr' in end['flags']:
        flags['rel_addr'] = end['flags']['rel_addr']
    if 'ctrsel' in start['flags']:
        flags['ctrsel'] = start['flags']['ctrsel']
    if 'ctrsel' in end['flags']:
        flags['ctrsel'] = end['flags']['ctrsel']
    if 'lutsel' in start['flags']:
        flags['lutsel'] = start['flags']['lutsel']
    if 'lutsel' in end['flags']:
        flags['lutsel'] = end['flags']['lutsel']

    if start['input'] <= end['input']:
        r = range(start['input'], end['input'] + 1)
    else:
        r = range(start['input'], end['input'] - 1, -1)
    ret: List[Input] = []
    for i in r:
        n: Input = {'muxsel': i, 'flags': flags, 'ele': ele}
        ret.append(n)
    return ret


# Parse a numerical field into an int, between a given minimum and maximum.
def parse_val(ele: Element, text: str, minimum: int, maximum: int) -> int:
    try:
        if text[:2] == '0x':
            n = int(text[2:], 16)
        elif text[:2] == '0b':
            n = int(text[2:], 2)
        else:
            n = int(text)
    except ValueError:
        raise bsasm_syntax_error(ele, f"'{text}' is not an integer")
    if n < minimum or n > maximum:
        raise bsasm_syntax_error(
            ele, f"'{text}' is out of range [{minimum}..{maximum}]"
        )
    return n


# Return an IP for a label text
def resolve_label(ele: Element, text: str, labels: Dict[str, int]) -> int:
    if text in labels:
        return labels[text]
    # No match. We could technically also see if the label is a direct IP, but I think
    # that is more likely to be used erroneously than on purpose. If you read this and
    # disagree, feel free to file an issue :)
    raise bsasm_syntax_error(ele, f"'{text}': Label not found.")


# Bitfields defining the instructions
OP_LOOP = 0x2000000
OP_ADD = 0x0000000
OP_IF = 0x0010000
OP_IFN = 0x0020000
OP_LDCTD = 0x0030000
OP_LDCTI = 0x0040000
OP_ADDCTI = 0x0050000


def check_chip_supports_inst(chipcfg: Chipcfg, instgroup: str, ele: Element) -> None:
    if 'support_all' in chipcfg and chipcfg['support_all']:
        return

    if instgroup not in chipcfg['extra_instruction_groups']:
        name = chipcfg['chipname']
        raise bsasm_syntax_error(
            ele, f'Chip {name} does not support this instruction'
        )


def add_op_to_inst(inst: Inst, op: Opcode, ele: Element) -> None:
    if 'op' in inst:
        raise bsasm_syntax_error(
            inst['op']['ele'], ele, f'Cannot have multiple opcodes in one instruction'
        )
    op['ele'] = ele
    inst['op'] = op


# Takes the elements generated by the parse routine and converts it to a
# representation of the bits in the Bitscrambler program.
def bsasm_assemble(elements: List[Element], chipcfg: Chipcfg) -> Tuple[List[Inst], Dict[str, int], List[int]]:
    # This assembler uses two passes: the first finds and resolves global
    # stuff, the second one encodes the actual instructions.

    # Set the meta-instruction values to their defaults
    meta: Dict[str, int] = {}
    for meta_inst_def in meta_inst_defs:
        meta[meta_inst_def['op']] = meta_inst_def['default']

    # Pass 1a: find IPs for labels, mark meta instructions
    # ToDo: also resolve 'def' symbols here once we implement them
    ip = 0
    ip_for_label: Dict[str, int] = {}
    inst_is_meta = False
    inst_start = True
    for ele in elements:
        if inst_start and is_meta(ele):
            # Start of meta-instruction (can only occur at first ele in instruction)
            inst_is_meta = True

        if inst_is_meta:
            ele['is_meta'] = True
        elif ele['is_label']:
            # Label. Record its IP.
            ip_for_label[ele['text']] = ip
            ele['is_meta'] = False
        else:
            ele['is_meta'] = False

        if ele['more_in_instruction']:
            inst_start = False
        else:
            # End of an instruction
            inst_start = True  # mark next element as start of inst
            if (not ele['is_meta']) and (not ele['is_label']):
                ip += 1
            inst_is_meta = False

    # Pass 1B: Collate and parse meta instructions
    inst_start = True
    for ele in elements:
        if inst_start and ele['is_meta']:
            if ele['text'][0:4] == 'cfg ':
                (key, val) = parse_meta_cfg(ele)
                meta[key] = val
                if ele['more_in_instruction']:
                    raise bsasm_syntax_error(
                        ele, 'garbage after cfg statement detected'
                    )
        inst_start = not ele['more_in_instruction']

    # Pass 1C: parse LUT data instructions. We do this after the meta instructions pass
    # as it requires the size of the LUT to figure out min/max boundaries.
    # Note a lut can be written both as 'lut 1 2 3' as well as 'lut 1,2,3' so we need
    # to account for both cases.
    lut_minmax_vals = {
        8:  (-128, 255),
        16: (-32768, 65537),
        32: (-2147483648, 4294967296 - 1),
    }
    minmax = lut_minmax_vals[meta['lut_width_bits']]
    lut = []
    is_lut = False
    for ele in elements:
        if ele['is_meta']:
            if is_lut:
                words = ele['text'].split(' ')
                for w in words:
                    lut.append(parse_val(ele, w, minmax[0], minmax[1]))
            if ele['text'][0:4] == 'lut ':
                is_lut = True
                words = ele['text'].split(' ')
                for w in words[1:]:
                    lut.append(parse_val(ele, w, minmax[0], minmax[1]))
            if not ele['more_in_instruction']:
                is_lut = False

    # Pass 2: Parse any instructions
    valid_read_write = [0, 8, 16, 32]
    insts: List[Inst] = []
    def_inst: Inst = {'mux': {}}
    inst = copy.deepcopy(def_inst)
    op: Opcode
    for ele in elements:
        if not ele['is_meta'] and not ele['is_label']:
            words = ele['text'].lower().split(' ')
            if words[0] == 'set':
                # set (output) (mux input)
                check_arg_ct(ele, words, 3)
                outs = parse_output_range(ele, words[1])
                ins = parse_input_range(ele, words[2], meta)
                if len(ins) != 1 and len(ins) != len(outs):
                    raise bsasm_syntax_error(ele, 'ranges not the same length')
                i = 0
                for out in outs:
                    if out in inst['mux']:
                        raise bsasm_syntax_error(
                            ele, f'output {out} already set earlier in instruction'
                        )
                    if len(ins) == 1:
                        # set range input
                        inst['mux'][out] = ins[0]
                    else:
                        # set range range
                        inst['mux'][out] = ins[i]
                        i = i + 1
            elif words[0] == 'write':
                # Write x bits to output fifo
                check_arg_ct(ele, words, 2)
                no = parse_val(ele, words[1], 0, 32)
                if no not in valid_read_write:
                    raise bsasm_syntax_error(
                        ele, f'{no} is not a valid amount of bits to write'
                    )
                inst['write'] = no
            elif words[0] == 'read':
                # Read x bits from input fifo
                check_arg_ct(ele, words, 2)
                no = parse_val(ele, words[1], 0, 32)
                if no not in valid_read_write:
                    raise bsasm_syntax_error(
                        ele, f'{no} is not a valid amount of bits to write'
                    )
                inst['read'] = no
            elif re.match('loop[ab]', words[0]):
                # LOOPc end_val ctr_add tgt
                check_arg_ct(ele, words, 4)
                op = {'op': OP_LOOP, 'ele': ele}
                op['c'] = 1 if words[0][4] == 'b' else 0
                op['end_val'] = parse_val(ele, words[1], -32768, 65535) & 0xffff
                op['ctr_add'] = parse_val(ele, words[2], -16, 15) & 31
                op['tgt'] = resolve_label(ele, words[3], ip_for_label)
                add_op_to_inst(inst, op, ele)
            elif re.match('add[ab]([hl])?', words[0]):
                # ADDc[h|l] ctr_add
                check_arg_ct(ele, words, 2)
                op = {'op': OP_ADD, 'ele': ele}
                op['c'] = 1 if words[0][3] == 'b' else 0
                if len(words[0]) == 4:
                    op['h'] = 1
                    op['l'] = 1
                else:
                    op['h'] = 1 if words[0][4] == 'h' else 0
                    op['l'] = 1 if words[0][4] == 'l' else 0
                op['ctr_add'] = parse_val(ele, words[1], -32768, 65535) & 0xffff
                add_op_to_inst(inst, op, ele)
            elif re.match('if(n)?', words[0]):
                # IF[N] ctl_cond_src tgt
                check_arg_ct(ele, words, 3)
                op = {'op': OP_IF if len(words[0]) == 2 else OP_IFN, 'ele': ele}
                op['ctl_cond_src'] = parse_input(ele, words[1], meta)
                op['tgt'] = resolve_label(ele, words[2], ip_for_label)
                add_op_to_inst(inst, op, ele)
            elif re.match('ldctd[ab]([hl])?', words[0]):
                # LDCTDc[h|l] ctr_set
                check_arg_ct(ele, words, 2)
                op = {'op': OP_LDCTD}
                op['c'] = 1 if words[0][5] == 'b' else 0
                if len(words[0]) == 6:
                    op['h'] = 1
                    op['l'] = 1
                else:
                    op['h'] = 1 if words[0][6] == 'h' else 0
                    op['l'] = 1 if words[0][6] == 'l' else 0
                op['ctr_add'] = parse_val(ele, words[1], -32768, 65535) & 0xffff
                add_op_to_inst(inst, op, ele)
            elif re.match('ldcti[ab]([hl])?', words[0]):
                # LDCTIc[h|l]
                check_arg_ct(ele, words, 1)
                op = {'op': OP_LDCTI}
                op['c'] = 1 if words[0][5] == 'b' else 0
                if len(words[0]) == 6:
                    op['h'] = 1
                    op['l'] = 1
                else:
                    op['h'] = 1 if words[0][6] == 'h' else 0
                    op['l'] = 1 if words[0][6] == 'l' else 0
                add_op_to_inst(inst, op, ele)
            elif re.match('addcti[ab]([hl])?$', words[0]):
                # ADDCTIc[h|l]
                check_chip_supports_inst(chipcfg, 'addcti', ele)
                check_arg_ct(ele, words, 1)
                op = {'op': OP_ADDCTI}
                op['c'] = 1 if words[0][6] == 'b' else 0
                if len(words[0]) == 7:
                    op['h'] = 1
                    op['l'] = 1
                else:
                    op['h'] = 1 if words[0][7] == 'h' else 0
                    op['l'] = 1 if words[0][7] == 'l' else 0
                add_op_to_inst(inst, op, ele)
            elif re.match('jmp', words[0]):
                # JMP tgt. Pseudo-op, translates to 'IF h tgt'
                check_arg_ct(ele, words, 2)
                op = {'op': OP_IF}
                op['ctl_cond_src'] = parse_input(ele, 'h', meta)
                op['tgt'] = resolve_label(ele, words[1], ip_for_label)
                add_op_to_inst(inst, op, ele)
            elif re.match('nop', words[0]):
                # NOP. Pseudo-op, translates to ADDA 0
                check_arg_ct(ele, words, 1)
                op = {'op': OP_ADD}
                op['h'] = 1
                op['l'] = 1
                op['ctr_add'] = 0
                add_op_to_inst(inst, op, ele)
            else:
                raise bsasm_syntax_error(ele, 'unknown instruction')

            if (
                (not ele['more_in_instruction'])
                and (not ele['is_label'])
                and (not ele['is_meta'])
            ):
                insts.append(inst)
                inst = copy.deepcopy(def_inst)
    return (insts, meta, lut)


# Quick and dirty way to assemble a bytearray from a bunch of bitfields.
# The implementation is not optimal as it handles data bit-by-bit, but it works fine.


class bitstream:
    bitpos: int
    data: list
    curbyte: int

    def __init__(self) -> None:
        self.data = []
        self.curbyte = 0
        self.bitpos = 0

    # Add a field of `bits` bits with the field having the value `val` at the end
    # of the bitstream
    def add_bits(self, val: int, bits: int) -> None:
        v = val
        for i in range(0, bits):
            self.curbyte = self.curbyte >> 1
            if v & 1:
                self.curbyte = self.curbyte | 0x80
            v = v >> 1
            self.bitpos += 1
            if self.bitpos == 8:
                self.bitpos = 0
                self.data.append(self.curbyte)
                self.curbyte = 0

    def to_bytearray(self) -> bytearray:
        return bytearray(self.data)

    def size(self) -> int:
        # Return size in bits
        return len(self.data) * 8 + self.bitpos


# This encodes all the instructions into binary.
def insts_to_binary(insts: List[Inst], meta: Dict[str, int], lut: list) -> bytearray:
    if len(insts) > 8:
        raise RuntimeError('Program has more than eight instructions.')
    ret = bytearray()

    # We need to reformat the LUT into 32-bit values, if not already in that format.
    lut_reformatted = []
    if meta['lut_width_bits'] == 8:
        while (len(lut) % 4) != 0:
            lut.append(0)
        for i in range(0, len(lut), 4):
            v = lut[i] & 255
            v += (lut[i + 1] & 255) << 8
            v += (lut[i + 2] & 255) << 16
            v += (lut[i + 3] & 255) << 24
            lut_reformatted.append(v)
    elif meta['lut_width_bits'] == 16:
        while (len(lut) % 2) != 0:
            lut.append(0)
        for i in range(0, len(lut), 2):
            v = lut[i] & 65535
            v += (lut[i + 1] & 65535) << 16
            lut_reformatted.append(v)
    else:  # 32-bit
        lut_reformatted = lut

    # Format of binary:
    # Header, with self-described length. Any fields that are known to the firmware
    #   past this length will be assumed to be 0.
    # Instructions, padded to 36 bytes per instruction line. Amount of instructions is
    #   defined in header.
    # LUT data, in 32-bit words. Length is defined in header.

    # Header. Note this should always be a multiple of 32 bytes.
    lut_width_vals = {8: 0, 16: 1, 32: 2}
    ret += struct.pack(
        '<BBBBHBBHBB',
        BITSCRAMBLER_BINARY_VER,  # byte
        BITSCRAMBLER_HW_REV,  # byte
        3,  # byte: Length of header in 32-bit words
        len(insts),  # byte: Instruction count
        len(lut_reformatted),  # short: Length of LUT in 32-bit words
        lut_width_vals[meta['lut_width_bits']],  # byte: LUT width setting (0, 1, 2)
        meta['prefetch'],  # byte: prefetch enabled/disabled
        meta['trailing_bytes'] * 8,  # short: number of trailing *bits* after eof
        meta['eof_on'],  # byte
        0,
    )  # byte: unused for now

    for inst in insts:
        bits = bitstream()
        # If the opcode also needs a source, we add it to the mux list as the 32th input
        if 'op' in inst and 'ctl_cond_src' in inst['op']:
            inst['mux'][32] = inst['op']['ctl_cond_src']

        # Check if mux bits are compatible and figure out flags needed
        # Also set unused mux lines to 'l'
        # Finally, insert mux bits into the bitstream.
        flags = {'rel_addr': 0, 'ctrsel': 0, 'lutsel': 0}

        for i in range(0, 33):
            if i in inst['mux']:
                # This could be optimized, but checking each input against each other input
                # allows us to easily tell the user exactly which inputs clash.
                for j in range(i + 1, 33):
                    if j in inst['mux']:
                        check_input_compatible(inst['mux'][i], inst['mux'][j])
                if 'flags' in inst['mux'][i]:
                    if 'rel_addr' in inst['mux'][i]['flags']:
                        flags['rel_addr'] = inst['mux'][i]['flags']['rel_addr']
                    if 'ctrsel' in inst['mux'][i]['flags']:
                        flags['ctrsel'] = inst['mux'][i]['flags']['ctrsel']
                    if 'lutsel' in inst['mux'][i]['flags']:
                        flags['lutsel'] = inst['mux'][i]['flags']['lutsel']
                if i < 32:
                    bits.add_bits(inst['mux'][i]['muxsel'], 7)
            else:
                # Input mux bit is undefined in the program. Set it to a
                # fixed-low input.
                high_input = parse_input({}, 'l', {'lut_width_bits': 8})
                if i < 32:
                    bits.add_bits(high_input['input'], 7)

        # Encode the opcode
        opcode = 0
        if 'op' not in inst:
            # Default to NOP, which is encoded as ADDA 0
            inst['op'] = {'op': OP_ADD, 'h': 1, 'l': 1, 'ctr_add': 0}
        if 'op' in inst['op']:
            opcode = inst['op']['op']
        if 'c' in inst['op']:
            opcode = opcode | ((1 << 24) if inst['op']['c'] else 0)
        if 'h' in inst['op']:
            opcode = opcode | ((1 << 23) if inst['op']['h'] else 0)
        if 'l' in inst['op']:
            opcode = opcode | ((1 << 22) if inst['op']['l'] else 0)
        if 'tgt' in inst['op']:
            opcode = opcode | (inst['op']['tgt'] << 21)
        if 'end_val' in inst['op']:
            opcode = opcode | (inst['op']['end_val'] << 5)
        if 'ctr_add' in inst['op']:  # also aliased to ctr_set
            opcode = opcode | (inst['op']['ctr_add'] << 0)
        if 'ctl_cond_src' in inst['op']:
            opcode = opcode | (inst['op']['ctl_cond_src']['input'] << 0)
        # Add the rest of the fields: read, write, source sel and reladr
        bits.add_bits(opcode, 26)
        val_for_read_write = {0: 0, 8: 1, 16: 2, 32: 3}
        if 'read' not in inst:
            inst['read'] = 0
        bits.add_bits(val_for_read_write[inst['read']], 2)
        if 'write' not in inst:
            inst['write'] = 0
        bits.add_bits(val_for_read_write[inst['write']], 2)
        bits.add_bits(flags['rel_addr'], 1)
        bits.add_bits(flags['ctrsel'], 1)
        bits.add_bits(flags['lutsel'], 1)
        if bits.size() != 257:
            raise RuntimeError(f'Internal error: instruction size is {bits.size()}!')
        # Pad instruction field to 36 bytes = 9 32-bit words
        bits.add_bits(0, 31)
        ret += bits.to_bytearray()

    for i in lut_reformatted:
        ret += struct.pack('<I', i & 0xffffffff)

    return ret


# Return the contents of a file
def read_file(filename: str) -> str:
    try:
        with open(filename, 'r') as f:
            file_content = f.read()
    except OSError:
        print(f'Error opening {filename}: {sys.exc_info()[0]}')
    return file_content


# Write a bytestring to a file
def write_file(filename: str, data: bytearray) -> None:
    with open(filename, 'wb') as f:
        f.write(data)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        prog=sys.argv[0],
        description='BitScrambler program assembler')
    parser.add_argument('infile', help='File name of assembly source to be assembled into a binary')
    parser.add_argument('outfile', help='File name of output binary', nargs='?', default=argparse.SUPPRESS)
    parser.add_argument('-c', help='Set chip capabilities json file; if set, returns an error when \
            an unsupported instruction is assembled',  default=argparse.SUPPRESS)
    args = parser.parse_args()

    chipcfg = Chipcfg()
    if 'c' in args:
        with open(args.c) as chipcfg_json:
            chipcfg = json.load(chipcfg_json)
    else:
        chipcfg = {'chipname': 'chip', 'extra_instruction_groups': [], 'support_all': True}

    if 'outfile' in args:
        outfile = args.outfile
    else:
        outfile = re.sub('.bsasm', '', args.infile) + '.bsbin'
    asm = read_file(args.infile)
    tokens = bsasm_parse(asm)
    insts, meta, lut = bsasm_assemble(tokens, chipcfg)
    out_data = insts_to_binary(insts, meta, lut)
    write_file(outfile, out_data)
    print(f'Written {len(insts)} instructions and {len(lut)} 32-bit words of LUT.')
