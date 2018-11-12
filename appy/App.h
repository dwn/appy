////////////////////////////////////////////////////////////////////////////////
//Dan Nielsen
//Basic application interface code.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Define.h"
#include <stddef.h>
#define appUISetCanvasSizeRotateOn(w,h) printf("[[setCanvasSizeRotateOn,%d,%d]]\n",w,h)
#define appUISetRotateOff() printf("[[setRotateOff]]\n")
#define appUISetHideOutput() printf("[[setHideOutput]]\n")
#define appUISetPauseAudioOn() printf("[[setPauseAudioOn]]\n") //Called by appSetPauseAudioOn()
#define appUISetPauseAudioOff() printf("[[setPauseAudioOff]]\n") //Called by appSetPauseAudioOff()
#define appUISetAudio(file) printf("[[setAudio,%s]]\n",file)
#define appUISetAccountPageOpen() printf("[[setAccountPageOpen]]\n")

//API
static inline bool appTextBoundary(int i) { return false; }
API
void appDisplay();
API
void appInit();
API
void appAudioInit();
API
void appClose();
API
void appReset();
API
void appUpdate();
API
void appSetFrameBufferClear();
API
void appSetZInvBufferClear();
API
void appSetZInvBufferMidLevel();
API
unsigned int appGetCounter();
API
unsigned int appGetInputCode();
API
unsigned char appGetKey();
API
unsigned short* appGetFrameBuffer();
API
unsigned short* appGetOffFrameBuffer();
API
unsigned short* appGetZInvBuffer();
API
short* appGetScratchBuffer();
API
char* appGetPlayBuffer(unsigned char i);
API
void appSetPlayBuffer(unsigned char i,const char* sz);
API
void appSetPlaySections(const char** arrPlaySection);
API
void appSetPlaySectionSequence(int track,const char* playSectionSeq);
API
void appSetPrintString(const char* text,unsigned char style=0,
                       bool (*boundaryFunc)(int)=appTextBoundary);
API
void appSetPrintInt(const int n);
API
void appSetPrintUint(const unsigned int n);
API
void appSetPrintDot(unsigned char r,unsigned char g,unsigned char b);
API
void appSetPrintLocation(int i);
API
bool appGetMouseClick();
API
bool appGetMouseHoldDown();
API
bool appGetMousePressDown();
API
bool appGetMousePressUp();
API
int appGetMouseCursorX();
API
int appGetMouseCursorY();
API
bool appGetMouseDown();
API
void appSetMouseCursorOn();
API
void appSetMouseCursorOff();
API
void appCursor(int x,int y,bool down,bool obeyHideRule);
API
void appSetDoubleBufferOn();
API
void appSetDoubleBufferOff();
API
unsigned int appGetTimer();
API
void appSetTimerOld();
API
unsigned int appGetTimerDifference();
API
unsigned int appGetTimerOld();
API
void appSetCounterClear();
API
void appSetCounterOld();
API
void appSetCounterAdd(int addend);
API
unsigned int appGetCounterOld();
API
unsigned int appGetCounterDifference();
API
void appSetDone();
API
void appSetIdleFunction(void (*func)(void));
API
void appSetShader(const char* fragShaderSource=NULL,
                  const char* vertShaderSource=NULL);
API
void appSetSoundEffect(unsigned int effectIndex);
API
void appSetPauseAudioOn();
API
void appSetPauseAudioOff();
