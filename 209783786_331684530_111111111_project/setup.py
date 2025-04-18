from setuptools import setup, Extension
import numpy as np

symnmf_module = Extension(
    'symnmf',
    sources=['symnmfmodule.c', 'symnmf.c'],
    include_dirs=[np.get_include()] # need it for processing the numpy array given
)

setup(
    name='symnmf',
    version='1.0',
    description='Python-C extension for Symmetric Non-negative Matrix Factorization',
    ext_modules=[symnmf_module],
)