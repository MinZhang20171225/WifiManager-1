#include "buttons.h"

PrintConsole topScreen, bottomScreen;
PrintConsole buttonBgConsoles[BUTTONS_AMOUNT];
PrintConsole buttonTextConsoles[BUTTONS_AMOUNT];

PrintConsole * consoleBgFromKey(u32 keys)
{
	switch(keys)
	{
		case BUTTON_SHOULDER_LEFT:
			return &buttonBgConsoles[0];
		case BUTTON_UP:
			return &buttonBgConsoles[1];
		case BUTTON_SHOULDER_RIGHT:
			return &buttonBgConsoles[2];
		case BUTTON_LEFT:
			return &buttonBgConsoles[3];
		case BUTTON_CENTER:
			return &buttonBgConsoles[4];
		case BUTTON_RIGHT:
			return &buttonBgConsoles[5];
		case BUTTON_DOWN:
			return &buttonBgConsoles[6];
		default:
			return NULL;
	}
}

PrintConsole * consoleTextFromKey(u32 keys)
{
	switch(keys)
	{
		case BUTTON_SHOULDER_LEFT:
			return &buttonTextConsoles[0];
		case BUTTON_UP:
			return &buttonTextConsoles[1];
		case BUTTON_SHOULDER_RIGHT:
			return &buttonTextConsoles[2];
		case BUTTON_LEFT:
			return &buttonTextConsoles[3];
		case BUTTON_CENTER:
			return &buttonTextConsoles[4];
		case BUTTON_RIGHT:
			return &buttonTextConsoles[5];
		case BUTTON_DOWN:
			return &buttonTextConsoles[6];
		default:
			return NULL;
	}
}

char * buttonStrFromKey(u32 keys)
{
	static char * key_str[GRID_SIZE] = {
		"[L]+(A)",
		"[\x18]+(A)",
		"[R]+(A)",
		"[\x1B]+(A)",
		"(A)",
		"[\x1A]+(A)",
		NULL,
		"[\x19]+(A)",
		NULL,
	};
	
	switch(keys)
	{
		case BUTTON_SHOULDER_LEFT:
			return key_str[0];
		case BUTTON_UP:
			return key_str[1];
		case BUTTON_SHOULDER_RIGHT:
			return key_str[2];
		case BUTTON_LEFT:
			return key_str[3];
		case BUTTON_CENTER:
			return key_str[4];
		case BUTTON_RIGHT:
			return key_str[5];
		case BUTTON_DOWN:
			return key_str[7];
		default:
			return NULL;
	}
}

void drawBG(void)
{
	for(int i = 0; i < (BUTTON_WIDTH*BUTTON_HEIGHT); i++)
		printf(" ");
};

void drawButton(button toDraw)
{
	PrintConsole * currentConsole = consoleBgFromKey(toDraw.keys);
	
	consoleSelect(currentConsole);
	printf("\x1b[47;30m\x1b[0;0H");
	drawBG();
	printf("\x1b[0;0H%s", buttonStrFromKey(toDraw.keys));
	
	currentConsole = consoleTextFromKey(toDraw.keys);
	consoleSelect(currentConsole);
	
	printf("\x1b[47;30m\x1b[0;0H%s", toDraw.text);
}

void initMenu(void)
{	
	u32 keys[GRID_SIZE] = {
		BUTTON_SHOULDER_LEFT,
		BUTTON_UP,
		BUTTON_SHOULDER_RIGHT,
		BUTTON_LEFT,
		BUTTON_CENTER,
		BUTTON_RIGHT,
		0,
		BUTTON_DOWN,
		0
	};
	
	consoleInit(GFX_BOTTOM, &bottomScreen);
	
	for(u32 y = 0; y < GRID_HEIGHT; y++)
	{
		for(u32 x = 0; x < GRID_WIDTH; x++)
		{
			PrintConsole * currentConsole = consoleBgFromKey(keys[(y*GRID_HEIGHT) +x]);
			if(currentConsole == NULL) continue;
			consoleInit(GFX_BOTTOM, currentConsole);
			consoleSetWindow(currentConsole,
				GRID_X_OFFSET + BUTTON_SEPARATION*x + BUTTON_WIDTH*x,
				GRID_Y_OFFSET + BUTTON_SEPARATION*y + BUTTON_HEIGHT*y,
				BUTTON_WIDTH, BUTTON_HEIGHT);
			
			currentConsole = consoleTextFromKey(keys[(y*GRID_HEIGHT) +x]);
			consoleInit(GFX_BOTTOM, currentConsole);
			consoleSetWindow(currentConsole,
				GRID_X_OFFSET + BUTTON_SEPARATION*x + BUTTON_WIDTH*x + TEXT_X_OFFSET,
				GRID_Y_OFFSET + BUTTON_SEPARATION*y + BUTTON_HEIGHT*y + TEXT_Y_OFFSET,
				TEXT_WIDTH, TEXT_HEIGHT);
		}
	}
	
	consoleInit(GFX_TOP, &topScreen);
}

void drawMenu(button buttons[BUTTONS_AMOUNT])
{
	gfxFlushBuffers();
	gfxSwapBuffers();
	
	gspWaitForVBlank();
	
	consoleSelect(&bottomScreen);
	consoleClear();
	
	for (int i = 0; i < BUTTONS_AMOUNT; i++) {
		if (buttons[i].visible) {
			drawButton(buttons[i]);
		}
	}
	
	consoleSelect(&topScreen);
	
	gfxFlushBuffers();
	gfxSwapBuffers();
	
	gspWaitForVBlank();
	
	gfxFlushBuffers();
	gfxSwapBuffers();
	
	gspWaitForVBlank();
}

bool isTouched(button testedButton)
{
	bool touched = false;
	
	touched = ((kDown & testedButton.keys) == testedButton.keys);
	return (touched && testedButton.visible);
}
