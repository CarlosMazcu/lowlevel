@ SNAKE_LEN (100)

@ typedef struct
@ {
@   int x; 0
@   int y; 4
@ } Slab;  8

@ typedef struct
@ {
@   Slab* slabs;    0
@   unsigned short len; 4
@   unsigned short head_slab; 6
@   int   speed_x; 8
@   int   speed_y; 12
@ } Snake; 16

@ static Slab snake_slabs [SNAKE_LEN];




@ int UpdateSnake (int keypad, Snake* snake, unsigned short* screen)
@ r0 -> keypad
@ r1 -> snake
@ r2 -> screen

@    .global UpdateSnake
UpdateSnake:
    stmdb   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}
    mov  r3,#0               @ int crash = 0
    ldr  r4,[r1]             @ Slab* slabs = snake->slabs 
    ldrh r5,[r1,#6]          @ int head_slab = snake->head_slab
    add  r6,r5,#1            @ int tail = snake->head_slab + 1;

    ldrh r7,[r1,#4]          @ r7 = snale->len
    cmp  r6,r7               @ if(tail >= snake_len)                                                                                                                          
    movge  r6,#0             @ tail = 0   

    add  r10,r4,r6,lsl #3    @ r10 = slabs[tail]                                                                                        
    ldr  r7,[r10]            @ slabs[tail].x
    ldr  r8,[r10,#4]              @  slabs[tail].y                                                           
    mov  r9, #240                  @  r9 = 240                  
    mul  r8,r9,r8                 @  slabs[tail].y * 240
    add  r8,r8,r7                 @  slabs[tail].x + slabs[tail].y * 240
    add  r8,r8,r8                 @  r8 * 2    
    mov  r7,#0                    @  r7 = 0
    strh r7,[r2,r8]               @  screen[slabs[tail].x + slabs[tail].y * 240] = 0; // BLACK    
                                  
    ldrh r7,[r1,#6]               @ r7 = snake->head_slab
    add  r7,r4,r7,lsl #3          @ r7 = slabs[snake->head_slab]
    ldr  r8,[r7]                  @  int new_x = slabs[snake->head_slab].x;
    ldr  r9,[r7,#4]               @  int new_y = slabs[snake->head_slab].y;

    add  r5,r5,#1                 @  head_slab++;

    ldrh r7,[r1,#4]           @ r7 = snake->len
    cmp  r5,r7                @ if(head_slab >= snake->len)
    movge r5,#0               @ head_slab = 0
    strh   r5,[r1,#6]          @ snake->head_slab = head_slab

@  int speed_x = snake->speed_x;
@  int speed_y = snake->speed_y;
    mov   r0,r3
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

@  int new_x = slabs[snake->head_slab].x;
@  int new_y = slabs[snake->head_slab].y;

@  head_slab++;
@  if (head_slab >= snake->len)
@    head_slab = 0;
@  snake->head_slab = head_slab;

@@  int speed_x = snake->speed_x;
@@  int speed_y = snake->speed_y;

@@  snake->speed_x = speed_x;
@@  snake->speed_y = speed_y;

@@  new_x += speed_x;
@@  new_y += speed_y;

@@  unsigned short* head_pix = screen + (new_x + new_y * 240);
@@  if (*head_pix == 0) {
@@    *head_pix = 0xffff; // Paint head, white

@@    slabs[head_slab].x = new_x;
@@    slabs[head_slab].y = new_y;

@@  } 
@@  else
@@    crash = 1;


@  keypad = crash;
@  return keypad;
@ }
