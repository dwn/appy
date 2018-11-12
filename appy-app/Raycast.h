////////////////////////////////////////////////////////////////////////////////
//Dan Nielsen
//Raycaster rendering engine.
////////////////////////////////////////////////////////////////////////////////
#pragma once
// #include "Common.h"
// #include "App.h"
// #include "AppGraph.h"
// #include "AppAudio.h"
// #include "MoreMath.h"
// #include <stdio.h>
// #include <string.h>

//#define RR_TEST //Use to inspect graphic quality

void rrInit();
void rrClose();
void rrReset();
void rrPaused();
void rrClearArtifact();
void rrLoopInit(bool updateNoise=true);
void rrLoopUpdate();
void rrSegue(void (*callback)(void),int shaderIndexSet=0);
void rrSegueHelper();
void rrPretitleLoop();
void rrTitleLoop();
void rrMainLoop();
void rrSetGraphicBuffer(unsigned short* buffSet);
void rrSetVelocityForward(int velFwdSet);
int rrGetAngle();
int rrGetX();
int rrGetY();
int rrGetH_AboveWater(int x,int y);
int rrGetHeight(bool noNegativeHeight=false);
void rrUpdate_AboveWater();
void rrBackground_AboveWater();
void rrScene_AboveWater();
void rrUpdate_UnderWater();
void rrBackground_UnderWater();
void rrScene_UnderWater();
void rrUpdate_Cave();
void rrUpdate_GrassyField();
