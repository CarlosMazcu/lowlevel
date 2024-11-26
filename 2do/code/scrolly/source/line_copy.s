
.global LineCopy

@ void LineCopy (unsigned short* dst, const unsigned short* src, int l)

@ r0 -> unsigned short* dst
@ r1 -> const unsigned short* src
@ r2 -> int l

LineCopy:
    cmp   r2,#0   @ compare r2 with 0
    beq   exit    @ branch if equal; if(l==0) branch to "exit"

loop:
    ldrh  r3,[r1]   @ read an unsigned short from r1 used as pointer
    strh  r3,[r0]   @ write an unsigned short to memory, using r0 as pointer
    add   r0,r0,#2  @ pointer +2, to point to next unsigned short. Equivalent to C "dst++"
    add   r1,r1,#2  @ same for src
    subs r2,r2,#1   @ l--, and update the condition register ("s" sufix). Similar to "cmp" with 0
    bne  loop       @ branch if not equal to 0, branch to "loop"

exit:
    bx  lr          @ return to caller

