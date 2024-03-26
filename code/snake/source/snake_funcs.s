@ SNAKE_LEN (100)

@ typedef struct
@ {
@   int x, y;
@ } Slab;

@ typedef struct
@ {
@   Slab* slabs;
@   unsigned short len;
@   unsigned short head_slab;
@   int   speed_x, speed_y;
@ } Snake;

@ static Slab snake_slabs [SNAKE_LEN];




@ int UpdateSnake (int keypad, Snake* snake, unsigned short* screen)
@ r0 -> keypad
@ r1 -> snake
@ r2 -> screen

@    .global UpdateSnake
UpdateSnake:
    stmdb   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}
    mov r3,#0     @ int crash = 0
    ldr r4,[r1]   @ r4 = snake->slabs 
    str r4,[r4]   @ slabs = r4

    ldmia   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}
    bx      lr







@int UpdateSnake (int keypad, Snake* snake, unsigned short* screen)
@{
@  int crash = 0;
@  Slab* slabs = snake->slabs;
@  int head_slab = snake->head_slab;
@  // Cut the tail
@  int tail = snake->head_slab + 1;
@  if (tail >= snake->len)
@    tail = 0;

@  screen[slabs[tail].x + slabs[tail].y * 240] = 0; // BLACK 

@   return crash;
@ }
