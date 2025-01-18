from setuptools import Extension, setup

module = Extension("mykmeanssp", sources=['kmeansmodule.c'])
setup(name='kmeansmodule.c',
     version='1.0',
     description='Module to apply k-means algorithm',
     ext_modules=[module])