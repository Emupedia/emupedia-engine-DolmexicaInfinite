#pragma once

#include <prism/datastructures.h>

#include <prism/mugenspritefilereader.h>
#include <prism/mugenanimationhandler.h>
#include <prism/mugensoundfilereader.h>
#include <prism/actorhandler.h>

#include "mugenstatereader.h"
#include "mugencommandreader.h"
#include "playerhitdata.h"
#include "afterimage.h"

struct PhysicsHandlerElement;

typedef enum {
	PLAYER_BIND_POSITION_TYPE_AXIS,
	PLAYER_BIND_POSITION_TYPE_HEAD,
	PLAYER_BIND_POSITION_TYPE_MID,
} DreamPlayerBindPositionType;

typedef enum {
	VICTORY_TYPE_NORMAL,
	VICTORY_TYPE_SPECIAL,
	VICTORY_TYPE_HYPER,
	VICTORY_TYPE_THROW,
	VICTORY_TYPE_CHEESE,
	VICTORY_TYPE_TIMEOVER,
	VICTORY_TYPE_SUICIDE,
	VICTORY_TYPE_TEAMMATE, // never used because team modes not part of Dolmexica
} VictoryType;

#define PLAYER_Z 40
#define PLAYER_Z_PRIORITY_DELTA 0.1
#define PLAYER_Z_PLAYER_2_OFFSET 0.01
#define EXPLOD_SPRITE_Z_OFFSET 0.001

typedef struct {
	char mDefinitionPath[1024];
	int mHasPalettePath;
	char* mPalettePath;
	char* mSpritePath;
	DreamMugenCommands mCommands;
	MugenAnimations mAnimations;
	MugenSpriteFile mSprites;
	MugenSounds mSounds;
	DreamMugenConstants mConstants;
} DreamPlayerFiles;


typedef struct {
	char mName[100];
	char mDisplayName[100];
	char mVersion[100];
	char mMugenVersion[100];
	char mAuthor[100];

	char mPaletteDefaults[100];
	Vector3DI mLocalCoordinates;

} DreamPlayerHeaderConstants;

typedef struct {
	int mHasCustomDisplayName;
	char mDisplayName[100];
} DreamPlayerHeaderCustomOverrides;

typedef struct {
	DreamPlayerHeaderConstants mConstants;
	DreamPlayerFiles mFiles;
	DreamPlayerHeaderCustomOverrides mCustomOverrides;
} DreamPlayerHeader;

typedef struct {
	MugenAnimationHandlerElement* mAnimationElement;

	Position mShadowPosition;

} DreamPlayerShadow;

typedef struct {
	MugenAnimationHandlerElement* mAnimationElement;
	Position mPosition;

} DreamPlayerReflection;

typedef struct {
	int mLastDustTime;

} DreamPlayerDust;

typedef struct {
	int mCollisionTextID;
	Position mCollisionTextPosition;

} DreamPlayerDebugData;

struct DreamPlayer {
	DreamPlayerHeader* mHeader;
	DreamMugenConstantsSizeData mCustomSizeData;

	DreamPlayer* mRoot;
	DreamPlayer* mOtherPlayer;
	int mPreferredPalette;
	int mRootID;
	int mControllerID;
	int mID;
	int mIsHelper;
	int mIsProjectile;
	int mProjectileID;
	int mProjectileDataID;

	List mHelpers; // contains DreamPlayer
	std::list<PlayerHitData> mReceivedHitData;
	std::set<DreamPlayer*> mReceivedReversalDefPlayers;
	std::set<std::pair<int, DreamPlayer*>> mActiveTargets;
	DreamPlayer* mParent;
	int mHelperIDInParent;
	int mHelperIDInRoot;
	int mHelperIDInStore;

	IntMap mProjectiles; // contains DreamPlayer
	int mHasLastContactProjectile;
	int mLastContactProjectileID;
	int mLastContactProjectileTime;
	int mLastContactProjectileWasCanceled;
	int mLastContactProjectileWasGuarded;
	int mLastContactProjectileWasHit;

	int mAILevel;

	int mVars[100];
	int mSystemVars[100];
	double mFloatVars[100];
	double mSystemFloatVars[100];

	int mCommandID;
	int mStateMachineID;
	MugenAnimations* mActiveAnimations;
	MugenAnimationHandlerElement* mAnimationElement;

	PhysicsHandlerElement* mPhysicsElement;
	int mHitDataID;
	PlayerHitData mPassiveHitData;
	PlayerHitData mActiveHitData;
	PlayerHitOverrides mHitOverrides;

	DreamMugenStateType mStateType;
	DreamMugenStateMoveType mMoveType;
	DreamMugenStatePhysics mStatePhysics;

	int mIsInControl;
	int mMoveContactCounter;
	int mMoveHit;
	int mMoveGuarded;
	int mLastHitGuarded;
	int mIsAlive;
	FaceDirection mFaceDirection;

	int mNoWalkFlag;
	int mNoAutoTurnFlag;
	int mNoLandFlag;
	int mPushDisabledFlag;
	int mNoJuggleCheckFlag;
	int mIntroFlag;
	int mNoAirGuardFlag;
	int mNoCrouchGuardFlag;
	int mNoStandGuardFlag;
	int mNoKOSoundFlag;
	int mNoKOSlowdownFlag;
	int mUnguardableFlag;
	int mTransparencyFlag;

	int mWidthFlag;
	int mInvisibilityFlag;
	Vector3DI mOneTickStageWidth;
	Vector3DI mOneTickPlayerWidth;
	Vector3D mDrawOffset;

	int mJumpFlank;
	int mAirJumpCounter;

	int mIsHitOver;
	int mIsFalling;
	int mCanRecoverFromFall;
	int mRecoverTimeSinceHitPause;
	int mRecoverTime;

	double mDefenseMultiplier;
	double mSuperDefenseMultiplier;

	int mIsFrozen;
	Position mFreezePosition;

	int mIsLyingDown;
	int mLyingDownTime;

	int mIsHitPaused;
	int mHitPauseNow;
	int mHitPauseDuration;

	int mSuperMoveTime;
	int mPauseMoveTime;

	int mIsHitShakeActive;
	int mHitShakeNow;
	int mHitShakeDuration;

	int mIsHitOverWaitActive;
	int mHitOverNow;
	int mHitOverDuration;

	int mIsAngleActive;
	double mAngle;

	Vector3D mTempScale;

	int mLife;
	int mPower;

	int mCheeseWinFlag;
	int mSuicideWinFlag;

	int mHitCount;
	int mFallAmountInCombo;

	double mAttackMultiplier;

	int mMoveReversed;

	int mIsBound;
	int mBoundNow;
	int mBoundDuration;
	int mBoundFaceSet;
	Position mBoundOffset;
	DreamPlayerBindPositionType mBoundPositionType;
	DreamPlayer* mBoundTarget;
	int mBoundID;

	List mBoundHelpers;

	int mRoundsExisted;
	int mComboCounter;
	int mDisplayedComboCounter;

	int mRoundsWon;

	int mIsBoundToScreenForever;
	int mIsBoundToScreenForTick;
	Vector3DI mIsCameraFollowing;

	double mStartLifePercentage;

	int mIsGuardingInternally;
	int mIsBeingJuggled;
	int mAirJugglePoints;

	double mTimeDilatationNow;
	int mTimeDilatationUpdates;
	double mTimeDilatation;

	DreamPlayerDust mDustClouds[2];

	DreamHitDefAttributeSlot mNotHitBy[2];

	DreamPlayerShadow mShadow;
	DreamPlayerReflection mReflection;
	DreamPlayerAfterImage mAfterImage;
	DreamPlayerDebugData mDebug;

	int mIsDestroyed;
};

void loadPlayers(MemoryStack* tMemoryStack);
void loadPlayerSprites();
void unloadPlayers();
void resetPlayers();
void resetPlayersEntirely();
void resetPlayerPosition(DreamPlayer* p);
void updatePlayers();
void drawPlayers();

ActorBlueprint getPreStateMachinePlayersBlueprint();
ActorBlueprint getPostStateMachinePlayersBlueprint();

int hasLoadedPlayerSprites();

void playerHitCB(void* tData, void* tHitData, int tOtherCollisionList);
void playerReversalHitCB(void* tData, void* tHitData, int tOtherCollisionList);

void setPlayerDefinitionPath(int i, const char* tDefinitionPath);
void getPlayerDefinitionPath(char* tDst, int i);
void setPlayerPreferredPalette(int i, int tPalette);
void setPlayerPreferredPaletteRandom(int i);

DreamPlayer* getRootPlayer(int i);
DreamPlayer* getPlayerRoot(DreamPlayer* p);
DreamPlayer* getPlayerParent(DreamPlayer* p);

int getPlayerState(DreamPlayer* p); 
int getPlayerPreviousState(DreamPlayer* p);
int getPlayerStateJugglePoints(DreamPlayer* p);

DreamMugenStateType getPlayerStateType(DreamPlayer* p);
void setPlayerStateType(DreamPlayer* p, DreamMugenStateType tType);
DreamMugenStateMoveType getPlayerStateMoveType(DreamPlayer* p);
void setPlayerStateMoveType(DreamPlayer* p, DreamMugenStateMoveType tType);


int getPlayerControl(DreamPlayer* p);
void setPlayerControl(DreamPlayer* p, int tNewControl);

DreamMugenStatePhysics getPlayerPhysics(DreamPlayer* p);
void setPlayerPhysics(DreamPlayer* p, DreamMugenStatePhysics tNewPhysics);

int getPlayerMoveContactCounter(DreamPlayer* p);
void resetPlayerMoveContactCounter(DreamPlayer* p);
void setPlayerMoveContactCounterActive(DreamPlayer* p);

int getPlayerVariable(DreamPlayer* p, int tIndex);
int* getPlayerVariableReference(DreamPlayer* p, int tIndex);
void setPlayerVariable(DreamPlayer* p, int tIndex, int tValue);
void addPlayerVariable(DreamPlayer* p, int tIndex, int tValue);
int getPlayerSystemVariable(DreamPlayer* p, int tIndex);
void setPlayerSystemVariable(DreamPlayer* p, int tIndex, int tValue);
void addPlayerSystemVariable(DreamPlayer* p, int tIndex, int tValue);
double getPlayerFloatVariable(DreamPlayer* p, int tIndex);
void setPlayerFloatVariable(DreamPlayer* p, int tIndex, double tValue);
void addPlayerFloatVariable(DreamPlayer* p, int tIndex, double tValue);
double getPlayerSystemFloatVariable(DreamPlayer* p, int tIndex);
void setPlayerSystemFloatVariable(DreamPlayer* p, int tIndex, double tValue);
void addPlayerSystemFloatVariable(DreamPlayer* p, int tIndex, double tValue);

int getPlayerTimeInState(DreamPlayer* p);
int getPlayerAnimationNumber(DreamPlayer* p);
int getPlayerAnimationStep(DreamPlayer* p);
int getPlayerAnimationStepAmount(DreamPlayer* p);
int getPlayerAnimationStepDuration(DreamPlayer* p);
int getPlayerAnimationTimeDeltaUntilFinished(DreamPlayer* p);
int hasPlayerAnimationLooped(DreamPlayer* p);
int getPlayerAnimationDuration(DreamPlayer* p);
int getPlayerAnimationTime(DreamPlayer* p);
int getPlayerSpriteGroup(DreamPlayer* p);
int getPlayerSpriteElement(DreamPlayer* p);

Vector3D getPlayerPosition(DreamPlayer* p, int tCoordinateP);
double getPlayerPositionBasedOnScreenCenterX(DreamPlayer* p, int tCoordinateP);
double getPlayerScreenPositionX(DreamPlayer* p, int tCoordinateP);
double getPlayerPositionX(DreamPlayer* p, int tCoordinateP);
double getPlayerPositionBasedOnStageFloorY(DreamPlayer* p, int tCoordinateP);
double getPlayerScreenPositionY(DreamPlayer* p, int tCoordinateP);
double getPlayerPositionY(DreamPlayer* p, int tCoordinateP);
double getPlayerVelocityX(DreamPlayer* p, int tCoordinateP);
double getPlayerVelocityY(DreamPlayer* p, int tCoordinateP);

int getPlayerDataLife(DreamPlayer* p);
int getPlayerDataAttack(DreamPlayer* p);
int getPlayerDataDefense(DreamPlayer* p); 
int getPlayerDataLiedownTime(DreamPlayer* p);
int getPlayerDataAirjuggle(DreamPlayer* p);
int getPlayerDataSparkNo(DreamPlayer* p);
int getPlayerDataGuardSparkNo(DreamPlayer* p);
int getPlayerDataKOEcho(DreamPlayer* p);
int getPlayerDataIntPersistIndex(DreamPlayer* p);
int getPlayerDataFloatPersistIndex(DreamPlayer* p);

int getPlayerSizeAirBack(DreamPlayer* p);
int getPlayerSizeAirFront(DreamPlayer* p);
int getPlayerSizeAttackDist(DreamPlayer* p);
int getPlayerSizeProjectileAttackDist(DreamPlayer* p);
int getPlayerSizeProjectilesDoScale(DreamPlayer* p);
int getPlayerSizeShadowOffset(DreamPlayer* p);
int getPlayerSizeDrawOffsetX(DreamPlayer* p);
int getPlayerSizeDrawOffsetY(DreamPlayer* p);

double getPlayerVelocityAirGetHitGroundRecoverX(DreamPlayer* p);
double getPlayerVelocityAirGetHitGroundRecoverY(DreamPlayer* p);
double getPlayerVelocityAirGetHitAirRecoverMulX(DreamPlayer* p);
double getPlayerVelocityAirGetHitAirRecoverMulY(DreamPlayer* p);
double getPlayerVelocityAirGetHitAirRecoverAddX(DreamPlayer* p);
double getPlayerVelocityAirGetHitAirRecoverAddY(DreamPlayer* p);
double getPlayerVelocityAirGetHitAirRecoverBack(DreamPlayer* p);
double getPlayerVelocityAirGetHitAirRecoverFwd(DreamPlayer* p);
double getPlayerVelocityAirGetHitAirRecoverUp(DreamPlayer* p);
double getPlayerVelocityAirGetHitAirRecoverDown(DreamPlayer* p);

int getPlayerMovementAirJumpNum(DreamPlayer* p);
void setPlayerMovementAirJumpNum(DreamPlayer* p, int tAmount); // for testing only
int getPlayerMovementAirJumpHeight(DreamPlayer* p);
double getPlayerMovementJumpChangeAnimThreshold(DreamPlayer* p);
double getPlayerMovementAirGetHitAirRecoverYAccel(DreamPlayer* p);

double getPlayerStandFriction(DreamPlayer* p);
double getPlayerStandFrictionThreshold(DreamPlayer* p);
double getPlayerCrouchFriction(DreamPlayer* p);
double getPlayerCrouchFrictionThreshold(DreamPlayer* p);
double getPlayerAirGetHitGroundLevelY(DreamPlayer* p);
double getPlayerAirGetHitGroundRecoveryGroundLevelY(DreamPlayer* p);
double getPlayerAirGetHitGroundRecoveryGroundYTheshold(DreamPlayer* p);
double getPlayerAirGetHitAirRecoveryVelocityYThreshold(DreamPlayer* p);
double getPlayerAirGetHitTripGroundLevelY(DreamPlayer* p);
double getPlayerDownBounceOffsetX(DreamPlayer* p);
double getPlayerDownBounceOffsetY(DreamPlayer* p);
double getPlayerDownVerticalBounceAcceleration(DreamPlayer* p);
double getPlayerDownBounceGroundLevel(DreamPlayer* p);
double getPlayerLyingDownFrictionThreshold(DreamPlayer* p);
double getPlayerVerticalAcceleration(DreamPlayer* p);

double getPlayerForwardWalkVelocityX(DreamPlayer* p);
double getPlayerBackwardWalkVelocityX(DreamPlayer* p);
double getPlayerForwardRunVelocityX(DreamPlayer* p);
double getPlayerForwardRunVelocityY(DreamPlayer* p);
double getPlayerBackwardRunVelocityX(DreamPlayer* p);
double getPlayerBackwardRunVelocityY(DreamPlayer* p);
double getPlayerBackwardRunJumpVelocityX(DreamPlayer* p);
double getPlayerForwardRunJumpVelocityX(DreamPlayer* p);
double getPlayerNeutralJumpVelocityX(DreamPlayer* p);
double getPlayerForwardJumpVelocityX(DreamPlayer* p);
double getPlayerBackwardJumpVelocityX(DreamPlayer* p);
double getPlayerJumpVelocityY(DreamPlayer* p);
double getPlayerNeutralAirJumpVelocityX(DreamPlayer* p);
double getPlayerForwardAirJumpVelocityX(DreamPlayer* p);
double getPlayerBackwardAirJumpVelocityX(DreamPlayer* p);
double getPlayerAirJumpVelocityY(DreamPlayer* p);

int isPlayerAlive(DreamPlayer* p);
int isPlayerDestroyed(DreamPlayer* p);

void setPlayerVelocityX(DreamPlayer* p, double x, int tCoordinateP);
void setPlayerVelocityY(DreamPlayer* p, double y, int tCoordinateP);
void multiplyPlayerVelocityX(DreamPlayer* p, double x, int tCoordinateP);
void multiplyPlayerVelocityY(DreamPlayer* p, double y, int tCoordinateP);
void addPlayerVelocityX(DreamPlayer* p, double x, int tCoordinateP);
void addPlayerVelocityY(DreamPlayer* p, double y, int tCoordinateP);

void setPlayerPosition(DreamPlayer* p, Position tPosition, int tCoordinateP);
void setPlayerPositionX(DreamPlayer* p, double x, int tCoordinateP);
void setPlayerPositionY(DreamPlayer* p, double y, int tCoordinateP);
void addPlayerPositionX(DreamPlayer* p, double x, int tCoordinateP);
void addPlayerPositionY(DreamPlayer* p, double y, int tCoordinateP);
void setPlayerPositionBasedOnScreenCenterX(DreamPlayer* p, double x, int tCoordinateP);

int isPlayerCommandActive(DreamPlayer* p, const char* tCommandName);
int isPlayerCommandActiveWithLookup(DreamPlayer* p, int tCommandLookupIndex);

int hasPlayerState(DreamPlayer* p, int mNewState);
int hasPlayerStateSelf(DreamPlayer* p, int mNewState);
void changePlayerState(DreamPlayer* p, int mNewState);
void changePlayerStateToSelf(DreamPlayer* p, int mNewState);
void changePlayerStateToOtherPlayerStateMachine(DreamPlayer* p, DreamPlayer* tOtherPlayer, int mNewState);
void changePlayerStateToOtherPlayerStateMachineBeforeImmediatelyEvaluatingIt(DreamPlayer* p, DreamPlayer* tOtherPlayer, int mNewState);
void changePlayerStateBeforeImmediatelyEvaluatingIt(DreamPlayer* p, int mNewState);
void changePlayerStateToSelfBeforeImmediatelyEvaluatingIt(DreamPlayer* p, int tNewState);
void changePlayerStateIfDifferent(DreamPlayer* p, int tNewState);
void setPlayerStatemachineToUpdateAgain(DreamPlayer* p);

void changePlayerAnimation(DreamPlayer* p, int tNewAnimation);
void changePlayerAnimationWithStartStep(DreamPlayer* p, int tNewAnimation, int tStartStep);
void changePlayerAnimationToPlayer2AnimationWithStartStep(DreamPlayer* p, int tNewAnimation, int tStartStep);
void setPlayerAnimationFinishedCallback(DreamPlayer* p, void(*tFunc)(void*), void* tCaller);

int isPlayerStartingAnimationElementWithID(DreamPlayer* p, int tStepID);
int getPlayerTimeFromAnimationElement(DreamPlayer* p, int tStep);
int getPlayerAnimationElementFromTimeOffset(DreamPlayer* p, int tTime);
int isPlayerAnimationTimeOffsetInAnimation(DreamPlayer* p, int tTime);
int getPlayerAnimationTimeWhenStepStarts(DreamPlayer* p, int tStep);

double calculateSpriteZFromSpritePriority(int tPriority, int tRootID, int tIsExplod);
void setPlayerSpritePriority(DreamPlayer* p, int tPriority);

void setPlayerNoWalkFlag(DreamPlayer* p);
void setPlayerNoAutoTurnFlag(DreamPlayer* p);
void setPlayerInvisibleFlag(DreamPlayer* p);
void setPlayerNoLandFlag(DreamPlayer* p);
void setPlayerNoShadow(DreamPlayer* p);
void setAllPlayersNoShadow();
void setPlayerPushDisabledFlag(DreamPlayer* p, int tIsDisabled);
void setPlayerNoJuggleCheckFlag(DreamPlayer* p);
void setPlayerIntroFlag(DreamPlayer* p);
void setPlayerNoAirGuardFlag(DreamPlayer* p); 
void setPlayerNoCrouchGuardFlag(DreamPlayer* p);
void setPlayerNoStandGuardFlag(DreamPlayer* p);
void setPlayerNoKOSoundFlag(DreamPlayer* p);
void setPlayerNoKOSlowdownFlag(DreamPlayer* p);
void setPlayerUnguardableFlag(DreamPlayer* p);

int getPlayerNoKOSlowdownFlag(DreamPlayer* p);
int getPlayerUnguardableFlag(DreamPlayer* p);

int isPlayerInIntro(DreamPlayer* p);

int doesPlayerHaveAnimation(DreamPlayer* p, int tAnimation);
int doesPlayerHaveAnimationHimself(DreamPlayer* p, int tAnimation);

int isPlayerFalling(DreamPlayer* p);
int canPlayerRecoverFromFalling(DreamPlayer* p);
int isPlayerHitShakeOver(DreamPlayer* p);
void setPlayerHitShakeOver(DreamPlayer* p);
int isPlayerHitOver(DreamPlayer* p);
void setPlayerHitOver(DreamPlayer* p);
int getPlayerHitTime(DreamPlayer* p);
double getPlayerHitVelocityX(DreamPlayer* p, int tCoordinateP);
double getPlayerHitVelocityY(DreamPlayer* p, int tCoordinateP);

int getPlayerSlideTime(DreamPlayer* p);

double getPlayerDefenseMultiplier(DreamPlayer* p);
void setPlayerDefenseMultiplier(DreamPlayer* p, double tValue);
void setPlayerSuperDefenseMultiplier(DreamPlayer* p, double tValue);
void setPlayerPositionFrozen(DreamPlayer* p);
void setPlayerPositionUnfrozen(DreamPlayer* p);

MugenSpriteFile* getPlayerSprites(DreamPlayer* p);
MugenAnimations* getPlayerAnimations(DreamPlayer* p);
MugenAnimation* getPlayerAnimation(DreamPlayer* p, int tNumber);
MugenSounds* getPlayerSounds(DreamPlayer* p);

void setCustomPlayerDisplayName(int i, const std::string tName);
int getPlayerCoordinateP(DreamPlayer* p);
char* getPlayerDisplayName(DreamPlayer* p);
char* getPlayerName(DreamPlayer* p);
char* getPlayerAuthorName(DreamPlayer* p);

int isPlayerPaused(DreamPlayer* p);
int isPlayerHitPaused(DreamPlayer* p);
void setPlayerHitPaused(DreamPlayer* p, int tDuration);
void setPlayerUnHitPaused(DreamPlayer* p);

double getPlayerDeathVelAddY(DreamPlayer* p);
void addPlayerDamage(DreamPlayer* p, DreamPlayer* tDamagingPlayer, int tDamage);

int getPlayerTargetAmount(DreamPlayer* p);
int getPlayerTargetAmountWithID(DreamPlayer* p, int tID);
DreamPlayer* getPlayerTargetWithID(DreamPlayer* p, int tID);
void dropPlayerTargets(DreamPlayer* p, int tExcludeID, int tIsKeepingOneAtMost);

DreamPlayer* getPlayerByIndex(int i);
int getTotalPlayerAmount();

int getPlayerHelperAmount(DreamPlayer* p);
int getPlayerHelperAmountWithID(DreamPlayer* p, int tID);
DreamPlayer* getPlayerHelperOrNullIfNonexistant(DreamPlayer* p, int tID);

int getPlayerProjectileAmount(DreamPlayer* p);
int getPlayerProjectileAmountWithID(DreamPlayer* p, int tID);
int getPlayerProjectileTimeSinceCancel(DreamPlayer* p, int tID);
int getPlayerProjectileTimeSinceContact(DreamPlayer* p, int tID);
int getPlayerProjectileTimeSinceGuarded(DreamPlayer* p, int tID);
int getPlayerProjectileTimeSinceHit(DreamPlayer* p, int tID);
int getPlayerProjectileHit(DreamPlayer* p, int tID);
int getPlayerProjectileContact(DreamPlayer* p, int tID);
int getPlayerProjectileGuarded(DreamPlayer* p, int tID);

int getPlayerTimeLeftInHitPause(DreamPlayer* p);
void setPlayerPauseMoveTime(DreamPlayer* p, int tPauseMoveTime);
void setPlayerSuperMoveTime(DreamPlayer* p, int tSuperMoveTime);

double getPlayerFrontAxisDistanceToScreen(DreamPlayer* p);
double getPlayerBackAxisDistanceToScreen(DreamPlayer* p);

double getPlayerFrontBodyDistanceToScreen(DreamPlayer* p);
double getPlayerBackBodyDistanceToScreen(DreamPlayer* p);

double getPlayerFrontWidth(DreamPlayer* p);
double getPlayerFrontWidthPlayer(DreamPlayer* p);
double getPlayerFrontWidthStage(DreamPlayer* p);
double getPlayerBackWidth(DreamPlayer* p);
double getPlayerBackWidthPlayer(DreamPlayer* p);
double getPlayerBackWidthStage(DreamPlayer* p);
double getPlayerFrontX(DreamPlayer* p, int tCoordinateP);
double getPlayerFrontXPlayer(DreamPlayer* p, int tCoordinateP);
double getPlayerFrontXStage(DreamPlayer* p, int tCoordinateP);
double getPlayerBackX(DreamPlayer* p, int tCoordinateP);
double getPlayerBackXPlayer(DreamPlayer* p, int tCoordinateP);
double getPlayerBackXStage(DreamPlayer* p, int tCoordinateP);
int isPlayerInCorner(DreamPlayer* p);

double getPlayerScreenEdgeInFrontX(DreamPlayer* p);
double getPlayerScreenEdgeInBackX(DreamPlayer* p);

double getPlayerDistanceToFrontOfOtherPlayerX(DreamPlayer* p);
double getPlayerAxisDistanceX(DreamPlayer* p);
double getPlayerAxisDistanceY(DreamPlayer* p);
double getPlayerDistanceToRootX(DreamPlayer* p);
double getPlayerDistanceToRootY(DreamPlayer* p);
double getPlayerDistanceToParentX(DreamPlayer* p);
double getPlayerDistanceToParentY(DreamPlayer* p);

int getPlayerGroundSizeFront(DreamPlayer* p);
void setPlayerGroundSizeFront(DreamPlayer* p, int tGroundSizeFront);
int getPlayerGroundSizeBack(DreamPlayer* p);
void setPlayerGroundSizeBack(DreamPlayer* p, int tGroundSizeBack);
int getPlayerAirSizeFront(DreamPlayer* p);
void setPlayerAirSizeFront(DreamPlayer* p, int tAirSizeFront);
int getPlayerAirSizeBack(DreamPlayer* p);
void setPlayerAirSizeBack(DreamPlayer* p, int tAirSizeBack);
int getPlayerHeight(DreamPlayer* p);
void setPlayerHeight(DreamPlayer* p, int tHeight);

void increasePlayerRoundsExisted();
void increasePlayerRoundsWon(DreamPlayer* p);
int hasPlayerWonByKO(DreamPlayer* p);
int hasPlayerLostByKO(DreamPlayer* p);
int hasPlayerWonPerfectly(DreamPlayer* p);
int hasPlayerWonByTime(DreamPlayer* p);
int hasPlayerWon(DreamPlayer* p);
int hasPlayerLost(DreamPlayer* p);
int hasPlayerDrawn(DreamPlayer* p);

int hasPlayerMoveHitOtherPlayer(DreamPlayer* p);
int isPlayerHit(DreamPlayer* p);
int getPlayerMoveReversed(DreamPlayer* p);
int getPlayerMoveHit(DreamPlayer* p);
void setPlayerMoveHit(DreamPlayer* p);
void setPlayerMoveHitReset(DreamPlayer* p);
int getPlayerMoveGuarded(DreamPlayer* p);
void setPlayerMoveGuarded(DreamPlayer* p);
int getLastPlayerHitGuarded(DreamPlayer* p);

int getPlayerFallAmountInCombo(DreamPlayer* p);
void increasePlayerFallAmountInCombo(DreamPlayer* p);
void resetPlayerFallAmountInCombo(DreamPlayer* p);

int getPlayerHitCount(DreamPlayer* p);
int getPlayerUniqueHitCount(DreamPlayer* p);
void increasePlayerHitCount(DreamPlayer* p);
void resetPlayerHitCount(DreamPlayer* p);
void increasePlayerComboCounter(DreamPlayer* p, int tValue);

double getPlayerAttackMultiplier(DreamPlayer* p);
void setPlayerAttackMultiplier(DreamPlayer* p, double tValue);

double getPlayerFallDefenseMultiplier(DreamPlayer* p);

void setPlayerHuman(int i);
void setPlayerArtificial(int i, int tValue);
int isPlayerHuman(DreamPlayer* p);
int getPlayerAILevel(DreamPlayer* p);

void setPlayerStartLifePercentage(int tIndex, double tPercentage);
double getPlayerLifePercentage(DreamPlayer* p);
void setPlayerLife(DreamPlayer* p, DreamPlayer* tLifeGivingPlayer, int tLife);
void addPlayerLife(DreamPlayer* p, DreamPlayer* tLifeGivingPlayer, int tLife);
int getPlayerLife(DreamPlayer* p);
int getPlayerLifeMax(DreamPlayer* p);
int getPlayerPower(DreamPlayer* p);
int getPlayerPowerMax(DreamPlayer* p);
void setPlayerPower(DreamPlayer* p, int tPower);
void addPlayerPower(DreamPlayer* p, int tPower);

int isPlayerBeingAttacked(DreamPlayer* p);
int isPlayerInGuardDistance(DreamPlayer* p);
int getDefaultPlayerAttackDistance(DreamPlayer* p);

Position getPlayerHeadPosition(DreamPlayer* p);
double getPlayerHeadPositionX(DreamPlayer* p);
double getPlayerHeadPositionY(DreamPlayer* p);
void setPlayerHeadPosition(DreamPlayer* p, double tX, double tY);

Position getPlayerMiddlePosition(DreamPlayer* p);
double getPlayerMiddlePositionX(DreamPlayer* p);
double getPlayerMiddlePositionY(DreamPlayer* p);
void setPlayerMiddlePosition(DreamPlayer* p, double tX, double tY);

int getPlayerShadowOffset(DreamPlayer* p);
void setPlayerShadowOffset(DreamPlayer* p, int tOffset);

int isPlayerHelper(DreamPlayer* p);

void setPlayerIsFacingRight(DreamPlayer* p, int tIsFacingRight);
int getPlayerIsFacingRight(DreamPlayer* p);
void turnPlayerAround(DreamPlayer* p);

DreamPlayer* getPlayerOtherPlayer(DreamPlayer* p);

double getPlayerScaleX(DreamPlayer* p);
void setPlayerScaleX(DreamPlayer* p, double tScaleX);
double getPlayerScaleY(DreamPlayer* p);
void setPlayerScaleY(DreamPlayer* p, double tScaleY);

int getPlayerDoesScaleProjectiles(DreamPlayer* p);
void setPlayerDoesScaleProjectiles(DreamPlayer* p, int tDoesScaleProjectiles);

DreamPlayer* clonePlayerAsHelper(DreamPlayer* p);
void destroyPlayer(DreamPlayer* tPlayer);
int getPlayerID(DreamPlayer* p);
void setPlayerID(DreamPlayer* p, int tID);
void setPlayerHelperControl(DreamPlayer* p, int tCanControl);

DreamPlayer* createNewProjectileFromPlayer(DreamPlayer* p);
void removeProjectile(DreamPlayer* p);

int getPlayerControlTime(DreamPlayer* p);
int getPlayerRecoverTime(DreamPlayer* p);

void setPlayerTempScaleActive(DreamPlayer* p, Vector3D tScale);
void setPlayerDrawAngleActive(DreamPlayer* p);
void addPlayerDrawAngle(DreamPlayer* p, double tAngle);
void multiplyPlayerDrawAngle(DreamPlayer* p, double tFactor);
void setPlayerDrawAngleValue(DreamPlayer* p, double tAngle);

void bindPlayerToRoot(DreamPlayer* p, int tTime, int tFacing, Vector3D tOffset);
void bindPlayerToParent(DreamPlayer* p, int tTime, int tFacing, Vector3D tOffset);
void bindPlayerToTarget(DreamPlayer* p, int tTime, Vector3D tOffset, DreamPlayerBindPositionType tBindPositionType, int tID);
int isPlayerBound(DreamPlayer* p);

void bindPlayerTargetToPlayer(DreamPlayer* p, int tTime, Vector3D tOffset, int tID);
void addPlayerTargetLife(DreamPlayer* p, DreamPlayer* tLifeGivingPlayer, int tID, int tLife, int tCanKill, int tIsAbsolute);
void addPlayerTargetPower(DreamPlayer* p, int tID, int tPower);
void addPlayerTargetVelocityX(DreamPlayer* p, int tID, double tValue, int tCoordinateP);
void addPlayerTargetVelocityY(DreamPlayer* p, int tID, double tValue, int tCoordinateP);
void setPlayerTargetVelocityX(DreamPlayer* p, int tID, double tValue, int tCoordinateP);
void setPlayerTargetVelocityY(DreamPlayer* p, int tID, double tValue, int tCoordinateP);
void setPlayerTargetControl(DreamPlayer* p, int tID, int tControl);
void setPlayerTargetHitOver(DreamPlayer* p, int tID);
void setPlayerTargetFacing(DreamPlayer* p, int tID, int tFacing);
void changePlayerTargetState(DreamPlayer* p, int tID, int tNewState);

int doesPlayerIDExist(DreamPlayer* p, int tID);
DreamPlayer* getPlayerByIDOrNullIfNonexistant(DreamPlayer* p, int tID);

int getPlayerRoundsExisted(DreamPlayer* p);

int getPlayerPaletteNumber(DreamPlayer* p);

void setPlayerScreenBoundForTick(DreamPlayer* p, int tIsBoundToScreen, int tIsCameraFollowingX, int tIsCameraFollowingY);
void setPlayerScreenBoundForever(DreamPlayer* p, int tIsBoundToScreen);

void resetPlayerHitBy(DreamPlayer* p, int tSlot);
void resetPlayerNotHitBy(DreamPlayer* p, int tSlot);
void setPlayerNotHitByFlag1(DreamPlayer* p, int tSlot, const char* tFlag);
void addPlayerNotHitByFlag2(DreamPlayer* p, int tSlot, const char* tFlag);
void setPlayerNotHitByTime(DreamPlayer* p, int tSlot, int tTime);

int getDefaultPlayerSparkNumberIsInPlayerFile(DreamPlayer* p);
int getDefaultPlayerSparkNumber(DreamPlayer* p);
int getDefaultPlayerGuardSparkNumberIsInPlayerFile(DreamPlayer* p);
int getDefaultPlayerGuardSparkNumber(DreamPlayer* p);

int isPlayerProjectile(DreamPlayer* p);
int isPlayerHomeTeam(DreamPlayer* p);

void setPlayerDrawOffsetX(DreamPlayer* p, double tValue, int tCoordinateP);
void setPlayerDrawOffsetY(DreamPlayer* p, double tValue, int tCoordinateP);

void setPlayerOneFrameTransparency(DreamPlayer* p, BlendType tType, int tAlphaSource, int tAlphaDest);
void setPlayerWidthOneFrame(DreamPlayer* p, Vector3DI tEdgeWidth, Vector3DI tPlayerWidth);

void addPlayerDust(DreamPlayer* p, int tDustIndex, Position tPos, int tSpacing);
VictoryType getPlayerVictoryType(DreamPlayer* p);
int isPlayerAtFullLife(DreamPlayer* p);

int getPlayerVictoryQuoteIndex(DreamPlayer* p);
void setPlayerVictoryQuoteIndex(DreamPlayer* p, int tIndex);

void setPlayersToTrainingMode();
void setPlayersToRealFightMode();

int isPlayer(DreamPlayer* p);
int isValidPlayerOrProjectile(DreamPlayer* p);
int isGeneralPlayer(DreamPlayer* p);
int isPlayerTargetValid(DreamPlayer* p);

int isPlayerCollisionDebugActive();
void setPlayerCollisionDebug(int tIsActive);

void turnPlayerTowardsOtherPlayer(DreamPlayer* p);

int isPlayerInputAllowed(DreamPlayer* p);

int getPlayerTimeDilationUpdates(DreamPlayer* p);
double getPlayerSpeed(DreamPlayer* p);
void setPlayersSpeed(double tSpeed);
void setPlayerTargetsSuperDefenseMultiplier(DreamPlayer* tPlayer);

Vector3DI getIsCameraFollowingPlayer(DreamPlayer* p);

int parsePlayerSoundEffectChannel(int tChannel, DreamPlayer* tPlayer);
double getPlayerVolumeModifier(DreamPlayer* tPlayer);
double getPlayerMidiVolumeForPrism(DreamPlayer* tPlayer);
