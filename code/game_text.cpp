#pragma once

#include "SDL2/SDL_ttf.h"
#include "common.cpp"


struct GameFont {
	TTF_Font *data;
	i32      size;
};

void GameFontInit(GameFont *font,
				  const char *path,
				  i32 size) {
	font->data = TTF_OpenFont(path, size);
	Assert(font->data);

	font->size = size;
}
