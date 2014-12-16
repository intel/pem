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
Serial Port communication for the Peripheral Emulator.
"""

import binascii
import time
import logging

import serial

from phys import Phys

# importlib is used within an exec


class SerialConnection(Phys):
    """
    Serial Connection Object
    """
    # pylint: disable=abstract-class-little-used
    _imports = {"serial"}
    _arguments = {
        "port":
        '"--port", dest="port", default="/dev/ttyUSB2", '
        'action="store", help="Path to the Serial Port device file."',
        "baudrate":
        '"--baudrate", dest="baudrate", default="19200", '
        'action="store", help="Baud rate of the serial port."',
        "bytesize":
        '"--bytesize", dest="bytesize", '
        'default=serial.EIGHTBITS, action="store", '
        'help="Number of bits per byte of the serial port."',
        "parity":
        '"--parity", dest="parity", default=serial.PARITY_NONE, '
        'action="store", help="Parity of the serial port."',
        "stopbits":
        '"--stopbits", dest="stopbits", '
        'default=serial.STOPBITS_ONE, action="store", '
        'help="Parity of the serial port."',
        "timeout":
        '"--timeout", dest="timeout", '
        'default=1, action="store", '
        'help="Use timeout (1) or not (0)."',
    }

    def __init__(self, args):
        """
        Initalizes the connection with the HW emulator.
        """
        # pylint: disable=unused-argument
        super(SerialConnection, self).__init__()
        self._ser = serial.Serial()
        for parm in self._arguments.keys():
            # pylint: disable=exec-used
            exec("self._ser." + parm + " = args." + parm)
        self._ser.xonxoff = False
        self._ser.writeTimeout = 2
        self._ser.open()
        # The following sequence will reset the Arduino
        self._ser.setDTR(True)
        self._ser.setDsrDtr(True)
        self._ser.setRTS(True)
        self._ser.setRtsCts(True)
        time.sleep(0.22)
        self._ser.setDTR(False)
        self._ser.setDsrDtr(False)
        self._ser.setRTS(False)
        self._ser.setRtsCts(False)
        self._ser.flushInput()
        self._ser.flushOutput()
        # Let the Arduino recover from the reset.
        time.sleep(1)
        logging.info("Initialized Serial port {0}".format(self._ser.port))
        # pylint: enable=unused-argument

    def _test_connection(self):
        """
        Checks that the connection is established and functioning.
        """
        return self._ser.isOpen()

    def _chars_available(self):
        """
        Returns the number of charaters that can be read.
        """
        time.sleep(0.01)
        return self._ser.inWaiting()

    def _read_char(self):
        """
        Read a char from the physical layer.
        """
        retval = self._ser.read()
        return retval

    def _write_char(self, character):
        """
        Write a char to the physical layer.
        """
        logging.debug("WRITE {0}".format(binascii.hexlify(character)))
        return self._ser.write(character)

    def _flush(self):
        """
        Waits untill all the data is written.
        """
        return self._ser.flush()

    def close(self):
        """
        Terminates the connection.
        """
        return self._ser.close()
    # pylint: enable=abstract-class-little-used
