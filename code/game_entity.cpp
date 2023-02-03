#pragma once

#include "common.cpp"
#include "game.cpp"
#include "game_input.cpp"

struct GameEntity {
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

void GameEntityRender(GameEntity *entity, SDL_Renderer *renderer, bool hightlight) {
	if (entity->invisible) {
		return;
	}
	SDL_Rect rect = {};
	rect.x = entity->x;
	rect.y = entity->y;
	rect.w = entity->w;
	rect.h = entity->h;
	
	if (entity->texture) {
		SDL_RenderCopy(renderer, entity->texture, 0, &rect);
	}

	if (hightlight) {
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 100);
		SDL_RenderFillRect(renderer, &rect);
	}
}

bool GameEntityPointIn(int x, int y, int w, int h, int px, int py) {
	SDL_Point point;
	point.x = px;
	point.y = py;

	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;

	return SDL_PointInRect(&point, &rect);
}

bool GameEntityMouseIn(GameEntity *entity, GameMouse *mouse) {
	return GameEntityPointIn(entity->x, entity->y, entity->w, entity->h, mouse->x, mouse->y);
}

/// NOTE(): Basic Minkowski-based Collision Detection
bool GameEntityCollsionChecking(GameEntity *e0, GameEntity *e1) {
	int rx = e0->x / 2;
	int ry = e0->y / 2;

	int px = e0->x + rx;
	int py = e0->y + ry;

	int x = e1->x - rx;
	int y = e1->y - ry;
	int w = e1->w + e0->x;
	int h = e1->h + e0->y;

	return GameEntityPointIn(x, y, w, h, px, py);
}

void GameEntityGrab(GameEntity *entity, GameInput *input) {
	if (!input->mouse.left.is_down) {
		entity->is_grabbed = false;
		entity->mouse_rel_x = -1;
		entity->mouse_rel_y = -1;
	} else {
		entity->is_grabbed = true;
		if (entity->mouse_rel_x == -1) {
			entity->mouse_rel_x = input->mouse.x - entity->x;
			entity->mouse_rel_y = input->mouse.y - entity->y;
		}
		entity->x = input->mouse.x - entity->mouse_rel_x;
		entity->y = input->mouse.y - entity->mouse_rel_y;
	}
}

#define MAX_ENTITY 1000

struct GameEntityPool {
	bool       initialized;
	GameEntity data[MAX_ENTITY];
	size_t     len;
	size_t     used;
};

void GameEntityPoolInit(GameEntityPool *gel) {
	gel->len = 0;
	gel->used = 0;
	gel->initialized = true;
	memset(gel->data, 0, MAX_ENTITY);
}

void GameEntityPoolResize(GameEntityPool *gel, size_t len) {
	gel->len = len;
}

GameEntity *GameEntityPoolGet(GameEntityPool *gel, int idx) {
	if (idx < 0) {
		Assert(gel->len > gel->used);
		return &gel->data[gel->used++];
	}

	Assert(idx < gel->len);
	return &gel->data[idx];
}
