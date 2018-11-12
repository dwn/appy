////////////////////////////////////////////////////////////////////////////////
//Dan Nielsen
////////////////////////////////////////////////////////////////////////////////
#include "Raycast.h"
#include "Common.h"
#include "App.h"
#include "AppGraph.h"
#include "AppAudio.h"
#include "MoreMath.h"
#include "Skel3D.h"
#include "Splat.h"
#include "nanojpeg.h"
#include <emscripten/fetch.h>
#define rrSetZInv(zInvBuff,dist) (*(zInvBuff)=(1000000000)/dist)

//Game variable declaration
bool underWater;
unsigned int wetGlassCountdown;
//Declaration
bool readyForNextKey;
bool readyToFetch;
unsigned int inputCode,counter,shaderIndex,shaderIndexOld;
int tta,velRgt,velFwd,velUp,accUp;
int xOffset,yOffset,zOffset;
unsigned short *buff,*offBuff,*zInvBuff;
Noise *noise0,*noise1;
bool mouseDown;
//  int posSphere[3],posLight[3];
int ind,tmp,tmp2,tmp3,tmp4,tmp5,j,yPerspOld,h00,sinTta,cosTta;
Skel3D *skel3D;
Splat *splat;
int posSkel3D[3];
unsigned char colorCurr[3],rgbBuff[3];
const char* shaderSource[][2]={
  {NULL,NULL},
  { //Mandelbrot transform
    "precision mediump float;"
    "varying vec2 v_texcoord;"
    "uniform sampler2D u_texture;"
    "void main() {"
    "  vec2 q=.6245*vec2(v_texcoord);"
    "  for(int i=0;i<8;i++)"
    "    q=vec2(q[0]*q[0]-q[1]*q[1],2.*q[0]*q[1])+.6245*vec2(v_texcoord);"
    "  q[0]=mod(q[0],.6245);"
    "  q[1]=mod(q[1],.46875);"
    "  q[0]=(q[0]>.0005? q[0] : .0005);" //Hide unseemly seam
    "  gl_FragColor=texture2D(u_texture,q);"
    "}",NULL}};
void (*rrUpdate)();
void (*rrUpdate2)();

unsigned short rrSphereSurface(int x,int y,int z,Noise* noise,unsigned int t) {
  unsigned int w=200+(noise->get2D(x>>11,(y>>11)-(t<<3),0xff,0xff)>>5);
  return ((w>>3)<<11)|((w>>2)<<5)|(w>>3);
}

void rrOutlineText(unsigned short* buff,int x,int y,const char* txt,
                   unsigned char r,unsigned char g,unsigned char b,
                   unsigned char rOutline,unsigned char gOutline,unsigned char bOutline) {
  agTextSetPosition(x,y); agText(buff,txt,rOutline,gOutline,bOutline);  
  agTextSetPosition(x+2,y); agText(buff,txt,rOutline,gOutline,bOutline);  
  agTextSetPosition(x,y+2); agText(buff,txt,rOutline,gOutline,bOutline);  
  agTextSetPosition(x+2,y+2); agText(buff,txt,rOutline,gOutline,bOutline);  
  agTextSetPosition(x+1,y+1); agText(buff,txt,r,g,b);  
}

void rrImageFetchedCallback(emscripten_fetch_t* fetch) {
  nj_result_t error=njDecode(fetch->data,fetch->numBytes);
  if (error) printf("Error reading image (%d)\n",error);
  unsigned char *src=njGetImage(); //RGB chunks
  for(int y=APP_FRAMEHEIGHT-1;y>=0;y--) {
    for(int x=0;x<APP_FRAMEWIDTH;x++) {
      commonSetPixel(offBuff+y*APP_FRAMEWIDTH+x,src[0],src[1],src[2]);
      src+=3;
    }
  }
  emscripten_fetch_close(fetch);
  appSetShader(shaderSource[shaderIndex][0],shaderSource[shaderIndex][1]);
  appDisplay();
  rrUpdate=rrUpdate2;
  rrLoopUpdate();
  appSetCounterClear();
  readyToFetch=true;
}

void rrImageFetch(const char* url) {
  if (readyToFetch) {
    readyToFetch=false;
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod,"GET");
    attr.attributes=EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.onsuccess=rrImageFetchedCallback;
    attr.onerror=NULL;
    emscripten_fetch(&attr,url);
  }
}

void rrInit() {
  //Game variable init
  underWater=false;
  wetGlassCountdown=0;
  //Init
  readyForNextKey=true;
  readyToFetch=true;
  shaderIndex=0;
  shaderIndexOld=0;
  buff=appGetFrameBuffer();
  offBuff=appGetOffFrameBuffer();
  zInvBuff=appGetZInvBuffer();
  noise0=new Noise();
  noise1=new Noise();
  mouseDown=false;
  skel3D=new Skel3D(buff);
  splat=new Splat(buff,zInvBuff);
  agTextFont(AG_ROMANFONT);
  // setDoubleBufferOn();
  // setKeyRepeatOn();
  appSetMouseCursorOff();
  rrReset();
  njInit(); //Nanojpeg
  rrSegue(rrPretitleLoop,1);
  rrLoopUpdate();
}

void rrClose() {
  delete splat;
  delete skel3D;
  delete noise1;
  delete noise0;
}

void rrReset() {
  appUISetHideOutput();
  appSetCounterClear();
  commonRandomCounterSeed(0);
  tta=velRgt=velFwd=yOffset=zOffset=0,
  xOffset=-250000000;
  rrClearArtifact();
}

void rrPaused() {
  appSetPauseAudioOn();
  if (!appGetKey()) readyForNextKey=true;
  if (appGetMouseDown()||(appGetKey()&&readyForNextKey)) {
    if (appGetKey()==27) {
      readyForNextKey=false;
      appUISetAccountPageOpen();
    } else {
      appSetPauseAudioOff();
      rrUpdate=rrMainLoop;
      rrClearArtifact();
      rrLoopUpdate();
    }
  }
}

void rrClearArtifact() {
  velUp=1024; accUp=-256; //Begin with jump to clear graphic artifact
}

void rrLoopInit(bool updateNoise) {
  if (updateNoise) noise0->update();
  inputCode=appGetInputCode();
  mouseDown=appGetMouseDown();
  counter=appGetCounter();
}

void rrLoopUpdate() {
  appUpdate();
  appSetIdleFunction(rrUpdate);
}

void rrSegue(void (*callback)(void),int shaderIndexSet) {
  agFullScreenBlit(zInvBuff,offBuff);
  agFullScreenStaticFill(offBuff,3);
  appSetCounterClear();
  rrUpdate=rrSegueHelper;
  shaderIndexOld=shaderIndex;
  shaderIndex=shaderIndexSet;
  rrUpdate2=callback;
}

void rrSegueHelper() {
  rrLoopInit();
  agFullScreenCellularAutomataFill(offBuff,1,3,3,0,counter%3);
  agFullScreenLightBlendBlit(buff,offBuff);
  if (counter>=16)
    agFullScreenDim(buff);
  if (counter>=32) {
    if (shaderIndex!=shaderIndexOld)
      appSetShader(shaderSource[shaderIndex][0],shaderSource[shaderIndex][1]);
    agFullScreenBlit(offBuff,zInvBuff);
    if (rrUpdate2==rrTitleLoop) {
      aaReset(0);
      appSetPlayBuffer(0,"Slets_bygin");
    }
    else if (rrUpdate2==rrMainLoop) {
      aaReset(0);
      appSetPlayBuffer(0,"Slets_pla_rar^_ni]z");
    }
    rrUpdate=rrUpdate2;
  }
  rrLoopUpdate();
}

void rrPretitleLoop() {
  rrLoopInit(false);
  // agFullScreenSpiralIllusion(offBuff,true,counter);
  // agFullScreenWind(buff,noise0,counter);
  // agFullScreenLightBlendBlit(buff,offBuff);
  // agTextSetPosition(250,290); agText(buff,"Amp ",160,160,160);
  // agTextSetPosition(330,290); agText(buff,"l ",160,160,160);
  // agTextSetPosition(355,290); agText(buff,"e ",160,160,160);
  // agTextSetPosition(277,220); agText(buff,"A ",160,160,160);
  // agTextSetPosition(300,220); agText(buff,"r ",160,160,160);
  // agTextSetPosition(315,220); agText(buff,"t ",160,160,160);
  // agTextSetPosition(335,220); agText(buff,"s ",160,160,160);
  agFullScreenSoap(buff,noise0,counter);
  appCursor(284+((mmAbs(512-((int)counter&0x3ff))*mmCos(counter<<4))>>17),
            255+((mmAbs(512-((int)counter&0x3ff))*(mmSin(3*(counter<<2))>>1))>>17),
            true,false);
  appSetPrintLocation(15*80+(counter%55)); appSetPrintString("Click to begin... ",1);
  int pos=100+(mmSin(counter<<4)>>10);
  rrOutlineText(buff,pos,295,"p ",255,255,255,255,0,255);
  rrOutlineText(buff,pos+25,295,"o ",255,255,255,255,0,255);
  rrOutlineText(buff,pos+48,295,"i ",255,255,255,255,0,255);
  rrOutlineText(buff,pos+70,295,"e ",255,255,255,255,0,255);
  rrOutlineText(buff,pos+54,319,". ",255,255,255,255,0,255);
  rrOutlineText(buff,pos+98,295,"s ",255,255,255,255,0,255);
  rrOutlineText(buff,pos+131,295,"y ",255,255,255,255,0,255);
  rrOutlineText(buff,pos+160,295,"s ",255,255,255,255,0,255);
  appSetCounterAdd(1);
  if (appGetMouseDown()) { //Mouse interaction only, because some browsers require it in order to begin audio
    shaderIndexOld=shaderIndex;
    shaderIndex=0;
    rrUpdate2=rrTitleLoop;
    rrImageFetch("static/coast.jpg");
  }
  rrLoopUpdate();
}

void rrTitleLoop() {
  rrLoopInit();
  if (counter<128) {
    agColorBlit(buff,offBuff,AG_WIDTH,AG_HEIGHT,AG_HALFWIDTH,AG_HALFHEIGHT,false,64,counter);
  } else {
    agFullScreenWind(buff,noise0,counter);
    agFullScreenHeavyBlendBlit(buff,offBuff);
  }
  rrOutlineText(buff,125,350,"c r y s t a l",220,30,20,90,15,10);
  rrOutlineText(buff,185,270,"c o a s t ",220,30,20,90,15,10);
  if (appGetMouseDown()||appGetKey()) {
    for(unsigned int x=0; x<1024; x++) { //Cloudy sky
      for(unsigned int y=0; y<(AG_NUMPIXELS>>10); y++) {
        int a=noise1->getTurbulence2D(x<<2,y<<4,0xff,0xff,5)<<5;
        a=(a<0? 0 : a);
        a=255-(256<<15)/mmExp2(a)+(y>>2);
        commonSetPixel(offBuff+(y<<10)+x,a>255? 255 : a,160,220);
      }
    }
    printf("Control:\n\nMouse - turn\nW A S D -  move\nSpace - jump\nQ - interact & take\nZ - dive & surface (where deep enough)\nEscape - settings\n");
    rrSegue(rrMainLoop);
  }
  rrLoopUpdate();
}

void rrMainLoop() {
  rrLoopInit();
  int xOffsetOld=xOffset,yOffsetOld=yOffset,ttaOld=tta;
  //Mouse
  tta-=(appGetMouseCursorX()-AG_HALFWIDTH)>>6;
  tta=(tta+0x400)&0x3ff;
  //Keyboard
  if (!readyForNextKey) readyForNextKey=!appGetKey();
  if (appGetKey()==27) {
    rrUpdate=rrPaused;
    rrLoopUpdate();
  }
  if (inputCode&APP_INPUTAAFTERD) { if (inputCode&APP_INPUTA) velRgt=-192; }
  else { if (!(inputCode&APP_INPUTD)) velRgt=192; }
  if (inputCode&APP_INPUTWAFTERS) { if (inputCode&APP_INPUTW) velFwd=192; }
  else { if (!(inputCode&APP_INPUTS)) velFwd=-192; }
  if (inputCode&APP_INPUTSPACE) { if (zOffset==0) { velUp=2048; } accUp=-256; }
  if (inputCode&APP_INPUTQ&&readyForNextKey) { readyForNextKey=false; }
  if (inputCode&APP_INPUTZ&&readyForNextKey) {
    if (underWater) {
      underWater=false;
      wetGlassCountdown=32;
    } else {
      if (rrGetHeight()<=-150<<16) { //Needs only be half depth, since hill scaled by two when underwater
        underWater=true;
      } else {
        appSetSoundEffect(APP_SOUNDEFFECTBEEP);
      }
    }
    readyForNextKey=false;
  }
  //Drag to a stop
  if (velRgt)
    { xOffset+=sinTta*(velRgt+=(velRgt<0? 8 : -8)); yOffset-=cosTta*velRgt; }
  if (velFwd)
    { xOffset+=cosTta*(velFwd+=(velFwd<0? 8 : -8)); yOffset+=sinTta*velFwd; }
  //Keep in range
  xOffset=(xOffset<-0x70000000? -0x70000000 :
                                (xOffset>0x70000000? 0x70000000 : xOffset));
  yOffset=(yOffset<-0x70000000? -0x70000000 :
                                (yOffset>0x70000000? 0x70000000 : yOffset));
  //Useful calculations
  sinTta=mmSin(tta); cosTta=mmCos(tta);
  if (underWater) {
    h00=-300<<16;
    rrUpdate_UnderWater();
    rrScene_UnderWater();
  } else {
    //Fall
    zOffset+=(velUp+=accUp); if (zOffset<0) { zOffset=0; velUp=0; }
    h00=rrGetHeight(true);
    h00+=(zOffset>0? zOffset<<12 : -wetGlassCountdown<<20);
    rrUpdate_AboveWater();
    rrScene_AboveWater();
  }
  if (underWater&&rrGetHeight()>-150<<16) {
    xOffset=xOffsetOld; yOffset=yOffsetOld; tta=ttaOld;
  }
splat->update();
  rrLoopUpdate();
}

void rrSetGraphicBuffer(unsigned short* buffSet) { buff=buffSet; }

void rrSetVelocityForward(int velFwdSet) { velFwd=velFwdSet; }

int rrGetAngle() { return tta; }

int rrGetX() { return xOffset; }

int rrGetY() { return yOffset; }

int rrGetH_AboveWater(int x,int y) { //Get height
  bool dummyBool; int dummyInt;
  return agLagoon(1,noise1,noise0,x,y,counter,&dummyBool,&dummyInt)-h00;
}

int rrGetHeight(bool noNegativeHeight) { //Get maximum height along entire screen length
  tmp=(xOffset+AG_WIDTH*(cosTta-sinTta))>>8; tmp2=(yOffset+AG_WIDTH*(sinTta+cosTta))>>8;
  int h=-2147483647; //Min int32
  for(int i=0;i<AG_WIDTH;i++) {
    tmp3=agHill(noise1,tmp,tmp2); if (tmp3>h) h=tmp3; if (h<0&&noNegativeHeight) h=0;
    tmp+=sinTta>>7; tmp2-=cosTta>>7;
  }
  return h;
}

////////////////////////////////////////////////////////////////////////////////

void rrUpdate_AboveWater() {
  //\--+  /
  //  \|/ 90-deg FOV
  int x0=(xOffset+AG_WIDTH*(cosTta-sinTta))>>8,y0=(yOffset+AG_WIDTH*(sinTta+cosTta))>>8;
  if (counter&1) { x0+=sinTta>>7; y0-=cosTta>>7; /*(0x100+(h00>>17))>>8;*/ }
  //Rays (vertical screen scanlines)
  for(j=counter&1;(ind=j)<AG_WIDTH;j+=2) {
    int vx=(x0-(xOffset>>8)),vy=(y0-(yOffset>>8)),dist=AG_HALFWIDTH<<8,
        l=mmSqrRt((vx>>8)*(vx>>8)+(vy>>8)*(vy>>8)),
        x=x0,y=y0,yPersp,h=yPerspOld=0,hOld;
    vx=(vx/l); vy=(vy/l); int dDist=(dist/l);
    bool isLand=false,wasLand;
    //x0*=(0x100+(h00>>17))>>8; y0*=(0x100+(h00>>17))>>8; dist*=(0x100+(h00>>17))>>8;
    //Detail levels
    int lengthInvPwr=0,distFin=dist; //+0x1000;//+(h00>>6);
    for(int detailLevel=0;detailLevel<5+((j&0x2)>>1)+((j&0x4)>>1);detailLevel++) {
      switch(detailLevel) {
        break; case 0:
          distFin+=0x10000; lengthInvPwr+=2; vx<<=5; vy<<=5; dDist<<=5;
        break; case 1:
          distFin+=0x15000; lengthInvPwr++; vx<<=1; vy<<=1; dDist<<=1;
        break; case 2: //This level antialiases water
          distFin+=0x20000; lengthInvPwr++; vx<<=1; vy<<=1; dDist<<=1;
        break; case 3: //Antialiases land and water
          distFin+=0x40000; lengthInvPwr++; vx<<=1; vy<<=1; dDist<<=1;
        break; case 4: //Antialiases land and water
          distFin+=0x80000; lengthInvPwr++; vx<<=1; vy<<=1; dDist<<=1;
        break; case 5: //Scans every other interlaced line
          distFin+=0x160000; //lengthInvPwr++; vx<<=1; vy<<=1; dDist<<=1;
        break; case 6: //Scans every fourth interlaced line, blending with old
          distFin+=0x10000; lengthInvPwr--; vx>>=1; vy>>=1; dDist>>=1;
      }
      //Evaluate heights in detail level
      do {
        hOld=h; wasLand=isLand;
        h=agLagoon(detailLevel>=4,noise1,noise0,x,y,counter,&isLand,&tmp3)-
          h00-(304<<16);
        tmp3-=h00+(304<<16);
        //Perspective-transform sample and calculate color for sample
        if (yPerspOld<(yPersp=AG_HALFHEIGHT+h/dist)) {
          if (isLand) { //Land color
            tmp=(detailLevel<=4?
                  (noise0->get2D((x>>8)-(counter<<3),y>>8,0xff,0xff)>>7)+(dist>>14) :
                  210-(dist>>13));
            tmp=(tmp<0? 0 : tmp); //Keeps distant land lit
            colorCurr[0]=tmp+135; colorCurr[1]=tmp+120; colorCurr[2]=tmp+95;
          } else { //Water color
            tmp=((h-hOld)>>lengthInvPwr)>>9;
            tmp2=(dist>>12)-((h+h00-0xf0000)>>16)-395;
            tmp2=(tmp2>58? 58 : tmp2);
            tmp=mmClamp(tmp,-8,8)+tmp2;
            //tmp2+=(detailLevel&1)*50; //Uncomment for testing only
            colorCurr[0]=tmp2+98; colorCurr[1]=tmp+144; colorCurr[2]=tmp+189;
            tmp2=(h-tmp3)>>16;
            tmp=128-tmp2;
            if (tmp2<128) { //If shore, blend water and land
              colorCurr[0]=(135*tmp+colorCurr[0]*tmp2)>>7;
              colorCurr[1]=(120*tmp+colorCurr[1]*tmp2)>>7;
              colorCurr[2]=(95*tmp+colorCurr[2]*tmp2)>>7;
            }
          }
          //Coordinate boundary lines
          colorCurr[0]=(mmAbs(x-0x700000)<40000||mmAbs(x+0x700000)<40000||
                        mmAbs(y-0x700000)<40000||mmAbs(y+0x700000)<40000?
                        (255-(dist>>13))+(((dist>>13)*colorCurr[0])>>8) :
                        colorCurr[0]);
          //Spot
          colorCurr[1]=
            (((x-(posSkel3D[2]<<2))>>9)*((x-(posSkel3D[2]<<2))>>9)+
              ((y+(posSkel3D[0]<<2))>>9)*((y+(posSkel3D[0]<<2))>>9)<25000?
                (255-(dist>>13))+(((dist>>13)*colorCurr[1])>>8) : colorCurr[1]);
          //Render small segment
          do {
            if ((!isLand&&detailLevel==2)||detailLevel>2) {
              if (detailLevel>4) {
                if (detailLevel==5) { //Set 2 pixels for detailLevel 5
                  tmp4=rrSetZInv(zInvBuff+ind,dist);
                  commonSetPixel(buff+ind,
                                 colorCurr[0],colorCurr[1],colorCurr[2]);
                  *(zInvBuff+ind-2)=tmp4;
                  commonSetPixel(buff+ind-2,
                                 colorCurr[0],colorCurr[1],colorCurr[2]);
                } else { //Set 4 pixels for detailLevel 6
                  tmp4=rrSetZInv(zInvBuff+ind,dist);
                  //Blend color with old pixel color
                  commonGetRGB(rgbBuff,buff+ind);
                  tmp=(colorCurr[0]+15*rgbBuff[0])>>4;
                  tmp2=(colorCurr[1]+15*rgbBuff[1])>>4;
                  tmp3=(colorCurr[2]+15*rgbBuff[2])>>4;
                  commonSetPixel(buff+ind,tmp,tmp2,tmp3);
                  *(zInvBuff+ind-2)=tmp4;
                  commonSetPixel(buff+ind-2,tmp,tmp2,tmp3);
                  *(zInvBuff+ind-4)=tmp4;
                  commonSetPixel(buff+ind-4,tmp,tmp2,tmp3);
                  *(zInvBuff+ind-6)=tmp4;
                  commonSetPixel(buff+ind-6,tmp,tmp2,tmp3);
                }
              } else {
                rrSetZInv(zInvBuff+ind,dist);
                if (velFwd||velRgt) { //If moving, do not antialias
                  commonSetPixel(buff+ind,colorCurr[0],
                                          colorCurr[1],
                                          colorCurr[2]);
                } else { //Antialias all of detailLevel 2,3 - water only for 4
                  commonGetRGB(rgbBuff,buff+ind-(j>0));
                  commonSetPixel(buff+ind,(colorCurr[0]+rgbBuff[0])>>1,
                                          (colorCurr[1]+rgbBuff[1])>>1,
                                          (colorCurr[2]+rgbBuff[2])>>1);
                }
              }
            } else { //detailLevel 0,1
              rrSetZInv(zInvBuff+ind,dist);
              //No blending
              commonSetPixel(buff+ind,
                              colorCurr[0],colorCurr[1],colorCurr[2]);
            }
            yPerspOld++; ind+=AG_WIDTH;
          } while(yPerspOld<=yPersp);
          x-=vx>>1; y-=vy>>1; dist-=dDist>>1;
        }
        x+=vx; y+=vy; dist+=dDist;
      } while(dist<distFin);
    }
    rrBackground_AboveWater();
    x0+=sinTta>>6; y0-=cosTta>>6;
  }
}

//Finishes a vertical scanline with the background image
void rrBackground_AboveWater() {
  //Fill water to shoreline
  tmp3=j-((tta<<1)+(tta>>1))+2280; //In a space of 2560=2048+512
  int bound=206+32-(h00>>21)+((tmp3>>7)&1);
  for(;yPerspOld<bound;yPerspOld++) {
    ind+=AG_WIDTH;
    tmp=commonPermute256[((j>>1)^yPerspOld^counter)&0xff]>>4; //commonRandomUchar()&0xf;
    *(zInvBuff+ind)=0;
    commonSetPixel(buff+ind,tmp+140,tmp+192,253);
  }
  //Beach
  tmp=mmAbs(noise1->get2D(tmp3<<1,128,0xff,0xff)>>7)+
      mmAbs(noise1->get2D(tmp3<<2,128,0xff,0xff)>>8)+
      mmAbs(noise1->get2D(tmp3<<3,128,0xff,0xff)>>9)+
      (mmSin((tmp3>>2)-64)>>9)+192;
  bound=241-(h00>>21)+(commonPermute256[(tmp3>>4)&0xff]&1);
  if (bound>tmp) bound=tmp;
  for(yPerspOld++;yPerspOld<bound;yPerspOld++) {
    ind+=AG_WIDTH; zInvBuff[ind]=0; agPoint(buff+ind,145,150,190);
  }
  //First mountain range
  bound=tmp-32-(h00>>21);
  for(;yPerspOld<bound;yPerspOld++) {
    ind+=AG_WIDTH;
    tmp=(h00>>21)+yPerspOld;
    tmp2=638+(commonPermute256[((((tmp3+(tmp&0x1))>>1)&0xff)^
                                commonPermute256[tmp&0xff])]>>3)-(tmp<<1);
    tmp4=((((commonPermute256[(((tmp3>>1)^(tmp2>>4))&0xff)^
                              commonPermute256[(tmp>>1)&0xff]])+
            (tmp2>>3))>>3)&3)<<4;
    tmp2-=tmp4+(tmp4>>1);
    tmp5=tmp-224; tmp5<<=1;
    tmp4=200+(noise0->get2D((tmp3+(counter<<2))<<1,tmp<<2,0x7,0x7)>>5);
    *(zInvBuff+ind)=0;
    commonSetPixel(buff+ind,
                   ((255-tmp5)*(130-(tmp>>4)))>>8,
                   ((((tmp5*tmp2*tmp4)>>8)+(255-tmp5)*(160-(tmp>>4))))>>8,
                   ((255-tmp5)*255)>>8);
  }
  //Second mountain range
  bound=mmAbs(noise1->get2D(tmp3,394,0xff,0xff)>>5)+
        mmAbs(noise1->get2D(tmp3<<1,394,0xff,0xff)>>6)+
        mmAbs(noise1->get2D(tmp3<<2,394,0xff,0xff)>>7)+
        (mmSin((tmp3>>2)-64)>>9)+180-(h00>>21);
  for(;yPerspOld<bound;yPerspOld++) {
    tmp=(h00>>21)+yPerspOld;
    if (tmp3<1567-((308-tmp)>>4)||tmp3>1570) {
      ind+=AG_WIDTH;
      tmp2=138+
           (commonPermute256[((tmp3&0xff)^commonPermute256[tmp&0xff])]>>3)-
           (tmp-240);
      tmp5=tmp-192;
      tmp4=200+(noise0->get2D((tmp3+(counter<<2))<<1,tmp<<2,0x7,0x7)>>5);
      *(zInvBuff+ind)=0;
      agPoint(buff+ind,
              ((255-tmp5)*(130-(tmp>>4)))>>8,
              ((((tmp5*tmp2*tmp4)>>8)+(255-tmp5)*(160-(tmp>>4))))>>8,
              ((255-tmp5)*255)>>8);
    }
    else {
      //Waterfall fills in landscape seam
      ind+=AG_WIDTH;
      tmp2=260-(tmp>>1)+
           (commonPermute256[((tmp3&0xff)^
                             commonPermute256[(tmp+(counter>>1))&0xff])]>>3);
      zInvBuff[ind]=0;
      agPoint(buff+ind,tmp2,tmp2,160);
    }
  }
  //Fill water to horizon
  bound=AG_HALFHEIGHT-(h00>>22);
  for(yPerspOld++;yPerspOld<bound;yPerspOld++) {
    ind+=AG_WIDTH;
    tmp=commonPermute256[(j^yPerspOld^counter)&0xff]>>4; //commonRandomUchar()&0xf;
    *(zInvBuff+ind)=0;
    commonSetPixel(buff+ind,tmp+130,tmp+182,243);
  }
  //Flat sky
  bound=AG_HEIGHT; //352-(h00>>21);
  const int tmp2=(bound-181)<<10,
            tmp3=(j+(-tta<<1))&0x3ff;
  for(tmp=(yPerspOld-181)<<10;tmp<=tmp2;tmp+=1024) {
    *(zInvBuff+(ind+=AG_WIDTH))=0;
    buff[ind]=offBuff[tmp3+tmp];
  }
/*//Cloud base
  bound=200+480-(h00>>21);
  for(;yPerspOld<bound;yPerspOld++) {
    ind+=AG_WIDTH;
    k=yPerspOld+(h00>>21);
    tmp=(agCloud(noise0,((j-(tta<<1)+counter)&0x3ff)<<1,k<<1,counter)>>6)+
        (mmSCurve(((k+32)<<1)&0xff)>>2)+106;
    if (tmp<138) tmp=138;
    if (tmp>255) tmp=255;
    *(zInvBuff+ind)=0;
    agPoint(buff+ind,tmp-31,tmp,255);
  }
  //Full cloud
  bound=200+AG_HEIGHT;
  for(;yPerspOld<bound;yPerspOld++) {
    ind+=AG_WIDTH;
    k=yPerspOld+(h00>>21);
    tmp=(agCloud(noise0,((j-(tta<<1)+counter)&0x3ff)<<1,k<<1,counter)>>6)+170;
    if (tmp<138) tmp=138;
    if (tmp>255) tmp=255;
    *(zInvBuff+ind)=0;
    agPoint(buff+ind,tmp-31,tmp,255);
  }*/
}

void rrScene_AboveWater() { //Everything else in the scene
  //Birds
  if (tta>460&&tta<840) { //In view
    tmp2=mmSin(counter&0x1ff)>>11;
    for(int i=0;i<40;i+=4) {
      tmp=((tta<<1)+(tta>>1)+1660-i-(counter&0x1ff))%2560;
      if (tmp>=0&&tmp<AG_WIDTH) {
        ind=((287+tmp2+(commonPermute256[i]>>4)+
              (mmSin((i<<6)+(counter<<3))>>14)+commonRandomBool()-
              (h00>>21))*AG_WIDTH+tmp);
        if (!zInvBuff[ind]) commonSetPixel(buff+ind,80,80,96);
      }
    }
  }
  //Sphere
//  posSphere[0]=mmSin(counter<<2)<<2;
//  posSphere[1]=(mmCos(counter<<2)<<2)-(h00>>7);
//  posSphere[2]=0x100000;
//  posLight[0]=sinTta<<3;
//  posLight[1]=0x180000;
//  posLight[2]=cosTta<<3;
//  agZRaytraceSphere(buff,rrGetZInvBuffer(),posSphere,0x1a000,posLight,
//                    rrSphereSurface,noise1,counter);
  //Actor
  appSetPrintLocation(30); appSetPrintInt(rrGetX()); appSetPrintInt(rrGetY());
  posSkel3D[0]=0; posSkel3D[1]=0; posSkel3D[2]=-counter<<9;
  skel3D->setPosition((rrGetY()>>10),
    (rrGetH_AboveWater(posSkel3D[2]<<2,-posSkel3D[0]<<2)/680)-(h00/13000)-9800,
    posSkel3D[2]-(rrGetX()>>10));
  skel3D->setViewAngle(0,-tta,0);
//   -rrGetH(rrGetX()+3200*cosTta,rrGetY()+3200*sinTta)>>13,3200<<4);
//    rrGetY()>>12,rrGetH(skel3D->rrGetH(),(skel3D->rrGetX()-rrGetY())>>12,-rrGetX()>>12);
  //skel3D->setPosition(,0,);
  skel3D->update();
  if (wetGlassCountdown) { agFullScreenWetGlass(buff,noise0,counter,wetGlassCountdown); wetGlassCountdown--; }
}

////////////////////////////////////////////////////////////////////////////////

void rrUpdate_UnderWater() {
  //\--+  /
  //  \|/ 90-deg FOV
  int x0=(xOffset+AG_WIDTH*(cosTta-sinTta))>>8,y0=(yOffset+AG_WIDTH*(sinTta+cosTta))>>8;
  if (counter&1) { x0+=sinTta>>7; y0-=cosTta>>7; /*(0x100+(h00>>17))>>8;*/ }
  //Rays (vertical screen scanlines)
  for(j=counter&1;(ind=j)<AG_WIDTH;j+=2) {
    int vx=(x0-(xOffset>>8)),vy=(y0-(yOffset>>8)),dist=AG_HALFWIDTH<<8,
        l=mmSqrRt((vx>>8)*(vx>>8)+(vy>>8)*(vy>>8)),
        x=x0,y=y0,yPersp,h=yPerspOld=0,hOld;
    vx=(vx/l); vy=(vy/l); int dDist=(dist/l);
    //x0*=(0x100+(h00>>17))>>8; y0*=(0x100+(h00>>17))>>8; dist*=(0x100+(h00>>17))>>8;
    //Detail levels
    bool cont=true;
    int lengthInvPwr=0,distFin=dist; //+0x1000;//+(h00>>6);
    for(int detailLevel=0;cont&&detailLevel<5+((j&0x2)>>1)+((j&0x4)>>1);detailLevel++) {
      switch(detailLevel) {
        break; case 0:
          distFin+=0x10000; lengthInvPwr+=2; vx<<=5; vy<<=5; dDist<<=5;
        break; case 1:
          distFin+=0x15000; lengthInvPwr++; vx<<=1; vy<<=1; dDist<<=1;
        break; case 2: //This level antialiases water
          distFin+=0x20000; lengthInvPwr++; vx<<=1; vy<<=1; dDist<<=1;
        break; case 3: //Antialiases land and water
          distFin+=0x40000; lengthInvPwr++; vx<<=1; vy<<=1; dDist<<=1;
        break; case 4: //Antialiases land and water
          distFin+=0x80000; lengthInvPwr++; vx<<=1; vy<<=1; dDist<<=1;
        break; case 5: //Scans every other interlaced line
          distFin+=0x160000; //lengthInvPwr++; vx<<=1; vy<<=1; dDist<<=1;
        break; case 6: //Scans every fourth interlaced line, blending with old
          distFin+=0x10000; lengthInvPwr--; vx>>=1; vy>>=1; dDist>>=1;
      }
      //Evaluate heights in detail level
      do {
        hOld=h;
        h=(agHill(noise1,x,y)<<1)-h00;
        //Perspective-transform sample and calculate color for sample
        if (cont&&yPerspOld<(yPersp=AG_HALFHEIGHT+h/dist)) {
          tmp=(detailLevel<=4?
                (noise0->get2D((x>>8)-(counter<<3),y>>8,0xff,0xff)>>7)+(dist>>14) :
                210-(dist>>13));
          tmp=(tmp<0? 0 : tmp); //Keeps distant land lit
          if (h<-h00+(commonRandomUchar()<<11)) { //Ground underWater
            colorCurr[0]=tmp+95; colorCurr[1]=tmp+100; colorCurr[2]=tmp+105;
          } else { //Ground above water
            colorCurr[0]=tmp+60; colorCurr[1]=tmp+95; colorCurr[2]=tmp+175;
          }
          //Coordinate boundary lines
          colorCurr[0]=(mmAbs(x-0x700000)<40000||mmAbs(x+0x700000)<40000||
                        mmAbs(y-0x700000)<40000||mmAbs(y+0x700000)<40000?
                        (255-(dist>>13))+(((dist>>13)*colorCurr[0])>>8) :
                        colorCurr[0]);
          //Spot
          colorCurr[1]=
            (((x-(posSkel3D[2]<<2))>>9)*((x-(posSkel3D[2]<<2))>>9)+
              ((y+(posSkel3D[0]<<2))>>9)*((y+(posSkel3D[0]<<2))>>9)<25000?
                (255-(dist>>13))+(((dist>>13)*colorCurr[1])>>8) : colorCurr[1]);
          //Render small segment
          do {
            if (detailLevel>2) {
              if (detailLevel>4) {
                if (detailLevel==5) { //Set 2 pixels for detailLevel 5
                  tmp4=rrSetZInv(zInvBuff+ind,dist);
                  commonSetPixel(buff+ind,
                                 colorCurr[0],colorCurr[1],colorCurr[2]);
                  *(zInvBuff+ind-2)=tmp4;
                  commonSetPixel(buff+ind-2,
                                 colorCurr[0],colorCurr[1],colorCurr[2]);
                } else { //Set 4 pixels for detailLevel 6
                  tmp4=rrSetZInv(zInvBuff+ind,dist);
                  //Blend color with old pixel color
                  commonGetRGB(rgbBuff,buff+ind);
                  tmp=(colorCurr[0]+15*rgbBuff[0])>>4;
                  tmp2=(colorCurr[1]+15*rgbBuff[1])>>4;
                  tmp3=(colorCurr[2]+15*rgbBuff[2])>>4;
                  commonSetPixel(buff+ind,tmp,tmp2,tmp3);
                  *(zInvBuff+ind-2)=tmp4;
                  commonSetPixel(buff+ind-2,tmp,tmp2,tmp3);
                  *(zInvBuff+ind-4)=tmp4;
                  commonSetPixel(buff+ind-4,tmp,tmp2,tmp3);
                  *(zInvBuff+ind-6)=tmp4;
                  commonSetPixel(buff+ind-6,tmp,tmp2,tmp3);
                }
              } else {
                rrSetZInv(zInvBuff+ind,dist);
                if (velFwd||velRgt) { //If moving, do not antialias
                  commonSetPixel(buff+ind,colorCurr[0],
                                          colorCurr[1],
                                          colorCurr[2]);
                } else { //Antialias all of detailLevel 2,3 - water only for 4
                  commonGetRGB(rgbBuff,buff+ind-(j>0));
                  commonSetPixel(buff+ind,(colorCurr[0]+rgbBuff[0])>>1,
                                          (colorCurr[1]+rgbBuff[1])>>1,
                                          (colorCurr[2]+rgbBuff[2])>>1);
                }
              }
            } else { //detailLevel 0,1
              rrSetZInv(zInvBuff+ind,dist);
              //No blending
              commonSetPixel(buff+ind,
                              colorCurr[0],colorCurr[1],colorCurr[2]);
            }
            yPerspOld++; ind+=AG_WIDTH;
            if (yPerspOld>=AG_HEIGHT) cont=false;
          } while(cont&&yPerspOld<=yPersp);
          x-=vx>>1; y-=vy>>1; dist-=dDist>>1;
        }
        x+=vx; y+=vy; dist+=dDist;
      } while(cont&&dist<distFin);
    }
    if (cont) rrBackground_UnderWater();
    x0+=sinTta>>6; y0-=cosTta>>6;
  }
}

//Finishes a vertical scanline with the background image
void rrBackground_UnderWater() {
  for(yPerspOld++;yPerspOld<AG_HEIGHT-256;yPerspOld++) {
    ind+=AG_WIDTH; zInvBuff[ind]=0; buff[ind]=(0x8<<11)|(0x20<<5)|0x10;
  }
  for(yPerspOld++;yPerspOld<AG_HEIGHT;yPerspOld++) {
    tmp=mmAbs(noise0->get2D(((j-AG_HALFWIDTH)*(0x80+AG_HEIGHT-yPerspOld))>>7,(yPerspOld<<2)-(counter<<3),0xff,0xff))>>8;
    tmp2=yPerspOld-(AG_HEIGHT-256);
    ind+=AG_WIDTH;
    zInvBuff[ind]=0;
    tmp2=(tmp2>>3)+((tmp*tmp2)>>7);
    tmp2=(tmp2>0xf? 0xf : tmp2);
    buff[ind]=(0x8<<11)|((0x20+(tmp>>2))<<5)|0x10+tmp2;
  }
}

void rrScene_UnderWater() { //Everything else in the scene
  agFullScreenDistort(buff,noise0);
}

////////////////////////////////////////////////////////////////////////////////

void rrUpdate_Cave() {
}

////////////////////////////////////////////////////////////////////////////////

void rrUpdate_GrassyField() {
  //tmp+=(detailLevel<1? (commonPermute256[(((y>>10)&0xff)^
  //                      commonPermute256[(x>>10)&0xff])]>>5) : 0);
}

////////////////////////////////////////////////////////////////////////////////

int main() {
  appInit();
  rrInit();
  return 0;
}
