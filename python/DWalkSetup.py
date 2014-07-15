#!/usr/bin/env python
# encoding: utf-8
"""
DWalkSetup.py

Created by Kevin Françoisse on 2009-03-27.

Launcher : python DWalkSetup.py build
"""
# System imports
from distutils.core import *
from distutils      import sysconfig

# Third-party modules - we depend on numpy for everything
import numpy

# Obtain the numpy include directory.  This logic works across numpy versions.
try:
    numpy_include = numpy.get_include()
except AttributeError:
    numpy_include = numpy.get_numpy_include()

# dwalk extension module
_pyDWalk = Extension("_pyDWalk",
                   ["pyDWalk.c","pyDWalk.i","../src/dwalk.c", "../src/tools.c", "../src/memory.c","../src/files.c","../src/matrix.c","../src/crossval.c","../src/perfmeasure.c","../src/textdisplay.c","../src/graphStat.c","../src/similarity.c"],
                   include_dirs = ['../src', numpy_include, '../include', '../DWalks/include', '../external_Libraries/include'],
                   )

# NumyTypemapTests setup
setup(name        = "dwalk Function",
      description = "Function that perform a D-Walk on a graph and output an array of classes betweeness",
      author      = "Kevin Françoisse",
      py_modules  = ["pyDWalk"],
      ext_modules = [_pyDWalk]
      )
