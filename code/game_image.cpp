#pragma once

#include "common.cpp"

struct GameImage {
	SDL_Texture *sdl_texture;
};

void GameImageLoadFile(GameImage *image, const char *file) {
	image->sdl_texture = IMG_LoadTexture(global_renderer, file);
	Assert(image->sdl_texture);	
}

void GameImageFreeContents(GameImage *image) {
	SDL_DestroyTexture(image->sdl_texture);
}
