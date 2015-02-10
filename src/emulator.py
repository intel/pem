#
# Copyright (c) 2014, 2015, Intel Corporation.
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
Peripheral Emulator
"""

import time
import logging
import argparse
# pylint: disable=unused-import
# importlib is used within an exec
import importlib
# pylint: enable=unused-import

import json

from pem.inputrecorder import start_ui
from pem.inputrecorder import wait_for_ui
from pem.serialconnection import SerialConnection
from pem.keyboard import Keyboard

# pylint: disable=eval-used


class Emulator(object):
    """
    Peripheral Emulator.
    Provides recording and playback functionality for a set of devices.
    """

    # pylint: disable=too-few-public-methods
    _args = None
    _phys = None
    _parser = None
    _in_queue = None
    _MAX_RETRANSMISSIONS = 5

    @classmethod
    def _parse_basic_args(cls):
        """
        Handles all the basic command line arguments.
        """
        interfaces = ["serialconnection"]
        cls._parser = argparse.ArgumentParser(
            description="Peripherals Emulator.")
        cls._parser.add_argument(
            "--interface", dest="interface", required=True, choices=interfaces,
            action="store", help="Type of interface for the emulator.")
        group = cls._parser.add_mutually_exclusive_group(required=True)
        group.add_argument("--record", dest="data_sink",
                           default=None, action="store",
                           help="File where to store the recorded sequence.")
        group.add_argument("--playback", dest="data_source",
                           default=None, action="store",
                           help="File containing the pre-recorded sequence.")
        cls._args, _ = cls._parser.parse_known_args()

    @classmethod
    def _parse_phys_args(cls, arguments, imports):
        """
        Handles the command line arguments related to the selected interface.
        """
        for imp in imports:
            # pylint: disable=exec-used
            exec(imp + " = importlib.import_module('" + imp + "')")
            # pylint: enable=exec-used
        for arg_definition in arguments:
            eval("cls._parser.add_argument(" + arg_definition + ")")
        cls._args = cls._parser.parse_args()

    @classmethod
    def _transmit_with_retry(cls, payload):
        """
        Sends data to the emulator, retrying each message up to
        _MAX_RETRANSMISSION times.
        """
        retransmissions = 0
        while True:
            cls._phys.transmit_message(payload)
            if cls._phys.receive_message(timeout_ms=200) is False:
                logging.critical("Receive Error")
                retransmissions += 1
                if retransmissions == cls._MAX_RETRANSMISSIONS:
                    logging.critical("Too many retransmissions.")
                    return False
            else:
                return True

    @classmethod
    def _record(cls):
        """
        Reads data from the selected input device and stores it to a file.
        The data is also forwarded to the emulator.
        """
        # pylint: disable=too-many-branches
        def keyboard_formatter(timestamp, buf):
            """
            Handler for recording the events.
            """
            return {"type": "keyboard", "timestamp": timestamp,
                    "buffer": buf}

        def control(command):
            """
            Handler for passing commands.
            """
            return {"type": "control", "command": command}

        keyboard = Keyboard()
        handlers = {"keyboard": keyboard_formatter,
                    "control": control}
        cls._in_queue = start_ui(handlers)
        while True:
            if cls._in_queue.empty() is False:
                data = cls._in_queue.get()
                if data["type"] == "control":
                    if data["command"] == "Start":
                        output_data = []
                    elif data["command"] in ("Stop", "Quit"):
                        if "output_data" in locals():
                            try:
                                with open(cls._args.data_sink, "wb") as out_f:
                                    for line in output_data:
                                        out_f.write(json.dumps(line) + '\n')
                            except IOError:
                                logging.critical("Cannot save data to {0}."
                                                 .format(cls._args.data_sink))
                                return -1
                        if data["command"] == "Quit":
                            wait_for_ui()
                        return 0
                elif data["type"] == "keyboard":
                    output_data.append(data)
                    payload = keyboard.encode(data)
                    if not cls._transmit_with_retry(payload):
                        return -1
            else:
                if cls._phys.receive_message(timeout_ms=500) is False:
                    logging.critical("Device is dead\n")
        # pylint: enable=too-many-branches
        return 0



    @classmethod
    def _playback(cls):
        """
        Reads data from a file and plays it back on the emulator.
        """
        try:
            with open(cls._args.data_source, "rb") as in_f:
                lines = in_f.readlines()
                recorded_data = []
                for line in lines:
                    recorded_data.append(json.loads(line))
        except IOError:
            logging.critical("Failed to open {0}."
                             .format(cls._args.data_source))
            return -1
        keyboard = Keyboard()
        prev_time = 0
        while not cls._phys.receive_message(timeout_ms=500):
            pass
        for data in recorded_data:
            if data["type"] == "keyboard":
                payload = keyboard.encode(data)
                time.sleep((data["timestamp"] - prev_time) / 1000)
                prev_time = data["timestamp"]
                if not cls._transmit_with_retry(payload):
                    return -1
        return 0

    @classmethod
    def main(cls):
        """
        Main function.
        """
        logging.basicConfig(level=logging.INFO)
        cls._parse_basic_args()
        if cls._args.interface == "serialconnection":
            arguments, imports = SerialConnection.prepare_arguments()
            cls._parse_phys_args(arguments=arguments,
                                 imports=imports)
            cls._phys = SerialConnection(cls._args)
        if cls._args.data_sink is not None:
            return cls._record()
        elif cls._args.data_source is not None:
            return cls._playback()
        else:
            return -1
    # pylint: enable=too-few-public-methods

