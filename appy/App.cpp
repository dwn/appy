////////////////////////////////////////////////////////////////////////////////
//Dan Nielsen
////////////////////////////////////////////////////////////////////////////////
#include "App.h"
#include "Common.h"
#include "MoreMath.h"
#include "AppAudio.h"
#include "Version.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#define GL_GLEXT_PROTOTYPES //For more, #define EGL_EGLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glut.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <emscripten/html5.h>
#include <emscripten/emscripten.h>

////////////////////////////////////////////////////////////////////////////////
//Common application code
////////////////////////////////////////////////////////////////////////////////

struct Vert { float pos[3]; float tex[2]; };

GLuint _appShaderProgId;
GLuint _appTexture;
GLuint _appTexCoordLoc;
ALCdevice *_appAudioDevice=NULL;
ALCcontext *_appAudioContext;
ALuint _appAudioSrc[9]; //Track 8 is only for sound effects, which play without delay
bool _appWasSrcPlaying[9];
bool _appCanvasRotated;
int _posText;
unsigned short _appFrameBuff[1024*1024]; //GL requires power of 2
unsigned short *_appOffFrameBuff;
unsigned short *_appZInvBuff;
unsigned int _appTimer;
unsigned int _appTimerOld;
unsigned int _appTimerLastFrameUpdate;
unsigned int _appCounter;
unsigned int _appCounterOld;
bool _appDone;
bool _appDoubleFrameBuffer;
short *_appScratchBuff;
unsigned int _appInputCode;
short *_appSoundBuffTmp;
char *_appPlayBuff[8];
const char** _appPlaySection;
bool _appMouseDownHistory[16];
bool _appHideMouseCursor;
bool _appAudioPlaying;
int _appXMouse,_appYMouse,
         _appXMouseOld,_appYMouseOld,
         _appXMouseCursor,_appYMouseCursor,
         _appXMouseCursorOld,_appYMouseCursorOld,
         _appXMouseScl,_appYMouseScl,
         _appXMouseOffset,_appYMouseOffset;
unsigned char _appKey;
void (*_appIdleFunc)(void);

unsigned int appGetCounter() { return _appCounter; }
unsigned short* appGetFrameBuffer() { return _appFrameBuff; }
unsigned short* appGetOffFrameBuffer() { return _appOffFrameBuff; }
unsigned short* appGetZInvBuffer() { return _appZInvBuff; }
void appSetPrintLocation(int i) { _posText=i; }
unsigned int appGetTimer() { return _appTimer; }
void appSetTimerOld() { _appTimerOld=_appTimer; }
unsigned int appGetTimerDifference() { return _appTimer-_appTimerOld; }
unsigned int appGetTimerOld() { return _appTimerOld; }
void appSetCounterClear() { _appCounter=0; }
void appSetCounterOld() { _appCounterOld=_appCounter; }
void appSetCounterAdd(int addend=1) { _appCounter+=addend; }
unsigned int appGetCounterOld() { return _appCounterOld; }
unsigned int appGetCounterDifference() { return _appCounter-_appCounterOld; }
void appSetDone() { _appDone=true; }

void appSetFrameBufferClear() //Clears only on-screen buffer
  { memset(_appFrameBuff,0,sizeof(unsigned short)*APP_FRAMEWIDTH*APP_FRAMEHEIGHT); }

void appSetZInvBufferClear()
  { memset(_appZInvBuff,0,sizeof(unsigned short)*APP_FRAMEWIDTH*APP_FRAMEHEIGHT); }

void appSetZInvBufferMidLevel()
  { memset(_appZInvBuff,128,sizeof(unsigned short)*APP_FRAMEWIDTH*APP_FRAMEHEIGHT); }

//Strings must end with a space (before the string termination character)
//Values for style:
//0: regular
//1: headline
//2: underline
//3: bold
//4: crumple
//5: crumple with shadow
void appSetPrintString(const char* text,unsigned char style,
                    bool (*boundaryFunc)(int)) {
  if (text[0]==' '&&text[1]=='\0') { _posText++; return; } //Early return on " "
  int k,lenBar,lenWord,wordCount,lenWordCount,h,xText,yText,widthSpace,
      lineStart,xTextOffset;
  unsigned short *index;
  unsigned char *crumpleCurr;
  bool newLine,shadow;
  int numLoops=(style==4? 2 : 1);
  int _posTexto=_posText;
  for(int loopIndex=0;loopIndex<numLoops;loopIndex++) {
    k=0;
    widthSpace=0;
    _posText=_posTexto;
    while(text[k]!='\0') {
      newLine=false;
      while(boundaryFunc(_posText)) _posText++;
      lenBar=0;
      lineStart=80*(_posText/80);
      while(!boundaryFunc(_posText)&&_posText<(lineStart+80)&&text[k+lenBar]!=13&&
            text[k+lenBar]!='\0') {
        lenBar++;
        _posText++;
      }
      if (style==2) {
        for(int n=lenBar;n>1;n--) {
          xText=((_posText-n)%80)<<3;
          yText=(29-((_posText-n)/80))<<4;
          index=appGetFrameBuffer()+yText*APP_FRAMEWIDTH+xText;
          for(int i=0;i<8;i++) {
            commonSetPixel(index,APP_UNDERLINERED(i),
                                 APP_UNDERLINEGREEN(i),
                                 APP_UNDERLINEBLUE(i));
            index++;
          }
        }
      }
      lenWordCount=0;
      wordCount=-1;
      do {
        lenWord=0;
        do {
          k++;
          lenWord++;
        } while(text[k]!=' '&&text[k]!='\0');
        k++;
        lenWordCount+=lenWord+1;
        wordCount++;
      } while(lenBar>=lenWordCount);
      k-=lenWord+1; //Back up one word (with space included)
      lenWordCount-=lenWord+1;
      xText=((_posText-lenBar)%80)<<3;
      yText=(29-((_posText-lenBar)/80))<<4;
      if (wordCount==1) {
        widthSpace=(lenBar-lenWordCount)<<2;
        xText+=widthSpace;
      }
      if (wordCount>1) {
        widthSpace=((lenBar-lenWordCount+wordCount-1)<<3)/(wordCount-1);
      }
      for(int n=lenWordCount;n>1;n--) {
        h=text[k-n]<<4;
        if (h<0) h+=256<<4;
        xTextOffset=(((yText>>4)*(7*7*7*7*7))>>4)&0x7; //Random value
        for(int j=0;j<16;j++,yText++,h++) {
          for(int i=0;i<8;i++,xText++) {
            if (commonSymbol[h]&(1<<i)) {
              index=appGetFrameBuffer()+yText*APP_FRAMEWIDTH+xText;
              shadow=((i==0)||((i>0)&&!(commonSymbol[h]&(1<<(i-1)))));
              if (!(style||shadow)) {
                commonSetPixel(index,APP_TEXTRED(i,j),
                                     APP_TEXTGREEN(i,j),
                                     APP_TEXTBLUE(i,j));
              }
              if (style==1||style==2||style==3) {
                commonSetPixel(index,
                  (style==3? APP_BOLDRED(i,j) :
                             (shadow? (APP_HEADLINERED(i,j)>>3)+32 :
                                      APP_HEADLINERED(i,j))),
                  (style==3? APP_BOLDGREEN(i,j) :
                             (shadow? (APP_HEADLINEGREEN(i,j)>>3)+32 :
                                      APP_HEADLINEGREEN(i,j))),
                  (style==3? APP_BOLDBLUE(i,j) :
                             (shadow? (APP_HEADLINEBLUE(i,j)>>3)+32 :
                                      APP_HEADLINEBLUE(i,j))));
              }
              if (style==4) {
                if (loopIndex) {
                  crumpleCurr=((unsigned char*)_appZInvBuff)+yText*APP_FRAMEWIDTH+
                              xText+xTextOffset;
                  index+=(((*crumpleCurr>>4)-8)>>1)*APP_FRAMEWIDTH+xTextOffset;
                  if (!shadow) {
                    commonSetPixel(index,
                      (APP_TEXTRED(i,j)+commonGetRed(index))>>1,
                      (APP_TEXTGREEN(i,j)+commonGetGreen(index))>>1,
                      (APP_TEXTBLUE(i,j)+commonGetBlue(index))>>1);
                  }
                } else {
                    crumpleCurr=((unsigned char*)_appZInvBuff)+yText*APP_FRAMEWIDTH+
                                xText+xTextOffset;
                    index+=((*crumpleCurr>>4)-8)*APP_FRAMEWIDTH+xTextOffset;
                    commonSetPixel(index,
                      shadow? 0 : (APP_TEXTRED(i,j)+commonGetRed(index))>>1,
                      shadow? 0 : (APP_TEXTGREEN(i,j)+commonGetGreen(index))>>1,
                      shadow? 0 : (APP_TEXTBLUE(i,j)+commonGetBlue(index))>>1);
                }
              }
            }
          }
          xText-=8;
        }
        if (text[k-n]==' ') {
          xText+=widthSpace;
        } else {
          xText+=8;
        }
        yText-=16;
      }
      if (text[k]==13) { _posText=lineStart+80; k+=2; }
    }
  }
}

void appSetPrintInt(const int n)
  { char text[16]; sprintf(text,"%d ",n); appSetPrintString(text); }

void appSetPrintUint(const unsigned int n) //TODO: Handle uppermost range of unsigned int
  { char text[16]; sprintf(text,"%d ",(int)n); appSetPrintString(text); }

void appSetPrintDot(unsigned char r,unsigned char g,unsigned char b) {
  unsigned short *p=_appFrameBuff+(APP_FRAMEWIDTH<<4)*(29-(_posText/80))+((_posText%80)<<3);
  const unsigned short c=((r>>3)<<11)|((g>>2)<<5)|(b>>3);
  p+=APP_FRAMEWIDTH<<2;
  p[2]=p[3]=p[4]=p[5]=0;
  p+=APP_FRAMEWIDTH;
  p[1]=p[6]=0; p[2]=p[3]=p[4]=p[5]=c;
  p+=APP_FRAMEWIDTH;
  p[0]=p[7]=0; p[1]=p[2]=p[3]=p[4]=p[5]=p[6]=c;
  p+=APP_FRAMEWIDTH;
  p[0]=p[7]=0; p[1]=p[2]=p[3]=p[4]=p[5]=p[6]=c;
  p+=APP_FRAMEWIDTH;
  p[0]=p[7]=0; p[1]=p[2]=p[3]=p[4]=p[5]=p[6]=c;
  p+=APP_FRAMEWIDTH;
  p[0]=p[7]=0; p[1]=p[2]=p[3]=p[4]=p[5]=p[6]=c;
  p+=APP_FRAMEWIDTH;
  p[1]=p[6]=0; p[2]=p[3]=p[4]=p[5]=c;
  p+=APP_FRAMEWIDTH;
  p[2]=p[3]=p[4]=p[5]=0;
  _posText+=2;
}

unsigned int appGetInputCode() { return _appInputCode; }

unsigned char appGetKey() { return _appKey; } //Last key pressed down

short* appGetScratchBuffer() { return _appScratchBuff; }

char* appGetPlayBuffer(unsigned char i) { return _appPlayBuff[i]; }

int appGetMouseCursorX() { return _appXMouseCursor; }

int appGetMouseCursorY() { return _appYMouseCursor; }

bool appGetMouseDown() { return _appMouseDownHistory[0]; }

bool appGetMouseClick() {
  if (!_appMouseDownHistory[0]) {
    unsigned char sumDiff=0;
    for(unsigned int i=0;i<15;i++) sumDiff+=mmAbs(_appMouseDownHistory[i+1]-_appMouseDownHistory[i]);
    if (sumDiff>1) {
      memset(_appMouseDownHistory,0,16*sizeof(bool));
      return true;
    }
  }
  return false;
}

bool appGetMouseHoldDown() {
  unsigned char sum=0;
  for(unsigned int i=0;i<16;i++) sum+=_appMouseDownHistory[i];
  return sum==16;
}

bool appGetMousePressDown() {
  return _appMouseDownHistory[0]&&!_appMouseDownHistory[1];
}

bool appGetMousePressUp() {
  return !_appMouseDownHistory[0]&&_appMouseDownHistory[1];
}

void appSetPlaySections(const char** playSection) {
  _appPlaySection=playSection;
}

void appSetPlaySectionSequence(int track,const char* playSectionSeq) {
  char *p=(char*)_appScratchBuff; //TODO: use _appPlayBuffer instead
  strcpy(p,"M");
  for(;*playSectionSeq!='\0';playSectionSeq++)
    strcat(p,_appPlaySection[*playSectionSeq-'0']);
  appSetPlayBuffer(track,p);
}

void appSetMouseCursorOn() { _appHideMouseCursor=false; }

void appSetMouseCursorOff() { _appHideMouseCursor=true; }

void appCursor(int x=_appXMouseCursor,int y=_appYMouseCursor,bool down=_appMouseDownHistory[0],bool obeyHideRule=true) {
  if (_appHideMouseCursor&&obeyHideRule) return;
  unsigned int counter=_appCounter;
  const char lft=(x<16? -x : -16);
  const char rgt=(x>APP_FRAMEWIDTH-16? APP_FRAMEWIDTH-x : 16);
  const char top=(y<16? -y : -16);
  const char btm=(y>APP_FRAMEHEIGHT-16? APP_FRAMEHEIGHT-y : 16);
  unsigned short *pixel=_appFrameBuff+(y+top)*APP_FRAMEWIDTH+x+lft;
  unsigned char rgb[3];
  int radiusSqr,a,b=96-(mmSin(counter<<3)>>12);
  for(int j=top;j<btm;j++) {
    for(int i=lft;i<rgt;i++) {
      radiusSqr=i*i+j*j;
      if (radiusSqr<12*12) {
        a=b+0x700/(radiusSqr+0x1c);
        int g=(a>128? a : 0);
        commonGetRGB(rgb,pixel);
        commonSetPixel(pixel,((g<<7)+rgb[0]*(128-(g>>2)))>>8,
                             ((a<<7)+rgb[1]*(128-(a>>2)))>>8,
                             ((a<<8)+rgb[2]*(128-(a>>1)))>>8);
      }
      pixel++;
    }
    pixel+=APP_FRAMEWIDTH-(rgt-lft);
  }
  if (down) { //Swirling particles
    for(int i=0;i<256;i++) {
      a=x+((commonPermute256[counter&0xff]-128)>>3);
      counter++;
      b=y+((commonPermute256[counter&0xff]-128)>>3);
      if ((a-x)*(a-x)+(b-y)*(b-y)<256) {
        const int x2=a+(mmSin((counter<<3)+(i<<3))>>13);
        const int y2=b+((255-i)>>2);
        if (x2>=0 && x2<APP_FRAMEWIDTH && y2>=0 && y2<APP_FRAMEHEIGHT) {
          pixel=_appFrameBuff+y2*APP_FRAMEWIDTH+x2;
          commonGetRGB(rgb,pixel);
          commonSetPixel(pixel,
                        ((i<<8)+rgb[0]*(256-i))>>8,
                        (rgb[1]*(256-i))>>8,
                        ((i<<8)+rgb[2]*(256-i))>>8);
        }
      }
    }
  }
}

void appMouseMotion(int x,int y) {
  if (_appCanvasRotated) {
    int tmp=x; x=y; y=tmp;
  } else {
    y=APP_FRAMEHEIGHT-1-y;
  }
  _appXMouseOld=_appXMouse;
  _appYMouseOld=_appYMouse;
  _appXMouse=((x*_appXMouseScl)>>16)-_appXMouseOffset;
  _appYMouse=((y*_appYMouseScl)>>16)-_appYMouseOffset;
  _appXMouseCursor=(_appXMouse<0? 0 :
                    _appXMouse>=APP_FRAMEWIDTH? APP_FRAMEWIDTH-1 :
                    _appXMouse);
  _appYMouseCursor=(_appYMouse<0? 0 :
                    _appYMouse>=APP_FRAMEHEIGHT? APP_FRAMEHEIGHT-1 :
                    _appYMouse);
}

void appKeyboard(unsigned char key,int x,int y) {
  switch(_appKey=key) {
    case ' ':
      _appInputCode|=APP_INPUTSPACE;
    break; case 'Q': case 'q':
      _appInputCode|=APP_INPUTQ;
    break; case 'A': case 'a':
      _appInputCode|=APP_INPUTA|APP_INPUTAAFTERD;
    break; case 'D': case 'd':
      _appInputCode&=~(APP_INPUTD|APP_INPUTAAFTERD);
    break; case 'W': case 'w':
      _appInputCode|=APP_INPUTW|APP_INPUTWAFTERS;
    break; case 'S': case 's':
      _appInputCode&=~(APP_INPUTS|APP_INPUTWAFTERS);
    break; case 'Z': case 'z':
      _appInputCode|=APP_INPUTZ;
    break; case 27:
      _appInputCode|=APP_INPUTESC;
  }
}

void appKeyboardUp(unsigned char key,int x,int y) {
  _appKey='\0';
  switch(key) {
    case ' ':
      _appInputCode&=~APP_INPUTSPACE;
    break; case 'Q': case 'q':
      _appInputCode&=~APP_INPUTQ;
    break; case 'A': case 'a':
      _appInputCode&=~(APP_INPUTA|APP_INPUTAAFTERD);
    break; case 'D': case 'd':
      _appInputCode|=APP_INPUTD|APP_INPUTAAFTERD;
    break; case 'W': case 'w':
      _appInputCode&=~(APP_INPUTW|APP_INPUTWAFTERS);
    break; case 'S': case 's':
      _appInputCode|=APP_INPUTS|APP_INPUTWAFTERS;
    break; case 'Z': case 'z':
      _appInputCode&=~APP_INPUTZ;
    break; case 27:
      _appInputCode&=~APP_INPUTESC;
  }
}

void appReset() {
  appSetShader();
  appSetFrameBufferClear();
  appSetZInvBufferClear();
  memset(_appMouseDownHistory,0,16*sizeof(bool));
  _appInputCode=APP_INPUTDEFAULT;
  _appDone=false;
  _posText=0;
  _appXMouse=0;
  _appYMouse=0;
  _appXMouseOld=0;
  _appYMouseOld=0;
  _appXMouseCursor=0;
  _appYMouseCursor=0;
  _appXMouseCursorOld=0;
  _appYMouseCursorOld=0;
  _appTimer=(int)emscripten_get_now();;
  _appCounter=0;
  appSetCounterOld();
  appSetTimerOld();
  _appTimerLastFrameUpdate=_appTimer;
}

////////////////////////////////////////////////////////////////////////////////
//Application code with dependency
////////////////////////////////////////////////////////////////////////////////

void appSetDoubleBufferOn() {
  _appDoubleFrameBuffer=true;
  glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);
  glutInitWindowSize(APP_FRAMEWIDTH,APP_FRAMEHEIGHT);
  glutCreateWindow(""); //TODO: destroy?
}

void appSetDoubleBufferOff() {
  _appDoubleFrameBuffer=false;
  glutInitDisplayMode(GLUT_RGB|GLUT_SINGLE);
  glutInitWindowSize(APP_FRAMEWIDTH,APP_FRAMEHEIGHT);
  glutCreateWindow(""); //TODO: destroy?
}

void appSpecialKeyboard(int key,int x,int y) {
  switch(key) {
    case GLUT_KEY_LEFT:
      _appInputCode|=APP_INPUTLEFT|APP_INPUTLEFTAFTERRIGHT;
    break; case GLUT_KEY_RIGHT:
      _appInputCode&=~(APP_INPUTRIGHT|APP_INPUTLEFTAFTERRIGHT);
    break; case GLUT_KEY_UP:
      _appInputCode|=APP_INPUTUP|APP_INPUTUPAFTERDOWN;
    break; case GLUT_KEY_DOWN:
      _appInputCode&=~(APP_INPUTDOWN|APP_INPUTUPAFTERDOWN);
  }
}

void appSpecialKeyboardUp(int key,int x,int y) {
  switch(key) {
    case GLUT_KEY_LEFT:
      _appInputCode&=~(APP_INPUTLEFT|APP_INPUTLEFTAFTERRIGHT);
    break; case GLUT_KEY_RIGHT:
      _appInputCode|=APP_INPUTRIGHT|APP_INPUTLEFTAFTERRIGHT;
    break; case GLUT_KEY_UP:
      _appInputCode&=~(APP_INPUTUP|APP_INPUTUPAFTERDOWN);
    break; case GLUT_KEY_DOWN:
      _appInputCode|=APP_INPUTDOWN|APP_INPUTUPAFTERDOWN;
  }
}

void appSetPlayBuffer(unsigned char i,const char* sz) {
  sprintf(_appPlayBuff[i],*sz=='S'? "S_%s" : "%s",sz); //Prefix speech with brief pause
  aaReset(i);
  ALuint audioBuffInd[APP_NUMSOUNDBUFFERS];
  alSourceUnqueueBuffers(_appAudioSrc[i],APP_NUMSOUNDBUFFERS,audioBuffInd);
  for(unsigned int j=0;j<APP_NUMSOUNDBUFFERS;j++) {
    memset(_appSoundBuffTmp,0,APP_SOUNDBUFFERSIZE<<1);
    alBufferData(audioBuffInd[j],AL_FORMAT_STEREO16,_appSoundBuffTmp,APP_SOUNDBUFFERSIZE,APP_SOUNDRATE);
  }
  alSourceQueueBuffers(_appAudioSrc[i],APP_NUMSOUNDBUFFERS,audioBuffInd);
  alSourcePlay(_appAudioSrc[i]);
}

void appMouseButton(int buttonCode,int state,int x,int y) {
  _appMouseDownHistory[0]=(state==GLUT_DOWN);
  appMouseMotion(x,y);
  if (!_appAudioDevice) appAudioInit(); //Playing audio requires user interaction in some browsers
}

void appAudioInit() {
  _appAudioDevice=alcOpenDevice(alcGetString(NULL,ALC_DEFAULT_DEVICE_SPECIFIER));
  _appAudioContext=alcCreateContext(_appAudioDevice,NULL);
  alcMakeContextCurrent(_appAudioContext);
  alGenSources(9,_appAudioSrc);
  for(unsigned int i=0;i<8;i++) {
    ALuint audioBuffInd[APP_NUMSOUNDBUFFERS];
    alGenBuffers(APP_NUMSOUNDBUFFERS,audioBuffInd);
    for(unsigned int j=0;j<APP_NUMSOUNDBUFFERS;j++)
      alBufferData(audioBuffInd[j],AL_FORMAT_STEREO16,_appSoundBuffTmp,APP_SOUNDBUFFERSIZE,APP_SOUNDRATE);
    alSourceQueueBuffers(_appAudioSrc[i],APP_NUMSOUNDBUFFERS,audioBuffInd);
  }
  for(unsigned int i=0;i<8;i++) alSourcePlay(_appAudioSrc[i]);
  alGenSources(1,&_appAudioSrc[8]);
  ALuint audioBuffInd;
  alGenBuffers(1,&audioBuffInd);
  alBufferData(audioBuffInd,AL_FORMAT_STEREO16,_appSoundBuffTmp,APP_SOUNDBUFFERSIZE,APP_SOUNDRATE);
  alSourceQueueBuffers(_appAudioSrc[8],1,&audioBuffInd);
  alSourcePlay(_appAudioSrc[8]);
}

EM_BOOL appResizeCallback(int eventType,const void* uiEvent,void* userData) {
  double wFlt,hFlt;
  emscripten_get_element_css_size("page",&wFlt,&hFlt);
  unsigned int w=(unsigned int)wFlt,h=(unsigned int)hFlt;
  _appCanvasRotated=false;
  if (w<h) {
    _appCanvasRotated=true;
    unsigned int tmp=w; w=h; h=tmp;
  }
  unsigned int wCanvas=w,hCanvas=h;
  _appXMouseScl=0x10000;
  _appYMouseScl=0x10000;
  _appXMouseOffset=0;
  _appYMouseOffset=0;
  bool wide=false;
  if ((w<<16)/h>(APP_FRAMEWIDTH<<16)/APP_FRAMEHEIGHT) {
    wide=true;
    wCanvas=(APP_FRAMEWIDTH*h)/APP_FRAMEHEIGHT;
    _appXMouseScl=(w<<16)/wCanvas;
    _appXMouseOffset=((w-wCanvas)*APP_FRAMEWIDTH)/(wCanvas<<1);
  } else {
    hCanvas=(APP_FRAMEHEIGHT*w)/APP_FRAMEWIDTH;
    _appYMouseScl=(h<<16)/hCanvas;
    _appYMouseOffset=((h-hCanvas)*APP_FRAMEHEIGHT)/(hCanvas<<1);
  }
  if (_appCanvasRotated) {
    appUISetCanvasSizeRotateOn(wCanvas,hCanvas);
    _appXMouseScl=(wCanvas<<16)/hCanvas;
    const unsigned int wEps=w+APP_FRAMEWIDTH-APP_FRAMEHEIGHT-((APP_FRAMEWIDTH-APP_FRAMEHEIGHT)*(h-hCanvas))/(w-hCanvas);
    _appYMouseScl=((APP_FRAMEHEIGHT*wEps)<<8)/((APP_FRAMEWIDTH*hCanvas)>>8);
    if (wide) {
      const unsigned int wEps2=w+APP_FRAMEWIDTH-APP_FRAMEHEIGHT+(APP_FRAMEHEIGHT*(w-wCanvas))/h;
      _appYMouseOffset=((wEps2-hCanvas)*APP_FRAMEWIDTH)/(w<<1);
      _appXMouseOffset=0;
    } else {
      _appYMouseOffset=((wEps-hCanvas)*APP_FRAMEWIDTH)/(w<<1);
    }
  }
  else appUISetRotateOff();
  return false;
}

void appDisplay() {
  appCursor();
  glEnableVertexAttribArray(_appTexCoordLoc);
  glTexSubImage2D(GL_TEXTURE_2D,0,0,0,APP_FRAMEWIDTH,APP_FRAMEHEIGHT,GL_RGB,
                  GL_UNSIGNED_SHORT_5_6_5,_appFrameBuff);
  glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_BYTE,0);
  glFlush();
  if (_appDoubleFrameBuffer) glutSwapBuffers();
}

void appInit() {
  _appKey='\0';
  _appShaderProgId=0;
  _appAudioPlaying=true;
  _appCanvasRotated=false;
  _appHideMouseCursor=true;
  _appXMouseScl=0x10000;
  _appYMouseScl=0x10000;
  _appXMouseOffset=0;
  _appYMouseOffset=0;
  _appOffFrameBuff=_appFrameBuff+APP_NUMPIXELS;
  _appZInvBuff=_appOffFrameBuff+APP_NUMPIXELS;
  _appSoundBuffTmp=(short*)(_appZInvBuff+APP_NUMPIXELS);
  _appScratchBuff=_appSoundBuffTmp+APP_SOUNDBUFFERSIZE;
  _appPlayBuff[0]=(char*)(_appSoundBuffTmp+APP_SCRATCHBUFFERSIZE);
  _appPlayBuff[1]=_appPlayBuff[0]+APP_PLAYBUFFERSIZE;
  _appPlayBuff[2]=_appPlayBuff[1]+APP_PLAYBUFFERSIZE;
  _appPlayBuff[3]=_appPlayBuff[2]+APP_PLAYBUFFERSIZE;
  _appPlayBuff[4]=_appPlayBuff[3]+APP_PLAYBUFFERSIZE;
  _appPlayBuff[5]=_appPlayBuff[4]+APP_PLAYBUFFERSIZE;
  _appPlayBuff[6]=_appPlayBuff[5]+APP_PLAYBUFFERSIZE;
  _appPlayBuff[7]=_appPlayBuff[6]+APP_PLAYBUFFERSIZE;
  memset(_appFrameBuff,0,sizeof(unsigned short)*1024*1024); //Clear all buffers
  int argc=1; glutInit(&argc,NULL);
  appSetDoubleBufferOff();
  glutDisplayFunc(appDisplay);
  glutPassiveMotionFunc(appMouseMotion);
  glutMotionFunc(appMouseMotion);
  glutMouseFunc(appMouseButton);
  glutKeyboardFunc(appKeyboard);
  glutKeyboardUpFunc(appKeyboardUp);
  glutSpecialFunc(appSpecialKeyboard);
  glutSpecialUpFunc(appSpecialKeyboardUp);
  //glutSetCursor(GLUT_CURSOR_NONE);
//emscripten_set_canvas_element_size("#canvas",1000,800);///////////////////////////////////////////////////////////////////<==================
  EmscriptenFullscreenStrategy s;
  memset(&s,0,sizeof(s));
  s.scaleMode=EMSCRIPTEN_FULLSCREEN_SCALE_ASPECT;
  s.canvasResolutionScaleMode=EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_NONE;
  s.filteringMode=EMSCRIPTEN_FULLSCREEN_FILTERING_BILINEAR;
  s.canvasResizedCallback=appResizeCallback;
  emscripten_request_fullscreen_strategy(NULL,true,&s);
  emscripten_enter_soft_fullscreen(NULL,&s);
  const Vert vert[4] = //Rectangle 1024x1024 partially offscreen
    {{{0,0,320},{0,0}},{{1024,0,320},{1,0}},{{1024,1024,320},{1,1}},{{0,1024,320},{0,1}}};
  const GLubyte poly[6]={2,1,0,3,2,0};
  GLuint vertBuffId,indexBuffId;
  glGenBuffers(1,&vertBuffId);
  glBindBuffer(GL_ARRAY_BUFFER,vertBuffId);
  glBufferData(GL_ARRAY_BUFFER,sizeof(vert),&vert[0],GL_STATIC_DRAW);
  glGenBuffers(1,&indexBuffId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,indexBuffId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(poly),&poly[0],GL_STATIC_DRAW);
  glGenTextures(1,&_appTexture);
  glBindTexture(GL_TEXTURE_2D,_appTexture);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,1024,1024,0,GL_RGB,
               GL_UNSIGNED_SHORT_5_6_5,_appFrameBuff);
  glDisable(GL_DEPTH_TEST); glDisable(GL_ALPHA_TEST); glDisable(GL_LIGHTING);
  glDisable(GL_BLEND); glDisable(GL_CULL_FACE); glDisable(GL_AUTO_NORMAL);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,_appTexture);
  appReset();
}

void appClose() {
  //TODO: close gl, glut, glsl program, stop playing audio, delete audio buffers
  alDeleteSources(9,_appAudioSrc);
  alcMakeContextCurrent(NULL);
  alcDestroyContext(_appAudioContext);
  alcCloseDevice(_appAudioDevice);
}

void appSetIdleFunction(void (*func)(void)) { //Okay to call this function repeatedly in a loop
  if (_appIdleFunc!=func) glutIdleFunc(func);
  _appIdleFunc=func;
}

GLuint appCompileShader(GLenum type,const char* data) {
  GLuint shader=glCreateShader(type);
  glShaderSource(shader,1,&data,NULL);
  glCompileShader(shader);
  GLint compile_status;
  glGetShaderiv(shader,GL_COMPILE_STATUS,&compile_status);
  if (compile_status!=GL_TRUE) { //Check error
    char buffer[1024];
    GLsizei length;
    glGetShaderInfoLog(shader,sizeof(buffer),&length,buffer);
    return GL_FALSE;
  }
  return shader;
}

void appSetShader(const char* fragShaderSource,
                  const char* vertShaderSource) {
  if (_appShaderProgId) glDeleteProgram(_appShaderProgId);
  GLuint fragShaderId=appCompileShader(GL_FRAGMENT_SHADER,fragShaderSource?
    fragShaderSource :
    "precision mediump float;"
    "varying vec2 v_texcoord;"
    "uniform sampler2D u_apptexture;"
    "void main() {"
    "  gl_FragColor=texture2D(u_apptexture,v_texcoord);"
    "}");
  if (!fragShaderId) return;
  GLuint vertShaderId=appCompileShader(GL_VERTEX_SHADER,vertShaderSource?
    vertShaderSource :
    "uniform mat4 u_mvp;"
    "attribute vec2 a_texcoord;"
    "attribute vec4 a_position;"
    "varying vec2 v_texcoord;"
    "void main() {"
    "  gl_Position=u_mvp*a_position;"
    "  v_texcoord=a_texcoord;"
    "}");
  if (!vertShaderId) return;
  _appShaderProgId=glCreateProgram();
  glAttachShader(_appShaderProgId,fragShaderId);
  glAttachShader(_appShaderProgId,vertShaderId);
  glLinkProgram(_appShaderProgId);
  glDetachShader(_appShaderProgId,fragShaderId);
  glDetachShader(_appShaderProgId,vertShaderId);
  glDeleteShader(fragShaderId);
  glDeleteShader(vertShaderId);
  GLint linkStatus;
  glGetProgramiv(_appShaderProgId,GL_LINK_STATUS,&linkStatus);
  if (linkStatus!=GL_TRUE) return; //Link error
  GLuint textureLoc=glGetUniformLocation(_appShaderProgId,"u_appTexture");
  GLuint positionLoc=glGetAttribLocation(_appShaderProgId,"a_position");
  _appTexCoordLoc=glGetAttribLocation(_appShaderProgId,"a_texcoord");
  GLuint mvpLoc=glGetUniformLocation(_appShaderProgId,"u_mvp");
  glUseProgram(_appShaderProgId);
  const float mvp[]={1./APP_HALFFRAMEWIDTH,0,0,0,0,1./APP_HALFFRAMEHEIGHT,0,0,
                     0,0,0,0,-1,-1,0,1};
  glUniformMatrix4fv(mvpLoc,1,GL_FALSE,mvp);
  glVertexAttribPointer(positionLoc,3,GL_FLOAT,GL_FALSE,sizeof(Vert),
                        reinterpret_cast<void*>(offsetof(Vert,pos)));
  glEnableVertexAttribArray(positionLoc);
  glVertexAttribPointer(_appTexCoordLoc,2,GL_FLOAT,GL_FALSE,sizeof(Vert),
                        reinterpret_cast<void*>(offsetof(Vert,tex)));
}

void appSetSoundEffect(unsigned int effectIndex) {
  ALuint audioBuffInd;
  alSourceUnqueueBuffers(_appAudioSrc[8],1,&audioBuffInd);
  short *p=_appSoundBuffTmp;
  unsigned int dx;
  switch(effectIndex) {
    case APP_SOUNDEFFECTBEEP:
      dx=16;
      for(unsigned int i=0,x=0;i<(APP_SOUNDEFFECTBUFFERSIZE>>2);i++) {
        *p=mmShortSin(x)>>2;
        p++;
        x+=dx;
        if (!(i&0x7f)) dx--;
        if (!(i&0xff)) dx=16;
      }
    break; case APP_SOUNDEFFECTPOP:
      dx=16;
      for(unsigned int i=0,x=0;i<(APP_SOUNDEFFECTBUFFERSIZE>>2);i++) {
        *p=mmShortSin(x)>>2;
        p++;
        x+=dx;
        if (!(i&0xff)) dx--;
      }
    break; case APP_SOUNDEFFECTTHUNDER:
      for(unsigned int i=0,x=0;i<(APP_SOUNDEFFECTBUFFERSIZE>>2);i++) {
        *p=commonPermute256[i>>2]<<6;
        p++;
      }
  }
  aaUpsampleAndMakeStereo(_appSoundBuffTmp,APP_SOUNDEFFECTBUFFERSIZE>>2,NULL);
  alBufferData(audioBuffInd,AL_FORMAT_STEREO16,_appSoundBuffTmp,APP_SOUNDEFFECTBUFFERSIZE,APP_SOUNDRATE);
  alSourceQueueBuffers(_appAudioSrc[8],1,&audioBuffInd);
  alSourcePlay(_appAudioSrc[8]);
}

void appUpdate() {
  if (_appDone) appClose();
  for(unsigned int i=15;i>0;i--) _appMouseDownHistory[i]=_appMouseDownHistory[i-1];
  _appCounter++;
  _appTimer=(int)emscripten_get_now();
#ifdef APP_PRINTVERSION
  appSetPrintLocation(0); appSetPrintString(APP_VERSION);
#endif
  if ((_appTimer>>5)!=(_appTimerLastFrameUpdate>>5)) { //Throttle to 32 fps
    _appTimerLastFrameUpdate=_appTimer;
    glutPostRedisplay();
  }
  if (_appAudioDevice) {
    for(unsigned int i=0;i<8;i++) {
      if (*_appPlayBuff[i]) {
        ALint n;
        alGetSourcei(_appAudioSrc[i],AL_BUFFERS_PROCESSED,&n);
        while(n--) {
          ALuint audioBuffInd;
          alSourceUnqueueBuffers(_appAudioSrc[i],1,&audioBuffInd);
          aaPlay(i,_appPlayBuff[i],_appSoundBuffTmp,_appScratchBuff);
          aaUpsampleAndMakeStereo(_appSoundBuffTmp,APP_SOUNDBUFFERSIZE>>2,NULL);
          alBufferData(audioBuffInd,AL_FORMAT_STEREO16,_appSoundBuffTmp,APP_SOUNDBUFFERSIZE,APP_SOUNDRATE);
          alSourceQueueBuffers(_appAudioSrc[i],1,&audioBuffInd);
        }
      }
    }
  }
}

void appSetPauseAudioOn() { //Okay to call this function repeatedly in a loop
  if (_appAudioPlaying) {
    appUISetPauseAudioOn();
    ALenum state;
    for(unsigned int i=0;i<9;i++) {
      alGetSourcei(_appAudioSrc[i],AL_SOURCE_STATE,&state);
      _appWasSrcPlaying[i]=(state==AL_PLAYING);
      if (_appWasSrcPlaying[i]) alSourcePause(_appAudioSrc[i]);
    }
  }
  _appAudioPlaying=false;
}

void appSetPauseAudioOff() { //Okay to call this function repeatedly in a loop
  if (!_appAudioPlaying) {
    appUISetPauseAudioOff();
    for(unsigned int i=0;i<9;i++)
      if (_appWasSrcPlaying[i]) alSourcePlay(_appAudioSrc[i]);     
  }
  _appAudioPlaying=true;
}
