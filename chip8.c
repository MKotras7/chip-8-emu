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
	
	struct timespec lastTime;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &lastTime);
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
	FILE *fileptr = fopen(argv[1], "r");
	fread(memory + 0x200, 1, 3584, fileptr);
	fclose(fileptr);
	
	pc = 0x200;
	//Increment pc starts at true, some functions might set it to false.
	int incrementPC = 1;
	while(pc <= 0xFFF)
	{
		struct timespec newTime;
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &newTime);
		long int totalNewNanos = (((long int)newTime.tv_sec * ((long int) 1000000000)) + newTime.tv_nsec);
		long int totalLastNanos = (((long int)lastTime.tv_sec * ((long int) 1000000000)) + lastTime.tv_nsec);
		long int totalNanoDifference = totalNewNanos - totalLastNanos;
		
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
		union Instruction instruction = getInstruction(pc, memory);
		
		//{CLS, RET, JPADDR, CALLADDR, SEVXBYTE, SNEVXBYTE, SEVXVY, LDVXBYTE, ADDVXBYTE, LDVXVY, ORVXVY, ANDVXVY, XORVXVY, ADDVXVY, SUBVXVY, SHRVXVY, SUBNVXVY, SHLVXVY, SNEVXVY, LDIADDR, JPV0ADDR, RNDVXBYTE, DRWVXVYNIBBLE, SKPVX, SKNPVX, LDVXDT, LDVXK, LDDTVX, LDSTVX, ADDIVX, LDFVX, LDBVX, LDIVX, LDVXI, NOP}
		switch(decode(instruction))
		{
			case CLS:
			{
				erase();
				break;
			}
			case RET:
			{
				unsigned short topOfStack = stack[sp];
				sp--;
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
				sp++;
				stack[sp] = pc;
				pc = instruction.bottom12Bits;
				incrementPC = 0;
				break;
			}
			case SEVXBYTE:
			{
				unsigned char kk = instruction.lowByte;
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				if(*vXptr == kk)
				{
					pc+=2;
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
					pc+=2;
				}
				break;
			}
			case SEVXVY:
			{
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				
				unsigned char y = instruction.nibble1;
				unsigned char *vYptr = getRegister(y);
				
				if(*vXptr != *vYptr)
				{
					pc+=2;
				}
				break;
			}
			case LDVXBYTE:
			{
				//printf("LD Vx, byte");
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				unsigned char kk = instruction.lowByte;
				*vXptr = kk;
				
				//printf("LD V%x 0x%02X", x, kk);
				break;
			}
			case ADDVXBYTE:
			{
				//printf("ADD Vx, byte");
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				unsigned char kk = instruction.lowByte;
				//printf("ADD V%x 0x%02X", x, kk);
				*vXptr += kk;
				break;
			}
			case LDVXVY:
			{
				//printf("LD Vx, Vy");
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				
				unsigned char y = instruction.nibble1;
				unsigned char *vYptr = getRegister(y);
				
				*vXptr = *vYptr;
				
				//printf("LD V%x, V%x", x, y);
				break;
			}
			case ORVXVY:
			{
				//printf("OR Vx, Vy");
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				
				unsigned char y = instruction.nibble1;
				unsigned char *vYptr = getRegister(y);
				
				*vXptr |= *vYptr;
				
				//printf("OR V%x, V%x", x, y);
				break;
			}
			case ANDVXVY:
			{
				//printf("AND Vx, Vy");
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				
				unsigned char y = instruction.nibble1;
				unsigned char *vYptr = getRegister(y);
				
				*vXptr &= *vYptr;
				
				//printf("AND V%x, V%x", x, y);
				break;
			}
			case XORVXVY:
			{
				//printf("XOR Vx, Vy");
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				
				unsigned char y = instruction.nibble1;
				unsigned char *vYptr = getRegister(y);
				
				*vXptr ^= *vYptr;
				
				//printf("XOR V%x, V%x", x, y);
				break;
			}
			case ADDVXVY:
			{
				//printf("ADD Vx, Vy");
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				
				unsigned char y = instruction.nibble1;
				unsigned char *vYptr = getRegister(y);
				
				//Check for overflow
				int result = (int)*vXptr + (int)*vYptr;
				vF = result > 0xFF;
				
				*vXptr += *vYptr;
				//printf("ADD V%x, V%x", x, y);
				break;
			}
			case SUBVXVY:
			{
				//printf("SUB Vx, Vy");
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				
				unsigned char y = instruction.nibble1;
				unsigned char *vYptr = getRegister(y);
				
				vF = *vXptr > *vYptr;
				*vXptr -= *vYptr;
				//printf("SUB V%x, V%x", x, y);
				break;
			}
			case SHRVXVY:
			{
				//printf("SHR Vx {, Vy}");
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				//set lsb == 1
				vF = ((*vXptr) & 1) == 1;
				*vXptr >>= 1;
				//printf("SHR V%x", x);
				break;
			}
			case SUBNVXVY:
			{
				//printf("SUBN Vx, Vy");
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				
				unsigned char y = instruction.nibble1;
				unsigned char *vYptr = getRegister(y);
				vF = *vYptr > *vXptr;
				*vXptr -= *vYptr;
				//printf("SUBN V%x, V%x", x, y);
				break;
			}
			case SHLVXVY:
			{
				//printf("SHL Vx {, Vy}");
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				//set msb == 1
				vF = (*vXptr & 0x8000) == 0x8000;
				//vX *= 2
				*vXptr <<= 1;
				//printf("SHL V%x {, Vy}", x);
				break;
			}
			case SNEVXVY:
			{
				//printf("SNE Vx, Vy");
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				
				unsigned char y = instruction.nibble1;
				unsigned char *vYptr = getRegister(y);
				if((*vXptr) != (*vYptr))
				{
					pc += 2;
				}
				//printf("SNE V%x, V%x", x, y);
				break;
			}
			case LDIADDR:
			{
				//printf("LD I, addr");
				unsigned short address = instruction.bottom12Bits;
				//printf("LD I=0x%01X->0x%03X", i, address); 
				instructionRegister = address;
				//printf("LD I, 0x%03X", address);
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
							move( (startY + j) % 32, (startX + k) % 64);
							addch(' ' | A_REVERSE);
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
				timeout(0);
				unsigned char key = (unsigned char) getch();
				
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				if(key == *vXptr)
				{
					pc += 2;
				}
				break;
			}
			case SKNPVX:
			{
				timeout(0);
				unsigned char key = (unsigned char) getch();
				
				unsigned char x = instruction.nibble2;
				unsigned char *vXptr = getRegister(x);
				if(key != *vXptr)
				{
					pc += 2;
				}
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
				notimeout(win, 1);
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
				for(int iterator = 0; iterator <= x; iterator++)
				{
					unsigned char *registerPointer = getRegister(iterator);
					memory[instructionRegister + iterator] = *registerPointer;
				}
				break;
			}
			case LDVXI:
			{
				unsigned char x = instruction.nibble2;
				for(int j = 0; j <= x; j++)
				{
					unsigned char *registerPointer = getRegister(j);
					*registerPointer = memory[instructionRegister+j];
				}
				break;
			}
			case NOP:
			{
				break;
			}
		}
		
		//printf("PC: 0x%03X Instruction: 0x%04X: \n", pc, instruction);
		/*
		if((instruction & 0xF000) == 0x0000)
		{
			printf("SYS addr");
			//This is ignored by modern interpreters according to Cowgod's Chip-8 Technical Reference
		}
		else */ 
		/* if(instruction == 0x00E0)
		{
			//printf("CLS");
			erase();
		}
		else if(instruction == 0x00EE)
		{
			//printf("RET");
			unsigned short topOfStack = stack[sp];
			//printf("RET, PC=0x%03X->0x%03X", pc, topOfStack);
			sp--;
			pc = topOfStack;
		}
		else if((instruction & 0xF000) == 0x1000)
		{
			//printf("JP addr");
			unsigned short address = getLower12bit(instruction);
			//printf("JP 0x%03X", address);
			pc = address;
			incrementPC = 0;
		}
		else if((instruction & 0xF000) == 0x2000)
		{
			//printf("CALL addr");
			sp++;
			stack[sp] = pc;
			unsigned short address = getLower12bit(instruction);
			//printf("CALL 0x%03X", address);
			pc = address;
			incrementPC = 0;
		}
		else if((instruction & 0xF000) == 0x3000)
		{
			//printf("SE Vx, byte");
			unsigned char kk = getLower8bit(instruction);
			unsigned char x = getNibble(instruction, 2);
			unsigned char *vXptr = getRegister(x);
			//printf("SE V%x, 0x%x ", x, kk);
			if(*vXptr == kk)
			{
				pc+=2;
				//printf("skip");
			}
		}
		else if((instruction & 0xF000) == 0x4000)
		{
			//printf("SNE Vx, byte");
			unsigned char kk = getLower8bit(instruction);
			unsigned char x = getNibble(instruction, 2);
			unsigned char *vXptr = getRegister(x);
			//printf("SE V%x, 0x%X ", x, kk);
			if(*vXptr != kk)
			{
				pc+=2;
				//printf("skip");
			}
		}
		else if((instruction & 0xF00F) == 0x5000)
		{
			//printf("SE Vx, Vy");
			unsigned char x = getNibble(instruction, 2);
			unsigned char *vXptr = getRegister(x);
			
			unsigned char y = getNibble(instruction, 1);
			unsigned char *vYptr = getRegister(y);
			//printf("SE V%x, V%x ", x, y);
			if(*vXptr != *vYptr)
			{
				pc+=2;
				//printf("skip");
			}
		}
		else if((instruction & 0xF000) == 0x6000)
		{
			//printf("LD Vx, byte");
			unsigned char x = getNibble(instruction, 2);
			unsigned char *vXptr = getRegister(x);
			unsigned char kk = getLower8bit(instruction);
			*vXptr = kk;
			
			//printf("LD V%x 0x%02X", x, kk);
		}
		else if((instruction & 0xF000) == 0x7000)
		{
			//printf("ADD Vx, byte");
			unsigned char x = getNibble(instruction, 2);
			unsigned char *vXptr = getRegister(x);
			unsigned char kk = getLower8bit(instruction);
			//printf("ADD V%x 0x%02X", x, kk);
			*vXptr += kk;
		}
		else if((instruction & 0xF00F) == 0x8000)
		{
			//printf("LD Vx, Vy");
			unsigned char x = getNibble(instruction, 2);
			unsigned char *vXptr = getRegister(x);
			
			unsigned char y = getNibble(instruction, 1);
			unsigned char *vYptr = getRegister(y);
			
			*vXptr = *vYptr;
			
			//printf("LD V%x, V%x", x, y);
		}
		else if((instruction & 0xF00F) == 0x8001)
		{
			//printf("OR Vx, Vy");
			unsigned char x = getNibble(instruction, 2);
			unsigned char *vXptr = getRegister(x);
			
			unsigned char y = getNibble(instruction, 1);
			unsigned char *vYptr = getRegister(y);
			
			*vXptr |= *vYptr;
			
			//printf("OR V%x, V%x", x, y);
		}
		else if((instruction & 0xF00F) == 0x8002)
		{
			//printf("AND Vx, Vy");
			unsigned char x = getNibble(instruction, 2);
			unsigned char *vXptr = getRegister(x);
			
			unsigned char y = getNibble(instruction, 1);
			unsigned char *vYptr = getRegister(y);
			
			*vXptr &= *vYptr;
			
			//printf("AND V%x, V%x", x, y);
		}
		else if((instruction & 0xF00F) == 0x8003)
		{
			//printf("XOR Vx, Vy");
			unsigned char x = getNibble(instruction, 2);
			unsigned char *vXptr = getRegister(x);
			
			unsigned char y = getNibble(instruction, 1);
			unsigned char *vYptr = getRegister(y);
			
			*vXptr ^= *vYptr;
			
			//printf("XOR V%x, V%x", x, y);
		}
		else if((instruction & 0xF00F) == 0x8004)
		{
			//printf("ADD Vx, Vy");
			unsigned char x = getNibble(instruction, 2);
			unsigned char *vXptr = getRegister(x);
			
			unsigned char y = getNibble(instruction, 1);
			unsigned char *vYptr = getRegister(y);
			
			//Check for overflow
			int result = (int)*vXptr + (int)*vYptr;
			vF = result > 0xFF;
			
			*vXptr += *vYptr;
			//printf("ADD V%x, V%x", x, y);
		}
		else if((instruction & 0xF00F) == 0x8005)
		{
			//printf("SUB Vx, Vy");
			unsigned char x = getNibble(instruction, 2);
			unsigned char *vXptr = getRegister(x);
			
			unsigned char y = getNibble(instruction, 1);
			unsigned char *vYptr = getRegister(y);
			
			vF = *vXptr > *vYptr;
			*vXptr -= *vYptr;
			//printf("SUB V%x, V%x", x, y);
		}
		else if((instruction & 0xF00F) == 0x8006)
		{
			//printf("SHR Vx {, Vy}");
			unsigned char x = getNibble(instruction, 2);
			unsigned char *vXptr = getRegister(x);
			//set lsb == 1
			vF = ((*vXptr) & 1) == 1;
			*vXptr >>= 1;
			//printf("SHR V%x", x);
		}
		else if((instruction & 0xF00F) == 0x8007)
		{
			//printf("SUBN Vx, Vy");
			unsigned char x = getNibble(instruction, 2);
			unsigned char *vXptr = getRegister(x);
			
			unsigned char y = getNibble(instruction, 1);
			unsigned char *vYptr = getRegister(y);
			vF = *vYptr > *vXptr;
			*vXptr -= *vYptr;
			//printf("SUBN V%x, V%x", x, y);
		}
		else if((instruction & 0xF00F) == 0x800E)
		{
			//printf("SHL Vx {, Vy}");
			unsigned char x = getNibble(instruction, 2);
			unsigned char *vXptr = getRegister(x);
			//set msb == 1
			vF = (*vXptr & 0x8000) == 0x8000;
			//vX *= 2
			*vXptr <<= 1;
			//printf("SHL V%x {, Vy}", x);
		}
		else if((instruction & 0xF00F) == 0x9000)
		{
			//printf("SNE Vx, Vy");
			unsigned char x = getNibble(instruction, 2);
			unsigned char *vXptr = getRegister(x);
			
			unsigned char y = getNibble(instruction, 1);
			unsigned char *vYptr = getRegister(y);
			if((*vXptr) != (*vYptr))
			{
				pc += 2;
			}
			//printf("SNE V%x, V%x", x, y);
		}
		else if((instruction & 0xF000) == 0xA000)
		{
			//printf("LD I, addr");
			short address = getLower12bit(instruction);
			//printf("LD I=0x%01X->0x%03X", i, address); 
			instructionRegister = address;
			//printf("LD I, 0x%03X", address);
		}
		else if((instruction & 0xF000) == 0xB000)
		{
			//printf("JP V0, addr");
			short address = getLower12bit(instruction);
			pc = address + v0;
			
			incrementPC = 0;
			//printf("JP V0, 0x%03X", address);
		}
		else if((instruction & 0xF000) == 0xC000)
		{
			//printf("RND Vx, byte");
			unsigned char x = getNibble(instruction, 2);
			unsigned char *vXptr = getRegister(x);
			unsigned char randomValue = (unsigned char)rand();
			unsigned char kk = getLower8bit(instruction);
			*vXptr = randomValue & kk;
			//printf("RND V%x, 0x%02X", x, kk);
		}
		else if((instruction & 0xF000) == 0xD000)
		{
			//printf("DRW Vx, Vy, nibble");
			
			unsigned char x = getNibble(instruction, 2);
			unsigned char *vXptr = getRegister(x);
			
			unsigned char y = getNibble(instruction, 1);
			unsigned char *vYptr = getRegister(y);
			
			unsigned char n = getNibble(instruction, 0);
			unsigned char startX = *vXptr;
			unsigned char startY = *vYptr;
			int anyChanged = 0;
			for(int j = 0; j < n; j++) //Sprite line number
			{
				unsigned char spriteLine = memory[instructionRegister+j];
				for(int k = 7; k >= 0; k--)
				{
					
					
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
						move( (startY + j) % 32, (startX + k) % 64);
						addch(' ' | A_REVERSE);
					}
					
					spriteLine >>= 1;
				}
			}
			refresh();
			vF = anyChanged;
			//Display
		}
		else if((instruction & 0xF0FF) == 0xE09E)
		{
			//printf("SKP Vx");
			timeout(0);
			unsigned char key = (unsigned char) getch();
			
			unsigned char x = getNibble(instruction, 2);
			unsigned char *vXptr = getRegister(x);
			if(key == *vXptr)
			{
				pc += 2;
			}
			//Input
		}
		else if((instruction & 0xF0FF) == 0xE0A1)
		{
			//printf("SKNP Vx");
			timeout(0);
			unsigned char key = (unsigned char) getch();
			
			unsigned char x = getNibble(instruction, 2);
			unsigned char *vXptr = getRegister(x);
			if(key != *vXptr)
			{
				pc += 2;
			}
			//Input
		}
		else if((instruction & 0xF0FF) == 0xF007)
		{
			//printf("LD Vx, DT");
			unsigned char x = getNibble(instruction, 2);
			unsigned char *vXptr = getRegister(x);
			*vXptr = delayTimer;
			
			//printf("LD V%x, DT", x);
		}
		else if((instruction & 0xF0FF) == 0xF00A)
		{
			//printf("LD Vx, K");
			notimeout(win, 1);
			char key = (char) getch();
			//unsigned char key = 0;
			unsigned char x = getNibble(instruction, 2);
			unsigned char *vXptr = getRegister(x);
			
			*vXptr = keyToKeypad(key);
			//Input
		}
		else if((instruction & 0xF0FF) == 0xF015)
		{
			//printf("LD DT, Vx");
			unsigned char x = getNibble(instruction, 2);
			unsigned char *vXptr = getRegister(x);
			delayTimer = *vXptr;
			//printf("LD DT, V%x", x);
		}
		else if((instruction & 0xF0FF) == 0xF018)
		{
			//printf("LD ST, Vx");
			unsigned char x = getNibble(instruction, 2);
			unsigned char *vXptr = getRegister(x);
			soundTimer = *vXptr;
			//printf("LD ST, V%x", x);
		}
		else if((instruction & 0xF0FF) == 0xF01E)
		{
			//printf("ADD I, Vx");
			unsigned char x = getNibble(instruction, 2);
			unsigned char *vXptr = getRegister(x);
			instructionRegister += *vXptr;
			//printf("ADD I, V%x", x);
		}
		else if((instruction & 0xF0FF) == 0xF029)
		{
			//printf("LD F, Vx");
			unsigned char x = getNibble(instruction, 2);
			unsigned char *vXptr = getRegister(x);
			instructionRegister = 5 * (*vXptr);
			//Display
		}
		else if((instruction & 0xF0FF) == 0xF033)
		{
			//printf("LD B, Vx");
			unsigned char x = getNibble(instruction, 2);
			unsigned char *vXptr = getRegister(x);
			memory[instructionRegister  ] = ((*vXptr) / 100) % 10;
			memory[instructionRegister+1] = ((*vXptr) / 10 ) % 10;
			memory[instructionRegister+2] = ((*vXptr)      ) % 10;
			//printf("LD B, V%x", x);
		}
		else if((instruction & 0xF0FF) == 0xF055)
		{
			//printf("LD [I], Vx");
			unsigned char x = getNibble(instruction, 2);
			//unsigned char *registerPointer1 = getRegister(registerValue1);
			for(int iterator = 0; iterator <= x; iterator++)
			{
				unsigned char *registerPointer = getRegister(iterator);
				memory[instructionRegister + iterator] = *registerPointer;
			}
			//printf("LD [I], V%x", x);
		}
		else if((instruction & 0xF0FF) == 0xF065)
		{
			//printf("LD Vx, [I]");
			unsigned char x = getNibble(instruction, 2);
			for(int j = 0; j <= x; j++)
			{
				unsigned char *registerPointer = getRegister(j);
				*registerPointer = memory[instructionRegister+j];
			}
			//printf("LD V%x, [I]", x);
		}
		else
		{
			//printf("nop");
		}
		*/
		
		//printf("\n");
		if(incrementPC)
		{
			pc+=2;
		}
		incrementPC = 1;
	}
	move(33, 0);
	printw("End of game. Press any key to continue.");
	getch();
    endwin();
	return 0;
}