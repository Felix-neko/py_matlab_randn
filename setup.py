import os
from setuptools import setup, Extension
from distutils.command.build_ext import build_ext

import numpy


class build_ext_subclass( build_ext ):
    def build_extensions(self):
        compiler_type_string = self.compiler.compiler_type
        if compiler_type_string == "msvc":
            compiler_option = '/Ox'
        else:
            compiler_option = '-O3'
        for e in self.extensions:
            e.extra_compile_args = [compiler_option]
        build_ext.build_extensions(self)


matlab_random = Extension('matlab_random', sources=['py_matlab_random.cpp', 'matlab_random.cpp'],
                          include_dirs=[numpy.get_include()],
                          language='c++')


modules = [matlab_random]

reqs = open(os.path.join(os.path.dirname(__file__), "requirements.txt"), "r").readlines()


setup(name='random_number',
      version='0.0.3',
      ext_modules=modules,
      instasll_requires=reqs,
      cmdclass={'build_ext': build_ext_subclass})
