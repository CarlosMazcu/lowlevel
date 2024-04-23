@ void Voronoi (unsigned short* screen, const TPoint* points, int npoints, 
                const unsigned short* palette);

@ r0 -> unsigned short* screen
@ r1 -> TPoint* points
@ r2 -> int npointa
@ r3 -> unsigned short* palette

@ TPoint
@ {
@    unsigned int x; #0
@    unsigned int y; #4
@ }#8


    .extern Closest
    .global Voronoi
Voronoi:

    mov r4,#0       @ int y = 0
loop_y:
    mov r5,#0       @ int x = 0
loop_x:
    @ int c = Closest(points, npoints, x, y)



    add r0,r0,#2    @ screen++
    add r5,r5,#1    @ x++
    cmp r5,#240
    blt loop_x

    add r4,r4,#1    @ y++
    cmp r4,#160
    blt loop_y



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
