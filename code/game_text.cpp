/*
 * Author: Mai Duy Duong
 */

#pragma once

#include "SDL2/SDL_ttf.h"
#include "common.cpp"


struct GameFont {
	TTF_Font *data;
	int      size;
};

void GameFontInit(GameFont *font,
				  const char *path,
				  int size) {
	font->data = TTF_OpenFont(path, size);
	Assert(font->data);
	font->size = size;
}
