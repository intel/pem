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
Support class providing buffers with dual-access mode;
by byte and by field name.
"""

from struct import Struct
import binascii

BYTE_ORDER_NATIVE_ALIGNED = "@"
BYTE_ORDER_NATIVE_UNALIGNED = "="
BYTE_ORDER_LITTLE_ENDIAN = "<"
BYTE_ORDER_BIG_ENDIAN = ">"
BYTE_ORDER_NETWORK = "!"


class MappedBuffer(object):
    """
    Helper class: it's meant for creating specific data buffers,
    with fields of defined length and name.
    Each field from the buffer will be accessible as field of the class.
    """
    # pylint: disable=too-many-instance-attributes
    def __init__(self, fields=(), byte_order=BYTE_ORDER_LITTLE_ENDIAN):
        """
        The init function expects a tuple of 1-entry dictionaries,
        where the key is the name of the field and the value is the
        representation of the field, according to the standars for the
        struct.Struct class.
        """
        self._fields = fields
        format_string = byte_order
        for field in self._fields:
            field_formatter = list(field.values())[0]
            format_string += field_formatter
            if field_formatter is not 'x':  # Only if it's not padding
                # pylint: disable=exec-used
                exec("self." + list(field.keys())[0] + " = 0")
                # pylint: enable=exec-used
        self._buffer = Struct(format_string)
        self.buffer_length = len(self.pack())
        self.buffer_checksum = 0

    @staticmethod
    def calculate_checksum(packed_data):
        """
        Calculates the checksum (over 1 byte) of a given array.
        """
        _sum = 0
        for char in packed_data:
            if type(char) is int:
                _sum = _sum + char
            else:
                _sum = _sum + ord(char)

        return _sum % 256

    def pack(self):
        """
        Returns a string composed by all the parts of the message.
        """
        values = []
        for field in self._fields:
            if list(field.values())[0] is not "x":  # Only if it's not padding
                # pylint: disable=eval-used
                values.append(eval("self." + list(field.keys())[0]))
                # pylint: enable=eval-used
        tupl = tuple(values)
        # pylint: disable=star-args
        return self._buffer.pack(*tupl)
        # pylint: enable=star-args

    def unpack_from(self, packed_data, offset=0):
        """
        Reads a string with binary data and updates the corresponding fields.
        Starts from an arbitrary position.
        """
        retval = self._buffer.unpack_from(buffer=packed_data.encode(), offset=offset)
        index = 0
        for field in self._fields:
            # pylint: disable=exec-used
            exec("self." + list(field.keys())[0] + " = " + str(retval[index]))
            # pylint: enable=exec-used
            index += 1
        self.buffer_checksum = self.calculate_checksum(packed_data)
        return retval

    def unpack(self, packed_data):
        """
        Reads a string with binary data and updates the corresponding fields.
        Starts from  the first character.
        """
        return self.unpack_from(packed_data=packed_data, offset=0)

    def hexlify(self):
        """
        Converts a buffer into its hex representation.
        """
        return binascii.hexlify(self.pack())
