#!python3

from jinx import *

CC = "clang"
LD = "x86_64-elf-ld"
AS = "nasm"
OBJCOPY = "x86_64-elf-objcopy"

arch = "x86_64"
sub = ""
vendor = "unknown"
sys = "none"
env = "elf"
CFLAGS = f"-c -ffreestanding -g -I src/ -I src/libs/ -I thirdparty/limine/ --target={arch}{sub}-{vendor}-{sys}-{env} -fPIC -mno-red-zone"

LFLAGS = "-m elf_x86_64"
AFLAGS = "-f elf64"

c = {
    "src/arch/x86_64/drivers/pic.c": "build/drivers/pic.o",
    "src/arch/x86_64/kernel/arch.c": "build/kernel/arch.c.o",
    "src/arch/x86_64/kernel/idt.c": "build/kernel/idt.o",
    "src/arch/x86_64/kernel/interrupts.c": "build/kernel/interrupts.o",
    "src/drivers/serial.c": "build/drivers/serial.o",
    "src/drivers/gop.c": "build/drivers/gop.o",
    "src/kernel/kernel.c": "build/kernel/kernel.o",
    "src/libs/core/memory.c": "build/libs/core/memory.o",
    "src/libs/core/string.c": "build/libs/core/string.o",
}

a = {
    "src/arch/x86_64/kernel/arch.asm": "build/kernel/arch.asm.o"
}

font_in = "thirdparty/font.psfu"
font_out = "build/font.o"

executable = "build/kernel.elf"
cdrom = "build/jinx.iso"

# Collect object file list

objects = []

for src, obj in c.items():
    objects.append(obj)

for src, obj in a.items():
    objects.append(obj)

objects.append(font_out)

# Make build directories

print("** Making build directories")

build_dirs = ["build/boot", "build/drivers", "build/kernel", "build/libs/core"]
for dir in build_dirs:
    if not exists(dir):
        makedirs(dir)

# Clean build files from previous builds

print("** Cleaning build files")

for obj in objects:
    if exists(obj):
        remove(obj)

if exists(executable):
    remove(executable)

if exists(cdrom):
    remove(cdrom)

# Compile into object files

for src, obj in c.items():
    print(f"** CC      {src}")
    check(system(f"{CC} {CFLAGS} {src} -o {obj}"))

for src, obj in a.items():
    print(f"** AS      {src}")
    check(system(f"{AS} {AFLAGS} {src} -o {obj}"))

print(f"** OBJCOPY {font_in} {font_out}")
check(system(f"{OBJCOPY} -O elf64-x86-64 -B i386 -I binary {font_in} {font_out}"))

# Link objects

print(f"** LD      {executable}")
check(system(f"{LD} {LFLAGS} -T src/linker.ld -o {executable} {" ".join(objects)}"))

# Create and run ISO

print(f"Creating {cdrom}")

cd_root_directory = "build/cd_root"
boot_directory = f"{cd_root_directory}/boot/"
limine_prefix = "thirdparty/limine"
limine_boot_files = ["limine-bios-cd.bin", "limine-bios.sys", "limine-uefi-cd.bin"]
limine_conf = "src/limine.conf"

if not exists(boot_directory):
    makedirs(boot_directory)

if not exists(limine_prefix):
    makedirs(limine_prefix)

for f in limine_boot_files:
    copy(f"{limine_prefix}/{f}", boot_directory)

copy(limine_conf, boot_directory)
copy(executable, boot_directory)

# ISO 9660
xorriso_filesystem_flags = "-as mkisofs -no-emul-boot -boot-load-size 4 -boot-info-table"
xorriso_bios_flags = "-b boot/limine-bios-cd.bin"
xorriso_efi_flags = "--efi-boot boot/limine-uefi-cd.bin -efi-boot-part --efi-boot-image"
xorriso_other_flags = "--protective-ms-dos-label"

check(system(f"xorriso {xorriso_filesystem_flags} {xorriso_bios_flags} {xorriso_efi_flags} {cd_root_directory} -o {cdrom}"))
check(system(f"thirdparty/limine/limine bios-install {cdrom}"))

# Run

subcommand = argv[1] if len(argv) >= 2 else None

if subcommand == "debug" or subcommand == "run":
    qemu_flags = f"-serial stdio -cdrom {cdrom}"
    qemu_flags += " -no-reboot -no-shutdown -M smm=off -d int"

    if subcommand == "debug":
        qemu_flags += " -S -s"

    check(system(f"qemu-system-x86_64 {qemu_flags}"))
