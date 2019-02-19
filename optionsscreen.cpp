#include "optionsscreen.h"

#include <prism/wrapper.h>
#include <prism/input.h>
#include <prism/mugentexthandler.h>
#include <prism/mugenanimationhandler.h>
#include <prism/clipboardhandler.h>
#include <prism/screeneffect.h>
#include <prism/mugensoundfilereader.h>
#include <prism/memoryhandler.h>
#include <prism/mugendefreader.h>
#include <prism/animation.h>
#include <prism/stlutil.h>
#include <prism/system.h>

#include "titlescreen.h"
#include "menubackground.h"
#include "boxcursorhandler.h"
#include "config.h"

using namespace std;

typedef struct {
	Vector3DI mCursorMoveSound;
	Vector3DI mCursorDoneSound;
	Vector3DI mCancelSound;
} OptionsHeader;

typedef enum {
	GENERAL_SETTING_DIFFICULTY,
	GENERAL_SETTING_LIFE,
	GENERAL_SETTING_TIME_LIMIT,
	GENERAL_SETTING_GAME_SPEED,
	GENERAL_SETTING_WAV_VOLUME,
	GENERAL_SETTING_MIDI_VOLUME,
	GENERAL_SETTING_INPUT_CONFIG,
	GENERAL_SETTING_TEAM_MODE_CONFIG,
	GENERAL_SETTING_LOAD_SAVE,
	GENERAL_SETTING_DEFAULT_VALUES,
	GENERAL_SETTING_RETURN,
	GENERAL_SETTING_AMOUNT
} GeneralSettingType;


typedef struct {
	int mBoxCursorID;

	int mSelected;
	
	int mSelectableTextID[GENERAL_SETTING_AMOUNT];
	int mSettingTextID[GENERAL_SETTING_AMOUNT];
} GeneralOptionsScreen;

typedef enum {
	INPUT_CONFIG_SETTING_KEY_CONFIG,
	INPUT_CONFIG_SETTING_JOYSTICK_TYPE,
	INPUT_CONFIG_SETTING_JOYSTICK_CONFIG,
	INPUT_CONFIG_SETTING_DEFAULT_VALUES,
	INPUT_CONFIG_SETTING_RETURN,

	INPUT_CONFIG_SETTING_AMOUNT
} InputConfigSettingType;

typedef struct {
	int mBoxCursorID[2];
	int mSelected[2];
	int mPlayerTextID[2];

	int mSelectableTextID[2][INPUT_CONFIG_SETTING_AMOUNT];
	int mSettingTextID[2][INPUT_CONFIG_SETTING_AMOUNT];
} InputConfigOptionsScreen;

typedef enum {
	KEY_CONFIG_SETTING_CONFIG_ALL,
	KEY_CONFIG_SETTING_UP,
	KEY_CONFIG_SETTING_DOWN,
	KEY_CONFIG_SETTING_LEFT,
	KEY_CONFIG_SETTING_RIGHT,
	KEY_CONFIG_SETTING_A,
	KEY_CONFIG_SETTING_B,
	KEY_CONFIG_SETTING_C,
	KEY_CONFIG_SETTING_X,
	KEY_CONFIG_SETTING_Y,
	KEY_CONFIG_SETTING_Z,
	KEY_CONFIG_SETTING_START,
	KEY_CONFIG_SETTING_DEFAULT,
	KEY_CONFIG_SETTING_EXIT,

	KEY_CONFIG_SETTING_AMOUNT
} KeyConfigSettingType;

typedef struct {
	int mBoxCursorID;
	Vector3DI mSelected;
	int mPlayerTextID[2];

	int mSelectableTextID[2][KEY_CONFIG_SETTING_AMOUNT];
	int mSettingTextID[2][KEY_CONFIG_SETTING_AMOUNT];
} KeyConfigOptionsScreen;

typedef enum {
	OPTION_SCREEN_GENERAL,
	OPTION_SCREEN_INPUT_CONFIG,
	OPTION_SCREEN_KEY_CONFIG,
} OptionScreenType;

static struct {
	TextureData mWhiteTexture;

	MugenDefScript mScript;
	MugenSpriteFile mSprites;
	MugenAnimations mAnimations;
	MugenSounds mSounds;

	OptionsHeader mHeader;
	GeneralOptionsScreen mGeneral;
	InputConfigOptionsScreen mInputConfig;
	KeyConfigOptionsScreen mKeyConfig;
	int mHeaderTextID;
	int mBackgroundAnimationID;

	OptionScreenType mActiveScreen;
} gOptionsScreenData;

static void loadOptionsHeader() {
	gOptionsScreenData.mHeader.mCursorMoveSound = getMugenDefVectorIOrDefault(&gOptionsScreenData.mScript, "Option Info", "cursor.move.snd", makeVector3DI(1, 0, 0));
	gOptionsScreenData.mHeader.mCursorDoneSound = getMugenDefVectorIOrDefault(&gOptionsScreenData.mScript, "Option Info", "cursor.done.snd", makeVector3DI(1, 0, 0));
	gOptionsScreenData.mHeader.mCancelSound = getMugenDefVectorIOrDefault(&gOptionsScreenData.mScript, "Option Info", "cancel.snd", makeVector3DI(1, 0, 0));

}

static void setSelectedGeneralOptionInactive() {
	setMugenTextColor(gOptionsScreenData.mGeneral.mSelectableTextID[gOptionsScreenData.mGeneral.mSelected], getMugenTextColorFromMugenTextColorIndex(8));

	Vector3D selectionColor = getMugenTextColor(gOptionsScreenData.mGeneral.mSettingTextID[gOptionsScreenData.mGeneral.mSelected]);
	if (selectionColor.z != 0) {
		setMugenTextColor(gOptionsScreenData.mGeneral.mSettingTextID[gOptionsScreenData.mGeneral.mSelected], getMugenTextColorFromMugenTextColorIndex(8));
	}
}

static void setSelectedGeneralOptionActive() {
	setMugenTextColor(gOptionsScreenData.mGeneral.mSelectableTextID[gOptionsScreenData.mGeneral.mSelected], getMugenTextColorFromMugenTextColorIndex(0));

	Vector3D selectionColor = getMugenTextColor(gOptionsScreenData.mGeneral.mSettingTextID[gOptionsScreenData.mGeneral.mSelected]);
	if (selectionColor.z != 0) {
		setMugenTextColor(gOptionsScreenData.mGeneral.mSettingTextID[gOptionsScreenData.mGeneral.mSelected], getMugenTextColorFromMugenTextColorIndex(0));
	}

	Position pos = getMugenTextPosition(gOptionsScreenData.mGeneral.mSelectableTextID[gOptionsScreenData.mGeneral.mSelected]);
	pos.z = 0;
	setBoxCursorPosition(gOptionsScreenData.mGeneral.mBoxCursorID, pos);
}

static void updateSelectedGeneralOption(int tDelta) {
	setSelectedGeneralOptionInactive();
	gOptionsScreenData.mGeneral.mSelected += tDelta;
	gOptionsScreenData.mGeneral.mSelected = (gOptionsScreenData.mGeneral.mSelected + GENERAL_SETTING_AMOUNT) % GENERAL_SETTING_AMOUNT;
	tryPlayMugenSound(&gOptionsScreenData.mSounds, gOptionsScreenData.mHeader.mCursorMoveSound.x, gOptionsScreenData.mHeader.mCursorMoveSound.y);
	setSelectedGeneralOptionActive();
}

static void setDifficultyOptionText();
static void setLifeOptionText();
static void setTimeLimitOptionText();
static void setGameSpeedOptionText();
static void setVolumeOptionText(GeneralSettingType tType, int(*tGet)());

static void loadGeneralOptionsScreen() {
	setAnimationPosition(gOptionsScreenData.mBackgroundAnimationID, makePosition(52, 35, 40));
	setAnimationSize(gOptionsScreenData.mBackgroundAnimationID, makePosition(216, 185, 1), makePosition(0, 0, 0));
	
	changeMugenText(gOptionsScreenData.mHeaderTextID, "OPTIONS");
	setMugenTextPosition(gOptionsScreenData.mHeaderTextID, makePosition(160, 20, 60));
	gOptionsScreenData.mActiveScreen = OPTION_SCREEN_GENERAL;

	int offsetX = 70;
	int startY = 50;
	int offsetY = 13;
	gOptionsScreenData.mGeneral.mSelectableTextID[GENERAL_SETTING_DIFFICULTY] = addMugenTextMugenStyle("Difficulty", makePosition(offsetX, startY, 60), makeVector3DI(2, 8, 1));
	gOptionsScreenData.mGeneral.mSettingTextID[GENERAL_SETTING_DIFFICULTY] = addMugenTextMugenStyle("Hard 8", makePosition(320 - offsetX, startY, 60), makeVector3DI(2, 8, -1));
	setDifficultyOptionText();

	gOptionsScreenData.mGeneral.mSelectableTextID[GENERAL_SETTING_LIFE] = addMugenTextMugenStyle("Life", makePosition(offsetX, startY + offsetY * 1, 60), makeVector3DI(2, 8, 1));
	gOptionsScreenData.mGeneral.mSettingTextID[GENERAL_SETTING_LIFE] = addMugenTextMugenStyle("100%", makePosition(320 - offsetX, startY + offsetY * 1, 60), makeVector3DI(2, 8, -1));
	setLifeOptionText();

	gOptionsScreenData.mGeneral.mSelectableTextID[GENERAL_SETTING_TIME_LIMIT] = addMugenTextMugenStyle("Time Limit", makePosition(offsetX, startY + offsetY * 2, 60), makeVector3DI(2, 8, 1));
	gOptionsScreenData.mGeneral.mSettingTextID[GENERAL_SETTING_TIME_LIMIT] = addMugenTextMugenStyle("99", makePosition(320 - offsetX, startY + offsetY * 2, 60), makeVector3DI(2, 8, -1));
	setTimeLimitOptionText();

	gOptionsScreenData.mGeneral.mSelectableTextID[GENERAL_SETTING_GAME_SPEED] = addMugenTextMugenStyle("Game Speed", makePosition(offsetX, startY + offsetY * 3, 60), makeVector3DI(2, 8, 1));
	gOptionsScreenData.mGeneral.mSettingTextID[GENERAL_SETTING_GAME_SPEED] = addMugenTextMugenStyle("Normal", makePosition(320 - offsetX, startY + offsetY * 3, 60), makeVector3DI(2, 8, -1));
	setGameSpeedOptionText();

	gOptionsScreenData.mGeneral.mSelectableTextID[GENERAL_SETTING_WAV_VOLUME] = addMugenTextMugenStyle("Wav Volume", makePosition(offsetX, startY + offsetY * 4, 60), makeVector3DI(2, 8, 1));
	gOptionsScreenData.mGeneral.mSettingTextID[GENERAL_SETTING_WAV_VOLUME] = addMugenTextMugenStyle("50", makePosition(320 - offsetX, startY + offsetY * 4, 60), makeVector3DI(2, 8, -1));
	setVolumeOptionText(GENERAL_SETTING_WAV_VOLUME, getGameWavVolume);

	gOptionsScreenData.mGeneral.mSelectableTextID[GENERAL_SETTING_MIDI_VOLUME] = addMugenTextMugenStyle("Midi Volume", makePosition(offsetX, startY + offsetY * 5, 60), makeVector3DI(2, 8, 1));
	gOptionsScreenData.mGeneral.mSettingTextID[GENERAL_SETTING_MIDI_VOLUME] = addMugenTextMugenStyle("50", makePosition(320 - offsetX, startY + offsetY * 5, 60), makeVector3DI(2, 8, -1));
	setVolumeOptionText(GENERAL_SETTING_MIDI_VOLUME, getGameMidiVolume);

	gOptionsScreenData.mGeneral.mSelectableTextID[GENERAL_SETTING_INPUT_CONFIG] = addMugenTextMugenStyle("Input Config", makePosition(offsetX, startY + offsetY * 6, 60), makeVector3DI(2, 8, 1));
	if (!isOnDreamcast()) {
		gOptionsScreenData.mGeneral.mSettingTextID[GENERAL_SETTING_INPUT_CONFIG] = addMugenTextMugenStyle("(F1)", makePosition(320 - offsetX, startY + offsetY * 6, 60), makeVector3DI(2, 5, -1));
	}
	else {
		gOptionsScreenData.mGeneral.mSettingTextID[GENERAL_SETTING_INPUT_CONFIG] = addMugenTextMugenStyle("(X)", makePosition(320 - offsetX, startY + offsetY * 6, 60), makeVector3DI(2, 5, -1));
	}
	gOptionsScreenData.mGeneral.mSelectableTextID[GENERAL_SETTING_TEAM_MODE_CONFIG] = addMugenTextMugenStyle("Team Mode Config", makePosition(offsetX, startY + offsetY * 7, 60), makeVector3DI(2, 8, 1));
	if (!isOnDreamcast()) {
		gOptionsScreenData.mGeneral.mSettingTextID[GENERAL_SETTING_TEAM_MODE_CONFIG] = addMugenTextMugenStyle("(F2)", makePosition(320 - offsetX, startY + offsetY * 7, 60), makeVector3DI(2, 5, -1));
	}
	else {
		gOptionsScreenData.mGeneral.mSettingTextID[GENERAL_SETTING_TEAM_MODE_CONFIG] = addMugenTextMugenStyle("(Y)", makePosition(320 - offsetX, startY + offsetY * 7, 60), makeVector3DI(2, 2, -1));
	}
	gOptionsScreenData.mGeneral.mSelectableTextID[GENERAL_SETTING_LOAD_SAVE] = addMugenTextMugenStyle("Load/Save", makePosition(offsetX, startY + offsetY * 9, 60), makeVector3DI(2, 8, 1));
	gOptionsScreenData.mGeneral.mSettingTextID[GENERAL_SETTING_LOAD_SAVE] = addMugenTextMugenStyle("Load", makePosition(320 - offsetX, startY + offsetY * 9, 60), makeVector3DI(2, 8, -1));

	gOptionsScreenData.mGeneral.mSelectableTextID[GENERAL_SETTING_DEFAULT_VALUES] = addMugenTextMugenStyle("Default Values", makePosition(offsetX, startY + offsetY * 10, 60), makeVector3DI(2, 8, 1));
	gOptionsScreenData.mGeneral.mSettingTextID[GENERAL_SETTING_DEFAULT_VALUES] = addMugenTextMugenStyle("", makePosition(320 - offsetX, startY + offsetY * 10, 60), makeVector3DI(2, 8, -1));

	gOptionsScreenData.mGeneral.mSelectableTextID[GENERAL_SETTING_RETURN] = addMugenTextMugenStyle("Return to Main Menu", makePosition(offsetX, startY + offsetY * 12, 60), makeVector3DI(2, 8, 1));
	if (!isOnDreamcast()) {
		gOptionsScreenData.mGeneral.mSettingTextID[GENERAL_SETTING_RETURN] = addMugenTextMugenStyle("(Esc)", makePosition(320 - offsetX, startY + offsetY * 12, 60), makeVector3DI(2, 5, -1));
	}
	else {
		gOptionsScreenData.mGeneral.mSettingTextID[GENERAL_SETTING_RETURN] = addMugenTextMugenStyle("(B)", makePosition(320 - offsetX, startY + offsetY * 12, 60), makeVector3DI(2, 3, -1));
	}
	gOptionsScreenData.mGeneral.mBoxCursorID = addBoxCursor(makePosition(0, 0, 0), makePosition(0, 0, 50), makeGeoRectangle(-6, -11, 320 - 2 * offsetX + 10, 14));

	setSelectedGeneralOptionActive();
}

static void unloadGeneralOptionsScreen() {
	for (int i = 0; i < GENERAL_SETTING_AMOUNT; i++) {
		removeMugenText(gOptionsScreenData.mGeneral.mSelectableTextID[i]);
		removeMugenText(gOptionsScreenData.mGeneral.mSettingTextID[i]);
	}

	removeBoxCursor(gOptionsScreenData.mGeneral.mBoxCursorID);
}

static void setSelectedInputConfigOptionActive(int i) {

	setMugenTextColor(gOptionsScreenData.mInputConfig.mSelectableTextID[i][gOptionsScreenData.mInputConfig.mSelected[i]], getMugenTextColorFromMugenTextColorIndex(0));

	Vector3D selectionColor = getMugenTextColor(gOptionsScreenData.mInputConfig.mSettingTextID[i][gOptionsScreenData.mInputConfig.mSelected[i]]);
	if (selectionColor.z != 0) {
		setMugenTextColor(gOptionsScreenData.mInputConfig.mSettingTextID[i][gOptionsScreenData.mInputConfig.mSelected[i]], getMugenTextColorFromMugenTextColorIndex(0));
	}

	Position pos = getMugenTextPosition(gOptionsScreenData.mInputConfig.mSelectableTextID[i][gOptionsScreenData.mInputConfig.mSelected[i]]);
	pos.z = 0;
	setBoxCursorPosition(gOptionsScreenData.mInputConfig.mBoxCursorID[i], pos);
}

static void setSelectedInputConfigOptionsActive() {
	for (int i = 0; i < 2; i++) {
		setSelectedInputConfigOptionActive(i);
	}
}

static void setSelectedInputConfigOptionInactive(int i) {
	setMugenTextColor(gOptionsScreenData.mInputConfig.mSelectableTextID[i][gOptionsScreenData.mInputConfig.mSelected[i]], getMugenTextColorFromMugenTextColorIndex(8));

	Vector3D selectionColor = getMugenTextColor(gOptionsScreenData.mInputConfig.mSettingTextID[i][gOptionsScreenData.mInputConfig.mSelected[i]]);
	if (selectionColor.z != 0) {
		setMugenTextColor(gOptionsScreenData.mInputConfig.mSettingTextID[i][gOptionsScreenData.mInputConfig.mSelected[i]], getMugenTextColorFromMugenTextColorIndex(8));
	}
}

static void loadInputConfigOptionsScreen() {
	setAnimationPosition(gOptionsScreenData.mBackgroundAnimationID, makePosition(13, 35, 40));
	setAnimationSize(gOptionsScreenData.mBackgroundAnimationID, makePosition(292, 121, 1), makePosition(0, 0, 0));


	changeMugenText(gOptionsScreenData.mHeaderTextID, "INPUT CONFIG");
	setMugenTextPosition(gOptionsScreenData.mHeaderTextID, makePosition(160, 20, 60));
	gOptionsScreenData.mActiveScreen = OPTION_SCREEN_INPUT_CONFIG;

	gOptionsScreenData.mInputConfig.mPlayerTextID[0] = addMugenTextMugenStyle("PLAYER 1", makePosition(80, 48, 60), makeVector3DI(2, 4, 0));
	gOptionsScreenData.mInputConfig.mPlayerTextID[1] = addMugenTextMugenStyle("PLAYER 2", makePosition(240, 48, 60), makeVector3DI(2, 1, 0));


	int left = 0;
	int right = 160;
	int offsetX = 22;
	int startY = 64;
	int offsetY = 13;
	for (int i = 0; i < 2; i++) {

		gOptionsScreenData.mInputConfig.mSelectableTextID[i][INPUT_CONFIG_SETTING_KEY_CONFIG] = addMugenTextMugenStyle("Key Config", makePosition(left + offsetX, startY, 60), makeVector3DI(2, 8, 1));
		if (!i) {
			gOptionsScreenData.mInputConfig.mSettingTextID[i][INPUT_CONFIG_SETTING_KEY_CONFIG] = addMugenTextMugenStyle("", makePosition(right - offsetX, startY, 60), makeVector3DI(2, 8, -1));
		}
		else {
			if (!isOnDreamcast()) {
				gOptionsScreenData.mInputConfig.mSettingTextID[i][INPUT_CONFIG_SETTING_KEY_CONFIG] = addMugenTextMugenStyle("(F1)", makePosition(right - offsetX, startY, 60), makeVector3DI(2, 5, -1));
			}
			else {
				gOptionsScreenData.mInputConfig.mSettingTextID[i][INPUT_CONFIG_SETTING_KEY_CONFIG] = addMugenTextMugenStyle("(X)", makePosition(right - offsetX, startY, 60), makeVector3DI(2, 5, -1));
			}
		}
		gOptionsScreenData.mInputConfig.mSelectableTextID[i][INPUT_CONFIG_SETTING_JOYSTICK_TYPE] = addMugenTextMugenStyle("Joystick Type", makePosition(left + offsetX, startY + offsetY * 2, 60), makeVector3DI(2, 8, 1));
		gOptionsScreenData.mInputConfig.mSettingTextID[i][INPUT_CONFIG_SETTING_JOYSTICK_TYPE] = addMugenTextMugenStyle("Disabled", makePosition(right - offsetX, startY + offsetY * 2, 60), makeVector3DI(2, 8, -1));

		gOptionsScreenData.mInputConfig.mSelectableTextID[i][INPUT_CONFIG_SETTING_JOYSTICK_CONFIG] = addMugenTextMugenStyle("Joystick Config", makePosition(left + offsetX, startY + offsetY * 3, 60), makeVector3DI(2, 8, 1));
		if (!i) {
			gOptionsScreenData.mInputConfig.mSettingTextID[i][INPUT_CONFIG_SETTING_JOYSTICK_CONFIG] = addMugenTextMugenStyle("", makePosition(right - offsetX, startY + offsetY * 3, 60), makeVector3DI(2, 8, -1));
		}
		else {
			if (!isOnDreamcast()) {
				gOptionsScreenData.mInputConfig.mSettingTextID[i][INPUT_CONFIG_SETTING_JOYSTICK_CONFIG] = addMugenTextMugenStyle("(F2)", makePosition(right - offsetX, startY + offsetY * 3, 60), makeVector3DI(2, 5, -1));
			}
			else {
				gOptionsScreenData.mInputConfig.mSettingTextID[i][INPUT_CONFIG_SETTING_JOYSTICK_CONFIG] = addMugenTextMugenStyle("(Y)", makePosition(right - offsetX, startY + offsetY * 3, 60), makeVector3DI(2, 2, -1));
			}
		}
		gOptionsScreenData.mInputConfig.mSelectableTextID[i][INPUT_CONFIG_SETTING_DEFAULT_VALUES] = addMugenTextMugenStyle("Default Values", makePosition(left + offsetX, startY + offsetY * 5, 60), makeVector3DI(2, 8, 1));
		gOptionsScreenData.mInputConfig.mSettingTextID[i][INPUT_CONFIG_SETTING_DEFAULT_VALUES] = addMugenTextMugenStyle("", makePosition(right - offsetX, startY + offsetY * 5, 60), makeVector3DI(2, 8, -1));

		gOptionsScreenData.mInputConfig.mSelectableTextID[i][INPUT_CONFIG_SETTING_RETURN] = addMugenTextMugenStyle("Return to Options", makePosition(left + offsetX, startY + offsetY * 6, 60), makeVector3DI(2, 8, 1));
		
		if (!i) {
			gOptionsScreenData.mInputConfig.mSettingTextID[i][INPUT_CONFIG_SETTING_RETURN] = addMugenTextMugenStyle("", makePosition(right - offsetX, startY + offsetY * 6, 60), makeVector3DI(2, 8, -1));
		}
		else {
			if (!isOnDreamcast()) {
				gOptionsScreenData.mInputConfig.mSettingTextID[i][INPUT_CONFIG_SETTING_RETURN] = addMugenTextMugenStyle("(Esc)", makePosition(right - offsetX, startY + offsetY * 6, 60), makeVector3DI(2, 5, -1));
			}
			else {
				gOptionsScreenData.mInputConfig.mSettingTextID[i][INPUT_CONFIG_SETTING_RETURN] = addMugenTextMugenStyle("(B)", makePosition(right - offsetX, startY + offsetY * 6, 60), makeVector3DI(2, 3, -1));
			}
		}
		gOptionsScreenData.mInputConfig.mBoxCursorID[i] = addBoxCursor(makePosition(0, 0, 0), makePosition(0, 0, 50), makeGeoRectangle(-6, -11, 160 - 2 * offsetX + 10, 14));

		left = 160;
		right = 320;
	}

	
	setSelectedInputConfigOptionsActive();
}

static void unloadInputConfigOptionsScreen() {
	for (int i = 0; i < 2; i++) {
		removeMugenText(gOptionsScreenData.mInputConfig.mPlayerTextID[i]);

		for (int j = 0; j < INPUT_CONFIG_SETTING_AMOUNT; j++) {
			removeMugenText(gOptionsScreenData.mInputConfig.mSelectableTextID[i][j]);
			removeMugenText(gOptionsScreenData.mInputConfig.mSettingTextID[i][j]);
		}
		removeBoxCursor(gOptionsScreenData.mInputConfig.mBoxCursorID[i]);

	}

}

static void setSelectedKeyConfigOptionActive() {

	setMugenTextColor(gOptionsScreenData.mKeyConfig.mSelectableTextID[gOptionsScreenData.mKeyConfig.mSelected.x][gOptionsScreenData.mKeyConfig.mSelected.y], getMugenTextColorFromMugenTextColorIndex(0));

	Vector3D selectionColor = getMugenTextColor(gOptionsScreenData.mKeyConfig.mSettingTextID[gOptionsScreenData.mKeyConfig.mSelected.x][gOptionsScreenData.mKeyConfig.mSelected.y]);
	if (selectionColor.z != 0) {
		setMugenTextColor(gOptionsScreenData.mKeyConfig.mSettingTextID[gOptionsScreenData.mKeyConfig.mSelected.x][gOptionsScreenData.mKeyConfig.mSelected.y], getMugenTextColorFromMugenTextColorIndex(0));
	}

	Position pos = getMugenTextPosition(gOptionsScreenData.mKeyConfig.mSelectableTextID[gOptionsScreenData.mKeyConfig.mSelected.x][gOptionsScreenData.mKeyConfig.mSelected.y]);
	pos.z = 0;
	setBoxCursorPosition(gOptionsScreenData.mKeyConfig.mBoxCursorID, pos);
}

static void setSelectedKeyConfigOptionInactive() {
	setMugenTextColor(gOptionsScreenData.mKeyConfig.mSelectableTextID[gOptionsScreenData.mKeyConfig.mSelected.x][gOptionsScreenData.mKeyConfig.mSelected.y], getMugenTextColorFromMugenTextColorIndex(8));

	Vector3D selectionColor = getMugenTextColor(gOptionsScreenData.mKeyConfig.mSettingTextID[gOptionsScreenData.mKeyConfig.mSelected.x][gOptionsScreenData.mKeyConfig.mSelected.y]);
	if (selectionColor.z != 0) {
		setMugenTextColor(gOptionsScreenData.mKeyConfig.mSettingTextID[gOptionsScreenData.mKeyConfig.mSelected.x][gOptionsScreenData.mKeyConfig.mSelected.y], getMugenTextColorFromMugenTextColorIndex(8));
	}
}


static void loadKeyConfigOptionsScreen() {
	setAnimationPosition(gOptionsScreenData.mBackgroundAnimationID, makePosition(33, 15, 40));
	setAnimationSize(gOptionsScreenData.mBackgroundAnimationID, makePosition(252, 221, 1), makePosition(0, 0, 0));

	changeMugenText(gOptionsScreenData.mHeaderTextID, "KEY CONFIG");
	setMugenTextPosition(gOptionsScreenData.mHeaderTextID, makePosition(161, 11, 60));
	gOptionsScreenData.mActiveScreen = OPTION_SCREEN_KEY_CONFIG;

	gOptionsScreenData.mKeyConfig.mPlayerTextID[0] = addMugenTextMugenStyle("PLAYER 1", makePosition(91, 28, 60), makeVector3DI(2, 4, 0));
	gOptionsScreenData.mKeyConfig.mPlayerTextID[1] = addMugenTextMugenStyle("PLAYER 2", makePosition(230, 28, 60), makeVector3DI(2, 1, 0));


	int left = 38;
	int right = 142;
	int offsetX = 11;
	int startY = 44;
	int offsetY = 13;
	for (int i = 0; i < 1; i++) {

		gOptionsScreenData.mKeyConfig.mSelectableTextID[i][KEY_CONFIG_SETTING_CONFIG_ALL] = addMugenTextMugenStyle("Config all", makePosition(left + offsetX, startY, 60), makeVector3DI(2, 8, 1));
		gOptionsScreenData.mKeyConfig.mSettingTextID[i][KEY_CONFIG_SETTING_CONFIG_ALL] = addMugenTextMugenStyle("(F1)", makePosition(right - offsetX, startY, 60), makeVector3DI(2, 5, -1));

		gOptionsScreenData.mKeyConfig.mSelectableTextID[i][KEY_CONFIG_SETTING_UP] = addMugenTextMugenStyle("Up", makePosition(left + offsetX, startY + offsetY * 1, 60), makeVector3DI(2, 8, 1));
		gOptionsScreenData.mKeyConfig.mSettingTextID[i][KEY_CONFIG_SETTING_UP] = addMugenTextMugenStyle("Up", makePosition(right - offsetX, startY + offsetY * 1, 60), makeVector3DI(2, 8, -1));

		gOptionsScreenData.mKeyConfig.mSelectableTextID[i][KEY_CONFIG_SETTING_DOWN] = addMugenTextMugenStyle("Down", makePosition(left + offsetX, startY + offsetY * 2, 60), makeVector3DI(2, 8, 1));
		gOptionsScreenData.mKeyConfig.mSettingTextID[i][KEY_CONFIG_SETTING_DOWN] = addMugenTextMugenStyle("Down", makePosition(right - offsetX, startY + offsetY * 2, 60), makeVector3DI(2, 8, -1));

		gOptionsScreenData.mKeyConfig.mSelectableTextID[i][KEY_CONFIG_SETTING_LEFT] = addMugenTextMugenStyle("Left", makePosition(left + offsetX, startY + offsetY * 3, 60), makeVector3DI(2, 8, 1));
		gOptionsScreenData.mKeyConfig.mSettingTextID[i][KEY_CONFIG_SETTING_LEFT] = addMugenTextMugenStyle("Left", makePosition(right - offsetX, startY + offsetY * 3, 60), makeVector3DI(2, 8, -1));

		gOptionsScreenData.mKeyConfig.mSelectableTextID[i][KEY_CONFIG_SETTING_RIGHT] = addMugenTextMugenStyle("Right", makePosition(left + offsetX, startY + offsetY * 4, 60), makeVector3DI(2, 8, 1));
		gOptionsScreenData.mKeyConfig.mSettingTextID[i][KEY_CONFIG_SETTING_RIGHT] = addMugenTextMugenStyle("Right", makePosition(right - offsetX, startY + offsetY * 4, 60), makeVector3DI(2, 8, -1));

		gOptionsScreenData.mKeyConfig.mSelectableTextID[i][KEY_CONFIG_SETTING_A] = addMugenTextMugenStyle("A", makePosition(left + offsetX, startY + offsetY * 5, 60), makeVector3DI(2, 8, 1));
		gOptionsScreenData.mKeyConfig.mSettingTextID[i][KEY_CONFIG_SETTING_A] = addMugenTextMugenStyle("A", makePosition(right - offsetX, startY + offsetY * 5, 60), makeVector3DI(2, 8, -1));

		gOptionsScreenData.mKeyConfig.mSelectableTextID[i][KEY_CONFIG_SETTING_B] = addMugenTextMugenStyle("B", makePosition(left + offsetX, startY + offsetY * 6, 60), makeVector3DI(2, 8, 1));
		gOptionsScreenData.mKeyConfig.mSettingTextID[i][KEY_CONFIG_SETTING_B] = addMugenTextMugenStyle("B", makePosition(right - offsetX, startY + offsetY * 6, 60), makeVector3DI(2, 8, -1));

		gOptionsScreenData.mKeyConfig.mSelectableTextID[i][KEY_CONFIG_SETTING_C] = addMugenTextMugenStyle("C", makePosition(left + offsetX, startY + offsetY * 7, 60), makeVector3DI(2, 8, 1));
		gOptionsScreenData.mKeyConfig.mSettingTextID[i][KEY_CONFIG_SETTING_C] = addMugenTextMugenStyle("C", makePosition(right - offsetX, startY + offsetY * 7, 60), makeVector3DI(2, 8, -1));

		gOptionsScreenData.mKeyConfig.mSelectableTextID[i][KEY_CONFIG_SETTING_X] = addMugenTextMugenStyle("X", makePosition(left + offsetX, startY + offsetY * 8, 60), makeVector3DI(2, 8, 1));
		gOptionsScreenData.mKeyConfig.mSettingTextID[i][KEY_CONFIG_SETTING_X] = addMugenTextMugenStyle("X", makePosition(right - offsetX, startY + offsetY * 8, 60), makeVector3DI(2, 8, -1));

		gOptionsScreenData.mKeyConfig.mSelectableTextID[i][KEY_CONFIG_SETTING_Y] = addMugenTextMugenStyle("Y", makePosition(left + offsetX, startY + offsetY * 9, 60), makeVector3DI(2, 8, 1));
		gOptionsScreenData.mKeyConfig.mSettingTextID[i][KEY_CONFIG_SETTING_Y] = addMugenTextMugenStyle("Y", makePosition(right - offsetX, startY + offsetY * 9, 60), makeVector3DI(2, 8, -1));

		gOptionsScreenData.mKeyConfig.mSelectableTextID[i][KEY_CONFIG_SETTING_Z] = addMugenTextMugenStyle("Z", makePosition(left + offsetX, startY + offsetY * 10, 60), makeVector3DI(2, 8, 1));
		gOptionsScreenData.mKeyConfig.mSettingTextID[i][KEY_CONFIG_SETTING_Z] = addMugenTextMugenStyle("Z", makePosition(right - offsetX, startY + offsetY * 10, 60), makeVector3DI(2, 8, -1));

		gOptionsScreenData.mKeyConfig.mSelectableTextID[i][KEY_CONFIG_SETTING_START] = addMugenTextMugenStyle("Start", makePosition(left + offsetX, startY + offsetY * 11, 60), makeVector3DI(2, 8, 1));
		gOptionsScreenData.mKeyConfig.mSettingTextID[i][KEY_CONFIG_SETTING_START] = addMugenTextMugenStyle("Enter", makePosition(right - offsetX, startY + offsetY * 11, 60), makeVector3DI(2, 8, -1));

		gOptionsScreenData.mKeyConfig.mSelectableTextID[i][KEY_CONFIG_SETTING_DEFAULT] = addMugenTextMugenStyle("Default", makePosition(left + offsetX, startY + offsetY * 13, 60), makeVector3DI(2, 8, 1));
		gOptionsScreenData.mKeyConfig.mSettingTextID[i][KEY_CONFIG_SETTING_DEFAULT] = addMugenTextMugenStyle("(F3)", makePosition(right - offsetX, startY + offsetY * 13, 60), makeVector3DI(2, 5, -1));

		gOptionsScreenData.mKeyConfig.mSelectableTextID[i][KEY_CONFIG_SETTING_EXIT] = addMugenTextMugenStyle("Exit", makePosition(left + offsetX, startY + offsetY * 14, 60), makeVector3DI(2, 8, 1));
		gOptionsScreenData.mKeyConfig.mSettingTextID[i][KEY_CONFIG_SETTING_EXIT] = addMugenTextMugenStyle("(Esc)", makePosition(right - offsetX, startY + offsetY * 14, 60), makeVector3DI(2, 5, -1));

		left = 160;
		right = 320;
	}

	gOptionsScreenData.mKeyConfig.mBoxCursorID = addBoxCursor(makePosition(0, 0, 0), makePosition(0, 0, 50), makeGeoRectangle(-5, -11, 92, 14));

	setSelectedKeyConfigOptionActive();
}

static void unloadKeyConfigOptionsScreen() {
	for (int i = 0; i < 1; i++) {
		removeMugenText(gOptionsScreenData.mKeyConfig.mPlayerTextID[i]);

		for (int j = 0; j < KEY_CONFIG_SETTING_AMOUNT; j++) {
			removeMugenText(gOptionsScreenData.mKeyConfig.mSelectableTextID[i][j]);
			removeMugenText(gOptionsScreenData.mKeyConfig.mSettingTextID[i][j]);
		}
	}

	removeBoxCursor(gOptionsScreenData.mKeyConfig.mBoxCursorID);
}


static void loadOptionsScreen() {
	instantiateActor(getBoxCursorHandler());

	gOptionsScreenData.mWhiteTexture = getEmptyWhiteTexture();

	char folder[1024];
	loadMugenDefScript(&gOptionsScreenData.mScript, "assets/data/system.def");
	getPathToFile(folder, "assets/data/system.def");
	setWorkingDirectory(folder);

	char* text = getAllocatedMugenDefStringVariable(&gOptionsScreenData.mScript, "Files", "spr");
	gOptionsScreenData.mSprites = loadMugenSpriteFileWithoutPalette(text);
	gOptionsScreenData.mAnimations = loadMugenAnimationFile("system.def");
	freeMemory(text);

	text = getAllocatedMugenDefStringVariable(&gOptionsScreenData.mScript, "Files", "snd");
	gOptionsScreenData.mSounds = loadMugenSoundFile(text);
	freeMemory(text);

	loadOptionsHeader();
	loadMenuBackground(&gOptionsScreenData.mScript, &gOptionsScreenData.mSprites, &gOptionsScreenData.mAnimations, "OptionBGdef", "OptionBG");

	gOptionsScreenData.mHeaderTextID = addMugenTextMugenStyle("OPTIONS", makePosition(160, 20, 60), makeVector3DI(2, 0, 0));
	gOptionsScreenData.mBackgroundAnimationID = playOneFrameAnimationLoop(makePosition(52, 35, 40), &gOptionsScreenData.mWhiteTexture);
	setAnimationColor(gOptionsScreenData.mBackgroundAnimationID, 0, 0, 0.6);
	setAnimationTransparency(gOptionsScreenData.mBackgroundAnimationID, 0.7);

	loadKeyConfigOptionsScreen();

	setWorkingDirectory("/");
}

static void inputConfigToGeneralOptions() {
	if (gOptionsScreenData.mActiveScreen != OPTION_SCREEN_INPUT_CONFIG) return;

	unloadInputConfigOptionsScreen();
	loadGeneralOptionsScreen();
}

static void generalToInputConfigOptions() {
	if (gOptionsScreenData.mActiveScreen != OPTION_SCREEN_GENERAL) return;

	unloadGeneralOptionsScreen();
	loadInputConfigOptionsScreen();
}

static void keyConfigToInputConfigOptions() {
	if (gOptionsScreenData.mActiveScreen != OPTION_SCREEN_KEY_CONFIG) return;

	unloadKeyConfigOptionsScreen();
	loadInputConfigOptionsScreen();
}

static void inputConfigToKeyConfigOptions() {
	if (gOptionsScreenData.mActiveScreen != OPTION_SCREEN_INPUT_CONFIG) return;

	unloadInputConfigOptionsScreen();
	loadKeyConfigOptionsScreen();
}

static void updateOptionScreenGeneralSelect() {
	if (gOptionsScreenData.mGeneral.mSelected == GENERAL_SETTING_INPUT_CONFIG) {
		generalToInputConfigOptions();
	}
	else if (gOptionsScreenData.mGeneral.mSelected == GENERAL_SETTING_RETURN) {
		tryPlayMugenSound(&gOptionsScreenData.mSounds, gOptionsScreenData.mHeader.mCancelSound.x, gOptionsScreenData.mHeader.mCancelSound.y);
		setNewScreen(getDreamTitleScreen());
	} 
}

static string gDifficultyNames[] = {
	"Invalid",
	"Easy 1",
	"Easy 2",
	"Medium 3",
	"Medium 4",
	"Medium 5",
	"Hard 6",
	"Hard 7",
	"Hard 8",
};

static void setDifficultyOptionText() {
	changeMugenText(gOptionsScreenData.mGeneral.mSettingTextID[GENERAL_SETTING_DIFFICULTY], gDifficultyNames[getDifficulty()].data());
}

static void changeDifficultyOption(int tDelta) {
	int currentDifficulty = getDifficulty();
	if (tDelta == -1 && currentDifficulty > 1) currentDifficulty--;
	if (tDelta == 1 && currentDifficulty < 8) currentDifficulty++;
	setDifficulty(currentDifficulty);
	setDifficultyOptionText();
}

static void setLifeOptionText() {
	char lifeText[30];
	sprintf(lifeText, "%d%%", getLifeStartPercentageNumber());
	changeMugenText(gOptionsScreenData.mGeneral.mSettingTextID[GENERAL_SETTING_LIFE], lifeText);
}

static void changeLifeOption(int tDelta) {
	int lifeNumber = getLifeStartPercentageNumber();
	if (tDelta == -1 && lifeNumber > 30) lifeNumber -=10;
	if (tDelta == 1 && lifeNumber < 300) lifeNumber += 10;
	setLifeStartPercentageNumber(lifeNumber);
	setLifeOptionText();
}

static void setTimeLimitOptionText() {
	if (isGlobalTimerInfinite()) {
		changeMugenText(gOptionsScreenData.mGeneral.mSettingTextID[GENERAL_SETTING_TIME_LIMIT], "None");
	}
	else {
		char timeLimitText[30];
		sprintf(timeLimitText, "%d", getGlobalTimerDuration());
		changeMugenText(gOptionsScreenData.mGeneral.mSettingTextID[GENERAL_SETTING_TIME_LIMIT], timeLimitText);
	}
}


static void changeTimeLimitOption(int tDelta) {
	int timeLimit = getGlobalTimerDuration();
	int isInfinite = isGlobalTimerInfinite();
	if (tDelta == -1) {
		if (isInfinite) {
			timeLimit = 99;
			isInfinite = 0;
		}
		else if (timeLimit == 99) timeLimit = 80;
		else if(timeLimit > 20) timeLimit -= 20;
	}
	if (tDelta == 1 && !isInfinite) {
		if(timeLimit == 80) timeLimit = 99;
		else if (timeLimit == 99) {
			isInfinite = 1;
		}
		else {
			timeLimit += 20;
		}
	}

	if (isInfinite) {
		setGlobalTimerInfinite();
	}
	else {
		setGlobalTimerDuration(timeLimit);
	}
	setTimeLimitOptionText();
}

static void setGameSpeedOptionText() {
	int currentSpeed = getGlobalGameSpeed();
	char gameSpeedText[30];
	if (!currentSpeed) {
		changeMugenText(gOptionsScreenData.mGeneral.mSettingTextID[GENERAL_SETTING_GAME_SPEED], "Normal");
	}
	else if(currentSpeed > 0){
		sprintf(gameSpeedText, "Fast %d", currentSpeed);
		changeMugenText(gOptionsScreenData.mGeneral.mSettingTextID[GENERAL_SETTING_GAME_SPEED], gameSpeedText);
	}
	else {
		sprintf(gameSpeedText, "Slow %d", -currentSpeed);
		changeMugenText(gOptionsScreenData.mGeneral.mSettingTextID[GENERAL_SETTING_GAME_SPEED], gameSpeedText);
	}
}

static void changeGameSpeedOption(int tDelta) {
	int currentSpeed = getGlobalGameSpeed();
	if (tDelta == -1 && currentSpeed > -9) currentSpeed--;
	if (tDelta == 1 && currentSpeed < 9) currentSpeed++;
	setGlobalGameSpeed(currentSpeed);
	setGameSpeedOptionText();
}

static void setVolumeOptionText(GeneralSettingType tType, int(*tGet)()) {
	int currentVolume = tGet();
	if (currentVolume == 0) {
		changeMugenText(gOptionsScreenData.mGeneral.mSettingTextID[tType], "Off");
	}
	else if (currentVolume == 100) {
		changeMugenText(gOptionsScreenData.mGeneral.mSettingTextID[tType], "Max");
	}  
	else {
		char volumeText[30];
		sprintf(volumeText, "%d", currentVolume);
		changeMugenText(gOptionsScreenData.mGeneral.mSettingTextID[tType], volumeText);
	}
}

static void changeVolumeOption(int tDelta, GeneralSettingType tType, int(*tGet)(), void(*tSet)(int)) {
	int currentVolume = tGet();
	if (tDelta == -1 && currentVolume > 0) currentVolume--;
	if (tDelta == 1 && currentVolume < 100) currentVolume++;
	tSet(currentVolume);
	setVolumeOptionText(tType, tGet);
}

static void changeSelectedGeneralOption(int tDelta) {
	if (gOptionsScreenData.mGeneral.mSelected == GENERAL_SETTING_DIFFICULTY) {
		changeDifficultyOption(tDelta);
	}
	else if (gOptionsScreenData.mGeneral.mSelected == GENERAL_SETTING_LIFE) {
		changeLifeOption(tDelta);
	}
	else if (gOptionsScreenData.mGeneral.mSelected == GENERAL_SETTING_TIME_LIMIT) {
		changeTimeLimitOption(tDelta);
	}
	else if (gOptionsScreenData.mGeneral.mSelected == GENERAL_SETTING_GAME_SPEED) {
		changeGameSpeedOption(tDelta);
	}
	else if (gOptionsScreenData.mGeneral.mSelected == GENERAL_SETTING_WAV_VOLUME) {
		changeVolumeOption(tDelta, GENERAL_SETTING_WAV_VOLUME, getGameWavVolume, setGameWavVolume);
	}
	else if (gOptionsScreenData.mGeneral.mSelected == GENERAL_SETTING_MIDI_VOLUME) {
		changeVolumeOption(tDelta, GENERAL_SETTING_MIDI_VOLUME, getGameMidiVolume, setGameMidiVolume);
	}
}

static void updateOptionScreenGeneral() {
	if (hasPressedUpFlank()) {
		updateSelectedGeneralOption(-1);
	} else if (hasPressedDownFlank()) {
		updateSelectedGeneralOption(1);
	}

	if (hasPressedLeftFlank()) {
		changeSelectedGeneralOption(-1);
	}
	else if (hasPressedRightFlank()) {
		changeSelectedGeneralOption(1);
	}

	if (hasPressedAFlank() || hasPressedStartFlank()) {
		updateOptionScreenGeneralSelect();
	}
	else if (hasPressedXFlank()) {
		generalToInputConfigOptions();
	}

	if (hasPressedBFlank()) {
		setNewScreen(getDreamTitleScreen());
	}
}

static void updateSelectedInputConfigOption(int tPlayerIndex, int tDelta) {
	setSelectedInputConfigOptionInactive(tPlayerIndex);
	gOptionsScreenData.mInputConfig.mSelected[tPlayerIndex] += tDelta;
	gOptionsScreenData.mInputConfig.mSelected[tPlayerIndex] = (gOptionsScreenData.mInputConfig.mSelected[tPlayerIndex] + INPUT_CONFIG_SETTING_AMOUNT) % INPUT_CONFIG_SETTING_AMOUNT;
	tryPlayMugenSound(&gOptionsScreenData.mSounds, gOptionsScreenData.mHeader.mCursorMoveSound.x, gOptionsScreenData.mHeader.mCursorMoveSound.y);
	setSelectedInputConfigOptionActive(tPlayerIndex);
}

static void updateOptionScreenInputConfigSelect(int i) {
	if (gOptionsScreenData.mInputConfig.mSelected[i] == INPUT_CONFIG_SETTING_KEY_CONFIG) {
		inputConfigToKeyConfigOptions();
	}
	else if (gOptionsScreenData.mInputConfig.mSelected[i] == INPUT_CONFIG_SETTING_RETURN) {
		inputConfigToGeneralOptions();
	}
}

static void updateOptionScreenInputConfig() {

	for (int i = 0; i < 2; i++) {
		if (hasPressedUpFlankSingle(i)) {
			updateSelectedInputConfigOption(i, -1);
		}
		else if (hasPressedDownFlankSingle(i)) {
			updateSelectedInputConfigOption(i, 1);
		}

		if (hasPressedAFlankSingle(i) || hasPressedStartFlankSingle(i)) {
			updateOptionScreenInputConfigSelect(i);
		}
	}



	if (hasPressedBFlank()) {
		inputConfigToGeneralOptions();
	}
}

static void updateSelectedKeyConfigOption(int tDeltaX, int tDeltaY) {
	setSelectedKeyConfigOptionInactive();
	gOptionsScreenData.mKeyConfig.mSelected.x += tDeltaX;
	gOptionsScreenData.mKeyConfig.mSelected.x = (gOptionsScreenData.mKeyConfig.mSelected.x + 2) % 2;

	if (tDeltaY == 1) {
		if (gOptionsScreenData.mKeyConfig.mSelected.y == KEY_CONFIG_SETTING_CONFIG_ALL) gOptionsScreenData.mKeyConfig.mSelected.y = KEY_CONFIG_SETTING_DEFAULT;
		else gOptionsScreenData.mKeyConfig.mSelected.y += tDeltaY;
	}
	else if (tDeltaY == -1) {
		if (gOptionsScreenData.mKeyConfig.mSelected.y == KEY_CONFIG_SETTING_DEFAULT) gOptionsScreenData.mKeyConfig.mSelected.y = KEY_CONFIG_SETTING_CONFIG_ALL;
		else gOptionsScreenData.mKeyConfig.mSelected.y += tDeltaY;
	}
	gOptionsScreenData.mKeyConfig.mSelected.y = (gOptionsScreenData.mKeyConfig.mSelected.y + KEY_CONFIG_SETTING_AMOUNT) % KEY_CONFIG_SETTING_AMOUNT;
	tryPlayMugenSound(&gOptionsScreenData.mSounds, gOptionsScreenData.mHeader.mCursorMoveSound.x, gOptionsScreenData.mHeader.mCursorMoveSound.y);
	setSelectedKeyConfigOptionActive();
}

static void updateOptionScreenKeyConfigSelect() {
	if (gOptionsScreenData.mKeyConfig.mSelected.y == KEY_CONFIG_SETTING_EXIT) {
		keyConfigToInputConfigOptions();
	}
}

static void updateOptionScreenKeyConfig() {

		if (hasPressedUpFlank()) {
			updateSelectedKeyConfigOption(0, -1);
		}
		else if (hasPressedDownFlank()) {
			updateSelectedKeyConfigOption(0, 1);
		}

		if (hasPressedLeftFlank()) {
			updateSelectedKeyConfigOption(-1, 0);
		} else if (hasPressedRightFlank()) {
			updateSelectedKeyConfigOption(1, 0);
		}

		if (hasPressedAFlank() || hasPressedStartFlank()) {
			updateOptionScreenKeyConfigSelect();
		}

	if (hasPressedBFlank()) {
		keyConfigToInputConfigOptions();
	}
}

static void updateOptionScreenSelection() {
	if (gOptionsScreenData.mActiveScreen == OPTION_SCREEN_GENERAL) {
		updateOptionScreenGeneral();
	} else 	if (gOptionsScreenData.mActiveScreen == OPTION_SCREEN_INPUT_CONFIG) {
		updateOptionScreenInputConfig();
	} else if (gOptionsScreenData.mActiveScreen == OPTION_SCREEN_KEY_CONFIG) {
		updateOptionScreenKeyConfig();
	}
}

static void updateOptionsScreen() {
	updateOptionScreenSelection();
}

static Screen gOptionsScreen;

static Screen* getOptionsScreen() {
	gOptionsScreen = makeScreen(loadOptionsScreen, updateOptionsScreen);
	return &gOptionsScreen;
};

void startOptionsScreen()
{
	setNewScreen(getOptionsScreen());
}