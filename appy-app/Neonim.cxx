////////////////////////////////////////////////////////////////////////////////
//Dan Nielsen
////////////////////////////////////////////////////////////////////////////////
#include "Neonim.h"
#include "Common.h"
#include "App.h"
#include "AppGraph.h"
#include "AppAudio.h"
#include "MoreMath.h"
#include <stdio.h>
#include <string.h>

const char *playSection[]={
  "e3.20.0eed",
  "e3.20.1eed",
  "e3.20.2eed",
  "e3.20.3eed",
  "e3.20.4eed",
  "e3.20.5eed",
  "e3.20.6eed",
  "e3.20.7eed"};
const char* playSectionSeq[]={
  "2234",
  "3445",
};
const char *shaderSource[][2]={
  {NULL,NULL},
  {NULL,NULL}};
const int posMark[3][8][2]={
  {{320,300},{362,283},{380,240},{362,197},
   {320,180},{278,197},{260,240},{278,283}},
  {{320,367},{410,331},{447,240},{410,149},
   {320,113},{230,149},{193,240},{230,331}},
  {{320,434},{457,378},{514,240},{457,102},
   {320,46},{183,102},{126,240},{183,378}}};
unsigned char n[5];
unsigned char moveMap[8]=
  {0,0,1,0,2,1,3,0};
unsigned char showCode[8]=
  {0x01,0x11,0x45,0x55,0xab,0xbb,0xef,0xff};
unsigned int counter,shaderIndex,shaderIndexOld;
unsigned short *buff,*offBuff,*zInvBuff;
Noise *noise;
bool mouseDown;
unsigned char playerIndex,compPlayer; //Player index values lie in [1..3]
const char *playerName[3]; //Names of players in order of play
char selectionOld;
unsigned char nOld;
bool selectRing;
unsigned char numMoves;
int x,y;
int posIntersect[3],posIntersectSave[3];
char winner;
void (*nimUpdate)();
void (*nimUpdate2)();
void (*nimUpdateOld)();

void nimPaused() {
  appSetPauseAudioOn();
  if (appGetMouseDown()||(appGetKey()&&appGetKey()!=27)) {
    nimUpdate=nimUpdateOld;
    nimLoopUpdate();
  }
}

void nimLoopUpdate() {
  appUpdate();
  appSetIdleFunction(nimUpdate);
}

void nimPretitleLoop() {
  nimLoopInit(false);
  agFullScreenWoodFill(buff,noise,2); //Background image
  appSetPrintLocation(15*80+31); appSetPrintString("Click to begin... ",1);
  if (appGetMouseDown()||(appGetKey()&&appGetKey()!=27)) {
    nimSegue(&nimTitleLoop);
  }
  nimLoopUpdate();
}

void nimInit() {
  // appSetPlaySections(playSection);
  // appSetPlaySectionSequence(0,playSectionSeq[0]);
  // appSetPlaySectionSequence(1,playSectionSeq[1]);
  shaderIndex=0;
  shaderIndexOld=0;
  buff=appGetFrameBuffer();
  offBuff=appGetOffFrameBuffer();
  zInvBuff=appGetZInvBuffer();
  noise=new Noise();
  mouseDown=false;
  agTextFont(AG_ROMANFONT);
  nimReset();
  nimUpdate=&nimPreTitleLoop;
  nimLoopUpdate();
}

void nimClose() {
  delete noise;
}

void nimReset() {
  appUISetHideOutput();
  agSetHueAngle(0);
  selectRing=false;
  playerIndex=1;
  winner=0;
  compPlayer=0;
  appSetCounterClear();
  numMoves=0;
  selectionOld=-1;
  commonRandomCounterSeed(0);
  //Initialize member arrays
  n[0]=0;
  n[1]=0;
  n[2]=5;
  n[3]=1;
  n[4]=1;
}

//Returns 0=unfinished, {1,2}=index of winning player, -1=cat won
char nimGameState(unsigned char v[]) {
  return 0;
}

int nimRateConfiguration(unsigned char v[],unsigned char index) {
  return 0;
}

void nimLoopInit(bool updateNoise) {
  if (updateNoise) noise->update();
  mouseDown=appGetMouseDown();
  counter=appGetCounter();
  x=appGetMouseCursorX(); y=appGetMouseCursorY();
}

void nimSegue(void (*callback)(void),int shaderIndexSet) {
  agFullScreenBlit(zInvBuff,offBuff);
  //agFullScreenStaticFill(offBuff,3);
  appSetCounterClear();
  nimUpdate=&nimCurtainLoop;
  shaderIndexOld=shaderIndex;
  shaderIndex=shaderIndexSet;
  nimUpdate2=callback;
}

void nimCurtainLoop() {
  nimLoopInit(false);
  agFullScreenWoodFill(buff,noise,2);
  if (counter>=16) {
    if (shaderIndex!=shaderIndexOld)
      appSetShader(shaderSource[shaderIndex][0],shaderSource[shaderIndex][1]);
    agFullScreenBlit(offBuff,zInvBuff);
    if (nimUpdate2==&nimTitleLoop) {
      appSetPlayBuffer(0,"Sc`lwy_pla_^_gamm");
    }
    else if (nimUpdate2==&nimMainLoop) {
      appSetPlayBuffer(0,"Slets_pla_nyonim");
    }
    nimUpdate=nimUpdate2;
  }
  nimLoopUpdate();
}

void nimTitleLoop() {
  nimLoopInit();
  agFullScreenPebbleFill(buff,noise);
  agTextSetPosition(220,240); agText(buff,"Neo*Nim ",0,0,0);
  agTextSetPosition(222,240); agText(buff,"Neo*Nim ",0,0,0);
  agTextSetPosition(220,242); agText(buff,"Neo*Nim ",0,0,0);
  agTextSetPosition(222,242); agText(buff,"Neo*Nim ",0,0,0);
  agTextSetPosition(221,241); agText(buff,"Neo*Nim ",255,255,255);
  if (appGetMouseDown()||appGetKey()) {
    if (appGetKey()==27) {
      nimUpdateOld=nimUpdate;
      nimUpdate=&nimPaused;
      //nimSegue(&nimPreTitleLoop);
    }
    else {
      appSetSoundEffect(APP_SOUNDEFFECTBEEP);
      agFullScreenWoodFill(offBuff,noise,3);
      nimSegue(&nimMainLoop);
    }
  }
  nimLoopUpdate();
}

void nimMainLoop() {
  nimLoopInit();
  nimDrawBoard();
  if (appGetKey()==27) { //Esc pressed, so nobody wins
    nimReset();
    nimSegue(&nimTitleLoop);
  }
  const int v[]={x-APP_HALFFRAMEWIDTH,y-APP_HALFFRAMEHEIGHT};
  unsigned int dist=mmNorm2Vec2(v);
  if (selectRing) {
    if (playerIndex==1) {
      agQuad(buff,0,0,
                  90<<16,0,
                  90<<16,160<<16,
                  0,160<<16,
                  200,200,200);
    }
    else { //playerIndex==2
      agQuad(buff,APP_FRAMEWIDTH<<16,0,
                  (APP_FRAMEWIDTH-90)<<16,0,
                  (APP_FRAMEWIDTH-90)<<16,160<<16,
                  APP_FRAMEWIDTH<<16,160<<16,
                  200,200,200);
    }
  }
  if (appGetMouseClick()) {
    if (selectRing) {
      if (dist<95) {
        n[playerIndex-1]=0;
        n[2]+=nOld;
        selectRing=false;
        playerIndex=nimNextPlayer(playerIndex);
      }
      else if (dist<160) {
        n[playerIndex-1]=0;
        n[3]+=nOld;
        selectRing=false;
        playerIndex=nimNextPlayer(playerIndex);
      }
      else if (dist<225) {
        n[playerIndex-1]=0;
        n[4]+=nOld;
        selectRing=false;
        playerIndex=nimNextPlayer(playerIndex);
      }
    }
    else { //selectRing==false
      if (playerIndex==1&&
          x<90&&y>APP_FRAMEHEIGHT-200) {
        nOld=n[0];
        selectRing=true;
      }
      else if (playerIndex==2&&
               x>APP_FRAMEWIDTH-90&&y>APP_FRAMEHEIGHT-200) {
        nOld=n[1];
        selectRing=true;
      }
      else if (dist<95&&n[2]) {
        nOld=n[2];
        nimTakeFromRing(2);
        n[2]=moveMap[n[2]];
        n[playerIndex-1]+=nOld-n[2];
        if (!(n[2]||n[3]||n[4])) {
          winner=playerIndex;
          printf("Player %d wins!\n",winner);
          appSetPlayBuffer(0,"Mc2.4.5dcdeedeg.2bC");
          nimUpdate=&nimEndSequenceLoop;
        }
        playerIndex=nimNextPlayer(playerIndex);
      }
      else if (dist>=95&&dist<160&&n[3]) {
        nOld=n[3];
        nimTakeFromRing(3);
        n[3]=moveMap[n[3]];
        n[playerIndex-1]+=nOld-n[3];
        if (!(n[2]||n[3]||n[4])) {
          winner=playerIndex;
          printf("Player %d wins!\n",winner);
          appSetPlayBuffer(0,"Mc2.4.5dcdeedeg.2bC");
          nimUpdate=&nimEndSequenceLoop;
        }
        playerIndex=nimNextPlayer(playerIndex);
      }
      else if (dist>=160&&dist<225&&n[4]) {
        nOld=n[4];
        nimTakeFromRing(4);
        n[4]=moveMap[n[4]];
        n[playerIndex-1]+=nOld-n[4];
        if (!(n[2]||n[3]||n[4])) {
          winner=playerIndex;
          printf("Player %d wins!\n",winner);
          appSetPlayBuffer(0,"Mc2.4.5dcdeedeg.2bC");
          nimUpdate=&nimEndSequenceLoop;
        }
        playerIndex=nimNextPlayer(playerIndex);
      }
    }
  }
  nimLoopUpdate();
}

void nimEndSequenceLoop() {
  nimLoopInit();
  agSetHueAngle(appGetCounter()<<4);
  agFullScreenBlit(buff,offBuff);
  appSetPrintLocation(15*80+33);
  appSetPrintString(winner==1? "PLAYER 1 WINS! " :
                               "PLAYER 2 WINS! ",1);
  if (appGetMouseDown()||(appGetKey()&&appGetKey()!=27)) {
    agFullScreenWoodFill(offBuff,noise,2); //Background image
    nimReset();
    nimSegue(&nimTitleLoop);
  }
  nimLoopUpdate();
}

//Value of ring: 4=innermost, 5=middle, 6=outermost
void nimTakeFromRing(char ringIndex) {
//  code=showCode[ringIndex];
//  for(unsigned int i=0;i<8;i++) {
  //TODO///////////////////////////
  //IIRC think this was supposed to show animation?
//  }
}

void nimCheckCatGame() {
}

void nimBlackMarker(int xPos,int yPos) {
  agCircle(buff,xPos,yPos,12<<16,0,0,0);
  agPaintFill(buff,xPos>>16,yPos>>16,0,0,0);
  agCircle(buff,xPos,yPos,14<<16,90,70,50);
  agCircle(buff,xPos,yPos,13<<16,90,70,50);
  agCircle(buff,xPos,yPos,12<<16,60,45,35);
  agCircle(buff,xPos,yPos,11<<16,30,23,17);
}

void nimWhiteMarker(int xPos,int yPos) {
  agSquare(buff,15,xPos>>16,yPos>>16,255,255,255);
  agCircle(buff,xPos,yPos,12<<16,135,135,135);
  agCircle(buff,xPos,yPos,11<<16,165,165,165);
  agCircle(buff,xPos,yPos,10<<16,195,195,195);
  agCircle(buff,xPos,yPos,9<<16,225,225,225);
}

void nimDrawBoard() {
  agFullScreenBlendBlit(buff,offBuff); //Background image
  for(unsigned char i=0;i<n[0];i++)
    nimWhiteMarker(((i>>2)+1)<<21,((i&3)+1)<<21);
  for(unsigned char i=0;i<n[1];i++)
    nimWhiteMarker(APP_FRAMEWIDTH-(((i>>2)+1)<<21),((i&3)+1)<<21);
  for(unsigned char j=0;j<3;j++) {
    unsigned char code=showCode[n[j+2]];
    nimBlackMarker(posMark[j][0][0]<<16,posMark[j][0][1]<<16);
    for(unsigned char i=1;i<8;i++)
      if (code&(1<<i))
        nimWhiteMarker(posMark[j][i][0]<<16,posMark[j][i][1]<<16);
  }
}

void mainUpdate() {
  unsigned short *buff=appGetFrameBuffer();
  for(unsigned int i=0;i<APP_NUMPIXELS;i++) {
    buff[i]=i%1000;
  }
  appUpdate();
}

int main() {
  appInit();
  nimInit();
  return 0;
}
