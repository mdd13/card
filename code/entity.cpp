#pragma once

#include "common.h"
#include "game.cpp"

struct Entity {
	int x;
	int y;
	int w;
	int h;

	bool is_grabbed;
	int mouse_rel_x;
	int mouse_rel_y;

	SDL_Texture *texture;
};

void EntityRender(Entity *entity, SDL_Renderer *renderer, bool have_outline) {
	if (!entity->texture) {
		return;
	}

	SDL_Rect rect = {};
	rect.x = entity->x;
	rect.y = entity->y;
	rect.w = entity->w;
	rect.h = entity->h;
	SDL_RenderCopy(renderer, entity->texture, 0, &rect);

	if (have_outline) {
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 100);
		SDL_RenderFillRect(renderer, &rect);
	}
}

bool EntityMouseIn(Entity *entity, GameMouse *mouse) {
	SDL_Point point;
	point.x = mouse->x;
	point.y = mouse->y;

	SDL_Rect rect;
	rect.x = entity->x;
	rect.y = entity->y;
	rect.w = entity->w;
	rect.h = entity->h;

	return SDL_PointInRect(&point, &rect);
}

void EntityGrab(Entity *entity, GameInput *input) {
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
