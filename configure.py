#!python3

from jinx import *

# Making directories

print("** Making directories")

if not exists("thirdparty"):
    mkdir("thirdparty")

# Download and build Limine bootloader

print("** Downloading limine")

limine_directory = "thirdparty/limine"

if exists(limine_directory):
    print("** Limine already exists")
else:
    limine_remote = "https://github.com/limine-bootloader/limine"
    limine_branch = "v8.0.9-binary"

    check(system(f"cd thirdparty && git clone -b {limine_branch} --single-branch --depth=1 {limine_remote}"))

    print("** Building limine")

    make_with_next_arg_as_cwd = "-C"
    check(system(f"make {make_with_next_arg_as_cwd} {limine_directory} limine"))

# Download PC Screen Font

print("** Downloading font")

font = "thirdparty/font.psfu"

if exists(font):
    print("** Font already exists")
else:
    curl_follow_redirects = "-L"
    font_remote = "https://github.com/legionus/kbd/raw/master/data/consolefonts/default8x16.psfu"
    check(system(f"curl {curl_follow_redirects} {font_remote} > {font}"))
