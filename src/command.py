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
Command template.
"""

# Structure of a generic command
# The command doesn't require integrity verification
# because that part is provided by the "message"
# which acts as container for commands.
# [0]    Command length (N+1)
#        Can be used to skip unrecognized commands.
# [1]    Command ID
# [2..N] Optional Command-specific data

import logging

from pem.mappedbuffer import MappedBuffer

CMD_PING = 0

class Command(object):
    """
    Template of generic command.
    """
    # pylint: disable=too-few-public-methods
    _ranges = ()

    def __init__(self, id_range):
        self._id_range = id_range
        self._header = MappedBuffer(fields=(
            {"length": "B"},
            {"command_id": "B"},
        ))

    def _add_range(self, new_range):
        """
        Checks that the range is both formally correct and
        doesn't overalp with existing ones.
        """
        try:
            if (len(new_range) is 2 and 0 <= new_range[0] and
                    new_range[0] <= new_range[1] and new_range[1] < 255) \
               is not True:
                return False
            for registered_range in self._ranges:
                if (registered_range[0] <= new_range[0] and
                        new_range[0] <= registered_range[1]) or \
                    (registered_range[0] <= new_range[1] and
                     new_range[1] <= registered_range[1]):
                    return False
        except TypeError as exception:
            logging.error("Exception {0}\n".format(exception) +
                          "while trying to register exception range: {0}".
                          format(new_range))
            return False

    def pack(self, command_id=CMD_PING, data=""):
        """
        To be used only by derived classes.
        Returns the compacted message.
        """
        self._header.length = \
            self._header.buffer_length + \
            len(data)
        self._header.command_id = command_id
        return self._header.pack() + data
    # pylint: enable=too-few-public-methods
