; fib.s
; March 1, 2021
; Josh Minton & Alex Sensintaffaf
; This code iterates through the fibonacci sequence until a signed overflow is reached.
.text
MOV r0, #1
MOV r1, #2

LOOP:
ADDS r0, r0, r1
BVS END
ADDS r1, r0, r1
BVS END
B LOOP

END:
SWI #10