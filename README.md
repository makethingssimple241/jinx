## Introduction

### Features supported

#### x86-64 architecture

1. Text-based graphics driver in graphics video mode
1. Driver for COM1 and COM2 serial ports
1. Driver for 2 cascaded 8259 PICs

## Building and running

### Requirements

1. `python3`
1. `git`
1. `make`
1. `cc` (used to build Limine)
1. `curl`
1. `clang` (you can modify `build.py` to use other compilers)
1. `x86_64-elf-binutils`
1. `xorriso` (you can modify `build.py` to use generate the cdrom differently)
1. `qemu-system-x86_64`

> **Note**
> 
> - `python3`,
> - `git`,
> - `make`,
> - `cc`,
> - `curl`,
> - `clang`,
> - `x86_64-elf-ld` (from `x86_64-elf-binutils`),
> - `xorriso`,  and
> - `qemu-system-x86_64`
> 
> will be invoked as listed above.
> 
> You must either modify the related Python scripts and `Makefile`s to invoke the tools the way your environment expects them to be invoked, or modify your environment such that the above listed tools can be invoked as is.

### Guide

1. Run `configure.py` which will download all the necessary third-party files
1. Run `build.py` which will build and produce a cdrom
1. Pass `run` to `build.py` if you would like the cdrom to be ran with `qemu-system-x86_64`

> **Note**
> 
> Run `unconfigure.py` to remove all the build files and third-party files downloaded by `configure.py`

### Debugging

When debugging:

1. Pass `debug` to `build.py`. Doing so will pass `-s -S` to `qemu-system-x86_64` which will allow you to attach a remote debugger to debug the operating system.
1. Start a debugger that can be used with QEMU and set the remote target to `localhost:1234`
1. Set the architecture and file (kernel file) according to the way you do so in your debugger

Happy debugging 🎉

## Acknowledgements

### OSDev

- Followed [OSDev's tutorial on Limine](https://wiki.osdev.org/Limine_Bare_Bones)

- Followed [OSDev's tutorial on Serial Ports](https://wiki.osdev.org/Serial_Ports)

- Referred to [*So how do I program this stuff?*](https://wiki.osdev.org/Interrupts#So_how_do_I_program_this_stuff?) of [OSDev's article on Interrupts](https://wiki.osdev.org/Interrupts)

- Followed [OSDev's tutorial on Interrupts](https://wiki.osdev.org/Interrupts_Tutorial)

- Referred to `IO_WAIT` of [OSDev's inline assembly examples](https://wiki.osdev.org/Inline_Assembly/Examples#IO_WAIT)

- Referred to [*divisor by 0 exception error*](https://f.osdev.org/viewtopic.php?t=8885) of [OSDev Forum](https://f.osdev.org/)

- Referred to [*Interrupts seem to invalidate stack*](https://f.osdev.org/viewtopic.php?t=56425) of [OSDev Forum](https://f.osdev.org/)

### Other resources

- Referred to *The Serial Ports* of [Build your Own Operating #3](https://gayan1999malinda.medium.com/build-your-own-operating-system-2-410b1ff759ae)

### Open source software used

- Used [Limine](https://github.com/limine-bootloader/limine)

- Used PC Screen Font [default8x16.psfu](https://github.com/legionus/kbd/blob/master/data/consolefonts/default8x16.psfu) from [kbd](https://github.com/legionus/kbd) under [GNU General Public License (GPL), version 2](https://github.com/legionus/kbd/blob/master/LICENSE)

## License

See `LICENSE` for the license of this project.
