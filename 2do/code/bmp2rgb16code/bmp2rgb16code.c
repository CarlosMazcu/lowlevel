
#include <SDL.h>

static void pix_out (int w, int h, int r, int g, int b)
{
    static int cnt = 0;
    int rgb16 = (r >> 3) | ((g >> 3) << 5) | ((b >> 3) << 10);
    if (cnt == 0)
        fprintf (stdout, "const unsigned short pic[%d * %d] = {\n    ", w, h);

    fprintf (stdout, "0x%x,", rgb16);

    if ((cnt % 8) == 7)
        fprintf (stdout, "\n    ");
    cnt++;
}

int main (int a, char** b)
{
    int x, y;
    SDL_Surface *temp;
 
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
	    printf("Unable to initialize SDL: %s\n", SDL_GetError());
	    return 1;
    }

    temp = SDL_LoadBMP(b[1]);
    if (temp == NULL) {
	    printf("Unable to load bitmap: %s\n", SDL_GetError());
	    return 1;
    }
     
    SDL_LockSurface (temp);

    unsigned char* pix = (unsigned char*)temp->pixels;
    for (y=0; y<temp->h;y++)
        for (x=0; x<temp->w; x++)
            pix_out (temp->w, temp->h, *pix++, *pix++, *pix++);

    fprintf (stdout, "};\n\n");

    SDL_UnlockSurface (temp);

    SDL_FreeSurface(temp);

    return 1;
}
