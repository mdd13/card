#pragma once

#include "common.cpp"
#include "game_text.cpp"
#include "game_draw.cpp"
#include "game_image.cpp"
#include "game_input.cpp"

#define INTERACTION_HOVER (1 >> 1 - 1)
#define INTERACTION_CLICK (1 >> 2 - 1)
#define INTERACTION_DRAG  (1 >> 3 - 1)

struct UiInteraction {
	int mouse_rel_x;
	int mouse_rel_y;
};

struct UiLayout {
	int x;
	int y;
	int w;
	int h;

	bool is_grabbed;
	bool invisible;
	
	int mouse_rel_x;
	int mouse_rel_y;
};

void UiLayoutDrawImage(UiLayout *layout,
					   GameImage *image) {
	if (layout->invisible) {
		return;
	}
	GameDrawImage(image, layout->x, layout->y, layout->w, layout->h);
}

void UiLayoutDrawColor(UiLayout *layout, int32_t color) {
	if (layout->invisible) {
		return;
	}
	GameDrawColor(color, layout->x, layout->y, layout->w, layout->h);
}


void UiLayoutDrawText(UiLayout *layout, GameFont *font,
					  char *text,
					  int color) {
	if (layout->invisible) {
		return;
	}
	GameDrawText(font, text, layout->x, layout->y, color);
}

INTERNAL inline bool PointInRect(int px, int py,
								 int x, int y, int w, int h) {
	return (px >= x && px < x + w) && (py >= y && py < y + h);
}

bool UiLayoutMouseIn(UiLayout *layout, GameMouse *mouse) {
	return PointInRect(mouse->x, mouse->y,
					   layout->x, layout->y,
					   layout->w, layout->h);
}

/// NOTE(): Basic Minkowski-based Collision Detection
bool UiLayoutCollsionChecking(UiLayout *e0, UiLayout *e1) {
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

void UiLayoutGrab(UiLayout *layout, GameInput *input) {
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
