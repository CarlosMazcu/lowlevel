
test.o:     file format elf32-littlearm


Disassembly of section .text:

00000000 <nasty>:
   0:	e52db004 	push	{r11}		; (str r11, [r13, #-4]!)
   4:	e28db000 	add	r11, r13, #0
   8:	e24dd014 	sub	r13, r13, #20
   c:	e50b0010 	str	r0, [r11, #-16]
  10:	e50b1014 	str	r1, [r11, #-20]	; 0xffffffec
  14:	e3a03000 	mov	r3, #0
  18:	e50b3008 	str	r3, [r11, #-8]
  1c:	ea00000a 	b	4c <nasty+0x4c>
  20:	e51b3008 	ldr	r3, [r11, #-8]
  24:	e1a03103 	lsl	r3, r3, #2
  28:	e51b2010 	ldr	r2, [r11, #-16]
  2c:	e0823003 	add	r3, r2, r3
  30:	e5933000 	ldr	r3, [r3]
  34:	e51b200c 	ldr	r2, [r11, #-12]  @ <---
  38:	e0823003 	add	r3, r2, r3
  3c:	e50b300c 	str	r3, [r11, #-12]
  40:	e51b3008 	ldr	r3, [r11, #-8]
  44:	e2833001 	add	r3, r3, #1
  48:	e50b3008 	str	r3, [r11, #-8]
  4c:	e51b2008 	ldr	r2, [r11, #-8]
  50:	e51b3014 	ldr	r3, [r11, #-20]	; 0xffffffec
  54:	e1520003 	cmp	r2, r3
  58:	bafffff0 	blt	20 <nasty+0x20>
  5c:	e51b300c 	ldr	r3, [r11, #-12]
  60:	e1a00003 	mov	r0, r3
  64:	e24bd000 	sub	r13, r11, #0
  68:	e49db004 	pop	{r11}		; (ldr r11, [r13], #4)
  6c:	e12fff1e 	bx	r14
