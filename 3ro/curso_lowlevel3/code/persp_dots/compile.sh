
gcc -c -g -Wall -O2 -o buildobjs/persp_dots.o persp_dots.c

cd buildobjs
gcc -o ../persp_dots.elf persp_dots.o -lSDL -lm
