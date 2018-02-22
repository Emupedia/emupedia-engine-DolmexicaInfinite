#include "mugenexplod.h"

#include <prism/geometry.h>
#include <prism/physics.h>
#include <prism/datastructures.h>
#include <prism/memoryhandler.h>
#include <prism/mugenanimationhandler.h>
#include <prism/physicshandler.h>
#include <prism/log.h>
#include <prism/system.h>

#include "fightui.h"
#include "stage.h"
#include "mugenstagehandler.h"

typedef struct {
	DreamPlayer* mPlayer;

	int mIsInFightDefFile;
	int mAnimationNumber;

	int mExternalID;
	Position mPosition;
	DreamExplodPositionType mPositionType;

	int mIsFlippedHorizontally;
	int mIsFlippedVertically;

	int mBindTime;

	Velocity mVelocity;
	Acceleration mAcceleration;

	Vector3DI mRandomOffset;
	int mRemoveTime;
	int mIsSuperMove;

	int mSuperMoveTime;
	int mPauseMoveTime;

	Vector3D mScale;
	int mSpritePriority;
	int mIsOnTop;

	int mIsUsingStageShadow;
	Vector3DI mShadow;

	int mUsesOwnPalette;
	int mIsRemovedOnGetHit;

	int mIgnoreHitPause;

	int mHasTransparencyType;
	DreamExplodTransparencyType mTransparencyType;

	int mIsFacingRight;
	int mPhysicsID;
	int mAnimationID;
	int mNow;

} Explod;


static struct {
	IntMap mExplods;
} gData;

static void loadExplods(void* tData) {
	(void)tData;
	gData.mExplods = new_int_map();
}

int addExplod(DreamPlayer* tPlayer)
{
	Explod* e = allocMemory(sizeof(Explod));
	e->mPlayer = tPlayer;

	return int_map_push_back_owned(&gData.mExplods, e);
}

void setExplodAnimation(int tID, int tIsInFightDefFile, int tAnimationNumber)
{
	Explod* e = int_map_get(&gData.mExplods, tID);
	e->mIsInFightDefFile = tIsInFightDefFile;
	e->mAnimationNumber = tAnimationNumber;
}

void setExplodID(int tID, int tExternalID)
{
	Explod* e = int_map_get(&gData.mExplods, tID);
	e->mExternalID = tExternalID;
}

void setExplodPosition(int tID, int tOffsetX, int tOffsetY)
{
	Explod* e = int_map_get(&gData.mExplods, tID);
	e->mPosition = makePosition(tOffsetX, tOffsetY, 0);
}

void setExplodPositionType(int tID, DreamExplodPositionType tType)
{
	Explod* e = int_map_get(&gData.mExplods, tID);
	e->mPositionType = tType;
}

void setExplodHorizontalFacing(int tID, int tFacing)
{
	Explod* e = int_map_get(&gData.mExplods, tID);
	if(e->mPosition.x >= 0) e->mIsFlippedHorizontally = tFacing == -1;
	else e->mIsFlippedHorizontally = tFacing == 1;
}

void setExplodVerticalFacing(int tID, int tFacing)
{
	Explod* e = int_map_get(&gData.mExplods, tID);
	e->mIsFlippedVertically = tFacing == -1;
}

void setExplodBindTime(int tID, int tBindTime)
{
	Explod* e = int_map_get(&gData.mExplods, tID);
	e->mBindTime = tBindTime;
}

void setExplodVelocity(int tID, double tX, double tY)
{
	Explod* e = int_map_get(&gData.mExplods, tID);
	e->mVelocity = makePosition(tX, tY, 0);
}

void setExplodAcceleration(int tID, double tX, double tY)
{
	Explod* e = int_map_get(&gData.mExplods, tID);
	e->mAcceleration = makePosition(tX, tY, 0);
}

void setExplodRandomOffset(int tID, int tX, int tY)
{
	Explod* e = int_map_get(&gData.mExplods, tID);
	e->mRandomOffset = makeVector3DI(tX, tY, 0);
}

void setExplodRemoveTime(int tID, int tRemoveTime)
{
	Explod* e = int_map_get(&gData.mExplods, tID);
	e->mRemoveTime = tRemoveTime;
}

void setExplodSuperMove(int tID, int tIsSuperMove)
{
	Explod* e = int_map_get(&gData.mExplods, tID);
	e->mIsSuperMove = tIsSuperMove;
}

void setExplodSuperMoveTime(int tID, int tSuperMoveTime)
{
	Explod* e = int_map_get(&gData.mExplods, tID);
	e->mSuperMoveTime = tSuperMoveTime;
}

void setExplodPauseMoveTime(int tID, int tPauseMoveTime)
{
	Explod* e = int_map_get(&gData.mExplods, tID);
	e->mPauseMoveTime = tPauseMoveTime;
}

void setExplodScale(int tID, double tX, double tY)
{
	Explod* e = int_map_get(&gData.mExplods, tID);
	e->mScale = makePosition(tX, tY, 0);
}

void setExplodSpritePriority(int tID, int tSpritePriority)
{
	Explod* e = int_map_get(&gData.mExplods, tID);
	e->mSpritePriority = tSpritePriority;
}

void setExplodOnTop(int tID, int tIsOnTop)
{
	Explod* e = int_map_get(&gData.mExplods, tID);
	e->mIsOnTop = tIsOnTop;
}

void setExplodShadow(int tID, int tR, int tG, int tB)
{
	Explod* e = int_map_get(&gData.mExplods, tID);
	e->mShadow = makeVector3DI(tR, tG, tB);
}

void setExplodOwnPalette(int tID, int tUsesOwnPalette)
{
	Explod* e = int_map_get(&gData.mExplods, tID);
	e->mUsesOwnPalette = tUsesOwnPalette;
}

void setExplodRemoveOnGetHit(int tID, int tIsRemovedOnGetHit)
{
	Explod* e = int_map_get(&gData.mExplods, tID);
	e->mIsRemovedOnGetHit = tIsRemovedOnGetHit;
}

void setExplodIgnoreHitPause(int tID, int tIgnoreHitPause)
{
	Explod* e = int_map_get(&gData.mExplods, tID);
	e->mIgnoreHitPause = tIgnoreHitPause;
}

void setExplodTransparencyType(int tID, int tHasTransparencyType, DreamExplodTransparencyType tTransparencyType)
{
	Explod* e = int_map_get(&gData.mExplods, tID);
	e->mHasTransparencyType = tHasTransparencyType;
	e->mTransparencyType = tTransparencyType;
}

static void explodAnimationFinishedCB(void* tCaller);

void finalizeExplod(int tID)
{
	Explod* e = int_map_get(&gData.mExplods, tID);

	MugenSpriteFile* sprites;
	MugenAnimation* animation;
	if (e->mIsInFightDefFile) {
		sprites = getDreamFightEffectSprites();
		animation = getDreamFightEffectAnimation(e->mAnimationNumber);
	}
	else {
		sprites = getPlayerSprites(e->mPlayer);
		animation = getPlayerAnimation(e->mPlayer, e->mAnimationNumber);
	}

	e->mIsFacingRight = getPlayerIsFacingRight(e->mPlayer);
	e->mPhysicsID = addToPhysicsHandler(getFinalPositionFromPositionType(e->mPositionType, e->mPosition, e->mPlayer));
	
	if (!e->mIsFacingRight) {
		e->mVelocity.x = -e->mVelocity.x;
		e->mAcceleration.x = -e->mAcceleration.x;
	}
	addAccelerationToHandledPhysics(e->mPhysicsID, e->mVelocity);

	Position p = getDreamStageCoordinateSystemOffset(getPlayerCoordinateP(e->mPlayer));
	p.z = 10 + 0.1 * e->mSpritePriority;
	e->mAnimationID = addMugenAnimation(animation, sprites, p);
	setMugenAnimationBasePosition(e->mAnimationID, getHandledPhysicsPositionReference(e->mPhysicsID));
	setMugenAnimationCameraPositionReference(e->mAnimationID, getDreamMugenStageHandlerCameraPositionReference());
	setMugenAnimationCallback(e->mAnimationID, explodAnimationFinishedCB, e);

	e->mNow = 0;
}

static void unloadExplod(Explod* e) {
	removeMugenAnimation(e->mAnimationID);
	removeFromPhysicsHandler(e->mPhysicsID);
}

int getExplodAmount(DreamPlayer * tPlayer)
{
	(void)tPlayer;
	return 0; // TODO
}

int getExplodAmountWithID(DreamPlayer * tPlayer, int tID)
{
	(void)tPlayer;
	(void)tID;
	return 0; // TODO
}

static void explodAnimationFinishedCB(void* tCaller) {
	Explod* e = tCaller;
	if (e->mRemoveTime != -2) return;

	e->mRemoveTime = 0; // TODO
}

static int updateExplodRemoveTime(Explod* e) {
	if (e->mRemoveTime < 0) return 0;

	e->mNow++;
	return e->mNow >= e->mRemoveTime;
}

static void updateExplodPhysics(Explod* e) {
	addAccelerationToHandledPhysics(e->mPhysicsID, e->mAcceleration);
}

static int updateSingleExplod(void* tCaller, void* tData) {
	(void)tCaller;
	Explod* e = tData;

	if (updateExplodRemoveTime(e)) {
		unloadExplod(e);
		return 1;
	}

	updateExplodPhysics(e);

	return 0;
}

static void updateExplods(void* tData) {
	int_map_remove_predicate(&gData.mExplods, updateSingleExplod, NULL);
}

ActorBlueprint DreamExplodHandler = {
	.mLoad = loadExplods,
	.mUpdate = updateExplods,
};


Position getFinalPositionFromPositionType(DreamExplodPositionType tPositionType, Position mOffset, DreamPlayer* tPlayer) {
	if (tPositionType == EXPLOD_POSITION_TYPE_RELATIVE_TO_P1) {
		DreamPlayer* target = tPlayer;
		Position p = getPlayerPosition(target, getPlayerCoordinateP(tPlayer));
		int isReversed = !getPlayerIsFacingRight(target);
		if (isReversed) mOffset.x *= -1;
		return vecAdd(p, mOffset);
	}
	else if (tPositionType == EXPLOD_POSITION_TYPE_RELATIVE_TO_P2) {
		DreamPlayer* target = getPlayerOtherPlayer(tPlayer);
		Position p = getPlayerPosition(target, getPlayerCoordinateP(tPlayer));
		int isReversed = !getPlayerIsFacingRight(target);
		if (isReversed) mOffset.x *= -1;
		return vecAdd(p, mOffset);
	}
	else if (tPositionType == EXPLOD_POSITION_TYPE_RELATIVE_TO_FRONT) {
		DreamPlayer* target = tPlayer;
		Position p = makePosition(getPlayerScreenEdgeInFrontX(target), getDreamStageTopOfScreenBasedOnPlayer(getPlayerCoordinateP(target)), 0);
		int isReversed = !getPlayerIsFacingRight(target);
		if (isReversed) mOffset.x *= -1;
		return vecAdd(p, mOffset);
	}
	else if (tPositionType == EXPLOD_POSITION_TYPE_RELATIVE_TO_BACK) {
		DreamPlayer* target = tPlayer;
		Position p = makePosition(getPlayerScreenEdgeInBackX(target), getDreamStageTopOfScreenBasedOnPlayer(getPlayerCoordinateP(target)), 0);
		int isReversed = getPlayerIsFacingRight(target);
		if (isReversed) mOffset.x *= -1;
		return vecAdd(p, mOffset);
	}
	else if (tPositionType == EXPLOD_POSITION_TYPE_RELATIVE_TO_LEFT) {
		Position p = makePosition(getDreamStageLeftOfScreenBasedOnPlayer(getPlayerCoordinateP(tPlayer)), getDreamStageTopOfScreenBasedOnPlayer(getPlayerCoordinateP(tPlayer)), 0);
		return vecAdd(p, mOffset);
	}
	else if (tPositionType == EXPLOD_POSITION_TYPE_RELATIVE_TO_LEFT) {
		Position p = makePosition(getDreamStageRightOfScreenBasedOnPlayer(getPlayerCoordinateP(tPlayer)), getDreamStageTopOfScreenBasedOnPlayer(getPlayerCoordinateP(tPlayer)), 0);
		return vecAdd(p, mOffset);
	}
	else if (tPositionType == EXPLOD_POSITION_TYPE_NONE) {
		Position p = makePosition(getDreamGameWidth(getPlayerCoordinateP(tPlayer)) / 2, 0, 0);
		return vecAdd(p, mOffset);
	}
	else {
		logError("Unrecognized position type.");
		logErrorInteger(tPositionType);
		abortSystem();
		return makePosition(0, 0, 0);
	}

}