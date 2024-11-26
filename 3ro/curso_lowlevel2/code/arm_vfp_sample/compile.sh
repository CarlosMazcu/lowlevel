
gcc -c -g -Wall -O2 -mfpu=vfp -o obj/vfp_sample.o vfp_sample.c
gcc -c -g -Wall -O2 -o obj/chrono.o chrono.c
gcc -c -g -o obj/rotate_inner.o rotate_inner_simple.s
gcc -c -g -o obj/display_vertices.o display_vertices.s


cd obj

gcc -o ../vfp_sample.elf vfp_sample.o rotate_inner.o display_vertices.o chrono.o  -lm -lSDL 
