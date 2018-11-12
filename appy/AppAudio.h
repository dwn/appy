////////////////////////////////////////////////////////////////////////////////
//Dan Nielsen
//Audio-generating routines.
//Reference:
//aaSpeechPhoneme() based on code by Franky with Smash Designs.
//aaBird() based on code by Andy Farnell in the UK.
////////////////////////////////////////////////////////////////////////////////
//Waveform functions return the number of samples remaining to be played
//If sound length is infinite, function returns -1
//
//Musical instruments:
//0 string pluck
//1 sinusoidal wave
//2 triangle wave
//3 sawtooth wave
//4 rectangular wave
//5 singing
//6 birds
#pragma once
#include "Noise.h"
#include <stddef.h>
#define AA_SIZE (APP_SOUNDBUFFERSIZE>>3)
#define AA_MUSICDURATIONMULTIPLIER 768
#define AA_SOUNDSAMPLERATE 22050
#define AA_SPEECHSAMPLERATE 22050

////////////////////////////////////////////////////////////////////////////////
//Common
////////////////////////////////////////////////////////////////////////////////

API
void aaReset(unsigned char track=-1);
API
void aaUpsampleAndMakeStereo(short* dataSrc,const unsigned int lengthBefore,
                             short* dataDst=NULL);

////////////////////////////////////////////////////////////////////////////////
//Pass filter
////////////////////////////////////////////////////////////////////////////////

API
void aaLowPass(short* data,int freqCut,int totalCounterBegin);
API
void aaHighPass(short* data,int freqCut,int totalCounterBegin);
API
void aaSetBandPass(int* b,int* c,int f,int bw);
API
void aaBandPassSample(int b,int c,int* y,int* z0,int* z1,int sample);

////////////////////////////////////////////////////////////////////////////////
//Basic oscillator
////////////////////////////////////////////////////////////////////////////////

API
int aaSawtooth(unsigned char track,short* data,int length,int freq);
API
int aaSine(unsigned char track,short* data,int length,int freq);
API
int aaSquare(unsigned char track,short* data,int length,int period);
API
int aaTriangle(unsigned char track,short* data,int length,int period);
API
int aaSilence(unsigned char track,short* data,int length);

////////////////////////////////////////////////////////////////////////////////
//Audio effect
////////////////////////////////////////////////////////////////////////////////

API
int aaBird(unsigned char track,short* data,int length);
API
int aaRain(unsigned char track,short* data,int length,Noise* noise);
API
int aaNoise(unsigned char track,short* data,int length,int freq,
            Noise* noise);
API
int aaPluck(unsigned char track,short* data,short* scratch,int length,
            int period);
//API
int aaSpeechPhoneme(unsigned char track,short* data,int f0,int f1,int b0,
                    int b1,int a0,int a1,int asp,int length,int freq);
//API
bool aaSpeech(unsigned char track,const char* text,short* data);

////////////////////////////////////////////////////////////////////////////////
//Music
////////////////////////////////////////////////////////////////////////////////

//API
void aaMusicNote(unsigned char track,short* data,short* scratch);
//API
bool aaMusic(unsigned char track,const char* text,short* data,short* scratch);

////////////////////////////////////////////////////////////////////////////////
//Multiplay - accepts a string containing both music and speech
////////////////////////////////////////////////////////////////////////////////

//API
bool aaChompPlayString(unsigned char track,char* text);
API
bool aaPlay(unsigned char track,char* text,short* data,short* scratch);
