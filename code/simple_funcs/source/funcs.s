@ --------------------------
@int Biggest (int a, int b)
@ r0 -> a
@ r1 -> b
@ return in r0

    .global Biggest
Biggest:
    cmp r1,r0
    movgt r0,r1   @ res = b
    bx lr

@ --------------------------

@ int Smallest(int a, int b)
@ r0 -> a
@ r1 -> b

    .global Smallest
Smallest:
    cmp r1,r0
    movlt r0,r1
    bx lr

@ --------------------------

@ int TotalOfArrat (int *array, int len)
@ r0 -> *array
@ r1 -> len
@ r2 -> res
@ r3 -> v

    .global TotalOfArray
TotalOfArray:
    ldr r2,[r0],#4  @ int res = *array++
    sub r1,r1,#1    @ len--
total_of_not_equal_loop:
    cmp r1,#0       
    beq total_of_exit_not_equal_loop
    ldr r3,[r0],#4  @ int v = *array++
    add r2,r2,r3    @ res = res + v
    sub r1,r1,#1    @ len--
    b total_of_not_equal_loop
total_of_exit_not_equal_loop:
    mov r0,r2
    bx lr

@ --------------------------

@ int BiggestOfArray (int *array, int len)
@ r0 -> *array
@ r1 -> len
@ r2 -> res
@ r3 -> v

    .global BiggestOfArray
BiggestOfArray:
    ldr r2,[r0],#4  @ int res = *array++
    sub r1,r1,#1    @ l--
biggest_of_not_equal_loop:
    cmp r1,#0
    beq biggest_of_exit_not_equal_loop
    ldr r3,[r0],#4  @ int v = *array++
    cmp r3,r2       @ if(v > res)
    movgt r2,r3     @ res = v
    sub r1,r1,#1    @ len--
    b biggest_of_not_equal_loop
biggest_of_exit_not_equal_loop:
    mov r0,r2       @ return res
    bx lr

@ --------------------------

@ int SmallestOfArray (int *array, int len)
@ r0 -> *array
@ r1 -> len
@ r2 -> res
@ r3 -> v

    .global SmallestOfArray
SmallestOfArray:
    ldr r2,[r0],#4  @ int res = *array++
    sub r1,r1,#1    @ len--
smallest_of_not_equal_loop:
    cmp r1,#0
    beq smallest_of_exit_not_equal_loop
    ldr r3,[r0],#4   @ int v = *array++
    cmp r3,r2       @ if(v < res)
    movlt r2,r3     @ res = v
    sub r1,r1,#1    @ len--
    b smallest_of_not_equal_loop
smallest_of_exit_not_equal_loop:
    mov r0,r2       @ return res
    bx lr




































