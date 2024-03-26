
@typedef struct {
@  unsigned char  x, y;     @ 0, 1
@  unsigned char  alive;    @ 2
@  char           PADDING; // Just to be sure sizeof(Particle) is a factor of 4
@} Particle; sizeof() == 4

@typedef struct
@{
@  unsigned short x, y; 0, 2
@} Source; sizeof() == 4



@ void SeedParticles (const Source* sources, Particle* particles, int nparticles, unsigned short* screen)
@ r0 -> sources
@ r1 -> particles
@ r2 -> nparticles
@ r3 -> screen

    .global SeedParticles

SeedParticles:
    stmdb   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}

    mov     r8,#8
seed_loop:

    ldrb    r4,[r1,#2]          @ particles->alive
    cmp     r4,#0
    bne     particle_alive

    ldrh    r4,[r0,#0]          @ int x = sources->x
    ldrh    r5,[r0,#2]          @ int y = sources->y

    mov     r7,#240             @ r7 = 240
    mul     r6,r5,r7            @ r6 = y * 240 (y * SCREEN_W) 
    add     r6,r6,r4            @ r6 = r6 + x
    mov     r6,r6,lsl #1        @ r6 * sizeof(short)    
    ldrh    r6,[r3,r6]          @ pixel = screen[x + y * SCREEN_W]
    
    cmp     r6,#0               @ if(pixel == BLACK)
    
    streqb    r4,[r1,#0]        @ particles->x = x
    streqb    r5,[r1,#1]        @ particles->y = y
    moveq     r6,#1    
    streqb    r6,[r1,#2]        @ particles->alive = 1
    add       r0,r0,#4          @ sources++
    sub       r8,r8,#1          @ nsources--     
    
particle_alive:
    add     r1,r1,#4            @ particles++
    subs     r2,r2,#1            @ nparticles--

    beq     seed_done
    cmp     r8,#0
    bne     seed_loop
seed_done:

    ldmia   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}
    bx      lr

@ void UpdateParticles (Particle* particles, int nparticles, unsigned short* screen)
@ r0 -> particles
@ r1 -> nparticles
@ r2 -> screen

    .global UpdateParticles
UpdateParticles:
    stmdb   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}
    mov     r11,#240            @ r6 = SCREEN_W(240)

    mov     r12, #0xFF00       
    orr     r12, r12, #0xFF     
    
    do_while_loop:
    ldrb    r3,[r0,#2]          @ particles->alive
    cmp     r3,#1
    bne     particle_alive_update

    ldrb    r4,[r0]             @ int x = particles->x
    ldrb    r5,[r0,#1]          @ int y = particles->y
    mul     r7,r11,r5           @ r7 = SCREEN_W * y
    add     r7,r7,r4            @ r7 = x + SCREEN_W * y
    add     r7,r7,r7            @ (x + SCREEN_W * y) * 2 -> (short)
    add     r7,r2,r7            @ current = &screen[x + SCREEN_W * y]
    
    add     r6,r7,#480          @ down* = (SCREEN_W(r6) * 2) + current(r7)
    mov     r8,#33              @ int new_x(r8) = NO_SLOT(33)

    ldrh    r9,[r6]             @ r9 = down[0]
    cmp     r9,#0               @ if(r9 == BLACK)
    bne particles_left
    mov     r8,#0               @ new_x = 0
    b end_find
particles_left:
    ldrh    r9,[r6,#2]          @ r9 = down[1]
    cmp     r9,#0               @ if(r9 == BLACK)
    bne     particles_right
    mov     r8,#1               @ new_x = 1
    b end_find
particles_right:
    ldrh    r9,[r6,#-2]         @ r9 = down[-1]
    cmp     r9,#0               @ if(r9 == BLACK)
    bne     end_find
    mov     r8,#-1              @ new_x = -1
end_find:

    cmp     r8,#33              @ if(new_x != NO_SLOT(33))
    beq     kill_particle

    mov     r9,#0               @ r9 = BLACK
    strh    r9,[r7]             @ *current = BLACK(r9)
    add     r10,r8,r8           @ r10 = new_x * 2


    strh    r12,[r6,r10]         @ down[new_x] = WHITE
    add     r4,r4,r8            @ x = x + new_x    
    strb    r4,[r0]             @ particles->x = (x + new_x)(r4)
    add     r5,r5,#1            @ y = y + 1
    strb    r5,[r0,#1]          @ particles->y = y
    b next_particle 
kill_particle:
    mov     r9,#0
    strb    r9,[r0,#2]          @ particles->alive = 0
next_particle:

particle_alive_update:
    add     r0,r0,#4            @ particles++
    subs    r1,r1,#1            @ nparticles--
    bne     do_while_loop    
    ldmia   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}
    bx      lr


@  //do {//
@    if (particles->alive) {
@      int x = particles->x;
@      int y = particles->y;
@      unsigned short* current = &screen [x + SCREEN_W * y];
@      unsigned short* down = current + SCREEN_W;
@      int new_x = NO_SLOT;
@      if (down[0] == BLACK)
@        new_x = 0;      // There is room just down the current position
@      //else if (down[-1] == BLACK)
@          //new_x = -1;      // There is room letf-down 
@        else if (down[1] == BLACK)
@            new_x = 1;      // There is room right-down 
@      if (new_x != NO_SLOT) {
@        *current = BLACK; // Clean the old position
@        down[new_x] = WHITE; // Ocuppy the new position, color white
@        particles->x = x + new_x;
@        particles->y = y + 1;
@      }
@      else


@        particles->alive = 0;
@    }
@
@    particles++;
@    nparticles--;
@
@  //} while (nparticles != 0);//  // Do while there is particles left

