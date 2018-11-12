////////////////////////////////////////////////////////////////////////////////
//Dan Nielsen
//Graphics-generating routines.
//Reference:
//Distance transform based on code by Pedro Felzenszwalb with U Chicago.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Noise.h"
#define AG_IMAGEDIRECTORY APP_IMAGEDIRECTORY
#define AG_TEXTDIRECTORY APP_TEXTDIRECTORY
#define AG_FONTDIRECTORY APP_FONTDIRECTORY
#define AG_OUTPUTDIRECTORY APP_OUTPUTDIRECTORY
#define AG_WIDTH APP_FRAMEWIDTH
#define AG_HEIGHT APP_FRAMEHEIGHT
#define AG_NUMPIXELS APP_NUMPIXELS
#define AG_HALFWIDTH APP_HALFFRAMEWIDTH
#define AG_HALFHEIGHT APP_HALFFRAMEHEIGHT
#define AG_LUMRED 79 //These 3 values should sum to 256
#define AG_LUMGREEN 156
#define AG_LUMBLUE 21
#define AG_ROMANFONT "13 m8o:m8j:m8d7e7j:m8c:m8b7a8f:m8f:m8k:m8p7u7v:m8v:m8w:m8x8y7t:m8t:o8m:c8m:w8m:k8m " \
                     "33 cm:r: 34 ch:di 35 cw:dx:lj:qo 36 cw:ig1ln2sq 37 g:iq:s: 38 c1g2m1q2w:mo:ds2y " \
                     "39 ch 40 d1m2x 41 c1n2w 42 cm:gi:bn:dl 43 hr:ln 44 r2v 45 ln 46 r: 47 dq 48 c1in2r1lg2c:hm " \
                     "49 g1cw 50 g2c1i4q7t 51 g3b:g2cd2ih:i2s:q5s 52 c2l7o:dx 53 b8d:b3g:h3g:h2r1l1m 54 d1c1l2r1n2l " \
                     "55 g2bd1w 56 c1h2c:h1l2r1n2h 57 i2m1g2c1i4r4q 58 h:r: 59 h:r2v 60 ils 61 gi:ln 62 gnq " \
                     "63 g2c1i2m:r: 64 ih1mni2c1gl2rs1o 65 q6u:n1qcn3x:c6g 66 rcd2ihi2s1l:c1b 67 di2c1l2r3o " \
                     "68 bq:b0i3s4q:p5q 69 dbq7t:gh 70 b8e:g8i:bv 71 di2c1l2r1ns2w:m4n 72 a3bq:ds:L7j 73 b7d:qs:cr " \
                     "74 b7ds2w1q1r 75 dg:a1bq:g1s2y 76 bq7t 77 p1qbmds2w1r 78 q7bs7d3c 79 c1in2r1lg2c " \
                     "80 bq:g2c1i2m1g 81 c1in2r1lg2c:m5l:ms4t 82 qbd2ig:hs6y 83 c8b1gh2rq1lm 84 ae:cw " \
                     "85 a2bl2r1nd:n2t 86 a1brd 87 bqhsd 88 bs:dq 89 bh:dh:hr 90 b7dq7t 91 dcrs 92 bs " \
                     "93 bcrq 94 gci 95 qs 96 ci 97 h3g:h1n:n8s:t3s:l3n:l1qr1n 98 h1l2r1n2h:a1bq 99 i5n2h1rs " \
                     "100 h1l2r1n2h:e1ds 101 ln2h1r:s5r 102 c7w:c8i:i6d:l7n 103 h1n2r1l2h:ix7v 104 a1bq2h1n:x5n " \
                     "105 g5h:r7h:r4s:c: 106 g2hw2l5q:c: 107 a1bq:lh:l8s 108 g2c:r7c:o6r 109 f1gq:l2hr:m2is2w:q1p " \
                     "110 f1gq:l2h1ns2w 111 h1l2r1n2h:m 112 h1l2r1n2h:f2gv1u 113 h1l2r1n2h:j1ix2y 114 f1gv:l2n1m " \
                     "115 h3i:h1mn2sq1l 116 b1cr2s:g7i 117 l3g:l2r1n:j1in2t 118 f1gr1i2h 119 f1gq1hr1i2c 120 f1g2s2n:iu " \
                     "121 f1gl2r1n:ix7v 122 g7iq7t 123 m9d:m9x 124 cw 125 n9c:n9w 126 l7n "
#define agPoint(pixel,r,g,b) agSetPixel(pixel,r,g,b)

extern void (*agSetPixel)(unsigned short* pixel,unsigned char r,unsigned char g,
                          unsigned char b);

////////////////////////////////////////////////////////////////////////////////
//Basic
////////////////////////////////////////////////////////////////////////////////

API
unsigned char agGetLuminance(unsigned short* pixel);
API
void agSetHueAngle(int angle); //Optimal performance when angle=0
API
void agSetOverdrawOff();
API
void agSetOverdrawOn();
//API
void agSetPixelDrawMode();
//API
void agHueRotateSetPixel(unsigned short* pixel,unsigned char r,unsigned char g,
                         unsigned char b);
//API
void agHueRotateNoOverdrawSetPixel(unsigned short* pixel,unsigned char r,
                                   unsigned char g,unsigned char b);
//API
void agNoOverdrawSetPixel(unsigned short* pixel,unsigned char r,unsigned char g,
                          unsigned char b);
API
unsigned char agGetSize();
API
void agSetSize(unsigned char size);

////////////////////////////////////////////////////////////////////////////////
//Blit
////////////////////////////////////////////////////////////////////////////////

API
void agBlit(unsigned short* buff,unsigned short* src,int widthSrc,int heightSrc,
            int xPos,int yPos);
API
void agGrayBlit(unsigned short* buff,unsigned short* src,
                int widthSrc,int heightSrc,int xPos,int yPos);
API
void agFullScreenGrayUcharBlit(unsigned char* buff,unsigned short* src);
API
void agSaturateBlit(unsigned short* buff,unsigned short* src,
                    int widthSrc,int heightSrc,int xPos,int yPos,
                    unsigned char sat);
API
void agFullScreenBlit(unsigned short* buff,unsigned short* src);
API
void agFullScreenLightBlendBlit(unsigned short* buff,unsigned short* src);
API
void agFullScreenHeavyBlendBlit(unsigned short* buff,unsigned short* src);
API
void agFullScreenBlurBlit(unsigned short* buff,unsigned short* src);
API
void agGreenScreenBlit(unsigned short* buff,unsigned short* src,
                       int widthSrc,int heightSrc,int xPos,int yPos);
API
void agColorBlit(unsigned short* buff,unsigned short* src,int widthSrc,
                 int heightSrc,int xPos,int yPos,unsigned char swap,
                 unsigned char contrast,unsigned char brightness);
API
void agRotateGreenScreenBlit(unsigned short* buff,
                             unsigned short* src,
                             int widthSrc,int heightSrc,
                             int xPos,int yPos,int tta);
API
void agFullScreenWaveBlit(unsigned short* buff,unsigned short* src,
                          unsigned char strength,unsigned char counter);
API
void agSetCrumpleBuffer(unsigned char* crumpleBuff,unsigned short* buff);
API
void agGreenScreenCrumpleBlit(unsigned short* buff,unsigned char* crumpleBuff,
                              unsigned short* src,int widthSrc,int heightSrc,
                              int xPos,int yPos);

////////////////////////////////////////////////////////////////////////////////
//Basic full-screen effect
////////////////////////////////////////////////////////////////////////////////

API
void agFullScreenDim(unsigned short* buff);
API
void agFullScreenBlur(unsigned short* buff);
API
void agFullScreenMonochrome(unsigned short *buff,bool whiteBkg,unsigned char r,
                            unsigned char g,unsigned char b);
API
void agFullScreenSwapTwoColor(unsigned short* buff);

////////////////////////////////////////////////////////////////////////////////
//2D primitive
////////////////////////////////////////////////////////////////////////////////

API
void agLine(unsigned short* buff,int x0,int y0,int x1,int y1,
            unsigned char r,unsigned char g,unsigned char b);
API
void agQuad(unsigned short* buff,int x0,int y0,int x1,int y1,int x2,int y2,
            int x3,int y3,unsigned char r,unsigned char g,unsigned char b);
API
void agSquare(unsigned short* buff,int width,int x,int y,
              unsigned char r,unsigned char g,unsigned char b);
API
void agCircle(unsigned short* buff,int x,int y,int radius,
              unsigned char r,unsigned char g,unsigned char b);
API
void agEllipticArc(unsigned short* buff,int x0,int y0,int x1,int y1,
                   unsigned char numQuarters,bool orientation,
                   unsigned char r,unsigned char g,unsigned char b);
API
void agBezierCurve(unsigned  short* buff,int* p,int cx,int cy,
                   unsigned char r,unsigned char g,unsigned char b);
API
void agCurve(unsigned short* buff,int x0,int y0,int x1,int y1,
             char curveForm,bool flipBowlShape,
             unsigned char r,unsigned char g,unsigned char b);
API
void agClosedCurve(unsigned short* buff,int* p,int n,int cx,int cy,
                   unsigned char r,unsigned char g,unsigned char b);
API
void agNURBS(unsigned short* buff,unsigned int numControlPoints,int* p,
             unsigned short* w,unsigned short* u,int cx,int cy,
             unsigned char r,unsigned char g,unsigned char b);
API
void agPaintFill(unsigned short* buff,int x,int y,
                 unsigned char r,unsigned char g,unsigned char b);
API
void agWaveform(unsigned short* buff,void* data,unsigned int size,
                unsigned char elementSizeInBytes);

////////////////////////////////////////////////////////////////////////////////
//3D primitive
////////////////////////////////////////////////////////////////////////////////

API
void agZOctagon(unsigned short* buff,unsigned short* zInvBuff,
                int width,int height,int x,int y,unsigned short zInv,
                unsigned char r,unsigned char g,unsigned char b);
API
void agZRectangle(unsigned short* buff,unsigned short* zInvBuff,
                  int width,int height,int x,int y,unsigned short zInv,
                  unsigned char r,unsigned char g,unsigned char b);
API
void agZSquare(unsigned short* buff,unsigned short* zInvBuff,
               int width,int x,int y,unsigned short zInv,
               unsigned char r,unsigned char g,unsigned char b);
API
void agZPoint(unsigned short* buff,unsigned short* zInvBuff,
              int x,int y,unsigned short zInv,
              unsigned char r,unsigned char g,unsigned char b);
API
void agZRaytraceSphere(unsigned short* buff,unsigned short* zInvBuff,
                       const int posObj[3],const int radius,const int posLight[3],
                       unsigned short (*texture)(int,int,int,Noise*,
                       unsigned int),Noise* noise,unsigned int counter);
API
void agRaytraceSphere(unsigned short* buff,const int pos[3],
                      const int radius,const int posLight[3],
                      unsigned short (*texture)
                      (int,int,int,Noise*,unsigned int),Noise* noise,
                      const unsigned int counter);

////////////////////////////////////////////////////////////////////////////////
//Procedural texture fill
////////////////////////////////////////////////////////////////////////////////

API
void agNoiseFill(unsigned short* buff,Noise* noise,int width,int height);
API
void agFullScreenStaticFill(unsigned short* buff,unsigned char numStates);
API
void agFullScreenXORFill(unsigned short* buff);
API
void agFullScreenWoodFill(unsigned short* buff,Noise* noise,
                          unsigned char cutWood);
API
void agFullScreenPebbleFill(unsigned short* buff,Noise* noise);
API
void agFullScreenCellularAutomataFill(unsigned short* buff,
                                      unsigned char radius,
                                      unsigned char threshold,
                                      unsigned char numStates,
                                      bool vonNeumannNotMoore,bool buffIndex);
API
void agFullScreenSoap(unsigned short* buff,Noise* noise,unsigned int counter);
API
void agFullScreenSpiralIllusion(unsigned short* buff,bool blur,
                                unsigned int counter);

////////////////////////////////////////////////////////////////////////////////
//Procedural heightfield sample
////////////////////////////////////////////////////////////////////////////////

API
int agCloud(Noise* noise,int x,int y,int offset);
API
int agSea(bool noDetail,Noise* noise,int x,int y,unsigned int counter);
API
int agHill(Noise* noise,int x,int y);
API
int agLagoon(bool noDetail,Noise* noise0,Noise* noise1,int x,int y,
             unsigned int counter,bool* isLand,int* tmp);

////////////////////////////////////////////////////////////////////////////////
//Text
////////////////////////////////////////////////////////////////////////////////

API
void agTextFont(const char* szFontData);
API
void agTextGrapheme(unsigned short* buff,char c,
                    unsigned char r,unsigned char g,unsigned char b);
API
void agText(unsigned short* buff,const char* text,
            unsigned char r=48,unsigned char g=48,unsigned char b=32);
API
void agTextSetPosition(int x,int y);

////////////////////////////////////////////////////////////////////////////////
//Special effect
////////////////////////////////////////////////////////////////////////////////

API
void agFullScreenWind(unsigned short* buff,Noise* noise,unsigned int counter);
API
void agFullScreenWater(unsigned short* buff,unsigned short* bkgImage,
                       unsigned char* zInvBuff,bool produceDroplets,
                       unsigned int counter);
API
void agFullScreenWetGlass(unsigned short* buff,Noise* noise,
                          unsigned int counter,unsigned char thickness=128);
API
void agFullScreenDistort(unsigned short* buff,Noise* noise);
API
void agRockSketch(unsigned short* buff);

////////////////////////////////////////////////////////////////////////////////
//Non-colorspace operation
////////////////////////////////////////////////////////////////////////////////

API
void agFullScreenSmooth(unsigned short* buff,unsigned short* src);
API
void agFullScreenScale(unsigned short* buff,int mul,int rsh);
API
void agFullScreenCenterValue(unsigned short* buff);
//API
void agDistanceTransform1D(unsigned short *buff,unsigned short *src,
                           unsigned int size,int* v,int* z);
API
void agFullScreenDistanceTransform(unsigned short* buff);
API
void agFullScreenExtendedDistanceTransform(unsigned short* buff,
                                           unsigned short* buff2);
