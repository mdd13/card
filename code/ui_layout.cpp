#pragma once

#include "common.cpp"

#include "game_text.cpp"
#include "game_draw.cpp"
#include "game_image.cpp" 
#include "game_input.cpp"


#define UiInteractionNone 0
#define UiInteractionDrag ((1 << 1) - 1)
#define UiInteractionSelect ((1 << 2) - 1)


struct UiInteraction {
	i32 flag;
	i32 mouse_rel_x;
	i32 mouse_rel_y;
};

struct UiLayout {
	i32 x;
	i32 y;
	i32 w;
	i32 h;
	b32 invisible;

	UiInteraction interaction;
};

void UiLayoutDrawImage(UiLayout *layout,
					   GameImage *image) {
	if (layout->invisible) {
		return;
	}
	GameDrawImage(image, layout->x, layout->y, layout->w, layout->h);
}

void UiLayoutDrawColor(UiLayout *layout, i32 color) {
	if (layout->invisible) {
		return;
	}
	GameDrawColor(color, layout->x, layout->y, layout->w, layout->h);
}

void UiLayoutDrawText(UiLayout *layout, GameFont *font,
					  char *text, i32 color) {
	if (layout->invisible) {
		return;
	}
	GameDrawText(font, text, layout->x, layout->y, color);
}

inline b32 PointInRect(i32 px, i32 py,
					    i32 x, i32 y, i32 w, i32 h) {
	return (px >= x && px < x + w) && (py >= y && py < y + h);
}

b32 UiLayoutMouseIn(UiLayout *layout, GameMouse *mouse) {
	return PointInRect(mouse->x, mouse->y,
					   layout->x, layout->y,
					   layout->w, layout->h);
}

/// NOTE: Basic Minkowski-based Collision Detection
b32 UiLayoutCollsionChecking(UiLayout *e0, UiLayout *e1) {
	i32 rx = e0->x / 2;
	i32 ry = e0->y / 2;

	i32 px = e0->x + rx;
	i32 py = e0->y + ry;

	i32 x = e1->x - rx;
	i32 y = e1->y - ry;
	i32 w = e1->w + e0->x;
	i32 h = e1->h + e0->y;

	return PointInRect(px, py, x, y, w, h);
}

inline b32 UiLayoutIsDrag(UiLayout *layout) {
	return (b32)(layout->interaction.flag & UiInteractionDrag);
}

void UiLayoutDrag(UiLayout *layout, GameMouse *mouse) {
	if (!UiLayoutIsDrag(layout)) {
		layout->interaction.flag |=	UiInteractionDrag;
		layout->interaction.mouse_rel_x = mouse->x - layout->x;
		layout->interaction.mouse_rel_y = mouse->y - layout->y;
	} 
	layout->x = mouse->x - layout->interaction.mouse_rel_x;
	layout->y = mouse->y - layout->interaction.mouse_rel_y;
}


void UiLayoutReleaseDrag(UiLayout *layout) {
	layout->interaction.flag ^= UiInteractionDrag;
}

inline b32 UiLayoutIsSelect(UiLayout *layout) {
	return (b32)(layout->interaction.flag & UiInteractionSelect);
}

void UiLayoutSelect(UiLayout *layout) {
	layout->interaction.flag |= UiInteractionDrag;
}

void UiLayoutUnselect(UiLayout *layout) {
	layout->interaction.flag ^= UiInteractionDrag;
}
