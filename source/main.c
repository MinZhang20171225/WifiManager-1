#include "blocks.h"
#include "save.h"
#include "stringutils.h"
#include "buttons.h"
#include "filebrowser/filebrowser.h"

#define WORKING_DIR "/3ds/WifiManager/"

u32 kDown = 1;
touchPosition touchPos;

void mainMenu(void);
void quit(void);

void quit(void)
{
	cfguExit();
	gfxExit();
	APT_HardwareResetAsync();
	for(;;);
}

void openSlot(int selected_slot)
{
	printf("Slot %i selected!", selected_slot);
	
	Result ret = 0;
	
	void dumpSlot(void)
	{
		printf("Saving WiFi slot %i...\n", selected_slot);
		wifiBlock slotData;
		ret = getWifiSlot(selected_slot, &slotData);
		
		char * suffix = "_wifislot.bin";
		char filename[0x20+strlen(suffix)+1];
		strcpy(filename, "Empty slot");
		
		if (slotData.network.use) strncpy(filename, slotData.network.SSID, 0x20);
		else if (slotData.network_WPS.use) strncpy(filename, slotData.network_WPS.SSID, 0x20);
		
		strcat(filename, suffix);
		cleanPath(filename);
		printf("Saving slot to:\n%s%s\n", WORKING_DIR, filename);
		saveBufToFile((u8*)&slotData, CFG_WIFI_SLOT_SIZE, filename);
		printf("Done!\n");
	}
	
	void restoreSlot(void)
	{
		printf("Restoring WiFi slot %i...\n", selected_slot);
		
		consoleSelect(&topScreen);
		char * filepath = filebrowser(WORKING_DIR);
		consoleSelect(&bottomScreen);
		if (filepath == NULL) {
			printf("Cancelling restore...\n");
			return;
		}
		
		printf("Loading data from:\n%s\nto WiFi slot %i\n", filepath, selected_slot);
		wifiBlock slotData;
		loadFileToBuf((u8*)&slotData, CFG_WIFI_SLOT_SIZE, filepath);
		free(filepath);
		
		fixSlotCRC(&slotData);
		ret = setWifiSlot(selected_slot, &slotData);
		printf("Saving config savegame...\n");
		ret = CFG_UpdateConfigNANDSavegame();
		if (ret) printf("CFG_UpdateConfigNANDSavegame\nresult: 0x%.8lx\n", ret);
		printf("Done!\n");
	}
	
	void clearSlot(void)
	{
		printf("Clearing WiFi slot %i...\n", selected_slot);
		
		u8 * emptySlot = malloc(CFG_WIFI_SLOT_SIZE);
		memset(emptySlot, 0, CFG_WIFI_SLOT_SIZE);
		ret = setWifiSlot(selected_slot, (wifiBlock*)&emptySlot);
		printf("Saving config savegame...\n");
		ret = CFG_UpdateConfigNANDSavegame();
		if (R_FAILED(ret)) printf("CFG_UpdateConfigNANDSavegame\nresult: 0x%.8lx\n", ret);
		free(emptySlot);
		
		printf("Done!\n");
	}
	
	button slotButtons[BUTTONS_AMOUNT] = {
		{false, 0, NULL, NULL},
		{false, 0, NULL, NULL},
		{true, BUTTON_LEFT, "Dump to\nfile", &dumpSlot},
		{true, BUTTON_UP, "Restore\nfrom\nfile", &restoreSlot},
		{true, BUTTON_RIGHT, "Clear\nslot", &clearSlot},
		{true, BUTTON_DOWN, "Back to\nmain\nmenu", &mainMenu},
		{true, BUTTON_CENTER, "Quit", &quit}
	};
	
	while (aptMainLoop()) {
		
		if (kDown) drawMenu(slotButtons);
		
		hidScanInput();
		
		kDown = hidKeysDown();
		// hidTouchRead(&touchPos);
		
		for (int i = 0; i < BUTTONS_AMOUNT; i++) {
			if (isTouched(slotButtons[i]))
				slotButtons[i].function();
		}
		
		gspWaitForVBlank();
	}
}

void clearAllSlots(void)
{
	Result ret = 0;
	printf("Clearing all slots...\n");
	u8 * emptySlot = malloc(CFG_WIFI_SLOT_SIZE);
	memset(emptySlot, 0, CFG_WIFI_SLOT_SIZE);
	
	for (int i = 0; i < CFG_WIFI_SLOTS; i++)
	{
		printf("Clearing slot %i...\n", i);
		ret = setWifiSlot(i, (wifiBlock*)&emptySlot);
		printf("Saving config savegame...\n");
		ret = CFG_UpdateConfigNANDSavegame();
		if (R_FAILED(ret)) printf("CFG_UpdateConfigNANDSavegame\nresult: 0x%.8lx\n", ret);
		printf("Done!\n");
	}
	free(emptySlot);
	
	printf("Done!\n");
}

void mainMenu(void)
{
	void openSlot1(void) { openSlot(1); }
	void openSlot2(void) { openSlot(2); }
	void openSlot3(void) { openSlot(3); }
	
	button mainMenuButtons[BUTTONS_AMOUNT] = {
		{false, 0, NULL, NULL},
		{false, 0, NULL, NULL},
		{true, BUTTON_LEFT, "Access\nslot 1", &openSlot1},
		{true, BUTTON_UP, "Access\nslot 2", &openSlot2},
		{true, BUTTON_RIGHT, "Access\nslot 3", &openSlot3},
		{true, BUTTON_DOWN, "Clear\nall\nslots", &clearAllSlots},
		{true, BUTTON_CENTER, "Quit", &quit},
	};
		
	while (aptMainLoop()) {
		
		if (kDown) drawMenu(mainMenuButtons);
		
		hidScanInput();
		
		kDown = hidKeysDown();
		// hidTouchRead(&touchPos);
		
		for (int i = 0; i < BUTTONS_AMOUNT; i++) {
			if (isTouched(mainMenuButtons[i]))
				mainMenuButtons[i].function();
		}
	}
}

int main() {
	
	chdir(WORKING_DIR);
	
	gfxInitDefault();
	gfxSetDoubleBuffering(GFX_TOP, false);
	gfxSetDoubleBuffering(GFX_BOTTOM, true);
	initMenu();
	
	Result ret = cfguInit();
	if (R_FAILED(ret)) printf("Error in cfguInit !\nresult: 0x%08x\n", (unsigned int)ret);
	
	mainMenu();
	
	return 0;
}
