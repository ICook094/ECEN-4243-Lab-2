#define N_CUR ( (CURRENT_STATE.CPSR>>31) & 0x00000001 )
#define Z_CUR ( (CURRENT_STATE.CPSR>>30) & 0x00000001 )
#define C_CUR ( (CURRENT_STATE.CPSR>>29) & 0x00000001 )
#define V_CUR ( (CURRENT_STATE.CPSR>>28) & 0x00000001 )
#define N_NXT ( (NEXT_STATE.CPSR>>31) & 0x00000001 )
#define Z_NXT ( (NEXT_STATE.CPSR>>30) & 0x00000001 )
#define C_NXT ( (NEXT_STATE.CPSR>>29) & 0x00000001 )
#define V_NXT ( (NEXT_STATE.CPSR>>28) & 0x00000001 )

#define N_N 0x80000000
#define Z_N 0x40000000
#define C_N 0x20000000
#define V_N 0x10000000

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"

//returns 0 if condition is not met
int check_cond(int CC) {
	switch(CC) {
		case 0: return Z_CUR; //EQ
		case 1: return ~Z_CUR&1; //NE
		case 2: return C_CUR; //CS/HS
		case 3: return ~C_CUR&1; //CC/LO
		case 4: return N_CUR; //MI
		case 5: return ~N_CUR&1; //PL
		case 6: return V_CUR; //VS
		case 7: return ~V_CUR&1; //VC
		case 8: return C_CUR&(~Z_CUR&1); //HI
		case 9: return (~C_CUR&1)|Z_CUR; //LS
		case 10: return N_CUR == V_CUR; //GE
		case 11: return N_CUR != V_CUR; //LT
		case 12: return (Z_CUR == 0)&&(N_CUR == V_CUR); //GT
		case 13: return (Z_CUR == 1)||(N_CUR != V_CUR); //LE
		case 14: return 1; //AL
		case 15: return 1; //Unconditional
		default: return -1; //somethings gone wrong if you get here
	}
}

//sign extends an n bit number to make them compatible with c signed arithmatic
int sign_extend(int x, int n) {
	int m = 1U << (n - 1);
	int r = (x ^ m) - m;
	return r;
}

//Handles addressing modes to prevent large chunks of repeated code.
//Given the I bit and Operand2, returns an appropriately shifted/rotated operand.
//C does not have a reliable way of implementing arithmetic versus logical shifts,
//because the implementation of >> and << are compiler dependant. 
//TODO: Add logic to handle the shifts setting the carry flag
int addressing_mode_handler(int I, int Operand2) {
	
	int Src2;

	if(I == 0) {
		int sh = (Operand2 & 0x00000060) >> 5;
		int shamt5 = (Operand2 & 0x00000F80) >> 7;
		int bit4 = (Operand2 & 0x00000010) >> 4;
		int Rm = Operand2 & 0x0000000F;
		int Rs = (Operand2 & 0x00000F00) >> 8;
		
		if(bit4 == 0) {
			switch(sh) {
				case 0: Src2 = CURRENT_STATE.REGS[Rm] << shamt5;
				break;
				case 1: Src2 = CURRENT_STATE.REGS[Rm] >> shamt5;
				break;
				case 2: Src2 = CURRENT_STATE.REGS[Rm] >> shamt5;
				break;
				case 3: Src2 = (CURRENT_STATE.REGS[Rm] >> shamt5) | (CURRENT_STATE.REGS[Rm] << (32 - shamt5));
				break;
			}
		}
		else {
			switch(sh) {
				case 0: Src2 = CURRENT_STATE.REGS[Rm] << CURRENT_STATE.REGS[Rs];
				break;
				case 1: Src2 = CURRENT_STATE.REGS[Rm] >> CURRENT_STATE.REGS[Rs];
				break;
				case 2: Src2 = CURRENT_STATE.REGS[Rm] >> CURRENT_STATE.REGS[Rs];
				break;
				case 3: Src2 = (CURRENT_STATE.REGS[Rm] >> CURRENT_STATE.REGS[Rs]) | (CURRENT_STATE.REGS[Rm] << (32 - CURRENT_STATE.REGS[Rs]));
				break;
			}
		}
	}

	if(I == 1) {
		int rotate = Operand2 >> 8;
		int Imm = Operand2 & 0x000000FF;
		Src2 = Imm>>2*rotate|(Imm<<(32-2*rotate));
	}
	
	return Src2;
}

//same as above, but for memory instructions
int addressing_mode_mem(int I, int Operand2) {
	if(I == 0)
		return sign_extend(Operand2, 12);
	else {
		int sh = (Operand2 & 0x00000060) >> 5;
		int shamt5 = (Operand2 & 0x00000F80) >> 7;
		int Rm = Operand2 & 0x0000000F;
		int Src2;
		switch(sh) {
			case 0: Src2 = CURRENT_STATE.REGS[Rm] << shamt5;
			break;
			case 1: Src2 = CURRENT_STATE.REGS[Rm] >> shamt5;
			break;
			case 2: Src2 = CURRENT_STATE.REGS[Rm] >> shamt5;
			break;
			case 3: Src2 = (CURRENT_STATE.REGS[Rm] >> shamt5) | (CURRENT_STATE.REGS[Rm] << (32 - shamt5));
			break;
		}
		return Src2;
	}
}

//Sets the status flags given the result of an operation
//to prevent large chunks of repeated code.
//Since C and V behaviors are instruction dependant, they are implemented below as needed
int set_flags(int num) {
	if(num < 0)
		NEXT_STATE.CPSR |= N_N;
	if(num == 0)
		NEXT_STATE.CPSR |= Z_N;
	
	return 0;
}

//Same as above, but with additional logic to handle C and V flags
//for addition/subtraction related instructions
int set_flags_as(int cur, long long num, int Src2, int Rn) {
		if(cur < 0)
			NEXT_STATE.CPSR |= N_N;
		if(cur == 0)
			NEXT_STATE.CPSR |= Z_N;
		if(num > 0xFFFFFFFF)
			NEXT_STATE.CPSR |= C_N;
		if((~(CURRENT_STATE.REGS[Rn] ^ Src2)) & (CURRENT_STATE.REGS[Rn] ^ num) & 0x80000000)
			NEXT_STATE.CPSR |= V_N;
}

int ADD (int Rd, int Rn, int Operand2, int I, int S) {
	long long sum;
	int Src2 = addressing_mode_handler(I, Operand2);

	sum = CURRENT_STATE.REGS[Rn] + Src2;
	int cur = (int)sum;
	NEXT_STATE.REGS[Rd] = cur;
	
	//Add/sub operations have additionally logic for setting the C and V flags
	//So do not make use of the standard set_flags method
	if(S == 1)
		set_flags_as(cur, sum, Src2, Rn);
		
	return 0;
}

int ADC (int Rd, int Rn, int Operand2, int I, int S) {
	long long sum;
	int Src2 = addressing_mode_handler(I, Operand2);
	
	//The difference between ADD and ADC is the addition of the current value of the carry flag.
	sum = CURRENT_STATE.REGS[Rn] + Src2 + C_CUR;
	int cur = (int)sum;
	NEXT_STATE.REGS[Rd] = cur;
	
	if(S == 1)
		set_flags_as(cur, sum, Src2, Rn);
	
	return 0;
}

int AND (int Rd, int Rn, int Operand2, int I, int S) {
	int cur;
	int Src2 = addressing_mode_handler(I, Operand2);
	
	cur = CURRENT_STATE.REGS[Rn] & Src2;
	NEXT_STATE.REGS[Rd] = cur;

	if(S == 1)
		set_flags(cur);
		
	return 0;
}

  
int B (int imm24) {
	//printf("DEBUG: pc = 0x%X\n", CURRENT_STATE.PC);
    NEXT_STATE.PC = (CURRENT_STATE.PC + 4) + (imm24 << 2);
	//printf("DEBUG: off = %d, next pc = 0x%X\n", imm24<<2, NEXT_STATE.PC);

}

int BL (int imm24) {
	//printf("DEBUG: pc = 0x%X\n", CURRENT_STATE.PC);
    NEXT_STATE.PC = (CURRENT_STATE.PC + 4) + (imm24 << 2);
	//printf("DEBUG: off = %d, next pc = 0x%X\n", imm24<<2, NEXT_STATE.PC);
    NEXT_STATE.REGS[14] = (CURRENT_STATE.PC + 8) - 4;
}

int BIC (int Rd, int Rn, int Operand2, int I, int S) { 
	int cur;
	int Src2 = addressing_mode_handler(I, Operand2);
	
	cur = CURRENT_STATE.REGS[Rn] & ~Src2;
	NEXT_STATE.REGS[Rd] = cur;

	if(S == 1)
		set_flags(cur);
		
	return 0;
}

//Sets flags based on Rn + Src2 without storing the result
int CMN (int Rd, int Rn, int Operand2, int I, int S){
	long long sum;
	int Src2 = addressing_mode_handler(I, Operand2);
	
	sum = CURRENT_STATE.REGS[Rn] + Src2;
	int cur = (int)sum;
	
	set_flags_as(cur, sum, Src2, Rn);
}

//Same as CMN but with Rn - Src2 instead of Rn + Src2
int CMP (int Rd, int Rn, int Operand2, int I, int S) {
	long long sum;
	int Src2 = addressing_mode_handler(I, Operand2);
	
	sum = CURRENT_STATE.REGS[Rn] - Src2;
	int cur = (int)sum;
	//printf("DEBUG: cur = %d\n", cur);
	
	set_flags_as(cur, sum, Src2, Rn);
	
	//printf("DEBUG: CPSR = %X\n", NEXT_STATE.CPSR);
	return 0;

}

int EOR (int Rd, int Rn, int Operand2, int I, int S){
	int cur;
	int Src2 = addressing_mode_handler(I, Operand2);
	
	cur = CURRENT_STATE.REGS[Rn] ^ Src2;
	NEXT_STATE.REGS[Rd] = cur;

	if(S == 1)
		set_flags(cur);
		
	return 0;
	
}

int LDR(int Rd, int Rn, int Operand2, int I) {
	int Src2 = addressing_mode_mem(I, Operand2);
	int Adr = CURRENT_STATE.REGS[Rn] + Src2;
	NEXT_STATE.REGS[Rd] = mem_read_32(Adr);
	//printf("DEBUG: adr = 0x%X, src2 = %d, rd = %d\n", Adr, Src2, NEXT_STATE.REGS[Rd]);
	return 0;
}

int LDRB(int Rd, int Rn, int Operand2, int I) {
	int Src2 = addressing_mode_mem(I, Operand2);
	int Adr = CURRENT_STATE.REGS[Rn] + Src2;
	NEXT_STATE.REGS[Rd] = mem_read_32(Adr)&0xFF;
	//printf("DEBUG: adr = 0x%X, src2 = %d, rd = %d\n", Adr, Src2, NEXT_STATE.REGS[Rd]&0xFF);
	return 0;
}

//This MOV function implements the functionality of shift and rotate functions through use of addressing modes.
//This includes the LSL, LSR, ASR, RRX, and ROR instructions.
//Consequentially, the data_process() will incorrectly identify all of these as MOV.
int MOV (int Rd, int Rn, int Operand2, int I, int S){
	int Src2 = addressing_mode_handler(I, Operand2);
	NEXT_STATE.REGS[Rd] = Src2;
	
	if(S == 1)
		set_flags(Src2);
	return 0;
}

int MVN (int Rd, int Rn, int Operand2, int I, int S){
	int cur;
	//int Src2 = addressing_mode_handler(I, Operand2);
	
	cur = ~CURRENT_STATE.REGS[Rn];
	NEXT_STATE.REGS[Rd] = cur;

	if(S == 1)
		set_flags(cur);
		
	return 0;
}

int ORR (int Rd, int Rn, int Operand2, int I, int S){
	int cur;
	int Src2 = addressing_mode_handler(I, Operand2);
	
	cur = CURRENT_STATE.REGS[Rn] | Src2;
	NEXT_STATE.REGS[Rd] = cur;

	if(S == 1)
		set_flags(cur);
		
	return 0;
}

int RSB (int Rd, int Rn, int Operand2, int I, int S){
	long long dif;
	int Src2 = addressing_mode_handler(I, Operand2);

	dif = Src2 - CURRENT_STATE.REGS[Rn];
	int cur = (int)dif;
	NEXT_STATE.REGS[Rd] = cur;
	
	if(S == 1)
		set_flags_as(cur, dif, Src2, Rn);
		
	return 0;
}

int RSC (int Rd, int Rn, int Operand2, int I, int S){
	long long dif;
	int Src2 = addressing_mode_handler(I, Operand2);

	dif = Src2 - CURRENT_STATE.REGS[Rn] - (~C_CUR&1);
	int cur = (int)dif;
	NEXT_STATE.REGS[Rd] = cur;
	
	if(S == 1)
		set_flags_as(cur, dif, Src2, Rn);
		
	return 0;
}

int SBC (int Rd, int Rn, int Operand2, int I, int S){
	long long dif;
	int Src2 = addressing_mode_handler(I, Operand2);

	//SBC is identical to SUB save for the inverted C bit term.
	dif = CURRENT_STATE.REGS[Rn] - Src2 - (~C_CUR&1);
	int cur = (int)dif;
	NEXT_STATE.REGS[Rd] = cur;
	
	if(S == 1)
		set_flags_as(cur, dif, Src2, Rn);
		
	return 0;
}

int STR(int Rd, int Rn, int Operand2, int I) {
	int Src2 = addressing_mode_mem(I, Operand2);
	int Adr = CURRENT_STATE.REGS[Rn] + Src2;
	//printf("DEBUG: adr = 0x%X, src2 = %d, rd = %d\n", Adr, Src2, CURRENT_STATE.REGS[Rd]);
	mem_write_32(Adr, CURRENT_STATE.REGS[Rd]);
	return 0;
}

int STRB(int Rd, int Rn, int Operand2, int I) {
	int Src2 = addressing_mode_mem(I, Operand2);
	int Adr = CURRENT_STATE.REGS[Rn] + Src2;
	//printf("DEBUG: adr = 0x%X, src2 = %d, rd = %d\n", Adr, Src2, CURRENT_STATE.REGS[Rd]&0xFF);
	mem_write_32(Adr, (CURRENT_STATE.REGS[Rd]&0xFF));
	return 0;
}

int SUB (int Rd, int Rn, int Operand2, int I, int S){
	long long dif;
	int Src2 = addressing_mode_handler(I, Operand2);

	dif = CURRENT_STATE.REGS[Rn] - Src2;
	int cur = (int)dif;
	NEXT_STATE.REGS[Rd] = cur;
	
	if(S == 1)
		set_flags_as(cur, dif, Src2, Rn);
	
	return 0;
}

int TEQ (int Rd, int Rn, int Operand2, int I, int S){
	int cur;
	int Src2 = addressing_mode_handler(I, Operand2);
	
	cur = CURRENT_STATE.REGS[Rn] ^ Src2;

	set_flags(cur);
		
	return 0;
}

int TST (int Rd, int Rn, int Operand2, int I, int S){
	int cur;
	int Src2 = addressing_mode_handler(I, Operand2);
	
	cur = CURRENT_STATE.REGS[Rn] & Src2;

	set_flags(cur);
		
	return 0;
}

int SWI (char* i_){return 0;}
