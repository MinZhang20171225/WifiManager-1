#pragma once

#include "basic.h"

#define GRID_WIDTH 3
#define GRID_HEIGHT 3
#define GRID_SIZE (GRID_WIDTH*GRID_HEIGHT)
#define BUTTONS_AMOUNT GRID_SIZE

#define GRID_X_OFFSET 4
#define GRID_Y_OFFSET 4

#define BUTTON_SEPARATION 2
#define BUTTON_WIDTH 9
#define BUTTON_HEIGHT 6

#define TEXT_X_OFFSET 1
#define TEXT_Y_OFFSET 2

#define TEXT_WIDTH (BUTTON_WIDTH-BUTTON_SEPARATION)
#define TEXT_HEIGHT (BUTTON_HEIGHT-BUTTON_SEPARATION)

#define BUTTON_LEFT (KEY_LEFT | KEY_A)
#define BUTTON_RIGHT (KEY_RIGHT | KEY_A)
#define BUTTON_UP (KEY_UP | KEY_A)
#define BUTTON_DOWN (KEY_DOWN | KEY_A)
#define BUTTON_CENTER (KEY_A)

#define BUTTON_SHOULDER_LEFT (KEY_L | KEY_A)
#define BUTTON_SHOULDER_RIGHT (KEY_R | KEY_A)

typedef struct {
	bool visible;
	u32 keys;
	char * text;
	void (*function)();
} button;

extern u32 kDown;
extern touchPosition touchPos;
extern PrintConsole topScreen, bottomScreen;
extern PrintConsole buttonBgConsoles[BUTTONS_AMOUNT];
extern PrintConsole buttonTextConsoles[BUTTONS_AMOUNT];

void initMenu(void);
void drawMenu(button buttons[BUTTONS_AMOUNT]);
bool isTouched(button testedButton);
