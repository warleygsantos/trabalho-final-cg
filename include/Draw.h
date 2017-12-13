#ifndef DRAW_H_INCLUDED
#define DRAW_H_INCLUDED

#include "Objeto3D.h"
#include "ColorSystem.h"
#include <vector>

#ifdef _WIN32
#include <SDL.h>
#include <SDL_ttf.h>
#endif

#ifdef __linux
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#endif

typedef struct No
{
    int x;
    int y;
} No;

void desenhaPoligono(SDL_Renderer *, Objeto3D*, int, int);

#endif // DRAW_H_INCLUDED
