#include <string.h>
#include <stdio.h>

enum Opcode {CLS, RET, JPADDR, CALLADDR, SEVXBYTE, SNEVXBYTE, SEVXVY, LDVXBYTE, ADDVXBYTE, LDVXVY, ORVXVY, ANDVXVY, XORVXVY, ADDVXVY, SUBVXVY, SHRVXVY, SUBNVXVY, SHLVXVY, SNEVXVY, LDIADDR, JPV0ADDR, RNDVXBYTE, DRWVXVYNIBBLE, SKPVX, SKNPVX, LDVXDT, LDVXK, LDDTVX, LDSTVX, ADDIVX, LDFVX, LDBVX, LDIVX, LDVXI, NOP};

union Instruction
{
	unsigned short whole;
	struct {
		unsigned char lowByte;
		unsigned char highByte;
	};
	struct {
		unsigned char nibble0: 4;
		unsigned char nibble1: 4;
		unsigned char nibble2: 4;
		unsigned char nibble3: 4;
	};
	struct {
		unsigned short bottom12Bits: 12;
	};
};

union Instruction getInstruction(unsigned short pc, char memory[])
{
	union Instruction instruction;
	instruction.whole = (((short)memory[pc]) << 8) + (short)memory[pc+1];
	return instruction;
}

enum Opcode decode(union Instruction instruction)
{
	switch(instruction.nibble3)
	{
		case 0x0: switch(instruction.lowByte)
			{
				case 0xE0: return CLS;
				case 0xEE: return RET;
				default: return NOP;
			}
		case 0x1: return JPADDR;
		case 0x2: return CALLADDR;
		case 0x3: return SEVXBYTE;
		case 0x4: return SNEVXBYTE;
		case 0x5: return SEVXVY;
		case 0x6: return LDVXBYTE;
		case 0x7: return ADDVXBYTE;
		case 0x8: switch(instruction.nibble0)
			{
				case 0x0: return LDVXVY;
				case 0x1: return ORVXVY;
				case 0x2: return ANDVXVY;
				case 0x3: return XORVXVY;
				case 0x4: return ADDVXVY;
				case 0x5: return SUBVXVY;
				case 0x6: return SHRVXVY;
				case 0x7: return SUBNVXVY;
				case 0xE: return SHLVXVY;
				default: return NOP;
			}
		case 0x9: return SNEVXVY;
		case 0xA: return LDIADDR;
		case 0xB: return JPV0ADDR;
		case 0xC: return RNDVXBYTE;
		case 0xD: return DRWVXVYNIBBLE;
		case 0xE: switch(instruction.lowByte)
			{
				case 0x9E: return SKPVX;
				case 0xA1: return SKNPVX;
				default: return NOP;
			}
		case 0xF: switch(instruction.lowByte)
			{
				case 0x07: return LDVXDT;
				case 0x0A: return LDVXK;
				case 0x15: return LDDTVX;
				case 0x18: return LDSTVX;
				case 0x1E: return ADDIVX;
				case 0x29: return LDFVX;
				case 0x33: return LDBVX;
				case 0x55: return LDIVX;
				case 0x65: return LDVXI;
				default: return NOP;
			}
		default: return NOP;
	}
}

const char * opcodeString(enum Opcode opcode)
{
	switch(opcode)
	{
		case CLS:
			{
				return "CLS";
			}
		case RET:
			{
				return "RET";
			}
		case JPADDR:
			{
				return "JPADDR";
			}
		case CALLADDR:
			{
				return "CALLADDR";
			}
		case SEVXBYTE:
			{
				return "SEVXBYTE";
			}
		case SNEVXBYTE:
			{
				return "SNEVXBYTE";
			}
		case SEVXVY:
			{
				return "SEVXVY";
			}
		case LDVXBYTE:
			{
				return "LDVXBYTE";
			}
		case ADDVXBYTE:
			{
				return "ADDVXBYTE";
			}
		case LDVXVY:
			{
				return "LDVXVY";
			}
		case ORVXVY:
			{
				return "ORVXVY";
			}
		case ANDVXVY:
			{
				return "ANDVXVY";
			}
		case XORVXVY:
			{
				return "XORVXVY";
			}
		case ADDVXVY:
			{
				return "ADDVXVY";
			}
		case SUBVXVY:
			{
				return "SUBVXVY";
			}
		case SHRVXVY:
			{
				return "SHRVXVY";
			}
		case SUBNVXVY:
			{
				return "SUBNVXVY";
			}
		case SHLVXVY:
			{
				return "SHLVXVY";
			}
		case SNEVXVY:
			{
				return "SNEVXVY";
			}
		case LDIADDR:
			{
				return "LDIADDR";
			}
		case JPV0ADDR:
			{
				return "JPV0ADDR";
			}
		case RNDVXBYTE:
			{
				return "RNDVXBYTE";
			}
		case DRWVXVYNIBBLE:
			{
				return "DRWVXVYNIBBLE";
			}
		case SKPVX:
			{
				return "SKPVX";
			}
		case SKNPVX:
			{
				return "SKNPVX";
			}
		case LDVXDT:
			{
				return "LDVXDT";
			}
		case LDVXK:
			{
				return "LDVXK";
			}
		case LDDTVX:
			{
				return "LDDTVX";
			}
		case LDSTVX:
			{
				return "LDSTVX";
			}
		case ADDIVX:
			{
				return "ADDIVX";
			}
		case LDFVX:
			{
				return "LDFVX";
			}
		case LDBVX:
			{
				return "LDBVX";
			}
		case LDIVX:
			{
				return "LDIVX";
			}
		case LDVXI:
			{
				return "LDVXI";
			}
		case NOP:
			{
				return "NOP";
			}
	}
}

void getParameters(union Instruction instruction, char * buffer)
{
	//sprintf(buffer, "");
	strcpy(buffer, "                 ");
	switch(decode(instruction))
	{
		case CLS:
			{
				break;
			}
		case RET:
			{
				break;
			}
		case JPADDR:
			{
				//sprintf(buffer, "0x%03X", instruction.bottom12Bits);
				break;
			}
		case CALLADDR:
			{
				break;
			}
		case SEVXBYTE:
			{
				/*
				unsigned char x = instruction.nibble2;
				unsigned char kk = instruction.lowByte;
				sprintf(buffer, "v%X    0x%02X", x, kk);
				*/
				break;
			}
		case SNEVXBYTE:
			{
				break;
			}
		case SEVXVY:
			{
				break;
			}
		case LDVXBYTE:
			{
				break;
			}
		case ADDVXBYTE:
			{
				break;
			}
		case LDVXVY:
			{
				break;
			}
		case ORVXVY:
			{
				break;
			}
		case ANDVXVY:
			{
				break;
			}
		case XORVXVY:
			{
				break;
			}
		case ADDVXVY:
			{
				break;
			}
		case SUBVXVY:
			{
				break;
			}
		case SHRVXVY:
			{
				break;
			}
		case SUBNVXVY:
			{
				break;
			}
		case SHLVXVY:
			{
				break;
			}
		case SNEVXVY:
			{
				break;
			}
		case LDIADDR:
			{
				break;
			}
		case JPV0ADDR:
			{
				break;
			}
		case RNDVXBYTE:
			{
				break;
			}
		case DRWVXVYNIBBLE:
			{
				unsigned char x = instruction.nibble2;
				unsigned char y = instruction.nibble1;
				unsigned char n = instruction.nibble0;
				sprintf(buffer, "v%X    v%X    0x%02X", x, y, n);
				break;
			}
		case SKPVX:
			{
				break;
			}
		case SKNPVX:
			{
				break;
			}
		case LDVXDT:
			{
				break;
			}
		case LDVXK:
			{
				break;
			}
		case LDDTVX:
			{
				break;
			}
		case LDSTVX:
			{
				break;
			}
		case ADDIVX:
			{
				break;
			}
		case LDFVX:
			{
				break;
			}
		case LDBVX:
			{
				break;
			}
		case LDIVX:
			{
				break;
			}
		case LDVXI:
			{
				break;
			}
		case NOP:
			{
				break;
			}
	}
}

/*
	Big switch statement for each instruction
	switch(opcode)
	{
		case CLS:
			{
				break;
			}
		case RET:
			{
				break;
			}
		case JPADDR:
			{
				break;
			}
		case CALLADDR:
			{
				break;
			}
		case SEVXBYTE:
			{
				break;
			}
		case SNEVXBYTE:
			{
				break;
			}
		case SEVXVY:
			{
				break;
			}
		case LDVXBYTE:
			{
				break;
			}
		case ADDVXBYTE:
			{
				break;
			}
		case LDVXVY:
			{
				break;
			}
		case ORVXVY:
			{
				break;
			}
		case ANDVXVY:
			{
				break;
			}
		case XORVXVY:
			{
				break;
			}
		case ADDVXVY:
			{
				break;
			}
		case SUBVXVY:
			{
				break;
			}
		case SHRVXVY:
			{
				break;
			}
		case SUBNVXVY:
			{
				break;
			}
		case SHLVXVY:
			{
				break;
			}
		case SNEVXVY:
			{
				break;
			}
		case LDIADDR:
			{
				break;
			}
		case JPV0ADDR:
			{
				break;
			}
		case RNDVXBYTE:
			{
				break;
			}
		case DRWVXVYNIBBLE:
			{
				break;
			}
		case SKPVX:
			{
				break;
			}
		case SKNPVX:
			{
				break;
			}
		case LDVXDT:
			{
				break;
			}
		case LDVXK:
			{
				break;
			}
		case LDDTVX:
			{
				break;
			}
		case LDSTVX:
			{
				break;
			}
		case ADDIVX:
			{
				break;
			}
		case LDFVX:
			{
				break;
			}
		case LDBVX:
			{
				break;
			}
		case LDIVX:
			{
				break;
			}
		case LDVXI:
			{
				break;
			}
		case NOP:
			{
				break;
			}
	}
*/


/*
int main()
{
	srand(clock());
	{
		union Instruction instruction;
		instruction.whole = (unsigned short int) rand();
		printf("0x%04X\n", instruction.whole);
		printf("0x%02X 0x%02X\n", instruction.highByte, instruction.lowByte);
		printf("0x%01X 0x%01X 0x%01X 0x%01X\n", instruction.nibble3, instruction.nibble2, instruction.nibble1, instruction.nibble0);
	}

	
	union Instruction instruction;
	instruction.whole = 0x00E0;
	printf("%d\n", decode(instruction));
	instruction.whole = 0x00EE;
	printf("%d\n", decode(instruction));
}
*/