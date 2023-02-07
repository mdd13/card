/*
 * Author: Mai Duy Duong
 */

#pragma once

#include "common.cpp"
#include "mem.cpp"


struct GameFont {
	TTF_Font *data;
	int      size;
};

struct GameText {
	int x;
	int y;
	int w;
	int h;
	char     *data;
	GameFont font;
};

void GameFontInit(GameFont *font,
				  const char *path,
				  int size) {
	font->data = TTF_OpenFont(path, size);
	Assert(font->data);
	font->size = size;
}

void GameTextUpdateText(GameText *game_text, char *new_text) {
	game_text->data = new_text;
}

void GameTextRender(GameText *game_text, SDL_Renderer *renderer, int color) {
	SDL_Color sdl_color = {};
	sdl_color.r = (uint8_t)((color & 0xff0000) >> 16);
	sdl_color.g = (uint8_t)((color & 0x00ff00) >> 8);
	sdl_color.b = (uint8_t)((color & 0x0000ff) >> 0);
	sdl_color.a = 0xff;

	SDL_Surface *surface = TTF_RenderUTF8_Solid(
		game_text->font.data,
		game_text->data,
		sdl_color
		);
	Assert(surface);

	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

	game_text->w = surface->w;
	game_text->h = surface->h;	
	
	SDL_Rect rect = {};
	rect.x = game_text->x;
	rect.y = game_text->y;
	rect.w = surface->w;
	rect.h = surface->h;
	SDL_RenderCopy(renderer, texture, 0, &rect);

	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}
