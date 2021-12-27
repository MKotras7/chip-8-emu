
# # chip-8-emu
A work in progress [CHIP-8](https://en.wikipedia.org/wiki/CHIP-8) emulator written in C using the `ncurses` API for the display in a terminal.

## Installation and Usage
 - Download the source code from this repo
 - Compile with `gcc ./chip8.c -lncurses`
 - run the created `a.out` with a path to your ROM, ie: `./a.out ./ROMS/test_opcode.ch8`
 - Ensure your terminal window is 112x32 pixels or greater, if it is less than this resolution, some things will not draw correctly.

## Inputs
Inputs for this emulator are
```
1234
QWER
ASDF
ZXCV
```
Due to ncurses limitations, the way inputs work is tapping an input key will toggle the button on or off in the emulator, tapping the same key a second time will disable that key. One dirty solution to get around this is to set up a macro using whatever tools you might have access to (autohotkey for windows is one that works) to send a press of the key on both the press and release of a key.
## Modes
When the emulator starts, it will be in a 'stepping' mode, when this is active, the emulator will pause in between execution cycles and wait for user input.

 - If the user presses "O" the emulator will pause for 1/4th of a second, the purpose of this is that you can enter inputs to the emulator during this delay.
 - If "P" is pressed the emulator will exit 'stepping' mode and there will be no delay between execution cycles.
 - If anything else is pressed, the emulator will execute the current instruction and once complete will pause until the next input.
## Developing for my emulator
When the program counter runs off 0xFFF, it will halt execution and will wait for input to close the screen and return to the terminal. So there is no need to add an infinite loop at the bottom of the code with my implementation.
There is a window off to the side of the screen which is useful for debugging, 
## Sources and other links
 - Cowgod's Chip-8 Technical Reference: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
 - CHIP-8 wikipedia: https://en.wikipedia.org/wiki/CHIP-8

