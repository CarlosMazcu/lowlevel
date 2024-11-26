@ void Paint (unsigned short* dst, unsigned char* sprite, int stride_pixels, unsigned int color)
@ r0 -> dst
@ r1 -> sprite
@ r2 -> stride_pixels
@ r3 -> color

    .global Paint
Paint:
    stmdb   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}


    mov  r7,#0x1f
    and  r8,r3,r7            @ ball_r = color & 0x1f
    and  r9,r7,r3,lsr #5     @ ball_g = (color >> 5) & 0x1f
    and  r10,r7,r3,lsr #10   @ ball_b = (color >> 10) & 0x1f     

    mov  r4,#0               @ int y = 0
loop_screen_y:
    cmp  r4,#16     
    bge  end_loop_screen_y   @ y < 16

    mov  r6,#0               @ int x = 0
loop_screen_x:
    cmp  r6,#16
    bge  end_loop_screen_x   @ x < 16
    ldrb r5,[r1],#1          @ int t = *sprite++
    cmp  r5,#0               @ if(t)
    beq  not_paint

    ldrh r11,[r0]            @ back_color = *dst

    and  r12,r11,r7          @ r = back_color & 0x1f;
    add  r3,r12,r12,lsl #1   @ r3 = (r << 1) + r
    add  r3,r3,r8            @ r3 = (r << 1) + r + ball_r
    mov  r3,r3,asr #2        @ r3 = r3 >> 2
    
    and  r12,r7,r11,lsr #5   @ g = (back_color >> 5) & 0x1f

    add  r14,r12,r12,lsl #1  @ r14 = (g << 1) + g
    add  r14,r14,r9          @ r14 = (g << 1) + g + ball_g
    mov  r14,r14,asr #2      @ r14 = r14 >> 2

    and  r5,r7,r11,lsr #10   @ b = (back_color >> 10) & 0x1f
    add  r12,r5,r5, lsl #1   @ r12 = (b << 1) + b
    add  r12,r12,r10         @ r12 = (b << 1) + b + ball_b
    mov  r12,r12,asr #2      @ r12 = r12 >>2

    mov  r12,r12,lsl #10     @ r12 = r12 << 10
    orr  r3,r3,r14,lsl #5    @ r | g << 5
    orr  r3,r3,r12           @ r | g << 5 | b << 10

    strh r3,[r0]             @ *dst = r | g << 5 | b << 10
not_paint:
    add  r0,r0,#2            @ dst++           
    add  r6,r6,#1            @ x++
    b loop_screen_x
end_loop_screen_x:

    sub  r6,r2,#16           @ stride_pixels - side
    add  r0,r0,r6,lsl #1     @ dst += ((stride_pixels - side)*2)
    add  r4,r4,#1            @ y++
    b loop_screen_y
end_loop_screen_y:
    ldmia   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}
    bx      lr

    
   



@ void Paint (unsigned short* dst, unsigned char* sprite, int stride_pixels, unsigned int color)
@ {
@    int ball_r = color & 0x1f;
@    int ball_g = (color >> 5) & 0x1f;
@    int ball_b = (color >> 10) & 0x1f;
@   int x, y;
@   int side = BALL_SIDE;
@   for (y=0; y<side; y++) {
@     for (x=0; x<side; x++) {
@       int t = *sprite++; // read solid/transparent
@       if (t) {
@       unsigned int back_color = *dst;
@       Unpack color
@       int r = back_color & 0x1f;
@       int g = (back_color >> 5) & 0x1f;
@       int b = (back_color >> 10) & 0x1f;


@       r = ((r << 1) + r + ball_r) >> 2;
       @// g = ((g << 1) + g + ball_g) >> 2;
       @// b = ((b << 1) + b + ball_b) >> 2;


@        *dst = r | (g << 5) | (b<<10);  // *dst = 0xFFFF;
@       }
@       dst++;
@     }
@
@     dst += stride_pixels - side;
@   } 



