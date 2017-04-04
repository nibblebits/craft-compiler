# Craft Compiler V0.1.0
A general purpose compiler and compiler framework for the Craft programming language

Website: http://craft-language.org

If you have any problems send a message to "daniel" on the Craft language forums: http://craft-language.org/forum

# Repository cloning information
The Craft compiler repository uses the submodule MagicOMF that was written for this project.
When cloning you must specify the --recursive flag or MagicOMF may not be downloaded.

Command to clone: git clone --recursive https://github.com/nibblebits/craft-compiler


# Current object format targets:
OMF(Object Module Format)

# Current processor targets
8086 processor

# Current linkers
Binary linker

# Current supported systems your executable can be compiled for
MS-DOS - (You can use the DOSBOX emulator to run your executable)

Bootable executable (no operating system required)

# How to compile?

Currently only Windows is supported, please download Cygwin and install the g++-14 compiler and the "make" Cygwin packages.
Once installed ensure that your environment PATH variable is set to the "bin" directory of Cygwin.

Once your ready open Cygwin terminal and navigate to the root directory of Craft Compiler and then do the commands:

make clean

make release

This will clean the project of object files and then rebuild the compiler in release mode.

If you wish to have debugging information provided to you when running the compiler then please build the compiler with the commands:

make clean

make debug

You will not really need to build the compiler in debug mode unless you are working on the compiler to improve the project
"make release" should be efficient.

You do not always need to use "make clean" only when you want to do a full rebuild.

