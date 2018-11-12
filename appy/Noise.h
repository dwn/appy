////////////////////////////////////////////////////////////////////////////////
//Dan Nielsen
//1D, 2D, and 3D dynamic Perlin-based noise functions.
//The dynamic approach taken here is novel.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Define.h"

struct API Noise {
  Noise();
  ~Noise();
  void update();
  int get1D(int x,int xiMask);
  int get2D(int x,int y,int xiMask,int yiMask);
  int get3D(int x,int y,int z,int xiMask,int yiMask,int ziMask);
  int getTurbulence2D(int x,int y,int xiMask,int yiMask,
                      unsigned char numOctaves);
  int getTurbulence3D(int x,int y,int z,int xiMask,int yiMask,int ziMask,
                      unsigned char numOctaves);
private:
  int quickDot1D(int g,int dx);
  int quickDot2D(int g,int dx,int dy);
  int quickDot3D(int g,int dx,int dy,int dz);
  unsigned char grad[256];
};
