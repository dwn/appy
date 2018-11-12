////////////////////////////////////////////////////////////////////////////////
//Dan Nielsen
//Three-player 3D Tic-Tac-Toe.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Define.h"
#include "ppapi/utility/completion_callback_factory.h"
#include "App.h"
#include "AppGraph.h"
#define TTTOE3D_PREDICTIONDEPTH 3 //Tries 2 best plays, then 2 probable from each, etc., to this depth
#define TTTOE3D_N 27 //Number of cells, i.e. levels in entire solution tree
#define TTTOE3D_DEPTH 3 //Number of tree levels to evaluate below current level
#define TTTOE3D_WIN 50000 //Standard increment of goodness value for a win
#define TTTOE3D_LINEPAIR 2000 //Goodness for a pair in a grid-aligned line
#define TTTOE3D_PLANEPAIR 2500 //Goodness for a diagonal pair in a plane
#define TTTOE3D_DIAGPAIR 3000 //Goodness for a pair aligned between the corners of the cube 
#define TTTOE3D_WINTHRESHOLD TTTOE3D_WIN //If goodness exceeds this value, win exists
#define TTTOE3D_ATTENUATION 128 //In tree, this is weighting factor applied to future-turn
                                //goodness over nearer-turn; 256=unity, 128=half, etc.
#define TTTOE3D_SIZE 35102 //Maximum number of nodes that can be stored
                           //If unsure, use 2+2*(TTTOE3D_N!/(TTTOE3D_N-TTTOE3D_DEPTH)!)
struct /*API*/ TTToe3D : public AppInstance {
  TTToe3D(PP_Instance instance);
  ~TTToe3D();
  void DidChangeView(const pp::View& view);
private:
  void (TTToe3D::*update)(int32_t);
  void (TTToe3D::*update2)(int32_t);
  pp::CompletionCallbackFactory<TTToe3D> callback_factory_;
  void reset();
  void loopInit(bool updateNoise=true);
  void loopUpdate();
  void segue(void (TTToe3D::*callback)(int32_t),int shaderIndexSet=0);
  void curtainLoop(int32_t);
  void pretitleLoop(int32_t);
  void titleLoop(int32_t);
  void mainLoop(int32_t);
  void endSequenceLoop(int32_t);
  bool getInput(unsigned char v[],const char* playerName[],int player);
  void checkCatGame();
  void drawBoard();
  void swap(unsigned char& x,unsigned char& y) { unsigned char tmp=x; x=y; y=tmp; }
  void swap(int* x,int* y) { int tmp=x[0]; x[0]=y[0]; y[0]=tmp;
                                 tmp=x[1]; x[1]=y[1]; y[1]=tmp;
                                 tmp=x[2]; x[2]=y[2]; y[2]=tmp; }
  unsigned char* node(unsigned char q[],int p) { return q+TTTOE3D_N*p; }
  unsigned char& cell(unsigned char v[],int a,int b,int c) { return v[3*(3*a+b)+c]; }
  unsigned char nextPlayer(unsigned char ind,unsigned char i=1) { return ((ind+i-1)%3)+1; }
  void print(unsigned char v[]);
  char gameState(unsigned char v[]);
  int rateConfiguration(unsigned char v[],unsigned char index);
  int match(unsigned char v[],unsigned char q[],int b,int n,int par[],int parent,
            unsigned char* transformCode=NULL);
  void setRoot(unsigned char move);
  bool mouseRaySphereIntersect(const int pos[3],const int radius,int* posIntersect=NULL);
  void setPosSphereToTransformed();
  unsigned char currSelect,q[TTTOE3D_N*TTTOE3D_SIZE],
                index,compPlayer; //Player index values lie in [1..3]
  unsigned int inputCode,counter,shaderIndex,shaderIndexOld;
  unsigned short *buff,*offBuff,*zInvBuff;
  Noise *noise;
  bool mouseDown;
  int b[TTTOE3D_DEPTH+1],
      n[TTTOE3D_DEPTH+1],
      par[TTTOE3D_SIZE],
      state[TTTOE3D_SIZE],
      goodness[TTTOE3D_SIZE];
  const char *playerName[3]; //Names of players in order of play
  int posSphere[TTTOE3D_N][3],posLight[3],posSpherePrm[TTTOE3D_N][3],mat[12];
  unsigned char orderMap[TTTOE3D_N],orderMapInit[TTTOE3D_N],
                numChildren[TTTOE3D_SIZE];
  char selectionOld;
  bool underCursor[TTTOE3D_N];
  unsigned char winCell[3],numMoves;
  int x,y;
  int tta,phi;
  int posIntersect[3],posIntersectSave[3];
  char winner;
  int tmpInt;
  static const int center[3];
  static const unsigned char winCond[][3][3];
  static const size_t numWinCond;
  static const char *playerNameUnordered[];
  static const char *shaderSource[][2];
};

class AppModule : public pp::Module {
 public:
  AppModule() : pp::Module() {}
  virtual ~AppModule() {}
  virtual pp::Instance* CreateInstance(PP_Instance instance) {
    return new TTToe3D(instance);
  }
};

namespace pp {
Module* CreateModule() { return new AppModule(); }
} //Namespace pp
