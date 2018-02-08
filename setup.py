from distutils.core import setup, Extension
import os
import distutils.ccompiler as cc
import numpy

import pip

mkl_root_dir = os.environ.get('MKLROOT')

if cc.get_default_compiler() is 'msvc':
    cflags = ['/Ox']
else:
    cflags = ['-O3']

matlab_random = Extension('matlab_random', sources=['py_matlab_random.cpp', 'matlab_random.cpp'],
                          include_dirs=[numpy.get_include()],
                          extra_compile_args=cflags,
                          language='c++')


modules = [matlab_random]

reqs = list(pip.req.parse_requirements(os.path.join(os.path.dirname(__file__), "requirements.txt"), session="hack"))

req_specs = [req.name for req in reqs]

setup(name='random_number',
      version='0.0.2',
      ext_modules=modules,
      requires=req_specs)
