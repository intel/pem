#!/usr/bin/env python
# vim: ai ts=4 sts=4 et sw=4
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

"""Setup for Peripherals Emulator Tool."""

from setuptools import setup
import os

PROJECT_NAME = "pem"
DATA_PATH = os.path.join("share", PROJECT_NAME)
DATA_ARDUINO_SRC_PATH = os.path.join(DATA_PATH, "pem_Arduino")
DATA_DOCS_PATH = os.path.join(DATA_PATH, "docs")

setup(name=PROJECT_NAME,
      version="0.1.3",
      description="Peripherals Emulator",
      author="Igor Stoppa",
      author_email="igor.stoppa@intel.com",
      package_dir={PROJECT_NAME: "src"},
      packages=[PROJECT_NAME, ],
      data_files=[(DATA_PATH, []),
                  (DATA_ARDUINO_SRC_PATH, []),
                  (DATA_DOCS_PATH, []),
                 ],
      include_package_data=True,
      entry_points={'console_scripts': ['pem = pem.main:main'],},
     )
