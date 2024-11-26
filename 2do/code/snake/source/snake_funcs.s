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

    .global UpdateSnake
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
    mov  r9, #240                 @  r9 = 240                  
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
    strh r5,[r1,#6]           @ snake->head_slab = head_slab

    ldr  r10,[r1,#8]            @ int speed_x = snake->speed_x;
    ldr  r11,[r1,#12]           @ int speed_y = snake->speed_y;

    cmp r0,#0                   @  if (keypad != 0) {
    beq not_keypad
    mov r10,#0                  @ speed_x = 0
    mov r11,#0                  @ speed_y = 0

    ands r6,r0,#1               @ if (keypad & 1)
    movne r10,#1               @ speed_x = 1;

    ands r6,r0,#2               @ if (keypad & 2)
    movne r10,#-1                @ speed_x = -1;                      
              
    ands r6,r0,#4               @ if (keypad & 4)
    movne r11,#-1               @ speed_y = -1;            

    ands r6,r0,#8               @ if (keypad & 8)
    movne r11,#1                @ speed_y = 1;              

    str  r10,[r1,#8]            @ snake->speed_x = speed_x;
    str  r11,[r1,#12]           @ snake->speed_y = speed_y;

not_keypad:

    add r8,r8,r10               @  new_x += speed_x;
    add r9,r9,r11               @  new_y += speed_y;

    mov r12,#240           @ r12 = 240
    mul r12,r9,r12         @ r12 = new_y * 240
    add r12,r12,r8          @ r12 = new_x + new_y * 240
    add r12,r2,r12,lsl #1  @ unsigned short* head_pix = screen + (new_x + new_y * 240);
    ldrh r14,[r12]         @ r14 = *head_pix  
    cmp  r14,#0            @ @if (*head_pix == 0)
    bne  crash  
    mov  r10,#0xFF00
    orr  r10,r10,#0xFF       @ r10 = white
    strh r10,[r12]           @ *head_pix = 0xffff /*WHITE*/;

    mov r11, #0             @ r11 = 0
    add r11,r4,r5,lsl #3    @ r11 = Slabs[head_slabs] 
    str r8,[r11]            @ slabs[head_slab].x = new_x
    str r9,[r11,#4]         @ slabs[head_slab].y = new_y;
crash:
    cmp  r14,#0            @ if (*head_pix != 0)/*else*/
    beq  not_crash         
    mov  r3,#1             @    crash = 1;
not_crash:
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

@  int speed_x = snake->speed_x;
@  int speed_y = snake->speed_y;



@@  if (keypad != 0) {
@@    speed_x = speed_y = 0;
@@    if (keypad & 1)
@@      speed_x = 1;
@@    if (keypad & 2)
@@      speed_x = -1;
@@    if (keypad & 4)
@@      speed_y = -1;
@@    if (keypad & 8)
@@      speed_y = 1;
@@      snake->speed_x = speed_x;
@@      snake->speed_y = speed_y;
@@    }

@  new_x += speed_x;
@  new_y += speed_y;

@  unsigned short* head_pix = screen + (new_x + new_y * 240);
@  if (*head_pix == 0) {
@    *head_pix = 0xffff; // Paint head, white

@    slabs[head_slab].x = new_x;
@    slabs[head_slab].y = new_y;

@  } 
@  else
@    crash = 1;


@  keypad = crash;
@  return keypad;
@ }
