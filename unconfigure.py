#!python3

from jinx import *

if exists("__pycache__"):
    rmtree("__pycache__")

if exists("build"):
    rmtree("build")

if exists("thirdparty"):
    rmtree("thirdparty")
