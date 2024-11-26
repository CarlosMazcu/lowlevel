
clang -c -g -O2 -o buildobjs/basic_sdl_ex.o basic_sdl_ex.c

cd buildobjs
clang -o ../basic_sdl_ex.elf basic_sdl_ex.o -lSDL 
