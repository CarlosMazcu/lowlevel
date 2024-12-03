
clang -c -g -Wall -O0 -o obj/zoom.o zoom.c
clang -c -g -Wall -O0 -o obj/chrono.o chrono.c
clang -c -g -Wno-trigraphs -o obj/bosque.o bosque.c

cd obj

clang -o ../zoom.elf bosque.o zoom.o chrono.o  -lSDL -lm 
