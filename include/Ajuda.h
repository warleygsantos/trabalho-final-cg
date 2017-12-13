#ifndef AJUDA_H_INCLUDED
#define AJUDA_H_INCLUDED

#ifdef _WIN32
#include <SDL.h>
#include <SDL_ttf.h>
#endif

#ifdef __linux
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#endif

#include <string>
#include <array>
#include <vector>

void SProjecao(SDL_Renderer *, int, int);
SDL_Surface *criaAjuda(int, int);

SDL_Surface *imprimeTexto(TTF_Font *, std::string, SDL_Color, int, int);
#endif // AJUDA_H_INCLUDED
