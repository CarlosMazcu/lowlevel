
test.o:     file format elf32-littlearm


Disassembly of section .text:

00000000 <nasty>:
   0:	e3510000 	cmp	r1, #0
   4:	da000005 	ble	20 <nasty+0x20>
   8:	e1a03000 	mov	r3, r0
   c:	e0801101 	add	r1, r0, r1, lsl #2
  10:	e4930004 	ldr	r0, [r3], #4
  14:	e1530001 	cmp	r3, r1
  18:	e0822000 	add	r2, r2, r0
  1c:	1afffffb 	bne	10 <nasty+0x10>
  20:	e1a00002 	mov	r0, r2
  24:	e12fff1e 	bx	r14
