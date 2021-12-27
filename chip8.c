#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<ncurses.h>
#include<time.h>
#include "instructions.c"

//Data storage
unsigned char memory[4096];
unsigned short stack[16];
//General purpose registers
unsigned char v0;
unsigned char v1;
unsigned char v2;
unsigned char v3;
unsigned char v4;
unsigned char v5;
unsigned char v6;
unsigned char v7;
unsigned char v8;
unsigned char v9;
unsigned char vA;
unsigned char vB;
unsigned char vC;
unsigned char vD;
unsigned char vE;
unsigned char vF;
//Special register I
unsigned short instructionRegister;
//Timer registers
unsigned char delayTimer;
unsigned char soundTimer;
//Program Counter and Stack Pointer
unsigned short pc;
unsigned char sp;

unsigned char keyToKeypad(char c)
{
	switch(c)
	{
		case '1': return 0x1;
		case '2': return 0x2;
		case '3': return 0x3;
		case '4': return 0xC;
		case 'q': return 0x4;
		case 'w': return 0x5;
		case 'e': return 0x6;
		case 'r': return 0xD;
		case 'a': return 0x7;
		case 's': return 0x8;
		case 'd': return 0x9;
		case 'f': return 0xE;
		case 'z': return 0xA;
		case 'x': return 0x0;
		case 'c': return 0xB;
		case 'v': return 0xF;
		default: return 0x0;
	}
}

//If this returns ERR, it means no keys have been pressed.
//Otherwise, the returned value will be the most recently pressed key.
char getMostRecentChar()
{
	timeout(0);
	char mostRecentchar = ERR;
	char getchChar;
	while( (getchChar = getch()) != ERR)
	{
		mostRecentchar = getchChar;
	}
	return mostRecentchar;
}

/*
unsigned short getInstruction(short pc)
{
	return (((short)memory[pc]) << 8) + (short)memory[pc+1];
}
*/

unsigned short getLower12bit(short value)
{
	return value & 0x0FFF;
}

unsigned char getLower8bit(short value)
{
	return value & 0xFF;
}

//gets the nth nibble, with 0 being the least significant nibble. 
unsigned short getNibble(short value, char n)
{
	return (value & (0xF << (n*4))) >> (n*4);
}

char * getRegister(short registerID)
{
	switch(registerID)
	{
		case 0x0:
			return &v0;
		case 0x1:
			return &v1;
		case 0x2:
			return &v2;
		case 0x3:
			return &v3;
		case 0x4:
			return &v4;
		case 0x5:
			return &v5;
		case 0x6:
			return &v6;
		case 0x7:
			return &v7;
		case 0x8:
			return &v8;
		case 0x9:
			return &v9;
		case 0xA:
			return &vA;
		case 0xB:
			return &vB;
		case 0xC:
			return &vC;
		case 0xD:
			return &vD;
		case 0xE:
			return &vE;
		case 0xF:
			return &vF;
	}
}

//Draws a box out of # symbols with a title,
//Does not refresh the display.
void drawBox(int x, int y, int width, int height, char title[])
{
	int titleLength = strlen(title);
	move(y, x);
	printw("#%s", title);
	for(int i = 0; i < width - titleLength - 1; i++)
	{
		addch('#');
	}
	
	for(int i = 1; i < height - 1; i++)
	{
		move(y + i, x); 
		addch('#');
		move(y + i, x + width - 1); 
		addch('#');
	}
	move(y + height - 1, x);
	for(int i = 0; i < width; i++)
	{
		addch('#');
	}
}

//Fills the screen with blanks over the bounds, inclusive of both ends
//Does not refresh the display
//Also, clears the input buffer.
void clearArea(int startX, int startY, int endX, int endY)
{
	for(int y = startY; y <= endY; y++)
	{
		move(y, startX);
		for(int x = startX; x <= endX; x++)
		{
			addch(' ');
		}
	}
}

int main(int argc, char *argv[])
{
    WINDOW *win = initscr();
    noecho();
	cbreak();
    srand(time(NULL));
    //srand(0);
	
	printw("The input for the game is 1234QWERASDFZXCV\n");
	printw("Press any key to begin. . .");
	move(0,0);
	getch();
	erase();
	//Initialize everything to 0
	memset(memory, 0, 4096);
	
	//This is used to assist in keeping track of the
	//most recently pressed char
	char mostRecentChar;
	
	struct timespec lastTime;
	clock_gettime(CLOCK_REALTIME, &lastTime);
	//Note: 1 second = 1_000_000_000 nanoseconds
	//Thus, 1/60th of a second is 16666667 ns
	
	//Load character sprites into the memory
	{
		//0
		memory[0] = 0XF0;
		memory[1] = 0X90;
		memory[2] = 0X90;
		memory[3] = 0X90;
		memory[4] = 0XF0;
		//1
		memory[5] = 0X20;
		memory[6] = 0X60;
		memory[7] = 0X20;
		memory[8] = 0X20;
		memory[9] = 0X70;
		//2
		memory[10] = 0XF0;
		memory[11] = 0X10;
		memory[12] = 0XF0;
		memory[13] = 0X80;
		memory[14] = 0XF0;
		//3
		memory[15] = 0XF0;
		memory[16] = 0X10;
		memory[17] = 0XF0;
		memory[18] = 0X10;
		memory[19] = 0XF0;
		//4
		memory[20] = 0X90;
		memory[21] = 0X90;
		memory[22] = 0XF0;
		memory[23] = 0X10;
		memory[24] = 0X10;
		//5
		memory[25] = 0XF0;
		memory[26] = 0X80;
		memory[27] = 0XF0;
		memory[28] = 0X10;
		memory[29] = 0XF0;
		//6
		memory[30] = 0XF0;
		memory[31] = 0X80;
		memory[32] = 0XF0;
		memory[33] = 0X90;
		memory[34] = 0XF0;
		//7
		memory[35] = 0XF0;
		memory[36] = 0X10;
		memory[37] = 0X20;
		memory[38] = 0X40;
		memory[39] = 0X40;
		//8
		memory[40] = 0XF0;
		memory[41] = 0X90;
		memory[42] = 0XF0;
		memory[43] = 0X90;
		memory[44] = 0XF0;
		//9
		memory[45] = 0XF0;
		memory[46] = 0X90;
		memory[47] = 0XF0;
		memory[48] = 0X10;
		memory[49] = 0XF0;
		//A
		memory[50] = 0XF0;
		memory[51] = 0X90;
		memory[52] = 0XF0;
		memory[53] = 0X90;
		memory[54] = 0X90;
		//B
		memory[55] = 0XE0;
		memory[56] = 0X90;
		memory[57] = 0XE0;
		memory[58] = 0X90;
		memory[59] = 0XE0;
		//C
		memory[60] = 0XF0;
		memory[61] = 0X80;
		memory[62] = 0X80;
		memory[63] = 0X80;
		memory[64] = 0XF0;
		//D
		memory[65] = 0XE0;
		memory[66] = 0X90;
		memory[67] = 0X90;
		memory[68] = 0X90;
		memory[69] = 0XE0;
		//E
		memory[70] = 0XF0;
		memory[71] = 0X80;
		memory[72] = 0XF0;
		memory[73] = 0X80;
		memory[74] = 0XF0;
		//F
		memory[75] = 0XF0;
		memory[76] = 0X80;
		memory[77] = 0XF0;
		memory[78] = 0X80;
		memory[79] = 0X80;
	}
	
	
	memset(stack, 0, 16);
	v0 = v1 = v2 = v3 = v4 = v5 = v6 = v7 = v8 = v9 = vA = vB = vC = vD = vE = vF = 0;
	instructionRegister = 0;
	delayTimer = soundTimer = 0;
	pc = 0;
	sp = 0;
	
	//Load file from args
	//Put everything from the ROM into RAM (upshifted 0x200 bytes)
	FILE *fileptr = fopen(argv[1], "rb");
	fread(memory + 0x200, 1, 0xFFF - 0x200, fileptr);
	fclose(fileptr);
	
	//Setup the instruction box
	int instructionBoxX = 64;
	int instructionBoxY = 0;
	int instructionBoxWidth = 48; //External width
	int instructionBoxHeight = 14;
	int instructionBoxDownshift = 2;
	drawBox(instructionBoxX, instructionBoxY, instructionBoxWidth, instructionBoxHeight, "INSTRUCTIONS");
	
	//Add the highlight
	move(instructionBoxY + instructionBoxDownshift + 1, instructionBoxX);
	addch('#' | A_REVERSE);
	move(instructionBoxY + instructionBoxDownshift + 1, instructionBoxX + instructionBoxWidth - 1);
	addch('#' | A_REVERSE);
	
	int opcodeX = 64;
	int opcodeY = 13;
	int opcodeWidth = 48;
	int opcodeHeight = 8;
	
	drawBox(opcodeX, opcodeY, opcodeWidth, opcodeHeight, "REGISTERS");
	
	int stepping = 1;
	
	pc = 0x200;
	//Increment pc starts at true, some functions might set it to false.
	int incrementPC = 1;
	while(pc <= 0xFFF)
	{
		union Instruction instruction = getInstruction(pc, memory);
		enum Opcode opcode = decode(instruction);
		
		clearArea(opcodeX + 1, opcodeY + 1, opcodeX + opcodeWidth - 2, opcodeY + opcodeHeight - 2);
		for(int x = 0; x < 4; x++)
		{
			for(int y = 0; y < 4; y++)
			{
				int registerID = y + (x*4);
				move(opcodeY + 1 + y, opcodeX + 2 + (12*x));
				unsigned char *vXptr = getRegister(registerID);
				
				printw("v%X: 0x%02X", registerID, *vXptr);
			}
		}
		
		move(opcodeY + 1 + 4, opcodeX + 2);
		printw("I: 0x%03X", instructionRegister);
		move(opcodeY + 1 + 4, opcodeX + opcodeWidth - 2 - 8);
		printw("DT: 0x%02X", delayTimer);
		move(opcodeY + 1 + 5, opcodeX + opcodeWidth - 2 - 8);
		printw("ST: 0x%02X", soundTimer);
		move(opcodeY + 1 + 4, opcodeX + 2 + 12);
		printw("PC: 0x%03X", pc);
		move(opcodeY + 1 + 4, opcodeX + 2 + 24);
		printw("SP: 0x%02X", sp);
		
		for(int i = 0; i < instructionBoxHeight - 2; i++)
		{
			unsigned short rowPC = pc - (instructionBoxDownshift * 2) + (2*i);
			
			move(instructionBoxY + i + 1, 66);
			//This is used for under/overflow checking. There is probably a much better way!
			int intPC = ((int)rowPC) - 4 + (2*i); 
			if(intPC < 0 || intPC > 0xFFF)
			{ //If out of bounds, just draw a blank
				for(int i = 0; i < instructionBoxWidth; i++)
				{
					addch(' ');
				}
			}
			else
			{
				union Instruction rowInstruction = getInstruction(rowPC, memory);
				enum Opcode rowOpcode = decode(rowInstruction);
				
				char opcodeNameBuffer[18];
				getParameters(rowInstruction, opcodeNameBuffer);
				
				printw("0x%03X %-13s %-17s", rowPC, opcodeString(rowOpcode), opcodeNameBuffer);
				
				printw(" ");
				printw("0x%04X", rowInstruction.whole);
			}
		}
		refresh();
		
		//Grab the most recently pressed key
		char mostRecentCharFromBuffer = getMostRecentChar();
		if(mostRecentCharFromBuffer != ERR)
		{
			mostRecentChar = mostRecentCharFromBuffer;
		}
		
		if(!stepping && (mostRecentChar == 'p'))
		{
			stepping = 1;
			mostRecentChar = ERR;
		}
		
		if(stepping)
		{
			//Wait for input
			timeout(-1);
			char nextChar = getch();
			if(nextChar == 'p')
			{ //Hitting P will run the game at full speed, can't be undone.
				stepping = 0;
			}
			else if(nextChar == 'o')
			{ //Hitting O will continue with a delay so you can get inputs in.
				struct timespec sleepTime;
				sleepTime.tv_sec = 0;
				sleepTime.tv_nsec = 250000000;
				nanosleep(&sleepTime, &sleepTime);
			} //Hitting anything else will do nothing
			else
			{
				
			}
		}
		
		struct timespec newTime;
		clock_gettime(CLOCK_REALTIME, &newTime);
		long int totalNewNanos = (((long int)newTime.tv_sec * ((long int) 1000000000)) + newTime.tv_nsec);
		long int totalLastNanos = (((long int)lastTime.tv_sec * ((long int) 1000000000)) + lastTime.tv_nsec);
		long int totalNanoDifference = totalNewNanos - totalLastNanos;
		
		//move(20, 20);
		//printw("%d --- %d", lastTime.tv_nsec, newTime.tv_nsec);
		
		//BUG IN PONG AT 0X232
		
		int decrementCount = (totalNanoDifference / 16666667);
		//int decrementCount = (totalNanoDifference / 16666667);
		if(delayTimer < decrementCount)
		{
			delayTimer = 0;
		}
		else
		{
			delayTimer -= decrementCount;
		}
		
		if(soundTimer < decrementCount)
		{
			soundTimer = 0;
		}
		else
		{
			soundTimer -= decrementCount;
		}
		
		if(decrementCount > 0)
		{
			lastTime = newTime;
		}
		
		//unsigned short instruction = getInstruction(pc);
		
		//{CLS, RET, JPADDR, CALLADDR, SEVXBYTE, SNEVXBYTE, SEVXVY, LDVXBYTE, ADDVXBYTE, LDVXVY, ORVXVY, ANDVXVY, XORVXVY, ADDVXVY, SUBVXVY, SHRVXVY, SUBNVXVY, SHLVXVY, SNEVXVY, LDIADDR, JPV0ADDR, RNDVXBYTE, DRWVXVYNIBBLE, SKPVX, SKNPVX, LDVXDT, LDVXK, LDDTVX, LDSTVX, ADDIVX, LDFVX, LDBVX, LDIVX, LDVXI, NOP}
		switch(decode(instruction))
		{
			case CLS:
			{
				clearArea(0, 0, 63, 31);
				refresh();
				//erase();
				break;
			}
			case RET:
			{
				unsigned short topOfStack = stack[sp--];
				pc = topOfStack;
				break;
			}
			case JPADDR:
			{
				pc = instruction.bottom12Bits;
				incrementPC = 0;
				break;
			}
			case CALLADDR:
			{
				stack[++sp] = pc;
				pc = instruction.bottom12Bits;
				incrementPC = 0;
				break;
			}
			case SEVXBYTE:
			{
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				unsigned char kk = instruction.lowByte;
				if(*vXptr == kk)
				{
					pc += 2;
				}
				break;
			}
			case SNEVXBYTE:
			{
				unsigned char kk = instruction.lowByte;
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				if(*vXptr != kk)
				{
					pc += 2;
				}
				break;
			}
			case SEVXVY:
			{
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				
				unsigned char y = instruction.nibble1;
				unsigned char *vYptr = getRegister(y);
				
				if(*vXptr == *vYptr)
				{
					pc += 2;
				}
				break;
			}
			case LDVXBYTE:
			{
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				unsigned char kk = instruction.lowByte;
				*vXptr = kk;
				break;
			}
			case ADDVXBYTE:
			{
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				unsigned char kk = instruction.lowByte;
				*vXptr += kk;
				break;
			}
			case LDVXVY:
			{
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				
				unsigned char y = instruction.nibble1;
				unsigned char *vYptr = getRegister(y);
				
				*vXptr = *vYptr;
				break;
			}
			case ORVXVY:
			{
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				
				unsigned char y = instruction.nibble1;
				unsigned char *vYptr = getRegister(y);
				
				*vXptr |= *vYptr;
				break;
			}
			case ANDVXVY:
			{
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				
				unsigned char y = instruction.nibble1;
				unsigned char *vYptr = getRegister(y);
				
				*vXptr &= *vYptr;
				break;
			}
			case XORVXVY:
			{
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				
				unsigned char y = instruction.nibble1;
				unsigned char *vYptr = getRegister(y);
				
				*vXptr ^= *vYptr;
				break;
			}
			case ADDVXVY:
			{
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				
				unsigned char y = instruction.nibble1;
				unsigned char *vYptr = getRegister(y);
				
				//Check for overflow
				int result = (int)*vXptr + (int)*vYptr;
				vF = result > 0xFF;
				
				*vXptr += *vYptr;
				break;
			}
			case SUBVXVY:
			{
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				
				unsigned char y = instruction.nibble1;
				unsigned char *vYptr = getRegister(y);
				
				vF = *vXptr > *vYptr;
				*vXptr -= *vYptr;
				break;
			}
			case SHRVXVY:
			{
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				//set lsb == 1
				vF = ((*vXptr) & 1) == 1;
				*vXptr >>= 1;
				break;
			}
			case SUBNVXVY:
			{
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				
				unsigned char y = instruction.nibble1;
				unsigned char *vYptr = getRegister(y);
				vF = *vYptr > *vXptr;
				*vXptr -= *vYptr;
				break;
			}
			case SHLVXVY:
			{
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				//set msb == 1
				vF = (*vXptr & 0x80) == 0x80;
				//vX *= 2
				*vXptr <<= 1;
				break;
			}
			case SNEVXVY:
			{
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				
				unsigned char y = instruction.nibble1;
				unsigned char *vYptr = getRegister(y);
				if((*vXptr) != (*vYptr))
				{
					pc += 2;
				}
				break;
			}
			case LDIADDR:
			{
				unsigned short address = instruction.bottom12Bits;
				instructionRegister = address;
				break;
			}
			case JPV0ADDR:
			{
				short address = instruction.bottom12Bits;
				pc = address + v0;
				
				incrementPC = 0;
				break;
			}
			case RNDVXBYTE:
			{
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				unsigned char randomValue = (unsigned char)rand();
				unsigned char kk = instruction.lowByte;
				*vXptr = randomValue & kk;
				break;
			}
			case DRWVXVYNIBBLE:
			{
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				
				unsigned char y = instruction.nibble1;
				unsigned char *vYptr = getRegister(y);
				
				unsigned char n = instruction.nibble0;
				unsigned char startX = *vXptr;
				unsigned char startY = *vYptr;
				int anyChanged = 0;
				for(int j = 0; j < n; j++) //Sprite line number
				{
					unsigned char spriteLine = memory[instructionRegister+j];
					for(int k = 7; k >= 0; k--)
					{
						move( (startY + j) % 32, (startX + k) % 64);
						int oldEnabled = (inch() & A_REVERSE) == A_REVERSE;
						int flipBit = (spriteLine&1);
						int newShouldBeEnabled = oldEnabled ^ flipBit;
						
						//If spriteLine&1 == 1 that means we are changing the pixel
						//Also, if newShouldBeEnabled == 0 that means we erased a pixel.
						//In this case, we should set anyChanged to true. and also
						//at the end, we set VF to 1
						if( ((flipBit)==1) && (newShouldBeEnabled==0))
						{ //This can probably be a 1 line |= statement
							anyChanged = 1;
						}
						
						if(flipBit)
						{ 
							addch(' ' | (oldEnabled ? 0 : A_REVERSE));
						}
						
						spriteLine >>= 1;
					}
				}
				refresh();
				vF = anyChanged;
				break;
			}
			case SKPVX:
			{
				char mostRecentCharFromBuffer = getMostRecentChar();
				if(mostRecentCharFromBuffer != ERR)
				{
					mostRecentChar = mostRecentCharFromBuffer;
				}
				unsigned char key = keyToKeypad(mostRecentChar);
				//unsigned char key = keyToKeypad(getch());
				
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				if(key == *vXptr)
				{
					pc += 2;
				}
				//This should definitely be removed in the future, but for now it is how I am handling holding keys down which will help handle multiple key checks in a row.
				//mostRecentChar = ERR;
				break;
			}
			case SKNPVX:
			{
				char mostRecentCharFromBuffer = getMostRecentChar();
				if(mostRecentCharFromBuffer != ERR)
				{
					mostRecentChar = mostRecentCharFromBuffer;
				}
				unsigned char key = keyToKeypad(mostRecentChar);
				//unsigned char key = keyToKeypad(getch());
				
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				move(20, 20);
				if(key != *vXptr)
				{
					pc += 2;
				}
				//mostRecentChar = ERR;
				break;
			}
			case LDVXDT:
			{
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				*vXptr = delayTimer;
				break;
			}
			case LDVXK:
			{
				//Set most recent char to empty, also clear the buffer.
				mostRecentChar = ERR;
				getMostRecentChar();
				
				//notimeout(win, 1);
				timeout(-1);
				char key = (char) getch();
				//unsigned char key = 0;
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				
				*vXptr = keyToKeypad(key);
				break;
			}
			case LDDTVX:
			{
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				delayTimer = *vXptr;
				break;
			}
			case LDSTVX:
			{
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				soundTimer = *vXptr;
				break;
			}
			case ADDIVX:
			{
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				instructionRegister += *vXptr;
				break;
			}
			case LDFVX:
			{
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				instructionRegister = 5 * (*vXptr);
				break;
			}
			case LDBVX:
			{
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				memory[instructionRegister  ] = ((*vXptr) / 100) % 10;
				memory[instructionRegister+1] = ((*vXptr) / 10 ) % 10;
				memory[instructionRegister+2] = ((*vXptr)      ) % 10;
				break;
			}
			case LDIVX:
			{
				unsigned char x = instruction.nibble2;
				//unsigned char *registerPointer1 = getRegister(registerValue1);
				for(int i = 0; i <= x; i++)
				{
					unsigned char *registerPointer = getRegister(i);
					memory[instructionRegister + i] = *registerPointer;
				}
				break;
			}
			case LDVXI:
			{
				unsigned char x = instruction.nibble2;
				for(int i = 0; i <= x; i++)
				{
					unsigned char *registerPointer = getRegister(i);
					*registerPointer = memory[instructionRegister + i];
				}
				break;
			}
			case NOP:
			{
				break;
			}
		}
		
		if(incrementPC)
		{
			pc+=2;
		}
		incrementPC = 1;
	}
	move(33, 0);
	printw("End of game. Press any key to continue.");
	//Clear the input buffer
	getMostRecentChar();
	timeout(-1);
	getch();
    endwin();
	return 0;
}