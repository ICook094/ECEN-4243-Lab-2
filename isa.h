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
//The provided version of this function was modified to be able to set the C and V flags if the S bit is set.
int ADD (int Rd, int Rn, int Operand2, int I, int S, int CC);
int ADC (int Rd, int Rn, int Operand2, int I, int S, int CC);
int AND (int Rd, int Rn, int Operand2, int I, int S, int CC);
int ASR (int Rd, int Rn, int Operand2, int I, int S, int CC);
int B (char* i_);
int BIC (int Rd, int Rn, int Operand2, int I, int S, int CC);
int BL (char* i_);
int CMN (int Rd, int Rn, int Operand2, int I, int S, int CC);
int CMP (int Rd, int Rn, int Operand2, int I, int S, int CC);
int EOR (int Rd, int Rn, int Operand2, int I, int S, int CC);
int LDR (char* i_);
int LDRB (char* i_);
int LSL (int Rd, int Rn, int Operand2, int I, int S, int CC);
int LSR (int Rd, int Rn, int Operand2, int I, int S, int CC);
int MLA (char* i_);
int MOV (int Rd, int Rn, int Operand2, int I, int S, int CC);
int MUL (char* i_);
int MVN (int Rd, int Rn, int Operand2, int I, int S, int CC);
int ORR (int Rd, int Rn, int Operand2, int I, int S, int CC);
int ROR (int Rd, int Rn, int Operand2, int I, int S, int CC);
int RRX (int Rd, int Rn, int Operand2, int I, int S, int CC);
int RSB (int Rd, int Rn, int Operand2, int I, int S, int CC);
int RSC (int Rd, int Rn, int Operand2, int I, int S, int CC);
int SBC (int Rd, int Rn, int Operand2, int I, int S, int CC);
int STR (char* i_);
int STRB (char* i_);
int SUB (int Rd, int Rn, int Operand2, int I, int S, int CC);
int TEQ (int Rd, int Rn, int Operand2, int I, int S, int CC);
int TST (int Rd, int Rn, int Operand2, int I, int S, int CC);
int SWI (char* i_);

#endif
