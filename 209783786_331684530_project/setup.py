from setuptools import setup, Extension

symnmf_module = Extension(
    'symnmf',
    sources=['symnmfmodule.c', 'symnmf.c'],
    include_dirs=['.'],
    extra_compile_args=['-ansi', '-Wall', '-Wextra', '-Werror', '-pedantic-errors']
)

setup(
    name='symnmf',
    version='1.0',
    description='Python-C extension for Symmetric Non-negative Matrix Factorization',
    ext_modules=[symnmf_module],
)
