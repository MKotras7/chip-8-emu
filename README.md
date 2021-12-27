# # chip-8-emu
A work in progress [CHIP-8](https://en.wikipedia.org/wiki/CHIP-8) emulator written in C using the `ncurses` API for the display in a terminal.
 gcc ./chip8.c -lncurses && ./a.out ./ROMS/test_opcode.ch8 Input keys are 1234 QWER ASDF ZXCV

Hi! I'm your first Markdown file in **StackEdit**. If you want to learn about StackEdit, you can read me. If you want to play with Markdown, you can edit me. Once you have finished with me, you can create new files by opening the **file explorer** on the left corner of the navigation bar.


## Installation and Usage
 - Download the source code from this repo
 - Compile with `gcc ./chip8.c -lncurses`
 - run the created `a.out` with a path to your ROM, ie: `./a.out ./ROMS/test_opcode.ch8`

## Inputs
Inputs for this emulator are
```
1234
QWER
ASDF
ZXCV
```
## Modes
When the emulator starts, it will be in a 'stepping' mode, when this is active, the emulator will pause in between execution cycles and wait for user input.

 - If the user presses "O" the emulator will pause for 1/4th of a second, the purpose of this is that you can enter inputs to the emulator during this delay.
 - If "P" is pressed the emulator will exit 'stepping' mode and there will be no delay between execution cycles.
 - If anything else is pressed, the emulator will execute the current instruction and once complete will pause until the next input.
## Developing for my emulator
Unlike some CHIP-8 implementations, when the program counter runs off 0xFFF, it will halt execution and will wait for input to close the screen and return to the terminal.
## Sources and other links
 - Cowgod's Chip-8 Technical Reference: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
 - CHIP-8 wikipedia: https://en.wikipedia.org/wiki/CHIP-8

