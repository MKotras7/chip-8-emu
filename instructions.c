enum opcode {CLS, RET, JPADDR, CALLADDR, SEVXBYTE, SNEVXBYTE, SEVXVY, LDVXBYTE, ADDVXBYTE, LDVXVY, ORVXVY, ANDVXVY, XORVXVY, ADDVXVY, SUBVXVY, SHRVXVY, SUBNVXVY, SHLVXVY, SNEVXVY, LDIADDR, JPV0ADDR, RNDVXBYTE, DRWVXVYNIBBLE, SKPVX, SKNPVX, LDVXDT, LDVXK, LDDTVX, LDSTVX, ADDIVX, LDFVX, LDBVX, LDIVX, LDVXI, NOP};

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

enum opcode decode(union Instruction instruction)
{
	switch(instruction.nibble3)
	{
		case 0x0: switch(instruction.lowByte)
			{
				case 0xE0: return CLS;
				case 0xEE: return RET;
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
			}
		default: return NOP;
	}
}

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