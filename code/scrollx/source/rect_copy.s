
.global RectCopy

@ void RectCopy (unsigned short* dst, const unsigned short* src, int src_width)
@ r0 -> unsigned short* dst
@ r1 -> const unsigned short* src
@ r2 -> int src_width

RectCopy:
  stmdb sp!,{r4,r5}
  
  mov   r3,#0       @ y = 0
loop_y:
  cmp   r3,#160     @ SCREEN_HEIGHT
  bge   exit

  mov   r4,#0       @ x = 0
loop_x:
  cmp   r4,#240     @ SCREEN_WIDTH
  beq   exit_loop_x
  ldrh  r5,[r1]     @ tmp  = *src
  strh  r5,[r0]     @ *dst = tmp
  add   r0,r0,#2    @ dst++
  add   r1,r1,#2    @ src++
  add   r4,r4,#1    @ x++
  b  loop_x

exit_loop_x:
  add   r1,r1,r2,lsl #1   @ src += src_width * sizeof(short)
  sub   r1,r1,#480        @ src -= SCREEN_WIDTH * sizeof(short)
  add   r3,r3,#1          @ y++
  b  loop_y

exit:
  ldmia sp!,{r4,r5}
  bx  lr

