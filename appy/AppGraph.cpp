////////////////////////////////////////////////////////////////////////////////
//Dan Nielsen
////////////////////////////////////////////////////////////////////////////////
#include "AppGraph.h"
#include "Common.h"
#include "MoreMath.h"
#include <stdio.h>
#include <limits.h>
#include <string.h> //For strlen()
#include <ctype.h> //For tolower()
#include <stdlib.h>

//Variables [curveForm4b,positionIndex5b,positionIndex5b] giving curve type and
//beginning and ending curve positions on a 30-point (5x6) grid for each of 24
//possible curves in each of 128 possible characters
static unsigned short _agTextCharacterCurveData[128][24];
static unsigned char _agSize=8; //Grid spacing
static unsigned char _agThickness=128; //Font boldness
static int _agTextX=100;
static int _agTextY=380;
static int _agTextOffsetX=30;
static int _agTextOffsetY=80;
//static unsigned int _agTextGuideNumPoints;
static int _agTextGuidePoint[256];
static unsigned short _agTextGuideWeight[128];
static unsigned short _agTextGuideKnot[131];
static int _agHueRotateMatrix[12];
static bool _agHueRotateMode=false,_agOverdrawMode=true;
void (*agSetPixel)(unsigned short* pixel,unsigned char r,unsigned char g,
                   unsigned char b)=commonSetPixel;

////////////////////////////////////////////////////////////////////////////////
//Basic
////////////////////////////////////////////////////////////////////////////////

unsigned char agGetLuminance(unsigned short* pixel) {
  unsigned char rgb[3]; commonGetRGB(rgb,pixel);
  return (AG_LUMRED*rgb[0]+AG_LUMGREEN*rgb[1]+AG_LUMBLUE*rgb[2])>>8;
}

void agNoOverdrawSetPixel(unsigned short* pixel,unsigned char r,unsigned char g,
                          unsigned char b) {
  if (!*pixel) commonSetPixel(pixel,r,g,b);
}

void agSetOverdrawOff() { _agOverdrawMode=false; agSetPixelDrawMode(); }

void agSetOverdrawOn() { _agOverdrawMode=true; agSetPixelDrawMode(); }

void agSetPixelDrawMode() {
  if (_agOverdrawMode) {
    if (_agHueRotateMode) agSetPixel=agHueRotateSetPixel;
    else agSetPixel=commonSetPixel;
  }
  else {
    if (_agHueRotateMode) agSetPixel=agHueRotateNoOverdrawSetPixel;
    else agSetPixel=agNoOverdrawSetPixel;
  }
}

void agHueRotateSetPixel(unsigned short* pixel,unsigned char r,unsigned char g,
                         unsigned char b) {
  //Transform color vector
  int v[3];
  v[0]=_agHueRotateMatrix[ 0]*r+
       _agHueRotateMatrix[ 1]*g+
       _agHueRotateMatrix[ 2]*b+_agHueRotateMatrix[ 3];
  v[1]=_agHueRotateMatrix[ 4]*r+
       _agHueRotateMatrix[ 5]*g+
       _agHueRotateMatrix[ 6]*b+_agHueRotateMatrix[ 7];
  v[2]=_agHueRotateMatrix[ 8]*r+
       _agHueRotateMatrix[ 9]*g+
       _agHueRotateMatrix[10]*b+_agHueRotateMatrix[11];
  commonSetPixel(pixel,
                 (v[0]>>=16)<0? 0 : (v[0]>255? 255: v[0]),
                 (v[1]>>=16)<0? 0 : (v[1]>255? 255: v[1]),
                 (v[2]>>=16)<0? 0 : (v[2]>255? 255: v[2]));
}

void agHueRotateNoOverdrawSetPixel(unsigned short* pixel,unsigned char r,
                                   unsigned char g,unsigned char b) {
  if (!*pixel) {
    //Transform color vector
    int v[3];
    v[0]=_agHueRotateMatrix[ 0]*r+
         _agHueRotateMatrix[ 1]*g+
         _agHueRotateMatrix[ 2]*b+_agHueRotateMatrix[ 3];
    v[1]=_agHueRotateMatrix[ 4]*r+
         _agHueRotateMatrix[ 5]*g+
         _agHueRotateMatrix[ 6]*b+_agHueRotateMatrix[ 7];
    v[2]=_agHueRotateMatrix[ 8]*r+
         _agHueRotateMatrix[ 9]*g+
         _agHueRotateMatrix[10]*b+_agHueRotateMatrix[11];
    commonSetPixel(pixel,
                   (v[0]>>=16)<0? 0 : (v[0]>255? 255: v[0]),
                   (v[1]>>=16)<0? 0 : (v[1]>255? 255: v[1]),
                   (v[2]>>=16)<0? 0 : (v[2]>255? 255: v[2]));
  }
}

unsigned char agGetSize() { return _agSize; }

void agSetSize(unsigned char size) { _agSize=size; }

void agSetHueAngle(int angle) {
#define AG_LUMY -((AG_LUMRED+AG_LUMGREEN+AG_LUMBLUE)*MM_ONEOVERSQRRTTHREE)
#define AG_LUMSHEARX \
          (((AG_LUMRED*MM_SQRRTTWOOVERSQRRTTHREE- \
             AG_LUMGREEN*MM_ONEOVERSQRRTSIX- \
             AG_LUMBLUE*MM_ONEOVERSQRRTSIX)<<6)/(AG_LUMY>>10))
#define AG_LUMSHEARZ \
          ((((-AG_LUMGREEN+AG_LUMGREEN)*MM_ONEOVERSQRRTTWO)<<6)/(AG_LUMY>>10))
#define AG_FULLSCREENHUEROTATEERRORCORRECTION 0
  //Choose the most efficient set-pixel function
  _agHueRotateMode=(angle!=0); agSetPixelDrawMode(); if (!angle) return;
  int ma[12],mb[12];
  //Rotate the gray vector onto the positive y-axis, then shear the space so
  //that the vector of luminance weight constants falls in the plane aligned
  //with the y-axis
  ma[0]=MM_SQRRTTWOOVERSQRRTTHREE+
        (((AG_LUMSHEARX>>2)*(MM_ONEOVERSQRRTTHREE>>2))>>12);
  ma[1]=-MM_ONEOVERSQRRTSIX+(((AG_LUMSHEARX>>2)*(MM_ONEOVERSQRRTTHREE>>2))>>12);
  ma[2]=-MM_ONEOVERSQRRTSIX+(((AG_LUMSHEARX>>2)*(MM_ONEOVERSQRRTTHREE>>2))>>12);
  ma[3]=0;
  ma[4]=MM_ONEOVERSQRRTTHREE;
  ma[5]=MM_ONEOVERSQRRTTHREE;
  ma[6]=MM_ONEOVERSQRRTTHREE;
  ma[7]=0;
  ma[8]=(((AG_LUMSHEARZ>>2)*(MM_ONEOVERSQRRTTHREE>>2))>>12);
  ma[9]=(((AG_LUMSHEARZ>>2)*(MM_ONEOVERSQRRTTHREE>>2))>>12)-MM_ONEOVERSQRRTTWO;
  ma[10]=(((AG_LUMSHEARZ>>2)*(MM_ONEOVERSQRRTTHREE>>2))>>12)+MM_ONEOVERSQRRTTWO;
  ma[11]=0;
  //Rotate about y-axis
  int s=mmSin(angle),
      c=mmCos(angle);
  _agHueRotateMatrix[0]=c;
  _agHueRotateMatrix[1]=0;
  _agHueRotateMatrix[2]=-s;
  _agHueRotateMatrix[3]=0;
  _agHueRotateMatrix[4]=0;
  _agHueRotateMatrix[5]=0x10000;
  _agHueRotateMatrix[6]=0;
  _agHueRotateMatrix[7]=0;
  _agHueRotateMatrix[8]=s;
  _agHueRotateMatrix[9]=0;
  _agHueRotateMatrix[10]=c;
  _agHueRotateMatrix[11]=0;
  mmMat3x4FakeMul(mb,_agHueRotateMatrix,ma);
  //Unshear the space, and rotate the gray vector back into place
  ma[0]=MM_SQRRTTWOOVERSQRRTTHREE;
  ma[1]=-(((AG_LUMSHEARX>>2)*(MM_SQRRTTWOOVERSQRRTTHREE>>2))>>12)+
        MM_ONEOVERSQRRTTHREE;
  ma[2]=0;
  ma[3]=AG_FULLSCREENHUEROTATEERRORCORRECTION;
  ma[4]=-MM_ONEOVERSQRRTSIX;
  ma[5]=(((AG_LUMSHEARX>>2)*(MM_ONEOVERSQRRTSIX>>2))>>12)+
        MM_ONEOVERSQRRTTHREE+(((AG_LUMSHEARZ>>2)*(MM_ONEOVERSQRRTTWO>>2))>>12);
  ma[6]=-MM_ONEOVERSQRRTTWO;
  ma[7]=AG_FULLSCREENHUEROTATEERRORCORRECTION;
  ma[8]=-MM_ONEOVERSQRRTSIX;
  ma[9]=(((AG_LUMSHEARX>>2)*(MM_ONEOVERSQRRTSIX>>2))>>12)+MM_ONEOVERSQRRTTHREE-
        (((AG_LUMSHEARZ>>2)*(MM_ONEOVERSQRRTTWO>>2))>>12);
  ma[10]=MM_ONEOVERSQRRTTWO;
  ma[11]=AG_FULLSCREENHUEROTATEERRORCORRECTION;
  mmMat3x4FakeMul(_agHueRotateMatrix,ma,mb);
#undef AG_SCALEDLUMSHEARX
#undef AG_SCALEDLUMSHEARY
#undef AG_FULLSCREENHUEROTATEERRORCORRECTION
}

////////////////////////////////////////////////////////////////////////////////
//Blit
////////////////////////////////////////////////////////////////////////////////

void agBlit(unsigned short* buff,unsigned short* src,int widthSrc,int heightSrc,
            int xPos,int yPos) {
  buff+=((yPos-(heightSrc>>1))*AG_WIDTH+(xPos-(widthSrc>>1)));
  for(int y=0;y<heightSrc;y++) {
    memcpy(buff,src,widthSrc<<1);
    buff+=AG_WIDTH;
    src+=widthSrc;
  }
}

void agGrayBlit(unsigned short* buff,unsigned short* src,
                int widthSrc,int heightSrc,int xPos,int yPos) {
  buff+=((yPos-(heightSrc>>1))*AG_WIDTH+(xPos-(widthSrc>>1)));
  const int dBuff=(AG_WIDTH-widthSrc);
  int lum;
  for(int y=0;y<heightSrc;y++) {
    for(int x=0;x<widthSrc;x++,buff++,src++) {
      lum=agGetLuminance(src);
      agSetPixel(buff,lum,lum,lum);
    }
    buff+=dBuff;
  }
}

void agFullScreenGrayUcharBlit(unsigned char* buff,unsigned short* src) {
  for(int i=0;i<AG_NUMPIXELS;i++,buff++,src++)
    *buff=agGetLuminance(src);
}

//Value of sat is in [0..128]
void agSaturateBlit(unsigned short* buff,unsigned short* src,
                    int widthSrc,int heightSrc,int xPos,int yPos,
                    unsigned char sat) {
  buff+=(yPos-(heightSrc>>1))*AG_WIDTH+(xPos-(widthSrc>>1));
  const int dBuff=AG_WIDTH-widthSrc;
  const unsigned char satInv=128-sat,rc=(AG_LUMRED*satInv)>>7,
                                     gc=(AG_LUMGREEN*satInv)>>7,
                                     bc=(AG_LUMBLUE*satInv)>>7;
  int sum;
  unsigned char rgb[3];
  for(int y=0;y<heightSrc;y++) {
    for(int x=0;x<widthSrc;x++,buff++,src++) {
      commonGetRGB(rgb,src);
      sum=rc*rgb[0]+gc*rgb[1]+bc*rgb[2];
      agSetPixel(buff,(sat*rgb[0]+sum)>>8,(sat*rgb[1]+sum)>>8,
                      (sat*rgb[2]+sum)>>8);
    }
    buff+=dBuff;
  }
}

void agFullScreenBlit(unsigned short* buff,unsigned short* src) {
  if (!_agHueRotateMode&&_agOverdrawMode) {
    memcpy(buff,src,(AG_NUMPIXELS)<<1);
  }
  else {
    unsigned char rgb[3];
    for(int i=0;i<AG_NUMPIXELS;i++,buff++,src++) {
      commonGetRGB(rgb,src);
      agSetPixel(buff,rgb[0],rgb[1],rgb[2]);
    }
  }
}

void agFullScreenLightBlendBlit(unsigned short* buff,unsigned short* src) {
  unsigned char rgbBuff[3],rgbSrc[3],random=0;
  unsigned short a,b,c;
  for(unsigned int i=0;i<AG_NUMPIXELS;i++,buff++,src++) {
    commonGetRGB(rgbBuff,buff);
    commonGetRGB(rgbSrc,src);
    //Blend in source image with 1/8 intensity
    //Randomness keeps integer rounding error from trapping values
    a=((7*rgbBuff[0]+rgbSrc[0])>>3)+
      (random=(random? random-1 : commonRandomUchar()&0x7));
    b=((7*rgbBuff[1]+rgbSrc[1])>>3)+random;
    c=((7*rgbBuff[2]+rgbSrc[2])>>3)+random;
    a=(a>255? 255 : a);
    b=(b>255? 255 : b);
    c=(c>255? 255 : c);
    agSetPixel(buff,a,b,c);
  }
}


void agFullScreenHeavyBlendBlit(unsigned short* buff,unsigned short* src) {
  unsigned char rgbBuff[3],rgbSrc[3],random=0;
  unsigned short a,b,c;
  for(unsigned int i=0;i<AG_NUMPIXELS;i++,buff++,src++) {
    commonGetRGB(rgbBuff,buff);
    commonGetRGB(rgbSrc,src);
    //Blend in source image with 3/4 intensity
    //Randomness keeps integer rounding error from trapping values
    a=((rgbBuff[0]+3*rgbSrc[0])>>2)+
      (random=(random? random-1 : commonRandomUchar()&0x7));
    b=((rgbBuff[1]+3*rgbSrc[1])>>2)+random;
    c=((rgbBuff[2]+3*rgbSrc[2])>>2)+random;
    a=(a>255? 255 : a);
    b=(b>255? 255 : b);
    c=(c>255? 255 : c);
    agSetPixel(buff,a,b,c);
  }
}

void agFullScreenBlurBlit(unsigned short* buff,unsigned short* src) {
  unsigned char rgb[9][3];
  src+=AG_WIDTH+1;
  buff+=AG_WIDTH+1;
  for(int j=1;j<AG_HEIGHT-1;j++,buff+=2,src+=2) {
    for(int i=1;i<AG_WIDTH-1;i++,buff++,src++) {
      commonGetRGB(rgb[0],src-AG_WIDTH-1);
      commonGetRGB(rgb[1],src-AG_WIDTH);
      commonGetRGB(rgb[2],src-AG_WIDTH+1);
      commonGetRGB(rgb[3],src-1);
      commonGetRGB(rgb[4],src);
      commonGetRGB(rgb[5],src+1);
      commonGetRGB(rgb[6],src+AG_WIDTH-1);
      commonGetRGB(rgb[7],src+AG_WIDTH);
      commonGetRGB(rgb[8],src+AG_WIDTH+1);
      agSetPixel(buff,
                 (rgb[4][0]+
                  ((rgb[1][0]+rgb[3][0]+rgb[5][0]+rgb[7][0])>>1)+
                  ((rgb[0][0]+rgb[2][0]+rgb[6][0]+rgb[8][0])>>2))>>2,
                 (rgb[4][1]+
                  ((rgb[1][1]+rgb[3][1]+rgb[5][1]+rgb[7][1])>>1)+
                  ((rgb[0][1]+rgb[2][1]+rgb[6][1]+rgb[8][1])>>2))>>2,
                 (rgb[4][2]+
                  ((rgb[1][2]+rgb[3][2]+rgb[5][2]+rgb[7][2])>>1)+
                  ((rgb[0][2]+rgb[2][2]+rgb[6][2]+rgb[8][2])>>2))>>2);
    }
  }
}

void agGreenScreenBlit(unsigned short* buff,unsigned short* src,
                       int widthSrc,int heightSrc,int xPos,int yPos) {
  buff+=(yPos-(heightSrc>>1))*AG_WIDTH+(xPos-(widthSrc>>1));
  const int dBuff=AG_WIDTH-widthSrc;
  unsigned char rgbSrc[3];
  for(int y=0;y<heightSrc;y++) {
    for(int x=0;x<widthSrc;x++,buff++,src++) {
      commonGetRGB(rgbSrc,src);
      if (rgbSrc[0]>=128||rgbSrc[1]<128||rgbSrc[2]>=128)
        agSetPixel(buff,rgbSrc[0],rgbSrc[1],rgbSrc[2]);
    }
    buff+=dBuff;
  }
}

void agColorBlit(unsigned short* buff,unsigned short* src,int widthSrc,
                 int heightSrc,int xPos,int yPos,unsigned char swap,
                 unsigned char contrast,unsigned char brightness) {
  buff+=(yPos-(heightSrc>>1))*AG_WIDTH+(xPos-(widthSrc>>1));
  const int dBuff=AG_WIDTH-widthSrc;
  unsigned char rgbSrc[3],rgbTmp[3];
  for(int y=0;y<heightSrc;y++) {
    for(int x=0;x<widthSrc;x++,buff++,src++) {
      commonGetRGB(rgbSrc,src);
      rgbTmp[0]=
        128+((contrast*((((0x80-swap)*rgbSrc[0]+swap*rgbSrc[2])>>7)-128))>>7);
      rgbTmp[1]=
        128+((contrast*(rgbSrc[1]-128))>>7);
      rgbTmp[2]=
        128+((contrast*(((swap*rgbSrc[0]+(0x80-swap)*rgbSrc[2])>>7)-128))>>7);
      agSetPixel(buff,(brightness*rgbTmp[0])>>7,
                      (brightness*rgbTmp[1])>>7,
                      (brightness*rgbTmp[2])>>7);
    }
    buff+=dBuff;
  }
}

void agRotateGreenScreenBlit(unsigned short* buff,
                             unsigned short* src,
                             int widthSrc,int heightSrc,
                             int xPos,int yPos,int tta) {
  buff+=yPos*AG_WIDTH+xPos;
  const int dBuff=AG_WIDTH-widthSrc,
            c=mmCos(tta),s=mmSin(tta);
  int xSrc,ySrc,xoSrc,yoSrc,k;
  unsigned char rgbSrc[3];
  xSrc=xoSrc=(heightSrc>>1)*s-(widthSrc>>1)*c;
  ySrc=yoSrc=-(heightSrc>>1)*c-(widthSrc>>1)*s;
  for(int y=0;y<heightSrc;y++) {
    for(int x=0;x<widthSrc;x++) {
      k=(ySrc>>16)*AG_WIDTH+(xSrc>>16);
      commonGetRGB(rgbSrc,src+k);
      if (rgbSrc[0]>=128||rgbSrc[1]<80||rgbSrc[2]>=128) {
        agSetPixel(buff,rgbSrc[0],rgbSrc[1],rgbSrc[2]);
      }
      buff++;
      xSrc+=c;
      ySrc+=s;
    }
    buff+=dBuff;
    xoSrc-=s;
    yoSrc+=c;
    xSrc=xoSrc;
    ySrc=yoSrc;
  }
}

void agFullScreenWaveBlit(unsigned short* buff,unsigned short* src,
                          unsigned char strength,unsigned char counter) {
  unsigned short *srcCurr;
  int xCurr,yCurr;
  int offset;
  unsigned char rgbSrcCurr[3];
  for(int y=0;y<AG_HEIGHT;y++) {
    for(int x=0;x<AG_WIDTH;x++,buff++) {
      offset=(strength*mmSin(((x+y)<<1)+(counter<<4)))>>21;
      xCurr=x+offset;
      yCurr=y+offset;
      if (xCurr<0) xCurr=0;
      else if (xCurr>=AG_WIDTH) xCurr=(AG_WIDTH-1);
      if (yCurr<0) yCurr=0;
      else if (yCurr>=AG_HEIGHT) yCurr=(AG_HEIGHT-1);
      srcCurr=src+(yCurr*AG_WIDTH+xCurr);
      commonGetRGB(rgbSrcCurr,srcCurr);
      agSetPixel(buff,rgbSrcCurr[0],rgbSrcCurr[1],rgbSrcCurr[2]);
    }
  }
}

//Encodes the x- and y-derivatives of luminance into the crumple buffer
void agSetCrumpleBuffer(unsigned char* crumpleBuff,unsigned short* buff) {
  buff+=AG_WIDTH+1;
  crumpleBuff+=AG_WIDTH+1;
  for(int i=0;i<AG_NUMPIXELS-(AG_WIDTH+1);i++,crumpleBuff++,buff++) {
    *crumpleBuff=agGetLuminance(buff-1)-agGetLuminance(buff)+8;
    *crumpleBuff|=(agGetLuminance(buff-AG_WIDTH)-agGetLuminance(buff))<<4;
  }
}

void agGreenScreenCrumpleBlit(unsigned short* buff,unsigned char* crumpleBuff,
                              unsigned short* src,int widthSrc,int heightSrc,
                              int xPos,int yPos) {
  crumpleBuff+=(yPos-(heightSrc>>1))*AG_WIDTH+(xPos-(widthSrc>>1));
  buff+=(yPos-(heightSrc>>1))*AG_WIDTH+(xPos-(widthSrc>>1));
  const int dCrumpleBuff=AG_WIDTH-widthSrc;
  const int dBuff=dCrumpleBuff;
  unsigned short *srcCurr;
  unsigned char rgbSrcCurr[3];
  int xCurr,yCurr;
  for(int y=0;y<heightSrc;y++) {
    for(int x=0;x<widthSrc;x++,crumpleBuff++,buff++) {
      xCurr=x+(*crumpleBuff&0xf)-8;
      yCurr=y+(*crumpleBuff>>4)-8;
      if (xCurr<0) xCurr=0;
      else if (xCurr>=widthSrc) xCurr=(widthSrc-1);
      if (yCurr<0) yCurr=0;
      else if (yCurr>=heightSrc) yCurr=(heightSrc-1);
      srcCurr=src+(yCurr*widthSrc+xCurr);
      commonGetRGB(rgbSrcCurr,srcCurr);
      if (rgbSrcCurr[0]>=128||rgbSrcCurr[1]<80||rgbSrcCurr[2]>=128) {
        agSetPixel(buff,rgbSrcCurr[0],rgbSrcCurr[1],rgbSrcCurr[2]);
      }
    }
    crumpleBuff+=dCrumpleBuff;
    buff+=dBuff;
  }
}

////////////////////////////////////////////////////////////////////////////////
//Basic full-screen effect
////////////////////////////////////////////////////////////////////////////////

void agFullScreenDim(unsigned short* buff) {
  unsigned char rgb[3];
  for(int i=0;i<AG_NUMPIXELS;i++,buff++) {
    commonGetRGB(rgb,buff);
    if (rgb[0]>15) rgb[0]-=16;
    else rgb[0]=0;
    if (rgb[1]>15) rgb[1]-=16;
    else rgb[1]=0;
    if (rgb[2]>15) rgb[2]-=16;
    else rgb[2]=0;
    agSetPixel(buff,rgb[0],rgb[1],rgb[2]);
  }
}

//Smears directionally, but the effect is negligible
void agFullScreenBlur(unsigned short* buff) {
  unsigned char rgb[9][3];
  buff+=AG_WIDTH+1;
  for(int j=1;j<AG_HEIGHT-1;j++) {
    for(int i=1;i<AG_WIDTH-1;i++,buff++) {
      commonGetRGB(rgb[0],buff-AG_WIDTH-1);
      commonGetRGB(rgb[1],buff-AG_WIDTH);
      commonGetRGB(rgb[2],buff-AG_WIDTH+1);
      commonGetRGB(rgb[3],buff-1);
      commonGetRGB(rgb[4],buff);
      commonGetRGB(rgb[5],buff+1);
      commonGetRGB(rgb[6],buff+AG_WIDTH-1);
      commonGetRGB(rgb[7],buff+AG_WIDTH);
      commonGetRGB(rgb[8],buff+AG_WIDTH+1);
      agSetPixel(buff,
                 (rgb[4][0]+
                  ((rgb[1][0]+rgb[3][0]+rgb[5][0]+rgb[7][0])>>1)+
                  ((rgb[0][0]+rgb[2][0]+rgb[6][0]+rgb[8][0])>>2))>>2,
                 (rgb[4][1]+
                  ((rgb[1][1]+rgb[3][1]+rgb[5][1]+rgb[7][1])>>1)+
                  ((rgb[0][1]+rgb[2][1]+rgb[6][1]+rgb[8][1])>>2))>>2,
                 (rgb[4][2]+
                  ((rgb[1][2]+rgb[3][2]+rgb[5][2]+rgb[7][2])>>1)+
                  ((rgb[0][2]+rgb[2][2]+rgb[6][2]+rgb[8][2])>>2))>>2);
    }
    buff+=2;
  }
}

//Convert image of size AG_NUMPIXELS to two colors
//Boolean whiteBkg determines whether 0x0000 (black) or 0xffff (white) is to be
//considered the background color
//The other color used is given by r, g, and b
void agFullScreenMonochrome(unsigned short *buff,bool whiteBkg,unsigned char r,
                            unsigned char g,unsigned char b)
{
    for(unsigned int i = 0; i < AG_NUMPIXELS; i++)
        buff[i] = (whiteBkg?
                   (buff[i] == 0xffff? 0 : commonPackRGB(r,g,b)) :
                   (buff[i]? commonPackRGB(r,g,b) : 0));
}

//If image of size AG_NUMPIXELS is of two colors, this function swaps those colors
void agFullScreenSwapTwoColor(unsigned short* buff)
{
    unsigned short clr0 = *buff,clr1,*buff0 = buff;
    while(*buff++== clr0);
    clr1 = *(buff - 1);
    buff = buff0;
    for(int i = 0; i < AG_NUMPIXELS; i++,buff++)
        *buff = (*buff == clr0? clr1 : clr0);
}

////////////////////////////////////////////////////////////////////////////////
//2D primitive
////////////////////////////////////////////////////////////////////////////////

void agLine(unsigned short* buff,int x0,int y0,int x1,int y1,
            unsigned char r,unsigned char g,unsigned char b) {
#define AG_XMIN 0
#define AG_XMAX ((AG_WIDTH-1)<<16)
#define AG_YMIN 0
#define AG_YMAX ((AG_HEIGHT-1)<<16)
#define agSetOutCode(outCode,x,y) \
  (outCode)=0; \
  if ((y)>AG_YMAX) (outCode)|=0x1; \
  else if ((y)<AG_YMIN) (outCode)|=0x2; \
  if ((x)>AG_XMAX) (outCode)|=0x4; \
  else if ((x)<AG_XMIN) (outCode)|=0x8;
  //Clip line segment
  unsigned char outCode0=0x0,outCode1=0x0,outCodeOut;
  bool accept=false;
  int x=0,y=0;
  agSetOutCode(outCode0,x0,y0);
  agSetOutCode(outCode1,x1,y1);
  for(;;) {
    if (!(outCode0|outCode1)) { accept=true; break; }
    if (outCode0&outCode1) { break; }
    outCodeOut=(outCode0? outCode0 : outCode1);
    //Determine intersection point
    if (outCodeOut&0x1) { //Above rectangle
      x=x0+(((x1-x0)>>11)*((AG_YMAX-y0))>>11)/((y1-y0)>>10);
      y=AG_YMAX;
    }
    else {
      if (outCodeOut&0x2) { //Below rectangle
        x=x0+(((x1-x0)>>11)*((AG_YMIN-y0)>>11))/((y1-y0)>>10);
        y=AG_YMIN;
      }
      else {
        if (outCodeOut&0x4) { //Right of rectangle
          y=y0+(((y1-y0)>>11)*((AG_XMAX-x0)>>11))/((x1-x0)>>10);
          x=AG_XMAX;
        }
        else {
          if (outCodeOut&0x8) { //Left of rectangle
            y=y0+(((y1-y0)>>11)*((AG_XMIN-x0)>>11))/((x1-x0)>>10);
            x=AG_XMIN;
          }
        }
      }
    }
    //Move outside point to intersection point and prepare for next pass
    if (outCode0) {
      x0=x; y0=y;
      agSetOutCode(outCode0,x0,y0);
    }
    else {
      x1=x; y1=y;
      agSetOutCode(outCode1,x1,y1);
    }
  }
#undef AG_XMIN
#undef AG_XMAX
#undef AG_YMIN
#undef AG_YMAX
#undef agSetOutCode
  if (accept) {
    //Render line segment
    bool steep=(mmAbs(y1-y0)>mmAbs(x1-x0));
    int tmp;
    if (steep) {
      tmp=x0; x0=y0; y0=tmp;
      tmp=x1; x1=y1; y1=tmp;
    }
    if (x0>x1) {
      tmp=x0; x0=x1; x1=tmp;
      tmp=y0; y0=y1; y1=tmp;
    }
    int deltax=x1-x0,
        deltay=mmAbs(y1-y0),
        error=0,
        ystep,
        i;
    if ((y=y0)<y1) { ystep=0x10000; }
    else { ystep=-0x10000; }
    for(x=x0;x<=x1;x+=0x10000) {
      if (steep) {
        i=(y>>16)+(x>>16)*AG_WIDTH;
      }
      else {
        i=(x>>16)+(y>>16)*AG_WIDTH;
      }
      agSetPixel(buff+i,r,g,b);
      error+=deltay;
      if ((error<<1)>=deltax) {
        y+=ystep;
        error-=deltax;
      }
    }
  }
}

//Input p[]: signed 16.16-fixed
void agQuad(unsigned short* buff,int x0,int y0,int x1,int y1,int x2,int y2,
            int x3,int y3,unsigned char r,unsigned char g,unsigned char b) {
  agLine(buff,x0,y0,x1,y1,r,g,b);
  agLine(buff,x1,y1,x2,y2,r,g,b);
  agLine(buff,x2,y2,x3,y3,r,g,b);
  agLine(buff,x3,y3,x0,y0,r,g,b);
}

void agSquare(unsigned short* buff,int width,int x,int y,
              unsigned char r,unsigned char g,unsigned char b) {
  const int dPtr=AG_WIDTH-width;
  const int beginOffset=((y-(width>>1))*AG_WIDTH+(x-(width>>1)));
  unsigned short *ptr=buff+beginOffset;
  for(int j=0;j<width;j++) {
    for(int i=0;i<width;i++) {
      agSetPixel(ptr,r,g,b);
      ptr++;
    }
    ptr+=dPtr;
  }
}

//Input x,y,radius: signed 16.16-fixed
void agCircle(unsigned short* buff,int x,int y,int radius,
              unsigned char r,unsigned char g,unsigned char b) {
  x>>=16; y>>=16; radius>>=16;
  int dx=0,dy=radius-1,dxOffset,dyOffset,offset=AG_WIDTH*y+x,n=0,
      invRadius=0x10000/radius;
  while(dx<=dy) {
    dxOffset=AG_WIDTH*dx;
    dyOffset=AG_WIDTH*dy;
    agSetPixel(buff+(offset+dy-dxOffset),r,g,b);
    agSetPixel(buff+(offset+dx-dyOffset),r,g,b);
    agSetPixel(buff+(offset-dx-dyOffset),r,g,b);
    agSetPixel(buff+(offset-dy-dxOffset),r,g,b);
    agSetPixel(buff+(offset-dy+dxOffset),r,g,b);
    agSetPixel(buff+(offset-dx+dyOffset),r,g,b);
    agSetPixel(buff+(offset+dx+dyOffset),r,g,b);
    agSetPixel(buff+(offset+dy+dxOffset),r,g,b);
    dx++;
    n+=invRadius;
    dy=(int)((radius*mmSinAcosTable[n>>8])>>16);
  }
}

//Input 2 points defining bounding box of ellipse segment, except for the
//3/4-ellipse, where it bounds the missing quarter of an otherwise full ellipse
//In the case of a half-ellipse, the segment is anchored to the first point
//Input numQuarters as 1 for quarter-ellipse, 2 for half-ellipse, etc
//Input orientation is used when numQuarters is in [1..3]
void agEllipticArc(unsigned short* buff,int x0,int y0,int x1,int y1,
                   unsigned char numQuarters,bool orientation,
                   unsigned char r,unsigned char g,unsigned char b) {
  numQuarters=(numQuarters? numQuarters : 4);
  int tmp;
  bool q[4]={false,false,false,false};
  x0>>=16; x1>>=16; y0>>=16; y1>>=16;
  if (numQuarters&1) {
    if (orientation) { x0-=x1-x0; y1+=y1-y0; }
    else { x1+=x1-x0; y0-=y1-y0; }
    tmp=(((orientation<<1)+(y0>y1))<<1)+(x0>x1);
    q[tmp<4? tmp : 7-tmp]=true;
    if (numQuarters==3) {
      for(int i=0;i<4;i++) {
        q[i]=!q[i];
      }
    }
  }
  else if (numQuarters==2) {
    if (orientation) x0-=x1-x0;
    else y0-=y1-y0;
    tmp=(((orientation<<1)+(y0>y1))<<1)+(x0>x1);
    q[((tmp>1)<<1)-(tmp>3)-(tmp>4? tmp&1 : 0)]=
    q[1+((tmp>1)<<1)-(tmp>4? tmp&1 : 0)]=true;
  }
  else {
    q[0]=q[1]=q[2]=q[3]=true;
  }
  tmp=x0; x0=mmMin(x0,x1); x1=mmMax(tmp,x1);
  tmp=y0; y0=mmMin(y0,y1); y1=mmMax(tmp,y1);
  int cx=(x0+x1)>>1,cy=(y0+y1)>>1,
      rx=(x1-x0)>>1,ry=(y1-y0)>>1,
      rx2=rx*rx,ry2=ry*ry,
      twory2=ry2<<1,tworx2=rx2<<1,
      p=ry2-(rx2*ry)+(rx2>>2),
      x=0,y=ry,py=tworx2*y,px=0;
  do {
      if (q[0]) commonSetPixel(buff+cx-x+(cy+y)*AG_WIDTH,r,g,b);
      if (q[1]) commonSetPixel(buff+cx+x+(cy+y)*AG_WIDTH,r,g,b);
      if (q[2]) commonSetPixel(buff+cx-x+(cy-y)*AG_WIDTH,r,g,b);
      if (q[3]) commonSetPixel(buff+cx+x+(cy-y)*AG_WIDTH,r,g,b);
    x++;
    px+=twory2;
    if(p<0) {
      p+=ry2+px;
    }
    else {
      y--;
      py-=tworx2;
      p+=ry2+px-py;
    }
  } while(px<py);
  p=ry2*x*(x+1)+(ry2>>2)+rx2*(y-1)*(y-1)-rx2*ry2;
  do {
    if (q[0]) commonSetPixel(buff+cx-x+(cy+y)*AG_WIDTH,r,g,b);
    if (q[1]) commonSetPixel(buff+cx+x+(cy+y)*AG_WIDTH,r,g,b);
    if (q[2]) commonSetPixel(buff+cx-x+(cy-y)*AG_WIDTH,r,g,b);
    if (q[3]) commonSetPixel(buff+cx+x+(cy-y)*AG_WIDTH,r,g,b);
    y--;
    py-=tworx2;
    if(p>0) {
      p+=rx2-py;
    }
    else {
      x++;
      px+=twory2;
      p+=rx2-py+px;
    }
  } while(y>=0);
}

//Cubic Bezier curve (4 control points)
//Input p[],cx,cy: signed 16.16-fixed
void agBezierCurve(unsigned  short* buff,int* p,int cx,int cy,
                   unsigned char r,unsigned char g,unsigned char b) {
  int qxOld,qyOld,qx=p[0],qy=p[1],
      dqx=(3*(p[2]-p[0]))>>4,
      dqy=(3*(p[3]-p[1]))>>4,
      ddqx=(6*(p[0]+p[4]-(p[2]<<1)))>>8,
      ddqy=(6*(p[1]+p[5]-(p[3]<<1)))>>8,
      dddqx=(((6*(p[6]-p[0]+3*(p[2]-p[4])))>>9)*71/*Correction factor*/)>>9,
      dddqy=(((6*(p[7]-p[1]+3*(p[3]-p[5])))>>9)*71/*Correction factor*/)>>9;
  for(int i=0;i<16;i++) {
    qxOld=qx;
    qyOld=qy;
    qx+=dqx;
    qy+=dqy;
    dqx+=ddqx;
    dqy+=ddqy;
    ddqx+=dddqx;
    ddqy+=dddqy;
    agLine(buff,qxOld+cx,qyOld+cy,qx+cx,qy+cy,r,g,b);
  }
//  for(int i=0;i<8;i+=2) //Control points
//    commonSetPixel(buff+((p[i]+cx)>>16)+((p[i+1]+cy)>>16)*AG_WIDTH,
//                   255,255,255);
}

//Connects 2 points in a way defined by curveForm
//Values for curveForm:
//-1: line
//[1..2]: quarter-ellipse or half-circle (in the case x0==x1||y0==y1)
//[0;3..9]: cubic Bezier curves with various control points
//Input x0,y0,x1,y1: signed 16.16-fixed
void agCurve(unsigned short* buff,int x0,int y0,int x1,int y1,
             char curveForm,bool flipBowlShape,
             unsigned char r,unsigned char g,unsigned char b) {
  if (x0==x1&&y0==y1) {
    agCircle(buff,x0,y0,_agSize<<13,r,g,b); //Dot
    return;
  }
  if (curveForm<=-1) { //Line
    agLine(buff,x0,y0,x1,y1,r,g,b);
  }
  else {
    int dq;
    if (curveForm<=9) {
      int cx=(x0+x1)>>1,cy=(y0+y1)>>1,dx=(x1-x0)>>1,dy=(y1-y0)>>1,s,t,u,v,p[8];
      dq=(mmAbs(dx)+mmAbs(dy))>>1;
      int dq2=(dy>0? dq : -dq);
      dq=(dx>0? dq : -dq);
      p[0]=-dx;
      p[1]=-dy;
      p[6]=dx;
      p[7]=dy;
      s=t=u=v=0;
      if (curveForm<=0) {
        t=v=dy;
      }
      else if (curveForm<=2) { //Elliptic arc
        unsigned char numQuarters=1;
        bool orientation=curveForm&1;
        if (x0==x1) {
          if (y0>y1) { int tmp=y0; y0=y1; y1=tmp; } //For GM1 compatibility
          numQuarters=2;
          dq=(y1-y0)>>1;
          dq=(orientation? -dq : dq);
          orientation=true;
          x1+=dq;
        }
        else if (y0==y1) {
          if (x0>x1&&!flipBowlShape) { //For GM1 compatibility
            int tmp=x0; x0=x1; x1=tmp;
          }
          if (!flipBowlShape) { //For GM1 compatibility
            int tmp=x0; x0=x1; x1=tmp;
          }
          if (curveForm==2) {
            int tmp=x0; x0=x1; x1=tmp;
          }
          numQuarters=2;
          dq=(x1-x0)>>1;
//          dq=(orientation? dq : -dq); //Removed for GM1 compatibilty
          orientation=false;
          y1+=dq;
        }
        agEllipticArc(buff,x0,y0,x1,y1,numQuarters,orientation,r,g,b);
        return;
      }
      else {
        if (y0==y1) dq2=-dq2; //For GM1 compatibility
        if (curveForm<=3) { //Bezier curve
          s=dx; u=dq; v=dq2;
        }
        else if (curveForm<=4) {
          t=dy; u=dq; v=dq2;
        }
        else if (curveForm<=5) {
          s=dq; t=dq2; v=dy;
        }
        else if (curveForm<=6) {
          s=dq; t=dq2; u=dx;
        }
        else if (curveForm<=7) {
          s=u=dq; t=v=dq2;
        }
        else if (curveForm<=8) {
          if (mmAbs(dx)<mmAbs(dy)) { dq=-dq; dq2=-dq2; }
          s=u=dq; t=v=-dq2;
        }
        else {
          s=u=dx;
        }
      }
      p[2]=p[0]+s; p[3]=p[1]+t; p[4]=p[6]-u; p[5]=p[7]-v;
      agBezierCurve(buff,p,cx,cy,r,g,b);
//      //Control points
//      for(int i=0;i<8;i+=2) {
//        commonSetPixel(buff+((p[i]+cx)>>16)+((p[i+1]+cy)>>16)*AG_WIDTH,
//                       255,255,255);
//      }
    }
  }
}

//Closed cubic Hermite curve with n control points
//Input p[],cx,cy: signed 16.16-fixed
void agClosedCurve(unsigned short* buff,int* p,int n,int cx,int cy,
                   unsigned char r,unsigned char g,unsigned char b) {
  int qxOld,qyOld,qx,qy,dqx,dqy,ddqx,ddqy,dddqx,dddqy,m0x,m0y,m1x,m1y,i1;
  n<<=1;
  for(int i=0;i<n-1;i+=2) {
    i1=(i+2)%n;
//    agLine(buff,p[i]+cx,p[i+1]+cy,p[i1]+cx,p[i1+1]+cy,255,0,0); //Line sketch
    m0x=(p[i1]-p[(i-2+n)%n])>>1;
    m0y=(p[i1+1]-p[((i-2+n)%n)+1])>>1;
    m1x=(p[(i+4)%n]-p[i])>>1;
    m1y=(p[((i+4)%n)+1]-p[i+1])>>1;
    qx=p[i];
    qy=p[i+1];
    dqx=m0x>>4;
    dqy=m0y>>4;
    ddqx=(((6*(p[i1]-p[i])))-(((m0x<<1)+m1x)<<1))>>8;
    ddqy=(((6*(p[i1+1]-p[i+1])))-(((m0y<<1)+m1y)<<1))>>8;
    dddqx=(((12*qx-((3*p[i1])<<2)+6*(m0x+m1x))>>3)*71/*Correction*/)>>15;
    dddqy=(((12*qy-((3*p[i1+1])<<2)+6*(m0y+m1y))>>3)*71/*Correction*/)>>15;
    for(int j=0;j<16;j++) {
      qxOld=qx;
      qyOld=qy;
      qx+=dqx;
      qy+=dqy;
      dqx+=ddqx;
      dqy+=ddqy;
      ddqx+=dddqx;
      ddqy+=dddqy;
      //if (j==7) { qx=p[i1]<<16; qy=p[i1+1]<<16; }
      agLine(buff,qxOld+cx,qyOld+cy,qx+cx,qy+cy,r,g,b);
    }
  }
}

//Input p[]: signed 16.16-fixed
//Input w[]: unsigned 8.8-fixed
//Input u[]: unsigned 1.15-fixed (i.e. [0..32768])
//Array w[] contains half as many elements as p[] (which is (x,y))
//Array u[] contains (AG_NURBSDEGREE+1) more elements than w[]
void agNURBS(unsigned short* buff,unsigned int numControlPoints,int* p,
             unsigned short* w,unsigned short* u,int cx,int cy,
             unsigned char r,unsigned char g,unsigned char b) {
//TODO: Optimize algorithm, but be careful; it's lying on a razor's edge
#define AG_NURBSDEGREE 2 //Equal to (order-1), so for cubic, degree is 2
#define AG_NURBSLASTKNOTINDEX (numControlPoints+AG_NURBSDEGREE)
  unsigned int bt[256];
  int a,c,rx=cx,ry=cy,rxOld,ryOld;
  for(unsigned short t=u[AG_NURBSDEGREE];t<=u[numControlPoints];t+=256) {
    //B-spline
    //Initial rectangular function for polynomial basis
    for(int i=0;i<AG_NURBSLASTKNOTINDEX;i++) {
      //This should not work only as (t<=..), but does! Should be (t<..)?
      bt[i]=((t>=u[i])&&(t<=u[i+1]))<<16; //Unsigned 1.16-fixed
    }
    //Linear and beyond
    for(int k=1;k<=AG_NURBSDEGREE;k++) {
      for(int i=0;i<AG_NURBSLASTKNOTINDEX-k;i++) {
        a=u[i+k]-u[i];
        c=u[i+k+1]-u[i+1];
        bt[i]=(a? ((t-u[i])*bt[i])/a : 0)+(c? ((u[i+k+1]-t)*bt[i+1])/c : 0);
      }
    }
    rxOld=rx; ryOld=ry; rx=ry=c=0;
    //Normalization (ratio)
    for(int i=0;i<numControlPoints;i++) {
      c+=(a=((w[i]*(bt[i]>>3))>>4)); //Unsigned 10.17-fixed
      rx+=(a>>4)*(p[i<<1]>>8); //Unsigned 10.18-fixed
      ry+=(a>>4)*(p[(i<<1)+1]>>8); //Unsigned 10.18-fixed
    }
    c>>=6; //Unsigned 10.10-fixed
    rx=(c? ((rx/c)<<6)+cx : cx); //Unsigned 10.16-fixed
    ry=(c? ((ry/c)<<6)+cy : cy); //Unsigned 10.16-fixed
    if (t>u[AG_NURBSDEGREE]) agLine(buff,rxOld,ryOld,rx,ry,r,g,b);
  }
#undef AG_NURBSDEGREE
#undef AG_NURBSLASTKNOTINDEX
}

//Fills color to a border of same color
void agPaintFill(unsigned short* buff,int x,int y,
                 unsigned char r,unsigned char g,unsigned char b) {
  unsigned short *ptr=buff+y*AG_WIDTH+x;
  if (*ptr==commonPackRGB(r,g,b)) return;
  agSetPixel(ptr,r,g,b);
  if (x>0) agPaintFill(buff,x-1,y,r,g,b);
  if (x<AG_WIDTH-1) agPaintFill(buff,x+1,y,r,g,b);
  if (y>0) agPaintFill(buff,x,y-1,r,g,b);
  if (y<AG_HEIGHT-1) agPaintFill(buff,x,y+1,r,g,b);
}

//Values for elementSizeInBytes are 1,2,4
void agWaveform(unsigned short* buff,void* data,unsigned int size,
                unsigned char elementSizeInBytes) {
  int x=0,y=240<<16,xOld,yOld;
  const int dx=(AG_WIDTH<<16)/size;
  if (elementSizeInBytes<=1) {
    unsigned char *typedData=(unsigned char*)data;
    for(int i=0;i<size;i++) {
      xOld=x; yOld=y;
      x+=dx;
      y=(((*typedData*AG_HEIGHT)>>9)+240)<<16;
      agLine(buff,x,y,xOld,yOld,255,0,255);
      typedData++;
    }
  }
  else if (elementSizeInBytes<=2) {
    unsigned short *typedData=(unsigned short*)data;
    for(int i=0;i<size;i++) {
      xOld=x; yOld=y;
      x+=dx;
      y=(((*typedData*AG_HEIGHT)>>17)+240)<<16;
      agLine(buff,x,y,xOld,yOld,255,0,255);
      typedData++;
    }
  }
  else if (elementSizeInBytes<=4) {
    unsigned int *typedData=(unsigned int*)data;
    for(int i=0;i<size;i++) {
      xOld=x; yOld=y;
      x+=dx;
      y=(((*typedData*AG_HEIGHT)>>25)+240)<<16;
      agLine(buff,x,y,xOld,yOld,255,0,255);
      typedData++;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
//3D primitive
////////////////////////////////////////////////////////////////////////////////

void agZOctagon(unsigned short* buff,unsigned short* zInvBuff,
                int width,int height,int x,int y,unsigned short zInv,
                unsigned char r,unsigned char g,unsigned char b) {
  const int dZInvPtr=AG_WIDTH-width;
  const int dPtr=dZInvPtr;
  const int beginOffset=((y-(height>>1))*AG_WIDTH+(x-(width>>1)));
  unsigned short *zInvPtr=zInvBuff+beginOffset;
  unsigned short *ptr=buff+beginOffset;
  int xSkip=width*75, //Begins at width*(1/(2+sqrt(2)))
      dXSkip=-(width<<8)/height,
      xSkipTurnabout=-width*53-dXSkip; //At -width*(1/(2+2*sqrt(2)))-dXSkip
  for(int j=0;j<height;j++) {
    if (xSkip>0) {
      ptr+=(xSkip>>8);
      zInvPtr+=(xSkip>>8);
    }
    for(int i=0;i<width-(xSkip<0? 0 : (xSkip>>8)<<1);i++) {
      if (zInv>*zInvPtr) {
        *zInvPtr=zInv;
        agSetPixel(ptr,r,g,b);
      }
      zInvPtr++;
      ptr++;
    }
    if (xSkip>0) {
      ptr+=(xSkip>>8);
      zInvPtr+=(xSkip>>8);
    }
    ptr+=dPtr;
    zInvPtr+=dZInvPtr;
    if (xSkip<xSkipTurnabout) { dXSkip=-dXSkip; }
    xSkip+=dXSkip;
  }
}

void agZRectangle(unsigned short* buff,unsigned short* zInvBuff,
                  int width,int height,int x,int y,unsigned short zInv,
                  unsigned char r,unsigned char g,unsigned char b) {
  const int dZInvPtr=AG_WIDTH-width;
  const int dPtr=dZInvPtr;
  const int beginOffset=((y-(height>>1))*AG_WIDTH+(x-(width>>1)));
  unsigned short *zInvPtr=zInvBuff+beginOffset;
  unsigned short *ptr=buff+beginOffset;
  for(int j=0;j<height;j++) {
    for(int i=0;i<width;i++) {
      if (zInv>*zInvPtr) {
        *zInvPtr=zInv;
        agSetPixel(ptr,r,g,b);
      }
      zInvPtr++;
      ptr++;
    }
    ptr+=dPtr;
    zInvPtr+=dZInvPtr;
  }
}

void agZSquare(unsigned short* buff,unsigned short* zInvBuff,
               int width,int x,int y,unsigned short zInv,
               unsigned char r,unsigned char g,unsigned char b) {
  const int dZInvPtr=AG_WIDTH-width;
  const int dPtr=dZInvPtr;
  const int beginOffset=((y-(width>>1))*AG_WIDTH+(x-(width>>1)));
  unsigned short *zInvPtr=zInvBuff+beginOffset;
  unsigned short *ptr=buff+beginOffset;
  for(int j=0;j<width;j++) {
    for(int i=0;i<width;i++) {
      if (zInv>*zInvPtr) {
        *zInvPtr=zInv;
        agSetPixel(ptr,r,g,b);
      }
      zInvPtr++;
      ptr++;
    }
    ptr+=dPtr;
    zInvPtr+=dZInvPtr;
  }
}

void agZPoint(unsigned short* buff,unsigned short* zInvBuff,
              int x,int y,unsigned short zInv,
              unsigned char r,unsigned char g,unsigned char b) {
  const int beginOffset=y*AG_WIDTH+x;
  unsigned short *ptr=buff+beginOffset;
  if (zInv>zInvBuff[beginOffset]) {
    agSetPixel(ptr,r,g,b);
  }
}

void agZRaytraceSphere(unsigned short* buff,unsigned short* zInvBuff,
                       const int posObj[3],const int radius,const int posLight[3],
                       unsigned short (*texture)(int,int,int,Noise*,
                       unsigned int),Noise* noise,unsigned int counter) {
  //TODO
}

void agRaytraceSphere(unsigned short* buff,const int pos[3],
                      const int radius,const int posLight[3],
                      unsigned short (*texture)
                      (int,int,int,Noise*,unsigned int),Noise* noise,
                      const unsigned int counter) {
  const int xBoundingTerm=(pos[0]<0? -radius : radius),
            yBoundingTerm=(pos[1]<0? -radius : radius);
  int xScreen0=(320*(pos[0]-(radius+(radius>>1))))/
               (pos[2]+xBoundingTerm),
      yScreen0=(320*(pos[1]-(radius+(radius>>1))))/
               (pos[2]+yBoundingTerm),
      xScreen1=(320*(pos[0]+(radius+(radius>>1))))/
               (pos[2]-xBoundingTerm),
      yScreen1=(320*(pos[1]+(radius+(radius>>1))))/
               (pos[2]-yBoundingTerm);
  //Clip bounding rectangle
  xScreen0=(xScreen0<1-AG_HALFWIDTH? 1-AG_HALFWIDTH :
	          xScreen0>AG_HALFWIDTH-1? AG_HALFWIDTH-1 : xScreen0);
  xScreen1=(xScreen1<1-AG_HALFWIDTH? 1-AG_HALFWIDTH :
	          xScreen1>AG_HALFWIDTH-1? AG_HALFWIDTH-1 : xScreen1);
  yScreen0=(yScreen0<1-AG_HALFHEIGHT? 1-AG_HALFHEIGHT :
	          yScreen0>AG_HALFHEIGHT-1? AG_HALFHEIGHT-1 : yScreen0);
  yScreen1=(yScreen1<1-AG_HALFHEIGHT? 1-AG_HALFHEIGHT :
	          yScreen1>AG_HALFHEIGHT-1? AG_HALFHEIGHT-1 : yScreen1);
  const int xPos=pos[0]>>5,yPos=pos[1]>>5,zPos=pos[2]>>5,rad=radius>>5;
  //Raytrace
  for(int j=yScreen0;j<=yScreen1;j++) {
    for(int i=xScreen0;i<=xScreen1;i++) {
      const int lengthInv=0x20000000/mmSqrRt((i*i+j*j+320*320)<<10), //Signed .(8)16-fixed
                vx=(i*lengthInv)>>9, //Signed 2.15-fixed
                vy=(j*lengthInv)>>9, //Signed 2.15-fixed
                vz=(320*lengthInv)>>9, //Signed 2.15-fixed
                beta=(vx*xPos+vy*yPos+vz*zPos)>>15, //Signed 5.26-fixed
                param=beta*beta-(xPos*xPos+yPos*yPos+zPos*zPos-rad*rad); //Signed 9.22-fixed
      if (param>=0) {
        const int t=(beta-mmSqrRt(param))>>2, //Signed 11.9-fixed
                  xIntersect=t*(vx>>5), //Signed 11.19-fixed
                  yIntersect=t*(vy>>5),
                  zIntersect=t*(vz>>5),
                  nx=xIntersect-(pos[0]<<3), //Signed 11.19-fixed
                  ny=yIntersect-(pos[1]<<3),
                  nz=zIntersect-(pos[2]<<3),
                  dxLight=(posLight[0]<<3)-xIntersect, //Signed 11.19-fixed
                  dyLight=(posLight[1]<<3)-yIntersect,
                  dzLight=(posLight[2]<<3)-zIntersect;
        int diffuse=(((dxLight>>12)*(dxLight>>12)+
                      (dyLight>>12)*(dyLight>>12)+
                      (dzLight>>12)*(dzLight>>12))>>6); //Signed X.8-fixed
        //diffuse+=mmSqrRt(diffuse);
        diffuse=((nx>>10)*(dxLight>>11)+
                 (ny>>10)*(dyLight>>11)+
                 (nz>>10)*(dzLight>>11))/
                 (1+diffuse)+ //Signed 11.18-fixed numerator
                 60; //Wrap-lighting term
        if (diffuse<0) diffuse=0;
        const int specular0=diffuse*diffuse,
                  specular1=specular0*diffuse,
                  specular2=specular1*diffuse;
        unsigned short color=texture(nx,ny,nz,noise,counter);
        int r=(((128+diffuse)*commonGetRed(&color))>>10)+
              (specular0>>9)+(specular1>>15)+(specular2>>21);
        r=(r>255? 255 : r);
        int g=(((128+diffuse)*commonGetGreen(&color))>>10)+
              (specular0>>9)+(specular1>>15)+(specular2>>21);
        g=(g>255? 255 : g);
        int b=(((128+diffuse)*commonGetBlue(&color))>>10)+
              (specular0>>9)+(specular1>>15)+(specular2>>21);
        b=(b>255? 255 : b);
        agSetPixel(buff+AG_WIDTH*(240+j)+(320+i),r,g,b);
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
//Procedural texture fill
////////////////////////////////////////////////////////////////////////////////

void agNoiseFill(unsigned short* buff,Noise* noise,int width,int height) {
  for(int y=0;y<height;y++) {
    for(int x=0;x<width;x++) {
      agSetPixel(buff+(y*width+x),
                 (noise->get2D(x<<3,y<<3,0xff,0xff)>>6)+128,0,0);
    }
  }
}

void agFullScreenStaticFill(unsigned short* buff,unsigned char numStates) {
  unsigned char a,shift=8-mmCeilingLog2(numStates);
  for(int y=0;y<AG_HEIGHT;y++) {
    for(int x=0;x<AG_WIDTH;x++) {
      a=(commonRandomUchar()%numStates)<<shift;
      agSetPixel(buff+y*AG_WIDTH+x,a,a,a);
    }
  }
}

void agFullScreenXORFill(unsigned short* buff){
  for(int y=0;y<AG_HEIGHT;y++) {
    for(int x=0;x<AG_WIDTH;x++) {
      unsigned char c=x^y; //Must be 8 bits
      agSetPixel(buff+y*AG_WIDTH+x,c,0,c);
    }
  }
}

//cutWood: 0=noCut 1=planks 2=smallPlanks 3=threeRings
void agFullScreenWoodFill(unsigned short* buff,Noise* noise,
                          unsigned char cutWood) {
  unsigned char a;
  for(int y=0;y<AG_HEIGHT;y++) {
    for(int x=0;x<AG_WIDTH;x++) {
      const int xx=(cutWood==2? x<<1 : x),
                yy=(cutWood==2? y<<1 : y);
      a=((256<<3)+noise->get2D(xx<<4,yy,0xff,0xff));
      a=(a+commonPermute256[(commonPermute256[xx&0xff]^(yy>>1))&0xff])>>3;
      a-=a>>2;
      unsigned int q=xx;
      //Gaps between boards
      if (cutWood>=3) {
        const int v[]={xx-AG_HALFWIDTH,yy-AG_HALFHEIGHT};
        q=(mmApproxNorm2Vec2(v)+0x2)<<1;
      }
      if (cutWood>=2&&(cutWood!=3||q<=390))
        a=(a>>(mmAbs(q+7-((q+7)&0xf80))<=14))*(mmAbs(q+3-((q+3)&0xf80))>6);
      agSetPixel(buff,83+a,61+(a>>1),58+(a>>3));
      buff++;
    }
  }
}

//Call this function repeatedly to fill in gaps between pebbles
void agFullScreenPebbleFill(unsigned short* buff,Noise* noise) {
  int a,
      b=commonRandomUshort(),
      c=commonRandomUshort(),
      d=b&0xf;
  for(int y=0;y<AG_HEIGHT;y++) {
    for(int x=0;x<AG_WIDTH;x++) {
      a=mmAbs(noise->get2D((x<<3)+b,(y<<3)+c,0xff,0xff));
      a=(a>940? 940 : a);
      a=(a*a)>>9;
      a=(a*a)>>9;
      a=(a*a)>>12;
      a=(a*a)>>18;
      a-=32;
      a=30+((a<0? 0 : (a>255? 255 : a))>>1);
      if (a>30) {
        a+=noise->getTurbulence2D((x<<4)+b,(y<<4)+c,0xff,0xff,3)>>5;
        agSetPixel(buff,a-d,a-(d<<1),a-(d<<1));
      }
      buff++;
    }
  }
}

//2D cyclical cellular automata
//Should first call agFullScreenStaticFill() to initialize
//Choosing Moore gives a square neighborhood; von Neumann gives a diamond shape
//Uses only one buffer by operating on the red and blue channels, which leaves
//small fragments of red and blue around edges, a negligible (possibly
//desirable) artifact
//Examples (radius,threshold,numStates,vonNeumannNotMoore):
//spirals (1,3,3,0)
//lavaLamp (2,10,3,0)
//amoeba (3,10,2,1)
//blackVersusWhite (5,23,2,1)
//squarishSpirals (2,2,6,1)
//imperfect (1,2,4,0)
//perfect (1,3,4,0)
//cyclicalSpirals (3,5,8,0)
//cyclicalCellularAutomata (1,1,14,1)
void agFullScreenCellularAutomataFill(unsigned short* buff,
                                      unsigned char radius,
                                      unsigned char threshold,
                                      unsigned char numStates,
                                      bool vonNeumannNotMoore,bool buffIndex) {
  unsigned char shift;
  int a,b,c,hxBound;
  unsigned short *buffTmp;
  unsigned char (*func)(unsigned short* pixel)=(buffIndex? commonGetRed :
                                                           commonGetBlue);
  radius<<=1;
  shift=8-mmCeilingLog2(numStates);
  for(int j=0;j<AG_HEIGHT;j+=2) {
    for(int i=0;i<AG_WIDTH;i+=2) {
      a=0;
      buffTmp=buff+j*AG_WIDTH+i;
      b=func(buffTmp)>>shift;
      c=((b+1)%numStates)<<shift;
      for(int hy=-radius;hy<=radius;hy+=2) {
        hxBound=(vonNeumannNotMoore? radius-mmAbs(hy) : radius);
        for(int hx=-hxBound;hx<=hxBound;hx+=2) {
          a+=(c==func(buff+((j+hy+AG_HEIGHT)%AG_HEIGHT)*AG_WIDTH+
                            (i+hx+AG_WIDTH)%AG_WIDTH));
        }
      }
      b=((b+(a>=threshold))%numStates)<<shift;
      a=buffIndex? commonGetRed(buffTmp) : b;
      b=buffIndex? b : commonGetBlue(buffTmp);
      agSetPixel(buffTmp,a,0,b);
      agSetPixel(buffTmp+1,a,0,b);
      agSetPixel(buffTmp+AG_WIDTH,a,0,b);
      agSetPixel(buffTmp+AG_WIDTH+1,a,0,b);
    }
  }
}

void agFullScreenSoap(unsigned short* buff,Noise* noise,unsigned int counter) {
  int yo,q;
  buff+=AG_NUMPIXELS;
  for(int y=0;y<AG_HEIGHT;y++) {
    yo=(y&0xffffffc0)+64;
    for(int x=0;x<AG_WIDTH;x++) {
      buff--;
      q=(30<<6)+(mmSin((yo<<4)+((y-yo)*mmCos(((((163*(64+y-yo)*(80+y))>>10)*
        noise->getTurbulence2D(x<<2,(y-(counter<<1))<<2,0xf,0xff,4)
        )>>13))>>12))>>10);
      q=mmSqrRt(26*q*q+(
        ((y-AG_HALFHEIGHT)*(y-AG_HALFHEIGHT)+(x-AG_HALFWIDTH)*(x-AG_HALFWIDTH))
        <<5))>>1;
      //Mag of resultant wave = 2 * orig mag * cos(phase diff / 2)
      agSetPixel(buff,
                 mmAbs(mmShortSin(q+861))>>7,
                 mmAbs(mmShortSin(q+943))>>7,
                 mmAbs(mmShortSin(q))>>7);
    }
  }
}

void agFullScreenSpiralIllusion(unsigned short* buff,bool blur,
                                unsigned int counter) {
  int index,mag,dx,dy,ang,angOffset;
  unsigned int dist;
  for(int j=0;j<AG_HEIGHT;j++) {
    for(int i=0;i<AG_WIDTH;i++) {
      dx=i-320;
      dy=j-240;
      dist=mmSqrRt(dx*dx+dy*dy);
      ang=mmAtan(dy<<16,dx<<16)-counter;
      angOffset=mmApproxSqrRt(mmSqrRt(dist<<11)<<12)+counter;
      mag=((ang+angOffset)&0x40? 127 : 0);
      mag+=((ang-angOffset)&0x40? (dist>127? 127 : dist) : 0);
      index=(((angOffset+100)&0x40)>0);
      mag=(dist<3&&(mmAbs(dx)!=mmAbs(dy)||mmAbs(dy)!=2)?
             255 : //Hole
             (dist<7||dist==8||dist==10||dist==12||dist==15?
                0 : //Inner ring
                ((angOffset&0x3f)<7||(angOffset&0x3f)>(63-7))&&dist>15?
                  ((ang-(angOffset<<3)+60)&0x40? //Stripe
                     255*index :
                     255*!index) :
                mag));
      agSetPixel(buff+AG_WIDTH*j+i,mag,mag,mag);
    }
  }
  if (blur) agFullScreenBlur(buff);
}

////////////////////////////////////////////////////////////////////////////////
//Procedural heightfield sample
////////////////////////////////////////////////////////////////////////////////

//Input: (x,y)
//Output: signed 13-int
int agCloud(Noise* noise,int x,int y,int offset) {
  int a=noise->get2D(x+(offset<<2),y,0x7,0xff);
  return (a<0? -a : a)+
         ((mmSin((x+(offset<<2))<<3)>>7)*(mmSin((y-(offset<<2))<<3)>>8)>>10);
//  //Alternative algorithm for generating clouds (unsigned 7-int)
//  int a=noise->getTurbulence2D(x,y,0xff,0xff,5)<<5;
//  a=(a<0? 0 : a);
//  return a=128-(128<<16)/mmExp2(a);
}

//Input: (x<<10,y<<10)
//Output: signed 23-int
int agSea(bool noDetail,Noise* noise,int x,int y,unsigned int counter) {
  return (noDetail? 0 : (mmSin(counter<<3)<<5)+
                        (mmSin((x>>6)-(counter<<6))>>7)*(mmSin(y>>7)>>8)+
                        (mmSin((x>>8)-(counter<<5))>>6)*(mmSin(y>>10)>>6))-
          mmAbs(noise->get2D((x>>8)-(counter<<4),y>>9,0xff,0xff)<<9);

}

//Input: (x<<10,y<<10)
//Output: signed 12-int
int agHill(Noise* noise,int x,int y) {
  return (noise->get2D(x>>12,y>>12,0xff,0xff)<<15)-(x<<2);
}

//Input: (x<<10,y<<10)
//Output: signed 12-int
//Output function is centered at midheight of hills
int agLagoon(bool noDetail,Noise* noise0,Noise* noise1,int x,int y,
             unsigned int counter,bool* isLand,int* tmp) {
  //Land height
  *isLand=((*tmp=agHill(noise0,x,y)/*-((s-1)>>1)*(2000<<16)*/)>0);
          //((x>>5)*(x>>5)+(y>>5)*(y>>5)<0x2000000);
  return (*isLand? *tmp : agSea(noDetail,noise1,x,y,counter)-(16<<16));
}

////////////////////////////////////////////////////////////////////////////////
//Text
////////////////////////////////////////////////////////////////////////////////

void agTextFont(const char* szFontData) {
  for(unsigned int i=0;i<128;i++) //Preclear to 0xffff
    memset(_agTextCharacterCurveData[i],0xffff,24*sizeof(unsigned short));
  unsigned char characterIndex,curveIndex,n,numSubsegments;
  char *c,text[MAX_STR],szCharacterIndex[MAX_STR];
  unsigned int off=0;
  while(sscanf(szFontData+off,"%s %s ",szCharacterIndex,text)==2) {
    off+=strlen(szCharacterIndex)+strlen(text)+2;
    characterIndex=strtoul(szCharacterIndex,NULL,10);
    curveIndex=0;
    numSubsegments=0;
    c=text;
    while(*c!='\0') {
      c++;
      if (*c=='\0') break;
      if (isdigit(*c)) {
        _agTextCharacterCurveData[characterIndex][curveIndex]=
          tolower(*(c-1))-'a';
        n=*c-'0';
        c++;
        if (isdigit(*c)) {
          n=10*n+(*c-'0');
          c++;
        }
        _agTextCharacterCurveData[characterIndex][curveIndex]|=
          (n<<10)|(tolower(*c)-'a')<<5;
        numSubsegments++;
      }
      else if (*c==':') {
        if (!numSubsegments) {
          _agTextCharacterCurveData[characterIndex][curveIndex]=
            ((tolower(*(c-1))-'a')<<5)|(tolower(*(c-1))-'a');
        }
        else {
          curveIndex--;
        }
        numSubsegments=0;
        c++;
      }
      else {
        _agTextCharacterCurveData[characterIndex][curveIndex]=
          (0xf<<10)|((tolower(*c)-'a')<<5)|(tolower(*(c-1))-'a');
        numSubsegments++;
      }
      curveIndex++;
    }
  }
}

void agTextGrapheme(unsigned short* buff,char c,
                    unsigned char r,unsigned char g,unsigned char b) {
  _agTextGuidePoint[0]=65536;
  _agTextGuidePoint[1]=0;
  _agTextGuidePoint[2]=65536;
  _agTextGuidePoint[3]=65536;
  _agTextGuidePoint[4]=0;
  _agTextGuidePoint[5]=65536;
  _agTextGuidePoint[6]=-65536;
  _agTextGuidePoint[7]=65536;
  _agTextGuidePoint[8]=-65536;
  _agTextGuidePoint[9]=65536;
  _agTextGuidePoint[0]=-65536;
  _agTextGuidePoint[11]=-65536;
  _agTextGuidePoint[12]=0;
  _agTextGuidePoint[13]=-65536;
  _agTextGuidePoint[14]=65536;
  _agTextGuidePoint[15]=-65536;
  _agTextGuidePoint[16]=65536;
  _agTextGuidePoint[17]=0;
  _agTextGuideWeight[0]=256;
  _agTextGuideWeight[1]=181;
  _agTextGuideWeight[2]=256;
  _agTextGuideWeight[3]=181;
  _agTextGuideWeight[4]=256;
  _agTextGuideWeight[5]=181;
  _agTextGuideWeight[6]=256;
  _agTextGuideWeight[7]=181;
  _agTextGuideWeight[8]=256;
  _agTextGuideKnot[0]=0;
  _agTextGuideKnot[1]=0;
  _agTextGuideKnot[2]=0;
  _agTextGuideKnot[3]=8192;
  _agTextGuideKnot[4]=8192;
  _agTextGuideKnot[5]=16384;
  _agTextGuideKnot[6]=16384;
  _agTextGuideKnot[7]=24576;
  _agTextGuideKnot[8]=24576;
  _agTextGuideKnot[9]=32768;
  _agTextGuideKnot[10]=32768;
  _agTextGuideKnot[11]=32768;
  if (_agTextY>=60+_agTextOffsetY) {
    unsigned char i0,i1;
    int curveForm;
    unsigned short data;
    unsigned int gridSpace=_agSize<<16;
    unsigned char q=(_agSize*_agThickness)>>10;
    for(int k=-q;k<q;k++) {
      for(int j=0;j<4;j++) {
        for(int i=0;i<24&&(data=_agTextCharacterCurveData[(int)c][i])!=0xffff;i++) {
          i0=data&0x1f;
          i1=(data>>5)&0x1f;
          curveForm=(data>>10)&0xf;
          if (curveForm==0xf) curveForm=-1;
          agCurve(buff,(i0%5)*gridSpace+((_agTextX+k+(j&1))<<16),
                       -(i0/5)*gridSpace+((_agTextY+k+((j>>1)&1))<<16),
                       (i1%5)*gridSpace+((_agTextX+k+(j&1))<<16),
                       -(i1/5)*gridSpace+((_agTextY+k+((j>>1)&1))<<16),
                       curveForm,
                       ((i0%5>=2)&&(i1%5>=2))||(i1-i0==1&&(i1%5)==2),
                       r,g,b);
        }
      }
    }
    _agTextX+=_agTextOffsetX;
    if (_agTextX>AG_WIDTH-128) {
      _agTextX=80;
      _agTextY-=_agTextOffsetY;
    }
  }
}

void agText(unsigned short* buff,const char* text,
             unsigned char r,unsigned char g,unsigned char b) {
  for(;*text!='\0';text++) agTextGrapheme(buff,*text,r,g,b);
}

void agTextSetPosition(int x,int y) { _agTextX=x; _agTextY=y; }

//void agTextWord(unsigned short* buff,char* text,
//                void (guideFunc)(int u,int* xRet,int* yRet),
//                unsigned char (widthFunc)(int u),
//                bool (boundaryFunc)(int,int)) {
//}

////////////////////////////////////////////////////////////////////////////////
//Special effect
////////////////////////////////////////////////////////////////////////////////

//Combines with preexisting image
//Uses 1 buffer
void agFullScreenWind(unsigned short* buff,Noise* noise,unsigned int counter) {
  unsigned char rgb[3];
  unsigned short *pixel=buff;
  int q;
  for(int y=0;y<AG_HEIGHT;y++) {
    for(int x=0;x<AG_WIDTH;x++) {
      commonGetRGB(rgb,pixel);
      q=7*(128+(noise->get2D((x+(counter<<3))<<1,y<<3,0xff,0xff)>>6));
      rgb[0]=(rgb[0]+q)>>3;
      rgb[1]=(rgb[1]+q)>>3;
      rgb[2]=(rgb[2]+q)>>3;
      agSetPixel(pixel,rgb[0],rgb[1],rgb[2]);
      pixel++;
    }
  }
}

//Interlaced to save space
//Uses 2.5 buffers
void agFullScreenWater(unsigned short* buff,unsigned short* bkgImage,
                       unsigned char* zInvBuff,bool produceDroplets,
                       unsigned int counter) {
  int a,b,c,d,e,f,g,h;
  unsigned char rgb[3];
  //Droplet
  if (produceDroplets) {
    a=(commonRandomUchar()&0x1f)+32, //Droplet diameter
    //Droplet placement
    b=(commonRandomUshort()%(AG_WIDTH-a-4))+2,
    c=(commonRandomUshort()%(AG_HEIGHT-a-4))+2;
    for (int j=0;j<=a;j+=2) {
      for (int i=0;i<=a;i+=2) {
        if ((i-(a>>1))*(i-(a>>1))+(j-(a>>1))*(j-(a>>1))<((a*a)>>2))
          zInvBuff[(c+j)*AG_WIDTH+b+i]=128+(commonRandomUchar()>>3);
      }
    }
  }
  //Water simulation
  buff+=(AG_WIDTH-2)+(counter&1);
  zInvBuff+=(AG_WIDTH-2)+(counter&1);
  unsigned char *buff2=zInvBuff+1-((counter&1)<<1);
  for(int y=2;y<=(AG_HEIGHT-4);y+=2) {
    buff+=4+AG_WIDTH;
    zInvBuff+=4+AG_WIDTH;
    buff2+=4+AG_WIDTH;
    for(int x=2+(counter&1);x<=(AG_WIDTH-3);x+=2) {
      *zInvBuff=((*(buff2-2)+
                  *(buff2+2)+
                  *(buff2-(AG_WIDTH<<1))+
                  *(buff2+(AG_WIDTH<<1)))>>1)-*zInvBuff;
      *zInvBuff-=(*zInvBuff-128)>>5; //Dampening
      //Image sampling offset for upper pixel
      a=*zInvBuff+*(zInvBuff+1)+(commonRandomUchar()&0x7); //Random wiggle term
      b=*(zInvBuff+2)+*(zInvBuff+3);
      c=*(zInvBuff+(AG_WIDTH<<1))+*(zInvBuff+(AG_WIDTH<<1)+1);
      d=b-a;
      e=c-a;
      f=x-d;
      f=(f<0? 0 : f>(AG_WIDTH-1)? (AG_WIDTH-1) : f);
      g=y-e;
      g=(g<0? 0 : g>(AG_HEIGHT-1)? (AG_HEIGHT-1) : g);
      //Additive lighting for upper pixel
      commonGetRGB(rgb,bkgImage+g*AG_WIDTH+f);
      h=(d<0? 0 : d<<3)/*+(e<0? 0 : e<<3)*/;
      a=rgb[0]+h;
      b=rgb[1]+h;
      c=rgb[2]+h;
      a=(a>255? 255 : a);
      b=(b>255? 255 : b);
      c=(c>255? 255 : c);
      agSetPixel(buff,a,b,c);
      //Image sampling offset for lower pixel
      g=y+1-e;
      g=(g<0? 0 : g>(AG_HEIGHT-1)? (AG_HEIGHT-1) : g);
      //Additive lighting for lower pixel
      commonGetRGB(rgb,bkgImage+g*AG_WIDTH+f);
      a=rgb[0]+h;
      b=rgb[1]+h;
      c=rgb[2]+h;
      a=(a>255? 255 : a);
      b=(b>255? 255 : b);
      c=(c>255? 255 : c);
      agSetPixel(buff+AG_WIDTH,a,b,c);
      buff+=2;
      zInvBuff+=2;
      buff2+=2;
    }
  }
}

//Interlaced to save space
void agFullScreenWetGlass(unsigned short* buff,Noise* noise,
                          unsigned int counter,unsigned char thickness) {
  int h=0,hOld,dh;
  unsigned short *ptr;
  unsigned char rgb[3];
  const int dxOff=noise->get1D(counter<<2,0xff);
  int xOff=0;
  for(int y=2;y<AG_HEIGHT;y++) {
    for(int x=0;x<AG_WIDTH;x++) {
      hOld=h;
      h=noise->get2D((x<<4)+(xOff>>8),(y+(counter<<5))<<2,0xff,0xff);
      h=(h<0? 0 : h);
      dh=((h-hOld)*thickness)>>8;
      ptr=buff+(y-2+commonRandomBool())*AG_WIDTH+x;
      commonGetRGB(rgb,ptr+(dh>>2));
      ptr+=(AG_WIDTH<<1);
      dh=(dh>0? dh : 0);
      int r=rgb[0]+dh+(h>>8),g=rgb[1]+dh+(h>>8),b=rgb[2]+dh+(h>>8);
      commonSetPixel(ptr,r>255? 255 : (r<0? 0 : r),g>255? 255 : (g<0? 0 : g),b>255? 255 : (b<0? 0 : b));
    }
    xOff+=dxOff;
  }
}

void agFullScreenDistort(unsigned short* buff,Noise* noise) {
  unsigned char rgb0[3],rgb1[3];
  int xo,xf,xStep;
  unsigned short *ptr;
  for(int y=10;y<AG_HEIGHT-10;y++) {
    if (y&1) { xo=0; xf=AG_WIDTH; xStep=1; }
    else { xo=AG_WIDTH-1; xf=-1; xStep=-1; }
    for(int x=xo;x!=xf;x+=xStep) {
      const int xx=x<<2,yy=y<<2,
                h=noise->get2D(xx,yy,0xff,0xff)>>1,
                dhdx=(noise->get2D(xx+1,yy,0xff,0xff)>>1)-h,
                dhdy=(noise->get2D(xx,yy+1,0xff,0xff)>>1)-h;
      ptr=buff+y*AG_WIDTH+x;
      commonGetRGB(rgb0,ptr);
      commonGetRGB(rgb1,ptr+dhdy*AG_WIDTH+dhdx);
      commonSetPixel(ptr,(rgb0[0]+rgb1[0])>>1,(rgb0[1]+rgb1[1])>>1,(rgb0[2]+rgb1[2])>>1);
    }
  }
}

void agRockSketch(unsigned short* buff) {
  unsigned short *pixel;
  int height,d0,d1;
  bool ground,b;
  for(int i=0;i<AG_WIDTH-1;i++) {
    pixel=buff+i;
    //Special operations are performed for the ground, so mark it
    ground=*pixel;
    do {
      //Skip dark region
      while(!*pixel&&pixel<buff+AG_NUMPIXELS) {
        pixel+=AG_WIDTH;
      }
      //Calculation for lit region
      if (pixel<buff+AG_NUMPIXELS) {
        //Calculate derivative at beginning of lit scanline
        b=*(pixel+1);
        for(d0=0;(*(pixel+1+d0*AG_WIDTH)>0)==b;d0-=(b<<1)-1);
        if (b) d0++;
        d0=(d0>5? d0=5 : d0<-5? d0=-5 : d0);
//printf("%d ",d0);
//commonSetPixel(pixel,128+d0*4,0,0);
        //Calculate height of lit scanline
        for(height=0;*pixel&&pixel<buff+AG_NUMPIXELS;height++) {
          pixel+=AG_WIDTH;
        }
        pixel-=AG_WIDTH;
        //Calculate derivative at end of lit scanline
        b=*(pixel+1);
        for(d1=0;(*(pixel+1+d1*AG_WIDTH)>0)==b;d1+=(b<<1)-1);
        if (!b) d1--;
        d1=(d1>5? d1=5 : d1<-5? d1=-5 : d1);
//printf("%d ",d1);
//commonSetPixel(pixel,0,128+d1*4,0);
        //Special operations for ground
        //d0=(ground? (d1>5? 5 : (d1<-5? -5 : d1)) : d0);
        pixel+=AG_WIDTH;
        //Draw lit scanline
        pixel-=AG_WIDTH*height;
        for(int h=0;h<height;h++) {
          commonSetPixel(pixel,ground*100,128+((((height-h)*d0)+(h*d1))<<4)/
                         height,0);//height>>1,0);
          pixel+=AG_WIDTH;
        }
        ground=false;
      }
    } while(pixel<buff+AG_NUMPIXELS);
  }
//Sleep(1000);
}

//Uses 1 buffer
void agFullScreenOuterSpace(unsigned short* buff,unsigned int counter) {
}

void agFlower(unsigned short* buff,int x,int y,int radius,int angle,
              unsigned char numPetals,unsigned char r,unsigned char g,
              unsigned char b,bool fill) {
}

void agButterfly(unsigned short* buff,int x,int y,int size,int angle,
                 unsigned char wingPosition,unsigned char r,unsigned char g,
                 unsigned char b,bool fill) {
}

//Combine with preexisting image
void agFullScreenSnow(unsigned short* buff,unsigned int counter) {
}

//Combine with preexisting image
void agFullScreenRain(unsigned short* buff,unsigned int counter) {
}

//Combine with preexisting image
void agFullScreenDripBlood(unsigned short* buff) {
}

////////////////////////////////////////////////////////////////////////////////
//Non-colorspace operation
////////////////////////////////////////////////////////////////////////////////

//Smooth values in 16-bit buffer of size AG_NUMPIXELS
void agFullScreenSmooth(unsigned short* buff,unsigned short* src) {
  //TODO: This type of smoothing smears; although it makes practically no difference, might replace with separable Gaussian smoothing
  src+=AG_WIDTH+1;
  buff+=AG_WIDTH+1;
  for(unsigned int j=1; j<AG_HEIGHT-1; j++) {
    for(unsigned int i=1; i<AG_WIDTH-1; i++) {
      *buff=(*src +
             ((src[-AG_WIDTH]+src[AG_WIDTH]+src[-1]+src[1])>>1)+
             ((src[-1-AG_WIDTH]+src[1-AG_WIDTH]+src[AG_WIDTH-1]+src[AG_WIDTH + 1])>>2))>>2;
      src++;
      buff++;
    }
    src+=2;
    buff+=2;
  }
}

//Scale values in 16-bit buffer of size AG_NUMPIXELS
//Variable mul is a multiplier, and rsh is a right-shift applied afterward
void agFullScreenScale(unsigned short* buff,int mul,int rsh) {
    for(unsigned int i=0; i<AG_NUMPIXELS; i++)
      buff[i]=(mul*buff[i])>>rsh;
}

//Recenters all the values in a 16-bit buffer of size
//AG_NUMPIXELS at USHRT_MAX/2
void agFullScreenCenterValue(unsigned short* buff) {
  unsigned short min=USHRT_MAX,max=0;
  for(unsigned int i=0; i<AG_NUMPIXELS; i++) {
    min=mmMin(buff[i],min);
    max=mmMax(buff[i],max);
  }
  const unsigned short DC=(USHRT_MAX>>1)-(min>>1)-(max>>1); //Distributed rshift to prevent ushort overflow
  for(unsigned int i=0; i<AG_NUMPIXELS; i++)
    buff[i]+=DC;
}

//Distance transform of 1D array using squared distance
//Last two arguments must be dimensioned v[size],z[size + 1], but they do not
//contain meaningful data; they exist so arrays need not be allocated
//repeatedly
//Used by agDistanceTransform2D in x and y independently
void agDistanceTransform1D(unsigned short *buff,unsigned short *src,
                           unsigned int size,int* v,int* z) {
#define AG_DTINFINITY 0xffffu
  v[0]=0;
  z[0]=-((int)AG_DTINFINITY)<<11;
  z[1]=((int)AG_DTINFINITY)<<11;
  int s,k=0;
  for(unsigned int q=1; q<size; q++) {
    k++;
    do {
        k--;
        s=((q*q-v[k]*v[k]+src[q]-src[v[k]])<<10)/(q-v[k]);
    }
    while(s<=z[k]);
    k++;
    v[k]=q;
    z[k]=s;
    z[k+1]=((int)AG_DTINFINITY)<<11;
  }
  k=0;
  for(unsigned int q=0; q<size; q++)
  {
    while(z[k+1] < (int)(q<<11)) k++;
    buff[q]=(q-v[k])*(q-v[k])+src[v[k]];
	  buff[q]=mmMin(buff[q],AG_DTINFINITY);
  }
}

//Distance transform of image; each resultant value in the buffer represents
//distance to nearest black pixel in original image
void agFullScreenDistanceTransform(unsigned short* buff) {
  unsigned short d[AG_WIDTH],f[AG_WIDTH];
  int v[AG_WIDTH],z[AG_WIDTH + 1];
  //Transform along columns
  for(int x = 0; x < AG_WIDTH; x++) {
    for(int y = 0; y < AG_HEIGHT; y++) {
      f[y]=buff[y*AG_WIDTH+x];
    }
    agDistanceTransform1D(d,f,AG_HEIGHT,v,z);
    for(int y=0; y<AG_HEIGHT; y++)
    {
      buff[AG_WIDTH*y+x]=d[y];
    }
  }
  //Transform along rows
  for(int y = 0; y < AG_HEIGHT; y++) {
    agDistanceTransform1D(d,buff+y*AG_WIDTH,AG_WIDTH,v,z);
    memcpy(buff+y*AG_WIDTH,d,AG_WIDTH*sizeof(unsigned short));
  }
  for(int i=0; i<AG_NUMPIXELS; i++) {
    //At this point we have the squared distance, but let us take the SqrRt
    //since that is probably what is desired; to optimize, you might not
    commonSetPixel(buff,(unsigned char)mmSqrRt(mmSqrRt(*buff << 16)),0,0);
    buff++;
  }
#undef AG_DTINFINITY
}

//This is a special function that converts an image to a smooth heightfield
//Basically it applies two distance transforms to an image; one of those is
//then inverted and added to the other, which places hilltops onto the plateaus
//This function is used in image-based clearspacing
void agFullScreenExtendedDistanceTransform(unsigned short* buff,
                                           unsigned short* buff2) {
  unsigned short max = 0;
  agFullScreenBlit(buff2,buff);
  agFullScreenSwapTwoColor(buff);
  agFullScreenDistanceTransform(buff);
  agFullScreenScale(buff,1,1);
  agFullScreenDistanceTransform(buff2);
  agFullScreenScale(buff2,1,1);
  for(unsigned int i = 0; i < AG_NUMPIXELS; i++)
    max = mmMax(buff2[i],max);
  for(unsigned int i = 0; i < AG_NUMPIXELS; i++)
    buff[i] += max - buff2[i];
  agFullScreenSmooth(buff2,buff);
  agFullScreenSmooth(buff,buff2);
}
