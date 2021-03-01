/***************************************************************/
/*                                                             */
/*   ARMv4-32 Instruction Level Simulator                      */
/*                                                             */
/*   ECEN 4243                                                 */
/*   Oklahoma State University                                 */
/*                                                             */
/***************************************************************/

//Code completed by:
//Alex Sensintaffaf
//Joshua Minton

#ifndef _SIM_ISA_H_
#define _SIM_ISA_H_


//Helper function for conditional execution
//Returns 1 if input condition is met, and 0 if it's not
int check_cond(int CC);
//sign extends a n bit number to make compitable with c signed arithmatic
int sign_extend(int x, int n);
//The provided version of this function was modified to be able to set the C and V flags if the S bit is set.
int ADD (int Rd, int Rn, int Operand2, int I, int S);
int ADC (int Rd, int Rn, int Operand2, int I, int S);
int AND (int Rd, int Rn, int Operand2, int I, int S);
int ASR (int Rd, int Rn, int Operand2, int I, int S);
int B (int imm24);
int BIC (int Rd, int Rn, int Operand2, int I, int S);
int BL (int imm24);
int CMN (int Rd, int Rn, int Operand2, int I, int S);
int CMP (int Rd, int Rn, int Operand2, int I, int S);
int EOR (int Rd, int Rn, int Operand2, int I, int S);
int LDR (int Rd, int Rn, int Operand2, int I);
int LDRB (int Rd, int Rn, int Operand2, int I);
int LSL (int Rd, int Rn, int Operand2, int I, int S);
int LSR (int Rd, int Rn, int Operand2, int I, int S);
//int MLA (char* i_);
int MOV (int Rd, int Rn, int Operand2, int I, int S);
//int MUL (char* i_);
int MVN (int Rd, int Rn, int Operand2, int I, int S);
int ORR (int Rd, int Rn, int Operand2, int I, int S);
int ROR (int Rd, int Rn, int Operand2, int I, int S);
int RRX (int Rd, int Rn, int Operand2, int I, int S);
int RSB (int Rd, int Rn, int Operand2, int I, int S);
int RSC (int Rd, int Rn, int Operand2, int I, int S);
int SBC (int Rd, int Rn, int Operand2, int I, int S);
int STR (int Rd, int Rn, int Operand2, int I);
int STRB (int Rd, int Rn, int Operand2, int I);
int SUB (int Rd, int Rn, int Operand2, int I, int S);
int TEQ (int Rd, int Rn, int Operand2, int I, int S);
int TST (int Rd, int Rn, int Operand2, int I, int S);
int SWI (char* i_);

#endif
