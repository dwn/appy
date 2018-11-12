////////////////////////////////////////////////////////////////////////////////
//Dan Nielsen
//Novel variant of nim.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Define.h"
#include "ppapi/utility/completion_callback_factory.h"
#include "App.h"
#include "AppGraph.h"

struct /*API*/ Icosian : public AppInstance {
  Icosian(PP_Instance instance);
  ~Icosian();
  void DidChangeView(const pp::View& view);
private:
  void (Icosian::*update)(int32_t);
  void (Icosian::*update2)(int32_t);
  pp::CompletionCallbackFactory<Icosian> callback_factory_;
  void reset();
  void loopInit(bool updateNoise=true);
  void loopUpdate();
  void pretitleLoop(int32_t);
  void segue(void (Icosian::*callback)(int32_t),int shaderIndexSet=0);
  void curtainLoop(int32_t);
  void titleLoop(int32_t);
  void mainLoop(int32_t);
  void endSequenceLoop(int32_t);
  static const char *shaderSource[][2];
  unsigned int inputCode,counter,shaderIndex,shaderIndexOld;
  unsigned short *buff,*offBuff,*zInvBuff;
  Noise *noise;
  bool mouseDown;
  int xMouse,yMouse;
  void background();
};

class AppModule : public pp::Module {
 public:
  AppModule() : pp::Module() {}
  virtual ~AppModule() {}
  virtual pp::Instance* CreateInstance(PP_Instance instance) {
    return new Icosian(instance);
  }
};

namespace pp {
Module* CreateModule() { return new AppModule(); }
} //Namespace pp
