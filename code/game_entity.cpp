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

INTERNAL inline bool PointInRect(int px, int py,
								 int x, int y, int w, int h) {
	return (px >= x && px < x + w) && (py >= y && py < y + h);
}

bool GameEntityMouseIn(GameEntity *entity, GameMouse *mouse) {
	return PointInRect(mouse->x, mouse->y,
					   entity->x, entity->y,
					   entity->w, entity->h);
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

	return PointInRect(px, py, x, y, w, h);
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

void GameEntityPoolInit(GameEntityPool *entity_pool) {
	entity_pool->len = 0;
	entity_pool->used = 0;
	entity_pool->initialized = true;
	memset(entity_pool->data, 0, MAX_ENTITY);
}

void GameEntityPoolResize(GameEntityPool *entity_pool, size_t len) {
	entity_pool->len = len;
}

GameEntity *GameEntityPoolGet(GameEntityPool *entity_pool, int idx) {
	if (idx < 0) {
		Assert(entity_pool->len > entity_pool->used);
		return &entity_pool->data[entity_pool->used++];
	}

	Assert(idx < entity_pool->len);
	return &entity_pool->data[idx];
}
