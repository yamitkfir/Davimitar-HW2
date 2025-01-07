from setuptools import Extension, setup

module = Extension("kmeansmodule", sources=['kmeansmodule.c'])
setup(name='kmeansmodule',
     version='1.0',
     description='Python wrapper for custom C extension',
     ext_modules=[module])