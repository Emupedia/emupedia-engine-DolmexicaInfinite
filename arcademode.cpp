#include "arcademode.h"

#include <assert.h>
#include <algorithm>

#include <prism/mugendefreader.h>
#include <prism/math.h>
#include <prism/log.h>

#include "characterselectscreen.h"
#include "titlescreen.h"
#include "storyscreen.h"
#include "playerdefinition.h"
#include "versusscreen.h"
#include "fightscreen.h"
#include "gamelogic.h"
#include "stage.h"
#include "fightui.h"
#include "fightresultdisplay.h"
#include "config.h"
#include "victoryquotescreen.h"

using namespace std;

typedef struct {
	char mDefinitionPath[300];
	char mStagePath[300];
	char mMusicPath[300];
} ArcadeCharacter;

static struct {
	int mEnemyAmount;
	ArcadeCharacter mEnemies[100];
	int mCurrentEnemy;
	int mHasEnding;

	int mHasGameOver;
	char mGameOverPath[300];

	int mHasDefaultEnding;
	char mDefaultEndingPath[300];

	int mHasCredits;
	char mCreditsPath[300];

	int mHasVictoryQuoteScreen;
} gArcadeModeData;

static void fightFinishedCB();

static void gameOverFinishedCB() {
	setNewScreen(getDreamTitleScreen());
}

static void fightLoseCB() {
	if (gArcadeModeData.mHasGameOver) {
		setStoryDefinitionFile(gArcadeModeData.mGameOverPath);
		setStoryScreenFinishedCB(gameOverFinishedCB);
		setNewScreen(getStoryScreen());
	}
	else {
		gameOverFinishedCB();
	}
}

static void versusScreenFinishedCB() {

	int isFinalFight = gArcadeModeData.mCurrentEnemy == gArcadeModeData.mEnemyAmount - 1;
	setGameModeArcade();
	setPlayerArtificial(1, calculateAIRampDifficulty(gArcadeModeData.mCurrentEnemy, getArcadeAIRampStart(), getArcadeAIRampEnd()));
	setFightResultActive(isFinalFight && !gArcadeModeData.mHasEnding);
	startFightScreen(fightFinishedCB, fightLoseCB);
}

typedef struct {
	int mIsSelected;
	int mOrder;
	char mDefinitionPath[300];
	char mStagePath[300];
	char mMusicPath[300];
} SingleArcadeEnemy;

typedef struct {
	Vector mEnemies;
} Order;

typedef struct {
	IntMap mOrders;
} AddEnemyCaller;

static void addNewEnemyOrderToCaller(AddEnemyCaller* tCaller, int tOrder) {
	Order* e = (Order*)allocMemory(sizeof(Order));
	e->mEnemies = new_vector();

	int_map_push_owned(&tCaller->mOrders, tOrder, e);
}

static void addSingleEnemyToSelection(void* tCaller, void* tData) {
	AddEnemyCaller* caller = (AddEnemyCaller*)tCaller;
	MugenDefScriptGroupElement* element = (MugenDefScriptGroupElement*)tData;

	if (!isMugenDefStringVectorVariableAsElement(element)) return;

	MugenStringVector stringVector = getMugenDefStringVectorVariableAsElement(element);
	assert(stringVector.mSize >= 2);

	SingleArcadeEnemy* e = (SingleArcadeEnemy*)allocMemory(sizeof(SingleArcadeEnemy));
	e->mIsSelected = 0;
	e->mOrder = 1;
	getCharacterSelectNamePath(stringVector.mElement[0], e->mDefinitionPath);
	if (!isFile(e->mDefinitionPath)) {
		logWarningFormat("Unable to find def file %s. Ignoring character.", e->mDefinitionPath);
		freeMemory(e);
		return;
	}
	if (!strcmp("random", stringVector.mElement[1])) {
		strcpy(e->mStagePath, stringVector.mElement[1]);
	}
	else {
		sprintf(e->mStagePath, "%s%s", getDolmexicaAssetFolder().c_str(), stringVector.mElement[1]);
	}
	*e->mMusicPath = '\0';

	parseOptionalCharacterSelectParameters(stringVector, &e->mOrder, NULL, e->mMusicPath);

	if (!int_map_contains(&caller->mOrders, e->mOrder)) {
		addNewEnemyOrderToCaller(caller, e->mOrder);
	}
	Order* order = (Order*)int_map_get(&caller->mOrders, e->mOrder);

	vector_push_back_owned(&order->mEnemies, e);
}

static void addArcadeEnemy(SingleArcadeEnemy* tEnemy) {
	int index = gArcadeModeData.mEnemyAmount;
	ArcadeCharacter* e = &gArcadeModeData.mEnemies[index];
	strcpy(e->mDefinitionPath, tEnemy->mDefinitionPath);
	strcpy(e->mStagePath, tEnemy->mStagePath);
	strcpy(e->mMusicPath, tEnemy->mMusicPath);

	gArcadeModeData.mEnemyAmount++;
}

static void addOrderEnemies(int tOrder, int tAmount, AddEnemyCaller* tCaller) {
	if (!int_map_contains(&tCaller->mOrders, tOrder)) return;

	Order* order = (Order*)int_map_get(&tCaller->mOrders, tOrder);

	tAmount = min(tAmount, vector_size(&order->mEnemies));

	int i;
	for (i = 0; i < tAmount; i++) {
		int j;
		for (j = 0; j < 100; j++) {
			int index = randfromInteger(0, vector_size(&order->mEnemies) - 1);
			SingleArcadeEnemy* enemy = (SingleArcadeEnemy*)vector_get(&order->mEnemies, index);

			if (enemy->mIsSelected) continue;

			addArcadeEnemy(enemy);
			enemy->mIsSelected = 1;
			break;
		}
	}
}

static int unloadSingleOrder(void* tCaller, void* tData) {
	(void)tCaller;
	Order* e = (Order*)tData;
	delete_vector(&e->mEnemies);
	return 1;
}

static void generateEnemies() {
	MugenDefScript script;
	loadMugenDefScript(&script, getDolmexicaAssetFolder() + "data/select.def");

	MugenStringVector enemyTypeAmountVector = getMugenDefStringVectorVariable(&script, "Options", "arcade.maxmatches");

	MugenDefScriptGroup* group = &script.mGroups["Characters"];
	AddEnemyCaller caller;
	caller.mOrders = new_int_map();
	list_map(&group->mOrderedElementList, addSingleEnemyToSelection, &caller);


	gArcadeModeData.mEnemyAmount = 0;
	int i;
	for (i = 0; i < enemyTypeAmountVector.mSize; i++) {
		addOrderEnemies(i+1, atoi(enemyTypeAmountVector.mElement[i]), &caller);
	}

	int_map_remove_predicate(&caller.mOrders, unloadSingleOrder, NULL);
	delete_int_map(&caller.mOrders);

	unloadMugenDefScript(script);
}

static void creditsFinishedCB() {
	startArcadeMode();

}

static void endingScreenFinishedCB() {
	if (gArcadeModeData.mHasCredits) {
		setStoryDefinitionFile(gArcadeModeData.mCreditsPath);
		setStoryScreenFinishedCB(creditsFinishedCB);
		setNewScreen(getStoryScreen());
		return;
	}

	creditsFinishedCB();
}

static void startEnding() {
	char folder[1024];
	char path[1024];
	getPlayerDefinitionPath(path, 0);
	getPathToFile(folder, path);
	MugenDefScript script;
	loadMugenDefScript(&script, path);
	char* endingDefinitionFile;

	if (gArcadeModeData.mHasEnding) {
		endingDefinitionFile = getAllocatedMugenDefStringVariable(&script, "Arcade", "ending.storyboard");
		sprintf(path, "%s%s", folder, endingDefinitionFile);
		if (isFile(path)) {
			setStoryDefinitionFile(path);
			setStoryScreenFinishedCB(endingScreenFinishedCB);
			setNewScreen(getStoryScreen());
			return;
		}
	}
	else if (gArcadeModeData.mHasDefaultEnding) {
		setStoryDefinitionFile(gArcadeModeData.mDefaultEndingPath);
		setStoryScreenFinishedCB(endingScreenFinishedCB);
		setNewScreen(getStoryScreen());
		return;
	}

	endingScreenFinishedCB();

}

static void victoryQuoteFinishedCB() {
	setPlayerDefinitionPath(1, gArcadeModeData.mEnemies[gArcadeModeData.mCurrentEnemy].mDefinitionPath);

	if (!strcmp("random", gArcadeModeData.mEnemies[gArcadeModeData.mCurrentEnemy].mStagePath)) {
		MugenDefScript script;
		loadMugenDefScript(&script, getDolmexicaAssetFolder() + "data/select.def");
		setStageRandom(&script);
	}
	else {
		setDreamStageMugenDefinition(gArcadeModeData.mEnemies[gArcadeModeData.mCurrentEnemy].mStagePath, gArcadeModeData.mEnemies[gArcadeModeData.mCurrentEnemy].mMusicPath);
	}
	setVersusScreenMatchNumber(gArcadeModeData.mCurrentEnemy + 1);
	setVersusScreenFinishedCB(versusScreenFinishedCB);
	setNewScreen(getVersusScreen());
}

static void fightFinishedCB() {
	gArcadeModeData.mCurrentEnemy++;

	if (gArcadeModeData.mCurrentEnemy == gArcadeModeData.mEnemyAmount) {
		startEnding();
		return;
	}

	if (gArcadeModeData.mCurrentEnemy > 0 && gArcadeModeData.mHasVictoryQuoteScreen) {
		setVictoryQuoteScreenFinishedCB(victoryQuoteFinishedCB);
		setNewScreen(getVictoryQuoteScreen());
	}
	else {
		victoryQuoteFinishedCB();
	}
}

static void introScreenFinishedCB() {
	gArcadeModeData.mCurrentEnemy = -1;
	fightFinishedCB();
}

static void characterSelectFinishedCB() {
	generateEnemies();

	char folder[1024];
	char path[1024];
	getPlayerDefinitionPath(path, 0);
	getPathToFile(folder, path);
	MugenDefScript script; 
	loadMugenDefScript(&script, path);
	int hasIntro;
	char* introDefinitionFile;
	hasIntro = isMugenDefStringVariable(&script, "Arcade", "intro.storyboard");
	gArcadeModeData.mHasEnding = isMugenDefStringVariable(&script, "Arcade", "ending.storyboard");

	int isGoingToStory = 0;

	if (hasIntro) {
		introDefinitionFile = getAllocatedMugenDefStringVariable(&script, "Arcade", "intro.storyboard");
		sprintf(path, "%s%s", folder, introDefinitionFile);
		if (isFile(path)) {
			setStoryDefinitionFile(path);
			setStoryScreenFinishedCB(introScreenFinishedCB);
			isGoingToStory = 1;
		}
		freeMemory(introDefinitionFile);
	}


	unloadMugenDefScript(script);

	if (isGoingToStory) {
		setNewScreen(getStoryScreen());
	}
	else {
		introScreenFinishedCB();
	}
}

static void loadWinScreenFromScript(MugenDefScript* tScript) {
	int isEnabled = getMugenDefIntegerOrDefault(tScript, "Win Screen", "enabled", 0);
	char* message = getAllocatedMugenDefStringOrDefault(tScript, "Win Screen", "wintext.text", "Congratulations!");
	Vector3DI font = getMugenDefVectorIOrDefault(tScript, "Win Screen", "wintext.font", makeVector3DI(2, 0, 0));
	Position offset = getMugenDefVectorOrDefault(tScript, "Win Screen", "wintext.offset", makePosition(0, 0, 0));
	int displayTime = getMugenDefIntegerOrDefault(tScript, "Win Screen", "wintext.displaytime", -1);
	int layerNo = getMugenDefIntegerOrDefault(tScript, "Win Screen", "wintext.layerno", 2);
	int fadeInTime = getMugenDefIntegerOrDefault(tScript, "Win Screen", "fadein.time", 30);
	int poseTime = getMugenDefIntegerOrDefault(tScript, "Win Screen", "pose.time", 300);
	int fadeOutTime = getMugenDefIntegerOrDefault(tScript, "Win Screen", "fadeout.time", 30);

	setFightResultData(isEnabled, message, font, offset, displayTime, layerNo, fadeInTime, poseTime, fadeOutTime, 1);

	freeMemory(message);
}

static void loadSingleStoryboard(MugenDefScript* tScript, const char* tFolder, const char* tGroup, int* oHasStoryboard, char* oStoryBoardPath) {

	*oHasStoryboard = getMugenDefIntegerOrDefault(tScript, tGroup, "enabled", 0);
	if (!(*oHasStoryboard)) return;

	const auto name = getSTLMugenDefStringOrDefault(tScript, tGroup, "storyboard", "");
	if (isFile(name.c_str())) {
		strcpy(oStoryBoardPath, name.c_str());
	}
	else {
		sprintf(oStoryBoardPath, "%s%s", tFolder, name.c_str());
	}
}

static void loadStoryboardsFromScript(MugenDefScript* tScript, const char* tFolder) {
	
	loadSingleStoryboard(tScript, tFolder, "Game Over Screen", &gArcadeModeData.mHasGameOver, gArcadeModeData.mGameOverPath);
	loadSingleStoryboard(tScript, tFolder, "Default Ending", &gArcadeModeData.mHasDefaultEnding, gArcadeModeData.mDefaultEndingPath);
	loadSingleStoryboard(tScript, tFolder, "End Credits", &gArcadeModeData.mHasCredits, gArcadeModeData.mCreditsPath);
}

static void loadVictoryQuoteScreenFromScript(MugenDefScript* tScript) {
	gArcadeModeData.mHasVictoryQuoteScreen = getMugenDefIntegerOrDefault(tScript, "Victory Screen", "enabled", 0);
}

static void loadArcadeModeHeaderFromScript() {
	char scriptPath[1000];
	char folder[1000];

	strcpy(scriptPath, (getDolmexicaAssetFolder() + getMotifPath()).c_str());
	getPathToFile(folder, scriptPath);

	MugenDefScript script;
	loadMugenDefScript(&script, scriptPath);

	loadWinScreenFromScript(&script);
	loadStoryboardsFromScript(&script, folder);
	loadVictoryQuoteScreenFromScript(&script);

	unloadMugenDefScript(script);
}

void startArcadeMode()
{
	loadArcadeModeHeaderFromScript();

	setCharacterSelectScreenModeName("Arcade");
	setCharacterSelectOnePlayer();
	setCharacterSelectStageInactive();
	setCharacterSelectFinishedCB(characterSelectFinishedCB);
	setNewScreen(getCharacterSelectScreen());
}

int getArcadeModeMatchNumber()
{
	return gArcadeModeData.mCurrentEnemy + 1;
}