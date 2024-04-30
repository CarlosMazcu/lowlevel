@ void Voronoi (unsigned short* screen, const TPoint* points, int npoints, 
@                const unsigned short* palette);

@ r0 -> unsigned short* screen
@ r1 -> TPoint* points
@ r2 -> int npoints
@ r3 -> unsigned short* palette

@ TPoint
@ {
@    unsigned int x; #0
@    unsigned int y; #4
@ }#8


    .extern Closest
    .global Voronoi
Voronoi:
    stmdb   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}

    mov r6,r0       @ r6 = screen
    mov r7,r1       @ r7 = points
    mov r8,r2       @ r8 = npoints
    mov r9,r3       @ r9 = palette
    

    mov r4,#0       @ int y = 0
loop_y:
    mov r5,#0       @ int x = 0
loop_x:
    
    @@@@@@@@@@
    mov r3,r4       @ r3 = y
    mov r2,r5       @ r2 = x
    mov r1,r2       @ r1 = npoints    
    mov r0,r1       @ r0 = points
    @@@@@@@@@@
    
    @ bl Closest
    @ int c = Closest(points, npoints, x, y)

    add  r2,r2,r2     @ x = x * 2
    ldrh r10,[r9,r2]  @ r10 = palette[x];
    strh r10,[r6]     @ *screen = r10



    add r8,r8,#2    @ screen++
    add r5,r5,#1    @ x++
    cmp r5,#240
    blt loop_x

    add r4,r4,#1    @ y++
    cmp r4,#160
    blt loop_y

    ldmia   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}
    bx      lr



@ WIDTH 240
@ HEIGHT 160

@ {
@   int x,y;
@   for(y = 0; y<SCREEN_HEIGHT; y++) {
@     for(x = 0; x<SCREEN_WIDTH; x++) {
@       int c = Closest (points, npoints, x, y);
@       *screen = palette [c];
@       screen++;
@     }
@   }
@ }
