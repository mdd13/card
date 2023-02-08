#pragma once

#include "common.cpp"

#include "game_text.cpp"
#include "game_image.cpp"

void GameDrawImage(GameImage *image,
				   i32 rx, i32 ry,
				   i32 rw, i32 rh) {
	SDL_Rect rect = {};
	rect.x = rx;
	rect.y = ry;
	rect.w = rw;
	rect.h = rh;

	SDL_RenderCopy(global_renderer, image->sdl_texture, 0, &rect);
}

void GameDrawImageRotate(GameImage *image,
						 i32 rx, i32 ry,
						 i32 rw, i32 rh, f64 angle) {
	SDL_Rect rect = {};
	rect.x = rx;
	rect.y = ry;
	rect.w = rw;
	rect.h = rh;

	SDL_RenderCopyEx(global_renderer, image->sdl_texture, 0,
					 &rect, angle, 0, SDL_FLIP_NONE);
}

void GameDrawText(GameFont *font,
				  char *text,
				  i32 rx, i32 ry,
				  i32 color) {
	if (!text) {
		return;
	}
	
	SDL_Color sdl_color = {};
    sdl_color.r = (u8)((color & 0xff000000) >> 24);
	sdl_color.g = (u8)((color & 0x00ff0000) >> 16);
	sdl_color.b = (u8)((color & 0x0000ff00) >> 8);
	sdl_color.a = (u8)((color & 0x000000ff) >> 0);

	SDL_Surface *surface = TTF_RenderUTF8_Solid(font->data, text, sdl_color);
	Assert(surface);

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

void GameDrawColor(i32 color,
				   i32 rx, i32 ry,
				   i32 rw, i32 rh) {
	SDL_Rect rect = {};
	rect.x = rx;
	rect.y = ry;
	rect.w = rw;
	rect.h = rh;

	u8 r = (u8)((color & 0xff000000) >> 24);
	u8 g = (u8)((color & 0x00ff0000) >> 16);
	u8 b = (u8)((color & 0x0000ff00) >> 8);
	u8 a = (u8)((color & 0x000000ff) >> 0);

	SDL_SetRenderDrawColor(global_renderer, r, g, b, a);
	SDL_RenderFillRect(global_renderer, &rect);
}
