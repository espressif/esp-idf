#!/usr/bin/env python

# NOTE: this file is compatible with upstream version. Modifications are added regarding handling imports which can be
# droped after using kconfiglib from Python package

# Copyright (c) 2018-2019, Nordic Semiconductor ASA and Ulf Magnusson
# SPDX-License-Identifier: ISC

"""
Overview
========

A curses-based Python 2/3 menuconfig implementation. The interface should feel
familiar to people used to mconf ('make menuconfig').

Supports the same keys as mconf, and also supports a set of keybindings
inspired by Vi:

  J/K     : Down/Up
  L       : Enter menu/Toggle item
  H       : Leave menu
  Ctrl-D/U: Page Down/Page Up
  G/End   : Jump to end of list
  g/Home  : Jump to beginning of list

[Space] toggles values if possible, and enters menus otherwise. [Enter] works
the other way around.

The mconf feature where pressing a key jumps to a menu entry with that
character in it in the current menu isn't supported. A jump-to feature for
jumping directly to any symbol (including invisible symbols), choice, menu or
comment (as in a Kconfig 'comment "Foo"') is available instead.

A few different modes are available:

  F: Toggle show-help mode, which shows the help text of the currently selected
  item in the window at the bottom of the menu display. This is handy when
  browsing through options.

  C: Toggle show-name mode, which shows the symbol name before each symbol menu
  entry

  A: Toggle show-all mode, which shows all items, including currently invisible
  items and items that lack a prompt. Invisible items are drawn in a different
  style to make them stand out.


Running
=======

menuconfig.py can be run either as a standalone executable or by calling the
menuconfig() function with an existing Kconfig instance. The second option is a
bit inflexible in that it will still load and save .config, etc.

When run in standalone mode, the top-level Kconfig file to load can be passed
as a command-line argument. With no argument, it defaults to "Kconfig".

The KCONFIG_CONFIG environment variable specifies the .config file to load (if
it exists) and save. If KCONFIG_CONFIG is unset, ".config" is used.

When overwriting a configuration file, the old version is saved to
<filename>.old (e.g. .config.old).

$srctree is supported through Kconfiglib.


Color schemes
=============

It is possible to customize the color scheme by setting the MENUCONFIG_STYLE
environment variable. For example, setting it to 'aquatic' will enable an
alternative, less yellow, more 'make menuconfig'-like color scheme, contributed
by Mitja Horvat (pinkfluid).

This is the current list of built-in styles:
    - default       classic Kconfiglib theme with a yellow accent
    - monochrome    colorless theme (uses only bold and standout) attributes,
                    this style is used if the terminal doesn't support colors
    - aquatic       blue tinted style loosely resembling the lxdialog theme

It is possible to customize the current style by changing colors of UI
elements on the screen. This is the list of elements that can be stylized:

    - path          Top row in the main display, with the menu path
    - separator     Separator lines between windows. Also used for the top line
                    in the symbol information display.
    - list          List of items, e.g. the main display
    - selection     Style for the selected item
    - inv-list      Like list, but for invisible items. Used in show-all mode.
    - inv-selection Like selection, but for invisible items. Used in show-all
                    mode.
    - help          Help text windows at the bottom of various fullscreen
                    dialogs
    - show-help     Window showing the help text in show-help mode
    - frame         Frame around dialog boxes
    - body          Body of dialog boxes
    - edit          Edit box in pop-up dialogs
    - jump-edit     Edit box in jump-to dialog
    - text          Symbol information text

The color definition is a comma separated list of attributes:

    - fg:COLOR      Set the foreground/background colors. COLOR can be one of
      * or *        the basic 16 colors (black, red, green, yellow, blue,
    - bg:COLOR      magenta, cyan, white and brighter versions, for example,
                    brightred). On terminals that support more than 8 colors,
                    you can also directly put in a color number, e.g. fg:123
                    (hexadecimal and octal constants are accepted as well).
                    Colors outside the range -1..curses.COLORS-1 (which is
                    terminal-dependent) are ignored (with a warning). The COLOR
                    can be also specified using a RGB value in the HTML
                    notation, for example #RRGGBB. If the terminal supports
                    color changing, the color is rendered accurately.
                    Otherwise, the visually nearest color is used.

                    If the background or foreground color of an element is not
                    specified, it defaults to -1, representing the default
                    terminal foreground or background color.

                    Note: On some terminals a bright version of the color
                    implies bold.
    - bold          Use bold text
    - underline     Use underline text
    - standout      Standout text attribute (reverse color)

More often than not, some UI elements share the same color definition. In such
cases the right value may specify an UI element from which the color definition
will be copied. For example, "separator=help" will apply the current color
definition for "help" to "separator".

A keyword without the '=' is assumed to be a style template. The template name
is looked up in the built-in styles list and the style definition is expanded
in-place. With this, built-in styles can be used as basis for new styles.

For example, take the aquatic theme and give it a red selection bar:

MENUCONFIG_STYLE="aquatic selection=fg:white,bg:red"

If there's an error in the style definition or if a missing style is assigned
to, the assignment will be ignored, along with a warning being printed on
stderr.

The 'default' theme is always implicitly parsed first (or the 'monochrome'
theme if the terminal lacks colors), so the following two settings have the
same effect:

    MENUCONFIG_STYLE="selection=fg:white,bg:red"
    MENUCONFIG_STYLE="default selection=fg:white,bg:red"


Other features
==============

  - Seamless terminal resizing

  - No dependencies on *nix, as the 'curses' module is in the Python standard
    library

  - Unicode text entry

  - Improved information screen compared to mconf:

      * Expressions are split up by their top-level &&/|| operands to improve
        readability

      * Undefined symbols in expressions are pointed out

      * Menus and comments have information displays

      * Kconfig definitions are printed

      * The include path is shown, listing the locations of the 'source'
        statements that included the Kconfig file of the symbol (or other
        item)


Limitations
===========

Doesn't work out of the box on Windows, but can be made to work with 'pip
install windows-curses'. See the
https://github.com/zephyrproject-rtos/windows-curses repository.

'pip install kconfiglib' on Windows automatically installs windows-curses
to make the menuconfig usable.
"""
from __future__ import print_function

import sys
try:
    import curses
except ImportError:
    print('"windows-curses" package is required in Windows command line. Please install it by running '
          '"{} -m pip install --user windows-curses" (or without the "--user" option)'
          ''.format(sys.executable))
    exit(1)

import errno
import locale
import os
import re
import textwrap

try:
    from . import kconfiglib
except Exception:
    sys.path.insert(0, os.path.dirname(os.path.realpath(__file__)))
    # fix that kconfig gets imported from the current directory. This change can be dropped after using menuconfig.py
    # and kconfiglib.py from global package
from kconfiglib import Symbol, Choice, MENU, COMMENT, MenuNode, \
                       BOOL, TRISTATE, STRING, INT, HEX, \
                       AND, OR, \
                       expr_str, expr_value, split_expr, \
                       standard_sc_expr_str, \
                       TRI_TO_STR, TYPE_TO_STR, \
                       standard_kconfig, standard_config_filename


#
# Configuration variables
#

# If True, try to change LC_CTYPE to a UTF-8 locale if it is set to the C
# locale (which implies ASCII). This fixes curses Unicode I/O issues on systems
# with bad defaults. ncurses configures itself from the locale settings.
#
# Related PEP: https://www.python.org/dev/peps/pep-0538/
_CHANGE_C_LC_CTYPE_TO_UTF8 = True

# How many steps an implicit submenu will be indented. Implicit submenus are
# created when an item depends on the symbol before it. Note that symbols
# defined with 'menuconfig' create a separate menu instead of indenting.
_SUBMENU_INDENT = 4

# Number of steps for Page Up/Down to jump
_PG_JUMP = 6

# Height of the help window in show-help mode
_SHOW_HELP_HEIGHT = 8

# How far the cursor needs to be from the edge of the window before it starts
# to scroll. Used for the main menu display, the information display, the
# search display, and for text boxes.
_SCROLL_OFFSET = 5

# Minimum width of dialogs that ask for text input
_INPUT_DIALOG_MIN_WIDTH = 30

# Number of arrows pointing up/down to draw when a window is scrolled
_N_SCROLL_ARROWS = 14

# Lines of help text shown at the bottom of the "main" display
_MAIN_HELP_LINES = """
[Space/Enter] Toggle/enter  [ESC] Leave menu           [S] Save
[O] Load                    [?] Symbol info            [/] Jump to symbol
[F] Toggle show-help mode   [C] Toggle show-name mode  [A] Toggle show-all mode
[Q] Quit (prompts for save) [D] Save minimal config (advanced)
"""[1:-1].split("\n")

# Lines of help text shown at the bottom of the information dialog
_INFO_HELP_LINES = """
[ESC/q] Return to menu      [/] Jump to symbol
"""[1:-1].split("\n")

# Lines of help text shown at the bottom of the search dialog
_JUMP_TO_HELP_LINES = """
Type text to narrow the search. Regexes are supported (via Python's 're'
module). The up/down cursor keys step in the list. [Enter] jumps to the
selected symbol. [ESC] aborts the search. Type multiple space-separated
strings/regexes to find entries that match all of them. Type Ctrl-F to
view the help of the selected item without leaving the dialog.
"""[1:-1].split("\n")

#
# Styling
#

_STYLES = {
    "default": """
    path=fg:black,bg:white,bold
    separator=fg:black,bg:yellow,bold
    list=fg:black,bg:white
    selection=fg:white,bg:blue,bold
    inv-list=fg:red,bg:white
    inv-selection=fg:red,bg:blue
    help=path
    show-help=list
    frame=fg:black,bg:yellow,bold
    body=fg:white,bg:black
    edit=fg:white,bg:blue
    jump-edit=edit
    text=list
    """,

    # This style is forced on terminals that do no support colors
    "monochrome": """
    path=bold
    separator=bold,standout
    list=
    selection=bold,standout
    inv-list=bold
    inv-selection=bold,standout
    help=bold
    show-help=
    frame=bold,standout
    body=
    edit=standout
    jump-edit=
    text=
    """,

    # Blue tinted style loosely resembling lxdialog
    "aquatic": """
    path=fg:cyan,bg:blue,bold
    separator=fg:white,bg:cyan,bold
    help=path
    frame=fg:white,bg:cyan,bold
    body=fg:brightwhite,bg:blue
    edit=fg:black,bg:white
    """
}

# Standard colors definition
_STYLE_STD_COLORS = {
    # Basic colors
    "black":         curses.COLOR_BLACK,
    "red":           curses.COLOR_RED,
    "green":         curses.COLOR_GREEN,
    "yellow":        curses.COLOR_YELLOW,
    "blue":          curses.COLOR_BLUE,
    "magenta":       curses.COLOR_MAGENTA,
    "cyan":          curses.COLOR_CYAN,
    "white":         curses.COLOR_WHITE,

    # Bright versions
    "brightblack":   curses.COLOR_BLACK + 8,
    "brightred":     curses.COLOR_RED + 8,
    "brightgreen":   curses.COLOR_GREEN + 8,
    "brightyellow":  curses.COLOR_YELLOW + 8,
    "brightblue":    curses.COLOR_BLUE + 8,
    "brightmagenta": curses.COLOR_MAGENTA + 8,
    "brightcyan":    curses.COLOR_CYAN + 8,
    "brightwhite":   curses.COLOR_WHITE + 8,

    # Aliases
    "purple":        curses.COLOR_MAGENTA,
    "brightpurple":  curses.COLOR_MAGENTA + 8,
}


def _rgb_to_6cube(rgb):
    # Converts an 888 RGB color to a 3-tuple (nice in that it's hashable)
    # representing the closest xterm 256-color 6x6x6 color cube color.
    #
    # The xterm 256-color extension uses a RGB color palette with components in
    # the range 0-5 (a 6x6x6 cube). The catch is that the mapping is nonlinear.
    # Index 0 in the 6x6x6 cube is mapped to 0, index 1 to 95, then 135, 175,
    # etc., in increments of 40. See the links below:
    #
    #   https://commons.wikimedia.org/wiki/File:Xterm_256color_chart.svg
    #   https://github.com/tmux/tmux/blob/master/colour.c

    # 48 is the middle ground between 0 and 95.
    return tuple(0 if x < 48 else int(round(max(1, (x - 55)/40))) for x in rgb)


def _6cube_to_rgb(r6g6b6):
    # Returns the 888 RGB color for a 666 xterm color cube index

    return tuple(0 if x == 0 else 40*x + 55 for x in r6g6b6)


def _rgb_to_gray(rgb):
    # Converts an 888 RGB color to the index of an xterm 256-color grayscale
    # color with approx. the same perceived brightness

    # Calculate the luminance (gray intensity) of the color. See
    #   https://stackoverflow.com/questions/596216/formula-to-determine-brightness-of-rgb-color
    # and
    #   https://www.w3.org/TR/AERT/#color-contrast
    luma = 0.299*rgb[0] + 0.587*rgb[1] + 0.114*rgb[2]

    # Closest index in the grayscale palette, which starts at RGB 0x080808,
    # with stepping 0x0A0A0A
    index = int(round((luma - 8)/10))

    # Clamp the index to 0-23, corresponding to 232-255
    return max(0, min(index, 23))


def _gray_to_rgb(index):
    # Convert a grayscale index to its closet single RGB component

    return 3*(10*index + 8,)  # Returns a 3-tuple


# Obscure Python: We never pass a value for rgb2index, and it keeps pointing to
# the same dict. This avoids a global.
def _alloc_rgb(rgb, rgb2index={}):
    # Initialize a new entry in the xterm palette to the given RGB color,
    # returning its index. If the color has already been initialized, the index
    # of the existing entry is returned.
    #
    # ncurses is palette-based, so we need to overwrite palette entries to make
    # new colors.
    #
    # The colors from 0 to 15 are user-defined, and there's no way to query
    # their RGB values, so we better leave them untouched. Also leave any
    # hypothetical colors above 255 untouched (though we're unlikely to
    # allocate that many colors anyway).

    if rgb in rgb2index:
        return rgb2index[rgb]

    # Many terminals allow the user to customize the first 16 colors. Avoid
    # changing their values.
    color_index = 16 + len(rgb2index)
    if color_index >= 256:
        _warn("Unable to allocate new RGB color ", rgb, ". Too many colors "
              "allocated.")
        return 0

    # Map each RGB component from the range 0-255 to the range 0-1000, which is
    # what curses uses
    curses.init_color(color_index, *(int(round(1000*x/255)) for x in rgb))
    rgb2index[rgb] = color_index

    return color_index


def _color_from_num(num):
    # Returns the index of a color that looks like color 'num' in the xterm
    # 256-color palette (but that might not be 'num', if we're redefining
    # colors)

    # - _alloc_rgb() won't touch the first 16 colors or any (hypothetical)
    #   colors above 255, so we can always return them as-is
    #
    # - If the terminal doesn't support changing color definitions, or if
    #   curses.COLORS < 256, _alloc_rgb() won't touch any color, and all colors
    #   can be returned as-is
    if num < 16 or num > 255 or not curses.can_change_color() or \
       curses.COLORS < 256:
        return num

    # _alloc_rgb() might redefine colors, so emulate the xterm 256-color
    # palette by allocating new colors instead of returning color numbers
    # directly

    if num < 232:
        num -= 16
        return _alloc_rgb(_6cube_to_rgb(((num//36)%6, (num//6)%6, num%6)))

    return _alloc_rgb(_gray_to_rgb(num - 232))


def _color_from_rgb(rgb):
    # Returns the index of a color matching the 888 RGB color 'rgb'. The
    # returned color might be an ~exact match or an approximation, depending on
    # terminal capabilities.

    # Calculates the Euclidean distance between two RGB colors
    def dist(r1, r2): return sum((x - y)**2 for x, y in zip(r1, r2))

    if curses.COLORS >= 256:
        # Assume we're dealing with xterm's 256-color extension

        if curses.can_change_color():
            # Best case -- the terminal supports changing palette entries via
            # curses.init_color(). Initialize an unused palette entry and
            # return it.
            return _alloc_rgb(rgb)

        # Second best case -- pick between the xterm 256-color extension colors

        # Closest 6-cube "color" color
        c6 = _rgb_to_6cube(rgb)
        # Closest gray color
        gray = _rgb_to_gray(rgb)

        if dist(rgb, _6cube_to_rgb(c6)) < dist(rgb, _gray_to_rgb(gray)):
            # Use the "color" color from the 6x6x6 color palette. Calculate the
            # color number from the 6-cube index triplet.
            return 16 + 36*c6[0] + 6*c6[1] + c6[2]

        # Use the color from the gray palette
        return 232 + gray

    # Terminal not in xterm 256-color mode. This is probably the best we can
    # do, or is it? Submit patches. :)
    min_dist = float('inf')
    best = -1
    for color in range(curses.COLORS):
        # ncurses uses the range 0..1000. Scale that down to 0..255.
        d = dist(rgb, tuple(int(round(255*c/1000))
                            for c in curses.color_content(color)))
        if d < min_dist:
            min_dist = d
            best = color

    return best


def _parse_style(style_str, parsing_default):
    # Parses a string with '<element>=<style>' assignments. Anything not
    # containing '=' is assumed to be a reference to a built-in style, which is
    # treated as if all the assignments from the style were inserted at that
    # point in the string.
    #
    # The parsing_default flag is set to True when we're implicitly parsing the
    # 'default'/'monochrome' style, to prevent warnings.

    for sline in style_str.split():
        # Words without a "=" character represents a style template
        if "=" in sline:
            key, data = sline.split("=", 1)

            # The 'default' style template is assumed to define all keys. We
            # run _style_to_curses() for non-existing keys as well, so that we
            # print warnings for errors to the right of '=' for those too.
            if key not in _style and not parsing_default:
                _warn("Ignoring non-existent style", key)

            # If data is a reference to another key, copy its style
            if data in _style:
                _style[key] = _style[data]
            else:
                _style[key] = _style_to_curses(data)

        elif sline in _STYLES:
            # Recursively parse style template. Ignore styles that don't exist,
            # for backwards/forwards compatibility.
            _parse_style(_STYLES[sline], parsing_default)

        else:
            _warn("Ignoring non-existent style template", sline)

# Dictionary mapping element types to the curses attributes used to display
# them
_style = {}


def _style_to_curses(style_def):
    # Parses a style definition string (<element>=<style>), returning
    # a (fg_color, bg_color, attributes) tuple.

    def parse_color(color_def):
        color_def = color_def.split(":", 1)[1]

        if color_def in _STYLE_STD_COLORS:
            return _color_from_num(_STYLE_STD_COLORS[color_def])

        # HTML format, #RRGGBB
        if re.match("#[A-Fa-f0-9]{6}", color_def):
            return _color_from_rgb((
                int(color_def[1:3], 16),
                int(color_def[3:5], 16),
                int(color_def[5:7], 16)))

        try:
            color_num = _color_from_num(int(color_def, 0))
        except ValueError:
            _warn("Ignoring color ", color_def, "that's neither predefined "
                  "nor a number")

            return -1

        if not -1 <= color_num < curses.COLORS:
            _warn("Ignoring color {}, which is outside the range "
                  "-1..curses.COLORS-1 (-1..{})"
                  .format(color_def, curses.COLORS - 1))

            return -1

        return color_num

    fg_color = -1
    bg_color = -1
    attrs = 0

    if style_def:
        for field in style_def.split(","):
            if field.startswith("fg:"):
                fg_color = parse_color(field)
            elif field.startswith("bg:"):
                bg_color = parse_color(field)
            elif field == "bold":
                # A_BOLD tends to produce faint and hard-to-read text on the
                # Windows console, especially with the old color scheme, before
                # the introduction of
                # https://blogs.msdn.microsoft.com/commandline/2017/08/02/updating-the-windows-console-colors/
                attrs |= curses.A_NORMAL if _IS_WINDOWS else curses.A_BOLD
            elif field == "standout":
                attrs |= curses.A_STANDOUT
            elif field == "underline":
                attrs |= curses.A_UNDERLINE
            else:
                _warn("Ignoring unknown style attribute", field)

    return _style_attr(fg_color, bg_color, attrs)


def _init_styles():
    if curses.has_colors():
        curses.use_default_colors()

    # Use the 'monochrome' style template as the base on terminals without
    # color
    _parse_style("default" if curses.has_colors() else "monochrome", True)

    # Add any user-defined style from the environment
    if "MENUCONFIG_STYLE" in os.environ:
        _parse_style(os.environ["MENUCONFIG_STYLE"], False)


# color_attribs holds the color pairs we've already created, indexed by a
# (<foreground color>, <background color>) tuple.
#
# Obscure Python: We never pass a value for color_attribs, and it keeps
# pointing to the same dict. This avoids a global.
def _style_attr(fg_color, bg_color, attribs, color_attribs={}):
    # Returns an attribute with the specified foreground and background color
    # and the attributes in 'attribs'. Reuses color pairs already created if
    # possible, and creates a new color pair otherwise.
    #
    # Returns 'attribs' if colors aren't supported.

    if not curses.has_colors():
        return attribs

    if (fg_color, bg_color) not in color_attribs:
        # Create new color pair. Color pair number 0 is hardcoded and cannot be
        # changed, hence the +1s.
        curses.init_pair(len(color_attribs) + 1, fg_color, bg_color)
        color_attribs[(fg_color, bg_color)] = \
            curses.color_pair(len(color_attribs) + 1)

    return color_attribs[(fg_color, bg_color)] | attribs


#
# Main application
#


def _main():
    menuconfig(standard_kconfig())


def menuconfig(kconf):
    """
    Launches the configuration interface, returning after the user exits.

    kconf:
      Kconfig instance to be configured
    """
    global _kconf
    global _conf_filename
    global _conf_changed
    global _minconf_filename
    global _show_all

    _kconf = kconf

    # Filename to save configuration to
    _conf_filename = standard_config_filename()

    # Load existing configuration and set _conf_changed True if it is outdated
    _conf_changed = _load_config()

    # Filename to save minimal configuration to
    _minconf_filename = "defconfig"

    # Any visible items in the top menu?
    _show_all = False
    if not _shown_nodes(kconf.top_node):
        # Nothing visible. Start in show-all mode and try again.
        _show_all = True
        if not _shown_nodes(kconf.top_node):
            # Give up. The implementation relies on always having a selected
            # node.
            print("Empty configuration -- nothing to configure.\n"
                  "Check that environment variables are set properly.")
            return

    # Disable warnings. They get mangled in curses mode, and we deal with
    # errors ourselves.
    kconf.warn = False

    # Make curses use the locale settings specified in the environment
    locale.setlocale(locale.LC_ALL, "")

    # Try to fix Unicode issues on systems with bad defaults
    if _CHANGE_C_LC_CTYPE_TO_UTF8:
        _change_c_lc_ctype_to_utf8()

    # Get rid of the delay between pressing ESC and jumping to the parent menu,
    # unless the user has set ESCDELAY (see ncurses(3)). This makes the UI much
    # smoother to work with.
    #
    # Note: This is strictly pretty iffy, since escape codes for e.g. cursor
    # keys start with ESC, but I've never seen it cause problems in practice
    # (probably because it's unlikely that the escape code for a key would get
    # split up across read()s, at least with a terminal emulator). Please
    # report if you run into issues. Some suitable small default value could be
    # used here instead in that case. Maybe it's silly to not put in the
    # smallest imperceptible delay here already, though I don't like guessing.
    #
    # (From a quick glance at the ncurses source code, ESCDELAY might only be
    # relevant for mouse events there, so maybe escapes are assumed to arrive
    # in one piece already...)
    os.environ.setdefault("ESCDELAY", "0")

    # Enter curses mode. _menuconfig() returns a string to print on exit, after
    # curses has been de-initialized.
    print(curses.wrapper(_menuconfig))


def _load_config():
    # Loads any existing .config file. See the Kconfig.load_config() docstring.
    #
    # Returns True if .config is missing or outdated. We always prompt for
    # saving the configuration in that case.

    print(_kconf.load_config())
    if not os.path.exists(_conf_filename):
        # No .config
        return True

    return _needs_save()


def _needs_save():
    # Returns True if a just-loaded .config file is outdated (would get
    # modified when saving)

    if _kconf.missing_syms:
        # Assignments to undefined symbols in the .config
        return True

    for sym in _kconf.unique_defined_syms:
        if sym.user_value is None:
            if sym.config_string:
                # Unwritten symbol
                return True
        elif sym.orig_type in (BOOL, TRISTATE):
            if sym.tri_value != sym.user_value:
                # Written bool/tristate symbol, new value
                return True
        elif sym.str_value != sym.user_value:
            # Written string/int/hex symbol, new value
            return True

    # No need to prompt for save
    return False


# Global variables used below:
#
#   _stdscr:
#     stdscr from curses
#
#   _cur_menu:
#     Menu node of the menu (or menuconfig symbol, or choice) currently being
#     shown
#
#   _shown:
#     List of items in _cur_menu that are shown (ignoring scrolling). In
#     show-all mode, this list contains all items in _cur_menu. Otherwise, it
#     contains just the visible items.
#
#   _sel_node_i:
#     Index in _shown of the currently selected node
#
#   _menu_scroll:
#     Index in _shown of the top row of the main display
#
#   _parent_screen_rows:
#     List/stack of the row numbers that the selections in the parent menus
#     appeared on. This is used to prevent the scrolling from jumping around
#     when going in and out of menus.
#
#   _show_help/_show_name/_show_all:
#     If True, the corresponding mode is on. See the module docstring.
#
#   _conf_filename:
#     File to save the configuration to
#
#   _minconf_filename:
#     File to save minimal configurations to
#
#   _conf_changed:
#     True if the configuration has been changed. If False, we don't bother
#     showing the save-and-quit dialog.
#
#     We reset this to False whenever the configuration is saved explicitly
#     from the save dialog.


def _menuconfig(stdscr):
    # Logic for the main display, with the list of symbols, etc.

    global _stdscr
    global _conf_filename
    global _conf_changed
    global _minconf_filename
    global _show_help
    global _show_name

    _stdscr = stdscr

    _init()

    while True:
        _draw_main()
        curses.doupdate()


        c = _getch_compat(_menu_win)

        if c == curses.KEY_RESIZE:
            _resize_main()

        elif c in (curses.KEY_DOWN, "j", "J"):
            _select_next_menu_entry()

        elif c in (curses.KEY_UP, "k", "K"):
            _select_prev_menu_entry()

        elif c in (curses.KEY_NPAGE, "\x04"):  # Page Down/Ctrl-D
            # Keep it simple. This way we get sane behavior for small windows,
            # etc., for free.
            for _ in range(_PG_JUMP):
                _select_next_menu_entry()

        elif c in (curses.KEY_PPAGE, "\x15"):  # Page Up/Ctrl-U
            for _ in range(_PG_JUMP):
                _select_prev_menu_entry()

        elif c in (curses.KEY_END, "G"):
            _select_last_menu_entry()

        elif c in (curses.KEY_HOME, "g"):
            _select_first_menu_entry()

        elif c == " ":
            # Toggle the node if possible
            sel_node = _shown[_sel_node_i]
            if not _change_node(sel_node):
                _enter_menu(sel_node)

        elif c in (curses.KEY_RIGHT, "\n", "l", "L"):
            # Enter the node if possible
            sel_node = _shown[_sel_node_i]
            if not _enter_menu(sel_node):
                _change_node(sel_node)

        elif c in ("n", "N"):
            _set_sel_node_tri_val(0)

        elif c in ("m", "M"):
            _set_sel_node_tri_val(1)

        elif c in ("y", "Y"):
            _set_sel_node_tri_val(2)

        elif c in (curses.KEY_LEFT, curses.KEY_BACKSPACE, _ERASE_CHAR,
                   "\x1B", "h", "H"):  # \x1B = ESC

            if c == "\x1B" and _cur_menu is _kconf.top_node:
                res = _quit_dialog()
                if res:
                    return res
            else:
                _leave_menu()

        elif c in ("o", "O"):
            _load_dialog()

        elif c in ("s", "S"):
            filename = _save_dialog(_kconf.write_config, _conf_filename,
                                    "configuration")
            if filename:
                _conf_filename = filename
                _conf_changed = False

        elif c in ("d", "D"):
            filename = _save_dialog(_kconf.write_min_config, _minconf_filename,
                                    "minimal configuration")
            if filename:
                _minconf_filename = filename

        elif c == "/":
            _jump_to_dialog()
            # The terminal might have been resized while the fullscreen jump-to
            # dialog was open
            _resize_main()

        elif c == "?":
            _info_dialog(_shown[_sel_node_i], False)
            # The terminal might have been resized while the fullscreen info
            # dialog was open
            _resize_main()

        elif c in ("f", "F"):
            _show_help = not _show_help
            _set_style(_help_win, "show-help" if _show_help else "help")
            _resize_main()

        elif c in ("c", "C"):
            _show_name = not _show_name

        elif c in ("a", "A"):
            _toggle_show_all()

        elif c in ("q", "Q"):
            res = _quit_dialog()
            if res:
                return res


def _quit_dialog():
    if not _conf_changed:
        return "No changes to save (for '{}')".format(_conf_filename)

    while True:
        c = _key_dialog(
            "Quit",
            " Save configuration?\n"
            "\n"
            "(Y)es  (N)o  (C)ancel",
            "ync")

        if c is None or c == "c":
            return None

        if c == "y":
            # Returns a message to print
            msg = _try_save(_kconf.write_config, _conf_filename, "configuration")
            if msg:
                return msg

        elif c == "n":
            return "Configuration ({}) was not saved".format(_conf_filename)


def _init():
    # Initializes the main display with the list of symbols, etc. Also does
    # misc. global initialization that needs to happen after initializing
    # curses.

    global _ERASE_CHAR

    global _path_win
    global _top_sep_win
    global _menu_win
    global _bot_sep_win
    global _help_win

    global _parent_screen_rows
    global _cur_menu
    global _shown
    global _sel_node_i
    global _menu_scroll

    global _show_help
    global _show_name

    # Looking for this in addition to KEY_BACKSPACE (which is unreliable) makes
    # backspace work with TERM=vt100. That makes it likely to work in sane
    # environments.
    _ERASE_CHAR = curses.erasechar()
    if sys.version_info[0] >= 3:
        # erasechar() returns a one-byte bytes object on Python 3. This sets
        # _ERASE_CHAR to a blank string if it can't be decoded, which should be
        # harmless.
        _ERASE_CHAR = _ERASE_CHAR.decode("utf-8", "ignore")

    _init_styles()

    # Hide the cursor
    _safe_curs_set(0)

    # Initialize windows

    # Top row, with menu path
    _path_win = _styled_win("path")

    # Separator below menu path, with title and arrows pointing up
    _top_sep_win = _styled_win("separator")

    # List of menu entries with symbols, etc.
    _menu_win = _styled_win("list")
    _menu_win.keypad(True)

    # Row below menu list, with arrows pointing down
    _bot_sep_win = _styled_win("separator")

    # Help window with keys at the bottom. Shows help texts in show-help mode.
    _help_win = _styled_win("help")

    # The rows we'd like the nodes in the parent menus to appear on. This
    # prevents the scroll from jumping around when going in and out of menus.
    _parent_screen_rows = []

    # Initial state

    _cur_menu = _kconf.top_node
    _shown = _shown_nodes(_cur_menu)
    _sel_node_i = _menu_scroll = 0

    _show_help = _show_name = False

    # Give windows their initial size
    _resize_main()


def _resize_main():
    # Resizes the main display, with the list of symbols, etc., to fill the
    # terminal

    global _menu_scroll

    screen_height, screen_width = _stdscr.getmaxyx()

    _path_win.resize(1, screen_width)
    _top_sep_win.resize(1, screen_width)
    _bot_sep_win.resize(1, screen_width)

    help_win_height = _SHOW_HELP_HEIGHT if _show_help else \
        len(_MAIN_HELP_LINES)

    menu_win_height = screen_height - help_win_height - 3

    if menu_win_height >= 1:
        _menu_win.resize(menu_win_height, screen_width)
        _help_win.resize(help_win_height, screen_width)

        _top_sep_win.mvwin(1, 0)
        _menu_win.mvwin(2, 0)
        _bot_sep_win.mvwin(2 + menu_win_height, 0)
        _help_win.mvwin(2 + menu_win_height + 1, 0)
    else:
        # Degenerate case. Give up on nice rendering and just prevent errors.

        menu_win_height = 1

        _menu_win.resize(1, screen_width)
        _help_win.resize(1, screen_width)

        for win in _top_sep_win, _menu_win, _bot_sep_win, _help_win:
            win.mvwin(0, 0)

    # Adjust the scroll so that the selected node is still within the window,
    # if needed
    if _sel_node_i - _menu_scroll >= menu_win_height:
        _menu_scroll = _sel_node_i - menu_win_height + 1


def _height(win):
    # Returns the height of 'win'

    return win.getmaxyx()[0]


def _width(win):
    # Returns the width of 'win'

    return win.getmaxyx()[1]


def _enter_menu(menu):
    # Makes 'menu' the currently displayed menu. In addition to actual 'menu's,
    # "menu" here includes choices and symbols defined with the 'menuconfig'
    # keyword.
    #
    # Returns False if 'menu' can't be entered.

    global _cur_menu
    global _shown
    global _sel_node_i
    global _menu_scroll

    if not menu.is_menuconfig:
        # Not a menu
        return False

    shown_sub = _shown_nodes(menu)
    # Never enter empty menus. We depend on having a current node.
    if not shown_sub:
        return False

    # Remember where the current node appears on the screen, so we can try
    # to get it to appear in the same place when we leave the menu
    _parent_screen_rows.append(_sel_node_i - _menu_scroll)

    # Jump into menu
    _cur_menu = menu
    _shown = shown_sub
    _sel_node_i = _menu_scroll = 0

    if isinstance(menu.item, Choice):
        _select_selected_choice_sym()

    return True


def _select_selected_choice_sym():
    # Puts the cursor on the currently selected (y-valued) choice symbol, if
    # any. Does nothing if if the choice has no selection (is not visible/in y
    # mode).

    global _sel_node_i

    choice = _cur_menu.item
    if choice.selection:
        # Search through all menu nodes to handle choice symbols being defined
        # in multiple locations
        for node in choice.selection.nodes:
            if node in _shown:
                _sel_node_i = _shown.index(node)
                _center_vertically()
                return


def _jump_to(node):
    # Jumps directly to the menu node 'node'

    global _cur_menu
    global _shown
    global _sel_node_i
    global _menu_scroll
    global _show_all
    global _parent_screen_rows

    # Clear remembered menu locations. We might not even have been in the
    # parent menus before.
    _parent_screen_rows = []

    old_show_all = _show_all
    jump_into = (isinstance(node.item, Choice) or node.item == MENU) and \
                node.list

    # If we're jumping to a non-empty choice or menu, jump to the first entry
    # in it instead of jumping to its menu node
    if jump_into:
        _cur_menu = node
        node = node.list
    else:
        _cur_menu = _parent_menu(node)

    _shown = _shown_nodes(_cur_menu)
    if node not in _shown:
        # The node wouldn't be shown. Turn on show-all to show it.
        _show_all = True
        _shown = _shown_nodes(_cur_menu)

    _sel_node_i = _shown.index(node)

    if jump_into and not old_show_all and _show_all:
        # If we're jumping into a choice or menu and were forced to turn on
        # show-all because the first entry wasn't visible, try turning it off.
        # That will land us at the first visible node if there are visible
        # nodes, and is a no-op otherwise.
        _toggle_show_all()

    _center_vertically()

    # If we're jumping to a non-empty choice, jump to the selected symbol, if
    # any
    if jump_into and isinstance(_cur_menu.item, Choice):
        _select_selected_choice_sym()


def _leave_menu():
    # Jumps to the parent menu of the current menu. Does nothing if we're in
    # the top menu.

    global _cur_menu
    global _shown
    global _sel_node_i
    global _menu_scroll

    if _cur_menu is _kconf.top_node:
        return

    # Jump to parent menu
    parent = _parent_menu(_cur_menu)
    _shown = _shown_nodes(parent)
    _sel_node_i = _shown.index(_cur_menu)
    _cur_menu = parent

    # Try to make the menu entry appear on the same row on the screen as it did
    # before we entered the menu.

    if _parent_screen_rows:
        # The terminal might have shrunk since we were last in the parent menu
        screen_row = min(_parent_screen_rows.pop(), _height(_menu_win) - 1)
        _menu_scroll = max(_sel_node_i - screen_row, 0)
    else:
        # No saved parent menu locations, meaning we jumped directly to some
        # node earlier
        _center_vertically()


def _select_next_menu_entry():
    # Selects the menu entry after the current one, adjusting the scroll if
    # necessary. Does nothing if we're already at the last menu entry.

    global _sel_node_i
    global _menu_scroll

    if _sel_node_i < len(_shown) - 1:
        # Jump to the next node
        _sel_node_i += 1

        # If the new node is sufficiently close to the edge of the menu window
        # (as determined by _SCROLL_OFFSET), increase the scroll by one. This
        # gives nice and non-jumpy behavior even when
        # _SCROLL_OFFSET >= _height(_menu_win).
        if _sel_node_i >= _menu_scroll + _height(_menu_win) - _SCROLL_OFFSET \
           and _menu_scroll < _max_scroll(_shown, _menu_win):

            _menu_scroll += 1


def _select_prev_menu_entry():
    # Selects the menu entry before the current one, adjusting the scroll if
    # necessary. Does nothing if we're already at the first menu entry.

    global _sel_node_i
    global _menu_scroll

    if _sel_node_i > 0:
        # Jump to the previous node
        _sel_node_i -= 1

        # See _select_next_menu_entry()
        if _sel_node_i < _menu_scroll + _SCROLL_OFFSET:
            _menu_scroll = max(_menu_scroll - 1, 0)


def _select_last_menu_entry():
    # Selects the last menu entry in the current menu

    global _sel_node_i
    global _menu_scroll

    _sel_node_i = len(_shown) - 1
    _menu_scroll = _max_scroll(_shown, _menu_win)


def _select_first_menu_entry():
    # Selects the first menu entry in the current menu

    global _sel_node_i
    global _menu_scroll

    _sel_node_i = _menu_scroll = 0


def _toggle_show_all():
    # Toggles show-all mode on/off. If turning it off would give no visible
    # items in the current menu, it is left on.

    global _show_all
    global _shown
    global _sel_node_i
    global _menu_scroll

    # Row on the screen the cursor is on. Preferably we want the same row to
    # stay highlighted.
    old_row = _sel_node_i - _menu_scroll

    _show_all = not _show_all
    # List of new nodes to be shown after toggling _show_all
    new_shown = _shown_nodes(_cur_menu)

    # Find a good node to select. The selected node might disappear if show-all
    # mode is turned off.

    # Select the previously selected node itself if it is still visible. If
    # there are visible nodes before it, select the closest one.
    for node in _shown[_sel_node_i::-1]:
        if node in new_shown:
            _sel_node_i = new_shown.index(node)
            break
    else:
        # No visible nodes before the previously selected node. Select the
        # closest visible node after it instead.
        for node in _shown[_sel_node_i + 1:]:
            if node in new_shown:
                _sel_node_i = new_shown.index(node)
                break
        else:
            # No visible nodes at all, meaning show-all was turned off inside
            # an invisible menu. Don't allow that, as the implementation relies
            # on always having a selected node.
            _show_all = True
            return

    _shown = new_shown

    # Try to make the cursor stay on the same row in the menu window. This
    # might be impossible if too many nodes have disappeared above the node.
    _menu_scroll = max(_sel_node_i - old_row, 0)


def _center_vertically():
    # Centers the selected node vertically, if possible

    global _menu_scroll

    _menu_scroll = min(max(_sel_node_i - _height(_menu_win)//2, 0),
                       _max_scroll(_shown, _menu_win))


def _draw_main():
    # Draws the "main" display, with the list of symbols, the header, and the
    # footer.
    #
    # This could be optimized to only update the windows that have actually
    # changed, but keep it simple for now and let curses sort it out.

    term_width = _width(_stdscr)


    #
    # Update the separator row below the menu path
    #

    _top_sep_win.erase()

    # Draw arrows pointing up if the symbol window is scrolled down. Draw them
    # before drawing the title, so the title ends up on top for small windows.
    if _menu_scroll > 0:
        _safe_hline(_top_sep_win, 0, 4, curses.ACS_UARROW, _N_SCROLL_ARROWS)

    # Add the 'mainmenu' text as the title, centered at the top
    _safe_addstr(_top_sep_win,
                 0, max((term_width - len(_kconf.mainmenu_text))//2, 0),
                 _kconf.mainmenu_text)

    _top_sep_win.noutrefresh()

    # Note: The menu path at the top is deliberately updated last. See below.

    #
    # Update the symbol window
    #

    _menu_win.erase()

    # Draw the _shown nodes starting from index _menu_scroll up to either as
    # many as fit in the window, or to the end of _shown
    for i in range(_menu_scroll,
                   min(_menu_scroll + _height(_menu_win), len(_shown))):

        node = _shown[i]

        # The 'not _show_all' test avoids showing invisible items in red
        # outside show-all mode, which could look confusing/broken. Invisible
        # symbols show up outside show-all mode if an invisible symbol has
        # visible children in an implicit (indented) menu.
        if _visible(node) or not _show_all:
            style = _style["selection" if i == _sel_node_i else "list"]
        else:
            style = _style["inv-selection" if i == _sel_node_i else "inv-list"]

        _safe_addstr(_menu_win, i - _menu_scroll, 0, _node_str(node), style)

    _menu_win.noutrefresh()


    #
    # Update the bottom separator window
    #

    _bot_sep_win.erase()

    # Draw arrows pointing down if the symbol window is scrolled up
    if _menu_scroll < _max_scroll(_shown, _menu_win):
        _safe_hline(_bot_sep_win, 0, 4, curses.ACS_DARROW, _N_SCROLL_ARROWS)

    # Indicate when show-name/show-help/show-all mode is enabled
    enabled_modes = []
    if _show_help:
        enabled_modes.append("show-help (toggle with [F])")
    if _show_name:
        enabled_modes.append("show-name")
    if _show_all:
        enabled_modes.append("show-all")
    if enabled_modes:
        s = " and ".join(enabled_modes) + " mode enabled"
        _safe_addstr(_bot_sep_win, 0, max(term_width - len(s) - 2, 0), s)

    _bot_sep_win.noutrefresh()


    #
    # Update the help window, which shows either key bindings or help texts
    #

    _help_win.erase()

    if _show_help:
        node = _shown[_sel_node_i]
        if isinstance(node.item, (Symbol, Choice)) and node.help:
            help_lines = textwrap.wrap(node.help, _width(_help_win))
            for i in range(min(_height(_help_win), len(help_lines))):
                _safe_addstr(_help_win, i, 0, help_lines[i])
        else:
            _safe_addstr(_help_win, 0, 0, "(no help)")
    else:
        for i, line in enumerate(_MAIN_HELP_LINES):
            _safe_addstr(_help_win, i, 0, line)

    _help_win.noutrefresh()


    #
    # Update the top row with the menu path.
    #
    # Doing this last leaves the cursor on the top row, which avoids some minor
    # annoying jumpiness in gnome-terminal when reducing the height of the
    # terminal. It seems to happen whenever the row with the cursor on it
    # disappears.
    #

    _path_win.erase()

    # Draw the menu path ("(Top) -> Menu -> Submenu -> ...")

    menu_prompts = []

    menu = _cur_menu
    while menu is not _kconf.top_node:
        # Promptless choices can be entered in show-all mode. Use
        # standard_sc_expr_str() for them, so they show up as
        # '<choice (name if any)>'.
        menu_prompts.append(menu.prompt[0] if menu.prompt else
                            standard_sc_expr_str(menu.item))
        menu = menu.parent
    menu_prompts.append("(Top)")
    menu_prompts.reverse()

    # Hack: We can't put ACS_RARROW directly in the string. Temporarily
    # represent it with NULL.
    menu_path_str = " \0 ".join(menu_prompts)

    # Scroll the menu path to the right if needed to make the current menu's
    # title visible
    if len(menu_path_str) > term_width:
        menu_path_str = menu_path_str[len(menu_path_str) - term_width:]

    # Print the path with the arrows reinserted
    split_path = menu_path_str.split("\0")
    _safe_addstr(_path_win, split_path[0])
    for s in split_path[1:]:
        _safe_addch(_path_win, curses.ACS_RARROW)
        _safe_addstr(_path_win, s)

    _path_win.noutrefresh()


def _parent_menu(node):
    # Returns the menu node of the menu that contains 'node'. In addition to
    # proper 'menu's, this might also be a 'menuconfig' symbol or a 'choice'.
    # "Menu" here means a menu in the interface.

    menu = node.parent
    while not menu.is_menuconfig:
        menu = menu.parent
    return menu


def _shown_nodes(menu):
    # Returns the list of menu nodes from 'menu' (see _parent_menu()) that
    # would be shown when entering it

    def rec(node):
        res = []

        while node:
            if _visible(node) or _show_all:
                res.append(node)
                if node.list and not node.is_menuconfig:
                    # Nodes from implicit menu created from dependencies. Will
                    # be shown indented. Note that is_menuconfig is True for
                    # menus and choices as well as 'menuconfig' symbols.
                    res += rec(node.list)

            elif node.list and isinstance(node.item, Symbol):
                # Show invisible symbols if they have visible children. This
                # can happen for an m/y-valued symbol with an optional prompt
                # ('prompt "foo" is COND') that is currently disabled. Note
                # that it applies to both 'config' and 'menuconfig' symbols.
                shown_children = rec(node.list)
                if shown_children:
                    res.append(node)
                    if not node.is_menuconfig:
                        res += shown_children

            node = node.next

        return res

    if isinstance(menu.item, Choice):
        # For named choices defined in multiple locations, entering the choice
        # at a particular menu node would normally only show the choice symbols
        # defined there (because that's what the MenuNode tree looks like).
        #
        # That might look confusing, and makes extending choices by defining
        # them in multiple locations less useful. Instead, gather all the child
        # menu nodes for all the choices whenever a choice is entered. That
        # makes all choice symbols visible at all locations.
        #
        # Choices can contain non-symbol items (people do all sorts of weird
        # stuff with them), hence the generality here. We really need to
        # preserve the menu tree at each choice location.
        #
        # Note: Named choices are pretty broken in the C tools, and this is
        # super obscure, so you probably won't find much that relies on this.
        # This whole 'if' could be deleted if you don't care about defining
        # choices in multiple locations to add symbols (which will still work,
        # just with things being displayed in a way that might be unexpected).

        # Do some additional work to avoid listing choice symbols twice if all
        # or part of the choice is copied in multiple locations (e.g. by
        # including some Kconfig file multiple times). We give the prompts at
        # the current location precedence.
        seen_syms = {node.item for node in rec(menu.list)
                     if isinstance(node.item, Symbol)}
        res = []
        for choice_node in menu.item.nodes:
            for node in rec(choice_node.list):
                # 'choice_node is menu' checks if we're dealing with the
                # current location
                if node.item not in seen_syms or choice_node is menu:
                    res.append(node)
                    if isinstance(node.item, Symbol):
                        seen_syms.add(node.item)
        return res

    return rec(menu.list)


def _visible(node):
    # Returns True if the node should appear in the menu (outside show-all
    # mode)

    return node.prompt and expr_value(node.prompt[1]) and not \
        (node.item == MENU and not expr_value(node.visibility))


def _change_node(node):
    # Changes the value of the menu node 'node' if it is a symbol. Bools and
    # tristates are toggled, while other symbol types pop up a text entry
    # dialog.
    #
    # Returns False if the value of 'node' can't be changed.

    if not _changeable(node):
        return False

    # sc = symbol/choice
    sc = node.item

    if sc.orig_type in (INT, HEX, STRING):
        s = sc.str_value

        while True:
            s = _input_dialog(
                "{} ({})".format(node.prompt[0], TYPE_TO_STR[sc.orig_type]),
                s, _range_info(sc))

            if s is None:
                break

            if sc.orig_type in (INT, HEX):
                s = s.strip()

                # 'make menuconfig' does this too. Hex values not starting with
                # '0x' are accepted when loading .config files though.
                if sc.orig_type == HEX and not s.startswith(("0x", "0X")):
                    s = "0x" + s

            if _check_valid(sc, s):
                _set_val(sc, s)
                break

    elif len(sc.assignable) == 1:
        # Handles choice symbols for choices in y mode, which are a special
        # case: .assignable can be (2,) while .tri_value is 0.
        _set_val(sc, sc.assignable[0])

    else:
        # Set the symbol to the value after the current value in
        # sc.assignable, with wrapping
        val_index = sc.assignable.index(sc.tri_value)
        _set_val(sc, sc.assignable[(val_index + 1) % len(sc.assignable)])


    if _is_y_mode_choice_sym(sc) and not node.list:
        # Immediately jump to the parent menu after making a choice selection,
        # like 'make menuconfig' does, except if the menu node has children
        # (which can happen if a symbol 'depends on' a choice symbol that
        # immediately precedes it).
        _leave_menu()


    return True


def _changeable(node):
    # Returns True if the value if 'node' can be changed

    sc = node.item

    if not isinstance(sc, (Symbol, Choice)):
        return False

    # This will hit for invisible symbols, which appear in show-all mode and
    # when an invisible symbol has visible children (which can happen e.g. for
    # symbols with optional prompts)
    if not (node.prompt and expr_value(node.prompt[1])):
        return False

    return sc.orig_type in (STRING, INT, HEX) or len(sc.assignable) > 1 \
        or _is_y_mode_choice_sym(sc)


def _set_sel_node_tri_val(tri_val):
    # Sets the value of the currently selected menu entry to 'tri_val', if that
    # value can be assigned

    sc = _shown[_sel_node_i].item
    if isinstance(sc, (Symbol, Choice)) and tri_val in sc.assignable:
        _set_val(sc, tri_val)


def _set_val(sc, val):
    # Wrapper around Symbol/Choice.set_value() for updating the menu state and
    # _conf_changed

    global _conf_changed

    # Use the string representation of tristate values. This makes the format
    # consistent for all symbol types.
    if val in TRI_TO_STR:
        val = TRI_TO_STR[val]

    if val != sc.str_value:
        sc.set_value(val)
        _conf_changed = True

        # Changing the value of the symbol might have changed what items in the
        # current menu are visible. Recalculate the state.
        _update_menu()


def _update_menu():
    # Updates the current menu after the value of a symbol or choice has been
    # changed. Changing a value might change which items in the menu are
    # visible.
    #
    # If possible, preserves the location of the cursor on the screen when
    # items are added/removed above the selected item.

    global _shown
    global _sel_node_i
    global _menu_scroll

    # Row on the screen the cursor was on
    old_row = _sel_node_i - _menu_scroll

    sel_node = _shown[_sel_node_i]

    # New visible nodes
    _shown = _shown_nodes(_cur_menu)

    # New index of selected node
    _sel_node_i = _shown.index(sel_node)

    # Try to make the cursor stay on the same row in the menu window. This
    # might be impossible if too many nodes have disappeared above the node.
    _menu_scroll = max(_sel_node_i - old_row, 0)


def _input_dialog(title, initial_text, info_text=None):
    # Pops up a dialog that prompts the user for a string
    #
    # title:
    #   Title to display at the top of the dialog window's border
    #
    # initial_text:
    #   Initial text to prefill the input field with
    #
    # info_text:
    #   String to show next to the input field. If None, just the input field
    #   is shown.

    win = _styled_win("body")
    win.keypad(True)

    info_lines = info_text.split("\n") if info_text else []

    # Give the input dialog its initial size
    _resize_input_dialog(win, title, info_lines)

    _safe_curs_set(2)

    # Input field text
    s = initial_text

    # Cursor position
    i = len(initial_text)

    def edit_width():
        return _width(win) - 4

    # Horizontal scroll offset
    hscroll = max(i - edit_width() + 1, 0)

    while True:
        # Draw the "main" display with the menu, etc., so that resizing still
        # works properly. This is like a stack of windows, only hardcoded for
        # now.
        _draw_main()
        _draw_input_dialog(win, title, info_lines, s, i, hscroll)
        curses.doupdate()


        c = _getch_compat(win)

        if c == curses.KEY_RESIZE:
            # Resize the main display too. The dialog floats above it.
            _resize_main()
            _resize_input_dialog(win, title, info_lines)

        elif c == "\n":
            _safe_curs_set(0)
            return s

        elif c == "\x1B":  # \x1B = ESC
            _safe_curs_set(0)
            return None

        else:
            s, i, hscroll = _edit_text(c, s, i, hscroll, edit_width())


def _resize_input_dialog(win, title, info_lines):
    # Resizes the input dialog to a size appropriate for the terminal size

    screen_height, screen_width = _stdscr.getmaxyx()

    win_height = 5
    if info_lines:
        win_height += len(info_lines) + 1
    win_height = min(win_height, screen_height)

    win_width = max(_INPUT_DIALOG_MIN_WIDTH,
                    len(title) + 4,
                    *(len(line) + 4 for line in info_lines))
    win_width = min(win_width, screen_width)

    win.resize(win_height, win_width)
    win.mvwin((screen_height - win_height)//2,
              (screen_width - win_width)//2)


def _draw_input_dialog(win, title, info_lines, s, i, hscroll):
    edit_width = _width(win) - 4

    win.erase()

    # Note: Perhaps having a separate window for the input field would be nicer
    visible_s = s[hscroll:hscroll + edit_width]
    _safe_addstr(win, 2, 2, visible_s + " "*(edit_width - len(visible_s)),
                 _style["edit"])

    for linenr, line in enumerate(info_lines):
        _safe_addstr(win, 4 + linenr, 2, line)

    # Draw the frame last so that it overwrites the body text for small windows
    _draw_frame(win, title)

    _safe_move(win, 2, 2 + i - hscroll)

    win.noutrefresh()


def _load_dialog():
    # Dialog for loading a new configuration

    global _conf_changed
    global _conf_filename
    global _show_all

    if _conf_changed:
        c = _key_dialog(
            "Load",
            "You have unsaved changes. Load new\n"
            "configuration anyway?\n"
            "\n"
            "         (O)K  (C)ancel",
            "oc")

        if c is None or c == "c":
            return

    filename = _conf_filename
    while True:
        filename = _input_dialog("File to load", filename, _load_save_info())
        if filename is None:
            return

        filename = os.path.expanduser(filename)

        if _try_load(filename):
            _conf_filename = filename
            _conf_changed = _needs_save()

            # Turn on show-all mode if the selected node is not visible after
            # loading the new configuration. _shown still holds the old state.
            if _shown[_sel_node_i] not in _shown_nodes(_cur_menu):
                _show_all = True

            _update_menu()

            # The message dialog indirectly updates the menu display, so _msg()
            # must be called after the new state has been initialized
            _msg("Success", "Loaded " + filename)
            return


def _try_load(filename):
    # Tries to load a configuration file. Pops up an error and returns False on
    # failure.
    #
    # filename:
    #   Configuration file to load

    try:
        _kconf.load_config(filename)
        return True
    except EnvironmentError as e:
        _error("Error loading '{}'\n\n{} (errno: {})"
               .format(filename, e.strerror, errno.errorcode[e.errno]))
        return False


def _save_dialog(save_fn, default_filename, description):
    # Dialog for saving the current configuration
    #
    # save_fn:
    #   Function to call with 'filename' to save the file
    #
    # default_filename:
    #   Prefilled filename in the input field
    #
    # description:
    #   String describing the thing being saved
    #
    # Return value:
    #   The path to the saved file, or None if no file was saved

    filename = default_filename
    while True:
        filename = _input_dialog("Filename to save {} to".format(description),
                                 filename, _load_save_info())
        if filename is None:
            return None

        filename = os.path.expanduser(filename)

        msg = _try_save(save_fn, filename, description)
        if msg:
            _msg("Success", msg)
            return filename


def _try_save(save_fn, filename, description):
    # Tries to save a configuration file. Returns a message to print on
    # success.
    #
    # save_fn:
    #   Function to call with 'filename' to save the file
    #
    # description:
    #   String describing the thing being saved
    #
    # Return value:
    #   A message to print on success, and None on failure

    try:
        # save_fn() returns a message to print
        return save_fn(filename)
    except EnvironmentError as e:
        _error("Error saving {} to '{}'\n\n{} (errno: {})"
               .format(description, e.filename, e.strerror,
                       errno.errorcode[e.errno]))
        return None


def _key_dialog(title, text, keys):
    # Pops up a dialog that can be closed by pressing a key
    #
    # title:
    #   Title to display at the top of the dialog window's border
    #
    # text:
    #   Text to show in the dialog
    #
    # keys:
    #   List of keys that will close the dialog. Other keys (besides ESC) are
    #   ignored. The caller is responsible for providing a hint about which
    #   keys can be pressed in 'text'.
    #
    # Return value:
    #   The key that was pressed to close the dialog. Uppercase characters are
    #   converted to lowercase. ESC will always close the dialog, and returns
    #   None.

    win = _styled_win("body")
    win.keypad(True)

    _resize_key_dialog(win, text)

    while True:
        # See _input_dialog()
        _draw_main()
        _draw_key_dialog(win, title, text)
        curses.doupdate()


        c = _getch_compat(win)

        if c == curses.KEY_RESIZE:
            # Resize the main display too. The dialog floats above it.
            _resize_main()
            _resize_key_dialog(win, text)

        elif c == "\x1B":  # \x1B = ESC
            return None

        elif isinstance(c, str):
            c = c.lower()
            if c in keys:
                return c


def _resize_key_dialog(win, text):
    # Resizes the key dialog to a size appropriate for the terminal size

    screen_height, screen_width = _stdscr.getmaxyx()

    lines = text.split("\n")

    win_height = min(len(lines) + 4, screen_height)
    win_width = min(max(len(line) for line in lines) + 4, screen_width)

    win.resize(win_height, win_width)
    win.mvwin((screen_height - win_height)//2,
              (screen_width - win_width)//2)


def _draw_key_dialog(win, title, text):
    win.erase()

    for i, line in enumerate(text.split("\n")):
        _safe_addstr(win, 2 + i, 2, line)

    # Draw the frame last so that it overwrites the body text for small windows
    _draw_frame(win, title)

    win.noutrefresh()


def _draw_frame(win, title):
    # Draw a frame around the inner edges of 'win', with 'title' at the top

    win_height, win_width = win.getmaxyx()

    win.attron(_style["frame"])

    # Draw top/bottom edge
    _safe_hline(win,              0, 0, " ", win_width)
    _safe_hline(win, win_height - 1, 0, " ", win_width)

    # Draw left/right edge
    _safe_vline(win, 0,             0, " ", win_height)
    _safe_vline(win, 0, win_width - 1, " ", win_height)

    # Draw title
    _safe_addstr(win, 0, max((win_width - len(title))//2, 0), title)

    win.attroff(_style["frame"])


def _jump_to_dialog():
    # Implements the jump-to dialog, where symbols can be looked up via
    # incremental search and jumped to.
    #
    # Returns True if the user jumped to a symbol, and False if the dialog was
    # canceled.

    s = ""  # Search text
    prev_s = None  # Previous search text
    s_i = 0  # Search text cursor position
    hscroll = 0  # Horizontal scroll offset

    sel_node_i = 0  # Index of selected row
    scroll = 0  # Index in 'matches' of the top row of the list

    # Edit box at the top
    edit_box = _styled_win("jump-edit")
    edit_box.keypad(True)

    # List of matches
    matches_win = _styled_win("list")

    # Bottom separator, with arrows pointing down
    bot_sep_win = _styled_win("separator")

    # Help window with instructions at the bottom
    help_win = _styled_win("help")

    # Give windows their initial size
    _resize_jump_to_dialog(edit_box, matches_win, bot_sep_win, help_win,
                           sel_node_i, scroll)

    _safe_curs_set(2)

    # Logic duplication with _select_{next,prev}_menu_entry(), except we do a
    # functional variant that returns the new (sel_node_i, scroll) values to
    # avoid 'nonlocal'. TODO: Can this be factored out in some nice way?

    def select_next_match():
        if sel_node_i == len(matches) - 1:
            return sel_node_i, scroll

        if sel_node_i + 1 >= scroll + _height(matches_win) - _SCROLL_OFFSET \
           and scroll < _max_scroll(matches, matches_win):

            return sel_node_i + 1, scroll + 1

        return sel_node_i + 1, scroll

    def select_prev_match():
        if sel_node_i == 0:
            return sel_node_i, scroll

        if sel_node_i - 1 < scroll + _SCROLL_OFFSET:
            return sel_node_i - 1, max(scroll - 1, 0)

        return sel_node_i - 1, scroll

    while True:
        if s != prev_s:
            # The search text changed. Find new matching nodes.

            prev_s = s

            try:
                # We could use re.IGNORECASE here instead of lower(), but this
                # is noticeably less jerky while inputting regexes like
                # '.*debug$' (though the '.*' is redundant there). Those
                # probably have bad interactions with re.search(), which
                # matches anywhere in the string.
                #
                # It's not horrible either way. Just a bit smoother.
                regex_searches = [re.compile(regex).search
                                  for regex in s.lower().split()]

                # No exception thrown, so the regexes are okay
                bad_re = None

                # List of matching nodes
                matches = []
                add_match = matches.append

                # Search symbols and choices

                for node in _sorted_sc_nodes():
                    # Symbol/choice
                    sc = node.item

                    for search in regex_searches:
                        # Both the name and the prompt might be missing, since
                        # we're searching both symbols and choices

                        # Does the regex match either the symbol name or the
                        # prompt (if any)?
                        if not (sc.name and search(sc.name.lower()) or
                                node.prompt and search(node.prompt[0].lower())):

                            # Give up on the first regex that doesn't match, to
                            # speed things up a bit when multiple regexes are
                            # entered
                            break

                    else:
                        add_match(node)

                # Search menus and comments

                for node in _sorted_menu_comment_nodes():
                    for search in regex_searches:
                        if not search(node.prompt[0].lower()):
                            break
                    else:
                        add_match(node)

            except re.error as e:
                # Bad regex. Remember the error message so we can show it.
                bad_re = "Bad regular expression"
                # re.error.msg was added in Python 3.5
                if hasattr(e, "msg"):
                    bad_re += ": " + e.msg

                matches = []

            # Reset scroll and jump to the top of the list of matches
            sel_node_i = scroll = 0

        _draw_jump_to_dialog(edit_box, matches_win, bot_sep_win, help_win,
                             s, s_i, hscroll,
                             bad_re, matches, sel_node_i, scroll)
        curses.doupdate()


        c = _getch_compat(edit_box)

        if c == "\n":
            if matches:
                _jump_to(matches[sel_node_i])
                _safe_curs_set(0)
                return True

        elif c == "\x1B":  # \x1B = ESC
            _safe_curs_set(0)
            return False

        elif c == curses.KEY_RESIZE:
            # We adjust the scroll so that the selected node stays visible in
            # the list when the terminal is resized, hence the 'scroll'
            # assignment
            scroll = _resize_jump_to_dialog(
                edit_box, matches_win, bot_sep_win, help_win,
                sel_node_i, scroll)

        elif c == "\x06":  # \x06 = Ctrl-F
            if matches:
                _safe_curs_set(0)
                _info_dialog(matches[sel_node_i], True)
                _safe_curs_set(2)

                scroll = _resize_jump_to_dialog(
                    edit_box, matches_win, bot_sep_win, help_win,
                    sel_node_i, scroll)

        elif c == curses.KEY_DOWN:
            sel_node_i, scroll = select_next_match()

        elif c == curses.KEY_UP:
            sel_node_i, scroll = select_prev_match()

        elif c in (curses.KEY_NPAGE, "\x04"):  # Page Down/Ctrl-D
            # Keep it simple. This way we get sane behavior for small windows,
            # etc., for free.
            for _ in range(_PG_JUMP):
                sel_node_i, scroll = select_next_match()

        # Page Up (no Ctrl-U, as it's already used by the edit box)
        elif c == curses.KEY_PPAGE:
            for _ in range(_PG_JUMP):
                sel_node_i, scroll = select_prev_match()

        elif c == curses.KEY_END:
            sel_node_i = len(matches) - 1
            scroll = _max_scroll(matches, matches_win)

        elif c == curses.KEY_HOME:
            sel_node_i = scroll = 0

        else:
            s, s_i, hscroll = _edit_text(c, s, s_i, hscroll,
                                         _width(edit_box) - 2)


# Obscure Python: We never pass a value for cached_nodes, and it keeps pointing
# to the same list. This avoids a global.
def _sorted_sc_nodes(cached_nodes=[]):
    # Returns a sorted list of symbol and choice nodes to search. The symbol
    # nodes appear first, sorted by name, and then the choice nodes, sorted by
    # prompt and (secondarily) name.

    if not cached_nodes:
        # Add symbol nodes
        for sym in sorted(_kconf.unique_defined_syms,
                          key=lambda sym: sym.name):
            # += is in-place for lists
            cached_nodes += sym.nodes

        # Add choice nodes

        choices = sorted(_kconf.unique_choices,
                         key=lambda choice: choice.name or "")

        cached_nodes += sorted(
            [node
             for choice in choices
                 for node in choice.nodes],
            key=lambda node: node.prompt[0] if node.prompt else "")

    return cached_nodes


def _sorted_menu_comment_nodes(cached_nodes=[]):
    # Returns a list of menu and comment nodes to search, sorted by prompt,
    # with the menus first

    if not cached_nodes:
        def prompt_text(mc):
            return mc.prompt[0]

        cached_nodes += sorted(_kconf.menus, key=prompt_text)
        cached_nodes += sorted(_kconf.comments, key=prompt_text)

    return cached_nodes


def _resize_jump_to_dialog(edit_box, matches_win, bot_sep_win, help_win,
                           sel_node_i, scroll):
    # Resizes the jump-to dialog to fill the terminal.
    #
    # Returns the new scroll index. We adjust the scroll if needed so that the
    # selected node stays visible.

    screen_height, screen_width = _stdscr.getmaxyx()

    bot_sep_win.resize(1, screen_width)

    help_win_height = len(_JUMP_TO_HELP_LINES)
    matches_win_height = screen_height - help_win_height - 4

    if matches_win_height >= 1:
        edit_box.resize(3, screen_width)
        matches_win.resize(matches_win_height, screen_width)
        help_win.resize(help_win_height, screen_width)

        matches_win.mvwin(3, 0)
        bot_sep_win.mvwin(3 + matches_win_height, 0)
        help_win.mvwin(3 + matches_win_height + 1, 0)
    else:
        # Degenerate case. Give up on nice rendering and just prevent errors.

        matches_win_height = 1

        edit_box.resize(screen_height, screen_width)
        matches_win.resize(1, screen_width)
        help_win.resize(1, screen_width)

        for win in matches_win, bot_sep_win, help_win:
            win.mvwin(0, 0)

    # Adjust the scroll so that the selected row is still within the window, if
    # needed
    if sel_node_i - scroll >= matches_win_height:
        return sel_node_i - matches_win_height + 1
    return scroll


def _draw_jump_to_dialog(edit_box, matches_win, bot_sep_win, help_win,
                         s, s_i, hscroll,
                         bad_re, matches, sel_node_i, scroll):

    edit_width = _width(edit_box) - 2


    #
    # Update list of matches
    #

    matches_win.erase()

    if matches:
        for i in range(scroll,
                       min(scroll + _height(matches_win), len(matches))):

            node = matches[i]

            if isinstance(node.item, (Symbol, Choice)):
                node_str = _name_and_val_str(node.item)
                if node.prompt:
                    node_str += ' "{}"'.format(node.prompt[0])
            elif node.item == MENU:
                node_str = 'menu "{}"'.format(node.prompt[0])
            else:  # node.item == COMMENT
                node_str = 'comment "{}"'.format(node.prompt[0])

            _safe_addstr(matches_win, i - scroll, 0, node_str,
                         _style["selection" if i == sel_node_i else "list"])

    else:
        # bad_re holds the error message from the re.error exception on errors
        _safe_addstr(matches_win, 0, 0, bad_re or "No matches")

    matches_win.noutrefresh()


    #
    # Update bottom separator line
    #

    bot_sep_win.erase()

    # Draw arrows pointing down if the symbol list is scrolled up
    if scroll < _max_scroll(matches, matches_win):
        _safe_hline(bot_sep_win, 0, 4, curses.ACS_DARROW, _N_SCROLL_ARROWS)

    bot_sep_win.noutrefresh()


    #
    # Update help window at bottom
    #

    help_win.erase()

    for i, line in enumerate(_JUMP_TO_HELP_LINES):
        _safe_addstr(help_win, i, 0, line)

    help_win.noutrefresh()


    #
    # Update edit box. We do this last since it makes it handy to position the
    # cursor.
    #

    edit_box.erase()

    _draw_frame(edit_box, "Jump to symbol/choice/menu/comment")

    # Draw arrows pointing up if the symbol list is scrolled down
    if scroll > 0:
        # TODO: Bit ugly that _style["frame"] is repeated here
        _safe_hline(edit_box, 2, 4, curses.ACS_UARROW, _N_SCROLL_ARROWS,
                    _style["frame"])

    visible_s = s[hscroll:hscroll + edit_width]
    _safe_addstr(edit_box, 1, 1, visible_s)

    _safe_move(edit_box, 1, 1 + s_i - hscroll)

    edit_box.noutrefresh()


def _info_dialog(node, from_jump_to_dialog):
    # Shows a fullscreen window with information about 'node'.
    #
    # If 'from_jump_to_dialog' is True, the information dialog was opened from
    # within the jump-to-dialog. In this case, we make '/' from within the
    # information dialog just return, to avoid a confusing recursive invocation
    # of the jump-to-dialog.

    # Top row, with title and arrows point up
    top_line_win = _styled_win("separator")

    # Text display
    text_win = _styled_win("text")
    text_win.keypad(True)

    # Bottom separator, with arrows pointing down
    bot_sep_win = _styled_win("separator")

    # Help window with keys at the bottom
    help_win = _styled_win("help")

    # Give windows their initial size
    _resize_info_dialog(top_line_win, text_win, bot_sep_win, help_win)


    # Get lines of help text
    lines = _info_str(node).split("\n")

    # Index of first row in 'lines' to show
    scroll = 0

    while True:
        _draw_info_dialog(node, lines, scroll, top_line_win, text_win,
                          bot_sep_win, help_win)
        curses.doupdate()


        c = _getch_compat(text_win)

        if c == curses.KEY_RESIZE:
            _resize_info_dialog(top_line_win, text_win, bot_sep_win, help_win)

        elif c in (curses.KEY_DOWN, "j", "J"):
            if scroll < _max_scroll(lines, text_win):
                scroll += 1

        elif c in (curses.KEY_NPAGE, "\x04"):  # Page Down/Ctrl-D
            scroll = min(scroll + _PG_JUMP, _max_scroll(lines, text_win))

        elif c in (curses.KEY_PPAGE, "\x15"):  # Page Up/Ctrl-U
            scroll = max(scroll - _PG_JUMP, 0)

        elif c in (curses.KEY_END, "G"):
            scroll = _max_scroll(lines, text_win)

        elif c in (curses.KEY_HOME, "g"):
            scroll = 0

        elif c in (curses.KEY_UP, "k", "K"):
            if scroll > 0:
                scroll -= 1

        elif c == "/":
            # Support starting a search from within the information dialog

            if from_jump_to_dialog:
                # Avoid recursion
                return

            if _jump_to_dialog():
                # Jumped to a symbol. Cancel the information dialog.
                return

            # Stay in the information dialog if the jump-to dialog was
            # canceled. Resize it in case the terminal was resized while the
            # fullscreen jump-to dialog was open.
            _resize_info_dialog(top_line_win, text_win, bot_sep_win, help_win)

        elif c in (curses.KEY_LEFT, curses.KEY_BACKSPACE, _ERASE_CHAR,
                   "\x1B",  # \x1B = ESC
                   "q", "Q", "h", "H"):

            return


def _resize_info_dialog(top_line_win, text_win, bot_sep_win, help_win):
    # Resizes the info dialog to fill the terminal

    screen_height, screen_width = _stdscr.getmaxyx()

    top_line_win.resize(1, screen_width)
    bot_sep_win.resize(1, screen_width)

    help_win_height = len(_INFO_HELP_LINES)
    text_win_height = screen_height - help_win_height - 2

    if text_win_height >= 1:
        text_win.resize(text_win_height, screen_width)
        help_win.resize(help_win_height, screen_width)

        text_win.mvwin(1, 0)
        bot_sep_win.mvwin(1 + text_win_height, 0)
        help_win.mvwin(1 + text_win_height + 1, 0)
    else:
        # Degenerate case. Give up on nice rendering and just prevent errors.

        text_win.resize(1, screen_width)
        help_win.resize(1, screen_width)

        for win in text_win, bot_sep_win, help_win:
            win.mvwin(0, 0)


def _draw_info_dialog(node, lines, scroll, top_line_win, text_win,
                      bot_sep_win, help_win):

    text_win_height, text_win_width = text_win.getmaxyx()


    # Note: The top row is deliberately updated last. See _draw_main().

    #
    # Update text display
    #

    text_win.erase()

    for i, line in enumerate(lines[scroll:scroll + text_win_height]):
        _safe_addstr(text_win, i, 0, line)

    text_win.noutrefresh()


    #
    # Update bottom separator line
    #

    bot_sep_win.erase()

    # Draw arrows pointing down if the symbol window is scrolled up
    if scroll < _max_scroll(lines, text_win):
        _safe_hline(bot_sep_win, 0, 4, curses.ACS_DARROW, _N_SCROLL_ARROWS)

    bot_sep_win.noutrefresh()


    #
    # Update help window at bottom
    #

    help_win.erase()

    for i, line in enumerate(_INFO_HELP_LINES):
        _safe_addstr(help_win, i, 0, line)

    help_win.noutrefresh()


    #
    # Update top row
    #

    top_line_win.erase()

    # Draw arrows pointing up if the information window is scrolled down. Draw
    # them before drawing the title, so the title ends up on top for small
    # windows.
    if scroll > 0:
        _safe_hline(top_line_win, 0, 4, curses.ACS_UARROW, _N_SCROLL_ARROWS)

    title = ("Symbol" if isinstance(node.item, Symbol) else
             "Choice" if isinstance(node.item, Choice) else
             "Menu"   if node.item == MENU else
             "Comment") + " information"
    _safe_addstr(top_line_win, 0, max((text_win_width - len(title))//2, 0),
                 title)

    top_line_win.noutrefresh()


def _info_str(node):
    # Returns information about the menu node 'node' as a string.
    #
    # The helper functions are responsible for adding newlines. This allows
    # them to return "" if they don't want to add any output.

    if isinstance(node.item, Symbol):
        sym = node.item

        return (
            _name_info(sym) +
            _prompt_info(sym) +
            "Type: {}\n".format(TYPE_TO_STR[sym.type]) +
            _value_info(sym) +
            _help_info(sym) +
            _direct_dep_info(sym) +
            _defaults_info(sym) +
            _select_imply_info(sym) +
            _kconfig_def_info(sym)
        )

    if isinstance(node.item, Choice):
        choice = node.item

        return (
            _name_info(choice) +
            _prompt_info(choice) +
            "Type: {}\n".format(TYPE_TO_STR[choice.type]) +
            'Mode: {}\n'.format(choice.str_value) +
            _help_info(choice) +
            _choice_syms_info(choice) +
            _direct_dep_info(choice) +
            _defaults_info(choice) +
            _kconfig_def_info(choice)
        )

    # node.item in (MENU, COMMENT)
    return _kconfig_def_info(node)


def _name_info(sc):
    # Returns a string with the name of the symbol/choice. Names are optional
    # for choices.

    return "Name: {}\n".format(sc.name) if sc.name else ""


def _prompt_info(sc):
    # Returns a string listing the prompts of 'sc' (Symbol or Choice)

    s = ""

    for node in sc.nodes:
        if node.prompt:
            s += "Prompt: {}\n".format(node.prompt[0])

    return s


def _value_info(sym):
    # Returns a string showing 'sym's value

    # Only put quotes around the value for string symbols
    return "Value: {}\n".format(
        '"{}"'.format(sym.str_value)
        if sym.orig_type == STRING
        else sym.str_value)


def _choice_syms_info(choice):
    # Returns a string listing the choice symbols in 'choice'. Adds
    # "(selected)" next to the selected one.

    s = "Choice symbols:\n"

    for sym in choice.syms:
        s += "  - " + sym.name
        if sym is choice.selection:
            s += " (selected)"
        s += "\n"

    return s + "\n"


def _help_info(sc):
    # Returns a string with the help text(s) of 'sc' (Symbol or Choice).
    # Symbols and choices defined in multiple locations can have multiple help
    # texts.

    s = "\n"

    for node in sc.nodes:
        if node.help is not None:
            s += "Help:\n\n{}\n\n".format(_indent(node.help, 2))

    return s


def _direct_dep_info(sc):
    # Returns a string describing the direct dependencies of 'sc' (Symbol or
    # Choice). The direct dependencies are the OR of the dependencies from each
    # definition location. The dependencies at each definition location come
    # from 'depends on' and dependencies inherited from parent items.

    return "" if sc.direct_dep is _kconf.y else \
        'Direct dependencies (={}):\n{}\n' \
        .format(TRI_TO_STR[expr_value(sc.direct_dep)],
                _split_expr_info(sc.direct_dep, 2))


def _defaults_info(sc):
    # Returns a string describing the defaults of 'sc' (Symbol or Choice)

    if not sc.defaults:
        return ""

    s = "Default"
    if len(sc.defaults) > 1:
        s += "s"
    s += ":\n"

    for val, cond in sc.orig_defaults:
        s += "  - "
        if isinstance(sc, Symbol):
            s += _expr_str(val)

            # Skip the tristate value hint if the expression is just a single
            # symbol. _expr_str() already shows its value as a string.
            #
            # This also avoids showing the tristate value for string/int/hex
            # defaults, which wouldn't make any sense.
            if isinstance(val, tuple):
                s += '  (={})'.format(TRI_TO_STR[expr_value(val)])
        else:
            # Don't print the value next to the symbol name for choice
            # defaults, as it looks a bit confusing
            s += val.name
        s += "\n"

        if cond is not _kconf.y:
            s += "    Condition (={}):\n{}" \
                 .format(TRI_TO_STR[expr_value(cond)],
                         _split_expr_info(cond, 4))

    return s + "\n"


def _split_expr_info(expr, indent):
    # Returns a string with 'expr' split into its top-level && or || operands,
    # with one operand per line, together with the operand's value. This is
    # usually enough to get something readable for long expressions. A fancier
    # recursive thingy would be possible too.
    #
    # indent:
    #   Number of leading spaces to add before the split expression.

    if len(split_expr(expr, AND)) > 1:
        split_op = AND
        op_str = "&&"
    else:
        split_op = OR
        op_str = "||"

    s = ""
    for i, term in enumerate(split_expr(expr, split_op)):
        s += "{}{} {}".format(indent*" ",
                              "  " if i == 0 else op_str,
                              _expr_str(term))

        # Don't bother showing the value hint if the expression is just a
        # single symbol. _expr_str() already shows its value.
        if isinstance(term, tuple):
            s += "  (={})".format(TRI_TO_STR[expr_value(term)])

        s += "\n"

    return s


def _select_imply_info(sym):
    # Returns a string with information about which symbols 'select' or 'imply'
    # 'sym'. The selecting/implying symbols are grouped according to which
    # value they select/imply 'sym' to (n/m/y).

    def sis(expr, val, title):
        # sis = selects/implies
        sis = [si for si in split_expr(expr, OR) if expr_value(si) == val]
        if not sis:
            return ""

        res = title
        for si in sis:
            res += "  - {}\n".format(split_expr(si, AND)[0].name)
        return res + "\n"

    s = ""

    if sym.rev_dep is not _kconf.n:
        s += sis(sym.rev_dep, 2,
                 "Symbols currently y-selecting this symbol:\n")
        s += sis(sym.rev_dep, 1,
                 "Symbols currently m-selecting this symbol:\n")
        s += sis(sym.rev_dep, 0,
                 "Symbols currently n-selecting this symbol (no effect):\n")

    if sym.weak_rev_dep is not _kconf.n:
        s += sis(sym.weak_rev_dep, 2,
                 "Symbols currently y-implying this symbol:\n")
        s += sis(sym.weak_rev_dep, 1,
                 "Symbols currently m-implying this symbol:\n")
        s += sis(sym.weak_rev_dep, 0,
                 "Symbols currently n-implying this symbol (no effect):\n")

    return s


def _kconfig_def_info(item):
    # Returns a string with the definition of 'item' in Kconfig syntax,
    # together with the definition location(s) and their include and menu paths

    nodes = [item] if isinstance(item, MenuNode) else item.nodes

    s = "Kconfig definition{}, with parent deps. propagated to 'depends on'\n" \
        .format("s" if len(nodes) > 1 else "")
    s += (len(s) - 1)*"="

    for node in nodes:
        s += "\n\n" \
             "At {}:{}\n" \
             "{}" \
             "Menu path: {}\n\n" \
             "{}" \
             .format(node.filename, node.linenr,
                     _include_path_info(node),
                     _menu_path_info(node),
                     _indent(node.custom_str(_name_and_val_str), 2))

    return s


def _include_path_info(node):
    if not node.include_path:
        # In the top-level Kconfig file
        return ""

    return "Included via {}\n".format(
        " -> ".join("{}:{}".format(filename, linenr)
                    for filename, linenr in node.include_path))


def _menu_path_info(node):
    # Returns a string describing the menu path leading up to 'node'

    path = ""

    while node.parent is not _kconf.top_node:
        node = node.parent

        # Promptless choices might appear among the parents. Use
        # standard_sc_expr_str() for them, so that they show up as
        # '<choice (name if any)>'.
        path = " -> " + (node.prompt[0] if node.prompt else
                         standard_sc_expr_str(node.item)) + path

    return "(Top)" + path


def _indent(s, n):
    # Returns 's' with each line indented 'n' spaces. textwrap.indent() is not
    # available in Python 2 (it's 3.3+).

    return "\n".join(n*" " + line for line in s.split("\n"))


def _name_and_val_str(sc):
    # Custom symbol/choice printer that shows symbol values after symbols

    # Show the values of non-constant (non-quoted) symbols that don't look like
    # numbers. Things like 123 are actually symbol references, and only work as
    # expected due to undefined symbols getting their name as their value.
    # Showing the symbol value for those isn't helpful though.
    if isinstance(sc, Symbol) and not sc.is_constant and not _is_num(sc.name):
        if not sc.nodes:
            # Undefined symbol reference
            return "{}(undefined/n)".format(sc.name)

        return '{}(={})'.format(sc.name, sc.str_value)

    # For other items, use the standard format
    return standard_sc_expr_str(sc)


def _expr_str(expr):
    # Custom expression printer that shows symbol values
    return expr_str(expr, _name_and_val_str)


def _styled_win(style):
    # Returns a new curses window with style 'style' and space as the fill
    # character. The initial dimensions are (1, 1), so the window needs to be
    # sized and positioned separately.

    win = curses.newwin(1, 1)
    _set_style(win, style)
    return win


def _set_style(win, style):
    # Changes the style of an existing window

    win.bkgdset(" ", _style[style])


def _max_scroll(lst, win):
    # Assuming 'lst' is a list of items to be displayed in 'win',
    # returns the maximum number of steps 'win' can be scrolled down.
    # We stop scrolling when the bottom item is visible.

    return max(0, len(lst) - _height(win))


def _edit_text(c, s, i, hscroll, width):
    # Implements text editing commands for edit boxes. Takes a character (which
    # could also be e.g. curses.KEY_LEFT) and the edit box state, and returns
    # the new state after the character has been processed.
    #
    # c:
    #   Character from user
    #
    # s:
    #   Current contents of string
    #
    # i:
    #   Current cursor index in string
    #
    # hscroll:
    #   Index in s of the leftmost character in the edit box, for horizontal
    #   scrolling
    #
    # width:
    #   Width in characters of the edit box
    #
    # Return value:
    #   An (s, i, hscroll) tuple for the new state

    if c == curses.KEY_LEFT:
        if i > 0:
            i -= 1

    elif c == curses.KEY_RIGHT:
        if i < len(s):
            i += 1

    elif c in (curses.KEY_HOME, "\x01"):  # \x01 = CTRL-A
        i = 0

    elif c in (curses.KEY_END, "\x05"):  # \x05 = CTRL-E
        i = len(s)

    elif c in (curses.KEY_BACKSPACE, _ERASE_CHAR):
        if i > 0:
            s = s[:i-1] + s[i:]
            i -= 1

    elif c == curses.KEY_DC:
        s = s[:i] + s[i+1:]

    elif c == "\x17":  # \x17 = CTRL-W
        # The \W removes characters like ',' one at a time
        new_i = re.search(r"(?:\w*|\W)\s*$", s[:i]).start()
        s = s[:new_i] + s[i:]
        i = new_i

    elif c == "\x0B":  # \x0B = CTRL-K
        s = s[:i]

    elif c == "\x15":  # \x15 = CTRL-U
        s = s[i:]
        i = 0

    elif isinstance(c, str):
        # Insert character
        s = s[:i] + c + s[i:]
        i += 1

    # Adjust the horizontal scroll so that the cursor never touches the left or
    # right edges of the edit box, except when it's at the beginning or the end
    # of the string
    if i < hscroll + _SCROLL_OFFSET:
        hscroll = max(i - _SCROLL_OFFSET, 0)
    elif i >= hscroll + width - _SCROLL_OFFSET:
        max_scroll = max(len(s) - width + 1, 0)
        hscroll = min(i - width + _SCROLL_OFFSET + 1, max_scroll)


    return s, i, hscroll


def _load_save_info():
    # Returns an information string for load/save dialog boxes

    return "(Relative to {})\n\nRefer to your home directory with ~" \
           .format(os.path.join(os.getcwd(), ""))


def _msg(title, text):
    # Pops up a message dialog that can be dismissed with Space/Enter/ESC

    _key_dialog(title, text, " \n")


def _error(text):
    # Pops up an error dialog that can be dismissed with Space/Enter/ESC

    _msg("Error", text)


def _node_str(node):
    # Returns the complete menu entry text for a menu node.
    #
    # Example return value: "[*] Support for X"

    # Calculate the indent to print the item with by checking how many levels
    # above it the closest 'menuconfig' item is (this includes menus and
    # choices as well as menuconfig symbols)
    indent = 0
    parent = node.parent
    while not parent.is_menuconfig:
        indent += _SUBMENU_INDENT
        parent = parent.parent

    # This approach gives nice alignment for empty string symbols ("()  Foo")
    s = "{:{}}".format(_value_str(node), 3 + indent)

    if _should_show_name(node):
        if isinstance(node.item, Symbol):
            s += " <{}>".format(node.item.name)
        else:
            # For choices, use standard_sc_expr_str(). That way they show up as
            # '<choice (name if any)>'.
            s += " " + standard_sc_expr_str(node.item)

    if node.prompt:
        if node.item == COMMENT:
            s += " *** {} ***".format(node.prompt[0])
        else:
            s += " " + node.prompt[0]

        if isinstance(node.item, Symbol):
            sym = node.item

            # Print "(NEW)" next to symbols without a user value (from e.g. a
            # .config), but skip it for choice symbols in choices in y mode,
            # and for symbols of UNKNOWN type (which generate a warning though)
            if sym.user_value is None and sym.orig_type and \
               not (sym.choice and sym.choice.tri_value == 2):

                s += " (NEW)"

    if isinstance(node.item, Choice) and node.item.tri_value == 2:
        # Print the prompt of the selected symbol after the choice for
        # choices in y mode
        sym = node.item.selection
        if sym:
            for sym_node in sym.nodes:
                # Use the prompt used at this choice location, in case the
                # choice symbol is defined in multiple locations
                if sym_node.parent is node and sym_node.prompt:
                    s += " ({})".format(sym_node.prompt[0])
                    break
            else:
                # If the symbol isn't defined at this choice location, then
                # just use whatever prompt we can find for it
                for sym_node in sym.nodes:
                    if sym_node.prompt:
                        s += " ({})".format(sym_node.prompt[0])
                        break

    # Print "--->" next to nodes that have menus that can potentially be
    # entered. Print "----" if the menu is empty. We don't allow those to be
    # entered.
    if node.is_menuconfig:
        s += "  --->" if _shown_nodes(node) else "  ----"

    return s


def _should_show_name(node):
    # Returns True if 'node' is a symbol or choice whose name should shown (if
    # any, as names are optional for choices)

    # The 'not node.prompt' case only hits in show-all mode, for promptless
    # symbols and choices
    return not node.prompt or \
           (_show_name and isinstance(node.item, (Symbol, Choice)))


def _value_str(node):
    # Returns the value part ("[*]", "<M>", "(foo)" etc.) of a menu node

    item = node.item

    if item in (MENU, COMMENT):
        return ""

    # Wouldn't normally happen, and generates a warning
    if not item.orig_type:
        return ""

    if item.orig_type in (STRING, INT, HEX):
        return "({})".format(item.str_value)

    # BOOL or TRISTATE

    if _is_y_mode_choice_sym(item):
        return "(X)" if item.choice.selection is item else "( )"

    tri_val_str = (" ", "M", "*")[item.tri_value]

    if len(item.assignable) <= 1:
        # Pinned to a single value
        return "" if isinstance(item, Choice) else "-{}-".format(tri_val_str)

    if item.type == BOOL:
        return "[{}]".format(tri_val_str)

    # item.type == TRISTATE
    if item.assignable == (1, 2):
        return "{{{}}}".format(tri_val_str)  # {M}/{*}
    return "<{}>".format(tri_val_str)


def _is_y_mode_choice_sym(item):
    # The choice mode is an upper bound on the visibility of choice symbols, so
    # we can check the choice symbols' own visibility to see if the choice is
    # in y mode
    return isinstance(item, Symbol) and item.choice and item.visibility == 2


def _check_valid(sym, s):
    # Returns True if the string 's' is a well-formed value for 'sym'.
    # Otherwise, displays an error and returns False.

    if sym.orig_type not in (INT, HEX):
        # Anything goes for non-int/hex symbols
        return True

    base = 10 if sym.orig_type == INT else 16
    try:
        int(s, base)
    except ValueError:
        _error("'{}' is a malformed {} value"
               .format(s, TYPE_TO_STR[sym.orig_type]))
        return False

    for low_sym, high_sym, cond in sym.ranges:
        if expr_value(cond):
            low_s = low_sym.str_value
            high_s = high_sym.str_value

            if not int(low_s, base) <= int(s, base) <= int(high_s, base):
                _error("{} is outside the range {}-{}"
                       .format(s, low_s, high_s))
                return False

            break

    return True


def _range_info(sym):
    # Returns a string with information about the valid range for the symbol
    # 'sym', or None if 'sym' doesn't have a range

    if sym.orig_type in (INT, HEX):
        for low, high, cond in sym.ranges:
            if expr_value(cond):
                return "Range: {}-{}".format(low.str_value, high.str_value)

    return None


def _is_num(name):
    # Heuristic to see if a symbol name looks like a number, for nicer output
    # when printing expressions. Things like 16 are actually symbol names, only
    # they get their name as their value when the symbol is undefined.

    try:
        int(name)
    except ValueError:
        if not name.startswith(("0x", "0X")):
            return False

        try:
            int(name, 16)
        except ValueError:
            return False

    return True


def _getch_compat(win):
    # Uses get_wch() if available (Python 3.3+) and getch() otherwise. Also
    # handles a PDCurses resizing quirk.

    if hasattr(win, "get_wch"):
        c = win.get_wch()
    else:
        c = win.getch()
        if 0 <= c <= 255:
            c = chr(c)

    # Decent resizing behavior on PDCurses requires calling resize_term(0, 0)
    # after receiving KEY_RESIZE, while ncurses (usually) handles terminal
    # resizing automatically in get(_w)ch() (see the end of the
    # resizeterm(3NCURSES) man page).
    #
    # resize_term(0, 0) reliably fails and does nothing on ncurses, so this
    # hack gives ncurses/PDCurses compatibility for resizing. I don't know
    # whether it would cause trouble for other implementations.
    if c == curses.KEY_RESIZE:
        try:
            curses.resize_term(0, 0)
        except curses.error:
            pass

    return c


def _warn(*args):
    # Temporarily returns from curses to shell mode and prints a warning to
    # stderr. The warning would get lost in curses mode.
    curses.endwin()
    print("menuconfig warning: ", end="", file=sys.stderr)
    print(*args, file=sys.stderr)
    curses.doupdate()


# Ignore exceptions from some functions that might fail, e.g. for small
# windows. They usually do reasonable things anyway.


def _safe_curs_set(visibility):
    try:
        curses.curs_set(visibility)
    except curses.error:
        pass


def _safe_addstr(win, *args):
    # Clip the line to avoid wrapping to the next line, which looks glitchy.
    # addchstr() would do it for us, but it's not available in the 'curses'
    # module.

    attr = None
    if isinstance(args[0], str):
        y, x = win.getyx()
        s = args[0]
        if len(args) == 2:
            attr = args[1]
    else:
        y, x, s = args[:3]
        if len(args) == 4:
            attr = args[3]

    maxlen = _width(win) - x
    s = s.expandtabs()

    try:
        # The 'curses' module uses wattr_set() internally if you pass 'attr',
        # overwriting the background style, so setting 'attr' to 0 in the first
        # case won't do the right thing
        if attr is None:
            win.addnstr(y, x, s, maxlen)
        else:
            win.addnstr(y, x, s, maxlen, attr)
    except curses.error:
        pass


def _safe_addch(win, *args):
    try:
        win.addch(*args)
    except curses.error:
        pass


def _safe_hline(win, *args):
    try:
        win.hline(*args)
    except curses.error:
        pass


def _safe_vline(win, *args):
    try:
        win.vline(*args)
    except curses.error:
        pass


def _safe_move(win, *args):
    try:
        win.move(*args)
    except curses.error:
        pass


def _change_c_lc_ctype_to_utf8():
    # See _CHANGE_C_LC_CTYPE_TO_UTF8

    if _IS_WINDOWS:
        # Windows rarely has issues here, and the PEP 538 implementation avoids
        # changing the locale on it. None of the UTF-8 locales below were
        # supported from some quick testing either. Play it safe.
        return

    def try_set_locale(loc):
        try:
            locale.setlocale(locale.LC_CTYPE, loc)
            return True
        except locale.Error:
            return False

    # Is LC_CTYPE set to the C locale?
    if locale.setlocale(locale.LC_CTYPE) == "C":
        # This list was taken from the PEP 538 implementation in the CPython
        # code, in Python/pylifecycle.c
        for loc in "C.UTF-8", "C.utf8", "UTF-8":
            if try_set_locale(loc):
                # LC_CTYPE successfully changed
                return


# Are we running on Windows?
_IS_WINDOWS = os.name == "nt"

if __name__ == "__main__":
    _main()
