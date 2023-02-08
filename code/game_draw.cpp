#pragma once

#include "common.cpp"

#include "game_text.cpp"
#include "game_image.cpp"

void GameDrawImage(GameImage *image,
				   int rx, int ry,
				   int rw, int rh) {
	SDL_Rect rect = {};
	rect.x = rx;
	rect.y = ry;
	rect.w = rw;
	rect.h = rh;

	SDL_RenderCopy(global_renderer, image->sdl_texture, 0, &rect);
}

void GameDrawText(GameFont *font,
				  char *text,
				  int rx, int ry,
				  int color) {
	SDL_Color sdl_color = {};
	sdl_color.r = (uint8_t)((color & 0xff0000) >> 24);
	sdl_color.g = (uint8_t)((color & 0x00ff00) >> 16);
	sdl_color.b = (uint8_t)((color & 0x0000ff) >> 8);
	sdl_color.a = (uint8_t)((color & 0x0000ff) >> 0);

	SDL_Surface *surface = TTF_RenderUTF8_Solid(font->data, text, sdl_color);
	SDL_Rect rect = {};
	rect.x = rx;
	rect.y = ry;
	rect.w = surface->w;
	rect.h = surface->h;
	SDL_Texture *texture = SDL_CreateTextureFromSurface(global_renderer, surface);
	SDL_RenderCopy(global_renderer, texture, 0, &rect);

	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

void GameDrawColor(int color,
				   int rx, int ry,
				   int rw, int rh) {
	SDL_Rect rect = {};
	rect.x = rx;
	rect.y = ry;
	rect.w = rw;
	rect.h = rh;

	uint8_t r = (uint8_t)((color & 0xff0000) >> 24);
	uint8_t g = (uint8_t)((color & 0x00ff00) >> 16);
	uint8_t b = (uint8_t)((color & 0x0000ff) >> 8);
	uint8_t a = (uint8_t)((color & 0x0000ff) >> 0);

	SDL_SetRenderDrawColor(global_renderer, r, g, b, a);
	SDL_RenderFillRect(global_renderer, &rect);
}
