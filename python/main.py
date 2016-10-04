#
# Copyright (c) 2015, Intel Corporation.
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
Main entry point for the emulator.
"""

import sys

from pem.emulator import Emulator

def main(argv=None):
    """
    Entry point for calling from other python code.
    """
    # In case of command line, the setuptools stub will
    # not pass anything, thus leaving untouched the original
    # values of sys.argv
    #
    # In case of use as library, the caller can pass the
    # desired values for the parameters.
    if argv is not None:
        backup_argv = sys.argv
        sys.argv = argv
    # pylint: disable=invalid-name
    emulator = Emulator()
    # pylint: enable=invalid-name
    emulator.main()
    if argv is not None:
        sys.argv = backup_argv


if __name__ == "__main__":
    sys.exit(main())
