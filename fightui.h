#pragma once

#include <prism/actorhandler.h>
#include <prism/geometry.h>

#include <prism/mugenspritefilereader.h>
#include <prism/mugenanimationreader.h>

#include "playerdefinition.h"

void playDreamHitSpark(Position tPosition, DreamPlayer* tPlayer, int tIsInPlayerFile, int tNumber, int tIsFacingRight, int tPositionCoordinateP);
void addDreamDustCloud(Position tPosition, int tIsFacingRight, int tCoordinateP);
void setDreamLifeBarPercentage(DreamPlayer* tPlayer, double tPercentage);
void setDreamPowerBarPercentage(DreamPlayer* tPlayer, double tPercentage, int tValue);
void enableDreamTimer();
void disableDreamTimer();
void resetDreamTimer();
void setTimerFinished();

MugenAnimation* getDreamFightEffectAnimation(int tNumber);
MugenSpriteFile* getDreamFightEffectSprites();
MugenSounds* getDreamCommonSounds();
int getDreamUICoordinateP();
double getDreamUIFightFXScale();

void playDreamRoundAnimation(int tRound, void(*tFunc)());
void playDreamFightAnimation(void(*tFunc)());
void playDreamKOAnimation(void(*tFunc)());
void playDreamDKOAnimation(void(*tFunc)());
void playDreamTOAnimation(void(*tFunc)());
void playDreamWinAnimation(char* tName, void(*tFunc)());
void playDreamDrawAnimation(void(*tFunc)());
void playDreamContinueAnimation(void(*tAnimationFinishedFunc)(), void(*tContinuePressedFunc)());
void setDreamTimeDisplayFinishedCB(void(*tTimeDisplayFinishedFunc)());

void setDreamBarInvisibleForOneFrame();

void setTimerFreezeFlag();
void setTimerInfinite();
void setTimerFinite();
int isTimerFinished();

void setEnvironmentColor(Vector3DI tColors, int tTime, int tIsUnderCharacters);
void setEnvironmentShake(int tDuration, double tFrequency, int tAmplitude, double tPhaseOffset);

void addNormalWinIcon(int tPlayer, int tIsPerfect);
void addSpecialWinIcon(int tPlayer, int tIsPerfect);
void addHyperWinIcon(int tPlayer, int tIsPerfect);
void addThrowWinIcon(int tPlayer, int tIsPerfect);
void addCheeseWinIcon(int tPlayer, int tIsPerfect);
void addTimeoverWinIcon(int tPlayer, int tIsPerfect);
void addSuicideWinIcon(int tPlayer, int tIsPerfect);
void addTeammateWinIcon(int tPlayer, int tIsPerfect);

void removeAllWinIcons();
void stopFightAndRoundAnimation();

void setUIFaces();
void setComboUIDisplay(int i, int tAmount);

int getSlowTime();
int getStartWaitTime();
int getOverWaitTime();
int getOverHitTime();
int getOverWinTime();
int getOverTime();

ActorBlueprint getDreamFightUIBP();