import pybind11
from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext

extension = Extension(
    "my_string",
    sources=[
        "main.cpp",
        "MyString.cpp",
    ],
    include_dirs=[pybind11.get_include()],
    language="c++",
    extra_compile_args=[
        "-std=c++17",
        "-fexceptions",
        "-Wa,-mbig-obj",
    ],
)

setup(
    name="my_string",
    version="1.0",
    ext_modules=[extension],
    cmdclass={"build_ext": build_ext},
)