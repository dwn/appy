////////////////////////////////////////////////////////////////////////////////
//Dan Nielsen
////////////////////////////////////////////////////////////////////////////////
#include "Skel3D.h"
#include "App.h"
#include "AppGraph.h"
#include "MoreMath.h"
//#include "Safety.h"
#include <string.h>

static const char _skel3DAnimSequence[]="012301230123012301230123ssjjssccss";

Skel3D::Skel3D(unsigned short* buffSet) : buff(buffSet) {
  int *ttaCurr=tta;
  int *phiCurr=phi;
  int *psiCurr=psi;

#define SKEL3D_ANG(a,b,c) \
  *ttaCurr++=(a)<<16; \
  *phiCurr++=(b)<<16; \
  *psiCurr++=(c)<<16;

  SKEL3D_ANG(   0,   0,   0); parent[  0]= -1; length[  0]=250<<SKEL3D_LENSCALE;
  SKEL3D_ANG( 256,   0,   0); parent[  1]= -1; length[  1]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG( 512,   0,   0); parent[  2]= -1; length[  2]=250<<SKEL3D_LENSCALE;
  SKEL3D_ANG( 768,   0,   0); parent[  3]= -1; length[  3]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG( 768,   0,   0); parent[  4]=  0; length[  4]=300<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[  5]=  1; length[  5]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG( 256,   0,   0); parent[  6]=  2; length[  6]=300<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[  7]=  3; length[  7]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[  8]=  4; length[  8]=300<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[  9]=  5; length[  9]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 10]=  6; length[ 10]=300<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 11]=  7; length[ 11]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG( 896,   0,   0); parent[ 12]=  8; length[ 12]=50<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 13]=  8; length[ 13]=50<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 14]=  9; length[ 14]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 15]= 10; length[ 15]=50<<SKEL3D_LENSCALE;
  SKEL3D_ANG( 128,   0,   0); parent[ 16]= 10; length[ 16]=50<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 17]= 11; length[ 17]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG( 128,   0,   0); parent[ 18]= 12; length[ 18]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG( 768,   0,   0); parent[ 19]= 13; length[ 19]=20<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 20]= 13; length[ 20]=40<<SKEL3D_LENSCALE;
  SKEL3D_ANG( 256,   0,   0); parent[ 21]= 13; length[ 21]=20<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 22]= 14; length[ 22]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG( 768,   0,   0); parent[ 23]= 15; length[ 23]=20<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 24]= 15; length[ 24]=40<<SKEL3D_LENSCALE;
  SKEL3D_ANG( 256,   0,   0); parent[ 25]= 15; length[ 25]=20<<SKEL3D_LENSCALE;
  SKEL3D_ANG( 896,   0,   0); parent[ 26]= 16; length[ 26]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 27]= 17; length[ 27]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 28]= 18; length[ 28]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG( 256,   0,   0); parent[ 29]= 19; length[ 29]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 30]= 20; length[ 30]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG( 768,   0,   0); parent[ 31]= 21; length[ 31]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 32]= 21; length[ 32]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 33]= 22; length[ 33]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 34]= 23; length[ 34]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG( 256,   0,   0); parent[ 35]= 23; length[ 35]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 36]= 24; length[ 36]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG( 768,   0,   0); parent[ 37]= 25; length[ 37]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 38]= 26; length[ 38]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 39]= 27; length[ 39]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 40]= 29; length[ 40]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 41]= 30; length[ 41]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 42]= 31; length[ 42]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG( 768,   0,   0); parent[ 43]= 32; length[ 43]=20<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 44]= 33; length[ 44]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG( 256,   0,   0); parent[ 45]= 34; length[ 45]=20<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 46]= 35; length[ 46]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 47]= 36; length[ 47]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 48]= 37; length[ 48]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 49]= 39; length[ 49]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 50]= 40; length[ 50]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 51]= 42; length[ 51]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 52]= 43; length[ 52]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 53]= 44; length[ 53]=150<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 54]= 45; length[ 54]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 55]= 46; length[ 55]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 56]= 48; length[ 56]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 57]= 49; length[ 57]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 58]= 52; length[ 58]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 59]= 54; length[ 59]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 60]= 57; length[ 60]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 61]= 60; length[ 61]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 62]= 61; length[ 62]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 63]= 62; length[ 63]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 64]= 63; length[ 64]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 65]= 64; length[ 65]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 66]= 65; length[ 66]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 67]= 66; length[ 67]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 68]= 67; length[ 68]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG( 768,   0,   0); parent[ 69]= 68; length[ 69]=130<<SKEL3D_LENSCALE;
  SKEL3D_ANG( 256,   0,   0); parent[ 70]= 68; length[ 70]=130<<SKEL3D_LENSCALE;
  SKEL3D_ANG( 256,   0,   0); parent[ 71]= 69; length[ 71]=400<<SKEL3D_LENSCALE;
  SKEL3D_ANG( 768,   0,   0); parent[ 72]= 70; length[ 72]=400<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 73]= 71; length[ 73]=300<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 74]= 72; length[ 74]=300<<SKEL3D_LENSCALE;
  SKEL3D_ANG( 256, 768,   0); parent[ 75]= 73; length[ 75]=50<<SKEL3D_LENSCALE;
  SKEL3D_ANG( 256, 256,   0); parent[ 76]= 73; length[ 76]=40<<SKEL3D_LENSCALE;
  SKEL3D_ANG( 256, 256,   0); parent[ 77]= 74; length[ 77]=40<<SKEL3D_LENSCALE;
  SKEL3D_ANG( 256, 768,   0); parent[ 78]= 74; length[ 78]=50<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 79]= 75; length[ 79]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 80]= 78; length[ 80]=30<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0, 768,   0); parent[ 81]= 79; length[ 81]=20<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 82]= 79; length[ 82]=20<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0, 256,   0); parent[ 83]= 79; length[ 83]=20<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0, 768,   0); parent[ 84]= 80; length[ 84]=20<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 85]= 80; length[ 85]=20<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0, 256,   0); parent[ 86]= 80; length[ 86]=20<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 87]= 81; length[ 87]=20<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0, 256,   0); parent[ 88]= 81; length[ 88]=20<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 89]= 82; length[ 89]=20<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0, 768,   0); parent[ 90]= 83; length[ 90]=20<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0, 256,   0); parent[ 91]= 84; length[ 91]=20<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 92]= 85; length[ 92]=20<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0, 768,   0); parent[ 93]= 86; length[ 93]=20<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 94]= 86; length[ 94]=20<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 95]= 87; length[ 95]=20<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0, 256,   0); parent[ 96]= 87; length[ 96]=20<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 97]= 88; length[ 97]=20<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 98]= 90; length[ 98]=20<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[ 99]= 91; length[ 99]=20<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[100]= 93; length[100]=20<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0, 768,   0); parent[101]= 94; length[101]=20<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[102]= 94; length[102]=20<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0, 256,   0); parent[103]= 95; length[103]=10<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[104]= 96; length[104]=20<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[105]=101; length[105]=20<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0, 768,   0); parent[106]=102; length[106]=10<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[107]=103; length[107]=10<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0,   0,   0); parent[108]=106; length[108]=10<<SKEL3D_LENSCALE;
  SKEL3D_ANG(   0, 768,   0); parent[109]=60; length[109]=100<<SKEL3D_LENSCALE;

#undef SKEL3D_ANG

  mmSetIdentityMat3x4(matOrigin);
//  mmSetRotMat3x4(matOrigin,0,512,0);
  matOrigin[11]=3200<<SKEL3D_LENSCALE;
  mmSetIdentityMat3x4(matView);
  for(int i=0; i<128; i++) evalParam[i]=0;
  animFrame=0;
//  mode=0;
}

Skel3D::~Skel3D() {
}

void Skel3D::update() {
//  inputCode=getInputCode();
//  if (inputCode&0x1b) {
//    if (inputCode&0x3) {
//      if (inputCode&0x2) {
//        mode=((mode+1)%SKEL3D_NUMMODES);
//      }
//      else {
//        mode=((mode+SKEL3D_NUMMODES-1)%SKEL3D_NUMMODES);
//      }
//    }
//    else {
//      if (inputCode&0x8) {
//        mode=((mode+SKEL3D_NUMMODES-1)%SKEL3D_NUMMODES);
//      }
//      else {
//        mode=((mode+1)%SKEL3D_NUMMODES);
//      }
//    }
//  }
  int matRel[12],matTmp[12],*matCurr,*matPar;
  int par;
  //Move forward
//  matOrigin[3]-=matOrigin[2]>>7;
//  matOrigin[7]-=matOrigin[6]>>7;
//  matOrigin[11]-=matOrigin[10]>>7;
  //Spin view
//  mmSetRotMat3x4(matView,200,10,getCounter());
  //Initialize matrices
  mmMat3x4FakeMul(matTmp,matView,matOrigin);
  mmClearMat3x4(matRel);
  evalParam[59]=matAccum[75][3];
  evalParam[60]=matAccum[75][7];
  evalParam[61]=matAccum[75][11];
  evalParam[62]=matAccum[78][3];
  evalParam[63]=matAccum[78][7];
  evalParam[64]=matAccum[78][11];
  for(int i=0;i<SKEL3D_NUMBONES;i++) {
    mmSetRotMat3x4(matRel,psi[i]>>16,phi[i]>>16,tta[i]>>16);
    matCurr=matAccum[i];
    if (parent[i]>=0) {
      par=parent[i];
      matPar=matAccum[par];
      mmMat3x4FakeMul(matCurr,matPar,matRel);
      matCurr[ 3]=(((matPar[0]>>2)*(length[par]>>6)) >> 8)+matPar[ 3];
      matCurr[ 7]=(((matPar[4]>>2)*(length[par]>>6)) >> 8)+matPar[ 7];
      matCurr[11]=(((matPar[8]>>2)*(length[par]>>6)) >> 8)+matPar[11];
    }
    else {
      mmMat3x4FakeMul(matCurr,matTmp,matRel);
    }
    tta[i]+=dTta[i]; phi[i]+=dPhi[i]; psi[i]+=dPsi[i];
  }
  if ((++currStep)>=(1<<SKEL3D_STEPPOWER)) {
    set(_skel3DAnimSequence[animFrame]);
    animFrame=(animFrame+1)%strlen(_skel3DAnimSequence);
    if (!animFrame) {
      evalParam[65]=(matAccum[75][3]+matAccum[78][3])>>1; //Task midfeet pos
      evalParam[66]=(matAccum[75][7]+matAccum[78][7])>>1;
      evalParam[67]=(matAccum[75][11]+matAccum[78][11])>>1;
    }
  }
  if (matAccum[0][11]>50000) {
    //Draw marker lines
    int pf[3];
    pf[0]=(((matAccum[17][1]>>2)*(10<<SKEL3D_LENSCALE)) >> 8)+matAccum[17][ 3];
    pf[1]=(((matAccum[17][5]>>2)*(10<<SKEL3D_LENSCALE)) >> 8)+matAccum[17][ 7];
    pf[2]=(((matAccum[17][9]>>2)*(10<<SKEL3D_LENSCALE)) >> 8)+matAccum[17][11];
    agLine(buff,
           (320+(320*matAccum[17][3])/matAccum[17][11])<<16,
           (240+(320*matAccum[17][7])/matAccum[17][11])<<16,
           (320+(320*pf[0])/pf[2])<<16,
           (240+(320*pf[1])/pf[2])<<16,255,0,0);
    pf[0]=(((matAccum[3][1]>>2)*(10<<SKEL3D_LENSCALE)) >> 8)+matAccum[3][ 3];
    pf[1]=(((matAccum[3][5]>>2)*(10<<SKEL3D_LENSCALE)) >> 8)+matAccum[3][ 7];
    pf[2]=(((matAccum[3][9]>>2)*(10<<SKEL3D_LENSCALE)) >> 8)+matAccum[3][11];
    agLine(buff,
           (320+(320*matAccum[3][3])/matAccum[3][11])<<16,
           (240+(320*matAccum[3][7])/matAccum[3][11])<<16,
           (320+(320*pf[0])/pf[2])<<16,
           (240+(320*pf[1])/pf[2])<<16,255,0,0);
    pf[0]=(((matAccum[70][0]>>2)*(10<<SKEL3D_LENSCALE)) >> 8)+matAccum[70][ 3];
    pf[1]=(((matAccum[70][4]>>2)*(10<<SKEL3D_LENSCALE)) >> 8)+matAccum[70][ 7];
    pf[2]=(((matAccum[70][8]>>2)*(10<<SKEL3D_LENSCALE)) >> 8)+matAccum[70][11];
    agLine(buff,
           (320+(320*matAccum[70][3])/matAccum[70][11])<<16,
           (240+(320*matAccum[70][7])/matAccum[70][11])<<16,
           (320+(320*pf[0])/pf[2])<<16,
           (240+(320*pf[1])/pf[2])<<16,255,0,0);
    pf[0]=(((matAccum[70][2]>>2)*(10<<SKEL3D_LENSCALE)) >> 8)+matAccum[70][ 3];
    pf[1]=(((matAccum[70][6]>>2)*(10<<SKEL3D_LENSCALE)) >> 8)+matAccum[70][ 7];
    pf[2]=(((matAccum[70][10]>>2)*(10<<SKEL3D_LENSCALE)) >> 8)+matAccum[70][11];
    agLine(buff,
           (320+(320*matAccum[70][3])/matAccum[70][11])<<16,
           (240+(320*matAccum[70][7])/matAccum[70][11])<<16,
           (320+(320*pf[0])/pf[2])<<16,
           (240+(320*pf[1])/pf[2])<<16,255,0,0);
    //Draw
    for(int i=0;i<SKEL3D_NUMBONES;i++) {
      int pf[3];
      pf[0]=(((matAccum[i][0]>>2)*(length[i]>>6)) >> 8)+matAccum[i][ 3];
      pf[1]=(((matAccum[i][4]>>2)*(length[i]>>6)) >> 8)+matAccum[i][ 7];
      pf[2]=(((matAccum[i][8]>>2)*(length[i]>>6)) >> 8)+matAccum[i][11];
      agLine(buff,
             (320+(320*matAccum[i][3])/matAccum[i][11])<<16,
             (240+(320*matAccum[i][7])/matAccum[i][11])<<16,
             (320+(320*pf[0])/pf[2])<<16,
             (240+(320*pf[1])/pf[2])<<16,0,0,255);
    }
  }
//  if (mode) {
//    //RULA
//    //Upper arm position
//    //Left
//    int xProj=((matAccum[4][0]>>2)*(matAccum[17][0]>>2)+
//               (matAccum[4][4]>>2)*(matAccum[17][4]>>2)+
//               (matAccum[4][8]>>2)*(matAccum[17][8]>>2))>>12;
//    int yProj=((matAccum[4][0]>>2)*(matAccum[17][1]>>2)+
//               (matAccum[4][4]>>2)*(matAccum[17][5]>>2)+
//               (matAccum[4][8]>>2)*(matAccum[17][9]>>2))>>12;
//    int zProj=((matAccum[4][0]>>2)*(matAccum[17][2]>>2)+
//               (matAccum[4][4]>>2)*(matAccum[17][6]>>2)+
//               (matAccum[4][8]>>2)*(matAccum[17][10]>>2))>>12;
//    int param=mmSqrRt((xProj>>2)*(xProj>>2)+(yProj>>2)*(yProj>>2));
//    int ang=mmAcos(param>>6);
//    if (xProj<0) ang=512-ang;
//    if (zProj>0) ang=(1024-ang)&0x3ff;
//    evalParam[0]=ang;
//    if (ang<=43||ang>=1024-43) evalParam[2]=1;
//    if ((ang>43&&ang<=128)||(ang<1024-43&&ang>=768)) evalParam[2]=2;
//    if (ang>128&&ang<=256) evalParam[2]=3;
//    if (ang>256&&ang<768) evalParam[2]=4;
//    //Right
//    xProj=((matAccum[6][0]>>2)*(matAccum[17][0]>>2)+
//           (matAccum[6][4]>>2)*(matAccum[17][4]>>2)+
//           (matAccum[6][8]>>2)*(matAccum[17][8]>>2))>>12;
//    yProj=((matAccum[6][0]>>2)*(matAccum[17][1]>>2)+
//           (matAccum[6][4]>>2)*(matAccum[17][5]>>2)+
//           (matAccum[6][8]>>2)*(matAccum[17][9]>>2))>>12;
//    zProj=((matAccum[6][0]>>2)*(matAccum[17][2]>>2)+
//           (matAccum[6][4]>>2)*(matAccum[17][6]>>2)+
//           (matAccum[6][8]>>2)*(matAccum[17][10]>>2))>>12;
//    param=mmSqrRt((xProj>>2)*(xProj>>2)+(yProj>>2)*(yProj>>2));
//    ang=mmAcos(param>>6);
//    if (xProj<0) ang=512-ang;
//    if (zProj>0) ang=(1024-ang)&0x3ff;
//    evalParam[1]=ang;
//    if (ang<=43||ang>=1024-43) evalParam[3]=1;
//    if ((ang>43&&ang<=128)||(ang<1024-43&&ang>=768)) evalParam[3]=2;
//    if (ang>128&&ang<=256) evalParam[3]=3;
//    if (ang>256&&ang<768) evalParam[3]=4;
//    //Shoulder raised
//    //Left
//    xProj=((matAccum[0][0]>>2)*(matAccum[3][0]>>2)+
//           (matAccum[0][4]>>2)*(matAccum[3][4]>>2)+
//           (matAccum[0][8]>>2)*(matAccum[3][8]>>2))>>12;
//    evalParam[4]=(xProj<-0x2800);
//    //Right
//    xProj=((matAccum[2][0]>>2)*(matAccum[3][0]>>2)+
//           (matAccum[2][4]>>2)*(matAccum[3][4]>>2)+
//           (matAccum[2][8]>>2)*(matAccum[3][8]>>2))>>12;
//    evalParam[5]=(xProj<-0x2800);
//    //Upper arm abducted
//    //Left
//    yProj=((matAccum[4][0]>>2)*(matAccum[17][1]>>2)+
//           (matAccum[4][4]>>2)*(matAccum[17][5]>>2)+
//           (matAccum[4][8]>>2)*(matAccum[17][9]>>2))>>12;
//    evalParam[6]=(yProj>0x9000);
//    //Right
//    yProj=((matAccum[6][0]>>2)*(matAccum[17][1]>>2)+
//           (matAccum[6][4]>>2)*(matAccum[17][5]>>2)+
//           (matAccum[6][8]>>2)*(matAccum[17][9]>>2))>>12;
//    evalParam[7]=(yProj<-0x9000);
//    //Upper arm support
//    //Left
//    evalParam[8]=0;
//    //Right
//    evalParam[9]=0;
//    //Lower arm position
//    //Left
//    xProj=((matAccum[4][0]>>2)*(matAccum[8][0]>>2)+
//           (matAccum[4][4]>>2)*(matAccum[8][4]>>2)+
//           (matAccum[4][8]>>2)*(matAccum[8][8]>>2))>>12;
//    ang=mmAcos(xProj>>8);
//    evalParam[10]=ang;
//    evalParam[12]=1+(ang>256);
//    //Right
//    xProj=((matAccum[6][0]>>2)*(matAccum[10][0]>>2)+
//           (matAccum[6][4]>>2)*(matAccum[10][4]>>2)+
//           (matAccum[6][8]>>2)*(matAccum[10][8]>>2))>>12;
//    ang=mmAcos(xProj>>8);
//    evalParam[11]=ang;
//    evalParam[13]=1+(ang>256);
//    //Lower arm crossing sagittal plane
//    //Left
//    param=(((matAccum[17][1]>>2)*((matAccum[12][3]-matAccum[17][3])>>8)+
//            (matAccum[17][5]>>2)*((matAccum[12][7]-matAccum[17][7])>>8)+
//            (matAccum[17][9]>>2)*((matAccum[12][11]-matAccum[17][11])>>8))>>6);
//    evalParam[14]=(param<0);
//    //Right
//    param=(((matAccum[17][1]>>2)*((matAccum[16][3]-matAccum[17][3])>>8)+
//            (matAccum[17][5]>>2)*((matAccum[16][7]-matAccum[17][7])>>8)+
//            (matAccum[17][9]>>2)*((matAccum[16][11]-matAccum[17][11])>>8))>>6);
//    evalParam[15]=(param>0);
//    //Lower arm out to side of body
//    //Left
//    yProj=((matAccum[8][0]>>2)*(matAccum[17][1]>>2)+
//           (matAccum[8][4]>>2)*(matAccum[17][5]>>2)+
//           (matAccum[8][8]>>2)*(matAccum[17][9]>>2))>>12;
//    evalParam[16]=(yProj>0x9000);
//    //Right
//    yProj=((matAccum[10][0]>>2)*(matAccum[17][1]>>2)+
//           (matAccum[10][4]>>2)*(matAccum[17][5]>>2)+
//           (matAccum[10][8]>>2)*(matAccum[17][9]>>2))>>12;
//    evalParam[17]=(yProj<-0x9000);
//    //Wrist position
//    //Left
//    xProj=((matAccum[13][0]>>2)*(matAccum[8][0]>>2)+
//           (matAccum[13][4]>>2)*(matAccum[8][4]>>2)+
//           (matAccum[13][8]>>2)*(matAccum[8][8]>>2))>>12;
//    yProj=((matAccum[13][0]>>2)*(matAccum[8][1]>>2)+
//           (matAccum[13][4]>>2)*(matAccum[8][5]>>2)+
//           (matAccum[13][8]>>2)*(matAccum[8][9]>>2))>>12;
//    zProj=((matAccum[13][0]>>2)*(matAccum[8][2]>>2)+
//           (matAccum[13][4]>>2)*(matAccum[8][6]>>2)+
//           (matAccum[13][8]>>2)*(matAccum[8][10]>>2))>>12;
//    param=mmSqrRt((xProj>>2)*(xProj>>2)+(yProj>>2)*(yProj>>2));
//    ang=mmAcos(param>>6);
//    evalParam[18]=ang;
//    evalParam[20]=1+(ang>20)+(ang>43);
//    //Right
//    xProj=((matAccum[15][0]>>2)*(matAccum[10][0]>>2)+
//           (matAccum[15][4]>>2)*(matAccum[10][4]>>2)+
//           (matAccum[15][8]>>2)*(matAccum[10][8]>>2))>>12;
//    yProj=((matAccum[15][0]>>2)*(matAccum[10][1]>>2)+
//           (matAccum[15][4]>>2)*(matAccum[10][5]>>2)+
//           (matAccum[15][8]>>2)*(matAccum[10][9]>>2))>>12;
//    zProj=((matAccum[15][0]>>2)*(matAccum[10][2]>>2)+
//           (matAccum[15][4]>>2)*(matAccum[10][6]>>2)+
//           (matAccum[15][8]>>2)*(matAccum[10][10]>>2))>>12;
//    param=mmSqrRt((xProj>>2)*(xProj>>2)+(yProj>>2)*(yProj>>2));
//    ang=mmAcos(param>>6);
//    evalParam[19]=ang;
//    evalParam[21]=1+(ang>20)+(ang>43);
//    //Wrist sidebend
//    //Left
//    xProj=((matAccum[13][0]>>2)*(matAccum[8][0]>>2)+
//           (matAccum[13][4]>>2)*(matAccum[8][4]>>2)+
//           (matAccum[13][8]>>2)*(matAccum[8][8]>>2))>>12;
//    yProj=((matAccum[13][0]>>2)*(matAccum[8][1]>>2)+
//           (matAccum[13][4]>>2)*(matAccum[8][5]>>2)+
//           (matAccum[13][8]>>2)*(matAccum[8][9]>>2))>>12;
//    zProj=((matAccum[13][0]>>2)*(matAccum[8][2]>>2)+
//           (matAccum[13][4]>>2)*(matAccum[8][6]>>2)+
//           (matAccum[13][8]>>2)*(matAccum[8][10]>>2))>>12;
//    param=mmSqrRt((xProj>>2)*(xProj>>2)+(zProj>>2)*(zProj>>2));
//    ang=mmAcos(param>>6);
//    evalParam[22]=ang;
//    evalParam[24]=(ang>30);
//    //Right
//    xProj=((matAccum[15][0]>>2)*(matAccum[10][0]>>2)+
//           (matAccum[15][4]>>2)*(matAccum[10][4]>>2)+
//           (matAccum[15][8]>>2)*(matAccum[10][8]>>2))>>12;
//    yProj=((matAccum[15][0]>>2)*(matAccum[10][1]>>2)+
//           (matAccum[15][4]>>2)*(matAccum[10][5]>>2)+
//           (matAccum[15][8]>>2)*(matAccum[10][9]>>2))>>12;
//    zProj=((matAccum[15][0]>>2)*(matAccum[10][2]>>2)+
//           (matAccum[15][4]>>2)*(matAccum[10][6]>>2)+
//           (matAccum[15][8]>>2)*(matAccum[10][10]>>2))>>12;
//    param=mmSqrRt((xProj>>2)*(xProj>>2)+(zProj>>2)*(zProj>>2));
//    ang=mmAcos(param>>6);
//    evalParam[23]=ang;
//    evalParam[25]=(ang>30);
//    //Wrist twist
//    ang=(psi[8]>>16);
//    evalParam[26]=ang;
//    evalParam[28]=1+(ang>300);
//    ang=(-psi[10]>>16);
//    evalParam[27]=ang;
//    evalParam[29]=1+(ang>300);
//    //Arm muscle use
//    evalParam[30]=0;
//    evalParam[31]=0;
//    //Arm load index
//    evalParam[32]=0;
//    evalParam[33]=0;
//    //Arm/wrist score
//    evalParam[34]=rulaArmWrist(evalParam[2],evalParam[4],evalParam[6],
//                                evalParam[8],evalParam[12],evalParam[14],
//                                evalParam[16],evalParam[20],evalParam[24],
//                                evalParam[28]);
//    evalParam[35]=rulaArmWrist(evalParam[3],evalParam[5],evalParam[7],
//                                evalParam[9],evalParam[13],evalParam[15],
//                                evalParam[17],evalParam[21],evalParam[25],
//                                evalParam[29]);
//    //Neck position
//    xProj=((matAccum[53][0]>>2)*(matAccum[17][0]>>2)+
//           (matAccum[53][4]>>2)*(matAccum[17][4]>>2)+
//           (matAccum[53][8]>>2)*(matAccum[17][8]>>2))>>12;
//    yProj=((matAccum[53][0]>>2)*(matAccum[17][1]>>2)+
//           (matAccum[53][4]>>2)*(matAccum[17][5]>>2)+
//           (matAccum[53][8]>>2)*(matAccum[17][9]>>2))>>12;
//    zProj=((matAccum[53][0]>>2)*(matAccum[17][2]>>2)+
//           (matAccum[53][4]>>2)*(matAccum[17][6]>>2)+
//           (matAccum[53][8]>>2)*(matAccum[17][10]>>2))>>12;
//    param=mmSqrRt((xProj>>2)*(xProj>>2)+(yProj>>2)*(yProj>>2));
//    ang=mmAcos(-param>>6);
//    if (xProj<0) ang=512-ang;
//    if (zProj>0) ang=(1024-ang)&0x3ff;
//    evalParam[36]=ang;
//    if (ang<=29||ang>=1024-14) evalParam[37]=1;
//    if (ang>29&&ang<=57) evalParam[37]=2;
//    if (ang>57&&ang<768) evalParam[37]=3;
//    if (ang<1024-14&&ang>=768) evalParam[37]=4;
//    //Neck twist
//    ang=(psi[1]>>16);
//    evalParam[38]=ang;
//    evalParam[39]=(ang>60);
//    //Neck sidebend
//    param=mmSqrRt((xProj>>2)*(xProj>>2)+(zProj>>2)*(zProj>>2));
//    ang=mmAcos(param>>6);
//    evalParam[40]=ang;
//    evalParam[41]=(ang>30);
//    //Trunk position
//    xProj=((matAccum[17][0]>>2)*(matAccum[70][0]>>2)+
//           (matAccum[17][4]>>2)*(matAccum[70][4]>>2)+
//           (matAccum[17][8]>>2)*(matAccum[70][8]>>2))>>12;
//    yProj=((matAccum[17][0]>>2)*(matAccum[70][1]>>2)+
//           (matAccum[17][4]>>2)*(matAccum[70][5]>>2)+
//           (matAccum[17][8]>>2)*(matAccum[70][9]>>2))>>12;
//    zProj=((matAccum[17][0]>>2)*(matAccum[70][2]>>2)+
//           (matAccum[17][4]>>2)*(matAccum[70][6]>>2)+
//           (matAccum[17][8]>>2)*(matAccum[70][10]>>2))>>12;
//    param=mmSqrRt((xProj>>2)*(xProj>>2)+(yProj>>2)*(yProj>>2));
//    ang=mmAcos(param>>6);
//    if (yProj<0) ang=512-ang;
//    if (zProj>0) ang=(1024-ang)&0x3ff;
//    ang=(ang+512)&0x3ff;
//    evalParam[42]=ang;
//    if (ang<=14||ang>=1024-29) evalParam[43]=1;
//    if (ang>14&&ang<=43) evalParam[43]=2;
//    if (ang>43&&ang<=171) evalParam[43]=3;
//    if (ang>171&&ang<1024-29) evalParam[43]=4;
//    //Trunk twist
//    ang=(psi[70]>>16);
//    evalParam[44]=ang;
//    evalParam[45]=(ang>60);
//    //Trunk sidebend
//    param=mmSqrRt((yProj>>2)*(yProj>>2)+(zProj>>2)*(zProj>>2));
//    ang=mmAcos(param>>6);
//    evalParam[46]=ang;
//    evalParam[47]=(ang>30);
//    //Leg unbalanced
//    evalParam[48]=0;
//    //Neck/trunk/leg score
//    evalParam[49]=rulaNeckTrunkLeg(evalParam[37],evalParam[39],evalParam[40],
//                                    evalParam[43],evalParam[45],evalParam[47],
//                                    evalParam[48]);
//    //Center muscle use
//    evalParam[50]=0;
//    //Center load index
//    evalParam[51]=0;
//    //Final score
//    evalParam[52]=rulaCombined(evalParam[34],evalParam[30],evalParam[32],
//                                evalParam[49],evalParam[50],evalParam[51]);
//    evalParam[53]=rulaCombined(evalParam[35],evalParam[31],evalParam[33],
//                                evalParam[49],evalParam[50],evalParam[51]);
//    //Foot
//    int dx=(matAccum[75][3]-matAccum[71][3])>>2;
//    int dy=(matAccum[75][7]-matAccum[71][7])>>2;
//    int dz=(matAccum[75][11]-matAccum[71][11])>>2;
//    xProj=dx*(matAccum[70][1]>>4)+dy*(matAccum[70][5]>>4)+dz*(matAccum[70][9]>>4);
//    dx=(matAccum[78][3]-matAccum[72][3])>>2;
//    dy=(matAccum[78][7]-matAccum[72][7])>>2;
//    dz=(matAccum[78][11]-matAccum[72][11])>>2;
//    evalParam[54]=evalParam[55];
//    evalParam[55]=(xProj<dx*(matAccum[70][1]>>4)+dy*(matAccum[70][5]>>4)+
//                   dz*(matAccum[70][9]>>4));
//    if (evalParam[54]&&!evalParam[55]) {
//      evalParam[57]=
//        (((matAccum[75][3]-matAccum[71][3])>>8)*(matAccum[70][2]>>2)+
//         ((matAccum[75][7]-matAccum[71][7])>>8)*(matAccum[70][6]>>2)+
//         ((matAccum[75][11]-matAccum[71][11])>>8)*(matAccum[70][10]>>2))>>6;
//      evalParam[58]=evalParam[57]-evalParam[56];
//      evalParam[56]=
//        (((matAccum[78][3]-matAccum[72][3])>>8)*(matAccum[70][2]>>2)+
//         ((matAccum[78][7]-matAccum[72][7])>>8)*(matAccum[70][6]>>2)+
//         ((matAccum[78][11]-matAccum[72][11])>>8)*(matAccum[70][10]>>2))>>6;
//    }
//    if (!evalParam[54]&&evalParam[55]) {
//      evalParam[57]=
//        (((matAccum[78][3]-matAccum[72][3])>>8)*(matAccum[70][2]>>2)+
//         ((matAccum[78][7]-matAccum[72][7])>>8)*(matAccum[70][6]>>2)+
//         ((matAccum[78][11]-matAccum[72][11])>>8)*(matAccum[70][10]>>2))>>6;
//      evalParam[58]=evalParam[57]-evalParam[56];
//      evalParam[56]=
//        (((matAccum[75][3]-matAccum[71][3])>>8)*(matAccum[70][2]>>2)+
//         ((matAccum[75][7]-matAccum[71][7])>>8)*(matAccum[70][6]>>2)+
//         ((matAccum[75][11]-matAccum[71][11])>>8)*(matAccum[70][10]>>2))>>6;
//    }
//    //Midfeet
//    evalParam[68]=(matAccum[75][3]+matAccum[78][3])>>1;
//    evalParam[69]=(matAccum[75][7]+matAccum[78][7])>>1;
//    evalParam[70]=(matAccum[75][11]+matAccum[78][11])>>1;
//    //Move agent
//    if (inputCode&0x40) {
//      if (evalParam[55]) {
//        matOrigin[3]-=(matAccum[75][3]-evalParam[59]);
//        matOrigin[7]-=(matAccum[75][7]-evalParam[60]);
//        matOrigin[11]-=(matAccum[75][11]-evalParam[61]);
//      }
//      else {
//        matOrigin[3]-=(matAccum[78][3]-evalParam[62]);
//        matOrigin[7]-=(matAccum[78][7]-evalParam[63]);
//        matOrigin[11]-=(matAccum[78][11]-evalParam[64]);
//      }
//    }
//    //Travel distance
//    dx=(evalParam[68]-evalParam[65])>>2;
//    dy=(evalParam[69]-evalParam[66])>>2;
//    dz=(evalParam[70]-evalParam[67])>>2;
//    evalParam[71]=mmSqrRt(dx*dx+dy*dy+dz*dz)<<2;
//    evalParam[72]=(381)<<SKEL3D_LENSCALE; //Horiz hand dist 0
//    evalParam[73]=evalParam[72]; //Horiz hand dist 1
//    evalParam[74]=evalParam[73]-evalParam[72];
//    evalParam[75]=0; //Horiz wrist load
//    evalParam[76]=(914)<<SKEL3D_LENSCALE; //Hand height 0
//    evalParam[77]=(1676)<<SKEL3D_LENSCALE; //Hand height 1
//    evalParam[78]=evalParam[77]-evalParam[76];
//    evalParam[79]=(98); //Vert wrist load
//    evalParam[80]=0; //Task
//    evalParam[81]=0; //Gender
//    evalParam[82]=(300); //Task period
//    evalParam[83]=(8*60*60); //Work duration
//    evalParam[84]=0; //Asymmetry angle 0
//    evalParam[85]=0; //Asymmetry angle 1
//    evalParam[86]=(2); //Coupling
//    //SNOOK
//    evalParam[87]=snookEval(evalParam[80],evalParam[81],evalParam[79],
//                             evalParam[76]>>SKEL3D_LENSCALE,
//                             evalParam[77]>>SKEL3D_LENSCALE,
//                             evalParam[72]>>SKEL3D_LENSCALE,evalParam[82],
//                             evalParam[71]>>SKEL3D_LENSCALE);
//    //NIOSH
//    int freq=60/evalParam[82];
//    int workDurIndex=(evalParam[83]<=3600? 0 : 1+(evalParam[83]>7200));
////    printf("%d* ",workDurIndex);
////    printf("origin: ");
//    evalParam[88]=nioshActionLimit(evalParam[72]<<(16-SKEL3D_LENSCALE),
//                                    evalParam[76]<<(16-SKEL3D_LENSCALE),
//                                    evalParam[78]<<(16-SKEL3D_LENSCALE),
//                                    freq,workDurIndex);
//    evalParam[89]=nioshActionLimit(evalParam[72]<<(16-SKEL3D_LENSCALE),
//                                    evalParam[77]<<(16-SKEL3D_LENSCALE),
//                                    evalParam[78]<<(16-SKEL3D_LENSCALE),
//                                    freq,workDurIndex);
//    evalParam[90]=3*evalParam[88];
//    evalParam[91]=3*evalParam[89];
//    int freqIndex=(evalParam[82]>=300? 0 :
//                                       evalParam[82]>=120? 1 :
//                                                           1+60/evalParam[82]);
//    evalParam[92]=nioshRecommendedWeightLimit(
//      evalParam[72]<<(16-SKEL3D_LENSCALE),evalParam[76]<<(16-SKEL3D_LENSCALE),
//      evalParam[78]<<(16-SKEL3D_LENSCALE),evalParam[84],
//      freqIndex,workDurIndex,evalParam[86]);
//    evalParam[93]=nioshRecommendedWeightLimit(
//      evalParam[72]<<(16-SKEL3D_LENSCALE),evalParam[77]<<(16-SKEL3D_LENSCALE),
//      evalParam[78]<<(16-SKEL3D_LENSCALE),evalParam[85],
//      freqIndex,workDurIndex,evalParam[86]);
//    evalParam[94]=(200*evalParam[79])/evalParam[92];
//    evalParam[95]=(200*evalParam[79])/evalParam[93];
//    evalParam[96]=35;
//    evalParam[97]=90;
//    evalParam[98]=0;
//    evalParam[99]=0;
//    evalParam[100]=0;
//    evalParam[101]=800;
//    COMPOSITE=200-((29*(evalParam[52]+evalParam[53]))+
//                       400-(evalParam[74]<<1)+
//                       (evalParam[81]+evalParam[82]))/6;
//    unsigned char textOffset=0;
//    if (mode==1) {
//      setPrintLocation(0*80);
//      setPrintString("0:1 (upper arm deg) = ");
//      setPrintInt((90*evalParam[0])>>8);
//      setPrintString(": ");
//      setPrintInt((90*evalParam[1])>>8);
//      setPrintLocation(1*80);
//      setPrintString("2:3 (upper arm pos) = ");
//      setPrintInt(evalParam[2]);
//      setPrintString(": ");
//      setPrintInt(evalParam[3]);
//      setPrintLocation(2*80);
//      setPrintString("4:5 (shoulder raised) = ");
//      setPrintInt(evalParam[4]);
//      setPrintString(": ");
//      setPrintInt(evalParam[5]);
//      setPrintLocation(3*80);
//      setPrintString("6:7 (upper arm abducted) = ");
//      setPrintInt(evalParam[6]);
//      setPrintString(": ");
//      setPrintInt(evalParam[7]);
//      setPrintLocation(4*80);
//      setPrintString("8:9 (upper arm support) = ");
//      setPrintInt(evalParam[8]);
//      setPrintString(": ");
//      setPrintInt(evalParam[9]);
//      setPrintLocation(5*80);
//      setPrintString("10:11 (low arm deg) = ");
//      setPrintInt((90*evalParam[10])>>8);
//      setPrintString(": ");
//      setPrintInt((90*evalParam[11])>>8);
//      setPrintLocation(6*80);
//      setPrintString("12:13 (low arm pos) = ");
//      setPrintInt(evalParam[12]);
//      setPrintString(": ");
//      setPrintInt(evalParam[13]);
//      setPrintLocation(7*80);
//      setPrintString("14:15 (low arm crosssagit) = ");
//      setPrintInt(evalParam[14]);
//      setPrintString(": ");
//      setPrintInt(evalParam[15]);
//      setPrintLocation(8*80);
//      setPrintString("16:17 (low arm sidejets) = ");
//      setPrintInt(evalParam[16]);
//      setPrintString(": ");
//      setPrintInt(evalParam[17]);
//      setPrintLocation(9*80);
//      setPrintString("18:19 (wrist deg) = ");
//      setPrintInt((90*evalParam[18])>>8);
//      setPrintString(": ");
//      setPrintInt((90*evalParam[19])>>8);
//      setPrintLocation(10*80);
//      setPrintString("20:21 (wrist pos) = ");
//      setPrintInt(evalParam[20]);
//      setPrintString(": ");
//      setPrintInt(evalParam[21]);
//      setPrintLocation(11*80);
//      setPrintString("22:23 (wrist sidebend deg) = ");
//      setPrintInt((90*evalParam[22])>>8);
//      setPrintString(": ");
//      setPrintInt((90*evalParam[23])>>8);
//      setPrintLocation(12*80);
//      setPrintString("24:25 (wrist sidebend) = ");
//      setPrintInt(evalParam[24]);
//      setPrintString(": ");
//      setPrintInt(evalParam[25]);
//      setPrintLocation(13*80);
//      setPrintString("26:27 (wrist twist deg) = ");
//      setPrintInt((90*evalParam[26])>>8);
//      setPrintString(": ");
//      setPrintInt((90*evalParam[27])>>8);
//      setPrintLocation(14*80);
//      setPrintString("28:29 (wrist twist) = ");
//      setPrintInt(evalParam[28]);
//      setPrintString(": ");
//      setPrintInt(evalParam[29]);
//      setPrintLocation(15*80);
//      setPrintString("30:31 (arm muscle use) = ");
//      setPrintInt(evalParam[30]);
//      setPrintString(": ");
//      setPrintInt(evalParam[31]);
//      setPrintLocation(16*80);
//      setPrintString("32:33 (arm load index) = ");
//      setPrintInt(evalParam[32]);
//      setPrintString(": ");
//      setPrintInt(evalParam[33]);
//      setPrintLocation(17*80);
//      setPrintString("34:35 (RULA arm wrist) = ");
//      setPrintInt(evalParam[34]);
//      setPrintString(": ");
//      setPrintInt(evalParam[35]);
//      setPrintLocation(18*80);
//      setPrintString("36 (neck deg) = ");
//      setPrintInt((90*evalParam[36])>>8);
//      setPrintLocation(19*80);
//      setPrintString("37 (neck pos) = ");
//      setPrintInt(evalParam[37]);
//      setPrintLocation(20*80);
//      setPrintString("38 (neck twist deg) = ");
//      setPrintInt((90*evalParam[38])>>8);
//      setPrintLocation(21*80);
//      setPrintString("39 (neck twist) = ");
//      setPrintInt(evalParam[39]);
//      setPrintLocation(22*80);
//      setPrintString("40 (neck sidebend deg) = ");
//      setPrintInt((90*evalParam[40])>>8);
//      setPrintLocation(23*80);
//      setPrintString("41 (neck sidebend) = ");
//      setPrintInt(evalParam[41]);
//      setPrintLocation(24*80);
//      setPrintString("42 (trunk deg) = ");
//      setPrintInt((90*evalParam[42])>>8);
//      setPrintLocation(25*80);
//      setPrintString("43 (trunk pos) = ");
//      setPrintInt(evalParam[43]);
//      setPrintLocation(26*80);
//      setPrintString("44 (trunk twist deg) = ");
//      setPrintInt((90*evalParam[44])>>8);
//      setPrintLocation(27*80);
//      setPrintString("45 (trunk twist) = ");
//      setPrintInt(evalParam[45]);
//      setPrintLocation(28*80);
//      setPrintString("46 (trunk sidebend deg) = ");
//      setPrintInt((90*evalParam[46])>>8);
//      setPrintLocation(29*80);
//      setPrintString("47 (trunk sidebend) = ");
//      setPrintInt(evalParam[47]);
//      textOffset=40;
//    }
//    setPrintLocation(0*80+textOffset);
//    setPrintString("48 (leg unbalanced) = ");
//    setPrintInt(evalParam[48]);
//    setPrintLocation(1*80+textOffset);
//    setPrintString("49 (RULA neck trunk leg) = ");
//    setPrintInt(evalParam[49]);
//    setPrintLocation(2*80+textOffset);
//    setPrintString("50 (center muscle use) = ");
//    setPrintInt(evalParam[50]);
//    setPrintLocation(3*80+textOffset);
//    setPrintString("51 (center load index) = ");
//    setPrintInt(evalParam[51]);
//    setPrintLocation(4*80+textOffset);
//    setPrintString("52:53 (RULA) = ");
//    setPrintInt(evalParam[52]);
//    setPrintString(": ");
//    setPrintInt(evalParam[53]);
//    setPrintLocation(5*80+textOffset);
//    setPrintString("54 (prev frame foot) = ");
//    setPrintInt(evalParam[54]);
//    setPrintLocation(6*80+textOffset);
//    setPrintString("55 (curr foot) = ");
//    setPrintInt(evalParam[55]);
//    setPrintLocation(7*80+textOffset);
//    setPrintString("56 (last ft proj mm) = ");
//    setPrintInt(evalParam[56]>>SKEL3D_LENSCALE);
//    setPrintLocation(8*80+textOffset);
//    setPrintString("57 (curr ft proj mm) = ");
//    setPrintInt(evalParam[57]>>SKEL3D_LENSCALE);
//    setPrintLocation(9*80+textOffset);
//    setPrintString("58 (footstep dist mm) = ");
//    setPrintInt(evalParam[58]>>SKEL3D_LENSCALE);
//    setPrintLocation(10*80+textOffset);
//    setPrintString("59:60:61 (prev frame lft ft pos mm) = ");
//    setPrintLocation(10+11*80+textOffset);
//    setPrintInt(evalParam[59]>>SKEL3D_LENSCALE);
//    setPrintString(": ");
//    setPrintInt(evalParam[60]>>SKEL3D_LENSCALE);
//    setPrintString(": ");
//    setPrintInt(evalParam[61]>>SKEL3D_LENSCALE);
//    setPrintLocation(12*80+textOffset);
//    setPrintString("62:63:64 (prev frame rgt ft pos mm) = ");
//    setPrintLocation(10+13*80+textOffset);
//    setPrintInt(evalParam[62]>>SKEL3D_LENSCALE);
//    setPrintString(": ");
//    setPrintInt(evalParam[63]>>SKEL3D_LENSCALE);
//    setPrintString(": ");
//    setPrintInt(evalParam[64]>>SKEL3D_LENSCALE);
//    setPrintLocation(14*80+textOffset);
//    setPrintString("65:66:67 (begin task midfeet mm) = ");
//    setPrintLocation(10+15*80+textOffset);
//    setPrintInt(evalParam[65]>>SKEL3D_LENSCALE);
//    setPrintString(": ");
//    setPrintInt(evalParam[66]>>SKEL3D_LENSCALE);
//    setPrintString(": ");
//    setPrintInt(evalParam[67]>>SKEL3D_LENSCALE);
//    setPrintLocation(16*80+textOffset);
//    setPrintString("68:69:70 (curr midfeet mm) = ");
//    setPrintLocation(10+17*80+textOffset);
//    setPrintInt(evalParam[68]>>SKEL3D_LENSCALE);
//    setPrintString(": ");
//    setPrintInt(evalParam[69]>>SKEL3D_LENSCALE);
//    setPrintString(": ");
//    setPrintInt(evalParam[70]>>SKEL3D_LENSCALE);
//    setPrintLocation(18*80+textOffset);
//    setPrintString("71 (travel dist mm) = ");
//    setPrintInt(evalParam[71]>>SKEL3D_LENSCALE);
//    setPrintLocation(19*80+textOffset);
//    setPrintString("72:73 (horiz hnd dist mm) = ");
//    setPrintInt(evalParam[72]>>SKEL3D_LENSCALE);
//    setPrintString(": ");
//    setPrintInt(evalParam[73]>>SKEL3D_LENSCALE);
//    setPrintLocation(20*80+textOffset);
//    setPrintString("74 (horiz hand displace mm) = ");
//    setPrintInt(evalParam[74]>>SKEL3D_LENSCALE);
//    setPrintLocation(21*80+textOffset);
//    setPrintString("75 (horiz wrist load N) = ");
//    setPrintInt(evalParam[75]);
//    setPrintLocation(22*80+textOffset);
//    setPrintString("76:77 (hand height mm) = ");
//    setPrintInt(evalParam[76]>>SKEL3D_LENSCALE);
//    setPrintString(": ");
//    setPrintInt(evalParam[77]>>SKEL3D_LENSCALE);
//    setPrintLocation(23*80+textOffset);
//    setPrintString("78 (vert hand displace mm) = ");
//    setPrintInt(evalParam[78]>>SKEL3D_LENSCALE);
//    setPrintLocation(24*80+textOffset);
//    setPrintString("79 (vert wrist load N) = ");
//    setPrintInt(evalParam[79]);
//    setPrintLocation(25*80+textOffset);
//    setPrintString("80 (task) = ");
//    setPrintInt(evalParam[80]);
//    setPrintLocation(26*80+textOffset);
//    setPrintString("81 (gender) = ");
//    setPrintInt(evalParam[81]);
//    setPrintLocation(27*80+textOffset);
//    setPrintString("82 (task period s) = ");
//    setPrintInt(evalParam[82]);
//    setPrintLocation(28*80+textOffset);
//    setPrintString("83 (work duration s) = ");
//    setPrintInt(evalParam[83]);
//    setPrintLocation(29*80+textOffset);
//    setPrintString("84:85 (asymmetry deg) = ");
//    setPrintInt((90*evalParam[84])>>8);
//    setPrintString(": ");
//    setPrintInt((90*evalParam[85])>>8);
//    if (mode==2) {
//      textOffset=40;
//      setPrintLocation(0*80+textOffset);
//      setPrintString("86 (coupling quality) = ");
//      setPrintInt(evalParam[86]);
//      setPrintLocation(1*80+textOffset);
//      setPrintString("87 (SNOOK prf) = ");
//      setPrintInt(evalParam[87]);
//      setPrintLocation(2*80+textOffset);
//      setPrintString("88:89 (NIOSH AL N) = ");
//      setPrintInt(evalParam[88]);
//      setPrintString(": ");
//      setPrintInt(evalParam[89]);
//      setPrintLocation(3*80+textOffset);
//      setPrintString("90:91 (NIOSH MPL N) = ");
//      setPrintInt(evalParam[90]);
//      setPrintString(": ");
//      setPrintInt(evalParam[91]);
//      setPrintLocation(4*80+textOffset);
//      setPrintString("92:93 (NIOSH RWL N) = ");
//      setPrintInt(evalParam[92]);
//      setPrintString(": ");
//      setPrintInt(evalParam[93]);
//      setPrintLocation(5*80+textOffset);
//      setPrintString("94:95 (NIOSH LI prf) = ");
//      setPrintInt(evalParam[94]);
//      setPrintString(": ");
//      setPrintInt(evalParam[95]);
//      setPrintLocation(6*80+textOffset);
//      setPrintString("96 (age yr) = ");
//      setPrintInt(evalParam[96]);
//      setPrintLocation(7*80+textOffset);
//      setPrintString("97 (heat stress index) = ");
//      setPrintInt(evalParam[97]);
//      setPrintLocation(8*80+textOffset);
//      setPrintString("98 (injury history index) = ");
//      setPrintInt(evalParam[98]);
//      setPrintLocation(9*80+textOffset);
//      setPrintString("99 (body fat prf) = ");
//      setPrintInt(evalParam[99]);
//      setPrintLocation(10*80+textOffset);
//      setPrintString("100 (body muscle prf) = ");
//      setPrintInt(evalParam[100]);
//      setPrintLocation(11*80+textOffset);
//      setPrintString("101 (body weight N) = ");
//      setPrintInt(evalParam[101]);
//    }
//  }
}

void Skel3D::set(char stance) {
  currStep=0;

  int *dTtaCurr=dTta; int *ttaCurr=tta;
  int *dPhiCurr=dPhi; int *phiCurr=phi;
  int *dPsiCurr=dPsi; int *psiCurr=psi;

  #define SKEL3D_ANG(a,b,c) \
    setDeltaAngle(dTtaCurr,*ttaCurr,(a)<<16); dTtaCurr++; ttaCurr++; \
    setDeltaAngle(dPhiCurr,*phiCurr,(b)<<16); dPhiCurr++; phiCurr++; \
    setDeltaAngle(dPsiCurr,*psiCurr,(c)<<16); dPsiCurr++; psiCurr++;

  switch(stance) {
    case 's': //Stand
      SKEL3D_ANG(   0,   0,   0); //0
      SKEL3D_ANG( 256,   0,   0); //1
      SKEL3D_ANG( 512,   0,   0); //2
      SKEL3D_ANG( 768,   0,   0); //3
      SKEL3D_ANG( 768,   0,   0); //4
      SKEL3D_ANG(   0,   0,   0); //5
      SKEL3D_ANG( 256,   0,   0); //6
      SKEL3D_ANG(   0,   0,   0); //7
      SKEL3D_ANG(   0,   0,   0); //8
      SKEL3D_ANG(   0,   0,   0); //9
      SKEL3D_ANG(   0,   0,   0); //10
      SKEL3D_ANG(   0,   0,   0); //11
      SKEL3D_ANG( 896,   0,   0); //12
      SKEL3D_ANG(   0,   0,   0); //13
      SKEL3D_ANG(   0,   0,   0); //14
      SKEL3D_ANG(   0,   0,   0); //15
      SKEL3D_ANG( 128,   0,   0); //16
      SKEL3D_ANG(   0,   0,   0); //17
      SKEL3D_ANG( 128,   0,   0); //18
      SKEL3D_ANG( 768,   0,   0); //19
      SKEL3D_ANG(   0,   0,   0); //20
      SKEL3D_ANG( 256,   0,   0); //21
      SKEL3D_ANG(   0,   0,   0); //22
      SKEL3D_ANG( 768,   0,   0); //23
      SKEL3D_ANG(   0,   0,   0); //24
      SKEL3D_ANG( 256,   0,   0); //25
      SKEL3D_ANG( 896,   0,   0); //26
      SKEL3D_ANG(   0,   0,   0); //27
      SKEL3D_ANG(   0,   0,   0); //28
      SKEL3D_ANG( 256,   0,   0); //29
      SKEL3D_ANG(   0,   0,   0); //30
      SKEL3D_ANG( 768,   0,   0); //31
      SKEL3D_ANG(   0,   0,   0); //32
      SKEL3D_ANG(   0,   0,   0); //33
      SKEL3D_ANG(   0,   0,   0); //34
      SKEL3D_ANG( 256,   0,   0); //35
      SKEL3D_ANG(   0,   0,   0); //36
      SKEL3D_ANG( 768,   0,   0); //37
      SKEL3D_ANG(   0,   0,   0); //38
      SKEL3D_ANG(   0,   0,   0); //39
      SKEL3D_ANG(   0,   0,   0); //40
      SKEL3D_ANG(   0,   0,   0); //41
      SKEL3D_ANG(   0,   0,   0); //42
      SKEL3D_ANG( 768,   0,   0); //43
      SKEL3D_ANG(   0,   0,   0); //44
      SKEL3D_ANG( 256,   0,   0); //45
      SKEL3D_ANG(   0,   0,   0); //46
      SKEL3D_ANG(   0,   0,   0); //47
      SKEL3D_ANG(   0,   0,   0); //48
      SKEL3D_ANG(   0,   0,   0); //49
      SKEL3D_ANG(   0,   0,   0); //50
      SKEL3D_ANG(   0,   0,   0); //51
      SKEL3D_ANG(   0,   0,   0); //52
      SKEL3D_ANG(   0,   0,   0); //53
      SKEL3D_ANG(   0,   0,   0); //54
      SKEL3D_ANG(   0,   0,   0); //55
      SKEL3D_ANG(   0,   0,   0); //56
      SKEL3D_ANG(   0,   0,   0); //57
      SKEL3D_ANG(   0,   0,   0); //58
      SKEL3D_ANG(   0,   0,   0); //59
      SKEL3D_ANG(   0,   0,   0); //60
      SKEL3D_ANG(   0,   0,   0); //61
      SKEL3D_ANG(   0,   0,   0); //62
      SKEL3D_ANG(   0,   0,   0); //63
      SKEL3D_ANG(   0,   0,   0); //64
      SKEL3D_ANG(   0,   0,   0); //65
      SKEL3D_ANG(   0,   0,   0); //66
      SKEL3D_ANG(   0,   0,   0); //67
      SKEL3D_ANG(   0,   0,   0); //68
      SKEL3D_ANG( 768,   0,   0); //69
      SKEL3D_ANG( 256,   0,   0); //70
      SKEL3D_ANG( 256,   0,   0); //71
      SKEL3D_ANG( 768,   0,   0); //72
      SKEL3D_ANG(   0,   0,   0); //73
      SKEL3D_ANG(   0,   0,   0); //74
      SKEL3D_ANG( 256, 768,   0); //75
      SKEL3D_ANG( 256, 256,   0); //76
      SKEL3D_ANG( 256, 256,   0); //77
      SKEL3D_ANG( 256, 768,   0); //78
      SKEL3D_ANG(   0,   0,   0); //79
      SKEL3D_ANG(   0,   0,   0); //80
      SKEL3D_ANG(   0, 768,   0); //81
      SKEL3D_ANG(   0,   0,   0); //82
      SKEL3D_ANG(   0, 256,   0); //83
      SKEL3D_ANG(   0, 768,   0); //84
      SKEL3D_ANG(   0,   0,   0); //85
      SKEL3D_ANG(   0, 256,   0); //86
      SKEL3D_ANG(   0,   0,   0); //87
      SKEL3D_ANG(   0, 256,   0); //88
      SKEL3D_ANG(   0,   0,   0); //89
      SKEL3D_ANG(   0, 768,   0); //90
      SKEL3D_ANG(   0, 256,   0); //91
      SKEL3D_ANG(   0,   0,   0); //92
      SKEL3D_ANG(   0, 768,   0); //93
      SKEL3D_ANG(   0,   0,   0); //94
      SKEL3D_ANG(   0,   0,   0); //95
      SKEL3D_ANG(   0, 256,   0); //96
      SKEL3D_ANG(   0,   0,   0); //97
      SKEL3D_ANG(   0,   0,   0); //98
      SKEL3D_ANG(   0,   0,   0); //99
      SKEL3D_ANG(   0,   0,   0); //100
      SKEL3D_ANG(   0, 768,   0); //101
      SKEL3D_ANG(   0,   0,   0); //102
      SKEL3D_ANG(   0, 256,   0); //103
      SKEL3D_ANG(   0,   0,   0); //104
      SKEL3D_ANG(   0,   0,   0); //105
      SKEL3D_ANG(   0, 768,   0); //106
      SKEL3D_ANG(   0,   0,   0); //107
      SKEL3D_ANG(   0,   0,   0); //108
      SKEL3D_ANG(   0, 768,   0); //109
    break; case 'c':
      SKEL3D_ANG(   0,   0,   0); //0
      SKEL3D_ANG( 256, 892,   0); //1
      SKEL3D_ANG( 512,   0,   0); //2
      SKEL3D_ANG( 768, 128,   0); //3
      SKEL3D_ANG( 768,   0,   0); //4
      SKEL3D_ANG(   0,   0,   0); //5
      SKEL3D_ANG( 256,   0,   0); //6
      SKEL3D_ANG(   0,   0,   0); //7
      SKEL3D_ANG(   0,   0,   0); //8
      SKEL3D_ANG(   0,   0,   0); //9
      SKEL3D_ANG(   0,   0,   0); //10
      SKEL3D_ANG(   0,   0,   0); //11
      SKEL3D_ANG( 896,   0,   0); //12
      SKEL3D_ANG(   0,   0,   0); //13
      SKEL3D_ANG(   0,   0,   0); //14
      SKEL3D_ANG(   0,   0,   0); //15
      SKEL3D_ANG( 128,   0,   0); //16
      SKEL3D_ANG(   0,   0,   0); //17
      SKEL3D_ANG( 128,   0,   0); //18
      SKEL3D_ANG( 768,   0,   0); //19
      SKEL3D_ANG(   0,   0,   0); //20
      SKEL3D_ANG( 256,   0,   0); //21
      SKEL3D_ANG(   0,   0,   0); //22
      SKEL3D_ANG( 768,   0,   0); //23
      SKEL3D_ANG(   0,   0,   0); //24
      SKEL3D_ANG( 256,   0,   0); //25
      SKEL3D_ANG( 896,   0,   0); //26
      SKEL3D_ANG(   0,   0,   0); //27
      SKEL3D_ANG(   0,   0,   0); //28
      SKEL3D_ANG( 256,   0,   0); //29
      SKEL3D_ANG(   0,   0,   0); //30
      SKEL3D_ANG( 768,   0,   0); //31
      SKEL3D_ANG(   0,   0,   0); //32
      SKEL3D_ANG(   0,   0,   0); //33
      SKEL3D_ANG(   0,   0,   0); //34
      SKEL3D_ANG( 256,   0,   0); //35
      SKEL3D_ANG(   0,   0,   0); //36
      SKEL3D_ANG( 768,   0,   0); //37
      SKEL3D_ANG(   0,   0,   0); //38
      SKEL3D_ANG(   0,   0,   0); //39
      SKEL3D_ANG(   0,   0,   0); //40
      SKEL3D_ANG(   0,   0,   0); //41
      SKEL3D_ANG(   0,   0,   0); //42
      SKEL3D_ANG( 768,   0,   0); //43
      SKEL3D_ANG(   0,   0,   0); //44
      SKEL3D_ANG( 256,   0,   0); //45
      SKEL3D_ANG(   0,   0,   0); //46
      SKEL3D_ANG(   0,   0,   0); //47
      SKEL3D_ANG(   0,   0,   0); //48
      SKEL3D_ANG(   0,   0,   0); //49
      SKEL3D_ANG(   0,   0,   0); //50
      SKEL3D_ANG(   0,   0,   0); //51
      SKEL3D_ANG(   0,   0,   0); //52
      SKEL3D_ANG(   0,   0,   0); //53
      SKEL3D_ANG(   0,   0,   0); //54
      SKEL3D_ANG(   0,   0,   0); //55
      SKEL3D_ANG(   0,   0,   0); //56
      SKEL3D_ANG(   0,   0,   0); //57
      SKEL3D_ANG(   0,   0,   0); //58
      SKEL3D_ANG(   0,   0,   0); //59
      SKEL3D_ANG(   0,   0,   0); //60
      SKEL3D_ANG(   0,   0,   0); //61
      SKEL3D_ANG(   0,   0,   0); //62
      SKEL3D_ANG(   0,   0,   0); //63
      SKEL3D_ANG(   0,   0,   0); //64
      SKEL3D_ANG(   0,   0,   0); //65
      SKEL3D_ANG(   0,   0,   0); //66
      SKEL3D_ANG(   0,   0,   0); //67
      SKEL3D_ANG(   0,   0,   0); //68
      SKEL3D_ANG( 768,   0,   0); //69
      SKEL3D_ANG( 256,   0,   0); //70
      SKEL3D_ANG( 256, 768,   0); //71
      SKEL3D_ANG( 768, 768,   0); //72
      SKEL3D_ANG(   0, 256,   0); //73
      SKEL3D_ANG(   0, 256,   0); //74
      SKEL3D_ANG( 256, 768,   0); //75
      SKEL3D_ANG( 256, 256,   0); //76
      SKEL3D_ANG( 256, 256,   0); //77
      SKEL3D_ANG( 256, 768,   0); //78
      SKEL3D_ANG(   0,   0,   0); //79
      SKEL3D_ANG(   0,   0,   0); //80
      SKEL3D_ANG(   0, 768,   0); //81
      SKEL3D_ANG(   0,   0,   0); //82
      SKEL3D_ANG(   0, 256,   0); //83
      SKEL3D_ANG(   0, 768,   0); //84
      SKEL3D_ANG(   0,   0,   0); //85
      SKEL3D_ANG(   0, 256,   0); //86
      SKEL3D_ANG(   0,   0,   0); //87
      SKEL3D_ANG(   0, 256,   0); //88
      SKEL3D_ANG(   0,   0,   0); //89
      SKEL3D_ANG(   0, 768,   0); //90
      SKEL3D_ANG(   0, 256,   0); //91
      SKEL3D_ANG(   0,   0,   0); //92
      SKEL3D_ANG(   0, 768,   0); //93
      SKEL3D_ANG(   0,   0,   0); //94
      SKEL3D_ANG(   0,   0,   0); //95
      SKEL3D_ANG(   0, 256,   0); //96
      SKEL3D_ANG(   0,   0,   0); //97
      SKEL3D_ANG(   0,   0,   0); //98
      SKEL3D_ANG(   0,   0,   0); //99
      SKEL3D_ANG(   0,   0,   0); //100
      SKEL3D_ANG(   0, 768,   0); //101
      SKEL3D_ANG(   0,   0,   0); //102
      SKEL3D_ANG(   0, 256,   0); //103
      SKEL3D_ANG(   0,   0,   0); //104
      SKEL3D_ANG(   0,   0,   0); //105
      SKEL3D_ANG(   0, 768,   0); //106
      SKEL3D_ANG(   0,   0,   0); //107
      SKEL3D_ANG(   0,   0,   0); //108
      SKEL3D_ANG(   0, 768,   0); //109
    break; case 't': //Torture
      SKEL3D_ANG(  30,   0,   0); //0
      SKEL3D_ANG( 256, 890, 256); //1
      SKEL3D_ANG( 462,   0,   0); //2
      SKEL3D_ANG( 768,   0,   0); //3
      SKEL3D_ANG( 512, 514,  60); //4
      SKEL3D_ANG(   0,   0,   0); //5
      SKEL3D_ANG(   0,   0,  40); //6
      SKEL3D_ANG(   0,   0,   0); //7
      SKEL3D_ANG( 600,   0, 270); //8
      SKEL3D_ANG(   0,   0,   0); //9
      SKEL3D_ANG( 356,   0,  40); //10
      SKEL3D_ANG(   0,   0,   0); //11
      SKEL3D_ANG( 896,   0,   0); //12
      SKEL3D_ANG( 900, 768,   0); //13
      SKEL3D_ANG(   0,   0,   0); //14
      SKEL3D_ANG(  60,   0,   0); //15
      SKEL3D_ANG( 128,   0,   0); //16
      SKEL3D_ANG(   0,   0,   0); //17
      SKEL3D_ANG( 128,   0,   0); //18
      SKEL3D_ANG( 768,   0,   0); //19
      SKEL3D_ANG(   0,   0,   0); //20
      SKEL3D_ANG( 256,   0,   0); //21
      SKEL3D_ANG(   0,   0,   0); //22
      SKEL3D_ANG( 768,   0,   0); //23
      SKEL3D_ANG(   0,   0,   0); //24
      SKEL3D_ANG( 256,   0,   0); //25
      SKEL3D_ANG( 896,   0,   0); //26
      SKEL3D_ANG(   0,   0,   0); //27
      SKEL3D_ANG(   0,   0,   0); //28
      SKEL3D_ANG( 256,   0,   0); //29
      SKEL3D_ANG(   0,   0,   0); //30
      SKEL3D_ANG( 768,   0,   0); //31
      SKEL3D_ANG(   0,   0,   0); //32
      SKEL3D_ANG(   0,   0,   0); //33
      SKEL3D_ANG(   0,   0,   0); //34
      SKEL3D_ANG( 256,   0,   0); //35
      SKEL3D_ANG(   0,   0,   0); //36
      SKEL3D_ANG( 768,   0,   0); //37
      SKEL3D_ANG(   0,   0,   0); //38
      SKEL3D_ANG(   0,   0,   0); //39
      SKEL3D_ANG(   0,   0,   0); //40
      SKEL3D_ANG(   0,   0,   0); //41
      SKEL3D_ANG(   0,   0,   0); //42
      SKEL3D_ANG( 768,   0,   0); //43
      SKEL3D_ANG(   0,   0,   0); //44
      SKEL3D_ANG( 256,   0,   0); //45
      SKEL3D_ANG(   0,   0,   0); //46
      SKEL3D_ANG(   0,   0,   0); //47
      SKEL3D_ANG(   0,   0,   0); //48
      SKEL3D_ANG(   0,   0,   0); //49
      SKEL3D_ANG(   0,   0,   0); //50
      SKEL3D_ANG(   0,   0,   0); //51
      SKEL3D_ANG(   0,   0,   0); //52
      SKEL3D_ANG(   0,   0,   0); //53
      SKEL3D_ANG(   0,   0,   0); //54
      SKEL3D_ANG(   0,   0,   0); //55
      SKEL3D_ANG(   0,   0,   0); //56
      SKEL3D_ANG(   0,   0,   0); //57
      SKEL3D_ANG(   0,   0,   0); //58
      SKEL3D_ANG(   0,   0,   0); //59
      SKEL3D_ANG(   0,   0,   0); //60
      SKEL3D_ANG(   0,   0,   0); //61
      SKEL3D_ANG(   0,   0,   0); //62
      SKEL3D_ANG(   0,   0,   0); //63
      SKEL3D_ANG(   0,   0,   0); //64
      SKEL3D_ANG(   0,   0,   0); //65
      SKEL3D_ANG(   0,   0,   0); //66
      SKEL3D_ANG(   0,   0,   0); //67
      SKEL3D_ANG(   0,   0,   0); //68
      SKEL3D_ANG( 868,   0, 904); //69
      SKEL3D_ANG( 356,   0, 120); //70
      SKEL3D_ANG( 256,   0,   0); //71
      SKEL3D_ANG( 768,   0,   0); //72
      SKEL3D_ANG(   0,   0,   0); //73
      SKEL3D_ANG(   0,   0,   0); //74
      SKEL3D_ANG( 256, 768,   0); //75
      SKEL3D_ANG( 256, 256,   0); //76
      SKEL3D_ANG( 256, 256,   0); //77
      SKEL3D_ANG( 256, 768,   0); //78
      SKEL3D_ANG(   0,   0,   0); //79
      SKEL3D_ANG(   0,   0,   0); //80
      SKEL3D_ANG(   0, 768,   0); //81
      SKEL3D_ANG(   0,   0,   0); //82
      SKEL3D_ANG(   0, 256,   0); //83
      SKEL3D_ANG(   0, 768,   0); //84
      SKEL3D_ANG(   0,   0,   0); //85
      SKEL3D_ANG(   0, 256,   0); //86
      SKEL3D_ANG(   0,   0,   0); //87
      SKEL3D_ANG(   0, 256,   0); //88
      SKEL3D_ANG(   0,   0,   0); //89
      SKEL3D_ANG(   0, 768,   0); //90
      SKEL3D_ANG(   0, 256,   0); //91
      SKEL3D_ANG(   0,   0,   0); //92
      SKEL3D_ANG(   0, 768,   0); //93
      SKEL3D_ANG(   0,   0,   0); //94
      SKEL3D_ANG(   0,   0,   0); //95
      SKEL3D_ANG(   0, 256,   0); //96
      SKEL3D_ANG(   0,   0,   0); //97
      SKEL3D_ANG(   0,   0,   0); //98
      SKEL3D_ANG(   0,   0,   0); //99
      SKEL3D_ANG(   0,   0,   0); //100
      SKEL3D_ANG(   0, 768,   0); //101
      SKEL3D_ANG(   0,   0,   0); //102
      SKEL3D_ANG(   0, 256,   0); //103
      SKEL3D_ANG(   0,   0,   0); //104
      SKEL3D_ANG(   0,   0,   0); //105
      SKEL3D_ANG(   0, 768,   0); //106
      SKEL3D_ANG(   0,   0,   0); //107
      SKEL3D_ANG(   0,   0,   0); //108
      SKEL3D_ANG(   0, 768,   0); //109
    break; case 'j': //Jump
      SKEL3D_ANG(   0,   0,   0); //0
      SKEL3D_ANG( 256,   0,   0); //1
      SKEL3D_ANG( 512,   0,   0); //2
      SKEL3D_ANG( 768,   0,   0); //3
      SKEL3D_ANG( 768,   0,   0); //4
      SKEL3D_ANG(   0,   0,   0); //5
      SKEL3D_ANG( 256,   0,   0); //6
      SKEL3D_ANG(   0,   0,   0); //7
      SKEL3D_ANG(   0,   0,   0); //8
      SKEL3D_ANG(   0,   0,   0); //9
      SKEL3D_ANG(   0,   0,   0); //10
      SKEL3D_ANG(   0,   0,   0); //11
      SKEL3D_ANG( 896,   0,   0); //12
      SKEL3D_ANG(   0,   0,   0); //13
      SKEL3D_ANG(   0,   0,   0); //14
      SKEL3D_ANG(   0,   0,   0); //15
      SKEL3D_ANG( 128,   0,   0); //16
      SKEL3D_ANG(   0,   0,   0); //17
      SKEL3D_ANG( 128,   0,   0); //18
      SKEL3D_ANG( 768,   0,   0); //19
      SKEL3D_ANG(   0,   0,   0); //20
      SKEL3D_ANG( 256,   0,   0); //21
      SKEL3D_ANG(   0,   0,   0); //22
      SKEL3D_ANG( 768,   0,   0); //23
      SKEL3D_ANG(   0,   0,   0); //24
      SKEL3D_ANG( 256,   0,   0); //25
      SKEL3D_ANG( 896,   0,   0); //26
      SKEL3D_ANG(   0,   0,   0); //27
      SKEL3D_ANG(   0,   0,   0); //28
      SKEL3D_ANG( 256,   0,   0); //29
      SKEL3D_ANG(   0,   0,   0); //30
      SKEL3D_ANG( 768,   0,   0); //31
      SKEL3D_ANG(   0,   0,   0); //32
      SKEL3D_ANG(   0,   0,   0); //33
      SKEL3D_ANG(   0,   0,   0); //34
      SKEL3D_ANG( 256,   0,   0); //35
      SKEL3D_ANG(   0,   0,   0); //36
      SKEL3D_ANG( 768,   0,   0); //37
      SKEL3D_ANG(   0,   0,   0); //38
      SKEL3D_ANG(   0,   0,   0); //39
      SKEL3D_ANG(   0,   0,   0); //40
      SKEL3D_ANG(   0,   0,   0); //41
      SKEL3D_ANG(   0,   0,   0); //42
      SKEL3D_ANG( 768,   0,   0); //43
      SKEL3D_ANG(   0,   0,   0); //44
      SKEL3D_ANG( 256,   0,   0); //45
      SKEL3D_ANG(   0,   0,   0); //46
      SKEL3D_ANG(   0,   0,   0); //47
      SKEL3D_ANG(   0,   0,   0); //48
      SKEL3D_ANG(   0,   0,   0); //49
      SKEL3D_ANG(   0,   0,   0); //50
      SKEL3D_ANG(   0,   0,   0); //51
      SKEL3D_ANG(   0,   0,   0); //52
      SKEL3D_ANG(   0,   0,   0); //53
      SKEL3D_ANG(   0,   0,   0); //54
      SKEL3D_ANG(   0,   0,   0); //55
      SKEL3D_ANG(   0,   0,   0); //56
      SKEL3D_ANG(   0,   0,   0); //57
      SKEL3D_ANG(   0,   0,   0); //58
      SKEL3D_ANG(   0,   0,   0); //59
      SKEL3D_ANG(   0,   0,   0); //60
      SKEL3D_ANG(   0,   0,   0); //61
      SKEL3D_ANG(   0,   0,   0); //62
      SKEL3D_ANG(   0,   0,   0); //63
      SKEL3D_ANG(   0,   0,   0); //64
      SKEL3D_ANG(   0,   0,   0); //65
      SKEL3D_ANG(   0,   0,   0); //66
      SKEL3D_ANG(   0,   0,   0); //67
      SKEL3D_ANG(   0,   0,   0); //68
      SKEL3D_ANG( 768,   0,   0); //69
      SKEL3D_ANG( 256,   0,   0); //70
      SKEL3D_ANG( 256,   0,   0); //71
      SKEL3D_ANG( 768,   0,   0); //72
      SKEL3D_ANG(   0,   0,   0); //73
      SKEL3D_ANG(   0,   0,   0); //74
      SKEL3D_ANG( 256, 768,   0); //75
      SKEL3D_ANG( 256, 256,   0); //76
      SKEL3D_ANG( 256, 256,   0); //77
      SKEL3D_ANG( 256, 768,   0); //78
      SKEL3D_ANG(   0,   0,   0); //79
      SKEL3D_ANG(   0,   0,   0); //80
      SKEL3D_ANG(   0, 768,   0); //81
      SKEL3D_ANG(   0,   0,   0); //82
      SKEL3D_ANG(   0, 256,   0); //83
      SKEL3D_ANG(   0, 768,   0); //84
      SKEL3D_ANG(   0,   0,   0); //85
      SKEL3D_ANG(   0, 256,   0); //86
      SKEL3D_ANG(   0,   0,   0); //87
      SKEL3D_ANG(   0, 256,   0); //88
      SKEL3D_ANG(   0,   0,   0); //89
      SKEL3D_ANG(   0, 768,   0); //90
      SKEL3D_ANG(   0, 256,   0); //91
      SKEL3D_ANG(   0,   0,   0); //92
      SKEL3D_ANG(   0, 768,   0); //93
      SKEL3D_ANG(   0,   0,   0); //94
      SKEL3D_ANG(   0,   0,   0); //95
      SKEL3D_ANG(   0, 256,   0); //96
      SKEL3D_ANG(   0,   0,   0); //97
      SKEL3D_ANG(   0,   0,   0); //98
      SKEL3D_ANG(   0,   0,   0); //99
      SKEL3D_ANG(   0,   0,   0); //100
      SKEL3D_ANG(   0, 768,   0); //101
      SKEL3D_ANG(   0,   0,   0); //102
      SKEL3D_ANG(   0, 256,   0); //103
      SKEL3D_ANG(   0,   0,   0); //104
      SKEL3D_ANG(   0,   0,   0); //105
      SKEL3D_ANG(   0, 768,   0); //106
      SKEL3D_ANG(   0,   0,   0); //107
      SKEL3D_ANG(   0,   0,   0); //108
      SKEL3D_ANG(   0, 768,   0); //109
    break; case '0': //Walk position 0
      SKEL3D_ANG(   0,   0,   0); //0
      SKEL3D_ANG( 256, 999,   0); //1
      SKEL3D_ANG( 512,   0,   0); //2
      SKEL3D_ANG( 768,   0,   0); //3
      SKEL3D_ANG( 768,  20,   0); //4
      SKEL3D_ANG(   0,   0,   0); //5
      SKEL3D_ANG( 256,  20,   0); //6
      SKEL3D_ANG(   0,   0,   0); //7
      SKEL3D_ANG(   0, 984, 256); //8
      SKEL3D_ANG(   0,   0,   0); //9
      SKEL3D_ANG(   0, 984, 768); //10
      SKEL3D_ANG(   0,   0,   0); //11
      SKEL3D_ANG( 896,   0,   0); //12
      SKEL3D_ANG(   0,   0,   0); //13
      SKEL3D_ANG(   0,   0,   0); //14
      SKEL3D_ANG(   0,   0,   0); //15
      SKEL3D_ANG( 128,   0,   0); //16
      SKEL3D_ANG(   0,   0,   0); //17
      SKEL3D_ANG( 128,   0,   0); //18
      SKEL3D_ANG( 768,   0,   0); //19
      SKEL3D_ANG(   0,   0,   0); //20
      SKEL3D_ANG( 256,   0,   0); //21
      SKEL3D_ANG(   0,   0,   0); //22
      SKEL3D_ANG( 768,   0,   0); //23
      SKEL3D_ANG(   0,   0,   0); //24
      SKEL3D_ANG( 256,   0,   0); //25
      SKEL3D_ANG( 896,   0,   0); //26
      SKEL3D_ANG(   0,   0,   0); //27
      SKEL3D_ANG(   0,   0,   0); //28
      SKEL3D_ANG( 256,   0,   0); //29
      SKEL3D_ANG(   0,   0,   0); //30
      SKEL3D_ANG( 768,   0,   0); //31
      SKEL3D_ANG(   0,   0,   0); //32
      SKEL3D_ANG(   0,   0,   0); //33
      SKEL3D_ANG(   0,   0,   0); //34
      SKEL3D_ANG( 256,   0,   0); //35
      SKEL3D_ANG(   0,   0,   0); //36
      SKEL3D_ANG( 768,   0,   0); //37
      SKEL3D_ANG(   0,   0,   0); //38
      SKEL3D_ANG(   0,   0,   0); //39
      SKEL3D_ANG(   0,   0,   0); //40
      SKEL3D_ANG(   0,   0,   0); //41
      SKEL3D_ANG(   0,   0,   0); //42
      SKEL3D_ANG( 768,   0,   0); //43
      SKEL3D_ANG(   0,   0,   0); //44
      SKEL3D_ANG( 256,   0,   0); //45
      SKEL3D_ANG(   0,   0,   0); //46
      SKEL3D_ANG(   0,   0,   0); //47
      SKEL3D_ANG(   0,   0,   0); //48
      SKEL3D_ANG(   0,   0,   0); //49
      SKEL3D_ANG(   0,   0,   0); //50
      SKEL3D_ANG(   0,   0,   0); //51
      SKEL3D_ANG(   0,   0,   0); //52
      SKEL3D_ANG(   0,   0,   0); //53
      SKEL3D_ANG(   0,   0,   0); //54
      SKEL3D_ANG(   0,   0,   0); //55
      SKEL3D_ANG(   0,   0,   0); //56
      SKEL3D_ANG(   0,   0,   0); //57
      SKEL3D_ANG(   0,   0,   0); //58
      SKEL3D_ANG(   0,   0,   0); //59
      SKEL3D_ANG(   0,   0,   0); //60
      SKEL3D_ANG(   0,   0,   0); //61
      SKEL3D_ANG(   0,   0,   0); //62
      SKEL3D_ANG(   0,   0,   0); //63
      SKEL3D_ANG(   0,   0,   0); //64
      SKEL3D_ANG(   0,   0,   0); //65
      SKEL3D_ANG(   0,   0,   0); //66
      SKEL3D_ANG(   0,   0,   0); //67
      SKEL3D_ANG(   0,   0,   0); //68
      SKEL3D_ANG( 768,   0,   0); //69
      SKEL3D_ANG( 256,   0,   0); //70
      SKEL3D_ANG( 256, 924,   0); //71
      SKEL3D_ANG( 768,   0,   0); //72
      SKEL3D_ANG(   0, 200,   0); //73
      SKEL3D_ANG(   0,   0,   0); //74
      SKEL3D_ANG( 256, 768,   0); //75
      SKEL3D_ANG( 256, 256,   0); //76
      SKEL3D_ANG( 256, 256,   0); //77
      SKEL3D_ANG( 256, 768,   0); //78
      SKEL3D_ANG(   0,   0,   0); //79
      SKEL3D_ANG(   0,   0,   0); //80
      SKEL3D_ANG(   0, 768,   0); //81
      SKEL3D_ANG(   0,   0,   0); //82
      SKEL3D_ANG(   0, 256,   0); //83
      SKEL3D_ANG(   0, 768,   0); //84
      SKEL3D_ANG(   0,   0,   0); //85
      SKEL3D_ANG(   0, 256,   0); //86
      SKEL3D_ANG(   0,   0,   0); //87
      SKEL3D_ANG(   0, 256,   0); //88
      SKEL3D_ANG(   0,   0,   0); //89
      SKEL3D_ANG(   0, 768,   0); //90
      SKEL3D_ANG(   0, 256,   0); //91
      SKEL3D_ANG(   0,   0,   0); //92
      SKEL3D_ANG(   0, 768,   0); //93
      SKEL3D_ANG(   0,   0,   0); //94
      SKEL3D_ANG(   0,   0,   0); //95
      SKEL3D_ANG(   0, 256,   0); //96
      SKEL3D_ANG(   0,   0,   0); //97
      SKEL3D_ANG(   0,   0,   0); //98
      SKEL3D_ANG(   0,   0,   0); //99
      SKEL3D_ANG(   0,   0,   0); //100
      SKEL3D_ANG(   0, 768,   0); //101
      SKEL3D_ANG(   0,   0,   0); //102
      SKEL3D_ANG(   0, 256,   0); //103
      SKEL3D_ANG(   0,   0,   0); //104
      SKEL3D_ANG(   0,   0,   0); //105
      SKEL3D_ANG(   0, 768,   0); //106
      SKEL3D_ANG(   0,   0,   0); //107
      SKEL3D_ANG(   0,   0,   0); //108
      SKEL3D_ANG(   0, 768,   0); //109
    break; case '1': //Walk position 1
      SKEL3D_ANG(   0,   0,   0); //0
      SKEL3D_ANG( 256, 999,   0); //1
      SKEL3D_ANG( 512,   0,   0); //2
      SKEL3D_ANG( 768,   0,   0); //3
      SKEL3D_ANG( 768,   0,   0); //4
      SKEL3D_ANG(   0,   0,   0); //5
      SKEL3D_ANG( 256,  30,   0); //6
      SKEL3D_ANG(   0,   0,   0); //7
      SKEL3D_ANG(   0, 974, 256); //8
      SKEL3D_ANG(   0,   0,   0); //9
      SKEL3D_ANG(   0,1009, 768); //10
      SKEL3D_ANG(   0,   0,   0); //11
      SKEL3D_ANG( 896,   0,   0); //12
      SKEL3D_ANG(   0,   0,   0); //13
      SKEL3D_ANG(   0,   0,   0); //14
      SKEL3D_ANG(   0,   0,   0); //15
      SKEL3D_ANG( 128,   0,   0); //16
      SKEL3D_ANG(   0,   0,   0); //17
      SKEL3D_ANG( 128,   0,   0); //18
      SKEL3D_ANG( 768,   0,   0); //19
      SKEL3D_ANG(   0,   0,   0); //20
      SKEL3D_ANG( 256,   0,   0); //21
      SKEL3D_ANG(   0,   0,   0); //22
      SKEL3D_ANG( 768,   0,   0); //23
      SKEL3D_ANG(   0,   0,   0); //24
      SKEL3D_ANG( 256,   0,   0); //25
      SKEL3D_ANG( 896,   0,   0); //26
      SKEL3D_ANG(   0,   0,   0); //27
      SKEL3D_ANG(   0,   0,   0); //28
      SKEL3D_ANG( 256,   0,   0); //29
      SKEL3D_ANG(   0,   0,   0); //30
      SKEL3D_ANG( 768,   0,   0); //31
      SKEL3D_ANG(   0,   0,   0); //32
      SKEL3D_ANG(   0,   0,   0); //33
      SKEL3D_ANG(   0,   0,   0); //34
      SKEL3D_ANG( 256,   0,   0); //35
      SKEL3D_ANG(   0,   0,   0); //36
      SKEL3D_ANG( 768,   0,   0); //37
      SKEL3D_ANG(   0,   0,   0); //38
      SKEL3D_ANG(   0,   0,   0); //39
      SKEL3D_ANG(   0,   0,   0); //40
      SKEL3D_ANG(   0,   0,   0); //41
      SKEL3D_ANG(   0,   0,   0); //42
      SKEL3D_ANG( 768,   0,   0); //43
      SKEL3D_ANG(   0,   0,   0); //44
      SKEL3D_ANG( 256,   0,   0); //45
      SKEL3D_ANG(   0,   0,   0); //46
      SKEL3D_ANG(   0,   0,   0); //47
      SKEL3D_ANG(   0,   0,   0); //48
      SKEL3D_ANG(   0,   0,   0); //49
      SKEL3D_ANG(   0,   0,   0); //50
      SKEL3D_ANG(   0,   0,   0); //51
      SKEL3D_ANG(   0,   0,   0); //52
      SKEL3D_ANG(   0,   0,   0); //53
      SKEL3D_ANG(   0,   0,   0); //54
      SKEL3D_ANG(   0,   0,   0); //55
      SKEL3D_ANG(   0,   0,   0); //56
      SKEL3D_ANG(   0,   0,   0); //57
      SKEL3D_ANG(   0,   0,   0); //58
      SKEL3D_ANG(   0,   0,   0); //59
      SKEL3D_ANG(   0,   0,   0); //60
      SKEL3D_ANG(   0,   0,   0); //61
      SKEL3D_ANG(   0,   0,   0); //62
      SKEL3D_ANG(   0,   0,   0); //63
      SKEL3D_ANG(   0,   0,   0); //64
      SKEL3D_ANG(   0,   0,   0); //65
      SKEL3D_ANG(   0,   0,   0); //66
      SKEL3D_ANG(   0,   0,   0); //67
      SKEL3D_ANG(   0,   0,   0); //68
      SKEL3D_ANG( 768,   0,   0); //69
      SKEL3D_ANG( 256,   0,   0); //70
      SKEL3D_ANG( 256, 974,   0); //71
      SKEL3D_ANG( 768,   0,   0); //72
      SKEL3D_ANG(   0,   0,   0); //73
      SKEL3D_ANG(   0, 100,   0); //74
      SKEL3D_ANG( 256, 768,   0); //75
      SKEL3D_ANG( 256, 256,   0); //76
      SKEL3D_ANG( 256, 256,   0); //77
      SKEL3D_ANG( 256, 768,   0); //78
      SKEL3D_ANG(   0,   0,   0); //79
      SKEL3D_ANG(   0,   0,   0); //80
      SKEL3D_ANG(   0, 768,   0); //81
      SKEL3D_ANG(   0,   0,   0); //82
      SKEL3D_ANG(   0, 256,   0); //83
      SKEL3D_ANG(   0, 768,   0); //84
      SKEL3D_ANG(   0,   0,   0); //85
      SKEL3D_ANG(   0, 256,   0); //86
      SKEL3D_ANG(   0,   0,   0); //87
      SKEL3D_ANG(   0, 256,   0); //88
      SKEL3D_ANG(   0,   0,   0); //89
      SKEL3D_ANG(   0, 768,   0); //90
      SKEL3D_ANG(   0, 256,   0); //91
      SKEL3D_ANG(   0,   0,   0); //92
      SKEL3D_ANG(   0, 768,   0); //93
      SKEL3D_ANG(   0,   0,   0); //94
      SKEL3D_ANG(   0,   0,   0); //95
      SKEL3D_ANG(   0, 256,   0); //96
      SKEL3D_ANG(   0,   0,   0); //97
      SKEL3D_ANG(   0,   0,   0); //98
      SKEL3D_ANG(   0,   0,   0); //99
      SKEL3D_ANG(   0,   0,   0); //100
      SKEL3D_ANG(   0, 768,   0); //101
      SKEL3D_ANG(   0,   0,   0); //102
      SKEL3D_ANG(   0, 256,   0); //103
      SKEL3D_ANG(   0,   0,   0); //104
      SKEL3D_ANG(   0,   0,   0); //105
      SKEL3D_ANG(   0, 768,   0); //106
      SKEL3D_ANG(   0,   0,   0); //107
      SKEL3D_ANG(   0,   0,   0); //108
      SKEL3D_ANG(   0, 768,   0); //109
    break; case '2': //Walk position 2
      SKEL3D_ANG(   0,   0,   0); //0
      SKEL3D_ANG( 256, 999,   0); //1
      SKEL3D_ANG( 512,   0,   0); //2
      SKEL3D_ANG( 768,   0,   0); //3
      SKEL3D_ANG( 768,  20,   0); //4
      SKEL3D_ANG(   0,   0,   0); //5
      SKEL3D_ANG( 256,  20,   0); //6
      SKEL3D_ANG(   0,   0,   0); //7
      SKEL3D_ANG(   0, 984, 256); //8
      SKEL3D_ANG(   0,   0,   0); //9
      SKEL3D_ANG(   0, 984, 768); //10
      SKEL3D_ANG(   0,   0,   0); //11
      SKEL3D_ANG( 896,   0,   0); //12
      SKEL3D_ANG(   0,   0,   0); //13
      SKEL3D_ANG(   0,   0,   0); //14
      SKEL3D_ANG(   0,   0,   0); //15
      SKEL3D_ANG( 128,   0,   0); //16
      SKEL3D_ANG(   0,   0,   0); //17
      SKEL3D_ANG( 128,   0,   0); //18
      SKEL3D_ANG( 768,   0,   0); //19
      SKEL3D_ANG(   0,   0,   0); //20
      SKEL3D_ANG( 256,   0,   0); //21
      SKEL3D_ANG(   0,   0,   0); //22
      SKEL3D_ANG( 768,   0,   0); //23
      SKEL3D_ANG(   0,   0,   0); //24
      SKEL3D_ANG( 256,   0,   0); //25
      SKEL3D_ANG( 896,   0,   0); //26
      SKEL3D_ANG(   0,   0,   0); //27
      SKEL3D_ANG(   0,   0,   0); //28
      SKEL3D_ANG( 256,   0,   0); //29
      SKEL3D_ANG(   0,   0,   0); //30
      SKEL3D_ANG( 768,   0,   0); //31
      SKEL3D_ANG(   0,   0,   0); //32
      SKEL3D_ANG(   0,   0,   0); //33
      SKEL3D_ANG(   0,   0,   0); //34
      SKEL3D_ANG( 256,   0,   0); //35
      SKEL3D_ANG(   0,   0,   0); //36
      SKEL3D_ANG( 768,   0,   0); //37
      SKEL3D_ANG(   0,   0,   0); //38
      SKEL3D_ANG(   0,   0,   0); //39
      SKEL3D_ANG(   0,   0,   0); //40
      SKEL3D_ANG(   0,   0,   0); //41
      SKEL3D_ANG(   0,   0,   0); //42
      SKEL3D_ANG( 768,   0,   0); //43
      SKEL3D_ANG(   0,   0,   0); //44
      SKEL3D_ANG( 256,   0,   0); //45
      SKEL3D_ANG(   0,   0,   0); //46
      SKEL3D_ANG(   0,   0,   0); //47
      SKEL3D_ANG(   0,   0,   0); //48
      SKEL3D_ANG(   0,   0,   0); //49
      SKEL3D_ANG(   0,   0,   0); //50
      SKEL3D_ANG(   0,   0,   0); //51
      SKEL3D_ANG(   0,   0,   0); //52
      SKEL3D_ANG(   0,   0,   0); //53
      SKEL3D_ANG(   0,   0,   0); //54
      SKEL3D_ANG(   0,   0,   0); //55
      SKEL3D_ANG(   0,   0,   0); //56
      SKEL3D_ANG(   0,   0,   0); //57
      SKEL3D_ANG(   0,   0,   0); //58
      SKEL3D_ANG(   0,   0,   0); //59
      SKEL3D_ANG(   0,   0,   0); //60
      SKEL3D_ANG(   0,   0,   0); //61
      SKEL3D_ANG(   0,   0,   0); //62
      SKEL3D_ANG(   0,   0,   0); //63
      SKEL3D_ANG(   0,   0,   0); //64
      SKEL3D_ANG(   0,   0,   0); //65
      SKEL3D_ANG(   0,   0,   0); //66
      SKEL3D_ANG(   0,   0,   0); //67
      SKEL3D_ANG(   0,   0,   0); //68
      SKEL3D_ANG( 768,   0,   0); //69
      SKEL3D_ANG( 256,   0,   0); //70
      SKEL3D_ANG( 256,   0,   0); //71
      SKEL3D_ANG( 768, 924,   0); //72
      SKEL3D_ANG(   0,   0,   0); //73
      SKEL3D_ANG(   0, 200,   0); //74
      SKEL3D_ANG( 256, 768,   0); //75
      SKEL3D_ANG( 256, 256,   0); //76
      SKEL3D_ANG( 256, 256,   0); //77
      SKEL3D_ANG( 256, 768,   0); //78
      SKEL3D_ANG(   0,   0,   0); //79
      SKEL3D_ANG(   0,   0,   0); //80
      SKEL3D_ANG(   0, 768,   0); //81
      SKEL3D_ANG(   0,   0,   0); //82
      SKEL3D_ANG(   0, 256,   0); //83
      SKEL3D_ANG(   0, 768,   0); //84
      SKEL3D_ANG(   0,   0,   0); //85
      SKEL3D_ANG(   0, 256,   0); //86
      SKEL3D_ANG(   0,   0,   0); //87
      SKEL3D_ANG(   0, 256,   0); //88
      SKEL3D_ANG(   0,   0,   0); //89
      SKEL3D_ANG(   0, 768,   0); //90
      SKEL3D_ANG(   0, 256,   0); //91
      SKEL3D_ANG(   0,   0,   0); //92
      SKEL3D_ANG(   0, 768,   0); //93
      SKEL3D_ANG(   0,   0,   0); //94
      SKEL3D_ANG(   0,   0,   0); //95
      SKEL3D_ANG(   0, 256,   0); //96
      SKEL3D_ANG(   0,   0,   0); //97
      SKEL3D_ANG(   0,   0,   0); //98
      SKEL3D_ANG(   0,   0,   0); //99
      SKEL3D_ANG(   0,   0,   0); //100
      SKEL3D_ANG(   0, 768,   0); //101
      SKEL3D_ANG(   0,   0,   0); //102
      SKEL3D_ANG(   0, 256,   0); //103
      SKEL3D_ANG(   0,   0,   0); //104
      SKEL3D_ANG(   0,   0,   0); //105
      SKEL3D_ANG(   0, 768,   0); //106
      SKEL3D_ANG(   0,   0,   0); //107
      SKEL3D_ANG(   0,   0,   0); //108
      SKEL3D_ANG(   0, 768,   0); //109
    break; case '3': //Walk position 3
      SKEL3D_ANG(   0,   0,   0); //0
      SKEL3D_ANG( 256, 999,   0); //1
      SKEL3D_ANG( 512,   0,   0); //2
      SKEL3D_ANG( 768,   0,   0); //3
      SKEL3D_ANG( 768,  30,   0); //4
      SKEL3D_ANG(   0,   0,   0); //5
      SKEL3D_ANG( 256,   0,   0); //6
      SKEL3D_ANG(   0,   0,   0); //7
      SKEL3D_ANG(   0,1009, 256); //8
      SKEL3D_ANG(   0,   0,   0); //9
      SKEL3D_ANG(   0, 974, 768); //10
      SKEL3D_ANG(   0,   0,   0); //11
      SKEL3D_ANG( 896,   0,   0); //12
      SKEL3D_ANG(   0,   0,   0); //13
      SKEL3D_ANG(   0,   0,   0); //14
      SKEL3D_ANG(   0,   0,   0); //15
      SKEL3D_ANG( 128,   0,   0); //16
      SKEL3D_ANG(   0,   0,   0); //17
      SKEL3D_ANG( 128,   0,   0); //18
      SKEL3D_ANG( 768,   0,   0); //19
      SKEL3D_ANG(   0,   0,   0); //20
      SKEL3D_ANG( 256,   0,   0); //21
      SKEL3D_ANG(   0,   0,   0); //22
      SKEL3D_ANG( 768,   0,   0); //23
      SKEL3D_ANG(   0,   0,   0); //24
      SKEL3D_ANG( 256,   0,   0); //25
      SKEL3D_ANG( 896,   0,   0); //26
      SKEL3D_ANG(   0,   0,   0); //27
      SKEL3D_ANG(   0,   0,   0); //28
      SKEL3D_ANG( 256,   0,   0); //29
      SKEL3D_ANG(   0,   0,   0); //30
      SKEL3D_ANG( 768,   0,   0); //31
      SKEL3D_ANG(   0,   0,   0); //32
      SKEL3D_ANG(   0,   0,   0); //33
      SKEL3D_ANG(   0,   0,   0); //34
      SKEL3D_ANG( 256,   0,   0); //35
      SKEL3D_ANG(   0,   0,   0); //36
      SKEL3D_ANG( 768,   0,   0); //37
      SKEL3D_ANG(   0,   0,   0); //38
      SKEL3D_ANG(   0,   0,   0); //39
      SKEL3D_ANG(   0,   0,   0); //40
      SKEL3D_ANG(   0,   0,   0); //41
      SKEL3D_ANG(   0,   0,   0); //42
      SKEL3D_ANG( 768,   0,   0); //43
      SKEL3D_ANG(   0,   0,   0); //44
      SKEL3D_ANG( 256,   0,   0); //45
      SKEL3D_ANG(   0,   0,   0); //46
      SKEL3D_ANG(   0,   0,   0); //47
      SKEL3D_ANG(   0,   0,   0); //48
      SKEL3D_ANG(   0,   0,   0); //49
      SKEL3D_ANG(   0,   0,   0); //50
      SKEL3D_ANG(   0,   0,   0); //51
      SKEL3D_ANG(   0,   0,   0); //52
      SKEL3D_ANG(   0,   0,   0); //53
      SKEL3D_ANG(   0,   0,   0); //54
      SKEL3D_ANG(   0,   0,   0); //55
      SKEL3D_ANG(   0,   0,   0); //56
      SKEL3D_ANG(   0,   0,   0); //57
      SKEL3D_ANG(   0,   0,   0); //58
      SKEL3D_ANG(   0,   0,   0); //59
      SKEL3D_ANG(   0,   0,   0); //60
      SKEL3D_ANG(   0,   0,   0); //61
      SKEL3D_ANG(   0,   0,   0); //62
      SKEL3D_ANG(   0,   0,   0); //63
      SKEL3D_ANG(   0,   0,   0); //64
      SKEL3D_ANG(   0,   0,   0); //65
      SKEL3D_ANG(   0,   0,   0); //66
      SKEL3D_ANG(   0,   0,   0); //67
      SKEL3D_ANG(   0,   0,   0); //68
      SKEL3D_ANG( 768,   0,   0); //69
      SKEL3D_ANG( 256,   0,   0); //70
      SKEL3D_ANG( 256,   0,   0); //71
      SKEL3D_ANG( 768, 974,   0); //72
      SKEL3D_ANG(   0, 100,   0); //73
      SKEL3D_ANG(   0,   0,   0); //74
      SKEL3D_ANG( 256, 768,   0); //75
      SKEL3D_ANG( 256, 256,   0); //76
      SKEL3D_ANG( 256, 256,   0); //77
      SKEL3D_ANG( 256, 768,   0); //78
      SKEL3D_ANG(   0,   0,   0); //79
      SKEL3D_ANG(   0,   0,   0); //80
      SKEL3D_ANG(   0, 768,   0); //81
      SKEL3D_ANG(   0,   0,   0); //82
      SKEL3D_ANG(   0, 256,   0); //83
      SKEL3D_ANG(   0, 768,   0); //84
      SKEL3D_ANG(   0,   0,   0); //85
      SKEL3D_ANG(   0, 256,   0); //86
      SKEL3D_ANG(   0,   0,   0); //87
      SKEL3D_ANG(   0, 256,   0); //88
      SKEL3D_ANG(   0,   0,   0); //89
      SKEL3D_ANG(   0, 768,   0); //90
      SKEL3D_ANG(   0, 256,   0); //91
      SKEL3D_ANG(   0,   0,   0); //92
      SKEL3D_ANG(   0, 768,   0); //93
      SKEL3D_ANG(   0,   0,   0); //94
      SKEL3D_ANG(   0,   0,   0); //95
      SKEL3D_ANG(   0, 256,   0); //96
      SKEL3D_ANG(   0,   0,   0); //97
      SKEL3D_ANG(   0,   0,   0); //98
      SKEL3D_ANG(   0,   0,   0); //99
      SKEL3D_ANG(   0,   0,   0); //100
      SKEL3D_ANG(   0, 768,   0); //101
      SKEL3D_ANG(   0,   0,   0); //102
      SKEL3D_ANG(   0, 256,   0); //103
      SKEL3D_ANG(   0,   0,   0); //104
      SKEL3D_ANG(   0,   0,   0); //105
      SKEL3D_ANG(   0, 768,   0); //106
      SKEL3D_ANG(   0,   0,   0); //107
      SKEL3D_ANG(   0,   0,   0); //108
      SKEL3D_ANG(   0, 768,   0); //109
  }

#undef SKEL3D_ANG

}

void Skel3D::setPosition(int x,int y,int z) {
  matOrigin[3]=x; matOrigin[7]=y; matOrigin[11]=z;
}

void Skel3D::setAngle(int psi,int phi,int tta) {
  mmSetRotMat3x4(matOrigin,psi,phi,tta);
}

void Skel3D::setViewAngle(int psi,int phi,int tta) {
  mmSetRotMat3x4(matView,psi,phi,tta);
}

void Skel3D::setDeltaAngle(int* dAng,int ang,int angFin) {
  *dAng=(angFin-ang)>>SKEL3D_STEPPOWER;
  if (mmAbs(*dAng)>((512<<16)>>SKEL3D_STEPPOWER)) {
    *dAng+=((*dAng)<0? ( 1024<<16)>>SKEL3D_STEPPOWER :
                       (-1024<<16)>>SKEL3D_STEPPOWER);
  }
}
