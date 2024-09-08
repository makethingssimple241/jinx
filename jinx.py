from os import *
from os.path import *
from shutil import *

def check(exit_code):
    if (exit_code != 0):
        exit(1)
