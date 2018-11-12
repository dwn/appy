////////////////////////////////////////////////////////////////////////////////
//Dan Nielsen
//Platformer engine.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Appy.h"

class App; extern App *app;

enum PlatformAnimationIndex { PLATFORM_LEFT,PLATFORM_RIGHT,PLATFORM_UP,
                              PLATFORM_DOWN };

typedef struct PlatformActor {
  int x,y,vx,vy,ay;
  int vxAbsMax;
  int vyTerminal;
  int vyJumpInitial;
  bool jumping;
  unsigned short *keySprite[16];
  int w[64],h[64];
  int l[64],r[64],u[64],d[64];
} PlatformActor;

class /*API*/ Platform {
public:
  Platform();
  ~Platform();
  void update();
/*  void setGraphicBuffer(unsigned short* buffSet) { buff=buffSet; }
private:
  void loadActorImages(PlatformActor* pa);
  PlatformActor ch;
  int accGrav;
  unsigned short *buff,*offBuff;
  unsigned short *map;*/
};
