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

import sys

from pem.emulator import Emulator

def main(argv=sys.argv[1:]):
    """
    Command line entry point.
    """
    # pylint: disable=invalid-name
    emulator = Emulator()
    # pylint: enable=invalid-name
    emulator.main()

if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
