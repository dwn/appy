////////////////////////////////////////////////////////////////////////////////
//Dan Nielsen
////////////////////////////////////////////////////////////////////////////////
#include "Icosian.h"

const char* Icosian::shaderSource[][2]={
  {NULL,NULL},
  { //Mandelbrot transform
    "precision mediump float;\n"
    "varying vec2 v_texcoord;\n"
    "uniform sampler2D u_texture;\n"
    "void main() {\n"
    "  gl_FragColor=texture2D(u_texture,v_texcoord);\n"
    "}\n",NULL}
};
const int ptArr[]={320<<16, 20<<16, 320<<16,114<<16,
                   190<<16, 44<<16, 222<<16, 86<<16,
                   216<<16,190<<16, 126<<16,162<<16,
                   162<<16,278<<16, 106<<16,296<<16,
                   256<<16,314<<16, 200<<16,390<<16,
                   320<<16,392<<16, 320<<16,452<<16,
                   450<<16, 44<<16, 418<<16, 86<<16,
                   424<<16,190<<16, 514<<16,162<<16,
                   478<<16,278<<16, 534<<16,296<<16,
                   384<<16,314<<16, 440<<16,390<<16};
const size_t ptArrSize=sizeof(ptArr)/sizeof(int);
const unsigned int edgeArr[]={1,2};
const size_t edgeArrSize=sizeof(edgeArr)/sizeof(unsigned int);

Icosian::Icosian(PP_Instance instance) :
  AppInstance(instance),
  callback_factory_(this),
  shaderIndex(0),shaderIndexOld(0),
  buff(getFrameBuffer()),offBuff(getOffFrameBuffer()),zInvBuff(getZInvBuffer()),
  noise(new Noise()),
  mouseDown(false) {
  agTextFont(AG_ROMANFONT);
  reset();
  update=&Icosian::pretitleLoop;
}

Icosian::~Icosian() {
}

void Icosian::DidChangeView(const pp::View& view) {
  if (context_.is_null()) {
    appInit();
    pretitleLoop(0);
  }
}
  
void Icosian::reset() {
  setCounterClear();
  commonRandomCounterSeed(0);
}

void Icosian::loopInit(bool updateNoise) {
  if (updateNoise) noise->update();
  inputCode=getInputCode();
  mouseDown=getMouseDown();
  counter=getCounter();
  xMouse=getMouseX(); yMouse=getMouseY();
}

void Icosian::loopUpdate() {
  appUpdate();
  context_.SwapBuffers(callback_factory_.NewCallback(update));
}

void Icosian::pretitleLoop(int32_t) {
  loopInit(false);
  agFullScreenSpiralIllusion(buff,true,counter);
  setPrintLocation(15*80+31); setPrintString("Click to begin... ",3);
  if (getMouseJustLocked()||getMouseDown()||
      (inputCode!=APP_INPUTDEFAULT&&!(inputCode&APP_INPUTESC))) {
    segue(&Icosian::titleLoop);
  }
  loopUpdate();
}

void Icosian::segue(void (Icosian::*callback)(int32_t),int shaderIndexSet) {
  agFullScreenBlit(zInvBuff,offBuff);
  //agFullScreenStaticFill(offBuff,3);
  setCounterClear();
  update=&Icosian::curtainLoop;
  shaderIndexOld=shaderIndex;
  shaderIndex=shaderIndexSet;
  update2=callback;
}

void Icosian::curtainLoop(int32_t) {
  loopInit();
  background();
  if (counter>=16) {
    if (shaderIndex!=shaderIndexOld)
      setShader(shaderSource[shaderIndex][0],
                shaderSource[shaderIndex][1]);
    agFullScreenBlit(offBuff,zInvBuff);
    if (update2==&Icosian::titleLoop) {
      aaReset(0);
      setPlayBuffer(0,"Sx`lwy_pla_^_gamm");
    }
    else if (update2==&Icosian::mainLoop) {
      aaReset(0);
      setPlayBuffer(0,"Slets_pla_ek_sin_t^");
    }
    update=update2;
  }
  loopUpdate();
}

void Icosian::titleLoop(int32_t) {
  loopInit(false);
  background();
  agTextSetPosition(230,240); agText(buff,"Exinta ",64,64,64);
  agTextSetPosition(232,240); agText(buff,"Exinta ",64,64,64);
  agTextSetPosition(230,242); agText(buff,"Exinta ",64,64,64);
  agTextSetPosition(232,242); agText(buff,"Exinta ",64,64,64);
  agTextSetPosition(231,241); agText(buff,"Exinta ",190,65,97);
  if (inputCode!=APP_INPUTDEFAULT||getMouseDown()) {
    if (inputCode&APP_INPUTESC) {
      segue(&Icosian::pretitleLoop);
    }
    else {
      agFullScreenWoodFill(offBuff,noise,3);
      segue(&Icosian::mainLoop);
    }
  }
  loopUpdate();
}

void Icosian::mainLoop(int32_t) {
  loopInit();
  for(unsigned int i=0;i<ptArrSize;i+=2) {
    agCircle(buff,ptArr[i],ptArr[i+1],5<<16,255,30,150);
    agPaintFill(buff,ptArr[i]>>16,ptArr[i+1]>>16,255,30,150);
    agCircle(buff,ptArr[i],ptArr[i+1],5<<16,255,255,255);
  }
  for(unsigned int i=0;i<edgeArrSize;i+=2) {
    agLine(buff,ptArr[i],ptArr[i+1],ptArr[edgeArr[i]<<1],ptArr[(edgeArr[i]<<1)+1],
           255,0,i*50);
    agLine(buff,ptArr[i],ptArr[i+1],ptArr[edgeArr[i+1]<<1],ptArr[(edgeArr[i+1]<<1)+1],
           0,255,i*50);
  }
  loopUpdate();
}

void Icosian::endSequenceLoop(int32_t) {
  loopInit();
  agSetHueAngle(counter<<4);
  agFullScreenBlit(buff,offBuff);
  if (getMouseDown()||inputCode!=APP_INPUTDEFAULT) {
    agFullScreenWoodFill(offBuff,noise,2); //Background image
    reset();
    segue(&Icosian::titleLoop);
  }
  loopUpdate();
}

void Icosian::background() {
  for(unsigned int x=0; x<1024; x++)
    for(unsigned int y=0; y<(AG_NUMPIXELS>>10); y++)
    {
      int a=noise->getTurbulence2D(x,y<<1,1,1,5)<<5;
      a=(a<0? 0 : a);
      a=127-(128<<15)/mmExp2(a);
      a>>=2;
      buff[(y<<10)+x]=(a<<11)|(a<<6)|a;
    }
}
