#pragma once

#include "common.cpp"
#include "game.cpp"
#include "game_input.cpp"

struct GameLayout {
	int x;
	int y;
	int w;
	int h;

	bool is_grabbed;
	bool invisible;
	
	int mouse_rel_x;
	int mouse_rel_y;

	SDL_Texture *texture;
};

void GameLayoutRender(GameLayout *layout, SDL_Renderer *renderer, bool hightlight) {
	if (layout->invisible) {
		return;
	}
	SDL_Rect rect = {};
	rect.x = layout->x;
	rect.y = layout->y;
	rect.w = layout->w;
	rect.h = layout->h;
	
	if (layout->texture) {
		SDL_RenderCopy(renderer, layout->texture, 0, &rect);
	}

	if (hightlight) {
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 100);
		SDL_RenderFillRect(renderer, &rect);
	}
}

INTERNAL inline bool PointInRect(int px, int py,
								 int x, int y, int w, int h) {
	return (px >= x && px < x + w) && (py >= y && py < y + h);
}

bool GameLayoutMouseIn(GameLayout *layout, GameMouse *mouse) {
	return PointInRect(mouse->x, mouse->y,
					   layout->x, layout->y,
					   layout->w, layout->h);
}

/// NOTE(): Basic Minkowski-based Collision Detection
bool GameLayoutCollsionChecking(GameLayout *e0, GameLayout *e1) {
	int rx = e0->x / 2;
	int ry = e0->y / 2;

	int px = e0->x + rx;
	int py = e0->y + ry;

	int x = e1->x - rx;
	int y = e1->y - ry;
	int w = e1->w + e0->x;
	int h = e1->h + e0->y;

	return PointInRect(px, py, x, y, w, h);
}

void GameLayoutGrab(GameLayout *layout, GameInput *input) {
	if (!input->mouse.left.is_down) {
		layout->is_grabbed = false;
		layout->mouse_rel_x = -1;
		layout->mouse_rel_y = -1;
	} else {
		layout->is_grabbed = true;
		if (layout->mouse_rel_x == -1) {
			layout->mouse_rel_x = input->mouse.x - layout->x;
			layout->mouse_rel_y = input->mouse.y - layout->y;
		}
		layout->x = input->mouse.x - layout->mouse_rel_x;
		layout->y = input->mouse.y - layout->mouse_rel_y;
	}
}

#define MAX_LAYOUT 1000

struct GameLayoutPool {
	bool       initialized;
	GameLayout data[MAX_LAYOUT];
	size_t     len;
	size_t     used;
};

void GameLayoutPoolInit(GameLayoutPool *layout_pool) {
	layout_pool->len = 0;
	layout_pool->used = 0;
	layout_pool->initialized = true;
	memset(layout_pool->data, 0, MAX_LAYOUT);
}

void GameLayoutPoolExpand(GameLayoutPool *layout_pool, size_t len) {
	layout_pool->len += len;
	Assert(layout_pool->len < MAX_LAYOUT);
}

GameLayout *GameLayoutPoolGet(GameLayoutPool *layout_pool, int idx) {
	if (idx < 0) {
		Assert(layout_pool->len > layout_pool->used);
		return &layout_pool->data[layout_pool->used++];
	}

	Assert(idx < layout_pool->len);
	return &layout_pool->data[idx];
}
