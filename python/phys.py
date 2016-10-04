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
Abstraction of the physical connection between
PC and Arduino.
The actual connection object will derive from this and provide
the implementation for the communication methods.
"""
import sys
import abc
import binascii
import datetime
import logging
from six import with_metaclass

try:
    from mappedbuffer import MappedBuffer
except ImportError:
    from .mappedbuffer import MappedBuffer

# Structure of the message, as seen by the phys layer
#
# Byte  Meaning
# [0]   Message signature - 1st half (0xAA)
# [1]   Message signature - 2nd half (0x55)
# [2]   Message length (N + 1), minimum is 5
# [3]   Message id - it will eventually rollover
# ...   Payload (optional, for a ping message)
# [N]   Checksum of the previous bytes


class Phys(with_metaclass(abc.ABCMeta, object)):
    """
    Physical Connection Object: interacts with the Arduino device.
    """
    _imports = {}
    _arguments = {}
    _message_id = 0
    _SIGNATURE = ['\xaa', '\x55']

    def __init__(self):
        """
        Abstraction of the HW layer.
        """
        self._signature = MappedBuffer(fields=(
            {"first_byte": "B"},
            {"second_byte": "B"},
        ))
        self._signature.first_byte = ord(self._SIGNATURE[0])
        self._signature.second_byte = ord(self._SIGNATURE[1])
        self._header = MappedBuffer(fields=(
            {"message_len": "B"},
            {"message_id": "B"},
        ))
        self._payload = []
        self._checksum = MappedBuffer(fields=(
            {"checksum": "B"},
        ))
        self._deadline = None

    def new(self):
        """
        Prepares for transmitting a new message.
        """
        self._payload = []
        self._message_id += 1

    @classmethod
    def prepare_arguments(cls):
        """
        Function providing a list of arguments configurations.
        Each entry will be loaded into ArgumentParser.add_argument
        The default dictionary - cls._arguments - is empty.
        """
        return list(cls._arguments.values()), cls._imports

    @abc.abstractmethod
    def _test_connection(self):
        """
        Checks that the connection is established and functioning.
        """

    @abc.abstractmethod
    def _chars_available(self):
        """
        Returns the number of charaters that can be read.
        """

    @abc.abstractmethod
    def _read_char(self):
        """
        Read a char from the physical layer.
        """

    @abc.abstractmethod
    def _write_char(self, character):
        """
        Write a char to the physical layer.
        """

    @abc.abstractmethod
    def _flush(self):
        """
        Waits untill all the data is written.
        """

    @abc.abstractmethod
    def close(self):
        """
        Terminates the connection.
        """

    def _calculate_deadline(self, timeout_ms):
        """
        Defines the deadline for the completion of the current
        TX or RX operation.
        """
        self._deadline = datetime.datetime.now() + \
            datetime.timedelta(milliseconds=timeout_ms)

    def _read_char_with_timeout(self):
        """
        As the name suggests, either a chracter is received from the Arduino,
        or the function will wait till the timeout expires.
        """
        while datetime.datetime.now() < self._deadline:
            if self._chars_available() > 0:
                return self._read_char()
        return False

    def _hook_message_signature(self):
        """
        Tries to identify the message signature, till
        either it finds it, or the allotted time runs out.
        """
        while True:
            new_char = self._read_char_with_timeout()
            while new_char == self._SIGNATURE[0]:
                new_char = self._read_char_with_timeout()
                if new_char == self._SIGNATURE[1]:
                    return True
            if new_char is False:
                return False

    def _receive_buffer(self, size):
        """
        Reads and stores "size"  characters.
        """
        receive_buffer = []
        while datetime.datetime.now() < self._deadline:
            if self._chars_available() > 0:
                receive_buffer.append(self._read_char())
                if len(receive_buffer) == size:
                    return receive_buffer
        logging.info("phys._receive_buffer timeout")
        return []

    def receive_message(self, timeout_ms=500):
        """
        Receives a message and stops either when the message is
        complete or when the allotted time expires, whichever
        comes first.
        """
        self._calculate_deadline(timeout_ms=timeout_ms)
        if self._hook_message_signature() is False:
            logging.info("Failed to obtain message signature "
                         "within allotted time")
            return False
        logging.debug("Got message signature.")
        header = self._receive_buffer(size=self._header.buffer_length)
        if header == []:
            logging.info("Failed to obtain message header "
                         "within allotted time")
            return False
        logging.debug("Got message header.")
        self._header.unpack(packed_data="".join(header))
        payload_len = \
            self._header.message_len - \
            self._signature.buffer_length - \
            self._header.buffer_length - \
            self._checksum.buffer_length
        if payload_len is not 0:
            self._payload = self._receive_buffer(size=payload_len)
            if self._payload == []:
                logging.info("Failed to obtain message payload "
                             "within allotted time")
                return False
            logging.debug("Got message payload.")
            for _ in self._payload:
                logging.debug("payload {0}".format(binascii.hexlify(_)))
        else:
            logging.debug("Got empty message without payload.")
            self._payload = []
        checksum = self._receive_buffer(size=self._checksum.buffer_length)
        if checksum == []:
            logging.info("Failed to obtain message checksum "
                         "within allotted time")
            return False
        logging.debug("Got message checksum.")
        self._checksum.unpack(packed_data="".join(checksum))
        calculated_checksum = \
            MappedBuffer.calculate_checksum(
                [self._SIGNATURE[0], self._SIGNATURE[1],
                 self._header.buffer_checksum,
                 MappedBuffer.calculate_checksum(self._payload)])
        if self._checksum.checksum == calculated_checksum:
            logging.info("RX: id = {0}\t\tpayload = {1}".
                         format(self._header.message_id, self._payload))
            logging.debug("Checksum verification successful.")
            return True
        else:
            logging.info("Checksum verification failed:\n" +
                         "Received = {0}\n".format(self._checksum.checksum) +
                         "Calculated = {0}".format(calculated_checksum))
            return False

    def transmit_message(self, payload=""):
        """
        Transmits a message
        """
        self._message_id = (self._message_id + 1) % 256
        self._header.message_len = \
            self._signature.buffer_length + \
            self._header.buffer_length + \
            len(payload) + \
            self._checksum.buffer_length
        self._header.message_id = self._message_id
        message = self._signature.pack() + self._header.pack() + payload
        self._checksum.checksum = MappedBuffer.calculate_checksum(message)
        message = message + self._checksum.pack()
        logging.info("TX {0}".format(binascii.hexlify(message)))
        for character in message:
            #check for python version
            if sys.version_info[0] == 2:
                self._write_char(character)
            elif sys.version_info[0] == 3:
                self._write_char(chr(character).encode("Latin-1"))
        self._flush()

    def ping(self):
        """
        Sends an empty message to the device and waits for an ACK
        to arrive before the timeout.
        """
        if self._test_connection() is False:
            return False
        self.transmit_message()
        return self.receive_message()
    # pylint: enable=abstract-class-not-used
