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
Command manager for USB Keyboard emulation.
"""

from command import Command
from mappedbuffer import MappedBuffer

CMD_NEW_KEYBOARD_DATA = 1

class Keyboard(Command):
    """
    Keyboard command manager.
    """
    def __init__(self):
        super(Keyboard, self).__init__((CMD_NEW_KEYBOARD_DATA, ))

    def encode(self, data):
        """
        Generates the byte sequence for a USB keyboard command.
        """
        # pylint: disable=exec-used
        # pylint: disable=unused-argument
        buf = MappedBuffer(fields=({"byte_0": "B"},
                                   {"byte_1": "B"},
                                   {"byte_2": "B"},
                                   {"byte_3": "B"},
                                   {"byte_4": "B"},
                                   {"byte_5": "B"},
                                   {"byte_6": "B"},
                                   {"byte_7": "B"},
                                  ))
        for index in range(0, 8):
            exec("buf.byte_" + str(index) +
                 " = data['buffer'][" + str(index) + "]")
        state = CMD_NEW_KEYBOARD_DATA
        return self.pack(command_id=state, data=buf.pack())
        # pylint: enable=exec-used
        # pylint: enable=unused-argument
