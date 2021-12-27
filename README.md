# chip-8-emu
A CHIP-8 emulator written in C
gcc ./chip8.c -lncurses && ./a.out ./ROMS/test_opcode.ch8
Input keys are 
1234
QWER
ASDF
ZXCV

When starting the emulator it will be in "stepping mode" where hitting any key (that isnt 'O' or 'P') will advance the program one cycle. When you hit 'O', the code will pause for 250 milliseconds to give you time to enter inputs. The 'P' key will take you out of step mode, and the program will run at full speed, you can press 'P' again to re-enter step mode at any time.

This emulator will detect when the program counter runs to the end of the ROM, and will stop executing.