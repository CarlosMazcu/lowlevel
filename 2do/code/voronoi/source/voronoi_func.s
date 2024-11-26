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
@Voronoi:
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
    mov r1,r8       @ r1 = npoints    
    mov r0,r7       @ r0 = points
    @@@@@@@@@@
    
    bl Closest          @ r0 = Closest(points, npoints, x, y)
    
    mov r0,r0,lsl #1     @ r0 = r0 << 2
    ldrh r10,[r9,r0]     @ r10 = palette[c];
    strh r10,[r6],#2     @ *screen = r10 / screen++

    add r5,r5,#1    @ x++
    cmp r5,#240
    blt loop_x

    add r4,r4,#1    @ y++
    cmp r4,#160
    blt loop_y

    ldmia   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}
    bx      lr

@@@@@@@ INLINE @@@@@@@@


    .global Voronoi
Voronoi:
    stmdb   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}


    mov r4,#0       @ int y = 0
loop_y2:
    mov r5,#0       @ int x = 0
loop_x2:
    
    mov r6,#1       @ int i = 0
    mov r7,#0       @ int closest = 0

    ldr r8,[r1,#0]  @ int xd = points[0].x 
    sub r8,r5       @ points[0].x - x
    ldr r9,[r1,#4]  @ int yd = points[0].y
    sub r9,r4       @ points[0].y - y
    
    mul r11,r8,r8   @ xd * xd
    mul r12,r9,r9   @ yd * yd
    add r11,r11,r12 @ min_dist = xd * xd + yd * yd
    
loop_closest:
    mov r12,r6,lsl#3    @ i * 8 
    add r12,r12,r1      @ points[i]
    
    ldr r8,[r12]  @ xd = points[i].x
    sub r8,r8,r5    @ xd = points[i].x - x;
    

    ldr r9,[r12,#4]    @ yd = points[i].y
    sub r9,r9,r4       @ yd = points[i].y - y;

    mul r8,r8,r8    @ xd = xd * xd
    mul r9,r9,r9    @ yd = yd * yd
    add r14,r8,r9   @ int dist = xd * xd + yd * yd;

    cmp r14,r11
    bge g_equal_dist   @ if (dist < min_dist) {
    
    mov r11,r14        @ min_dist = dist;
    mov r7,r6          @ closest = i;

g_equal_dist:

    add r6,#1   @i++
    cmp r6,r2
    blt loop_closest
   
    mov r7,r7,lsl #1     @ r7 = r7 << 2
    ldrh r10,[r3,r7]     @ r10 = palette[r7];
    strh r10,[r0],#2     @ *screen = r10 / screen++

    add r5,r5,#1    @ x++
    cmp r5,#240
    blt loop_x2

    add r4,r4,#1    @ y++
    cmp r4,#160
    blt loop_y2

    ldmia   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}
    bx      lr



@ WIDTH 240
@ HEIGHT 160

@ {
@   int x,y;
@   for(y = 0; y<SCREEN_HEIGHT; y++) {
@     for(x = 0; x<SCREEN_WIDTH; x++) {

@        int i = 0;
@        int closest = 0;    // 1st point is closest, by default
@        int xd = points[0].x - x;
@        int yd = points[0].y - y;
@        int min_dist = xd * xd + yd * yd;
@        for (i = 1; i < npoints; i++) {
@            xd = points[i].x - x;
@            yd = points[i].y - y;
@            int dist = xd * xd + yd * yd;
@            if (dist < min_dist) {
@                min_dist = dist;
@                closest = i;
@            }
@        }


@       *screen = palette [closest];
@       screen++;
@     }
@   }
@ }
