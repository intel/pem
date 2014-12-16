#
# Copyright (c) 2014, Intel Corporation.
#
# This program is free software; you can redistribute it and/or modify it
# under the terms and conditions of the GNU General Public License,
# version 2, as published by the Free Software Foundation.
#
# This program is distributed in the hope it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
# more details.
#

"""
Class implementing the behavior of an USB keyboard with US layout.
"""

class KeyboardBuffer(object):
    """
    Encoding from python symbols to USB US keyboard.
    """
    # pylint: disable=bad-continuation
    # pylint: disable=bad-whitespace
    _modifiers = {"Control_L": {"bitmask": 0x01, "pressed":False},
                  "Shift_L":   {"bitmask": 0x02, "pressed":False},
                  "Alt_L":     {"bitmask": 0x04, "pressed":False},
                  "Super_L":   {"bitmask": 0x08, "pressed":False},
                  "Multi_key": {"bitmask": 0x08, "pressed":False},
                  "Control_R": {"bitmask": 0x10, "pressed":False},
                  "Shift_R":   {"bitmask": 0x20, "pressed":False},
                  "Menu":      {"bitmask": 0x80, "pressed":False},
                  "ISO_Level3_Shift":
                               {"bitmask": 0x40, "pressed":False},
                  }
    _history = []
    _hid_table = {
        # Function keys
        'F1': 0x3A,  'F2': 0x3B,  'F3': 0x3C,  'F4': 0x3D,
        'F5': 0x3E,  'F6': 0x3F,  'F7': 0x40,  'F8': 0x41,
        'F9': 0x42, 'F10': 0x43, 'F11': 0x44, 'F12': 0x45,

        # Symbols above the row of numbers
        'exclam':     0x1E, 'at':       0x1F, 'numbersign':      0x20,
        'dollar':     0x21, 'percent':  0x22, 'dead_circumflex': 0x23,
        'ampersand':  0x24, 'asterisk': 0x25, 'parenleft':       0x26,
        'parenright': 0x27,

        # Row of numbers
        '1': 0x1E, '2': 0x1F, '3': 0x20, '4': 0x21, '5': 0x22,
        '6': 0x23, '7': 0x24, '8': 0x25, '9': 0x26, '0': 0x27,

        # Navigation/Editing
        'Insert': 0x49, 'Home': 0x4A, 'Prior': 0x4B,
        'Delete': 0x4C, 'End':  0x4D, 'Next':  0x4E,

        'Return': 0x28, 'Escape': 0x29, 'BackSpace': 0x2A,
        'Tab':    0x2B, 'space':  0x2C,

        # Miscellaneous Symbols - grouped by key (one per row)
        'minus':        0x2D, 'underscore': 0x2D,
        'plus':         0x2E, 'equal':      0x2E,
        'bracketleft':  0x2F, 'braceleft':  0x2F,
        'bracketright': 0x30, 'braceright': 0x30,
        'colon':        0x33, 'semicolon':  0x33,
        'apostrophe':   0x34, 'quotedbl':   0x34,
        'dead_grave':   0x35, 'dead_tilde': 0x35,
        'less':         0x36, 'comma':      0x36,
        'greater':      0x37, 'period':     0x37,

        #Arrow Keys
        'Right': 0x4f, 'Left': 0x50, 'Down': 0x51, 'Up': 0x52,
        }
    # pylint: enable=bad-continuation
    # pylint: enable=bad-whitespace

    @classmethod
    def _compose_modifier(cls):
        """
        Compose the modifier byte.
        """
        result = 0
        for key in cls._modifiers.keys():
            result = result | \
                        cls._modifiers[key]["bitmask"] * \
                        cls._modifiers[key]["pressed"]
        return result

    @classmethod
    def _translate(cls, key):
        """
        Maps keys read as events to the codes outputted by a USB keyboard
        with US layout.
        """
        if key in cls._hid_table.keys():
            hex_key = cls._hid_table[key]
        elif 'A' <= key and key <= 'Z':
            hex_key = ord(key) - ord('A') + 0x04
        elif 'a' <= key and key <= 'z':
            hex_key = ord(key) - ord('a') + 0x04
        else:
            hex_key = 0
        return hex_key

    @classmethod
    def press(cls, key):
        """
        Updates the history buffer accordingly to which key was
        pressed. Discard the oldest, in case there are more than 6.
        Modifiers are tracked separately in a bitmap and do not influence
        the count.
        """
        if key in cls._modifiers.keys():
            cls._modifiers[key]["pressed"] = True
            return
        key = cls._translate(key)
        if key is 0 or key in cls._history:
            return
        cls._history.append(key)
        if len(cls._history) > 6:
            cls._history.remove(cls._history[0])

    @classmethod
    def release(cls, key):
        """
        Updates the history buffer accordingly to which key was
        released. Modifiers are tracked separately in a bitmap and do not
        influence the count.
        """
        if key in cls._modifiers.keys():
            cls._modifiers[key]["pressed"] = False
            return
        key = cls._translate(key)
        if key is not 0 and key in cls._history:
            cls._history.remove(key)

    @classmethod
    def get_buffer(cls):
        """
        Returns the buffer to write out.
        """
        return [cls._compose_modifier(), 0] + \
               cls._history + [0] * (6 - len(cls._history))
