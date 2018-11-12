////////////////////////////////////////////////////////////////////////////////
//Dan Nielsen
////////////////////////////////////////////////////////////////////////////////
#include "TTToe3D.h"

#define TTTOE3D_ZERO {0,0,0}
#define TTTOE3D_ONE {1,1,1}
#define TTTOE3D_TWO {2,2,2}

const char* playerEndSequenceMusic=
  "Mc4.4.2gf#gageefebc.8";
const char* computerEndSequenceMusic=playerEndSequenceMusic;
const char* catEndSequenceMusic=playerEndSequenceMusic;
const int TTToe3D::center[3]={0,0,0x120000};
const unsigned char TTToe3D::winCond[][3][3]={
  //Orthogonal
  {TTTOE3D_ZERO,TTTOE3D_ZERO,{0,1,2}},
  {TTTOE3D_ZERO,TTTOE3D_ONE,{0,1,2}},
  {TTTOE3D_ZERO,TTTOE3D_TWO,{0,1,2}},
  {TTTOE3D_ONE,TTTOE3D_ZERO,{0,1,2}},
  {TTTOE3D_ONE,TTTOE3D_ONE,{0,1,2}},
  {TTTOE3D_ONE,TTTOE3D_TWO,{0,1,2}},
  {TTTOE3D_TWO,TTTOE3D_ZERO,{0,1,2}},
  {TTTOE3D_TWO,TTTOE3D_ONE,{0,1,2}},
  {TTTOE3D_TWO,TTTOE3D_TWO,{0,1,2}},
  {TTTOE3D_ZERO,{0,1,2},TTTOE3D_ZERO},
  {TTTOE3D_ZERO,{0,1,2},TTTOE3D_ONE},
  {TTTOE3D_ZERO,{0,1,2},TTTOE3D_TWO},
  {TTTOE3D_ONE,{0,1,2},TTTOE3D_ZERO},
  {TTTOE3D_ONE,{0,1,2},TTTOE3D_ONE},
  {TTTOE3D_ONE,{0,1,2},TTTOE3D_TWO},
  {TTTOE3D_TWO,{0,1,2},TTTOE3D_ZERO},
  {TTTOE3D_TWO,{0,1,2},TTTOE3D_ONE},
  {TTTOE3D_TWO,{0,1,2},TTTOE3D_TWO},
  {{0,1,2},TTTOE3D_ZERO,TTTOE3D_ZERO},
  {{0,1,2},TTTOE3D_ZERO,TTTOE3D_ONE},
  {{0,1,2},TTTOE3D_ZERO,TTTOE3D_TWO},
  {{0,1,2},TTTOE3D_ONE,TTTOE3D_ZERO},
  {{0,1,2},TTTOE3D_ONE,TTTOE3D_ONE},
  {{0,1,2},TTTOE3D_ONE,TTTOE3D_TWO},
  {{0,1,2},TTTOE3D_TWO,TTTOE3D_ZERO},
  {{0,1,2},TTTOE3D_TWO,TTTOE3D_ONE},
  {{0,1,2},TTTOE3D_TWO,TTTOE3D_TWO},
  //Planar diagonal
  {TTTOE3D_ZERO,{0,1,2},{0,1,2}},
  {TTTOE3D_ZERO,{2,1,0},{0,1,2}},
  {TTTOE3D_ONE,{0,1,2},{0,1,2}},
  {TTTOE3D_ONE,{2,1,0},{0,1,2}},
  {TTTOE3D_TWO,{0,1,2},{0,1,2}},
  {TTTOE3D_TWO,{2,1,0},{0,1,2}},
  {{0,1,2},TTTOE3D_ZERO,{0,1,2}},
  {{2,1,0},TTTOE3D_ZERO,{0,1,2}},
  {{0,1,2},TTTOE3D_ONE,{0,1,2}},
  {{2,1,0},TTTOE3D_ONE,{0,1,2}},
  {{0,1,2},TTTOE3D_TWO,{0,1,2}},
  {{2,1,0},TTTOE3D_TWO,{0,1,2}},
  {{0,1,2},{0,1,2},TTTOE3D_ZERO},
  {{2,1,0},{0,1,2},TTTOE3D_ZERO},
  {{0,1,2},{0,1,2},TTTOE3D_ONE},
  {{2,1,0},{0,1,2},TTTOE3D_ONE},
  {{0,1,2},{0,1,2},TTTOE3D_TWO},
  {{2,1,0},{0,1,2},TTTOE3D_TWO},
  //Fully diagonal
  {{0,1,2},{0,1,2},{0,1,2}},
  {{2,1,0},{0,1,2},{0,1,2}},
  {{0,1,2},{2,1,0},{0,1,2}},
  {{0,1,2},{0,1,2},{2,1,0}}};
const size_t TTToe3D::numWinCond=sizeof(winCond)/sizeof(winCond[0]);
const char* TTToe3D::playerNameUnordered[]={
  "Tackytron 3000 ","Sweet Thang ","Kludgenstein ","Wombat Charlie ",
  "Ms. Ellie Gant ","Connie Sewer ","Vlechem Mahone ","Cham Percival "};
const char* TTToe3D::shaderSource[][2]={
  {NULL,NULL},
  { //Mandelbrot transform
    "precision mediump float;\n"
    "varying vec2 v_texcoord;\n"
    "uniform sampler2D u_texture;\n"
    "void main() {\n"
    "  vec2 q=.625*vec2(v_texcoord);"
    "  for(int i=0;i<8;i++)"
    "    q=vec2(q[0]*q[0]-q[1]*q[1],2.*q[0]*q[1])+.625*vec2(v_texcoord);"
    "  q[0]=mod(q[0],.625);"
    "  q[1]=mod(q[1],.46875);"
    "  gl_FragColor=texture2D(u_texture,q);\n"
    "}\n",NULL}
};

unsigned short raycastSphereSurface0(int x,int y,int z,Noise* noise,
                                     unsigned int t) {
  unsigned int w=200+(noise->get2D(x>>11,(y>>11)-(t<<3),0xff,0xff)>>6);
  return ((w>>3)<<11)|((w>>2)<<5)|(w>>3);
}

unsigned short raycastSphereSurface1(int x,int y,int z,Noise* noise,
                                     unsigned int t) {
  unsigned int w=200+(noise->get2D(x>>11,(y>>11)-(t<<3),0xff,0xff)>>6);
  return ((w>>3)<<11);
}

unsigned short raycastSphereSurface2(int x,int y,int z,Noise* noise,
                                     unsigned int t) {
  unsigned int w=200+(noise->get2D(x>>11,(y>>11)-(t<<3),0xff,0xff)>>6);
  return ((w>>2)<<5);
}

unsigned short raycastSphereSurface3(int x,int y,int z,Noise* noise,
                                     unsigned int t) {
  unsigned int w=200+(noise->get2D(x>>11,(y>>11)-(t<<3),0xff,0xff)>>6);
  return (w>>3);
}

unsigned short raycastSphereSelectionSurface(int x,int y,int z,Noise* noise,
                                             unsigned int t) {
  return 0x07ff;
}

TTToe3D::TTToe3D(PP_Instance instance) :
  AppInstance(instance),
  callback_factory_(this),
  shaderIndex(0),shaderIndexOld(0),
  buff(getFrameBuffer()),offBuff(getOffFrameBuffer()),zInvBuff(getZInvBuffer()),
  noise(new Noise()),
  mouseDown(false),
  tta(0),phi(0) {
  agTextFont(AG_ROMANFONT);
  reset();
  update=&TTToe3D::pretitleLoop;
}

void TTToe3D::reset() {
  winner=0;
  compPlayer=0;
  setCounterClear();
  numMoves=0;
  selectionOld=-1;
  mmSetIdentityMat3x4(mat);
  mmSetPosMat3x4(mat,center);
  //Randomize players
  commonRandomCounterSeed(getTimer());
  //Initialize member arrays
  winCell[0]=winCell[1]=winCell[2]=0;
  b[0]=0; n[0]=1;
  for(int i=0;i<TTTOE3D_SIZE;i++) state[i]=numChildren[i]=goodness[i]=0;
  for(unsigned char i=0;i<n[0];i++) {
    par[i]=-1;
    for(unsigned char c=0;c<TTTOE3D_N;c++) node(q,i)[c]=0;
  }
  index=1;
  for(unsigned char i=0;i<TTTOE3D_N;i++) {
    posSphere[i][0]=((i%3)-1)*0x40000;
    posSphere[i][1]=(((i/3)%3)-1)*0x40000;
    posSphere[i][2]=(((i/9)%3)-1)*0x40000;
    orderMap[i]=0;
    orderMapInit[i]=i;
    underCursor[i]=false;
  }
  posLight[0]=0x000000; posLight[1]=0x180000; posLight[2]=0x100000;
  posIntersect[0]=0; posIntersect[1]=0; posIntersect[2]=0;
  setLog("RESET");
}

TTToe3D::~TTToe3D() {
}

//Tree, q, current node index, p, tree level, m
//Returns 0=unfinished, {1,2,3}=index of winning player, -1=cat won
char TTToe3D::gameState(unsigned char v[]) {
  //Win condition
  for(unsigned int i=0;i<numWinCond;i++) {
    if (cell(v,winCond[i][0][0],winCond[i][1][0],winCond[i][2][0])&&
        cell(v,winCond[i][0][0],winCond[i][1][0],winCond[i][2][0])==
        cell(v,winCond[i][0][1],winCond[i][1][1],winCond[i][2][1])&&
        cell(v,winCond[i][0][1],winCond[i][1][1],winCond[i][2][1])==
        cell(v,winCond[i][0][2],winCond[i][1][2],winCond[i][2][2])) {
      winCell[0]=3*(3*winCond[i][0][0]+winCond[i][1][0])+winCond[i][2][0];
      winCell[1]=3*(3*winCond[i][0][1]+winCond[i][1][1])+winCond[i][2][1];
      winCell[2]=3*(3*winCond[i][0][2]+winCond[i][1][2])+winCond[i][2][2];
      return index; //Winning-player index
    }
  }
  //Cat game
  if (cell(v,0,0,0)&&cell(v,0,0,1)&&cell(v,0,0,2)&&cell(v,0,1,0)&&cell(v,0,1,1)&&
      cell(v,0,1,2)&&cell(v,0,2,0)&&cell(v,0,2,1)&&cell(v,0,2,2)&&
      cell(v,1,0,0)&&cell(v,1,0,1)&&cell(v,1,0,2)&&cell(v,1,1,0)&&cell(v,1,1,1)&&
      cell(v,1,1,2)&&cell(v,1,2,0)&&cell(v,1,2,1)&&cell(v,1,2,2)&&
      cell(v,2,0,0)&&cell(v,2,0,1)&&cell(v,2,0,2)&&cell(v,2,1,0)&&cell(v,2,1,1)&&
      cell(v,2,1,2)&&cell(v,2,2,0)&&cell(v,2,2,1)&&cell(v,2,2,2)) return -1;
  //Unfinished
  return 0;
}

int TTToe3D::rateConfiguration(unsigned char v[],unsigned char index) {
#define TTTOE3D_addPairGoodness(x0,y0,z0, x1,y1,z1, x2,y2,z2, pairValue) \
  r+=(cell(v,x0,y0,z0)==cell(v,x1,y1,z1)&&cell(v,x0,y0,z0)&&!cell(v,x2,y2,z2))* \
     (cell(v,x0,y0,z0)==index? pairValue : -pairValue); \
  r+=(cell(v,x1,y1,z1)==cell(v,x2,y2,z2)&&cell(v,x1,y1,z1)&&!cell(v,x0,y0,z0))* \
     (cell(v,x1,y1,z1)==index? pairValue : -pairValue); \
  r+=(cell(v,x2,y2,z2)==cell(v,x0,y0,z0)&&cell(v,x2,y2,z2)&&!cell(v,x1,y1,z1))* \
     (cell(v,x2,y2,z2)==index? pairValue : -pairValue);
  int r=0;
  for(unsigned char j=0;j<3;j++)
    for(unsigned char i=0;i<3;i++) {
      TTTOE3D_addPairGoodness(0,i,j, 1,i,j, 2,i,j, TTTOE3D_LINEPAIR);
      TTTOE3D_addPairGoodness(i,0,j, i,1,j, i,2,j, TTTOE3D_LINEPAIR);
      TTTOE3D_addPairGoodness(i,j,0, i,j,1, i,j,2, TTTOE3D_LINEPAIR);
    }
  for(unsigned char i=0;i<3;i++) {
    TTTOE3D_addPairGoodness(i,0,0, i,1,1, i,2,2, TTTOE3D_PLANEPAIR);
    TTTOE3D_addPairGoodness(i,2,0, i,1,1, i,0,2, TTTOE3D_PLANEPAIR);
    TTTOE3D_addPairGoodness(0,i,0, 1,i,1, 2,i,2, TTTOE3D_PLANEPAIR);
    TTTOE3D_addPairGoodness(2,i,0, 1,i,1, 0,i,2, TTTOE3D_PLANEPAIR);
    TTTOE3D_addPairGoodness(0,0,i, 1,1,i, 2,2,i, TTTOE3D_PLANEPAIR);
    TTTOE3D_addPairGoodness(2,0,i, 1,1,i, 0,2,i, TTTOE3D_PLANEPAIR);
  }
  TTTOE3D_addPairGoodness(0,0,0, 1,1,1, 2,2,2, TTTOE3D_DIAGPAIR);
  TTTOE3D_addPairGoodness(2,0,0, 1,1,1, 0,2,2, TTTOE3D_DIAGPAIR);
  TTTOE3D_addPairGoodness(2,2,0, 1,1,1, 0,0,2, TTTOE3D_DIAGPAIR);
  TTTOE3D_addPairGoodness(0,2,0, 1,1,1, 2,0,2, TTTOE3D_DIAGPAIR);
  return r;
#undef TTTOE3D_addPairGoodness
}

void TTToe3D::print(unsigned char v[]) {
  for(unsigned char i=0;i<TTTOE3D_N;i++) {
    if (!(i%3)) setPrintString(" ");
    if (!(i%9)) setPrintLocation(80*(i/9)+60);
    setPrintDot((!v[i]||v[i]==1)*192,(!v[i]||v[i]==2)*192,(!v[i]||v[i]==3)*192);
  }
  if (winner>0) {
    for(unsigned char i=0;i<3;i++) {
      const unsigned char ind=winCell[i];
      setPrintLocation(80*(ind/9)+60+((ind%9)/3)+((ind%9)<<1));
      setPrintString("_ ");
    }
  }
  else if (selectionOld>=0) {
    setPrintLocation(80*(selectionOld/9)+60+((selectionOld%9)/3)+
                     ((selectionOld%9)<<1));
    setPrintString("_ ");
  }
}

int TTToe3D::match(unsigned char v[],unsigned char q[],int b,int n,int par[],int parent,
                   unsigned char* transformCode) {
  bool diff=true;
  for(int g=b+n-1;g>=b;g--) { //Other nodes in tree level, g
    if (par[g]!=parent) break; //Only look within same branch; break once outside that range
    for(unsigned char h=0;h<6;h++) //Possible orientations, h
      for(unsigned char r=0;r<8;r++) { //Possible reflections, r
        diff=false;
        for(unsigned char k=0;k<3;k++) { //Check
          for(unsigned char j=0;j<3;j++) { //all
            for(unsigned char i=0;i<3;i++) { //cells
              unsigned char ii=i,jj=j,kk=k;
              if (r&1) ii=2-ii; if (r&2) jj=2-jj; if (r&4) kk=2-kk;
              if (h&1) swap(ii,jj); if (h&2) swap(jj,kk); if (h&4) swap(kk,ii);
              diff=(cell(v,kk,jj,ii)!=cell(node(q,g),k,j,i));
              if (diff) break;
            }
            if (diff) break;
          }
          if (diff) break;
        }
        if (!diff) { //If symmetry with another node, return its index (and the transformCode)
          if (transformCode) *transformCode=h<<3|r;
          return g;
        }
      }
  }
  return -1;
}

void TTToe3D::DidChangeView(const pp::View& view) {
  if (context_.is_null()) {
    appInit();
    pretitleLoop(0);
  }
}

//Returns whether intersection exists
bool TTToe3D::mouseRaySphereIntersect(const int pos[3],const int radius,int* posIntersect) {
  const int dx=x-APP_HALFFRAMEWIDTH,dy=y-APP_HALFFRAMEHEIGHT,
            lengthInv=0x80000000/mmSqrRt((dx*dx+dy*dy+320*320)<<14),
            vx=(dx*lengthInv)>>9, //Signed 2.15-fixed
            vy=(-dy*lengthInv)>>9, //Signed 2.15-fixed
            vz=(320*lengthInv)>>9, //Signed 2.15-fixed
            xPos=pos[0]>>5,yPos=pos[1]>>5,zPos=pos[2]>>5,
            beta=(vx*xPos+vy*yPos+vz*zPos)>>15,
            r=radius>>5,
            param=beta*beta-(xPos*xPos+yPos*yPos+zPos*zPos-r*r); //Signed X.22-fixed
  if (param>=0&&posIntersect) {
    const int t=(beta-mmSqrRt(param))>>5; //Signed 11.6-fixed
    posIntersect[0]=t*(vx>>5); //Signed 8.16-fixed
    posIntersect[1]=t*(vy>>5);
    posIntersect[2]=t*(vz>>5);
  }
  return (param>=0);
}

void TTToe3D::setPosSphereToTransformed() {
  for(unsigned char i=0;i<TTTOE3D_N;i++) {
    posSphere[i][0]=posSpherePrm[i][0]-center[0];
    posSphere[i][1]=posSpherePrm[i][1]-center[1];
    posSphere[i][2]=posSpherePrm[i][2]-center[2];
    orderMapInit[i]=orderMap[i];
  }
}

void TTToe3D::loopInit(bool updateNoise) {
  if (updateNoise) noise->update();
  inputCode=getInputCode();
  mouseDown=getMouseDown();
  counter=getCounter();
  x=getMouseX(); y=getMouseY();
}

void TTToe3D::loopUpdate() {
  appUpdate();
  context_.SwapBuffers(callback_factory_.NewCallback(update));
}

void TTToe3D::segue(void (TTToe3D::*callback)(int32_t),int shaderIndexSet) {
  agFullScreenBlit(zInvBuff,offBuff);
  agFullScreenStaticFill(offBuff,3);
  setCounterClear();
  update=&TTToe3D::curtainLoop;
  shaderIndexOld=shaderIndex;
  shaderIndex=shaderIndexSet;
  update2=callback;
}

void TTToe3D::pretitleLoop(int32_t) {
  loopInit(false);
  agFullScreenXORFill(buff); //Background image
  setPrintLocation(15*80+31); setPrintString("Click to begin... ",1);
  if (getMouseJustLocked()||getMouseDown()||
      (inputCode!=APP_INPUTDEFAULT&&!(inputCode&APP_INPUTESC))) {
    agFullScreenXORFill(offBuff); //Background image
    segue(&TTToe3D::titleLoop,1);
  }
  loopUpdate();
}

void TTToe3D::titleLoop(int32_t) {
  loopInit();
  mmSetRotMat3x4(mat,counter<<1,3*counter,counter<<2);
  agFullScreenBlendBlit(buff,offBuff); //Background image
  drawBoard();
  agTextSetPosition(25,175); agText(buff,"Ick!Ack!Oh! ",255,255,255);
  agSparkleCursor(buff,284+((mmAbs(512-((int)counter&0x3ff))*mmCos(counter<<4))>>17),
                       255+((mmAbs(512-((int)counter&0x3ff))*(mmSin(3*(counter<<2))>>1))>>17),
                       counter);
  if (inputCode!=APP_INPUTDEFAULT||getMouseDown()) {
    if (inputCode&APP_INPUTESC) {
      segue(&TTToe3D::pretitleLoop);
    }
    else {
      //Top board background
      for(unsigned int j=428;j<APP_FRAMEHEIGHT;j++)
        for(unsigned int i=0,c=j-300;i<AG_WIDTH;i++)
          agPoint(offBuff+j*APP_FRAMEWIDTH+i,c,0,c);
      //Choose player information: 3 different players, 1 being the computer
      do {
        playerName[0]=playerNameUnordered[commonRandomUchar()&7];
        playerName[1]=playerNameUnordered[commonRandomUchar()&7];
        playerName[2]=playerNameUnordered[commonRandomUchar()&7];
        if (!strcmp(playerName[0],playerName[1])||
            !strcmp(playerName[1],playerName[2])||
            !strcmp(playerName[2],playerName[0])) continue;
        for(unsigned char i=0;i<3;i++)
          if (!strcmp(playerNameUnordered[0],playerName[i])) compPlayer=i+1;
      } while(!compPlayer);
      segue(&TTToe3D::mainLoop);
    }
  }
  loopUpdate();
}

void TTToe3D::checkCatGame() {
  if (numMoves==TTTOE3D_N)
    if (gameState(node(q,0))<0) { //Cat won
      winner=-1;
      compPlayer=0;
      setCounterClear();
      setPlayBuffer(0,catEndSequenceMusic);
      update=&TTToe3D::endSequenceLoop;
    }
}

void TTToe3D::drawBoard() {
  //Player-independent
  //Transform posSphere
  for(unsigned char i=0;i<TTTOE3D_N;i++) {
    mmMat3x4FakeMulVec3(posSpherePrm[i],mat,posSphere[i]); //Rotation
    orderMap[i]=orderMapInit[i];
  }
  for(unsigned char j=0;j<TTTOE3D_N-1;j++) { //Sort posSpherePrm by z-coordinate
    unsigned char indexOfMax=j;
    for(unsigned char i=j+1;i<TTTOE3D_N;i++)
      if (posSpherePrm[i][2]>posSpherePrm[indexOfMax][2])
        indexOfMax=i;
    swap(posSpherePrm[indexOfMax],posSpherePrm[j]);
    swap(orderMap[indexOfMax],orderMap[j]);
  }
  char selection=-1;
  if (compPlayer) { //During gameplay, not during title
    if (index!=compPlayer) { //Non-computer player
      if (!mouseDown) {
        int val=INT_MAX;
        for(unsigned char i=0;i<TTTOE3D_N;i++) //Determine spheres under cursor
          underCursor[i]=mouseRaySphereIntersect(posSpherePrm[i],0x1a000);
        for(unsigned char i=0;i<TTTOE3D_N;i++) //Foremost unoccupied sphere under cursor is selection
          if (underCursor[i]&&!node(q,0)[orderMap[i]]&&posSpherePrm[i][2]<val)
            { val=posSpherePrm[i][2]; selection=i; }
      }
setPrintLocation(160); setPrintInt(numMoves);
      if (getMouseClick()&&selection>=0) { //Commit selection on click
        setPlayBuffer(1,"Ma2");
        setPrintLocation(80); setPrintString("Selection made ");
        selectionOld=orderMap[(int)selection];
        node(q,0)[(int)selectionOld]=index; //Since orderMap is its own inverse, we use it here
        numMoves++;
        selection=-1;
        if (gameState(node(q,0))==index&&!winner) { //Current player won
          winner=index;
          compPlayer=0;
          setCounterClear();
          setPlayBuffer(0,playerEndSequenceMusic);
          update=&TTToe3D::endSequenceLoop;
        }
        checkCatGame();
        index=nextPlayer(index);
        if (index==compPlayer)
          { setPrintLocation(80+20); setPrintString("Please wait... "); }
      }
    }
    //Player-independent
    agFullScreenBlendBlit(buff,offBuff); //Background image
    setPrintLocation(0); setPrintString("Player: ");
    setPrintDot(192,0,0);
    setPrintString(playerName[0],index==1);
    setPrintDot(0,192,0);
    setPrintString(playerName[1],index==2);
    setPrintDot(0,0,192);
    setPrintString(playerName[2],index==3);
  }
  for(unsigned char i=0;i<TTTOE3D_N;i++) //Render spheres
    agRaytraceSphere(buff,posSpherePrm[i],0x1a000,posLight,
      i==selection? raycastSphereSelectionSurface :
      node(q,0)[orderMap[i]]<=0? raycastSphereSurface0 :
      node(q,0)[orderMap[i]]<=1? raycastSphereSurface1 :
      node(q,0)[orderMap[i]]<=2? raycastSphereSurface2 :
                                 raycastSphereSurface3,
      noise,counter);
}

void TTToe3D::mainLoop(int32_t) {
  loopInit();
  int val,move[2];
  unsigned char mr[2][TTTOE3D_N],boardOld[TTTOE3D_N];
#if TTTOE3D_PREDICTIONDEPTH>1
  bool refer[TTTOE3D_PREDICTIONDEPTH-1][2];
#endif
  if (inputCode&APP_INPUTESC) { //Esc pressed, so nobody wins
    agFullScreenXORFill(offBuff); //Background image
    reset();
    segue(&TTToe3D::titleLoop,1);
  }
  if (index==compPlayer) {
    for(unsigned int i=0;i<TTTOE3D_N;i++) //Save off current board configuration
      boardOld[i]=node(q,0)[i];
    for(unsigned char h=0;h<TTTOE3D_PREDICTIONDEPTH;h++) { //For next predicted moves
      //Build tree of next possible moves, pruning symmetries that preserve the basic arrangement
      //EXAMPLE:
      //   .  _____ b[0]=1
      //  /|\  ____ b[1]=3 __ 1   \
      // /\ /|\  __ b[2]=5 __ 2 m | DEPTH=3
      //  /\  /\  _ b[3]=4 __ 3   /
      //(no children here) __ 4
      for(unsigned char m=1;m<=TTTOE3D_DEPTH+1;m++) { //Tree level, m (zero-indexed)
        if (m<=TTTOE3D_DEPTH) {
          n[m]=0; //Number of nodes in level, n[m]
          b[m]=b[m-1]+n[m-1]; //Beginning node index of tree level, b[m]
        }
        for(int f=b[m-1];f<b[m-1]+n[m-1];f++) { //Parent node index, f, level of children to try, m
          goodness[f]+=rateConfiguration(node(q,f),nextPlayer(index,m-1)); //Rate pairs
          if (state[f]>0) //No children if node is win-loss state
            goodness[f]+=(m==2? TTTOE3D_WIN : -TTTOE3D_WIN); //Weight win positive or negative
          else if (m<=TTTOE3D_DEPTH&&numMoves+m+h-1<TTTOE3D_N) //Assign all possible child configurations if not last level or filled board
            for(unsigned char c=0;c<TTTOE3D_N;c++) { //Cell to try for new child, c
              for(unsigned char i=0;i<TTTOE3D_N;i++) node(q,b[m]+n[m])[i]=node(q,f)[i]; //Copy parent
              if (node(q,b[m]+n[m])[c]) continue; //If cell already occupied, try next cell
              node(q,b[m]+n[m])[c]=nextPlayer(index,m-1); //Mark cell with player index
              if (match(node(q,b[m]+n[m]),q,b[m],n[m],par,f)<0) { //If no symmetry found,
                par[b[m]+n[m]]=f;                                 //set parent node index,
                numChildren[par[b[m]+n[m]]]++;                    //set numChildren of parent,
                state[b[m]+n[m]]=gameState(node(q,b[m]+n[m]));    //set game state variable,
                n[m]++;                                           //and append data to tree
              }
            }
        }
      }
      //Scan through tree from last level, accumulating goodness values of nodes as we go
      for(unsigned char m=TTTOE3D_DEPTH;m>1;m--) //Tree level, m (zero-indexed)
        for(int s=b[m];s<b[m]+n[m];s++) //Node index, s
          goodness[par[s]]+=(TTTOE3D_ATTENUATION*goodness[s])/(numChildren[par[s]]<<8);
      //Determine best 2 moves based on current tree
      move[0]=move[1]=0; val=INT_MIN;
      for(int i=b[1];i<b[1]+n[1];i++)
        if (goodness[i]>=val) {
          val=goodness[i];
          move[1]=move[0];
          move[0]=i;
        }
      if (h) {
#if TTTOE3D_PREDICTIONDEPTH>1
        refer[h-1][0]=par[move[0]]; refer[h-1][1]=par[move[1]];
#endif
      }
      else {
        for(unsigned char i=0;i<TTTOE3D_N;i++)
          { mr[0][i]=node(q,move[0])[i]; mr[1][i]=node(q,move[1])[i]; }
      }
      //Clear tree and set best 2 moves as root of next tree evaluation
      for(int i=0;i<TTTOE3D_SIZE;i++) numChildren[i]=goodness[i]=0;
      for(unsigned char i=0;i<TTTOE3D_N;i++) node(q,0)[i]=node(q,move[0])[i];
      par[0]=-1; numChildren[0]=0; state[0]=state[move[0]]; n[0]=1;
      if (move[1]) {
        for(unsigned char i=0;i<TTTOE3D_N;i++) node(q,1)[i]=node(q,move[1])[i];
        par[1]=-1; numChildren[1]=0; state[1]=state[move[1]]; n[0]=2;
      }
      index=nextPlayer(index);
    }
    //Computer made predictions, so it climbs back to its favored move
    bool r=0;
#if TTTOE3D_PREDICTIONDEPTH>1
    for(int i=TTTOE3D_PREDICTIONDEPTH-2;i>=0;i--) r=refer[i][r];
#endif
    //Set selectionOld as difference between this move and the last
    for(unsigned char i=0;i<TTTOE3D_N;i++)
      if (boardOld[i]!=mr[r][i]) selectionOld=i;
    //Set move as root of tree
    setPlayBuffer(2,"Mf1");
    numMoves++;
    for(unsigned int i=0;i<TTTOE3D_SIZE;i++)
      numChildren[i]=goodness[i]=0;
    for(unsigned char i=0;i<TTTOE3D_N;i++)
      node(q,0)[i]=mr[r][i];
    par[0]=-1; numChildren[0]=0; state[0]=gameState(mr[r]); n[0]=1;
    if (state[0]==compPlayer) { //Computer won
      winner=compPlayer;
      compPlayer=0;
      setCounterClear();
      setPlayBuffer(0,computerEndSequenceMusic);
      update=&TTToe3D::endSequenceLoop;
      loopUpdate();
      return;
    }
    checkCatGame();
    //Next player
    index=nextPlayer(compPlayer);
  }
  drawBoard();
  if (mouseDown) {
    //Virtual trackball
    posIntersect[0]=(APP_HALFFRAMEWIDTH-x)<<4; //Signed 10.4-fixed
    posIntersect[1]=(y-APP_HALFFRAMEHEIGHT)<<4; //Signed 10.4-fixed
    const int lenSqr=posIntersect[0]*posIntersect[0]+
                     posIntersect[1]*posIntersect[1], //Unsigned 16.8-fixed
              radius=0xd400, //Unsigned 16.8-fixed
              radiusSqr=(radius>>4)*(radius>>4); //Unsigned 16.8-fixed
    posIntersect[2]=(lenSqr<=radiusSqr>>1? //Virtual sphere piecewise with hyperbola
                     mmSqrRt(radiusSqr-lenSqr) : //Signed 16.4-fixed
                     radiusSqr/mmSqrRt(lenSqr<<2)); //Signed 16.4-fixed
    int lenInv=0x10000/mmNorm2Vec3(posIntersect); //Unsigned 16.12-fixed
    posIntersect[0]*=lenInv; //Signed 10.16-fixed
    posIntersect[1]*=lenInv; //Signed 10.16-fixed
    posIntersect[2]*=lenInv; //Signed 10.16-fixed
    if (getMousePressDown()) { //Save current position and last movement if new mouse press
      setPosSphereToTransformed();
      posIntersectSave[0]=posIntersect[0]; //Signed 10.16-fixed
      posIntersectSave[1]=posIntersect[1]; //Signed 10.16-fixed
      posIntersectSave[2]=posIntersect[2]; //Signed 10.16-fixed
    }
    int quat[4]; //Components (x,y,z,w)
    mmCross(quat,posIntersect,posIntersectSave); //Signed 1.16-fixed
    quat[0]>>=8; //Signed 2.8-fixed
    quat[1]>>=8; //Signed 2.8-fixed
    quat[2]>>=8; //Signed 2.8-fixed
    quat[3]=0x100+(mmDot(posIntersect,posIntersectSave)>>8); //Signed 2.8-fixed
    lenInv=0x10000/mmSqrRt(quat[0]*quat[0]+quat[1]*quat[1]+
                           quat[2]*quat[2]+quat[3]*quat[3]); //Signed 16.8-fixed
    quat[0]*=lenInv; //Signed 2.16-fixed
    quat[1]*=lenInv; //Signed 2.16-fixed
    quat[2]*=lenInv; //Signed 2.16-fixed
    quat[3]*=lenInv; //Signed 2.16-fixed
    mmQuatToMat3x4(quat,mat);
  }
  else {
    if (index!=compPlayer) //Cursor
      agSparkleCursor(buff,x,APP_FRAMEHEIGHT-y,counter);
  }
  print(node(q,0));
  loopUpdate();
}

void TTToe3D::endSequenceLoop(int32_t) {
  loopInit();
  mmSetRotMat3x4(mat,counter,counter<<1,3*counter);
  agFullScreenBlendBlit(buff,offBuff); //Background image
  agSparkleCursor(buff,APP_HALFFRAMEWIDTH+(mmCos(counter<<4)>>9),
                       APP_HALFFRAMEHEIGHT+(mmSin(5*counter)>>9),
                       counter);
  drawBoard();
  if (winner>0) {
    setPrintLocation(80+11-(strlen(playerName[winner-1])>>1));
    setPrintString("********** ^_^ ");
    setPrintDot((winner==1)*192,(winner==2)*192,(winner==3)*192);
    setPrintString(playerName[winner-1]);
    setPrintDot((winner==1)*192,(winner==2)*192,(winner==3)*192);
    setPrintString("won! ^_^ ********** ");
  }
  else if (!winner) {
    setPrintLocation(80+21);
    setPrintString("??? Nobody won ??? ");
  }
  else {
    setPrintLocation(80+22);
    setPrintString("=^t^= The cat won! =^t^= ");
  }
  print(node(q,0));
  if (getMouseDown()||inputCode!=APP_INPUTDEFAULT) {
    agFullScreenXORFill(offBuff); //Background image
    reset();
    segue(&TTToe3D::titleLoop,1);
  }
  loopUpdate();
}

void TTToe3D::curtainLoop(int32_t) {
  loopInit();
  agFullScreenCellularAutomataFill(offBuff,1,3,3,0,counter%3);
  agFullScreenBlendBlit(buff,offBuff);
  if (counter>=16)
    agFullScreenDim(buff);
  if (counter>=32) {
    if (shaderIndex!=shaderIndexOld)
      setShader(shaderSource[shaderIndex][0],
                shaderSource[shaderIndex][1]);
    agFullScreenBlit(offBuff,zInvBuff);
    if (update2==&TTToe3D::titleLoop) {
      aaReset(0);
      setPlayBuffer(0,"Sx`lwy_pla_^_gamm");
    }
    else if (update2==&TTToe3D::mainLoop) {
      aaReset(0);
      setPlayBuffer(0,"Slets_pla_tik_t`k_too");
    }
    update=update2;
  }
  loopUpdate();
}
