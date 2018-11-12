////////////////////////////////////////////////////////////////////////////////
//Dan Nielsen
//Animated 3D skeleton.
////////////////////////////////////////////////////////////////////////////////
//Pelvis plane should align with ground surface when operating under a ground-
//perpendicular force and in a locomotive mode
//Gut bone defines center of trunk mass
#pragma once
#include "Define.h"

#define SKEL3D_NUMMODES 3
#define SKEL3D_NUMBONES 110
#define SKEL3D_STEPPOWER 3
#define SKEL3D_LENSCALE 4

struct API Skel3D {
  Skel3D(unsigned short* buffSet);
  ~Skel3D();
  void update();
  int getX() { return matOrigin[3]; }
  int getY() { return matOrigin[7]; }
  int getZ() { return matOrigin[11]; }
  void setPosition(int x,int y,int z);
  void setAngle(int psi,int phi,int tta);
  void setViewAngle(int psi,int phi,int tta);
  //void setMode(int modeSet) { mode=modeSet; }
private:
  void set(char stance);
  void setDeltaAngle(int* dAng,int ang,int angFin);
  unsigned short *buff;
  int tta[SKEL3D_NUMBONES];
  int phi[SKEL3D_NUMBONES];
  int psi[SKEL3D_NUMBONES];
  int matAccum[SKEL3D_NUMBONES][12];
  char parent[SKEL3D_NUMBONES];
  int length[SKEL3D_NUMBONES];
  int dTta[SKEL3D_NUMBONES];
  int dPhi[SKEL3D_NUMBONES];
  int dPsi[SKEL3D_NUMBONES];
  //int load[3*SKEL3D_NUMBONES] = {0};
  //unsigned char mass[SKEL3D_NUMBONES] = {0};
  //unsigned char center[SKEL3D_NUMBONES] = {0};
  //unsigned char flex[SKEL3D_NUMBONES] = {0};
  char currStep;
  int matOrigin[12];
  int matView[12];
  unsigned char animFrame;
  int evalParam[128];
  //unsigned int inputCode;
  //unsigned char mode;
};
